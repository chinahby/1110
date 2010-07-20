/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Camera QDSP Interface

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "camqdsp.h"
#include "camerai.h"

#ifdef CAMERA_USES_SOFTDSP
#include "camsensor.h"
#include "softdsp.h"

#define MATH_FACTOR_BIT         10
#define TEST_ENABLE
#ifdef TEST_ENABLE
#include "clk.h"
#endif

struct SoftDSPInfo{
    const camsensor_static_params_type *camsensor_params;
    CAMSoftDSP_MsgCBType    msgcb;
    int                     nPreviewWidth;
    int                     nPreviewHeight;
    int                     nCaptureWidth;
    int                     nCaptureHeight;
    uint16                 *pCaptureBuff;
    uint16                 *ppPreviewBuff[SOFTDSP_PREVIEW_BUFF_MAX];
    int                     currPreviewBuff;
    int                     nSeqNum;
    CAMSoftDSP_ResponseType MsgRespones[CAMSOFTDSP_MSG_MAX];
    boolean                 bCaptureState;
}g_SoftDSPInfo;

#ifdef TEST_ENABLE
static clk_cb_type softdsp_test_clk;
#endif

static void SoftDSP_FrameISR(void)
{
    if(g_SoftDSPInfo.msgcb)
    {
        g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_START_OF_FRAME;
        g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME]);
    }
}

#ifdef TEST_ENABLE
static void softdsp_test_clk_cb(int ms)
{
    SoftDSP_FrameISR();
    clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) 500, 0, FALSE);
}
#endif

static void SoftDSP_RefreshPreviewBuff(void)
{
    int i;
    g_SoftDSPInfo.ppPreviewBuff[g_SoftDSPInfo.currPreviewBuff] = NULL;
    for(i=0;i<SOFTDSP_PREVIEW_BUFF_MAX;i++)
    {
        if(g_SoftDSPInfo.ppPreviewBuff[i] != NULL)
        {
            g_SoftDSPInfo.currPreviewBuff = i;
        }
    }
}

static uint16 *SoftDSP_GetPreviewBuff(void)
{
    return g_SoftDSPInfo.ppPreviewBuff[g_SoftDSPInfo.currPreviewBuff];
}

static void SoftDSP_BMPScale(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
                             uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc)
{
    //第一步, 进行参数合法性检测
    //宽度缩放比
    uint32 fScalex =(wWidthSrc<<MATH_FACTOR_BIT) / wWidthDst;
    uint32 fScaley =(wHeightSrc<<MATH_FACTOR_BIT) / wHeightDst;
    
    //指向目标数据
    uint16* pbyDst = pBitsDst16;
    uint16* pbySrc;
    int i,j;
    uint32 xx,yy;
    
    //完成变换
    for( i = 0; i < wHeightDst;i++)
    {
        //取整
        yy = (i * fScaley)>>MATH_FACTOR_BIT;
        pbySrc = pBitsSrc16 + yy * nPitchSrc;
        
        for( j = 0;j < wWidthDst;j++)
        {
            //取整
            xx = (j * fScalex)>>MATH_FACTOR_BIT;
            
            //获取数据
            *(pbyDst+j) = *(pbySrc + xx);
        }
        pbyDst += nPitchDst;
    }
}

