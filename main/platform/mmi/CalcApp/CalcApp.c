/*=============================================================================

FILE: CalcApp.c

SERVICES: BREW Calculator and Tip Calculator applets

=============================================================================*/


/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who            what, where, why
----------   ---            ---------------------------------------------------
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif

#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "appscommonimages.brh"
#include "AppComFunc.h"
#include "calcapp.brh"
#include "Appscommon.h"
#include "AEEAnnunciator.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define  AEE_CALCAPP_RES_FILE (AEE_RES_LANGDIR CALCAPP_RES_FILE)

//
// Calc information stored on disk...
//

#define CALC_CFG_VERSION      0x1000
#define CALC_MAX_NUMBER_DISP  999999999
#define MAX_CALC_TEXT         9
/*小数部分最大个数*/
#define MAX_CALC_DECIMAL_NUMBER 4
/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/
#if defined (FEATURE_DISP_160X128)
/*显示数字线条宽度*/
#define CALC_NUMBER_LINE_WIDTH     2
/*显示数字宽度*/
#define CALC_NUMBER_WIDTH             9
/*显示数字高度*/
#define CALC_NUMBER_HEIGHT            19
/*显示计算区域X坐标原点*/
#define CALC_VAL_RECT_X                  10
/*显示计算区域最底行Y坐标原点*/
#define CALC_VAL_RECT_Y                  22
/*显示计算区域宽度*/
#define CALC_VAL_RECT_WIDTH          140
/*显示单行计算区域高度*/
#define CALC_VAL_RECT_HEIGHT         24
/*显示数字间间隔*/
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54
#elif defined (FEATURE_DISP_220X176)
/*显示数字线条宽度*/
#define CALC_NUMBER_LINE_WIDTH     2
/*显示数字宽度*/
#define CALC_NUMBER_WIDTH             9
/*显示数字高度*/
#define CALC_NUMBER_HEIGHT            19
/*显示计算区域X坐标原点*/
#define CALC_VAL_RECT_X                  10
/*显示计算区域最底行Y坐标原点*/
#define CALC_VAL_RECT_Y                  22
/*显示计算区域宽度*/
#define CALC_VAL_RECT_WIDTH          200
/*显示单行计算区域高度*/
#define CALC_VAL_RECT_HEIGHT         26
/*显示数字间间隔*/
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54

#elif defined(FEATURE_DISP_128X128)
/*显示数字线条宽度*/
#define CALC_NUMBER_LINE_WIDTH     2
/*显示数字宽度*/
#define CALC_NUMBER_WIDTH             9
/*显示数字高度*/
#define CALC_NUMBER_HEIGHT            19
/*显示计算区域X坐标原点*/
#define CALC_VAL_RECT_X                  10
/*显示计算区域最底行Y坐标原点*/
#define CALC_VAL_RECT_Y                  22
/*显示计算区域宽度*/
#define CALC_VAL_RECT_WIDTH          112
/*显示单行计算区域高度*/
#define CALC_VAL_RECT_HEIGHT         26
/*显示数字间间隔*/
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54

#elif defined(FEATURE_DISP_128X160)			//_Xs类坐标均未使用
//显示数字线条宽度
#define CALC_NUMBER_LINE_WIDTH     2
//显示数字宽度
#define CALC_NUMBER_WIDTH             9
//显示数字高度
#define CALC_NUMBER_HEIGHT            18
//显示计算区域X坐标原点
#define CALC_VAL_RECT_X                  7
//显示计算区域最底行Y坐标原点
#define CALC_VAL_RECT_Y                  22
//显示计算区域宽度
#define CALC_VAL_RECT_WIDTH          112
//显示单行计算区域高度
#define CALC_VAL_RECT_HEIGHT         26
//显示数字间间隔
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54

#elif defined(FEATURE_DISP_176X220)
//显示数字线条宽度
#define CALC_NUMBER_LINE_WIDTH     2
//显示数字宽度
#define CALC_NUMBER_WIDTH             9
//显示数字高度
#define CALC_NUMBER_HEIGHT            22
//显示计算区域X坐标原点
#define CALC_VAL_RECT_X                  10
//显示计算区域最底行Y坐标原点
#define CALC_VAL_RECT_Y                  35
//显示计算区域宽度
#define CALC_VAL_RECT_WIDTH          150
//显示单行计算区域高度
#define CALC_VAL_RECT_HEIGHT         26
//显示数字间间隔
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54

#elif defined(FEATURE_DISP_240X320)
//显示数字线条宽度
#define CALC_NUMBER_LINE_WIDTH        2
//显示数字宽度
#define CALC_NUMBER_WIDTH             9
//显示数字高度
#define CALC_NUMBER_HEIGHT            19
//显示计算区域X坐标原点
#define CALC_VAL_RECT_X              10
//显示计算区域最底行Y坐标原点
#define CALC_VAL_RECT_Y              32
//显示计算区域宽度
#define CALC_VAL_RECT_WIDTH          212
//显示单行计算区域高度
#define CALC_VAL_RECT_HEIGHT         26
//显示数字间间隔
#define CALC_BETWEEN_NUM_PIXEL       1
#ifdef FEATURE_LCD_TOUCH_ENABLE
#define CALC_ITEM                    18
#define CALC_LINE_ROW1               88//(88+25)
#define CALC_LINE_ROW2               125//(125+25)
#define CALC_LINE_ROW3               162//(162+25)
#define CALC_LINE_ROW4               199//(199+25)
#define CALC_LINE_ROW5               236//(236+25)
#define STARX                        5
#define SPACEX                       9
#define NUMWINDTH                    50
#define NUMHEIGHT                    27
#endif
//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54

#elif defined(FEATURE_DISP_320X240)
//显示数字线条宽度
#define CALC_NUMBER_LINE_WIDTH     2
//显示数字宽度
#define CALC_NUMBER_WIDTH             9
//显示数字高度
#define CALC_NUMBER_HEIGHT            24
//显示计算区域X坐标原点
#define CALC_VAL_RECT_X                  7
//显示计算区域最底行Y坐标原点
#define CALC_VAL_RECT_Y                  32
//显示计算区域宽度
#define CALC_VAL_RECT_WIDTH          280
//显示单行计算区域高度
#define CALC_VAL_RECT_HEIGHT         28
//显示数字间间隔
#define CALC_BETWEEN_NUM_PIXEL     1
    

//wlh 20090417 add icon x/y
//正负号
#define CALC_ZHENFU_X 12
#define CALC_ZHENFU_Y 113
#define CALC_ZHENFU_W 46
#define CALC_ZHENFU_H 53
#define CALC_ZHENFU_Xs 12
#define CALC_ZHENFU_Ys 113
#define CALC_ZHENFU_Ws 46
#define CALC_ZHENFU_Hs 53

//减号
#define CALC_SUBTRACT_X 58
#define CALC_SUBTRACT_Y 108
#define CALC_SUBTRACT_W 61
#define CALC_SUBTRACT_H 62
#define CALC_SUBTRACT_Xs 63
#define CALC_SUBTRACT_Ys 133
#define CALC_SUBTRACT_Ws 20
#define CALC_SUBTRACT_Hs 20

//加号
#define CALC_PLUS_X 58
#define CALC_PLUS_Y 108
#define CALC_PLUS_W 61
#define CALC_PLUS_H 62
#define CALC_PLUS_Xs 82
#define CALC_PLUS_Ys 113
#define CALC_PLUS_Ws 20
#define CALC_PLUS_Hs 20

//乘号
#define CALC_MULTI_X 58
#define CALC_MULTI_Y 108
#define CALC_MULTI_W 61
#define CALC_MULTI_H 62
#define CALC_MULTI_Xs 82
#define CALC_MULTI_Ys 152
#define CALC_MULTI_Ws 20
#define CALC_MULTI_Hs 20

//除号
#define CALC_DIVIDE_X 58
#define CALC_DIVIDE_Y 108
#define CALC_DIVIDE_W 61
#define CALC_DIVIDE_H 62
#define CALC_DIVIDE_Xs 102
#define CALC_DIVIDE_Ys 133
#define CALC_DIVIDE_Ws 20
#define CALC_DIVIDE_Hs 20

//等号
#define CALC_EQUAL_X 75
#define CALC_EQUAL_Y 126
#define CALC_EQUAL_W 28
#define CALC_EQUAL_H 28
#define CALC_EQUAL_Xs 83
#define CALC_EQUAL_Ys 133
#define CALC_EQUAL_Ws 20
#define CALC_EQUAL_Hs 20

//删除
#define CALC_RETURN_X 119
#define CALC_RETURN_Y 113
#define CALC_RETURN_W 47
#define CALC_RETURN_H 54
#define CALC_RETURN_Xs 119
#define CALC_RETURN_Ys 113
#define CALC_RETURN_Ws 47
#define CALC_RETURN_Hs 54


#endif

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define CALCAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE

