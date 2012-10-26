//**************************************************************
//Copyright (C), 2008-2008, AnyLook Co., Ltd.
//File name:    Rendering_Type16.h
//Author:       AnyLook
//Version:      1.0
//Date:         2008/07/31
//Description:
//   Rendering Include file
//Others: 
//Function List: 
//<Name>        <desc>
//Simple        
//History: 
//<author>      <time>      <version >  <desc>
// AnyLook    2009/01/09       1.0       build this moudle  
//**************************************************************
#include "Rendering.h"
#include "Rendering_Type.h"
#include "Rendering_Config.h"

#define MATH_FACTOR_BIT         10
#define MATH_ALPHA_MAX_OUT      26
#define MATH_ALPHA_MAX          32
#define MATH_ALPHA_MIN_IN       10
#define MATH_ALPHA_MAX_BIT      5
#define MATH_STEPPER(v,s)       ((v*g_StepPer[s])>>MATH_FACTOR_BIT)
#define MATH_STEPPER_AVG(v,s)   ((v*g_StepPer_avg[s])>>MATH_FACTOR_BIT)
#define MATH_STEPPER_SHAKE(v,s) ((v*g_StepPer_shake[s])>>MATH_FACTOR_BIT)

static const int    g_StepPer[REND_STEP_MAX+1]       = {0, 160, 316, 465, 602, 724, 828, 912, 974,1011,1024};
static const int    g_StepPer_avg[REND_STEP_MAX+1]   = {0, 102, 204, 306, 408, 510, 612, 714, 816, 918,1024};
static const int    g_StepPer_shake[REND_STEP_MAX+1] = {0, 160, 316, 465, 602, 724,1024, 912,1024,1011,1024};

static const int16  g_SinCos_Tab[360] = {
    0,   18,   36,   54,   71,   89,  107,  125,  143,  160,  178,  195,  213,  230,  248,
  265,  282,  299,  316,  333,  350,  367,  384,  400,  416,  433,  449,  465,  481,  496,
  512,  527,  543,  558,  573,  587,  602,  616,  630,  644,  658,  672,  685,  698,  711,
  724,  737,  749,  761,  773,  784,  796,  807,  818,  828,  839,  849,  859,  868,  878,
  887,  896,  904,  912,  920,  928,  935,  943,  949,  956,  962,  968,  974,  979,  984,
  989,  994,  998, 1002, 1005, 1008, 1011, 1014, 1016, 1018, 1020, 1022, 1023, 1023, 1024,
 1024, 1024, 1023, 1023, 1022, 1020, 1018, 1016, 1014, 1011, 1008, 1005, 1002,  998,  994,
  989,  984,  979,  974,  968,  962,  956,  949,  943,  935,  928,  920,  912,  904,  896,
  887,  878,  868,  859,  849,  839,  828,  818,  807,  796,  784,  773,  761,  749,  737,
  724,  711,  698,  685,  672,  658,  644,  630,  616,  602,  587,  573,  558,  543,  527,
  512,  496,  481,  465,  449,  433,  416,  400,  384,  367,  350,  333,  316,  299,  282,
  265,  248,  230,  213,  195,  178,  160,  143,  125,  107,   89,   71,   54,   36,   18,
    0,  -18,  -36,  -54,  -71,  -89, -107, -125, -143, -160, -178, -195, -213, -230, -248,
 -265, -282, -299, -316, -333, -350, -367, -384, -400, -416, -433, -449, -465, -481, -496,
 -512, -527, -543, -558, -573, -587, -602, -616, -630, -644, -658, -672, -685, -698, -711,
 -724, -737, -749, -761, -773, -784, -796, -807, -818, -828, -839, -849, -859, -868, -878,
 -887, -896, -904, -912, -920, -928, -935, -943, -949, -956, -962, -968, -974, -979, -984,
 -989, -994, -998,-1002,-1005,-1008,-1011,-1014,-1016,-1018,-1020,-1022,-1023,-1023,-1024,
-1024,-1024,-1023,-1023,-1022,-1020,-1018,-1016,-1014,-1011,-1008,-1005,-1002, -998, -994,
 -989, -984, -979, -974, -968, -962, -956, -949, -943, -935, -928, -920, -912, -904, -896,
 -887, -878, -868, -859, -849, -839, -828, -818, -807, -796, -784, -773, -761, -749, -737,
 -724, -711, -698, -685, -672, -658, -644, -630, -616, -602, -587, -573, -558, -543, -527,
 -512, -496, -481, -465, -449, -433, -416, -400, -384, -367, -350, -333, -316, -299, -282,
 -265, -248, -230, -213, -195, -178, -160, -143, -125, -107,  -89,  -71,  -54,  -36,  -18,
};

void *Rendering_Memcpy16(void *dest, void *src, int size)
{
    uint16 *pDest = (uint16*)dest;
    uint16 *pSrc  = (uint16*)src;
    int     sizemod;
    
    size = size>>1;
    sizemod = size&0x7;
    size = size>>3;
    while(size--)
    {
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
    }
    
    while(sizemod--)
    {
        *pDest++ = *pSrc++;
    }
	return dest;
}

void *Rendering_Memcpy32(void *dest, void *src, int size)
{
    uint32 *pDest = (uint32*)dest;
    uint32 *pSrc  = (uint32*)src;
    int     sizemod;
    
    size = size>>2;
    sizemod = size&0x7;
    size = size>>3;
    while(size--)
    {
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
    }
    
    while(sizemod--)
    {
        *pDest++ = *pSrc++;
    }
	return dest;

}

