#ifndef MDP_SYS_API_H
#define MDP_SYS_API_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains MDP system API.  
  Only the system level application(s) should use this API.

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp_api/main/latest/inc/mdp_sys_api.h#1 $ $DateTime: 2008/05/16 15:58:49 $ $Author: aansari $

when      who    what, where, why
--------  -----  ----------------------------------------------------------
03/23/06  cjr     Added mdp_mddi_link_suspend()/resume() API
01/05/06  cjr     Added mdp_is_in_vsync_blanking_period() API
11/07/06  cjr     Added mdp_blt() API
06/26/06  cjr     Added MDP_MAP_INFO under mdp_map_dest()
05/10/06  cjr     Added support for display API queue
02/23/06  cjr     Added mdp_enable_fullscreen_redirection()
02/22/06  trg     Added clear async mode (mdp error mode) prototype
11/01/05  cjr     Added mdp_start_ext_display() and mdp_stop_ext_display()
09/01/05  cjr	  Initial code
============================================================================*/

#include "mdp_api.h"

#ifdef FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT
#error code not present
#endif /* FEATURE_DISPLAY_ADAPTIVE_BACKLIGHT */
/* -----------------------------------------------------------------------
** destination mapping attribute structure
** ----------------------------------------------------------------------- */
typedef struct mdpMapParam_
{ 
  MDP_CWIN cwin;       /* display window */
  uint32 mdpOp;        /* supports flip and rotation only */
} MDP_MAP_INFO;

typedef enum
{
  MDP_GAMMA_POSTLUT,
  MDP_GAMMA_PRELUT
} MDP_GAMMA_LUT_TYPE;

typedef enum
{
  MDP_GAMMA_READ,
  MDP_GAMMA_WRITE
} MDP_GAMMA_OP_TYPE;
/*===========================================================================
    MDP DRIVER API FUNCTIONS PROTOTYPES
============================================================================*/

MDP_STATUS mdp_lock_ovimg(uint32 id, int32 flag);
void mdp_lock_all_ovimg(LCD_TYPE dest, int32 flag);
void mdp_enable_locked_ovimg(uint32 id, int32 flag);
void mdp_enable_all_locked_ovimg(LCD_TYPE dest, int32 flag);
void mdp_deregister_locked_ovimg(uint32 id);
void mdp_deregister_all_locked_ovimg(LCD_TYPE dest);
MDP_STATUS mdp_get_mdpImg(uint32 id, MDPIMG *mdpImg);
void mdp_set_fatal_err(void);
void mdp_clear_fatal_err(void);
void mdp_set_lcd_size(LCD_TYPE dest, LCD_TYPE_PHYS physDest, uint32 width, uint32 height);

#ifdef FEATURE_MDP2
#error code not present
#endif

