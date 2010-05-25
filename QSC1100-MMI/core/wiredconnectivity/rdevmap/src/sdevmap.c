/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S E R I A L   D E V I C E   M A P P E R   S E R V I C E S


GENERAL DESCRIPTION
  This module contains the Serial Device Mapper (SDM) services which are used
  to map a DMSS application to a particular SIO Port.
  
EXTERNALIZED FUNCTIONS

  sdm_init()
    Initialize Serial Device Mapper Services

  sdm_get_port_id()
    Returns the current SIO Port mapped to the specified Application.
                                                     
  sdm_uart2_opened()
    Informs the Serial Device Mapper that the second UART has been opened.

  sdm_uart2_closed()
    Informs the Serial Device Mapper that the second UART has been closed.

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

  sdm_mc_dev_map_from_nv()
    Stores the port map retrieved from NV  
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sdm_init() must be called prior to any of the other functions of
  this module.

  sdm_menu_init() must be called prior to any of the sdm_ui_* functions 
  of this module.
                 
Copyright (c) 1999, 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/sdevmap.c_v   1.14.1.0   16 May 2003 18:48:40   chenc  $
  $Header: //depot/asic/msmshared/services/rdevmap/02.00.XX/sdevmap.c#1 $ $DateTime: 2008/10/19 19:39:23 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/07   rh      Start the removal of sdevmap module
01/04/06   ptm     Restore SMEM code, removed RPCMP and DIAG_MP services and
                   devices.
11/18/05   vs      Removed / supressed lint warnings 
06/05/05   TMR     Merged in MSM7500 code
05/16/03   cc      HWIO porting for MSM6100. 
10/17/02   cc      Added FEATURE_JCDMA_1X support. 
07/15/02   ak      Added FEATURE_DATA_MM support.
10/16/01   cc      Merge in mods of SIMBA from MSM5200_SIO.00.00.17, includes 
                   following 1 item.
05/01/01   dwp     Change arg type for INTLOCK_SAV() to uint32.
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
03/26/01   dl      Add support for SMEM_DS port.
02/28/01   dwp     Merge in Bama mods from MSM5105_SIO.00.00.09.
02/19/01   dwp     Change diag_default_baudrate & ds_default_baudrate to 
                   *_baudrate. Use new NV type nv_sio_baudrate_type now.
01/04/01   dwp     More changes to make SDM port map interfaces mutually 
                   exclusive with FEATURE_RUNTIME_DEVMAP while keeping baud
                   rate change interfaces.
12/15/00   dwp     Make SDM port map interfaces mutually exclusive with
                   FEATURE_RUNTIME_DEVMAP.
12/11/00   dwp     Fix FEATURE test for intializing SDM_APP_BT_DEFAULT_PORT.
11/27/00   dwp     Change SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_NA. Made DS
                   default to U1 even in BT mode.
11/14/00   dwp     Added support for FEATURE_RUNTIME_DEVMAP.
11/07/00   dwp     Corrected #define for SDM_APP_DIAG_DEFAULT_PORT for case
                   of DS,no USB, no BT, no U2.
08/02/00   dwp     Additional FEATURE_BT modifications.
07/28/00   dwp     Added support for T_MSM33 define.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
06/30/00   jwh     Corrected featurization of FEATURE_DS, etc.
06/02/00   jwh     Corrected SDM_RULE_DS_NEEDS_PORT - was .._GETS_PORT.
05/11/00   jwh     Merge with MSM3100.
04/17/00   jwh     Deleted DIAG baud rate UI choices below 38400 and 57600. No 
                   longer include dsmgr.h. Changed ds_change_baud() interface.
04/13/00   jwh     Initialize so as to enable HFK when 
                   FEATURE_SDEVMAP_SHARE_HFK and FEATURE_SECOND_UART defined.
                   Renamed SDM_HFK_ENABLE and SDM_HFK_DISABLE as 
                   SDM_UART2_ENABLE and SDM_UART2_DISABLE.
