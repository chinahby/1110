/*===========================================================================
                              OEMDebug.c

  This file contains the OEM routines which provide debug support.

        Copyright © 1999-2005,2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "OEMFeatures.h"

#if defined(AEE_SIMULATOR)
#define MSG_LEVEL MSG_LVL_HIGH
#endif /* #if defined(AEE_SIMULATOR) */

#include "msg.h"

#include "OEMDebug.h"

#include "AEE_OEMEvent.h"
#include "AEEStdLib.h"


#if defined(AEE_SIMULATOR)
# include "SDKTAPI.h"
#else
# include "msm/OEMPhone.h"
# if !defined(FEATURE_MULTIMODE_ARCH) 
#  define SYS_OPRT_MODE_ONLINE CM_OPRT_MODE_ONLINE
#  define SYS_OPRT_MODE_OFFLINE CM_OPRT_MODE_OFFLINE
# endif // !defined(FEATURE_MULTIMODE_ARCH)

// event reporting headers and types
# include "event.h"

typedef PACKED struct
{
   AEECLSID cls;
   uint32   pl;
} event_brew_type;

#endif // defined(AEE_SIMULATOR)


#define DBG_NUM_STORES 16  /* sheesh! 1k of output debug string stuff */
#define DBG_FMT_SIZE   128
#define DBG_FNAME_SIZE 16

// 
// Debug Key Values
//
enum OEMDbgKeyCode {
   OEM_DBG_RESET = 1000,        // 1000 : Clear all Debugs
   OEM_DBG_END_CALL            // 1001 : End the current call
};


/*===========================================================================
  OEM Functions
===========================================================================*/

void OEMDebug_MsgPut(int nLevel, const char *cpszFile, int nLine, const char *cpszMsg)
{
#if !defined(MSG_LEGACY_ERROR)
# define msg_const_type    msg_store_type
# define fname             file_ptr
# define fmt               xx_fmt
# define msg_send(x)       msg_put((x),0,0,0)
#endif /* if !defined(MSG_LEGACY_ERROR) */

   static msg_const_type  ams[DBG_NUM_STORES] = {0};
   static char            apszFmts[DBG_NUM_STORES][DBG_FMT_SIZE] = {0};
   static char            apszFnames[DBG_NUM_STORES][DBG_FNAME_SIZE] = {0};
   static int             nIdx = -1;
   msg_const_type        *pms;


   if (-1 == nIdx) {
      msg_const_type *pmsEnd = ams + DBG_NUM_STORES;
      char *pszFmt;
      char *pszFname;
     
      ZEROAT(&apszFmts);
     
      for (pms = ams, pszFmt = apszFmts[0], pszFname = apszFnames[0];
           pms < pmsEnd; 
           pms++, pszFmt += sizeof(apszFmts[0]), 
              pszFname += sizeof(apszFnames[0])) {
         
#if defined(MSG_LEGACY_ERROR)
# if !defined(MSG_SSID_DFLT)
         pms->desc.ss_id   = MSG_SSID_LEGACY;
# else /* if !defined(MSG_SSID_DFLT) */
         pms->desc.ss_id   = MSG_SSID_DFLT;
# endif /* if !defined(MSG_SSID_DFLT) */
#endif /* if !defined(MSG_LEGACY_ERROR) */

         pms->fmt    = pszFmt;
         pms->fname  = pszFname;
         pszFname[0] = '*';
      }
   }

   // Increment the buffer index, to use the next buffer.  Hopefully, the MSG
   // subsystem is done with it by now.
   nIdx++;
   if (nIdx >= DBG_NUM_STORES) {
      nIdx = 0;
   }
   pms = &ams[nIdx];

   STRLCPY((char *)pms->fmt, cpszMsg, DBG_FMT_SIZE);

   // strip %, so dmss doesn't try to printf the 0s passed to MSG_ERROR
   {
      char *pc,c;
      for (pc = (char *)pms->fmt; 0 != (c = *pc); pc++) { 
         if (c == '%') {
            *pc = 'X';
         }
      }
   }

   switch (nLevel) {
#if defined(MSG_LEGACY_ERROR)
      case DBGPRINTF_LEVEL_LOW:
         pms->desc.ss_mask = MSG_LEGACY_LOW;
         break;
      case DBGPRINTF_LEVEL_MED:
         pms->desc.ss_mask = MSG_LEGACY_MED;
         break;
      case DBGPRINTF_LEVEL_FATAL:
         pms->desc.ss_mask = MSG_LEGACY_FATAL;
         break;
      case DBGPRINTF_LEVEL_HIGH:
         pms->desc.ss_mask = MSG_LEGACY_HIGH;
         break;
      case DBGPRINTF_LEVEL_ERROR:
      default:
         pms->desc.ss_mask = MSG_LEGACY_ERROR;
         break;
#else /* if !defined(MSG_LEGACY_ERROR) */
      case DBGPRINTF_LEVEL_LOW:
         pms->level = MSG_LVL_LOW;
         break;
      case DBGPRINTF_LEVEL_MED:
         pms->level = MSG_LVL_MED;
         break;
      case DBGPRINTF_LEVEL_FATAL:
         pms->level = MSG_LVL_FATAL;
         break;
      case DBGPRINTF_LEVEL_HIGH:
         pms->level = MSG_LVL_HIGH;
         break;
      case DBGPRINTF_LEVEL_ERROR:
      default:
         pms->level = MSG_LVL_ERROR;
         break;
#endif  /* if !defined(MSG_LEGACY_ERROR) */
   } 

#if !defined(MSG_LEGACY_ERROR)
   pms->line      = nLine;
#else /* if !defined(MSG_LEGACY_ERROR) */
   pms->desc.line = nLine;
#endif /* if !defined(MSG_LEGACY_ERROR) */

   STRLCPY((char *)pms->fname+1,cpszFile,DBG_FNAME_SIZE-1);

   msg_send(pms);

} // OEMDebug_MsgPut