//区分点击的区域
typedef enum CALCRecttype
{
	TYPE_PLUS,//加
	TYPE_SUBTRACT,//减
	TYPE_MULTI,//乘
	TYPE_DIVIDE,//除
	TYPE_ZHENFU,//正负
    TYPE_RETURN,//删除
	TYPE_EQUAL,//等号
	TYPE_BACK//返回
}CALCRecttype;
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

// Configuration information saved by the applet when it exits
typedef struct _CalcCfg {
    double  vMem;  // Current value in the Calcular "memory".
} CalcCfg;

typedef enum {
    OP_UNDEFINED = 0,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_EQUAL
} OpType;

typedef struct _calcnum {
    OpType           wOp;
    double           v;
} calcnum;

typedef struct _CCalcApp {
    AEEApplet a;
    AEERect   m_rc;      // Applet rectangle

    AEECLSID  m_cls;    // ClassID of the applet
                       // (either AEECLSID_CALC or AEECLSID_TIPCALC)

    AECHAR    m_szText[64]; // Current value being displayed
                                          // on the screen.
    calcnum   m_pValList[8];
    int       m_nValNum;
    boolean   operatorPushed;
    int16     m_cxInd;      // number of horizontal pixels required by the
                           // memory indicator (M) on the left of the display
    AEERect   m_valRect;    // Rectangle for use when drawing the value
    boolean   m_bClearLast;

    OpType    m_wLastOperator;

    AECHAR    m_szMemAnnun[2]; // String value of the memory indicator (M)

    CalcCfg   m_cfg;
    double     m_TempValue;
#if MIN_BREW_VERSION(3,0)
    byte        m_repeat_count;
#endif
    boolean   m_bIdle;
	CALCRecttype m_rtype;//wlh 20090417 add区分点击的区域
	IAnnunciator        *m_pIAnn;
#ifdef FEATURE_LCD_TOUCH_ENABLE
	boolean m_bup;
	int16   m_i;
#endif
} CCalcApp;


/*===========================================================================

                       STATIC/LOCAL FUNCTION DECLARATIONS

===========================================================================*/

#if defined(BREW_STATIC_APP)

int     CalcMod_Load(IShell   *ps,
                            void     *pHelpers,
                            IModule **pMod);

static int     CalcMod_CreateInstance(AEECLSID  ClsId,
                                      IShell   *pIShell,
                                      IModule  *po,
                                      void    **ppObj);
#endif /* defined(BREW_STATIC_APP) */


static boolean Calc_HandleEvent(CCalcApp *pme,
                                AEEEvent eCode,
                                uint16   wParam,
                                uint32   dwParam);
static void    Calc_ClearVals(CCalcApp *pme);
static void    Calc_SetupValRect(CCalcApp *pme);
static void    Calc_Startup(CCalcApp *pme, AEEAppStart *as);
static void    Calc_DrawScreen(CCalcApp *pme);
static void    Calc_PushVal(CCalcApp *pme, OpType wOp);
static void    Calc_AddChar(CCalcApp *pme, AECHAR chAdd, boolean bUnique);
static void    Calc_ToggleSign(CCalcApp *pme);
static void Calc_ShowAnnun( void *pme);
static int16   Calc_NumText(CCalcApp *pme,
                            AECHAR* ptext,
                            int16     x,
                            RGBVAL    clr,
                            boolean   bDraw,
                            boolean  bAllSmall);
static void    Calc_DrawNum(CCalcApp *pme);
static void    Calc_SetVal(CCalcApp *pme,double v,boolean bUpdate);
static double  Calc_GetVal(CCalcApp *pme);
static int     Calc_GetOpPrecedence(OpType op);
static uint16  Calc_GetOpResourceId(OpType op);
static boolean Calc_FloatToWStr(double v, AECHAR * psz, int nSize);

//wlh 20090417 add 为了区别点击，加，减，乘除，等号等等
static void CALC_drawClipRectWithOffset(CCalcApp *pMe,uint32 imageId,AEERect *rect)
{
	IImage *image = ISHELL_LoadResImage( pMe->a.m_pIShell,
                                         AEE_APPSCOMMONRES_IMAGESFILE,
                                         imageId);
#ifndef FEATURE_USES_LOWMEM
    if( image != NULL)
#endif    
    {
        AEERect oldClip;
		IDISPLAY_GetClipRect( pMe->a.m_pIDisplay, &oldClip);
        IDISPLAY_SetClipRect( pMe->a.m_pIDisplay, rect);
#ifndef FEATURE_USES_LOWMEM
        IIMAGE_SetOffset( image, rect->x, rect->y);
        IIMAGE_SetDrawSize( image, rect->dx, rect->dy);
        IIMAGE_Draw( image, rect->x, rect->y);
        IIMAGE_Release( image);
        image = NULL;
#else
		{
			AEERect rect;
		
			rect.x = 0;rect.y=0;rect.dx=SCREEN_WIDTH,rect.dy=SCREEN_HEIGHT;
			IDISPLAY_DrawRect(pMe->a.m_pIDisplay,&rect,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
		}
#endif
        IDISPLAY_SetClipRect( pMe->a.m_pIDisplay, &oldClip);
    }
}
static void CALC_DrawImageWithOffset( CCalcApp *pMe)//wlh 20090417 add 为了区别点击，加，减，乘除，等号等等
{
	AEERect clip;
	if(pMe->m_rtype == TYPE_PLUS)
		SETAEERECT( &clip, CALC_PLUS_X, CALC_PLUS_Y,CALC_PLUS_W,CALC_PLUS_H);

	else if(pMe->m_rtype == TYPE_SUBTRACT)
		SETAEERECT( &clip, CALC_SUBTRACT_X, CALC_SUBTRACT_Y,CALC_SUBTRACT_W,CALC_SUBTRACT_H);

	else if(pMe->m_rtype == TYPE_MULTI)
		SETAEERECT( &clip, CALC_MULTI_X, CALC_MULTI_Y,CALC_MULTI_W,CALC_MULTI_H);

	else if(pMe->m_rtype == TYPE_DIVIDE)
		SETAEERECT( &clip, CALC_DIVIDE_X, CALC_DIVIDE_Y,CALC_DIVIDE_W,CALC_DIVIDE_H);

	else if(pMe->m_rtype == TYPE_ZHENFU)
		SETAEERECT( &clip, CALC_ZHENFU_X, CALC_ZHENFU_Y,CALC_ZHENFU_W,CALC_ZHENFU_H);

	else if(pMe->m_rtype == TYPE_RETURN)
		SETAEERECT( &clip, CALC_RETURN_X, CALC_RETURN_Y,CALC_RETURN_W,CALC_RETURN_H);

	else if(pMe->m_rtype == TYPE_EQUAL)
		SETAEERECT( &clip, CALC_EQUAL_X, CALC_EQUAL_Y,CALC_EQUAL_W,CALC_EQUAL_H);
	else 
		return;
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128) || defined(FEATURE_DISP_176X220)
	CALC_drawClipRectWithOffset(pMe,IDB_CALCAPP_GROUND,&clip);
#else
#ifndef FEATURE_USES_LOWMEM
    CALC_drawClipRectWithOffset(pMe,IDB_CALCAPP,&clip);
#else
    CALC_drawClipRectWithOffset(pMe,0,&clip);
#endif
#endif
    IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, FALSE);
}
/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/
#if defined(BREW_STATIC_APP)
/*=============================================================================
FUNCTION: CalcMod_Load

DESCRIPTION:

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CalcMod_Load(IShell   *ps,
                 void     *pHelpers,
                 IModule **pMod)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                            ps,
                            pHelpers,
                            pMod,
                            (PFNMODCREATEINST)CalcMod_CreateInstance,
                            (PFNFREEMODDATA)NULL);
}

#endif /* defined(BREW_STATIC_APP) */

