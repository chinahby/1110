/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Port Map Diagnostic Interface 

Supports automated test of the Port Mapping menu via DIAG commands  

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //depot/asic/msmshared/apps/Core/portmapdiag.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/06   cvs     Fix compile warnings
08/05/03   kar     Fixed key sequence due to Time & Date menu removal
02/27/03   kp      Created
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "diagcmd.h"
#include "diagpkt.h"

#include "portmapdiag.h"

#include "hsdiag.h"

#ifdef FEATURE_PORTMAP_TEST_AUTOMATION

/*===========================================================================
    Define the keypress table here.
===========================================================================*/
//XXX#define PORTMAPDIAG_MAX_KEY_PRESSES 18


/* *INDENT-OFF* */
HSDIAG_KEYMAP_DECLARE (portmapdiag_keymap)

  /*  Key sequence to get to the portmap settings from the main
      menu's top left icon (9 keys)*/
  HSKM_ACT( PORTMAPDIAG_START ), HSKM( DOWN ), HSKM( RIGHT ), HSKM( SEL ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( SEL ),
    HSKM( SEL ),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (3 keys) */
  HSKM_ACT( PORTMAPDIAG_END ), HSKM( END ), HSKM( UP ), HSKM( LEFT ),

  /*  Key sequence to get to the top of the data portmap setting (13 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM ( UP ),

  /*  Key sequence to set the data port to NULL and return to main menu home (17 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_NULL ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_UART1 ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to USB CDC and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_USBCDC ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_BTNA ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to get to the top of the diag portmap setting (13 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set the diag port to NULL and return to main menu home (17 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_NULL ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_UART1 ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to UART2 and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_UART2 ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to USB and return to main menu home (20 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_USB ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to get to the top of the gps portmap setting (14 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set the GPS NMEA port to NULL and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_NULL ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the GPS NMEA port to UART1 and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_UART1 ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the GPS NMEA port to UART2 and return to main menu home (20 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_UART2 ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END )

HSDIAG_KEYMAP_END
HSDIAG_KEYMAP_DECLARE (portmapdiag_keymap2)

  /*  Key sequence to get to the portmap settings from the main
      menu's top left icon (9 keys)*/
  HSKM_ACT( PORTMAPDIAG_START ), HSKM( DOWN ), HSKM( RIGHT ), HSKM( SEL ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( SEL ),
    HSKM( SEL ),

  /*  Key sequence to end and return to the main menu with the top left
      icon selected (3 keys) */
  HSKM_ACT( PORTMAPDIAG_END ), HSKM( END ), HSKM( UP ), HSKM( RIGHT ), 
    HSKM( RIGHT ), HSKM( UP ),

  /*  Key sequence to get to the top of the data portmap setting (13 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM ( UP ),

  /*  Key sequence to set the data port to NULL and return to main menu home (17 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_NULL ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_UART1 ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to USB CDC and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_USBCDC ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the data port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DATA_BTNA ), HSKM_SEQ( PORTMAPDIAG_DATA_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to get to the top of the diag portmap setting (13 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( SEL ), HSKM( UP ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set the diag port to NULL and return to main menu home (17 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_NULL ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to UART1 and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_UART1 ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to UART2 and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_UART2 ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the diag port to USB and return to main menu home (20 keys) */
  HSKM_ACT( PORTMAPDIAG_DIAG_USB ), HSKM_SEQ( PORTMAPDIAG_DIAG_MENU_HOME ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to get to the top of the gps portmap setting (14 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM_SEQ( PORTMAPDIAG_START ), 
    HSKM( DOWN ), HSKM( DOWN ), HSKM( SEL ), HSKM( UP ), HSKM( UP ),

  /*  Key sequence to set the GPS NMEA port to NULL and return to main menu home (18 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_NULL ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the GPS NMEA port to UART1 and return to main menu home (19 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_UART1 ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( DOWN ), HSKM( SEL ), 
    HSKM_SEQ( PORTMAPDIAG_END ),

  /*  Key sequence to set the GPS NMEA port to UART2 and return to main menu home (20 keys) */
  HSKM_ACT( PORTMAPDIAG_GPS_NMEA_UART2 ), 
    HSKM_SEQ( PORTMAPDIAG_GPS_NMEA_MENU_HOME ), HSKM( DOWN ), HSKM( DOWN ), 
    HSKM( SEL ), HSKM_SEQ( PORTMAPDIAG_END )

HSDIAG_KEYMAP_END
/* *INDENT-ON* */

/*===========================================================================

FUNCTION PORTMAPDIAG_CMD

DESCRIPTION
  Performs the command specified in the request packet.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean portmap_hack = 0;

PACKED void *
portmapdiag_handler (PACKED void *req, word pkt_len)
{
  portmapdiag_cmd_rsp_type *rsp = NULL;

  PORTMAPDIAG_CommandsType command_code = PORTMAPDIAG_START;

  if (req)
  {
    command_code = (PORTMAPDIAG_CommandsType) diagpkt_subsys_get_cmd_code (req);

    /*  Verify that the command code is valid */
    if (command_code >= PORTMAPDIAG_MAX_CMD)
    {
      rsp = (portmapdiag_cmd_rsp_type *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len);
    }
    else
    {
      rsp = (portmapdiag_cmd_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_PORT_MAP_SETTINGS,
        command_code, pkt_len);

      if (rsp)
      {
        /*  Send out our packet */
        diagpkt_commit (rsp);

        rsp = NULL;
      }

      /* Call keypress_handler with proper table reference.  The table is 
         constructed using command code as the action code. */
//XXX
      if (portmap_hack == 0)
      {
        hsdiag_keymap (command_code, portmapdiag_keymap);
      }
      else
      {
        hsdiag_keymap (command_code, portmapdiag_keymap2);
      }
    }
  
  }

  return (rsp);
}

/*  Map the diag functions to the proper commands */

static const diagpkt_user_table_entry_type portmapdiag_diag_tbl[] = {
  {PORTMAPDIAG_START, PORTMAPDIAG_MAX_CMD, portmapdiag_handler}
};

void
portmapdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_PORT_MAP_SETTINGS, portmapdiag_diag_tbl);
}

#endif /*FEATURE_PORTMAP_TEST_AUTOMATION*/

