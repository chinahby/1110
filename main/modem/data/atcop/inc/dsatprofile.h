#ifndef DSATPROFILE_H
#define DSATPROFILE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D  P R O F I L E                        
                        

GENERAL DESCRIPTION
  This module contains AT commands funcions and variables if the main AT 
  commands code is featurized out of the build 

   Copyright (c) 2001 - 2011 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatprofile.h#5 $ $DateTime: 2011/04/20 04:28:48 $ $Author: c_avijay $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/11   dvk     Fixed compilation issues.
05/18/09   bs      Fixed compilation issues.
03/05/09   bs      Added support for mmgsdi interface.
01/30/09   sn      Added support for SCRM, TRTL.
10/15/07 sq/mga/yz Merged enums dsat_mode_enum_type and dsat_result_enum_type
11/10/06   an      Fixed compilation errors for 6020
11/01/06   an      Created this file for providing stub functions when AT 
                   commands code is stripped out of the build



*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "comdef.h"
#include "customer.h"
#include "nv.h"
#include "sio.h"
#include "dstask.h"

#ifdef FEATURE_DATA_STRIP_ATCOP

typedef unsigned int dsat_num_item_type;

typedef enum
{
  DSAT_DIAL_EMPTY,          /*  Empty                    */
  DSAT_DIAL_DIGIT,          /*  Digits                   */
} dsat_dial_val_e_type;


/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
     The following enumerated type represents the states of ATCOP: command, 
     online data, and online command. The ME is in command state when the 
     transport layer is not in the ESTABLISHED state. When in command state
     or online command state, all AT commands received on Rm are processed.
     When the ME is in the online data state, the ME passes all data received
     on the Rm directly to the IWF.  
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CMD, 
  DSAT_ONLINE_DATA,
  DSAT_ONLINE_CMD
} dsat_mode_enum_type;

/*---------------------------------------------------------------------------
    The numerical values in this "enum" are important:  
    The values for OK thru NO_ANSWER must match the numeric codes in EIA-602.                         
    The values for the cellular extention codes must begin with the 
      value MIN_CELL_RES (defined in dsati.h), and increase in sequence.            
    The values for the cellular error codes must being with the value 
      MIN_CELL_ERR (defined in dsati.h), and increase in sequence.

    No specific values are required for ETSI extended format of incoming
    indications, as they are only displayed as strings.
    
    The values for the internal codes must be negative, other than those
    used to identify the range of particular result code types.
---------------------------------------------------------------------------*/

typedef enum
{
  /*-------------------------------------------------------------------------
    Numbered per ITU-T V.25 ter
  -------------------------------------------------------------------------*/
  DSAT_OK             = 0,    /*  AT: OK            */
  DSAT_CONNECT        = 1,    /*  AT: CONNECT       */
  DSAT_RING           = 2,    /*  AT: RING          */
  DSAT_NO_CARRIER     = 3,    /*  AT: NO CARRIER    */
  DSAT_ERROR          = 4,    /*  AT: ERROR         */
  DSAT_NO_DIALTONE    = 6,    /*  AT: NO DIAL TONE  */
  DSAT_BUSY           = 7,    /*  AT: BUSY          */
  DSAT_NO_ANSWER      = 8,    /*  AT: NO ANSWER     */
  DSAT_MAX_BASIC      = 9,    /*  End of basic result codes                */

  DSAT_CRC_CODE_VOICE = 31,  /* normal voice                       */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)  
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /*-------------------------------------------------------------------------
     Not reported to TE
     Note that DSAT_CXT_ORIG is defined as -13 and not as -12  
     because some builds used -12 for other definition.
  -------------------------------------------------------------------------*/
  DSAT_ATH_CMD        = -18,  /*  internal: hangup                   */
  DSAT_ASYNC_EVENT    = -17,  /*  internal: async event, not associated
                                            with a command              */
  DSAT_ASYNC_CMD      = -16,  /*  internal: async command in progress   */
  DSAT_CMD_ERR_RSP    = -15,  /*  internal: send cmd specific error
                                            rsp only and no result code */

  DSAT_NO_RSP         = -14,  /*  internal: send no response to cmd  */
  DSAT_CXT_ORIG       = -13,  /*  internal: originate w/ unknown cmd */

  DSAT_DO_QCOTC       = -11,  /*  internal: open traffic channel     */
  DSAT_DO_DM          = -10,  /*  internal: change to DM mode        */
  DSAT_DO_QCDAM       = -9,   /*  internal: analog dial for PCMCIA   */
  DSAT_DO_HARD_PARSE  = -8,   /*  internal: parse cmd w/o delimiter  */
  DSAT_DO_CHV         = -7,   /*  internal: cellular hang up voice   */
  DSAT_DO_CDV         = -6,   /*  internal: cellular dial voice      */
  DSAT_TEST           = -5,   /*  internal: remote test command      */
  DSAT_PARAM          = -4,   /*  internal: parameter out of range   */
  DSAT_UNREC          = -3,   /*  internal: unrecognized command     */
  DSAT_DO_ANS         = -2,   /*  internal: answer  (auto or ATA)    */
  DSAT_DO_ORIG        = -1    /*  internal: originate data call      */

} dsat_result_enum_type;

