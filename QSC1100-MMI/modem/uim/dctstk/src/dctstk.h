#ifndef DCTSTK_H
#define DCTSTK_H

/*======================================================
FILE:  dctstk.h

GENERAL DESCRIPTION:
    DCTSTK, data card thin-ui STK include file

        Copyright © 2007 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
=====================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/dctstk/rel/07H1_2/src/dctstk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/07   st      Change name for ui_handle_dctstk_cmds to handle_dctstk_cmds 
05/01/07   st      Initial Creation.
=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "tmc.h"
#include "task.h"
#include "mmgsdilib.h"
#include "gstk_exp.h"
#include "wms.h"
#include "gstk.h"

#define DCTSTK_MAX_ADDITIONAL_INFO	10

typedef enum
{
  DCTSTK_RESULT_FAIL = 0x00,
  DCTSTK_RESULT_PASS = 0x01,
  DCTSTK_RESULT_UNKNOWN = 0xFF
}
dctstk_result_enum_type;

typedef enum
{
  DCTSTK_NO_INIT = 0x00,
  DCTSTK_MMGSDI_REG_COMPLETE = 0x01,
  DCTSTK_GSTK_REG_INIT = 0x02,
  DCTSTK_INITIALIZED = 0x03,
  DCTSTK_UNKNOWN_STATE = 0xFF
}
dctstk_state_enum_type;

typedef enum
{
  DCTSTK_SEND_TR_SMS_OK = 0x01,
  DCTSTK_SEND_TR_SMS_FAIL = 0x02,
  DCTSTK_SEND_TR_MAX_ENUM = 0x7FFFFFFF
//  DCTSTK_SEND_TR_CALLSETUP_OK = 0x02,
//  DCTSTK_SEND_TR_CALLSETUP_FAIL = 0x02,
}
dctstk_cmd_enum_type;

typedef enum
{
  DCTSTK_CACHE_STATE_FREE,        // No entry at this index
  DCTSTK_CACHE_STATE_NEW,         // New entry
} DCTSTKCmdCacheState;

typedef struct {
  q_link_type           link;         /* Queue field */
  dctstk_cmd_enum_type  cmd;          /* DCTSTK_SEND_TR_SMS_OK, DCTSTK_SEND_TR_SMS_FAIL, etc. */
  uint32                dwParam;      /* additional info GSTK */
//  uint32                extraParam;   /* extra param to GSTK, not used for now */
} dctstk_cmd_type;

typedef struct
{
  DCTSTKCmdCacheState        state;
  gstk_cmd_enum_type         command_id;
  uint32                     cmd_detail_reference;
  uint8                      command_number;
  uint32                     user_data;
} DCTSTKCmdCache;


typedef struct {
	dctstk_state_enum_type	  	dctstk_state;   
	DCTSTKCmdCache            	cmdCache;
	mmgsdi_client_id_type     	dctstk_mmgsdi_client_id;
	gstk_client_id_type       	dctstk_gstk_client_id;
	wms_client_id_type        	dctstk_wms_id;         /* wms client id */
	wms_client_message_s_type  *md_psMessageToWms;
	boolean                    	term_resp_sent;        /* whether the terminal response has */
													  /* been sent or not for the last proactive command */

}
dctstk_Info_type;

/*============================================================================
 DCTSTKAdditionalInfo: 
=============================================================================*/
typedef struct {
    uint8                       		nLength;
    gstk_additional_info_code_enum_type additionalInfo[DCTSTK_MAX_ADDITIONAL_INFO];
    uint8                      			*pAdditionalInfoData;
}
DCTSTKAdditionalInfo;

extern q_type dctsk_cmd_q;


/* MACROS */

/* ===========================================================================
   MACRO:       DCTSTK_MALLOC_AND_VALIDATE
   DESCRIPTION: Allocation TMC memory according to the input size
                Check if memory allocation is successful or not
                memset the memory location to 0x00
   INPUT:       p_mem: pointer to be allocated
                size: size of the allocation
				status : DCTSTK result
  ===========================================================================*/