04/08/00   jwh     Added sdm_uart2_opened() and sdm_uart2_closed() to enable
                   and disable the HFK.
04/04/00   jwh     Added sdm_menu_init(), sdm_ui_get_table_diag_baud(),
                   sdm_ui_get_table_ds_baud(), sdm_ui_get_table_dev_map(),
                   sdm_ui_set_diag_baud(), sdm_ui_set_ds_baud(),
                   sdm_ui_set_dev_map().
10/18/99   jwh     Initial Release.
10/05/99   jwh     Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  -------------------------------------------------------------------------
              General Serial Device Mapper REQUIREMENT:

  If FEATURE_SERIAL_DEVICE_MAPPER is defined then all applications using 
  SIO services must also use the Serial Device Mapper services for all 
  Port determinations.  
  -------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  -------------------------------------------------------------------------
  Mapping applications to SIO Ports is SUBJECT to various CONSTRAINTS:

  1) Obviously, only Ports that exist should be assigned.  (We assume that 
     sio_port_id_type enum values are only defined when that Port exists.)

  2) If FEATURE_DS is defined then DS must have a valid Port assigned.
     See SDM_RULE_DS_NEEDS_PORT. (This restriction may be relaxed in a 
     future release.)

  3) If DS isn't mapped to SIO_PORT_UART_MAIN, then DIAG must be! Since
     downloading currently requires the main UART, this constraint avoids 
     producing "One Time Programable" (OTP) phones See 
     SDM_RULE_DOWNLOAD_NEEDS_MAIN.
  -------------------------------------------------------------------------
  The following FEATURES affect the default SIO Port map:

        FEATURE_DS
        FEATURE_SECOND_UART
        FEATURE_USB_CDC_ACM 

  -------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"              /* Customer Specific Definitions        */

#include "err.h"
#include "sio.h"  
#include "msm.h"
#include "sdevmap.h"               /* Serial Device Mapper.                */
#include "sdevmenu.h"

#ifdef FEATURE_DS_UI_BAUD
   #ifdef FEATURE_DATA_MM
     #include "ds3gsiolib.h"
   #else
     #include "dsatcop.h"
   #endif /* FEATURE_DATA_MM */
#endif /* FEATURE_DS_UI_BAUD */

#ifdef FEATURE_DIAG_UI_BAUD
#include "diag.h"
#endif /* FEATURE_DIAG_UI_BAUD */

#define SDM_CHANGE_BAUD_NOW   TRUE

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/
#ifdef SDM_UI_MENU
  #error Cannot externally #define SDM_UI_MENU 
#else 
  #if ( defined( FEATURE_DIAG_UI_BAUD    ) || \
        defined( FEATURE_DS_UI_BAUD      ) || \
        defined( FEATURE_SDEVMAP_UI_MENU )    \
      )
    /* for brevity */
    #define SDM_UI_MENU
  #endif /* .._DIAG_UI_BAUD or .._DS_UI_BAUD or ..SDEVMAP_UI_MENU */
#endif
/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
#ifdef FEATURE_DS_UI_BAUD
sdm_ui_baud_menu_type    sdm_ds_baud_ui_menu;   /* total menu structure */

/*---------------------------------------------------------------------------
  SDM UI DS baud rate menu "Scrollable Options" sub-structure definition:
  -------------------------------------------------------------------------*/
/*lint -e818*/
/*lint -save -e785*/

