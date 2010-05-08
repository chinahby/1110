#ifndef MCCAP_H
#define MCCAP_H
/*===========================================================================

          MAIN CONTROL SYSTEM CAPACITY MANAGER

GENERAL DESCRIPTION
   This module is keeping track of the underlying hardware and system capability.
   All other call processing modules look to this module to figure out if a
   particular parameter or configuration is do-able by the mobile.

DESCRIPTION
   This module has data structures to keep track of the capacity info and
   the interfaces to allow for the other modules to query  or validate any
   parameter

Copyright (c) 1999-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccap.h_v   1.13   01 Oct 2002 16:00:16   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mccap.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Re-featurized FEATURE_IS2000_DYNAMIC_FEATURE.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/07/06   fh      AKA/AES: Cleaned up AES capability helper functions.
06/23/06   pg      Added support for RPC.
03/02      fh      Added AES support
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
09/16/04   fc      Added support for reverse DV.
08/23/04   az      Dynamic feature for Rel D negative SCI
08/09/04   fc      Service negotiation changes for Release C and D.
06/16/04   fc      Added DF support for Sync ID and Reconnect message.
04/08/04   bkm     Channel mask updates and new interface for checking
                   simult pdch/sch support. 
01/05/04   bkm     Renamed mccap_check_chind_and_rc to mccap_check_rc_for_chan.
12/02/03   bkm     Added capability check on DV EXT_CH_IND.
10/23/03   va      Added fns for RELC capability reporting
08/13/03   yll     Fixed compile errors.
08/08/03   fc      Added support for IS2000 Release C.
11/05/02   bkm     Added support for JCDMA Max Rate feature.
10/01/02   az      Mainlined FEATURE_IS2000
08/06/02   jrp     Changed MCCAP_REACH_RATE_SUPPORTED from a macro to a
                   function.
07/11/02   ph      Changed mccap_get_preferred_sch_coding() to return the
                   coding type which supports the max rate.
05/31/02   va      Added support for mccap_is_simult_fch_dcch_supported
04/03/02   jrp     Added MCCAP_REACH_RATE_SUPPORTED macro.
03/05/02   fc      Corrected function prototype.
02/22/02   fc      Added support for Release A common channel.
05/04/01   va      Added mcap_is_cs_supported() function prototype. 
02/28/01   ph      merged in from MSM_CP.05.00.55
                   11/29/00   lcc/hrk Added prototype for mccap_get_preferred_sch_coding().
02/27/01   kk      Added prototype for 3x capability.
02/27/01   va      Added prototype for mccap_is_flex_rates_supported
                   and mccap_is_sch_var_rates_supported. 
01/10/01   va      Added prototype for mccap_is_td_supported function, td_mode 
                   is an input argument.
01/10/01   ph      Added prototype for mccap_get_encryption_cap_info().
07/10/00   va      Added prototype for mccap_get_geo_loc_cap().
06/12/00   va      Removed FEATURE_IS2000_SCH to check the num of
                   supported fwd and rev SCHs.
06/02/00   lcc     Added prototype for mccap_is_r_fch_gating_supported.
04/07/00   va      Added support to retrieve min pwr control step size.
03/06/00   lcc     Corrected some comments and added some SCH related function
                   prototypes.
12/20/99   va      Added mccap_get_ext_mux_option_info for supporting
                   Extended mux option IR.
11/15/99   va      Initial revision of the file.
                   Created for the IS2000 Changes. This file keeps the capabaility
                   database and initializes it correctly based on s/w, h/w
                   capability.
===========================================================================*/
#include "cai.h"
#include "target.h"
#include "customer.h"
#include "caii.h"
#include "mcc.h"
#include "mclog.h"
#include "txcmc.h"

typedef enum
{
   MCCAP_FCH      = 0,
   MCCAP_DCCH     = 1,
   MCCAP_FWD_SCH  = 2,
   MCCAP_REV_SCH  = 3,
#ifdef FEATURE_IS2000_REL_C
   MCCAP_FOR_PDCH = 4,
#endif /* FEATURE_IS2000_REL_C */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
   MCCAP_CH_TYPE_END=255
} mccap_ch_type;