/*=============================================================================
FUNCTION: CalcMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   ClsId:
   *pIShell:
   *po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#if defined(BREW_STATIC_APP)
static int CalcMod_CreateInstance(AEECLSID  ClsId,
                                  IShell   *pIShell,
                                  IModule  *po,
                                  void    **ppObj)
#else
int AEEClsCreateInstance(AEECLSID  ClsId,
                         IShell   *pIShell,
                         IModule  *po,
                         void    **ppObj)
#endif
{
    *ppObj = NULL;

    if (!AEEApplet_New(sizeof(CCalcApp),
                       ClsId,
                       pIShell,
                       po,
                       (IApplet**)ppObj,
                       (AEEHANDLER) Calc_HandleEvent,
                       (PFNFREEAPPDATA) NULL)) {
       return ENOMEMORY;
    }

    ((CCalcApp *)*ppObj)->m_cls = ClsId;
    return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: Calc_HandleEvent

DESCRIPTION:

PARAMETERS:
   *pme:
   eCode:
   wParam:
   dwParam:

RETURN VALUE:
   boolean:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void drawImage( CCalcApp *pMe, char *resFile, int16 resId, int x, int y)
{

    IImage *image = ISHELL_LoadResImage( pMe->a.m_pIShell, resFile, resId);
    if( image != NULL)
    {
        IIMAGE_Draw( image, x, y);
        IIMAGE_Release( image);
		image = NULL;
    }
}

#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((b)<(a)) ? (a) : (b))
#endif
static void drawImageWithOffset( CCalcApp *pMe, char *resFile, int16 resId, int x, int y, AEERect *pRectOffset)
{

    IImage *image = ISHELL_LoadResImage( pMe->a.m_pIShell, resFile, resId);
#ifndef FEATURE_USES_LOWMEM
    if( image != NULL)
#endif
    {

        AEERect oldClip;
        AEERect clip;

        IDISPLAY_GetClipRect( pMe->a.m_pIDisplay, &oldClip);
        SETAEERECT( &clip, x, y, pRectOffset->dx + x, pRectOffset->dy + y);
        SETAEERECT( &oldClip, oldClip.x, oldClip.y, oldClip.dx + oldClip.x, oldClip.dy + oldClip.y);

        clip.x  = max( clip.x, oldClip.x);
        clip.y  = max( clip.y, oldClip.y);
        clip.dx = min( clip.dx, oldClip.dx);
        clip.dy = min( clip.dy, oldClip.dy);
        clip.dx = clip.dx - clip.x;
        clip.dy = clip.dy - clip.y;
        oldClip.dx = oldClip.dx - oldClip.x;
        oldClip.dy = oldClip.dy - oldClip.y;

        IDISPLAY_SetClipRect( pMe->a.m_pIDisplay, &clip);
#ifndef FEATURE_USES_LOWMEM
        IIMAGE_SetOffset( image, pRectOffset->x, pRectOffset->y);
        IIMAGE_SetDrawSize( image, pRectOffset->dx, pRectOffset->dy);
        IIMAGE_Draw( image, x, y);
        IIMAGE_Release( image);
		image = NULL;
#else
		{
			AEERect rect;
		
			rect.x = 0;rect.y=0;rect.dx=SCREEN_WIDTH,rect.dy=SCREEN_HEIGHT;
			IDISPLAY_DrawRect(pMe->a.m_pIDisplay,&rect,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
		}
#endif
        IDISPLAY_SetClipRect( pMe->a.m_pIDisplay, &oldClip);
    }
}

static void Calc_Reset( CCalcApp* pme)
{
    pme->m_bIdle = TRUE;
    Calc_ClearVals( pme);
    Calc_SetVal( pme, 0.0, FALSE);
    Calc_ShowAnnun( pme);
    Calc_DrawNum( pme);
}

static void Calc_Backspace( CCalcApp *pme)
{
    if( pme->m_szText == 0 || WSTRLEN( pme->m_szText) == 0)
    {
        if( pme->m_nValNum > 0)
        {
            // If the current number is zero but there are still
            // operators/values in the entire expression, simulate
            // a press of the Clear softkey to remove the entire
            // expression
            MSG_FATAL("reset...............",0,0,0);
            Calc_Reset( pme);
        }
        else
        {
            ISHELL_CloseApplet( pme->a.m_pIShell, FALSE);
        }
    }
    else
    {
#if 0
        if( pme->m_bClearLast)
        {
            Calc_Reset( pme);
        }
        else
#endif
        if(pme->m_bIdle)
        {
            ISHELL_CloseApplet( pme->a.m_pIShell, FALSE);
        }
        else
        {
          Calc_AddChar( pme, 'B', FALSE);
        }
    }
}

static void Calc_Overflow( CCalcApp *pme)
{
        AECHAR szError[16];
        AEERect rc;
        OpType wOp;
        RGBVAL nOldFontColor;

        rc = pme->m_valRect;
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
		drawImageWithOffset(pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, rc.x, rc.y, &rc);
#else
#ifndef FEATURE_USES_LOWMEM
        drawImageWithOffset(pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, rc.x, rc.y, &rc);
#else
        drawImageWithOffset(pme, AEE_APPSCOMMONRES_IMAGESFILE, 0, rc.x, rc.y, &rc);
#endif
#endif
        Calc_ClearVals(pme);
        pme->m_szText[1] = '0';
        wOp = OP_UNDEFINED;
        (void) ISHELL_LoadResString(pme->a.m_pIShell,
                AEE_CALCAPP_RES_FILE,
                IDS_OVERFLOW,
                szError,
                sizeof(szError));
        nOldFontColor = IDISPLAY_SetColor( pme->a.m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);
        (void) IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_NORMAL, szError, -1, rc.x, rc.y, &rc, IDF_ALIGN_RIGHT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);
        (void)IDISPLAY_SetColor( pme->a.m_pIDisplay, CLR_USER_TEXT, nOldFontColor/*RGB_BLACK*/);
        Calc_ShowAnnun(pme);
        IDISPLAY_Update(pme->a.m_pIDisplay);
}

