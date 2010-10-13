/* 
** #pragma subtitle("Touch Panel Driver ")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Touch Panel BREW OEM layer implement  R O U T I N E S

GENERAL DESCRIPTION
   本文件实现触屏坐标校准的接口函数.

EXTERNALIZED FUNCTIONS

REFERENCES
  《线性误差参数模型理论Draft.doc》
  《模拟校准编程Draft.doc》
  《参数模型分析.xls》

         INITIALIZATION AND SEQUENCING REQUIREMENTS
  
              Copyright (c) 2004 Azlun Xie
             by T.M.Ltd. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.
   
when       who     what, where, why
--------   ---     -------------------------------------------------------
04/08/05   xzl     add OEMTPanel_ResetDriver() for UI reset the hanged driver. 
12/15/04   xzl     add functions to support CaliApp to read pen down point
                   from the driver and implement the real calculating 
                   and using of calibration paramters.
12/08/04   xzl     初始版本。
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_TOUCHPANEL

#include "OEMTPanel.h"

#include "disp.h" //for BREW_OFFSET_X/Y
//#include "disp_options.h" //for screen width
#include "err.h"
/* <EJECT> */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX(OS) Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Pubilc Data                                  */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

//模拟NV，用于校准触摸屏的误差 
int PointCounter=0; //假设我们出厂的触摸屏都有一个通用校准初值，无需立刻校准

//校准参数，有初值，无需立刻校准
CalibrateParam NV_CalibrateParam=
{
    0x00010000, 0,
    0,          0x00010000,
    0,          0
};

//三个LCD参考校准坐标点。
matrix_data_type LCDPoints[3][3]=
{
    REFPOINT1X, REFPOINT1Y,1,
    REFPOINT2X, REFPOINT2Y,1,
    REFPOINT3X, REFPOINT3Y,1
};

//三个TSC实际校准坐标点，运行校准后会被写入新数据。
matrix_data_type TSCPoints[3][3]=
{
    REFPOINT1X, REFPOINT1Y,1,
    REFPOINT2X, REFPOINT2Y,1,
    REFPOINT3X, REFPOINT3Y,1
};

//校准参数，有初值，无需立刻校准
CalibrateParam OEMPreCaliParam=
{
    PRESET_Ex_KXX,PRESET_Ex_KXY,        
    PRESET_Ex_KYX,PRESET_Ex_KYY,
    PRESET_Ex_M,  PRESET_Ex_N
};
/* <EJECT> */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         Public Functions                                 */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

