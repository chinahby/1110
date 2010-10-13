#ifndef FTM_1X_CAL_V2_H
#define FTM_1X_CAL_V2_H
/*===========================================================================

                 F T M   1 X   C A L I B R A T I O N   A P I
                              V E R S I O N   2

DESCRIPTION
  This module consolidates RF calibration and NV management, with
  extensions for IntelliCeiver enabled radios.


Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_cal_v2.h#1 $
$DateTime: 2008/04/11 05:11:57 $
$Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/04   bmg     Created

===========================================================================*/

#include "comdef.h"
#include "ftmdiag.h"
#include "rficap.h"

/*===========================================================================

                              DEFINITIONS

===========================================================================*/

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   band;
} ftm_1x_cal_nv_commit_req_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   path;
  int2    power;
} ftm_1x_cal_dvga_req_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   path;
  uint1   lna_step;
  int2    power;
} ftm_1x_cal_lna_req_type;

typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   path;
} ftm_1x_cal_im2_req_type;

#ifdef RF_HAS_POWER_MODE_LIB
typedef PACKED struct
{
  uint1   cmd;
  uint1   sub_sys;
  uint2   mode;
  uint2   ftm_cmd;
  uint2   req_len;
  uint2   rsp_len;
  uint1   path;
  int2    power;
} ftm_1x_cal_intelliceiver_req_type;
#endif /* RF_HAS_POWER_MODE_LIB */

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION FTM_1X_CAL_DISPATCH

DESCRIPTION
  Handles all 1x calibration APIs

DEPENDENCIES
   None.

RETURN VALUE
   FTM response structure, with the response returned by the individual
   API handler.

SIDE EFFECTS
   API handlers will update RF NV internal data structures with the
   latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_dispatch( void* req );


/*===========================================================================

FUNCTION FTM_1X_CAL_NV_COMMIT

DESCRIPTION
  Write RX calibration to NV

DEPENDENCIES
   None.

RETURN VALUE
   FTM response structure, with FTM_RSP_DO_LEGACY set.

SIDE EFFECTS
   All supported RX calibration NV items are written to NV.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_nv_commit( ftm_1x_cal_nv_commit_req_type* req );


/*===========================================================================

FUNCTION FTM_1X_CAL_DVGA

DESCRIPTION
  Performes DVGA calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   A CDMA waveform with Ior equal to the passsed parameter <power>
   must be applied to the radio, on the correct band and channel.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_dvga( ftm_1x_cal_dvga_req_type* req );


/*===========================================================================

FUNCTION FTM_1X_CAL_LNA

DESCRIPTION
  Performes LNA offset calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   A CDMA waveform with Ior equal to the passsed parameter <power>
   must be applied to the radio, on the correct band and channel.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_lna( ftm_1x_cal_lna_req_type* req );


/*===========================================================================

FUNCTION FTM_1X_CAL_IM2

DESCRIPTION
  Performes IM2 calibration

DEPENDENCIES
   The radio must be tuned to the correct band and channel.
   An IM2 calibration signal must be applied to the radio at
   the correct level and offset.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_im2( ftm_1x_cal_im2_req_type* req );


#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

FUNCTION FTM_1X_CAL_INTELLICEIVER

DESCRIPTION
  Performes IntelliCeiver calibration

DEPENDENCIES
  The radio must be tuned to a band and channel, and the proper chipset
  dependent IntelliCiever calibration signal applied to the antenna
  input.

RETURN VALUE
   FTM response structure, with a newly allocated response buffer.

SIDE EFFECTS
   Updates RF NV internal data structures with the latest calibration.

===========================================================================*/
ftm_rsp_pkt_type ftm_1x_cal_intelliceiver( ftm_1x_cal_intelliceiver_req_type* req );
#endif /* RF_HAS_POWER_MODE_LIB */


#endif /* FTM_1X_CAL_V2_H */

