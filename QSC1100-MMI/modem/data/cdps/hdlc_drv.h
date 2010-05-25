#ifndef HDLC_DRV_H
#define HDLC_DRV_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H D L C    D R I V E R
                    
                        H E A D E R    F I L E

GENERAL DESCRIPTION
  This module contains definitions and declarations for using the 
  HDLC ( High-level Data Link Control ) driver.
  

EXTERNALIZED FUNCTIONS
  
  HDLC_WRITE_BUF
    Writes data into deframer/framer input buffer.
  
  HDLC_READ_BUF
    Reads data out of deframer/framer output buffer.

  HDLC_DEFRAMER_CMD
    Send a command to HDLC deframer. The commands include:
      reset:      
        The HDLC deframer software reset.
      write_done: 
        Indicates that a complete RLP packet has been written to input buffer
  
  HDLC_FRAMER_CMD        
    Send a command to HDLC framer. The commands include:
      reset:
        The HDLC framer software reset.
      insert_flag:
        Forces the framer to insert a flag byte (0x7e) into the framer buffer.
      insert_crc:
        Forces the framer to insert the two CRC byts into the framer buffer.
      drain_buf:
        Forces the residual bytes to be written into the framer buffer.
      write_done:
        Indicates that a PPP packet has been written to the input buffer.
  
EXTERNALIZED MACROS
  
        
  HDLC_DEFRAMER_READ_HEADER
    Reads the next deframer header structure from deframer output buffer into
    the passed struct.
    
  HDLC_DEFRAMER_GET_HEADER_CNT
    Returns the number of header structurs inside the output buffer.
    
  HDLC_DEFRAMER_GET_BYTE_CNT
    Returns the number of deframer output buffer bytes that have been used.
    
  HDLC_DEFRAMER_CRC_IS_VALID
    Indicates whether a PPP's CRC is valid by looking at the header structure.
    
  HDLC_DEFRAMER_PPP_ENDS
    Indicates whether we reach the end of a PPP packet.    
    
  HDLC_DEFRAMER_NO_ERR
    Returns TRUE if there is no error occured during deframing.
    
  HDLC_FRAMER_GET_BYTE_CNT
    Returns the number of framer output buffer bytes that have been used.
  
  HDLC_FRAMER_SET_ACCM
    Set the ACCM value for the framer.
  
  HDLC_FRAMER_NO_ERR
    Returns TRUE if there is no error occured during framing.

  
Copyright (c) 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/hdlc_drv.h_v   1.2   04 Dec 2002 00:09:26   vramaswa  $
$Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/hdlc_drv.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $
   
when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/02/02    vr     Reduced HDLC_DATA_BUF_SIZE to prevent the HW 
                   framer buffer from overflowing if all the bytes need to
                   be escaped.
10/08/02    vr     Reduced HDLC_MAX_FRAMER_BYTES_SMALL_POOL by 1 to prevent
10/11/02    usb    Merging file to MM_DATA archive from COMMON
10/08/02    vr     Reduced HDLC_MAX_FRAMER_BYTES_SMALL_POOL by 1 to prevent
                   potential memory scribble
08/09/02    mvl    Added FEATURE_HDLC_HW_ACCEL
07/30/02    igt    remove macros HDLC_FRAMER_CLEAR_ERR and 
                   HDLC_DEFRAMER_CLEAR_ERR as the hw is now read only
06/05/02    dwp    Modify header inclusion criteria.
05/20/02    vr     Changed interfaces for hdlc_read/write_buf and code review
                   changes
12/31/01    sq     Made deframer/framer commands to be enum type.
08/12/01    sq     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "msm.h"
#ifdef FEATURE_HDR
#error code not present
#endif

#if defined(FEATURE_PPP_HW_ACCEL) || defined(FEATURE_HDLC_HW_ACCEL)

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Deframer command definitions
---------------------------------------------------------------------------*/
typedef enum
{
  HDLC_DEFRAMER_RESET      = 0x0,
  HDLC_DEFRAMER_WRITE_DONE = 0x1,
  HDLC_DEFRAMER_CMD_MAX
} hdlc_deframer_cmd_type;

