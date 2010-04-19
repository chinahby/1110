#ifndef MSGCFG_H
#define MSGCFG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
        DIAGNOSTIC MESSAGE SERVICE 2.0 CONFIGURATION FILE

GENERAL DESCRIPTION

  MSG 2.0 offers the ability to utilize more build-time and run-time 
  filtering of debug messages.  
  
  This file contains definitions to identify subsystem IDs (SSIDs) and
  the ability to map legacy debug messages (MSG_LOW, MSG_MED, etc) to
  different SSIDs.  SSIDs are externally published and must not change
  once published.

  This file also defines and maintains a table used by MSG 2.0 to store
  build-time and run-time masks.
  
  This file includes msgtgt.h, which is a target-specific file used to 
  customize MSG 2.0's configuration.  It may specify SSID build masks, 
  the default build mask (for undefined build masks), and other 
  configurable features.  Defaults are listed in this file.
                      
Copyright (c) 2002-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/msgcfg.h#26 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/08   sj      Included size for table and Fixed orphan file issue by FTM 
12/15/06   as      Fixed compiler warnings.
12/14/04   as      Reallocated SSID's used by WinCE to L4LINUX.
03/07/03   lad     Initial SSID deployment.
12/03/02   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "msg.h"    
#include "msgtgt.h" /* Target-specific MSG config info, such as build masks */

/* This constant specifies the default max file and format string length.  
   It should be arbitrarily large as it is only used to protect against 
   invalid string pointers. */
#ifndef MSG_MAX_STRLEN
  #define MSG_MAX_STRLEN 512
#endif

/* Default build mask for all SSIDs may be specified in msgtgt.h. */
#ifndef MSG_BUILD_MASK_DFLT
  /*---------------------------------------------------------------------------
    MSG_LEVEL is the minimum message priority which can be logged. If MSG_LEVEL
    is not defined, default is NONE.  If MSG_LEVEL is set to MSG_LVL_NONE, then
    there will be no calls to msg_put(), msg_send().
  ---------------------------------------------------------------------------*/
  #ifdef MSG_LEVEL
    #define MSG_BUILD_MASK_DFLT MSG_LEVEL
  #else
    #define MSG_BUILD_MASK_DFLT 0
  #endif
#endif
    
#ifndef MSG_LEVEL
  #define MSG_LEVEL MSG_LVL_NONE
#endif

#ifndef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_DFLT
#endif
  
/*---------------------------------------------------------------------------
  This section contains configuration entries for all SSIDs.  
  
  Build-time filtering is accomplsihed using a name hash in a macro.  As a 
  result, naming convention must be adhered to.
  
  SSIDs must be common and unique across all targets.  The SSIDs and the 
  meaning of the mask bits for the SSID are to be published in an externalized
  interface.  They cannot be change once published.
  
  Not all targets support all SSIDs.
  
  MSG_SSID_DFLT is used to expand legacy macros (MSG_LOW, MSG_MED, e.t.c.).
  Legacy macros may be mapped to another SSID (listed here) via the build 
  environment.  For each invocation of the compiler, MSG_BT_SSID_LEGACY may
  be specified to override the default SSID for legacy macros (0).

---------------------------------------------------------------------------*/

#define MSG_SSID_GEN_FIRST  0
  /* Legacy messages may be mapped to a different SSID 
     by the build environment. */
  #define MSG_SSID_DFLT     0
  #define MSG_SSID_AUDFMT   1
  #define MSG_SSID_AVS      2
  #define MSG_SSID_BOOT     3
  #define MSG_SSID_BT       4
  #define MSG_SSID_CM       5
  #define MSG_SSID_CMX      6 
  #define MSG_SSID_DIAG     7
  #define MSG_SSID_DSM      8
  #define MSG_SSID_FS       9
  #define MSG_SSID_HS      10
  #define MSG_SSID_MDSP    11
  #define MSG_SSID_QDSP    12
  #define MSG_SSID_REX     13
  #define MSG_SSID_RF      14
  #define MSG_SSID_SD      15
  #define MSG_SSID_SIO     16
  #define MSG_SSID_VS      17
  #define MSG_SSID_WMS     18
  #define MSG_SSID_GPS     19
  #define MSG_SSID_MMOC    20
  #define MSG_SSID_RUIM    21
  #define MSG_SSID_TMC     22
  #define MSG_SSID_FTM     23
  #define MSG_SSID_MMGPS   24
  #define MSG_SSID_SLEEP   25
  #define MSG_SSID_SAM     26
  #define MSG_SSID_SRM     27
  #define MSG_SSID_SFAT    28
  #define MSG_SSID_JOYST   29
  #define MSG_SSID_MFLO    30
  #define MSG_SSID_DTV     31
  #define MSG_SSID_TCXOMGR 32
  #define MSG_SSID_EFS     33
  #define MSG_SSID_IRDA    34
  #define MSG_SSID_FM_RADIO 35 
  #define MSG_SSID_AAM     36
  #define MSG_SSID_BM      37
  #define MSG_SSID_PE      38
  #define MSG_SSID_QIPCALL 39
  #define MSG_SSID_FLUTE   40
  #define MSG_SSID_CAMERA  41
  #define MSG_SSID_HSUSB   42
  #define MSG_SSID_FC      43
  #define MSG_SSID_USBHOST 44
  #define MSG_SSID_PROFILER 45
  #define MSG_SSID_MGP     46
  #define MSG_SSID_MGPME   47
  #define MSG_SSID_GPSOS   48
  #define MSG_SSID_MGPPE   49
  #define MSG_SSID_GPSSM   50
  #define MSG_SSID_IMS     51
  #define MSG_SSID_MBP_RF  52
  #define MSG_SSID_SNS     53
  #define MSG_SSID_WM      54
#define MSG_SSID_GEN_LAST  54

/* Messages arising from ONCRPC AMSS modules */
#define MSG_SSID_ONCRPC             500
#define MSG_SSID_ONCRPC_MISC_MODEM  501
#define MSG_SSID_ONCRPC_MISC_APPS   502
#define MSG_SSID_ONCRPC_CM_MODEM    503
#define MSG_SSID_ONCRPC_CM_APPS     504
#define MSG_SSID_ONCRPC_DB          505
#define MSG_SSID_ONCRPC_SND         506
#define MSG_SSID_ONCRPC_LAST        506

#ifdef FEATURE_IS2000

/* Default master category for 1X. */
#define MSG_SSID_1X             1000
  #define MSG_SSID_1X_ACP       1001 
  #define MSG_SSID_1X_DCP       1002
  #define MSG_SSID_1X_DEC       1003
  #define MSG_SSID_1X_ENC       1004
  #define MSG_SSID_1X_GPSSRCH   1005
  #define MSG_SSID_1X_MUX       1006
  #define MSG_SSID_1X_SRCH      1007
#define MSG_SSID_1X_LAST        1007

#endif /* FEATURE_IS2000 */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */


#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)

/* Default master category for data services. */
#define MSG_SSID_DS             5000
  #define MSG_SSID_DS_RLP     5001   
  #define MSG_SSID_DS_PPP     5002  
  #define MSG_SSID_DS_TCPIP   5003  
  #define MSG_SSID_DS_IS707   5004   
  #define MSG_SSID_DS_3GMGR   5005 
  #define MSG_SSID_DS_PS      5006    
  #define MSG_SSID_DS_MIP     5007   
  #define MSG_SSID_DS_UMTS    5008  
  #define MSG_SSID_DS_GPRS    5009  
  #define MSG_SSID_DS_GSM     5010  
  #define MSG_SSID_DS_SOCKETS 5011 
  #define MSG_SSID_DS_ATCOP   5012  
  #define MSG_SSID_DS_SIO     5013
  #define MSG_SSID_DS_BCMCS   5014
  #define MSG_SSID_DS_MLRLP   5015
  #define MSG_SSID_DS_RTP     5016
  #define MSG_SSID_DS_SIPSTACK 5017
  #define MSG_SSID_DS_ROHC     5018
  #define MSG_SSID_DS_DOQOS    5019
  #define MSG_SSID_DS_IPC      5020
  #define MSG_SSID_DS_SHIM     5021