sdm_ui_baud_item_type sdm_ui_ds_items_struct[SDM_MAX_UI_ITEMS] = {
                     /*  |123456789012|  -- twelve chars per line          */
                     /*   x              -- first char must be blank       */
#ifndef FEATURE_JCDMA_1X
                       { "      9,600 ", SIO_BITRATE_9600   },
                       { "     19,200 ", SIO_BITRATE_19200  },
                       { "     38,400 ", SIO_BITRATE_38400  },
                       { "     57,600 ", SIO_BITRATE_57600  },
#endif
                       { "    115,200 ", SIO_BITRATE_115200 },
#ifdef FEATURE_UART_TCXO_CLK_FREQ
                       { "    230,400 ", SIO_BITRATE_230400 },
#ifdef FEATURE_UART_RATE1
                       { "    460,800 ", SIO_BITRATE_460800 },
#endif
#endif
#ifdef FEATURE_UART_RATE1
                       { "    460,800 ", SIO_BITRATE_460800 },
#endif
#ifdef FEATURE_USE_UART1DM
                       { "    921,600 ", SIO_BITRATE_921600 },
#endif
                       { "\0", SIO_BITRATE_MAX }     /* marks end of list */
};
#endif /* FEATURE_DS_UI_BAUD */

#ifdef FEATURE_DIAG_UI_BAUD
sdm_ui_baud_menu_type    sdm_diag_baud_ui_menu; /* total menu structure */

/*---------------------------------------------------------------------------
  SDM UI DIAG baud rate menu "Scrollable Options" sub-structure definition:
  -------------------------------------------------------------------------*/
sdm_ui_baud_item_type sdm_ui_diag_items_struct[SDM_MAX_UI_ITEMS] = {
                     /*  |123456789012|  -- twelve chars per line          */
                     /*   x              -- first char must be blank       */
                       { "     38,400 ", SIO_BITRATE_38400  },
                       { "    115,200 ", SIO_BITRATE_115200 },
                       { "\0", SIO_BITRATE_MAX }     /* marks end of list */
};
#endif /* FEATURE_DIAG_UI_BAUD */


/*lint restore*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

FUNCTION SDM_INIT                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes the Serial Device Mapper Services

DEPENDENCIES
  Should be called just once.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void sdm_init ( void )
{


#ifdef SDM_UI_MENU
  sdm_menu_init();
#endif /* SDM_UI_MENU */

} /* sdm_init */



#ifdef SDM_UI_MENU
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
)
{
  uint8 i; 

#ifdef FEATURE_DS_UI_BAUD
  /*-------------------------------------------------------------------------
    SDM DS BAUD rate UI MENU initialization
  -------------------------------------------------------------------------*/
  
  sdm_ds_baud_ui_menu.heading =   " DS BAUD    ";    /* */
  sdm_ds_baud_ui_menu.current_index = SDM_UNKNOWN_INDEX;
  
  /* Copy the menu entries */
  
  for( i=0; i < SDM_MAX_UI_ITEMS; i++) {
    sdm_ds_baud_ui_menu.items[i] = & (sdm_ui_ds_items_struct[i]);
    if ( *(sdm_ui_ds_items_struct[i].item_string) == '\0' ) {
      break; /* no more valid entries */
    }
  }
 
  if( i >= SDM_MAX_UI_ITEMS ) {
    ERR_FATAL( "ds baud menu init failure",0,0,0);
  }

  sdm_ds_baud_ui_menu.num_items = i;
#endif /* FEATURE_DS_UI_BAUD */

#ifdef FEATURE_DIAG_UI_BAUD 
  /*-------------------------------------------------------------------------
    SDM DIAG BAUD rate UI MENU initialization
  -------------------------------------------------------------------------*/

  sdm_diag_baud_ui_menu.heading =   " DIAG BAUD: ";    /* */
  sdm_diag_baud_ui_menu.current_index = SDM_UNKNOWN_INDEX;
  
  /* Copy the menu entries */
  
  for( i=0; i < SDM_MAX_UI_ITEMS; i++) {
    sdm_diag_baud_ui_menu.items[i] = & (sdm_ui_diag_items_struct[i]);
    if ( *(sdm_ui_diag_items_struct[i].item_string) == '\0' ) {
      break; /* no more valid entries */
    }
  }
 
  if( i >= SDM_MAX_UI_ITEMS ) {
    ERR_FATAL( "diag baud menu init failure",0,0,0);
  }

  sdm_diag_baud_ui_menu.num_items = i;
#endif /* FEATURE_DIAG_UI_BAUD */

} /* sdm_menu_init */

