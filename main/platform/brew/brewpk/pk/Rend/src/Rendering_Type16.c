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

#define BYTE_PERPIXEL   2
#define MATH_FACTOR     10000
#define MATH_FACTOR2    20000
static uint16 g_LightBuf[] = {0xFFFF,0xF7DE,0xEFDB,0xE79C,0};

static int16  g_SinCos_Tab[360] = {
0,174,348,523,697,871,1045,1218,1391,1564,1736,1908,2079,2249,2419,2588,2756,2923,3090,3255,3420,3583,3746,3907,4067,4226,4383,4539,4694,4848,
5000,5150,5299,5446,5591,5735,5877,6018,6156,6293,6427,6560,6691,6819,6946,7071,7193,7313,7431,7547,7660,7771,7880,7986,8090,8191,8290,8386,8480,8571,8660,8746,8829,8910,8987,9063,9135,9205,9271,9335,9396,9455,9510,9563,9612,9659,9702,9743,9781,9816,9848,9876,9902,9925,9945,9961,9975,9986,9993,9998,
10000,9998,9993,9986,9975,9961,9945,9925,9902,9876,9848,9816,9781,9743,9702,9659,9612,9563,9510,9455,9396,9335,9271,9205,9135,9063,8987,8910,8829,8746,8660,8571,8480,8386,8290,8191,8090,7986,7880,7771,7660,7547,7431,7313,7193,7071,6946,6819,6691,6560,6427,6293,6156,6018,5877,5735,5591,5446,5299,5150,
5000,4848,4694,4539,4383,4226,4067,3907,3746,3583,3420,3255,3090,2923,2756,2588,2419,2249,2079,1908,1736,1564,1391,1218,1045,871,697,523,348,174,
0,-174,-348,-523,-697,-871,-1045,-1218,-1391,-1564,-1736,-1908,-2079,-2249,-2419,-2588,-2756,-2923,-3090,-3255,-3420,-3583,-3746,-3907,-4067,-4226,-4383,-4539,-4694,-4848,
-5000,-5150,-5299,-5446,-5591,-5735,-5877,-6018,-6156,-6293,-6427,-6560,-6691,-6819,-6946,-7071,-7193,-7313,-7431,-7547,-7660,-7771,-7880,-7986,-8090,-8191,-8290,-8386,-8480,-8571,-8660,-8746,-8829,-8910,-8987,-9063,-9135,-9205,-9271,-9335,-9396,-9455,-9510,-9563,-9612,-9659,-9702,-9743,-9781,-9816,-9848,-9876,-9902,-9925,-9945,-9961,-9975,-9986,-9993,-9998,
-10000,-9998,-9993,-9986,-9975,-9961,-9945,-9925,-9902,-9876,-9848,-9816,-9781,-9743,-9702,-9659,-9612,-9563,-9510,-9455,-9396,-9335,-9271,-9205,-9135,-9063,-8987,-8910,-8829,-8746,-8660,-8571,-8480,-8386,-8290,-8191,-8090,-7986,-7880,-7771,-7660,-7547,-7431,-7313,-7193,-7071,-6946,-6819,-6691,-6560,-6427,-6293,-6156,-6018,-5877,-5735,-5591,-5446,-5299,-5150,
-5000,-4848,-4694,-4539,-4383,-4226,-4067,-3907,-3746,-3583,-3420,-3255,-3090,-2923,-2756,-2588,-2419,-2249,-2079,-1908,-1736,-1564,-1391,-1218,-1045,-871,-697,-523,-348,-174
};

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

static INLINE uint16 Fun_BMPInterpolate(uint16* lpbySrcXY, int16 x, int16 y,int16 nScanWidth, int16 nScanHeight, int16 nPitch)
{
	uint16 rgb16;
	//行字节数, 可以将dwWidthBytes作为参数传递过来
	uint16 PixelValue;
	//相邻的四个像素最右下角点的x, y坐标偏移量
	int nx = 1;
	int ny = 1;
	
	uint16 abyRed[2][2], abyGreen[2][2],abyBlue[2][2];
	
	//像素点(x, y)的数据位置
	uint16* pbySrc = lpbySrcXY;
	uint16* pbySrcTemp;
	
	if((x + 1) > (nScanWidth - 1)) nx = 0;
	if((y + 1) > (nScanHeight - 1)) ny = 0;
	
	//相邻四个像素的像素值
	//获取像素数值.
	
	//(x, y) = (x, y) + (0, 0)
	PixelValue = 0;
	PixelValue = *pbySrc++;
	abyRed[0][0] = (PixelValue & 0xF800);
	abyGreen[0][0] =(PixelValue & 0x07E0);
	abyBlue[0][0] = (PixelValue & 0x001F);
	
	//(x + 1, y) = (x, y) + (1, 0)
	pbySrc = (lpbySrcXY + nx);
	PixelValue = 0;
	PixelValue = *pbySrc++;
	abyRed[1][0] = (PixelValue & 0xF800);
	abyGreen[1][0] =(PixelValue & 0x07E0);
	abyBlue[1][0] = (PixelValue & 0x001F);


	
	//指向下一行数据
	pbySrcTemp = (lpbySrcXY + ny * nPitch);

	//(x , y + 1) = (x, y) + (0, 1)
	pbySrc = pbySrcTemp;
	PixelValue = 0;
	PixelValue = *pbySrc++;
	abyRed[0][1] = (PixelValue & 0xF800);
	abyGreen[0][1] =(PixelValue & 0x07E0);
	abyBlue[0][1] = (PixelValue & 0x001F);
	
	//(x + 1, y + 1) = (x, y) + (1, 1)
	pbySrc = pbySrcTemp + (nx);
	PixelValue = 0;
	PixelValue = *pbySrc++;
	abyRed[1][1] = (PixelValue & 0xF800);
	abyGreen[1][1] =(PixelValue & 0x07E0);
	abyBlue[1][1] = (PixelValue & 0x001F);
	
	rgb16 = 0;
	rgb16 = (uint16)(((((abyRed[0][0]+abyRed[0][1]+abyRed[1][0]+abyRed[1][1])/4)) )& 0xF800);
	rgb16 |= (uint16)(((((abyGreen[0][0]+abyGreen[0][1]+abyGreen[1][0]+abyGreen[1][1])/4)) )& 0x07E0);
	rgb16 |= (uint16)(((((abyBlue[0][0]+abyBlue[0][1]+abyBlue[1][0]+abyBlue[1][1])/4)) )& 0x001F);

	return rgb16;
}