#define MSG_SSID_DS_LAST       5021
  
#endif /* defined (FEATURE_DS) || defined (FEATURE_DATA) */

/* Default master category for Security. */
#define MSG_SSID_SEC                5500
#define MSG_SSID_SEC_CRYPTO         5501  /* Cryptography */
#define MSG_SSID_SEC_SSL            5502  /* Secure Sockets Layer */
#define MSG_SSID_SEC_IPSEC          5503  /* Internet Protocol Security */
#define MSG_SSID_SEC_SFS            5504  /* Secure File System */
#define MSG_SSID_SEC_TEST           5505  /* Security Test Subsystem */
#define MSG_SSID_SEC_CNTAGENT       5506  /* Content Agent Interface */
#define MSG_SSID_SEC_RIGHTSMGR      5507  /* Rights Manager Interface */
#define MSG_SSID_SEC_ROAP           5508  /* Rights Object Aquisition Protocol */
#define MSG_SSID_SEC_MEDIAMGR       5509  /* Media Manager Interface */ 
#define MSG_SSID_SEC_IDSTORE        5510  /* ID Store Interface */
#define MSG_SSID_SEC_IXFILE         5511  /* File interface */
#define MSG_SSID_SEC_IXSQL          5512  /* SQL interface */
#define MSG_SSID_SEC_IXCOMMON       5513  /* Common Interface */
#define MSG_SSID_SEC_BCASTCNTAGENT  5514  /* Broadcast Content Agent Interface */
#define MSG_SSID_SEC_LAST           5514


/* Default master category for applications. */
#define MSG_SSID_APPS                    6000
  #define MSG_SSID_APPS_APPMGR           6001
  #define MSG_SSID_APPS_UI               6002
  #define MSG_SSID_APPS_QTV              6003
  #define MSG_SSID_APPS_QVP              6004
  #define MSG_SSID_APPS_QVP_STATISTICS   6005
  #define MSG_SSID_APPS_QVP_VENCODER     6006
  #define MSG_SSID_APPS_QVP_MODEM        6007
  #define MSG_SSID_APPS_QVP_UI           6008
  #define MSG_SSID_APPS_QVP_STACK        6009
  #define MSG_SSID_APPS_QVP_VDECODER     6010
  #define MSG_SSID_APPS_ACM              6011
  #define MSG_SSID_APPS_HEAP_PROFILE     6012
  #define MSG_SSID_APPS_QTV_GENERAL      6013
  #define MSG_SSID_APPS_QTV_DEBUG        6014
  #define MSG_SSID_APPS_QTV_STATISTICS   6015
  #define MSG_SSID_APPS_QTV_UI_TASK      6016
  #define MSG_SSID_APPS_QTV_MP4_PLAYER   6017
  #define MSG_SSID_APPS_QTV_AUDIO_TASK   6018
  #define MSG_SSID_APPS_QTV_VIDEO_TASK   6019
  #define MSG_SSID_APPS_QTV_STREAMING    6020
  #define MSG_SSID_APPS_QTV_MPEG4_TASK   6021
  #define MSG_SSID_APPS_QTV_FILE_OPS     6022
  #define MSG_SSID_APPS_QTV_RTP          6023
  #define MSG_SSID_APPS_QTV_RTCP         6024
  #define MSG_SSID_APPS_QTV_RTSP         6025
  #define MSG_SSID_APPS_QTV_SDP_PARSE    6026
  #define MSG_SSID_APPS_QTV_ATOM_PARSE   6027
  #define MSG_SSID_APPS_QTV_TEXT_TASK    6028
  #define MSG_SSID_APPS_QTV_DEC_DSP_IF   6029
  #define MSG_SSID_APPS_QTV_STREAM_RECORDING 6030
  #define MSG_SSID_APPS_QTV_CONFIGURATION    6031
  #define MSG_SSID_APPS_QCAMERA              6032
  #define MSG_SSID_APPS_QCAMCORDER           6033
  #define MSG_SSID_APPS_BREW                 6034
  #define MSG_SSID_APPS_QDJ                  6035
  #define MSG_SSID_APPS_QDTX                 6036
  #define MSG_SSID_APPS_QTV_BCAST_FLO        6037
  #define MSG_SSID_APPS_MDP_GENERAL          6038
  #define MSG_SSID_APPS_PBM                  6039  
  #define MSG_SSID_APPS_GRAPHICS_GENERAL     6040
  #define MSG_SSID_APPS_GRAPHICS_EGL         6041
  #define MSG_SSID_APPS_GRAPHICS_OPENGL      6042
  #define MSG_SSID_APPS_GRAPHICS_DIRECT3D    6043
  #define MSG_SSID_APPS_GRAPHICS_SVG         6044
  #define MSG_SSID_APPS_GRAPHICS_OPENVG      6045
  #define MSG_SSID_APPS_GRAPHICS_2D          6046
  #define MSG_SSID_APPS_GRAPHICS_QXPROFILER  6047
  #define MSG_SSID_APPS_GRAPHICS_DSP         6048
  #define MSG_SSID_APPS_GRAPHICS_GRP         6049
  #define MSG_SSID_APPS_GRAPHICS_MDP         6050
#define MSG_SSID_APPS_LAST                   6050


/* Default master category for aDSP Tasks. */
#define MSG_SSID_ADSPTASKS                     6500
  #define MSG_SSID_ADSPTASKS_KERNEL            6501
  #define MSG_SSID_ADSPTASKS_AFETASK           6502
  #define MSG_SSID_ADSPTASKS_VOICEPROCTASK     6503
  #define MSG_SSID_ADSPTASKS_VOCDECTASK        6504
  #define MSG_SSID_ADSPTASKS_VOCENCTASK        6505
  #define MSG_SSID_ADSPTASKS_VIDEOTASK         6506
  #define MSG_SSID_ADSPTASKS_VFETASK           6507
  #define MSG_SSID_ADSPTASKS_VIDEOENCTASK      6508
  #define MSG_SSID_ADSPTASKS_JPEGTASK          6509
  #define MSG_SSID_ADSPTASKS_AUDPPTASK         6510
  #define MSG_SSID_ADSPTASKS_AUDPLAY0TASK      6511
  #define MSG_SSID_ADSPTASKS_AUDPLAY1TASK      6512
  #define MSG_SSID_ADSPTASKS_AUDPLAY2TASK      6513
  #define MSG_SSID_ADSPTASKS_AUDPLAY3TASK      6514
  #define MSG_SSID_ADSPTASKS_AUDPLAY4TASK      6515
  #define MSG_SSID_ADSPTASKS_LPMTASK           6516
  #define MSG_SSID_ADSPTASKS_DIAGTASK          6517
  #define MSG_SSID_ADSPTASKS_AUDRECTASK        6518
  #define MSG_SSID_ADSPTASKS_AUDPREPROCTASK    6519
  #define MSG_SSID_ADSPTASKS_MODMATHTASK       6520
  #define MSG_SSID_ADSPTASKS_GRAPHICSTASK      6521

#define MSG_SSID_ADSPTASKS_LAST                6521


/* Messages arising from Linux on L4, or its drivers or applications. */
#define MSG_SSID_L4LINUX_KERNEL          7000
#define MSG_SSID_L4LINUX_KEYPAD          7001
#define MSG_SSID_L4LINUX_APPS            7002
#define MSG_SSID_L4LINUX_QDDAEMON        7003
#define MSG_SSID_L4LINUX_LAST            MSG_SSID_L4LINUX_QDDAEMON

