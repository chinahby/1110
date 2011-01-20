
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           A H B  P e r f o r m a n c e  M o ni t o r   D r i v e r
          
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc6055/services/profdiag/apm.c#6 $  $DateTime: 2007/07/04 16:54:06 $  $Author: fserrao $ 


 when      who     what, where, why
 --------  -----   ----------------------------------------------------------
 02/21/07  jwh     Quick fix for diag overload. Config table update.
 01/08/07  fs       Adapted for QSC6055.
 11/17/06  jwh     Begin changes for SC1X UXMC profiling engineering build
 09/06/06  cc      Creat the file.
 
============================================================================*/

/*============================================================================

                       INCLUDE FILES FOR MODULE
                       
============================================================================*/
#include "clk.h"
#include "msmhwio.h"
#include "apm.h"
#include "apmi.h"
#include "profdiagi.h"

#undef FEATURE_HACKTEST_APM
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

__align(4) apm_config_table_type apm_config_table;
apm_status_type apm_status_table;

static boolean apm_monitor_active;
uint32 apm_total_samples = 0;
uint32 apm_log_buffer_index;
uint32 apm_send_mem_buffer_timer_period = 10; /* diag pkt period in ms */
uint32 apm_test_duration = 0;
uint32 apm_samples_collected = 0;

#ifdef FEATURE_APM_BUFFERED_LOG
#error code not present
#else
    uint32 apm_counter_values[APM_TOTAL_COUNTERS_TO_READ];
#endif /* FEATURE_APM_BUFFERED_LOG */

static clk_cb_type apm_log_sample_timer;
static clk_cb_type apm_send_mem_buffer_timer;

static void apm_log_sample_timer_cb(int dummy);
static void apm_send_mem_buffer_timer_cb(int dummy);

#ifdef FEATURE_HACKTEST_APM
#error code not present
#endif /* FEATURE_HACKTEST_APM */

/*===========================================================================

FUNCTION  APM_INIT

DESCRIPTION
   Initialize data structure for UXMC BUS performance Monitor driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void apm_init(void)
{
   clk_def(&apm_send_mem_buffer_timer);
   clk_def(&apm_log_sample_timer);

   apm_monitor_active = FALSE;

   /* clear error history */
   apm_status_table.error_notice = apm_err_none;
   apm_status_table.dropped_pkts = 0;
   /* init status area */ 
   apm_status_table.max_cfg_regs      = APM_MAX_NUM_CFG_REGISTERS;
   apm_status_table.min_sample_period = APM_MIN_SAMPLE_PERIOD;
   apm_status_table.max_sample_period = APM_MAX_SAMPLE_PERIOD;
   apm_status_table.max_sample_iterations = APM_MAX_SAMPLES;

   /* Setup for SW Accumulate and Reset Counters */
   HWIO_BPM_CFG_OUT(
          0x3 << HWIO_BPM_CFG_CGC_CLK_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_SW_ACCUMULATE_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_CYC_CNTR_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT3_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT2_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT1_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT0_CNTRS_RESET_SHFT );
   /* Release Reset of Counters */
   HWIO_BPM_CFG_OUT(
          0x3 << HWIO_BPM_CFG_CGC_CLK_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_SW_ACCUMULATE_ENA_SHFT );

   /* Reset cntl logic */
   HWIO_BPM_CMD_OUT( 0x1 << HWIO_BPM_CMD_SW_RESET_SHFT );
   HWIO_BPM_CMD_OUT( 0x0 << HWIO_BPM_CMD_SW_RESET_SHFT );

   /* Configure 'saturation' parameter for cycle counter */
   HWIO_BPM_TIMER_OUT( 0xffff0000 );
   HWIO_BPM_CYC_CFG_OUT(
          0x1 << HWIO_BPM_CYC_CFG_CYC_SAT_ENA_SHFT
        | 0x1 << HWIO_BPM_CYC_CFG_PORT3_SAT_ENA_SHFT
        | 0x1 << HWIO_BPM_CYC_CFG_PORT2_SAT_ENA_SHFT
        | 0x1 << HWIO_BPM_CYC_CFG_PORT1_SAT_ENA_SHFT
        | 0x1 << HWIO_BPM_CYC_CFG_PORT0_SAT_ENA_SHFT );

