/*===========================================================================

    CAMCORDER MESSAGE MACRO HEADER FILE

DESCRIPTION
  This header file contains Message Macros that use Camcorder SSID


Copyright (c) 2003 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/camcorder_msg.h#3 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/08   anil     RVCT compiler warning fixes
07/04/08   ps      Removed RVCT compiler warnings
03/18/07   dhaval  Removed memory leak when FEATURE_VIDEO_CLIP_INFO is 
                   disable for customer build.
03/06/07   jdas    Removed Banned API
07/14/06   jdas    Added CAMCORDER_LOG_MSG macro
10/17/05   gm      File Created

================================================================================*/

#ifndef CAMCORDER_MSG_H
#define CAMCORDER_MSG_H


#include "msg.h"

#ifdef FEATURE_VIDEO_ENCODE
void camcorder_msg_clipinfo(char * msg_format,uint32 a,uint32 b,uint32 c);
#endif //FEATURE_VIDEO_ENCODE

#ifdef FEATURE_VIDEO_CLIP_INFO
#include <stdio.h>
#include <stdlib.h>
typedef struct video_fmt_msg_list_type_t {
    char                                *msg;
    uint16                              msg_size;
    struct video_fmt_msg_list_type_t    *next;
}video_fmt_msg_list_type;

extern video_fmt_msg_list_type *msg_head;
extern video_fmt_msg_list_type *msg_curr;
extern boolean                 log_msg;

#define CAMCORDER_LOG_MSG(msg_format,a,b,c)    camcorder_msg_clipinfo(msg_format,a,b,c)
#else         
#define CAMCORDER_LOG_MSG(msg_format,a,b,c)    (void *)0
#endif

/* Replace all MSG_HIGH/LOW/MED by CAMCORDER_MSG_3(msg_mask,.........)
 * The msg_mask used will depend on the context in which it
 * is called
 * */
#ifdef MSG_SSID_APPS_QCAMCORDER

/* These 5 macros will be used to replace the existing MSG_HIGH/LOW that use the legacy masks */
    #define CAMCORDER_MSG_HIGH(msg_format,a,b,c)       MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_HIGH,msg_format,a,b,c)
    #define CAMCORDER_MSG_LOW(msg_format,a,b,c)        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_LOW,msg_format,a,b,c)
    #define CAMCORDER_MSG_MED(msg_format,a,b,c)        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_MED,msg_format,a,b,c)
    /*lint -esym(666,CAMCORDER_MSG_ERROR) suppress warning of "expression with side effects"*/
   #ifdef FEATURE_VIDEO_CLIP_INFO
   #define CAMCORDER_MSG_ERROR(msg_format,a,b,c)									 \
		   MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_ERROR,msg_format,a,b,c);			\
		   CAMCORDER_LOG_MSG(msg_format,a,b,c) 
   #else
   #define CAMCORDER_MSG_ERROR(msg_format,a,b,c)									\
		   MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_ERROR,msg_format,a,b,c)
   #endif

   /*lint -esym(666,CAMCORDER_MSG_FATAL) suppress warning of "expression with side effects"*/
   #ifdef FEATURE_VIDEO_CLIP_INFO
   #define CAMCORDER_MSG_FATAL(msg_format,a,b,c)                                    \
        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_FATAL,msg_format,a,b,c);           \
        CAMCORDER_LOG_MSG(msg_format,a,b,c)
   #else
   #define CAMCORDER_MSG_FATAL(msg_format,a,b,c)                                    \
        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_FATAL,msg_format,a,b,c)
   #endif
        

 /* Macros to use the User-defined masks  */
    #define CAMCORDER_MSG_3(msg_mask,msg_format,a,b,c)              MSG_3(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c)
    #define CAMCORDER_MSG_4(msg_mask,msg_format,a,b,c,d)            MSG_4(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d)
    #define CAMCORDER_MSG_5(msg_mask,msg_format,a,b,c,d,e)          MSG_5(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e)
    #define CAMCORDER_MSG_6(msg_mask,msg_format,a,b,c,d,e,f)        MSG_6(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f)
    #define CAMCORDER_MSG_7(msg_mask,msg_format,a,b,c,d,e,f,g)      MSG_7(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f,g)
    #define CAMCORDER_MSG_8(msg_mask,msg_format,a,b,c,d,e,f,g,h)    MSG_8(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f,g,h)


#else

    /* If camcorder SSID is not defined */
    #define CAMCORDER_MSG_HIGH(msg_format,a,b,c)       MSG_HIGH(msg_format,a,b,c)
    #define CAMCORDER_MSG_LOW(msg_format,a,b,c)        MSG_LOW(msg_format,a,b,c)
    #define CAMCORDER_MSG_MED(msg_format,a,b,c)        MSG_MED(msg_format,a,b,c)
    #define CAMCORDER_MSG_ERROR(msg_format,a,b,c)      MSG_ERROR(msg_format,a,b,c)
    #define CAMCORDER_MSG_FATAL(msg_format,a,b,c)      MSG_FATAL(msg_format,a,b,c)

#endif
 	

/* Define Message masks for Camcorder-App layer
 * Rename CAMCORDER_% to whatever mask name you need
 * Its a 32-bit mask, only three for the App layer the rest will be for services layer 
 *  */
 #define CAMCORDER_1     	   (MSG_MASK_8)
 #define CAMCORDER_2     	   (MSG_MASK_9)
 #define CAMCORDER_3     	   (MSG_MASK_10)
 #define CAMCORDER_4     	   (MSG_MASK_11)
 #define CAMCORDER_5     	   (MSG_MASK_12)
 #define CAMCORDER_6     	   (MSG_MASK_13)
 #define CAMCORDER_7     	   (MSG_MASK_14)
 #define CAMCORDER_8     	   (MSG_MASK_15)
 #define CAMCORDER_9     	   (MSG_MASK_16)
 #define CAMCORDER_10     	   (MSG_MASK_17)
 #define CAMCORDER_11     	   (MSG_MASK_18)
 #define CAMCORDER_12    	   (MSG_MASK_19)
 #define CAMCORDER_13     	   (MSG_MASK_20)
 #define CAMCORDER_14     	   (MSG_MASK_21)
 #define CAMCORDER_16     	   (MSG_MASK_22)
 #define CAMCORDER_17     	   (MSG_MASK_23)
 #define CAMCORDER_18     	   (MSG_MASK_24)
 #define CAMCORDER_19     	   (MSG_MASK_25)
 #define CAMCORDER_20     	   (MSG_MASK_26)
 #define CAMCORDER_21     	   (MSG_MASK_27)	
 #define CAMCORDER_22     	   (MSG_MASK_28)	
 #define CAMCORDER_23     	   (MSG_MASK_29)	
 #define CAMCORDER_24     	   (MSG_MASK_30)	
 #define CAMCORDER_25     	   (MSG_MASK_31)
 

#endif



