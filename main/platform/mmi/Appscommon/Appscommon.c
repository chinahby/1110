/*==============================================================================
//           A P P S   C O M M O N   F U N C T I O N S
// 文件：
//        Appscommon.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        本文件提供静态 Applet 用到的公共函数的实现。         
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------
==============================================================================*/


/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "Appscommon.h"

#include "AEE_OEM.h"
#include "AEE_OEMEvent.h"


// 字符串资源文件
#include "appscommon.brh"

// 图片资源文件
#include "appscommonimages.brh"
#include "CallApp.h"

#define APPSCOMMON_STR2KEY_INCLUDE
#include "Appscommon_Str2Key.h"
#if !defined( AEE_SIMULATOR)
#include "OEMConfig.h"
#include "OEMCFGI.h"
#endif//#if !defined( AEE_SIMULATOR)

#include "AEEGraphics.h"
#ifndef WIN32
#include "disp.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "OEMClassIDs.h"
#include "msg.h"
/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
const char GsmToUcs2[GSM_CHARACTER_SET_SIZE] =
{            /*+0x0        +0x1        +0x2        +0x3        +0x4        +0x5        +0x6        +0x7*/
/*0x00*/ (char)0x40, (char)0xa3, (char)0x24, (char)0xa5, (char)0xe8, (char)0xe9, (char)0xf9, (char)0xec,
/*0x08*/ (char)0xf2, (char)0xc7, (char)0x0a, (char)0xd8, (char)0xf8, (char)0x0d, (char)0xc5, (char)0xe5,
/*0x10*/ (char)0,    (char)0x5f, (char)0,    (char)0,    (char)0,    (char)0,    (char)0,    (char)0,
/*0x18*/ (char)0,    (char)0,    (char)0,    (char)0x20, (char)0xc6, (char)0xe6, (char)0xdf, (char)0xc9,
/*0x20*/ (char)0x20, (char)0x21, (char)0x22, (char)0x23, (char)0x24, (char)0x25, (char)0x26, (char)0x27,
/*0x28*/ (char)0x28, (char)0x29, (char)0x2a, (char)0x2b, (char)0x2c, (char)0x2d, (char)0x2e, (char)0x2f,
/*0x30*/ (char)0x30, (char)0x31, (char)0x32, (char)0x33, (char)0x34, (char)0x35, (char)0x36, (char)0x37,
/*0x37*/ (char)0x38, (char)0x39, (char)0x3a, (char)0x3b, (char)0x3c, (char)0x3d, (char)0x3e, (char)0x3f,
/*0x40*/ (char)0x40, (char)0x41, (char)0x42, (char)0x43, (char)0x44, (char)0x45, (char)0x46, (char)0x47,
/*0x48*/ (char)0x48, (char)0x49, (char)0x4a, (char)0x4b, (char)0x4c, (char)0x4d, (char)0x4e, (char)0x4f,
/*0x50*/ (char)0x50, (char)0x51, (char)0x52, (char)0x53, (char)0x54, (char)0x55, (char)0x56, (char)0x57,
/*0x58*/ (char)0x58, (char)0x59, (char)0x5a, (char)0xc4, (char)0xd6, (char)0xd1, (char)0xdc, (char)0x5f,
/*0x60*/ (char)0xbf, (char)0x61, (char)0x62, (char)0x63, (char)0x64, (char)0x65, (char)0x66, (char)0x67,
/*0x68*/ (char)0x68, (char)0x69, (char)0x6a, (char)0x6b, (char)0x6c, (char)0x6d, (char)0x6e, (char)0x6f,
/*0x70*/ (char)0x70, (char)0x71, (char)0x72, (char)0x73, (char)0x74, (char)0x75, (char)0x76, (char)0x77,
/*0x78*/ (char)0x78, (char)0x79, (char)0x7a, (char)0xe4, (char)0xf6, (char)0xf1, (char)0xfc, (char)0xe0
};

const AECHAR  Ucs2ToGsm[UCS2_TO_GSM_TABLE_SIZE] =
{           /*+0x0      +0x1        +0x2        +0x3        +0x4        +0x5        +0x6        +0x7*/
/*0x00*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x08*/    NON_GSM,    NON_GSM,    0x0a,       NON_GSM,    NON_GSM,    0x0d,       NON_GSM,    NON_GSM,
/*0x10*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x18*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x20*/    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
/*0x28*/    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
/*0x30*/    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
/*0x38*/    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
/*0x40*/    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
/*0x48*/    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
/*0x50*/    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
/*0x58*/    0x58,       0x59,       0x5a,       NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    0x5f,
/*0x60*/    NON_GSM,    0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
/*0x68*/    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
/*0x70*/    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
/*0x78*/    0x78,       0x79,       0x7a,       NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x80*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x88*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x90*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0x98*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0xa0*/    NON_GSM,    0x21,       NON_GSM,    0x01,       NON_GSM,       0x03,       NON_GSM,    NON_GSM,
/*0xa8*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0xb0*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,
/*0xb8*/    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    NON_GSM,    0x3f,
/*0xc0*/    0x41,       0x41,       0x41,       0x41,       0x5b,       0x0e,       0x1c,       0x43,
/*0xc8*/    0x45,       0x45,       0x45,       0x45,       0x49,       0x49,       0x49,       0x60,
/*0xd0*/    NON_GSM,    0x4e,       0x4f,       0x4f,       0x4f,       0x4f,       0x5c,       0x2a,
/*0xd8*/    0x0b,       0x55,       0x55,       0x55,       0x55,       0x59,       NON_GSM,    0x1e,
/*0xe0*/    0x61,       0x61,       0x61,       0x61,       0x7b,       0x0f,       0x1d,       0x63,
/*0xe8*/    0x65,       0x65,       0x65,       0x65,       0x69,       0x69,       0x69,       0x69,
/*0xf0*/    NON_GSM,    0x6e,       0x6f,       0x6f,       0x6f,       0x6f,       0x7c,       0x2f,
/*0xf8*/    0x0c,       0x06,       0x75,       0x75,       0x75,       0x79,       NON_GSM,    0x79
};

#ifdef FEATURE_LANG_HEBREW
// Ucs2ToHebrew 表使用说明: 
// 如要检索UNICODE字符 wChar 转换为 Hebrew 的值，请使用 Ucs2ToHebrew[wChar & 0x00FF]
// 特殊字符 0x203E 、0x00D7 不能使用此表。0x203E-->0xAF 、0x00D7-->0xAA 。 返回值 NON_HEB
// 表示无相应字符，转换失败
const AECHAR  Ucs2ToHebrew[HEBREW_CONVERT_TABLE_SIZE] =
{           /*+0x0      +0x1        +0x2        +0x3        +0x4        +0x5        +0x6        +0x7*/
/*0x00*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x08*/    NON_HEB,    NON_HEB,    0x0a,       NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x10*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    0xdf,
/*0x18*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x20*/    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
/*0x28*/    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
/*0x30*/    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
/*0x38*/    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
/*0x40*/    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
/*0x48*/    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
/*0x50*/    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
/*0x58*/    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
/*0x60*/    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
/*0x68*/    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
/*0x70*/    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
/*0x78*/    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       NON_HEB,
/*0x80*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x88*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x90*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0x98*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0xa0*/    0xa0,       NON_HEB,    0xa2,       0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
/*0xa8*/    0xa8,       0xa9,       NON_HEB,    0xab,       0xac,       0xad,       0xae,       NON_HEB,
/*0xb0*/    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
/*0xb8*/    0xb8,       0xb9,       NON_HEB,    0xbb,       0xbc,       0xbd,       0xbe,       NON_HEB,
/*0xc0*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0xc8*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0xd0*/    0xe0,       0xe1,       0xe2,       0xe3,       0xe4,       0xe5,       0xe6,       0xe7,
/*0xd8*/    0xe8,       0xe9,       0xea,       0xeb,       0xec,       0xed,       0xee,       0xef,
/*0xe0*/    0xf0,       0xf1,       0xf2,       0xf3,       0xf4,       0xf5,       0xf6,       0xf7,
/*0xe8*/    0xf8,       0xf9,       0xfa,       NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,
/*0xf0*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    0xba,
/*0xf8*/    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB,    NON_HEB
};

// HebrewToUcs2 表使用说明: 
// 如要检索无符号 char 字符 c 转换为 Hebrew UNICODE 的值，请使用 Ucs2ToHebrew[c] . 返回值为 0，表示转换
// 不成功
const AECHAR  HebrewToUcs2[HEBREW_CONVERT_TABLE_SIZE] =
{           /*+0x0      +0x1        +0x2        +0x3        +0x4        +0x5        +0x6        +0x7*/
/*0x00*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x08*/    0,          0,          0x0a,       0,          0,          0,          0,          0,
/*0x10*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x18*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x20*/    0x20,       0x21,       0x22,       0x23,       0x24,       0x25,       0x26,       0x27,
/*0x28*/    0x28,       0x29,       0x2a,       0x2b,       0x2c,       0x2d,       0x2e,       0x2f,
/*0x30*/    0x30,       0x31,       0x32,       0x33,       0x34,       0x35,       0x36,       0x37,
/*0x38*/    0x38,       0x39,       0x3a,       0x3b,       0x3c,       0x3d,       0x3e,       0x3f,
/*0x40*/    0x40,       0x41,       0x42,       0x43,       0x44,       0x45,       0x46,       0x47,
/*0x48*/    0x48,       0x49,       0x4a,       0x4b,       0x4c,       0x4d,       0x4e,       0x4f,
/*0x50*/    0x50,       0x51,       0x52,       0x53,       0x54,       0x55,       0x56,       0x57,
/*0x58*/    0x58,       0x59,       0x5a,       0x5b,       0x5c,       0x5d,       0x5e,       0x5f,
/*0x60*/    0x60,       0x61,       0x62,       0x63,       0x64,       0x65,       0x66,       0x67,
/*0x68*/    0x68,       0x69,       0x6a,       0x6b,       0x6c,       0x6d,       0x6e,       0x6f,
/*0x70*/    0x70,       0x71,       0x72,       0x73,       0x74,       0x75,       0x76,       0x77,
/*0x78*/    0x78,       0x79,       0x7a,       0x7b,       0x7c,       0x7d,       0x7e,       0,
/*0x80*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x88*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x90*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0x98*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0xa0*/    0xa0,       0,          0xa2,       0xa3,       0xa4,       0xa5,       0xa6,       0xa7,
/*0xa8*/    0xa8,       0xa9,       0xd7,       0xab,       0xac,       0xad,       0xae,       0x203e,
/*0xb0*/    0xb0,       0xb1,       0xb2,       0xb3,       0xb4,       0xb5,       0xb6,       0xb7,
/*0xb8*/    0xb8,       0xb9,       0xf7,       0xbb,       0xbc,       0xbd,       0xbe,       0,
/*0xc0*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0xc8*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0xd0*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0xd8*/    0,          0,          0,          0,          0,          0,          0,          0,
/*0xe0*/    0x05d0,     0x05d1,     0x05d2,     0x05d3,     0x05d4,     0x05d5,     0x05d6,     0x05d7,
/*0xe8*/    0x05d8,     0x05d9,     0x05da,     0x05db,     0x05dc,     0x05dd,     0x05de,     0x05df,
/*0xf0*/    0x05e0,     0x05e1,     0x05e2,     0x05e3,     0x05e4,     0x05e5,     0x05e6,     0x05e7,
/*0xf8*/    0x05e8,     0x05e9,     0x05ea,     0,          0,          0,          0,          0
};
#endif

/*==============================================================================
                                 
                                内部数据类型定义
                                 
==============================================================================*/

/*=============================================================================
                                函数申明
==============================================================================*/
static void ALKWSTRLOWER(AECHAR * pszDest);

/*==============================================================================
                                 
                                 本地全局变量定义
                                 
==============================================================================*/
#if !defined( AEE_SIMULATOR)
//static call_start_info_type call_start_info_call;
#endif

 static boolean gbArrowFlag = FALSE;
/*==============================================================================

                                 函数定义
                                 
==============================================================================*/