static void Fun_BMPScale(uint16 *pBitsDst16, int16 wWidthDst, int16 wHeightDst, int16 nPitchDst,
						 uint16* pBitsSrc16, int16 wWidthSrc, int16 wHeightSrc, int16 nPitchSrc)
{
	//第一步, 进行参数合法性检测
	//宽度缩放比
	uint32 fScalex = MATH_FACTOR*wWidthSrc / wWidthDst;
	uint32 fScaley = MATH_FACTOR*wHeightSrc / wHeightDst;
    
	//指向目标数据
	uint16* pbyDst = pBitsDst16;
	uint16* pbySrc;
	uint16* pbyCurrent;
	int i,j;
    
	//完成变换
	for( i = 0; i < wHeightDst;i++)
	{
		//取整
		uint32 yy = (i * fScaley)/MATH_FACTOR;
        
		pbySrc = pBitsSrc16 + yy * nPitchSrc;
        
		for( j = 0;j < wWidthDst;j++)
		{
			//取整
			uint32 xx = (j * fScalex)/MATH_FACTOR;

			//获取数据
			pbyCurrent =  pbySrc + xx;
			*(pbyDst+j) = Fun_BMPInterpolate(pbyCurrent, (int16)xx, (int16)yy, wWidthSrc,  wHeightSrc, nPitchSrc);
		}
		pbyDst += nPitchDst;
	}
}

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
	uint16* pbySrc;
	uint16* pbyCurrent;
	int16 newDstWidth, newDstHeight,xOff,yOff,newxOff,newyOff;
	int32 x,y,xx,yx,xy,yy,xz,yz;
	int i,j;
	int16 newxAngle,newyAngle,COSNewX,COSNewY,SINNewX,SINNewY;
	int16 COSX,COSY,COSZ,SINX,SINY,SINZ;
	
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
	SINX      = SIN(xAngle);
	SINY      = SIN(yAngle);
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

	fScaleX = MATH_FACTOR*wWidthSrc /newDstWidth;
	fScaleY = MATH_FACTOR*wHeightSrc / newDstHeight;

	//完成变换
	for( i = 0; i < wHeightDst;i++)
	{
		// 缩放
		y = ((i+yOff) * fScaleY)/MATH_FACTOR;

		for( j = 0;j < wWidthDst;j++)
		{
			// 缩放
			x = ((j+xOff) * fScaleX)/MATH_FACTOR;
			// z轴旋转
			// 如果是逆时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   -   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   +   (x1   -   x0)   *   sina   +   y0   
            // 如果是顺时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   +   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   -   (x1   -   x0)   *   sina   +   y0
			xz = ((x-xCenter)*COSZ -(y-yCenter)*SINZ)/MATH_FACTOR + xCenter;
			yz = ((y-yCenter)*COSZ +(x-xCenter)*SINZ)/MATH_FACTOR + yCenter;
			
			// y轴旋转
			newDstHeight = wHeightSrc - ((int16)xz-xCenter)*SINNewY/COSNewY;
			newyOff = (newDstHeight - wHeightSrc)*yCenter/wHeightSrc;
			newfScaleY = MATH_FACTOR*wHeightSrc / newDstHeight;
			xy = (xz-xCenter)*MATH_FACTOR/COSY+xCenter;
			//yy = yz + (xz-xCenter)*SINNewY/COSNewY;
			yy = (yz+newyOff)*newfScaleY/MATH_FACTOR;
			
			// x轴旋转
			newDstWidth = wWidthSrc - ((int16)yy-yCenter)*SINNewX/COSNewX;
			newxOff = (newDstWidth - wWidthSrc)*xCenter/wWidthSrc;
			newfScaleX = MATH_FACTOR*wWidthSrc / newDstWidth;
			//xx = xy + (yy-yCenter)*SINNewX/COSNewX;
			xx = (xy+newxOff)*newfScaleX/MATH_FACTOR;
			yx = (yy-yCenter)*MATH_FACTOR/(COSX)+yCenter;
			
			//获取数据
			if(xx >= wWidthSrc || yx >= wHeightSrc || xx<0 || yx<0)
			{
				// Nothing todo
				//*(pbyDst+j) = REND_DEFAULT_CLR;
			}
			else
			{
				pbySrc = pBitsSrc16 + yx * nPitchSrc;
				pbyCurrent =  pbySrc + xx;
				*(pbyDst+j) = *pbyCurrent;
				//*(pbyDst+j) = Fun_BMPInterpolate(pbyCurrent, (int16)xx, (int16)yx, wWidthSrc, wHeightSrc, nPitchSrc);
			}
		}
		pbyDst += nPitchDst;
	}
}