static boolean Calc_HandleEvent(CCalcApp *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	BottomBar_Param_type BarParam;        //wlh 20090417 add   

	#ifdef FEATURE_LCD_TOUCH_ENABLE
	AEERect rc[CALC_ITEM]= 
					  	{
					  		{STARX,CALC_LINE_ROW1,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX),CALC_LINE_ROW1,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*2,CALC_LINE_ROW1,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*3,CALC_LINE_ROW1,NUMWINDTH,NUMHEIGHT},
					  		{STARX,CALC_LINE_ROW2,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX),CALC_LINE_ROW2,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*2,CALC_LINE_ROW2,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*3,CALC_LINE_ROW2,NUMWINDTH,NUMHEIGHT},
					  		{STARX,CALC_LINE_ROW3,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX),CALC_LINE_ROW3,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*2,CALC_LINE_ROW3,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*3,CALC_LINE_ROW3,NUMWINDTH,NUMHEIGHT},
					  		{STARX,CALC_LINE_ROW4,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX),CALC_LINE_ROW4,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*2,CALC_LINE_ROW4,NUMWINDTH,NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*3,CALC_LINE_ROW4,NUMWINDTH,(NUMHEIGHT+CALC_LINE_ROW5-CALC_LINE_ROW4)},
					  		{STARX,CALC_LINE_ROW5,NUMWINDTH+(NUMWINDTH+SPACEX),NUMHEIGHT},
					  		{STARX+(NUMWINDTH+SPACEX)*2,CALC_LINE_ROW5,NUMWINDTH,NUMHEIGHT}
					  	};
  int imageId[CALC_ITEM] = {IDB_CALCAPP_C_DOWN,IDB_CALCAPP_SIGN_DOWN,IDB_CALCAPP_DIV_DOWN,IDB_CALCAPP_MUL_DOWN
  						  ,IDB_CALCAPP_NUM7_DOWN,IDB_CALCAPP_NUM8_DOWN,IDB_CALCAPP_NUM9_DOWN,IDB_CALCAPP_SUB_DOWN
  						  ,IDB_CALCAPP_NUM4_DOWN,IDB_CALCAPP_NUM5_DOWN,IDB_CALCAPP_NUM6_DOWN,IDB_CALCAPP_ADD_DOWN
  						  ,IDB_CALCAPP_NUM1_DOWN,IDB_CALCAPP_NUM2_DOWN,IDB_CALCAPP_NUM3_DOWN,IDB_CALCAPP_EQUAL_DOWN
  						  ,IDB_CALCAPP_NUM0_DOWN,IDB_CALCAPP_DOT_DOWN};
  int imageIdN[CALC_ITEM] = {IDB_CALCAPP_C_NORMAL,IDB_CALCAPP_SIGN_NORMAL,IDB_CALCAPP_DIV_NORMAL,IDB_CALCAPP_MUL_NORMAL
  						  ,IDB_CALCAPP_NUM7_NORMAL,IDB_CALCAPP_NUM8_NORMAL,IDB_CALCAPP_NUM9_NORMAL,IDB_CALCAPP_SUB_NORMAL
  						  ,IDB_CALCAPP_NUM4_NORMAL,IDB_CALCAPP_NUM5_NORMAL,IDB_CALCAPP_NUM6_NORMAL,IDB_CALCAPP_ADD_NORMAL
  						  ,IDB_CALCAPP_NUM1_NORMAL,IDB_CALCAPP_NUM2_NORMAL,IDB_CALCAPP_NUM3_NORMAL,IDB_CALCAPP_EQUAL_NORMAL
  						  ,IDB_CALCAPP_NUM0_NORMAL,IDB_CALCAPP_DOT_NORMAL};
  AECHAR digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
  #endif
  
    if (pme == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_APP_START:
            pme->m_TempValue = 0.0;
            pme->m_bIdle = TRUE;
			#ifdef FEATURE_LCD_TOUCH_ENABLE
			pme->m_bup = TRUE;
			pme->m_i = -1;
			#endif
            Calc_Startup(pme, (AEEAppStart *) dwParam);
			IANNUNCIATOR_SetFieldIsActiveEx(pme->m_pIAnn,FALSE);   
			//IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn,FALSE);
			//IANNUNCIATOR_SetFieldText(pme->m_pIAnn,NULL);
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pme->a.m_pIShell,
                        AEE_CALCAPP_RES_FILE,                                
                        IDS_CALCTITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
            }
			MEMSET(&BarParam, 0, sizeof(BarParam));//wlh 20090417 add 
			BarParam.eBBarType = BTBAR_BACK;      //wlh 20090417 add
			DrawBottomBar(pme->a.m_pIDisplay,&BarParam);    //wlh 20090417 add

			IDISPLAY_Update(pme->a.m_pIDisplay);
			
            return TRUE;

        case EVT_APP_STOP:
            ISHELL_CancelTimer( pme->a.m_pIShell, 0, pme);
            (void) ISHELL_SetPrefs(pme->a.m_pIShell, AEECLSID_CALCAPP,
            CALC_CFG_VERSION, &pme->m_cfg, sizeof(CalcCfg));

            Calc_ClearVals(pme);
			IANNUNCIATOR_SetHasTitleText(pme->m_pIAnn,TRUE);
			if(NULL != pme->m_pIAnn)
			{
				IANNUNCIATOR_Release(pme->m_pIAnn);
        		pme->m_pIAnn = NULL;
			}
            return TRUE;

        case EVT_APP_SUSPEND:
            ISHELL_CancelTimer( pme->a.m_pIShell, 0, pme);
            pme->m_TempValue = Calc_GetVal(pme);
            return TRUE;

        case EVT_APP_RESUME:
            Calc_DrawScreen(pme);
            Calc_SetVal(pme, pme->m_TempValue, TRUE);
            return TRUE;
        case EVT_KEY_HELD:
        {
            switch (wParam)
            {
                case AVK_SOFT2:
                case AVK_CLR:
                    Calc_Reset( pme);
                    break;

                default:
                    break;
            }
            return TRUE;
        }

        case EVT_KEY_RELEASE:
        {
#if 0            
            switch (wParam)
            {
                case AVK_SOFT2:
                case AVK_CLR:
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_RETURN, CALC_RETURN_X, CALC_RETURN_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_RETURN;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_Backspace(pme);
                    break;

                default:
                    pme->m_bIdle = FALSE;
                    break;
            }
#endif
            if((wParam != AVK_SOFT2) && (wParam != AVK_CLR))
            {
                pme->m_bIdle = FALSE;
            }
            return TRUE;
        }
        case EVT_KEY:
        {
#ifdef FEATURE_VERSION_W317A	
			MSG_FATAL("***zzg Calc WSTRLEN(pme->m_szText)=%x***", WSTRLEN(pme->m_szText), 0, 0);

			if (pme->m_szText == 0)
			{
				MSG_FATAL("***zzg pme->m_szText == 0***", 0, 0, 0);
			}
			else
			{
				MSG_FATAL("***zzg pme->m_szText != 0***", 0, 0, 0);
			}

			if (WSTRLEN(pme->m_szText) != 0)
			{
				MEMSET(&BarParam, 0, sizeof(BarParam));
				BarParam.eBBarType = BTBAR_DELETE;     		

				MSG_FATAL("***zzg BarParam.eBBarType = BTBAR_DELETE***", 0, 0, 0);
			}
			else
			{	
				MEMSET(&BarParam, 0, sizeof(BarParam));
				BarParam.eBBarType = BTBAR_BACK;    

				MSG_FATAL("***zzg BarParam.eBBarType = BTBAR_BACK***", 0, 0, 0);
			}
			DrawBottomBar(pme->a.m_pIDisplay,&BarParam); 
			IDISPLAY_Update(pme->a.m_pIDisplay);
#endif

            switch (wParam)
            {
            	case AVK_O:
                case AVK_UP:
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_PLUS, CALC_PLUS_X, CALC_PLUS_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_PLUS;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_ADD);
                    break;
				case AVK_I:
                case AVK_LEFT:
					#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_MULTI, CALC_MULTI_X, CALC_MULTI_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_MULTI;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_MUL);
					#else
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_SUBTRACT, CALC_SUBTRACT_X, CALC_SUBTRACT_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_SUBTRACT;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_SUB);
					#endif
                    break;
#if defined(FEATURE_DISP_128X160) || defined(FEATURE_DISP_176X220)
#else
#endif         
                case AVK_DOWN:
					#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_SUBTRACT, CALC_SUBTRACT_X, CALC_SUBTRACT_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_SUBTRACT;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_SUB);
					#else
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_MULTI, CALC_MULTI_X, CALC_MULTI_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_MULTI;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_MUL);
					#endif
                    break;
                case AVK_G:
                case AVK_RIGHT:
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_DIVIDE, CALC_DIVIDE_X, CALC_DIVIDE_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_DIVIDE;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_DIV);
                    break;
#ifndef WIN32
#if defined( AEE_SIMULATOR)
                case AVK_SELECT:
#else
                case AVK_INFO:
#endif
#else
                case AVK_INFO:
#endif
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_EQUAL, CALC_EQUAL_X, CALC_EQUAL_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_EQUAL;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_PushVal(pme, OP_EQUAL);
                    break;
#ifndef WIN32
#if defined( AEE_SIMULATOR)
                case AVK_SOFT1:
#else
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128) || defined(FEATURE_DISP_176X220)
#ifdef FEATURE_VERSION_SKY
				case AVK_POUND:
#else
                case AVK_STAR:
#endif
#else
                case AVK_SELECT:
#endif
#endif
#else
				case AVK_SELECT:
#endif
					//wlh 20090417 add start
					//modi ydc 20090408
					//drawImage( pme,AEE_APPSCOMMONRES_IMAGESFILE,IDB_CALCAPP_ZHENFU, CALC_ZHENFU_X, CALC_ZHENFU_Y);
					IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
					pme->m_rtype = TYPE_ZHENFU;
					ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
					//wlh 20090417 add end
                    Calc_ToggleSign(pme);
                    break;
                case AVK_SOFT2:
                case AVK_CLR:
					MSG_FATAL("AVK_CLR............",0,0,0);
					#ifdef FEATURE_LCD_TOUCH_ENABLE
					ISHELL_CloseApplet( pme->a.m_pIShell, FALSE);
					#else
                	#ifdef FEATURE_ALL_KEY_PAD  //add by yangdecai 
                	if(dwParam == 1)
                	#else
                	if(TRUE)
                	#endif
                	{
                    	IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
                    	pme->m_rtype = TYPE_RETURN;
                    	ISHELL_SetTimer(pme->a.m_pIShell,50,(PFNNOTIFY)CALC_DrawImageWithOffset, pme);
                    	Calc_Backspace(pme);
                    }
                    else
                    {
                    	ISHELL_CloseApplet( pme->a.m_pIShell, FALSE);
                    }
					#endif
                    break;
//#if defined(FEATURE_DISP_176X220)
//				case AVK_STAR:
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128) || defined(FEATURE_DISP_176X220)
#ifdef FEATURE_VERSION_SKY
                case AVK_STAR:
#else
				case AVK_POUND:
#endif                    
#endif
                case AVK_M:
                    Calc_AddChar(pme, (AECHAR)'.', TRUE);
                    break;
#if !(defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128) || defined(FEATURE_DISP_176X220))
                case AVK_POUND:
                    Calc_Reset( pme);
                    break;
