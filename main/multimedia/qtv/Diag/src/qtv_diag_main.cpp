/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          QTV Diagnostic Packet Service Client Routines

General Description
  This file contains implementations for QTV Diag test commands via the 
  DMSS diagnostics packet service.

Copyright (c) 2002-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

$Header: //source/qcom/qct/multimedia/qtv/diag/main/latest/src/qtv_diag_main.cpp#16 $
$DateTime: 2010/06/10 02:47:50 $
$Change: 1329400 $
                           Edit History

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/04   anandr Initial Revision
===========================================================================*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"
#include "qtv_msg.h"
#include "vdecoder_types.h"

#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE

#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "oscl_file_io.h"
#include "qtvsystem.h"
#include "qcplayer_misc.h"
#include "qtv_diag_main.h"
#include "qtv_diag_display.h"
#include "qtv_msg.h"
#include "qtv_diag_vdec.h"

// for string operations
#include "qcplayer_oscl_utils.h"

#include "mp4buf.h"
// For AllocateOutputBuffers()                          
#include "OEMMediaMPEG4.h"

extern "C" {
#include "mp4dec_api.h"
#include "event.h"      
#include "assert.h"     
}
#include "qtv_event.h"
#include "QtvPlayer.h"

#ifdef T_MSM7500
#error code not present
#endif

#ifndef FEATURE_QTV_MDP
#include "OEMHeap.h"
#endif

#ifdef QTV_DIAG_REPLAY
#include "FIFOByteBuffer.h"
#include "zrex.h"
#endif
  


/***************************QTVDIAG Task specific defs***********/
/***************************QTVDIAG Task specific defs***********/

/***************************************************************
***********QTVDIAG TASK AND PARAMETER DEFINITIONS************
***************************************************************/

/* QTVDiag task related declarations
** REX priority for the qtvdiag_task Task 
** setting this to same priority as FTM ...
** need to revisit 
*/

static void qtvdiag_task (dword dummy);

static void qtvdiag_handle_cmds(void);

#define QTVDIAG_PRI        TASK_PRIORITY(UI_PRI_ORDER)
#ifdef QTV_DIAG_REPLAY
#define QTVDIAG_REPLAY_PRI TASK_PRIORITY(UI_PRI_ORDER+1)
#endif
/* Size (in units of rex_stack_word_type) of PDSM stack */
#define QTVDIAG_STACK_SIZ   (8192/sizeof(rex_stack_word_type))

/* Set to handle all signals. */
#define QTVDIAG_HANDLE_ALWAYS_SIGS 0xBFFFFFFF

#ifdef QTV_DIAG_REPLAY
/* Size of API buffer to store APIs to be replayed. */
/* NOTE: TBD: Size needs to be tuned */
#define QTVDIAG_API_BUFFER_SIZE_IN_BYTES 4096
#endif

#define QTVDIAG_REPLAY_TIME_RESOLUTION   10

static rex_stack_word_type   qtvdiag_stack[QTVDIAG_STACK_SIZ];
static rex_tcb_type          qtvdiag_tcb;

// DIAG task timeout value 
#define QTVDIAG_RESPONSE_TIMEOUT_MSEC    5000

/* qtvdiag signal definition */
#define QTVDIAG_CMD_SIG          0x00000001
#define QTVDIAG_EXIT_SIG         0x00000002

#define QTV_MAX_FILE_SIZE                              5 * 1048576 // 5Mb
#define QTV_HTTP_SIMULATOR_OFFSET_UPDATE_TIME          1000 //in msec

#define QTV_DIAG_QVGA_SIZE                    115200 /* QVGA is 320 x 240 x 1.5 bytes/pixel */

/*Push or Pull interface identification macro*/
#define	QTV_DIAG_PUSH_INTERFACE        0x01
#define	QTV_DIAG_PULL_INTERFACE         0x02

#define  QTV_DIAG_TASK_NAME			 	"QTVDiag"

/* Sigs to wait on should be an or of all the signals QTVDiag handles */
static rex_sigs_type my_sigs = QTVDIAG_CMD_SIG | QTVDIAG_EXIT_SIG;

/* This is a structure which contains signal type that is handled by 
** qtvdiag task and the corresponding signal handler function to be
** called if the signal is set
*/
typedef struct
{
  rex_sigs_type sig;
  void    (*sig_handler_fnc)(void);
}qtvdiag_sig_table_type;  

/* Table used by the qtvdiag task to route signals to the appropriate
** signal handler
*/
static qtvdiag_sig_table_type qtvdiag_sig_handler_table[] =
{
  { QTVDIAG_CMD_SIG, qtvdiag_handle_cmds }
};

/* Cmd interface for qtvdiag */
/* Q used to pass commands from the diag task to the qtvdiag task
 */
static q_type  qtvdiag_cmd_q;

/* Flag to indicate whether to log audio data buffer from QTV to CMX */
bool audioBuf_log_flag = false;
/*used to determine if the task is already running*/
static boolean qtvdiag_task_is_running = FALSE;
static uint32 diag_pkt_seq_number = 0;
static unsigned char* video_buffer_ptr[QtvPlayer::QTV_PRIORITY_COUNT] = { NULL, NULL, NULL, NULL};
// Array where the de fragmented data is stored
static char api_arr[TEMP_BUFFER_MAX_SIZE];

//Keeps track of previous fragment descriptor
static uint8  prev_fragment_desc = 0;

//Keeps track of total payload in the fragments received
static uint16 valid_api_payload  = 0;

//Variables for Http simulator, Pull interface
  struct PushPullInterfaceSimulationT
  {
    uint32 nInterfaceID;
    uint32 buffersize;
    uint32 HTTPBufferOffset;
    QtvPlayer::InstanceHandleT  pHandle;
    rex_timer_type* pTimer;
  };

PushPullInterfaceSimulationT PushPullDataBuffer[QtvPlayer::QTV_PRIORITY_COUNT] = 
  {
    {0, 0, 0, NULL, NULL},
    {0, 0, 0, NULL, NULL},
    {0, 0, 0, NULL, NULL},
    {0, 0, 0, NULL, NULL}
  };

struct HttpSimulatorBufferOffsetData
{
  unsigned long dnldDataRate;
  uint32 priority;
};

HttpSimulatorBufferOffsetData bufferOffsetData[QtvPlayer::QTV_PRIORITY_COUNT] = 
{{0, 0}, {0, 0}, {0, 0}, {0, 0}};

//DIAG and QtvDIAG thread sync events
static QCConditionType QtvDiagResponseSync;
QtvDiagMessageStructT *pMsgToQtvDiag = NULL;

/* Qtv Diag App Pointer, to store application specific any data */
QtvDiagAppT *pQtvDiagApp = NULL;

#ifdef FEATURE_QTVDIAG_REPLAY 
/* Qtv API Replay Diag App Pointer */
QtvReplayDiagAppT *pQtvReplayDiagApp = NULL;
#endif /*FEATURE_QTVDIAG_REPLAY*/

/***************************end QTVDIAG Task specific defs***********/

/***** Forward declaration of command handlers used by diag *****/
/***** Forward declaration of command handlers used by diag *****/
PACKED void * qtvdiag_handle_api_cmd( PACKED void *, word );
PACKED void * qtvdiag_start_task( PACKED void *, word );
PACKED void* start_qtvdiag_framework_cmd_handler( PACKED void * , word);
PACKED void * qtvdiag_end_task( PACKED void *, word );
PACKED void * stop_qtvdiag_framework_cmd_handler ( PACKED void * , word );
PACKED void * qtvdiag_test_api_cmd_handler( PACKED void *request_packet_ptr,word);
#ifdef QTV_DIAG_REPLAY
PACKED void * qtvdiag_replay_api_cmd_handler( PACKED void *request_packet_ptr,word);
#endif
PACKED void * qtvdiag_register_for_callback( PACKED void *, word );
PACKED void * qtvdiag_use_heap_memory( PACKED void *, word );
PACKED void * qtvdiag_handle_get_info_cmd( PACKED void *, word );
/***** END Forward declaration of command handlers used by diag *****/
/***** END Forward declaration of command handlers used by diag *****/

static void QtvDiag_CommandComplete();
static void QtvDiag_End();


/* We can't use ZUtils's clock function as ZUtils::Init has to be called 
before any of its functions can be used. But this will lead to duplicate 
initialization of ZUtils when QtvPlayer::Initalize is invoked by Replay*/
unsigned long SystemTimeInMsec()
{
  static zrex_time_offset_type systemTimeZero = {0,0};
  zrex_time_offset_type timeNow;
  unsigned long msec;
  INTLOCK();
  zrex_get_current_time_offset(&timeNow);

  msec = 1000*(timeNow.seconds - systemTimeZero.seconds) + (timeNow.milliseconds - systemTimeZero.milliseconds);

  INTFREE();
  return msec;
}

#ifdef QTV_DIAG_REPLAY
/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  Replay

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class Replay//:public FIFOByteBuffer//,public ZUtils
{
public:
  Replay();
  // Replay();
  ~Replay();
  // Start: initializes any internal data structures needed and starts the PARROT thread
  // This should be called in response to the START_TASK message in case replay mode is selected.
  bool Start();

  // This is called after defragmented API(s) are extracted from the PARROT_API diag packet
  bool QueueApi(uint8 * packedApiPacket,uint32 LengthinBytes);
  //This fetches the current free space in AMSS buffer to accommodate apis
  uint32 FreeAmssBufSize();
  // Stop: issues an event to kill the PARROT thread after all APIs in the buffer are consumed.
  // This should be called in response to the ZEROCOUNT (or STOP) message in case replay mode was selected in the START message
  // Sets the StopFlag to true. WARNING: TBD: TODO to be implemented)
  bool Stop();
  // Different states of PARROT Thread

private:
  // Current state of Replay object
  typedef enum state_e_type
  {
    INIT=0,
    WAITING,
    PROCESSING,
    TERMINATING
  };
  // Thread Function
  void ReplayThreadFunc();
  static void ThreadFuncWrapper(unsigned long arg);    
  uint32 TimeDeltaToWait(uint32 timeDelta);
  bool ExecuteApi(char * packedApi, uint32 timeDelta, uint32 APIsize);
  void Clear();

  bool StopFlag;
  state_e_type State;
  uint32 LastExpectedExecTime;
  bool FirstApi;
  FIFOByteBuffer APIQueue;
};

Replay::Replay()
:APIQueue(QTVDIAG_API_BUFFER_SIZE_IN_BYTES) 
{
  Clear();
}
/*===========================================================================

FUNCTION Replay::Clear()

DESCRIPTION
   Resets the state to INIT and should empty the APIQueue

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void Replay::Clear()
{
  // WARNING: TODO: Clear the APIQueue of all contents
  StopFlag = false;
  LastExpectedExecTime = 0;
  State = INIT;
  FirstApi = false;
}

Replay::~Replay(){}

//Flag indicating whether to run API testing or replay
static bool QtvDiag_InReplaySession = false; 

//creation of Replay class object
static Replay * QtvDiag_pReplayObj = 0;

inline bool Replay::QueueApi(uint8 * packedApiPacket, uint32 lengthInBytes)
{
  if( (WAITING == State) || (PROCESSING == State) )
  {
    return APIQueue.AddToBack(packedApiPacket, lengthInBytes);
  }

  return false;
}
inline uint32 Replay::FreeAmssBufSize()
{
  return APIQueue.GetFreeSize();
}
/*===========================================================================

FUNCTION TimeDeltaToWait

DESCRIPTION
   It will calculate the exact time to execte the api using target clock function

DEPENDENCIES
  

RETURN VALUE
  integer value 

SIDE EFFECTS
  None.

===========================================================================*/

uint32 Replay::TimeDeltaToWait(uint32 timeDelta)
{
  uint32 currTime = SystemTimeInMsec();
  // No need to worry about circular subtraction here as 32-bit unsigned
  // subtraction is circular by nature.
  uint32 timeElapsed = currTime - (uint32)LastExpectedExecTime;
  return (timeElapsed > timeDelta) ? 0 : timeDelta - timeElapsed;
}

/*===========================================================================

FUNCTION Replay::ExecuteApi

DESCRIPTION
   It will invode the qtvdiag_exec_api fucntion with the api info

DEPENDENCIES
  

RETURN VALUE
  true or false value 

SIDE EFFECTS
  None.

===========================================================================*/
bool Replay::ExecuteApi(char * packedApi, uint32 timeDelta, uint32 APIsize)
{
  if(FirstApi)
  {
    LastExpectedExecTime = SystemTimeInMsec();
    FirstApi = false;
  }
  else
  {
    LastExpectedExecTime += timeDelta;
  }
  
  return qtvdiag_exec_api(packedApi,NULL, APIsize);
}

/*===========================================================================

FUNCTION ReplayThreadFunc

DESCRIPTION
   This is the thread(PARROT) function definition

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void Replay::ReplayThreadFunc()
{
  uint32 headerSize = getAPIheaderSize();
  uint32 timeDelta;
  uint32 totAPIsize;
  //Upper Threshold and lower threshold values of Main Buffer
  uint32 UpperThreshold=500,LowerThreshold=200;

  // start off with a sleep time of 1 milliseconds
  const unsigned int sleepTime = 1;
  bool quit = false;
  bool exec_api_return = false;

//  char  data[250];
  char* data;
  //Initializing the target clock
  //Execution of different states of PARROT Thread
  while (!quit)
  {
    switch (State)
    {
    case INIT:
      State = WAITING;
      break;
    case WAITING:
      //check if stop condition has happened (i.e. zerocount)
      if (StopFlag)
      {
        State = TERMINATING;
        break;
      }
      //check for Upper Threshold condition
      if ((APIQueue.GetMaxSize() - APIQueue.GetFreeSize())  > UpperThreshold)
      {
        State = PROCESSING;
        break;
      }
      
      zrex_sleep(1);
      break;
    case PROCESSING:
      if (StopFlag)
      {
        State = TERMINATING;
        break;
      }
      else if(APIQueue.GetMaxSize() == APIQueue.GetFreeSize())
      {
        State = WAITING;
        break;
      }

      data = (char*)QTV_Malloc(1000);
      if(data)
      {
        //Accessing the data from MainBuffer without updating Header
        if (APIQueue.PeekAtFront((byte *)data,headerSize))
        {
          //parse the Header to get Time delta and totApiSize 
          if (parseHeader(data, &timeDelta, &totAPIsize))
          {
  	      //Accessing the data from MainBuffer with updating Header
  	      APIQueue.RemoveFromFront((byte*)data, totAPIsize);
	      //sleep for timeDelta millisec
	      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Time delta is %d",timeDelta );
              zrex_sleep(timeDelta);
	      //Executes the api
	      exec_api_return = ExecuteApi(data,timeDelta,totAPIsize);
	      QTV_Free(data);
          }
        }
        else
        {
          quit = true;
          break;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc failed" );
      }
      // Checking weather the occupied buffer space is lower than LowerThreshold
      if ((APIQueue.GetMaxSize() - APIQueue.GetFreeSize()) < LowerThreshold)
      {
        // WARNING: TBD: TODO: send DATA_REQ event
          event_report_payload(EVENT_QTV_DATA_REQ,(uint8)0,(void*)NULL);
      }
      break;
    case TERMINATING:
      //Loop run till execution of the remaining api's in the MainBuffer
      while ((APIQueue.GetMaxSize() - APIQueue.GetFreeSize()) > 0)
      {
        data = (char*)QTV_Malloc(1000);
        if(data)
        {
          if (APIQueue.PeekAtFront((byte*)data,headerSize))
          {
            if (parseHeader(data, &timeDelta, &totAPIsize))
            {
              timeDelta = TimeDeltaToWait(timeDelta);
	        APIQueue.RemoveFromFront((byte*)data, totAPIsize);
	        zrex_sleep(timeDelta);
	        exec_api_return = ExecuteApi(data,timeDelta,totAPIsize);
	        QTV_Free(data);
            }
          }
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed" );
        }
      }
      quit = true;
      Clear();
      break;
    default:
      ASSERT(0);
    }
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Replay task exiting...+++++++++++++++++++++",0);
}

/*===========================================================================

FUNCTION 
   Replay::Start

DESCRIPTION
   It will start the Thread 

DEPENDENCIES
 zrex.h 

RETURN VALUE
  True if thread created successfully otherwise  false 

SIDE EFFECTS
  None.

===========================================================================*/
bool Replay::Start()
{
  if(INIT != State)
  {
    return false;
  }
  
  zrex_handle_type threadHandle = 
  zrex_create_thread(
                     QTVDIAG_STACK_SIZ,
                     /* Requested stack size
                     */
                     QTVDIAG_REPLAY_PRI,
                     /* Requested priority
                     */
                     (zrex_task_func_type)ThreadFuncWrapper,
                     /* Thread function
                     */
                     (unsigned long)this,
                     /* Parameter to pass to thread function when it is executed
                     */
                     "PARROT",
                     /* ASCII string name for the thread, can be NULL
                     */
                     0
                     /* Indicated whether or not the thread should be started
                     ** in the suspended state.  When TRUE, the thread will be
                     ** created but only scheduable after a call to zrex_resume_thread,
                     ** FALSE indicates the thread is immediately scheduable after
                     ** creation
                     */
                    );
  if(NULL == threadHandle)
  {
    return false;           
  }

  return true;
}

bool Replay::Stop()
{
  if((WAITING==State) || (PROCESSING==State))
  {
    StopFlag = true;
  }

  return false;
}
/*===========================================================================

FUNCTION 
   Replay::ThreadFuncWrapper

DESCRIPTION
   It is thread wrapper function 

DEPENDENCIES
 None 

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void Replay::ThreadFuncWrapper(unsigned long arg)
{
  ASSERT(arg);
  ((Replay*)arg)->ReplayThreadFunc();
} 

#endif

/*===========================================================================

FUNCTION qtvdiag_post_cmd

DESCRIPTION
   helper function used by the qtv callback to post a command onto the qtvdiag
   thread so that the command can be executed in the qtvdiag thread context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  ends up waking up the QTVDIAG task
===========================================================================*/
void qtvdiag_post_cmd( qtvdiag_cmd_s_type *cmd_ptr )
{
  (void)q_link( cmd_ptr, &(cmd_ptr->link) );

  if ( cmd_ptr != NULL )
  {
    /* handled by task later */
    q_put( &qtvdiag_cmd_q, &(cmd_ptr->link) );

    /* set the appropirate signal, Q the appropriate cmd and return */
    (void)rex_set_sigs( &qtvdiag_tcb, QTVDIAG_CMD_SIG );
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Setting QTVDIAG_CMD_SIG");
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Out of Buffers");
  }

}

/*===========================================================================

FUNCTION qtvdiag_handle_play_frame

DESCRIPTION
   This function handles the play frame command used to display the
   decoded frame. It makes use of the display APIs to actually render
   the frames. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Plays a frame that was decoded and then releases it so that the DSP can
  re-use the buffer.

===========================================================================*/
void qtvdiag_handle_play_frame( QtvPlayer::InstanceHandleT handle )
{
  QtvPlayer::FrameInfoT  frameInfo;
  QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;
  
  /* Ask PV for the frame information */
  nReturn = QtvPlayer::GetFrameInfo(frameInfo, handle);

  if( nReturn != QtvPlayer::QTV_RETURN_OK ||
#ifdef FEATURE_QTV_MDP
      frameInfo.YUVBuffer == NULL )
#else
      frameInfo.RGBBuffer == NULL )
#endif /* FEATURE_QTV_MDP */

  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Unable to get frame");
    return;
  }

  /* Display the frame centered on the screen */
  qtvdiag_display_frame (
#ifdef FEATURE_QTV_MDP
                          (unsigned short *) frameInfo.YUVBuffer,
#else
                          (unsigned short *) frameInfo.RGBBuffer,
#endif /* FEATURE_QTV_MDP */
                          (uint16)frameInfo.Height, (uint16)frameInfo.Width );

  /* Tell QDSP that the frame buffer has been rendered */

  if(pQtvDiagApp)  	
  {
    /* Store the frame info data in QtvDiagApp buffer*/
       pQtvDiagApp->nFrameInfoData.RGBBuffer  = frameInfo.RGBBuffer;
	pQtvDiagApp->nFrameInfoData.YUVBuffer  = frameInfo.YUVBuffer;
	pQtvDiagApp->nFrameInfoData.Height       =  frameInfo.Height;
	pQtvDiagApp->nFrameInfoData.Width        = frameInfo.Width;
	pQtvDiagApp->nFrameInfoData.Timestamp = frameInfo.Timestamp;
	pQtvDiagApp->nFrameInfoData.NumConcealedMB = frameInfo.NumConcealedMB;
	pQtvDiagApp->nFrameInfoData.bI_VOP      = frameInfo.bI_VOP;	  
  }
  
  QtvPlayer::ReleaseCurrentVideoFrameBuffer(frameInfo.RGBBuffer, handle);
}

/*===========================================================================

FUNCTION qtvdiag_callbackFn

DESCRIPTION
   This is the callback function used to register with QTV to get events back 
   for API executed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern "C" void qtvdiag_callbackFn( 
                                   QtvPlayer::PlayerStatusT status,
                                   void *pData,
                                   QtvPlayer::PlayerStateRecordT state
                                   )
{
  qtvdiag_cmd_s_type *cmd_ptr = NULL;
  qtv_status_diag_event_payload_type qtv_status_diag_event_payload;
  qtv_status_diag_event_payload.handle = NULL;
  qtv_status_diag_event_payload.pClientData = NULL;
  bool sendevent = false;

  if(pData)
  {
    qtv_status_diag_event_payload.pClientData = *((uint32 *)pData);
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QTV Engine Callback");
  QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Status %d, pData %d PlayPos", status, pData, state.playbackMsec );

  QtvDiagInfoData::SendAll((int) status);

  switch (status)
  {
  case QtvPlayer::QTV_PLAYER_STATUS_DECODE_FRAME:

#ifdef FEATURE_QTVDIAG_REPLAY
    if( !pQtvReplayDiagApp || !(pQtvReplayDiagApp->bCheckAPIEventSensitivity) )
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY */ 
    {
      /* Queue a callback to the diag thread to render the decoded frame 
      */
      cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));

      if( cmd_ptr != NULL )
      {
        memset(cmd_ptr,0,sizeof(qtvdiag_cmd_s_type));
        cmd_ptr->cmd_id = QTVDIAG_CMD_RENDER_FRAME;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
      }
    }
    break;

  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_COMPLETE:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED:
  case QtvPlayer::QTV_PLAYER_STATUS_CONNECTING:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY:
  case QtvPlayer::QTV_PLAYER_STATUS_OPENING:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYING:
  case QtvPlayer::QTV_PLAYER_STATUS_PAUSED:
  case QtvPlayer::QTV_PLAYER_STATUS_REPOSITIONING:
  case QtvPlayer::QTV_PLAYER_STATUS_IDLE:
    /* The PV code never sends this error! */
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_SCALING_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_TEXT_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_STOPPED:
  case QtvPlayer::PV_PLAYER_STATUS_DOWNLOAD_TEMPFILE_ERROR:
  case QtvPlayer::QTV_HTTP_PARSER_STATUS_READY:
  case QtvPlayer::QTV_HTTP_PARSER_STATUS_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED:
    // Required for testing In-Call Qtv
  case QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED:
  case QtvPlayer::QTV_PLAYER_STATUS_SUSPEND_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_READY:
  case QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED:
  case QtvPlayer::QTV_PLAYER_STATUS_TRACK_LIST_UPDATE:
  case QtvPlayer::QTV_PLAYER_STATUS_END_OF_TSB:
  case QtvPlayer::QTV_PLAYER_STATUS_TSB_DATA_LOSS:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X:
  case QtvPlayer::QTV_PLAYER_STATUS_TRACK_STATE_CHANGE:
  case QtvPlayer::QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY:  	
  case QtvPlayer::QTV_PLAYER_STATUS_PSEUDO_PAUSE:
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QTV_PLAYER_STATUS: %d", 
		  status);
	  sendevent = true;
	  break;

  case QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW:
    /* Time shift buffer overflow */
    if(pQtvDiagApp->BCastResumeModeFromPause)
    {
      /* Queue a callback to the diag thread to handle play clip */
      cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));

      if( cmd_ptr != NULL )
      {
        memset(cmd_ptr,0,sizeof(qtvdiag_cmd_s_type));
        cmd_ptr->cmd_id = QTVDIAG_CMD_HANDLE_PLAY;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
      }
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW");
    sendevent = true;
    break;

  case QtvPlayer::QTV_PLAYER_STREAM_AUTH_REQUIRED:
  case QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE:
  case QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED:
  case QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE:
  case QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED:
  case QtvPlayer::QTV_COMMAND_PLAY_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_STOP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_STOP_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_FAILED:
  case QtvPlayer::QTV_COMMAND_NEXT_FRAME_COMPLETE:
  case QtvPlayer::QTV_COMMAND_NEXT_FRAME_FAILED:
  case QtvPlayer::QTV_COMMAND_PREV_FRAME_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PREV_FRAME_FAILED:
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_COMPLETE:
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_FAILED:
  case QtvPlayer::QTV_PSEUDO_STREAM_ALREADY_ACTIVE:
  case QtvPlayer::QTV_COMMAND_ROTATE_COMPLETE:
  case QtvPlayer::QTV_COMMAND_ROTATE_FAILED:
  case QtvPlayer::QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED:
  case QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_TEXT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_TEXT_FAILED:
  case QtvPlayer::QTV_COMMAND_RECORD_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RECORD_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_RECORD_STOP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RECORD_STOP_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_VIDEO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_AUDIO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_AUDIO_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_TEXT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_TEXT_FAILED:
  case QtvPlayer::QTV_COMMAND_SKIP_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_SKIP_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_PARSE_FRAGMENT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PARSE_FRAGMENT_FAILED:
  case QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_COMPLETE:
  case QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_FAILED:
  case QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_FAILED:
  case QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_FAILED:
  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE:
  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_FAILED:
  case QtvPlayer::QTV_STREAM_UNSUPPORTED_MEDIA_TYPE:
  case QtvPlayer::QTV_STREAM_RATE_ADAPTATION_SUPPORTED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_DISALLOWED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FILE_ALREADY_EXISTS:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_COMPLETE:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_NOT_ENOUGH_SPACE:
  case QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_FAILED:
  case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_FAILED:
  case QtvPlayer::QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE:
  case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE:
  case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_ALL_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED:
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
		  "QTV_PLAYER_COMMAND_STATUS: %d", status);
    sendevent = true;
    break;

  default:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Unhandled Status");
    break;
  }

  if(sendevent == true)
  {
    qtv_status_diag_event_payload.qtv_status = status;
    event_report_payload(EVENT_QTV_DIAG_PLAYER_STATUS, sizeof(qtv_status_diag_event_payload_type), &qtv_status_diag_event_payload);
    sendevent = false;
  }