/*==============================================================================
函数:
    MenuAutoScroll

说明:
    实现按上下方向键时，菜单自动循环滚动功能。

参数:
    pMenu [in]：指向 IMenuCtl 接口的指针。
    eCode [in]：事件代码。
    wParam：事件参数

返回值:
    none

备注:

==============================================================================*/
void MenuAutoScroll(IMenuCtl *pMenu, AEEEvent eCode, uint16 wParam)
{
    // 此功能现可通过设置菜单控件属性实现，这里先屏蔽直至最后删除此函数
#if 0    
    static uint16 wSelItem = 0;
    int nCount;
    uint16 wLastItem, wFirstItem;

    if ((pMenu == NULL) || 
        ((wParam != AVK_UP) && (wParam != AVK_DOWN)))
    {
        return;
    }

    nCount = IMENUCTL_GetItemCount(pMenu);
    if (nCount <= 1)
    {
        return;
    }
    wLastItem  = IMENUCTL_GetItemID(pMenu,nCount - 1);
    wFirstItem = IMENUCTL_GetItemID(pMenu, 0);

    switch (eCode) 
    {
        case EVT_KEY_PRESS:
            wSelItem = IMENUCTL_GetSel(pMenu);
            break;
            
        case EVT_KEY_RELEASE:
            switch (wParam)
            {
                case AVK_UP:
                    if (wSelItem == wFirstItem)
                    {
                        IMENUCTL_SetSel(pMenu, wLastItem);
                        (void)IMENUCTL_Redraw(pMenu);
                    }
                    break;
                    
                case AVK_DOWN:
                    if (wSelItem == wLastItem)
                    {
                        IMENUCTL_SetSel(pMenu, wFirstItem);
                        (void)IMENUCTL_Redraw(pMenu);
                    }
                    break;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
#endif    
}
static boolean AppsCommon_GetTxtIDFromBarType(BottomBar_Param_type *pBTBarParam, 
      uint16 *pnLeftTxtID,
      uint16 *pnMidTxtID,
      uint16 *pnRightTxtID)
{
   BottomBar_e_Type    eBBarType;
   uint16      nResID_L = 0;// 左
   uint16      nResID_R = 0;// 右
   uint16      nResID_M = 0;// 中

   if(!pBTBarParam || !(pnLeftTxtID || pnMidTxtID || pnRightTxtID))
      return FALSE;

   eBBarType = pBTBarParam->eBBarType;

   if(eBBarType & BTBAR_SINGLE_MASK) {
      switch (eBBarType) {
           // -----返回
         case BTBAR_BACK:
            nResID_R = IDS_BACK;
            break;

            // -----取消
         case BTBAR_CANCEL:
            nResID_R = IDS_STRING_CANCEL;
            break;

		//Add By zzg 2012_10_31
		case BTBAR_END_CALL:
            nResID_R = IDS_END_CALL;
            break;
		//Add End

            // -----关闭
         case BTBAR_CLOSE:
            nResID_R = IDS_CLOSE;
            break;

            /*guoys add @2008.10.20 for no match dont use select*/    
         case BTBAR_DELETE:
#ifdef FEATURE_VERSION_W317A
			nResID_R = IDS_CLEAR_EX;
#else		 	
            nResID_R = IDS_DEL;
#endif
            break;

            // -----挂机: 
         case BTBAR_END:
            nResID_R = IDS_END;
            break;

            // ---------下一步
         case BTBAR_NEXT:
            nResID_R = IDS_NEXT;
            break;

            //ADD XIAO.CHENG
         case BTBAR_OK:
            nResID_L = IDS_OK;
            break; 
         
         case BTBAR_PROMPT_OK:
            nResID_M = IDS_OK;
            break;

            // -----紧急呼叫
         case BTBAR_SOS:
            nResID_R = IDS_SOS;
            break;

            // -----停止
         case BTBAR_STOP:
            nResID_R = IDS_STOP;
            break;
			
		case BTBAR_OPTION:
			nResID_L = IDS_OPTION;
            break;
		case BTBAR_SNOOZE:    
			nResID_L = IDS_SNOOZE;
            nResID_R = IDS_STOP;
            break;

         default:
            break;
      }
   }
   else if(eBBarType & BTBAR_DOUBLE_MASK) {
      switch (eBBarType) {
         /*jinqing add,2009/01/08*/
         case BTBAR_ACCEPT_REJECT:
            nResID_L = IDS_BLUETOOTH_ACCEPT;
            nResID_R = IDS_REJECT;  
            break;
		/*	
		//Add By zzg 2011_12_12	
		case BTBAR_LOCK_BACK:
			nResID_L = IDS_LOCK;
			nResID_R = IDS_BACK;	
			break;
		//Add End
		*/
		
         case BTBAR_ADD_BACK:
            nResID_L=IDS_ADD;
            nResID_R=IDS_BACK;
            break;

         case BTBAR_ANSWER_IGNORE:
            nResID_L = IDS_ANSWER;
            nResID_R = IDS_IGNORE;
            break;

         case BTBAR_ANSWER_REJECT:
            nResID_L = IDS_ANSWER;
#ifdef FEATURE_VERSION_C316
            nResID_R = IDS_END_CALL;
#else
            nResID_R = IDS_REJECT;
#endif
            break;

         case BTBAR_ANSWER_MUTE:
            nResID_L = IDS_ANSWER;
            nResID_R = IDS_MUTE;
            break;

		//Add By zzg 2012_10_31
		case BTBAR_ANSWER_SILENT:
            nResID_L = IDS_ANSWER;
            nResID_R = IDS_SILENT;
            break;
		//Add End


#ifdef FEATURE_VERSION_C316
         case BTBAR_OPTION_SILENT:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_MUTE;
            break;
#endif
         case BTBAR_NEXT_DEL:
            nResID_L = IDS_NEXT;
            nResID_R = IDS_DEL;
            break;

         case BTBAR_ANSWER_UNMUTE:
            nResID_L = IDS_ANSWER;
            nResID_R = IDS_UNMUTE;
            break;

         case BTBAR_BACK_DELETE:
            nResID_L = IDS_BACK;
            nResID_R = IDS_DEL;
            break;

            // 返回-----紧急呼叫: 
         case BTBAR_BACK_SOS:
            nResID_L = IDS_BACK;
            nResID_R = IDS_SOS;
            break;
         case BTBAR_MENU_SOS:
         	nResID_L = IDS_MENU;
            nResID_R = IDS_SOS;
         	break;
         case BTBAR_STOP_BACK:
            nResID_R = IDS_BACK; 
            nResID_L = IDS_STOP;
            break;     

         case BTBAR_CALL_BACK:
            nResID_L = IDS_CALL;
            nResID_R = IDS_BACK;  
            break;

            // 继续-----返回
         case BTBAR_CONTINU_BACK:
            nResID_L = IDS_CONTINU;
            nResID_R = IDS_BACK;
            break;

            // 继续-----删除
         case BTBAR_CONTINU_DELETE:
            nResID_L = IDS_CONTINU;
            nResID_R = IDS_DEL;
            break;

         case BTBAR_CREATE_BACK:
            nResID_L=IDS_CREATE;
            nResID_R=IDS_BACK;
            break;             

            // 删除---返回
         case BTBAR_EARSE_BACK:
            nResID_L = IDS_DEL;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_EXCHANGE_BACK:
            nResID_L = IDS_EXCHANGE;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_EXCHANGE_DELETE:
            nResID_L = IDS_EXCHANGE;
            nResID_R = IDS_DEL;  
            break;          

            // 编辑-----返回
         case BTBAR_EDIT_BACK:
            nResID_L = IDS_EDIT;
            nResID_R = IDS_BACK;
            break;

            // 查找-----返回
         case BTBAR_FIND_BACK:
            nResID_L = IDS_FIND;
            nResID_R = IDS_BACK;
            break;
         case BTBAR_GALLERY_BACK:
#ifdef FEATURE_VERSION_W317A
			nResID_L = IDS_FILE_MANAGER; 
            nResID_M = IDS_COMMON_HELP;
#else
            nResID_L = IDS_GALLERY;     
#endif
            nResID_R = IDS_BACK;
            break;              
            
         case BTBAR_LEFT_START_NULL:
            nResID_M = IDS_START;
            nResID_R = IDS_BACK;
            break;
                
            //菜单---电话本
         case BTBAR_MENU_CONTACTS:
            nResID_L = IDS_MENU;
			//#ifdef FEATURE_VERSION_C337
			//nResID_R = IDS_PHONE_BOOK;
			//#else
			#if defined(FEATURE_VERSION_K212)
			nResID_R = IDS_CONTANT_K212;
			#else
            nResID_R = IDS_STRING_CONTACTS;
			#endif
			//#endif
            break;
			
            
        case BTBAR_OK_SOS:
            nResID_L = IDS_OK;
			nResID_R = IDS_SOS;
			break;    
					
		case BTBAR_MENU_FRENDUO:
			nResID_L = IDS_MENU;
			nResID_R = IDS_FRENDUO_LIST;
			break;
		
		case BTBAR_FACEBOOK_CHAT:
			nResID_L = IDS_FACEBOOK;
			nResID_R = IDS_CHAT;
			break;
		
		case BTBAR_VIEWMORE_BACK:
            nResID_L = IDS_MORE;
			nResID_M = IDS_OK;
			nResID_R = IDS_BACK;
			break;		

            // 菜单-----返回
         case BTBAR_MENU_BACK:
            nResID_L = IDS_MENU;
            nResID_R = IDS_BACK;
            break;

            // 确定-----返回
         case BTBAR_OK_BACK:
            nResID_L = IDS_OK;
            nResID_R = IDS_BACK;
            break;
            
         case BTBAR_ACTIVATE_BACK:
            nResID_L = IDS_MIZONEACTIVATE;
            nResID_R = IDS_BACK;
            break; 

         case BTBAR_MIZONE_SELECT_BACK:
            nResID_L = IDS_SELECT;
            nResID_R = IDS_BACK;
            break; 

         case BTBAR_FUNZONE_BACK:
            nResID_L = IDS_FUNZONE;
            nResID_R = IDS_BACK;
            break; 
            // 确定-----取消
         case BTBAR_OK_CANCEL:
            nResID_L = IDS_OK;
            nResID_R = IDS_CANCEL;
            break;
                      
            // 确定-----删除
         case BTBAR_OK_DEL:
         case BTBAR_OK_DELETE:
            nResID_L = IDS_OK;
#ifdef FEATURE_VERSION_W317A
			nResID_R = IDS_CLEAR_EX;
#else
            nResID_R = IDS_DEL;
#endif
            break;

            // 选项-----返回
         case BTBAR_OPTION_BACK:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_OPTIONS_BACK:
            nResID_L = IDS_BAROPTIONS;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_OPTION_STOP:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_STOP;
            break;
            
            // 选项-----删除
         case BTBAR_OPTION_DELETE:
            nResID_L = IDS_OPTION;
#ifdef FEATURE_VERSION_W317A
			nResID_R = IDS_CLEAR_EX;
#else			
            nResID_R = IDS_DEL;
#endif
            break; 

         case BTBAR_OPTION_DEL:
            nResID_L = IDS_OPTION;
#ifdef FEATURE_VERSION_W317A
			nResID_R = IDS_CLEAR_EX;
#else			
            nResID_R = IDS_DEL;
#endif
            break;   

            //选项---挂机:
         case BTBAR_OPTION_ENDCALL:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_STRING_END_CALL;
            break;

         case BTBAR_OPTION_HANDSFREEON:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_HANDSFREE_ON;  
            break;

		//Add By zzg 2012_10_31		
		case BTBAR_OPTION_HANDS_FREE:
			nResID_L = IDS_OPTION;
            nResID_R = IDS_HANDS_FREE;  
            break;
		case BTBAR_OPTION_HANDS_HELD:
			nResID_L = IDS_OPTION;
            nResID_R = IDS_HANDS_HELD;  
            break;	
		//Add End

         case BTBAR_OPTION_NORMAL:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_HANDSFREE_OFF;  
            break;

         case BTBAR_OPTION_UP:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_UP;
            break;

         case BTBAR_PAUSE_BACK:
            nResID_L = IDS_PAUSE;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_PLAY_BACK:
            nResID_M = IDS_PLAY;            
            nResID_R = IDS_BACK;
            break;  
			
		 case BTBAR_0PTION_PLAY_BACK:
		 	nResID_L = IDS_OPTION;
		 	nResID_M = IDS_PLAY;            
            nResID_R = IDS_BACK;
		 	break;

         case BTBAR_PLAY_STOP:
            nResID_M = IDS_PLAY;            
            nResID_R = IDS_STOP;
            break; 

         case BTBAR_GALLERY_PLAY_BACK:
#ifndef FEATURE_VERSION_C337
#ifndef FEATURE_VERSION_IC241A_MMX
            nResID_L = IDS_GALLERY;
#endif
#endif
            nResID_M = IDS_PLAY;            
            nResID_R = IDS_BACK;
            break;

            // 阅读-----返回
         case BTBAR_READ_BACK:
            nResID_L = IDS_READ;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_RECORD_STOP:
            nResID_L = IDS_RECORD;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_REPLAY_BACK:
            nResID_L = IDS_REPLAY;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_RESET_BACK:
            nResID_L = IDS_RESET;
            nResID_R = IDS_BACK;
            break; 

         case BTBAR_RESUME_BACK:
            nResID_L = IDS_RESUME;
            nResID_R = IDS_BACK;  
            break;

         case BTBAR_RETRY_CANCEL:
            nResID_L = IDS_RETRY;
            nResID_R = IDS_STRING_CANCEL;
            break;

            // 保存-----返回
         case BTBAR_SAVE_BACK:
            nResID_L = IDS_SAVE;
            nResID_R = IDS_BACK;
            break;

            // 保存-----取消
         case BTBAR_SAVE_CANCEL:
            nResID_L = IDS_SAVE;
            nResID_R = IDS_BACK;
            break;            

            // 保存-----删除
         case BTBAR_SAVE_DELETE:
            nResID_L = IDS_SAVE;
#ifdef FEATURE_VERSION_W317A
			nResID_R = IDS_CLEAR_EX;
#else			
            nResID_R = IDS_DEL;
#endif
            break;

            // 选择-----返回
         case BTBAR_SELECT_BACK:
            nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
            break;
		case BTBAR_PRIVIEW_BACK:
			nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_MODIFY_BACK:
			nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_SELECT_PLAY_BACK:
			nResID_L = IDS_OK;	//SELECT;
			nResID_M = IDS_PLAY;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_SELECT_STOP_BACK:
			nResID_L = IDS_OK;	//SELECT;
			nResID_M = IDS_STOP;
            nResID_R = IDS_BACK;
		 	break;
            //选择--------删除
         case BTBAR_SELECT_DEL:
            nResID_L = IDS_OK; //IDS_SELECT;
            nResID_R = IDS_DEL;
            break;

         case BTBAR_SELECT_STOP:
            nResID_L = IDS_OK; //IDS_SELECT;
            nResID_R = IDS_STOP;
            break;

            // 发送---删除
         case BTBAR_SEND_EARSE:
            nResID_L = IDS_SEND;
            nResID_R = IDS_DEL;
            break;

            // 发送---返回
         case BTBAR_SEND_BACK:
            nResID_L = IDS_SEND;
            nResID_R = IDS_BACK;
            break;  
         
            
         case BTBAR_SNOOZE_STOP:
            nResID_L = IDS_SNOOZE;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_START_BACK:
            nResID_L = IDS_START;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_SNOOZE_DONE:
            nResID_L = IDS_SNOOZE;
            nResID_R = IDS_POP_DONE;
            break;

         case BTBAR_UNLOCK_SOS:
            nResID_R = IDS_STRING_UNLOCK;
            nResID_L = IDS_SOS;
            break;
#if (defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_IC241A_MMX))
		case BTBAR_UNLOCK_M:
            nResID_L = IDS_STRING_UNLOCK;
            break; 
#else
            
        case BTBAR_UNLOCK_M:
            nResID_M = IDS_STRING_UNLOCK;
            break;    
#endif			
            
        case BTBAR_UNLOCK_L:
            nResID_L= IDS_STRING_UNLOCK;
            break;
            
		case BTBAR_UNLOCK:
			nResID_R = IDS_STRING_UNLOCK;
			break;
        case BTBAR_LUNLOCK:
            nResID_L = IDS_STRING_UNLOCK;
            nResID_R = IDS_SOS;
            break;		
            
#ifdef FEATURE_SPORTS_APP
         case BTBAR_PAUSE_STOP:
            nResID_L = IDS_PAUSE;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_RESUME_STOP:
            nResID_L = IDS_RESUME;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_SPORT_STOP:
            nResID_L = IDS_SPORT;
            nResID_R = IDS_STOP;
            break;            
#endif

         case BTBAR_VIEW_SNOOZE:         
            nResID_L = IDS_VIEW;
            nResID_R = IDS_SNOOZE;
            break;

         case BTBAR_VIEW_STOP:         
            nResID_L = IDS_VIEW;
            nResID_R = IDS_STOP;
            break;
#ifdef FEATURE_FLEXI_STATIC_BREW_APP
    	case BTBAR_FNASRANI_FPORTAL:
    		nResID_L = IDS_FNASRANI;
    		nResID_R = IDS_FPORTAL;
    		break;

    	case BTBAR_FMUSLIM_FPORTAL:
    		nResID_L = IDS_FMUSLIM;
    		nResID_R = IDS_FPORTAL;
    		break;

    	case BTBAR_FGURU_FPORTAL:
    		nResID_L = IDS_FGURU;
    		nResID_R = IDS_FPORTAL;
    		break;
    		
    	case BTBAR_CONTACTS_FPORTAL:
			//#ifdef FEATURE_VERSION_C337
			//nResID_L = IDS_PHONE_BOOK;
			//#else
    		nResID_L = IDS_STRING_CONTACTS;
			//#endif
    		nResID_R = IDS_FPORTAL;
    		break;            
    	case BTBAR_OPTION_SAVE_BACK:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_SAVE;
            nResID_R = IDS_BACK;
            break;
#endif
    	case BTBAR_FMENU_CANTACT:
    		nResID_L = IDS_FMENU;
			//#ifdef FEATURE_VERSION_C337
			//nResID_R = IDS_PHONE_BOOK;
			//#else
    		nResID_R = IDS_STRING_CONTACTS;
			//#endif
    		break; 
    	case BTBAR_MENU_FMENU:
    		nResID_L = IDS_MENU;
    		nResID_R = IDS_FMENU;
    		break;
		case BTBAR_YES_NO:
            nResID_L = IDS_YES;
            nResID_R = IDS_NO;
            break;

		//Add By zzg 2012_11_09
		case BTBAR_VIEW_CANCEL:
			nResID_L = IDS_VIEW;
			nResID_R = IDS_CANCEL;
			break;
		case BTBAR_SEARCH_BACK:
			nResID_L = IDS_SEARCH;
			nResID_R = IDS_BACK;
		//Add End	
         default:
            break;
      }
   }
   else if(eBBarType & BTBAR_TRI_MASK) {
      switch (eBBarType) {
	  	 //Add By zzg 2012_12_10
	  	 case BTBAR_GALLERY_HELP_BACK:
            nResID_L = IDS_GALLERY;   
            nResID_M = IDS_COMMON_HELP;
            nResID_R = IDS_BACK;
			break;
	  	 //Add End
         case BTBAR_CANCEL_RESUME_NULL:
            nResID_L = IDS_RESET;
            nResID_M = IDS_RESUME;
            nResID_R = IDS_CANCEL;
            break;
            
         case BTBAR_FULLSCREEN_PAUSE_STOP:
		 	#ifndef FEATURE_VERSION_C337
            #ifndef FEATURE_VERSION_IC241A_MMX
            nResID_L = IDS_FULLSCREEN; 
            #endif
			#endif
            nResID_M = IDS_PAUSE;            
            nResID_R = IDS_STOP;
            break;  
            
         case BTBAR_JEWISH_TODAY_BACK:
            nResID_L = IDS_JEWISH;
            nResID_M = IDS_TOTODAY;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_OPTION_CREATE_BACK:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_CREATE;
            nResID_R = IDS_BACK;
            break;

            // for FM Radio list using
         case BTBAR_OPTION_EDIT_BACK:
            {
               nResID_L = IDS_OPTION_FM; 
               nResID_M = IDS_EDIT;
               nResID_R = IDS_FM_BACK; // IDS_BACK;
            }
            break;

         case BTBAR_OPTION_OK_BACK:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_OK;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_OPTION_OK_DEL:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_OK;
            nResID_R = IDS_DEL;
            break;

         case BTBAR_OPTION_PAUSE_STOP:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_PAUSE;
            nResID_R = IDS_STOP;
            break;
            
         case BTBAR_OPTION_PLAY_BACK:
            nResID_L=IDS_OPTION;
            nResID_M=IDS_PLAY;
            nResID_R=IDS_BACK;
            break;
            
         case BTBAR_OPTION_STOP_BACK:
            nResID_L=IDS_OPTION;
            nResID_M=IDS_STOP;
            nResID_R=IDS_BACK;
            break;

         case BTBAR_OPTION_ZOOM_BACK:
            nResID_L=IDS_OPTION;
            nResID_M=IDS_FULLSCREEN;
            nResID_R=IDS_BACK;
            break;
            
         case BTBAR_OPTION_PLAY_STOP:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_PLAY;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_OPTION_SAVE_DEL:
            nResID_L = IDS_OPTION;
            nResID_M = IDS_SAVE;
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_IC241A_MMX)
            nResID_R = IDS_CLEAR_EX;	
#else
            nResID_R = IDS_DEL;	
#endif
            break;
		case BTBAR_OPTION_SAVE_BACK:
			nResID_L = IDS_OPTION;
            nResID_M = IDS_SAVE;
            nResID_R = IDS_BACK;
            break;
            // option---今天---返回
         case BTBAR_OPTION_TODAY_BACK:
#ifdef FEATURE_VERSION_C260_IC19
            nResID_L = IDS_OPTION_EX;
#else
            nResID_L = IDS_OPTION;
#endif
            nResID_M = IDS_TOTODAY;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_SAVE_AM_BACK:
            nResID_L = IDS_SAVE;
            nResID_M = IDS_APPSCOMMON_AM;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_SAVE_IP_DELETE:
            nResID_L = IDS_SAVE;
            nResID_M = IDS_STRING_IP;
            nResID_R = IDS_DEL;
            break;

         case BTBAR_SAVE_PM_BACK:
            nResID_L = IDS_SAVE;
            nResID_M = IDS_APPSCOMMON_PM;
            nResID_R = IDS_BACK;
            break;

            // 选择---删除---返回
         case BTBAR_SELECT_DEL_BACK:
            nResID_L = IDS_SELECT; //IDS_SELECT;
            nResID_M = IDS_DEL; //IDS_DEL;
            nResID_R = IDS_BACK;
            break;

            // 选择---确定---返回
         case BTBAR_SELECT_OK_BACK:
            nResID_L = IDS_SELECT; //IDS_SELECT;
            nResID_M = IDS_OK; //IDS_OK;
            nResID_R = IDS_BACK;
            break;

            // 选择---确定---删除
         case BTBAR_SELECT_OK_DEL:
            nResID_L = IDS_SELECT; //IDS_SELECT;
            nResID_M = IDS_OK; //IDS_OK;
            nResID_R = IDS_DEL;
            break;      

            // 选择---设置---返回
         case BTBAR_SELECT_SET_BACK:
            nResID_L = IDS_SELECT; //IDS_SELECT;
            nResID_M = IDS_SETTING;
            nResID_R = IDS_BACK;
            break;

         case BTBAR_SNOOZE_CONTINUE_STOP:
            nResID_L = IDS_SNOOZE;
            nResID_M = IDS_CONTINU;
            nResID_R = IDS_STOP;
            break;

         case BTBAR_CONTINUE_RESET_BACK:
            nResID_L = IDS_CONTINU;
            nResID_M = IDS_RESET;
            nResID_R = IDS_BACK;
            break;
            
        case BTBAR_CALL_MODIFY_BACK:
            nResID_L = IDS_CALL;
            nResID_M = IDS_MODIFY;            
            nResID_R = IDS_BACK;  
            break;

        case BTBAR_YES_NO_CANCEL:
            nResID_L = IDS_YES;
            nResID_M = IDS_NO;
            nResID_R = IDS_CANCEL;
			break;
		case BTBAR_CONTINUE:
			nResID_L = IDS_CONTINUE;
			break;
         default:
            break;
      }
   }
   else {
      return FALSE;
   }

   if(pnLeftTxtID )
      *pnLeftTxtID = nResID_L;

   if(pnMidTxtID)
      *pnMidTxtID = nResID_M;

   if(pnRightTxtID)
      *pnRightTxtID = nResID_R;

   if(nResID_L != 0 || nResID_M != 0 || nResID_R != 0)
      return  TRUE;
   else
      return FALSE;

}//AppsCommon_GetTxtIDFromBarType

/*==============================================================================
函数:
    DrawBottomBar

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    BParam [in]: 底部提示条相关参数指针。

返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条，语言无关的底部提示图片，请自行绘制，不
    要调用此函数！同时本函数不负责界面的更新，界面的更新由调用者自行完成。最多
    支持底部有3个按钮提示。
==============================================================================*/
void DrawBottomBar(IDisplay  * pIDisplay, BottomBar_Param_type *BParam)
{
#ifdef FEATURE_FUNCS_BOTTOM_BAR
    IImage      *pBarImg = NULL;
    uint16      nResID_L = 0;// 左
    uint16      nResID_R = 0;// 右
    uint16      nResID_M = 0;// 中
    AEERect     rc;
    AECHAR      wszBar_L[20]={0};// 左
    AECHAR      wszBar_R[20]={0};// 右
    AECHAR      wszBar_M[20]={0};// 中
#ifdef FEATURE_FUNCS_THEME    
    RGBVAL      oldColor;
    Theme_Param_type Theme_Param;
#endif /* FEATURE_FUNCS_THEME */    
    IShell      *pShell = AEE_GetShell();
#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL  nBgColor = APPSCOMMON_BG_COLOR;
#endif
    
    if ((NULL == pShell) || (NULL == pIDisplay) || (NULL == BParam))
    {
        return;
    }

#ifdef FEATURE_FUNCS_THEME    
    // 获取主题参数
    Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */    
    
    // 确定底条的矩形
    if (NULL != BParam->prc)
    {
        rc = *(BParam->prc);
        /* 强制居中*/
        rc.x = 0;
        rc.dx = DISP_WIDTH;
    }
    else
    {
        AEEDeviceInfo devinfo;
        int nBarH = GetBottomBarHeight(pIDisplay);
        
        MEMSET(&devinfo, 0, sizeof(devinfo));
        ISHELL_GetDeviceInfo(pShell, &devinfo);
        SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
    }
   
    AppsCommon_GetTxtIDFromBarType(BParam, &nResID_L, &nResID_M, &nResID_R); 

   // 加载按钮文本
    if (nResID_L != 0)
    {
        (void) ISHELL_LoadResString(pShell,
                                    AEE_APPSCOMMONRES_LANGFILE,
                                    nResID_L,
                                    wszBar_L,
                                    sizeof(wszBar_L));
    }
    if (nResID_R != 0)
    {
        (void) ISHELL_LoadResString(pShell,
                                    AEE_APPSCOMMONRES_LANGFILE,
                                    nResID_R,
                                    wszBar_R,
                                    sizeof(wszBar_R));
    }
    if (nResID_M != 0)
    {
        (void) ISHELL_LoadResString(pShell,
                                    AEE_APPSCOMMONRES_LANGFILE,
                                    nResID_M,
                                    wszBar_M,
                                    sizeof(wszBar_M));
    }

//Del By zzg 2010_07_14
/*	
#ifdef FEATURE_RANDOM_MENU_COLOR
    if(SUCCESS == OEM_GetConfig(CFGI_MENU_BGCOLOR, &nBgColor, sizeof(nBgColor)))
    {
        int i, nColorHeight = 14;
        AEERect rect;
        uint32 nRVal = GET_RGB_R(nBgColor),
                 nGVal = GET_RGB_G(nBgColor),
                 nBVal = GET_RGB_B(nBgColor);

        SETAEERECT(&rect, 0, rc.y + nColorHeight, DISP_WIDTH, rc.dy - nColorHeight);

        IDISPLAY_FillRect(pIDisplay, &rect, nBgColor);
        rect.y = rc.y + nColorHeight - 1;
        rect.dy = 1;

        for(i = 0; i < nColorHeight; i++, rect.y--)
        {
            nRVal = MIN(255, (nRVal+8));
            nGVal = MIN(255, (nGVal+8));
            nBVal = MIN(255, (nBVal+8));
            nBgColor = MAKE_RGB(nRVal, nGVal, nBVal);
            IDISPLAY_FillRect(pIDisplay, &rect, nBgColor);
        }
    }
    // 获取颜色不成功时才加载图片
    else
#endif// FEATURE_RANDOM_MENU_COLOR
*/
    {
        #ifndef FEATURE_VERSION_SKY
        // 加载底条背景图片
        if ((STRLEN(BParam->strImgResFile) > 0) && 
            (BParam->nImgResID != 0))
        {// 用户传入参数优先
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          BParam->strImgResFile, 
                                          BParam->nImgResID);
        }
        #else
        if (BParam->nImgResID != 0)
        {// 用户传入参数优先
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          AEE_APPSCOMMONRES_IMAGESFILE, 
                                          BParam->nImgResID);
        }
        #endif
#ifdef FEATURE_FUNCS_THEME

        else if ((STRLEN(Theme_Param.strBBarImgResFile) > 0) && 
                 (0 != Theme_Param.nBBarImgResID))
        {// 主题参数次之
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          Theme_Param.strBBarImgResFile, 
                                          Theme_Param.nBBarImgResID);
        }
         else
        {// 主题参数次之
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          AEE_APPSCOMMONRES_IMAGESFILE, 
                                          IDI_BOTTOMBAR);
        }
