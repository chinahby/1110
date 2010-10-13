#ifndef DISP_TASK_H
#define DISP_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            D I S P L A Y  I N T E R N A L  H E A D E R   F I L E

GENERAL DESCRIPTION
  This header file contains all the internal definitions to disp task

Copyright(c) 2001 - 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/display/mdp1/disp_task/main/latest/inc/disp_task.h#4 $ $DateTime: 2009/01/12 06:15:45 $ $Author: rkashyap $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/09   rk       Corrected DISP_TASK_MIN_MEMORY_REQ calculation
11/07/07   dw       Reduce display task buffer count for memory reduction
09/05/07   spw      Change DISP_MDP_POWER_OFF_EXP_TIME to 1 sec
08/27/07   spw      Change DISP_MDP_POWER_OFF_EXP_TIME to 50ms
07/27/06   cjr      Added MDP power off timer signal
07/17/06   dw       Remove FEATURE_DISP_TASK
05/10/06   cjr      display API queue
10/13/05   cjr      Added support for MDPOP_QUEUE_EXPAND 
06/13/05   cjr      Initial creation of file.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Definitions for queue services                  */
#include "mdp_api.h"
#include "msg.h"
#include "timetick.h"
#include "task.h"
#include "mdp_buffers.h"

/*---------------------------------------------------------------------------
** DISP General Defines
---------------------------------------------------------------------------*/
/* This signal is set when the diag_rpt_timer expires.
*/
#define DISP_RPT_TIMER_SIG           0x0001

/* This signal is set when an item is put on the disp_cmd_q.
*/
#define DISP_CMD_Q_SIG               0x0002

/* This signal is set when low level driver operation is complete
*/
#define MDP_FROM_ISR_SIG             0x0004

/* This signal is set to handle MDP callback
*/
#define DISP_CMD_MDP_CB              0x0008

/* This signal is set to handle MDP callback
*/
  #define DISP_MDP_POWER_OFF_TIMER_SIG 0x0010

/* mdp power off timer expiration in msec   
*/

#ifdef FEATURE_WINCE
#error code not present
#else  // !#ifdef FEATURE_WINCE
  #define DISP_MDP_POWER_OFF_EXP_TIME  (0)
#endif // #ifdef FEATURE_WINCE


#define DISP_MSG(x,y,z,w)  {if (disp_task_dbg) MSG_HIGH(x,y,z,w);}

/* DISP Command Queue Available Packets.
*/

#if defined( FEATURE_MDP2 ) || defined( FEATURE_WINCE )
#error code not present
#else
  #ifdef FEATURE_MDP_QCOPY
  #define DISP_NUM_PACKETS 4    /* # of Free Packets Allocated  */
  #else
    #define DISP_NUM_PACKETS 2  /* Reduce buffers if QCopy not needed */
  #endif /* FEATURE_MDP_QCOPY */
#endif

#define DISP_TASK_TIMETICK_MAX_INDEX 32

// DISP_TASK_TIMETICK_MAX_INDEX defines the number of empty slot to store 
// timetick count for each update.  And if this number is less than the 
// queue size, there will be a overlap in the time tick history array.
#if (DISP_TASK_TIMETICK_MAX_INDEX <= DISP_NUM_PACKETS)
#error "DISP_TASK_TIMETICK_MAX_INDEX must be greater than DISP_NUM_PACKETS"
#endif

#define DISP_NUM_PACKETS_API  16 /* # of Free Packets for API execution */
                                 /* supports upto DISP_NUM_PACKETS_API many tasks
                                    for queueing API execution */

/* maximum disp API argument storage */
#define DISP_API_MAX_ARG 4
  
#ifdef T_MSM7500 /* 7500 SURF */
#error code not present
#else
  #define DISP_TASK_BUF_WIDTH 240
  #define DISP_TASK_BUF_HEIGHT 320

  /* Short term solution to ensure sufficient heap at boot up for display needs.
  ** Proper fix involves ensuring BREW heap is available before display is initialized. */
  #if defined(T_QSC60X5) && !defined(FEATURE_TMD_20_QVGA)
    /* QCIF+ = ((176*220*2)+ 176*2 + 4 + 36)*7 = 544824B = 533KB = 545792B */
    /*((bufferByteSize + markerByteSize + alignment offset 
       + sizeof(frame_node_type) )*Num of buffers)*/
      #define DISP_TASK_MIN_MEMORY_REQUIREMENT 545792
  #else
    /* QVGA =((240*320*2)+ 240*2 + 4+ 36)*7 = 1078840B = 1054 KB = 1079296B*/
    /*((bufferByteSize + markerByteSize + alignment offset 
       + sizeof(frame_node_type))*Num of buffers)*/
      #define DISP_TASK_MIN_MEMORY_REQUIREMENT  1079296
  #endif /* T_QSC60X5 && !FEATURE_TMD_20_QVGA*/