#endif
            } //switch (wParam)

            if (wParam >= AVK_0 && wParam <= AVK_9)
            {
                AECHAR digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
                Calc_AddChar(pme, digits[wParam - AVK_0], FALSE);
                pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
            }

            return TRUE;
        } // case EVT_KEY
        #ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_MOVE:
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				//AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
				int i = 0;
				AEERect rct;
				IImage *image = NULL;
				nBarH = GetBottomBarHeight(pme->a.m_pIDisplay);
        		MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pme->a.m_pIShell, &devinfo);
				SETAEERECT(&rct, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rct))
				{
					if(wXPos >= rct.x + (rct.dx/3)*2 && wXPos < rct.x + (rct.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_CALCAPP,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
				MSG_FATAL("00000pme->m_bup==%d,pme->m_i====%d",pme->m_bup,pme->m_i,0);
				if(!pme->m_bup)
				{
				MSG_FATAL("11111pme->m_bup==%d,pme->m_i====%d",pme->m_bup,pme->m_i,0);
            	if((TOUCH_PT_IN_RECT(wXPos,wYPos,rc[pme->m_i])&&eCode==EVT_PEN_UP)||
				   (!TOUCH_PT_IN_RECT(wXPos,wYPos,rc[pme->m_i])&&eCode==EVT_PEN_MOVE)
				  )
            	{
            		
                	image = ISHELL_LoadResImage( pme->a.m_pIShell,
                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                    imageIdN[pme->m_i]);
					if(image!=NULL)
					{
						IIMAGE_Draw(image,rc[pme->m_i].x,rc[pme->m_i].y);
						IIMAGE_Release(image);
						image = NULL;
					}
					i = pme->m_i;
					pme->m_bup = TRUE;
					pme->m_i = -1;
					switch(i)
					{
						case 0:
						{
							//boolean rt = ISHELL_PostEvent(pme->a.m_pIShell,AEECLSID_CALCAPP,EVT_USER,AVK_CLR,0);
					 		//return rt;
					 		Calc_Reset( pme);
					 		
							return TRUE;
						}
							break;
						case 1:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_ZHENFU;
							
               			 	Calc_ToggleSign(pme);
							break;
						case 2:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_DIVIDE;
							
                			Calc_PushVal(pme, OP_DIV);
							break;
						case 3:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_MULTI;
							
                			Calc_PushVal(pme, OP_MUL);
							break;
						case 4:
							Calc_AddChar(pme, digits[7], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 5:
							Calc_AddChar(pme, digits[8], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 6:
							Calc_AddChar(pme, digits[9], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 7:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_SUBTRACT;
							
                			Calc_PushVal(pme, OP_SUB);
							break;
						case 8:
							Calc_AddChar(pme, digits[4], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 9:
							Calc_AddChar(pme, digits[5], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 10:
							Calc_AddChar(pme, digits[6], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 11:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_PLUS;
							
                			Calc_PushVal(pme, OP_ADD);
							break;
						case 12:
							Calc_AddChar(pme, digits[1], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 13:
							Calc_AddChar(pme, digits[2], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 14:
							Calc_AddChar(pme, digits[3], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 15:
							IDISPLAY_UpdateEx(pme->a.m_pIDisplay,FALSE);
							pme->m_rtype = TYPE_EQUAL;
							
                			Calc_PushVal(pme, OP_EQUAL);
							break;
						case 16:
            				Calc_AddChar(pme, digits[0], FALSE);
            				pme->m_TempValue = WSTRTOFLOAT(pme->m_szText);
							return TRUE;
							break;
						case 17:
							Calc_AddChar(pme, (AECHAR)'.', TRUE);
							return TRUE;
							break;
						default:
							break;
					}
					
            	}
                
				IDISPLAY_Update(pme->a.m_pIDisplay);
				}
				
			}
			break;
		case EVT_PEN_DOWN:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				int i = 0;
				int j = 0;
				
				int16 wXPos   = (int16)AEE_GET_X(dwParam);
				int16 wYPos   = (int16)AEE_GET_Y(dwParam);
				IImage *image = NULL;
				pme->m_i = -1;
				MSG_FATAL("wXPos===%d,     wYPos===%d",wXPos,wYPos,0);
                for(i = 0;i<CALC_ITEM;i++)
                {
                	if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc[i]))
                	{
                		pme->m_i = i;
						pme->m_bup = FALSE;
	                	image = ISHELL_LoadResImage( pme->a.m_pIShell,
                                        AEE_APPSCOMMONRES_IMAGESFILE,
                                        imageId[i]);
						if(image!=NULL)
						{
							MSG_FATAL("rc[i].x===%d,rc[i].y===%d,i===%d",rc[i].x,rc[i].y,i);
							IIMAGE_Draw(image,rc[i].x,rc[i].y);
							IIMAGE_Release( image);
							image = NULL;
						}
	                	break;
                	}
                } 
				IDISPLAY_Update(pme->a.m_pIDisplay);
			}
			break;
		case EVT_USER:
			if(wParam == AVK_CLR)
			{
				eCode = EVT_KEY;
				return Calc_HandleEvent(pme,eCode,wParam,dwParam);
		    }
#endif



    } // switch( eCode)

    return FALSE;
}

/*=============================================================================
FUNCTION: Calc_ClearVals

DESCRIPTION:  Reset the calculator.  The calculator memory is preserved.

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_ClearVals(CCalcApp *pme)
{
    if(pme == NULL)
    {
       return ;
    }

    pme->m_nValNum = 0;
    MEMSET( pme->m_pValList, 0, sizeof( pme->m_pValList));

	
#ifdef FEATURE_VERSION_C337
	MEMSET( pme->m_szText, 0, sizeof( pme->m_szText));		//Modify by zzg 2012_11_13
#else
	pme->m_szText[0] = (AECHAR) '\0';
#endif
    
    pme->m_wLastOperator = OP_UNDEFINED;
    pme->m_bClearLast = TRUE;
}

/*=============================================================================
FUNCTION: Calc_SetupValRect

DESCRIPTION:  Setup the CCalcApp:m_valRect rectangle, which is used to
              place the current calculator value on the display.

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_SetupValRect(CCalcApp *pme)
{
    pme->m_valRect.x  = CALC_VAL_RECT_X;///3;
    pme->m_valRect.y  = CALC_VAL_RECT_Y;//28;
    pme->m_valRect.dx = CALC_VAL_RECT_WIDTH;//120;
    pme->m_valRect.dy = CALC_VAL_RECT_HEIGHT;//16;
}


/*=============================================================================
FUNCTION: Calc_Startup

DESCRIPTION:

PARAMETERS:
   *pme:
   *as:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_Startup(CCalcApp *pme, AEEAppStart *as)
{
    if(pme == NULL||as == NULL)
    {
       return ;
    }
	if (AEE_SUCCESS != ISHELL_CreateInstance(pme->a.m_pIShell,
                                            AEECLSID_ANNUNCIATOR,
                                            (void **)&pme->m_pIAnn))
    {
        return ;
    }
    pme->m_rc = as->rc;

    pme->m_nValNum = 0;
    MEMSET( pme->m_pValList, 0, sizeof( pme->m_pValList));

    pme->m_bClearLast   = TRUE;
   (void) ISHELL_GetPrefs(pme->a.m_pIShell,
                  AEECLSID_CALCAPP,
                  CALC_CFG_VERSION,
                  &pme->m_cfg,
                  sizeof(pme->m_cfg)
              );
   Calc_DrawScreen(pme);
}

/*=============================================================================
FUNCTION: Calc_DrawScreen

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_DrawScreen(CCalcApp *pme)
{
	BottomBar_Param_type BarParam;        //wlh 20090417 add          
	MEMSET(&BarParam, 0, sizeof(BarParam));//wlh 20090417 add   
	BarParam.eBBarType = BTBAR_BACK;      //wlh 20090417 add

	Calc_SetupValRect(pme);
#if defined(FEATURE_DISP_220X176)
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, 0, 0);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, 0, 72);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_LINE, 0, 48);
#elif defined(FEATURE_DISP_176X220)
    drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, 0, 0);
    drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, 0, 104);
    drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_LINE, 0, 72);
#elif defined(FEATURE_DISP_128X160)
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, 0, 0);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, 0, 76);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_LINE, 0, 52);
#elif defined(FEATURE_DISP_160X128)
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, 0, 0);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, 0, 48);
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_LINE, 0, 48);

#else
#ifndef FEATURE_USES_LOWMEM
	drawImage( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, 0, 0);
#else
{
	AEERect rect;

	rect.x = 0;rect.y=0;rect.dx=SCREEN_WIDTH,rect.dy=SCREEN_HEIGHT;
	IDISPLAY_DrawRect(pme->a.m_pIDisplay,&rect,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
}
#endif
#endif
	Calc_SetVal(pme, 0.0, FALSE);
	Calc_ShowAnnun(pme);
	DrawBottomBar(pme->a.m_pIDisplay,&BarParam);    //wlh 20090417 add
	IDISPLAY_Update(pme->a.m_pIDisplay);
}
/*=============================================================================
FUNCTION: Calc_PushVal

DESCRIPTION:

PARAMETERS:
   *pme:
    wOp:  Operator to push.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean Calc_operate( CCalcApp* pme, int i)
{

    switch( pme->m_pValList[i].wOp)
    {
        case OP_MUL: //乘
            pme->m_pValList[i].v = FMUL( pme->m_pValList[i].v, pme->m_pValList[i + 1].v);
            break;

        case OP_ADD: //加
            pme->m_pValList[i].v = FADD( pme->m_pValList[i].v, pme->m_pValList[i + 1].v);
            break;

        case OP_DIV: //除
            if (FCMP_E( pme->m_pValList[i + 1].v, 0.0))//如果除数为0...
            {
                AECHAR szError[64];
                AEERect rc;
                RGBVAL nOldFontColor;

                //清空链表
                Calc_ClearVals(pme);

                //显示‘除数为0’
                rc = pme->m_valRect;
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
				drawImageWithOffset( pme,
                             AEE_APPSCOMMONRES_IMAGESFILE,
                             IDB_CALCAPP_GROUND,
                             rc.x,
                             rc.y,
                             &rc
                         );
#else
#ifndef FEATURE_USES_LOWMEM
                drawImageWithOffset( pme,
                             AEE_APPSCOMMONRES_IMAGESFILE,
                             IDB_CALCAPP,
                             rc.x,
                             rc.y,
                             &rc
                         );
#else
                drawImageWithOffset( pme,
                             AEE_APPSCOMMONRES_IMAGESFILE,
                             0,
                             rc.x,
                             rc.y,
                             &rc
                         );
#endif
#endif
                ISHELL_LoadResString(pme->a.m_pIShell,AEE_CALCAPP_RES_FILE,IDS_DIV_0,szError,sizeof(szError));
                nOldFontColor = IDISPLAY_SetColor( pme->a.m_pIDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, szError, -1, rc.x, rc.y, &rc, IDF_ALIGN_RIGHT|IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor( pme->a.m_pIDisplay, CLR_USER_TEXT, nOldFontColor/*RGB_BLACK*/);
                pme->m_nValNum = 0;
                Calc_ShowAnnun(pme);
                return FALSE;
            }
            else
            {
                //除数不为0，做除法运算
                pme->m_pValList[i].v = FDIV( pme->m_pValList[i].v, pme->m_pValList[i + 1].v);
            }
            break;

        case OP_SUB: //减
            pme->m_pValList[i].v = FSUB( pme->m_pValList[i].v, pme->m_pValList[i + 1].v);
            break;
    }

    return TRUE;
}

static void Calc_PushVal(CCalcApp *pme, OpType wOp)
{

    double  v   = 0.0;
    int     i   = 0;

    if(pme == NULL || pme->m_cls != AEECLSID_CALCAPP)
    {
       return ;
    }

    pme->operatorPushed = TRUE;
    pme->m_wLastOperator = wOp;

    if(pme->m_bClearLast && pme->m_nValNum > 0)
    {
        pme->m_pValList[pme->m_nValNum].wOp = wOp;

        if( wOp != OP_EQUAL)
        {
            Calc_ShowAnnun(pme);
            return;
        }
    }
    else
    {
        pme->m_nValNum ++;
        pme->m_pValList[pme->m_nValNum].wOp     = wOp;
        pme->m_pValList[pme->m_nValNum].v       = Calc_GetVal(pme);
        pme->m_bClearLast = TRUE;
    }

    //优先级高的先计算
    for( i = 1; i < pme->m_nValNum;)
    {

        //如果第一个node优先级高于第二个node优先级，首先计算第一个node
      //  if( Calc_GetOpPrecedence(pme->m_pValList[i].wOp) >=
      //      Calc_GetOpPrecedence(pme->m_pValList[i + 1].wOp)
      //  )
      //  {
            if( pme->m_pValList[i].wOp == OP_UNDEFINED)
            {
                Calc_Reset( pme);
                return;
            }

            if( !Calc_operate( pme, i))
            {
                return;
            }

            if( pme->m_pValList[i].v > CALC_MAX_NUMBER_DISP ||
                pme->m_pValList[i].v < (0 - CALC_MAX_NUMBER_DISP))
            {
                Calc_Overflow( pme);
                return;
            }

            pme->m_pValList[i].wOp = pme->m_pValList[i + 1].wOp;

            {
                int j = 0;
                for( j = i + 1; j < pme->m_nValNum; j ++)
                {
                    pme->m_pValList[j].v   = pme->m_pValList[j + 1].v;
                    pme->m_pValList[j].wOp = pme->m_pValList[j + 1].wOp;
                }

                pme->m_nValNum --;
            }

            i = 1;
        //}
        //else
       // {
        //    i ++;
       // }
    }


    if( wOp == OP_EQUAL)
    {
        v = 0;

        while( pme->m_nValNum > 0)
        {
            v   = pme->m_pValList[1].v;
            wOp = pme->m_pValList[1].wOp;

            if( v > CALC_MAX_NUMBER_DISP ||
                pme->m_pValList[i].v < (0 - CALC_MAX_NUMBER_DISP))
            {
                Calc_Overflow( pme);
                pme->m_nValNum = 0;
                return;
            }

            if( pme->m_nValNum == 1)
            {
                pme->m_nValNum = 0;
                break;
            }

            if( !Calc_operate( pme, 1))
            {
                return;
            }

            pme->m_pValList[1].wOp = pme->m_pValList[2].wOp;
            {
                int j = 0;
                for( j = 2; j < pme->m_nValNum; j ++)
                {
                    pme->m_pValList[j].v   = pme->m_pValList[j + 1].v;
                    pme->m_pValList[j].wOp = pme->m_pValList[j + 1].wOp;
                }
            }
            pme->m_nValNum --;
        }

        //显示运算结果
        if( v == 0.0)
        {
            MEMSET( pme->m_szText, 0, sizeof( pme->m_szText));
            pme->m_szText[0] = '0';
            Calc_DrawNum( pme);
        }
        else
        {
            Calc_SetVal(pme, v, FALSE);
        }
    }
    else
    {
        MEMSET( pme->m_szText, 0, sizeof( pme->m_szText));
        Calc_DrawNum( pme);
    }

    Calc_ShowAnnun(pme);
}

/*=============================================================================
FUNCTION: Calc_AddChar

DESCRIPTION:

PARAMETERS:
   *pme:
   chAdd:  Character to add (or zero to remove the most recent character)
   bUnique:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_AddChar(CCalcApp *pme, AECHAR chAdd, boolean bUnique)
{
    AECHAR *psz,
            ch;
    int     i = 0;
    int     nIntLen = 0, nTailLen = 0;
    AECHAR *pszBase = pme->m_szText;
    boolean bMenuSel;
    boolean bDot = FALSE;

    bMenuSel = FALSE;

    if (pme->m_wLastOperator != OP_UNDEFINED)
    {
        if (pme->m_wLastOperator != OP_EQUAL)
        {
            bMenuSel = TRUE;
        }
        pme->m_wLastOperator = OP_UNDEFINED;
    }

    if(chAdd >= '0' && chAdd <= '9')
    {
        int nZeroPos = (pszBase[0] == '-')?(1):(0);

        if(pszBase[nZeroPos] == '0' && WSTRLEN( pme->m_szText) == 1 + nZeroPos)
        {
            pszBase[nZeroPos] = chAdd;
            pszBase[nZeroPos + 1] = 0;
            pme->m_bClearLast = FALSE;
            goto __calc_addchar__end__;
        }
    }

    if (pme->m_bClearLast)
    {
        if( pme->m_nValNum == 0 &&
            WSTRLEN( pme->m_szText) > 0 &&
            //!( pszBase[0] == '0' && pszBase[1] == '.')
            !( WSTRLEN( pme->m_szText) == 1 && pszBase[0] == '0')
            )
        {
            MEMSET( pme->m_szText, 0, sizeof( pme->m_szText));
            pszBase[0] = '0';
            //pszBase[1] = '.';

            pme->m_bClearLast = TRUE;
            goto __calc_addchar__end__;
        }

        pszBase[0]          = 0;
        pme->m_bClearLast   = FALSE;

        if (pme->m_cls == AEECLSID_CALCAPP)
        {
            // Update the annunciators as well.  This is needed when
            // the user presses a new digit after the Equals operator
            // was used.  The equals annunciator should be removed
            // from the display.
            //pme->m_wLastOperator = OP_UNDEFINED;
            //Calc_SetVal(pme,chAdd,FALSE);
            //Calc_AddChar( pme, 'B', FALSE);
			Calc_ShowAnnun(pme);
        }
    }

    // Find the NULL character (psz will point to that location)
    for (psz = pszBase, i = 0; (ch = *psz) != 0; i++, psz++) 
    {
        if (bUnique && ch == chAdd)
        {
            return; // Unable to add character
        }
        //count int length
        if(ch == '.')
        {
            bDot = TRUE;
            break;
        }
    }
    //to count the numbers before and behind the radix point 
    nIntLen = (pszBase[0] == '-')? (i):(i + 1);
    if(TRUE == bDot)
    {
        for (nTailLen = i; (ch = *psz) != 0; i++, psz++)
        {
            if (bUnique && ch == chAdd)
            {
                return; // Unable to add character
            }
        }
        nTailLen = i - nTailLen - 1;
    }

    // If we are removing a character...
    if( chAdd == 'B')
    {
        psz--;

        // If it's the tip calculator, we are dealing with currency.
        // In that case, pull the values from left to right and pre-pend a 0.
        // We will then strip leading zeros below...
        if (!i)
        {
            // Trying to delete a character when the string is NULL!
            goto __calc_addchar__end__;
        }

        *psz = 0;

        // If the new string only consists of a minus character, remove
        // it too.
        if ( (psz-1 == pszBase) && ((AECHAR) '-' == *pszBase))
        {
            *pszBase = 0;
        }

        if( !*pszBase && pme->m_nValNum == 0)
        {
            MEMSET( pme->m_szText, 0, sizeof( pme->m_szText));
            pszBase[0] = '0';
            //pszBase[1] = '.';

            // Force the 0 to be removed the next time...
            pme->m_bClearLast = TRUE;
            pme->m_bIdle= TRUE;
        }
    }
    else if( chAdd == '.' && i == 0)
    {
        psz[0] = '0';
        psz[1] = '.';
    }
    else
    {
        if (((TRUE != bDot) && (nIntLen > MAX_CALC_TEXT) && (chAdd != '.')) || 
                ((TRUE == bDot) && (nTailLen >= MAX_CALC_DECIMAL_NUMBER)))
        {
            return; // Unable to add character
        }

        *psz = chAdd;
        psz[1] = 0;
    }

__calc_addchar__end__:
    Calc_DrawNum( pme);
}
/*=============================================================================
FUNCTION: Calc_ToggleSign

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_ToggleSign(CCalcApp *pme)
{
    AECHAR  *pszDst;
    AECHAR  *pszSrc;

    if(pme == NULL)
    {
       return ;
    }
    if (pme->m_cls == AEECLSID_CALCAPP) {
       pszDst = pme->m_szText;

       if (*pszDst == (AECHAR)'-') {

          // Remove the minus sign by shifting the rest of the
          // digits left by one
          pszSrc = pszDst + 1;
          for (;;) {
             *pszDst = *pszSrc;
             if (*pszDst == 0) {
                break;
             }
             pszSrc++;
             pszDst++;
          }
       } else {

          // Make room for the minus sign at the front of the text by
          // shifting the digits right by one.
          while (*pszDst) {
          pszDst++;
          }
          pszSrc = pszDst - 1;
          pszDst[1] = 0;
          while (pszSrc >= pme->m_szText) {
             *pszDst = *pszSrc;
             pszSrc--;
             pszDst--;
          }
          *pszDst = (AECHAR) '-';
       }
        pme->m_bClearLast = FALSE;
        Calc_DrawNum( pme);
    }
}

/*=============================================================================
FUNCTION: Calc_ShowAnnun

DESCRIPTION:  Draw the memory and operator annunciators

PARAMETERS:
   *pme:
    wOp: Operator to display (if OP_UNDEFINED, the last operator in
                              CCalcApp::m_pValList will be used)
   bUpdate:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_ShowAnnun( void* pMe)
{
    CCalcApp* pme = pMe;
    AEERect   rc;
    //int16     nHeight;
    IDisplay *pd = pme->a.m_pIDisplay;
#if 0
    AECHAR    szBuff[2];
#endif

    if(pme == NULL)
    {
       return ;
    }
    {
        AECHAR    text[32] = {0};
        int           length    = 0;
        int           i            = 0;
        
        SETAEERECT(&rc, pme->m_valRect.x, pme->m_valRect.y - 2*pme->m_valRect.dy, pme->m_valRect.dx, 2*pme->m_valRect.dy);   
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
		drawImageWithOffset( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP_GROUND, rc.x, rc.y, &rc);
#else
#ifndef FEATURE_USES_LOWMEM
        drawImageWithOffset( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDB_CALCAPP, rc.x, rc.y, &rc);
#else
        drawImageWithOffset( pme, AEE_APPSCOMMONRES_IMAGESFILE, 0, rc.x, rc.y, &rc);
#endif
#endif
        for( i = 1; i <= pme->m_nValNum; i ++)
        {
            Calc_FloatToWStr(pme->m_pValList[i].v, text, sizeof(text));
            length = WSTRLEN( text);
            if(length == 0)
            {
                text[length ++] = (AECHAR)'0';
            }
            (void) ISHELL_LoadResString(pme->a.m_pIShell,
                                   AEE_CALCAPP_RES_FILE,
                                   Calc_GetOpResourceId( pme->m_pValList[i].wOp),
                                   text + length,
                                   (32 - length) * sizeof(AECHAR)
                               );
            length = WSTRLEN( text);
            text[length ++] = 0;
            
            if( length > 0)
            {
                int x = 0;
                int16 width = 0;
                
                pme->m_valRect.y = rc.y + (i + 1 - pme->m_nValNum)*pme->m_valRect.dy;
                width = Calc_NumText( pme, text, -1, RGB_WHITE, FALSE, TRUE); 
                x = (width <= pme->m_valRect.dx)? (pme->m_valRect.x + pme->m_valRect.dx - width):(pme->m_valRect.x);
                IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &pme->m_valRect);
                (void)Calc_NumText( pme, text, x, RGB_WHITE, TRUE, TRUE);
            }
        }

        Calc_SetupValRect( pme);
        IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &pme->m_rc);
    }

    IDISPLAY_Update(pd);
}

/*=============================================================================
FUNCTION: Calc_NumText

DESCRIPTION:  Measures the screen width of the CCalcApp::m_szText[] string,
              and optionally draws it on the display.

PARAMETERS:
   *pme:
   x:  Base x value to start drawing at (ignored if bDraw is FALSE)
   clr:  Text colour to use  (ignored if bDraw is FALSE)
   bDraw:  TRUE if the text should be drawn as well as measured
   bAllSmall:  TRUE if the normal font should always be used

RETURN VALUE:
   int:  Width of the text in pixels

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int16 Calc_NumText(CCalcApp *pme,
                          AECHAR* ptext,
                          int16     x,
                          RGBVAL    clr,
                          boolean   bDraw,
                          boolean  bAllSmall)
{
    IDisplay *pd = pme->a.m_pIDisplay;
    int16     nWidth,
              nTotalWidth;
    AECHAR   *pch,
              ch;
    int16     nLHeight,
              nHeight,
              ySmall,
              yLarge,
              yDot,
              y,
              ySymbol;
    AEEFont   fnt;

    if(pme == NULL)
    {
       return 0;
    }
    nTotalWidth = 0;

    // Y for small characters is up from bottom of y for large characters...
    nHeight = (int16) IDISPLAY_GetFontMetrics(pd, AEE_FONT_BOLD, NULL,NULL);

    // Determine size and starting y for large characters...
    nLHeight = (int16) IDISPLAY_GetFontMetrics(pd, AEE_FONT_LARGE, NULL,NULL);

    // Set normal (AM/PM, etc.) up from the bottom of the large numbers and
    // colons in the middle...
#if 1
    bAllSmall = TRUE;
    ySmall = yDot = ySymbol = yLarge = pme->m_valRect.y;
#else
    if (pme->m_rc.dy > nLHeight) {
       yLarge = pme->m_rc.y + ((pme->m_rc.dy - nLHeight) / 2);
       ySmall = (yLarge + nLHeight) - nHeight;
       if (bAllSmall) {
          yDot = ySmall;
          ySymbol = ySmall;
       } else {
          yDot  = yLarge + nLHeight - nHeight;
          ySymbol = yLarge + ((nLHeight - nHeight) / 2);
       }
    } else {
       bAllSmall = TRUE;
       ySmall = yDot = ySymbol = yLarge = (pme->m_rc.y + 1);
    }
#endif

    // Draw or measure each character...
    clr = IDISPLAY_SetColor(pd, CLR_USER_TEXT, clr);
    for (pch = ptext; ( ch = *pch) != 0; pch++)
    {

        if (!bAllSmall && ((ch <= (AECHAR)'9' && ch >= (AECHAR)'0') || ch == (AECHAR)'.' || ch == (AECHAR)'-'))
        {
            fnt = AEE_FONT_LARGE;
            y = yLarge;
        }
        else
        {
            fnt = AEE_FONT_BOLD;
            if (ch == (AECHAR)'.')
            {
                y = yDot;
            }
            else
            {
                if (ch == (AECHAR)'$')
                {
                    y = ySymbol;
                }
                else
                {
                    y = ySmall - CALC_BETWEEN_NUM_PIXEL;
                }
            }
        }
        
       if( ch <= (AECHAR)'9' && ch >= (AECHAR)'0')
       {
           nWidth = CALC_NUMBER_WIDTH + CALC_BETWEEN_NUM_PIXEL;
       }
       else if(ch == '.')
       {
           nWidth = CALC_BETWEEN_NUM_PIXEL + (int16) IDISPLAY_MeasureTextEx(pd,
                                       fnt,
                                       (const AECHAR *)pch,
                                       1,
                                       -1,
                                       NULL
                                   );
       }
       else
       {
           AECHAR tmp[] = {'÷'}; //because '+' is wider than any other operator, so used its width as standard
           nWidth = CALC_BETWEEN_NUM_PIXEL + (int16) IDISPLAY_MeasureTextEx(pd,
                                       fnt,
                                       tmp,//(const AECHAR *)pch, 
                                       1,
                                       -1,
                                       NULL
                                   );
       }       

       if (bDraw)
       {
           if( ch <= (AECHAR)'9' && ch >= (AECHAR)'0')
           {
               AEERect  rect    = {0};
               RGBVAL   nFontColor = (pme->m_bIdle)?(MAKE_RGB(0xC0, 0xC0, 0xC0)):(RGB_WHITE);
               //int      nHeight = CALC_NUMBER_HEIGHT;//11;

               y = y + pme->m_valRect.dy - CALC_NUMBER_HEIGHT;
               SETAEERECT( &rect, x, y, CALC_NUMBER_WIDTH, CALC_NUMBER_HEIGHT);
               //drawImageWithOffset( pme, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CALCAPP_DIGITS, x, y, &rect);
               Appscommon_DrawDigitalNumber(pme->a.m_pIDisplay, (ch - '0'), CALC_NUMBER_LINE_WIDTH, &rect, nFontColor/*RGB_WHITE*/);
           }
           else
           {
              (void) IDISPLAY_DrawText( pd, fnt, pch, 1, x, y, &pme->m_valRect, IDF_TEXT_TRANSPARENT| IDF_ALIGN_BOTTOM);
           }
       }

       nTotalWidth += nWidth;
       x += nWidth;
    }
    (void) IDISPLAY_SetColor(pd, CLR_USER_TEXT, clr);

    return nTotalWidth;
}