#else
        else
        {
            // Add by pyuangui 20121221
            #ifdef FEATURE_VERSION_W317A
            if(AEE_Active()==AEECLSID_MAIN_MENU)
            {
               pBarImg = ISHELL_LoadResImage(pShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,
                                          IDI_MAINMENU_BOTTOMBAR);
            }
            else
            #endif
            {
              pBarImg = ISHELL_LoadResImage(pShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,
                                          IDI_BOTTOMBAR);
            }
        }
#endif /* FEATURE_FUNCS_THEME */   

        // 绘制底部提示条
        if (NULL != pBarImg)
        {
            IIMAGE_Draw(pBarImg, 0, rc.y);
            
            IIMAGE_Release(pBarImg);
            pBarImg = NULL;
        } 

#ifdef FEATURE_FUNCS_THEME
        else
        {
            IDISPLAY_FillRect(pIDisplay, &rc, Theme_Param.themeColor);
        }
#endif
    }

#ifdef FEATURE_FUNCS_THEME
    oldColor = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, Theme_Param.textColor);
#else

    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);

#endif /* FEATURE_FUNCS_THEME */    
    
#if !defined( FEATURE_CARRIER_ISRAEL_PELEPHONE)  // modify the code on 080922
    rc.x+=5;
    rc.dx-=10;
#endif
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
    // 绘制文本-左键
    if (WSTRLEN(wszBar_L)>0)
    {
        (void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    wszBar_L, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
    }
    
    // 绘制文本-右键
    if (WSTRLEN(wszBar_R)>0)
    {
        (void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    wszBar_R, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    IDF_ALIGN_BOTTOM | IDF_ALIGN_RIGHT | IDF_TEXT_TRANSPARENT);
    }
   
    // 绘制文本-中间键
    if (WSTRLEN(wszBar_M)>0)
    {
#ifdef FEATURE_FUNCS_THEME
        (void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    wszBar_M, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    IDF_ALIGN_BOTTOM | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
#else
/*
        rc.y -= 2;
        (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
        DrawTextWithProfile(pShell, 
                            pIDisplay, 
                            RGB_WHITE_NO_TRANS, 
                            AEE_FONT_BOLD, 
                            wszBar_M, 
                            -1, 
                            0, 
                            0, 
                            &rc, 
                            IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);
        (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
        rc.y += 2;
        */  //modi by yangdecai
        
         // add by pyuangui 20121231
         #ifdef FEATURE_VERSION_W317A
         if(BParam->eBBarType == BTBAR_GALLERY_BACK)
          {
            if (WSTRLEN(wszBar_M)>0)
            	{
      	  	(void) IDISPLAY_DrawText(pIDisplay, 
                          AEE_FONT_BOLD, 
                          wszBar_M, 
                          -1, 
                          95, 
                          0, 
                          &rc, 
                          IDF_ALIGN_BOTTOM | IDF_TEXT_TRANSPARENT);
            	}
      	  }
		  else
		  #endif	
		  //Add End
		  {
            (void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    wszBar_M, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    IDF_ALIGN_BOTTOM | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
		  }
#endif
    }

    if (gbArrowFlag)
    {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE 
        // 主题参数次之
#ifdef FEATURE_WHITE_BG
        pBarImg = ISHELL_LoadResImage(pShell, 
                                      AEE_APPSCOMMONRES_IMAGESFILE,
                                      IDB_EDIT_UP_DOWN_BMP_BLACK);
#else
        pBarImg = ISHELL_LoadResImage(pShell, 
                                      AEE_APPSCOMMONRES_IMAGESFILE,
                                      IDB_EDIT_UP_DOWN_BMP);
#endif

        // 绘制底部提示条的上下箭头
        if (NULL != pBarImg)
        {
            #ifdef FEATURE_DISP_176X220
            IIMAGE_Draw(pBarImg, 85, 182);
            #else
            IIMAGE_Draw(pBarImg, 61, 117);
            #endif
            IIMAGE_Release(pBarImg);
            pBarImg = NULL;
        }
#else
    	wszBar_M[0] = 0x2195;
   	    wszBar_M[1] = 0;
    	(void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    wszBar_M, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    IDF_ALIGN_BOTTOM | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
#endif
    }    
    
#ifdef FEATURE_FUNCS_THEME    
    // 恢复显示文本颜色
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, oldColor);
#else
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif /* FEATURE_FUNCS_THEME */    
#endif /* FEATURE_FUNCS_BOTTOM_BAR */
	IDisplay_Update(pIDisplay);
}   


//Add By zzg 2011_09_08
/*==============================================================================

函数:
    DrawBottomBg

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
  
返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条背景图。
==============================================================================*/
void DrawBottomBg(IDisplay  * pIDisplay)
{
#ifdef FEATURE_FUNCS_BOTTOM_BAR
    IImage      *pBarImg = NULL;    
    AEERect     rc;
    
#ifdef FEATURE_FUNCS_THEME    
    RGBVAL      oldColor;
    Theme_Param_type Theme_Param;
#endif /* FEATURE_FUNCS_THEME */    

    IShell      *pShell = AEE_GetShell();

#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL  nBgColor = APPSCOMMON_BG_COLOR;
#endif

	AEEDeviceInfo devinfo;
    int nBarH = GetBottomBarHeight(pIDisplay);    
	
    if ((NULL == pShell) || (NULL == pIDisplay))
    {
        return;
    }

#ifdef FEATURE_FUNCS_THEME    
    // 获取主题参数
    Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */    

	MEMSET(&devinfo, 0, sizeof(devinfo));
    ISHELL_GetDeviceInfo(pShell, &devinfo);
    SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

    #ifdef FEATURE_VERSION_SKY
    pBarImg = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BOTTOMBARBROWSER);
    #else
	pBarImg = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_BOTTOMBAR);
    #endif
   
    // 绘制底部提示条
    if (NULL != pBarImg)
    {
        IIMAGE_Draw(pBarImg, 0, rc.y);            
        IIMAGE_Release(pBarImg);
        pBarImg = NULL;		
    } 

#ifdef FEATURE_FUNCS_THEME
    else
    {
        IDISPLAY_FillRect(pIDisplay, &rc, Theme_Param.themeColor);
    }
#endif
     
#endif /* FEATURE_FUNCS_BOTTOM_BAR */
	IDisplay_Update(pIDisplay);
}


/*==============================================================================
函数:
    DrawBottomText

说明:
    本函数用于画与语言相关的底部提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    pText: 底部提示条相关文本。
    type:   底部左中右类型。

返回值:
    none

备注:
    本函数用于画与语言相关的底部提示条文本。
==============================================================================*/
void DrawBottomText(IDisplay  * pIDisplay,	char* pText, int type)
{
#ifdef FEATURE_FUNCS_BOTTOM_BAR
    IImage      *pBarImg = NULL;    
    AEERect     rc;
    AECHAR      wszBar[20]={0};// 左    
    uint32		flags;
    
#ifdef FEATURE_FUNCS_THEME    
    RGBVAL      oldColor;
    Theme_Param_type Theme_Param;
#endif /* FEATURE_FUNCS_THEME */    

    IShell      *pShell = AEE_GetShell();

#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL  nBgColor = APPSCOMMON_BG_COLOR;
#endif

	AEEDeviceInfo devinfo;
    int nBarH = GetBottomBarHeight(pIDisplay);
    	    
    if ((NULL == pShell) || (NULL == pIDisplay))
    {
        return;
    }

	STRTOWSTR(pText, wszBar, sizeof(wszBar));
	

#ifdef FEATURE_FUNCS_THEME    
    // 获取主题参数
    Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */    

	MEMSET(&devinfo, 0, sizeof(devinfo));
    ISHELL_GetDeviceInfo(pShell, &devinfo);
    SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

	flags = IDF_ALIGN_BOTTOM | IDF_TEXT_TRANSPARENT;

	 // 加载按钮文本
    switch(type)
    {
		case SOFTKEY_LEFT:
		{	
			flags |= IDF_ALIGN_LEFT;
			break;
		}
		case SOFTKEY_CENTER:
		{	
			flags |= IDF_ALIGN_CENTER;
			break;
		}
		case SOFTKEY_RIGHT:
		{			
			flags |= IDF_ALIGN_RIGHT;
			break;
		}
		default :
		{
			break;
		}
	}  

#ifdef FEATURE_FUNCS_THEME
    oldColor = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, Theme_Param.textColor);
#else

    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);

#endif /* FEATURE_FUNCS_THEME */    
    
#if !defined( FEATURE_CARRIER_ISRAEL_PELEPHONE)  // modify the code on 080922
    rc.x+=5;
    rc.dx-=10;
#endif
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);	

	// 绘制文本
    if (WSTRLEN(wszBar)>0)
    {
        (void) IDISPLAY_DrawText(pIDisplay, 
			                    AEE_FONT_BOLD, 
			                    wszBar, 
			                    -1, 
			                    0, 
			                    0, 
			                    &rc, 
			                    flags);
    } 

    
#ifdef FEATURE_FUNCS_THEME    
    // 恢复显示文本颜色
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, oldColor);
#else
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif /* FEATURE_FUNCS_THEME */    
#endif /* FEATURE_FUNCS_BOTTOM_BAR */
	IDisplay_Update(pIDisplay);
}   
//Add End


/*==============================================================================
函数:
    DrawTitleBar

说明:
    本函数用于画标题提示条。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。
    TParam [in]: 标题条相关参数指针。

返回值:
    none

备注:
    本函数用于?晏馓酰崾咎跬计醋苑亲试次募匦胂略氐绞只；嬷频谋晏馕谋?
    可由参数传入。同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawTitleBar(IDisplay  * pIDisplay, TitleBar_Param_type *TParam)
{
#ifdef FEATURE_FUNCS_TITLE_BAR
    AECHAR      *pwszTitle = NULL;
    IImage      *pBarImg = NULL;
    AEERect     rc;
    uint32      dwFlags;
#ifdef FEATURE_FUNCS_THEME    
    Theme_Param_type    Theme_Param;
#endif /* FEATURE_FUNCS_THEME */    
    IShell  *pShell = AEE_GetShell();
#ifdef FEATURE_RANDOM_MENU_COLOR
    RGBVAL  nBgColor = APPSCOMMON_BG_COLOR;
#endif
    
    if ((NULL == pShell) || (NULL == pIDisplay) || (NULL == TParam))
    {
        return;
    }

#ifdef FEATURE_FUNCS_THEME    
    // 获取主题参数
    Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */    
    
    // 确定标题条的矩形
    if (NULL != TParam->prc)
    {
        rc = *(TParam->prc);
        /* 强制居中*/
        rc.x = 0;
        rc.dx = DISP_WIDTH;
    }
    else
    {
        AEEDeviceInfo devinfo;
        int     nBarH = GetTitleBarHeight(pIDisplay);
        
        MEMSET(&devinfo, 0, sizeof(devinfo));
        ISHELL_GetDeviceInfo(pShell, &devinfo);
        SETAEERECT(&rc, 0, 0, devinfo.cxScreen, nBarH);
    }

//Del By zzg 2010_07_14
/*
#ifdef FEATURE_RANDOM_MENU_COLOR
    if(SUCCESS == OEM_GetConfig(CFGI_MENU_BGCOLOR, &nBgColor, sizeof(nBgColor)))
    {
        int i, nColorHeight = 19;
        AEERect rect;
        uint32 nRVal = GET_RGB_R(nBgColor),
                 nGVal = GET_RGB_G(nBgColor),
                 nBVal = GET_RGB_B(nBgColor);

        SETAEERECT(&rect, 0, rc.y + nColorHeight, DISP_WIDTH, rc.dy - nColorHeight);

        IDISPLAY_FillRect(pIDisplay, &rect, nBgColor);
        rect.y = rc.y + nColorHeight - 1;
        rect.dy = 1;

        for(i = 0; i < nColorHeight; i++, rect.y--)
        {
            nRVal = MIN(255, (nRVal+6));
            nGVal = MIN(255, (nGVal+6));
            nBVal = MIN(255, (nBVal+6));
            nBgColor = MAKE_RGB(nRVal, nGVal, nBVal);
            IDISPLAY_FillRect(pIDisplay, &rect, nBgColor);
        }
    }
    // 获取颜色不成功时才加载图片
    else
#endif// FEATURE_RANDOM_MENU_COLOR
*/
    {
        if ((STRLEN(TParam->strImgResFile) > 0) && 
            (0 != TParam->nImgResID))
        {// 用户传入参数优先
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          TParam->strImgResFile, 
                                          TParam->nImgResID);
        }
#ifdef FEATURE_FUNCS_THEME

        else if ((STRLEN(Theme_Param.strTBarImgResFile) > 0) && 
                 (0 != Theme_Param.nTBarImgResID))
        {// 主题参数次之
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          Theme_Param.strBBarImgResFile, 
                                          Theme_Param.nBBarImgResID);
        }
         else
        {// 主题参数次之
            pBarImg = ISHELL_LoadResImage(pShell, 
                                          AEE_APPSCOMMONRES_IMAGESFILE, 
                                          IDI_TITLEBAR);
        }
#else
        else
        {
            #ifndef FEATURE_VERSION_K212_HUALU
            //Add by pyuangui 20121221
            #ifdef FEATURE_VERSION_W317A
            if(AEE_Active()==AEECLSID_MAIN_MENU)
            {
               pBarImg = ISHELL_LoadResImage(pShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,
                                          IDI_MAINMENU_TITLEBAR);
            }
            else
            #endif //Add End
            {
               pBarImg = ISHELL_LoadResImage(pShell,
                                          AEE_APPSCOMMONRES_IMAGESFILE,
                                          IDI_TITLEBAR);
            }
            #endif
        }
#endif /* FEATURE_FUNCS_THEME */    
//IDISPLAY_UpdateEx(pIDisplay, FALSE);//wlh
        // 绘制标题条背景
        #ifndef FEATURE_VERSION_K212_HUALU
        if (NULL != pBarImg)
        {
            IIMAGE_Draw(pBarImg, 0, rc.y);
            IIMAGE_Release(pBarImg);
            pBarImg = NULL;
        }
        #else
        {
            AEERect bgRect;
            bgRect.y = rc.y;
            bgRect.dy = STATEBAR_HEIGHT;  
            bgRect.x = 0;
            bgRect.dx = 320;
            if(AEE_Active()==AEECLSID_MAIN_MENU)
            {
                 #ifdef FEATURE_VERSION_K212_VHOPE
                 IDISPLAY_DrawRect(pIDisplay,
                              &bgRect,
                              RGB_NONE,
                              MAKE_RGB(0,0,0),
                              IDF_RECT_FILL);
                 #else
                 IDISPLAY_DrawRect(pIDisplay,
                              &bgRect,
                              RGB_NONE,
                              MAKE_RGB(255,255,255),
                              IDF_RECT_FILL);
                 #endif
            }
            else
            {
                IDISPLAY_DrawRect(pIDisplay,
                              &bgRect,
                              RGB_NONE,
                              MAKE_RGB(0,0,0),
                              IDF_RECT_FILL);
            }
        }
        #endif
//IDISPLAY_UpdateEx(pIDisplay, FALSE);//wlh 
#ifdef FEATURE_FUNCS_THEME    
        else
        {
            IDISPLAY_FillRect(pIDisplay, &rc, Theme_Param.themeColor);
        }
#endif /* FEATURE_FUNCS_THEME */
    }

    /*绘制左右两个箭头*/
    if(TParam->eTBarType == TBAR_ARROW)
    {
        IImage *pArrow = NULL;
        AEEImageInfo iInfo = {0};

        pArrow = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_LEFTARROW);
        if(pArrow != NULL)
        {
            IIMAGE_GetInfo(pArrow, &iInfo);
            IImage_Draw(pArrow, 5, (rc.dy - iInfo.cy)/2);
            IImage_Release(pArrow);
            pArrow = NULL;
        }
        pArrow = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_RIGHTARROW);
        if(pArrow != NULL)
        {
            IIMAGE_GetInfo(pArrow, &iInfo);
            IImage_Draw(pArrow, rc.dx - 5 - iInfo.cx, (rc.dy - iInfo.cy)/2);
            IImage_Release(pArrow);
            pArrow = NULL;
        }
    }
//IDISPLAY_UpdateEx(pIDisplay, FALSE);//wlh 
    // 绘制标题条文本
    if (NULL != TParam->pwszTitle)
    {
        pwszTitle = TParam->pwszTitle;
    }
    else if ((TParam->strTitleResFile != NULL) && (0 != TParam->nTitleResID))
    {
        int32  dwSize = 0;
        
        dwSize = 64 * sizeof(AECHAR);
        pwszTitle = (AECHAR *)MALLOC(dwSize);
        if (NULL != pwszTitle)
        {
            (void)ISHELL_LoadResString(pShell, 
                                       TParam->strTitleResFile, 
                                       TParam->nTitleResID, 
                                       pwszTitle, 
                                       dwSize);
        }
    }
    if (NULL != pwszTitle)
    {
#ifdef FEATURE_FUNCS_THEME    
        RGBVAL oldColor = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, Theme_Param.textColor);
#else
        #if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K212_HUALU)
        if(AEE_Active()==AEECLSID_MAIN_MENU)
        {
          #ifdef FEATURE_VERSION_K212_VHOPE
          IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
          #else
          IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
          #endif
        }
        else
        #endif

		#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_IC241A_MMX)
		{
#ifdef FEATURE_VERSION_IN50_MMX
            IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
#else
            IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif
        }
		#else
		{
          IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
        }
		#endif       
#endif /* FEATURE_FUNCS_THEME */    
        
        dwFlags = TParam->dwAlignFlags | 
                  IDF_TEXT_TRANSPARENT |
                  IDF_ALIGN_MIDDLE;

        // 绘制标题文本
        (void) IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_NORMAL, 
                    pwszTitle, 
                    -1, 
                    0, 
                    0, 
                    &rc, 
                    dwFlags);
       
        if (NULL == TParam->pwszTitle)
        {
            FREE(pwszTitle);
        }
        
