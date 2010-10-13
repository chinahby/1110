#ifndef RXFI_H
#define RXFI_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        RECEIVER FRONT END DRIVER API HEADER FILE

GENERAL DESCRIPTION
  This header file defines the interface functions to drive the Receiver 
  Front End Block.  Any Receiver Front End Block driver has to implement all 
  the interface functions declared in this file in order to be used by any 
  modules/files in DMSS.

RX FRONT END DRIVER INTERFACE FUNCTIONS

    
    rxf_init
        This function initializes and configures the RX Front End. This function 
        must be called before invoking any other API functions.
    

    rxf_reset
        This function resets the  RX Front End Block by writing to the RXF_RESET.
    
    set_rxf_test_mode
        This function sets the rxf test mode. rxf_test_mode_type parameter can have 
        either of the following values:
            rxf_test_mode_0 ; I and Q get test_bus1 and test_bus2
            rxf_test_mode_1 ; I gets test_bus1 & "00000000", Q gets test_bus2 and "00000000"
            rxf_test_mode_2 ; I gets test_bus1 sign extended, Q gets test_bus2 sign extended
    


    HOW rxf.c IS USING THESE API FUNCTIONS
    
        Function rxf_init() will be called first for initialization.
        

Copyright (c) 2001, 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004, 2005       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rxfi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/07   zg      Remove obsolete functions rxf_save_dc_offset(void) and
                   rxf_restore_dc_offset(void)
09/28/05   dyc     Remove rxf_init() proto from here.  Exists in rxf.h
07/17/02   dyc     Added prototype for rxf_disable_dacc()
                   (merge from dbc rf.01.00.12)
03/21/02   sar     Added prototypes for functions rxf_set_vga_gain_offset()
                   rxf_save_dc_offset(), and rxf_restore_dc_offset().
03/18/02   sar     Removed PDM Clock Change functions.
01/17/02   sar     Added prototypes for rxf_wakeup,rxf_wakeup_delayed,
                   rxf_dc_acquire, rxf_dc_track,rxf_set_pdm_clk_digital and 
                   rxf_set_pdm_clk_tcx0 to support digital and analog sleep 
                   modes in RX Front.
11/19/01   sar     Added protypes for GetRxfIDCOffset, GetRxfQDCOffset and    
                   SetVgaGainOffset. 
11/02/01   tn      added a void to rxf_set_mode.
10/08/01   aks     Removed extern test_mode_type def, included rf.h header.
09/10/01   sar     Created.

===========================================================================*/


//#include "comdef.h"
#include "rf.h"

/* ----------------------------------------------------------------
** These enums define the legal mode settings for test_mode
** ---------------------------------------------------------------- */

///extern test_mode_type;


/* ========================== API FUNCTION PROTOTYPES ==================== */

