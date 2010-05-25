#ifndef PMAPP_HSED_H
#define PMAPP_HSED_H

/*===========================================================================


               P M A P P _ H S E D   H E A D E R    F I L E

DESCRIPTION
    This file contains function prototypes and variable/type/constant 
  declarations for applications developed for the Qualcomm Power
  Manager Chip Set.
    
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_hsed.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   jtn     New file, moved PMIC headset functions from pmapp.h
===========================================================================*/

#if defined(FEATURE_PMIC_HS_SEND_END_DETECT)
/*===========================================================================

FUNCTION pm_hsed_get_key_pressed                           EXTERNAL FUNCTION

DESCRIPTION
  This funcion returns the latched status of the HSED button press prior to
calling this function. The latched status requires pm_hsed_clear_key_pressed to
clear
    
PARAMETERS
  None.  

RETURN VALUE
  Boolean: TRUE = a HSED button has been pressed prior to function call 
           FALSE = no HSED button has been pressed prior to function call 

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_hsed_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean pm_hsed_get_key_pressed(void);

/*===========================================================================

FUNCTION pm_hsed_clear_key_pressed                           EXTERNAL FUNCTION

DESCRIPTION
  This funcion clears the latched status of the HSED button press 
    
PARAMETERS
  None.  

RETURN VALUE
  None  

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed()
  4) pm_hsed_init() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void pm_hsed_clear_key_pressed(void);

#if defined (FEATURE_PMIC_HSED_PMIC4)

/*===========================================================================

FUNCTION pm_hsed_get_inserted                               EXTERNAL FUNCTION

DESCRIPTION
  This function is used to check whether the headset is inserted or removed 
    
PARAMETERS
  None.  

RETURN VALUE
  TRUE if headset is inserted.
  FALSE if headset is removed.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern boolean pm_hsed_get_inserted(void);

#endif
/*===========================================================================

FUNCTION pm_hsed_init                                EXTERNAL FUNCTION

DESCRIPTION
  This function initializes the headset send/end detect settings and ISR 
    
PARAMETERS
  None.  

RETURN VALUE
  None.

DEPENDENCIES
    The following functions need to be called before we call this 
  function:
  
  1) rflib_init() 
  2) pm_init()
  3) pm_init_delayed() 
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void pm_hsed_init(void);
#endif

#endif //PMAPP_HSED_H