/* Messages arising from Iguana on L4, or its servers and drivers. */
#define MSG_SSID_L4IGUANA_IGUANASERVER   7100   /* Iguana Server itself */
#define MSG_SSID_L4IGUANA_EFS2           7101   /* platform/apps stuff */
#define MSG_SSID_L4IGUANA_QDMS           7102
#define MSG_SSID_L4IGUANA_REX            7103
#define MSG_SSID_L4IGUANA_SMMS           7104
#define MSG_SSID_L4IGUANA_FRAMEBUFFER    7105   /* platform/iguana stuff */
#define MSG_SSID_L4IGUANA_KEYPAD         7106
#define MSG_SSID_L4IGUANA_NAMING         7107
#define MSG_SSID_L4IGUANA_SDIO           7108
#define MSG_SSID_L4IGUANA_SERIAL         7109
#define MSG_SSID_L4IGUANA_TIMER          7110
#define MSG_SSID_L4IGUANA_TRAMP          7111
#define MSG_SSID_L4IGUANA_LAST           MSG_SSID_L4IGUANA_TRAMP

/* Messages arising from L4-specific AMSS modules */
#define MSG_SSID_L4AMSS_QDIAG            7200
#define MSG_SSID_L4AMSS_APS              7201
#define MSG_SSID_L4AMSS_LAST             MSG_SSID_L4AMSS_APS


#ifdef FEATURE_HIT

/* Default master category for HIT. */
#define MSG_SSID_HIT         8000
#define MSG_SSID_HIT_LAST    8000

#endif /* FEATURE_HIT */

/* Default master category for UMB. */
#define MSG_SSID_UMB         9000
#define MSG_SSID_UMB_APP     9001    /* UMB Application component */
#define MSG_SSID_UMB_DS      9002      /* UMB Data Services component */
#define MSG_SSID_UMB_CP      9003      /* UMB Call Processing component */
#define MSG_SSID_UMB_RLL     9004      /* UMB Radio Link Layer component */
#define MSG_SSID_UMB_MAC     9005    /* UMB MAC component */
#define MSG_SSID_UMB_SRCH    9006   /* UMB SRCH component */
#define MSG_SSID_UMB_FW      9007     /* UMB Firmware component */
#define MSG_SSID_UMB_PLT     9008     /* UMB PLT component */
#define MSG_SSID_UMB_LAST    9008


/* EXAMPLE: Entry for an SSID */
#if 0

/* MSG_SSID_FOO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FOO
  #define MSG_BUILD_MASK_MSG_SSID_FOO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FOO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_LEGACY MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FOO

#endif

#endif /* end example */


/* MSG_SSID_AUDFMT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_AUDFMT
  #define MSG_BUILD_MASK_MSG_SSID_AUDFMT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_AUDFMT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_AUDFMT

#endif

/* MSG_SSID_AVS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_AVS
  #define MSG_BUILD_MASK_MSG_SSID_AVS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_AVS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_AVS

#endif


/* MSG_SSID_BOOT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_BOOT
  #define MSG_BUILD_MASK_MSG_SSID_BOOT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_BOOT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_BOOT

#endif

/* MSG_SSID_BT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_BT
  #define MSG_BUILD_MASK_MSG_SSID_BT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_BT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_BT

#endif


/* MSG_SSID_CM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_CM
  #define MSG_BUILD_MASK_MSG_SSID_CM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_CM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_CM

#endif


/* MSG_SSID_CMX */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_CMX
  #define MSG_BUILD_MASK_MSG_SSID_CMX    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_CMX)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_CMX

#endif

/* MSG_SSID_DIAG */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DIAG
  #define MSG_BUILD_MASK_MSG_SSID_DIAG    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DIAG)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DIAG

#endif


/* MSG_SSID_DSM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DSM
  #define MSG_BUILD_MASK_MSG_SSID_DSM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DSM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DSM

#endif


/* MSG_SSID_FS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FS
  #define MSG_BUILD_MASK_MSG_SSID_FS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FS

#endif

/* MSG_SSID_HS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_HS
  #define MSG_BUILD_MASK_MSG_SSID_HS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_HS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_HS

#endif

/* MSG_SSID_MDSP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MDSP
  #define MSG_BUILD_MASK_MSG_SSID_MDSP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MDSP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MDSP

#endif


/* MSG_SSID_QDSP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_QDSP
  #define MSG_BUILD_MASK_MSG_SSID_QDSP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_QDSP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_QDSP

#endif

/* MSG_SSID_REX */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_REX
  #define MSG_BUILD_MASK_MSG_SSID_REX    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_REX)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_REX

#endif

/* MSG_SSID_RF */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_RF
  #define MSG_BUILD_MASK_MSG_SSID_RF    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_RF)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_RF

#endif

/* MSG_SSID_SD */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SD
  #define MSG_BUILD_MASK_MSG_SSID_SD    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SD)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SD

#endif


/* MSG_SSID_SIO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SIO
  #define MSG_BUILD_MASK_MSG_SSID_SIO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SIO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SIO

#endif


/* MSG_SSID_VS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_VS
  #define MSG_BUILD_MASK_MSG_SSID_VS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_VS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_VS

#endif

/* MSG_SSID_WMS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_WMS
  #define MSG_BUILD_MASK_MSG_SSID_WMS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_WMS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_WMS

#endif

/* MSG_SSID_GPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_GPS
  #define MSG_BUILD_MASK_MSG_SSID_GPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_GPS)
#error code not present
#endif

/* MSG_SSID_MMOC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MMOC
  #define MSG_BUILD_MASK_MSG_SSID_MMOC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MMOC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MMOC

#endif

/* MSG_SSID_RUIM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_RUIM
  #define MSG_BUILD_MASK_MSG_SSID_RUIM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_RUIM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_RUIM

#endif

/* MSG_SSID_TMC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_TMC
  #define MSG_BUILD_MASK_MSG_SSID_TMC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_TMC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_TMC

#endif

/* MSG_SSID_FTM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FTM
  #define MSG_BUILD_MASK_MSG_SSID_FTM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FTM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FTM

#endif

/* MSG_SSID_MMGPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MMGPS
  #define MSG_BUILD_MASK_MSG_SSID_MMGPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MMGPS)
#error code not present
#endif

/* MSG_SSID_SLEEP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SLEEP
  #define MSG_BUILD_MASK_MSG_SSID_SLEEP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SLEEP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SLEEP

#endif

/* MSG_SSID_SAM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SAM
  #define MSG_BUILD_MASK_MSG_SSID_SAM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SAM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SAM

#endif

/* MSG_SSID_SRM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SRM
  #define MSG_BUILD_MASK_MSG_SSID_SRM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SRM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SRM

#endif

/* MSG_SSID_SFAT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SFAT
  #define MSG_BUILD_MASK_MSG_SSID_SFAT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SFAT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SFAT

#endif

/* MSG_SSID_JOYST */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_JOYST
  #define MSG_BUILD_MASK_MSG_SSID_JOYST    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_JOYST)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_JOYST

#endif

/* MSG_SSID_MFLO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MFLO
  #define MSG_BUILD_MASK_MSG_SSID_MFLO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MFLO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MFLO

#endif

/* MSG_SSID_DTV */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DTV
  #define MSG_BUILD_MASK_MSG_SSID_DTV    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DTV)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DTV

#endif

/* MSG_SSID_FLUTE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FLUTE
  #define MSG_BUILD_MASK_MSG_SSID_FLUTE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FLUTE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FLUTE

#endif

/* MSG_SSID_CAMERA */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_CAMERA
  #define MSG_BUILD_MASK_MSG_SSID_CAMERA    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_CAMERA)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_CAMERA

#endif

/* MSG_SSID_USBHOST */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_USBHOST
  #define MSG_BUILD_MASK_MSG_SSID_USBHOST    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_USBHOST)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_USBHOST

#endif

/* MSG_SSID_PROFILER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_PROFILER
  #define MSG_BUILD_MASK_MSG_SSID_PROFILER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_PROFILER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_PROFILER

#endif

/* MSG_SSID_TCXOMGR */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_TCXOMGR
  #define MSG_BUILD_MASK_MSG_SSID_TCXOMGR    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_TCXOMGR)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_TCXOMGR