void *Rendering_Memset16(void *buffer, int c, int size)
{
    uint16 *p = (uint16 *)buffer;
    int sizemod;
    size = size>>1;
    sizemod = size&0x7;
    size = size>>3;
    while(size--)
    {
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
    }
    
    while(sizemod--)
    {
        *p++ = c;
    }
	return buffer;
}

void *Rendering_Memset32(void *buffer, int c, int size)
{
    uint32 *p = (uint32 *)buffer;
    int sizemod;
    size = size>>2;
    sizemod = size&0x7;
    size = size>>3;
    while(size--)
    {
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
    }
    
    while(sizemod--)
    {
        *p++ = c;
    }
	return buffer;
}

static INLINE int Fun_GetSizeByBPP(RendDraw *pMe, int nWidth)
{
    return (((nWidth<<3)*pMe->nDepth)+63)>>6;
}

static INLINE int16 Fun_MathSIN(int16 Angle)
{
    if(Angle>=0)
    {
        return g_SinCos_Tab[Angle%360];
    }
    else
    {
        Angle = -Angle;
        return (-g_SinCos_Tab[Angle%360]);
    }
}

static INLINE int16 Fun_MathCOS(int16 Angle)
{
    if(Angle>=0)
    {
        return g_SinCos_Tab[(90+Angle)%360];
    }
    else
    {
        Angle = -Angle;
        return g_SinCos_Tab[(90+Angle)%360];
    }
}
#define SIN(A) Fun_MathSIN(A)
#define COS(A) Fun_MathCOS(A)

static INLINE int16 Fun_GetxyOffAngle(int16 Angle, int16 zxAngle)
{
    Angle = Angle%360;
    
    if(Angle <0) 
    {
        Angle = 360+Angle;
    }

    if(Angle <= 90)
    {
        Angle = Angle*zxAngle/90;
    }
    else if(Angle <= 180)
    {
        Angle = (Angle-180)*zxAngle/90;
    }
    else if(Angle <= 270)
    {
        Angle = (Angle-180)*zxAngle/90;
    }
    else
    {
        Angle = (Angle-360)*zxAngle/90;
    }
    return Angle;
}

static INLINE void Fun_BMPScale(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
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

static INLINE void Fun_BMPCopy(uint16 *pBitsDst16, uint16* pBitsSrc16, int16 wCpyWidth, int16 wCpyHeight, int16 nPitch)
{
    if(wCpyWidth == nPitch)
    {
        REND_MEMCPY16(pBitsDst16, pBitsSrc16, wCpyHeight*nPitch*sizeof(uint16));
    }
    else
    {
        int i;
        int nSize;
        int nSizeMod;
        uint16 *pDest,*pSrc;
        
        for(i=0; i<wCpyHeight; i++)
        {
            pDest    = pBitsDst16;
            pSrc     = pBitsSrc16;
            nSize    = wCpyWidth>>3;
            nSizeMod = wCpyWidth&0x7;
            
            while(nSize--)
            {
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
                *pDest++ = *pSrc++;
            }
            
            while(nSizeMod--)
            {
                *pDest++ = *pSrc++;
            }
            
            pBitsDst16 += nPitch;
            pBitsSrc16 += nPitch;
        }
    }
}

//wAlpha: 透明程度，分为32等级，0是100%透明,pBitsBg16是pBitsDst16的背景色
static void Fun_BMPScaleAlpha(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst, 
                              uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc, uint16 *pBitsBg16, uint16 wAlpha)
{
    //第一步, 进行参数合法性检测
    //宽度缩放比
    uint32 fScalex =(wWidthSrc<<MATH_FACTOR_BIT) / wWidthDst;
    uint32 fScaley =(wHeightSrc<<MATH_FACTOR_BIT) / wHeightDst;
    
    //指向目标数据
    uint16* pbyDst    = pBitsDst16;
    uint16* pbyBg     = pBitsBg16;
    uint16* pbySrc;
    int i,j;
    uint32 xx,yy;
    uint16 R1, G1, B1;
    uint16 R2, G2, B2;
    uint16 rgb16;
    
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
            rgb16      = *(pbySrc + xx);
            R1 = (rgb16 & 0xF800);
            G1 = (rgb16 & 0x07E0);
            B1 = (rgb16 & 0x001F);
            
            rgb16 = *(pbyBg+j);
            R2 = (rgb16 & 0xF800);
            G2 = (rgb16 & 0x07E0);
            B2 = (rgb16 & 0x001F);
            
            *(pbyDst+j) = (uint16)(((((R1-R2)*wAlpha)>>MATH_ALPHA_MAX_BIT)+R2)& 0xF800)
                         |(uint16)(((((G1-G2)*wAlpha)>>MATH_ALPHA_MAX_BIT)+G2)& 0x07E0)
                         |(uint16)(((((B1-B2)*wAlpha)>>MATH_ALPHA_MAX_BIT)+B2)& 0x001F);
        }
        pbyDst += nPitchDst;
        pbyBg  += nPitchDst;
    }
}