/*===========================================================================

FUNCTION rxf_disable_dacc

DESCRIPTION
  This function disables the dacc
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
 dacc is disabled.
===========================================================================*/
extern void rxf_disable_dacc(void);
/*===========================================================================

FUNCTION rxf_set_mode

DESCRIPTION
  This function sets the mode of operation for the RX Front End Driver.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void rxf_set_mode(void);
/*===========================================================================

FUNCTION rxf_reset

DESCRIPTION
  This function resets the  RX Front End Block by writing to the RXF_RESET.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void rxf_reset(void);
/*===========================================================================

FUNCTION set_rxf_test_mode

DESCRIPTION
    This function sets the rxf test mode. The parameter value should be set to
    either of these values.
    
    0 : I and Q get test_bus1 and test_bus2.
    1 : I gets test_bus1 & "00000000", Q gets test_bus2 and "00000000".
    2 : I gets test_bus1 sign extended, Q gets test_bus2 sign extended.
      
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void set_rxf_test_mode(byte mode);

/*===========================================================================

FUNCTION enable_rxf_front

DESCRIPTION
    This function enables or disables the Rxf Front End. The parameter enable
    is set to TRUE if the Rxf Front is desired to be enabled otherwise it 
    should be set to FALSE.
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void enable_rxf_front(boolean enable);

/*===========================================================================

FUNCTION set_test_mode

DESCRIPTION
    This function selects the iput source for Y1 and Y2. 
    
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void set_test_mode(test_mode_type mode);
/*===========================================================================

FUNCTION rxf_sleep

DESCRIPTION
    This function puts rxf in the sleep mode. 
    
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void rxf_sleep(void);
/*===========================================================================

FUNCTION rxf_wakeup

DESCRIPTION
    This function transitions the rxf to active state from sleep mode. 
    
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
extern void rxf_wakeup(void);

/*=========================================================================

FUNCTION rxf_wakeup_delayed

DESCRIPTION
    This function is called approx 72 us after rxf_wakeup to set the DC 
    Cancellation loops to tracking mode. In digital RFR5600 mode the function
    also restores the PDM clock to digital if the application has not already
    done so.  
    
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Transitions the DC offset loops to tracking mode.
=========================================================================*/
extern void rxf_wakeup_delayed(void);
/*===========================================================================

FUNCTION rxf_dc_acquire

DESCRIPTION
    This function will force the dc cancellation mode to enter acquisition 
    mode. rxf_fm_dc_track() function must be called to transition CG loop 
    into tracking mode.

DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  DC Cancellation is held in acquisition mode unless rxf_fm_dc_track() 
  function is called.
===========================================================================*/
extern void rxf_dc_acquire(void);

/*===========================================================================

FUNCTION rxf_dc_track

DESCRIPTION
    This function will force the dc cancellation mode to enter tracking mode.
    This function must be called after rxf_fm_dc_acquire() function to set 
    the CG Loop back to tracking.
DEPENDENCIES
  None.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void rxf_dc_track(void);
/*===========================================================================

FUNCTION GetRxfIDCOffset

DESCRIPTION
    This function will read RXF_CG_IOFFSET register and return the value as
    unsigned dword.

DEPENDENCIES
  None.
 
RETURN VALUE
  dword type with RXF_CG_IOFFSET register content
  
SIDE EFFECTS
  None.
===========================================================================*/
extern dword GetRxfIDCOffset(void);

/*===========================================================================

FUNCTION GetRxfQDCOffset

DESCRIPTION
    This function will read RXF_CG_QOFFSET register and return the value as
    unsigned dword.

DEPENDENCIES
  None.
 
RETURN VALUE
  dword type with RXF_CG_QOFFSET register content
  
SIDE EFFECTS
  None.
===========================================================================*/
extern dword GetRxfQDCOffset(void);

/*===========================================================================

FUNCTION SetVgaGainOffset

DESCRIPTION
    This function will set the vga_gain_offset bits in the RXF_DVGA_CTL register
    with the 13-bits in the parameter offset.

DEPENDENCIES
  None.
 
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void SetVgaGainOffset(word);
/*============================================================================

FUNCTION RXF_SET_VGA_GAIN_OFFSET

DESCRIPTION
  This function will load in vga gain offsets compensated for the tuned 
  frequency.

DEPENDENCIES
  rf_freq_index must have been set before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
extern void rxf_set_vga_gain_offset( void );

/*===========================================================================

FUNCTION  rxf_save_dc_offset

DESCRIPTION
  This function saves DC offset before tuning away.
      
DEPENDENCIES
  In the current implementation, IACCUM3 & QACCUM3 are the only accumulators
  being used. If it changes in future, the corresponding implementation 
  changes are needed.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
//extern void rxf_save_dc_offset(void);
/*===========================================================================

FUNCTION  rxf_restore_dc_offset

DESCRIPTION
  This function restores all the DC Offset Block after tuning back.
      
DEPENDENCIES
  In the current implementation, IACCUM3 & QACCUM3 are the only accumulators
  being used. If it changes in future, the corresponding implementation 
  changes are needed.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
//extern void rxf_restore_dc_offset(void);

#endif   /* RXF_H */

