#ifndef SDCC_BSP_H
#define SDCC_BSP_H
 /**********************************************************************
 * sdcc_bsp.h
 *
 * SDCC(Secure Digital Card Controller) driver BSP.
 *
 * This file external data structures, definitions for SDCC BSP used
 * exclusively by the SDCC driver.
 *
 * Copyright (c) 2008
 * Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************

                             Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/bsp/sdcc_bsp.h#2 $

when        who  what, where, why
----------  ---  --------------------------------------------------------------
2008-12-19  vj   Added macro definiton SDCC_PM_VREG_LEVEL
2008-10-31  vin  Initial version
-----------------------------------------------------------------------------*/

/*****************************************************************************/
/*                    INCLUDES                                               */
/*****************************************************************************/
#include "comdef.h"

/*****************************************************************************/
/*              DEFINITIONS AND CONSTANTS                                    */
/*****************************************************************************/

#if defined(FEATURE_WLAN) || defined(FEATURE_L4LINUX)
#error code not present
#endif

/* Vdd control options for sdcc */
typedef enum
{
   SDCC_BSP_VDD_OFF,
   SDCC_BSP_VDD_ON
}sdcc_bsp_vdd_ctl_type;

/* SDCC BSP Error Types */
typedef enum
{
   SDCC_BSP_NO_ERROR,
   SDCC_BSP_ERR_UNKNOWN,
   SDCC_BSP_ERR_INVALID_PARAM,
   SDCC_BSP_ERR_FEATURE_UNSUPPORTED
} sdcc_bsp_err_type;

/*****************************************************************************/
/*                           PROTOTYPES                                      */
/*****************************************************************************/

/*=============================================================================
 * FUNCTION      sdcc_bsp_vdd_control
 *
 * DESCRIPTION   This function turns ON/OFF the Vdd source to the card in
 *               use.
 * 
 * PARAMETERS    [IN] state : Desired power state: on or off
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_vdd_control (sdcc_bsp_vdd_ctl_type state);


/*=============================================================================
 * FUNCTION      sdcc_bsp_slot_interrupt_exists
 *
 * DESCRIPTION   This function returns TRUE if hardware supports detection
 *               of card removal / insertion.
 *
 *
 * RETURN VALUE  TRUE if supported, FALSE otherwise
 *
 *===========================================================================*/
boolean
sdcc_bsp_slot_interrupt_exists(void);

/*=============================================================================
 * FUNCTION      sdcc_bsp_enable_slot_int
 *
 * DESCRIPTION   Setup the mechanism for detecting the card insertion/removal
 *               and associate an ISR to be called when a card is inserted
 *               or removed.
 *
 * PARAMETERS    [IN]: Pointer to the ISR function
 *
 * RETURN VALUE  None
 *
 *===========================================================================*/
void
sdcc_bsp_enable_slot_int(void *isr);
#endif /* #ifndef SDCC_BSP_H */