// 带xyz旋转角度轴的变换和平移
static void Fun_BMPxyzScale3D(BmpType *pDst, BmpType *pSrc, BmpPos *pPos)
{
	//宽度缩放比
	int32 fScaleX,newfScaleX;
	int32 fScaleY,newfScaleY;

	//指向目标数据
	uint16* pbyDst = pDst->m_pBmp;
	uint16* pbySrc;
	uint16* pbyCurrent;
	int16 newDstWidth, newDstHeight,xOff,yOff,newxOff,newyOff;
	int32 x,y,xx,yx,xy,yy,xz,yz;
	int i,j;
	int16 newxAngle,newyAngle,COSNewX,COSNewY,SINNewX,SINNewY;
	int16 COSX,COSY,COSZ,SINX,SINY,SINZ;
	
	if(pDst->m_wWidth<= 0 || pDst->m_wHeight<=0 || pSrc->m_wWidth<= 0 || pSrc->m_wHeight<=0)
	{
		return;
	}

	newxAngle = Fun_GetxyOffAngle(pPos->m_xAngle, pPos->m_zxAngle);
	newyAngle = Fun_GetxyOffAngle(pPos->m_yAngle, pPos->m_zxAngle);
	COSNewX   = COS(newxAngle);
	COSNewY   = COS(newyAngle);
	SINNewX   = SIN(newxAngle);
	SINNewY   = SIN(newyAngle);
	COSX      = COS(pPos->m_xAngle);
	COSY      = COS(pPos->m_yAngle);
	COSZ      = COS(pPos->m_zAngle);
	SINX      = SIN(pPos->m_xAngle);
	SINY      = SIN(pPos->m_yAngle);
	SINZ      = SIN(pPos->m_zAngle);

	if(COSY == 0) COSY = 1;
	if(COSX == 0) COSX = 1;

	newDstWidth  = pDst->m_wWidth - pPos->m_z*pDst->m_wWidth/REND_Z_MAX;
	newDstHeight = pDst->m_wHeight - pPos->m_z*pDst->m_wHeight/REND_Z_MAX;
	xOff = (newDstWidth - pDst->m_wWidth)*pPos->m_xCenter/pSrc->m_wWidth-pPos->m_xMove;
	yOff = (newDstHeight - pDst->m_wHeight)*pPos->m_yCenter/pSrc->m_wHeight-pPos->m_yMove;
	
	if(newDstWidth<= 0 || newDstHeight<=0)
	{
		return;
	}

	fScaleX = MATH_FACTOR*pSrc->m_wWidth /newDstWidth;
	fScaleY = MATH_FACTOR*pSrc->m_wHeight / newDstHeight;

	//完成变换
	for( i = 0; i < pDst->m_wHeight; i++)
	{
		// 缩放
		y = ((i+yOff) * fScaleY)/MATH_FACTOR;

		for( j = 0;j < pDst->m_wWidth; j++)
		{
			// 缩放
			x = ((j+xOff) * fScaleX)/MATH_FACTOR;
			// z轴旋转
			// 如果是逆时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   -   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   +   (x1   -   x0)   *   sina   +   y0   
            // 如果是顺时针旋转：   
            // x2   =   (x1   -   x0)   *   cosa   +   (y1   -   y0)   *   sina   +   x0   
            // y2   =   (y1   -   y0)   *   cosa   -   (x1   -   x0)   *   sina   +   y0
			xz = ((x-pPos->m_xCenter)*COSZ -(y-pPos->m_yCenter)*SINZ)/MATH_FACTOR + pPos->m_xCenter;
			yz = ((y-pPos->m_yCenter)*COSZ +(x-pPos->m_xCenter)*SINZ)/MATH_FACTOR + pPos->m_yCenter;
			
			// y轴旋转
			newDstHeight = pSrc->m_wHeight - ((int16)xz-pPos->m_xCenter)*SINNewY/COSNewY;
			newyOff = (newDstHeight - pSrc->m_wHeight)*pPos->m_yCenter/pSrc->m_wHeight;
			newfScaleY = MATH_FACTOR*pSrc->m_wHeight / newDstHeight;
			xy = (xz-pPos->m_xCenter)*MATH_FACTOR/COSY+pPos->m_xCenter;
			//yy = yz + (xz-xCenter)*SINNewY/COSNewY;
			yy = (yz+newyOff)*newfScaleY/MATH_FACTOR;
			
			// x轴旋转
			newDstWidth = pSrc->m_wWidth - ((int16)yy-pPos->m_yCenter)*SINNewX/COSNewX;
			newxOff = (newDstWidth - pSrc->m_wWidth)*pPos->m_xCenter/pSrc->m_wWidth;
			newfScaleX = MATH_FACTOR*pSrc->m_wWidth / newDstWidth;
			//xx = xy + (yy-yCenter)*SINNewX/COSNewX;
			xx = (xy+newxOff)*newfScaleX/MATH_FACTOR;
			yx = (yy-pPos->m_yCenter)*MATH_FACTOR/(COSX)+pPos->m_yCenter;
			
			//获取数据
			if(xx >= pSrc->m_wWidth || yx >= pSrc->m_wHeight || xx<0 || yx<0)
			{
				// Nothing todo
				//*(pbyDst+j) = REND_DEFAULT_CLR;
			}
			else
			{
				pbySrc = (uint16 *)pSrc->m_pBmp + yx * pSrc->m_nPitch;
				pbyCurrent =  pbySrc + xx;
				*(pbyDst+j) = *pbyCurrent;
				//*(pbyDst+j) = Fun_BMPInterpolate(pbyCurrent, (int16)xx, (int16)yx, pSrc->m_wWidth, pSrc->m_wHeight, pSrc->m_nPitch);
			}
		}
		pbyDst += pDst->m_nPitch;
	}
}