#ifdef FEATURE_FUNCS_THEME    
        // 恢复显示文本颜色
        (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, oldColor);
#else
        (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif /* FEATURE_FUNCS_THEME */    
    }
#endif /* FEATURE_FUNCS_TITLE_BAR */
//IDISPLAY_UpdateEx(pIDisplay, FALSE);//wlh 
}
/*==============================================================================
函数:
    DrawBackground

说明:
    本函数用于画控件背景图。

参数:
    pDisplay   [in]：IDisplay指针。
    pRect      [in]：目标区域大小。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void drawTheImage( IImage *image, AEERect *pRect)
{

    IIMAGE_SetOffset( image, pRect->x, pRect->y);
    IIMAGE_SetDrawSize( image, pRect->dx, pRect->dy);
    IIMAGE_Draw( image, pRect->x, pRect->y);
    IIMAGE_Release( image);
}

void DrawBackground( IDisplay *pDisplay, AEERect *pRect)
{   
	#ifdef FEATURE_LOW_MEM_BIGFONT
	IImage *image = NULL;
	#else
    IImage *image = ISHELL_LoadResImage( AEE_GetShell(),
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_BACKGROUND
                           );
	#endif
    if( image == NULL)
    {
        MSG_FATAL( ";DrawBackground, load wall paper failed",0,0,0);
    }
    else
    {
        drawTheImage( image, pRect);
    }
}

/*==============================================================================
函数:
    DrawGreyBitTextWithProfile

说明:
    函数用于绘制带轮廓的文本。

参数:
    pShell [in]：IShell 接口指针。
    pDisplay [in]：IDisplay 接口指针。
    ProfileColor [in]：文本轮廓的RGBVAL颜色值。
    参数 Font、pcText、nChars、x、y、prcBackground、dwFlags 含义同接口函数
    IDISPLAY_DrawText 。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
extern int GreyBitBrewFont_DrawText(IDisplay *p, int nSize, const AECHAR *psz, int nl, int x, int y, const AEERect *prcb, uint32 flags);

void DrawGreyBitTextWithProfile(IShell* pShell, 
    IDisplay * pDisplay, 
    RGBVAL ProfileColor, 
    int Font, 
    const AECHAR * pcText, 
    int nChars,
    int x, 
    int y, 
    const AEERect * prcBackground, 
    uint32 dwFlags
)
{
#ifndef FEATURE_LOW_MEM_BIGFONT
	RGBVAL oldTextClr;
	AEERect ClipRc;
	AEERect rc;
	AEEDeviceInfo di;

	// 满足下列任意条件程序应立即返回
	if ((NULL == pDisplay) || 
		(NULL == pcText) || 
		(NULL == pShell))
	{
		return;
	}

	

	#if 1
	// 求绘制文本时的剪切矩形
	ISHELL_GetDeviceInfo(pShell, &di);
	ClipRc.x = 0;
	ClipRc.y = 0;
	ClipRc.dx = di.cxScreen;
	ClipRc.dy = di.cyScreen;
	if (NULL != prcBackground)	
	{
		ClipRc = *prcBackground;
	}
	
	rc = ClipRc;
#ifdef FEATURE_VERSION_K202
	Font = 16;
#endif
	// 绘制文本的边
	if (dwFlags & IDF_ALIGN_RIGHT)
	{
		rc.dx = ClipRc.dx - 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
					
		rc.dx = ClipRc.dx + 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
	}
	else if ( (dwFlags & IDF_ALIGN_LEFT) || (dwFlags & IDF_ALIGN_CENTER) )
	{
		rc.x = ClipRc.x - 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
		rc.x = ClipRc.x + 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
	}
	else
	{
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x-1, 
					y, 
					NULL, 
					dwFlags);
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x+1, 
					y, 
					NULL, 
					dwFlags);
	}
	
	rc = (prcBackground ? *prcBackground : ClipRc); 
	
	if (dwFlags & IDF_ALIGN_BOTTOM)
	{
		rc.dy = ClipRc.dy - 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc,
					dwFlags);
		rc.dy = ClipRc.dy + 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
	}
	else if ( (dwFlags & IDF_ALIGN_TOP) || (dwFlags & IDF_ALIGN_MIDDLE) )
	{
		rc.y = ClipRc.y - 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
		rc.y = ClipRc.y + 1;
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y, 
					&rc, 
					dwFlags);
	}
	else
	{
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y - 1, 
					NULL, 
					dwFlags);
		(void) GreyBitBrewFont_DrawText(pDisplay, 
					Font, 
					pcText, 
					nChars, 
					x, 
					y + 1, 
					NULL, 
					dwFlags);
	}
	#else
	// 绘制文本
	rc.dx = prcBackground->dx;
	rc.dy = prcBackground->dy;
	rc.x = prcBackground->x-1;
	rc.y = prcBackground->y-1;
	(void) GreyBitBrewFont_DrawText(pDisplay, 
				Font+2, 
				pcText, 
				nChars, 
				x, 
				y, 
				&rc,//prcBackground, 
				dwFlags);
	#endif
	// 设置文本描边时边的颜色，同时保存原来文本颜色值
	oldTextClr = IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, ProfileColor);
	
	
	// 绘制文本
	(void) GreyBitBrewFont_DrawText(pDisplay, 
				Font, 
				pcText, 
				nChars, 
				x, 
				y, 
				prcBackground, 
				dwFlags);
	// 恢复初始文本颜色
	(void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, oldTextClr);
	#endif
}

/*==============================================================================
函数:
    DrawTextWithProfile

说明:
    函数用于绘制带轮廓的文本。

参数:
    pShell [in]：IShell 接口指针。
    pDisplay [in]：IDisplay 接口指针。
    ProfileColor [in]：文本轮廓的RGBVAL颜色值。
    参数 Font、pcText、nChars、x、y、prcBackground、dwFlags 含义同接口函数
    IDISPLAY_DrawText 。

返回值:
    none

备注:
    同时本函数不负责界面的更新，界面的更新由调用者自行完成。
==============================================================================*/
void DrawTextWithProfile(IShell* pShell, 
    IDisplay * pDisplay, 
    RGBVAL ProfileColor, 
    AEEFont Font, 
    const AECHAR * pcText, 
    int nChars,
    int x, 
    int y, 
    const AEERect * prcBackground, 
    uint32 dwFlags
)
{
    RGBVAL oldTextClr;
    AEERect ClipRc;
    AEERect rc;
    AEEDeviceInfo di;

    // 满足下列任意条件程序应立即返回
    if ((NULL == pDisplay) || 
        (NULL == pcText) || 
        (NULL == pShell))
    {
        return;
    }

	
    // 设置文本描边时边的颜色，同时保存原来文本颜色值
    oldTextClr = IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, ~ProfileColor);
	
    // 求绘制文本时的剪切矩形
    ISHELL_GetDeviceInfo(pShell, &di);
    ClipRc.x = 0;
    ClipRc.y = 0;
    ClipRc.dx = di.cxScreen;
    ClipRc.dy = di.cyScreen;
    if (NULL != prcBackground)  
    {
        ClipRc = *prcBackground;
    }
    
    rc = ClipRc;
    
    // 绘制文本的边
    if (dwFlags & IDF_ALIGN_RIGHT)
    {
        rc.dx = ClipRc.dx - 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
                    
        rc.dx = ClipRc.dx + 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
    }
    else if ( (dwFlags & IDF_ALIGN_LEFT) || (dwFlags & IDF_ALIGN_CENTER) )
    {
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
        rc.x = ClipRc.x - 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
        rc.x = ClipRc.x + 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
    }
    else
    {
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x-1, 
                    y, 
                    NULL, 
                    dwFlags);
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x+1, 
                    y, 
                    NULL, 
                    dwFlags);
    }
    
    rc = (prcBackground ? *prcBackground : ClipRc); 
    
    if (dwFlags & IDF_ALIGN_BOTTOM)
    {
        rc.dy = ClipRc.dy - 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc,
                    dwFlags);
        rc.dy = ClipRc.dy + 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
    }
    else if ( (dwFlags & IDF_ALIGN_TOP) || (dwFlags & IDF_ALIGN_MIDDLE) )
    {
        rc.y = ClipRc.y - 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
        rc.y = ClipRc.y + 1;
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y, 
                    &rc, 
                    dwFlags);
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
    }
    else
    {
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y - 1, 
                    NULL, 
                    dwFlags);
        (void) IDISPLAY_DrawText(pDisplay, 
                    Font, 
                    pcText, 
                    nChars, 
                    x, 
                    y + 1, 
                    NULL, 
                    dwFlags);
        MSG_FATAL("rc.dx=%d", rc.dx,0,0);
    }
	
	(void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, ProfileColor);
    // 绘制文本
    (void) IDISPLAY_DrawText(pDisplay, 
                Font, 
                pcText, 
                nChars, 
                x, 
                y, 
                prcBackground, 
                dwFlags);
	// 恢复初始文本颜色
    (void)IDISPLAY_SetColor(pDisplay, CLR_USER_TEXT, oldTextClr);

    IDisplay_Update(pDisplay);      //Add By zzg 2013_09_09
}

/*==============================================================================
函数:
    EncodePWDToUint16

说明:
    将输入字符串转换为无符号整型数。

参数:
    pszPWD [in]：密码字符串。

返回值:
    none

备注:
    密码字符串长度最多为8，且只含字符0-9及*和#
==============================================================================*/
uint16 EncodePWDToUint16(char *pszPWD)
{
    int  index, nLen;
    float wPow = 0.1;
    uint16 wRet = 0;
    uint16 wTep = 10000;
    uint16  wPSD[10];
    
    if (NULL == pszPWD)
    {
        return wRet;
    }
    
    MEMSET(wPSD, 0, sizeof(wPSD));     
    
    nLen = STRLEN(pszPWD);

    if (nLen > 8)
    {
        return wRet;
    }       

    for (index = 0; index < nLen; index++)
    {
        wPow = wPow * 10; 
             
        switch(pszPWD[index])
        {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                wPSD[index] = (uint16)(pszPWD[index] - 48); 
                wTep += wPSD[index] * wPow;                                         
                break;

            case '*':
                wTep += (uint16)(index + 1) * 37;
                
                break;

            case '#':
                wTep += (uint16)(index + 1) * 97;
                break;
                
            case '0':
               wTep += (uint16)(index + 1) * 67;
               break;
            default:
                break;
        }           
                 
    }  

    wRet = wTep;  
    return wRet;
}

/*==============================================================================
函数:
    Appscom_GetThemeParameters

说明:
    函数用于获取当前设置的主题相关的参数。

参数:
    TParam [in/out]: 用于返回结果的参数指针。

返回值:
    none

备注:

==============================================================================*/
void Appscom_GetThemeParameters(Theme_Param_type *TParam)
{
#if defined( AEE_SIMULATOR)
    if (NULL == TParam)
    {
        return;
    }
    
    MEMSET(TParam, 0, sizeof(Theme_Param_type));
    
    // 下列参数尚未与主题关联
    TParam->textColor = MAKE_RGB( 0xde, 0xde, 0xde);
    TParam->seltextColor = RGB_BLACK;
    TParam->bkcolor = RGB_BLACK;
    TParam->themeColor = MAKE_RGB(0xfe, 0x61, 0x01);
    return;
#else
    
#ifdef FEATURE_FUNCS_THEME    
    byte ThemeType = DESKTOP_THEME_ORANGE;
    
    if (NULL == TParam)
    {
        return;
    }
    
    MEMSET(TParam, 0, sizeof(Theme_Param_type));
    
    // 下列参数尚未与主题关联
    TParam->textColor = MAKE_RGB( 0xde, 0xde, 0xde);
    TParam->seltextColor = RGB_BLACK;
    TParam->bkcolor = RGB_BLACK;
    
    // 下列参数尚未采用
    //TParam->selbkcolor = ?;

    (void)OEM_GetConfig(CFGI_DESKTOP_THEME, &ThemeType, sizeof(ThemeType));
    switch (ThemeType)
    {
        case DESKTOP_THEME_GREEN:
            TParam->themeColor = MAKE_RGB(0x75, 0xb1, 0x17);
            break;
            
        case DESKTOP_THEME_BLUE:
            TParam->themeColor = MAKE_RGB(0x00, 0x99, 0xff);
            break;

        case DESKTOP_THEME_DEEP_BLUE:
            TParam->themeColor = MAKE_RGB(0x38, 0x65, 0xdf);
            break;
            
        case DESKTOP_THEME_RED:
            TParam->themeColor = MAKE_RGB(0xfe, 0x41, 0x31);
            break;
        
        case DESKTOP_THEME_ORANGE:
            TParam->themeColor = MAKE_RGB(0xfe, 0x61, 0x01);
            break;
            
        case DESKTOP_THEME_YELLOW:
            TParam->themeColor = MAKE_RGB(0xfe, 0x8e, 0x00);
            break;
            
        default:
            break;
    }

#else//部分代码中会取字体以及背景色，在这里关闭feature后做一些处理

#ifdef FEATURE_WHITE_BG
    TParam->textColor = RGB_BLACK;  //MAKE_RGB( 0xde, 0xde, 0xde);
    TParam->seltextColor = RGB_WHITE; //RGB_BLACK;
    TParam->bkcolor = RGB_WHITE; //RGB_BLACK;
    TParam->themeColor = RGB_WHITE; //RGB_BLACK;
#else
    TParam->textColor = MAKE_RGB( 0xde, 0xde, 0xde);
    TParam->seltextColor = RGB_BLACK;
    TParam->bkcolor = RGB_BLACK;
    TParam->themeColor = RGB_BLACK;
#endif

#endif /* FEATURE_FUNCS_THEME */    
#endif //#if defined( AEE_SIMULATOR)
}

/*==============================================================================
函数:
    GetTitleBarHeight

说明:
    本函数用于获取标题提示条的高度。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。

返回值:
    none

备注:

==============================================================================*/
int GetTitleBarHeight(IDisplay  * pIDisplay)
{
#ifndef FEATURE_FUNCS_TITLE_BAR
    return 0;
#else    
    if (NULL == pIDisplay)
    {
        return 0;
    }
    
    return TITLEBAR_HEIGHT;//这里不能适应所有的尺寸，可以考虑使用FEATURE分开不同的屏幕尺寸。(IDISPLAY_GetFontMetrics(pIDisplay, AEE_FONT_BOLD, NULL, NULL));
#endif /* FEATURE_FUNCS_TITLE_BAR */    
}

/*==============================================================================
函数:
    GetBottomBarHeight

说明:
    本函数用于获取底部提示条的高度。

参数:
    pIDisplay [in]: 指向 IDisplay 接口的指针。

返回值:
    none

备注:

==============================================================================*/
int GetBottomBarHeight(IDisplay  * pIDisplay)
{
#ifndef FEATURE_FUNCS_BOTTOM_BAR
    return 0;
#else    
    if (NULL == pIDisplay)
    {
        return 0;
    }
    
    return BOTTOMBAR_HEIGHT;//这里不能适应所有的尺寸，可以考虑使用FEATURE分开不同的屏幕尺寸。(IDISPLAY_GetFontMetrics(pIDisplay, AEE_FONT_BOLD, NULL, NULL));
#endif /* FEATURE_FUNCS_BOTTOM_BAR */    
}


//Add By zzg 2011_12_08
void ReDrawPromptMessage(void *pShell)
{
	IImage          *StringBgMsgImg; 
	AEEImageInfo 	StringBgImgInfo = {0};
	AEERect         totalrect;
	AEEDeviceInfo   devinfo;         

	DBGPRINTF("***zzg ReDrawPromptMessage***");
	
	MEMSET(&devinfo, 0, sizeof(devinfo));
    ISHELL_GetDeviceInfo(pShell, &devinfo);

	totalrect.x = devinfo.cxScreen / 2;    
    totalrect.y = (devinfo.cyScreen  - BOTTOMBAR_HEIGHT)/2;	//Add By zzg 2010_07_16
	
    //Draw string background picture
    StringBgMsgImg = ISHELL_LoadResImage(pShell,
                        AEE_APPSCOMMONRES_IMAGESFILE,
                        IDB_PROMPT_MSG_BG);
    if(StringBgMsgImg != NULL)
    {
        IIMAGE_GetInfo(StringBgMsgImg, &StringBgImgInfo);
        //totalrect.x = (devinfo.cxScreen - StringBgImgInfo.cx)/2;
        IIMAGE_Draw(StringBgMsgImg, totalrect.x - StringBgImgInfo.cx/2, totalrect.y - StringBgImgInfo.cy/2);
        IIMAGE_Release(StringBgMsgImg);
        StringBgMsgImg = NULL;       
    }
#ifdef FEATURE_VERSION_K212
	ISHELL_SetTimer(pShell, 500, (PFNNOTIFY)(ReDrawPromptMessage),pShell);
#else
	ISHELL_SetTimer(pShell, 1000, (PFNNOTIFY)(ReDrawPromptMessage),pShell);
#endif
}



void CancelReDrawPromptMessage(void *pShell)
{	
	ISHELL_CancelTimer(pShell, (PFNNOTIFY)(ReDrawPromptMessage),pShell);
}
//Add End


