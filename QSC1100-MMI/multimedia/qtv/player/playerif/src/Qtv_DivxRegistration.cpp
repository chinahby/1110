/* =======================================================================
                               Qtv_DivxRegistration.cpp
DESCRIPTION
 Retrieves the DRM registration code for DivX DRM.

Copyright 2008 Qualcomm Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History
$Header: //source/qcom/qct/multimedia/qtv/player/playerif/main/latest/src/Qtv_DivxRegistration.cpp#1 $
$DateTime: 2008/05/19 02:58:01 $
$Change: 661882 $
========================================================================== */
/* ==========================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "Qtv_DivxRegistration.h"
/*==========================================================================*/

#ifdef FEATURE_QTV_DIVX_DRM

/* ==========================================================================
                     Include DRM library and AVI Parser Data Types
========================================================================== */
#include "DrmApi.h"
/*==========================================================================*/

/* ======================================================================
FUNCTION
  QtvDivXDrmClient::QtvDivXDrmClient

DESCRIPTION
  Constructor

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvDivXDrmClient::QtvDivXDrmClient()
{
  memset(m_regCode,0,DRM_REGISTRATION_CODE_BYTES);
  m_drmContext = NULL;
  m_nLength = 0;
}
/* ======================================================================
FUNCTION
  QtvDivXDrmClient::~QtvDivXDrmClient

DESCRIPTION
  Destructor

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
QtvDivXDrmClient::~QtvDivXDrmClient()
{
  if(m_drmContext)
  {
    free(m_drmContext);
    m_drmContext = NULL;
    m_nLength = 0;
  }
}
/* ======================================================================
FUNCTION
  QtvDivXDrmClient::GetRegistrationCode

DESCRIPTION
  Retrieves the DivX DRM registration code.

DEPENDENCIES
  None

RETURN VALUE
  True is successful otherwise returns false.

SIDE EFFECTS
  None

========================================================================== */
bool QtvDivXDrmClient::GetRegistrationCode(char* code,int* max_length)
{   
  drmErrorCodes_t result;	
  avi_uint32 drmContextLength;

  if( !max_length )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QtvDivXDrmClient::GetRegistrationCode INVALID PARAMETER..");
    return false;
  }

  if(!code)
  {
    *max_length = DRM_REGISTRATION_CODE_BYTES;
    return true;
  }
  if((*max_length) < DRM_REGISTRATION_CODE_BYTES)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "QtvDivXDrmClient::GetRegistrationCode INVALID PARAMETER..");
    return false;
  }
  if(m_nLength > 0)
  {
    //No need to retrieve the code again.
    memcpy(code,m_regCode,DRM_REGISTRATION_CODE_BYTES);
    return true;
  }
  result = drmInitSystem( NULL,&drmContextLength );

  m_drmContext = (uint8_t*)malloc( drmContextLength  );

  if(!m_drmContext)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmInitSystem memory allocation failed..");
    return false;
  }
  result = drmInitSystem( m_drmContext, &drmContextLength );
  if ( DRM_SUCCESS!= result )
	{
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmInitSystem failed result %ld",result);
    return false;
	}
  for ( int i = 0; i < 3; i++ )
  {
	  drmSetRandomSample( m_drmContext );
	}
  result = drmGetRegistrationCodeString( m_drmContext,m_regCode );
  if ( DRM_SUCCESS!= result )
	{
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "drmGetRegistrationCodeString failed result %ld",result);
    return false;
	}
  m_nLength = drmContextLength;
  *max_length = DRM_REGISTRATION_CODE_BYTES;
  memcpy(code,m_regCode,DRM_REGISTRATION_CODE_BYTES);
  return true;
}    

#endif//#ifdef FEATURE_QTV_DIVX_DRM
