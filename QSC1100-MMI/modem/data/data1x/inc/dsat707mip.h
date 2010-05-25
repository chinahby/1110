#ifndef DSAT707MIP_H
#define DSAT707MIP_H
/*===========================================================================

                    D A T A   S E R V I C E S  M I P

                A T   C O M M A N D   P R O C E S S O R
                
                I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode Mobile IP commands ) processor.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.

dsat707_exec_qcmipp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

dsat707_exec_qcmipt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.

dsat707_exec_qcmipep_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.

dsat707_exec_qcmipmass_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

dsat707_exec_qcmipmhss_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

dsat707_exec_qcmipmaspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

dsat707_exec_qcmipmhspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

dsat707_exec_qcmiprt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.
  
dsat707_exec_qcmipnai_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.
  
dsat707_exec_qcmipha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.
  
dsat707_exec_qcmippha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

dsat707_exec_qcmipsha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.
  
dsat707_exec_qcmipgetp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.
  
  Copyright (c) 1995 - 2008 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mip.h_v   1.6   02 Dec 2002 20:00:02   akhare  $
$Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/inc/dsat707mip.h#2 $ $DateTime: 2008/07/21 02:23:28 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/08   bs      Added support for CDMA NAARefresh.
12/02/02   ak      Made set_mdr_val and set_crm_val external functions.
11/14/02   jd      Removed prototypes for dsat707_erase_shadow_nv 
09/18/02   jay     Removed AT_TEST feature and formatted the code.
07/12/02   ak      Extern for dsat707_check_mdr_val.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_DS_MOBILE_IP

#include "dsat.h"
#include "dsati.h"

/*---------------------------------------------------------------------------
  Shadow NV - For certain attributes which can be set using AT Commands,  
  the user has an option of either committing the entered value to NV
  or using the value uncommitted. This soft NV structure keeps track of
  such preferences. 
---------------------------------------------------------------------------*/ 
typedef struct
{
  char *nai;              /* NAI                         */
  byte *mn_aaa_ss;        /* MN-AAA Shared Secret        */
  byte *mn_ha_ss;         /* MN-HA Shared Secret         */
  byte *mn_aaa_ss_len;    /* MN-AAA Shared Secret Length */
  byte *mn_ha_ss_len;     /* MN-HA Shared Secret Length  */
  boolean *rev_tun;       /* Reverse Tunneling toggle    */
  uint32 *mn_aaa_spi;     /* MN-AAA SPI                  */      
  uint32 *mn_ha_spi;      /* MN-HA SPI                   */
  uint32 *ha_ip ;         /* Home Address                */
  uint32 *pri_ha_ip;      /* Pri HA IP addr              */
  uint32 *sec_ha_ip;      /* Sec HA IP addr              */
}shadow_nv_type;

extern shadow_nv_type shadow_nv;
extern byte ss_size;

/*--------------------------------------------------------------------------
  Return type for ds_atcop_init_mip_profile()
---------------------------------------------------------------------------*/
typedef enum
{
  INIT_MIP_PROF_OK,
  INIT_MIP_PROF_ERROR   /* Profile Initialization error */
}dsat707_mip_result_type;

/*--------------------------------------------------------------------------
  Return type for ds_atcop_strip_quotes() and ds_atcop_ip_str_to_uint32()
---------------------------------------------------------------------------*/
typedef enum
{
  STRIP_QUOTES_OK,
  IP_STR_UINT32_OK,
  STRIP_QUOTES_ERROR,   /* Strip quotes error */
  IP_STR_UINT32_ERROR   /* Conversion error */
}dsat707_util_result_type;