/*==============================================================================
函数:
    DrawPromptMessage

说明:

参数:
    pIDisplay  [in]：IDisplay指针。
    pStatic    [in]: IStatic控件。    
    PromptMsg_Param_type [in]：提示消息相关参数指针

返回值:
    none

备注:
==============================================================================*/
void DrawPromptMessage (IDisplay *pIDisplay,
                        IStatic *pStatic,
                        PromptMsg_Param_type *PParam)
{      
    IShell          *pShell;     
    uint16          pMsgImgResID = 0;// warning
    IImage          *pMsgImg; 
    IImage          *StringBgMsgImg; 
    IImage          *TitleBgMsgImg;      
    AEERect         totalrect;
    AEERect         strrect;
    AEERect         titlerect; 
    BottomBar_Param_type bottomParam;
    TitleBar_Param_type  titleParam;
    AEEDeviceInfo   devinfo;         
    boolean         drawbottomStr;  
    boolean         drawbgimage = FALSE;  
    AECHAR         *pwszMsg = NULL;  

	MSG_FATAL("***zzg DrawPromptMessage***",0,0,0);
               
    pShell = AEE_GetShell();                       
    
    if ((NULL == pShell) 
        || (NULL == pIDisplay) 
        || (NULL == pStatic) 
        || (NULL == PParam))
    {
        return;
    }   
        
    switch (PParam->ePMsgType)
    {
        case MESSAGE_CONFIRM:
            drawbottomStr = TRUE;
            pMsgImgResID = IDB_CONFIRM;        
            break;
            
        case MESSAGE_NONE:
        default:        
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_INFORMATION;
            break;
#ifndef FEATURE_USES_LOWMEM
//以下由于图片以及spec的修改，重新将图片画在中间，底条根据用户需求来画(与cs0x相同)
        case MESSAGE_ERR:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_ERROR;
            break;
            
        case MESSAGE_WARNNING:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_WARNNING;        
            break;

#ifdef FEATURE_VERSION_C316
        case MESSAGE_UNLOCK:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_UNLOCK;        
            break;
#endif
						
        case MESSAGE_WAITING:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_WAITING;        
            break;
            
        case MESSAGE_INFORMATION:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_INFORMATION;        
            break;   
            
        case MESSAGE_INFORMATIVE:
            drawbottomStr = FALSE;
            pMsgImgResID = IDB_INFORMATIVE;        
            break;               
#endif
    }  
    
    //Draw shadow for screen
#ifdef FEATURE_VERSION_NO_BG
    IDisplay_FillRect(pIDisplay, PParam->prc, RGB_BLACK);        
#else
    pMsgImg = ISHELL_LoadResImage(pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_BACKGROUND);
    if(pMsgImg != NULL)
    {                  
        #ifdef FEATURE_VERSION_SKY
        {
        AEERect rc;
        SETAEERECT(&rc, 0, 0, 176,22);  
        IDISPLAY_DrawRect(pIDisplay,&rc,RGB_BLACK,RGB_BLACK,IDF_RECT_FILL);
        IIMAGE_Draw(pMsgImg, 0, 22);
        }
        #else
        IIMAGE_Draw(pMsgImg, 0, 0);
        #endif
        IIMAGE_Release(pMsgImg);
        pMsgImg = NULL;
    }
    else
    {
        return;
    }
#endif

    if (NULL != PParam->prc)
    {
        totalrect = *(PParam->prc);

        titlerect.dy = GetBottomBarHeight(pIDisplay);
        titlerect.dx = totalrect.dx;
        titlerect.x = totalrect.x;
        titlerect.y = totalrect.y;

        strrect.dy = totalrect.dy - titlerect.dy;
        strrect.dx = totalrect.dx;
        strrect.x = totalrect.x;
        strrect.y = totalrect.y - titlerect.dy;  
        IDISPLAY_EraseRect(pIDisplay, &totalrect);  
    }
    else
    {
        AEEImageInfo StringBgImgInfo = {0};
        AEEImageInfo TitleBgImgInfo = {0};
        
        //Draw back ground picture   
        MEMSET(&devinfo, 0, sizeof(devinfo));
        ISHELL_GetDeviceInfo(pShell, &devinfo);     
        
#if 0 
        totalrect.x = devinfo.cxScreen/5;
        if(devinfo.cxScreen == devinfo.cyScreen)
        {
            totalrect.y = devinfo.cyScreen/5; 
        }
        else
        {
            totalrect.y = devinfo.cyScreen/5- GetBottomBarHeight(pIDisplay); 
        }        
#endif    
/*
        //Draw title background picture
        TitleBgMsgImg = ISHELL_LoadResImage(pShell,
                                    AEE_APPSCOMMONRES_IMAGESFILE,
                                    pMsgImgResID); 
        if(TitleBgMsgImg != NULL)
        {
            IIMAGE_GetInfo(TitleBgMsgImg, &TitleBgImgInfo);
            IIMAGE_Draw(TitleBgMsgImg, totalrect.x, totalrect.y);
            IIMAGE_Release(TitleBgMsgImg);
            TitleBgMsgImg = NULL; 
            
            //Draw string background picture
            StringBgMsgImg = ISHELL_LoadResImage(pShell,
                                AEE_APPSCOMMONRES_IMAGESFILE,
                                IDB_PROMPT_MSG_BG); 
            if(StringBgMsgImg != NULL)
            {
                IIMAGE_GetInfo(StringBgMsgImg, &StringBgImgInfo);
                IIMAGE_Draw(StringBgMsgImg, totalrect.x, totalrect.y+TitleBgImgInfo.cy);
                IIMAGE_Release(StringBgMsgImg);
                StringBgMsgImg = NULL; 
            }             
        }       
*/
/*
        totalrect.x = 0;
        totalrect.y = devinfo.cyScreen - GetBottomBarHeight(pIDisplay);
*/

        totalrect.x = devinfo.cxScreen / 2;
        //totalrect.y = (devinfo.cyScreen - TITLEBAR_HEIGHT - BOTTOMBAR_HEIGHT)/2 + TITLEBAR_HEIGHT;
        totalrect.y = (devinfo.cyScreen  - BOTTOMBAR_HEIGHT)/2;	//Add By zzg 2010_07_16
		
        //Draw string background picture
        StringBgMsgImg = ISHELL_LoadResImage(pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_PROMPT_MSG_BG);
        if(StringBgMsgImg != NULL)
        {
            IIMAGE_GetInfo(StringBgMsgImg, &StringBgImgInfo);
            //totalrect.x = (devinfo.cxScreen - StringBgImgInfo.cx)/2;
            IIMAGE_Draw(StringBgMsgImg, totalrect.x - StringBgImgInfo.cx/2, totalrect.y - StringBgImgInfo.cy/2);
            
            MSG_FATAL("***zzg DrawPromptMessage StringBgImgInfo.dx=%d, StringBgImgInfo.dy=%d***",StringBgImgInfo.cx,StringBgImgInfo.cy,0);
            MSG_FATAL("***zzg DrawPromptMessage BGImage.x=%d, BGImage.y=%d***",totalrect.x - StringBgImgInfo.cx/2,totalrect.y - StringBgImgInfo.cy/2,0);
           
            IIMAGE_Release(StringBgMsgImg);
            StringBgMsgImg = NULL;  
            
            //Draw title background picture
            TitleBgMsgImg = ISHELL_LoadResImage(pShell,
                                        AEE_APPSCOMMONRES_IMAGESFILE,
                                        pMsgImgResID);
            if(TitleBgMsgImg != NULL)
            {
                IIMAGE_GetInfo(TitleBgMsgImg, &TitleBgImgInfo);
                
                IIMAGE_Draw(TitleBgMsgImg, totalrect.x - TitleBgImgInfo.cx/2, totalrect.y - StringBgImgInfo.cy/2 + TitleBgImgInfo.cy/2);
                IIMAGE_Release(TitleBgMsgImg);
                TitleBgMsgImg = NULL;               
            }
                                   
        }
        if((TitleBgImgInfo.cy!= 0) && (StringBgImgInfo.cy!= 0))
        {              
            totalrect.dy = StringBgImgInfo.cy + TitleBgImgInfo.cy; 
            totalrect.dx = TitleBgImgInfo.cx;        

            titlerect.dy = TitleBgImgInfo.cy;
            titlerect.dx = TitleBgImgInfo.cx;
            titlerect.x = totalrect.x - TitleBgImgInfo.cx/2;
            titlerect.y = totalrect.y  - StringBgImgInfo.cy/2 + TitleBgImgInfo.cy/2;
            #if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S)|| \
            defined(FEATURE_VERSION_C11)|| defined(FEATURE_VERSION_C180) || defined(FEATURE_VERSION_W027)|| \
            defined(FEATURE_VERSION_C316)|| defined(FEATURE_VERSION_W021_CT100)|| defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_EC99)|| \
            defined(FEATURE_VERSION_W021_C11)||defined(FEATURE_VERSION_K292)
            strrect.dy = StringBgImgInfo.cy/2 + 30;
			#ifdef FEATURE_VERSION_W317A   // add by pyuangui 20121225
			strrect.dy = StringBgImgInfo.cy/2 + 40;
			strrect.dx = StringBgImgInfo.cx+10;
			#else
            strrect.dx = StringBgImgInfo.cx;
			#endif
            strrect.x = totalrect.x - StringBgImgInfo.cx/2;            
			strrect.y = totalrect.y - 15;	
            #elif defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
			strrect.dy = StringBgImgInfo.cy*3/4;
            strrect.dx = StringBgImgInfo.cx;
            strrect.x = totalrect.x - StringBgImgInfo.cx/2;
            strrect.y = totalrect.y-80;    
            #else
            strrect.dy = StringBgImgInfo.cy/2;
            strrect.dx = StringBgImgInfo.cx;
            strrect.x = totalrect.x - StringBgImgInfo.cx/2;
            strrect.y = totalrect.y;            
            #endif

            
            drawbgimage = TRUE;                           
        }
        else
        {     
            //Draw totalrect  
            totalrect.dx  = devinfo.cxScreen * 75 / 100;
            totalrect.dy = devinfo.cyScreen * 75 / 100;

            titlerect.dy = GetBottomBarHeight(pIDisplay);
            titlerect.dx = totalrect.dx;
            titlerect.x = totalrect.x;
            titlerect.y = totalrect.y;

            strrect.dy = totalrect.dy - titlerect.dy;
            strrect.dx = totalrect.dx;
            strrect.x = totalrect.x;
            strrect.y = totalrect.y - titlerect.dy;
            IDISPLAY_EraseRect(pIDisplay, &totalrect);     
        } 
    }
        
    //Draw title 
    if(PParam->pwszTitle != NULL)
    {
        MEMSET(&titleParam,0,sizeof(TitleBar_Param_type));
        titleParam.prc = &titlerect;
        titleParam.pwszTitle = PParam->pwszTitle;
        DrawTitleBar(pIDisplay, &titleParam);
    }
    
    //Draw bottom
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
 if(PParam->eBBarType == BTBAR_NONE)	
 {
 	bottomParam.eBBarType = BTBAR_OK_BACK;
 }
 else
#endif
 {
    MEMSET(&bottomParam,0,sizeof(BottomBar_Param_type));
    if(NULL != PParam->eBBarType)
    {
        bottomParam.eBBarType = PParam->eBBarType;
    }
    else
    {
        if(TRUE == drawbottomStr)
        {
            bottomParam.eBBarType = BTBAR_BACK; //BTBAR_SELECT_BACK;            

        }  
		
    }
    #ifdef FEATURE_VERSION_K212_HUALU
    if(bottomParam.eBBarType != BTBAR_NONE)
    #endif
    {
        DrawBottomBar(pIDisplay, &bottomParam);  
    }
 }
	
/*
#ifdef FEATURE_VERSION_W208S
	strrect.x += 5;
	strrect.dx -= 10;
#endif
*/

#if defined(FEATURE_VERSION_C337) ||defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_EC99) ||defined(FEATURE_VERSION_IC241A_MMX)
#if defined (FEATURE_VERSION_C260_IC18) || defined(FEATURE_VERSION_IC241A_MMX)
        strrect.x += 5;
        strrect.y += 15;
        strrect.dx -= 10;        
        strrect.dy -= 35;
#elif defined (FEATURE_VERSION_K212_20D) || defined(FEATURE_VERSION_EC99)    
        strrect.x += 10;       
        strrect.dx -= 20;        
        strrect.dy -= 30;
#else
        strrect.x += 5;
        strrect.dx -= 10;        
        strrect.dy -= 20;
#endif        
#endif

    MSG_FATAL("***zzg DrawPromptMessage strrect.x=%d, strrect.y=%d***",strrect.x,strrect.y,0);
    MSG_FATAL("***zzg DrawPromptMessage strrect.dx=%d, strrect.dy=%d***",strrect.dx,strrect.dy,0);
     
    //Draw string  
    ISTATIC_SetRect(pStatic, &strrect);
    if(NULL != PParam->pwszMsg)
    {
        pwszMsg = PParam->pwszMsg;
    }
    else if((NULL != PParam->strMsgResFile) && (0 != PParam->nMsgResID))
    {
        int32  dwSize = 0;
        
        dwSize = 128 * sizeof(AECHAR);
        pwszMsg = (AECHAR *)MALLOC(dwSize);
        if (NULL != pwszMsg)
        {
            (void)ISHELL_LoadResString(pShell, 
                                       PParam->strMsgResFile, 
                                       PParam->nMsgResID, 
                                       pwszMsg, 
                                       dwSize);
        }            
    }       
    if (NULL != pwszMsg)
    {
#ifndef FEATURE_USES_LOWMEM 
        IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_WHITE);
#else
        IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
#endif
        if (TRUE== drawbgimage)
        {
        	DBGPRINTF("***zzg Appscommon DrawPromptMessage***");                 
#if defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_K212_20D)||defined(FEATURE_VERSION_EC99)||defined(FEATURE_VERSION_IC241A_MMX)         
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT|ST_TRANSPARENTBACK|ST_SPECIAL_BG);   	//Modify by zzg 2011_12_31
#else
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT|ST_TRANSPARENTBACK);   
#endif
        }
        else
        {
            ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT); 
        }   

		MSG_FATAL("***zzg Appscommon DrawPromptMessage***", 0, 0, 0);

		ISTATIC_SetFontColor(pStatic, RGB_WHITE);		//Add By zzg 2011_12_07

		/*
		//为了同步AEE_Static 的Auto_scroll 
		{
			uint32      nMS;
			if(OEM_GetConfig(CFGI_ISTATIC_SCROLL, &nMS, sizeof(uint32)) || nMS == 0)
			{
         		nMS = 1000;
      		}

			//nMS *= pStatic->m_nPageLines;	
			
      		ISHELL_SetTimer(pShell, nMS, (PFNNOTIFY)(ReDrawPromptMessage),pShell);
		}
		*/
		
		
        (void)ISTATIC_SetText(pStatic, 
                              NULL, 
                              pwszMsg, 
                              AEE_FONT_NORMAL, 
                              AEE_FONT_NORMAL);
   
        //Redraw
        (void)ISTATIC_Redraw(pStatic);
        if (NULL == PParam->pwszMsg)
        {
            FREE(pwszMsg);
        }
        IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
    } 
}

#ifdef FEATURE_KEYGUARD
void Appscomm_Draw_Keyguard_Msg(IDisplay *pIDisplay,IStatic *pStatic,boolean unlockkey)
{
    PromptMsg_Param_type m_PromptMsg;
    AEERect rect = {0};
    ISTATIC_SetRect(pStatic, &rect);
    MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
    //SETAEERECT(&rect, 0, 16, 128, 112);
    //m_PromptMsg.prc = &rect;

	if ( (NULL == pIDisplay) 
        || (NULL == pStatic))
    {
        return;
    } 
//Add By zzg 2010_11_23
#ifdef FEATURE_UNLOCK_KEY_SPACE
	m_PromptMsg.nMsgResID = IDS_MSG_KEYGUARD_SPACE;
#else
	if(TRUE == unlockkey)
	{
	    m_PromptMsg.nMsgResID = IDS_MSG_KEYGUARD_UNLOCKKEY;
	}
	else
	{
	    m_PromptMsg.nMsgResID = IDS_MSG_KEYGUARD;        
	}
#endif
//Add End

	/*
    if(TRUE == unlockkey)
    {
        m_PromptMsg.nMsgResID = IDS_MSG_KEYGUARD_UNLOCKKEY;
    }
    else
    {
        m_PromptMsg.nMsgResID = IDS_MSG_KEYGUARD;        
    }
    */
    
    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSCOMMONRES_LANGFILE,MAX_FILE_NAME);
#ifdef FEATURE_VERSION_C316 
    m_PromptMsg.ePMsgType = MESSAGE_UNLOCK;
#else
    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
#endif
    m_PromptMsg.eBBarType = BTBAR_NONE;   
    DrawPromptMessage(pIDisplay,pStatic,&m_PromptMsg);
    IDISPLAY_UpdateEx(pIDisplay,FALSE);
}

//Add By zzg 2012_12_03
void Appscomm_Draw_Keyguard_Information(IDisplay *pIDisplay,IStatic *pStatic,boolean unlockkey)
{
    PromptMsg_Param_type m_PromptMsg;
    AEERect rect = {0};
#if defined(FEATURE_VERSION_C316)
      {
		    boolean bData = FALSE;
		    OEM_GetConfig(CFGI_ONEKEY_LOCK_KEYPAD,&bData, sizeof(bData));
			if(OEMKeyguard_IsEnabled() && bData)
		    {
		        return ;
			}
      }
#endif		
    ISTATIC_SetRect(pStatic, &rect);
    MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
    //SETAEERECT(&rect, 0, 16, 128, 112);
    //m_PromptMsg.prc = &rect;

	if ( (NULL == pIDisplay) 
        || (NULL == pStatic))
    {
        return;
    } 

	if(TRUE == unlockkey)
	{
	    m_PromptMsg.nMsgResID = IDS_KEYPAD_UNLOCKED;
	}
	else
	{
	    m_PromptMsg.nMsgResID = IDS_KEYPAD_LOCKED;        
	}
    
    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSCOMMONRES_LANGFILE,MAX_FILE_NAME);
#ifdef FEATURE_VERSION_C316 
    m_PromptMsg.ePMsgType = MESSAGE_UNLOCK;
#else
    m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
#endif 
    m_PromptMsg.eBBarType = BTBAR_NONE;        
    DrawPromptMessage(pIDisplay,pStatic,&m_PromptMsg);
    IDISPLAY_UpdateEx(pIDisplay,FALSE);
}
//Add End

#ifdef FEATURE_LCD_TOUCH_ENABLE 
void Appscomm_Draw_Keyguard_Slide(IDisplay *pIDisplay,uint16 x,uint16 y)
{
	IImage *m_imageSlide = NULL;
	IShell      *pShell = AEE_GetShell();
	int tempx = 0;
	if ((NULL == pIDisplay))
    {
        return;
    } 
	
	tempx=x-151;
	MSG_FATAL("x======%d",x,0,0);
	m_imageSlide = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SLIDE_BAR);
	if(m_imageSlide != NULL)
	{
		IIMAGE_Draw(m_imageSlide,tempx,y);                          
		IIMAGE_Release(m_imageSlide);
		m_imageSlide = NULL;
	}
	IDISPLAY_UpdateEx(pIDisplay,FALSE);
}
void Appscomm_Draw_Keyguard_BackGroud(IDisplay *pIDisplay,uint16 x,uint16 y)
{
	IImage *m_imageSlide = NULL;
	IShell		*pShell = AEE_GetShell();
	if ((NULL == pIDisplay))
	{
		return;
	} 
	m_imageSlide = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SLIDE_BOTTOM);
	if(m_imageSlide != NULL)
	{
		IIMAGE_Draw(m_imageSlide,x,y);							
		IIMAGE_Release(m_imageSlide);
		m_imageSlide = NULL;
	}
	IDISPLAY_UpdateEx(pIDisplay,FALSE);
}
void Appscomm_Draw_Keyguard_BackGroudbar(IDisplay *pIDisplay,uint16 x,uint16 y)
{
	IImage *m_imageSlide = NULL;
	IImage *m_imageSlides = NULL;
	IShell		*pShell = AEE_GetShell();
	if ((NULL == pIDisplay))
	{
		return;
	} 
	m_imageSlides = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SLIDE_BAR);
	m_imageSlide = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SLIDE_BOTTOM);
	if(m_imageSlide != NULL)
	{
		IIMAGE_Draw(m_imageSlide,x,y);							
		IIMAGE_Release(m_imageSlide);
		m_imageSlide = NULL;
	}
	if(m_imageSlides != NULL)
	{
		IIMAGE_Draw(m_imageSlides,-151,260);							
		IIMAGE_Release(m_imageSlides);
		m_imageSlides = NULL;
	}
	IDISPLAY_UpdateEx(pIDisplay,FALSE);
}

#endif

