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
#define SOFTDSP_PCLK_MASK       0x40000
#define SOFTDSP_HSYNC_MASK      0x80000
#define MATH_FACTOR_BIT         10
#define SOFT_MCLK
#define SOFTDSP_KICK_DOG()      HWIO_OUTM(WDOG_RESET,HWIO_FMSK(WDOG_RESET,WATCH_DOG),1)

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

static void SoftDSP_FrameISR(void)
{
    if(g_SoftDSPInfo.msgcb)
    {
#ifndef FEATURE_DSP
#ifdef SOFT_MCLK
        gpio_tlmm_config(GPIO_OUTPUT_10);
#endif
#endif
        g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME].responseMsg = CAMSOFTDSP_MSG_START_OF_FRAME;
        g_SoftDSPInfo.msgcb(&g_SoftDSPInfo.MsgRespones[CAMSOFTDSP_MSG_START_OF_FRAME]);
    }
}

static void SoftDSP_EnableISR(void)
{
    (void)gpio_int_set_handler ( SOFTDSP_INT, ACTIVE_LOW, SoftDSP_FrameISR);
}

static void SoftDSP_DisableISR(void)
{
    (void)gpio_int_set_handler ( SOFTDSP_INT, ACTIVE_LOW, NULL);
}

static void SoftDSP_RefreshPreviewBuff(void)
{
    int i;
    g_SoftDSPInfo.ppPreviewBuff[g_SoftDSPInfo.currPreviewBuff] = NULL;
    for(i=0;i<SOFTDSP_PREVIEW_BUFF_MAX;i++)
    {
        if(g_SoftDSPInfo.ppPreviewBuff[i] != NULL)
        {
            g_SoftDSPInfo.currPreviewBuff = i;
            break;
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
    int nMod;
    
    clkh = inpdw(HWIO_GPIO2_OUT_0_ADDR)|SOFTDSP_CLK_H;
    clkl = inpdw(HWIO_GPIO2_OUT_0_ADDR)&SOFTDSP_CLK_MASK;
    nMod = nCount%8;
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
        SOFTDSP_KICK_DOG();
    }

    while(nMod--)
    {
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
        outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
    }
}
#endif

static void SoftDSP_Catch2x1Data(void)
{
    uint16 *pBuff = NULL;
    uint32  pixelSize = 0;
    CAMSoftDSP_EndOfFrameMessageType *pEofFrame;
    
    if(g_SoftDSPInfo.bCaptureState)
    {
        pBuff = g_SoftDSPInfo.pCaptureBuff;
        pixelSize = g_SoftDSPInfo.nCaptureWidth;
        SoftDSP_DisableISR();
    }
    else
    {
        pBuff = SoftDSP_GetPreviewBuff();
        pixelSize = g_SoftDSPInfo.nPreviewWidth;
    }
    
    if(pBuff)
    {
        // 从Sensor获取数据
        int x,y;
        register uint16 data,*pData;
#ifdef SOFT_MCLK
        register uint32 clkh,clkl;
        int xStart,xOutEnd;
        
        clkh = inpdw(HWIO_GPIO2_OUT_0_ADDR)|SOFTDSP_CLK_H;
        clkl = inpdw(HWIO_GPIO2_OUT_0_ADDR)&SOFTDSP_CLK_MASK;
        xStart = g_SoftDSPInfo.camsensor_params->camif_window_width_config.firstPixel;
        xOutEnd = g_SoftDSPInfo.camsensor_params->camif_frame_config.pixelsPerLine - \
                  g_SoftDSPInfo.camsensor_params->camif_window_width_config.lastPixel;

        MSG_FATAL("SoftDSP_CatchSensorData 0x%x %d %d",pBuff,xStart,xOutEnd);
        MSG_FATAL("SoftDSP_CatchSensorData %d %d %d",g_SoftDSPInfo.bCaptureState,pixelSize,g_SoftDSPInfo.camsensor_params->camif_window_height_config.lastLine);
        INTLOCK();
        // HSYNC
        while(1)
        {
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
            SOFTDSP_KICK_DOG();
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
            if(!(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK))
            {
                break;
            }
        }
        SOFTDSP_KICK_DOG();
#endif
        pData = pBuff;
#ifdef SOFT_MCLK
        for(y=0;y<g_SoftDSPInfo.camsensor_params->camif_window_height_config.firstLine;y++)
        {
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK)
                {
                    break;
                }
            }
            
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_PCLK_MASK)
                {
                    break;
                }
            }
            SoftDSP_ClockOut(g_SoftDSPInfo.camsensor_params->camif_frame_config.pixelsPerLine*2);
            SOFTDSP_KICK_DOG();
        }