static void Chousi(RendDraw *pMe,int Step, int MaxStep)
{
	int i,j;
	int x = 0;	
	int y = 0;
	int h = 0;
	int l = 0;
	int startx,starty;
	uint16 * temp = NULL;
	uint16 *pDestBuf = NULL;
	uint16 *pSrcBuf = NULL;
	int Width_X = pMe->wWidthSrc/MaxStep;//x轴每次要抽走的间隔
	int Width_X_Left = (pMe->wWidthSrc - Width_X*MaxStep)/2;//每次的x轴的起始地址
	int Width_Y = pMe->wHeightSrc/MaxStep;//y轴每次要抽走的间隔
	int Width_Y_Top = (pMe->wHeightSrc - Width_Y*MaxStep)/2;//每次的y轴的起始地址

	int nowx = Width_X_Left*2 + Step*Width_X;//当前缩小后的宽
	int nowy = Width_Y_Top*2 + Step*Width_Y;//当前缩小后的高

	temp = (uint16 *)REND_MALLOC( nowx * nowy * BYTE_PERPIXEL);

	if(!temp)
		return;
    REND_MEMSET(temp, REND_DEFAULT_CLR, nowx * nowy * BYTE_PERPIXEL);
	startx = (pMe->wWidthSrc - nowx)/2;
	starty = (pMe->wHeightSrc - nowy)/2;

	pDestBuf = (uint16 *)pMe->pDest + ((starty - 1)*pMe->wWidthSrc + startx);
	pSrcBuf  = (uint16 *)pMe->pSrc1;
    
	y = 0;
	//x轴
	for(i = 0; i < pMe->wHeightSrc; i++)
	{
		//if(i == (Width_Y_Top + y))//确定列中选中的行
		if((i >= (Width_Y_Top + y)) && (i < (Width_Y_Top + y + Step)))//确定列中选中的行
		{
			x = 0;
            h++;
			l = 0;
			l = l + Width_X_Left;
			REND_MEMCPY(temp, pSrcBuf, Width_X_Left*BYTE_PERPIXEL);//取左边多出的一段
			temp = (uint16 *)temp + Width_X_Left;
			pSrcBuf = (uint16 *)pSrcBuf + Width_X_Left;

		//y轴
			for(j = 0; j < pMe->wWidthSrc; j++)
			{
				if(j == (Width_X_Left + x))//确定行中选中的列点
				{
					if(j< pMe->wWidthSrc - MaxStep)
					{
						l = l + Step;
						REND_MEMCPY(temp, pSrcBuf, Step*BYTE_PERPIXEL);
						x += MaxStep;
						temp = (uint16 *)temp + Step;
						pSrcBuf = (uint16 *)pSrcBuf + MaxStep;
					}
					else
					{
						l = l + Width_X_Left;
						REND_MEMCPY(temp, pSrcBuf, Width_X_Left*BYTE_PERPIXEL);//取右边多出的一段
						temp = (uint16 *)temp + Width_X_Left;
						pSrcBuf = (uint16 *)pSrcBuf + Width_X_Left;
					}
				}
			}
			//y += MaxStep;
		}
		else
		{
			if(i >= Width_Y_Top + y + MaxStep - 1)
				y += MaxStep;
		}
		pSrcBuf  += pMe->wWidthSrc;
	}

	temp = (uint16 *)(temp - nowx * nowy);

	
	for(i = starty; i < starty + nowy + 1; i++)
	{
		REND_MEMCPY(pDestBuf, temp, nowx*BYTE_PERPIXEL);
		temp = (uint16 *)(temp + nowx);
		pDestBuf += pMe->wWidthSrc;
	}
	
	REND_FREE(temp);
}
static void Fun_REND_SHOW_LEFTRIGHT_OUT(RendDraw *pMe, int Step, int MaxStep)
{
	/*
	if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);

		w = pMe->wWidthSrc*Step/MaxStep;
		h = pMe->wHeightSrc*Step/MaxStep;
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;

		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);
		
		// 复制左边
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
		Fun_BMPScale(pDestBuf,(pMe->wWidthSrc-w)>>1,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc>>1,pMe->wHeightSrc,pMe->wWidthSrc);
		
		// 复制右边
		pDestBuf = (uint16 *)pMe->pDest+xStart+w;
		pSrcBuf  = (uint16 *)pMe->pSrc2+(pMe->wWidthSrc>>1);
		Fun_BMPScale(pDestBuf,(pMe->wWidthSrc-w)>>1,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc>>1,pMe->wHeightSrc,pMe->wWidthSrc);
	}
	*/
	if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
        Chousi(pMe,Step,MaxStep);
	}
}

static void Fun_REND_SHOW_LEFTRIGHT_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
		
		w = pMe->wWidthSrc-(pMe->wWidthSrc*Step/MaxStep);
		h = pMe->wHeightSrc-(pMe->wHeightSrc*Step/MaxStep);
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;
		
		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);
		
		// 复制左边
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
		Fun_BMPScale(pDestBuf,(pMe->wWidthSrc-w)>>1,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc>>1,pMe->wHeightSrc,pMe->wWidthSrc);
		
		// 复制右边
		pDestBuf = (uint16 *)pMe->pDest+xStart+w;
		pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc>>1);
		Fun_BMPScale(pDestBuf,(pMe->wWidthSrc-w)>>1,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc>>1,pMe->wHeightSrc,pMe->wWidthSrc);
	}
}

static void Fun_REND_SHOW_TOPBOTTOM_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);

		w = pMe->wWidthSrc*Step/MaxStep;
		h = pMe->wHeightSrc*Step/MaxStep;
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;

		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

		// 复制上边
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,(pMe->wHeightSrc-h)>>1,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc>>1,pMe->wWidthSrc);

		// 复制下边
		pDestBuf = (uint16 *)pMe->pDest+(yStart+h)*pMe->wWidthSrc;
		pSrcBuf  = (uint16 *)pMe->pSrc2+pMe->wWidthSrc*(pMe->wHeightSrc>>1);
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,(pMe->wHeightSrc-h)>>1,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc>>1,pMe->wWidthSrc);
	}
}

static void Fun_REND_SHOW_TOPBOTTOM_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);

		w = pMe->wWidthSrc-(pMe->wWidthSrc*Step/MaxStep);
		h = pMe->wHeightSrc-(pMe->wHeightSrc*Step/MaxStep);
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;

		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);

		// 复制上边
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,(pMe->wHeightSrc-h)>>1,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc>>1,pMe->wWidthSrc);

		// 复制下边
		pDestBuf = (uint16 *)pMe->pDest+(yStart+h)*pMe->wWidthSrc;
		pSrcBuf  = (uint16 *)pMe->pSrc1+pMe->wWidthSrc*(pMe->wHeightSrc>>1);
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,(pMe->wHeightSrc-h)>>1,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc>>1,pMe->wWidthSrc);
	}
}

