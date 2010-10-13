#ifndef __OEMFMRADIO_PRIV_H__
#define __OEMFMRADIO_PRIV_H__

//#include "fmrds_api.h"
/*
 * The below work is really bad, whenever the header file changes we have
 * to change here. Prolly that's one advantage of enums where they will
 * come in as real handy. Need to monitor these values as per header files
 */

/*
 * The bad code starts here 
 */

/* these would be passed in the CallBack cancel data., and would be 
 * used when the user calls CB_Cancel function.
 */
#define IFMRADIO_CB_SETSTN_IDX   0
#define IFMRADIO_CB_GET_CHNLS    1
#define IFMRADIO_CB_SET_BAND     2

/* indicates the current number of call back functions currently 
 * being implemented 
 */
#define IFMRADIO_NR_CALLBACKS    3
#define EVT_FMRADIO_MAX  EVT_FMRADIO_SERVICE_AVIALBLE

//#define FMRDS_EV_MAX_EVENT FMRDS_EV_MAX_EVENT


#define AEEFMRadio_MSG_LOW(str,a,b,c)      if(fmrds_enable_dbgs) { DBGPRINTF_LOW(str,a,b,c);    }
#define AEEFMRadio_MSG_MEDIUM(str,a,b,c)   if(fmrds_enable_dbgs) { DBGPRINTF_MEDIUM(str,a,b,c); }
#define AEEFMRadio_MSG_HIGH(str,a,b,c)     if(fmrds_enable_dbgs) { DBGPRINTF_HIGH(str,a,b,c);   }
#define AEEFMRadio_MSG_ERROR(str,a,b,c)    if(fmrds_enable_dbgs) { DBGPRINTF_ERROR(str,a,b,c);  }
#define AEEFMRadio_MSG_FATAL(str,a,b,c)    if(fmrds_enable_dbgs) { DBGPRINTF_FATAL(str,a,b,c);  }

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

/*
 * The bad code ends here 
 */

#define IFMRADIO_US_EUROPE_BAND_MIN        87500
#define IFMRADIO_US_EUROPE_BAND_MAX        108000

#define IFMRADIO_OTHERS_BAND_MIN        87500
#define IFMRADIO_OTHERS_BAND_MAX        108000

#define IFMRADIO_JAPAN_BAND_MIN            76000
#define IFMRADIO_JAPAN_BAND_MAX            90000

#define IFMRADIO_NR_BITS_SIG_STRENGTH      4

#define IFMRADIO_MAX_SIGNAL_STRENGTH       1 << IFMRADIO_NR_BITS_SIG_STRENGTH

typedef teFmrdsEventType efmrds_event_type;
typedef tsFmrdsEvMsgType fmrds_event_msg_type;
typedef teFmrdsCmdQStatusType fmrds_cmd_q_status_type;
typedef teFmrdsSrchDirType efm_search_direction;
typedef void (FMRadioEventNotify)(void *pNotifyData,fmrds_event_msg_type *pFMRDS_Event);

#define FMRadio_CBCancel(pcb) \
   do { \
      if (0 != (pcb)->pfnCancel) { \
         (pcb)->pfnCancel(pcb); \
      } \
   } while (0)

typedef struct FMRadio_Callback FMRadio_Callback;
typedef void (FMRadioCBCancel)(FMRadio_Callback * pcb);

typedef struct FMRadio_Callback
{
   struct FMRadio_Callback     *pNext;
   void                        *pmc;
   FMRadioCBCancel             *pfnCancel;
   void                        *pCancelData;
   FMRadioEventNotify          *pfnNotify;
   void                        *pNotifyData;
   void                        *pReserved;

}FMRadio_Callback;

#define FMRadioCALLBACK_Init(pcb, pcf, pcx) {(pcb)->pfnNotify = (FMRadioEventNotify *)(pcf); \
                                    (pcb)->pNotifyData = (pcx);}


/*
================================================================================
OEMFMRadio_Register_Events

Description:
   This function can be used by other apps/extensions which are interested
   in listening to certain events from FMRadio driver

   One such example is IFMRDS, it may want to register for event notifications
   availability of RDS/RBDS data
   
Prototype:
   int OEMFMRadio_Register_Events(FMRadio_Callback   *pcb,
                                  efmrds_event_type  *pefmrds_event_type,
                                  uint32             nNumEvents);

Parameters:
   pcb                : Callback of type FMRadio_Callback, same functionality
                        as in AEECallback
   pefmrds_event_type : List of events in which users are interested to listen
   nNumEvents         : Number of events in the above list
   
Return Value:
   AEE_SUCCESS : If we are able to register for events
   EFAILED     : Otherwise

Comments:
   None

See Also:
   None
   
==============================================================================*/
int OEMFMRadio_Register_Events(FMRadio_Callback   *pcb,
                               efmrds_event_type  *pefmrds_event_type,
                               uint32             nNumEvents);





#endif /* __OEMFMRADIO_PRIV_H__ */
