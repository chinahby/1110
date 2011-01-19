#ifndef SNDID_H
#define SNDID_H
/*===========================================================================

          S O U N D   P R E D E F I N E D  S O U  N D S  
           D E C L A R A T I O N  H E A D E R  F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the sound database.

Copyright (c) 1999 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/sndid.h#3 $ $DateTime: 2008/04/24 18:15:13 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/06    sm     Added CEPT dial tone sound.
02/03/06    sm     Added CEPT ringing sound.
01/06/06    sm     Added CEPT sounds: SND_RADIO_PATH_ACK, 
                   SND_RADIO_PATH_NOT_AVAIL, SND_CEPT_CALL_WAITING
11/12/05    sm     Added GSM sounds:  SND_SUBSCRIBER_BUSY, SND_CONGESTION, 
                   SND_ERROR_INFO, SND_NUMBER_UNOBTAINABLE, SND_AUTH_FAILURE.
02/28/01    st     Removed WLL sounds; removed unnecessary featurization.
12/04/00    st     Added SPECIAL_INFO sound.
01/27/00    st     Added SND_HFK_CALL_ORIG and FEATURE_PHONE_VR_HFK to VR-HFK
                   sounds.
08/17/99    st     Created file.

===========================================================================*/

#include "comdef.h"

/* Predefined sounds (complex tone sequences like ringing)
*/
enum sdb_sounds {          /* Description of sound                 */

  SND_FIRST_SND = -1,   /* Use for range checking last sound    */

