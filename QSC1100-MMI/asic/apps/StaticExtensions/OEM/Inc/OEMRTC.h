#ifndef OEMRTC_H
#define OEMRTC_H

/*======================================================
FILE:  OEMRTC.h

SERVICES:  OEMRTC member functions

GENERAL DESCRIPTION:


        Copyright ?1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMRTC.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
=============================================================================*/
/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/
typedef void (*time_list_cb)(void * p_data);

enum
{
    OEMRTC_SET_POWER_DOWN = 0x00,
    OEMRTC_SET_POWER_ON,
    OEMRTC_SET_ALARM_1,
    OEMRTC_SET_ALARM_2,
    OEMRTC_SET_ALARM_3,
    OEMRTC_SET_ALARM_4,
    OEMRTC_SET_ALARM_5,
    OEMRTC_SET_ALARM_6
};


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
boolean OEMRTC_Add_Time_To_Timer_List(uint32 time ,byte timer_sig, time_list_cb cb, void * p_data);
boolean OEMRTC_Del_Time_Node(byte del_sig);
void    OEMRTC_Run_Call_Back_Function(void);
void *  OEMRTC_hs_bsp_test_rtc_get(void);
//void    OEMRTC_Process_User_Power_Off(void);
void    OEMRTC_Free_All_Node(void);
//void    OEMRTC_hs_bsp_test_rtc_set(void *m_time);
#ifdef FEATRUE_AUTO_POWER
boolean OEMRTC_Process_Auto_Power_Down(IShell *pShell);
boolean OEMRTC_Process_Auto_Power_On(void);
#endif //#ifdef FEATRUE_AUTO_POWER

boolean OEMRTC_Process_PowerDown_Alarm(void);
void      OEMRTC_Phone_Reset(void *ppp);

boolean      OEMRTC_Get_Current_Time(uint32 *get_time);
#endif //OEMRTC_H

