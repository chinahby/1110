#ifndef FTM_COMMON_DISPATCH_H
#define FTM_COMMON_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M  C O M M O N  D I S P A T C H   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that are common
  between 1x and UMTS.


Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_common_dispatch.h#1 $ $DateTime: 2008/04/14 04:57:43 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/07   anr	   Removed support for XO FT samples in the XO response packet. Will add
				   it as a log instead since it's for debug purposes.
08-19-05   ka      Created
===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"
#include "ftm.h"

#if defined (FTM_HAS_XO_FACTORY_CAL) 
#include "tcxomgr_cal.h"
#endif

typedef struct 
{
  uint16 error_code;
} ftm_common_result_type;

#if defined (FTM_HAS_XO_FACTORY_CAL) 
//XO Cal Request Packet Structure
typedef struct {
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  int32 offset;
  int32 temp_span;								/*  Temperature span to be achieved */
  uint16 min_ft_samples;						/*  Min FT samples */
  uint16 max_ft_samples;						/*  Max FT Samples */
  uint16 substage_time;							
  int2 pdm_value; 								/*  Tx AGC value to use when turning PA on */
  uint8 pa_range;								/*  PA Range to heat the PA */
  uint16 timeout;								/*  Timeout for the FT collection */
  tcxomgr_cal_record_enum_type write_nv;		/*  Decides if TCXO manager writes XO cal Nv */
} ftm_xo_cal_request_type;

//XO Cal Response Packet Structure
typedef struct {
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  tcxomgr_cal_state_enum_type state;			  /* State of the coefficients. */	
  tcxomgr_cal_fac_data_type   nom_coef;           /* nominal coefficients     */
  tcxomgr_cal_fac_data_type   cal_coef;  		  /* Calibrated coefficients  */	
  tcxomgr_cal_info_type 	 tcxomgr_cal_info_pkt; /* Calibration information */
} ftm_xo_cal_response_type;
#endif
/*===========================================================================

FUNCTION FTM_COMMON_DISPATCH

DESCRIPTION
   This function handles requests to run common tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_common_dispatch(ftm_common_pkt_type *cmd_ptr);

#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_COMMON_DISPATCH_H