#ifdef FEATURE_HACKTEST_APM
#error code not present
#endif

} /* end apm_init() */


/*===========================================================================

FUNCTION  APM_Validate_config_table

DESCRIPTION
   This function validates the configuration table contents.        
   
DEPENDENCIES
  It's upto the user to provide valid  configuration for each address and 
  use correct combination of certain registers.   

RETURN VALUE
  Boolean:  TRUE -- configuration data are valid
            FALSE -- Configuration data are INVALID    

SIDE EFFECTS
  None

===========================================================================*/
boolean apm_validate_config_table(void)
{
  int i;
  boolean return_val = FALSE;

  if ( apm_config_table.version != APM_CONFIG_TABLE_VERSION )
  {
     apm_status_table.error_notice = apm_err_version;
     return return_val;
  }
  if ( apm_config_table.num_registers > APM_MAX_NUM_CFG_REGISTERS )
  {
     apm_status_table.error_notice = apm_err_num_regs_too_big;
     return return_val;
  }
  if ( apm_config_table.sample_iterations > APM_MAX_SAMPLES )
  {
     apm_status_table.error_notice = apm_err_sample_iterations;
     return return_val;
  }
  if (( apm_config_table.sample_period > APM_MAX_SAMPLE_PERIOD ) ||
      ( apm_config_table.sample_period < APM_MIN_SAMPLE_PERIOD )     )
  {
     apm_status_table.error_notice = apm_err_sample_period;
     return return_val;
  }

  if (apm_config_table.version == APM_CONFIG_TABLE_VERSION)
  {
     for (i = 0; i<apm_config_table.num_registers; i++ )
     {
       if (  apm_config_table.registers[i].addr %4 == 0
              &&((apm_config_table.registers[i].addr  >= HWIO_BPM_CFG_ADDR  
                    && apm_config_table.registers[i].addr <= HWIO_BPM_Cn_CFG6_ADDR(16) )
                 ||(apm_config_table.registers[i].addr  >= HWIO_EBI1_CFG_ADDR
                    && apm_config_table.registers[i].addr <= HWIO_EBI2_DEBUG_BUS_CFG_ADDR)
                 ||(apm_config_table.registers[i].addr  >= HWIO_SDRAM_OPT_CFG_ADDR
                    && apm_config_table.registers[i].addr <= HWIO_SDRAM_FPGA_CONTROLLER_CFG5_ADDR)) )
       {
          continue;
       }
       else 
       {
         apm_status_table.error_notice = apm_err_address;
         break;
       }
     }

     if ( i == apm_config_table.num_registers)
     {    
       return_val = TRUE;
     }
     else
     {
       apm_status_table.error_notice = apm_err_num_regs_wrong;
     }
  }

  return return_val;

} /* end apm_validate_config_table() */

/*===========================================================================

FUNCTION  APM_START_MONITOR

DESCRIPTION
   This function programs UXMC bus monitor according to configure data from 
apm_config_table, and starts the monitor. 

   Following steps are followed to start the monitor. 
   1. 
   
DEPENDENCIES
  apm_validate_config_table must have been called and returned TRUE.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void apm_start_monitor(void)
{
#ifdef T_SDRAM
  int i;
#endif
  uint32 x;

  if (apm_config_table.version == APM_CONFIG_TABLE_VERSION)
  {

    /* Reset BPM counters */
    HWIO_BPM_CFG_OUT(
          0x3 << HWIO_BPM_CFG_CGC_CLK_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_SW_ACCUMULATE_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_CYC_CNTR_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_RESTART_LOCK_DIS_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT3_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT2_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT1_CNTRS_RESET_SHFT
        | 0x1 << HWIO_BPM_CFG_PORT0_CNTRS_RESET_SHFT );
    /* Release BPM Counters from reset*/
    HWIO_BPM_CFG_OUT(
          0x3 << HWIO_BPM_CFG_CGC_CLK_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_SW_ACCUMULATE_ENA_SHFT
        | 0x1 << HWIO_BPM_CFG_RESTART_LOCK_DIS_SHFT );
        
    /* Get ready to start counters */
    HWIO_BPM_CMD_OUT( 0 );

    /* Backup, reset, restore BUFC_PROF_CFG {backup required bcos these registers
       are already configured by apm_start_log() routine} */
    x = HWIO_EBI1_BUFC_PROF_COUNT_CFG_IN;
    HWIO_EBI1_BUFC_PROF_COUNT_CFG_OUT (0);
    HWIO_EBI1_BUFC_PROF_COUNT_CFG_OUT (x);
    