/*---------------------------------------------------------------------------
  Deframer header field masks
---------------------------------------------------------------------------*/
#define     HDLC_CRC_PASS_MASK          0x80000000
#define     HDLC_PPP_END_MASK           0x40000000
#define     HDLC_PPP_LENGTH_MASK        0x000000ff

/*---------------------------------------------------------------------------
  Framer/deframer input data buffer sizes
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_DATA_BUF_SIZE  256
#define     HDLC_FRAMER_DATA_BUF_SIZE    256

/*---------------------------------------------------------------------------
  The hardware expects 4-byte alignment of certain memory addresses
---------------------------------------------------------------------------*/
#define     HDLC_ALIGN_SIZE              4

/*---------------------------------------------------------------------------
  Framer command definitions
---------------------------------------------------------------------------*/
typedef enum
{
  HDLC_FRAMER_RESET        = 0x0,
  HDLC_FRAMER_WRITE_DONE   = 0x1,
  HDLC_FRAMER_INSERT_FLAG  = 0x2,
  HDLC_FRAMER_INSERT_CRC   = 0x3,
  HDLC_FRAMER_DRAIN        = 0x4,
  HDLC_FRAMER_CMD_MAX
} hdlc_framer_cmd_type;

/*---------------------------------------------------------------------------
  Value to write to error buffer to clear framer/deframer error
---------------------------------------------------------------------------*/
#define     HDLC_CLR_ERR_CMD            0x1

/*---------------------------------------------------------------------------
  Framer or deframer i/o buffer id
---------------------------------------------------------------------------*/
typedef enum
{
  HDLC_FRAMER_DATA_BUF     = 0x0,
  HDLC_DEFRAMER_DATA_BUF   = 0x1,
  HDLC_HW_BUF_MAX
} hdlc_hw_buf_type;

/*---------------------------------------------------------------------------
  Externalized constants needed for using the HDLC hardware framer/deframer
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  When framing the packet, we need to determine which pool to use to store 
  the framer output in, based on the number of bytes in the packet that is
  input to the framer. If the determined pool is running low on memory, 
  the other pool may be used.
 
  Small item has N bytes, but we may need to shift the data_ptr by
  upto 3 bytes if it is not aligned at the 4-byte boundary. So, we have
  a max of N-3 bytes. In the worst case, every byte in the original PPP
  packet may need to be escaped which gives a max input size of (N-3)/2 if
  we use small items. Use one less than this calculated value to take into
  consideration the possible 7E that might be added to the DSM item.
---------------------------------------------------------------------------*/
#define HDLC_MAX_FRAMER_BYTES_SMALL_POOL                                    \
                ((int)(((DSM_DS_SMALL_ITEM_SIZ - 3)/2) - 1))

/*---------------------------------------------------------------------------
  The data buffer size of the hardware framer is 256 bytes. 
---------------------------------------------------------------------------*/
#define HDLC_DATA_BUF_SIZE 255

/*---------------------------------------------------------------------------
  The data buffer size of the hardware framer is 25r56 bytes. But in the
  worst case, we may need to escape every byte. So limit the input size to
  the framer to 127 bytes.
---------------------------------------------------------------------------*/
#define HDLC_MAX_FRAMER_INPUT_SIZE (HDLC_DATA_BUF_SIZE/2)

/*---------------------------------------------------------------------------
  The maximum output buffer size for the hardware deframer is 256 bytes.
  However, the output of the deframer contains one 4-byte header for each
  complete PPP packet that is deframed and usually contains one 4-byte
  trailer at the end of the output buffer. In the worst case, none of the
  bytes in the input PPP packets to the deframer could be escaped and the 
  total number of the bytes in the deframer output will be equal to

  total_input_bytes + (num_hdrs * 4) - num_7Es_delimiting_PPP_frames

  As the result the maximum number of bytes we can input to the deframer
  in one write is less than 256. The very worst case would be all one byte
  packets to the deframer that would give us total_input_bytes = 256/4 = 64
---------------------------------------------------------------------------*/
#define HDLC_MAX_DEFRAMER_INPUT_SIZE (HDLC_DATA_BUF_SIZE/4)