/*==================================================================================
==================================================================================
FUNCITON NAME: mdp_lock_ovimg()

DESCRIPTION
	This function is to lock or unlock overlay image.
	The state of locked overlay can NOT be changed until it's being unlocked.	
	
PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL

	int32 flag
		TRUE = Lock or FALSE = Unlock
		
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL

==================================================================================
==================================================================================
FUNCITON NAME: mdp_lock_all_ovimg()

DESCRIPTION
	This function is to lock or unlock all overlay images.
	
PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of the overlay image list

	int32 flag
		TRUE = Lock or FALSE = Unlock
					
EXAMPLE:
		
RETURN VALUE:
    Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
         
==================================================================================
==================================================================================
FUNCITON NAME: mdp_enable_locked_ovimg()

DESCRIPTION
	This function is to enable/disable locked overlay image.
  It first unlocks the overlay and disables the overlay.
  And then, it locks the overlay back again.
	
PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL

	int32 flag
		TRUE = Enable or FALSE = Disable
					
EXAMPLE:
		
RETURN VALUE:
    None
    
==================================================================================
==================================================================================
FUNCITON NAME: mdp_enable_all_locked_ovimg()

DESCRIPTION
	This function is to enable/disable all locked overlay images.
	
PARAMETERS:	
	LCD_TYPE dest
		Specifies the destination of the overlay image list

	int32 flag
		TRUE = Enable or FALSE = Disable
					
EXAMPLE:
		
RETURN VALUE:
    None

==================================================================================
==================================================================================
FUNCITON NAME: mdp_get_mdpImg 

DESCRIPTION
	This function is to store the previously registered MDP image info.
	
PARAMETERS:	
	uint32 id
		Previously returned registration ID or NULL

	MDPIMG *mdpImg
		mdpImg location to store
					
EXAMPLE:
		
RETURN VALUE:
    None

==================================================================================
==================================================================================
FUNCITON NAME: mdp_map_dest()

DESCRIPTION
  This function is to map one logical display destination.  
  	
PARAMETERS:	
  LCD_TYPE src : src destination (from)
  LCD_TYPE dest: dest destination (to)
  MDP_MAP_INFO mapInfo: destination mapping info
					
EXAMPLE:
        MDP_MAP_INFO mapInfo;
        
        mapInfo.cwin.x1 = 0;
        mapInfo.cwin.x2 = 239;
        mapInfo.cwin.y1 = 0;
        mapInfo.cwin.y2 = 319;
        mapInfo.mdpOp = MDPOP_NOP;
                                        
	mdp_map_dest(DISPLAY_TYPE_1, DISPLAY_TYPE_2, &mapInfo);		
  	
RETURN VALUE:
  Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL

==================================================================================
==================================================================================
FUNCITON NAME: mdp_start_ext_display()

DESCRIPTION
  This function is to start an external display device such as TV or Ext MDDI.
  	
PARAMETERS:	
  LCD_TYPE lgDest
    logical display destination to be mapped

  LCD_TYPE_PHYS physDest
    physical display destination to be mapped

  uint32 width
    width of display device in number of pixel 

  uint32 height
    height of display device in number of pixel
					
EXAMPLE:
		
RETURN VALUE:
  Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL

==================================================================================
==================================================================================
FUNCITON NAME: mdp_stop_ext_display()


DESCRIPTION
  This function is to stop an external display device such as TV or Ext MDDI.
  	
PARAMETERS:	
  LCD_TYPE_PHYS physDest
    physical display destination to stop displaying
    					
EXAMPLE:
		
RETURN VALUE:
  None

==================================================================================
==================================================================================
FUNCTION mdp_set_fatal_err()

DESCRIPTION
  Set MDP variable to flag an error message update.  Call before displaying
  updates where task switching will not be supported.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  All future mdp updates will occur syncrounously until
  mdp_clear_fatal_err() is called.  Running the MDP in error mode will
  significantly impact performance.

==================================================================================
==================================================================================
FUNCTION mdp_clear_fatal_err()

DESCRIPTION
  Clear MDP variable to flag an error message update.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  See mdp_set_fatal_err() above.

==================================================================================
==================================================================================
FUNCITON NAME: void mdp_enable_fullscreen_redirection()

DESCRIPTION
  This function is to enable a single/fullscreen buffering as a redirected-input.
  This will guarantee no side effect from scale conversion of display region.
  	
PARAMETERS:	
  None
					
EXAMPLE:
		
RETURN VALUE:
  None

==================================================================================
==================================================================================
FUNCITON NAME: void mdp_set_lcd_size()

DESCRIPTION
  This function is to configure the logical device channel from physical device info.
  	
PARAMETERS:	
  
  LCD_TYPE lgDest
    logical display destination to be mapped

  LCD_TYPE_PHYS physDest
    physical display destination to be mapped

  uint32 width
    width of display device in number of pixel 

  uint32 height
    height of display device in number of pixel
					
EXAMPLE:
		
RETURN VALUE:
  None

==================================================================================
==================================================================================
FUNCITON NAME: void mdp_blt()

DESCRIPTION
  This function is to support a direct access to MDP HW for bitmap blending 
  	
PARAMETERS:	
  MDPIMG mdpImg - MDP image structure  
					
EXAMPLE:
		
RETURN VALUE:
  None

==================================================================================
==================================================================================
FUNCITON NAME: void mdp_is_in_vsync_blanking_period()

DESCRIPTION
  This function is to return TRUE if the current vsync line counter is in
  the blanking period.
  	
PARAMETERS:	
					
EXAMPLE:
		
RETURN VALUE:
  TRUE if it's in vsync blanking period otherwise returns FALSE.

==================================================================================
==================================================================================

FUNCITON NAME: mdp_register_read/write()

DESCRIPTION
  This function is to read/write MDP register.
 
PARAMETERS:	
  uint32 addr : MDP address location to read(op = READ) or write (op = WRITE)
  uint32 *val : Data pointer to store (op = READ) or fetch (op = WRITE)
 
EXAMPLE:
		
RETURN VALUE:
  Returns MDP_SUCCESSFUL if successful, otherwise returns MDP_FAIL
  

==================================================================================
==================================================================================

FUNCTION mdp_mddi_link_suspend()

DESCRIPTION
  This function is to suspend MDP data transfer to MDDI host

RETURN VALUE
  None

SIDE EFFECTS
  All display updates to MDDI LCD will be suspended until mdp_mddi_link_resume()
  gets called

==================================================================================
==================================================================================

FUNCTION mdp_mddi_link_resume()

DESCRIPTION
  This function is to resume MDP data transfer to MDDI host

RETURN VALUE
  None

SIDE EFFECTS
  None
  
==================================================================================
=================================================================================*/
	
	
#endif /* MDP_SYS_API_H */