#ifdef T_SDRAM
    /* Backup, reset, restore SDRAM_PERF_CNTRn_CFG */
    for (i=0; i<2; ++i)
    {
      x = HWIO_SDRAM_PERF_CNTRn_CFG_INI (i);
      HWIO_SDRAM_PERF_CNTRn_CFG_OUTI (i, 1);
      HWIO_SDRAM_PERF_CNTRn_CFG_OUTI (i, x);
    }
#endif    
    
    INTLOCK();
    /* start the apm_log_sample_timer */
    clk_reg(&apm_log_sample_timer,
            apm_log_sample_timer_cb,
            (int4) apm_config_table.sample_period,
            (int4) 0,
            FALSE);

    /* Start counters */
    HWIO_BPM_CMD_OUT(
          0x0 << HWIO_BPM_CMD_SW_RESET_SHFT
        | 0x1 << HWIO_BPM_CMD_ACCUMULATE_SHFT );

    INTFREE();
  }

  /* for different version number, the HW design is different and  the procedure 
     to start the monitor will be different as well.  Future procedure 
     implementations, SW should check for version number first, then implement the
      procedure accordingly */ 

}   /* end apm_start_monitor() */

/*===========================================================================

FUNCTION  apm_log_sample_timer_CB

DESCRIPTION
   This function is called every apm_config_table.sample_period ms after 
   apm_start_logging() is called, until apm_log_sample_timer is dereged. 
   apm_log_sample_timer is dereged when apm_config_table.sample_iterations expires.
   
   This function will  do
   1. alloc a log packet. 
   2. fill the log packet with numbers in apm_trans_monitor and 
      apm_latency_monitor
   3. send the log packet. 
   
   Or maybe I should ssend a signal and do these in task context?  if do
   that, need to INTLOCK() m
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void apm_log_sample_timer_cb(int dummy)
{
  int i;

    apm_samples_collected++;

  #ifndef FEATURE_APM_BUFFERED_LOG
    apm_log_type *log = NULL;
    const unsigned int log_length = sizeof (apm_log_type);

    /* In this case, we have room for only one sample */
    apm_log_buffer_index = 0;
  #endif

  /* Stop BPM counters */
  HWIO_BPM_CMD_OUT( 0x0 << HWIO_BPM_CMD_ACCUMULATE_SHFT );
  
#if 0
  uint32 x;
  /* Freeze BUFC_PROF counters (backup, freeze, restore */
  x = HWIO_EBI1_BUFC_PROF_COUNT_CFG_IN;
  HWIO_EBI1_BUFC_PROF_COUNT_CFG_OUT( 0x111 );
  HWIO_EBI1_BUFC_PROF_COUNT_CFG_OUT (x);
#endif
  
  /* Read BPM cycle counter */
  apm_counter_values[apm_log_buffer_index++] = HWIO_BPM_CYC_OUT_IN;
  
  /* Read BPM port counters */
  for ( i=0; i<APM_MAX_NUM_BPM_PORT_COUNTERS; i++ )
  {
    apm_counter_values[apm_log_buffer_index++] = HWIO_BPM_Cn_OUT_INI(i);
  }

  /* Read BUFC_PROF counters */
  apm_counter_values[apm_log_buffer_index++] = HWIO_EBI1_BUFC_PROF_COUNT0_IN;
  apm_counter_values[apm_log_buffer_index++] = HWIO_EBI1_BUFC_PROF_COUNT1_IN;
  apm_counter_values[apm_log_buffer_index++] = HWIO_EBI1_BUFC_PROF_COUNT2_IN;