#endif
#ifdef FEATURE_KEYGUARD
void Appscommon_Draw_Keyguard_Time(IDisplay *pIDisplay)
{
    JulianType  jDate;
    AEERect rect = {0};
    AECHAR  wFormat[32] = {0}, wstrDisp[32] = {0}, wstrWeekDay[16] = {0};
    AEEDeviceInfo devinfo = {0};
    IShell      *pShell = AEE_GetShell();
    int         nLineWidth = 4, nNumberWidth = 20, nNumberHeight = 40, nOffset = 5,
                xStartPos = 0, yStartPos = 0, nTextLen = 0, 
                nFontHeight = IDISPLAY_GetFontMetrics(pIDisplay, AEE_FONT_NORMAL, NULL, NULL);
    byte      bTFmt = 0;
    uint16    nWeekResID = 0, nHour = 0;
    RGBVAL  nOldFontColor = RGB_WHITE;
	BottomBar_e_Type    eBBarType = BTBAR_NONE; 
    #if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
	nNumberWidth = 41;
	nNumberHeight = 61;
	nOffset = 5;
	#endif
    MSG_FATAL("Appscommon_Draw_Keyguard_Time......",0,0,0);
    if (NULL == pIDisplay)
    {
        return;
    } 
    ISHELL_GetDeviceInfo(pShell, &devinfo);
    //clear screen
    SETAEERECT(&rect, 0, 0, devinfo.cxScreen, devinfo.cyScreen);
    IDISPLAY_FillRect(pIDisplay, &rect, RGB_BLACK);
    
    GetJulianDate(GETTIMESECONDS(), &jDate);
    
    // draw day
#ifdef FEATURE_TIME_DATA_SETTING
    (void)OEM_GetConfig(CFGI_DATE_FORMAT, &bTFmt, sizeof(bTFmt));

    switch(bTFmt)
    {
        case OEMNV_DATEFORM_DMY:
            WSTRLCPY(wFormat,L"%02d/%02d/%04d",32);
            WSPRINTF(wstrDisp, sizeof(wstrDisp), wFormat, jDate.wDay, jDate.wMonth, jDate.wYear);
            break;

        case OEMNV_DATEFORM_MDY:
            WSTRLCPY(wFormat,L"%02d/%02d/%04d",32);
            WSPRINTF(wstrDisp, sizeof(wstrDisp), wFormat, jDate.wMonth, jDate.wDay, jDate.wYear);
            break;
            
        default:
        case OEMNV_DATEFORM_YMD:
            WSTRLCPY(wFormat,L"%04d/%02d/%02d",32);
            WSPRINTF(wstrDisp, sizeof(wstrDisp), wFormat, jDate.wYear, jDate.wMonth, jDate.wDay);
            break;
    }
#else
    WSTRLCPY(wFormat,L"%04d-%02d-%02d",32);
    WSPRINTF(wstrDisp, sizeof(wstrDisp), wFormat, jDate.wYear, jDate.wMonth, jDate.wDay);
#endif
    nTextLen = WSTRLEN(wstrDisp);
    wstrDisp[nTextLen] = L' ';
    wstrDisp[nTextLen+1] = L' ';
    wstrDisp[nTextLen+2] = L'\0';
    
    switch (jDate.wWeekDay)
    {
        case 0: // 星期一
            nWeekResID = AEE_IDS_MONDAY;
            break;
            
        case 1: // 星期二
            nWeekResID = AEE_IDS_TUESDAY;
            break;

        case 2: // 星期三
            nWeekResID = AEE_IDS_WEDNESDAY;
            break;

        case 3: // 星期四
            nWeekResID = AEE_IDS_THURSDAY;
            break;

        case 4: // 星期五
            nWeekResID = AEE_IDS_FRIDAY;
            break;

        case 5: // 星期六
            nWeekResID = AEE_IDS_SATURDAY;
            break;

        default:  // 6:星期日
            nWeekResID = AEE_IDS_SUNDAY;
            break;
    }
    (void) ISHELL_LoadResString(pShell, AEECONTROLS_RES_FILE, nWeekResID, wstrWeekDay, sizeof(wstrWeekDay));
    
    wstrWeekDay[3] = L'\0';
    nTextLen = WSTRLEN(wstrDisp);
    WSTRCPY(wstrDisp+nTextLen, wstrWeekDay);
    
    nTextLen = IDISPLAY_MeasureText(pIDisplay, AEE_FONT_NORMAL, wstrDisp);
    xStartPos = (devinfo.cxScreen - nTextLen)/2;
    yStartPos = (devinfo.cyScreen*2/5) - nFontHeight - 2*nOffset;
    
    nOldFontColor = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, nOldFontColor);
    
    IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, wstrDisp, -1, xStartPos, yStartPos, NULL, IDF_TEXT_TRANSPARENT);
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
    nTextLen = (nNumberWidth + nOffset)*4 + nLineWidth*3;
#else
	nTextLen = (nNumberWidth + nOffset)*4 + nLineWidth;
#endif
    xStartPos = (devinfo.cxScreen - nTextLen)/2;
	
	MSG_FATAL("xStartPos=======%d",xStartPos,0,0);
    yStartPos = (devinfo.cyScreen*2/5);

    // get time format
    (void)OEM_GetConfig(CFGI_TIME_FORMAT, &bTFmt, sizeof(bTFmt));

    if (bTFmt == OEMNV_TIMEFORM_AMPM)
    {
        nHour = (jDate.wHour > 12) ? (jDate.wHour - 12) : jDate.wHour;
        if(nHour/10 == 1)
        {
            xStartPos -= (nNumberWidth - nLineWidth)/2;
        }
        MEMSET(wstrDisp, 0, sizeof(wstrDisp));
        wstrDisp[0] = (jDate.wHour >= 12)?(L'P'):(L'A');
        wstrDisp[1] = L'M';
        wstrDisp[2] = L'\0';
        nTextLen = IDISPLAY_MeasureText(pIDisplay, AEE_FONT_NORMAL, wstrDisp);
        xStartPos -= (nTextLen + nOffset)/2;
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
		if(xStartPos<0)
		{
			xStartPos = 5;
		}
	#endif
        IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, wstrDisp, -1, 
                       xStartPos + (nNumberWidth + nOffset)*4 + nLineWidth*3 + nOffset, 
                       yStartPos + nNumberHeight - nFontHeight, 
                       NULL, IDF_TEXT_TRANSPARENT);
    }
    else
    {
        nHour = jDate.wHour;
        if(nHour/10 == 1)
        {
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
            xStartPos -= (nNumberWidth - nLineWidth)/2;
			#endif
        }
    }
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
	
	 // draw hour
    SETAEERECT(&rect, xStartPos, yStartPos, nNumberWidth, nNumberHeight);
	Appscommon_DrawDigitalNumberImage(pIDisplay, (nHour/10), nLineWidth, &rect);
	
	rect.x += nNumberWidth + nOffset;
    Appscommon_DrawDigitalNumberImage(pIDisplay, (nHour%10), nLineWidth, &rect);

	// draw colon
    SETAEERECT(&rect, xStartPos + 2*(nNumberWidth) + nOffset, yStartPos, nLineWidth*3, nLineWidth);
    Appscommon_DrawDigitalNumberImage(pIDisplay, 10, nLineWidth, &rect);

    
    // draw minute
    SETAEERECT(&rect, xStartPos + 2*(nNumberWidth + nOffset) + nLineWidth*3 + nOffset, yStartPos, nNumberWidth, nNumberHeight);
    Appscommon_DrawDigitalNumberImage(pIDisplay, (jDate.wMinute/10), nLineWidth, &rect);
    rect.x += nNumberWidth + nOffset;
    Appscommon_DrawDigitalNumberImage(pIDisplay, (jDate.wMinute%10), nLineWidth, &rect);
	
	#else
    // draw hour
    SETAEERECT(&rect, xStartPos, yStartPos, nNumberWidth, nNumberHeight);
    Appscommon_DrawDigitalNumber(pIDisplay, (nHour/10), nLineWidth, &rect, RGB_WHITE);
    rect.x += nNumberWidth + nOffset;
    Appscommon_DrawDigitalNumber(pIDisplay, (nHour%10), nLineWidth, &rect, RGB_WHITE);

    // draw colon
    SETAEERECT(&rect, xStartPos + 2*(nNumberWidth + nOffset), yStartPos + nNumberHeight/2 - nLineWidth, nLineWidth, nLineWidth);
    IDISPLAY_FillRect(pIDisplay, &rect, RGB_WHITE);
    rect.y = yStartPos + nNumberHeight*4/5 - nLineWidth;
    IDISPLAY_FillRect(pIDisplay, &rect, RGB_WHITE);
    
    // draw minute
    SETAEERECT(&rect, xStartPos + 2*(nNumberWidth + nOffset) + nLineWidth + nOffset, yStartPos, nNumberWidth, nNumberHeight);
    Appscommon_DrawDigitalNumber(pIDisplay, (jDate.wMinute/10), nLineWidth, &rect, RGB_WHITE);
    rect.x += nNumberWidth + nOffset;
    Appscommon_DrawDigitalNumber(pIDisplay, (jDate.wMinute%10), nLineWidth, &rect, RGB_WHITE);
    
    (void)IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, nOldFontColor);
	#endif
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
	eBBarType = BTBAR_UNLOCK_L;
    DrawBottomBar_Ex(pShell, pIDisplay,eBBarType);
	#endif
    IDISPLAY_Update(pIDisplay);
}
#endif
void DrawBottomBar_Ex(IShell    *m_pIShell, IDisplay  * pIDisplay, BottomBar_e_Type    eBBarType)
{
    AEEDeviceInfo devinfo;
    AEERect rc;
    uint16      nResID_L = 0;// 左
    uint16      nResID_R = 0;// 右
    uint16      nResID_M = 0;// 中   ////modi by yangdecai 2010-08-04
    AECHAR      wszBuf[16]= {(AECHAR)'\0'};
    int nBarH = 0;
    int nFontH = 0;
	
    if ((NULL == m_pIShell) || (NULL == pIDisplay))
    {
        return;
    }

	

    nBarH = GetBottomBarHeight(pIDisplay);
    nFontH = IDISPLAY_GetFontMetrics(pIDisplay, AEE_FONT_NORMAL, NULL, NULL);

    MEMSET(&devinfo, 0, sizeof(devinfo));
    ISHELL_GetDeviceInfo(m_pIShell, &devinfo);
    //SETAEERECT(&rc, 2, devinfo.cyScreen-nBarH, devinfo.cxScreen-4, nBarH);
    if(nBarH < nFontH + 2)
    {
        //因为右对齐会在右边自动空出一个像素，所以左边的矩形边界应当比右边多一个像素
        SETAEERECT(&rc, 3, devinfo.cyScreen-nBarH, devinfo.cxScreen-5, nBarH);
    }
    else
    {
        SETAEERECT(&rc, 3, devinfo.cyScreen-nFontH-2, devinfo.cxScreen-5, nFontH);
    }

    switch (eBBarType)
    {
        case BTBAR_MESSAGES_CONTACTS:
            nResID_L = IDS_MESSAGES;
			//#ifdef FEATURE_VERSION_C337
			//nResID_R = IDS_PHONE_BOOK;
			//#else
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_IC241A_MMX)
			nResID_R = IDS_NAMES;
#else
			nResID_R = IDS_STRING_CONTACTS;
#endif			            
			//#endif
            break;

        case BTBAR_SHORTCUT_CONTACTS:
            nResID_L = IDS_SHORTCUT;
	        nResID_R = IDS_NAMES;
            break;

        case BTBAR_RECENTCALLS_CONTACTS:
            nResID_L = IDS_RECENTCALLS;
	        nResID_R = IDS_STRING_CONTACTS;
            break;
            
        case BTBAR_MENU_CONTACTS:
            nResID_L = IDS_MENU;
			//#ifdef FEATURE_VERSION_C337
			//nResID_R = IDS_PHONE_BOOK;
			//#else
            #if defined(FEATURE_VERSION_K212)
			nResID_R = IDS_CONTANT_K212;
			#else
            nResID_R = IDS_STRING_CONTACTS;
			#endif
			//#endif
            break;
            
		case BTBAR_MENU_SOS:
         	nResID_L = IDS_MENU;
            nResID_R = IDS_SOS;
         	break;

		/*
		//Add By zzg 2011_12_12
		case BTBAR_LOCK_BACK:
         	nResID_L = IDS_LOCK;
            nResID_R = IDS_BACK;
         	break;
		//Add End
		*/

        case BTBAR_OK_SOS:
            nResID_L = IDS_OK;
			nResID_R = IDS_SOS;
			break;
			
		case BTBAR_MENU_FRENDUO:
			nResID_L = IDS_MENU;
			nResID_R = IDS_FRENDUO_LIST;
			break;
		
		case BTBAR_FACEBOOK_CHAT:
			nResID_L = IDS_FACEBOOK;
			nResID_R = IDS_CHAT;
			break;
            
		case BTBAR_VIEWMORE_BACK:
            nResID_L = IDS_MORE;
			nResID_M = IDS_OK;   ////add by yangdecai 2010-08-04
			nResID_R = IDS_BACK;
			break;
		case BTBAR_OK_BACK:
            nResID_L = IDS_OK;
			nResID_R = IDS_BACK;
			break;
        case BTBAR_UNLOCK_SOS:
            nResID_R = IDS_STRING_UNLOCK;
            nResID_L = IDS_SOS;
            break;
#if (defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_C316) || defined (FEATURE_VERSION_IC241A_MMX))
		case BTBAR_UNLOCK_M:
            nResID_L = IDS_STRING_UNLOCK;
            break; 
#else
            
        case BTBAR_UNLOCK_M:
            nResID_M = IDS_STRING_UNLOCK;
            break;    
#endif	
            
        case BTBAR_UNLOCK_L:
            nResID_L= IDS_STRING_UNLOCK;
            break;
            
		case BTBAR_UNLOCK:
			nResID_R = IDS_STRING_UNLOCK;
			break;
            
        case BTBAR_LUNLOCK:
            nResID_L = IDS_STRING_UNLOCK;
            nResID_R = IDS_SOS;
            break;			
            
        case BTBAR_BACK:
            nResID_R = IDS_BACK;
            break;

        case BTBAR_OK:
            nResID_L = IDS_OK;
            break;
        
        case BTBAR_OPTION_ENDCALL:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_STRING_END_CALL;
            break;
               
		case BTBAR_OPTION:
			nResID_L = IDS_OPTION;
            break;
        // 选择-----返回
         case BTBAR_SELECT_BACK:
            nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
            break;
         case BTBAR_PRIVIEW_BACK:
		 	nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_MODIFY_BACK:
			nResID_L = IDS_OK;	//SELECT;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_SELECT_PLAY_BACK:
			nResID_L = IDS_OK;	//SELECT;
			nResID_M = IDS_PLAY;
            nResID_R = IDS_BACK;
		 	break;
		case BTBAR_SELECT_STOP_BACK:
			nResID_L = IDS_OK;	//SELECT;
			nResID_M = IDS_STOP;
            nResID_R = IDS_BACK;
		 	break;
        case BTBAR_SAVE_BACK:
            nResID_L = IDS_SAVE;
            nResID_R = IDS_BACK;
            break;
#ifdef FEATURE_FLEXI_STATIC_BREW_APP
        case BTBAR_FNASRANI_FPORTAL:
            nResID_L = IDS_FNASRANI;
            nResID_R = IDS_FPORTAL;
            break;
        case BTBAR_FMUSLIM_FPORTAL:
            nResID_L = IDS_FMUSLIM;
            nResID_R = IDS_FPORTAL;
            break;  
        case BTBAR_FGURU_FPORTAL:
            nResID_L = IDS_FGURU;
            nResID_R = IDS_FPORTAL;
            break;
            
        case BTBAR_CONTACTS_FPORTAL:
			//#ifdef FEATURE_VERSION_C337
			//nResID_L = IDS_PHONE_BOOK;
			//#else
            nResID_L = IDS_STRING_CONTACTS;
			//#endif
            nResID_R = IDS_FPORTAL;
            break;
#endif  /*FEATURE_FLEXI_STATIC_BREW_APP*/
		case BTBAR_FMENU_CANTACT:
			nResID_L = IDS_FMENU;
			//#ifdef FEATURE_VERSION_C337
			//nResID_R = IDS_PHONE_BOOK;
			//#else
			nResID_R = IDS_STRING_CONTACTS;
			//#endif
			break; 
		case BTBAR_MENU_FMENU:
			nResID_L = IDS_MENU;
			nResID_R = IDS_FMENU;
			break;
        case BTBAR_MIZONE_SELECT_BACK:
            nResID_L = IDS_SELECT;
            nResID_R = IDS_BACK;
            break; 
        case BTBAR_ACTIVATE_BACK:
            nResID_L = IDS_MIZONEACTIVATE;
            nResID_R = IDS_BACK;
            break;     
    }
	
    if(nResID_L)
    {
        ISHELL_LoadResString(m_pIShell,AEE_APPSCOMMONRES_LANGFILE,nResID_L,wszBuf, sizeof(wszBuf));
        DrawTextWithProfile(m_pIShell, pIDisplay,RGB_WHITE_NO_TRANS,AEE_FONT_NORMAL, wszBuf, -1,0, 0, &rc,  IDF_ALIGN_MIDDLE | IDF_ALIGN_LEFT  | IDF_TEXT_TRANSPARENT);
    } 	
	
    if(nResID_R)
    {
        ISHELL_LoadResString(m_pIShell,AEE_APPSCOMMONRES_LANGFILE,nResID_R,wszBuf, sizeof(wszBuf));
        DrawTextWithProfile(m_pIShell, pIDisplay,RGB_WHITE_NO_TRANS,AEE_FONT_NORMAL, wszBuf, -1,0, 0, &rc,  IDF_ALIGN_MIDDLE | IDF_ALIGN_RIGHT  | IDF_TEXT_TRANSPARENT);
    }	
	
	//add by yangdecai 2010-08-04
	if(nResID_M)
	{
		ISHELL_LoadResString(m_pIShell,AEE_APPSCOMMONRES_LANGFILE,nResID_M,wszBuf, sizeof(wszBuf));
        DrawTextWithProfile(m_pIShell, pIDisplay,RGB_WHITE_NO_TRANS,AEE_FONT_NORMAL, wszBuf, -1,0, 0, &rc,  IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
	}	

    IDisplay_Update(pIDisplay);
}

/*==============================================================================
函数: 
    DecodeAlphaString
       
说明: 
    本函数解码 Alpha 域字符串。
       
参数: 
    pdata [in]：指向输入数据字节序列。
    nLen [in]：输入数据字节序列长度,单位字节。
    wstrOut [out]： 输出Buffer
    nBufSize [in]:  输出Buffer大小,单位 AECHAR
       
返回值:
    none
       
备注:
       
==============================================================================*/
void DecodeAlphaString(byte *pdata, int nLen, AECHAR *wstrOut, int nBufSize)
{
    int nChars = 0,i;
    AECHAR  wBase;
    byte bTep;
    
    if ((NULL == pdata) || (NULL == wstrOut))
    {
        return;
    }
    
    if (nLen<=0 || nBufSize<=0)
    {
        return;
    }
    
    switch(pdata[0])
    {
        // Unicode 编码,无基址
        case 0x80:
            if (nBufSize*sizeof(AECHAR)>nLen)
            {
                uint16 *pUs;
                
                MEMCPY((void*)wstrOut, (void*)&pdata[1], nLen-1);
                pUs = wstrOut;
                for (i=0; i<(nLen-1)/2; i++)
                {
                   if ((*pUs == 0xFFFF) || (*pUs == 0x0000))
                   {
                       break;
                   }
                   *pUs = NTOHS(*pUs);
                   pUs++;
                }
                wstrOut[i] = 0;
            }
            break;
            
        // Unicode 编码1,定义了一个半基址:
        // 编码说明: 字节pdata[2]定义了16位二进制数wBase 0xxxxxxxx0000000中xxxxxxxx部分,
        //           若后续字节的最高位为1,则该字节后7位与wBase的和表示一个Unicode 编码
        //           字符,否则该字符为GSM default alphabet characters
        case 0x81:
            if (nLen>3)
            {
                nChars = pdata[1];
                wBase = pdata[2];
                wBase = wBase<<7;
                
                nChars = ((nChars <= nLen-3) ? nChars : (nLen-3));
                for (i=0; i<nChars && i<(nBufSize-1); i++)
                {
                    bTep = pdata[i+3];
                    if (bTep & 0x80)
                    {
                        wstrOut[i] = (bTep & 0x7F) + wBase;
                    }
                    else
                    {
                        wstrOut[i] = (AECHAR)GsmToUcs2[bTep];
                    }
                }
                wstrOut[i] = 0;
            }
            break;
        
        // Unicode 编码2,定义了一个基址: 
        // 编码说明: 字节pdata[2]、pdata[3]定义了一个16位Unicode 编码基址字符,
        //           若后续字节的最高位为1,则该字节后7位与wBase的和表示一个Unicode 编码
        //           字符,否则该字符为GSM default alphabet characters
        case 0x82:
            if (nLen>4)
            {
                nChars = pdata[1];
                
                MEMCPY((void*)&wBase, &pdata[2], sizeof(wBase));
                wBase = HTONS(wBase);
                
                nChars = ((nChars <= nLen-4) ? nChars : (nLen-4));
                for (i=0; i<nChars&& i<(nBufSize-1); i++)
                {
                    bTep = pdata[i+4];
                    if (bTep & 0x80)
                    {
                        wstrOut[i] = (bTep & 0x7F) + wBase;
                    }
                    else
                    {
                        wstrOut[i] = (AECHAR)GsmToUcs2[bTep];
                    }
                }
                wstrOut[i] = 0;
            }
            break;
            
        // 默认 GSM default alphabet characters only
        default:
            for (i=0; i<nLen && i<(nBufSize-1); i++)
            {
                if (pdata[i]==0xFF || pdata[i]==0x00)
                {
                    break;
                }
                wstrOut[i] = (AECHAR)GsmToUcs2[pdata[i]];
            }
            wstrOut[i] = 0;
            break;
    }
}

