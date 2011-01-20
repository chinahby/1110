/*===========================================================================

FILE: Qcamcorder_msg.h

Description:
This file contains definitions to support reporting of messages for error reporting and debugging.

===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.


 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 10/17/05   gm     File created
 
==============================================================================*/



#ifndef QCAMCORDER_MSG_H
#define QCAMCORDER_MSG_H


#include "msg.h"

/* Replace all MSG_HIGH/LOW/MED by QCAMCORDER_MSG_HIGH/LOW/MED(msg_mask,.........)
 * The msg_mask used will depend on the context in which it
 * is called
 * */
#ifdef MSG_SSID_APPS_QCAMCORDER

/* These 5 macros will be used to replace the existing MSG_HIGH/LOW that use the legacy masks */
   #define QCAMCORDER_MSG_HIGH(msg_format,a,b,c)       MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_HIGH,msg_format,a,b,c)
   #define QCAMCORDER_MSG_LOW(msg_format,a,b,c)        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_LOW,msg_format,a,b,c)
   #define QCAMCORDER_MSG_MED(msg_format,a,b,c)        MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_MED,msg_format,a,b,c)
   #define QCAMCORDER_MSG_ERROR(msg_format,a,b,c)      MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_ERROR,msg_format,a,b,c)
   #define QCAMCORDER_MSG_FATAL(msg_format,a,b,c)      MSG_3(MSG_SSID_APPS_QCAMCORDER,MSG_LEGACY_FATAL,msg_format,a,b,c)

 /* Macros to use the User-defined masks  */
   #define QCAMCORDER_MSG_3(msg_mask,msg_format,a,b,c)              MSG_3(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c)
   #define QCAMCORDER_MSG_4(msg_mask,msg_format,a,b,c,d)            MSG_4(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d)
   #define QCAMCORDER_MSG_5(msg_mask,msg_format,a,b,c,d,e)          MSG_5(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e)
   #define QCAMCORDER_MSG_6(msg_mask,msg_format,a,b,c,d,e,f)        MSG_6(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f)
   #define QCAMCORDER_MSG_7(msg_mask,msg_format,a,b,c,d,e,f,g)      MSG_7(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f,g)
   #define QCAMCORDER_MSG_8(msg_mask,msg_format,a,b,c,d,e,f,g,h)    MSG_8(MSG_SSID_APPS_QCAMCORDER,msg_mask,msg_format,a,b,c,d,e,f,g,h)


#else

    /* If camcorder SSID is not defined */
    #define QCAMCORDER_MSG_HIGH(msg_format,a,b,c)       MSG_HIGH(msg_format,a,b,c)
    #define QCAMCORDER_MSG_LOW(msg_format,a,b,c)        MSG_LOW(msg_format,a,b,c)
    #define QCAMCORDER_MSG_MED(msg_format,a,b,c)        MSG_MED(msg_format,a,b,c)
    #define QCAMCORDER_MSG_ERROR(msg_format,a,b,c)      MSG_ERROR(msg_format,a,b,c)
    #define QCAMCORDER_MSG_FATAL(msg_format,a,b,c)      MSG_FATAL(msg_format,a,b,c)

#endif
 	

/* Define Message masks for Camcorder-App layer
 * Rename QCAMCORDER_APP% to whatever mask name you need
 * Its a 32-bit mask, only three for the App layer the rest will be for
 * services layer 
 *  */
#define QCAMCORDER_APP1     	   (MSG_MASK_5)
#define QCAMCORDER_APP2     	   (MSG_MASK_6)
#define QCAMCORDER_APP3     	   (MSG_MASK_7)

#endif