/*===========================================================================

FUNCTION mccap_check_rc_for_chan

DESCRIPTION
  Given the channel masks and the fwd and rev RCs, this function checks if 
  these RCs are doable and compatible with each other for the given channels.

  The RC parameters must be valid for all channels in the mask.  Separate
  calls to validate PDCH and RC and FCH/DCCH and RC are required.

DEPENDENCIES
  None.

RETURN VALUE
  status: true if this is good and false if it is not doable.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccap_check_rc_for_chan
(
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  cai_radio_config_type fwd_rc,
  cai_radio_config_type rev_rc
);

/*===========================================================================

FUNCTION mccap_is_enhanced_operation_supported

DESCRIPTION
   This function returns true if QPCH or any RC class 2 RCs are supported, if
   niether of these 2 is supported, false is returned

DEPENDENCIES
  None.

RETURN VALUE
  status: true or false as described above.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_enhanced_operation_supported( void );

/*===========================================================================

FUNCTION mccap_is_mac_supported

DESCRIPTION
  This function returns true if MAC is supported else FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  status: true or false as described above.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_mac_supported( void );

#ifdef FEATURE_IS2000_SCH
/*===========================================================================

FUNCTION mccap_num_f_sch_supported

DESCRIPTION
  This function returns number of F-SCH supported.

DEPENDENCIES
  None.

RETURN VALUE
  See description above.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_num_f_sch_supported( void );

/*===========================================================================

FUNCTION mccap_num_r_sch_supported

DESCRIPTION
  This function returns number of R-SCH supported.

DEPENDENCIES
  None.

RETURN VALUE
  See description above.

SIDE EFFECTS
  None.

===========================================================================*/
byte mccap_num_r_sch_supported( void );
#endif /* FEATURE_IS2000_SCH */

/*===========================================================================

FUNCTION mccap_get_ch_specific_fields

DESCRIPTION
  This function returns the channel specific fields for a given channel

DEPENDENCIES
  None

RETURN VALUE
  status: true if the specified channel is supported else false

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_get_ch_specific_fields
(
  mccap_ch_type  chan_type,
  void * data_ptr
);

/*===========================================================================

FUNCTION mccap_get_chan_cfg_info

DESCRIPTION
   This function populates the passed in channel config IR

DEPENDENCIES
  initialize function should be called before this.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
extern void  mccap_get_chan_cfg_info( caii_chan_cfg_capa_info_type *ccc_ptr);

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_get_thx_cch_supported

DESCRIPTION
   This function populates the passed 3x supported boolean

DEPENDENCIES
  initialize function should be called before this.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean  mccap_get_thx_cch_supported ( void );
#endif


#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION mccap_reach_rate_supported

DESCRIPTION
   Determines whether the MS supports the REACH rate_word.

DEPENDENCIES
  None.

RETURN VALUE
   True if supported.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_reach_rate_supported(txc_each_rate_type rate);
#endif // FEATURE_IS2000_REL_A_CC




/*===========================================================================

FUNCTION mccap_get_advanced_capability_fields

DESCRIPTION
   This function populates IS2000 specific fields for
   the capability info record passed in.

DEPENDENCIES
  initialize function should be called before this.

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
extern void  mccap_get_advanced_capability_fields( caii_capa_rec_type *cap_ptr);

/*===========================================================================

FUNCTION mccap_get_ext_mux_option_info

DESCRIPTION
   This function populates the ext mux option record pointer
   from the system capability info

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccap_get_ext_mux_option_info ( caii_ext_mult_opt_info_type  *rec_ptr);

/*===========================================================================

FUNCTION mccap_get_min_pwr_ctrl_step

DESCRIPTION
   This function returns the minimum pwr ctrl step size that can
   be supported by the underlying MSM

DEPENDENCIES
   None

RETURN VALUE
   Min pwr control step size

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mccap_get_min_pwr_ctrl_step ( void );

/*===========================================================================

FUNCTION mccap_get_geo_loc_cap

DESCRIPTION
   This function returns the GEO location Capability of the MS

DEPENDENCIES
   None

RETURN VALUE
  geo location capability value

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mccap_get_geo_loc_cap ( void );


/*===========================================================================

FUNCTION mccap_get_preferred_sch_coding

DESCRIPTION
  This function returns turbo coding if turbo code is supported and the max
  turbo rate is better or equal to conv; otherwise, convoluational coding is returned.
  It is assumed that conv coding is always supported.  For simplicity, we are
  making the assumption that coding support is uniform across all SCH physical
  channels (0, 1, F, R).

DEPENDENCIES
  The caller has to make sure that SCH is supported.  If this function is called
  when SCH is not supported, the result is unpredictable..

RETURN VALUE
  Coding type that supports the maximum rate.

SIDE EFFECTS
  None.

===========================================================================*/
cai_sch_coding_type mccap_get_preferred_sch_coding( void );