int Appscomm_is_incoming_state(int bb)
{
    static int g_is_incoming = 0;
    if (bb>=0)
    {
        g_is_incoming = bb;
    }
    return g_is_incoming;   
}

void SetArrowFlagonIM(boolean bFlag)
{
    gbArrowFlag = bFlag;
}
boolean GetArrowFlagonIM()
{
     return (gbArrowFlag);
}


/*==============================================================================
函数: 
    Appscommon_DrawDialogBoxFrame
       
说明: 
    绘制弹出对话框边框及背景色。
       
参数: 
    pBoxRect [in]: 输入将要绘制对话框的矩形范围。 
    bUpward [in]: 对话框箭头三角形是否在上方。
    nFrameColor [in]: 对话框边框颜色。
    nBgColor [in]: 对话框背景颜色
       
返回值:
    TRUE: 绘制成功
    FALSE: 绘制失败
       
备注:
       
==============================================================================*/
boolean Appscommon_DrawDialogBoxFrame(AEERect *pBoxRect, boolean bUpward, RGBVAL nFrameColor, RGBVAL nBgColor)
{
    AEERect        rect = *pBoxRect;
    int               nTriangleHeight = 4; /* 对话框箭头小三角高度*/
    int               nMaxXOffset = MAX(15, pBoxRect->dx/4); /* 箭头起点x轴方向最大偏移*/
    IShell           *pShell = AEE_GetShell();
    IGraphics      *pGraphic;
    AEETriangle   triangle;
    
    if (NULL == pShell)
    {
        return FALSE;
    }
    
    if(SUCCESS != ISHELL_CreateInstance(pShell, AEECLSID_GRAPHICS, (void **)&pGraphic))
    {
        return FALSE;
    }
    
    IGRAPHICS_SetFillMode(pGraphic, TRUE);
    /* 设置对话框背景颜色*/
    IGRAPHICS_SetFillColor(pGraphic, GET_RGB_R(nBgColor), GET_RGB_G(nBgColor), GET_RGB_B(nBgColor), 0);
    /* 设置对话框边框颜色*/
    IGRAPHICS_SetColor(pGraphic, GET_RGB_R(nFrameColor), GET_RGB_G(nFrameColor), GET_RGB_B(nFrameColor), 0);

    /* 修正对话框高度，剪掉箭头三角形高度*/
    if(bUpward)
    {
        rect.y += (nTriangleHeight - 1);
    }
    rect.dy -= (nTriangleHeight - 1);

    /* 绘制对话框主体背景*/
    IGRAPHICS_DrawRoundRectangle(pGraphic, &rect, 6, 6);

    triangle.x0 = MIN(pBoxRect->x + nMaxXOffset, pBoxRect->x + pBoxRect->dx/2 - nTriangleHeight/2);
    triangle.x1 = triangle.x0;
    triangle.x2 = triangle.x0 + nTriangleHeight;
    
    triangle.y0 = (bUpward) ? (pBoxRect->y) : (pBoxRect->y + pBoxRect->dy - 1);
    triangle.y1 = (bUpward) ? (triangle.y0 + nTriangleHeight) : (triangle.y0 - nTriangleHeight);
    triangle.y2 = triangle.y1;

    /* 绘制箭头三角形*/
    IGRAPHICS_DrawTriangle(pGraphic, &triangle);
    
    /* 这里需要把对话框背景颜色赋给边框遮掉边缘交界处线段*/
    IGRAPHICS_SetColor(pGraphic, GET_RGB_R(nBgColor), GET_RGB_G(nBgColor), GET_RGB_B(nBgColor), 0);
    
    SETAEERECT(&rect, triangle.x0, triangle.y1, nTriangleHeight + 1, 1);
    IGRAPHICS_DrawRect(pGraphic, &rect);
    
    if(pGraphic != NULL)
    {
        IGRAPHICS_Release(pGraphic);
        pGraphic = NULL;
    }
    return TRUE;
}

/*==============================================================================
函数: 
    Appscommon_DrawPopUpDialogBox
       
说明: 
    绘制弹出对话框(单行，含文本)。
       
参数: 
    pIDisplay [in]:
    x [in]: 输入将要绘制对话框左上角x坐标。
    y [in]: 输入将要绘制对话框左上角y坐标。
    strDisplay[in]:输入将要绘制对话的文本内容。
       
返回值:
    TRUE: 绘制成功
    FALSE: 绘制失败
       
备注:
    可以在函数调用之前使用IDISPLAY_SetColor设置文本颜色，最大显示长度为
    五分之四屏幕宽，显示起点x不小于五分之一屏幕宽，显示起点y不大于
    menu最低一条可显示的起始坐标， 不符的将自动调整。
       
==============================================================================*/
boolean Appscommon_DrawPopUpDialogBox(IDisplay *pIDisplay, int x, int y, AECHAR *strDisplay)
{
    AEERect                rect;
    boolean                 bUpward;
    int                        nLength = IDISPLAY_MeasureText(pIDisplay, AEE_FONT_BOLD, strDisplay) + 5;
    int                        nFontHeight = IDISPLAY_GetFontMetrics(pIDisplay, AEE_FONT_BOLD, NULL, NULL);
    int                        nDialogHeight = MENUITEM_HEIGHT;
    int                        xPos, yPos, minXOffset, maxYOffset;
    AEEDeviceInfo        di = {0};
    IShell                   *pShell = AEE_GetShell();
    
    if ((NULL == pShell) || (NULL == pIDisplay))
    {
        return FALSE;
    }
    
    ISHELL_GetDeviceInfo(pShell, &di);
    minXOffset = di.cxScreen/5;
    maxYOffset = di.cyScreen - BOTTOMBAR_HEIGHT - nDialogHeight;
    bUpward = (boolean)(y <= maxYOffset);
    nLength = MIN(di.cxScreen - minXOffset, nLength);
    xPos = MIN(di.cxScreen - nLength, MAX(minXOffset, x));
    yPos = (bUpward)?(y):(maxYOffset - nDialogHeight);
    
    SETAEERECT(&rect, xPos, yPos, nLength, nDialogHeight);
    
    if(Appscommon_DrawDialogBoxFrame(&rect, bUpward, MAKE_RGB(0xFF, 0x70, 0x00), RGB_WHITE) != TRUE)
    {
        return FALSE;
    }

    SETAEERECT(&rect, xPos + 3, yPos + (nDialogHeight - 3 - nFontHeight)/2 + 1, nLength - 5, nFontHeight);

    if(bUpward)
    {
        rect.y += 3;
    }
    
    IDISPLAY_DrawText(pIDisplay, 
                    AEE_FONT_BOLD, 
                    strDisplay, 
                    -1, 
                    rect.x, 
                    rect.y, 
                    &rect, 
                    IDF_TEXT_TRANSPARENT);
    
    return TRUE;
}
#if defined(FEATURE_VERSION_K212)||defined(FEATURE_QVGA_INHERIT_K212)
boolean Appscommon_DrawDigitalNumberImage(IDisplay *pDisplay, int number, int nLineWidth, AEERect *fontRect)
{
	IImage              *m_pImageTimeIcon;
    IShell      *pShell = AEE_GetShell();
	m_pImageTimeIcon = ISHELL_LoadResImage(pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDI_TIME_0+number);
	if(m_pImageTimeIcon!=NULL)
	{
	    MSG_FATAL("m_pImageTimeIcon is not null........",0,0,0);
		IIMAGE_Draw(m_pImageTimeIcon,fontRect->x,fontRect->y);
		(void)IIMAGE_Release(m_pImageTimeIcon);
        m_pImageTimeIcon = NULL;
	}
}

#endif
/*==============================================================================
函数: 
    Appscommon_DrawDigitalNumber
       
说明: 
    绘制仿七段数码管数字
       
参数: 
    pDisplay [in]: 
    number[in]: 需要绘制的数字。
    nLineWidth[in]: 需要绘制的数字线条宽度。
    fontRect [in]: 绘制数字的边界矩形框。
    fontColor[in]: 需要绘制的字体颜色。
       
返回值:
    FALSE: 绘制失败
    TRUE: 绘制成功
       
备注:
       
==============================================================================*/
boolean Appscommon_DrawDigitalNumber (IDisplay *pDisplay, int number, int nLineWidth, AEERect *fontRect, RGBVAL fontColor)
{
	if (NULL == pDisplay)
    {
        return;
    } 
    
    if(number > 9 || number < 0 || pDisplay == NULL)
    {
        return FALSE;
    }
    else
    {
        AEERect rect = {0};
        AEERect rect_temp = {0};
        AEERect rect_point = {0};
        int i = 0;
        int nWidth = nLineWidth;
        int nLength = fontRect->dx - 2;
        int nHeight = (fontRect->dy - 2)/2;
        int nHeight2 = (nWidth%2 == 0)? (nHeight - 1) : (nHeight);
        
        if(number == 0 ||
            number == 2 ||
            number == 3 ||
            number == 5 ||
            number == 6 ||
            number == 7 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x + 1, fontRect->y, nLength, 1);
            rect_temp.x = rect.x-2;
            rect_temp.y = rect.y-1;
            rect_temp.dx = rect.dx+4;
            rect_temp.dy = rect.dy;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x-1;
                rect_point.dx = 1;
                rect_point.y = rect.y;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.x = rect.x+rect.dx+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x++;
                rect.y++;
                rect.dx -= 2;
            }
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 2 ||
            number == 3 ||
            number == 4 ||
            number == 5 ||
            number == 6 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x + nWidth, fontRect->y + fontRect->dy - nHeight2 - 1 - (nWidth + 1)/2, nLength - 2*(nWidth - 1), 1);
            if(nWidth%2 == 0)
            {
                rect.y--;
            }
            //补偿中间横条的长度
            {
                rect.x -=  (nWidth - 1)/2;
                rect.dx +=  ((nWidth - 1)/2)*2;
            }
            rect_temp.x = rect.x+1;
            rect_temp.y = rect.y-1;
            rect_temp.dx = rect.dx-2;
            rect_temp.dy = rect.dy;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < ((nWidth + 1)/2); i++)
            {
                rect_point.x = rect.x-1;
                rect_point.dx = 1;
                rect_point.y = rect.y;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.x = rect.x+rect.dx+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.y += (nWidth - 2*i - 1);

                rect_point.x = rect.x-1;
                rect_point.dx = 1;
                rect_point.y = rect.y;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.x = rect.x+rect.dx+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.y -= (nWidth - 2*i - 2);
                rect.x--;
                rect.dx += 2;
            }
            rect.y += (nWidth - 2*i + 1);
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 0 ||
            number == 2 ||
            number == 3 ||
            number == 5 ||
            number == 6 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x + 1, fontRect->y + fontRect->dy - 1, nLength, 1);
            rect_temp.x = rect.x-2;
            rect_temp.y = rect.y+1;
            rect_temp.dx = rect.dx+4;
            rect_temp.dy = rect.dy;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x-1;
                rect_point.dx = 1;
                rect_point.y = rect.y;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.x = rect.x+rect.dx+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x++;
                rect.y--;
                rect.dx -= 2;
            }
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 0 ||
            number == 4 ||
            number == 5 ||
            number == 6 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x, fontRect->y + 1, 1, nHeight);
            rect_temp.x = rect.x-1;
            rect_temp.y = rect.y-2;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+4;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x;
                rect_point.dx = 1;
                rect_point.y = rect.y-1;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.y = rect.y+rect.dy+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x++;
                rect.y++;
                rect.dy -= 2;
            }
            rect_temp.x = rect.x;
            rect_temp.y = rect.y;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+1;
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 0 ||
            number == 2 ||
            number == 6 ||
            number == 8)
        {
            SETAEERECT(&rect, fontRect->x, fontRect->y + fontRect->dy - nHeight2 - 1, 1, nHeight2);
            if(number == 0 ||
                number == 1 ||
                number == 7)
            {
                if(nWidth%2 == 0 && fontRect->dy%2 != 0)
                {
                    rect.y--;
                    rect.dy++;
                }
            }
            rect_temp.x = rect.x-1;
            rect_temp.y = rect.y-2;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+4;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x;
                rect_point.dx = 1;
                rect_point.y = rect.y-1;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.y = rect.y+rect.dy+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x++;
                rect.y++;
                rect.dy -= 2;
            }
            rect_temp.x = rect.x;
            rect_temp.y = rect.y;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+1;
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 0 ||
            number == 1 ||
            number == 2 ||
            number == 3 ||
            number == 4 ||
            number == 7 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x + nLength + 1, fontRect->y + 1, 1, nHeight);
            rect_temp.x = rect.x+1;
            rect_temp.y = rect.y-2;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+4;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x;
                rect_point.dx = 1;
                rect_point.y = rect.y-1;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.y = rect.y+rect.dy+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x--;
                rect.y++;
                rect.dy -= 2;
            }
            rect_temp.x = rect.x;
            rect_temp.y = rect.y;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+1;
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
        if(number == 0 ||
            number == 1 ||
            number == 3 ||
            number == 4 ||
            number == 5 ||
            number == 6 ||
            number == 7 ||
            number == 8 ||
            number == 9)
        {
            SETAEERECT(&rect, fontRect->x + nLength + 1, fontRect->y + fontRect->dy - nHeight2 - 1, 1, nHeight2);
            if(number == 0 ||
                number == 1 ||
                number == 7)
            {
                if(nWidth%2 == 0 && fontRect->dy%2 != 0)
                {
                    rect.y--;
                    rect.dy++;
                }
            }
            rect_temp.x = rect.x+1;
            rect_temp.y = rect.y-2;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+4;
            IDISPLAY_FillRect(pDisplay, &rect_temp, MAKE_RGB(0,0,0));
            for(i = 0; i < nWidth; i++)
            {
                rect_point.x = rect.x;
                rect_point.dx = 1;
                rect_point.y = rect.y-1;
                rect_point.dy = 1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                rect_point.y = rect.y+rect.dy+1;
                IDISPLAY_FillRect(pDisplay, &rect_point, MAKE_RGB(0,0,0));
                IDISPLAY_FillRect(pDisplay, &rect, fontColor);
                rect.x--;
                rect.y++;
                rect.dy -= 2;
            }
            rect_temp.x = rect.x;
            rect_temp.y = rect.y;
            rect_temp.dx = rect.dx;
            rect_temp.dy = rect.dy+1;
            IDISPLAY_FillRect(pDisplay, &rect, MAKE_RGB(0,0,0));
        }
    }
    return TRUE;
}
/*==============================================================================
函数: 
    Appscommon_CompareName

说明: 
    本函数用于比较两个字符串以便排序。

参数: 
    wstrNam1 [in]: 字符串1。
    wstrNam2 [in]: 字符串2。
返回值: 
    1 : wstrNam1>wstrNam2
    0 : wstrNam1==wstrNam2
    -1: wstrNam1<wstrNam2

备注:
    如果是电话本调用 OEM 层在初始化 Cache 时会调用此函数来对 Cache 排序。UI 层不必再排序。
    其他模块调用需自行排序

==============================================================================*/
int Appscommon_CompareName(AECHAR *wstrNam1,AECHAR *wstrNam2)
{

    AECHAR  alpha1[MAX_STR +1]={0,};
    AECHAR  alpha2[MAX_STR +1]={0,};
    AECHAR  wTemp = 0;

     if (wstrNam1 && !wstrNam2)
    {
        return(1);
    }
    
    if (!wstrNam1 && wstrNam2)
    {
        return(-1);
    }
    
    if (!wstrNam1 && !wstrNam2)
    {
        return(0);
    }
    

    // Convert to letter
    WStr2Letter(alpha1, MAX_STR, wstrNam1);
    ALKWSTRLOWER(alpha1); //brew 自带函数遇到字符串中含有0xA1的字符时会重启
    
    // Note:为了区分 ASCII 码和中文字符这里对第一个转换字符作了偏移量的处理
    //      以区分ASCII 码和中文
    if (wstrNam1[0] > OFFSET_ASCII)
    {
        // CHN Unicode
        wTemp = wstrNam1[0];
    }
    
    // Convert to letter
    WStr2Letter(alpha2, MAX_STR, wstrNam2);
    ALKWSTRLOWER(alpha2);   //brew 自带函数遇到字符串中含有0xA1的字符时会重启
    
    // Note:为了区分 ASCII 码和中文字符这里对第一个转换字符作了偏移量的处理
    //      以区分ASCII 码和中文
    if (wstrNam2[0] > OFFSET_ASCII)
    {
        if (wTemp)
        {
            // CHN Unicode vs CHN Unicode
            if (alpha1[0] == alpha2[0])
            {
                // 如果首字母相等，则按笔划次序排列
                if (wTemp > wstrNam2[0])
                {
                    return(1);
                }
                else if (wTemp < wstrNam2[0])
                {
                    return(-1);
                }
                else
                {
                    // Continue to cmp
                }
            }
        }
        else
        {
            // ASCII vs CHN Unicode
#if defined(FEATURE_CARRIER_SUDAN_SUDATEL)
            return (1);
#endif
            return (-1);
        }
    }
    else
    {
        if (wTemp)
        {
            // CHN Unicode vs ASCII
#if defined(FEATURE_CARRIER_SUDAN_SUDATEL)
            return (-1);
#endif
            return(1);
        }
    }
    
    return WSTRCMP(alpha1, alpha2);
}
static void ALKWSTRLOWER(AECHAR * pszDest)
{
   AECHAR   ch;

   if(pszDest)
   {
        while((ch = *pszDest) != (AECHAR)0)
       {
              if(ch >= (AECHAR)'A' && ch <= (AECHAR)'Z') 
             {
                     ch += (AECHAR)('a' - 'A');
              }
              else if(ch >= CON_ACCENT_UPPER_START && ch <= CON_ACCENT_UPPER_END)
		      {          
                     ch += (AECHAR)(CON_ACCENT_LOWER_START - CON_ACCENT_UPPER_START);
              }
              else 
              {
                break;
              }
               *pszDest = ch;
                pszDest++;
         }
   }
   else
   {
   	   return;
   }
}