#if 0
// 带xyz旋转角度轴的变换
// xAngle:绕x轴旋转角度
// yAngle:绕y轴旋转角度
// zAngle:绕z轴旋转角度
// xCenter:绕轴旋转的x中心点
// yCenter:绕轴旋转的y中心点
// z:z轴深度
// zxAngle:z轴与x轴之间的夹角，旋转时的最大角度
static void Fun_BMPxyzScale(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
                            uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc,
                            int16 xAngle, int16 yAngle, int16 zAngle, int16 xCenter, int16 yCenter, int16 z, int16 zxAngle)
{
    //宽度缩放比
    int32 fScaleX,newfScaleX;
    int32 fScaleY,newfScaleY;

    //指向目标数据
    uint16* pbyDst = pBitsDst16;
    int16 newDstWidth, newDstHeight,xOff,yOff,newxOff,newyOff;
    int32 x,y,xx,yx,xy,yy,xz,yz;
    int i,j;
    int16 newxAngle,newyAngle,COSNewX,COSNewY,SINNewX,SINNewY;
    int16 COSX,COSY,COSZ,SINZ;
    
    if(wWidthDst<= 0 || wHeightDst<=0 || wWidthSrc<= 0 || wHeightSrc<=0)
    {
        return;
    }

    newxAngle = Fun_GetxyOffAngle(xAngle, zxAngle);
    newyAngle = Fun_GetxyOffAngle(yAngle, zxAngle);
    COSNewX   = COS(newxAngle);
    COSNewY   = COS(newyAngle);
    SINNewX   = SIN(newxAngle);
    SINNewY   = SIN(newyAngle);
    COSX      = COS(xAngle);
    COSY      = COS(yAngle);
    COSZ      = COS(zAngle);
    SINZ      = SIN(zAngle);

    if(COSY == 0) COSY = 1;
    if(COSX == 0) COSX = 1;

    newDstWidth  = wWidthDst - z*wWidthDst/REND_Z_MAX;
    newDstHeight = wHeightDst - z*wHeightDst/REND_Z_MAX;
    xOff = (newDstWidth - wWidthDst)*xCenter/wWidthSrc;
    yOff = (newDstHeight - wHeightDst)*yCenter/wHeightSrc;
    
    if(newDstWidth<= 0 || newDstHeight<=0)
    {
        return;
    }

    fScaleX = (wWidthSrc<<MATH_FACTOR_BIT) /newDstWidth;
    fScaleY = (wHeightSrc<<MATH_FACTOR_BIT) / newDstHeight;

    //完成变换
    for( i = 0; i < wHeightDst;i++)
    {
        // 缩放
        y = ((i+yOff) * fScaleY)>>MATH_FACTOR_BIT;

        for( j = 0;j < wWidthDst;j++)
        {
            // 缩放
            x = ((j+xOff) * fScaleX)>>MATH_FACTOR_BIT;
            // z轴旋转
            // 如果是逆时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   -   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   +   (x1   -   x0)   *   sina   +   y0   
            // 如果是顺时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   +   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   -   (x1   -   x0)   *   sina   +   y0
            xz = (((x-xCenter)*COSZ -(y-yCenter)*SINZ)>>MATH_FACTOR_BIT) + xCenter;
            yz = (((y-yCenter)*COSZ +(x-xCenter)*SINZ)>>MATH_FACTOR_BIT) + yCenter;
            
            // y轴旋转
            newDstHeight = wHeightSrc - ((int16)xz-xCenter)*SINNewY/COSNewY;
            newyOff = (newDstHeight - wHeightSrc)*yCenter/wHeightSrc;
            newfScaleY = (wHeightSrc<<MATH_FACTOR_BIT) / newDstHeight;
            xy = ((xz-xCenter)<<MATH_FACTOR_BIT)/COSY+xCenter;
            //yy = yz + (xz-xCenter)*SINNewY/COSNewY;
            yy = ((yz+newyOff)*newfScaleY)>>MATH_FACTOR_BIT;
            
            // x轴旋转
            newDstWidth = wWidthSrc - ((int16)yy-yCenter)*SINNewX/COSNewX;
            newxOff = (newDstWidth - wWidthSrc)*xCenter/wWidthSrc;
            newfScaleX = (wWidthSrc<<MATH_FACTOR_BIT) / newDstWidth;
            //xx = xy + (yy-yCenter)*SINNewX/COSNewX;
            xx = ((xy+newxOff)*newfScaleX)>>MATH_FACTOR_BIT;
            yx = ((yy-yCenter)<<MATH_FACTOR_BIT)/(COSX)+yCenter;
            
            //获取数据
            if(xx >= wWidthSrc || yx >= wHeightSrc || xx<0 || yx<0)
            {
                *(pbyDst+j) = REND_DEFAULT_CLR;
            }
            else
            {
                *(pbyDst+j) = *(pBitsSrc16 + (yx*nPitchSrc) + xx);
            }
        }
        pbyDst += nPitchDst;
    }
}
#endif

