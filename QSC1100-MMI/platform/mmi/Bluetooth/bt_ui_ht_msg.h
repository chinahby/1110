#ifndef _BTUI_HT_MSG_
#define _BTUI_HT_MSG_

#include "btui_platform.h"


//////////////////////////////////////////////////////////////////////////

typedef enum
{
	BTUI_DM_MSG_TYPE = 0,
	BTUI_AG_MSG_TYPE,
	BTUI_OPS_MSG_TYPE,
	BTUI_OPC_MSG_TYPE,
	BTUI_FTS_MSG_TYPE,
	BTUI_CTP_MSG_TYPE
}BTUI_MSG_TYPE;


/* mmi message buffer functions. */
tBTUI_BTA_MSG* btui_get_msg(void);
void btui_free_msg(tBTUI_BTA_MSG* pMsg);
void btui_send_msg(BTUI_MSG_TYPE msg_type, tBTUI_BTA_MSG* pMsg);


#endif	// _BTUI_HT_MSG_