/*=============================================================================
FUNCTION: Calc_DrawNum

DESCRIPTION: Draws CCalcApp::m_szText[] string on the display right justified.

PARAMETERS:
   *pme:
   bUpdate:  TRUE if IDISPLAY_Update() should be called once the number
             has been drawn.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_DrawNum( CCalcApp *pme)
{

    boolean   bSmall     = 0;
    int16     nWidth     = 0;
    AEERect   rc         = pme->m_valRect;
    BottomBar_Param_type BarParam;                 
	MEMSET(&BarParam, 0, sizeof(BarParam));
    
#if defined(FEATURE_DISP_220X176)||defined(FEATURE_DISP_128X160)||defined(FEATURE_DISP_160X128)|| defined(FEATURE_DISP_176X220)
	drawImageWithOffset( pme,
                 AEE_APPSCOMMONRES_IMAGESFILE,
                 IDB_CALCAPP_GROUND,
                 rc.x,
                 rc.y,
                 &rc
             );
#else
#ifndef FEATURE_USES_LOWMEM
    drawImageWithOffset( pme,
                 AEE_APPSCOMMONRES_IMAGESFILE,
                 IDB_CALCAPP,
                 rc.x,
                 rc.y,
                 &rc
             );
#else
    drawImageWithOffset( pme,
                 AEE_APPSCOMMONRES_IMAGESFILE,
                 0,
                 rc.x,
                 rc.y,
                 &rc
             );
#endif
#endif
#if 0
    bSmall = FALSE;
    nWidth = Calc_NumText( pme, -1, RGB_WHITE, FALSE, bSmall);

    if( nWidth >= (rc.dx - (pme->m_cxInd + 2)))
#endif
    {
       // The large font is too large, use the small font instead...
       bSmall = TRUE;
       nWidth = Calc_NumText( pme, pme->m_szText, -1, RGB_WHITE/*RGB_BLACK*/, FALSE, bSmall);
    }
