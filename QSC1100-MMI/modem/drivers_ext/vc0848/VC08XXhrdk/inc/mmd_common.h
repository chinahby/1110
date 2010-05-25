
#ifndef _MMD_COMMON_H_
#define _MMD_COMMON_H_

#include "MMD_COMMON_TypeDef.h"
#include "MMD_COMMON_Debug.h"
#include "MMD_USER_Depend.h"
#include "MMD_COMMON_CommandLists.h" 

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MMD_RET_NOERR           0x00000000
#define MMD_RET_PROCESSING      0xF0000001
#define MMD_RET_INVALIDPARAM    0xF0000002
#define MMD_RET_NORESOURCE      0xF0000003
#define MMD_RET_TIMEOUT         0xF0000004
#define MMD_RET_MODEERR         0xF0000005
#define MMD_RET_DOWNCODE_ERR        0xF0000006
#define MMD_RET_TEST_REG_ERR        0xF0000007
#define MMD_RET_TEST_SRAM_ERR   0xF0000008
#define MMD_RET_TEST_SDRAM_ERR  0xF0000009
#define MMD_RET_BIU_ERR             0xF000000A
#define MMD_RET_LIF_ERR             0xF000000B
#define MMD_RET_MALLOC_ERR             0xF000000C
#define MMD_RET_FREE_ERR             0xF000000D
#define MMD_RET_HARD_INIT            0xF000000E
#define MMD_RET_PARA_ERR            0xF000000F
#define MMD_RET_RESTART_ERR        0xF0000010
#define MMD_RET_SCENE_ERR 			0xF0000011
#define MMD_RET_DEVICENOSPACE		0xF0000012
#define MMD_RET_ERR             0xF000FFFF



#if(1)//def MMD_ENDIAN_LITTLE
/* 0x1234 to 0x1234 */
#define MMD_ENDIAN16(Value16)  (Value16)
/* 0x12345678 to 0x12345678 */
#define MMD_ENDIAN32(Value32)  (Value32)
#else /*MMD_ENDIAN_LITTLE*/
/* 0x1234 to 0x3412 */
#define MMD_ENDIAN16(Value16)  ((((Value16)&0xFF00)>>8)|\
                                (((Value16)&0x00FF)<<8))
/* 0x12345678 to 0x12345678 */
#define MMD_ENDIAN32(Value32)  ((((Value32)&0x000000FF)<<24)|\
                                (((Value32)&0x0000FF00)<< 8)|\
                                (((Value32)&0x00FF0000)>> 8)|\
                                (((Value32)&0xFF000000)>>24))
#endif /*MMD_ENDIAN_LITTLE*/

#endif /*_MMD_COMMON_H_*/

