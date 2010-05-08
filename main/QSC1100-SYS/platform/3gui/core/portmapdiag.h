#ifndef PORTMAPDIAG_H
#define PORTMAPDIAG_H

/*============================================================================
   Enumerated types for portmap diag
============================================================================*/

/*
    Enumerated type defining available application command codes for portmap
    settings.
*/

typedef enum
{
  PORTMAPDIAG_START = 0,
  PORTMAPDIAG_END,
  PORTMAPDIAG_DATA_MENU_HOME,
  PORTMAPDIAG_DATA_NULL,
  PORTMAPDIAG_DATA_UART1,
//5
  PORTMAPDIAG_DATA_USBCDC,
  PORTMAPDIAG_DATA_BTNA,
  PORTMAPDIAG_DIAG_MENU_HOME,
  PORTMAPDIAG_DIAG_NULL,
  PORTMAPDIAG_DIAG_UART1,
//10
  PORTMAPDIAG_DIAG_UART2,
  PORTMAPDIAG_DIAG_USB,
  PORTMAPDIAG_GPS_NMEA_MENU_HOME,
  PORTMAPDIAG_GPS_NMEA_NULL,
  PORTMAPDIAG_GPS_NMEA_UART1,
//15
  PORTMAPDIAG_GPS_NMEA_UART2,
  PORTMAPDIAG_MAX_CMD,
  PORTMAPDIAG_FORCE_4_BYTES = 0x10000000
} PORTMAPDIAG_CommandsType;

/*============================================================================

PACKET   PORTMAPDIAG_CMD

PURPOSE  Sent from the DM to the DMSS to execute a particular portmap function.

RESPONSE The DM sends back a portmapdiag_cmd_rsp_type packet to give the 
         status of the request.

============================================================================*/
typedef struct
{
  diagpkt_subsys_header_type header;

} portmapdiag_cmd_rsp_type;

#endif /*  PORTMAPDIAG_H */