// 带xyz旋转角度轴的变换
// xAngle:绕x轴旋转角度
// xCenter:绕轴旋转的x中心点
// yCenter:绕轴旋转的y中心点
// z:z轴深度
// zxAngle:z轴与x轴之间的夹角，旋转时的最大角度
static void Fun_BMPxScaleZ(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
                           uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc,
                           int16 xAngle, int16 xCenter, int16 yCenter, int16 z, int16 zxAngle)
{
    //宽度缩放比
    int32 fScaleX;
    int32 fScaleY;

    //指向目标数据
    uint16* pbyDst = pBitsDst16;
    uint16* pbySrc;
    int16 newDstWidth, newDstHeight,xOff,yOff;
    int32 x,y,xx,yx;
    int i,j,iMax,jMax,jj;
    int16 newxAngle,COSNewX,SINNewX;
    int16 COSX;

    newxAngle = Fun_GetxyOffAngle(xAngle, zxAngle);
    COSNewX   = COS(newxAngle);
    SINNewX   = SIN(newxAngle);
    COSX      = COS(xAngle);
    
    if(COSX == 0) COSX = 1;
    
    newDstWidth  = wWidthDst - z*wWidthDst/REND_Z_MAX;
    newDstHeight = wHeightDst - z*wHeightDst/REND_Z_MAX;
    xOff = (newDstWidth - wWidthDst)*xCenter/wWidthSrc;
    yOff = (newDstHeight - wHeightDst)*yCenter/wHeightSrc;
    
    if(newDstWidth<= 0 || newDstHeight<=0)
    {
        return;
    }

    fScaleX = (wWidthSrc<<MATH_FACTOR_BIT) /newDstWidth;
    fScaleY = (wHeightSrc<<MATH_FACTOR_BIT) / newDstHeight;
    iMax = wHeightDst+yOff;
    jMax = wWidthDst +xOff;

    if(wWidthSrc == newDstWidth)
    {
        //完成变换
        for(i=yOff; i<iMax; i++)
        {
            // 缩放
            y = (i*fScaleY)>>MATH_FACTOR_BIT;
            yx = ((y-yCenter)<<MATH_FACTOR_BIT)/(COSX)+yCenter;
            
            if(yx >= wHeightSrc || yx<0)
            {
                for(j=0; j<wWidthDst; j++)
                {
                    *(pbyDst+j) = REND_DEFAULT_CLR;
                }
            }
            else
            {
                newDstWidth = wWidthSrc - ((int16)y-yCenter)*SINNewX/COSNewX;
                pbySrc = pBitsSrc16 + (yx * nPitchSrc);
                
                for(j=0 ; j<wWidthDst; j++)
                {
                    // x轴旋转
                    xx = ((j - xCenter)*wWidthSrc)/newDstWidth + xCenter;
                    
                    //获取数据
                    if(xx >= wWidthSrc || xx<0)
                    {
                        *(pbyDst+j) = REND_DEFAULT_CLR;
                    }
                    else
                    {
                        *(pbyDst+j) = *(pbySrc + xx);
                    }
                }
            }
            pbyDst += nPitchDst;
        }
    }
    else
    {
        //完成变换
        for(i=yOff; i<iMax; i++)
        {
            // 缩放
            y = (i*fScaleY)>>MATH_FACTOR_BIT;
            yx = ((y-yCenter)<<MATH_FACTOR_BIT)/(COSX)+yCenter;
            
            if(yx >= wHeightSrc || yx<0)
            {
                for(j=0; j<wWidthDst; j++)
                {
                    *(pbyDst+j) = REND_DEFAULT_CLR;
                }
            }
            else
            {
                newDstWidth = wWidthSrc - ((int16)y-yCenter)*SINNewX/COSNewX;
                pbySrc = pBitsSrc16 + (yx * nPitchSrc);
                
                for(j=xOff, jj=0; j<jMax; j++, jj++)
                {
                    // 缩放
                    x = (j*fScaleX)>>MATH_FACTOR_BIT;
                    
                    // x轴旋转
                    xx = ((x - xCenter)*wWidthSrc)/newDstWidth + xCenter;
                    
                    //获取数据
                    if(xx >= wWidthSrc || xx<0)
                    {
                        *(pbyDst+jj) = REND_DEFAULT_CLR;
                    }
                    else
                    {
                        *(pbyDst+jj) = *(pbySrc + xx);
                    }
                }
            }
            pbyDst += nPitchDst;
        }
    }
}