#if 0
    {
        static int  x = 999;
        
        if( nWidth <= rc.dx)
        {
            x = rc.dx - nWidth;
        }
        else
        {
            if( x <= ( rc.x - nWidth + rc.dx))
            {
                x = rc.x + 12;
            }
            else
            {
                x -= 12;
            }

            ISHELL_SetTimer( pme->a.m_pIShell, 500, (PFNNOTIFY)Calc_DrawNum, pme);
            }

            IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &rc);
            (void)Calc_NumText( pme, pme->m_szText, x, RGB_WHITE/*RGB_BLACK*/, TRUE, bSmall);
            IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &pme->m_rc);
        }
#else
    {
        int  x = 0;
        
        if( nWidth <= rc.dx)
        {
            x = rc.x + rc.dx - nWidth;
        }
        else
        {
            x = rc.x;
        }
        IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &rc); 
        (void)Calc_NumText( pme, pme->m_szText, x, RGB_WHITE, TRUE, bSmall);
        IDISPLAY_SetClipRect( pme->a.m_pIDisplay, &pme->m_rc);
    }
#endif
    //if (bUpdate)
    {
         
           if(pme->m_bClearLast == FALSE && WSTRLEN( pme->m_szText) > 0)
       	   {
             BarParam.eBBarType = BTBAR_DELETE;      
           }
           else
           {
       	     BarParam.eBBarType = BTBAR_BACK;     
           }
           DrawBottomBar(pme->a.m_pIDisplay,&BarParam);
           IDISPLAY_Update(pme->a.m_pIDisplay);
    }
}