#ifdef T_SDRAM
  /* Read SDRAM_PERF_CNTR counters */
  apm_counter_values[apm_log_buffer_index++] = HWIO_SDRAM_PERF_CNTRn_OUT_INI(0);
  apm_counter_values[apm_log_buffer_index++] = HWIO_SDRAM_PERF_CNTRn_OUT_INI(1);
  apm_counter_values[apm_log_buffer_index++] = HWIO_SDRAM_PERF_CNTR_STATUS_IN;
#else  
  /* No SDRAM, just reset the place holders */
  apm_counter_values[apm_log_buffer_index++] = 0;
  apm_counter_values[apm_log_buffer_index++] = 0;
  apm_counter_values[apm_log_buffer_index++] = 0;
#endif  

#ifndef FEATURE_APM_BUFFERED_LOG
  /* Allocate a LOG packet  */
  log = (apm_log_type *) log_alloc (LOG_APM_C, log_length);

  if (log)
  {
    log->apm_dropped_pkt_cnt = apm_status_table.dropped_pkts;

    /* fill the log packet with numbers in apm_counter_values */
    memcpy( (void *)  log->uxmc_counters,
            (void *)  apm_counter_values,
            sizeof(apm_counter_values) );

    /* send the log packet */
    log_commit(log);

  }
  else
  { /* else, log packet is dropped due to limited memory pool */
    apm_status_table.dropped_pkts++;
  }
#endif /* FEATURE_APM_BUFFERED_LOG */

  apm_status_table.max_index_written = apm_log_buffer_index - 1;

  /* Start UXMC monitors again if more to do */
  if ( apm_samples_collected < apm_total_samples )
  {
    apm_start_monitor();
  }
  else
  {
    apm_status_table.total_samples = apm_total_samples;

#ifdef FEATURE_APM_BUFFERED_LOG
#error code not present
#else
    apm_monitor_active = FALSE; /* That's all Folk! */
#endif /* FEATURE_APM_BUFFERED_LOG */
  }
} /* end apm_log_sample_timer_cb() */

/*===========================================================================

FUNCTION  APM_SEND_MEM_BUFFER_CB

DESCRIPTION
   This function will start a new timer and send the memory buffered data 
   back to the host.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void apm_send_mem_buffer_timer_cb(int dummy)
{
/* HACK_DEBUG 2 */

   apm_log_type *log = NULL;
   const unsigned int log_length = sizeof (apm_log_type);

   /* Allocate a LOG packet  */
   log = (apm_log_type *) log_alloc (LOG_APM_C, log_length);

   if (log)
   {
     log->apm_dropped_pkt_cnt = apm_status_table.dropped_pkts;

     /* fill the log packet with numbers in apm_counter_values */
     memcpy( (void *)  log->uxmc_counters,
             (void *)  &apm_counter_values[APM_TOTAL_COUNTERS_TO_READ * (apm_status_table.samples_sent)],
             sizeof(log->uxmc_counters) );

     /* send the log packet */
     log_commit(log);

     apm_status_table.samples_sent++; /* another one sent */

   }
   else
   { /* else, log packet is dropped due to limited memory pool */
     apm_status_table.diag_retries++;
   }

   if ( apm_status_table.samples_sent >= apm_total_samples ) /* Are we there yet? */
   {
      apm_monitor_active = FALSE; /* That's all Folk! */
   }
   else
   {
      clk_reg(&apm_send_mem_buffer_timer,
            apm_send_mem_buffer_timer_cb,
            (int4) apm_send_mem_buffer_timer_period,
            (int4) 0,
            FALSE);
   }
        

} /* apm_send_mem_buffer_timer_cb() */


