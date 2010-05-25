/*****************************************************************************
**
**  Name:          hcilp.h
**
**  Description:
**      This file contains definitions and function prototypes for the.
**      HCI low power mode module.
**
**  Copyright (c) 2002-2004, WIDCOMM Inc., All Rights Reserved.
**  WIDCOMM Bluetooth Core. Proprietary and confidential.
******************************************************************************/
#ifndef HCILP_H
#define HCILP_H

#include "upio.h"
#include "userial.h"

/* sleep mode */
enum {
    HCILP_SLEEP_MODE_DISABLE,
    HCILP_SLEEP_MODE_UART,
    HCILP_SLEEP_MODE_UART_WITH_MSG,
    HCILP_SLEEP_MODE_USB,
    HCILP_SLEEP_MODE_H4IBSS,
    HCILP_SLEEP_MODE_USB_WITH_HOST_WAKE,
    HCILP_SLEEP_MODE_SDIO,
    HCILP_SLEEP_MODE_UART_CS_N,
    HCILP_SLEEP_MODE_SPI,
    HCILP_SLEEP_MODE_H5,
    HCILP_SLEEP_MODE_H4DS
};

/* sleep mode parameters */
typedef struct
{
    tUSERIAL_PORT   port;
    UINT8   sleep_mode;                     /* Sleep Mode 0,1,4,9 */
    UINT8   host_stack_idle_threshold;      /* Host Idle Treshold in 300ms if mode = 1 */
    UINT8   host_controller_idle_threshold; /* Host Controller Idle Treshold in 300ms if mode = 1 */
    UINT8   bt_wake_polarity;               /* BT_WAKE Polarity - 0=Active Low, 1= Active High */
    UINT8   host_wake_polarity;             /* HOST_WAKE Polarity - 0=Active Low, 1= Active High */
    UINT8   allow_host_sleep_during_sco;    /* Allow host Sleep during SCO */
    UINT8   combine_sleep_mode_and_lpm;     /* Combine Sleep Mode and LPM */
    UINT8   enable_uart_txd_tri_state;      /* UART_TXD Tri-State */
    UINT8   active_connection_handling;     /*  */
    UINT8   resume_timeout;                 /*  */
} tHCILP_Params;

/*******************************************************************************
**  tHCILP_Enable_CBACK Callback Functions
**  Input Param     status: 0 for success, 1 for fail
**
*******************************************************************************/
#define HCILP_STATUS_SUCCESS    0
#define HCILP_STATUS_FAILURE    1

typedef void (tHCILP_ENABLE_CBACK) (UINT8 status);

/*******************************************************************************
**  tHCILP_Wakeup_BT_CBACK Callback Functions
**
*******************************************************************************/
typedef void (tHCILP_WAKEUP_BT_CBACK) (void);

#if (defined(H4IBSS_INCLUDED) && H4IBSS_INCLUDED == TRUE)
/* Enumeration of H4IBSS state machine events */
enum
{
    H4IBSS_API_ALLOW_BT_SLEEP_EVT,  /* Host is idle */
    H4IBSS_API_WAKE_UP_BT_EVT,      /* Host wants to send packet */
    H4IBSS_SLEEP_REQ_EVT,           /* BT wants to sleep */
    H4IBSS_SLEEP_REQ_ACK_EVT,       /* BT accepts to sleep */
    H4IBSS_SLEEP_REQ_NACK_EVT,      /* BT didn't agree to sleep */
    H4IBSS_RX_LINE_HIGH_EVT,        /* BT wants to wake or is woken */
    H4IBSS_TIMEOUT_EVT,             /* timeout */
    H4BISS_NUM_EVTS
};