boolean CaliTest_HandleEvent(hs_pen_type hCode,int CoodX,int CoodY)
{  
    switch (hCode)
    {
    //响应笔事件，显示笔事件信息
    case HS_PEN_DOWN_K:
    case HS_PEN_MOVE_K:
        if(hCode==HS_PEN_DOWN_K)
        {
            STD_OUTPUT("HS_PEN_D.:0x%x,(%d,%d)",
                       hCode,CoodX,CoodY);
        }
        else
        {
            STD_OUTPUT("HS_PEN_M.:0x%x,(%d,%d)",
                       hCode, CoodX,CoodY);
        }

        //tsc_CoorMakeWarp(pMe->m_pIShell,&dwParam);//人为制造误差

        if(hCode==HS_PEN_DOWN_K
           &&PointCounter<3)
        {
            
            TSCPoints[PointCounter][0]=(matrix_data_type)CoodX;
            TSCPoints[PointCounter][1]=(matrix_data_type)CoodY;
            TSCPoints[PointCounter][2]=1;

            if(PointCounter<=2)
            {
                 STD_OUTPUT("PC%d,(%d,%d)",
                             PointCounter,
                             TSCPoints[PointCounter][0],
                             TSCPoints[PointCounter][1]);
            }//end if(PointCounter<2)

            if(PointCounter==2)
            {
                tsc_CalculateCalibrateParam(
                    LCDPoints,
                    TSCPoints,
                    &NV_CalibrateParam);//校准误差
#if 0                    
                STD_OUTPUT("NV_CP         ",0,0,0);
                STD_OUTPUT("0x%8x,0x%8x,%d",NV_CalibrateParam.kxx,NV_CalibrateParam.kxy,0);
                STD_OUTPUT("0x%8x,0x%8x,%d",NV_CalibrateParam.kyx,NV_CalibrateParam.kyy,0);
                STD_OUTPUT("  %8d,  %8d,%d",NV_CalibrateParam.m,NV_CalibrateParam.n,1);
                STD_OUTPUT("              ",0,0,0);
                STD_OUTPUT("PointCounter%d",PointCounter,0,0);
        
                tsc_SetCalibrateParam(&NV_CalibrateParam,&gPreExCaliParam);
                tsc_RestoreNVCalibrateParam(&NV_CalibrateParam);
#endif//0
            }
            PointCounter++;
        }//end if(ecode==EVT_PEN_UP  &&PointCounter<3)
        return (TRUE);
    
    case HS_PEN_UP_K:
        if(PointCounter>=3)
        {
           PointCounter=0;
        }
        STD_OUTPUT("HS_PEN_U.:0x%x,(%d,%d)", hCode, CoodX,CoodY);
        return (TRUE);
   }//end switch
   return FALSE;
}
#if 0
/*-----------------------------------------------------------------------------
//使用参数，去掉校准影响的还原校准前的坐标值
-----------------------------------------------------------------------------*/
boolean OEMTPanel_CalculateOEMToHW(MTMATRIX_PTR pOEMPointBuf,
                             MTMATRIX_PTR pHWPointBuf,
                             CalibrateParam *pCParam
                            )
{
    //利用该逆，求校准前的坐标
    return TRUE;
}
#endif