#ifdef FEATURE_QTVDIAG_REPLAY
  if(pQtvReplayDiagApp && pQtvReplayDiagApp->bCheckAPIEventSensitivity)
  {
    /* Qtv Replay Diag */
    if((pQtvReplayDiagApp->currentAPICmdInfo.nEventId == status))
    {
      if(pQtvReplayDiagApp->bIsCurrentAPIInfoValid && pQtvReplayDiagApp->nState == QTV_DIAG_REPLAY_WAIT_ON_EVENT )
      {
        /* wait for the rest of the time and execute the API */
        pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_WAIT_ON_TIME_DELTA;
        QtvDiagReplayPostCmd(QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API);
      }
    }
  }
  else
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY */
  {
    if( cmd_ptr != NULL )
    {
      /* if a command was generated then Q and wait for qtvdiag
      ** task to process it
      */
      qtvdiag_post_cmd( cmd_ptr );
    }
  }

}//lint !e1746

/*===========================================================================

FUNCTION qtvdiag_callbackFunc

DESCRIPTION
   This is the callback function used to register with QTV to get events back 
   for API executed. Same as above but this is used in case of multiple instances.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern "C" void qtvdiag_callbackFunc( 
                                     QtvPlayer::PlayerStatusT status,
                                     void *pClientData,
                                     QtvPlayer::PlayerStateRecordT state,
                                     QtvPlayer::InstanceHandleT handle,
                                     void *pUserData
                                     )
{
  qtvdiag_cmd_s_type *cmd_ptr = NULL;
  qtv_status_diag_event_payload_type qtv_status_diag_event_payload;
  qtv_status_diag_event_payload.pClientData = NULL;
  bool sendevent = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QTV Engine Callback");
  QTV_MSG_PRIO5(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Status %d, pData %d, PlayPos %d, Handle %d, UserData %d", status, pClientData, state.playbackMsec, handle, pUserData);

  QtvDiagInfoData::SendAll((int) status);

  switch (status)
  {
  case QtvPlayer::QTV_PLAYER_STATUS_DECODE_FRAME:
    /* Queue a callback to the diag thread to render the decoded frame 
    */
    cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));
    if( cmd_ptr != NULL )
    {
      memset(cmd_ptr,0,sizeof(qtvdiag_cmd_s_type));
      cmd_ptr->cmd_id = QTVDIAG_CMD_RENDER_FRAME;
      cmd_ptr->cmd_data= (uint32)handle;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
    }
    break;

  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_COMPLETE:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED:
  case QtvPlayer::QTV_PLAYER_STATUS_CONNECTING:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY:
  case QtvPlayer::QTV_PLAYER_STATUS_OPENING:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYING:
  case QtvPlayer::QTV_PLAYER_STATUS_PAUSED:
  case QtvPlayer::QTV_PLAYER_STATUS_REPOSITIONING:
  case QtvPlayer::QTV_PLAYER_STATUS_IDLE:
    /* The PV code never sends this error! */
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_SCALING_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_AUDIO_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_TEXT_MEDIA_END:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_STOPPED:
  case QtvPlayer::PV_PLAYER_STATUS_DOWNLOAD_TEMPFILE_ERROR:
  case QtvPlayer::QTV_HTTP_PARSER_STATUS_READY:
  case QtvPlayer::QTV_HTTP_PARSER_STATUS_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED:
    // Required for testing In-Call Qtv
  case QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED:
  case QtvPlayer::QTV_PLAYER_STATUS_SUSPEND_FAILED:
  case QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_READY:
  case QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED:
  case QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED:
  case QtvPlayer::QTV_PLAYER_STATUS_TRACK_LIST_UPDATE:
  case QtvPlayer::QTV_PLAYER_STATUS_END_OF_TSB:
  case QtvPlayer::QTV_PLAYER_STATUS_TSB_DATA_LOSS:
  case QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X:
  case QtvPlayer::QTV_PLAYER_STATUS_TRACK_STATE_CHANGE:
  case QtvPlayer::QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED:
  case QtvPlayer::QTV_PLAYER_STATUS_NEXT_GENERIC_TEXT_READY:
  case QtvPlayer::QTV_PLAYER_STATUS_PSEUDO_PAUSE:
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QTV_PLAYER_STATUS: %d", 
		  status);
    sendevent = true;
    break;
  case QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW:
    /* Time shift buffer overflow */
    if(pQtvDiagApp->BCastResumeModeFromPause)
    {
      /* Queue a callback to the diag thread to handle play clip */
      cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));
      if( cmd_ptr != NULL )
      {
        memset(cmd_ptr,0,sizeof(qtvdiag_cmd_s_type));
        cmd_ptr->cmd_id = QTVDIAG_CMD_HANDLE_PLAY;
        cmd_ptr->cmd_data= (uint32)handle;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
      }
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW");
    sendevent = true;
    break;

  case QtvPlayer::QTV_PLAYER_STREAM_AUTH_REQUIRED:
  case QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE:
  case QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED:
  case QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE:
  case QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED:
  case QtvPlayer::QTV_COMMAND_PLAY_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_STOP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_STOP_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_FAILED:
  case QtvPlayer::QTV_COMMAND_NEXT_FRAME_COMPLETE:
  case QtvPlayer::QTV_COMMAND_NEXT_FRAME_FAILED:
  case QtvPlayer::QTV_COMMAND_PREV_FRAME_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PREV_FRAME_FAILED:
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_COMPLETE:
  case QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_FAILED:
  case QtvPlayer::QTV_PSEUDO_STREAM_ALREADY_ACTIVE:
  case QtvPlayer::QTV_COMMAND_ROTATE_COMPLETE:
  case QtvPlayer::QTV_COMMAND_ROTATE_FAILED:
  case QtvPlayer::QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED:
  case QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_FAILED:
  case QtvPlayer::QTV_COMMAND_PAUSE_TEXT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PAUSE_TEXT_FAILED:
  case QtvPlayer::QTV_COMMAND_RECORD_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RECORD_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_RECORD_STOP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RECORD_STOP_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_VIDEO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_AUDIO_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_AUDIO_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_TEXT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_TEXT_FAILED:
  case QtvPlayer::QTV_COMMAND_SKIP_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_SKIP_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_PARSE_FRAGMENT_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PARSE_FRAGMENT_FAILED:
  case QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_COMPLETE:
  case QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_FAILED:
  case QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_COMPLETE:
  case QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_FAILED:
  case QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_FAILED:
  case QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_FAILED:
  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE:
  case QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_FAILED:
  case QtvPlayer::QTV_STREAM_UNSUPPORTED_MEDIA_TYPE:
  case QtvPlayer::QTV_STREAM_RATE_ADAPTATION_SUPPORTED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_DISALLOWED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FILE_ALREADY_EXISTS:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_COMPLETE:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED:
  case QtvPlayer::QTV_BUFFER_TO_FILE_COPY_NOT_ENOUGH_SPACE:
  case QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_COMPLETE:
  case QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_FAILED:
  case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_COMPLETE:
  case QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_FAILED:
  case QtvPlayer::QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE:
  case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE:
  case QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_DONE:
  case QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_MUTE_ALL_FAILED:
  case QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE:
  case QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED:
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
		   "QTV_PLAYER_COMMAND_STATUS: %d", status);
    sendevent = true;
    break;

  default:
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Unhandled Status");
    break;
  }

  if(sendevent == true)
  {
    qtv_status_diag_event_payload.qtv_status = status;
    qtv_status_diag_event_payload.handle = (uint32)handle;
    if(pClientData)
    {
      qtv_status_diag_event_payload.pClientData = *((uint32 *)pClientData);
    }
    event_report_payload(EVENT_QTV_DIAG_PLAYER_STATUS, sizeof(qtv_status_diag_event_payload_type), &qtv_status_diag_event_payload);
    sendevent = false;
  }

  if( cmd_ptr != NULL )
  {
    /* if a command was generated then Q and wait for qtvdiag
    ** task to process it
    */
    qtvdiag_post_cmd( cmd_ptr );
  }
}//lint !e1746

/*===========================================================================

FUNCTION QTVDIAG_HANDLE_SIGS

DESCRIPTION
  This functions takes the signal just set for the qtvdiag_task and
  appropriately handles it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
void qtvdiag_handle_sigs ( rex_sigs_type sigs )
{
  int loop_count, count;

  count=sizeof(qtvdiag_sig_handler_table)/sizeof(qtvdiag_sig_table_type);

  for ( loop_count=0; loop_count<count; loop_count++ )
  {
    if ( qtvdiag_sig_handler_table[loop_count].sig & sigs )
    {
      qtvdiag_sig_handler_table[loop_count].sig_handler_fnc();
    }
  }
}

/*===========================================================================

FUNCTION QTVDIAG_HANDLE_CMDS

DESCRIPTION
  Handles the commands in the queue, and frees the command pointers afterwards.  
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void qtvdiag_handle_cmds( void )
{
  qtvdiag_cmd_s_type *cmd_ptr = NULL;
  QtvDiagMessageStructT  *pMessageStruct = NULL;

  cmd_ptr = (qtvdiag_cmd_s_type *) q_get( &qtvdiag_cmd_q );

  // Handle all commands in the queue, in case two or more signals result in fewer
  // notifications.
  while (cmd_ptr != NULL) 
  {
    switch ( cmd_ptr->cmd_id )
    { 
      case QTVDIAG_CMD_EXEC_API_CALL:
      {
            if(cmd_ptr->cmd_data)
            {
               pMessageStruct = (QtvDiagMessageStructT *)cmd_ptr->cmd_data;
               pMessageStruct->exec_api_return = qtvdiag_exec_api(pMessageStruct->api_arr_ptr,pMessageStruct->response_pkt_payload_ptr,pMessageStruct->curr_api_len);
               QtvDiag_CommandComplete();
            } 
            break;
      }
      case QTVDIAG_CMD_RENDER_FRAME:
      {
        /* Handle the rendering of the frame and associated audio playback */
        qtvdiag_handle_play_frame( (QtvPlayer::InstanceHandleT )(cmd_ptr->cmd_data) );
      }
        break;

      case QTVDIAG_CMD_HANDLE_PLAY:
	if(pQtvDiagApp->BCastResumeModeFromPause == QTV_DIAG_ISDBT_RESUME)
	{
	   QtvPlayer::PlayClip(-1,-1,(QtvPlayer::InstanceHandleT )(cmd_ptr->cmd_data) );
	}
	else if(pQtvDiagApp->BCastResumeModeFromPause == QTV_DIAG_ISDBT_PLAY_LIVE)
	{
	   QtvPlayer::PlayClip(0,-1,(QtvPlayer::InstanceHandleT )(cmd_ptr->cmd_data) );
	}
	pQtvDiagApp->BCastResumeModeFromPause = QTV_DIAG_ISDBT_NOP;
      break;

  #ifdef FEATURE_QTV_DEBUG_CPU_LOADING
      case QTVDIAG_CMD_CPU_LOADING:
      {
        QtvDiagCPULoadingHandler();
      }
      break;
#endif /* #ifdef FEATURE_QTV_DEBUG_CPU_LOADING*/

#ifdef FEATURE_QTVDIAG_REPLAY

      case QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API:
      {
        /* Handle current API */
        QtvDiagReplayExecuteCurrentAPICmdHandler();
      }
      break;
  
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY*/
      default:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                      "Unknown qtvdiag CMD %d", cmd_ptr->cmd_id);
        break;
      }
    }
    QTV_Free(cmd_ptr); /* Free the old buffer */

    cmd_ptr = (qtvdiag_cmd_s_type *) q_get( &qtvdiag_cmd_q );
  }   
}



/*===========================================================================

FUNCTION QTVDIAG_ALLOC_RESPONSE_PACKET

DESCRIPTION
  Allocate a subsystem response packet for an ordinary QTV command.
  Set the diag error code to QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN and
  the QTV return code to QTV_RETURN_ERROR.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_command_response_s_type *qtvdiag_alloc_response_packet(
  uint16 command_id) 
{
  qtvdiag_command_response_s_type *response_ptr =
    (qtvdiag_command_response_s_type *) 
    diagpkt_subsys_alloc( (int)DIAG_SUBSYS_QTV, command_id, 
                          sizeof(qtvdiag_command_response_s_type));

  if (NULL == response_ptr) 
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Cannot allocate QTV Diag response packet");
      return NULL;
  }

  // Error flags default to on
  response_ptr->diag_error_code = (uint8) QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN;
  response_ptr->qtv_return_code = (uint32) QtvPlayer::QTV_RETURN_ERROR;
  
  return response_ptr;
}
/*===========================================================================

FUNCTION QTVDIAG_CREATE_REPLAYAPI_RESPONSE_PACKET

DESCRIPTION
  Allocate a subsystem response packet for qtvdiag_Replayapi command.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

#ifdef QTV_DIAG_REPLAY
qtvdiag_Replayapi_response_s_type *qtvdiag_create_Replayapi_response_packet(uint16 command_id)
{
  qtvdiag_Replayapi_response_s_type *response_ptr =
  (qtvdiag_Replayapi_response_s_type *) 
  diagpkt_subsys_alloc( (int)DIAG_SUBSYS_QTV, command_id,
                        sizeof(qtvdiag_Replayapi_response_s_type));

  if (NULL == response_ptr)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                 "Cannot allocate QTV Diag response packet");
    return NULL;
  }

  return response_ptr;
}
#endif
/*===========================================================================

FUNCTION QTVDIAG_CREATE_TESTAPI_RESPONSE_PACKET

DESCRIPTION
  Allocate a subsystem response packet for qtvdiag_testapi command.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_testapi_response_s_type *qtvdiag_create_testapi_response_packet(uint16 command_id)
{
  qtvdiag_testapi_response_s_type *response_ptr =
  (qtvdiag_testapi_response_s_type *) 
  diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_QTV, command_id,
                        sizeof(qtvdiag_testapi_response_s_type));

  if (NULL == response_ptr)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Cannot allocate QTV Diag response packet");
    return NULL;
  }

  return response_ptr;
}
/*===========================================================================

FUNCTION qtvdiag_create_starttask_response_packet

DESCRIPTION
 Allocate a subsystem response packet for qtvdiag_start_task command.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet, NULL if allocation fails.

SIDE EFFECTS
  None.

===========================================================================*/

qtvdiag_starttask_response_s_type *qtvdiag_create_starttask_response_packet(uint16 command_id)
{
  qtvdiag_starttask_response_s_type *response_ptr =
  (qtvdiag_starttask_response_s_type *) 
  diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_QTV, command_id, 
                        sizeof(qtvdiag_starttask_response_s_type));

  if (NULL == response_ptr)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Cannot allocate QTV Diag response packet");
    return NULL;
  }

  return response_ptr;
}

/*===========================================================================

FUNCTION QTVDIAG_START_TASK

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to START_TASK and results in the qtvdiag task
  being started. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * qtvdiag_start_task ( PACKED void * /*req_pkt_ptr*/, word /*pkt_len*/ )
{
  bool issued = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Starting QTVDiag task");

  TASKLOCK();
  if (!qtvdiag_task_is_running)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Creating QTVDiag task");

    std_strlcpy(qtvdiag_tcb.task_name,QTV_DIAG_TASK_NAME,(strlen(QTV_DIAG_TASK_NAME) + 1) ) ;

    rex_def_task( &qtvdiag_tcb, 
                  (rex_stack_word_type*) qtvdiag_stack, 
                  QTVDIAG_STACK_SIZ,
                  QTVDIAG_PRI, 
                  qtvdiag_task, 
                  0L ); 

    issued = true;

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Done starting TASK");
  }
  TASKFREE();

  qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_COMMAND_START_TASK);

  if (NULL == response_ptr) 
  {
    return NULL;
  }

  if (issued) 
  {
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;

    response_ptr->qtv_return_code = (uint32) QtvPlayer::QTV_RETURN_OK;        
  } 
  else 
  {
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
  }

  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
}/*end qtvdiag_start_task*/ /*lint !e715 !e818 suppressed lint warning*/ 


/*===========================================================================

FUNCTION QTVDIAG_END_TASK

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to END_TASK and results in the qtvdiag task
  being terminated. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * qtvdiag_end_task ( PACKED void * /*req_pkt_ptr*/, word /*pkt_len*/ )
{
  bool issued = false;

  if (qtvdiag_task_is_running)
  {
    (void)rex_set_sigs( &qtvdiag_tcb, QTVDIAG_EXIT_SIG );
  
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "deleting QTVDiag task");
    while (qtvdiag_task_is_running)
    {
      zrex_sleep(200);
    }
    issued = true;
  }
  else
  {
    issued = false;
  }

  qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_COMMAND_END_TASK);

  if (NULL == response_ptr) 
  {
      return NULL;
  }

  if (issued) 
  {
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
    
    response_ptr->qtv_return_code = (int)QtvPlayer::QTV_RETURN_OK;

  }
  else
  { 
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
  }

  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
}
/*===========================================================================

FUNCTION start_qtvdiag_framework_cmd_handler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to START_TASK and results in the qtvdiag task
  being started. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/

PACKED void* start_qtvdiag_framework_cmd_handler( PACKED void * request_packet_ptr, word)
{
  bool issued = false;
  bool is_dupe_diag_pkt = true;
  uint8 isReplaySession = 0;
  uint32 free_amssbuffer = 0;
  uint32 target_protocol_ver = (uint32)TARGET_PROTOCOL_VERSION;
  uint32 target_api_ver = (uint32)TARGET_API_VERSION;
  qtvdiag_cmd_error_code_e_type diag_error_code= QTVDIAG_ERROR_CODE_GENERIC_ERROR;
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  char *api_packet_ptr = (char *) request_packet_ptr;
  if(!pQtvDiagApp)
  {
    /* initialize only for once */
  diag_pkt_seq_number = 0;
  }
  is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
  if(is_dupe_diag_pkt)
  {
    return NULL;
  }
  //skip header
  api_packet_ptr += sizeof(diagpkt_subsys_header_type);
  //skip the diag packet sequence number
  api_packet_ptr += sizeof(uint32);

  UNPACK_UINT8(isReplaySession,api_packet_ptr); 

  qtvdiag_starttask_response_s_type *response_ptr =
  qtvdiag_create_starttask_response_packet((uint16) QTVDIAG_FRAMEWORK_START_CMD_ID);
  // WARNING: TODO: Need to handle errors gracefully
  if (NULL == response_ptr)
  {
    return NULL;
  }
  if(isReplaySession == 1)
  {
#ifdef QTV_DIAG_REPLAY
    if(!QtvDiag_pReplayObj->Start())
    {
      return NULL;
    }
    free_amssbuffer = QtvDiag_pReplayObj->FreeAmssBufSize();
    QtvDiag_InReplaySession = true;
#endif
    
  }

  char* response_pkt_ptr = response_ptr->response_payload;

  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, target_protocol_ver);
  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, target_api_ver);

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Starting QTVDiag task");
    TASKLOCK();
    if (!qtvdiag_task_is_running)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Creating QTVDiag task");
      std_strlcpy(qtvdiag_tcb.task_name,QTV_DIAG_TASK_NAME,(strlen(QTV_DIAG_TASK_NAME) + 1) ) ;
      rex_def_task( &qtvdiag_tcb, 
                    (rex_stack_word_type*) qtvdiag_stack, 
                    QTVDIAG_STACK_SIZ,
                    QTVDIAG_PRI, 
                    qtvdiag_task, 
                    0L ); 
      issued = true;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Done starting TASK");
    }
    TASKFREE();

    if (issued)
    {
      diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
      retVal          = QtvPlayer::QTV_RETURN_OK;

      pQtvDiagApp = (QtvDiagAppT*)QTV_Malloc( sizeof(QtvDiagAppT) );
      if(pQtvDiagApp)
      {
          memset(pQtvDiagApp,0x00, sizeof(QtvDiagAppT));
          pQtvDiagApp->call_info_ptr = (CallInfoT *)QTV_Malloc(sizeof(CallInfoT));
          if(pQtvDiagApp->call_info_ptr)
          {
             memset(pQtvDiagApp->call_info_ptr,0x00, sizeof(CallInfoT));
          }
      }
      pMsgToQtvDiag = (QtvDiagMessageStructT*)QTV_Malloc( sizeof(QtvDiagMessageStructT) );
      if(pMsgToQtvDiag)
      {
          memset(pMsgToQtvDiag,0x00, sizeof(QtvDiagMessageStructT));
      }
      else
      {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for pMsgToQtvDiag");
      }
    }
    else
    {
      diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
    }
    uint16 ret = (uint16)retVal;
    FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
    FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
    FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, free_amssbuffer);

    diagpkt_commit(response_ptr);
    return(PACKED void *) response_ptr;
  
}/*end qtvdiag_start_task*/

/*===========================================================================

FUNCTION 
  is_duplicate_diag_packet

DESCRIPTION
  This will return whether the diag packet received by the command handle
  is duplicate packet or received first time
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
bool is_duplicate_diag_packet(char* diag_pkt_ptr)
{
  uint32 sequence_num = 0;
  char *pkt_ptr = diag_pkt_ptr;
  uint16 sub_sys_cmd_code;
  pkt_ptr += sizeof(uint16);
  UNPACK_UINT16(sub_sys_cmd_code,pkt_ptr); 
  UNPACK_UINT32(sequence_num,pkt_ptr);
  if(sequence_num > diag_pkt_seq_number)
  {
    diag_pkt_seq_number = sequence_num;
    return false;
  }
  else
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "Duplicate diag pkt.Subsys cmd code is %d, Packet sequence number %d",
            sub_sys_cmd_code,sequence_num);
    return true;
  }
}

/*===========================================================================

FUNCTION stop_qtvdiag_framework_cmd_handler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to END_TASK and results in the qtvdiag task
  being terminated. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * stop_qtvdiag_framework_cmd_handler ( PACKED void *request_packet_ptr , word )
{
  bool issued = false;
  bool is_dupe_diag_pkt = true;
  uint32 count;
  char *api_packet_ptr = (char *) request_packet_ptr;
  
  is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
  if(is_dupe_diag_pkt)
  {
      return NULL;
  }
  /* Stop http Simulator */
  for(count = 0; count < QtvPlayer::QTV_PRIORITY_COUNT; count++)
  {
    HttpSimulatorStop((QtvPlayer::InstanceHandleT)count);
  }
  if (qtvdiag_task_is_running)
  {
    (void)rex_set_sigs( &qtvdiag_tcb, QTVDIAG_EXIT_SIG );

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "deleting QTVDiag task");
    while (qtvdiag_task_is_running)
    {
      zrex_sleep(200);
    }
    issued = true;
  }
  else
  {
    issued = false;
  }
#ifdef QTV_DIAG_REPLAY
  if(QtvDiag_InReplaySession == true)
  {
    QtvDiag_pReplayObj->Stop();
    QtvDiag_InReplaySession = false;
  }
#endif
  qtvdiag_command_response_s_type *response_ptr =
  qtvdiag_alloc_response_packet((uint16) QTVDIAG_FRAMEWORK_STOP_CMD_ID);

  if (NULL == response_ptr)
  {
    diag_pkt_seq_number = 0;
    return NULL;
  }

  if (issued)
  {
    response_ptr->diag_error_code =
    (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;

    response_ptr->qtv_return_code = (int)QtvPlayer::QTV_RETURN_OK;

    /* free diag app */
    if(pQtvDiagApp)
    {
      if(pQtvDiagApp->call_info_ptr)
      {
        QTV_Free(pQtvDiagApp->call_info_ptr);
        pQtvDiagApp->call_info_ptr = NULL;
      }
      QTV_Free(pQtvDiagApp);
      pQtvDiagApp = NULL;
    }
    //if the message struct pointer is allocated then free the memory
    if (pMsgToQtvDiag)
    {
       QTV_Free(pMsgToQtvDiag);
       pMsgToQtvDiag = NULL;
    }
  }
  else
  {
    response_ptr->diag_error_code = 
    (uint8) QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
  }

  QtvDiag_End(); // counter part of QtvDiag_Init
  diagpkt_commit(response_ptr);
  diag_pkt_seq_number = 0;
  return(PACKED void *) response_ptr;
}