static void Fun_REND_SHOW_CENTER_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		//REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		w = pMe->wWidthSrc - (pMe->wWidthSrc>>1)*(Step)/MaxStep;
		h = pMe->wHeightSrc - (pMe->wHeightSrc>>1)*(Step)/MaxStep;
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2+yStart*pMe->wWidthSrc+xStart;
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,w,h,pMe->wWidthSrc);

		w = pMe->wWidthSrc*Step/MaxStep;
		h = pMe->wHeightSrc*Step/MaxStep;
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;
		
		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);
	}
}

static void Fun_REND_SHOW_CENTER_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		//REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		w = (pMe->wWidthSrc>>1)+(pMe->wWidthSrc>>1)*(Step)/MaxStep;
		h = (pMe->wHeightSrc>>1)+(pMe->wHeightSrc>>1)*(Step)/MaxStep;
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1+yStart*pMe->wWidthSrc+xStart;
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,w,h,pMe->wWidthSrc);

		w = pMe->wWidthSrc-(pMe->wWidthSrc*Step/MaxStep);
		h = pMe->wHeightSrc-(pMe->wHeightSrc*Step/MaxStep);
		xStart = (pMe->wWidthSrc-w)>>1;
		yStart = (pMe->wHeightSrc-h)>>1;

		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPScale(pDestBuf,w,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);
	}
}

static void Fun_REND_MOVE_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
	// 翻书效果
    /*if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i,j;
		int Max = pMe->wWidthSrc-(pMe->wWidthSrc*g_CosI[(Step*10)/MaxStep])/10000;
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPScale(pDestBuf,pMe->wWidthSrc-Max,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc);
		pDestBuf = (uint16 *)pMe->pDest+pMe->wWidthSrc-Max;
		pSrcBuf  = (uint16 *)pMe->pSrc1+pMe->wWidthSrc-Max;
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, Max*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}*/
	if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		int i;

		w = pMe->wWidthSrc*Step/MaxStep;
		h = pMe->wHeightSrc;
		xStart = pMe->wWidthSrc-w;
		yStart = 0;

		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1+xStart;
	    
		for(i=0;i<h;i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
		

		pDestBuf = (uint16 *)pMe->pDest+w;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    w = pMe->wWidthSrc - w;
		for(i = 0; i < h; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}

static void Fun_REND_MOVE_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		int i;

		w = pMe->wWidthSrc*Step/MaxStep;
		h = pMe->wHeightSrc;
		xStart = pMe->wWidthSrc-w;
		yStart = 0;

		pDestBuf = (uint16 *)pMe->pDest+xStart;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		for(i=0;i<h;i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
		

		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2+w;
	    w = pMe->wWidthSrc - w;
		for(i = 0; i < h; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}

static void Fun_REND_MOVE_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		int i;

		w = pMe->wWidthSrc;
		h = pMe->wHeightSrc*Step/MaxStep;
		xStart = 0;
		yStart = pMe->wHeightSrc-h;

		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1+yStart*pMe->wWidthSrc;
	    
		for(i=0;i<h;i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
		

		pDestBuf = (uint16 *)pMe->pDest+h*pMe->wWidthSrc;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    h = pMe->wHeightSrc - h;
		for(i = 0; i < h; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}

static void Fun_REND_MOVE_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int16 xStart, yStart;
		int16 w, h;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		int i;

		w = pMe->wWidthSrc;
		h = pMe->wHeightSrc*Step/MaxStep;
		xStart = 0;
		yStart = pMe->wHeightSrc-h;

		pDestBuf = (uint16 *)pMe->pDest+yStart*pMe->wWidthSrc;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		for(i=0;i<h;i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2+h*pMe->wWidthSrc;
	    h = pMe->wHeightSrc - h;
		for(i = 0; i < h; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, w*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}

static void Fun_REND_FLY_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc-Max);
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, Max*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}

static void Fun_REND_FLY_LEFT_FLEX(RendDraw *pMe, int Step, int MaxStep)//wlh 20090406 add 
{
    if(Step == 0)
	{
		//REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc-Max);
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, Max*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}
static void Fun_REND_FLY_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc-Max);
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, Max*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}
static void Fun_REND_FLY_RIGHT_FLEX(RendDraw *pMe, int Step, int MaxStep)//wlh 20090406 add 
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc-Max);
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			REND_MEMCPY(pDestBuf, pSrcBuf, Max*BYTE_PERPIXEL);
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
}
static void Fun_REND_FLY_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int Max = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+ (pMe->wWidthSrc*(pMe->wHeightSrc - Max));
	    
		REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*Max*BYTE_PERPIXEL);
	}
}
static void Fun_REND_FLY_TOP_FLEX(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int Max = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc*(pMe->wHeightSrc - Max));
	    
		REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*Max*BYTE_PERPIXEL);
	}
}
static void Fun_REND_FLY_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int Max = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc*(pMe->wHeightSrc - Max));
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*Max*BYTE_PERPIXEL);
	}
}
static void Fun_REND_FLY_BOTTOM_FLEX(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int Max = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc*(pMe->wHeightSrc - Max));
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*Max*BYTE_PERPIXEL);
	}
}
static void Fun_REND_FLY_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLY_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLY_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLY_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FLYOUT_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTAT_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    //if(Step == 0)
	//{
	//	REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	//}
	//else
	
	//REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	{
		int16 Angle1 = 90*Step/MaxStep-90;
		int16 Angle2 = 90*Step/MaxStep;
		int16 w     = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPxyzScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle1,0,240,160,0,30);
		
		pDestBuf = (uint16 *)pMe->pDest+(w);
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc-w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle2,0,0,160,0,30);
	}
	/*
	 if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i,j;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf = (uint16 *)pMe->pDest + Max;
		uint16 *pSrcBuf  = (uint16 *)pMe->pDest;
	    
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			for(j = 0; j < pMe->wWidthSrc; j++)
			{
				if(j%10 != Step)
				REND_MEMCPY(pDestBuf, pSrcBuf, BYTE_PERPIXEL);
				pDestBuf += 1;
				pSrcBuf  += 1;
			}
			pDestBuf += Max;
		}
	}*/
}

