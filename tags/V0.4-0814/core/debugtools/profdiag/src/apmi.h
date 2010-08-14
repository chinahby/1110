
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           A H B  P e r f o r m a n c e  M o ni t o r   D r i v e r
          
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc6055/services/profdiag/apmi.h#6 $  $DateTime: 2008/06/04 22:29:14 $  $Author: rrevanur $ 


 when      who     what, where, why
 --------  -----   ----------------------------------------------------------
 01/08/07  fs      cycle counter added to apm_log_type
 09/06/06  cc      Creat the file.

 
============================================================================*/

/*============================================================================

                       INCLUDE FILES FOR MODULE
                       
============================================================================*/
#include "comdef.h"
#include "diagpkt.h"
#include "log.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define APM_MAX_NUM_CFG_REGISTERS    ( 256 * 3 ) /* by request */   
#define APM_MAX_SAMPLES               3000   /* number of samples */
#define APM_MIN_SAMPLE_PERIOD           10   /* msec */
#define APM_MAX_SAMPLE_PERIOD          250   /* msec */
#define APM_CONFIG_TABLE_VERSION     (0x00030000)

#define APM_MAX_NUM_BPM_PORT_COUNTERS   (4)
#define APM_MAX_NUM_BPM_CYCLE_COUNTERS  (1)
#define APM_TOTAL_BPM_COUNTERS           (APM_MAX_NUM_BPM_PORT_COUNTERS         \
                                             + APM_MAX_NUM_BPM_CYCLE_COUNTERS)

#define APM_MAX_NUM_BUFC_PROF_COUNTERS  (3)
#define APM_MAX_NUM_SDRAM_PERF_COUNTERS (3)

#define APM_TOTAL_COUNTERS_TO_READ      (APM_TOTAL_BPM_COUNTERS                 \
                                            + APM_MAX_NUM_BUFC_PROF_COUNTERS    \
                                            + APM_MAX_NUM_SDRAM_PERF_COUNTERS)

/* Values and meanings for apm_error_notice variable. */
#define apm_err_none               0
#define apm_err_version            1
#define apm_err_num_regs_too_big   2
#define apm_err_sample_iterations  3
#define apm_err_sample_period      4
#define apm_err_address            5
#define apm_err_num_regs_wrong     6

typedef PACKED struct
{
  uint32 addr;
  uint32 value;
} apm_reg_addr_value_type;

typedef PACKED struct
{
  uint32 addr;
  uint32 value;
  uint32 mask;
} apm_reg_addr_value_mask_type;

typedef PACKED struct
{
  uint32 version;        /* version # identify the procedure of operating 
                            version # identify the procedure of operating
                            AHB performance Monitors. it should value 1 for 
                            MSMs priori to SC2X ?? Huh??  We'll use the value
                            of 6055 for now... To try and make sure that we
                            don't collide with anything else. */

  uint32 num_registers;  /* Total number of registers to program, 
                            For version 1 HW, maxim number is 16 (4 latency
                            Monitors, 4 transection moniter each with CFG0, CFG1 
                            and CTRL registers. 
                            Future HW design can have as much as 96 registers */
  uint32 sample_period;  /* How often to read counters & send log pkt? (ms) */
  uint32 sample_iterations;  /* Total duration (ms) to run test */

  uint32 reserved_0;
  uint32 reserved_1;
  uint32 reserved_2;
  uint32 reserved_3;
  uint32 reserved_4;
  uint32 reserved_5;
  uint32 reserved_6;
  uint32 reserved_7;

  apm_reg_addr_value_type  registers[APM_MAX_NUM_CFG_REGISTERS];
}apm_config_table_type;


typedef PACKED struct 
{
  log_hdr_type    xx_header; 
  uint32   apm_dropped_pkt_cnt;
  uint32   uxmc_counters[APM_TOTAL_COUNTERS_TO_READ];
} apm_log_type;

typedef PACKED struct
{ 
  uint32   error_notice;
  uint32   total_samples;
  uint32   samples_sent;
  uint32   dropped_pkts;
  uint32   diag_retries;
  uint32   max_index_written;
  uint32   reserved_5;
  uint32   reserved_6;
  uint32   reserved_7;
  uint32   reserved_8;
  uint32   reserved_9;
  uint32   reserved_10;
  uint32   max_cfg_regs;
  uint32   min_sample_period;
  uint32   max_sample_period;
  uint32   max_sample_iterations;
} apm_status_type;