// 带xyz旋转角度轴的变换
// yAngle:绕y轴旋转角度
// xCenter:绕轴旋转的x中心点
// yCenter:绕轴旋转的y中心点
// z:z轴深度
// zxAngle:z轴与x轴之间的夹角，旋转时的最大角度
static void Fun_BMPyScaleZ(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
                           uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc,
                           int16 yAngle, int16 xCenter, int16 yCenter, int16 z, int16 zxAngle)
{
    //宽度缩放比
    int32 fScaleX;
    int32 fScaleY;

    //指向目标数据
    uint16* pbyDst = pBitsDst16;
    uint16* pbySrc;
    int16 newDstWidth, newDstHeight,xOff,yOff;
    int32 x,y,xy,yy;
    int i,j,iMax,jMax,jj;
    int16 newyAngle,COSNewY,SINNewY;
    int16 COSY;
    
    newyAngle = Fun_GetxyOffAngle(yAngle, zxAngle);
    COSNewY   = COS(newyAngle);
    SINNewY   = SIN(newyAngle);
    COSY      = COS(yAngle);

    if(COSY == 0) COSY = 1;

    newDstWidth  = wWidthDst - z*wWidthDst/REND_Z_MAX;
    newDstHeight = wHeightDst - z*wHeightDst/REND_Z_MAX;
    xOff = (newDstWidth - wWidthDst)*xCenter/wWidthSrc;
    yOff = (newDstHeight - wHeightDst)*yCenter/wHeightSrc;
    
    if(newDstWidth<= 0 || newDstHeight<=0)
    {
        return;
    }

    fScaleX = (wWidthSrc<<MATH_FACTOR_BIT) /newDstWidth;
    fScaleY = (wHeightSrc<<MATH_FACTOR_BIT) / newDstHeight;
    iMax = wHeightDst+yOff;
    jMax = wWidthDst +xOff;
    
    if(wHeightSrc == newDstHeight)
    {
        //完成变换
        for(j=xOff, jj=0; j<jMax; j++, jj++)
        {
            pbyDst = pBitsDst16+jj;
            // 缩放
            x = (j*fScaleX)>>MATH_FACTOR_BIT;
            xy = ((x-xCenter)<<MATH_FACTOR_BIT)/COSY+xCenter;

            if(xy >= wWidthSrc || xy<0)
            {
                for( i = 0;i < wHeightDst;i++)
                {
                    *pbyDst = REND_DEFAULT_CLR;
                    pbyDst += nPitchDst;
                }
            }
            else
            {
                newDstHeight = wHeightSrc - ((((int16)x-xCenter)*SINNewY)/COSNewY);
                pbySrc = pBitsSrc16 + xy;
                
                for(i=yOff; i<iMax; i++)
                {                   
                    // y轴旋转
                    yy = ((i - yCenter)*wHeightSrc)/newDstHeight + yCenter; 
                    
                    //获取数据
                    if(yy >= wHeightSrc || yy<0)
                    {
                        *pbyDst = REND_DEFAULT_CLR;
                    }
                    else
                    {
                        *pbyDst = *(pbySrc + (yy * nPitchSrc));
                    }
                    pbyDst += nPitchDst;
                }
            }
        }
    }
    else
    {
        //完成变换
        for(j=xOff, jj=0; j<jMax; j++, jj++)
        {
            pbyDst = pBitsDst16+jj;
            // 缩放
            x = (j*fScaleX)>>MATH_FACTOR_BIT;
            xy = ((x-xCenter)<<MATH_FACTOR_BIT)/COSY+xCenter;

            if(xy >= wWidthSrc || xy<0)
            {
                for( i = 0;i < wHeightDst;i++)
                {
                    *pbyDst = REND_DEFAULT_CLR;
                    pbyDst += nPitchDst;
                }
            }
            else
            {
                newDstHeight = wHeightSrc - ((((int16)x-xCenter)*SINNewY)/COSNewY);
                pbySrc = pBitsSrc16 + xy;
                
                for(i=yOff; i<iMax; i++)
                {
                    // 缩放
                    y = (i*fScaleY)>>MATH_FACTOR_BIT;
                    
                    // y轴旋转
                    yy = ((y - yCenter)*wHeightSrc)/newDstHeight + yCenter; 
                    
                    //获取数据
                    if(yy >= wHeightSrc || yy<0)
                    {
                        *pbyDst = REND_DEFAULT_CLR;
                    }
                    else
                    {
                        *pbyDst = *(pbySrc + (yy * nPitchSrc));
                    }
                    pbyDst += nPitchDst;
                }
            }
        }
    }
}
#include "AEEStdLib.h"
static void Fun_REND_SHOW_HORZ_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    DBGPRINTF_FATAL("Fun_REND_SHOW_HORZ_OUT Enter %d",Step);
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart;
        int16 w,w1;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;

        w = MATH_STEPPER(pMe->wWidthSrc,Step);
        xStart = (pMe->wWidthSrc-w)>>1;

        pDestBuf = (uint16 *)pMe->pDest+xStart;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        Fun_BMPScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

        w1 = xStart;
        // 复制左边
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2+(pMe->wWidthSrc-w1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, w1, pMe->wHeightSrc, pMe->wWidthSrc);
        
        // 复制右边
        pDestBuf = (uint16 *)pMe->pDest+pMe->wWidthSrc-w1;
        pSrcBuf  = (uint16 *)pMe->pSrc2+(pMe->wWidthSrc>>1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, w1, pMe->wHeightSrc, pMe->wWidthSrc);
    }
    DBGPRINTF_FATAL("Fun_REND_SHOW_HORZ_OUT Leave");
}

static void Fun_REND_SHOW_HORZ_IN(RendDraw *pMe, int Step, int MaxStep)
{
    DBGPRINTF_FATAL("Fun_REND_SHOW_HORZ_IN Enter %d",Step);
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart;
        int16 w, w1;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        w = pMe->wWidthSrc-MATH_STEPPER(pMe->wWidthSrc,Step);
        xStart = (pMe->wWidthSrc-w)>>1;
        
        pDestBuf = (uint16 *)pMe->pDest+xStart;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        Fun_BMPScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

        w1 = xStart;
        // 复制左边
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1+((pMe->wWidthSrc>>1)-w1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, w1, pMe->wHeightSrc, pMe->wWidthSrc);
        
        // 复制右边
        pDestBuf = (uint16 *)pMe->pDest+pMe->wWidthSrc-w1;
        pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc>>1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, w1, pMe->wHeightSrc, pMe->wWidthSrc);
    }
    DBGPRINTF_FATAL("Fun_REND_SHOW_HORZ_IN Leave");
}

static void Fun_REND_SHOW_VERT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 yStart;
        int16 h, h1;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        h = MATH_STEPPER(pMe->wHeightSrc,Step);
        yStart = (pMe->wHeightSrc-h)>>1;
        
        pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        Fun_BMPScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

        h1 = yStart;
        // 复制上边
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2+(((pMe->wHeightSrc>>1)-h1)*pMe->wWidthSrc);
        Fun_BMPCopy(pDestBuf, pSrcBuf, pMe->wWidthSrc, h1, pMe->wWidthSrc);

        // 复制下边
        pDestBuf = (uint16 *)pMe->pDest+(pMe->wHeightSrc-h1)*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc2+pMe->wWidthSrc*(pMe->wHeightSrc>>1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, pMe->wWidthSrc, h1, pMe->wWidthSrc);
    }
}