#endif

/* MSG_SSID_EFS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_EFS
  #define MSG_BUILD_MASK_MSG_SSID_EFS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_EFS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_EFS

#endif

/* MSG_SSID_IRDA */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_IRDA
  #define MSG_BUILD_MASK_MSG_SSID_IRDA    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_IRDA)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_IRDA

#endif

/* MSG_SSID_FM_RADIO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FM_RADIO
  #define MSG_BUILD_MASK_MSG_SSID_FM_RADIO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FM_RADIO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FM_RADIO

#endif

/* MSG_SSID_AAM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_AAM
  #define MSG_BUILD_MASK_MSG_SSID_AAM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_AAM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_AAM

#endif

/* MSG_SSID_BM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_BM
  #define MSG_BUILD_MASK_MSG_SSID_BM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_BM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_BM

#endif

/* MSG_SSID_PE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_PE
  #define MSG_BUILD_MASK_MSG_SSID_PE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_PE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_PE

#endif

/* MSG_SSID_QIPCALL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_QIPCALL
  #define MSG_BUILD_MASK_MSG_SSID_QIPCALL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_QIPCALL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_QIPCALL

#endif

/* MSG_SSID_HSUSB */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_HSUSB
  #define MSG_BUILD_MASK_MSG_SSID_HSUSB    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_HSUSB)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_HSUSB

#endif

/* MSG_SSID_FC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_FC
  #define MSG_BUILD_MASK_MSG_SSID_FC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_FC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_FC

#endif

/* MSG_SSID_MGP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MGP
  #define MSG_BUILD_MASK_MSG_SSID_MGP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MGP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MGP

#endif

/* MSG_SSID_MGPME */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPME
  #define MSG_BUILD_MASK_MSG_SSID_MGPME    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MGPME)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MGPME

#endif

/* MSG_SSID_GPSOS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSOS
  #define MSG_BUILD_MASK_MSG_SSID_GPSOS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_GPSOS)
#error code not present
#endif

/* MSG_SSID_MGPPE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MGPPE
  #define MSG_BUILD_MASK_MSG_SSID_MGPPE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MGPPE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MGPPE

#endif

/* MSG_SSID_GPSSM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_GPSSM
  #define MSG_BUILD_MASK_MSG_SSID_GPSSM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_GPSSM)
#error code not present
#endif

/* MSG_SSID_IMS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_IMS
  #define MSG_BUILD_MASK_MSG_SSID_IMS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_IMS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_IMS

#endif

/* MSG_SSID_MBP_RF */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_MBP_RF
  #define MSG_BUILD_MASK_MSG_SSID_MBP_RF    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_MBP_RF)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_MBP_RF

#endif

/* MSG_SSID_SNS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SNS
  #define MSG_BUILD_MASK_MSG_SSID_SNS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SNS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SNS

#endif

/* MSG_SSID_WM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_WM
  #define MSG_BUILD_MASK_MSG_SSID_WM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_WM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_WM

#endif

/* MSG_SSID_ONCRPC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC

#endif

/* MSG_SSID_ONCRPC_MISC_MODEM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_MISC_MODEM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM

#endif

/* MSG_SSID_ONCRPC_MISC_APPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_MISC_APPS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS

#endif

/* MSG_SSID_ONCRPC_CM_MODEM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_CM_MODEM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM

#endif

/* MSG_SSID_ONCRPC_CM_APPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_CM_APPS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS

#endif

/* MSG_SSID_ONCRPC_DB */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_DB)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB

#endif

/* MSG_SSID_ONCRPC_SND */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND
  #define MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ONCRPC_SND)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND

#endif

#ifdef FEATURE_IS2000
/* 1X related SSIDs */

/* MSG_SSID_1X */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X
  #define MSG_BUILD_MASK_MSG_SSID_1X    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X

#endif

/* MSG_SSID_1X_ACP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_ACP
  #define MSG_BUILD_MASK_MSG_SSID_1X_ACP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_ACP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_ACP

#endif

/* MSG_SSID_1X_DCP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_DCP
  #define MSG_BUILD_MASK_MSG_SSID_1X_DCP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_DCP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_DCP

#endif

/* MSG_SSID_1X_DEC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_DEC
  #define MSG_BUILD_MASK_MSG_SSID_1X_DEC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_DEC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_DEC

#endif

/* MSG_SSID_1X_ENC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_ENC
  #define MSG_BUILD_MASK_MSG_SSID_1X_ENC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_ENC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_ENC

#endif

/* MSG_SSID_1X_GPSSRCH */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH
  #define MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_GPSSRCH)
#error code not present
#endif

/* MSG_SSID_1X_MUX */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_MUX
  #define MSG_BUILD_MASK_MSG_SSID_1X_MUX    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_MUX)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_MUX

#endif

/* MSG_SSID_1X_SRCH */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_1X_SRCH
  #define MSG_BUILD_MASK_MSG_SSID_1X_SRCH    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_1X_SRCH)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_1X_SRCH

#endif

#endif  /* FEATURE_IS2000 */



#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */



#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
/* DS SSIDs  */

/* MSG_SSID_DS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS
  #define MSG_BUILD_MASK_MSG_SSID_DS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS

#endif

/* MSG_SSID_DS_RLP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_RLP
  #define MSG_BUILD_MASK_MSG_SSID_DS_RLP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_RLP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_RLP

#endif

/* MSG_SSID_DS_PPP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_PPP
  #define MSG_BUILD_MASK_MSG_SSID_DS_PPP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_PPP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_PPP

#endif

/* MSG_SSID_DS_TCPIP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_TCPIP
  #define MSG_BUILD_MASK_MSG_SSID_DS_TCPIP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_TCPIP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_TCPIP

#endif

/* MSG_SSID_DS_IS707 */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_IS707
  #define MSG_BUILD_MASK_MSG_SSID_DS_IS707    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_IS707)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_IS707

#endif

/* MSG_SSID_DS_3GMGR */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_3GMGR
  #define MSG_BUILD_MASK_MSG_SSID_DS_3GMGR    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_3GMGR)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_3GMGR

#endif

/* MSG_SSID_DS_PS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_PS
  #define MSG_BUILD_MASK_MSG_SSID_DS_PS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_PS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_PS

#endif

/* MSG_SSID_DS_MIP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_MIP
  #define MSG_BUILD_MASK_MSG_SSID_DS_MIP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_MIP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_MIP

#endif

/* MSG_SSID_DS_UMTS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_UMTS
  #define MSG_BUILD_MASK_MSG_SSID_DS_UMTS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_UMTS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_UMTS

#endif

/* MSG_SSID_DS_GPRS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_GPRS
  #define MSG_BUILD_MASK_MSG_SSID_DS_GPRS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_GPRS)
#error code not present
#endif

/* MSG_SSID_DS_GSM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_GSM
  #define MSG_BUILD_MASK_MSG_SSID_DS_GSM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_GSM)
#error code not present
#endif

/* MSG_SSID_DS_SOCKETS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS
  #define MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_SOCKETS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS

#endif

/* MSG_SSID_DS_ATCOP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_ATCOP
  #define MSG_BUILD_MASK_MSG_SSID_DS_ATCOP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_ATCOP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_ATCOP

#endif

/* MSG_SSID_DS_SIO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_SIO
  #define MSG_BUILD_MASK_MSG_SSID_DS_SIO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_SIO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_SIO

#endif

/* MSG_SSID_DS_BCMCS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_BCMCS
  #define MSG_BUILD_MASK_MSG_SSID_DS_BCMCS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_BCMCS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_BCMCS

#endif

/* MSG_SSID_DS_MLRLP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_MLRLP
  #define MSG_BUILD_MASK_MSG_SSID_DS_MLRLP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_MLRLP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_MLRLP

#endif

/* MSG_SSID_DS_RTP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_RTP
  #define MSG_BUILD_MASK_MSG_SSID_DS_RTP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_RTP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_RTP

#endif

/* MSG_SSID_DS_SIPSTACK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK
  #define MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_SIPSTACK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK

#endif

/* MSG_SSID_DS_ROHC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_ROHC
  #define MSG_BUILD_MASK_MSG_SSID_DS_ROHC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_ROHC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_ROHC

#endif

/* MSG_SSID_DS_DOQOS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_DOQOS
  #define MSG_BUILD_MASK_MSG_SSID_DS_DOQOS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_DOQOS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_DOQOS

#endif

/* MSG_SSID_DS_IPC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_IPC
  #define MSG_BUILD_MASK_MSG_SSID_DS_IPC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_IPC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_IPC

#endif

/* MSG_SSID_DS_SHIM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_DS_SHIM
  #define MSG_BUILD_MASK_MSG_SSID_DS_SHIM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_DS_SHIM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_DS_SHIM

#endif

#endif /* defined (FEATURE_DS) || defined (FEATURE_DATA) */