int GetSingleNumberWidth(IDisplay *pDisplay, AEEFont Font)
{
    AECHAR testNumber[] = {'0','\0'};
    int nNumberWidth = 0;

	if (NULL == pDisplay)
    {
        return;
    } 
    
    nNumberWidth = IDISPLAY_MeasureText(pDisplay, Font, testNumber);

    return nNumberWidth;
}
/*==============================================================================
函数：
       IsDoubleBytesIME

说明：
       将给定消息格式化已显示给用户。

参数：

返回值：

备注：:
        
==============================================================================*/
boolean IsDoubleBytesIME(AEETextInputMode imnputmode)
{
    switch(imnputmode)
    {
#if defined(FEATURE_LANG_CHINESE)
    case AEE_TM_PINYIN:
    case AEE_TM_STROKE:
    case AEE_TM_RAPID:
        return TRUE;
#endif// defined(FEATURE_LANG_CHINESE)

#if defined(FEATURE_LANG_ARABIC)
    case AEE_TM_ARABIC:
    case AEE_TM_ARABIC_R:
        return TRUE;
#endif// defined(FEATURE_LANG_ARABIC)

#if defined(FEATURE_LANG_HEBREW)
    case AEE_TM_HEBREW:
    case AEE_TM_HEBREW_R:
        return TRUE;
#endif// defined(FEATURE_LANG_HEBREW)

#if defined(FEATURE_LANG_HINDI)
    case AEE_TM_HINDI:
        return TRUE;
#endif// defined(FEATURE_LANG_HINDI)

#if defined(FEATURE_LANG_RUSSIAN)
    case AEE_TM_RUSSIAN:
    case AEE_TM_RUSSIAN_R:
        return TRUE;
#endif// defined(FEATURE_LANG_RUSSIAN)

#if defined(FEATURE_LANG_THAI)
    case AEE_TM_THAI:
    case AEE_TM_THAI_R:
        return TRUE;
#endif// defined(FEATURE_LANG_THAI)

#if defined(FEATURE_LANG_SPANISH)
    case AEE_TM_SPANISH_EU:
    case AEE_TM_SPANISH_EU_R:
    case AEE_TM_SPANISH_LA:
    case AEE_TM_SPANISH_LA_R:
        return TRUE;
#endif// defined(FEATURE_LANG_SPANISH)

#if defined(FEATURE_LANG_PORTUGUESE)
    case AEE_TM_PORTUGUESE_BR:
    case AEE_TM_PORTUGUESE_BR_R:
    case AEE_TM_PORTUGUESE_EU:
    case AEE_TM_PORTUGUESE_EU_R:
        return TRUE;
#endif// defined(FEATURE_LANG_PORTUGUESE)

#if defined(FEATURE_LANG_ITALIAN)
    case AEE_TM_ITALIAN:
    case AEE_TM_ITALIAN_R:
        return TRUE;
#endif// defined(FEATURE_LANG_ITALIAN)

#if defined(FEATURE_LANG_ROMANIAN)
    case AEE_TM_ROMANIAN:
    case AEE_TM_ROMANIAN_R:
        return TRUE;
#endif// defined(FEATURE_LANG_ROMANIAN)

#if defined(FEATURE_LANG_VIETNAMESE)
    case AEE_TM_VIETNAMESE:
    case AEE_TM_VIETNAMESE_R:
        return TRUE;
#endif// defined(FEATURE_LANG_VIETNAMESE)

//-------------------------------------------------------------------
//             Single bytes
//-------------------------------------------------------------------
#if defined(FEATURE_INPUTMODE_INDONESIAN)
    case AEE_TM_INDONESIAN:
    case AEE_TM_INDONESIAN_R:
        return FALSE;
#endif // defined(FEATURE_INPUTMODE_INDONESIAN)

    case AEE_TM_NUMBERS:
    case AEE_TM_LETTERS:
    case AEE_TM_EZTEXT:
    //case AEE_TM_ENGLISH_UK:
    //case AEE_TM_ENGLISH_UK_R:
    //case AEE_TM_ENGLISH_US:
    //case AEE_TM_ENGLISH_US_R:
    default:
        return FALSE;
    }
}


/*==============================================================================
函数: 
    HaveNoneASCIIChar

说明: 
    判断宽字符串中是否含有非ASCII字符。

参数: 
    pWstr [in]: 指向宽字符串。
    nPostion [out]: 非ASCII字符第一次出现的位置。

返回值: 
    若含有非ASCII字符，返回TRUE；否则，FALSE。

备注: 

==============================================================================*/
boolean HaveNoneASCIIChar ( AECHAR * pWstr, int * pPostion)
{
    int i ,len;
    AECHAR *pWstrTep = pWstr;

    if (NULL == pWstrTep)
    {
        return FALSE;
    }
    
    len = WSTRLEN(pWstrTep);
    if (len <= 0)
    {
        return FALSE;
    }
#if defined(FEATURE_SMS_GSM7BIT)
    for (i=0; ((i < len) && ((*pWstr) < UCS2_TO_GSM_TABLE_SIZE)); i++,pWstr++)
#else
    for (i=0; ((i < len) && ((*pWstr) < 128)); i++,pWstr++)
#endif    
    {
        //ERR("i='%c',=%d",*pWstr,*pWstr,0);//Azlun
        ;
    }
    
    if (i < len)
    {
        if (NULL != pPostion)
        {
            *pPostion = i;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*==============================================================================
函数: 
    Appscommon_ResetBackground

说明: 
    绘制某块区域的背景图片。

参数: 
    pDisplay [in]: 
    BgImage [in]: 背景图片指针
    BgColor [in]: 背景颜色
    rect [in]: 需要绘制的矩形范围
    x,y[in]: 图片左上角在屏幕中的起点横坐标与纵坐标
    
返回值: 
    无

备注:
    如果背景图片不为空，则绘制图片在指定矩形范围，
    否则将指定范围填充为背景色。

==============================================================================*/
void Appscommon_ResetBackground(IDisplay *pDisplay, IImage *BgImage, RGBVAL BgColor, AEERect * rect, int x, int y)
{
    if(pDisplay == NULL)
    {
        return;
    }
    
    if(NULL != BgImage)
    {
        AEERect oldClip = {0};
        
        IDisplay_GetClipRect(pDisplay, &oldClip);
        IDisplay_SetClipRect(pDisplay, rect);
        IImage_Draw(BgImage, x, y);
        IDisplay_SetClipRect(pDisplay, &oldClip);
    }
    else
    {
        IDisplay_FillRect(pDisplay, rect, BgColor);
    }
}

void Appscommon_ResetBackground_Start(IDisplay *pDisplay, IImage *BgImage, RGBVAL BgColor, AEERect * rect, int x, int y)
{
    if(pDisplay == NULL)
    {
        return;
    }
    
    if(NULL != BgImage)
    {
        AEERect oldClip = {0};
        
        //IDisplay_GetClipRect(pDisplay, &oldClip);
        //IDisplay_SetClipRect(pDisplay, rect);
        IIMAGE_SetOffset(BgImage,x,rect->y);
        IImage_SetDrawSize(BgImage,rect->dx,rect->dy);
        IImage_Start(BgImage, x, rect->y);
         // 设置动画速度(毫秒)
        IIMAGE_SetAnimationRate(BgImage, 600);
        IIMAGE_SetParm(BgImage, IPARM_PLAYTIMES, 1, 0);

        // 设置图像中的帧数
        IIMAGE_SetFrameCount(BgImage, 1);
        //IDisplay_SetClipRect(pDisplay, &oldClip);
    }
    else
    {
        IDisplay_FillRect(pDisplay, rect, BgColor);
    }
}

/*==============================================================================
函数: 
    Appscommon_DrawScrollBar

说明: 
    绘制滚动条。

参数: 
    pDisplay [in]: 
    nCurrentIdx [in]: 当前页面最下面一项索引值(以1为起点)
    nTotalItem [in]: 项目总数
    nPageItem[in]: 单屏幕最大显示项目数
    ScrollRect [in]: 需要绘制的矩形范围
    
返回值: 
    无

备注:

==============================================================================*/
#include "msg.h"
void Appscommon_DrawScrollBar(IDisplay *pDisplay, int nCurrentIdx, int nTotalItem, int nPageItem, AEERect *ScrollRect)
{
    AEERect rctThumb, rectScrollBar = *ScrollRect;
    int   x, y, dx, dy, nRange;
    RGBVAL cScrollbar = MAKE_RGB(0xDE, 0xDE, 0xDE), 
    cScrollbarFill = /*MAKE_RGB(0xFF, 0xC0, 0x60)*/MAKE_RGB(0xFF, 0x70, 0x00), 
               cBackGround = APPSCOMMON_BG_COLOR;

    if(nTotalItem <= nPageItem || pDisplay == NULL)
    {
        return;
    }
#ifdef FEATURE_RANDOM_MENU_COLOR
    OEM_GetConfig(CFGI_MENU_BGCOLOR, &cBackGround, sizeof(cBackGround));
#endif

    IDISPLAY_FillRect(pDisplay, ScrollRect, cBackGround);
    rectScrollBar.x += 2*AEE_FRAME_SIZE;
    rectScrollBar.dx -= 2*AEE_FRAME_SIZE;
#ifdef FEATURE_SCROLLBAR_USE_STYLE
    nRange = ScrollRect->dy - 4*AEE_FRAME_SIZE;
    y = ScrollRect->y + 2*AEE_FRAME_SIZE;
    x = rectScrollBar.x;
    dx = rectScrollBar.dx;
#else
    nRange = ScrollRect->dy - 2*AEE_FRAME_SIZE;
    y = ScrollRect->y + AEE_FRAME_SIZE;
    x = rectScrollBar.x + AEE_FRAME_SIZE;
    dx = rectScrollBar.dx - 2*AEE_FRAME_SIZE;
#endif
    dy = MAX(3,(nRange * nPageItem)/nTotalItem);
    if(nCurrentIdx > nPageItem)
    {
        y = MIN(y + nRange - dy, y + ((nRange - dy) * (nCurrentIdx - nPageItem))/(nTotalItem - nPageItem));
    }
    SETAEERECT(&rctThumb, x, y, dx, dy);
    
#ifdef FEATURE_SCROLLBAR_USE_STYLE
    IDISPLAY_FillRect(pDisplay, &rectScrollBar, cScrollbar);
    IDISPLAY_FillRect(pDisplay, &rctThumb, cScrollbarFill);
    {
        AEERect rectScrollbarDark;
        RGBVAL  cScrollbarDark = MAKE_RGB(0xFF, 0x70, 0x00);
        
        x += AEE_FRAME_SIZE;
        y -= AEE_FRAME_SIZE;
        dx -= 2*AEE_FRAME_SIZE;
        dy += 2*AEE_FRAME_SIZE;
        SETAEERECT(&rectScrollbarDark, x, y, dx, dy);
        IDISPLAY_FillRect(pDisplay, &rectScrollbarDark, cScrollbarDark);
    }
#else
    IDISPLAY_DrawFrame(pDisplay, &rectScrollBar, AEE_FT_BOX, cScrollbar);
    IDISPLAY_FillRect(pDisplay, &rctThumb, cScrollbarFill);
#endif
}
#ifdef FEATURE_LANG_CHINESE
AECHAR CNUnicode2Letter(AECHAR c)
{
    CNUni2Letter(c);
    
    return c;
}

/*=============================================================================

Macro : Letter2Alpha

DESCRIPTION: 
    将英文字母转换为对应的按键字符
    
PARAMETERS:
    letter:
    
=============================================================================*/
AECHAR CNLetter2Alpha(AECHAR letter)                                        
{                                                           
    AECHAR  temp[2] = {'\0', '\0'};                         
    temp[0] = letter;                                       
    WSTRLOWER(temp);                                        
    letter = temp[0];                                       
    if(letter >= 'a' && letter <= 'z')                      
    {                                                       
        letter = (AECHAR)letter2alphaTable[letter -'a'];    
    }       

    return letter;
}

#endif
/*==============================================================================
函数: 
    Appscommon_GetRandomColor

说明: 
    绘制滚动条。

参数: 
    nMaxVal [in]: 像素平均值最大值，用来限制获取色彩的亮度，
    如不需要限制，设置为255即可
    
返回值: 
    获得的颜色RGB值

备注:

==============================================================================*/
RGBVAL Appscommon_GetRandomColor(uint32 nMaxVal) 
{
    byte          nRGBVal[3];
    RGBVAL      nColor;

    GETRAND(nRGBVal, sizeof(nRGBVal));
    
    if(nRGBVal[0] + nRGBVal[1] + nRGBVal[2] > nMaxVal*3)
    {
        nRGBVal[0] = (nRGBVal[0] > nMaxVal)?(nRGBVal[0]*nMaxVal/255):(nRGBVal[0]);
        nRGBVal[1] = (nRGBVal[1] > nMaxVal)?(nRGBVal[1]*nMaxVal/255):(nRGBVal[1]);
        nRGBVal[2] = (nRGBVal[2] > nMaxVal)?(nRGBVal[2]*nMaxVal/255):(nRGBVal[2]);
    }

    nColor = MAKE_RGB((uint32)nRGBVal[0], (uint32)nRGBVal[1], (uint32)nRGBVal[2]);
    return nColor;
}

/*==============================================================================
函数: 
    Appscommon_ResetBackgroundEx

说明: 
    绘制某块区域的背景色以及图片。

参数: 
    pDisplay [in]: 
    rect [in]: 需要绘制的矩形范围
    bDrawImage[in] : 是否绘制图片，FALSE仅填充背景色
    
返回值: 
    无

备注:
    将指定范围填充为默认的背景色APPSCOMMON_BG_COLOR，如果传入参数为TRUE时，
    绘制默认背景图片。

==============================================================================*/
void Appscommon_ResetBackgroundEx(IDisplay *pDisplay, AEERect * rect, boolean bDrawImage)
{
    RGBVAL nBgColor = APPSCOMMON_BG_COLOR;

    if(pDisplay == NULL)
    {
        return;
    }
    
#ifdef FEATURE_RANDOM_MENU_COLOR
    (void)OEM_GetConfig(CFGI_MENU_BGCOLOR, &nBgColor, sizeof(nBgColor));
#endif
        
    if(!bDrawImage)
    {
        IDisplay_FillRect(pDisplay, rect, nBgColor);
    }
    else
    {
        IImage *pImageBg = NULL;
        IShell  *pShell = AEE_GetShell();
        int      xPos = 0, yPos = 0;
#ifdef FEATURE_RANDOM_MENU_COLOR
        byte     nRandomMenu = 0;

        (void)OEM_GetConfig(CFGI_RANDOM_MENU, (void*)&nRandomMenu, sizeof(nRandomMenu));
		#if 0
        if(nRandomMenu != 0)
        {
            /* 有透明通道的图片尽量减小尺寸，仅留下有颜色的部分，起始坐标重设*/
            xPos = APPSCOMMON_MENUBG_XPOS;
            yPos = APPSCOMMON_MENUBG_YPOS;
            pImageBg = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MENU_BACKGROUND_TRANS);
            IDisplay_FillRect(pDisplay, rect, nBgColor);
        }
        else
	    #endif
#endif
        {
            /* 由于透明通道会大大降低图像的显示速度，默认使用一张无透明色的图片*/
#ifndef FEATURE_VERSION_NO_BG
            pImageBg = ISHELL_LoadResImage(pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);  //moci by yangdecai
#endif            
        }

        Appscommon_ResetBackground(pDisplay, pImageBg, nBgColor, rect, xPos, yPos);

        if(pImageBg != NULL)
        {
            IImage_Release(pImageBg);
            pImageBg = NULL;
        }
    }
}
/*******************************************************************************
**
** Function         Appscommon_bluetooth_used_sd
**
** Description
**                  
**
** Returns          int
*******************************************************************************/
int Appscommon_bluetooth_used_sd(int aa)
{
#ifdef FEATURE_SUPPORT_BT_APP
    static int g_bluetoothused_sd = 0;
    if (aa>=0)
    {
        g_bluetoothused_sd = aa;
    }
    return g_bluetoothused_sd;
#else
    return 0;
#endif
}
/*==============================================================================
函数:
    IPAdd_FromNetValueToString

说明:
    函数将IP地址从网络值转换为字符串形式。

参数:
    dwAddNet [in]：IP地址网络值。
    pszbuf [in/out]：返回结果的 Buffer。
    nSize [in]：Buffer 大小。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean IPAdd_FromNetValueToString(uint32 dwAddNet, char *pszbuf, int nSize)
{
    uint8 b1, b2, b3, b4;
    
    if (!pszbuf || (nSize < 16))
    {
        return FALSE;
    }
    
    MEMSET(pszbuf, 0, nSize);
    
    b4 = (dwAddNet >> 24);
    b3 = (dwAddNet >> 16) & 0xff;
    b2 = (dwAddNet >> 8) & 0xff;
    b1 = dwAddNet & 0xff;
    
    SPRINTF(pszbuf, "%d.%d.%d.%d", b1, b2, b3, b4);
    
    return TRUE;
}

/*==============================================================================
函数:
    IPAdd_FromStringToNetValue

说明:
    函数将IP地址从字符串形式转换为网络值。

参数:
    pdwAddNet [in/out]：返回IP地址网络值的 Buffer。
    pszbuf [in]：字符串形式的IP地址。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean IPAdd_FromStringToNetValue(uint32 *pdwAddNet, char *pszbuf)
{
    int     nByte = 0;
    char    c;
    char    *pstrTep = NULL;   
    
    if (!pdwAddNet || !pszbuf)
    {
        return FALSE;
    }
    pstrTep = pszbuf;
    c = *pstrTep;

    while ((c >= '0') && (c <= '9')) 
    {
        int n = 0;
        
        while ((c >= '0') && (c <= '9'))
        {
            n = n*10 + (c - '0');
            ++pstrTep;
            c = *pstrTep;
        }
    
        ((char*)pdwAddNet)[nByte++] = n;
    
        if (nByte == 4 || *pstrTep != '.')
        {
            break;
        }
    
        ++pstrTep;
        c = *pstrTep;
    }
    
    c = *pstrTep;

    if (nByte < 4 || 
        ((c >= '0') && (c <= '9')) ||
        (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))))  
    {
        *pdwAddNet = 0xFFFFFFFF;    // Invalid address
        return FALSE;
    }

    return TRUE;
}

/*==============================================================================
函数:
    Appscommon_FormatTimeString

说明:
    函数将时间转换为字符串显示格式。

参数:
    nSeconds [in]：需要转换的时间。
    wstrTime [in]：输出字符串缓存地址。
    nTimeStringLength[in]：输出字符串缓存区长度。

返回值:
    TRUE: 转换成功
    FALSE:转换失败

备注:

==============================================================================*/
boolean Appscommon_FormatTimeString(uint32 nSeconds, AECHAR* wstrTime, int nTimeStringLength)
{
    if( NULL == wstrTime || nTimeStringLength < 9*(sizeof(AECHAR)))
    {
        return FALSE;
    }
    else
    {
        JulianType      julianInfo       = {0};
        AECHAR         formatString[16] = {0};
        byte              timeFormatType      = 0;

        if(nSeconds > 0)
        {
            GETJULIANDATE( nSeconds, &julianInfo);
        }
        MEMSET( formatString, 0, sizeof(formatString));

        OEM_GetConfig( CFGI_TIME_FORMAT, &timeFormatType, sizeof( byte));
        
#if defined( AEE_SIMULATOR)
        timeFormatType = OEMNV_TIMEFORM_AMPM;
#endif

        if (timeFormatType == OEMNV_TIMEFORM_AMPM)
        {
            if( julianInfo.wHour >= 12)
            {
                STRTOWSTR( "%02d:%02d PM", formatString, sizeof( formatString));
            }
            else
            {
                STRTOWSTR( "%02d:%02d AM", formatString, sizeof( formatString));
            }

            julianInfo.wHour = (julianInfo.wHour > 12) ? (julianInfo.wHour - 12) : julianInfo.wHour;
        }
        else
        {
            STRTOWSTR( "%02d:%02d", formatString, sizeof( formatString));
        }

        // 格式化时间字符串
        MEMSET( wstrTime, 0, nTimeStringLength);
        WSPRINTF( wstrTime, nTimeStringLength, formatString, julianInfo.wHour, julianInfo.wMinute);

        return TRUE;
    }
}

/*==============================================================================
函数: 
    app_media_scheduler
       
说明: 
    判断当前需要的多媒体设备(VC0848)是否已被占用
       
参数: 
    none
返回值:
    APP_MEDIA_ALLOW:            允许使用
    APP_MEDIA_IMPACT_BY_FM:     已被FM占用
    APP_MEDIA_IMPACT_BY_MP3:    已被MP3占用
       
备注:
    
==============================================================================*/
int app_media_scheduler(void)
{
    boolean b_FMBackground = FALSE;
  
    OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
    if((TRUE == b_FMBackground) && (AEECLSID_APP_FMRADIO != ISHELL_ActiveApplet(AEE_GetShell())))
    {
        return APP_MEDIA_IMPACT_BY_FM;
    }
#ifdef FEATURE_APP_MUSICPLAYER    
    else if(IsMp3PlayerStatusOnBG()) 
    {
        return APP_MEDIA_IMPACT_BY_MP3;        
    }
#endif    
    return APP_MEDIA_ALLOW;


}