#endif // T_MSM7500

#ifndef FEATURE_MDP2  
  #define DISP_TASK_BUFFER_SHARING
#endif

/* This signal is for display data request
*/

typedef enum
{
  DISP_CMD_DISP_UPDATE,  /* display update operation */
  DISP_CMD_API_UPDATE,   /* synchronous API call */
  DISP_CMD_INVALID             /* Invalid command */
} disp_cmd_type;


/* Header for DISP commands
*/

typedef MDP_CB disp_cb_type;

typedef struct disp_packet_type_ disp_packet_type;

typedef struct disp_packet_type_
{
  disp_packet_type * prev;
  disp_packet_type * next;  
  disp_cmd_type      cmd;                /* Command to perform */
  disp_cb_type       cb;                 /* Callback type */
  MDPIMG             mdpImg;             /* MDP Image Descriptor */
  uint32           * orig_bmy_addr;      /* Original MDP bmy_addr */
  mdp_frame_type   * disp_frame;         /* MDP1 buffer storage */
  MDP_CWIN           real_cwin;          /* Actual MDP x-y coordinate */  
  int                qid;                /* Queue index id */  

#ifdef DISP_TASK_BUFFER_SHARING
  boolean            registered;         /* packet is owned by other module */
  boolean            done;               /* disp task are done with this packet */
#endif  
  boolean            queue_copy_done;    /* disp task copied user buffer into 
                                            queue buffer */
} disp_packet_type;                      /* disp packet data structure*/

typedef struct disp_packet_type_API_ disp_packet_type_API;

typedef struct disp_packet_type_API_
{
  disp_packet_type_API * prev;
  disp_packet_type_API * next;  
  disp_cmd_type          cmd;            /* Command to perform */
  disp_cb_type           cb;             /* Callback type */  
  uint32                 api_id;         /* which API to call? */
  uint32                 arg[DISP_API_MAX_ARG]; /* maximum arguments storage */
} disp_packet_type_API;                  /* API disp packet data structure*/


/*---------------------------------------------------------------------------
** DISP local function prototypes
**---------------------------------------------------------------------------
*/
LOCAL void disp_task_init (void);
LOCAL void disp_task_offline (void);
LOCAL void disp_task_shutdown (void);
LOCAL void disp_task(dword ignored);
LOCAL void disp_cmd_parser (void);
LOCAL disp_packet_type * disp_q_get(void);
LOCAL boolean disp_node_already_in_queue(disp_packet_type * node);
LOCAL void disp_q_put(disp_packet_type *node);
LOCAL void disp_q_put_API(disp_packet_type_API *node);
LOCAL void disp_imglist_remove(disp_packet_type *node);
LOCAL disp_packet_type * disp_imglist_get(MDPIMG *mdpImg, MDP_CWIN *old_cwin);
LOCAL disp_packet_type_API * disp_imglist_get_API(void);
LOCAL void disp_imglist_free(disp_packet_type *node);
LOCAL void disp_imglist_free_API(disp_packet_type_API *node);
LOCAL void disp_imglist_init(void);
LOCAL void disp_task_default_cb(void *arg, MDP_STATUS status);
LOCAL void disp_q_put_complete(disp_packet_type *node);
LOCAL disp_packet_type * disp_q_get_complete(void);
LOCAL void disp_task_cb_clean_up(void);

#ifdef DISP_TASK_C
LOCAL boolean disp_task_dbg = 0;
#else
extern boolean disp_task_dbg;
#endif

#ifdef FEATURE_WINCE
#error code not present
#else
  #define DISP_INTLOCK()  INTLOCK()
  #define DISP_INTFREE()  INTFREE()
#endif

#endif /* DISP_TASK_H */

