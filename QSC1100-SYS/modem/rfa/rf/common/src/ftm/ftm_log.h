/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Log

GENERAL DESCRIPTION
  This is the header file for FTM Logging.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/ftm/ftm_log.h#1 $ 
  $DateTime: 2008/04/11 05:11:57 $ 
  $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/06    dp     Add HDET limiting loop logging for UMTS targets.
06/26/06   et      GSM QUADBAND support
02/01/06   rsr     Lint fixes.
09/12/05   hkk     Changes for TX/RX sweep cal 
06/19/05   jac     Again, made log ID param of ftm_log_malloc() uint16 so log codes
                   larger than 0xFF can always be used.
06/14/05   rsr     Added log support for therm and vbatt
05/09/05   bmg     Added IntelliCeiver log
03/07/05   bmg     Made log ID param of ftm_log_malloc() uint16 so log codes
                   larger than 0xFF can always be used.
12/09/04   rsr     Added support for autocalibration
11/12/04   ra      Add FTM_LOG_HDET_TRACKING
10/12/04   jac     Mainlined ASYNC packet code.
09/16/04   dp      Add WCDMA AGC and FTM GSM BER log codes
09/08/04   ra      1X merge with UMTS
10/17/03   ka      Removed expiration parameter from log enable API.
10/15/03   ka      Added data structures for flexible callback-based logging.
10/08/03   ka      Added new ftm log types for DC loop monitoring.
09/16/03   wd      Added define for FTM_LOG_HEADER_SIZE wich returns the size
                   of the FTM LOG Header with out the data.
09/12/03   wd      Initial revision.

===========================================================================*/

#ifndef FTM_LOG_H
#define FTM_LOG_H

#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm.h"
#include "ftmicap.h"
#include "ftmdiag.h"


#ifdef FTM_HAS_LOGGING



#define FTM_LOG_ELEMENT_COUNT(x) ( sizeof(x) / sizeof(x[0]))


/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "log_codes.h"
#if defined(FTM_HAS_1X)
#include "log_dmss.h"
#include "ftm_1x_log.h"
#elif defined(FTM_HAS_UMTS)
#error code not present
#else
#error need appropriate log header
#endif


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/
/* FTM LOG Header size with out data, since the data size depends on the 
   FTM log id 
*/
#define FTM_LOG_HEADER_SIZE (sizeof(log_ftm_gen_type) - 1)

/* FTM Log IDs */
typedef enum {
  
#if (defined(FTM_HAS_UMTS) || defined(FTM_HAS_GSM))
#error code not present
#endif     
    
  FTM_LOG_PRINTF          = 0x0,
  FTM_LOG_BT              = 0x1,

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif
  
  FTM_LOG_1X_AGC          = 0x4,
  FTM_LOG_HDR_AGC         = 0x5,
  FTM_LOG_1X_AGC_C1       = 0x6,
  FTM_LOG_HDR_AGC_C1      = 0x7,
  FTM_LOG_AUTOCAL         = 0xA, 

  FTM_LOG_GSM_THERM_VBATT  = 0xB,
  FTM_LOG_TX_RX_SWEEP      = 0xB,
#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */

  FTM_LOG_INTELLICEIVER   = 0xB,

  FTM_LOG_MAX,
} ftm_log_id_type;

typedef enum {
  FTM_LOG_ENABLE,
  FTM_LOG_DISABLE,
  FTM_LOG_CMD_MAX,
} ftm_log_cmd_type;



#if defined(FTM_HAS_UMTS)
#error code not present
#endif /* FTM_HAS_UMTS */


#ifdef FEATURE_FTM_HWTC
#error code not present
#endif /* FEATURE_FTM_HWTC */


typedef PACKED struct
{
  uint16          log_id;
  int4            interval;
  int             num_buffer;
} ftm_log_packed_param_type;

/* This type specifies the full request packet (including diag subsystem header) for configuration of
 * a particular FTM log id 
 *
 */
typedef PACKED struct
{
  diagpkt_subsys_header_type  diag_hdr;
  ftm_cmd_header_type         ftm_hdr;
  ftm_log_packed_param_type   params;
} ftm_log_req_pkt_type;

typedef void (ftm_log_cb_func_ptr_type)(int4);

typedef struct
{
  clk_cb_type              *cb_struct;
  ftm_log_cb_func_ptr_type *cb_func;
  boolean                  active;
  int                      num_buffer;
} ftm_log_config_type;

typedef void (*ftm_log_init_fn_type)( void);
typedef void (*ftm_log_exit_fn_type)( void);


typedef struct
{
    
    ftm_log_id_type            log_id;
    ftm_log_config_type        log_config;
    ftm_log_init_fn_type       log_init_func;
    ftm_log_exit_fn_type       log_exit_func;

} ftm_log_config_table_entry;

/*===========================================================================

                          FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

FUNCTION FTM_LOG_MALLOC

DESCRIPTION
   This function allocates memory for an ftm lok pkt.

DEPENDENCIES
   None.

===========================================================================*/
PACKED void * ftm_log_malloc( uint16 ftm_log_id, unsigned int length);

/*===========================================================================

FUNCTION FTM_LOG_GET_DATA_PTR

DESCRIPTION
   This function returns a ptr to the data portion of a given ftm log pkt.

DEPENDENCIES
   None.

===========================================================================*/
PACKED void * ftm_log_get_data_ptr (void *log_pkt);

/*===========================================================================

FUNCTION FTM_LOG_PRINTF

DESCRIPTION
  This function is a printf clone, it will work just as printf does, but 
  will output logged to the dm monitor. 


DEPENDENCIES
   None.

===========================================================================*/
void ftm_log_printf(const char *fmt, ... );

void ftm_log_enable(ftm_log_id_type log_id, int4 interval, int num_buffer);
void ftm_log_disable(ftm_log_id_type log_id);

ftm_rsp_pkt_type ftm_log_dispatch(ftm_log_req_pkt_type  *cmd_ptr);

#endif /* FEATURE_FTM_LOG */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_LOG_H */