/*-----------------------------------------------------------------------------
//使用参数，先去掉OEM层BREW坐标偏移，再去掉校准影响，的还原校准前的坐标值
-----------------------------------------------------------------------------*/
boolean OEMTPanel_CalculateUIToHW(MTMATRIX_PTR pUIPointBuf,
                                  MTMATRIX_PTR pHWPointBuf,
                                  CalibrateParam *pCParam
                                 )
{
    MTMATRIX TransMatrixReciprocal;
    MTMATRIX TransMatrix;//       [MT_MAX_SIZE][MT_MAX_SIZE];

    TransMatrix[0][0] = pCParam->kxx;
    TransMatrix[1][0] = pCParam->kxy;
    TransMatrix[2][0] = pCParam->m ;

    TransMatrix[0][1] = pCParam->kyx;
    TransMatrix[1][1] = pCParam->kyy;
    TransMatrix[2][1] = pCParam->n ;

    TransMatrix[0][2] = 0;
    TransMatrix[1][2] = 0;
    TransMatrix[2][2] = 1;
        
    //1.求现用校准矩阵的逆
    /*求3x3 方阵的逆，已放大65536倍*/
    Matrix_Reciprocal(TransMatrix,TransMatrixReciprocal);
#if 1
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("TransMatrix R   ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",TransMatrixReciprocal[0][0],TransMatrixReciprocal[0][1],TransMatrixReciprocal[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",TransMatrixReciprocal[1][0],TransMatrixReciprocal[1][1],TransMatrixReciprocal[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",TransMatrixReciprocal[2][0],TransMatrixReciprocal[2][1],TransMatrixReciprocal[2][2]);
    STD_OUTPUT("                ",0,0,0);
#endif//0
    

    //2.将BREW坐标偏移值混入矩阵中，以抵消该值
    TransMatrixReciprocal[2][0]-=BREW_OFFSET_X;
    TransMatrixReciprocal[2][1]-=BREW_OFFSET_Y;
        
    //3.利用该逆，求校准前的坐标
    /*求3x3 方阵的乘积，保留放大65536倍*/
    Matrix_Multiply  (pUIPointBuf,          MT_MAX_SIZE,MT_MAX_SIZE,
                      TransMatrixReciprocal,MT_MAX_SIZE,MT_MAX_SIZE,
                      pHWPointBuf); 
#if 1
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("pUIPointBuf R   ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[0][0],pUIPointBuf[0][1],pUIPointBuf[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[1][0],pUIPointBuf[1][1],pUIPointBuf[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[2][0],pUIPointBuf[2][1],pUIPointBuf[2][2]);
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("pHWPointBuf R   ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pHWPointBuf[0][0],pHWPointBuf[0][1],pHWPointBuf[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pHWPointBuf[1][0],pHWPointBuf[1][1],pHWPointBuf[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pHWPointBuf[2][0],pHWPointBuf[2][1],pHWPointBuf[2][2]);
    STD_OUTPUT("                ",0,0,0);
#endif//0
   
    return TRUE;
}

/*-----------------------------------------------------------------------------
//只去掉OEM层BREW坐标偏移
-----------------------------------------------------------------------------*/
boolean OEMTPanel_CalculateUIToOEM(MTMATRIX_PTR pUIPointBuf,
                                   MTMATRIX_PTR pOEMPointBuf
                                  )
{
    int i;
    for(i=0;i<MT_MAX_SIZE;i++)
    {
        pOEMPointBuf[i][0]=pUIPointBuf[i][0]+BREW_OFFSET_X;
        pOEMPointBuf[i][1]=pUIPointBuf[i][1]+BREW_OFFSET_Y;
        pOEMPointBuf[i][2]=pUIPointBuf[i][2];
    }
#if 1
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("pUIPointBuf     ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[0][0],pUIPointBuf[0][1],pUIPointBuf[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[1][0],pUIPointBuf[1][1],pUIPointBuf[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pUIPointBuf[2][0],pUIPointBuf[2][1],pUIPointBuf[2][2]);
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("pOEMPointBuf    ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pOEMPointBuf[0][0],pOEMPointBuf[0][1],pOEMPointBuf[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pOEMPointBuf[1][0],pOEMPointBuf[1][1],pOEMPointBuf[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pOEMPointBuf[2][0],pOEMPointBuf[2][1],pOEMPointBuf[2][2]);
    STD_OUTPUT("                ",0,0,0);
#endif//0
   
    return TRUE;
}

/*-----------------------------------------------------------------------------
//计算校准参数
-----------------------------------------------------------------------------*/
boolean OEMTPanel_CalCalibrateParam(MTMATRIX_PTR pLCDPointBuf,
                                    MTMATRIX_PTR pTSCPointBuf,
                                    CalibrateParam *pCParam
                                   )
{
#if 1
    STD_OUTPUT("                ",0,0,0);
    STD_OUTPUT("pTSCPointBuf    ",0,0,0);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pTSCPointBuf[0][0],pTSCPointBuf[0][1],pTSCPointBuf[0][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pTSCPointBuf[1][0],pTSCPointBuf[1][1],pTSCPointBuf[1][2]);
    STD_OUTPUT("0x%8x,0x%8x,0x%x",pTSCPointBuf[2][0],pTSCPointBuf[2][1],pTSCPointBuf[2][2]);
    STD_OUTPUT("                ",0,0,0);
#endif//0
    return tsc_CalculateCalibrateParam(pLCDPointBuf,pTSCPointBuf,pCParam);
}

/*-----------------------------------------------------------------------------
//设定校准参数
-----------------------------------------------------------------------------*/
boolean OEMTPanel_SetCalibrateParam(CalibrateParam *pszParam)
{
    return tsc_SetCalibrateParam(pszParam,&gCalibrateParam);
}

/*-----------------------------------------------------------------------------
//设定校准参数
-----------------------------------------------------------------------------*/
boolean OEMTPanel_SetCalibrateParamEx(CalibrateParam *pszParam)
{
    return tsc_SetCalibrateParam(pszParam,&gPreExCaliParam);
}

/*-----------------------------------------------------------------------------
//获得校准参数
-----------------------------------------------------------------------------*/
boolean OEMTPanel_GetCalibrateParam(CalibrateParam *pdstParam)
{
    return tsc_GetCalibrateParam(pdstParam);
}

/*-----------------------------------------------------------------------------
//存储校准参数到NV
-----------------------------------------------------------------------------*/
boolean OEMTPanel_StoreNVCalibrateParam(CalibrateParam *pszParam)
{
#if 1
    ERR("                ",0,0,0);
    ERR("StoreNV-Sim     ",0,0,0);
    ERR("0x%8x,0x%8x,0x%x",pszParam->kxx,pszParam->kyx,0);
    ERR("0x%8x,0x%8x,0x%x",pszParam->kxy,pszParam->kyy,0);
    ERR("0x%8x,0x%8x,0x%x",pszParam->m,  pszParam->n,1);
    ERR("                ",0,0,0);
#endif//0
return(TRUE);
}
/*-----------------------------------------------------------------------------
//从NV读取校准参数
-----------------------------------------------------------------------------*/
boolean OEMTPanel_RestoreNVCalibrateParam(CalibrateParam *pdstParam)
{
#ifdef USE_PRESET_CALIBRATEPARAM
#ifdef FEATURE_TSC_EXTEND_CALIBRATION
    pdstParam->kxx = PRESET_Ex_KXX;
    pdstParam->kxy = PRESET_Ex_KXY;
    pdstParam->kyx = PRESET_Ex_KYX;
    pdstParam->kyy = PRESET_Ex_KYY;
    pdstParam->m   = PRESET_Ex_M;
    pdstParam->n   = PRESET_Ex_N;
    return(TRUE);
#else//!FEATURE_TSC_EXTEND_CALIBRATION
    pdstParam->kxx = PRESET_KXX;
    pdstParam->kxy = PRESET_KXY;
    pdstParam->kyx = PRESET_KYX;
    pdstParam->kyy = PRESET_KYY;
    pdstParam->m   = PRESET_M;
    pdstParam->n   = PRESET_N;
    return(TRUE);
#endif//!FEATURE_TSC_EXTEND_CALIBRATION
#else//!USE_PRESET_CALIBRATEPARAM
    pdstParam->kxx = PRESET_KXX;
    pdstParam->kxy = PRESET_KXY;
    pdstParam->kyx = PRESET_KYX;
    pdstParam->kyy = PRESET_KYY;
    pdstParam->m   = PRESET_M;
    pdstParam->n   = PRESET_N;
    return(TRUE);
#endif//!USE_PRESET_CALIBRATEPARAM
}

//获得本次下笔的坐标点记录
void OEMTPanel_CoorGetPendownPoint(matrix_data_type *px,matrix_data_type *py)
{
    int i32_x;
    int i32_y;

    //use temp variables to read coordate for the driver
    tsc_CoorGetPendownPoint(&i32_x,&i32_y);
    //to fit the matirx calibration, force the data to the appropriate data type
    //now is 64 bit data
    *px = (matrix_data_type)i32_x;
    *py = (matrix_data_type)i32_y;
    //*px += BREW_OFFSET_X;
    //*py += BREW_OFFSET_Y;
}

void OEMTPanel_CoorCalibration(int x_TSC,int y_TSC,int *ret_x,int *ret_y )
{
    
    tsc_CoorCalibration ( x_TSC, y_TSC, ret_x, ret_y, &OEMPreCaliParam );
    
}

//reset the driver while the driver is hanged.
void OEMTPanel_ResetDriver()
{
    //prompt message
    ERR("UI reset touch panel driver",0,0,0);

    //reset it
    touchpanel_init();
}

/* <EJECT> */
#endif //FEATURE_TOUCHPANEL