  SND_ALERT = 0,                /* 0 -  Ringing for incoming call            */
  SND_WAKEUP,                   /* Wake-up/Power-up sound                    */
  SND_DIAL_TONE,                /* Dial tone                                 */
  SND_DTACO_ROAM_TONE,          /* DTACO roaming dial tone                   */
  SND_RING_BACK,                /* Ring-back sound                           */
  SND_INTERCEPT,                /* Send request intercepted locally          */
  SND_REORDER,                  /* System busy                               */
  SND_BUSY_ALERT,               /* Busy Signal                               */
  SND_CONFIRMATION,             /* Confirmation Tone                         */
  SND_CALL_WAITING,             /* Call Waiting                              */
  SND_ANSWER,                   /* 10 - Answer Tone                          */
  SND_OFF_HOOK,                 /* Off-Hook Warning                          */
  SND_NORMAL_ALERT,             /* "Normal" Alerting                         */
  SND_INTR_GROUP_ALERT,         /* Intergroup Alerting                       */
  SND_SPCL_ALERT,               /* Special/Priority Alerting                 */
  SND_PING_RING,                /* "Ping ring"                               */
  SND_IS54B_LONG_H_ALERT,       /* IS-54B High Long                          */
  SND_IS54B_SS_H_ALERT,         /* IS-54B High Short-short                   */
  SND_IS54B_SSL_H_ALERT,        /* IS-54B High Short-short-long              */
  SND_IS54B_SS2_H_ALERT,        /* IS-54B High Short-short-2                 */
  SND_IS54B_SLS_H_ALERT,        /* 20 - IS-54B High Short-long-short         */
  SND_IS54B_SSSS_H_ALERT,       /* IS-54B High Short-short-short-short       */
  SND_IS54B_PBX_LONG_H_ALERT,   /* IS-54B High PBX Long                      */
  SND_IS54B_PBX_SS_H_ALERT,     /* IS-54B High PBX Short-short               */
  SND_IS54B_PBX_SSL_H_ALERT,    /* IS-54B High PBX Short-short-long          */
  SND_IS54B_PBX_SLS_H_ALERT,    /* IS-54B High PBX Short-long-short          */
  SND_IS54B_PBX_SSSS_H_ALERT,   /* IS-54B High PBX Short-short-short-short   */
  SND_IS53A_PPPP_H_ALERT,       /* IS-53A High Pip-Pip-Pip-Pip Alert         */
  SND_IS54B_LONG_M_ALERT,       /* IS-54B Medium Long                        */
  SND_IS54B_SS_M_ALERT,         /* IS-54B Medium Short-short                 */
  SND_IS54B_SSL_M_ALERT,        /* 30 - IS-54B Medium Short-short-long       */
  SND_IS54B_SS2_M_ALERT,        /* IS-54B Medium Short-short-2               */
  SND_IS54B_SLS_M_ALERT,        /* IS-54B Medium Short-long-short            */
  SND_IS54B_SSSS_M_ALERT,       /* IS-54B Medium Short-short-short-short     */
  SND_IS54B_PBX_LONG_M_ALERT,   /* IS-54B Medium PBX Long                    */
  SND_IS54B_PBX_SS_M_ALERT,     /* IS-54B Medium PBX Short-short             */
  SND_IS54B_PBX_SSL_M_ALERT,    /* IS-54B Medium PBX Short-short-long        */
  SND_IS54B_PBX_SLS_M_ALERT,    /* IS-54B Medium PBX Short-long-short        */
  SND_IS54B_PBX_SSSS_M_ALERT,   /* IS-54B Medium PBX Short-short-short-short */
  SND_IS53A_PPPP_M_ALERT,       /* IS-53A Medium Pip-Pip-Pip-Pip Alert       */
  SND_IS54B_LONG_L_ALERT,       /* 40 - IS-54B Low Long                      */
  SND_IS54B_SS_L_ALERT,         /* IS-54B Low Short-short                    */
  SND_IS54B_SSL_L_ALERT,        /* IS-54B Low Short-short-long               */
  SND_IS54B_SS2_L_ALERT,        /* IS-54B Low Short-short-2                  */
  SND_IS54B_SLS_L_ALERT,        /* IS-54B Low Short-long-short               */
  SND_IS54B_SSSS_L_ALERT,       /* IS-54B Low Short-short-short-short        */
  SND_IS54B_PBX_LONG_L_ALERT,   /* IS-54B Low PBX Long                       */
  SND_IS54B_PBX_SS_L_ALERT,     /* IS-54B Low PBX Short-short                */
  SND_IS54B_PBX_SSL_L_ALERT,    /* IS-54B Low PBX Short-short-long           */
  SND_IS54B_PBX_SLS_L_ALERT,    /* IS-54B Low PBX Short-long-short           */
  SND_IS54B_PBX_SSSS_L_ALERT,   /* 50 - IS-54B Low PBX Short-short-short-shrt*/
  SND_IS53A_PPPP_L_ALERT,       /* IS-53A Low Pip-Pip-Pip-Pip Alert          */
  SND_FADE_TONE,                /* Tone to inform user of a fade             */
  SND_SVC_CHANGE,               /* Inform user of a service area change      */
  SND_HORN_ALERT,               /* Horn alert                                */
  SND_ABRV_REORDER,             /* Abbreviated System busy                   */
  SND_ABRV_INTERCEPT,           /* Abbrev'd Send request intercepted locally */
  SND_ALTERNATE_REORDER,        /* Alternate reorder                         */
  SND_MESSAGE_ALERT,            /* Message Waiting Signal                    */
  SND_ABRV_ALERT,               /* Abbreviated alert                         */
  SND_PIP_TONE,                 /* 60 - Pip Tone (Voice Mail Waiting)        */
  SND_ROAM_RING,                /* Ringing option while roaming              */
  SND_SVC_ACQ,                  /* Service acquired sound                    */
  SND_SVC_LOST,                 /* Service lost sound                        */
  SND_SVC_CHNG_MORE_PREF,       /* Change to a more preferred service sound  */
  SND_SVC_CHNG_LESS_PREF,       /* Change to a less preferred service sound  */
  SND_EXT_PWR_ON,               /* External power on sound                   */
  SND_EXT_PWR_OFF,              /* External power off sound                  */
  SND_RING_1,                   /* User selectable ring 1                    */
  SND_RING_2,                   /* User selectable ring 2                    */
  SND_RING_3,                   /* 70 - User selectable ring 3               */
  SND_RING_4,                   /* User selectable ring 4                    */
  SND_RING_5,                   /* User selectable ring 5                    */
  SND_RING_6,                   /* User selectable ring 6                    */
  SND_RING_7,                   /* User selectable ring 7                    */
  SND_RING_8,                   /* User selectable ring 8                    */
  SND_RING_9,                   /* User selectable ring 9                    */
  SND_VR_HFK_CALL_RECEIVED,     /* Call answer sound when in HFK             */
  SND_HFK_CALL_ORIG,            /* Call origination sound when in HFK        */
  SND_SPECIAL_INFO,             /* Special info sound                        */
                                /* GSM tones, defined in 3GPP 2.40           */
  SND_SUBSCRIBER_BUSY,          /* 80 - Subscriber busy sound                */
  SND_CONGESTION,               /* Congestion sound                          */
  SND_ERROR_INFO,               /* Error information sound                   */
  SND_NUMBER_UNOBTAINABLE,      /* Number unobtainable sound                 */
  SND_AUTH_FAILURE,             /* Authentication failure sound              */
  SND_RADIO_PATH_ACK,           /* Radio path acknowledgement sound          */
  SND_RADIO_PATH_NOT_AVAIL,     /* Radio path not available sound            */
  SND_CEPT_CALL_WAITING,        /* CEPT call waiting sound                   */
  SND_CEPT_RING,                /* CEPT ringing sound                        */
  SND_CEPT_DIAL_TONE,           /* CEPT dial tone                            */
  SND_LAST_SND                  /* Use for range checking last sound         */

};

#endif /* SNDID_H */