static void Fun_REND_SHOW_VERT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 yStart;
        int16 h,h1;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        h = pMe->wHeightSrc-MATH_STEPPER(pMe->wHeightSrc,Step);
        yStart = (pMe->wHeightSrc-h)>>1;

        pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        Fun_BMPScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

        h1 = yStart;
        // 复制上边
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1+(((pMe->wHeightSrc>>1)-h1)*pMe->wWidthSrc);
        Fun_BMPCopy(pDestBuf, pSrcBuf, pMe->wWidthSrc, h1, pMe->wWidthSrc);

        // 复制下边
        pDestBuf = (uint16 *)pMe->pDest+(pMe->wHeightSrc-h1)*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc1+pMe->wWidthSrc*(pMe->wHeightSrc>>1);
        Fun_BMPCopy(pDestBuf, pSrcBuf, pMe->wWidthSrc, h1, pMe->wWidthSrc);
    }
}

static void Fun_REND_FADE_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        w = pMe->wWidthSrc - MATH_STEPPER_AVG((pMe->wWidthSrc>>1),Step);
        h = pMe->wHeightSrc - MATH_STEPPER_AVG((pMe->wHeightSrc>>1),Step);
        xStart = (pMe->wWidthSrc-w)>>1;
        yStart = (pMe->wHeightSrc-h)>>1;
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2+yStart*pMe->wWidthSrc+xStart;
        Fun_BMPScaleAlpha(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,w,h,pMe->wWidthSrc,pMe->pSrc1,MATH_STEPPER_AVG(MATH_ALPHA_MAX_OUT,(MaxStep-Step)));
    }
}

static void Fun_REND_FADE_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;

        w = pMe->wWidthSrc - MATH_STEPPER_AVG((pMe->wWidthSrc>>1),(MaxStep-Step));
        h = pMe->wHeightSrc - MATH_STEPPER_AVG((pMe->wHeightSrc>>1),(MaxStep-Step));
        xStart = (pMe->wWidthSrc-w)>>1;
        yStart = (pMe->wHeightSrc-h)>>1;
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1+yStart*pMe->wWidthSrc+xStart;
        Fun_BMPScaleAlpha(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,w,h,pMe->wWidthSrc,pMe->pSrc2,MATH_ALPHA_MIN_IN+MATH_STEPPER_AVG((MATH_ALPHA_MAX-MATH_ALPHA_MIN_IN),Step));
    }
}

static void Fun_REND_MOVE_HORZ_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        int i;

        w = MATH_STEPPER(pMe->wWidthSrc,Step);
        h = pMe->wHeightSrc;
        xStart = pMe->wWidthSrc-w;
        yStart = 0;

        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1+xStart;
        
        for(i=0;i<h;i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
        

        pDestBuf = (uint16 *)pMe->pDest+w;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        w = pMe->wWidthSrc - w;
        for(i = 0; i < h; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_MOVE_HORZ_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        int i;

        w = MATH_STEPPER(pMe->wWidthSrc,Step);
        h = pMe->wHeightSrc;
        xStart = pMe->wWidthSrc-w;
        yStart = 0;

        pDestBuf = (uint16 *)pMe->pDest+xStart;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        for(i=0;i<h;i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
        

        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2+w;
        w = pMe->wWidthSrc - w;
        for(i = 0; i < h; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_MOVE_VERT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        int i;

        w = pMe->wWidthSrc;
        h = MATH_STEPPER(pMe->wHeightSrc,Step);
        xStart = 0;
        yStart = pMe->wHeightSrc-h;

        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1+yStart*pMe->wWidthSrc;
        
        for(i=0;i<h;i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
        

        pDestBuf = (uint16 *)pMe->pDest+h*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        h = pMe->wHeightSrc - h;
        for(i = 0; i < h; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_MOVE_VERT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 xStart, yStart;
        int16 w, h;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        int i;

        w = pMe->wWidthSrc;
        h = MATH_STEPPER(pMe->wHeightSrc,Step);
        xStart = 0;
        yStart = pMe->wHeightSrc-h;

        pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        for(i=0;i<h;i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2+h*pMe->wWidthSrc;
        h = pMe->wHeightSrc - h;
        for(i = 0; i < h; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, w));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_FLY_HORZ_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        int Max = MATH_STEPPER_SHAKE(pMe->wWidthSrc,Step);
        uint16 *pDestBuf = (uint16 *)pMe->pDest;
        uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc-Max);
        
        for(i = 0; i < pMe->wHeightSrc; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, Max));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_FLY_HORZ_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        int Max = MATH_STEPPER_SHAKE(pMe->wWidthSrc,Step);
        uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc-Max);
        uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        for(i = 0; i < pMe->wHeightSrc; i++)
        {
            REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, Max));
            pDestBuf += pMe->wWidthSrc;
            pSrcBuf  += pMe->wWidthSrc;
        }
    }
}

static void Fun_REND_FLY_VERT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int Max = MATH_STEPPER_SHAKE(pMe->wHeightSrc,Step);
        uint16 *pDestBuf = (uint16 *)pMe->pDest;
        uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+ (pMe->wWidthSrc*(pMe->wHeightSrc - Max));
        
        REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*Max));
    }
}