/*===========================================================================
FUNCTION qtvdiag_stop_replay_cmd_handler

DESCRIPTION
  Function called in the Diag context used to indicate that there are no more
  APIs left to process. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef QTV_DIAG_REPLAY
PACKED void * qtvdiag_stop_replay_cmd_handler ( PACKED void * , word )
{
  QtvDiag_pReplayObj->Stop();
  QtvDiag_InReplaySession = false;
  
  qtvdiag_command_response_s_type *response_ptr =
  qtvdiag_alloc_response_packet((uint16) QTVDIAG_STOP_REPLAY_CMD_HANDLER);

  if (NULL == response_ptr)
  {
    return NULL;
  }

  response_ptr->diag_error_code = (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  response_ptr->qtv_return_code = (int)QtvPlayer::QTV_RETURN_OK;
  diagpkt_commit(response_ptr);

  return(PACKED void *) response_ptr;
}
#endif

/*===========================================================================

FUNCTION QTVDIAG_USE_HEAP_MEMORY

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTV_COMMAND_USE_HEAP_MEMORY and results in
  heap memory being allocated. If there is existing allocated memory due to a 
  previous command, the previously allocated memory will be freed first.  

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * qtvdiag_use_heap_memory ( PACKED void * packet, word /*pkt_len*/ )
{
  static void *space = NULL;

  // Free existing space
  if (space != NULL) 
  {
      QTV_Free(space);
      space = NULL;
  }

  uint32 length = *((uint32 *) packet);

  qtvdiag_command_response_s_type *response_ptr = NULL;

  if (length > 0) 
  {
    space = QTV_Malloc((unsigned long) length);

    response_ptr =
      qtvdiag_alloc_response_packet((uint16) QTVDIAG_COMMAND_USE_HEAP_MEMORY);

    if (NULL == response_ptr) 
    {
        return NULL;
    }

    if (NULL == space) 
    {
      response_ptr->diag_error_code = 
        (uint8) QTVDIAG_ERROR_CODE_OUT_OF_MEMORY;
      response_ptr->qtv_return_code = (uint32) 0;  
    } 
    else 
    {
      response_ptr->diag_error_code = 
        (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;

      // Put the pointer value as the return code.
      response_ptr->qtv_return_code = (uint32) space;     
    }
  } 
  else 
  {
      space = NULL;

      response_ptr =
        qtvdiag_alloc_response_packet((uint16) QTVDIAG_COMMAND_USE_HEAP_MEMORY);
  
      if (NULL == response_ptr) 
      {
          return NULL;
      }

      response_ptr->diag_error_code = 
        (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;

      // Put the pointer value as the return code.
      response_ptr->qtv_return_code = (uint32) 0;     
  }


  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
} /*lint !e818 lint error supressed */

/*===========================================================================

FUNCTION QTVDIAG_REGISTER_FOR_CALLBACK

DESCRIPTION
  Function called in the Diag context that registers the QTV Diag callback
  function as the QTV callback function. The QTV Diag callback function renders 
  frames and sends back info to the client.
  
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  Sends back a response packet.

===========================================================================*/

PACKED void * qtvdiag_register_for_callback ( PACKED void * /* packet */, 
                                              word /*pkt_len*/ )
{ 

  QtvPlayer::ReturnT retval = 
    QtvPlayer::RegisterForCallback(qtvdiag_callbackFn, NULL);

  qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_COMMAND_REGISTER_FOR_CALLBACK);

  if (NULL == response_ptr) {
      return NULL;
  }

  response_ptr->diag_error_code = (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  response_ptr->qtv_return_code = (uint8) retval;

  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
}

CRITICAL_SECTION QtvDiagInfoData::cs;

QtvDiagInfoData *QtvDiagInfoData::infoArray[(int)QTVDIAG_INFO_MAX - (int)QTVDIAG_INFO_OFFSET];

/*===========================================================================

FUNCTION QtvDiagInfoData::Reset

DESCRIPTION
  Resets the send info data so that no info is sent the next time the
  callback function executes, unless the info is sticky. If the INIT
  argument is true, the info data system will be initialized.
  
DEPENDENCIES
  If init is false, QtvDiagInfoData::Reset(true) must have been called
  previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void QtvDiagInfoData::Reset(bool init) 
{
  if (init) 
  {
      InitializeCriticalSection(&cs);
  }

  for (int i = (int) QTVDIAG_INFO_OFFSET; i < (int) QTVDIAG_INFO_MAX; i++) 
  {
      int index = i - QTVDIAG_INFO_OFFSET;
      // Get rid of existing info
      if (init) 
      {
         infoArray[index] = NULL;
      } 
      else if ((NULL != infoArray[index]) && !infoArray[index]->isSticky) 
      {
         delete infoArray[index];          
         infoArray[index] = NULL;
      }
  }
}

/*===========================================================================

FUNCTION ShouldSend

DESCRIPTION
  Checks the send info array to see if the info corresponding to the argument
  type should be sent.

DEPENDENCIES
  QtvDiagInfoData::Reset(true) must have been called.

RETURN VALUE
  True iff the info for the argument type should be sent.

SIDE EFFECTS
  None.

===========================================================================*/
bool QtvDiagInfoData::ShouldSend(qtvdiag_info_e_type info_type)
{
  int index = ((int) info_type) - QTVDIAG_INFO_OFFSET;
  bool retval = false;

  /* Below if condition is added to fix klocwork warning */
  if( (0 <= index) && index < ((int)QTVDIAG_INFO_MAX - (int)QTVDIAG_INFO_OFFSET))
  {
    EnterCriticalSection(&cs);
    retval = (infoArray[index] != NULL);
    LeaveCriticalSection(&cs);
  }

  return retval;
}

/*===========================================================================

FUNCTION QTVDiagInitData::SetSend

DESCRIPTION
  Record the request to send the argument type of information.

DEPENDENCIES
  QtvDiagInfoData::Reset(true) must have been called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagInfoData::SetSendInfo(qtvdiag_info_e_type info_type, PACKED char *packet)
{  
  int index = ((int) info_type) - QTVDIAG_INFO_OFFSET;
                                  
  if ((index < 0) || ((uint32)index >= sizeof(infoArray) / sizeof(QtvDiagInfoData *)))
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown info type %d", info_type);
    return;
  }

  EnterCriticalSection(&cs);

  // Remove existing info
  if (NULL != infoArray[index]) 
  {
      delete infoArray[index];
  }

  // The code below could be replaced with a factory someday ...
  infoArray[index] = new QtvDiagInfoData(packet);  

  LeaveCriticalSection(&cs);
}

/*===========================================================================

FUNCTION QTVDiagInitData::SendAll

DESCRIPTION
  For each info type, send the info if requested. If the request is not 
  sticky, clear the info data so the info isn't sent next time.
  
  Note that the entire send process isn't atomic. If a request for info
  comes in while this function is running, it may or may not be handled,
  depending on where this function is in the info array.

DEPENDENCIES
  QtvDiagInfoData::Reset(true) must have been called.                             

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagInfoData::SendAll(int status) 
{
  for (int i = (int) QTVDIAG_INFO_OFFSET; i < (int) QTVDIAG_INFO_MAX; i++) 
  {
    qtvdiag_info_e_type info_type = (qtvdiag_info_e_type) i;

    if (ShouldSend(info_type)) 
    {
        (void)Send(info_type, infoArray[i - QTVDIAG_INFO_OFFSET], 
         (QtvPlayer::PlayerStatusT) status);
    }
  }

  // Clear non-sticky info requests.
  Reset(false);
}

/*===========================================================================

FUNCTION QtvDiagInfoData::PayloadSizeForInfo

DESCRIPTION
  Compute the size of the payload needed to hold the argument type of info.
  
DEPENDENCIES
  None.  

RETURN VALUE
  The computed size of the payload (smaller than the entire packet size). If
  the info type is unknown, returns -1.

SIDE EFFECTS
  None.
  
===========================================================================*/
int QtvDiagInfoData::PayloadSizeForInfo(qtvdiag_info_e_type info_type, void *pInfoSource) 
{
  int payload_size = 0;

  switch (info_type) 
  {
    case QTVDIAG_INFO_PLAYER_STATE:   
    {
      // This must be the maximum size under any featurization, so
      // can't use sizeof(PlayerStateRecordT).
      payload_size = 2 * sizeof(int32)  + 5 * sizeof(uint32);     
    }
    break;

    case QTVDIAG_INFO_PLAYER_STATUS:
    {
      payload_size = sizeof(uint32);
      
    }
    break;
      
    case QTVDIAG_INFO_FRAME_INFO:
    {
      //TO DO
      
    }
    break;

    case QTVDIAG_INFO_CLIP_INFO:
    {
      QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT *) pInfoSource;

      payload_size = 6 + 6 * sizeof(int32) + 3 * sizeof(uint32);

      // For track types
      if (pClipInfo->TrackCount > 0)
      {
        payload_size += pClipInfo->TrackCount;
      }
    }
    break;

    case QTVDIAG_INFO_AV_STATISTICS:
    case QTVDIAG_INFO_TIMED_TEXT_STATISTICS:
    // TO DO
    break;

    case QTVDIAG_INFO_RTP_STATISTICS:
    {
      int32 numTracks = ((QtvPlayer::RTPStatisticsT *) pInfoSource)->NumTracks;

      payload_size = 1 + numTracks * (1 + 7 * sizeof(int32)); /*lint !e737 !e713 */
    }
    break;

    default:
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown info type %d", (int) info_type);
    break;
  }

  return payload_size;
}

/*===========================================================================

FUNCTION QtvDiagInfoData::FillPlayerStateInfoPacket

DESCRIPTION
  Fill player state info. 
  
DEPENDENCIES
  None.  

RETURN VALUE
  Returns true/false.

SIDE EFFECTS
  None.
  
===========================================================================*/
bool QtvDiagInfoData::FillPlayerStateInfoPacket(PACKED char *info_ptr)
{
  QtvPlayer::PlayerStateRecordT state;

  if (QtvPlayer::GetPlayerState(state) != QtvPlayer::QTV_RETURN_OK) 
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't get player state to send");
    return false;
  }

  *((int32 *) info_ptr) = (int32)htonl(state.videoFrames); //lint !e826
  info_ptr += sizeof(int32);
  *((int32 *) info_ptr) = (int32)htonl(state.audioFrames); //lint !e826
  info_ptr += sizeof(int32);
  *((uint32 *) info_ptr) = (int32)htonl(state.playbackMsec); //lint !e826
  info_ptr += sizeof(uint32);
  *((uint32 *) info_ptr) = (int32)htonl(state.videoPlaybackMsec); //lint !e826
  info_ptr += sizeof(uint32);
  *((uint32 *) info_ptr) = (int32)htonl(state.audioPlaybackMsec); //lint !e826
  info_ptr += sizeof(uint32);

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  *((uint32 *) info_ptr) = htonl(state.textPlaybackMsec); //lint !e826
#else
  *((uint32 *) info_ptr) = 0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  // Important: skip even if timed text is not enabled so that info packet 
  // has uniform format.
  info_ptr += sizeof(uint32);

#ifdef FEATURE_QTV_PSEUDO_STREAM
  *((uint32 *) info_ptr) = htonl(state.playbackOffsetinBytes); //lint !e826      
#else
  *((uint32 *) info_ptr) = 0;
#endif      
  return true;
}

/*===========================================================================

FUNCTION QtvDiagInfoData::FillClipInfoInfoPacket

DESCRIPTION
  Fill clip info.   
  
DEPENDENCIES
  None.  

RETURN VALUE
  Returns true/false.

SIDE EFFECTS
  None.
  
===========================================================================*/
void QtvDiagInfoData::FillClipInfoInfoPacket(PACKED char *info_ptr, void *pInfoSource)
{
  QtvPlayer::ClipInfoT *pClipInfo = (QtvPlayer::ClipInfoT *) pInfoSource;

  *((uint8 *) info_ptr) = (uint8) pClipInfo->streamtype;
  info_ptr += sizeof(uint8);

  *((uint8 *) info_ptr) = (uint8) pClipInfo->RepositioningAllowed;
  info_ptr += sizeof(uint8);

  *((uint8 *) info_ptr) = (uint8) pClipInfo->AudioOnlyPlaybackAllowed;
  info_ptr += sizeof(uint8);

  *((uint8 *) info_ptr) = (uint8) pClipInfo->VideoOnlyPlaybackAllowed;
  info_ptr += sizeof(uint8);

  *((uint8 *) info_ptr) = (uint8) pClipInfo->MediaType;
  info_ptr += sizeof(uint8);

  int32 trackCount = pClipInfo->TrackCount;

  *((uint8 *) info_ptr) = (uint8) trackCount;
  info_ptr += sizeof(uint8);

  for (int i = 0; i < trackCount; i++)
  {
    *((uint8 *) info_ptr) = (uint8) pClipInfo->TrackTypes[i];
    info_ptr += sizeof(uint8);
  }

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->Width); //lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->Height); //lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->Duration); //lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->Size);//lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->AudioSamplingFreq);//lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl((int32) (pClipInfo->VideoFrameRate * 10000.0f)); /*lint!e572 !e826*/
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->NumAudioChannels); //lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->VideoEncodedBitRate); //lint !e826
  info_ptr += sizeof(int32);

  *((int32 *) info_ptr) = (int32)htonl(pClipInfo->AudioEncodedBitRate); //lint !e826
}

/*===========================================================================

FUNCTION QtvDiagInfoData::FillRTPStatisticsInfoPacket

DESCRIPTION
  Fills RTp statistics info. 
  
DEPENDENCIES
  None.  

RETURN VALUE
  Returns true/false.
 
SIDE EFFECTS
  None.
  
===========================================================================*/
bool QtvDiagInfoData::FillRTPStatisticsInfoPacket(PACKED char *info_ptr)
{

  QtvPlayer::RTPStatisticsT statistics; 

  if (QtvPlayer::GetRTPStatistics(statistics) != QtvPlayer::QTV_RETURN_OK)
  {
	return false;
  }
      
  int32 numTracks = statistics.NumTracks;
  if ((numTracks < 0) || (numTracks >= QTV_MAX_MEDIA_TRACKS)) 
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid # of tracks, can't send RTP info");
    return false;
  }

  *info_ptr = (char) numTracks;
  info_ptr++;

  PACKED char *info_ptr_base = info_ptr;

  for (int32 i = 0; i < numTracks; i++) 
  {
    QtvPlayer::TrackDataT *track_data_ptr = &(statistics.TrackData[i]);

    *((uint8 *) info_ptr) = (uint8) track_data_ptr->TrackType;
    info_ptr += (uint32)numTracks * sizeof(uint8);

    *((int32 *) info_ptr) = (int32)htonl(track_data_ptr->VideoIFrameInterval); //lint !e826
    info_ptr += (uint32)numTracks * sizeof(int32);

    *((int32 *) info_ptr) = (int32)htonl(track_data_ptr->LostPackets); //lint !e826
    info_ptr += (uint32)numTracks * sizeof(int32);

    *((int32 *) info_ptr) = (int32)htonl(track_data_ptr->PacketsReceived); //lint !e826
    info_ptr += (uint32)numTracks * sizeof(int32);

    *((int32 *) info_ptr) = (int32)htonl(track_data_ptr->NetworkJitter); //lint !e826
    info_ptr += (uint32)numTracks * sizeof(int32);
        
    *((int32 *) info_ptr) = (int32)htonl((int32) 10000.0 * track_data_ptr->VideoFrameRate); //lint !e826
    info_ptr += (uint32)numTracks * sizeof(int32);

    *((int32 *) info_ptr) = (int32)htonl(track_data_ptr->MediaDataRate); //lint !e826
        
    // Now point to the the next track's track type
    info_ptr = info_ptr_base +((uint32) i * sizeof(uint8));
  }   
  return true;
}

/*===========================================================================

FUNCTION QtvDiagInfoData::FillInfoPacketForDiag

DESCRIPTION
  Fill in the data based on the information requested.   
  
DEPENDENCIES
  None.  

RETURN VALUE
  Returns true/false.

SIDE EFFECTS
  None.
  
===========================================================================*/
bool QtvDiagInfoData::FillInfoPacketForDiag(qtvdiag_info_e_type info_type, PACKED char *info_ptr,
  QtvPlayer::PlayerStatusT status, void *pInfoSource)
{
  switch (info_type) 
  {
    case QTVDIAG_INFO_PLAYER_STATE:
	  if (!FillPlayerStateInfoPacket(info_ptr))
		return false;
    break;

    case QTVDIAG_INFO_PLAYER_STATUS:
    {
      *((int32 *) info_ptr) = (int32)htonl(status);    /*lint !e572 !e826 */
    }
    break;

    case QTVDIAG_INFO_FRAME_INFO:
    {
      // TO DO 
    }
    break;
  
    case QTVDIAG_INFO_CLIP_INFO:
	  FillClipInfoInfoPacket(info_ptr, pInfoSource);
    break;

    case QTVDIAG_INFO_AV_STATISTICS:
    case QTVDIAG_INFO_TIMED_TEXT_STATISTICS:
    break;  
  
    case QTVDIAG_INFO_RTP_STATISTICS:
	  if(!FillRTPStatisticsInfoPacket(info_ptr))
		return false;
    break;
  
    default:
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown info type %d", (int) info_type);
    return false;
  }

  return true;
}

/*===========================================================================

FUNCTION Send

DESCRIPTION
  Given a type of info to send, and additional options in infoData, send the
  info back to the host using a log packet. 
  
DEPENDENCIES
  None.  

RETURN VALUE
  A QTV return value indicating if the parameter was set successfully.  

SIDE EFFECTS
  Sends a response packet (of type qtvdiag_command_response_s_type) back to
  the Diag system.
  
===========================================================================*/
bool QtvDiagInfoData::Send(qtvdiag_info_e_type info_type, 
                           QtvDiagInfoData *infoData, QtvPlayer::PlayerStatusT status) 
{
  if (NULL == infoData)
  {
    return false;
  }

  int index = (int) info_type;

  if ((index < QTVDIAG_INFO_OFFSET) || (index >= (int) QTVDIAG_INFO_MAX)) 
  {
    return false;   
  }  

  // Some info types have a variable sized payload. If so, we need to get some
  // information first in order to determine the size.

  void *pInfoSource = NULL;
  QtvPlayer::ClipInfoT clipInfo;
  QtvPlayer::RTPStatisticsT statistics;

  switch (info_type)
  {
    case QTVDIAG_INFO_CLIP_INFO:
    {
      if (QtvPlayer::QTV_RETURN_OK != QtvPlayer::GetClipInfo(clipInfo))
      {         
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                     "Can't get clip info statistics to send");
        return false;        
      }
      pInfoSource = &clipInfo;
    }
    break;

    case QTVDIAG_INFO_RTP_STATISTICS:
    {     
      if (QtvPlayer::QTV_RETURN_OK != QtvPlayer::GetRTPStatistics(statistics)) 
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                     "Can't get RTP statistics to send");
        return false;        
      }
      pInfoSource = &statistics;
    }
    break;

  default :
    ASSERT(0);
    break;
  }

  // Figure out the size of the response packet that will contain the info.
  int payload_size = PayloadSizeForInfo(info_type, pInfoSource);

  if (payload_size <= 0)
  {
    return false;
  }

  // Create a packet with a subsystem ID that is the same as the info type.
  PACKED char *response_packet = (PACKED char *)   
                                 diagpkt_subsys_alloc( (uint32)DIAG_SUBSYS_QTV, (uint16) info_type,
                                                       sizeof(diagpkt_subsys_header_type) + (uint32)payload_size);

  if (NULL == response_packet) 
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Cannot allocate QTV Diag info response packet");
      return false;
  }
  
  // Get a pointer to where we can actually write data.
  PACKED char *info_ptr = response_packet + sizeof(diagpkt_subsys_header_type);

  // Zero the data area out to avoid security issues and easier interpretation.
  memset((void *) info_ptr, 0, (uint32)payload_size);

  // Fill in the info
  (void)FillInfoPacketForDiag(info_type, info_ptr, status, pInfoSource);

  diagpkt_commit(info_ptr);

  return true;
}//lint !e818

#ifdef FEATURE_BREW_3_0

#ifdef FEATURE_COMMON_MM_DIR
#define DEFAULT_CLIP_DIRECTORY "fs:/shared/media/"
#else /* FEATURE_COMMON_MM_DIR */
#define DEFAULT_CLIP_DIRECTORY "/brew/mod/mediaplayer/media/"
#endif /* FEATURE_COMMON_MM_DIR */

#else /* FEATURE_BREW_3_0 */

#ifdef FEATURE_COMMON_MM_DIR
#define DEFAULT_CLIP_DIRECTORY "shared/"
#else /* FEATURE_COMMON_MM_DIR */
#define DEFAULT_CLIP_DIRECTORY "media/"
#endif

#endif /* FEATURE_BREW_3_0 */


/*===========================================================================

FUNCTION QTVDIAG_MAKE_FILENAME

DESCRIPTION
  Given a pointer containing a null-terminated string, allocate a new string that 
  is the null-terminated version of the argument string. Also, if the argument 
  string isn't a absolute path (doesn't start with a '/', 'fs:/' and doesn't 
  contain "://" like a URL), prepend the normal directory
  path of media clips for the BREW version we are using, so that relative
  pathnames can be sent by the QXDM user.

  Sets p_length to the length of the original string, including the 
  null-terminator, so that the string can be skipped in the packet. This
  is set even if an error occurred.
   
  Sets *pp_name to a newly allocated, null-terminated string with the full 
  absolute pathname. This value must be freed by the caller! If an error
  occurred or if the original string just contains the null terminating 
  character, p_filename will point to NULL.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  true if successful, false if a memory allocation error occured
  
SIDE EFFECTS
  None
===========================================================================*/

static bool qtvdiag_make_filename(const char *packet, uint32 *p_length, 
                                  char **p_filename) 
{   
  int length = (int)strlen(packet);

  if (0 == length) 
  {
    *p_length = 1;
    *p_filename = NULL;        
    return true;
  }

  bool isAbsolutePath = (packet[0] == '/') || 
                        (strncmp(packet, "fs:/", 4) == 0) ||
                        (strstr(packet, "://") != NULL); /* look for URL syntax */


  uint32 total_length = 0;

  // Allocate enough memory for the filename, terminated by a null character.
  if (isAbsolutePath) 
  {
    // Absolute path, no need to prefix by clip directory.
    total_length = (uint32)length + 1;
  } 
  else 
  {        
    total_length = (uint32)length + strlen(DEFAULT_CLIP_DIRECTORY) + 1;
  }

  char *filename = (char *) QTV_Malloc(total_length);

  if (NULL == filename) 
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                  "Failed to allocate %ld bytes for filename", 
                  total_length);
    *p_length = (uint32)length + 1;
    *p_filename = NULL;
    return false;
  }

  if (isAbsolutePath) 
  {
    // No need to prefix by clip directory
    (void)std_strlcpy(filename, packet, total_length);
  } 
  else 
  {
    // Relative path, need to prefix by clip directory
    (void)std_strlcpy(filename, DEFAULT_CLIP_DIRECTORY, total_length);


    // Who cares about efficiency when using Diag? Just compute strlen() 
    // again...
    (void)std_strlcat(filename, packet, total_length);
  }

  // Remember to free this!!!
  *p_filename = filename;

  *p_length = (uint32)length + 1; // include null terminating character

  return true;
}
/*===========================================================================

FUNCTION QTVDIAG_REPLAYAPI

DESCRIPTION
  This function gets the byte stream from the host script and passes the
  data  to  "defrag_apipacket()"  to get full API info. It  then invokes 
  "qtvdiag_exec_api()" to execute the API and get the response packet and
  sends it to host script
  
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes
  diag happy that the signal was handled

SIDE EFFECTS
  None
===========================================================================*/

#ifdef QTV_DIAG_REPLAY
PACKED void * qtvdiag_replay_api_cmd_handler( PACKED void *request_packet_ptr,word /*param*/)
{
  qtvdiag_command_response_s_type *Replay_ptr;
  Replay_ptr = qtvdiag_alloc_response_packet(10);
  char *api_packet_ptr = (char *) request_packet_ptr;
  uint8 *api_arr_ptr = NULL;
  uint16 arr_index;
  uint16 defrag_pkt_len;
  uint32 free_amssbuffer = 0;
  qtvdiag_defrag_messg_e_type defragment_status;
  qtvdiag_Replayapi_response_s_type * handle_replay_response_ptr=NULL;
  char* response_pkt_payload_ptr = NULL;
  static uint32 curr_api_pack_len = 0;

  // Skip the header part in the api packet
  api_packet_ptr += sizeof(diagpkt_subsys_header_type);

  //Defragment the diag packet
  defragment_status = qtvdiag_defrag_diagpacket(api_packet_ptr, &curr_api_pack_len);
  handle_replay_response_ptr = 
  qtvdiag_create_Replayapi_response_packet((uint16) QTVDIAG_FRAMEWORK_REPLAYAPI_CMD_ID);  
  handle_replay_response_ptr->defragment_status = defragment_status;

  //Check whether a full api packet is formed or not
  if (QTVDIAG_DEFRAGMENT_DONE == defragment_status)
  {
    response_pkt_payload_ptr = handle_replay_response_ptr->response_payload;
    api_arr_ptr = (uint8 *)qtvdiag_get_defrag_data_ptr(&defrag_pkt_len);
    QtvDiag_pReplayObj->QueueApi(api_arr_ptr,defrag_pkt_len);
    //Make all the array elements to null to place next api packet
    for (arr_index = 0; arr_index < TEMP_BUFFER_MAX_SIZE; arr_index++)
    {
      api_arr[arr_index] = '\0';
    }
  }
  free_amssbuffer = QtvDiag_pReplayObj->FreeAmssBufSize();
  FILL_RESPONSE_PACKET_UINT32(response_pkt_payload_ptr, free_amssbuffer);
  diagpkt_commit(handle_replay_response_ptr);

  return(PACKED void *) handle_replay_response_ptr;
}
#endif

/*===========================================================================

FUNCTION QTVDIAG_TESTAPI

DESCRIPTION
  This function gets the byte stream from the host script and passes the
  data  to  "defrag_apipacket()"  to get full API info. It  then invokes 
  "qtvdiag_exec_api()" to execute the API and get the response packet and
  sends it to host script
  
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes
  diag happy that the signal was handled

SIDE EFFECTS
  None
===========================================================================*/
PACKED void * qtvdiag_test_api_cmd_handler( PACKED void *request_packet_ptr, word /*param*/)
{
  char *api_packet_ptr = (char *) request_packet_ptr;
  char* api_arr_ptr = NULL;
  char* response_pkt_payload_ptr;
  uint16 arr_index;
  uint16 defrag_pkt_len;
  static uint32 curr_api_len = 0;
  bool exec_api_return = false;
  bool is_dupe_diag_pkt = true;
  qtvdiag_defrag_messg_e_type defragment_status;
  qtvdiag_testapi_response_s_type * handle_api_response_ptr=NULL;
  qtvdiag_cmd_s_type *cmd_ptr = NULL;

  is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
  if(is_dupe_diag_pkt)
  {
      return NULL;
  }
  // Skip the header part in the api packet
  api_packet_ptr += sizeof(diagpkt_subsys_header_type);
  //Skip the 32bit diag packet sequence number.
  api_packet_ptr += sizeof(uint32);

  //Defragment the diag packet
  defragment_status = qtvdiag_defrag_diagpacket(api_packet_ptr,&curr_api_len);
  handle_api_response_ptr = 
  qtvdiag_create_testapi_response_packet((uint16) QTVDIAG_FRAMEWORK_SEND_TESTAPI_CMD_ID);
  if(handle_api_response_ptr)
  {
    handle_api_response_ptr->defragment_status = (uint8)defragment_status;

    //Check whether a full api packet is formed or not
    if (QTVDIAG_DEFRAGMENT_DONE == defragment_status)
    {
      response_pkt_payload_ptr = handle_api_response_ptr->response_payload;
      api_arr_ptr = (char *)qtvdiag_get_defrag_data_ptr(&defrag_pkt_len);

      if(pMsgToQtvDiag != NULL)
      {
        memset(pMsgToQtvDiag,0x00, sizeof(QtvDiagMessageStructT));
        pMsgToQtvDiag->api_arr_ptr = api_arr_ptr;
        pMsgToQtvDiag->response_pkt_payload_ptr = response_pkt_payload_ptr;
        pMsgToQtvDiag->curr_api_len =  curr_api_len;
     
        /* allocate the command pointer struct which holds command specific information
           to execute the API */
        cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));

        if( cmd_ptr != NULL )
        {
           memset(cmd_ptr,0,sizeof(qtvdiag_cmd_s_type));
           cmd_ptr->cmd_id = QTVDIAG_CMD_EXEC_API_CALL;
           cmd_ptr->cmd_data = (uint32)pMsgToQtvDiag;
        }
        else
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
        }    
        if( cmd_ptr != NULL )
        {
           /* if a command was generated then Q and wait for qtvdiag
            ** task to process it
            */
            qtvdiag_post_cmd( cmd_ptr );
        }
        bool bOK = true;
        // Wait untill QTV Diag task executes qtvdiag_exec_api and sends back the response
        QTV_MSG( QTVDIAG_GENERAL, "DIAG Taskr::SendCommand waiting for response sync" );
        bOK = QCUtils::WaitForCondition(&QtvDiagResponseSync,QTVDIAG_RESPONSE_TIMEOUT_MSEC);
        if (!bOK)
        {        
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Wait for condition failed");
        } 
        else
        {
           exec_api_return = pMsgToQtvDiag->exec_api_return;
        }
        
      }
      else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"pMsgToQtvDiag pointer not allocated");
      }	

      //Make all the array elements to null to place fresh api packet
      for (arr_index = 0; arr_index < TEMP_BUFFER_MAX_SIZE; arr_index++)
      {
        api_arr[arr_index] = '\0';
      }
      curr_api_len = 0;
      if(false == exec_api_return) //lint !e731
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"qtvdiag_exec_api() call failed");
      }
    }
    diagpkt_commit(handle_api_response_ptr);
  }
  return(PACKED void *) handle_api_response_ptr;
}//lint !e550