/* MSG_SSID_SEC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC
  #define MSG_BUILD_MASK_MSG_SSID_SEC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC

#endif

/* MSG_SSID_SEC_CRYPTO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO
  #define MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_CRYPTO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO

#endif

/* MSG_SSID_SEC_SSL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_SSL
  #define MSG_BUILD_MASK_MSG_SSID_SEC_SSL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_SSL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_SSL

#endif

/* MSG_SSID_SEC_IPSEC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC
  #define MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_IPSEC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC

#endif

/* MSG_SSID_SEC_SFS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_SFS
  #define MSG_BUILD_MASK_MSG_SSID_SEC_SFS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_SFS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_SFS

#endif

/* MSG_SSID_SEC_TEST */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_TEST
  #define MSG_BUILD_MASK_MSG_SSID_SEC_TEST    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_TEST)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_TEST

#endif

/* MSG_SSID_SEC_CNTAGENT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT
  #define MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_CNTAGENT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT

#endif

/* MSG_SSID_SEC_RIGHTSMGR */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR
  #define MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_RIGHTSMGR)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR

#endif

/* MSG_SSID_SEC_ROAP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_ROAP
  #define MSG_BUILD_MASK_MSG_SSID_SEC_ROAP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_ROAP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_ROAP

#endif

/* MSG_SSID_SEC_MEDIAMGR */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR
  #define MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_MEDIAMGR)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR

#endif

/* MSG_SSID_SEC_IDSTORE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE
  #define MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_IDSTORE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE

#endif

/* MSG_SSID_SEC_IXFILE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE
  #define MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_IXFILE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE

#endif

/* MSG_SSID_SEC_IXSQL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL
  #define MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_IXSQL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL

#endif

/* MSG_SSID_SEC_IXCOMMON */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON
  #define MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_IXCOMMON)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON

#endif

/* MSG_SSID_SEC_BCASTCNTAGENT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT
  #define MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_SEC_BCASTCNTAGENT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT

#endif

/* APPS SSIDs  */

/* MSG_SSID_APPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS
  #define MSG_BUILD_MASK_MSG_SSID_APPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS

#endif


/* MSG_SSID_APPS_APPMGR */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR
  #define MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_APPMGR)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR

#endif

/* MSG_SSID_APPS_UI */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_UI
  #define MSG_BUILD_MASK_MSG_SSID_APPS_UI    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_UI)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_UI

#endif

/* MSG_SSID_APPS_QTV */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV

#endif

/* MSG_SSID_APPS_QVP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP

#endif

/* MSG_SSID_APPS_QVP_STATISTICS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_STATISTICS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS

#endif

/* MSG_SSID_APPS_QVP_VENCODER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_VENCODER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER

#endif

/* MSG_SSID_APPS_QVP_MODEM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_MODEM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM

#endif

/* MSG_SSID_APPS_QVP_UI */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_UI)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI

#endif

/* MSG_SSID_APPS_QVP_STACK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_STACK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK

#endif

/* MSG_SSID_APPS_QVP_VDECODER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QVP_VDECODER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER

#endif

/* MSG_SSID_APPS_ACM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_ACM
  #define MSG_BUILD_MASK_MSG_SSID_APPS_ACM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_ACM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_ACM

#endif

/* MSG_SSID_APPS_HEAP_PROFILE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE
  #define MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_HEAP_PROFILE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE

#endif

/* MSG_SSID_APPS_QTV_GENERAL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_GENERAL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL

#endif

/* MSG_SSID_APPS_QTV_DEBUG */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_DEBUG)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG

#endif

/* MSG_SSID_APPS_QTV_STATISTICS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_STATISTICS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS

#endif

/* MSG_SSID_APPS_QTV_UI_TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_UI_TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK

#endif

/* MSG_SSID_APPS_QTV_MP4_PLAYER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_MP4_PLAYER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER

#endif

/* MSG_SSID_APPS_QTV_AUDIO_TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_AUDIO_TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK

#endif

/* MSG_SSID_APPS_QTV_VIDEO_TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_VIDEO_TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK

#endif

/* MSG_SSID_APPS_QTV_STREAMING */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_STREAMING)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING

#endif

/* MSG_SSID_APPS_QTV_MPEG4_TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_MPEG4_TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK

#endif

/* MSG_SSID_APPS_QTV_FILE_OPS  */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS 
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS     MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_FILE_OPS )
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS 

#endif

/* MSG_SSID_APPS_QTV_RTP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_RTP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP

#endif

/* MSG_SSID_APPS_QTV_RTCP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_RTCP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP

#endif

/* MSG_SSID_APPS_QTV_RTSP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_RTSP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP

#endif

/* MSG_SSID_APPS_QTV_SDP_PARSE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_SDP_PARSE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE

#endif

/* MSG_SSID_APPS_QTV_ATOM_PARSE */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_ATOM_PARSE)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE

#endif

/* MSG_SSID_APPS_QTV_TEXT_TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_TEXT_TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK

#endif

/* MSG_SSID_APPS_QTV_DEC_DSP_IF */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_DEC_DSP_IF)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF

#endif

/* MSG_SSID_APPS_QTV_STREAM_RECORDING */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_STREAM_RECORDING)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING

#endif

/* MSG_SSID_APPS_QTV_CONFIGURATION */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_CONFIGURATION)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION

#endif

/* MSG_SSID_APPS_QCAMERA */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QCAMERA)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA

#endif

/* MSG_SSID_APPS_QCAMCORDER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QCAMCORDER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER

#endif

/* MSG_SSID_APPS_BREW */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_BREW
  #define MSG_BUILD_MASK_MSG_SSID_APPS_BREW    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_BREW)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_BREW

#endif

/* MSG_SSID_APPS_QDJ */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QDJ
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QDJ    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QDJ)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QDJ

#endif

/* MSG_SSID_APPS_QDTX */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QDTX
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QDTX    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QDTX)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QDTX

#endif

/* MSG_SSID_APPS_QTV_BCAST_FLO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO
  #define MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_QTV_BCAST_FLO)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO

#endif

/* MSG_SSID_APPS_MDP_GENERAL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL
  #define MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_MDP_GENERAL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL

#endif

/* MSG_SSID_APPS_PBM */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_PBM
  #define MSG_BUILD_MASK_MSG_SSID_APPS_PBM    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_PBM)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_PBM

#endif

/* MSG_SSID_APPS_GRAPHICS_GENERAL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_GENERAL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL

#endif

/* MSG_SSID_APPS_GRAPHICS_EGL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_EGL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL

#endif

/* MSG_SSID_APPS_GRAPHICS_OPENGL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_OPENGL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL

#endif

/* MSG_SSID_APPS_GRAPHICS_DIRECT3D */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_DIRECT3D)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D

#endif

/* MSG_SSID_APPS_GRAPHICS_SVG */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_SVG)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG

#endif

/* MSG_SSID_APPS_GRAPHICS_OPENVG */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_OPENVG)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG

#endif

/* MSG_SSID_APPS_GRAPHICS_2D */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_2D)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D