#endif
        for(;y<g_SoftDSPInfo.camsensor_params->camif_window_height_config.lastLine;y++)
        {
#ifdef SOFT_MCLK
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK)
                {
                    break;
                }
            }

            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_PCLK_MASK)
                {
                    break;
                }
            }
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
            
            SoftDSP_ClockOut(xStart*2);
#endif
            for(x=0;x<pixelSize;x++)
            {
#ifdef SOFT_MCLK
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
#endif
                data = (uint16)(inpdw(SOFTDSP_DATA_ADDR)>>10)<<8;
#ifdef SOFT_MCLK
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
#endif
                data |= (uint16)(inpdw(SOFTDSP_DATA_ADDR)>>10)&0xFF;
                *pData++=data;
            }
            SOFTDSP_KICK_DOG();
#ifdef SOFT_MCLK
            SoftDSP_ClockOut(xOutEnd*2);
#endif
        }
		INTFREE();
#ifndef FEATURE_DSP        
#ifdef SOFT_MCLK
        gpio_tlmm_config(GP_PDM);
#endif
#endif
        if(g_SoftDSPInfo.bCaptureState)
        {
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
    else
    {
#ifndef FEATURE_DSP
#ifdef SOFT_MCLK
        gpio_tlmm_config(GP_PDM);
#endif
#endif
    }
}

static void SoftDSP_Catch1x1Data(void)
{
    uint16 *pBuff = NULL;
    uint32  pixelSize = 0;
    CAMSoftDSP_EndOfFrameMessageType *pEofFrame;
    
    if(g_SoftDSPInfo.bCaptureState)
    {
        pBuff = g_SoftDSPInfo.pCaptureBuff;
        pixelSize = g_SoftDSPInfo.nCaptureWidth;
        SoftDSP_DisableISR();
    }
    else
    {
        pBuff = SoftDSP_GetPreviewBuff();
        pixelSize = g_SoftDSPInfo.nPreviewWidth;
    }
    
    if(pBuff)
    {
        // 从Sensor获取数据
        int x,y;
        register uint16 data,*pData;
#ifdef SOFT_MCLK
        register uint32 clkh,clkl;
        int xStart,xOutEnd;
        
        clkh = inpdw(HWIO_GPIO2_OUT_0_ADDR)|SOFTDSP_CLK_H;
        clkl = inpdw(HWIO_GPIO2_OUT_0_ADDR)&SOFTDSP_CLK_MASK;
        xStart = g_SoftDSPInfo.camsensor_params->camif_window_width_config.firstPixel;
        xOutEnd = g_SoftDSPInfo.camsensor_params->camif_frame_config.pixelsPerLine - \
                  g_SoftDSPInfo.camsensor_params->camif_window_width_config.lastPixel;

        MSG_FATAL("SoftDSP_CatchSensorData 0x%x %d %d",pBuff,xStart,xOutEnd);
        MSG_FATAL("SoftDSP_CatchSensorData %d %d %d",g_SoftDSPInfo.bCaptureState,pixelSize,g_SoftDSPInfo.camsensor_params->camif_window_height_config.lastLine);
        INTLOCK();
        // HSYNC
        while(1)
        {
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
            SOFTDSP_KICK_DOG();
            outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
            if(!(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK))
            {
                break;
            }
        }
        SOFTDSP_KICK_DOG();
#endif
        pData = pBuff;
#ifdef SOFT_MCLK
        for(y=0;y<g_SoftDSPInfo.camsensor_params->camif_window_height_config.firstLine;y++)
        {
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK)
                {
                    break;
                }
            }
            
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_PCLK_MASK)
                {
                    break;
                }
            }
            SoftDSP_ClockOut(g_SoftDSPInfo.camsensor_params->camif_frame_config.pixelsPerLine);
            SOFTDSP_KICK_DOG();
        }
#endif
        for(;y<g_SoftDSPInfo.camsensor_params->camif_window_height_config.lastLine;y++)
        {
#ifdef SOFT_MCLK
            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_HSYNC_MASK)
                {
                    break;
                }
            }

            while(1)
            {
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkl);
                SOFTDSP_KICK_DOG();
                outpdw(HWIO_GPIO2_OUT_0_ADDR, clkh);
                if(inpdw(SOFTDSP_DATA_ADDR)&SOFTDSP_PCLK_MASK)
                {
                    break;
                }
            }
            
            SoftDSP_ClockOut(xStart);