/*===========================================================================

FUNCTION QTVDIAG_DEFRAG_DIAGPACKET

DESCRIPTION
  This function gets the  diag packet as  input and checks whether it is a
  fragment or a full API packet. In case of diag packet containing fragmentof
  API  information it waits for more diag packets to form a full API packet.

DEPENDENCIES
  None.

RETURN VALUE
  Defragment status

SIDE EFFECTS
  None.

===========================================================================*/
qtvdiag_defrag_messg_e_type qtvdiag_defrag_diagpacket(const char* diag_packet_ptr,
                                                      uint32* api_length )
{
  qtvdiag_defrag_messg_e_type defrag_status = QTVDIAG_DEFRAG_ERROR;
  char * api_packet_ptr = &api_arr[0];
  /* Extract the diag fragment descriptor to know whether the diag packet 
   * contains a full api information or part of an api information 
   */
  uint8 diag_frag_desc; 
  UNPACK_UINT8(diag_frag_desc,diag_packet_ptr);
  uint16 valid_diag_payload_len;
  UNPACK_UINT16(valid_diag_payload_len,diag_packet_ptr);
  *api_length += valid_diag_payload_len;


  //Skip 2 bytes which are reserved 
  SKIP_UINT16(diag_packet_ptr);


  /* Swith to the particular defragment descriptor
   *|-----------------------------------------------------------------|
   *| Desc|                     Description                           |
   *|-----------------------------------------------------------------|
   *|  0  |- One or more full API packets in single diag packet       | 
   *|  1  |- First fragment of single API packet                      |
   *|  2  |- Middle fragment of API packet which needs to be attached |
   *|     |  to previous fragment                                     |
   *|  3  |- Last fragment of API packet which needs to be attached to|
   *|     |  previous fragment                                        |
   *|_________________________________________________________________|
   */
  switch (diag_frag_desc)
  {
  case 0:
    {
      //The diag packet contains full api information
      if ((0 == prev_fragment_desc) || (3 == prev_fragment_desc))
      {
        memcpy(api_packet_ptr,diag_packet_ptr,valid_diag_payload_len);
        defrag_status = QTVDIAG_DEFRAGMENT_DONE;
        prev_fragment_desc = 0;
        valid_api_payload = valid_diag_payload_len;
      }
      else
      {
        defrag_status = QTVDIAG_DEFRAG_BADFRAGMENT;
        prev_fragment_desc = 0;
      }
    }
    break;

  case 1:
    {
      /* The diag packet contains  fragment of api information.
       * fragment descriptor tells that the received  fragment 
     * is first part of full api and we can expect some more
     * fragments.
     */
      // Make sure the previous fragment descriptor is 0 or 3
      if ((0 == prev_fragment_desc) || (3 == prev_fragment_desc))
      {

        memcpy(api_packet_ptr,diag_packet_ptr,DIAG_PAYLOAD_SIZE);
        valid_api_payload = DIAG_PAYLOAD_SIZE;
        defrag_status = QTVDIAG_DEFRAG_WAITINGFORMORE;
        prev_fragment_desc = 1;
      }
      else
      {
        defrag_status = QTVDIAG_DEFRAG_BADFRAGMENT;
      }
    }
    break;

  case 2:
    {
      /* The diag packet  contains  fragment  of api  information.
         * fragment  descriptor tells  that the  received  fragment 
       * is middle part of a full api  which needs to be  attached
       * to previous fragmentand we can expect some more fragments.
       */
      // Make sure the previous fragment descriptor is 1 or 2
      if ((1 == prev_fragment_desc) || (2 == prev_fragment_desc))
      {
        api_packet_ptr += valid_api_payload;
        memcpy(api_packet_ptr,diag_packet_ptr,DIAG_PAYLOAD_SIZE);
        valid_api_payload += DIAG_PAYLOAD_SIZE;
        defrag_status = QTVDIAG_DEFRAG_WAITINGFORMORE;
        prev_fragment_desc = 2;
      }
      else
      {
        defrag_status = QTVDIAG_DEFRAG_BADFRAGMENT;
      }
    }
    break;

  case 3:
    {
      /* The diag packet contains  fragment of api information.
         * fragment descriptor tells that the received  fragment 
       * is last part of full api which needs to be attached to
       * previous fragment and no more fragments for this api
       */
      // Make sure the previous fragment descriptor is 1 or 2
      if ((1 == prev_fragment_desc) || (2 == prev_fragment_desc))
      {
        api_packet_ptr += valid_api_payload;
        memcpy(api_packet_ptr,diag_packet_ptr,valid_diag_payload_len);
        valid_api_payload += valid_diag_payload_len;
        defrag_status = QTVDIAG_DEFRAGMENT_DONE;
        prev_fragment_desc = 3;
      }
      else
      {
        defrag_status = QTVDIAG_DEFRAG_BADFRAGMENT;
      }
    }
    break;

  default:
    {
      defrag_status = QTVDIAG_DEFRAG_BADFRAGMENT;
    }
    break;
  }
  return defrag_status;
}

/*===========================================================================

FUNCTION qtvdiag_get_defrag_data_ptr

DESCRIPTION
  Function returns the pointer to defragmented data.
  It also fills the length of defragmented data.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
void* qtvdiag_get_defrag_data_ptr(uint16* defrag_pkt_len_ptr)
{
  *defrag_pkt_len_ptr = valid_api_payload;
  return api_arr;

}

/*===========================================================================

FUNCTION getAPIheaderSize

DESCRIPTION
  Function returns the header size which includes size of timedelta,apiid and
  param length field.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 getAPIheaderSize()
{
  return API_HEADER_SIZE;
}

/*===========================================================================

FUNCTION parseHeader

DESCRIPTION
  Function fills the time delta and total api size values into
  *timeDelta_ptr and *totAPIsize_ptr respectively.
  param length field.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
bool parseHeader(char* api_hdr_ptr, uint32* timeDelta_ptr, uint32* totAPIsize_ptr)
{
  uint16 paramLength;
  UNPACK_UINT32(*timeDelta_ptr,api_hdr_ptr);
  SKIP_UINT16(api_hdr_ptr);
  UNPACK_UINT16(paramLength,api_hdr_ptr);
  *totAPIsize_ptr = paramLength + 8; //8=4(timedelta)+2(apiid)+2(paramlength)
  return TRUE;
} //lint !e818

/*===========================================================================

FUNCTION FetchDnldDataSize

DESCRIPTION
     Offset value is written to the pointer provided by the caller.
  
DEPENDENCIES
  None.

RETURN VALUE
   None
SIDE EFFECTS
  None
===========================================================================*/

void FetchDnldDataSize (uint32 pUserData, uint32 * dnldDataSize, boolean *, QtvPlayer::InstanceHandleT handle)
{
    uint32 handleIndex = 0;
    handleIndex = GetHandleIndex(handle);
    
    if(handleIndex >= QtvPlayer::QTV_PRIORITY_COUNT)
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "handleIndex is out of boundaries");
       return;
    }
    if(PushPullDataBuffer[handleIndex].HTTPBufferOffset == 0)
    {
      PushPullDataBuffer[handleIndex].HTTPBufferOffset = MAXIMUM_MIME_BUFFER_LENGTH; 
    }
    *dnldDataSize = PushPullDataBuffer[handleIndex].HTTPBufferOffset;

    QTV_USE_ARG1(pUserData);

    return;
}

/*===========================================================================

FUNCTION HttpSimulatorStart

DESCRIPTION
   Initializes the values for Http Simulation
  
DEPENDENCIES
  None.

RETURN VALUE
   None
   
SIDE EFFECTS
  None
===========================================================================*/

void HttpSimulatorStart( uint32 downld_rate, uint32 priority)
{
  if(priority >= QtvPlayer::QTV_PRIORITY_COUNT)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "priority is out of boundaries");
     return;
  }
  PushPullDataBuffer[priority].pTimer = NULL;
  /*    video_buffer_ptr = NULL; */
  PushPullDataBuffer[priority].HTTPBufferOffset = 0;

  if(PushPullDataBuffer[priority].buffersize > QTV_MAX_FILE_SIZE)
  {
    PushPullDataBuffer[priority].buffersize = QTV_MAX_FILE_SIZE; 
  }
  downld_rate  = (downld_rate*1024/8)*(QTV_HTTP_SIMULATOR_OFFSET_UPDATE_TIME/1000); //now downld_rate indicates the size of data in bytes
  bufferOffsetData[priority].dnldDataRate = (unsigned long)downld_rate;
  bufferOffsetData[priority].priority = priority;
  PushPullDataBuffer[priority].pTimer = rex_create_timer_ex(HttpSimulatorUpdateBufferOffset, (unsigned long)(&bufferOffsetData[priority]) );
  /* Below if condition is added to fix Klocwork waring */
  if( PushPullDataBuffer[priority].pTimer != NULL)
  {
    rex_set_timer(PushPullDataBuffer[priority].pTimer, QTV_HTTP_SIMULATOR_OFFSET_UPDATE_TIME);
  }
}

/*=========================================================================== 

FUNCTION HttpSimulatorStop

DESCRIPTION
      Terminates Http Simulation and deletes the timer.
  
DEPENDENCIES
  None.

RETURN VALUE
  None
   
SIDE EFFECTS
  None
===========================================================================*/

void HttpSimulatorStop(QtvPlayer::InstanceHandleT handle)
{
    uint32 handleIndex = 0;
    handleIndex = GetHandleIndex(handle);

    if(handleIndex >= QtvPlayer::QTV_PRIORITY_COUNT)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "handleIndex is out of boundaries");
      return;
    }

    PushPullDataBuffer[handleIndex].nInterfaceID = 0;
    PushPullDataBuffer[handleIndex].buffersize = 0;
    PushPullDataBuffer[handleIndex].HTTPBufferOffset = 0;
    PushPullDataBuffer[handleIndex].pHandle = NULL;
    if(PushPullDataBuffer[handleIndex].pTimer)
    {
      rex_delete_timer_ex(PushPullDataBuffer[handleIndex].pTimer);
      PushPullDataBuffer[handleIndex].pTimer = NULL;
    }
    bufferOffsetData[handleIndex].dnldDataRate = 0;
    bufferOffsetData[handleIndex].priority = 0;
}
/*===========================================================================

FUNCTION FetchDnldData

DESCRIPTION
     Updates the destination buffer with the data as per the updated offset
  
DEPENDENCIES
  None.

RETURN VALUE
      number of bytes read
      
SIDE EFFECTS
  None
===========================================================================*/

uint32 FetchDnldData (
      void *      dataBuf,         /* Destination buffer to copy the data */
      uint32      readSize,        /* Amount of data to be read */
      uint32      readPos,         /* Data read offset */
      uint32,                /* Media stream trackId */
      QtvPlayer::InstanceHandleT handle  /* For multiple instances added handle*/
    )
{
    uint32 numRead = 0;
    uint32 handleIndex = 0;
    handleIndex = GetHandleIndex(handle);

    if(handleIndex >= QtvPlayer::QTV_PRIORITY_COUNT)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "handleIndex is out of boundaries");
      return 0;
    }

    if ( readPos<=(PushPullDataBuffer[handleIndex].HTTPBufferOffset-1) )
    {
      /* now read from this location */
      uint32 sizeToRead = MIN((PushPullDataBuffer[handleIndex].HTTPBufferOffset-readPos), readSize);
      if ( sizeToRead > 0 )
      {
        memcpy(dataBuf, (video_buffer_ptr[handleIndex]+readPos), sizeToRead);
        numRead = sizeToRead;
      }
    }
    return numRead;
}

/*===========================================================================

FUNCTION HttpSimulatorUpdateBufferOffset

DESCRIPTION
   Updates the Offset of the buffer for every second so that Qtv can read the available data.

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void HttpSimulatorUpdateBufferOffset(unsigned long bufferOffsetData)
{
   unsigned long dnldDataSize;
   uint32 priority;

   if(bufferOffsetData == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "bufferOffsetData is NULL");
      return;
   }
   dnldDataSize =  ((HttpSimulatorBufferOffsetData*)bufferOffsetData)->dnldDataRate;
   priority = ((HttpSimulatorBufferOffsetData*)bufferOffsetData)->priority;

   PushPullDataBuffer[priority].HTTPBufferOffset += dnldDataSize;  
	  
   if (PushPullDataBuffer[priority].HTTPBufferOffset > PushPullDataBuffer[priority].buffersize)
   {
       PushPullDataBuffer[priority].HTTPBufferOffset  = PushPullDataBuffer[priority].buffersize;	
   }
	 
   if(PushPullDataBuffer[priority].HTTPBufferOffset <= PushPullDataBuffer[priority].buffersize)
   {
      if(PushPullDataBuffer[priority].pTimer)
      {
         if (PushPullDataBuffer[priority].nInterfaceID == QTV_DIAG_PULL_INTERFACE)
         {
             rex_set_timer(PushPullDataBuffer[priority].pTimer, QTV_HTTP_SIMULATOR_OFFSET_UPDATE_TIME);                     
         }
         else if (PushPullDataBuffer[priority].nInterfaceID == QTV_DIAG_PUSH_INTERFACE)		   
         {
             (void)QtvPlayer::UpdatePseudoStreamBufferWriteOffset (PushPullDataBuffer[priority].HTTPBufferOffset, PushPullDataBuffer[priority].pHandle);
             rex_set_timer(PushPullDataBuffer[priority].pTimer, QTV_HTTP_SIMULATOR_OFFSET_UPDATE_TIME);			                      
         }     
      }
  }
  return;
}

/*===========================================================================

FUNCTION GetHandleIndex

DESCRIPTION
   Get the handle index from QtvInstances priority array.
   
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 GetHandleIndex(QtvPlayer::InstanceHandleT handle)
{
  uint32 index = 0;
  uint32 count = 0;
  
  for (; count < QtvPlayer::QTV_PRIORITY_COUNT; count++)
  {
     if(PushPullDataBuffer[count].pHandle == handle)
     {
        index = count;
        break;
     }
  }
  return index;
}

/*===========================================================================

FUNCTION cmcall_inf_alloc

DESCRIPTION
   Initialises the call info ptr which holds data related to call

DEPENDENCIES
  None.

RETURN VALUE
   pointer holding the call information
  
SIDE EFFECTS
  None
===========================================================================*/
cm_mm_call_info_s_type  *cmcall_inf_alloc( void )
{
  static cm_mm_call_info_s_type  call_info;
    /* Currently there is only a need for one call info buffer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First initialize the entire buffer to 0
  */
#ifndef PLATFORM_LTK
  memset( &call_info,
           CM_CALL_EVENT_DEFAULT_FIELD_VALUE,
           sizeof(call_info) );

  /* Initialize the fields to default values
  */
  call_info.call_client_id              = CM_CLIENT_ID_ANONYMOUS;
  call_info.call_id                     = CM_CALL_ID_UNASSIGNED;
  call_info.call_state                  = CM_CALL_STATE_NONE;
  call_info.call_type                   = CM_CALL_TYPE_NONE;

  cm_num_init(&call_info.num);

  call_info.alpha.len                   = 0; //CM_ALPHA_LEN_NONE;
  memset(call_info.alpha.buf,0,sizeof( call_info.alpha.buf) );

  cm_num_init(&call_info.redirecting_number.num);

  call_info.signal.is_signal_info_avail = FALSE;
  call_info.signal.signal_type          = 0;
  call_info.signal.alert_pitch          = 0;
  call_info.signal.signal               = 0;


  call_info.end_status                  = CM_CALL_END_NONE;
  call_info.num_call_ids_ended          = 0;

  memset(&call_info.end_params, 0,  sizeof( cm_end_params_s_type) );
  call_info.end_params.call_id          = CM_CALL_ID_UNASSIGNED;
  call_info.end_params.info_type        = CM_CALL_MODE_INFO_NONE;

  call_info.sys_mode                    = SYS_SYS_MODE_NONE;
  call_info.call_connect_time           = 0;
  call_info.call_end_time               = 0;
  call_info.bytes_received              = 0;
  call_info.bytes_transmitted           = 0;

  memset(&call_info.mode_info, 0, sizeof( call_info.mode_info) );
  call_info.mode_info.info_type         = CM_CALL_MODE_INFO_NONE;


  call_info.line                        = CM_ALS_LINE_NONE;
  call_info.requesting_client_id        = CM_CLIENT_ID_ANONYMOUS;

  #if (defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
       defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM))
#error code not present
  #endif

  call_info.direction                   = CM_CALL_DIRECTION_NONE;
  call_info.modify                      = CM_CALL_MODIFY_NONE;

  call_info.mode_info.info.cdma_call.line_ctrl_polarity_included
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_toggle
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_reverse
                                        = FALSE;
  call_info.mode_info.info.cdma_call.line_ctrl_power_denial
                                        = 0;
  call_info.mode_info.info.cdma_call.orig_fail_layer
                                        = CM_CALL_ORIG_ERR_LAYER_NONE;

#endif /* PLATFORM_LTK */
  /* Simply return a pointer to the statically allocated
  ** call info buffer.
  */
  return &call_info;

} /* cmcall_inf_alloc() */


/*===========================================================================

FUNCTION qtvdiag_exec_api

DESCRIPTION
  Function called in the Diag context used to execute the Qtv APIs.
  
DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
bool qtvdiag_exec_api(char *request_packet_ptr, char* response_pkt_ptr, uint32 curr_api_len)
{
  // Get the API id 
  uint16 api_id;
  char *api_packet_ptr = request_packet_ptr;
  // skip timedelta
  SKIP_UINT32(api_packet_ptr);
  UNPACK_UINT16(api_id,api_packet_ptr);
  //skip api params length
  SKIP_UINT16(api_packet_ptr);

  qtvdiag_cmd_error_code_e_type diag_error_code =
  QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  /* The following flag tells whether api_id,diag_error_code and
     api retVal is filled in the response packet or not         */
  bool retval_flag = false; 

  switch (api_id)
  {
  case QTVDIAG_FRAMEWORK_MUTE_UNMUTE_PLAYBACK_TRACKS_ID:
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      uint32 nTrackSelected, eAction;
      uint32 pHandle;
      UNPACK_UINT32(nTrackSelected,api_packet_ptr);
      UNPACK_UINT32(eAction,api_packet_ptr);
      UNPACK_UINT32(pHandle,api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main MutePlaybackTracks()", 0 );
      if(pHandle)
      {
        retVal = QtvPlayer::MutePlaybackTracks(nTrackSelected, (QtvPlayer::MuteActionT)eAction, (QtvPlayer::InstanceHandleT)pHandle);
      }
      else
      {
        retVal = QtvPlayer::MutePlaybackTracks(nTrackSelected, (QtvPlayer::MuteActionT)eAction);
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif
    }
    break;
  case QTVDIAG_FRAMEWORK_READ_PLAYBACK_TRACKS_ID:
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      uint32 nTrackCount = 0;
      uint32 pHandle;
      QtvPlayer::TrackListT      *pTrackList = NULL;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main ReadPlayBackTracks()", 0 );

      // Get the track count first by calling ReadPlaybackTracks API by having TrackList NULL.
      if(pHandle)
      {
        retVal = QtvPlayer::ReadPlaybackTracks(&nTrackCount, NULL, (QtvPlayer::InstanceHandleT)pHandle);
      }
      else
      {
        retVal = QtvPlayer::ReadPlaybackTracks(&nTrackCount, NULL);
      }

      if (retVal == QtvPlayer::QTV_RETURN_OK)
      {
        retVal = QtvPlayer::QTV_RETURN_ERROR; /*Re-initialize this so that we get error incase malloc 
                                               fails in the next statement*/
        // Allocate memory to TrackList pointer by the track count number.
        pTrackList = (QtvPlayer::TrackListT *)QTV_Malloc((sizeof(QtvPlayer::TrackListT)) * nTrackCount);
        if (pTrackList)
        {
          // Calling again ReadPlaybackTracks API with proper TrackList pointer memory allocated and getting
          // the tracklist details for the given track count.
          memset(pTrackList,0x00,(sizeof(QtvPlayer::TrackListT)) * nTrackCount);
          
          if(pHandle)
          {
            retVal = QtvPlayer::ReadPlaybackTracks(&nTrackCount, pTrackList, (QtvPlayer::InstanceHandleT)pHandle);
          }
          else
          {
            retVal = QtvPlayer::ReadPlaybackTracks(&nTrackCount, pTrackList);
          }

          if (response_pkt_ptr) //this will be null in case of replay testing
          {
            // copy the api id to the response packet
            FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
            //copy the diag error code to the response packet
            FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
            // copy Qtv player return value to the response packet
            FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, retVal);
            // copy the track count to the response packet.
            FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, nTrackCount);
            // copy the track list details in response packet.
            for (int i = 0; i < nTrackCount; i++)
            {
              FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pTrackList[i].nTrackID);
              FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pTrackList[i].mediaType);
              FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pTrackList[i].bmTrackState);
            }
            retval_flag = true;
          }
          // Freeing the track list.
          QTV_Free(pTrackList);
          pTrackList = NULL;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc failed"); 
        }
      }
      else
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                        "QtvPlayer::ReadPlaybackTracks API failed", 0 );
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif
    }
    break;
  case QTVDIAG_FRAMEWORK_SELECT_PLAYBACK_TRACKS_ID:
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
      uint32 nTrackSelected, nAudioTrackId, nVideoTrackId, nTextTrackId;
      uint32 pHandle;
      UNPACK_UINT32(nTrackSelected,api_packet_ptr);
      UNPACK_UINT32(nAudioTrackId,api_packet_ptr);
      UNPACK_UINT32(nVideoTrackId,api_packet_ptr);
      UNPACK_UINT32(nTextTrackId,api_packet_ptr);
      UNPACK_UINT32(pHandle,api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside SelectPlaybackTracks()", 0 );
      if (pHandle)
      {
        retVal = QtvPlayer::SelectPlaybackTracks(nTrackSelected, nAudioTrackId, nVideoTrackId, nTextTrackId,(QtvPlayer::InstanceHandleT)pHandle);
      }
      else
      {
        retVal = QtvPlayer::SelectPlaybackTracks(nTrackSelected, nAudioTrackId, nVideoTrackId, nTextTrackId);
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif
    }
    break;

  case QTVDIAG_FRAMEWORK_INIT_API_ID:
    {
      uint32 prio;
      QtvPlayer::InstanceHandleT * pHandle = NULL;
      UNPACK_UINT32(prio, api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main init()", 0 );
      // Initialize the Qtv Player 
      if (prio == QtvPlayer::QTV_PRIORITY_DEFAULT ||
           prio == QtvPlayer::QTV_PRIORITY_INVALID) 
        //During Init(), invalid priority is acceptible and it is equivalent to default priority
      {
         retVal = QtvPlayer::Init((QtvPlayer::MallocOutputBufferT)qtvdiag_maollc,(QtvPlayer::FreeOutputBufferT)qtvdiag_free);
      }
      else
      {
         pHandle = (QtvPlayer::InstanceHandleT *)QTV_Malloc(sizeof(QtvPlayer::InstanceHandleT));    //2nd inst - high priority
         if(!pHandle)
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
         }
         else
         {
            retVal = QtvPlayer::Init((QtvPlayer::MallocOutputBufferT)qtvdiag_maollc,(QtvPlayer::FreeOutputBufferT)qtvdiag_free, pHandle);
         }
      }

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Init");
      uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        if(pHandle!=NULL)
        {
           FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, *pHandle);
           QTV_Free(pHandle);
           pHandle = NULL;
        }
        retval_flag = true;
      }
    }
    break;

  case QTVDIAG_FRAMEWORK_REGISTER_FOR_CALLBACK_API_ID:
    {
      uint32 pHandle;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main RegisterForCallback()", 0 );
      if(pHandle)
         retVal = QtvPlayer::RegisterForCallback(qtvdiag_callbackFunc, NULL, (QtvPlayer::InstanceHandleT)pHandle, NULL);
      else
	  retVal = QtvPlayer::RegisterForCallback(qtvdiag_callbackFn, NULL);
    }
    break;

  case QTVDIAG_FRAMEWORK_OPEN_URN_API_ID:
    {
      uint32 video_URN_length = 0;            
      uint32 audio_URN_length = 0;
      char *video_URN = NULL;
      char *audio_URN = NULL;
      video_URN_length = (strlen(api_packet_ptr)+1);
      if(video_URN_length >= curr_api_len || video_URN_length <= MIN_URN_LEN)
      {
        ASSERT(0);
      }
      video_URN = (char *)QTV_Malloc(video_URN_length);
      if (NULL != video_URN)
      {
        memcpy(video_URN,api_packet_ptr,video_URN_length);
      }
      else
      {
        ASSERT(0);
      }
      api_packet_ptr += video_URN_length;
      audio_URN_length = (strlen(api_packet_ptr)+1);
      if(audio_URN_length >= curr_api_len)
      {
        ASSERT(0);
      }
      if(audio_URN_length <= MIN_URN_LEN)
      {
        audio_URN = video_URN;
      }
      else
      {
        audio_URN = (char *)QTV_Malloc(audio_URN_length);
        if (NULL != audio_URN)
        {
          memcpy(audio_URN,api_packet_ptr,audio_URN_length);
        }
        else
        {
          ASSERT(0);
        }
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main OpenURN()", 0 );
      retVal = QtvPlayer::OpenURN(video_URN, audio_URN);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Opened URN");
      if (NULL!=video_URN)
      {
        QTV_Free(video_URN);
      }
      if (NULL!=audio_URN)
      {
        QTV_Free(audio_URN);
      }
      /*Register diag with conc_mgr to process call scenario*/
      if(pQtvDiagApp->call_info_ptr)
      {
          if(qtv_conc_mgr::register_qtv_cb_with_diag())
          {
             if(pQtvDiagApp->call_info_ptr->call_info) //If already in call, call callback now
             {
                pQtvDiagApp->call_info_ptr->call_event_func(pQtvDiagApp->call_info_ptr->call_event,
                                                            pQtvDiagApp->call_info_ptr->call_info);
             }
             else
             {
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Call not yet set up");
             }
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Registration with conc mgr successful");
          }
          else
          {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Registration with conc mgr failed");
          }
       }
    }
    break;
  case QTVDIAG_FRAMEWORK_OPEN_URN2_API_ID:
    {
      uint32 video_URN_length = 0;
      uint32 audio_URN_length = 0;
      uint32 text_URN_length = 0;
      char *video_URN = NULL;
      char *audio_URN = NULL;
      char *text_URN = NULL;
      uint32 pHandle,priority;
      video_URN_length = (strlen(api_packet_ptr)+1);
      if(video_URN_length >= curr_api_len || video_URN_length <= MIN_URN_LEN)
      {
        ASSERT(0);
      }
      video_URN = (char *)QTV_Malloc(video_URN_length);
      if (NULL != video_URN)
      {
        memcpy(video_URN,api_packet_ptr,video_URN_length);
      }
      else
      {
        ASSERT(0);
      }
      api_packet_ptr += video_URN_length;
      audio_URN_length = (strlen(api_packet_ptr)+1);
      if(audio_URN_length >= curr_api_len)
      {
        ASSERT(0);
      }
      if(audio_URN_length <= MIN_URN_LEN)
      {
        audio_URN = video_URN;
      }
      else
      {
        audio_URN = (char *)QTV_Malloc(audio_URN_length);
        if (NULL != audio_URN)
        {
          memcpy(audio_URN,api_packet_ptr,audio_URN_length);
        }
        else
        {
          ASSERT(0);
        }
      }
      api_packet_ptr += audio_URN_length;
      text_URN_length = (strlen(api_packet_ptr)+1);
      if(text_URN_length >= curr_api_len)
      {
        ASSERT(0);
      }
      if(text_URN_length <= MIN_URN_LEN)
      {
        text_URN = video_URN;
      }
      else
      {
        text_URN = (char *)QTV_Malloc(text_URN_length);
        if (NULL != text_URN)
        {
          memcpy(text_URN,api_packet_ptr,text_URN_length);
        }
        else
        {
          ASSERT(0);
        }
      }
	  api_packet_ptr += text_URN_length;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      UNPACK_UINT32(priority, api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main OpenURN()", 0 );
      if(pHandle)
      {
        retVal = QtvPlayer::OpenURN(video_URN, audio_URN,text_URN,(QtvPlayer::InstanceHandleT) pHandle, (QtvPlayer::InstancePriorityT)priority);
      }
      else
      {
        retVal = QtvPlayer::OpenURN(video_URN, audio_URN,text_URN);
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Opened URN");
      if (NULL!=video_URN)
      {
        QTV_Free(video_URN);
      }
      if (NULL!=audio_URN)
      {
        QTV_Free(audio_URN);
      }
      if (NULL!=text_URN)
      {
        QTV_Free(text_URN);
      }

      /*Register diag with conc_mgr to process call scenario*/
      if(pQtvDiagApp->call_info_ptr)
      {
         if(qtv_conc_mgr::register_qtv_cb_with_diag())
         { 
            /* If already in call, call callback now */
            if(pQtvDiagApp->call_info_ptr->call_info) 
            {
               pQtvDiagApp->call_info_ptr->call_event_func(pQtvDiagApp->call_info_ptr->call_event,
                                                           pQtvDiagApp->call_info_ptr->call_info);
            }
            else
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Call not yet setup");
            }
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                                          "Registration with conc mgr successful");
         }
         else
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "CM registration failed");
         }
      }
    }
    break;