#endif /* SDM_UI_MENU */

#ifdef FEATURE_DIAG_UI_BAUD
/*===========================================================================

FUNCTION SDM_UI_GET_TABLE_DIAG_BAUD                    EXTERNALIZED FUNCTION

DESCRIPTION
  Returns the DIAG baud selection menu. If NV contains a valid baud rate
  and it is in our list of baud rate choices, we indicate that as the 
  current baud rate - UI will mark this with a "checkmark".

DEPENDENCIES
  None

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
)
{
  uint8 i;
  sio_bitrate_type baud_from_nv;

  sdm_diag_baud_ui_menu.current_index = SDM_UNKNOWN_INDEX;

  if ( nv_result_stat == NV_DONE_S )
  {
    baud_from_nv = (sio_bitrate_type) nv_data_buf->diag_default_baudrate;

    for ( i = 0; i < sdm_diag_baud_ui_menu.num_items ; i++ )
    {
      if ( ((sdm_diag_baud_ui_menu.items[i])->baud_id ) == baud_from_nv )
      {
        sdm_diag_baud_ui_menu.current_index = i;
        break;
      }
    }
  }

#ifdef SDM_VERBOSE_DEBUG
  ERR( "current diag index is %d", sdm_diag_baud_ui_menu.current_index, 0, 0 );
#endif /* SDM_VERBOSE_DEBUG */

  return &sdm_diag_baud_ui_menu;
}

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
)
{
  uint8 i;
  sio_bitrate_type baud_from_nv;

  sdm_ds_baud_ui_menu.current_index = SDM_UNKNOWN_INDEX;

  if ( nv_result_stat == NV_DONE_S )
  {
    baud_from_nv = (sio_bitrate_type) nv_data_buf->ds_default_baudrate;

    for ( i = 0; i < sdm_ds_baud_ui_menu.num_items ; i++ )
    {
      if ( ((sdm_ds_baud_ui_menu.items[i])->baud_id ) == baud_from_nv )
      {
        sdm_ds_baud_ui_menu.current_index = i;
        break;
      }
    }
  }

#ifdef SDM_VERBOSE_DEBUG
  ERR( "current ds index is %d", sdm_ds_baud_ui_menu.current_index, 0, 0 );
#endif /* SDM_VERBOSE_DEBUG */

  return &sdm_ds_baud_ui_menu;
}

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

void sdm_ui_set_diag_baud( uint8 new_baud_index )
{
#ifdef SDM_VERBOSE_DEBUG
  ERR( "UI said set diag baud index to %d", new_baud_index, 0, 0 );
#endif /* SDM_VERBOSE_DEBUG */

  if( new_baud_index < sdm_diag_baud_ui_menu.num_items )
  {
    diag_change_default_sio_bitrate( 
                   (sdm_diag_baud_ui_menu.items[new_baud_index])->baud_id );
  }
  else 
  {
    MSG_HIGH( "Invalid UI DIAG baud %d", new_baud_index, 0, 0 );
  }

}

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

void sdm_ui_set_ds_baud( uint8 new_baud_index )
{
#ifdef SDM_VERBOSE_DEBUG
  ERR( "UI said set ds baud index to %d", new_baud_index, 0, 0 );
#endif /* SDM_VERBOSE_DEBUG */

  if( new_baud_index < sdm_ds_baud_ui_menu.num_items )
  {
#ifdef FEATURE_DATA_MM
    dsat_change_baud( (sdm_ds_baud_ui_menu.items[new_baud_index])->baud_id,
                       SDM_CHANGE_BAUD_NOW );
#else
    ds_change_baud( (sdm_ds_baud_ui_menu.items[new_baud_index])->baud_id,
                     SDM_CHANGE_BAUD_NOW );
#endif /* FEATURE_DATA_MM */
  }
  else 
  {
    MSG_HIGH( "Invalid UI DS baud %d", new_baud_index, 0, 0 );
  }
}


#endif /* FEATURE_DS_UI_BAUD */