#endif

/* MSG_SSID_APPS_GRAPHICS_QXPROFILER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_QXPROFILER)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER

#endif

/* MSG_SSID_APPS_GRAPHICS_DSP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_DSP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP

#endif

/* MSG_SSID_APPS_GRAPHICS_GRP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_GRP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP

#endif

/* MSG_SSID_APPS_GRAPHICS_MDP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP
  #define MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_APPS_GRAPHICS_MDP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP

#endif

/* MSG_SSID_ADSPTASKS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS

#endif

/* MSG_SSID_ADSPTASKS_KERNEL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_KERNEL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL

#endif

/* MSG_SSID_ADSPTASKS_AFETASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AFETASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK

#endif

/* MSG_SSID_ADSPTASKS_VOICEPROCTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VOICEPROCTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK

#endif

/* MSG_SSID_ADSPTASKS_VOCDECTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VOCDECTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK

#endif

/* MSG_SSID_ADSPTASKS_VOCENCTASK     */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VOCENCTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK

#endif

/* MSG_SSID_ADSPTASKS_VIDEOTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VIDEOTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK

#endif

/* MSG_SSID_ADSPTASKS_VFETASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VFETASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK

#endif

/* MSG_SSID_ADSPTASKS_VIDEOENCTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_VIDEOENCTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK

#endif

/* MSG_SSID_ADSPTASKS_JPEGTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_JPEGTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPPTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPPTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPLAY0TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPLAY0TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPLAY1TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPLAY1TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPLAY2TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPLAY2TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPLAY3TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPLAY3TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPLAY4TASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPLAY4TASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK

#endif

/* MSG_SSID_ADSPTASKS_LPMTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_LPMTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK

#endif

/* MSG_SSID_ADSPTASKS_DIAGTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_DIAGTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK

#endif

/* MSG_SSID_ADSPTASKS_AUDRECTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDRECTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK

#endif

/* MSG_SSID_ADSPTASKS_AUDPREPROCTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_AUDPREPROCTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK

#endif

/* MSG_SSID_ADSPTASKS_MODMATHTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_MODMATHTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK

#endif

/* MSG_SSID_ADSPTASKS_GRAPHICSTASK */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK
  #define MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_ADSPTASKS_GRAPHICSTASK)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK

#endif

/* OS WINCE SSIDs  */

/* MSG_SSID_L4LINUX_KERNEL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4LINUX_KERNEL
  #define MSG_BUILD_MASK_MSG_SSID_L4LINUX_KERNEL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4LINUX_KERNEL)
#error code not present
#endif

/* MSG_SSID_L4LINUX_KEYPAD */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4LINUX_KEYPAD
  #define MSG_BUILD_MASK_MSG_SSID_L4LINUX_KEYPAD    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4LINUX_KEYPAD)
#error code not present
#endif

/* MSG_SSID_L4LINUX_APPS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4LINUX_APPS
  #define MSG_BUILD_MASK_MSG_SSID_L4LINUX_APPS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4LINUX_APPS)
#error code not present
#endif

/* MSG_SSID_L4LINUX_QDDAEMON */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4LINUX_QDDAEMON
  #define MSG_BUILD_MASK_MSG_SSID_L4LINUX_QDDAEMON    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4LINUX_QDDAEMON)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_IGUANASERVER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_IGUANASERVER
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_IGUANASERVER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_IGUANASERVER)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_EFS2 */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_EFS2
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_EFS2    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_EFS2)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_QDMS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_QDMS
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_QDMS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_QDMS)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_REX */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_REX
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_REX    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_REX)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_SMMS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SMMS
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SMMS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_SMMS)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_FRAMEBUFFER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_FRAMEBUFFER
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_FRAMEBUFFER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_FRAMEBUFFER)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_KEYPAD */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_KEYPAD
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_KEYPAD    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_KEYPAD)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_NAMING */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_NAMING
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_NAMING    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_NAMING)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_SDIO */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SDIO
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SDIO    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_SDIO)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_SERIAL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SERIAL
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SERIAL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_SERIAL)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_TIMER */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TIMER
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TIMER    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_TIMER)
#error code not present
#endif

/* MSG_SSID_L4IGUANA_TRAMP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TRAMP
  #define MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TRAMP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4IGUANA_TRAMP)
#error code not present
#endif

/* MSG_SSID_L4AMSS_QDIAG */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4AMSS_QDIAG
  #define MSG_BUILD_MASK_MSG_SSID_L4AMSS_QDIAG    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4AMSS_QDIAG)
#error code not present
#endif

/* MSG_SSID_L4AMSS_APS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_L4AMSS_APS
  #define MSG_BUILD_MASK_MSG_SSID_L4AMSS_APS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_L4AMSS_APS)
#error code not present
#endif


#ifdef FEATURE_HIT

/* MSG_SSID_HIT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_HIT
  #define MSG_BUILD_MASK_MSG_SSID_HIT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_HIT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_HIT

#endif

#endif /* FEATURE_HIT */

/* MSG_SSID_UMB */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB
  #define MSG_BUILD_MASK_MSG_SSID_UMB    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB

#endif

/* MSG_SSID_UMB_APP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_APP
  #define MSG_BUILD_MASK_MSG_SSID_UMB_APP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_APP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_APP

#endif

/* MSG_SSID_UMB_DS */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_DS
  #define MSG_BUILD_MASK_MSG_SSID_UMB_DS    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_DS)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_DS

#endif

/* MSG_SSID_UMB_CP */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_CP
  #define MSG_BUILD_MASK_MSG_SSID_UMB_CP    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_CP)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_CP

#endif

/* MSG_SSID_UMB_RLL */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_RLL
  #define MSG_BUILD_MASK_MSG_SSID_UMB_RLL    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_RLL)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_RLL

#endif

/* MSG_SSID_UMB_MAC */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_MAC
  #define MSG_BUILD_MASK_MSG_SSID_UMB_MAC    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_MAC)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_MAC

#endif

/* MSG_SSID_UMB_SRCH */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_SRCH
  #define MSG_BUILD_MASK_MSG_SSID_UMB_SRCH    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_SRCH)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_SRCH

#endif

/* MSG_SSID_UMB_FW */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_FW
  #define MSG_BUILD_MASK_MSG_SSID_UMB_FW    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_FW)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_FW

#endif

/* MSG_SSID_UMB_PLT */

/* In case it is not defined in msgtgt.h */
#ifndef MSG_BUILD_MASK_MSG_SSID_UMB_PLT
  #define MSG_BUILD_MASK_MSG_SSID_UMB_PLT    MSG_BUILD_MASK_DFLT
#endif

/* If the build environment has specified this file to translate legacy
   messages to the this SSID. */
#if defined (MSG_BT_SSID_DFLT) && (MSG_BT_SSID_DFLT==MSG_SSID_UMB_PLT)
  
  #undef MSG_SSID_DFLT
  #define MSG_SSID_DFLT MSG_BT_SSID_DFLT
  
  #undef MSG_BUILD_MASK_MSG_SSID_DFLT
  #define MSG_BUILD_MASK_MSG_SSID_DFLT MSG_BUILD_MASK_MSG_SSID_UMB_PLT

#endif

/* These SSIDs are reserved for OEM (customer) use only. These IDs will
   never be used by Qualcomm. */
#define MSG_FIRST_TARGET_OEM_SSID (0xC000)
#define MSG_LAST_TARGET_OEM_SSID (0xCFFF)

/* Macros to generate the Build Mask and Run Time tables */

/*---------------------------------------------------------------------------
  This is the structure used for both the build mask array and the RT mask
  array.  It consists of a range of IDs and a pointer to an array indexed
  as follows:
  
  index:
  SSID - ssid_start
  
  Total # of entries:
  ssid_last - ssid_first + 1;
---------------------------------------------------------------------------*/
typedef struct
{
  uint16 ssid_first;      /* Start of range of supported SSIDs */
  uint16 ssid_last;       /* Last SSID in range */

  /* Array of (ssid_last - ssid_first + 1) masks */
  const uint32 *bt_mask_array;
  uint32 *rt_mask_array;
}
msg_mask_tbl_type;