/*===========================================================================

FUNCTION mccap_is_td_mode_supported

DESCRIPTION
   This function determines if the specified td_mode is supported
   by the MS. 

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccap_is_td_mode_supported( byte td_mode);   

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_get_encryption_cap_info

DESCRIPTION
   This function returns the Encryption Capability of the MS in the pointer passed.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccap_get_encryption_cap_info ( caii_enc_cap_type *cec_ptr );

/*===========================================================================

FUNCTION mccap_is_flex_rates_supported

DESCRIPTION
   This function determines if flex rates is supported by the MS. 
   Once we start supporting flex rates on only certain channels, this
   function will have to take channel as a input.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if flex rates is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_flex_rates_supported( void );

/*===========================================================================

FUNCTION mccap_is_sch_var_rates_supported

DESCRIPTION
   This function determines if var rates is supported on the SCH.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if var rates is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_sch_var_rates_supported( void );

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION mccap_is_csch_td_mode_supported

DESCRIPTION
   This function determines if the specified td_mode is supported
   on the common control channel by the MS. 

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccap_is_csch_td_mode_supported(byte td_mode);   

/*===========================================================================

FUNCTION mccap_is_csch_f_ccch_frame_size_supported

DESCRIPTION
   This function determines if the specified FCCCH frame size is supported 
   by the MS. 

DEPENDENCIES
   None

RETURN VALUE
  TRUE if the specified TD mode is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccap_is_csch_f_ccch_frame_size_supported
(
  cai_fccch_frame_size_type frame_size
);
#endif /* FEATURE_IS2000_REL_A_CC */
#endif // FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION mccap_is_simult_fch_dcch_supported

DESCRIPTION
   This function determines if FCH & DCCH are supported concurrently.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if concurrent FCH & DCCH is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_simult_fch_dcch_supported( void );

/*===========================================================================

FUNCTION mccap_is_f_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Fwd SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_f_sch_32x_supported(void);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccap_is_f_sch_32x_supported */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*===========================================================================

FUNCTION mccap_is_r_sch_32x_supported

DESCRIPTION
   This function determines whether the mobile supports 32X Rev SCH data rate

DEPENDENCIES
  None.

RETURN VALUE
   True if it supports.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_r_sch_32x_supported(void);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccap_is_r_sch_32x_supported */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccap_f_sch_max_rate

DESCRIPTION
   This function returns the forward SCH max rate.

DEPENDENCIES
  None.

RETURN VALUE
   This function returns the forward SCH max rate.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 mccap_f_sch_max_rate(void);