void OEMDebug_LogEvent( 
  AEEEvent evt, 
  AEECLSID cls,
  uint32   pl
)
{
#if defined(AEE_SIMULATOR)
   DBGPRINTF( "DBGEvent=0x%X cls=0x%X pl=0x%X", evt, cls, pl );
#else
   event_id_enum_type event;
   event_brew_type    payload;

   // map BREW event to DMSS event
   if( evt >= EVT_USER ){
      // map user events to the undefined DMSS BREW Events
      event = (event_id_enum_type)(EVENT_BREW_USER_656 + ( evt - EVT_USER ));
   }else{
      // map specific BREW events to specific DMSS events
      switch( evt ){
         case EVT_APP_START:
            event = EVENT_BREW_APP_START;
            break;
         case EVT_APP_STOP:
            event = EVENT_BREW_APP_STOP;
            break;
         case EVT_APP_SUSPEND:
            event = EVENT_BREW_APP_PAUSE;
            break;
         case EVT_APP_RESUME:
            event = EVENT_BREW_APP_RESUME;
            break;
         case EVT_EXT_CREATE:
            event = EVENT_BREW_EXT_MODULE_START;
            break;
         case EVT_EXT_RELEASE:
            event = EVENT_BREW_EXT_MODULE_STOP;
            break;
         case EVT_ASYNC_ERROR:
            event = EVENT_BREW_ERROR;
            break;
         default:
            // event not supported, ignore event
            return;
      }
   }

   // check event bounds
   if( event < EVENT_BREW_APP_START || event > EVENT_BREW_UNDEFINED_895 ){
      // out of bounds, ignore event
      return;
   }

   // if not class id is specified use the active applets
   if( cls == 0 ){
      cls = AEE_Active();
   }

   payload.cls = cls;
   payload.pl  = pl;

   // send event
   event_report_payload( (event_id_enum_type)event, sizeof(event_brew_type), (void*)&payload );
#endif
}


/*==================================================================
Function: OEMDebug_Trigger

Description:
  This function is called by BREW when a BREW debug sequence is 
  activated, to enable the OEM to extend BREW's debug sequences.

Prototype:
   void OEMDebug_Trigger(unsigned uDebugTriggerNum);

Parameter(s):
   uDebugTriggerNum: the number typed by the user as the debug trigger

Return Value: 
   None

Comments:
   This function is called for debug triggers 1000-1999.  Triggers
   less than 1000 are reserved for the BREW core.  Triggers greater
   than 1999 are reserved for dynamic components.

Side Effects:
   None.

See Also:

==================================================================*/
void OEMDebug_Trigger(unsigned uDebugTriggerNum)
{

   switch (uDebugTriggerNum) {

      case OEM_DBG_RESET:
         break;

 case OEM_DBG_END_CALL:
      {
#if defined(AEE_SIMULATOR)
         TAPISIM_EndCall(0);
#else
         OEMPhone *pip;
         int       nErr;
         
         nErr = OEMPhone_New(&pip);
         if (SUCCESS == nErr) {
            (void)OEMPhone_EndCall(pip);
            OEMPhone_Release(pip);
         }
#endif // defined(AEE_SIMULATOR)
         break;
      }

      default:
         break;
   }
}

