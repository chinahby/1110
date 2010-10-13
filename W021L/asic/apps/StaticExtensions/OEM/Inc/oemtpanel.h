#ifndef OEMTPANEL_H
#define OEMTPANEL_H
/*===========================================================================

              Touch Panel BREW OEM layer implement  H E A D E R  

DESCRIPTION
   这是为触摸屏坐标校准提供接口函数的头文件。本文件将定义坐标校准的接口和
   实现所需的常量等。
REFERENCES
  《线性误差参数模型理论Draft.doc》
  《模拟校准编程Draft.doc》
  《参数模型分析.xls》

                  Copyright (c) 2004 Azlun Xie
                 by T.M.Ltd. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.
  
when       who     what, where, why
--------   ---     -------------------------------------------------------
12/15/04   xzl     add functions declaration supporting CaliApp.
12/10/04   xzl     初始版本。

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "touchpanel.h"
#include "touchpanel_cali.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
Structure prototypes
-------------------------------------------------------------------*/


/*-------------------------------------------------------------------
Pubilc Data 
-------------------------------------------------------------------*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

//转换UI坐标为硬件坐标，以去除校准影响，协助校准
boolean OEMTPanel_CalculateUIToHW(MTMATRIX_PTR pUIPointBuf,
                                  MTMATRIX_PTR pHWPointBuf,
                                  CalibrateParam *pCParam
                                 );
//只去掉OEM层BREW坐标偏移
boolean OEMTPanel_CalculateUIToOEM(MTMATRIX_PTR pUIPointBuf,
                                   MTMATRIX_PTR pOEMPointBuf
                                  );

//计算校准参数
boolean OEMTPanel_CalCalibrateParam(MTMATRIX_PTR pLCDPointBuf,
                                    MTMATRIX_PTR pTSCPointBuf,
                                    CalibrateParam *pCParam
                                   );
//设置校准参数
boolean OEMTPanel_SetCalibrateParam(CalibrateParam *pszParam);
//设置校准参数
boolean OEMTPanel_SetCalibrateParamEx(CalibrateParam *pszParam);
   
//获得当前驱动使用的校准参数
boolean OEMTPanel_GetCalibrateParam(CalibrateParam *pdstParam);
//保存校准参数到NV
boolean OEMTPanel_StoreNVCalibrateParam(CalibrateParam *pszParam);
//从NV读取校准参数
boolean OEMTPanel_RestoreNVCalibrateParam(CalibrateParam *pdstParam);

//获得本次下笔的Driver坐标点记录
void OEMTPanel_CoorGetPendownPoint(matrix_data_type *px,matrix_data_type *py);

void OEMTPanel_CoorCalibration(int x_TSC,int y_TSC,int *ret_x,int *ret_y );

//reset the driver while the driver is hanged.
void OEMTPanel_ResetDriver(void);

#endif //OEMTPANEL_H