/* The above typedef and constant need to appear above the MSG_TBL_GEN flag 
   as a workaround to the IRAM csplit.pl utility. */
#ifdef MSG_TBL_GEN

/* This table must be maintained as SSIDs are added!
   NOTE: The legacy build mask is handled on a per-file basis. */
static const uint32 msg_bld_masks_gen[] = 
{
  MSG_LVL_LOW, /* special case - may vary by file is MSG_LEVEL is used */
  MSG_BUILD_MASK_MSG_SSID_AUDFMT,
  MSG_BUILD_MASK_MSG_SSID_AVS,
  MSG_BUILD_MASK_MSG_SSID_BOOT,
  MSG_BUILD_MASK_MSG_SSID_BT,
  MSG_BUILD_MASK_MSG_SSID_CM,
  MSG_BUILD_MASK_MSG_SSID_CMX,
  MSG_BUILD_MASK_MSG_SSID_DIAG,
  MSG_BUILD_MASK_MSG_SSID_DSM,
  MSG_BUILD_MASK_MSG_SSID_FS,
  MSG_BUILD_MASK_MSG_SSID_HS,
  MSG_BUILD_MASK_MSG_SSID_MDSP,
  MSG_BUILD_MASK_MSG_SSID_QDSP,
  MSG_BUILD_MASK_MSG_SSID_REX,
  MSG_BUILD_MASK_MSG_SSID_RF,
  MSG_BUILD_MASK_MSG_SSID_SD,
  MSG_BUILD_MASK_MSG_SSID_SIO,
  MSG_BUILD_MASK_MSG_SSID_VS,     
  MSG_BUILD_MASK_MSG_SSID_WMS,
  MSG_BUILD_MASK_MSG_SSID_GPS,
  MSG_BUILD_MASK_MSG_SSID_MMOC,
  MSG_BUILD_MASK_MSG_SSID_RUIM,
  MSG_BUILD_MASK_MSG_SSID_TMC,
  MSG_BUILD_MASK_MSG_SSID_FTM,
  MSG_BUILD_MASK_MSG_SSID_MMGPS,
  MSG_BUILD_MASK_MSG_SSID_SLEEP,
  MSG_BUILD_MASK_MSG_SSID_SAM,
  MSG_BUILD_MASK_MSG_SSID_SRM,
  MSG_BUILD_MASK_MSG_SSID_SFAT,
  MSG_BUILD_MASK_MSG_SSID_JOYST,
  MSG_BUILD_MASK_MSG_SSID_MFLO,
  MSG_BUILD_MASK_MSG_SSID_DTV,
  MSG_BUILD_MASK_MSG_SSID_TCXOMGR,
  MSG_BUILD_MASK_MSG_SSID_EFS,
  MSG_BUILD_MASK_MSG_SSID_IRDA,
  MSG_BUILD_MASK_MSG_SSID_FM_RADIO,
  MSG_BUILD_MASK_MSG_SSID_AAM,
  MSG_BUILD_MASK_MSG_SSID_BM,
  MSG_BUILD_MASK_MSG_SSID_PE,
  MSG_BUILD_MASK_MSG_SSID_QIPCALL,
  MSG_BUILD_MASK_MSG_SSID_FLUTE,
  MSG_BUILD_MASK_MSG_SSID_CAMERA,
  MSG_BUILD_MASK_MSG_SSID_HSUSB,
  MSG_BUILD_MASK_MSG_SSID_FC,
  MSG_BUILD_MASK_MSG_SSID_USBHOST,
  MSG_BUILD_MASK_MSG_SSID_PROFILER,
  MSG_BUILD_MASK_MSG_SSID_MGP,
  MSG_BUILD_MASK_MSG_SSID_MGPME,
  MSG_BUILD_MASK_MSG_SSID_GPSOS,
  MSG_BUILD_MASK_MSG_SSID_MGPPE,
  MSG_BUILD_MASK_MSG_SSID_GPSSM,
  MSG_BUILD_MASK_MSG_SSID_IMS,
  MSG_BUILD_MASK_MSG_SSID_MBP_RF,
  MSG_BUILD_MASK_MSG_SSID_SNS,
  MSG_BUILD_MASK_MSG_SSID_WM,

/* EXAMPLE, placing build mask in table for msg.c.  All build masks not defined
   in diagtgt.h will have a table entry that is the same as msg.c. */
#if 0
  MSG_BUILD_MASK_MSG_SSID_FOO
#endif
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_gen[sizeof (msg_bld_masks_gen) /
  sizeof (msg_bld_masks_gen[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_oncrpc[] = 
{
  MSG_BUILD_MASK_MSG_SSID_ONCRPC,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_oncrpc[sizeof (msg_bld_masks_oncrpc) /
  sizeof (msg_bld_masks_oncrpc[0])];


#ifdef FEATURE_IS2000

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_1X[] = 
{
  MSG_BUILD_MASK_MSG_SSID_1X,
  MSG_BUILD_MASK_MSG_SSID_1X_ACP,
  MSG_BUILD_MASK_MSG_SSID_1X_DCP,
  MSG_BUILD_MASK_MSG_SSID_1X_DEC,
  MSG_BUILD_MASK_MSG_SSID_1X_ENC,
  MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH,
  MSG_BUILD_MASK_MSG_SSID_1X_MUX,
  MSG_BUILD_MASK_MSG_SSID_1X_SRCH
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_1X[sizeof (msg_bld_masks_1X) /
  sizeof (msg_bld_masks_1X[0])];

#endif /* FEATURE_IS2000 */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */


#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_ds[] = 
{
  MSG_BUILD_MASK_MSG_SSID_DS,
  MSG_BUILD_MASK_MSG_SSID_DS_RLP,
  MSG_BUILD_MASK_MSG_SSID_DS_PPP,
  MSG_BUILD_MASK_MSG_SSID_DS_TCPIP,
  MSG_BUILD_MASK_MSG_SSID_DS_IS707,
  MSG_BUILD_MASK_MSG_SSID_DS_3GMGR,
  MSG_BUILD_MASK_MSG_SSID_DS_PS,
  MSG_BUILD_MASK_MSG_SSID_DS_MIP,
  MSG_BUILD_MASK_MSG_SSID_DS_UMTS,
  MSG_BUILD_MASK_MSG_SSID_DS_GPRS,
  MSG_BUILD_MASK_MSG_SSID_DS_GSM,
  MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS,
  MSG_BUILD_MASK_MSG_SSID_DS_ATCOP,
  MSG_BUILD_MASK_MSG_SSID_DS_SIO,
  MSG_BUILD_MASK_MSG_SSID_DS_BCMCS,
  MSG_BUILD_MASK_MSG_SSID_DS_MLRLP,
  MSG_BUILD_MASK_MSG_SSID_DS_RTP,
  MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK,
  MSG_BUILD_MASK_MSG_SSID_DS_ROHC,
  MSG_BUILD_MASK_MSG_SSID_DS_DOQOS,
  MSG_BUILD_MASK_MSG_SSID_DS_IPC,
  MSG_BUILD_MASK_MSG_SSID_DS_SHIM,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_ds[sizeof (msg_bld_masks_ds) /
  sizeof (msg_bld_masks_ds[0])];

#endif /* defined (FEATURE_DS) || defined (FEATURE_DATA) */

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_sec[] = 
{
  MSG_BUILD_MASK_MSG_SSID_SEC,
  MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO,
  MSG_BUILD_MASK_MSG_SSID_SEC_SSL,
  MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC,
  MSG_BUILD_MASK_MSG_SSID_SEC_SFS,
  MSG_BUILD_MASK_MSG_SSID_SEC_TEST,
  MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT,
  MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR,
  MSG_BUILD_MASK_MSG_SSID_SEC_ROAP,
  MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR,
  MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON,
  MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_sec[sizeof (msg_bld_masks_sec) /
  sizeof (msg_bld_masks_sec[0])];


/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_apps[] = 
{
  MSG_BUILD_MASK_MSG_SSID_APPS,
  MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR,
  MSG_BUILD_MASK_MSG_SSID_APPS_UI,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER,
  MSG_BUILD_MASK_MSG_SSID_APPS_ACM,
  MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION,
  MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA,
  MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER,
  MSG_BUILD_MASK_MSG_SSID_APPS_BREW,
  MSG_BUILD_MASK_MSG_SSID_APPS_QDJ,
  MSG_BUILD_MASK_MSG_SSID_APPS_QDTX,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO,
  MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_PBM,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_apps[sizeof (msg_bld_masks_apps) /
  sizeof (msg_bld_masks_apps[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_adsptasks[] = 
{
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK,      
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_adsptasks[sizeof (msg_bld_masks_adsptasks) /
  sizeof (msg_bld_masks_adsptasks[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_linux[] = 
{
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_KERNEL,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_KEYPAD,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_APPS,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_QDDAEMON,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_l4_linux[sizeof (msg_bld_masks_l4_linux) /
  sizeof (msg_bld_masks_l4_linux[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_iguana[] =
{
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_IGUANASERVER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_EFS2,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_QDMS,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_REX,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SMMS,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_FRAMEBUFFER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_KEYPAD,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_NAMING,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SDIO,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SERIAL,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TIMER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TRAMP,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_l4_iguana[sizeof (msg_bld_masks_l4_iguana) /
  sizeof (msg_bld_masks_l4_iguana[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_amss[] =
{
  MSG_BUILD_MASK_MSG_SSID_L4AMSS_QDIAG,
  MSG_BUILD_MASK_MSG_SSID_L4AMSS_APS,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_l4_amss[sizeof (msg_bld_masks_l4_amss) /
  sizeof (msg_bld_masks_l4_amss[0])];


#ifdef FEATURE_HIT

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_hit[] = 
{
  MSG_BUILD_MASK_MSG_SSID_HIT
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_hit[sizeof (msg_bld_masks_hit) /
  sizeof (msg_bld_masks_hit[0])];

#endif /* FEATURE_HIT */

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_umb[] = 
{
  MSG_BUILD_MASK_MSG_SSID_UMB,
  MSG_BUILD_MASK_MSG_SSID_UMB_APP,
  MSG_BUILD_MASK_MSG_SSID_UMB_DS,
  MSG_BUILD_MASK_MSG_SSID_UMB_CP,
  MSG_BUILD_MASK_MSG_SSID_UMB_RLL,
  MSG_BUILD_MASK_MSG_SSID_UMB_MAC,
  MSG_BUILD_MASK_MSG_SSID_UMB_SRCH,
  MSG_BUILD_MASK_MSG_SSID_UMB_FW,
  MSG_BUILD_MASK_MSG_SSID_UMB_PLT,
};

/* To will be initialized by msg_init() */
static uint32 msg_rt_masks_umb[sizeof (msg_bld_masks_umb) /
  sizeof (msg_bld_masks_umb[0])];

const msg_mask_tbl_type msg_mask_tbl[] = {
  {
    MSG_SSID_GEN_FIRST,
    /* FIRST + array count */
    MSG_SSID_GEN_FIRST +
      sizeof (msg_bld_masks_gen) / sizeof (msg_bld_masks_gen[0]) - 1,
    msg_bld_masks_gen,
    msg_rt_masks_gen
  }
  
  ,
  {
    MSG_SSID_ONCRPC,
    /* FIRST + array count */
    MSG_SSID_ONCRPC +
      sizeof (msg_bld_masks_oncrpc) / sizeof (msg_bld_masks_oncrpc[0]) - 1,
    msg_bld_masks_oncrpc,
    msg_rt_masks_oncrpc
  }

#ifdef FEATURE_IS2000
  ,
  {
    MSG_SSID_1X,
    /* FIRST + array count */
    MSG_SSID_1X +
      sizeof (msg_bld_masks_1X) / sizeof (msg_bld_masks_1X[0]) - 1,
    msg_bld_masks_1X,
    msg_rt_masks_1X
  }
#endif /* FEATURE_IS2000 */ 

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* defined (FEATURE_WCDMA) || defined (FEATURE_GSM) */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_WLAN
#error code not present
#endif /* FEATURE_WLAN */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
  ,
  {
    MSG_SSID_DS,
    /* FIRST + array count */
    MSG_SSID_DS +
      sizeof (msg_bld_masks_ds) / sizeof (msg_bld_masks_ds[0]) - 1,
    msg_bld_masks_ds,
    msg_rt_masks_ds
  }
#endif /* defined (FEATURE_DS) || defined (FEATURE_DATA) */

  ,
  {
    MSG_SSID_SEC,
    /* FIRST + array count */
    MSG_SSID_SEC +
      sizeof (msg_bld_masks_sec) / sizeof (msg_bld_masks_sec[0]) - 1,
    msg_bld_masks_sec,
    msg_rt_masks_sec
  }

  ,
  {
    MSG_SSID_APPS,
    /* FIRST + array count */
    MSG_SSID_APPS +
      sizeof (msg_bld_masks_apps) / sizeof (msg_bld_masks_apps[0]) - 1,
    msg_bld_masks_apps,
    msg_rt_masks_apps
  }

  ,
  {
    MSG_SSID_ADSPTASKS,
    /* FIRST + array count */
    MSG_SSID_ADSPTASKS +
      sizeof (msg_bld_masks_adsptasks) / sizeof (msg_bld_masks_adsptasks[0]) - 1,
    msg_bld_masks_adsptasks,
    msg_rt_masks_adsptasks
  }

  ,
  {
    MSG_SSID_L4LINUX_KERNEL,
    /* FIRST + array count */
    MSG_SSID_L4LINUX_KERNEL +
      sizeof (msg_bld_masks_l4_linux) / sizeof (msg_bld_masks_l4_linux[0]) - 1,
    msg_bld_masks_l4_linux,
    msg_rt_masks_l4_linux
  }

  ,
  {
    MSG_SSID_L4IGUANA_IGUANASERVER,
    /* FIRST + array count */
    MSG_SSID_L4IGUANA_IGUANASERVER +
      sizeof (msg_bld_masks_l4_iguana) / sizeof (msg_bld_masks_l4_iguana[0]) - 1,
    msg_bld_masks_l4_iguana,
    msg_rt_masks_l4_iguana
  }

  ,
  {
    MSG_SSID_L4AMSS_QDIAG,
    /* FIRST + array count */
    MSG_SSID_L4AMSS_QDIAG +
      sizeof (msg_bld_masks_l4_amss) / sizeof (msg_bld_masks_l4_amss[0]) - 1,
    msg_bld_masks_l4_amss,
    msg_rt_masks_l4_amss
  }

#ifdef FEATURE_HIT
  ,
  {
    MSG_SSID_HIT,
    /* FIRST + array count */
    MSG_SSID_HIT+
      sizeof (msg_bld_masks_hit) / sizeof (msg_bld_masks_hit[0]) - 1,
    msg_bld_masks_hit,
    msg_rt_masks_hit
  }

#endif /* FEATURE_HIT */

  ,
  {
    MSG_SSID_UMB,
    /* FIRST + array count */
    MSG_SSID_UMB+
      sizeof (msg_bld_masks_umb) / sizeof (msg_bld_masks_umb[0]) - 1,
    msg_bld_masks_umb,
    msg_rt_masks_umb
  }

};

/* Number of SSID range entries in table */
#define MSG_MASK_TBL_CNT (sizeof (msg_mask_tbl) / sizeof (msg_mask_tbl[0]))

const uint32 msg_mask_tbl_size = MSG_MASK_TBL_CNT;

#endif         /* MSG_TBL_GEN */

#endif         /* MSGCFG_H */