/*=============================================================================
FUNCTION: Calc_SetVal

DESCRIPTION:

PARAMETERS:
   *pme:
   v:
   bUpdate:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void Calc_SetVal(CCalcApp *pme, double v, boolean bUpdate)
{
    double temp;
    int nLen;

    if (pme == NULL)
    {
        return;
    }

    if (v > CALC_MAX_NUMBER_DISP ||
        v < (0 - CALC_MAX_NUMBER_DISP))
    {
        Calc_Overflow( pme);
        return;
    }

    //对于位数到顶的小数四舍五入
    if (v >= 1000000000.0)
    {
#ifndef WIN32//wlh 临时
        temp = v - (int)(v/1);
#else
		temp = v - (v/1);
#endif//WIN32  
        if (temp >= 0.5)
            v += 1;
    }
 
    Calc_FloatToWStr(v, pme->m_szText, sizeof(pme->m_szText));

    nLen = WSTRLEN(pme->m_szText);
    if (pme->m_szText[nLen - 1] == (AECHAR)'.')
    {
        pme->m_szText[nLen - 1] = 0;
    }
 
    Calc_DrawNum( pme);
}

/*=============================================================================
FUNCTION: Calc_GetVal

DESCRIPTION:  Converts the string value stored in CCalcApp:m_szText into
              a double value.

PARAMETERS:
   *pme:

RETURN VALUE:
   double:  Floating point value of CCalcApp:m_szText

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static double Calc_GetVal(CCalcApp *pme)
{
    if( pme == NULL || pme->m_szText[0] == 0)
    {
       return 0;
    }

    return WSTRTOFLOAT( pme->m_szText);
}

/*=============================================================================
FUNCTION: Calc_GetOpPrecedence

DESCRIPTION:  Returns a numberical representation of an operator's precedence.

PARAMETERS:
   op: operator

RETURN VALUE:
   int: numerical representation of the operator's precedence.  The greater
        this number, the greater the operator's precedence.

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int Calc_GetOpPrecedence(OpType op)
{
    switch (op) {
    case OP_UNDEFINED:
       return 0;
    case OP_ADD:
    case OP_SUB:
       return 1;
    case OP_MUL:
    case OP_DIV:
       return 2;
    case OP_EQUAL:
       return 0;
    }
    return 0;
}

/*=============================================================================
FUNCTION: Calc_GetOpResourceId

DESCRIPTION:  Return the string resource for the specified operator

PARAMETERS:
   op: Operator

RETURN VALUE:
   uint16: String resource Id for the operator

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint16  Calc_GetOpResourceId(OpType op)
{
    switch (op) {
    case OP_UNDEFINED:
       return 0;
    case OP_ADD:
       return IDC_ADD;
    case OP_SUB:
       return IDC_SUB;
    case OP_MUL:
       return IDC_MUL;
    case OP_DIV:
       return IDC_DIV;
    case OP_EQUAL:
       return IDC_EQUAL;
    }
    return 0;
}
/*================================================================
因系统库函数不能正确执行,所以改头换面,

重执行浮点数到双字节字符串的转换函数.

================================================================*/
static boolean Calc_FloatToWStr(double v, AECHAR * pdest, int nSize)
{

    char szBuff[32];
    AECHAR ch, *psz, *pszLastZero, *pszSrc, *pszDst;
    boolean bDot;

    if (!pdest || nSize <= 0)
        return FALSE;

    MEMSET( pdest, 0, nSize);
    if( v == 0)
    {
        pdest[0] = '0';
        return TRUE;
    }

    (void)snprintf( szBuff, 32,"%13.4f", v);
    (void)STRTOWSTR(szBuff, pdest, nSize);

    // Clip leading spaces...
    for (pszDst = pszSrc = pdest; ( ch = *pszSrc) == ' '; pszSrc++)
        ;

    if (pszSrc != pszDst)
    {
        while (1)
        { //lint !e716
            ch = *pszSrc;
            *pszDst = ch;
            if (!ch)
            {
                break;
            }
            pszDst++;
            pszSrc++;
        }
    }

    // Clip trailing zeros...
    for (bDot = FALSE, pszLastZero = NULL, psz = pdest; (ch = *psz) != 0; psz++)
    {

        if (ch == (AECHAR)'.')
        {
            bDot = TRUE;
        }
        else
        {
            if (bDot)
            {
                if (ch == (AECHAR)'0')
                {
                    if (!pszLastZero)
                    {
                        pszLastZero = psz;
                    }
                }
                else
                    pszLastZero = NULL;
            }
        }
    }

    // If the trailing zero followed the decimal point, clip them both...
    if (pszLastZero)
    {
        if (pszLastZero[-1] == (AECHAR)'.')
        {
            pszLastZero--;
        }
        *pszLastZero = 0;
    }
    return (TRUE);
}