static void Fun_REND_ROTAT_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    //REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	{
		int16 Angle1 = 90-90*Step/MaxStep;
		int16 Angle2 = -90*Step/MaxStep;
		int16 w     = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		pDestBuf = (uint16 *)pMe->pDest+(pMe->wWidthSrc-w);
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPxyzScale(pDestBuf,w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle1,0,0,160,0,30);
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc-w,pMe->wHeightSrc,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,0,Angle2,0,240,160,0,30);
	}
}

static void Fun_REND_ROTAT_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	{
		int16 Angle1 = 90*Step/MaxStep-90;
		int16 Angle2 = 90*Step/MaxStep;
		int16 h     = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle1,0,0,120,320,0,30);
		
		pDestBuf = (uint16 *)pMe->pDest+(h*pMe->wWidthSrc);
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc-h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle2,0,0,120,0,0,30);
	}
}

static void Fun_REND_ROTAT_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	{
		int16 Angle1 = 90-90*Step/MaxStep;
		int16 Angle2 = -90*Step/MaxStep;
		int16 h     = pMe->wHeightSrc*Step/MaxStep;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;
		
		pDestBuf = (uint16 *)pMe->pDest+(pMe->wHeightSrc-h)*pMe->wWidthSrc;
		pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle1,0,0,120,0,0,30);
		
		pDestBuf = (uint16 *)pMe->pDest;
		pSrcBuf  = (uint16 *)pMe->pSrc2;
	    
		Fun_BMPxyzScale(pDestBuf,pMe->wWidthSrc,pMe->wHeightSrc-h,pMe->wWidthSrc,pSrcBuf,pMe->wWidthSrc,pMe->wHeightSrc,pMe->wWidthSrc,Angle2,0,0,120,320,0,30);
	}
}

static void Fun_REND_ROTAT_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    REND_MEMSET32(pMe->pDest, REND_DEFAULT_CLR, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	{
		int16 Angle1 = 90*Step/MaxStep-90;
		int16 Angle2 = 90*Step/MaxStep;
		int16 h     = pMe->wHeightSrc*Step/MaxStep;
		BmpType myDst;
		BmpType mySrc;
		BmpPos  myPos;

		myDst.m_pBmp    = pMe->pDest;
		myDst.m_nPitch	= pMe->wWidthSrc;
		myDst.m_wWidth  = pMe->wWidthSrc;
		myDst.m_wHeight = pMe->wHeightSrc;

		mySrc.m_pBmp    = pMe->pSrc1;
		mySrc.m_nPitch	= pMe->wWidthSrc;
		mySrc.m_wWidth  = pMe->wWidthSrc;
		mySrc.m_wHeight = pMe->wHeightSrc;

		myPos.m_xAngle  = 0;
		myPos.m_yAngle  = -60;
		myPos.m_zAngle  = -45;
		myPos.m_xCenter = 120;
		myPos.m_yCenter = 0;
		myPos.m_z       = 200;
		myPos.m_zxAngle = 30;
		myPos.m_xMove   = 20;
		myPos.m_yMove   = 20;
	    
		Fun_BMPxyzScale3D(&myDst, &mySrc, &myPos);
		
		mySrc.m_pBmp    = pMe->pSrc2;
		mySrc.m_nPitch	= pMe->wWidthSrc;
		mySrc.m_wWidth  = pMe->wWidthSrc;
		mySrc.m_wHeight = pMe->wHeightSrc;

		myPos.m_xAngle  = 0;
		myPos.m_yAngle  = 30;
		myPos.m_zAngle  = -45;
		myPos.m_xCenter = 120;
		myPos.m_yCenter = 0;
		myPos.m_z       = 200;
		myPos.m_zxAngle = 30;
		myPos.m_xMove   = 79;
		myPos.m_yMove   = 79;
		Fun_BMPxyzScale3D(&myDst, &mySrc, &myPos);
	}
}

static void Fun_REND_ROTAT_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTAT_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTAT_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROTATOUT_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
//wlh 20090406 add start
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i,j;
	
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    	
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			for(j = 0;j < 8;j++)
			{
				if(Step != 9)
					REND_MEMCPY(pDestBuf + (j*22), pSrcBuf + (j*22), Step*2*BYTE_PERPIXEL);
				else
					REND_MEMCPY(pDestBuf + (j*22), pSrcBuf + (j*22), 22*BYTE_PERPIXEL);
			}
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
//wlh 20090406 add
}

static void Fun_REND_BLINDS_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
//wlh 20090406 add start
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i,j;
	
		uint16 *pDestBuf = (uint16 *)pMe->pDest;
		uint16 *pSrcBuf  = (uint16 *)pMe->pSrc1;
	    	
		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			for(j = 0;j < 8;j++)
			{
				if(Step != 9)
					REND_MEMCPY(pDestBuf + (j*22) + (22 - Step*2), pSrcBuf + (j*22) + (22 - Step*2), Step*2*BYTE_PERPIXEL);
				else
					REND_MEMCPY(pDestBuf + (j*22), pSrcBuf + (j*22), 22*BYTE_PERPIXEL);
			}
			pDestBuf += pMe->wWidthSrc;
			pSrcBuf  += pMe->wWidthSrc;
		}
	}
