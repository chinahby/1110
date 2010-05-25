#ifndef HSU_AL_DLOAD_ARMPRG_COMMON_H
#define HSU_AL_DLOAD_ARMPRG_COMMON_H

/*==============================================================================

HSU_AL_DLOAD_ARMPRG_COMMON

GENERAL DESCRIPTION
Common header file for the High Speed USB DLOAD and ARMPRG Adaptation Layers

EXTERNALIZED FUNCTIONS

hsu_al_dload_armprg_init_buffers
hsu_al_dload_armprg_handle_interrupts
hsu_al_dload_armprg_start_stack
hsu_al_dload_armprg_common_mark_dload_used_hsu
hsu_al_dload_armprg_common_unmark_dload_used_hsu
hsu_al_dload_armprg_common_dload_used_hsu

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#include "comdef.h"
#include "jtypes.h"

#ifndef BUILD_HOSTDL
/* DLOAD requires transmitting messages of size 4KB to the PC. 
** This is needed for the "USB Upload" feature, and also required
** by the DLOAD protocol spec starting from version 6 of the protocol.
*/
#define HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE 4096
#else
  #ifdef BUILD_NANDPRG
  #define HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE 1040
  #else
  #define HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE 2048
  #endif
#endif /* !BUILD_HOSTDL */

/* TODO: the following size was chosen arbitrarily and might need optimization*/
#define HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE 16384
#define HSU_AL_DLOAD_ARMPRG_MEM_SIZE (2*HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE + HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE)

#ifdef FEATURE_HSU_EBI
#error code not present
#else /* !FEATURE_HSU_EBI */
extern byte                        hsu_al_dload_armprg_rx_buffer[2][HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE];
extern byte                        hsu_al_dload_armprg_tx_buffer[HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE];
#endif /* FEATURE_HSU_EBI */

/*===========================================================================

FUNCTION hsu_al_dload_armprg_init_buffers

DESCRIPTION
This function initializes the rx and tx buffers 
to the appropriate memory addresses.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_dload_armprg_init_buffers(void);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_handle_interrupts

DESCRIPTION
This function uses os abstraction functions in order to
poll for interrupts, and call the isr and dsr if an interrupt
occurred.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_dload_armprg_handle_interrupts(void);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_start_stack

DESCRIPTION
This function initializes the memory
for the stack and starts the stack.

DEPENDENCIES
None.

RETURN VALUE
0 on success, error number otherwise.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_dload_armprg_start_stack(void);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_start_stack_ex

DESCRIPTION
This function initializes the memory
for the stack and starts the stack.
It receives a parameter that specifies whether to enumerate
using the same product ID used when running in AMSS (non-download mode).

DEPENDENCIES
None.

RETURN VALUE
0 on success, error number otherwise.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_dload_armprg_start_stack_ex(boolean use_amss_product_id);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_mark_dload_used_hsu

DESCRIPTION
This function sets a marker in IRAM that signifies that DLOAD worked
over high speed usb.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_dload_armprg_common_mark_dload_used_hsu(void);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_unmark_dload_used_hsu

DESCRIPTION
This function clears the marker in IRAM that signifies that DLOAD worked
over high speed usb (sets it to FALSE).

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_dload_armprg_common_unmark_dload_used_hsu(void);

/*===========================================================================

FUNCTION hsu_al_dload_armprg_common_dload_used_hsu

DESCRIPTION
This function checks whether DLOAD worked over high speed usb.

DEPENDENCIES

RETURN VALUE
TRUE if DLOAD worked over high speed usb, FALSE otherwise.

SIDE EFFECTS

===========================================================================*/

boolean hsu_al_dload_armprg_common_dload_used_hsu(void);

#endif /* HSU_AL_DLOAD_ARMPRG_COMMON_H */

