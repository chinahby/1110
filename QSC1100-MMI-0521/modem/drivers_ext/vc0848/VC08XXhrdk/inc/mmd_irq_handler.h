/* All rights reserved, Copyright (C) Vimicro LIMITED 2003-2004                                             */
/*----------------------------------------------------------------------------------------------------------*/
/*! @file    MMD_IRQ_Handler.h
    @brief   ARM IRQ controller
    @author  Zhang Hua                                                                      */
/*----------------------------------------------------------------------------------------------------------*/
#ifndef _MMD_IRQ_HANDLER_H_
#define _MMD_IRQ_HANDLER_H_

#include "MMD_Common.h"

MMD_U32 MMD_IRQ_Handler_Top(void);
MMD_U32 MMD_Check_Irq_Staus_And_Wait(MMD_U32 time);
void V820_interrupt_acknowledge(MMD_U32 vector);
extern volatile MMD_U32 g_Ack_MinCmd;
extern volatile MMD_U32 g_Ack_OP1;
extern volatile MMD_U32 g_Ack_OP2;

#endif

