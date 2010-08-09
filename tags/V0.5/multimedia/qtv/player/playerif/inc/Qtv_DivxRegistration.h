#ifndef _QTVPLAYER_DIVX_DRM_REG_H_
#define _QTVPLAYER_DIVX_DRM_REG_H_

/* =======================================================================
                               Qtv_DivxRegistration.h
DESCRIPTION
 Retrieves the DRM registration code for DivX DRM.

Copyright 2008 Qualcomm Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/player/playerif/main/latest/inc/Qtv_DivxRegistration.h#1 $
$DateTime: 2008/05/19 02:58:01 $
$Change: 661882 $
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"

#ifdef FEATURE_QTV_DIVX_DRM

#include "qtvInternalDefs.h"
#include "QtvPlayer.h"
#include "comdef.h"
#include "qtvsystem.h"
#include "qtv_msg.h"
#include "DrmTypes.h"
#include "aviDataTypes.h"

class QtvDivXDrmClient
{
  public:
    QtvDivXDrmClient();
    ~QtvDivXDrmClient();
    bool GetRegistrationCode(char*,int*);    

  private:
    char m_regCode[DRM_REGISTRATION_CODE_BYTES];
    avi_uint8* m_drmContext;
    int m_nLength;
};

#endif//#ifdef FEATURE_QTV_DIVX_DRM

#endif//#define _QTVPLAYER_DIVX_DRM_REG_H_
