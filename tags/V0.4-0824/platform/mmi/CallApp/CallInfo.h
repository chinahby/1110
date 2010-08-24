#ifndef __CALLINFO_H__
#define __CALLINFO_H__

#define CALL_INFO_YES 'Y'
#define CALL_INFO_NO  'N'

typedef enum {
  CALL_INFO_MIN = '0',
  CALL_INFO_DIAL,          // continue dial entering
  CALL_INFO_SEND,         // Start origination iwth number
  CALL_INFO_SEND_TO_BT,
  CALL_INFO_REDIAL,     // Start redial with number
  CALL_INFO_ANSWER,    // Answer incoming call from NIGPS

#ifdef FEATURE_UTK2
  CALL_INFO_UTK,          // ÓÉ¿¨·¢Æðºô½Ð
#endif
  CALL_INFO_IP_CALL,
#ifdef FEATURE_EDITABLE_RECORD
  CALL_INFO_TO,
  CALL_INFO_FROM,
  CALL_INFO_MISSED,
#endif
  CALL_INFO_MAX
} call_info_cat_type;

typedef struct {
    char b_closeAll;        // Close all applet when call finished
    char catgory;            // call_info_cat_type
    char dial_str[100];     // dial number
} call_start_info_type;

typedef enum {
   START_NONE,
   START_INCOMING_CALL,  // for incoming call
   START_END_CALL,            // when received END_CALL during not active
   START_WITH_ARGS,         // for dial, start call
   START_INCOMING_ANSWERED,  // NIGps answerd call, will receive ANSWER and CONNECT event
   START_DATA_CALLING,
   START_MAX
} start_call_type;

typedef enum {
   PI_ALLOWED = 0,
   PI_RESTRICTED,
   PI_UNAVAILABLE
} callPIType;

#endif // __CALLINFO_H__
