#ifndef PARBI_H_
#define PARBI_H_
/*===========================================================================

        Pacing Arbiter   F O R   Q S C 6 0 5 5

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the PARB functions for the QSC6055.

REFERENCES

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/parb/parbi.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/07   hvo     Ported to QSC1100 from QSC6055, QSC1100 has less AHB bus
                   and masters
===========================================================================*/

/*---------------------------------------------------------------------------

                    MACRO KICK_WATCHDOG

This macro resets the watchdog timer circuit.
---------------------------------------------------------------------------*/
#define KICK_WATCHDOG()                                               \
            HWIO_OUT(WDOG_RESET, HWIO_FMSK(WDOG_RESET,WATCH_DOG));


/*--------------------Ahb Bus Type Enum----------------------------------------------*/

typedef enum {
    SYS_BUS,
    APP_BUS,
    INVALID_BUS
} bus_type;

/*--------------------Ahb Bus Masters Enum-------------------------------------------*/

/* AHB SYS BUS MASTER, ID */
typedef enum {
    SYS_BUS_MASTER__ARMI = 0,
    SYS_BUS_MASTER__ARMD = 1,
    SYS_BUS_MASTER__TIC = 2,

    SYS_BUS_MASTER__INVALID = -1
} sys_bus_master_type;

/* AHB APP BUS MASTER, ID */
typedef enum {
    APP_BUS_MASTER__DUMMY_SW = 0,
    APP_BUS_MASTER__BPM = 1,
    APP_BUS_MASTER__USB = 2,
    APP_BUS_MASTER__ADSP_DME = 3,
    APP_BUS_MASTER__SDCC = 4,
	APP_BUS_MASTER__VIDEO = 5,
	APP_BUS_MASTER__MDP = 6,
    APP_BUS_MASTER__INVALID = -1
} app_bus_master_type;


/*--------------------Grant Table Enum-----------------------------------------------*/

/* Grant Table Slot Size */

typedef enum {
    GT_SLOT_SIZE__8 = 7,
    GT_SLOT_SIZE__16 = 15,
} gt_slot_size_type;


/*--------------------Ahb Bus Priority Struct----------------------------------------*/

/* AHB SYS Bus Priority, pre-determined, a priority of zero gets not access to bus */
#define SYS_AHB_ARMDI_PRIORITY      2
#define SYS_AHB_ARMD_PRIORITY       3
#define SYS_AHB_TIC_PRIORITY        4

typedef struct {
    sys_bus_master_type default_master;
    int armi_priority;
    int armd_priority;
    int tic_priority;
} sys_bus_priority_type;

/* AHB APP Bus Priority, highest is 7, lowest is 0 */
#define APP_AHB_SDCC_PRIORITY       1       
#define APP_AHB_USB_PRIORITY        2       
#define APP_AHB_ADSP_PRIORITY       3       
#define APP_AHB_MDP_PRIORITY        4       
#define APP_AHB_VIDEO_PRIORITY      5       
#define APP_AHB_BPM_PRIORITY        6
#define APP_AHB_DUMMY_SW_PRIORITY   7       

typedef struct {
    app_bus_master_type default_master;
    int adsp_priority;
    int video_priority;
    int bpm_priority;
	int mdp_priority;
	int sdcc_priority;
	int usb_priority;
    int dummy_sw_priority;
} app_bus_priority_type;


/*--------------------Grant Table Slot Info type-------------------------------------*/

typedef struct{
    uint8 sys_coarse_slot;
    uint8 sys_fine_slot;
    uint8 app_coarse_slot;
    uint8 app_fine_slot;
} gt_slot_info_type;

/*--------------------Isr counters---------------------------------------------------*/

#ifdef PARB_INT_INSTALL_ISR
/* PARB interrupt counter type structure */

typedef struct {
    volatile int sys_bus;
    volatile int app_bus;
} parb_isr_counter_type;
#endif /* PARB_INT_INSTALL_ISR */


/* Function prototypes */

void parb_priority_init(sys_bus_priority_type *sys_bus_priority,
                                        app_bus_priority_type *app_bus_priority);

void parb_grant_table_reset(gt_slot_size_type slot_size);

#ifdef PARB_INT_INSTALL_ISR
    void parb_interrupt_app_isr(void);
    void parb_interrupt_sys_isr(void);
#endif

#endif /* PARBI_H_ */