case QTVDIAG_FRAMEWORK_OPEN_URN3_API_ID:
    {
      uint32 video_URN_length = 0;            
      char *video_URN = NULL;
      uint32 video_BUF_size = 0;

      video_URN_length = (strlen(api_packet_ptr)+1);
      if(video_URN_length >= curr_api_len || video_URN_length <= MIN_URN_LEN)
      {
        ASSERT(0);
      }
      video_URN = (char *)QTV_Malloc(video_URN_length);
      if (NULL != video_URN)
      {
        memcpy(video_URN,api_packet_ptr,video_URN_length);
      }
      else
      {
        ASSERT(0);
      }
      api_packet_ptr += video_URN_length;
      UNPACK_UINT32(video_BUF_size,api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main OpenURN()", 0 );
      if(video_BUF_size < 1048576)
      {
         
         OSCL_FILE * fp = OSCL_FileOpen(video_URN, "r");
         video_BUF_size = OSCL_FileSize( video_URN );
         video_buffer_ptr[0] = (unsigned char *)QTV_Malloc(video_BUF_size);
         if(!video_buffer_ptr[0])
	  {
	    ASSERT(0);
	  }
 	  OSCL_FileRead (video_buffer_ptr[0], video_BUF_size, 1, fp);
	  retVal = QtvPlayer::OpenURN(video_buffer_ptr[0],video_BUF_size,video_buffer_ptr[0],video_BUF_size);
         OSCL_FileClose(fp);
      	}
       else
	{
	  retVal = QtvPlayer::QTV_RETURN_ERROR;
	}
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Opened URN");
	  
      if (NULL!=video_URN)
      {
        QTV_Free(video_URN);
      }
    }
    break;
	
  case QTVDIAG_FRAMEWORK_OPEN_URN4_API_ID:
    {
      diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_UNSUPPORTED;
      retVal = QtvPlayer::QTV_RETURN_ERROR;
    }
    break;
	
  case QTVDIAG_FRAMEWORK_PLAY_API_ID:
    {
      int32 start_position, end_position;
      uint32 pHandle;
      UNPACK_UINT32(start_position,api_packet_ptr);
      UNPACK_UINT32(end_position,api_packet_ptr);
      UNPACK_UINT32(pHandle,api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main PlayClip()", 0 );
      retVal = QtvPlayer::PlayClip(start_position,end_position,(QtvPlayer::InstanceHandleT)pHandle); 
    }
    break;

  case QTVDIAG_FRAMEWORK_TERMINATE_API_ID:
    {
      uint32 pHandle;
      uint32 handleIndex;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      handleIndex = GetHandleIndex((QtvPlayer::InstanceHandleT)pHandle);
      // Ensure that handleIndex doesn't exceeds the limit
      if((handleIndex < QtvPlayer::QTV_PRIORITY_COUNT) && (video_buffer_ptr[handleIndex]))
      {
        QTV_Free(video_buffer_ptr[handleIndex]);
        video_buffer_ptr[handleIndex] = NULL;
      }

      if(handleIndex >= QtvPlayer::QTV_PRIORITY_COUNT)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "handleIndex is out of boundaries");
      }

      if(pQtvDiagApp->call_info_ptr)  //Deregister diag(call mgr) with Qtv
      {
         pQtvDiagApp->call_info_ptr->call_event_func = NULL;
      }
      HttpSimulatorStop((QtvPlayer::InstanceHandleT)pHandle);
      retVal = QtvPlayer::Terminate((QtvPlayer::InstanceHandleT)pHandle);
    }
    break;

  case QTVDIAG_FRAMEWORK_PAUSE_API_ID:
    {
      uint32 pHandle;
      UNPACK_UINT32(pHandle,api_packet_ptr);
      retVal = QtvPlayer::Pause((QtvPlayer::InstanceHandleT)pHandle);
    }
    break;

  case QTVDIAG_FRAMEWORK_SET_ISDBT_STATUS:
   {
      uint32 mode;
      UNPACK_UINT32(mode,api_packet_ptr);
      pQtvDiagApp->BCastResumeModeFromPause = (BCastResumeModeFromPauseT)mode;
      retVal = QtvPlayer::QTV_RETURN_OK;
   }
   break;
	
  case QTVDIAG_FRAMEWORK_STOP_API_ID:
    {
      uint32 pHandle = 0;
      HttpSimulatorStop((QtvPlayer::InstanceHandleT)pHandle);
      UNPACK_UINT32(pHandle, api_packet_ptr);
      retVal = QtvPlayer::Stop((QtvPlayer::InstanceHandleT)pHandle);
    }
    break;

  case QTVDIAG_FRAMEWORK_PLAY_NEXT_FRAME_API_ID:
    {
      retVal = QtvPlayer::PlayNextFrame();
    }
    break;

  case QTVDIAG_FRAMEWORK_PLAY_PREVIOUS_FRAME_API_ID:
    {
      retVal = QtvPlayer::PlayPrevFrame();
    }
    break;

  case QTVDIAG_FRAMEWORK_SEEK_API_ID:
    { 
      // This is based on the code in OEMMediaMPEG4      
      QtvPlayer::ClipInfoT clipInfo;

      // Make sure we're allowed to reposition within the clip.
      retVal = QtvPlayer::GetClipInfo(clipInfo);
      if ((retVal != QtvPlayer::QTV_RETURN_OK) || !clipInfo.RepositioningAllowed)
      {
        diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
        break;
      }

      int32 offset = *((int32 *) api_packet_ptr); //lint !e826

      if (offset >= 0)
      {
        offset = MIN(offset, clipInfo.Duration);
      }
      else
      {
        offset = MAX(offset, 0);
      }

      retVal = QtvPlayer::PlayClip(offset, -1);
    }
    break; 


  case QTVDIAG_FRAMEWORK_SKIP_API_ID:
#ifdef FEATURE_FILE_FRAGMENTATION
    {
      int32 skipNumber = (int32) *((int32 *) api_packet_ptr); //lint !e826
      retVal = QtvPlayer::SkipClip(skipNumber);
    }
#else
    { /*lint !e527 */
      QTV_MSG_PRIO(QTVDIAG_STREAM_RECORDING, QTVDIAG_PRIO_HIGH, 
                   "Stream Recording not supported in this build");

      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
    }
#endif // #ifdef FEATURE_FILE_FRAGMENTATION
    break;

    // This should match the #ifdef whether RotateVideo() and ScaleVideo() are
    // defined.
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  case QTVDIAG_FRAMEWORK_ROTATE_VIDEO_API_ID:
    {
      QtvPlayer::RotationType direction = (QtvPlayer::RotationType) *((int16 *)api_packet_ptr);
      retVal = QtvPlayer::RotateVideo(direction);
      break; 
    }

  case QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID:
    {
      QtvPlayer::ScalingType scale = (QtvPlayer::ScalingType) *((int16 *)api_packet_ptr);
      retVal = QtvPlayer::ScaleVideo(scale);
      break; 
    }
#else
  case QTVDIAG_FRAMEWORK_ROTATE_VIDEO_API_ID:
  case QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID:
    { 
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
      break;
    }
#endif
#ifdef FEATURE_QTV_XSCALE_VIDEO
  case QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID_NEW:
    {
      int32 nPicHeight,nPicWidth;
      uint32 pHandle;
      UNPACK_UINT32(nPicWidth,api_packet_ptr);
      UNPACK_UINT32(nPicHeight,api_packet_ptr);
      UNPACK_UINT32(pHandle,api_packet_ptr);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main ScaleVideo()", 0 );
      retVal = QtvPlayer::ScaleVideo(nPicWidth, nPicHeight, (QtvPlayer::InstanceHandleT)pHandle);
      break;
   }
#endif
#if ( defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
  case QTVDIAG_FRAMEWORK_SET_PLAYBACK_SPEED:
    {
       uint32 nPlayBackSpeed;
       uint32 pHandle;
       UNPACK_UINT32(nPlayBackSpeed,api_packet_ptr);
       UNPACK_UINT32(pHandle,api_packet_ptr);
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main SetPlaybackSpeed()", 0 );       
       retVal = QtvPlayer::SetPlaybackSpeed((QtvPlayer::PlaybackSpeedT)nPlayBackSpeed, (QtvPlayer::InstanceHandleT)pHandle);
       break;
   }
#endif   
  case QTVDIAG_FRAMEWORK_PLAY_SYNC_FRAME:
    {
#ifdef FEATURE_QTV_GENERIC_BCAST
       int32 nSyncOffset;
       uint32 nTID;
       uint32 pHandle;
       
       UNPACK_UINT32(nSyncOffset, api_packet_ptr);
       UNPACK_UINT32(nTID, api_packet_ptr);
       UNPACK_UINT32(pHandle, api_packet_ptr);
       nTID = 0;
       
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main PlaySyncFrame()", 0 );           
       retVal = QtvPlayer::PlaySyncFrame((sint31)nSyncOffset, (uint32 *)&nTID,(QtvPlayer::InstanceHandleT)pHandle);
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif
       break;
    }  
    case QTVDIAG_FRAMEWORK_SET_USER_AGENT:
    {
       char *pUserAgentName = NULL;
       uint32 pHandle;
       uint32 UserAgentlength = 0;            
       UserAgentlength  = (strlen(api_packet_ptr)+1);

       pUserAgentName  = (char *)QTV_Malloc(UserAgentlength);
	  
       if (NULL != pUserAgentName)
       {
          memcpy(pUserAgentName,api_packet_ptr,UserAgentlength);
       }
       else
       {
          ASSERT(0);
       }
	  
       api_packet_ptr += UserAgentlength;

       UNPACK_UINT32(pHandle, api_packet_ptr);
       
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main SetUserAgent()", 0 );           
       retVal = QtvPlayer::SetUserAgent(pUserAgentName,(QtvPlayer::InstanceHandleT)pHandle);

	if (NULL != pUserAgentName)
	{
	  QTV_Free(pUserAgentName); 
           pUserAgentName = NULL;
	}
        break;
       }      
case QTVDIAG_FRAMEWORK_OPEN_PSEUDO_STREAM :
  {
      uint32 pHandle, priority, downld_rate =0;
      uint32 buffer_length = strlen(api_packet_ptr)+1;
      char * buffer_name = (char *)QTV_Malloc(buffer_length);

      PushPullDataBuffer[0].nInterfaceID = QTV_DIAG_PUSH_INTERFACE;

      if(buffer_name != NULL){
        memcpy(buffer_name, api_packet_ptr, buffer_length);
      }
      else
      {
        ASSERT(0);
      }
	  
      api_packet_ptr += buffer_length;
      UNPACK_UINT32(downld_rate,api_packet_ptr);
      UNPACK_UINT32(pHandle, api_packet_ptr);
      UNPACK_UINT32(priority, api_packet_ptr);
      OSCL_FILE * fp = OSCL_FileOpen(buffer_name, "r");
      PushPullDataBuffer[0].buffersize = OSCL_FileSize( buffer_name );

      if (video_buffer_ptr[0] )
      	{
      	    QTV_Free(video_buffer_ptr[0] );
	    video_buffer_ptr[0] = NULL;
			
      	}
      video_buffer_ptr[0] = (unsigned char *)QTV_Malloc(PushPullDataBuffer[0].buffersize);
      if(!video_buffer_ptr[0])
      {
           ASSERT(0);
      }
      buffer_length = OSCL_FileRead (video_buffer_ptr[0], PushPullDataBuffer[0].buffersize,1, fp); //buffer_length variable reused since this is not needed anymore

      if(pHandle)
      {
           PushPullDataBuffer[0].pHandle = (QtvPlayer::InstanceHandleT)pHandle;

           retVal = QtvPlayer::OpenPseudoStream(video_buffer_ptr[0], buffer_length,MAXIMUM_MIME_BUFFER_LENGTH ,
                                              (QtvPlayer::InstanceHandleT)pHandle,
                                              (QtvPlayer::InstancePriorityT)priority);
      }
      else
      {
           retVal = QtvPlayer::OpenPseudoStream(video_buffer_ptr[0], buffer_length, MAXIMUM_MIME_BUFFER_LENGTH);
      }

      HttpSimulatorStart(downld_rate, priority); //initialise the variables for pull interface
      
      OSCL_FileClose(fp);
      if (NULL!=buffer_name)
      {
           QTV_Free(buffer_name);
	    buffer_name = NULL;
      }
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
          diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
      }    
      break;
  }
  case QTVDIAG_FRAMEWORK_OPEN_PSEUDO_STREAM_3_ARGS: /* Open Pseudo stream with track selection */
  {
      uint32 pHandle, priority, downld_rate =0,track_selected = 0;
      uint32 buffer_length = strlen(api_packet_ptr)+1;
      char * buffer_name = (char *)QTV_Malloc(buffer_length);

      PushPullDataBuffer[0].nInterfaceID = QTV_DIAG_PUSH_INTERFACE;

      if(buffer_name != NULL){
        memcpy(buffer_name, api_packet_ptr, buffer_length);
      }
      else
      {
        ASSERT(0);
      }
	  
      api_packet_ptr += buffer_length;
      UNPACK_UINT32(downld_rate,api_packet_ptr);
      UNPACK_UINT32(track_selected,api_packet_ptr);      
      UNPACK_UINT32(pHandle, api_packet_ptr);
      UNPACK_UINT32(priority, api_packet_ptr);
      OSCL_FILE * fp = OSCL_FileOpen(buffer_name, "r");
      PushPullDataBuffer[0].buffersize = OSCL_FileSize( buffer_name );

	if (video_buffer_ptr[0] )
      	{
      	    QTV_Free(video_buffer_ptr[0] );
	    video_buffer_ptr[0] = NULL;
			
      	}
	  
      video_buffer_ptr[0] = (unsigned char *)QTV_Malloc(PushPullDataBuffer[0].buffersize);
      if(!video_buffer_ptr[0])
      {
           ASSERT(0);
      }
      buffer_length = OSCL_FileRead (video_buffer_ptr[0], PushPullDataBuffer[0].buffersize,1, fp); //buffer_length variable reused since this is not needed anymore

      if(pHandle)
      {
           PushPullDataBuffer[0].pHandle = (QtvPlayer::InstanceHandleT)pHandle;

           retVal = QtvPlayer::OpenPseudoStream(video_buffer_ptr[0], buffer_length,MAXIMUM_MIME_BUFFER_LENGTH ,
		   	                         track_selected,
                                              (QtvPlayer::InstanceHandleT)pHandle,
                                              (QtvPlayer::InstancePriorityT)priority);
      }
      else
      {
           retVal = QtvPlayer::OpenPseudoStream(video_buffer_ptr[0], buffer_length, MAXIMUM_MIME_BUFFER_LENGTH,track_selected);
      }

      HttpSimulatorStart(downld_rate, priority); //initialise the variables for pull interface
      
      OSCL_FileClose(fp);
      if (NULL!=buffer_name)
      {
           QTV_Free(buffer_name);
	    buffer_name = NULL;
      }
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
          diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
      }    
	break;
  }
#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  case QTVDIAG_FRAMEWORK_SET_DUAL_MONO_OUTPUT:
    {
       uint32 nDualMonoOutPut;
       uint32 pHandle;
       
       UNPACK_UINT32(nDualMonoOutPut, api_packet_ptr);
       UNPACK_UINT32(pHandle, api_packet_ptr);
       
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main SetDualMonoOutput()", 0 );           
       if(pHandle)
       {
           retVal = QtvPlayer::SetDualMonoOutput((QtvPlayer::DualMonoOutputT)nDualMonoOutPut, (QtvPlayer::InstanceHandleT)pHandle);
       }
       else
       {
           retVal = QtvPlayer::SetDualMonoOutput((QtvPlayer::DualMonoOutputT)nDualMonoOutPut);
       }   
       break;
    }  
#endif    
  case QTVDIAG_FRAMEWORK_REWIND_API_ID:
  case QTVDIAG_FRAMEWORK_FFWD_API_ID:
    {
      QtvPlayer::PlayerStateRecordT playerState;
      uint32 pHandle;
      int32 seekTime;// = (int32) *((int32 *) api_packet_ptr); //lint !e826
      UNPACK_UINT32(seekTime,api_packet_ptr);
      UNPACK_UINT32(pHandle, api_packet_ptr);
      retVal = QtvPlayer::GetPlayerState(playerState, (QtvPlayer::InstanceHandleT)pHandle);

      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        break;
      }

      int32 currTime = (int32)playerState.playbackMsec;

      if (api_id == (uint16)QTVDIAG_FRAMEWORK_REWIND_API_ID)
      {
        seekTime = -seekTime;
      }

      seekTime += currTime;

      if (seekTime < 0)
      {
        seekTime = 0;
      }

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                    "Repositioning clip to %d", seekTime);

      retVal = QtvPlayer::PlayClip(seekTime, -1,(QtvPlayer::InstanceHandleT)pHandle);
      break;
    }

  case QTVDIAG_FRAMEWORK_START_STOP_API_ID:
    {
      retVal = QtvPlayer::PlayClip(0, -1);

      int32 sleepTime = *((int32 *) api_packet_ptr); //lint !e826
      zrex_sleep((uint32)sleepTime);

      retVal = QtvPlayer::Terminate();
      break;
    }
  case QTVDIAG_FRAMEWORK_PAUSE_RESUME_API_ID:
    {
      retVal = QtvPlayer::PlayClip(-1,-1);
      break;
    }
  
case QTVDIAG_FRAMEWORK_SUSPEND: 
    {
      int value = 1;
      QtvPlayer::SetQTVConfigItem(QtvConfig::QTVCONFIG_PAUSE_IS_SUSPEND, &value);//lint !e534
      retVal = QtvPlayer::Pause();
      break;
    }

    // Required for testing In-Call Qtv
    case QTVDIAG_FRAMEWORK_SET_AUDIO_SOURCE: 
    {
      int source_val;
      UNPACK_UINT32(source_val, api_packet_ptr);
      QtvPlayer::AudioSourceFormatT source = (QtvPlayer::AudioSourceFormatT) source_val;
      retVal = QtvPlayer::SetAudioSource(source);
      break;
    }
	
    case QTVDIAG_FRAMEWORK_GET_3GPP_TIMEDTEXT_INFO:
  	{
	  QtvPlayer::ThreeGPPTimedTextInfo Info;
	  boolean retval = false;
	  retval = QtvPlayer::Get3GPPTimedTextInfo(&Info);
	  if (retval == false)
         {
           diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
           break;
         }
	  uint16 ret = (uint16)retval;
         if (response_pkt_ptr) //this will be null in case of replay testing
     	  {
        	// copy the api id to the response packet
        	FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        	//copy the diag error code to the response packet
        	FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        	// copy Qtv player return value to the response packet
        	FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        	FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwTextDataSize);
        	FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwDisplayFlags);
        	FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwHorzJustification);
        	FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwVertJustification);
        	FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwBoxTop);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwBoxLeft);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwBoxBottom);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwBoxRight);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwStartChar);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwEndChar);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwFontID);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwFontStyleFlags);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwFontSize);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwFontListSize);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwDuration);
		FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, Info.dwSampleModifiersSize);
		
        	retval_flag = true;
      	}
    }
  break;
  
  #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  
  case QTVDIAG_FRAMEWORK_GET_DOWNLOAD_TIME:
  {
      QtvPlayer::PlayerStateRecordT state;
      retVal = QtvPlayer::GetDownloadTime(state);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
         break;
      }
	
      uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, state.audioFrames);
	 FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, state.videoFrames);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.playbackMsec);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.videoPlaybackMsec);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.audioPlaybackMsec);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.textPlaybackMsec);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.playbackOffsetinBytes);
      	 retval_flag = true;
      	}
  }
  break;
  
  #endif
  
  case QTVDIAG_FRAMEWORK_GET_RAW_FRAME_INFO:
   {
      uint32 lTime, tracktype;
      QtvPlayer::RawFrameInfo pFrameInfo;
      UNPACK_UINT32(tracktype, api_packet_ptr);
      UNPACK_UINT32(lTime, api_packet_ptr);
      retVal = QtvPlayer::GetRawFrameInfo((QtvPlayer::TrackType)tracktype, lTime, &pFrameInfo);
       if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
	  break;
       }
	uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameNum);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameSize);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameOffset);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameTime);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.Framedelta);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.IsSyncFrame);
	 retval_flag = true;
      }
   }
  break;

  case QTVDIAG_FRAMEWORK_GET_LARGEST_RAW_FRAME_SIZE:
   {
      uint32 pFramesize, tracktype;
      UNPACK_UINT32(tracktype, api_packet_ptr);
      retVal = QtvPlayer::GetLargestRawFrameSize((QtvPlayer::TrackType)tracktype, &pFramesize);
       if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
	  break;
       }
	uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFramesize);
	 retval_flag = true;
      }
   }
  break;
  
  case QTVDIAG_FRAMEWORK_GET_RAW_TRACK_HEADER:
   {
      uint32 pBufsize, tracktype;
      UNPACK_UINT32(tracktype, api_packet_ptr);
      byte *pBuf;
      pBufsize = 30;
      pBuf = (byte *)(QTV_Malloc(pBufsize*(sizeof(byte))));
      if(pBuf)
      {
        retVal = QtvPlayer::GetRawTrackHeader((QtvPlayer::TrackType)tracktype, pBuf, &pBufsize);
        QTV_Free(pBuf);
      }
      else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
	  break;
       }
	uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pBufsize);
	 
	 retval_flag = true;
      }
   }
  break;
  
  case QTVDIAG_FRAMEWORK_GET_NUM_FRAMES:
   {
      uint32 tracktype;
      uint32 pNumframes;
      UNPACK_UINT32(tracktype, api_packet_ptr);
      retVal = QtvPlayer::GetNumFrames((QtvPlayer::TrackType)tracktype, &pNumframes);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
	  break;
       }
	uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pNumframes);
	 
	 retval_flag = true;
      }
   }
  break;

  case QTVDIAG_FRAMEWORK_GET_RAW_FRAME:
  {
      uint32 tracktype;
      uint32 FrameNum = 0;
      QtvPlayer::RawFrameInfo pFrameInfo;
      uint32 bufSize = 50;
      byte *pBuf = (byte *)QTV_Malloc(50*sizeof(byte));
      UNPACK_UINT32(tracktype, api_packet_ptr);
      if(pBuf)
      {
        retVal = QtvPlayer::GetRawFrame((QtvPlayer::TrackType)tracktype, FrameNum, &pFrameInfo, pBuf, bufSize);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
	  break;
       }
	uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameNum);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameSize);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameOffset);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.FrameTime);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.Framedelta);
	 FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pFrameInfo.IsSyncFrame);
	 retval_flag = true;
      }
  }
  break;

  case QTVDIAG_FRAMEWORK_GET_PLAYER_STATE:
  {
      QtvPlayer::PlayerStateRecordT state;
      uint32 pHandle;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      retVal = QtvPlayer::GetPlayerState(state,(QtvPlayer::InstanceHandleT)pHandle);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
         break;
      }
      uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
         // copy the api id to the response packet
         FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
         //copy the diag error code to the response packet
         FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
         // copy Qtv player return value to the response packet
         FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

	  FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, state.audioFrames);
	  FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, state.videoFrames);
	  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.playbackMsec);
	  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.videoPlaybackMsec);
	  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.audioPlaybackMsec);
	  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.textPlaybackMsec);
	  FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, state.playbackOffsetinBytes);
      	  retval_flag = true;
      }
  }
  break;
  
  case QTVDIAG_FRAMEWORK_OPEN_PULL_BUFFERED_STREAM:
  {   
      uint32 pHandle, priority, downld_rate =0;
      uint32 buffer_length = strlen(api_packet_ptr)+1;
      char * buffer_name = (char *)QTV_Malloc(buffer_length);
      if(buffer_name != NULL){
        memcpy(buffer_name, api_packet_ptr, buffer_length);
      }
      else
      {
        ASSERT(0);
      }
	  
      api_packet_ptr += buffer_length;
      UNPACK_UINT32(downld_rate,api_packet_ptr);
      UNPACK_UINT32(pHandle, api_packet_ptr);
      UNPACK_UINT32(priority, api_packet_ptr);
      OSCL_FILE * fp = OSCL_FileOpen(buffer_name, "r");
      PushPullDataBuffer[priority].buffersize = OSCL_FileSize( buffer_name );
      PushPullDataBuffer[priority].nInterfaceID = QTV_DIAG_PULL_INTERFACE;
      PushPullDataBuffer[priority].pHandle = (QtvPlayer::InstanceHandleT)pHandle;
      HttpSimulatorStart(downld_rate, priority); //initialise the variables for pull interface
      
      if(video_buffer_ptr[priority] == NULL)
      {
        video_buffer_ptr[priority] = (unsigned char *)QTV_Malloc(PushPullDataBuffer[priority].buffersize);
        if(!video_buffer_ptr[priority])
        {
          ASSERT(0);
        }
      }
      buffer_length = OSCL_FileRead (video_buffer_ptr[priority], PushPullDataBuffer[priority].buffersize,1, fp); //buffer_length variable reused since this is not needed anymore
      if(pHandle)
      {
        retVal = QtvPlayer::OpenPullBufferedStream((QtvPlayer::FetchBufferedDataSizeT)FetchDnldDataSize, (QtvPlayer::FetchBufferedDataT) FetchDnldData, 0x07,(QtvPlayer::InstanceHandleT)pHandle,(QtvPlayer::InstancePriorityT)priority);
      }
      else
      {
        retVal = QtvPlayer::OpenPullBufferedStream((QtvPlayer::FetchBufferedDataSizeT)FetchDnldDataSize, (QtvPlayer::FetchBufferedDataT) FetchDnldData, 0x07);
      }
      OSCL_FileClose(fp);
      if (NULL!=buffer_name)
      {
        QTV_Free(buffer_name);
	 buffer_name = NULL;
      }
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
        break;
      }    
  }
  break;

  case QTVDIAG_FRAMEWORK_GET_CLIP_INFO_API_ID:
    {
      // This is based on the code in OEMMediaMPEG4      
      QtvPlayer::ClipInfoT clipInfo;
      uint8 authorLengthFlag = 0;
      uint32 pHandle;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      // Make sure we're allowed to reposition within the clip.
      retVal = QtvPlayer::GetClipInfo(clipInfo, (QtvPlayer::InstanceHandleT)pHandle);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
        break;
      }
      uint16 ret = (uint16)retVal;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.streamtype);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.RepositioningAllowed);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.AudioOnlyPlaybackAllowed);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.VideoOnlyPlaybackAllowed);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.MediaType);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.TrackCount);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.Width);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.Height);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.Duration);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.Size);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.AudioSamplingFreq);
        FILL_RESPONSE_PACKET_FLOAT(response_pkt_ptr, clipInfo.VideoFrameRate);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.NumAudioChannels);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.VideoEncodedBitRate);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, clipInfo.AudioEncodedBitRate);
        for (int i = 0; i < (clipInfo.TrackCount); i++)
        {
          FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, clipInfo.TrackTypes[i]);
        }
        if(strlen(clipInfo.ExtInfo.StdInfo.Author) > 0)
	 {
	   authorLengthFlag = 1;
          FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, authorLengthFlag);
	   memcpy(response_pkt_ptr, clipInfo.ExtInfo.StdInfo.Author, QTV_MAX_AUTHOR_BYTES);
        }
        else
	 {
	   FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, authorLengthFlag);
	 }

        retval_flag = true;
      }
      break;
    }
 case QTVDIAG_FRAMEWORK_SET_QTV_CONFIG_ITEM:
 {
    uint32 config_item,value;
    UNPACK_UINT32(config_item, api_packet_ptr);
    UNPACK_UINT32(value, api_packet_ptr);
    if(QtvPlayer::SetQTVConfigItem((QtvConfig::QtvConfigItemId)config_item, 
                                                     (int *) (&value)))
    {
        retVal = QtvPlayer::QTV_RETURN_OK;
    }
    break;
 }
 case QTVDIAG_CALLBACK_FOR_CM:
  {
     uint32 call_event_input, call_type, mode_info, call_state;
     uint32 call_accept;
     UNPACK_UINT32(call_event_input, api_packet_ptr);
     UNPACK_UINT32(call_type, api_packet_ptr);
     UNPACK_UINT32(call_state, api_packet_ptr);
     UNPACK_UINT32(mode_info, api_packet_ptr);
     UNPACK_UINT32(call_accept, api_packet_ptr);
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Processing call event");
     
     if(pQtvDiagApp->call_info_ptr)
     {
         pQtvDiagApp->call_info_ptr->call_info = cmcall_inf_alloc();
         pQtvDiagApp->call_info_ptr->call_info->call_id = 0x2;
         pQtvDiagApp->call_info_ptr->call_event = (cm_call_event_e_type)call_event_input;
         pQtvDiagApp->call_info_ptr->call_info->call_type =
                                                  (cm_call_type_e_type)call_type;
         pQtvDiagApp->call_info_ptr->call_info->call_state = 
                                                  (cm_call_state_e_type)call_state;
         pQtvDiagApp->call_info_ptr->call_info->mode_info.info_type = 
                                        (cm_call_mode_info_e_type)mode_info;
         pQtvDiagApp->call_info_ptr->call_info->mode_info.info.cdma_call.srv_opt = 0x0003; //CAI_SO_VOICE_EVRC
         pQtvDiagApp->call_info_ptr->call_info->mode_info.info.gw_cs_call.accept =
                                                           (call_accept == 0) ? false : true;
          if(pQtvDiagApp->call_info_ptr->call_event_func)
          {
             pQtvDiagApp->call_info_ptr->call_event_func(pQtvDiagApp->call_info_ptr->call_event, 
                                                         pQtvDiagApp->call_info_ptr->call_info);
             pQtvDiagApp->call_info_ptr->call_info = NULL;
          }
          else
          {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                            "Qtv diag not yet registered with concurrency manager");
          }
          retVal = QtvPlayer::QTV_RETURN_OK;
       }
    }
  break;