static void Fun_REND_FLY_VERT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int Max = MATH_STEPPER_SHAKE(pMe->wHeightSrc,Step);
        uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc*(pMe->wHeightSrc - Max));
        uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        REND_MEMCPY16(pDestBuf, pSrcBuf, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*Max));
    }
}

// 3D左侧旋入
static void Fun_REND_ROTAT_HORZ_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 Angle1 = (90*Step/MaxStep)-90;
        int16 Angle2 = (90*Step/MaxStep);
        int16 w      = MATH_STEPPER(pMe->wWidthSrc,Step);
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        //Fun_BMPxyzScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle1,0,240,160,0,30);
        Fun_BMPyScaleZ(pDestBuf, w,              pMe->wHeightSrc, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc, pMe->wHeightSrc, pMe->wWidthSrc,
                       Angle1,   pMe->wWidthSrc, pMe->wHeightSrc>>1, 0, 30);
        
        pDestBuf = (uint16 *)pMe->pDest+(w);
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc-w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle2,0,0,160,0,30);
        Fun_BMPyScaleZ(pDestBuf, pMe->wWidthSrc-w, pMe->wHeightSrc, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc,   pMe->wHeightSrc, pMe->wWidthSrc,
                       Angle2,   0,                pMe->wHeightSrc>>1, 0, 30);
    }
}

// 3D右侧旋入
static void Fun_REND_ROTAT_HORZ_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 Angle1 = 90-(90*Step/MaxStep);
        int16 Angle2 = -(90*Step/MaxStep);
        int16 w      = MATH_STEPPER(pMe->wWidthSrc,Step);
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc-w);
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        //Fun_BMPxyzScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle1,0,0,160,0,30);
        Fun_BMPyScaleZ(pDestBuf, w,              pMe->wHeightSrc, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc, pMe->wHeightSrc, pMe->wWidthSrc,
                       Angle1,   0,              pMe->wHeightSrc>>1,0,30);
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc-w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle2,0,240,160,0,30);
        Fun_BMPyScaleZ(pDestBuf, pMe->wWidthSrc-w, pMe->wHeightSrc, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc,   pMe->wHeightSrc, pMe->wWidthSrc,
                       Angle2,   pMe->wWidthSrc,   pMe->wHeightSrc>>1, 0, 30);
    }
}

// 3D顶部旋入
static void Fun_REND_ROTAT_VERT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 Angle1 = (90*Step/MaxStep)-90;
        int16 Angle2 = (90*Step/MaxStep);
        int16 h      = MATH_STEPPER(pMe->wHeightSrc,Step);
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle1,0,0,120,320,0,30);
        Fun_BMPxScaleZ(pDestBuf, pMe->wWidthSrc, h,               pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc, pMe->wHeightSrc, pMe->wWidthSrc,
                       Angle1,   pMe->wWidthSrc>>1, pMe->wHeightSrc, 0, 30);
        
        pDestBuf = (uint16 *)pMe->pDest+(h*pMe->wWidthSrc);
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc-h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle2,0,0,120,0,0,30);
        Fun_BMPxScaleZ(pDestBuf, pMe->wWidthSrc, pMe->wHeightSrc-h, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc, pMe->wHeightSrc,   pMe->wWidthSrc,
                       Angle2,   pMe->wWidthSrc>>1, 0, 0, 30);
    }
}

// 3D底部旋入
static void Fun_REND_ROTAT_VERT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int16 Angle1 = 90-(90*Step/MaxStep);
        int16 Angle2 = -(90*Step/MaxStep);
        int16 h      = MATH_STEPPER(pMe->wHeightSrc,Step);
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        
        pDestBuf = (uint16 *)pMe->pDest+(pMe->wHeightSrc-h)*pMe->wWidthSrc;
        pSrcBuf  = (uint16 *)pMe->pSrc1;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle1,0,0,120,0,0,30);
        Fun_BMPxScaleZ(pDestBuf, pMe->wWidthSrc, h,               pMe->wWidthSrc,
                      pSrcBuf,   pMe->wWidthSrc, pMe->wHeightSrc, pMe->wWidthSrc,
                      Angle1,    pMe->wWidthSrc>>1, 0, 0, 30);
        
        pDestBuf = (uint16 *)pMe->pDest;
        pSrcBuf  = (uint16 *)pMe->pSrc2;
        
        //Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc-h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle2,0,0,120,320,0,30);
        Fun_BMPxScaleZ(pDestBuf, pMe->wWidthSrc, pMe->wHeightSrc-h, pMe->wWidthSrc,
                       pSrcBuf,  pMe->wWidthSrc, pMe->wHeightSrc,   pMe->wWidthSrc,
                       Angle2,   pMe->wWidthSrc>>1, pMe->wHeightSrc, 0, 30);
    }
}

#define REND_BLINDS_HORIZONTAL_NUM         8
#define REND_BLINDS_VERTICAL_NUM           10
// 百叶窗左侧
static void Fun_REND_BLINDS_VERT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        uint16 wBlindsWidth = pMe->wWidthSrc/REND_BLINDS_HORIZONTAL_NUM;
        uint16 wBlindsOpenWidth = MATH_STEPPER(wBlindsWidth,Step);
        
        for(i = 0; i < REND_BLINDS_HORIZONTAL_NUM; i++)
        {
            pDestBuf = (uint16 *)pMe->pDest+wBlindsWidth*i;
            pSrcBuf  = (uint16 *)pMe->pSrc1+wBlindsWidth*i;
            Fun_BMPCopy(pDestBuf, pSrcBuf, wBlindsOpenWidth, pMe->wHeightSrc, pMe->wWidthSrc);
        }
    }
}

