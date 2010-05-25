#ifndef _AP_PACKET_H_
#define _AP_PACKET_H_
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
; GENERAL DESCRIPTION
;	This header file contains externalized definitions from the packet layer.
; It is used by the stream buffering layer.
;
; EXTERNALIZED FUNCTIONS
;   None. 
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;	None
;
;  Copyright (c) 2002 by QUALCOMM Incorporated.  
;  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_packet.h#1 $ $DateTime: 2007/03/05 17:31:24 $ $Author: rtilak $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_packet.h#1 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 10/14/03   drh     Add section of defines to enable flash layer to not
                    overrun reply buffer with flash sector information.
 03/21/03   drh     Initial version

============================================================================*/


/* Maximum data length. */
#define MAX_DATA_LENGTH 1024

/* Number of packets.  NUMBER_OF_PACKETS * MAX_DATA_LENGTH will be our
   maximum window size. */
#define NUMBER_OF_PACKETS 100


/* ----------------------------------------------------------------------
 * Defines used to calculate size of reply buffer to QPST and also
 * number of Flash part sectors that will fit into that reply buffer.
 * Code in packet layer will cause a fatal error if the size of the
 * buffer is exceeded at run time.  Code in NOR flash layer will cause
 * a fatal error at compile time if number of sectors is exceeded.
  ----------------------------------------------------------------------*/

#define HOST_REPLY_BUFFER_SIZE  2048

/* Fixed size elements of Parameter Request Reply packet */
#define PACKET_OVERHEAD_SIZE    7
#define CMD_SIZE                1
#define MAGIC_SIZE              32
#define VERSION_SIZE            1
#define COMPAT_VERSION_SIZE     1
#define BLOCK_SIZE_SIZE         4
#define FLASH_BASE_SIZE         4
#define FLASH_ID_LEN_SIZE       1
#define WINDOW_SIZE_SIZE        2
#define NUM_SECTORS_SIZE        2
#define FEATURE_BITS_SIZE       4

/* Variable size element of Parameter Request Reply packet.  The length 
 * of the Flash ID string is indeterminate, but currently the largest is
 * 16 bytes, so allow double this size for growth. */
#define FLASH_ID_STRING_SIZE    32


/* Add up all the parts except sectors */
#define REPLY_FIXED_SIZE (PACKET_OVERHEAD_SIZE+CMD_SIZE+MAGIC_SIZE+ \
  VERSION_SIZE+COMPAT_VERSION_SIZE+BLOCK_SIZE_SIZE+FLASH_BASE_SIZE+ \
  FLASH_ID_LEN_SIZE+WINDOW_SIZE_SIZE+NUM_SECTORS_SIZE+ \
  FEATURE_BITS_SIZE+FLASH_ID_STRING_SIZE)


#define REPLY_BUFFER_SIZE   HOST_REPLY_BUFFER_SIZE

/* Calculate amount of 4 byte sector sizes which fit in remaining
 * portion of parameter request reply
 */
#define MAX_SECTORS      ((REPLY_BUFFER_SIZE - REPLY_FIXED_SIZE) / 4)


#endif /* _AP_PACKET_H_ */