//wlh 20090406 add    
}
//wlh 20090422 add 左右交叉百叶进
static void Fun_REND_BLINDS_IN(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBufLeft = (uint16 *)pMe->pDest;
		uint16 *pSrcBufLeft  = (uint16 *)pMe->pSrc1+(pMe->wWidthSrc-Max);
	    uint16 *pDestBufRight = (uint16 *)pMe->pDest+(pMe->wWidthSrc-Max);
		uint16 *pSrcBufRight  = (uint16 *)pMe->pSrc1;

		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			if((i/20)%2 == 0)
			{
				REND_MEMCPY(pDestBufLeft, pSrcBufLeft, Max*BYTE_PERPIXEL);
			}
			else
			{
				REND_MEMCPY(pDestBufRight, pSrcBufRight, Max*BYTE_PERPIXEL);
			}
			pDestBufLeft += pMe->wWidthSrc;
			pSrcBufLeft  += pMe->wWidthSrc;
			pDestBufRight += pMe->wWidthSrc;
			pSrcBufRight  += pMe->wWidthSrc;
		}
	} 
}
//wlh 20090422 add 左右交叉百叶出
static void Fun_REND_BLINDS_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc1, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max = pMe->wWidthSrc*Step/MaxStep;
		uint16 *pDestBufLeft = (uint16 *)pMe->pDest;
		uint16 *pSrcBufLeft  = (uint16 *)pMe->pSrc2 + Max;
	    uint16 *pDestBufRight = (uint16 *)pMe->pDest + Max;
		uint16 *pSrcBufRight  = (uint16 *)pMe->pSrc2;

		REND_MEMCPY(pMe->pDest, pMe->pSrc1, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);

		for(i = 0; i < pMe->wHeightSrc; i++)
		{
			if((i/20)%2 == 0)
			{
				REND_MEMCPY(pDestBufLeft, pSrcBufLeft, (pMe->wWidthSrc - Max)*BYTE_PERPIXEL);
			}
			else
			{
				REND_MEMCPY(pDestBufRight, pSrcBufRight, (pMe->wWidthSrc - Max)*BYTE_PERPIXEL);
			}
			pDestBufLeft += pMe->wWidthSrc;
			pSrcBufLeft  += pMe->wWidthSrc;
			pDestBufRight += pMe->wWidthSrc;
			pSrcBufRight  += pMe->wWidthSrc;
		}
	} 
}
static void Fun_REND_BLINDS_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDS_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_BLINDSOUT_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_LEFT(RendDraw *pMe, int Step, int MaxStep)
{

}

