
#ifndef SDEVMENU_H
#define SDEVMENU_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S E R I A L   D E V I C E   M E N U   S E R V I C E S

                         H E A D E R    F I L E


GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial Device Menu.

EXTERNALIZED FUNCTIONS

  sdm_menu_init()
    Initializes the Serial Device Menu services.

  sdm_ui_get_table_diag_baud()
    Returns the DIAG baud selection UI menu.

  sdm_ui_get_table_ds_baud()
    Returns the DS baud selection UI menu.

  sdm_ui_get_table_dev_map()
    Returns the Port Map selection UI menu.

  sdm_ui_set_diag_baud()
    Forwards the new DIAG baud request to DIAG.

  sdm_ui_set_ds_baud()
    Forwards the new DS baud request to DS.

  sdm_ui_set_dev_map()
    Sets up the selected port map to be saved into NV.


INITIALIZATION AND SEQUENCING REQUIREMENTS

  sdm_menu_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/sdevmenu.h_v   1.2   05 Sep 2002 15:14:16   ldefauw  $
  $Header: //depot/asic/msmshared/services/rdevmap/02.00.XX/sdevmenu.h#1 $ $DateTime: 2008/10/19 19:39:23 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/00   dwp     Added support for FEATURE_RUNTIME_DEVMAP.
04/10/00   jwh     Initial Release. 
04/04/00   jwh     Created module.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "customer.h"              /* Customer Specific Definitions        */
#include "sio.h"                   /* Serial I/O Services.                 */

#if ( defined( FEATURE_DIAG_UI_BAUD    ) || \
      defined( FEATURE_DS_UI_BAUD      ) || \
      defined( FEATURE_SDEVMAP_UI_MENU )    \
    )
#include "nv.h"
#endif

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/*---------------------------------------------------------------------------
 Structure definitions for SDM UI MENU items
---------------------------------------------------------------------------*/
#ifdef FEATURE_SDEVMAP_UI_MENU
typedef struct 
{
  char              *item_string;  /* Must be exactly 12 characters long   */        
  sdm_config_type    config_id;
} sdm_ui_map_item_type;
#endif /* FEATURE_SDEVMAP_UI_MENU */

#if ( defined( FEATURE_DIAG_UI_BAUD ) || defined( FEATURE_DS_UI_BAUD ))
typedef struct 
{
  char              *item_string;  /* Must be exactly 12 characters long   */
  sio_bitrate_type   baud_id;
} sdm_ui_baud_item_type;
#endif /* .._DIAG_UI_BAUD or .._DS_UI_BAUD */


#define SDM_MAX_UI_ITEMS     10  /* This is a limit from the UI code       */
                                 /* in any event must be less that         */
                                 /* the value of SDM_UNKNOWN_INDEX         */
#define SDM_UNKNOWN_INDEX  0xFF  /* Flag value to indicate that we don't   */
                                 /* know the current default               */

#ifdef FEATURE_SDEVMAP_UI_MENU
/*---------------------------------------------------------------------------
 * Structure definitions for port map UI MENU 
 *-------------------------------------------------------------------------*/

typedef struct
{
  char            *heading;       /* Must be exactly 12 characters long    */
                                  /* as an example: "DAT DIAG OPT"         */

  uint8            current_index; /* index of current value 0..0xFF        */
                                  /* "0xFF" indicates current_index is     */
                                  /* unknown. REAL item indexes range      */
                                  /* from zero to (SDM_MAX_UI_ITEMS - 1).  */

  uint8            num_items;     /* MAX of SDM_MAX_UI_ITEMS               */

  sdm_ui_map_item_type  *items[ SDM_MAX_UI_ITEMS ]; 

} sdm_ui_map_menu_type;
#endif /* FEATURE_SDEVMAP_UI_MENU */


#if ( defined( FEATURE_DIAG_UI_BAUD ) || defined( FEATURE_DS_UI_BAUD ))
/*---------------------------------------------------------------------------
 * Structure definitions for DS and DIAG baud rate UI MENU 
 *-------------------------------------------------------------------------*/

typedef struct
{
  char            *heading;       /* Must be exactly 12 characters long    */    /* */
                                  /* as an example: "    115,200 "         */

  uint8            current_index; /* index of current value 0..0xFF        */
                                  /* "0xFF" indicates current_index is     */
                                  /* unknown. REAL item indexes range      */
                                  /* from zero to (SDM_MAX_UI_ITEMS - 1).  */

  uint8            num_items;     /* MAX of SDM_MAX_UI_ITEMS               */

  sdm_ui_baud_item_type *items[ SDM_MAX_UI_ITEMS ]; 

} sdm_ui_baud_menu_type;
#endif /* .._DIAG_UI_BAUD or .._DS_UI_BAUD */

#if ( defined( FEATURE_DIAG_UI_BAUD    ) || \
      defined( FEATURE_DS_UI_BAUD      ) || \
      defined( FEATURE_SDEVMAP_UI_MENU )    \
    )
/*===========================================================================

FUNCTION SDM_MENU_INIT                                 EXTERNALIZED FUNCTION

DESCRIPTION
  Prepares the Serial Device Menu Services for operation.  Must be called
  before any other Serial Device Menu Services are used.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sdm_menu_init
(
  void
);

#endif /* .._DIAG_UI_BAUD or .._DS_UI_BAUD or ..SDEVMAP_UI_MENU */

#ifdef FEATURE_DIAG_UI_BAUD
/*===========================================================================

FUNCTION SDM_UI_GET_TABLE_DIAG_BAUD                    EXTERNALIZED FUNCTION

DESCRIPTION
  Returns the DIAG baud selection menu. If NV contains a valid baud rate
  and it is in our list of baud rate choices, we indicate that as the
  current baud rate - UI will mark this with a "checkmark".

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

sdm_ui_baud_menu_type * sdm_ui_get_table_diag_baud
( 
  nv_stat_enum_type nv_result_stat,
  nv_item_type     *nv_data_buf      /* only valid if
                                      * nv_result_stat == NV_DONE_S
                                      */
);

#endif /* FEATURE_DIAG_UI_BAUD */

#ifdef FEATURE_DS_UI_BAUD
/*===========================================================================

FUNCTION SDM_UI_GET_TABLE_DS_BAUD                     EXTERNALIZED FUNCTION

DESCRIPTION
  Returns the DS baud selection menu. If NV contains a valid baud rate
  and it is in our list of baud rate choices, we indicate that as the
  current baud rate - UI will mark this with a "checkmark".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

sdm_ui_baud_menu_type * sdm_ui_get_table_ds_baud
( 
  nv_stat_enum_type nv_result_stat,
  nv_item_type     *nv_data_buf      /* only valid if 
                                      * nv_result_stat == NV_DONE_S
                                      */
);

#endif /* FEATURE_DS_UI_BAUD */



#ifdef FEATURE_DIAG_UI_BAUD
/*===========================================================================

FUNCTION SDM_UI_SET_DIAG_BAUD                          EXTERNALIZED FUNCTION

DESCRIPTION
  Forwards the new DIAG baud request to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sdm_ui_set_diag_baud
( 
  uint8 new_baud_index 
);

#endif /* FEATURE_DIAG_UI_BAUD */
       
#ifdef FEATURE_DS_UI_BAUD
/*===========================================================================

FUNCTION SDM_UI_SET_DS_BAUD                            EXTERNALIZED FUNCTION

DESCRIPTION
  Forwards the new DS baud request to DS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sdm_ui_set_ds_baud
( 
  uint8 new_baud_index 
);

#endif /* FEATURE_DS_UI_BAUD */


#endif /* SDEVMENU_H */ 