typedef enum
{                       
  ATOI_OK,              /*  conversion complete             */
  ATOI_NO_ARG,          /*  nothing to convert              */
  ATOI_OUT_OF_RANGE     /*  character or value out of range */
} atoi_enum_type;

/*-------------------------------------------------------------------------
  Dial string for Packet/QNC calls. Defaults to "#777". Note that if the
  default string is changed, the length variable should also be updated.
-------------------------------------------------------------------------*/
extern byte dsat707_pkt_orig_str[];

extern dsat_num_item_type dsat707_crm_val;

extern dsat_num_item_type dsat707_qcmdr_val;

extern dsat_num_item_type dsat707_qcmip_val;

extern dsat_num_item_type dsat707_cta_val;

extern dsat_num_item_type dsat707_qcqnc_val;

#ifdef FEATURE_DS_IS707A_CMUX
extern dsat_num_item_type dsat707_cmux_val[];
extern const word dsat707_cmux_rev_table[];
extern const word dsat707_cmux_fwd_table[];
#else
extern dsat_num_item_type dsat707_cmux_val;
#endif /* FEATURE_DS_IS707A_CMUX */

/* Which s.o. set */
extern dsat_num_item_type dsat707_qcso_val;

extern dsat_num_item_type dsat_ipr_val;

extern uint32 dsat_qcdns_val[];

/*  Numeric Config items  */
#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
extern dsat_num_item_type dsat707_qcchs_val;
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
extern dsat_num_item_type dsat707_qcscrm_val;

/* TURN R-SCH throttle on/off */
extern dsat_num_item_type dsat707_qctrtl_val;
#endif /* FEATURE_IS2000_R_SCH */

#define MAX_VAL_NUM_ITEM 0xFFFFFFFF /*  Max value of a numeric AT parm     */

/*---------------------------------------------------------------------------
                           NV interface data 
---------------------------------------------------------------------------*/
extern nv_item_type         ds_nv_item;            /* actual data item     */

/*===========================================================================
  FUNCTION DSAT707_QCMIP_CHECK_MDR_VAL

  DESCRIPTION
    If the value of MDR is being changed to a value other than three, check 
    the value of QCMIP.  If it is not 0, return an error.
         
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean dsat707_qcmip_check_mdr_val
(
  void
);

/*===========================================================================
  FUNCTION DSAT707_SET_MDR_VAL

  DESCRIPTION
  This function sets the MDR value into NV and synchronizes the available
  service options.
  
  This function can be called from UI, if UI decides to have an interface
  to change the value.
  
  DEPENDENCIES
  Since it writes to NV, must execute in DS task.
  
  RETURN VALUE
  Boolean indicating success/fail.
  
  SIDE EFFECTS
  None.
===========================================================================*/
boolean dsat707_set_mdr_val
(
  unsigned int    qcmdr_val
);

/*===========================================================================
FUNCTION      DSATPROFILE_NV_SYNC

DESCRIPTION   This function reads all the 707-specific NV items out of
              NV.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dsatprofile_nv_sync(void);

#ifdef FEATURE_UIM_SUPPORT_3GPD
/*===========================================================================

FUNCTION  DSATPROFILE_INIT_ME

DESCRIPTION
  ME initialization function. This function does the following:
  - Registers the client call back function for GSDI events,
  - Register as a client of MMGSDI.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatprofile_init_me ( void );

/*===========================================================================

FUNCTION  DSATPROFILE_GSDI_EVENT_HANDLER

DESCRIPTION
  This is the handler function for CDMA specific GSDI events.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatprofile_gsdi_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#endif  /* FEATURE_DATA_STRIP_ATCOP */

#endif  /* DSATPROFILE_H */