#endif
            for(x=0;x<pixelSize;x++)
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
            SOFTDSP_KICK_DOG();
#ifdef SOFT_MCLK
            SoftDSP_ClockOut(xOutEnd);
#endif
        }
        INTFREE();
#ifndef FEATURE_DSP
#ifdef SOFT_MCLK
        gpio_tlmm_config(GP_PDM);
#endif
#endif        
        if(g_SoftDSPInfo.bCaptureState)
        {
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
    else
    {
#ifndef FEATURE_DSP
#ifdef SOFT_MCLK
        gpio_tlmm_config(GP_PDM);
#endif
#endif
    }
}

//================================================================================================
// Export Function
//================================================================================================
void SoftDSP_Init(void)
{
    memset(&g_SoftDSPInfo,0,sizeof(g_SoftDSPInfo));
    (void)gpio_int_set_detect( SOFTDSP_INT, DETECT_EDGE);
}

void SoftDSP_Exit(void)
{
}

int SoftDSP_Start(const camsensor_static_params_type *camsensor_params)
{
    MSG_CAMERADEBUG("SoftDSP_Start", 0, 0, 0);
    g_SoftDSPInfo.camsensor_params = camsensor_params;
    return 0;
}

int SoftDSP_Stop(void)
{
    int i;

    MSG_CAMERADEBUG("SoftDSP_Stop", 0, 0, 0);
    SoftDSP_DisableISR();
    g_SoftDSPInfo.msgcb = NULL;
    g_SoftDSPInfo.nPreviewWidth  = 0;
    g_SoftDSPInfo.nPreviewHeight = 0;
    g_SoftDSPInfo.bCaptureState  = FALSE;
    g_SoftDSPInfo.currPreviewBuff = 0;
    
    for(i=0;i<SOFTDSP_PREVIEW_BUFF_MAX;i++)
    {
        g_SoftDSPInfo.ppPreviewBuff[i] = NULL;
    }
    
    g_SoftDSPInfo.pCaptureBuff   = NULL;
    g_SoftDSPInfo.nCaptureWidth  = 0;
    g_SoftDSPInfo.nCaptureHeight = 0;
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
            g_SoftDSPInfo.currPreviewBuff = i;
            MSG_CAMERADEBUG("SoftDSP_PushPreviewBuff %d 0x%x", g_SoftDSPInfo.currPreviewBuff, g_SoftDSPInfo.ppPreviewBuff[i], 0);
            return 0;
        }
    }
    return 1;
}

int SoftDSP_Preview(CAMSoftDSP_MsgCBType cb, int dx, int dy)
{
    MSG_CAMERADEBUG("SoftDSP_Preview %d %d", dx, dy, 0);
    g_SoftDSPInfo.msgcb = cb;
    g_SoftDSPInfo.nPreviewWidth  = dx;
    g_SoftDSPInfo.nPreviewHeight = dy;
    g_SoftDSPInfo.bCaptureState  = FALSE;
    g_SoftDSPInfo.currPreviewBuff = 0;
    SoftDSP_EnableISR();
    return 0;
}

int SoftDSP_SetCaptureBuff(byte *pBuff)
{
    g_SoftDSPInfo.pCaptureBuff = (uint16*)pBuff;
    return 0;
}

int SoftDSP_Capture(CAMSoftDSP_MsgCBType cb, int dx, int dy)
{
    MSG_CAMERADEBUG("SoftDSP_Capture %d %d", dx, dy, 0);
    g_SoftDSPInfo.msgcb = cb;
    g_SoftDSPInfo.nCaptureWidth  = dx;
    g_SoftDSPInfo.nCaptureHeight = dy;
    g_SoftDSPInfo.bCaptureState  = TRUE;
    SoftDSP_EnableISR();
    return 0;
}

int SoftDSP_HandleMSG(int msg, void *pBuff)
{
    switch(msg){
    case CAMSOFTDSP_MSG_START_OF_FRAME:
        if(g_SoftDSPInfo.camsensor_params->pixel_clock == 1)
        {
            SoftDSP_Catch1x1Data();
        }
        else
        {
            SoftDSP_Catch2x1Data();
        }
        break;
        
    default:
        break;
    }
    return 0;
}

#endif // CAMERA_USES_SOFTDSP