case QTVDIAG_FRAMEWORK_GET_FRAME_INFO_API_ID:
    {   
      diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
      retVal  = QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
      uint16 ret = (uint16)retVal;
	  
      if (response_pkt_ptr  &&  pQtvDiagApp != NULL ) 
      {

	  diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
         retVal  = QtvPlayer::QTV_RETURN_OK;
         ret = (uint16)retVal;
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, pQtvDiagApp->nFrameInfoData.Height);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, pQtvDiagApp->nFrameInfoData.Width  );
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pQtvDiagApp->nFrameInfoData.Timestamp);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, pQtvDiagApp->nFrameInfoData.NumConcealedMB);
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, pQtvDiagApp->nFrameInfoData.bI_VOP  );
        retval_flag = true;
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_VIDEO_CODEC_TYPE_API_ID:
    {
      QtvPlayer::CodecType codectype;
      retVal = QtvPlayer::QTV_RETURN_OK;
      uint32 pHandle;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      codectype = QtvPlayer::GetVideoCodecType((QtvPlayer::InstanceHandleT)pHandle);
      if (codectype == QtvPlayer::QTV_UNKNOWN_CODEC)
      {
        retVal = QtvPlayer::QTV_RETURN_ERROR;
      }
      uint16 codec = (uint16)codectype;
      uint16 ret = (uint16)retVal;

      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        // copy audio codec type to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, codec);
        retval_flag = true;
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_AUDIO_CODEC_TYPE_API_ID:
    {
      QtvPlayer::CodecType codectype;
      retVal = QtvPlayer::QTV_RETURN_OK;
      uint32 pHandle;
      UNPACK_UINT32(pHandle, api_packet_ptr);
      codectype = QtvPlayer::GetAudioCodecType((QtvPlayer::InstanceHandleT)pHandle);
      if (codectype == QtvPlayer::QTV_UNKNOWN_CODEC)
      {
        retVal = QtvPlayer::QTV_RETURN_ERROR;
      }
      uint16 codec = (uint16)codectype;
      uint16 ret = (uint16)retVal;

      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        // copy audio codec type to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, codec);
        retval_flag = true;
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_AUDIO_VIDEO_STATS_API_ID:
    {
      QtvPlayer::AudioVideoStatisticsT statistics;
      retVal = QtvPlayer::GetAudioVideoStatistics(statistics);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
        break;
      }
      uint16 ret = (uint16)retVal;

      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);

        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Video.format);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Video.encodedFrameRate);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Video.encodedBitrate);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Video.frames);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Video.skippedDecode);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Video.skippedDisplay);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Video.failedDecode);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Video.maxDrop);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Video.decodedFrameRate);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Video.displayedFrameRate);

        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Audio.format);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.Audio.encodedBitrate);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Audio.frames);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Audio.nullSamples);
        FILL_RESPONSE_PACKET_INT32(response_pkt_ptr, statistics.Audio.maxDrop);

        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.AVSync.videoAheadMsec);
        FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, statistics.AVSync.videoBehindMsec);
        FILL_RESPONSE_PACKET_BOOL(response_pkt_ptr, statistics.AVSync.avSyncValid);
        retval_flag = true;
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_MIME_TYPE_API_ID:
    {
      uint32 URN_length = 0, MIME_type_length;
      char * URN = NULL;
      char * MIME_type = NULL;
      URN_length = (strlen(api_packet_ptr)+1);
      if(URN_length >= curr_api_len || URN_length <= MIN_URN_LEN)
      {
        ASSERT(0);
      }
      URN = (char *)QTV_Malloc(URN_length);
      if (NULL != URN)
      {
        memcpy(URN,api_packet_ptr,URN_length);
      }
      else
      {
        ASSERT(0);
      }
      MIME_type = (char *)QtvPlayer::GetMIMEType(URN);
      retVal = QtvPlayer::QTV_RETURN_OK;
      MIME_type_length = strlen(MIME_type)+1;
      uint16 ret = (uint16)retVal;

      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        if(MIME_type_length < MIME_TYPE_MAX_LEN)
        {
          FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, MIME_type_length);
          memcpy(response_pkt_ptr,MIME_type,MIME_type_length);
        }
        else
        {
          MIME_type_length = 0;
          FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, MIME_type_length);
        }
        
        retval_flag = true;
        if(URN)
        {
          QTV_Free(URN);
          URN = NULL;
        }
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_MEDIA_TYPE_API_ID:
    {
      uint32 URN_length = 0, MEDIA_type_length;
      char * URN = NULL;
      char * MEDIA_type = NULL;
      URN_length = (strlen(api_packet_ptr)+1);
      if(URN_length >= curr_api_len || URN_length <= MIN_URN_LEN)
      {
        ASSERT(0);
      }
      URN = (char *)QTV_Malloc(URN_length);
      if (NULL != URN)
      {
        memcpy(URN,api_packet_ptr,URN_length);
      }
      else
      {
        ASSERT(0);
      }
      MEDIA_type = (char *)QtvPlayer::GetMediaType(URN);
      retVal = QtvPlayer::QTV_RETURN_OK;
      MEDIA_type_length = strlen(MEDIA_type)+1;
      uint16 ret = (uint16)retVal;

      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        if(MEDIA_type_length < MEDIA_TYPE_MAX_LEN)
        {
          FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, MEDIA_type_length);
          memcpy(response_pkt_ptr,MEDIA_type,MEDIA_type_length);
        }
        else
        {
          MEDIA_type_length = 0;
          FILL_RESPONSE_PACKET_UINT32(response_pkt_ptr, MEDIA_type_length);
        }
        retval_flag = true;
        if(URN)
        {
          QTV_Free(URN);
          URN = NULL;
        }
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_URN_TYPE_API_ID:
    {
      bool bVideoURN = false;
      uint8 videoURN = 0;
      QtvPlayer::URNTypeT URNType;
      // WARNING: TBD: Should we be unpacking a boolean as a byte?
      UNPACK_UINT8(videoURN,api_packet_ptr);
      if(videoURN == 1)
      {
        bVideoURN = true;
      }
      retVal = QtvPlayer::GetURNType(URNType, bVideoURN);
      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
        break;
      }
      uint16 ret = (uint16)retVal;
      uint16 TypeofURN = (uint16)URNType;
      if (response_pkt_ptr) //this will be null in case of replay testing
      {
        // copy the api id to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
        //copy the diag error code to the response packet
        FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
        // copy Qtv player return value to the response packet
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
        FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, TypeofURN);
        retval_flag = true;
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_SET_VDEC_PARAMETER:
    {
      QtvPlayer::QtvVDECParameterDataT * paramData = NULL;
      uint32 paramValue;
      uint32 pHandle;
      uint32 paramId;

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main SetVDECParameter()", 0 );
	
      UNPACK_UINT32(paramId, api_packet_ptr);
      UNPACK_UINT32(paramValue, api_packet_ptr);
      UNPACK_UINT32(pHandle, api_packet_ptr);

      paramData = (QtvPlayer::QtvVDECParameterDataT *)QTV_Malloc(sizeof(QtvPlayer::QtvVDECParameterDataT));
      if(paramData)
      {
        paramData->freEnable.bFreEnable = (paramValue!=0);
        if (pHandle)
        {
          retVal = QtvPlayer::SetVDECParameter((QtvPlayer::QtvVDECParameterIdT)paramId, paramData, (QtvPlayer::InstanceHandleT)pHandle);
        }
        else
        {
          retVal = QtvPlayer::SetVDECParameter((QtvPlayer::QtvVDECParameterIdT)paramId, paramData);
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc failed"); 
      }
      QTV_Free(paramData);
      paramData = NULL;
      break;
    }
  default:
    {
      diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN;

      /* Unknown API command */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "Unknown API command %d", api_id);
      break;
    }
  }

  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "QTVDIAG command successful");
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "QTVDIAG command FAILED, retVal = %d", retVal);
  }
  uint16 ret = (uint16)retVal;

  if ((NULL != response_pkt_ptr ) && (false == retval_flag )) //lint !e731
  {
    // copy the api id to the response packet
    FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, api_id);
    //copy the diag error code to the response packet
    FILL_RESPONSE_PACKET_UINT8(response_pkt_ptr, diag_error_code);
    // copy Qtv player return value to the response packet
    FILL_RESPONSE_PACKET_UINT16(response_pkt_ptr, ret);
  }
  return TRUE;
}

/*===========================================================================
FUNCTION qtvdiag_maollc

DESCRIPTION
  Function for qtvdiag malloc 
  
DEPENDENCIES
  None.

RETURN VALUE
  void * 

SIDE EFFECTS
  None
===========================================================================*/
void * qtvdiag_maollc(const VDEC_STREAM_ID streamID,void *pCbData,uint32 nBytes)
{
  void *p;
#ifdef PLATFORM_LTK
#error code not present
#elif defined(T_MSM7500)
#error code not present
#else
    QTV_USE_ARG1(pCbData);
    QTV_USE_ARG1(streamID);
    p =  QTV_Malloc(nBytes);
#endif /* T_MSM7500 */
  return p;
}

/*===========================================================================
FUNCTION qtvdiag_free

DESCRIPTION
  Function for qtvdiag free
  
DEPENDENCIES
  None.

RETURN VALUE
  void 

SIDE EFFECTS
  None
===========================================================================*/
void qtvdiag_free(const VDEC_STREAM_ID streamID,void *pCbData,void* pBuf)
{
  if(pBuf!=NULL)
  {
 #ifdef PLATFORM_LTK
#error code not present
 #elif defined(T_MSM7500)
#error code not present
 #else
     QTV_Free(pBuf);
 #endif /* T_MSM7500 */
  }
  QTV_USE_ARG1(pCbData);
  QTV_USE_ARG1(streamID);
}
/*===========================================================================
FUNCTION QTVDIAG_HANDLE_API_CMD

DESCRIPTION
  Function called in the Diag context used to handle packets from Diag which
  have the sub command ID's that correspond to QTV player commands. This function 
  simply dispatches the QTV player to handle the command.
  
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None
===========================================================================*/

PACKED void * qtvdiag_handle_api_cmd( PACKED void *request_packet_ptr, word /*param*/)
{
  char *packet = (char *) request_packet_ptr;
  
    // Get the API id (same as subsystem command code)
  qtvdiag_command_e_type api_id = (qtvdiag_command_e_type) 
    diagpkt_subsys_get_cmd_code(request_packet_ptr);  

  // Skip the diag packet header.
  packet += sizeof(diagpkt_subsys_header_type);
  
  qtvdiag_cmd_error_code_e_type diag_error_code = 
    QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;

  switch (api_id)
  {
    case QTVDIAG_COMMAND_INIT:
    {
      // Initialize the Qtv Player 
      retVal = QtvPlayer::Init(AllocateOutputBuffer, FreeOutputBuffer);     
    }
    break;
    
    case QTVDIAG_COMMAND_OPEN_URN:
    {
      uint32 video_URN_length = 0;            
      char *video_URN = NULL;
        
      if (!qtvdiag_make_filename((const char *) packet, &video_URN_length, 
                                 &video_URN)) 
      {
          diag_error_code = QTVDIAG_ERROR_CODE_OUT_OF_MEMORY;
      }
      
      // video_URN_length is set regardless of success
      packet += video_URN_length;

      uint32 audio_URN_length = 0;
      char *audio_URN = NULL;

      if (!qtvdiag_make_filename((const char *) packet, &audio_URN_length, 
                                 &audio_URN)) 
      {
          diag_error_code = QTVDIAG_ERROR_CODE_OUT_OF_MEMORY;
      }
       
      // Will copy videoURN and audioURN into internal buffers.
      retVal = QtvPlayer::OpenURN(video_URN, audio_URN);

      if (NULL != video_URN) 
      {
        QTV_Free(video_URN);
      }

      if (NULL != audio_URN) 
      {
        QTV_Free(audio_URN);
      }     
    }
    break;

    case QTVDIAG_COMMAND_PLAY:
    {
      int32 start_position = *((int32 *) packet); //lint !e826
      packet += sizeof(int32);

      int32 end_position = * ((int32 *) packet); //lint !e826

      retVal = QtvPlayer::PlayClip(start_position, end_position);     
    }
    break;
  
    case QTVDIAG_COMMAND_TERMINATE:
    {
      retVal = QtvPlayer::Terminate();     
    }
    break;
    
    case QTVDIAG_COMMAND_PAUSE:
    {
      retVal = QtvPlayer::Pause();      
    }
    break;
    
    case QTVDIAG_COMMAND_STOP:
    {
      retVal = QtvPlayer::Stop();     
    }
    break;
    
    case QTVDIAG_COMMAND_PLAY_NEXT_FRAME:
    {
      retVal = QtvPlayer::PlayNextFrame();     
    }
    break;
    
    case QTVDIAG_COMMAND_PLAY_PREVIOUS_FRAME:
    {
      retVal = QtvPlayer::PlayPrevFrame();      
    }
    break;
    
#ifdef FEATURE_QTV_STREAM_RECORD
    case QTVDIAG_COMMAND_START_RECORDING:
    {
      bool overwrite = (*packet++ != (char) 0);

      uint8 mode = *packet++;

      uint32 duration = * ((uint32 *) packet); //lint !e826
      packet += sizeof(uint32);

      uint32 filename_length = 0;
      char *filename = NULL;
            
      if (qtvdiag_make_filename((const char *) packet, &filename_length, 
                                &filename)) 
      {
        // Should copy filename into its internal buffer
        if (filename != NULL)
        {
           retVal = QtvPlayer::RecordClip(filename, overwrite, mode, duration); 
        }      
        QTV_Free(filename);
      }
      else
      {
        diag_error_code = QTVDIAG_ERROR_CODE_OUT_OF_MEMORY;            
      }      
    }
    break;
    
    case QTVDIAG_COMMAND_STOP_RECORDING: 
    {
      retVal = QtvPlayer::StopClipRecording();
    }
    break;

    case QTVDIAG_COMMAND_SET_DFLT_RECORDING_DIR:
    {
      QTV_MSG_PRIO(QTVDIAG_STREAM_RECORDING, QTVDIAG_PRIO_HIGH, 
                   "Set Default Recording Dir not supported by QTVDiag yet");
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;      
    }
    break;

#else     

    case QTVDIAG_COMMAND_START_RECORDING:
    case QTVDIAG_COMMAND_STOP_RECORDING:
    case QTVDIAG_COMMAND_SET_DFLT_RECORDING_DIR:
    { 
      QTV_MSG_PRIO(QTVDIAG_STREAM_RECORDING, QTVDIAG_PRIO_HIGH, 
             "Stream Recording not supported in this build");

      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
      break;
    }
#endif // FEATURE_QTV_STREAM_RECORD

    case QTVDIAG_COMMAND_SEEK:
    { 
      // This is based on the code in OEMMediaMPEG4      
      QtvPlayer::ClipInfoT clipInfo;

      // Make sure we're allowed to reposition within the clip.
      retVal = QtvPlayer::GetClipInfo(clipInfo);
      if ((retVal != QtvPlayer::QTV_RETURN_OK) || !clipInfo.RepositioningAllowed)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;     
         break;
      }

#if 0
      QtvPlayer::PlayerStateRecordT state;

      retVal = QtvPlayer::GetPlayerState(state); 
      if (retVal != QTV_RETURN_OK)
      {
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;     
         break;
      }
             
      /* if we are buffering and we have not started playback yet (means initial buffering),
         don't do repositioning */
      if ((pMpeg4Player->GetLastStatus() == QtvPlayer::QTV_PLAYER_STATUS_BUFFERING) &&
          !state.audioFrames && !state.videoFrames )
      {
         retVal = QtvPlayer::QTV_RETURN_ERROR;
         diag_error_code = QTVDIAG_ERROR_CODE_ILLEGAL_STATE;     
         break;
      }
#endif

      int32 offset = *((int32 *) packet); //lint !e826

      if (offset >= 0)
      {
         offset = MIN(offset, clipInfo.Duration);
      }
      else
      {
         offset = MAX(offset, 0);
      }

      retVal = QtvPlayer::PlayClip(offset, -1);     
    }
    break; 

#ifdef FEATURE_FILE_FRAGMENTATION
    case QTVDIAG_COMMAND_SKIP:
    {
      int32 skipNumber = (int32) *((int32 *) packet); //lint !e826
      retVal = QtvPlayer::SkipClip(skipNumber);     
    }
#else
    case QTVDIAG_COMMAND_SKIP:
    { /*lint !e527 */
      QTV_MSG_PRIO(QTVDIAG_STREAM_RECORDING, QTVDIAG_PRIO_HIGH, 
             "Stream Recording not supported in this build");
    
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;      
    }
#endif // #ifdef FEATURE_FILE_FRAGMENTATION
    break;


    // This should match the #ifdef whether RotateVideo() and ScaleVideo() are
    // defined.
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    case QTVDIAG_COMMAND_ROTATE_VIDEO:
    {
      QtvPlayer::RotationType direction = (QtvPlayer::RotationType) *packet;
      retVal = QtvPlayer::RotateVideo(direction);
      break; 
    }

    case QTVDIAG_COMMAND_SCALE_VIDEO:
    {
      QtvPlayer::ScalingType scale = (QtvPlayer::ScalingType) *packet;
      retVal = QtvPlayer::ScaleVideo(scale);
      break; 
    }
#else
    case QTVDIAG_COMMAND_ROTATE_VIDEO:
    case QTVDIAG_COMMAND_SCALE_VIDEO:
    { 
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
      break;
    }
#endif

    case QTVDIAG_COMMAND_RESET_RTP_STATISTICS:
    {
#if 0
      // Finish implementing later ...
      int mask = (int) *packet;
      
      for (int i = 0; i < QTV_MAX_MEDIA_TRACKS; i++)        
      {
         if ((mask & (1 << i)) != 0) 
         {
             if (!pMpeg4Player->ResetRTPStatistics(i)) 
             {
                 retVal = QtvPlayer::QTV_RETURN_ERROR;
                 break;
             }
         }
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;      
#endif
      break; 
    }

    case QTVDIAG_COMMAND_SIMULATE_DECODING_ERROR:
    {
#if 0
      // Finish implementing later ...
      if (pMpeg4Player) {
        QTV_SIMULATE_ERROR_type *pEvent = QCCreateMessage(QTV_SIMULATE_ERROR);        

        if (NULL != pEvent)
        {
          pEvent->error_type = (qtvdiag_decoding_error_e_type) *packet;

          pMpeg4Player->PostMessage(pEvent);
          retVal = QTV_RETURN_OK;
        }
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif
    }
    break;

    case QTVDIAG_COMMAND_REWIND:  
    case QTVDIAG_COMMAND_FFWD:
    {
      QtvPlayer::PlayerStateRecordT playerState;
      retVal = QtvPlayer::GetPlayerState(playerState);

      if (retVal != QtvPlayer::QTV_RETURN_OK)
      {
        break;
      }

      int32 currTime = (int32)playerState.playbackMsec;
  
      int32 seekTime = (int32) *((int32 *) packet); //lint !e826
       
      if (api_id == QTVDIAG_COMMAND_REWIND)
      {
        seekTime = -seekTime;
      }

      seekTime += currTime;

      if (seekTime < 0)
      {
        seekTime = 0;
      }

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                    "Repositioning clip to %d", seekTime);
  
      retVal = QtvPlayer::PlayClip(seekTime, -1);
      break;
    }

    case QTVDIAG_COMMAND_SET_STREAM_CONFIG_ITEM:
    {
      enum QtvConfig::QtvConfigItemId item = 
        (enum QtvConfig::QtvConfigItemId) *((int32 *) packet); //lint !e826

      packet += sizeof(int32);

      int *value = (int *) packet; //lint !e826

      if (QtvPlayer::SetQTVConfigItem(item, value))
      {
        retVal = QtvPlayer::QTV_RETURN_OK;
      }
      else
      {
        retVal = QtvPlayer::QTV_RETURN_ERROR;
      }
      break;
    }

    case QTVDIAG_COMMAND_START_STOP:
    {
      retVal = QtvPlayer::PlayClip(0, -1);
  
      int32 sleepTime = *((int32 *) packet); //lint !e826
      zrex_sleep((uint32)sleepTime);
  
      retVal = QtvPlayer::Terminate();
      break;
    }

    // Required for testing In-Call Qtv
    case QTVDIAG_COMMAND_SUSPEND: 
    {
      int value = 1;
      QtvPlayer::SetQTVConfigItem(QtvConfig::QTVCONFIG_PAUSE_IS_SUSPEND, &value);//lint !e534
      retVal = QtvPlayer::Pause();
      break;
    }

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */


    case QTVDIAG_COMMAND_PLAYLIST_SKIP1:
    {
#ifdef FEATURE_QTV_PLAYLIST       
      int32 clipIndex = *((int32 *) packet);
      packet += sizeof(int32);

      char isClipIndexRelative = *packet; 
      packet++;

      char preferClientPlaylistSkip = *packet;
      packet++;

      retVal = QtvPlayer::PlaylistPlayClip(clipIndex,
        (isClipIndexRelative == 0) ? false : true,
        (preferClientPlaylistSkip == 0) ? false : true);
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif /* FEATURE_QTV_PLAYLIST */
      break;
    }

    case QTVDIAG_COMMAND_PLAYLIST_SKIP2:
    {
#ifdef FEATURE_QTV_PLAYLIST
      
      int32 clipIndex = *((int32 *) packet);
      packet += sizeof(int32);  

      int32 offset = *((int32 *) packet);
      packet += sizeof(int32);

      int32 when = *((int32 *) packet);
      packet += sizeof(int32);

      char isClipIndexRelative = *packet; 
      packet++;

      char *playlistName = (char *) QTV_Malloc(256);
      if(playlistName)
      {
        ZUtils::StrcpyN(playlistName, packet, 256);

        retVal = QtvPlayer::PlaylistPlayClip(playlistName, clipIndex,
          (isClipIndexRelative == 0) ? false : true, offset, when);

        QTV_Free(playlistName);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
#else
      retVal = QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
      diag_error_code = QTVDIAG_ERROR_CODE_GENERIC_ERROR;
#endif /* FEATURE_QTV_PLAYLIST */
      break;
    }

    default:
    {
      diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_UNKNOWN;

      /* Unknown API command */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "Unknown API command %d", api_id);
      break;
    }
  }

  if (retVal == QtvPlayer::QTV_RETURN_OK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "QTVDIAG command successful");
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "QTVDIAG command FAILED, retVal = %d", retVal);
  }
      
  // Acknowledge command with a packet.
  qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) api_id);


  if (NULL == response_ptr) 
  {
    return NULL;
  }

  response_ptr->diag_error_code = (uint8) diag_error_code;
  response_ptr->qtv_return_code = (uint32) retVal;

  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
}

