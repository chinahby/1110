#ifndef CUSTSEC_H
#define CUSTSEC_H
/*===========================================================================

            " C u s t - S E C "   H E A D E R   F I L E

DESCRIPTION
  Configuration for Secure Services Livrary.

  Copyright (c) 2001-2006  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/securemsm/baselines/qsc1100/main/latest/build/cust/custsec.h#8 $ $DateTime: 2009/04/02 13:23:11 $ $Author: amundi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/09   avm     moved FEATURE_SEC_SFS to top level
05/11/06   df      6800 version without SVD DSP image
04/23/06   df      6800 version w/o DRM
04/04/06   df      version for 6800 sec/sfs/rnd
04/03/06   df      version for 6250 with SEC_BOOT
03/30/06   df      version for 6550
03/26/06   df      added FEATURE_SECRND
03/01/06   df      added FEATURE_L4
01/19/06   df      added DRMV2 support
02/25/05   sb      Added IPC support
03/05/04   jay     Added DRM feature
06/23/03   om      Added comments for automated FEATURE doc generation.
06/19/03   om      Added FEATURE_SEC_QDSP4_COPROC_MM_OPT.
06/12/03   om      Updates for Cougar DME
04/04/03   om      Created file.

===========================================================================*/


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                        Security Services Modules
---------------------------------------------------------------------------*/

/* General cryptographic library */
#define FEATURE_SEC

/* secrnd task */
#define FEATURE_SECRND

#ifdef FEATURE_L4
#error code not present
#endif

/* should be defined for rex or L4 single PD */
#define SEC_L4_SINGLE_PD

/* IPSec protocol for IPv4 and VPN remote access client */
#undef FEATURE_SEC_IPSEC

/* SSLv3 and TLS 1.0 protocol support for secure socket connections */
#undef FEATURE_SEC_SSL 

/* DRM */
#undef FEATURE_SEC_DRM

/* QC-internal test applications for Security Services */
//#define FEATURE_SEC_TESTAPPS

/*---------------------------------------------------------------------------
                    Cryptographic library configuration
---------------------------------------------------------------------------*/

#ifdef FEATURE_SEC
  
  /* Enable X.509 certificate handling */
  #define FEATURE_SEC_X509

  /* Enable storage of root certs in EFS */
  #define FEATURE_SEC_EFS_CA

  /* ARM implementations of the algorithms */
  #define FEATURE_SEC_ARM_CRYPTO

  /* DSP implementations of the algorithms */
  #define FEATURE_SEC_DSP_CRYPTO

  /* Enable 3DES */
  #define FEATURE_EXPT_01

  /* ---------------------------------------------------- */
  /* ARM platform-specific comfiguration                  */
  /* ---------------------------------------------------- */
  #ifdef FEATURE_SEC_ARM_CRYPTO
  #endif

  /* ---------------------------------------------------- */
  /* DSP platform-specific comfiguration                  */
  /* ---------------------------------------------------- */
  #ifdef FEATURE_SEC_DSP_CRYPTO
//#define FEATURE_SEC_SVD_DSP_IMAGE  undefine for 6800 65 nm
  /* Enable write back cache flushing */
  #define FEATURE_SEC_WB_CACHE
  #endif
  
  /* Crypto test app */
  #ifdef FEATURE_SEC_TESTAPPS
#error code not present
  #else
  #undef FEATURE_SEC_CRYPT_TESTAPP
  #endif

  /* SEC test diag event reporting support */
  #define FEATURE_SECTEST_DIAG_EVENTS

  /* SEC crypto layer diag event reporting support */
  #undef FEATURE_SECCRYPT_DIAG_EVENTS

#endif  /*FEATURE_SEC */

#ifdef FEATURE_SEC_SFS
#include "custsfs.h"
#endif

#ifdef FEATURE_SEC_IPSEC
#include "custipsec.h"
#endif

#ifdef FEATURE_SEC_SSL
#include "custssl.h"
#endif

#endif /* CUSTSEC_H */

