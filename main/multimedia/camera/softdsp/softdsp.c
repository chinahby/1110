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
#include "gpio_int.h"

#define SOFTDSP_INT             GPIO_INT_52
#define SOFTDSP_DATA_ADDR       HWIO_GPIO2_IN_1_ADDR
#define SOFTDSP_CLK_ADDR        HWIO_GPIO2_OUT_0_ADDR
#define SOFTDSP_CLK_MASK        0xFFFFFBFF
#define SOFTDSP_CLK_H           0x400
#define SOFTDSP_CLK_L           0x000
#define MATH_FACTOR_BIT         10
//#define SOFT_MCLK
//#define TEST_ENABLE
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
    uint16                 *pThumbNailBuff;
    int                     nThumbWidth;
    int                     nThumbHeight;
    uint16                 *ppPreviewBuff[SOFTDSP_PREVIEW_BUFF_MAX];
    int                     currPreviewBuff;
    int                     nSeqNum;
    CAMSoftDSP_ResponseType MsgRespones[CAMSOFTDSP_MSG_MAX];
    boolean                 bCaptureState;
}g_SoftDSPInfo;

#ifdef TEST_ENABLE
#define UPDATE_TIME_MS  100
static clk_cb_type softdsp_test_clk;
#endif

static void SoftDSP_FrameISR(void)
{
    if(g_SoftDSPInfo.msgcb)
    {
#ifdef SOFT_MCLK
        gpio_tlmm_config(GPIO_OUTPUT_10);
#endif
        g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_START_OF_FRAME;
        g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME]);
    }
}

#ifdef TEST_ENABLE
static void softdsp_test_clk_cb(int ms)
{
    SoftDSP_FrameISR();
    if(!g_SoftDSPInfo.bCaptureState)
    {
        clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) UPDATE_TIME_MS, 0, FALSE);
    }
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

#ifdef SOFT_MCLK
static INLINE void SoftDSP_ClockOut(int nCount)
{
    register uint32 clkh,clkl;
    
    clkh = inpdw(HWIO_GPIO2_OUT_0_ADDR)|SOFTDSP_CLK_H;
    clkl = inpdw(HWIO_GPIO2_OUT_0_ADDR)&SOFTDSP_CLK_MASK;
    nCount >>= 3;
    while(nCount--)
    {
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
    }
}
#endif
#ifdef SOFT_MCLK
static boolean g_bTest = FALSE;
#endif
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
            memset(pBuff, ~color, pixelSize);
            memset(pBuff+(pixelSize/2), color++, pixelSize);
        }
#else
        // 从Sensor获取数据
        int i;
        register uint16 data,*pData;
#ifdef SOFT_MCLK
        register uint32 clkh,clkl;

        clkh = inpdw(HWIO_GPIO2_OUT_0_ADDR)|SOFTDSP_CLK_H;
        clkl = inpdw(HWIO_GPIO2_OUT_0_ADDR)&SOFTDSP_CLK_MASK;

        if(g_bTest)
        {
            while(g_bTest)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
            }
        }
#endif
        pData = pBuff;
        for(i=0;i<pixelSize;i++)
        {
#ifdef SOFT_MCLK
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
#endif
            data = (uint16)(inpdw(SOFTDSP_DATA_ADDR)>>10)<<8;
#ifdef SOFT_MCLK
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
#endif
            data |= (uint16)(inpdw(SOFTDSP_DATA_ADDR)>>10)&0xFF;
            *pData++=data;
        }
#ifdef SOFT_MCLK
        gpio_tlmm_config(GP_PDM);
#endif
#endif

        if(g_SoftDSPInfo.bCaptureState)
        {
            uint16 *pDst = g_SoftDSPInfo.pThumbNailBuff;
            
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
#ifdef SOFT_MCLK
    gpio_tlmm_config(GP_PDM);
#endif
}

#ifndef TEST_ENABLE
static void SoftDSP_EnableISR(void)
{
    (void)gpio_int_set_handler ( SOFTDSP_INT, ACTIVE_LOW, SoftDSP_FrameISR);
}

static void SoftDSP_DisableISR(void)
{
    (void)gpio_int_set_handler ( SOFTDSP_INT, ACTIVE_LOW, NULL);
}
#endif
//================================================================================================
// Export Function
//================================================================================================
void SoftDSP_Init(void)
{
    memset(&g_SoftDSPInfo,0,sizeof(g_SoftDSPInfo));
    (void)gpio_int_set_detect( SOFTDSP_INT, DETECT_EDGE);
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
#else
    SoftDSP_DisableISR();
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
    clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) UPDATE_TIME_MS, 0, FALSE);
#else
    SoftDSP_EnableISR();
#endif
    return 0;
}

int SoftDSP_SetCaptureBuff(byte *pBuff, byte *pThumbNail)
{
    g_SoftDSPInfo.pCaptureBuff = (uint16*)pBuff;
    g_SoftDSPInfo.pThumbNailBuff = (uint16*)pThumbNail;
    return 0;
}

int SoftDSP_Capture(CAMSoftDSP_MsgCBType cb, int dx, int dy, int thumbdx, int thumbdy)
{
    g_SoftDSPInfo.msgcb = cb;
    g_SoftDSPInfo.nCaptureWidth  = dx;
    g_SoftDSPInfo.nCaptureHeight = dy;
    g_SoftDSPInfo.nThumbWidth    = thumbdx;
    g_SoftDSPInfo.nThumbHeight   = thumbdy;
    g_SoftDSPInfo.bCaptureState  = TRUE;
#ifdef TEST_ENABLE
    clk_reg (&softdsp_test_clk, softdsp_test_clk_cb , (int32) UPDATE_TIME_MS, 0, FALSE);
#else
    SoftDSP_EnableISR();
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