/* H4IBSS Control Block */
typedef struct
{
    UINT8           state;
    tUSERIAL_PORT   port;
    BOOLEAN         is_sending_sleep_mode;  /* TRUE if H4IBSS sends sleep mode command */
} tH4IBSS_CB;
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
**  H4IBSS
*****************************************************************************/
#if (defined(H4IBSS_INCLUDED) && H4IBSS_INCLUDED == TRUE)
extern void h4ibss_init(tUSERIAL_PORT port);
extern void h4ibss_sm_execute (UINT8 event);
extern void h4ibss_timeout(UINT16 type);
extern void h4ibss_sleep_mode_evt(UINT8 *p, UINT8 evt_len);
extern BOOLEAN h4ibss_is_uart_active(void);
#endif
/*******************************************************************************
**
** Function          HCILP_IsTxDoneCplted
**
** Description       This function is to inform the hcilp module
**                   if data is waiting in the Tx Q or not.
**
**                   IsTxDone: TRUE if All data in the Tx Q are gone
**                             FALSE if all data in the Tx Q not gone.
**                   Typicaly this function must be called
**                   before USERIAL Write and in the Tx Done routine
**
** Returns           None
**
*******************************************************************************/
LPM_API extern void HCILP_IsTxDoneCplted(BOOLEAN);

/*******************************************************************************
 **
 ** Function           HCILP_Init
 **
 ** Description        Init HCILP data.
 **
 ** Output Parameter   None
 **
 ** Returns            void
 **
 *******************************************************************************/
LPM_API extern void HCILP_Init(tUSERIAL_PORT port);

/*******************************************************************************
**
** Function           HCILP_Enable
**
** Description        Enables Sleep mode.
**
** Output Parameter   None
**
** Returns            TRUE  if sleep mode command was issued to BT device
**                    FALSE if the feature is not supported
**
*******************************************************************************/
LPM_API extern BOOLEAN HCILP_Enable(tHCILP_Params *, tHCILP_ENABLE_CBACK *);

/*******************************************************************************
**
** Function           HCILP_Disable
**
** Description        Enables Sleep mode.
**
** Output Parameter   None
**
** Returns            TRUE  if sleep mode command was issued to BT device
**                    FALSE if the feature is not supported
**
*******************************************************************************/
LPM_API extern BOOLEAN HCILP_Disable(tHCILP_ENABLE_CBACK *);

/*******************************************************************************
**
** Function           HCILP_IsEnabled
**
** Description        return whether LPM is enabled or not
**
** Output Parameter   None
**
** Returns            TRUE  if sleep mode is enabled
**                    FALSE if sleep mode is not enabled
**
*******************************************************************************/
LPM_API extern BOOLEAN HCILP_IsEnabled(void);

/*******************************************************************************
**
** Function           HCILP_IsSleepState
**
** Description        Checks if its ok for application to go to sleep mode
**
** Output Parameter   None
**
** Returns            TRUE  if bluetooth is OK for app to go to sleep mode
**                    FALSE if bluetooth is awake and cannot go to sleep mode
**
*******************************************************************************/
BOOLEAN HCILP_IsSleepState(void);


/*******************************************************************************
**
** Function           HCILP_AppSleeping
**
** Description        Called by application when it wants to indicate to bluetooth
**                    that it is going into low power mode
**
** Output Parameter   None
**
**
*******************************************************************************/
void HCILP_AppSleeping(void);

/*******************************************************************************
**
** Function           HCILP_WakeupBTDevice
**
** Description        Called to wake up Bluetooth chip. This is called by HCIS
**                    when there is data to be sent over UART
**
** Output Parameter   None
**
** Returns            TRUE  If UART is active
**                    FALSE If UART is NOT active
**
**
*******************************************************************************/
LPM_API extern BOOLEAN HCILP_WakeupBTDevice(tHCILP_WAKEUP_BT_CBACK *);

/*******************************************************************************
**
** Function           HCILP_WakeupHost
**
** Description
**
** Output Parameter   None
**
** Returns            None
**
*******************************************************************************/
LPM_API extern void HCILP_WakeupHost(void);

/*******************************************************************************
**
** Function           HCILP_AllowBTDeviceSleep
**
** Description        Called to indicate to bluetooth chip that it can go to
**                    sleep mode.This is called by HCIS when all data has been
**                    read from UART
**
** Output Parameter   None
**
**
*******************************************************************************/
LPM_API extern void HCILP_AllowBTDeviceSleep(void);

/*******************************************************************************
**
** Function           hcilp_do_allow_dev_sleep
**
** Description        Called to indicate to bluetooth chip that it can go to
**                    sleep mode after BTU_TTYPE_LP_HC_IDLE_TO timeout
**
** Output Parameter   None
**
**
*******************************************************************************/
extern void hcilp_do_allow_dev_sleep(void);

#ifdef __cplusplus
}
#endif

#endif