/*===========================================================================

FUNCTION QTVDIAG_HANDLE_GET_INFO_CMD

DESCRIPTION
  Function called in the Diag context used to handle packets from Diag which
  request information about the QTV state to be sent back.
  
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None
===========================================================================*/
PACKED void * qtvdiag_handle_get_info_cmd( PACKED void *request_packet_ptr, word /*param*/)
{ 
  PACKED char *packet = (PACKED char *) request_packet_ptr;
  
  // Get the parameter id (same as subsystem command code)
  qtvdiag_info_e_type info_type = (qtvdiag_info_e_type) 
    diagpkt_subsys_get_cmd_code(request_packet_ptr);  

  // Skip the diag packet header.
  packet += sizeof(diagpkt_subsys_header_type);
  
  // Check if a valid info type
  if ((info_type < QTVDIAG_INFO_OFFSET) || (info_type >= QTVDIAG_INFO_MAX)) /*lint!e641 */
  {
    QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                   "Unknown info type: %d", (int) info_type);
  } 
  else 
  {
    QtvDiagInfoData::SetSendInfo(info_type, packet);      
  }

  return NULL;
}

/*===========================================================================

FUNCTION QTVDIAG_TASK

DESCRIPTION
  This function defines the qtvdiag task and is run when the the qtvdiag_start_task
  function is called.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
void qtvdiag_task ( dword /*dummy*/ )   /* Rex requirement, not used */ 
{
  rex_sigs_type sigs = NULL;

  /* Initialize the command Q as soon as the thread is up */
  (void)q_init( &qtvdiag_cmd_q );

  qtvdiag_task_is_running = TRUE;

  /* Wait on specific signals after initialization in INIT state
  */
  while ( TRUE ) //lint !e716
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Waiting for Signal");
    sigs = rex_wait( my_sigs ); 
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "got signal %d", sigs);

    /* Clear the signals which are handled here.
    */
    (void) rex_clr_sigs( &qtvdiag_tcb, sigs );

    if (sigs & QTVDIAG_EXIT_SIG)
    {
      // We have been asked to commit suicide.
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, 
                    "got exit signal, quitting", sigs);
      qtvdiag_task_is_running = FALSE;
      return;
    }

    /* Now handle the sets signals handled in this state.The signal handlers 
    ** will set the signal mask corresponding to the execution state Session 
    ** Manager.
    */
    qtvdiag_handle_sigs(sigs);
    
  } /* End While Loop */  
} /* End qtvdiag_task() */

#ifdef FEATURE_QTVDIAG_REPLAY

/*===========================================================================

FUNCTION QtvDiagReplayStartCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_START_CMD_ID and results in the qtvdiag replay
  being started. 

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayStartCmdHandler ( PACKED void * /* req_pkt_ptr */, word /* pkt_len */ )
{
  bool issued = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvDiagReplayStartCmdHandler");

  if(qtvdiag_task_is_running)
    return NULL;

  TASKLOCK();
  if (!qtvdiag_task_is_running)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Creating QTVDiag task");
    std_strlcpy(qtvdiag_tcb.task_name,QTV_DIAG_TASK_NAME,(strlen(QTV_DIAG_TASK_NAME) + 1) ) ;
    rex_def_task( &qtvdiag_tcb, 
                  (rex_stack_word_type*) qtvdiag_stack, 
                  QTVDIAG_STACK_SIZ,
                  QTVDIAG_PRI, 
                  qtvdiag_task, 
                  0L ); 

    issued = true;

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Done starting TASK");
  }
  TASKFREE();

  /* Init Replay */
  QtvDiagReplayInit ();

  /* Process first API */
  QtvDiagReplayProcessNextAPIHandler();

  qtvdiag_command_response_s_type *response_ptr = qtvdiag_alloc_response_packet((uint16) QTVDIAG_REPLAY_START_CMD_ID);

  if (NULL == response_ptr) 
  {
    return NULL;
  }

  if (issued) 
  {
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;

    response_ptr->qtv_return_code = (uint32) QtvPlayer::QTV_RETURN_OK;        
  } 
  else 
  {
    response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
  }

  diagpkt_commit(response_ptr);

  return (PACKED void *) response_ptr;
}/*end qtvdiag_start_task*/ /*lint !e715 !e818 suppressed lint warning*/ 

/*===========================================================================

FUNCTION QtvDiagReplayInit

DESCRIPTION
  Function called in the Diag context used to initialize replay APP

DEPENDENCIES
  None.

RETURN VALUE 
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayInit ()
{
  OSCL_FILE * fp =NULL;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvDiagReplayInit");

  /* Open API Cmds Input File */ 
  fp = OSCL_FileOpen("brew/mod/mediaplayer/media/QtvDiagAPIReplayInputFile.txt", "r",(21*1024));

  if(fp == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvDiagReplayInputFile open failed");
    return QTVDIAG_REPLAY_ERROR;
  }

  /* create QtvDiagReplayApp object */
  pQtvReplayDiagApp = (QtvReplayDiagAppT *)QTV_Malloc( sizeof(QtvReplayDiagAppT) );

  if(pQtvReplayDiagApp == NULL)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "pQtvReplayDiagApp malloc failed");
    return QTVDIAG_REPLAY_MEMORY_ERROR;
  }

  memset(pQtvReplayDiagApp, 0, sizeof(QtvReplayDiagAppT));
  
  pQtvReplayDiagApp->nReplayStartTime = SystemTimeInMsec(); 
  pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_PROCESS_NEXT_API;
  pQtvReplayDiagApp->currentAPIOffset = 0;
  pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr = fp;
  pQtvReplayDiagApp->pQtvDiagReplayTimer = rex_create_timer_ex(QtvDiagReplayExecCurrentAPICallback,(unsigned long)NULL);
  /* This is run time flag to select whether, we should check for API event sensitivity or not
  ** currently this is set to FALSE, we can turn on this flag once basic stuff is stable */
  pQtvReplayDiagApp->bCheckAPIEventSensitivity = FALSE;
  
  /* This is run time flag to select whether, we should check for keep live messages or not
  ** currently this is set to FALSE, we can turn on this flag once basic stuff is stable */
  pQtvReplayDiagApp->bCheckKeepLiveMessages = TRUE;
  if(pQtvReplayDiagApp->bCheckKeepLiveMessages)
  {
    pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer = rex_create_timer_ex(QtvDiagReplayKeepLiveCallback,(unsigned long)NULL);
    /* Below if condition is added to fix Klocwork waring */
    if(pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer != NULL)
    {
      rex_set_timer(pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer, QTVDIAG_REPLAY_KEEPLIVE_TIMEOUT);
    }
    pQtvReplayDiagApp->nLastKeepLiveMessageTime = SystemTimeInMsec();
  }

#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG 
  /* Open API Cmds Debug File */ 
  pQtvReplayDiagApp->pQtvDiagReplayDebugFilePtr = OSCL_FileOpen("brew/mod/mediaplayer/media/QtvDiagAPIReplayDebugFile.txt", "c");
#endif /*#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG  */

  return QTVDIAG_REPLAY_SUCCESS;
}

/*===========================================================================

FUNCTION QtvDiagReplayClose

DESCRIPTION
  Function to destroy/delete replay App

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayClose ()
{

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "QtvDiagReplayClose");

  if(pQtvReplayDiagApp)
  {

#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG 
    /* Close API Cmds Debug File */ 
    if(pQtvReplayDiagApp->pQtvDiagReplayDebugFilePtr)
    {
      OSCL_FileClose(pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr);
    }
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY_DEBUG */

    /* Delete Replay Timer */ 
    if(pQtvReplayDiagApp->pQtvDiagReplayTimer )
    {
      rex_delete_timer_ex(pQtvReplayDiagApp->pQtvDiagReplayTimer);
    }

    /* Delete Keep Live Timer */ 
    if(pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer )
    {
      rex_delete_timer_ex(pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer);
    }

    /* free QtvDiagReplayApp object */
    QTV_Free( pQtvReplayDiagApp);
    pQtvReplayDiagApp = NULL;
  }
  return QTVDIAG_REPLAY_SUCCESS;
}

/*===========================================================================

FUNCTION QtvDiagReplayExecCurrentAPICallback

DESCRIPTION
  Callback function to post cmd "QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API"

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayExecCurrentAPICallback(unsigned long)
{
  QtvDiagReplayPostCmd(QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API);
}

/*===========================================================================

FUNCTION QtvDiagReplayKeepLiveCallback

DESCRIPTION
  Callback function to to check whether client is alive or not

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayKeepLiveCallback(unsigned long)
{
  if(pQtvReplayDiagApp && pQtvReplayDiagApp->bCheckKeepLiveMessages)
  {
    /* check for recent client response */
    if((SystemTimeInMsec() - pQtvReplayDiagApp->nLastKeepLiveMessageTime) > QTVDIAG_REPLAY_KEEPLIVE_TIMEOUT)
    {
       /* This means no response from client from QTVDIAG_REPLAY_KEEPLIVE_TIMEOUT msec */
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No response from client, shutting down replay");
       (void)QtvDiagReplayStopCmdHandler(NULL,0);	  
    }
    else
    {
      rex_set_timer(pQtvReplayDiagApp->pQtvDiagReplayKeepLiveTimer, QTVDIAG_REPLAY_KEEPLIVE_TIMEOUT);  
    }
  } 

}

/*===========================================================================

FUNCTION QtvDiagReplayPostCmd

DESCRIPTION
  Function to post Repaly cmd

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayPostCmd(qtvdiag_cmd_e_type cmd)
{
  qtvdiag_cmd_s_type *cmd_ptr = NULL;

  cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));
      
  if( cmd_ptr != NULL )
  {
    cmd_ptr->cmd_id = cmd;
    qtvdiag_post_cmd( cmd_ptr );
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
  }
  return QTVDIAG_REPLAY_SUCCESS;
}

/*===========================================================================

FUNCTION QtvDiagReplayParseCurrentAPICmd

DESCRIPTION
  Function to parse current API cmd

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayParseCurrentAPICmd(void)
{
  uint8* pInputBuf = NULL;
  uint8* pTempInputBuf = NULL;
  uint32 numRead = 0;
  uint32 nLocalAPIOffset = 0;
  QtvDiagReplayErrorType nRet = QTVDIAG_REPLAY_SUCCESS;

  pInputBuf = (uint8*)QTV_Malloc(sizeof(pQtvReplayDiagApp->currentAPICmdInfo));

  if(pInputBuf == NULL)
  {
    return QTVDIAG_REPLAY_MEMORY_ERROR;
  }

  pTempInputBuf = pInputBuf;
 
  numRead = sizeof(pQtvReplayDiagApp->currentAPICmdInfo);
  if( numRead != OSCL_FileSeekRead ((void*)pTempInputBuf, numRead, 1, pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr,pQtvReplayDiagApp->currentAPIOffset+nLocalAPIOffset, SEEK_SET) )
  {
    nRet = QTVDIAG_REPLAY_READ_ERROR;
  }
  else
  {
    nLocalAPIOffset = nLocalAPIOffset+numRead;

    /* parse Sequence number */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nAPISeqNumber,pTempInputBuf);

    /* parse time delta */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nTimeDelta,pTempInputBuf);

    /* parse event id */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nEventId,pTempInputBuf);

    /* parse event timeout */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nEventTimeOut,pTempInputBuf);


    /* parse API Id */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nAPIId,pTempInputBuf);

    /* parse Number of args */
    UNPACK_UINT32(pQtvReplayDiagApp->currentAPICmdInfo.nNumberOfAPIArgs,pTempInputBuf);

    ASSERT( pQtvReplayDiagApp->currentAPICmdInfo.nNumberOfAPIArgs < QTVDIAG_REPLAY_MAX_API_ARGS);

#ifdef FEATURE_QTVDIAG_REPLAY_DEBUG 
    /* Debug code */
    { 
      uint8 fmtStringBuf[1024];
      uint32 fmtStringSize=0;
      char newlineChar = '\n';

      fmtStringSize = OSCL_snprintf((char*)fmtStringBuf,1024,"%cSEQ_NUM:%u%cTIME_DELTA:%u%cEVENT_ID:%d%cEVENT_TIME_OUT:%u%cAPI_ID:%u%cNUM_OF_ARGS:%u%c",
	                               newlineChar,pQtvReplayDiagApp->currentAPICmdInfo.nAPISeqNumber,newlineChar,
								   pQtvReplayDiagApp->currentAPICmdInfo.nTimeDelta,newlineChar,
								   pQtvReplayDiagApp->currentAPICmdInfo.nEventId,newlineChar,
								   pQtvReplayDiagApp->currentAPICmdInfo.nEventTimeOut,newlineChar,
								   pQtvReplayDiagApp->currentAPICmdInfo.nAPIId,newlineChar,
								   pQtvReplayDiagApp->currentAPICmdInfo.nNumberOfAPIArgs,newlineChar); 

      if( fmtStringSize != OSCL_FileWrite (fmtStringBuf, fmtStringSize,1,  pQtvReplayDiagApp->pQtvDiagReplayDebugFilePtr) )
      {
        nRet = QTVDIAG_REPLAY_READ_ERROR;
      }
    }
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY_DEBUG  */

    /* Extract the Arguments */
    for(uint32 i= 0; i< pQtvReplayDiagApp->currentAPICmdInfo.nNumberOfAPIArgs; i++)
    {
      uint8 ArgSizeBuf[10];
      uint8* pArgSizeBuf = ArgSizeBuf;

      /* Extract the current argument size */
      numRead = 4;
      if( numRead != OSCL_FileSeekRead ((void*)pArgSizeBuf, numRead, 1, pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr,pQtvReplayDiagApp->currentAPIOffset+nLocalAPIOffset, SEEK_SET ) )
      {
        nRet = QTVDIAG_REPLAY_READ_ERROR;
      }
      else
      {
        nLocalAPIOffset = nLocalAPIOffset+numRead;

        UNPACK_UINT32(pQtvReplayDiagApp->currentAPIArgsInfo[i].nArgSize ,pArgSizeBuf);

        /* Extract the current argument */
        pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer = (uint8*)QTV_Malloc(pQtvReplayDiagApp->currentAPIArgsInfo[i].nArgSize);

       if(pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer == NULL)
       {
         /* free already allocated buffers */
         for(uint32 j= 0; j< i; j++)
	     {
	       if(pQtvReplayDiagApp->currentAPIArgsInfo[j].pArgBuffer)
	       {
	         QTV_Free(pQtvReplayDiagApp->currentAPIArgsInfo[j].pArgBuffer);
		     pQtvReplayDiagApp->currentAPIArgsInfo[j].pArgBuffer = NULL;
	       }
	     }    
         nRet = QTVDIAG_REPLAY_MEMORY_ERROR;
	     break;
       }

       numRead = pQtvReplayDiagApp->currentAPIArgsInfo[i].nArgSize;
       if( numRead != OSCL_FileSeekRead ((void*)pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer, numRead, 1, pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr, pQtvReplayDiagApp->currentAPIOffset+nLocalAPIOffset, SEEK_SET) )
       {   
         nRet = QTVDIAG_REPLAY_READ_ERROR;
       }
       nLocalAPIOffset = nLocalAPIOffset+numRead;
     }
   }
   pQtvReplayDiagApp->currentAPIOffset = pQtvReplayDiagApp->currentAPIOffset+nLocalAPIOffset; 
   pQtvReplayDiagApp->bIsCurrentAPIInfoValid= TRUE;
 }

 if(pInputBuf)
 {
   QTV_Free(pInputBuf);
   pInputBuf = NULL;
 }
 return nRet; 
}

