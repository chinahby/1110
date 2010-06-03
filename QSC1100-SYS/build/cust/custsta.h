#ifndef CUSTSTA_H
#define CUSTSTA_H
/*===========================================================================

           C U S T S T A . H

DESCRIPTION

  Copyright (c) 2003   by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================
$PVCSPath: L:/src/asw/COMMON/vcs/custsta.h_v   1.3   05 Nov 2003 18:01:14   pbostley  $
$Header: //depot/asic/msmshared/services/sta/sta/custsta.h#5 $ $DateTime: 2006/07/31 20:37:37 $ $Author: mbonnet $
===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/

#define FEATURE_STA

#ifdef FEATURE_STA

  #ifndef FEATURE_ONCRPC
     #error STA requires ONCRPC. 
  #endif

//  #define FEATURE_STA_DSS

  #ifdef FEATURE_STA_DSS
//    #define FEATURE_STA_DSS_DSIFACE_REMOVED
    #define FEATURE_STA_DSS_MM
  #endif /* FEATURE_DS_SOCKETS */

//  #define FEATURE_STA_DNS

//  #define FEATURE_STA_DSSIOCTL

//  #define FEATURE_STA_SSL

  #if defined (FEATURE_STA_SSL) || defined (FEATURE_STA_DSS)
//      #define FEATURE_STA_SCHED
  #endif

//  #define FEATURE_STA_CM

//  #define FEATURE_STA_SECAPI

//  #define FEATURE_STA_WMS

//  #define FEATURE_STA_RPCSVC

//  #define FEATURE_STA_RPCCLNT

//  #define FEATURE_STA_PMCLNT

//  #define FEATURE_STA_RAT
  #ifdef FEATURE_STA_RAT
#error code not present
  #endif /* FEATURE_STA_RAT */

//  #define FEATURE_STA_PBM

#endif /* FEATURE_STA */

#endif /* CUSTSTA_H */