/*===========================================================================

FUNCTION  APM_GET_CFG_TBL_ADDR

DESCRIPTION
   This function will start Duration timer and start UXMC bus Monitor    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
PACKED void * apm_get_cfg_tbl_addr(
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
)
{
  DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_rsp_type *rsp_ptr;

  DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_req_type *req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req_ptr = ( DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_req_type * ) in_req_ptr;


  /* Verify that the length passed in and the action specified are
  ** valid.
  */
  if ( pkt_len != sizeof( DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_req_type ) )
  {
    return diagpkt_err_rsp(
             DIAG_BAD_LEN_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_req_type ) );
  }


  /* Allocate the response packet. */
  rsp_ptr = ( DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_rsp_type * )
    diagpkt_alloc( diagpkt_get_cmd_code( req_ptr ),
                   sizeof( DIAG_SUBSYS_PROF_GET_CFG_TBL_ADDR_F_rsp_type ) );

  /* The response packet looks a lot like the request packet. */
  memcpy( ( void* ) rsp_ptr,
          ( void* ) req_ptr,
          sizeof( diagpkt_subsys_header_type ) );

  /* Tell the terminal which profiling items are actually supported.
     Start off assuming we support nothing. */
  rsp_ptr->prof_mask = 0;


  if ( req_ptr->prof_mask & PROFDIAG_UXMC_STATS_BIT )
  {
      /* Start the profiling of the UXMC buses. */
    rsp_ptr->prof_mask |= PROFDIAG_UXMC_STATS_BIT;

    rsp_ptr->cfg_addr  = (void *) &apm_config_table;
    rsp_ptr->stat_addr = (void *) &apm_status_table;
  }

  #ifdef FEATURE_SIMULATE_LACK_OF_BW_TO_GPS
#error code not present
  #endif

  return rsp_ptr;

} /* end apm_get_cfg_tbl_addr() */

/*===========================================================================

FUNCTION  APM_START_LOG

DESCRIPTION
   This function will start Duration timer and start UXMC bus Monitor    

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
PACKED void * apm_start_log(
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
  )
{
  int i;

  DIAG_SUBSYS_PROF_START_LOG_F_rsp_type *rsp_ptr;

  DIAG_SUBSYS_PROF_START_LOG_F_req_type *req_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* clear error history */
  apm_status_table.error_notice = apm_err_none;
  apm_status_table.total_samples = 0;
  apm_status_table.samples_sent = 0;
  apm_status_table.dropped_pkts = 0;
  apm_status_table.diag_retries = 0;

  req_ptr = ( DIAG_SUBSYS_PROF_START_LOG_F_req_type * ) in_req_ptr;


  /* Verify that the length passed in and the action specified are
  ** valid.
  */
  if ( pkt_len != sizeof( DIAG_SUBSYS_PROF_START_LOG_F_req_type ) )
  {
    return diagpkt_err_rsp(
             DIAG_BAD_LEN_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_START_LOG_F_req_type ) );
  }

  if (!apm_validate_config_table())
  {

    return diagpkt_err_rsp(
             DIAG_BAD_PARM_F,
             req_ptr,
             sizeof( DIAG_SUBSYS_PROF_START_LOG_F_req_type ) );
  }

  /* Allocate the response packet. */
  rsp_ptr = ( DIAG_SUBSYS_PROF_START_LOG_F_rsp_type * )
    diagpkt_alloc( diagpkt_get_cmd_code( req_ptr ),
                   sizeof( DIAG_SUBSYS_PROF_START_LOG_F_rsp_type ) );

  /* The response packet looks a lot like the request packet. */
  memcpy( ( void* ) rsp_ptr,
          ( void* ) req_ptr,
          sizeof( diagpkt_subsys_header_type ) );

  /* Tell the terminal which profiling items are actually supported.
     Start off assuming we support nothing. */
  rsp_ptr->prof_mask = 0;

  if ( req_ptr->prof_mask & PROFDIAG_UXMC_STATS_BIT )
  {
      /* Start the profiling of the UXMC buses. */
    rsp_ptr->prof_mask |= PROFDIAG_UXMC_STATS_BIT;

    /* Configure the counters per the config table */
    for (i =0; i < apm_config_table.num_registers; i++)
    {
      out_dword( apm_config_table.registers[i].addr,
                 apm_config_table.registers[i].value);
    }

    apm_total_samples = apm_config_table.sample_iterations;

    apm_monitor_active = TRUE;
    apm_samples_collected = 0;

    /* Reset log buffer index */
    apm_log_buffer_index = 0;

    apm_start_monitor(); 
  }

  /* not support other profiling logs now , can be extended in the future */

  return rsp_ptr;
} /* end apm_start_log() */