static void SoftDSP_CatchSensorData(void)
{
    uint16 *pBuff = NULL;
    uint32  pixelSize = 0;
    CAMSoftDSP_EndOfFrameMessageType *pEofFrame;
    
    if(g_SoftDSPInfo.bCaptureState)
    {
        pBuff = g_SoftDSPInfo.pCaptureBuff;
        pixelSize = g_SoftDSPInfo.nCaptureWidth*g_SoftDSPInfo.nCaptureHeight;
    }
    else
    {
        pBuff = SoftDSP_GetPreviewBuff();
        pixelSize = g_SoftDSPInfo.nPreviewWidth*g_SoftDSPInfo.nPreviewHeight;
    }
    
    if(pBuff)
    {
#ifdef TEST_ENABLE
    {
        static byte color = 0;
        memset(pBuff, color++, pixelSize);
    }
#else
    // TODO:
    // 从Sensor获取数据
#endif
        if(g_SoftDSPInfo.bCaptureState)
        {
            uint16 *pDst = SoftDSP_GetPreviewBuff();
            
            // 发送通知
            if(pDst && g_SoftDSPInfo.msgcb)
            {
                SoftDSP_BMPScale(pDst,  g_SoftDSPInfo.nPreviewWidth, g_SoftDSPInfo.nPreviewHeight, g_SoftDSPInfo.nPreviewWidth,
                                 pBuff, g_SoftDSPInfo.nCaptureWidth, g_SoftDSPInfo.nCaptureHeight, g_SoftDSPInfo.nCaptureWidth);
                pEofFrame = &g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME].responsePayload.endOfFrame;
                pEofFrame->frame_seq_num = g_SoftDSPInfo.nSeqNum++;
                pEofFrame->pBuff         = (byte*)pDst;
                g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME;
                SoftDSP_RefreshPreviewBuff();
                g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME]);
            }
            
            // 发送通知
            if(g_SoftDSPInfo.msgcb)
            {
                pEofFrame = &g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME].responsePayload.endOfFrame;
                pEofFrame->frame_seq_num = g_SoftDSPInfo.nSeqNum++;
                pEofFrame->pBuff         = (byte*)pBuff;
                g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME;
                g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME]);
            }
        }
        else
        {
            // 发送通知
            if(g_SoftDSPInfo.msgcb)
            {
                pEofFrame = &g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME].responsePayload.endOfFrame;
                pEofFrame->frame_seq_num = g_SoftDSPInfo.nSeqNum++;
                pEofFrame->pBuff         = (byte*)pBuff;
                g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME;
                SoftDSP_RefreshPreviewBuff();
                g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME]);
            }
        }
    }
}

//================================================================================================
// Export Function
//================================================================================================
void SoftDSP_Init(void)
{
    memset(&g_SoftDSPInfo,0,sizeof(g_SoftDSPInfo));
#ifdef TEST_ENABLE
    clk_def (&softdsp_test_clk);
#endif
}

void SoftDSP_Exit(void)
{
}

int SoftDSP_Start(const camsensor_static_params_type *camsensor_params)
{
    g_SoftDSPInfo.camsensor_params = camsensor_params;
    return 0;
}

int SoftDSP_Stop(void)
{
#ifdef TEST_ENABLE
    clk_dereg (&softdsp_test_clk);
#endif
    memset(&g_SoftDSPInfo,0,sizeof(g_SoftDSPInfo));
    return 0;
}

int SoftDSP_PushPreviewBuff(byte *pBuff)
{
    int i;
    for(i=0;i<SOFTDSP_PREVIEW_BUFF_MAX;i++)
    {
        if(g_SoftDSPInfo.ppPreviewBuff[i] == NULL)
        {
            g_SoftDSPInfo.ppPreviewBuff[i] = (uint16*)pBuff;
            return 0;
        }
    }
    return 1;
}

int SoftDSP_Preview(CAMSoftDSP_MsgCBType cb, int dx, int dy)
{
    g_SoftDSPInfo.msgcb = cb;
    g_SoftDSPInfo.nPreviewWidth  = dx;
    g_SoftDSPInfo.nPreviewHeight = dy;
    g_SoftDSPInfo.bCaptureState  = FALSE;
#ifdef TEST_ENABLE
    clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) 500, 0, FALSE);
#endif
    return 0;
}

int SoftDSP_SetCaptureBuff(byte *pBuff)
{
    g_SoftDSPInfo.pCaptureBuff = (uint16*)pBuff;
    return 0;
}

int SoftDSP_Capture(CAMSoftDSP_MsgCBType cb, int dx, int dy)
{
    g_SoftDSPInfo.msgcb = cb;
    g_SoftDSPInfo.nCaptureWidth  = dx;
    g_SoftDSPInfo.nCaptureHeight = dy;
    g_SoftDSPInfo.bCaptureState  = TRUE;
#ifdef TEST_ENABLE
    clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) 500, 0, FALSE);
#endif
    return 0;
}

int SoftDSP_HandleMSG(int msg, void *pBuff)
{
    switch(msg){
    case CAMSOFTDSP_MSG_START_OF_FRAME:
        SoftDSP_CatchSensorData();
        break;
        
    default:
        break;
    }
    return 0;
}

#endif // CAMERA_USES_SOFTDSP