/*===========================================================================

                      MACRO  DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Read the 32-bit header structure from the deframer output buffer
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_READ_HEADER(hdr)                          \
            (   hdr = inpdw(DEFRAMER_DATA_IN_WW))
            
/*---------------------------------------------------------------------------
  Returns the number of header structurs inside the output buffer
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_GET_HEADER_CNT()                          \
            inp(DEFRAMER_HEADER_CNT_RB)

/*---------------------------------------------------------------------------
  Returns the number of deframer output buffer bytes that have been used
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_GET_BYTE_CNT()                            \
            inp(DEFRAMER_BYTE_CNT_RB)

/*---------------------------------------------------------------------------
  Returns the number of valid PPP bytes
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_GET_PPP_LENGTH(hdr)                       \
            (hdr & HDLC_PPP_LENGTH_MASK)

/*---------------------------------------------------------------------------
  Indicates whether a PPP's CRC is valid by looking at the header structure
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_CRC_IS_VALID(hdr)                         \
            ((boolean)((hdr & HDLC_CRC_PASS_MASK) != 0)) 

/*---------------------------------------------------------------------------
  Indicates whether we reach the end of a PPP packet
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_PPP_ENDS(hdr)                         \
            ((boolean)((hdr & HDLC_PPP_END_MASK) != 0))

/*---------------------------------------------------------------------------
  Returns TRUE if there is no error occured during deframing
---------------------------------------------------------------------------*/
#define     HDLC_DEFRAMER_NO_ERR()                                  \
            ((boolean)((inp(DEFRAMER_ERROR_RB)) == 0))

/*---------------------------------------------------------------------------
  Returns the number of framer output buffer bytes that have been used
---------------------------------------------------------------------------*/
#define     HDLC_FRAMER_GET_BYTE_CNT()                              \
            inp(FRAMER_BYTE_CNT_RB)            
            
/*---------------------------------------------------------------------------
  Set the ACCM value for the framer
---------------------------------------------------------------------------*/
#define     HDLC_FRAMER_SET_ACCM(x)                                 \
            outpdw(FRAMER_ACCM_VALUE_WW, (uint32) x)

/*---------------------------------------------------------------------------
  Returns TRUE if there is no error occured during framing
---------------------------------------------------------------------------*/
#define     HDLC_FRAMER_NO_ERR()                                    \
            ((boolean)((inp(FRAMER_ERROR_RB)) == 0))


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION HDLC_WRITE_BUF

DESCRIPTION
  This function writes data into deframer/framer input buffer.
  
DEPENDENCIES
  None.

PARAMETERS
  buf is the buffer to be written. 
  dest is the destination to write to - framer or deframer
  length is the number of bytes to write from buf

RETURN VALUE
  The number of bytes written to the framer/deframer

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdlc_write_buf
( 
  byte *buf, 
  hdlc_hw_buf_type dest, 
  uint16 length
);


/*===========================================================================
FUNCTION HDLC_READ_BUF

DESCRIPTION
  This function reads data out of deframer/framer output buffer.
  
DEPENDENCIES
  None.

PARAMETERS
  buf is the buffer to be read into. 
  src is the source to read from - framer or deframer
  length is the number of bytes to read into buf

RETURN VALUE
  Number of bytes read out of the output buffer.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdlc_read_buf
( 
  byte *buf, 
  hdlc_hw_buf_type src, 
  uint16 length
);


/*===========================================================================
FUNCTION HDLC_DEFRAMER_CMD

DESCRIPTION
  This function sends a command to the HDLC deframer.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd is the command to be sent

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdlc_deframer_cmd(hdlc_deframer_cmd_type cmd);


/*===========================================================================
FUNCTION HDLC_FRAMER_CMD

DESCRIPTION
  This function sends a command to the HDLC framer.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd is the command to be sent

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdlc_framer_cmd(hdlc_framer_cmd_type cmd);

#endif /* FEATURE_PPP_HW_ACCEL || FEATURE_HDLC_HW_ACCEL */

#endif /* HDLC_DRV_H */