// 百叶窗右侧
static void Fun_REND_BLINDS_VERT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        uint16 *pDestBuf;
        uint16 *pSrcBuf;
        uint16 wBlindsWidth = pMe->wWidthSrc/REND_BLINDS_HORIZONTAL_NUM;
        uint16 wBlindsOpenWidth = MATH_STEPPER(wBlindsWidth,Step);
        
        for(i = 0; i < REND_BLINDS_HORIZONTAL_NUM; i++)
        {
            pDestBuf = (uint16 *)pMe->pDest+wBlindsWidth*i+wBlindsWidth-wBlindsOpenWidth;
            pSrcBuf  = (uint16 *)pMe->pSrc1+wBlindsWidth*i+wBlindsWidth-wBlindsOpenWidth;
            Fun_BMPCopy(pDestBuf, pSrcBuf, wBlindsOpenWidth, pMe->wHeightSrc, pMe->wWidthSrc);
        }
    }
}

// 左右交叉百叶出
static void Fun_REND_BLINDS_INTER_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        int Max = MATH_STEPPER(pMe->wWidthSrc,Step);
        uint16 *pDestBufLeft  = (uint16 *)pMe->pDest;
        uint16 *pSrcBufLeft   = (uint16 *)pMe->pSrc2 + Max;
        uint16 *pDestBufRight = (uint16 *)pMe->pDest + Max;
        uint16 *pSrcBufRight  = (uint16 *)pMe->pSrc2;

        REND_MEMCPY16(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));

        for(i = 0; i < pMe->wHeightSrc; i++)
        {
            if((i/20)%2 == 0)
            {
                REND_MEMCPY16(pDestBufLeft, pSrcBufLeft, Fun_GetSizeByBPP(pMe, (pMe->wWidthSrc - Max)));
            }
            else
            {
                REND_MEMCPY16(pDestBufRight, pSrcBufRight, Fun_GetSizeByBPP(pMe, (pMe->wWidthSrc - Max)));
            }
            pDestBufLeft  += pMe->wWidthSrc;
            pSrcBufLeft   += pMe->wWidthSrc;
            pDestBufRight += pMe->wWidthSrc;
            pSrcBufRight  += pMe->wWidthSrc;
        }
    } 
}

// 左右交叉百叶进
static void Fun_REND_BLINDS_INTER_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == MaxStep)
    {
        REND_MEMCPY32(pMe->pDest, pMe->pSrc1, Fun_GetSizeByBPP(pMe, pMe->wWidthSrc*pMe->wHeightSrc));
    }
    else
    {
        int i;
        int Max = MATH_STEPPER(pMe->wWidthSrc,Step);
        uint16 *pDestBufLeft = (uint16 *)pMe->pDest;
        uint16 *pSrcBufLeft  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc-Max);
        uint16 *pDestBufRight = (uint16 *)pMe->pDest+(pMe->wWidthSrc-Max);
        uint16 *pSrcBufRight  = (uint16 *)pMe->pSrc1;

        for(i = 0; i < pMe->wHeightSrc; i++)
        {
            if((i/20)%2 == 0)
            {
                REND_MEMCPY16(pDestBufLeft, pSrcBufLeft, Fun_GetSizeByBPP(pMe, Max));
            }
            else
            {
                REND_MEMCPY16(pDestBufRight, pSrcBufRight, Fun_GetSizeByBPP(pMe, Max));
            }
            pDestBufLeft  += pMe->wWidthSrc;
            pSrcBufLeft   += pMe->wWidthSrc;
            pDestBufRight += pMe->wWidthSrc;
            pSrcBufRight  += pMe->wWidthSrc;
        }
    }
}

static const PFNRENDDRAW g_PFNRendDraw[REND_MAX] =
{
    Fun_REND_SHOW_HORZ_OUT,         //显示
    Fun_REND_SHOW_HORZ_IN,
    Fun_REND_SHOW_VERT_OUT,
    Fun_REND_SHOW_VERT_IN,
    Fun_REND_FADE_OUT,
    Fun_REND_FADE_IN,
    Fun_REND_MOVE_HORZ_OUT,
    Fun_REND_MOVE_HORZ_IN,
    Fun_REND_MOVE_VERT_OUT,
    Fun_REND_MOVE_VERT_IN,
    #if !defined(FEATURE_VERSION_W516) || !defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
    Fun_REND_FLY_HORZ_OUT,          //滑入
    Fun_REND_FLY_HORZ_IN,
    Fun_REND_FLY_VERT_OUT,
    Fun_REND_FLY_VERT_IN,
    #endif
    Fun_REND_ROTAT_HORZ_OUT,        //旋入
    Fun_REND_ROTAT_HORZ_IN,
    Fun_REND_ROTAT_VERT_OUT,
    Fun_REND_ROTAT_VERT_IN,
    Fun_REND_BLINDS_VERT_OUT,       //百叶窗入
    Fun_REND_BLINDS_VERT_IN,
    Fun_REND_BLINDS_INTER_OUT,
    Fun_REND_BLINDS_INTER_IN
};

PFNRENDDRAW Rendering_GetRendDraw16(RendType eType)
{
    return g_PFNRendDraw[eType];
}