/*===========================================================================

FUNCTION mccap_r_sch_max_rate

DESCRIPTION
   This function returns the reverse SCH max rate.

DEPENDENCIES
  None.

RETURN VALUE
   This function returns the reverse SCH max rate.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 mccap_r_sch_max_rate(void);
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION mccap_set_sch_sys_cap

DESCRIPTION
  This function was called by DS module for JCDMA Max Rate feature, and the
  Dynamic Feature module where the fwd and rev max rate are configurable.
  
  The JCDMA Max Rate feature requires the system capabilities reported by
  the MS to be adjusted to reflect limitations imposed by system level
  requirements rather than the actual capability of the MS software and
  hardware.

  For JCDMA Max Rate, the DS module calls this function to indicate if 
  F-SCH/R-SCH are supported and if so, the maximum rate (1x,2x,4x,8x,16x)
  on each channel and these limits are captured in the system_capability 
  structure. INTLOCK & INTFREE ensure that this update completes (in DS 
  task context) before MC accesses the data structure.

  For JCDMA Max Rate, this function assumes that IS-2000 Release A is 
  not supported and that RS2 SCH is supported, but RS2 16x is not supported.
  
  For Dynamic Feature, this function is called by DF module during powerup,
  when it reads the fwd and rev SCH max rate from NV. The maximum rate can
  be up to 16x or 32x, and RS2 16x can be supported.

DEPENDENCIES
  For JCDMA Max Rate, DS Module must update configuration in SNM to service
  negotiation and capability reporting of service options.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccap_set_sch_sys_cap
(
  boolean fwd_sch_supported,
  boolean rev_sch_supported,
  cai_sch_rate_type max_fwd_sch_rate, 
  cai_sch_rate_type max_rev_sch_rate
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mccap_set_sch_sys_cap */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
/*===========================================================================

FUNCTION MCCAP_DF_SET_SYS_CAP

  This function updates the system capability with the dynamic feature item.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  system_capability variable will be updated.
  
===========================================================================*/
void mccap_df_set_sys_cap
(
  void
);
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */

/*===========================================================================

FUNCTION mccap_item_is_supported

DESCRIPTION
   Returns TRUE if the dynamic feature specified is supported
  
DEPENDENCIES
  None.

RETURN VALUE
   Returns TRUE if the dynamic feature specified is supported.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccap_item_is_supported(mccap_df_type df_type);

#ifdef FEATURE_IS2000_REL_C

/*===========================================================================

FUNCTION mccap_is_msg_integrity_supported

DESCRIPTION
   This function determines if message integrity is supported by the MS.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if message integrity  is supported, else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean mccap_is_msg_integrity_supported( void );

/*===========================================================================

FUNCTION mccap_get_sig_integrity_sup_info

DESCRIPTION
   This function returns the Signaling Message Integrity support information
   of the MS in the pointer passed.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccap_get_sig_integrity_sup_info 
(
  caii_sig_msg_int_info_type *cec_ptr
);

/*===========================================================================

FUNCTION mccap_is_for_pdch_supported

DESCRIPTION
   Returns TRUE if FOR_PDCH is supported else FALSE

DEPENDENCIES
  None.

RETURN VALUE
   Returns TRUE if FOR_PDCH is supported else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 mccap_is_for_pdch_supported(void) ;

/*===========================================================================

FUNCTION mccap_is_ext_ch_ind_supported

DESCRIPTION
   Determine if the input OTA EXT_CH_IND value is supported by the mobile.

DEPENDENCIES
  Assumes that only 6 bits used for the ch_config_sup_mask.

RETURN VALUE
   Returns TRUE if the input EXT_CH_IND value is supported, else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mccap_is_ext_ch_ind_supported(byte ota_ext_ch_ind);

/*===========================================================================

FUNCTION mccap_is_simult_for_pdch_sch_supported

DESCRIPTION
  Determine if simultaneous F-PDCH and F-SCH are supported.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if simultaneous F-PDCH and F-SCH are supported else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern uint8 mccap_is_simult_for_pdch_sch_supported(void);
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */

#ifdef FEATURE_MEID_SUPPORT
/*===========================================================================

FUNCTION mccap_is_meid_supported

DESCRIPTION
   This function returns true if MEID is supported else false.

DEPENDENCIES
  None.

RETURN VALUE
  This function returns true if MEID is supported else false

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccap_is_meid_supported( void );
#endif /* FEATURE_MEID_SUPPORT */


#ifdef FEATURE_IS2000_REL_A_AES
#error code not present
#endif /* FEATURE_IS2000_REL_A_AES */

#endif // MCCAP_H

