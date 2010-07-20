#ifndef CAMSOFTDSP_H
#define CAMSOFTDSP_H

#ifdef CAMERA_USES_SOFTDSP


/* These are the QDSP response messages.  These are issued from the QDSP to the 
   microprocessor. */

typedef enum CAMSoftDSP_ResponseMsgType
{
  CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME,
  CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME,
  CAMSOFTDSP_MSG_ILLEGAL_COMMAND,
} CAMSoftDSP_ResponseMsgType;

typedef struct CAMSoftDSP_EndOfFrameMessageType
{
  byte              *pBuff;
  uint16             frame_seq_num;
} CAMSoftDSP_EndOfFrameMessageType;

typedef struct CAMSoftDSP_IllegalCommandMessageType
{
  uint16            wState;
  uint16            cmdCode;
} CAMSoftDSP_IllegalCommandMessageType;

typedef struct CAMSoftDSP_ResponseType
{
  union
  {
    CAMSoftDSP_EndOfFrameMessageType     endOfFrame;
    CAMSoftDSP_IllegalCommandMessageType illegalCmd;
  } responsePayload;
  /* Moved to the bottom of this structure since qdsp_memcpy32
   * does not support 16 bit aligned address. It supports only
   * 32 bit and 8 bit aligned address. */
  CAMSoftDSP_ResponseMsgType responseMsg;

} CAMSoftDSP_ResponseType;

/*
** Message processing callback function structure
*/
typedef void (* CAMSoftDSP_MsgCBType)(CAMSoftDSP_ResponseType  *pMsgInfo);

void    SoftDSP_Init(void);
void    SoftDSP_Exit(void);
int     SoftDSP_Start(const camsensor_static_params_type *camsensor_params);
int     SoftDSP_Stop(void);
int     SoftDSP_Preview(CAMSoftDSP_MsgCBType cb);
int     SoftDSP_Capture(CAMSoftDSP_MsgCBType cb);
int     SoftDSP_Ack1(void *pBuff);

#endif //#ifdef CAMERA_USES_SOFTDSP
#endif //CAMSOFTDSP_H
