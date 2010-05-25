/*============================================================================================================*/
/*    All rights reserved, Copyright (C) vimicro LIMITED 2003-2004 */
/*----------------------------------------------------------------------------------------------------------*/
/*! @file    COMMON_typedef.h                                                                                     
    @brief   common typedef file                                                                             
    @author Hideaki MIYASHITA
*/
/*============================================================================================================*/
#ifndef _COMMON_TYPEDEF_
#define _COMMON_TYPEDEF_

#include "mmd_data_type.h"

#define  HUGE               
#define  SHUGE              

typedef struct _CMD_ACK_FORMAT
{
    MMD_U32 flag;
    MMD_U32 cmd;
    MMD_U32 op1;
    MMD_U32 op2;
}CMD_ACK_FORMAT, *pCMD_ACK_FORMAT;
typedef MMD_U32 (*ModeCallback)(MMD_U32 cmd);

#ifndef BITDEF
#define BITDEF

enum BitDef
{
    BIT0    =   0x00000001,
    BIT1    =   0x00000002,
    BIT2    =   0x00000004,
    BIT3    =   0x00000008,
    BIT4    =   0x00000010,
    BIT5    =   0x00000020,
    BIT6    =   0x00000040,
    BIT7    =   0x00000080,
    BIT8    =   0x00000100,
    BIT9    =   0x00000200,
    BIT10   =   0x00000400,
    BIT11   =   0x00000800,
    BIT12   =   0x00001000,
    BIT13   =   0x00002000,
    BIT14   =   0x00004000,
    BIT15   =   0x00008000,
    BIT16   =   0x00010000,
    BIT17   =   0x00020000,
    BIT18   =   0x00040000,
    BIT19   =   0x00080000,
    BIT20   =   0x00100000,
    BIT21   =   0x00200000,
    BIT22   =   0x00400000,
    BIT23   =   0x00800000,
    BIT24   =   0x01000000,
    BIT25   =   0x02000000,
    BIT26   =   0x04000000,
    BIT27   =   0x08000000,
    BIT28   =   0x10000000,
    BIT29   =   0x20000000,
    BIT30   =   0x40000000
};
#define    BIT31   =   0x80000000
#endif
#endif