/*---------------------------------------------------------------------------
  qcmipt value is a bit field and can be used for testing various Mobile
  IP  features
  QCMIPT    Behavior
   -------------------------
  0         No MN_HA_Auth_2002bis, No Clear_txt DMU
  1         MN_HA_Auth_2002bis, No Clear_txt DMU
  2         No MN_HA_Auth_2002bis, Clear_txt DMU
  3         MN_HA_Auth_2002bis, Clear_txt DMU
---------------------------------------------------------------------------*/
#define MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT 0x01  /* RFC2002bis MN-HA Auth   */
#define MIP_QCMIPT_DMU_CLEARTEXT_BIT      0x02  /* clear text mode in DMU  */
#define MIP_QCMIPT_RSVD_03_BIT            0x04  /* reserved for future use */
#define MIP_QCMIPT_RSVD_04_BIT            0x08  /* reserved for future use */
#define MIP_QCMIPT_RSVD_05_BIT            0x10  /* reserved for future use */
#define MIP_QCMIPT_RSVD_06_BIT            0x20  /* reserved for future use */
#define MIP_QCMIPT_RSVD_07_BIT            0x40  /* reserved for future use */
#define MIP_QCMIPT_RSVD_08_BIT            0x80  /* reserved for future use */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.
  0 - Mobile IP disabled, Simple IP only.
  1 - Mobile IP preferred.
  2 - Mobile IP only.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmip_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipp_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.
  0 - Use of RFC 2002bis authentication is disabled. RFC 2002
  style authentication is used instead.
  1 - Use of RFC 2002bis authentication is enabled.  
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);  /* dsat707_exec_qcmipt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPEP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.
  0 - Disable the currently active profile.
  1 - Enable the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipep_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmass_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipep_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhss_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmhss_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmaspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmaspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmhspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmiprt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmiprt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPNAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipnai_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipnai_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmippha_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr /*  Place to put response       */
); /* dsat707_exec_qcmippha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPSHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipsha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipsha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPGETP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipgetp_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipgetp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASSX command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmassx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSSX command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcmipmhssx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);

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
  FUNCTION DS_ATCOP_MIP_USING_SHADOW_NV

  DESCRIPTION
   Checks if any of the shadow NV values are in use for MIP.

   Called from PS so make sure no inter-task contention issues

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE  - using shadow NV
    FALSE - not using shadow NV

  SIDE EFFECTS
   None
===========================================================================*/
boolean ds_atcop_mip_using_shadow_nv
(
  void
);

/*===========================================================================
  FUNCTION DSAT707_INIT_MIP_PROF

  DESCRIPTION
   This function initializes the MIP user profile and sets the default
   values.

  DEPENDENCIES
   None

  RETURN VALUE
    status ( success or error)

  SIDE EFFECTS
   None
===========================================================================*/
dsat707_mip_result_type dsat707_init_mip_prof
(
  void
) ;

/*===========================================================================
FUNCTION DSAT707_ERASE_SHADOW_NV

DESCRIPTION
  This functions flushes the shadow NV structure. It is called at the
  initilization of ATCoP,when the active profile has been changed and
  when the call is ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsat707_erase_shadow_nv
(
  void
);

/*===========================================================================
  FUNCTION DSAT707_FILL_MIP_ATCOP_VALS

  DESCRIPTION
   This function copies the values retreived from NV into atcop_val which is
   used for display.

  DEPENDENCIES
   None

  RETURN VALUE
    None

  SIDE EFFECTS
   None
===========================================================================*/
extern void dsat707_fill_mip_vals
(
  nv_stat_enum_type (* nv_get_fn)(nv_items_enum_type, nv_item_type *),
  nv_item_type *  nv_item
);

/*===========================================================================
FUNCTION DSAT707_QCMIP_SET_CRM_VAL

DESCRIPTION
  This function checks the value of QCMIP.  If it is greater than 0, it
  modifies the CRM value to 2, which enables network model operation.
  Changing the value QCMIP back to 0 will restore CRM to its original value
  before it was changed by QCMIP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat707_qcmip_set_crm_val(void);

/*===========================================================================
  FUNCTION DSAT707_QCMIP_SET_MDR_VAL

  DESCRIPTION
    This function checks the value of QCMIP.  If it is greater than 0, it
    modifies the MDR value to SO 33 preferred.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsat707_qcmip_set_mdr_val(void);

/*===========================================================================
  FUNCTION DSAT707_QCMIP_UPDATE

  DESCRIPTION
   Called by $QCMIP command and refresh event handler to update
   values associated with NV_DS_QCMIP_I.

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE : if read is SUCCESS
    FALSE: if read is FAILURE

  SIDE EFFECTS
   None
===========================================================================*/
boolean dsat707_qcmip_update
(
  boolean read_flag
);

/*===========================================================================
  FUNCTION DSAT707_QCMIPP_UPDATE

  DESCRIPTION
   Called by refresh event handler to update values associated with 
   NV_DS_MIP_ACTIVE_PROF_I.

  DEPENDENCIES
   None

  RETURN VALUE
    TRUE : if success
    FALSE: if failure

  SIDE EFFECTS
   None
===========================================================================*/
boolean dsat707_qcmipp_update
(
  boolean read_write_flag
);

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */
#endif /* FEATURE_DATA         */

#endif /* DSAT707MIP_H         */