/*===========================================================================

FUNCTION QtvDiagReplayExecCurrentAPIHandler

DESCRIPTION
  Function is called to handle various Qtv APIs

DEPENDENCIES
  None.

RETURN VALUE
  QtvDiagReplayErrorType
SIDE EFFECTS
  None.

===========================================================================*/
QtvDiagReplayErrorType QtvDiagReplayExecCurrentAPIHandler(void)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvDiagReplayExecCurrentAPIHandler");

  if( !pQtvReplayDiagApp || !pQtvReplayDiagApp->bIsCurrentAPIInfoValid
      ||( (pQtvReplayDiagApp->nState != QTV_DIAG_REPLAY_WAIT_ON_TIME_DELTA)
	      && (pQtvReplayDiagApp->nState != QTV_DIAG_REPLAY_WAIT_ON_EVENT) ) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvDiagReplayExecCurrentAPIHandler failed");
    return QTVDIAG_REPLAY_INVALID_ARGS;
  }
  switch (pQtvReplayDiagApp->currentAPICmdInfo.nAPIId)
  {
    case QTVDIAG_FRAMEWORK_INIT_API_ID:
    {
      // Initialize the Qtv Player 
      retVal = QtvPlayer::Init((QtvPlayer::MallocOutputBufferT)qtvdiag_maollc,(QtvPlayer::FreeOutputBufferT)qtvdiag_free);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main, Init %d", retVal);
      if(retVal == QtvPlayer::QTV_RETURN_OK)
      {
            ++(pQtvReplayDiagApp->numQtvInstances);
      }
      retVal = QtvPlayer::RegisterForCallback(qtvdiag_callbackFn, NULL);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Qtv_diag_main. RegisterForCallback() %d", retVal );
    }
    break;

    case QTVDIAG_FRAMEWORK_REGISTER_FOR_CALLBACK_API_ID:
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main RegisterForCallback()", 0 );
      retVal = QtvPlayer::RegisterForCallback(qtvdiag_callbackFn, NULL);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Qtv_diag_main. RegisterForCallback() %d", retVal );
    }
    break;

  case QTVDIAG_FRAMEWORK_OPEN_URN_API_ID:
    {
      char *video_URN = NULL;
      char *audio_URN = NULL;

      video_URN = (char*)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      audio_URN = (char*)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main OpenURN()", 0 );
      retVal = QtvPlayer::OpenURN(video_URN, audio_URN);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Opened URN %d", retVal);
    }
    break;

  case QTVDIAG_FRAMEWORK_OPEN_URN2_API_ID:
    {
       char *video_URN = NULL;
       char *audio_URN = NULL;
       char *text_URN = NULL;   

       video_URN = (char*)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
       audio_URN = (char*)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
       text_URN = (char*)pQtvReplayDiagApp->currentAPIArgsInfo[3].pArgBuffer;

       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main OpenURN()", 0 );
       retVal = QtvPlayer::OpenURN(video_URN, audio_URN, text_URN);
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Opened URN %d", retVal);
    }
    break;

  case QTVDIAG_FRAMEWORK_PLAY_API_ID:
    {
      int32 start_position, end_position;
      uint8* pArg1,*pArg2;

      pArg1 = (uint8*)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      pArg2 = (uint8*)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;

      UNPACK_UINT32(start_position,pArg1);
      UNPACK_UINT32(end_position,pArg2);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Inside Qtv_diag_main PlayClip()", 0 );
      retVal = QtvPlayer::PlayClip(start_position,end_position);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,PlayClip %d", retVal);
    }
    break; 

  case QTVDIAG_FRAMEWORK_PAUSE_API_ID:
    {
      retVal = QtvPlayer::Pause();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Pause %d", retVal);
    }
    break;
	
  case QTVDIAG_FRAMEWORK_STOP_API_ID:
    {
      retVal = QtvPlayer::Stop();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Stop %d", retVal);
    }
    break;

  case QTVDIAG_FRAMEWORK_TERMINATE_API_ID:
    {
      retVal = QtvPlayer::Terminate();
      if(retVal == QtvPlayer::QTV_RETURN_OK)
      {
         --(pQtvReplayDiagApp->numQtvInstances);
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Terminate %d", retVal);
    }
    break;
	
  case QTVDIAG_FRAMEWORK_GET_FRAME_INFO_API_ID:
  {
    /*display the frame, we already displayed the frame */
    // qtvdiag_handle_play_frame();
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetFrameInfo");
  }
  break;
  
  case QTVDIAG_FRAMEWORK_PLAY_NEXT_FRAME_API_ID:
    {
      retVal = QtvPlayer::PlayNextFrame();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,PlayNextFrame %d", retVal);
    }
    break;
	
  case QTVDIAG_FRAMEWORK_PLAY_PREVIOUS_FRAME_API_ID:
    {
      retVal = QtvPlayer::PlayPrevFrame();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,PlayPreviousFrame %d", retVal);
    }
    break;

  case QTVDIAG_FRAMEWORK_SET_QTV_CONFIG_ITEM: 
    {
      uint32 value, item_id;
      bool ret = false;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(item_id, pArg1);
      uint8 *pArg2 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
      UNPACK_UINT32(value, pArg2);
      ret = QtvPlayer::SetQTVConfigItem((QtvConfig::QtvConfigItemId)item_id, (int *)&value); //lint !e534
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,SetQtvConfigItem %d", ret);
      break;
    }
  
  case QTVDIAG_FRAMEWORK_SET_AUDIO_SOURCE: 
    {
      int source_val;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(source_val, pArg1);
      retVal = QtvPlayer::SetAudioSource((QtvPlayer::AudioSourceFormatT)source_val);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,SetAudioSource %d",retVal);
      break;
    }
  
  case QTVDIAG_FRAMEWORK_SKIP_API_ID: 
    {
      int32 skipNumber;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(skipNumber, pArg1);
      retVal = QtvPlayer::SkipClip(skipNumber);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,SkipClip %d", retVal);
      break;
    }

  case QTVDIAG_FRAMEWORK_RECORD_CLIP_API_ID: 
    {
      uint8 mode,overwrite;
      uint32 duration, filename_length;
      char *pArg1 = (char *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      uint8 *pArg2 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
      uint8 *pArg3 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[3].pArgBuffer;
      uint8 *pArg4 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[4].pArgBuffer;
      char *filename = NULL;
      UNPACK_UINT8(overwrite, pArg2);
      UNPACK_UINT8(mode, pArg3);
      UNPACK_UINT32(duration, pArg4);
      if (qtvdiag_make_filename((const char *) pArg1, &filename_length, 
                                &filename))
      {
        retVal = QtvPlayer::RecordClip(filename,(overwrite)?true:false,mode,duration);
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,RecordClip %d", retVal);
      break;
    }
  
  case QTVDIAG_FRAMEWORK_STOP_CLIP_RECORDING_API_ID: 
    {
      retVal = QtvPlayer::StopClipRecording();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,StopClipRecording %d", retVal);
      break;
    }
  
  case QTVDIAG_FRAMEWORK_GET_CLIP_INFO_API_ID: 
    {
      QtvPlayer::ClipInfoT *pClipinfo = (QtvPlayer::ClipInfoT *)QTV_Malloc(sizeof(QtvPlayer::ClipInfoT));
      if(pClipinfo)
      {
	  retVal = QtvPlayer::GetClipInfo(*pClipinfo);
	  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetClipInfo %d", retVal);
	  QTV_Free(pClipinfo);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_AUDIO_CODEC_TYPE_API_ID: 
    {
      QtvPlayer::CodecType retval;
      retval = QtvPlayer::GetAudioCodecType();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetAudioCodecType %d",retval);
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_VIDEO_CODEC_TYPE_API_ID: 
    {
      QtvPlayer::CodecType retval;
      retval = QtvPlayer::GetVideoCodecType();
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetVideoCodecType %d", retval);
      break;
    }
  
 #ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  case QTVDIAG_FRAMEWORK_ROTATE_VIDEO_API_ID:
    {
      uint32 direction;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(direction, pArg1);
      retVal = QtvPlayer::RotateVideo((QtvPlayer::RotationType)direction);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,RotateVideo %d", retVal);
      break; 
    }

  case QTVDIAG_FRAMEWORK_SCALE_VIDEO_API_ID:
    {
      uint32 scale;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(scale, pArg1);
      retVal = QtvPlayer::ScaleVideo((QtvPlayer::ScalingType)scale);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,ScaleVideo %d",retVal);
      break; 
    }
#endif

  case QTVDIAG_FRAMEWORK_GET_AUDIO_VIDEO_STATS_API_ID:
    {
      QtvPlayer::AudioVideoStatisticsT statistics;
      retVal = QtvPlayer::GetAudioVideoStatistics(statistics);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetAudioVideoStatistics %d",retVal);
      break; 
    }
  
case QTVDIAG_FRAMEWORK_GET_PLAYER_STATE:
  {
      QtvPlayer::PlayerStateRecordT state;
      retVal = QtvPlayer::GetPlayerState(state);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetPlayerState %d",retVal);
      break;
  }

case QTVDIAG_FRAMEWORK_GET_URN_TYPE_API_ID:
    {
      uint8 bVideoURN = false;
      QtvPlayer::URNTypeT URNType;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
      UNPACK_UINT8(bVideoURN,pArg1);
      retVal = QtvPlayer::GetURNType(URNType,(bVideoURN)?true:false);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetUrntype %d",retVal);
      break;
   }

  case QTVDIAG_FRAMEWORK_GET_MEDIA_TYPE_API_ID:
    {
      char *file = (char *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      (void *)QtvPlayer::GetMediaType(file);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetMediaType");
      break;
    }

  case QTVDIAG_FRAMEWORK_GET_MIME_TYPE_API_ID:
    {
      char *file = (char *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      QtvPlayer::GetMIMEType(file);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetMIMEType");
      break;
  }

  case QTVDIAG_FRAMEWORK_GET_NUM_FRAMES:
   {
      uint32 tracktype,pNumframes;
      char *pArg1 = (char *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(tracktype,pArg1);
      retVal = QtvPlayer::GetNumFrames((QtvPlayer::TrackType)tracktype, &pNumframes);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetNumFrames %d",retVal);
      break;
   }

  case QTVDIAG_FRAMEWORK_GET_3GPP_TIMEDTEXT_INFO:
  {
     QtvPlayer::ThreeGPPTimedTextInfo Info;
     boolean ret = false;
     ret = QtvPlayer::Get3GPPTimedTextInfo(&Info);
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Get3GPPTimedTextInfo %d",ret);
     break;
  }

 #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  
  case QTVDIAG_FRAMEWORK_GET_DOWNLOAD_TIME:
  {
      QtvPlayer::PlayerStateRecordT state;
      retVal = QtvPlayer::GetDownloadTime(state);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetDownloadTime %d", retVal);
      break;
  }
 #endif
 
  case QTVDIAG_FRAMEWORK_GET_RAW_FRAME_INFO:
   {
      uint32 lTime, tracktype;
      QtvPlayer::RawFrameInfo pFrameInfo;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      uint8 *pArg2 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
      UNPACK_UINT32(tracktype, pArg1);
      UNPACK_UINT32(lTime, pArg2);
      retVal = QtvPlayer::GetRawFrameInfo((QtvPlayer::TrackType)tracktype, lTime, &pFrameInfo);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetRawFrameInfo %d", retVal);
      break;
  }

  case QTVDIAG_FRAMEWORK_GET_LARGEST_RAW_FRAME_SIZE:
   {
      uint32 pFramesize, tracktype;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(tracktype, pArg1);
      retVal = QtvPlayer::GetLargestRawFrameSize((QtvPlayer::TrackType)tracktype, &pFramesize);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetLargestRawFrameSize %d",retVal);
      break;
  }

    case QTVDIAG_FRAMEWORK_GET_RAW_TRACK_HEADER:
   {
      uint32 tracktype,pBufsize = 30;                    //DEFAULT_HEADER_SIZE
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      UNPACK_UINT32(tracktype, pArg1);
      byte *pBuf = NULL;
      pBuf = (byte *)(QTV_Malloc(pBufsize*(sizeof(byte))));
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetRawTrackHeader %d",retVal);
      if(pBuf)
      {
        retVal = QtvPlayer::GetRawTrackHeader((QtvPlayer::TrackType)tracktype, pBuf, &pBufsize);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
      break;
   }
	
  case QTVDIAG_FRAMEWORK_GET_RAW_FRAME:
  {
      uint32 tracktype, bufSize, FrameNum = 0;
      QtvPlayer::RawFrameInfo pFrameInfo;
      uint8 *pArg1 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[1].pArgBuffer;
      uint8 *pArg2 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[2].pArgBuffer;
      uint8 *pArg3 = (uint8 *)pQtvReplayDiagApp->currentAPIArgsInfo[5].pArgBuffer;
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,GetRawFrame %d",retVal);
      UNPACK_UINT32(tracktype, pArg1);
      UNPACK_UINT32(FrameNum, pArg2);
      UNPACK_UINT32(bufSize, pArg3);
      byte *pBuf = (byte *)QTV_Malloc(bufSize*sizeof(byte));
      if(pBuf)
      {
        retVal = QtvPlayer::GetRawFrame((QtvPlayer::TrackType)tracktype, FrameNum, &pFrameInfo, pBuf, bufSize);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Malloc Failed");
      }
      break;
  }
  default:
    break;
  }
  
  return QTVDIAG_REPLAY_SUCCESS; 
}

/*===========================================================================

FUNCTION QtvDiagReplayKeepLiveCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID.

DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled
SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayKeepLiveCmdHandler ( PACKED void * req_pkt_ptr, word /*pkt_len*/ )
{
  bool is_dupe_diag_pkt = true;
  char *api_packet_ptr = (char *) req_pkt_ptr;
  qtvdiag_cmd_error_code_e_type diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  
  if(api_packet_ptr)
  {
    is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
    if(is_dupe_diag_pkt)
    {
      return NULL;
    }
  }

  if(req_pkt_ptr)
  {
    qtvdiag_command_response_s_type *response_pkt_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID);

    if (NULL == response_pkt_ptr)
    {
      diag_pkt_seq_number = 0;
      return NULL;
    }

    /* copy the diag error code to the response packet */
    response_pkt_ptr->diag_error_code = diag_error_code;
    
    if(pQtvReplayDiagApp && pQtvReplayDiagApp->bCheckKeepLiveMessages)
    {
      /* update nLastKeepLiveMessageTime */
      pQtvReplayDiagApp->nLastKeepLiveMessageTime = SystemTimeInMsec();

      /* Fill the response pkt with replay app state */
      response_pkt_ptr->qtv_return_code = pQtvReplayDiagApp->nState;
    }    

    diagpkt_commit(response_pkt_ptr);
    return(PACKED void *) response_pkt_ptr;
  }

  return NULL;
}

/*===========================================================================

FUNCTION QtvDiagReplayStopCmdHandler

DESCRIPTION
  Function called in the Diag context used to handle packets from diag which
  have the sub command ID set to QTVDIAG_REPLAY_STOP_CMD_ID.
DEPENDENCIES
  None.

RETURN VALUE
  PACKED void * return to Diag - of no consequence to us, makes diag happy that
  the signal was handled
SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagReplayStopCmdHandler ( PACKED void * req_pkt_ptr, word /* pkt_len */ )
{
  bool issued = false;
  bool is_dupe_diag_pkt = true;
  char *api_packet_ptr = (char *) req_pkt_ptr;
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
  
  if(api_packet_ptr)
  {
    is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
    if(is_dupe_diag_pkt)
    {
      return NULL;
    }
  }
  if (qtvdiag_task_is_running)
  {
    (void)rex_set_sigs( &qtvdiag_tcb, QTVDIAG_EXIT_SIG );

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "deleting QTVDiag task");
    while (qtvdiag_task_is_running)
    {
      zrex_sleep(200);
    }
    issued = true;
  }
  else
  {
    issued = false;
  }

   /* First close qtv instances that are currently running */
   if(pQtvReplayDiagApp->numQtvInstances) /*This assures that we are not calling terminate without init being called */
   {
     /* This will not take care of multiple instances */
     retVal = QtvPlayer::Terminate();
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Qtv_diag_main,Terminate");		  
   } 

  /* Clean up Replay App */
  (void)QtvDiagReplayClose();

  if(req_pkt_ptr)
  {
    qtvdiag_command_response_s_type *response_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_REPLAY_STOP_CMD_ID);

    if (NULL == response_ptr)
    {
      diag_pkt_seq_number = 0;
      return NULL;
    }

    if (issued)
    {
      response_ptr->diag_error_code =
      (uint8) QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
    }
    else
    {
      response_ptr->diag_error_code = 
      (uint8) QTVDIAG_ERROR_CODE_ILLEGAL_STATE;
    }
    response_ptr->qtv_return_code = (uint32)retVal;
    diagpkt_commit(response_ptr);
    diag_pkt_seq_number = 0;
    return(PACKED void *) response_ptr;
  }
  else
  {
    return NULL;
  }
}

/*===========================================================================

FUNCTION QtvDiagReplayProcessNextAPIHandler

DESCRIPTION
  Function to to handle "QTVDIAG_REPLAY_PROCESS_NEXT_API"

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayProcessNextAPIHandler ()
{
  int32 nTimeWait = 0;

  if(pQtvReplayDiagApp&&(pQtvReplayDiagApp->nState == QTV_DIAG_REPLAY_PROCESS_NEXT_API))
  {
    /* parse the current API cmd */
    if(QTVDIAG_REPLAY_SUCCESS != QtvDiagReplayParseCurrentAPICmd())
    {
        /* Done Replay */
        pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_DONE;
        pQtvReplayDiagApp->bIsCurrentAPIInfoValid = FALSE;

        /* Close API Cmds Input File */ 
        if(pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr)
        {
           OSCL_FileClose(pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr);
        }
        pQtvReplayDiagApp->pQtvDiagReplayInputFilePtr = NULL;
    }

    if(pQtvReplayDiagApp->bIsCurrentAPIInfoValid)
    {
      if(pQtvReplayDiagApp->bCheckAPIEventSensitivity 
         && (pQtvReplayDiagApp->currentAPICmdInfo.nEventId != -1) )
      {
        /* API is event sensitive */
        pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_WAIT_ON_EVENT;
      }
      else
      {
         /* API is time sensitive */
         pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_WAIT_ON_TIME_DELTA;

         /* calculate time to wait */
         nTimeWait = SystemTimeInMsec() - (pQtvReplayDiagApp->nReplayStartTime);
         nTimeWait = pQtvReplayDiagApp->currentAPICmdInfo.nTimeDelta - nTimeWait;

         if( nTimeWait < QTVDIAG_REPLAY_TIME_RESOLUTION )
         {
           /* No need to wait, execute API cmd */
           (void)QtvDiagReplayPostCmd(QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API);
         }
         else
         {
           /* set the timer */
           if(pQtvReplayDiagApp->pQtvDiagReplayTimer)
           {
             rex_set_timer(pQtvReplayDiagApp->pQtvDiagReplayTimer, nTimeWait);
           }
         }
       }
     }
     else
     {
       /* error/Done */
       pQtvReplayDiagApp->bIsCurrentAPIInfoValid = FALSE;
     }
   }
}

/*===========================================================================

FUNCTION QtvDiagReplayExecuteCurrentAPICmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_REPLAY_CMD_EXEC_CURRENT_API"

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagReplayExecuteCurrentAPICmdHandler ()
{
  if(pQtvReplayDiagApp&&(pQtvReplayDiagApp->nState == QTV_DIAG_REPLAY_WAIT_ON_TIME_DELTA))
  {
    /* Execute current API */
    QtvDiagReplayExecCurrentAPIHandler();

    /* Prepare to Process next API */
    for(uint32 i= 0; i< pQtvReplayDiagApp->currentAPICmdInfo.nNumberOfAPIArgs; i++)
    {
      if(pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer)
      {
        QTV_Free(pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer);
        pQtvReplayDiagApp->currentAPIArgsInfo[i].pArgBuffer = NULL;
      }		    
    }
    pQtvReplayDiagApp->bIsCurrentAPIInfoValid = FALSE;
    pQtvReplayDiagApp->nState = QTV_DIAG_REPLAY_PROCESS_NEXT_API;

	/* Process next API */
    QtvDiagReplayProcessNextAPIHandler();
  }
}

#endif /* #ifdef FEATURE_QTVDIAG_REPLAY */

#ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE
/*===========================================================================

FUNCTION QtvDiagMemoryFailureCmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_MEMORY_FAILURE_CMD_ID"

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagMemoryFailureCmdHandler( PACKED void * req_pkt_ptr, word /*pkt_len*/ )
{
  bool is_dupe_diag_pkt = true;
  char *api_packet_ptr = (char *) req_pkt_ptr;
  qtvdiag_cmd_error_code_e_type diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  /* This one we will use once we have the API in QTV to gracefully fail 
     the QTV_MALLOCs to test the error mechanism in QTV in case of 
     malloc fails */
  //uint8 nPercentageOfMallocFailure = 0;
  //uint32 nDurationOfMallocFailure = 0;
  
  if(api_packet_ptr)
  {
    is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
    if(is_dupe_diag_pkt)
    {
      return NULL;
    }
  }

  if(req_pkt_ptr)
  {
    qtvdiag_command_response_s_type *response_pkt_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_MEMORY_FAILURE_CMD_ID);

    if (NULL == response_pkt_ptr)
    {
      diag_pkt_seq_number = 0;
      return NULL;
    }

    /* copy the diag error code to the response packet */
    response_pkt_ptr->diag_error_code = diag_error_code;

    if(qtvdiag_task_is_running && pQtvDiagApp)
    {
      //Skip the header.
      api_packet_ptr += sizeof(diagpkt_subsys_header_type);
      
      //Skip the 32bit diag packet sequence number.
      api_packet_ptr += sizeof(uint32);

      /* extract/unpack the memory failure parameters */
      //UNPACK_UINT8(nPercentageOfMallocFailure,api_packet_ptr);
      //UNPACK_UINT32(nDurationOfMallocFailure,api_packet_ptr);

      /* call qtvsystem API below,  to update nPercentageOfMallocFailure, nDurationOfMallocFailure  */
          
      response_pkt_ptr->qtv_return_code = QtvPlayer::QTV_RETURN_OK ;    
    }
    else
    {
      response_pkt_ptr->qtv_return_code = QtvPlayer::QTV_RETURN_ERROR; 
    }

    diagpkt_commit(response_pkt_ptr);
    return(PACKED void *) response_pkt_ptr;
  }

  return NULL;
}
#endif // #ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE

#ifdef FEATURE_QTV_DEBUG_CPU_LOADING
/*===========================================================================

FUNCTION QtvDiagCPULoadingCmdHandler

DESCRIPTION
  Function to to handle "QTVDIAG_CPU_LOADING_CMD_ID"

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * QtvDiagCPULoadingCmdHandler( PACKED void * req_pkt_ptr, word /*pkt_len*/ )
{
  bool is_dupe_diag_pkt = true;
  char *api_packet_ptr = (char *) req_pkt_ptr;
  qtvdiag_cmd_error_code_e_type diag_error_code = QTVDIAG_ERROR_CODE_COMMAND_ISSUED;
  uint8 nPercentageOfCPULoading = 0;
  uint32 nDurationOfCPULoading = 0;
  
  if(api_packet_ptr)
  {
    is_dupe_diag_pkt = is_duplicate_diag_packet(api_packet_ptr);
    if(is_dupe_diag_pkt)
    {
      return NULL;
    }
  }

  if(req_pkt_ptr)
  {
    qtvdiag_command_response_s_type *response_pkt_ptr =
    qtvdiag_alloc_response_packet((uint16) QTVDIAG_CPU_LOADING_CMD_ID);

    if (NULL == response_pkt_ptr)
    {
      diag_pkt_seq_number = 0;
      return NULL;
    }

    /* copy the diag error code to the response packet */
    response_pkt_ptr->diag_error_code = diag_error_code;

    if(qtvdiag_task_is_running && pQtvDiagApp)
    {
      //Skip the header.
      api_packet_ptr += sizeof(diagpkt_subsys_header_type);
      
      //Skip the 32bit diag packet sequence number.
      api_packet_ptr += sizeof(uint32);
     
      /* extract/unpack the CPU loading parameters */
      UNPACK_UINT8(nPercentageOfCPULoading,api_packet_ptr);
      UNPACK_UINT32(nDurationOfCPULoading,api_packet_ptr);

      QtvDiagCPULoadingInit( nPercentageOfCPULoading, nDurationOfCPULoading);
      /* post command to load the CPU */
      QtvDiagPostCmd(QTVDIAG_CMD_CPU_LOADING);

      response_pkt_ptr->qtv_return_code = QtvPlayer::QTV_RETURN_OK ;    
    }
    else
    {
      response_pkt_ptr->qtv_return_code = QtvPlayer::QTV_RETURN_ERROR; 
    }

    diagpkt_commit(response_pkt_ptr);
    return(PACKED void *) response_pkt_ptr;
  }

  return NULL;
}
/*===========================================================================

FUNCTION QtvDiagCPULoadingInit

DESCRIPTION
  Function to init CPU loading parameters

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingInit( uint8 nPercentageOfCPULoading, uint32 nDurationOfCPULoading )
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvDiagCPULoadingInit,nPercentageOfCPULoading= %d,nDurationOfCPULoading= %d",nPercentageOfCPULoading,nDurationOfCPULoading );

  if(nPercentageOfCPULoading > 100)
    nPercentageOfCPULoading = 100;

  pQtvDiagApp->nPercentageOfCPULoading = nPercentageOfCPULoading;
  pQtvDiagApp->nDurationOfCPULoading = nDurationOfCPULoading;
  pQtvDiagApp->nPendingDurationOfCPULoading = nDurationOfCPULoading;
  pQtvDiagApp->nCPULoadingStartTime = 0;

  /* Create the CPU loading timer */
  pQtvDiagApp->pQtvDiagCPULoadingTimer = rex_create_timer_ex(QtvDiagCPULoadingTimerCallback,(unsigned long)0);
}

/*===========================================================================

FUNCTION QtvDiagCPULoadingTimerCallback

DESCRIPTION
  Function to post CPU loading cmd

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingTimerCallback( unsigned long)
{
  /* post command to load the CPU */
  QtvDiagPostCmd(QTVDIAG_CMD_CPU_LOADING);    
}

/*===========================================================================

FUNCTION QtvDiagCPULoadingHandler

DESCRIPTION
  Function to handle CPU loading 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingHandler()
{
  int32 nDurationToStuck = 0;  /* in 100 ms block */
  uint32 nTimeStart = SystemTimeInMsec();

  if(pQtvDiagApp && (pQtvDiagApp->nPendingDurationOfCPULoading > 0))
  {
    if(!(pQtvDiagApp->nCPULoadingStartTime))
    {
      pQtvDiagApp->nCPULoadingStartTime = SystemTimeInMsec();
    }
    nDurationToStuck = (QTVDIAG_CPU_LOADING_TIME_RESOLUTION *pQtvDiagApp->nPercentageOfCPULoading)/100;

    while( (SystemTimeInMsec() - nTimeStart) < nDurationToStuck )
    {
      /* Block the Low priority (priority greater than qtv diag task) tasks */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"CPU Loading: Block the low priority tasks with Qtv diag task");
      rex_sleep(0); // sleep for minimum duration to allow context switch to higher priority tasks.
    }

    /* update pending duration */
    pQtvDiagApp->nPendingDurationOfCPULoading = pQtvDiagApp->nCPULoadingStartTime + pQtvDiagApp->nDurationOfCPULoading - SystemTimeInMsec();
   
    /* release the CPU for rest of the duration (in 100 ms block) */ 
    rex_set_timer(pQtvDiagApp->pQtvDiagCPULoadingTimer, (QTVDIAG_CPU_LOADING_TIME_RESOLUTION - nDurationToStuck));
  }
  else
  {
    /* Stop CPU Loading */
    QtvDiagCPULoadingStop();
  }
}

/*===========================================================================

FUNCTION QtvDiagCPULoadingStop

DESCRIPTION
  Function to Stop CPU loading 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagCPULoadingStop()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QtvDiagCPULoadingStop");

  if(pQtvDiagApp && pQtvDiagApp->pQtvDiagCPULoadingTimer)
  {
    rex_delete_timer_ex(pQtvDiagApp->pQtvDiagCPULoadingTimer);
    pQtvDiagApp->pQtvDiagCPULoadingTimer = NULL;
  }
}

#endif // #ifdef FEATURE_QTV_DEBUG_CPU_LOADING

/*===========================================================================

FUNCTION QtvDiagPostCmd

DESCRIPTION
  Function to Post a Cmd to QtvDiag task

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void QtvDiagPostCmd(qtvdiag_cmd_e_type cmd)
{
  qtvdiag_cmd_s_type *cmd_ptr = NULL;

  cmd_ptr = (qtvdiag_cmd_s_type *) QTV_Malloc(sizeof(qtvdiag_cmd_s_type));
      
  if( cmd_ptr != NULL )
  {
    cmd_ptr->cmd_id = cmd;
    qtvdiag_post_cmd( cmd_ptr );
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QTV_Malloc failed for cmd");
  }
}

/*===========================================================================

FUNCTION cm_mm_client_diag_reg

DESCRIPTION
  This func registers the conc mgr call back with diag

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if registration succeeds

SIDE EFFECTS
  None

===========================================================================*/
bool cm_mm_client_diag_reg(cm_mm_call_event_f_type * event_func)
{
   if(pQtvDiagApp->call_info_ptr)
   { 
     pQtvDiagApp->call_info_ptr->call_event_func = event_func;
     return true;
   }
   else
     return false;
}

/* Diag table for the new subsystem commands. This table is registered with
** Diag along with our subsystem ID. Any packet received over the serial 
** interface that contains our subsystem ID triggers a lookup into this table.
** the lookup is based on the sub command id specified in the diag packet 
** if the lookup is successful, the corresponding function is called from
** this table
*
*  First few call individual functions, the rest all call 
*  qtvdiag_handle_api_cmd().
*/
static const diagpkt_user_table_entry_type qtvdiag_tbl[] =
{  
  {(int)QTVDIAG_COMMAND_START_TASK,   (int)QTVDIAG_COMMAND_START_TASK,   qtvdiag_start_task},
  {(int)QTVDIAG_COMMAND_END_TASK,     (int)QTVDIAG_COMMAND_END_TASK,     qtvdiag_end_task},
  {(int)QTVDIAG_FRAMEWORK_START_CMD_ID,(int)QTVDIAG_FRAMEWORK_START_CMD_ID, start_qtvdiag_framework_cmd_handler},
  {(int)QTVDIAG_FRAMEWORK_STOP_CMD_ID, (int)QTVDIAG_FRAMEWORK_STOP_CMD_ID, stop_qtvdiag_framework_cmd_handler},
  {(int)QTVDIAG_COMMAND_REGISTER_FOR_CALLBACK, (int)QTVDIAG_COMMAND_REGISTER_FOR_CALLBACK, qtvdiag_register_for_callback},
  {(int)QTVDIAG_COMMAND_USE_HEAP_MEMORY, (int)QTVDIAG_COMMAND_USE_HEAP_MEMORY, qtvdiag_use_heap_memory},    
  {(int)QTVDIAG_COMMAND_INIT, ((int) QTVDIAG_COMMAND_CARDINALITY) - 1, qtvdiag_handle_api_cmd},
  {(int)QTVDIAG_INFO_OFFSET, ((int) QTVDIAG_INFO_MAX) - 1, qtvdiag_handle_get_info_cmd},
  {(int)QTVDIAG_FRAMEWORK_SEND_TESTAPI_CMD_ID,(int)QTVDIAG_FRAMEWORK_SEND_TESTAPI_CMD_ID,qtvdiag_test_api_cmd_handler}
#ifdef QTV_DIAG_REPLAY
  ,
  {(int)QTVDIAG_FRAMEWORK_REPLAYAPI_CMD_ID,(int)QTVDIAG_FRAMEWORK_REPLAYAPI_CMD_ID,qtvdiag_replay_api_cmd_handler},
  {(int)QTVDIAG_FRAMEWORK_STOP_REPLAY_CMD_ID,(int)QTVDIAG_FRAMEWORK_STOP_REPLAY_CMD_ID,qtvdiag_stop_replay_cmd_handler}
#endif

#ifdef FEATURE_QTVDIAG_REPLAY
  ,{(int)QTVDIAG_REPLAY_START_CMD_ID,(int)QTVDIAG_REPLAY_START_CMD_ID,QtvDiagReplayStartCmdHandler}
  ,{(int)QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID,(int)QTVDIAG_REPLAY_KEEP_LIVE_CMD_ID,QtvDiagReplayKeepLiveCmdHandler}
  ,{(int)QTVDIAG_REPLAY_STOP_CMD_ID,(int)QTVDIAG_REPLAY_STOP_CMD_ID,QtvDiagReplayStopCmdHandler}
#endif /* #ifdef FEATURE_QTVDIAG_REPLAY*/

#ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE
  ,{(int)QTVDIAG_MEMORY_FAILURE_CMD_ID,(int)QTVDIAG_MEMORY_FAILURE_CMD_ID,QtvDiagMemoryFailureCmdHandler}
#endif /* #ifdef FEATURE_QTV_DEBUG_MEMORY_FAILURE*/

#ifdef FEATURE_QTV_DEBUG_CPU_LOADING
  ,{(int)QTVDIAG_CPU_LOADING_CMD_ID,(int)QTVDIAG_CPU_LOADING_CMD_ID,QtvDiagCPULoadingCmdHandler}
#endif /* #ifdef FEATURE_QTV_DEBUG_CPU_LOADING*/

};
#endif /* !FEATURE_DISABLE_QTV_DIAG_IFACE */

/*===========================================================================

FUNCTION qtvdiag_init

DESCRIPTION
  This is the entry function into Diag. This function is called by Diag 
  in the cdma2kdiag file when diag is initialized. This function lands up
  registering the table qtvdiag_tbl with diag. this table is then 
  subsequently used by Diag to route command received by it to our subystem
  to the correct function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Results in diag packets being routed to qtvdiag.

===========================================================================*/
extern "C" void qtvdiag_init (void)
{
  /* This feature is turned on by default when the feature FEATURE_MP4_DECODER 
  ** is enabled. if the feature needs to be turned off define the feature 
  ** FEATURE_DISABLE_QTV_DIAG_IFACE in order to disable QTVDIAG.
  */
#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE
  DIAGPKT_DISPATCH_TABLE_REGISTER ((int)DIAG_SUBSYS_QTV, qtvdiag_tbl);

  // Initialize QtvDiagInfoData static stuff
  QtvDiagInfoData::Reset(true);    
#ifdef QTV_DIAG_REPLAY
  QtvDiag_pReplayObj = new Replay();
  ASSERT(QtvDiag_pReplayObj);
#endif

  // Initialise the sync events which will be used between QtvDIAG and DIAG modules
  QCUtils::CreateCondition(&QtvDiagResponseSync,false,false);

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"QTVDiag Initialized");
#endif /* !FEATURE_DISABLE_QTV_DIAG_IFACE */
}

/* ======================================================================
FUNCTION
  QtvDiag_CommandComplete

DESCRIPTION
//
// Sets the condition

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QtvDiag_CommandComplete()
{
#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE
  QTV_MSG(QTVDIAG_GENERAL, "QTV Diag Cmd Done - QtvDiagResponseSync");
  QCUtils::SetCondition(&QtvDiagResponseSync);
#endif
}

/* ======================================================================
FUNCTION
  QtvDiag_End

DESCRIPTION
//
// This is called wnside the stop task so that all the resources be
// released.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void QtvDiag_End()
{
#ifndef FEATURE_DISABLE_QTV_DIAG_IFACE
   QTV_MSG(QTVDIAG_GENERAL, "QTV Diag is Ending");
   /*Destroy the Sync event*/
   QCUtils::DestroyCondition(&QtvDiagResponseSync);
   /*If pointers are allocated then release it*/
   if (pMsgToQtvDiag != NULL)
   {
      QTV_Free(pMsgToQtvDiag);
      pMsgToQtvDiag = NULL;
   }
 #endif
}