/*lint -emacro(506,DCTSTK_MALLOC_AND_VALIDATE) */
/*lint -emacro(774,DCTSTK_MALLOC_AND_VALIDATE) */
/*lint -emacro(732,DCTSTK_MALLOC_AND_VALIDATE) */
#define DCTSTK_MALLOC_AND_VALIDATE(p_mem, ptr_size, status)       \
  status = DCTSTK_RESULT_FAIL;                                            \
  if ( (ptr_size) > 0 ) /*lint !e774 */                                   \
  {                                                                       \
    (p_mem) = mem_malloc(&tmc_heap, (ptr_size));                          \
    if ( (p_mem) == NULL )                                                \
    {                                                                     \
	  MSG_ERROR("DCTSTK_MALLOC_AND_VALIDATE : TMC Heap space size = 0x%x not avail",(ptr_size),0,0); \
    }                                                                     \
    else                                                                  \
    {                                                                     \
      MSG_MED("Mem Malloc 0x%x, size=0x%x", (p_mem), (ptr_size), 0);      \
      memset((p_mem), 0x00, (ptr_size));                                        \
      status = DCTSTK_RESULT_PASS;                                              \
    }                                                                           \
  }

/* ===========================================================================
   MACRO:       DCTSTK_MEM_FREE
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will also assign the Pointer to NULL.
   ===========================================================================*/
/*lint -emacro(774,DCTSTK_MEM_FREE) */
#define DCTSTK_MEM_FREE(p_mem)                          \
  if ( (p_mem) == NULL )                                        \
  {                                                             \
	MSG_ERROR("DCTSTK_MEM_FREE Attempt to free NULL PTR : ",0,0,0); \
  }                                                             \
  else                                                          \
  {                                                             \
    MSG_MED("Mem Free 0x%x", (p_mem), 0, 0);                    \
    mem_free(&tmc_heap,(p_mem));                                \
    (p_mem) = NULL;                                             \
  }

/* Function prototypes */
void dctstk_init(void);

gstk_status_enum_type handle_dctstk_cmds(void);
dctstk_result_enum_type dctstk_post_cmd(dctstk_cmd_type *cmd_ptr);
dctstk_cmd_type  *dctstk_get_cmd_buf( void );

static void dctstk_MMGSDIClientIdRegCb(mmgsdi_return_enum_type status,mmgsdi_cnf_enum_type cnf,const mmgsdi_cnf_type *cnf_ptr);
static void dctstk_MMGSDIEvtCb(const mmgsdi_event_data_type *event);
static void dctstk_MMGSDIRspCb(mmgsdi_return_enum_type status,mmgsdi_cnf_enum_type cnf,const mmgsdi_cnf_type *cnf_ptr);

static void dctstkGSTKevtCb(gstk_cmd_from_card_type * req_rsp );
static boolean DCTSTK_UpdateProfileDownloadValue(void);

void dctstk_wms_callback (wms_cmd_id_e_type cmd, const void *user_data, wms_cmd_err_e_type cmd_err);
void dctstk_wmseventcb(wms_msg_event_e_type event, const wms_msg_event_info_s_type *pInfo, const boolean *shared);
static void dctstk_process_gstk_sendsmsreq (gstk_cmd_from_card_type * req_rsp);
static uint8 dctstk_ts_decode_address(const uint8 * data, wms_address_s_type * addr);
static gstk_cmd_enum_type DCTSTK_GetConfirmationType(gstk_cmd_enum_type proactive_cmd);

dctstk_result_enum_type DCTSTK_SendCommandResponse(gstk_cmd_enum_type request_type,gstk_general_result_enum_type result, DCTSTKAdditionalInfo *additional_result);

static gstk_additional_info_ptr_type *DCTSTK_AddnlInfoToGSTKAddnlInfo(DCTSTKAdditionalInfo *pIn);
static gstk_terminal_rsp_extra_param_type* DCTSTK_FillGSTKExtraParm(gstk_cmd_enum_type commandID);

#endif //DCTSTK_H