static void Fun_REND_SCROLL_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLL_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_SCROLLOUT_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_LEFT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_RIGHT(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_LEFTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_LEFTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_RIGHTTOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_ROLL_RIGHTBOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FIRE_TOP(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FIRE_BOTTOM(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FADE_IN(RendDraw *pMe, int Step, int MaxStep)
{
    
}

static void Fun_REND_FADE_OUT(RendDraw *pMe, int Step, int MaxStep)
{
    
}


static const PFNRENDDRAW g_PFNRendDraw[REND_MAX] =
{
    NULL,
    /*Fun_REND_SHOW_LEFTRIGHT_OUT,         //显示
	Fun_REND_SHOW_LEFTRIGHT_IN,
    Fun_REND_SHOW_TOPBOTTOM_OUT,
    Fun_REND_SHOW_TOPBOTTOM_IN,
	Fun_REND_SHOW_CENTER_OUT,
    Fun_REND_SHOW_CENTER_IN,*/
    Fun_REND_MOVE_LEFT,
    Fun_REND_MOVE_RIGHT,
   // Fun_REND_MOVE_TOP,
   // Fun_REND_MOVE_BOTTOM,
    //Fun_REND_FLY_LEFT,          //滑入
	//Fun_REND_FLY_LEFT_FLEX,     //带弹性左滑入//wlh 20090406 add
    //Fun_REND_FLY_RIGHT,
	//Fun_REND_FLY_RIGHT_FLEX,    //带弹性右滑入//wlh 20090406 add
    Fun_REND_FLY_TOP,
    Fun_REND_FLY_TOP_FLEX,     //带弹性上滑入//wlh 20090406 add
    Fun_REND_FLY_BOTTOM,
   // Fun_REND_FLY_BOTTOM_FLEX,     //带弹性下滑入//wlh 20090406 add
  /*   Fun_REND_FLY_LEFTTOP,
    Fun_REND_FLY_LEFTBOTTOM,
    Fun_REND_FLY_RIGHTTOP,
    Fun_REND_FLY_RIGHTBOTTOM,
    Fun_REND_FLYOUT_LEFT,       //滑出
    Fun_REND_FLYOUT_RIGHT,
    Fun_REND_FLYOUT_TOP,
    Fun_REND_FLYOUT_BOTTOM,
    Fun_REND_FLYOUT_LEFTTOP,
    Fun_REND_FLYOUT_LEFTBOTTOM,
    Fun_REND_FLYOUT_RIGHTTOP,
    Fun_REND_FLYOUT_RIGHTBOTTOM,*/
    //Fun_REND_ROTAT_LEFT,        //旋入
   // Fun_REND_ROTAT_RIGHT,
   /* Fun_REND_ROTAT_TOP,
    Fun_REND_ROTAT_BOTTOM,
    Fun_REND_ROTAT_LEFTTOP,
    Fun_REND_ROTAT_LEFTBOTTOM,
    Fun_REND_ROTAT_RIGHTTOP,
    Fun_REND_ROTAT_RIGHTBOTTOM,
    Fun_REND_ROTATOUT_LEFT,     //旋出
    Fun_REND_ROTATOUT_RIGHT,
    Fun_REND_ROTATOUT_TOP,
    Fun_REND_ROTATOUT_BOTTOM,
    Fun_REND_ROTATOUT_LEFTTOP,
    Fun_REND_ROTATOUT_LEFTBOTTOM,
    Fun_REND_ROTATOUT_RIGHTTOP,
    Fun_REND_ROTATOUT_RIGHTBOTTOM,*/
    Fun_REND_BLINDS_LEFT,       //百叶窗入
    Fun_REND_BLINDS_RIGHT,
	Fun_REND_BLINDS_IN,
	Fun_REND_BLINDS_OUT,/*
    Fun_REND_BLINDS_TOP,
    Fun_REND_BLINDS_BOTTOM,
    Fun_REND_BLINDS_LEFTTOP,
    Fun_REND_BLINDS_LEFTBOTTOM,
    Fun_REND_BLINDS_RIGHTTOP,
    Fun_REND_BLINDS_RIGHTBOTTOM,
    Fun_REND_BLINDSOUT_LEFT,    //百叶窗出
    Fun_REND_BLINDSOUT_RIGHT,
    Fun_REND_BLINDSOUT_TOP,
    Fun_REND_BLINDSOUT_BOTTOM,
    Fun_REND_BLINDSOUT_LEFTTOP,
    Fun_REND_BLINDSOUT_LEFTBOTTOM,
    Fun_REND_BLINDSOUT_RIGHTTOP,
    Fun_REND_BLINDSOUT_RIGHTBOTTOM,
    Fun_REND_SCROLL_LEFT,       //卷入
    Fun_REND_SCROLL_RIGHT,
    Fun_REND_SCROLL_TOP,
    Fun_REND_SCROLL_BOTTOM,
    Fun_REND_SCROLL_LEFTTOP,
    Fun_REND_SCROLL_LEFTBOTTOM,
    Fun_REND_SCROLL_RIGHTTOP,
    Fun_REND_SCROLL_RIGHTBOTTOM,
    Fun_REND_SCROLLOUT_LEFT,    //卷出
    Fun_REND_SCROLLOUT_RIGHT,
    Fun_REND_SCROLLOUT_TOP,
    Fun_REND_SCROLLOUT_BOTTOM,
    Fun_REND_SCROLLOUT_LEFTTOP,
    Fun_REND_SCROLLOUT_LEFTBOTTOM,
    Fun_REND_SCROLLOUT_RIGHTTOP,
    Fun_REND_SCROLLOUT_RIGHTBOTTOM,
    Fun_REND_ROLL_LEFT,         //立体滚动
    Fun_REND_ROLL_RIGHT,
    Fun_REND_ROLL_TOP,
    Fun_REND_ROLL_BOTTOM,
    Fun_REND_ROLL_LEFTTOP,
    Fun_REND_ROLL_LEFTBOTTOM,
    Fun_REND_ROLL_RIGHTTOP,
    Fun_REND_ROLL_RIGHTBOTTOM,
    Fun_REND_FIRE_TOP,          //火焰
    Fun_REND_FIRE_BOTTOM,
    Fun_REND_FADE_IN,           //渐变
    Fun_REND_FADE_OUT,*/
};

PFNRENDDRAW Rendering_GetRendDraw16(RendType eType)
{
    return g_PFNRendDraw[eType];
}
//wlh 20090406 add start
static void Fun_REND_Flex(RendDraw *pMe, int Step, int MaxStep)//弹性碰撞处理函数
{
     if(Step == 0)
	{
		REND_MEMCPY(pMe->pDest, pMe->pSrc2, pMe->wWidthSrc*pMe->wHeightSrc*BYTE_PERPIXEL);
	}
	else
	{
		int i;
		int Max;
		uint16 *pDestBuf;
		uint16 *pSrcBuf;

		switch(Step)
		{
			case 1:
				Max = 9;
				break;
			case 2:
				Max = 18;
				break;
			case 3:
				Max = 9;
				break;
			case 4:
				Max = 0;
				break;
			case 5:
				Max = 12;
				break;
			case 6:
				Max = 6;
				break;
			case 7:
				Max = 0;
				break;
			case 8:
				Max = 6;
				break;
			case 9:
				Max = 0;
				break;
			default:
				break;
		}

		if(pMe->iFlexDirection == DIRECTION_LEFT)
		{
			pDestBuf = (uint16 *)pMe->pDest;
			pSrcBuf  = (uint16 *)pMe->pSrc1 + Max;
	    
			for(i = 0; i < pMe->wHeightSrc; i++)
			{
				REND_MEMCPY(pDestBuf, pSrcBuf, (pMe->wWidthSrc - Max)*BYTE_PERPIXEL);
				pDestBuf += pMe->wWidthSrc;
				pSrcBuf  += pMe->wWidthSrc;
			}
		}
		else if(pMe->iFlexDirection == DIRECTION_RIGHT)
		{
			pDestBuf = (uint16 *)pMe->pDest  + Max ;
			pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
			for(i = 0; i < pMe->wHeightSrc; i++)
			{
				REND_MEMCPY(pDestBuf, pSrcBuf, (pMe->wWidthSrc - Max)*BYTE_PERPIXEL);
				pDestBuf += pMe->wWidthSrc;
				pSrcBuf  += pMe->wWidthSrc;
			}
		}
		else if(pMe->iFlexDirection == DIRECTION_TOP)
		{
			pDestBuf = (uint16 *)pMe->pDest ;
			pSrcBuf  = (uint16 *)pMe->pSrc1 + pMe->wWidthSrc*Max;
	    
			REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*(pMe->wHeightSrc - Max)*BYTE_PERPIXEL);
		}
		else if(pMe->iFlexDirection == DIRECTION_BOTTOM)
		{
			pDestBuf = (uint16 *)pMe->pDest  + pMe->wWidthSrc*Max ;
			pSrcBuf  = (uint16 *)pMe->pSrc1;
	    
			REND_MEMCPY(pDestBuf, pSrcBuf, pMe->wWidthSrc*(pMe->wHeightSrc - Max)*BYTE_PERPIXEL);
		}
	}
}
PFNRENDDRAW Rendering_GetRendDraw16Flex(void)//wlh 20090406 add for Flexibility
{
    return Fun_REND_Flex;
}

int Rendering_GetRend16FlexDirect(RendType eType)//弹性碰撞方向
{
    switch(eType)
	{
		//case REND_FLY_LEFT_FLEX:
		//	return DIRECTION_LEFT;
		//case REND_FLY_RIGHT_FLEX:
		//	return DIRECTION_RIGHT;
		case REND_FLY_TOP_FLEX:
			return DIRECTION_TOP;
		//case REND_FLY_BOTTOM_FLEX:
		//	return DIRECTION_BOTTOM;
		default:
			return DIRECTION_NONE;
	}
}
//wlh 20090406 add end