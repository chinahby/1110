/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  H D L C   H A R D W A R E   D R I V E R

GENERAL DESCRIPTION
  This is the HDR HDLC driver.  It provides an interface to the HDR
  HDLC hardware.  

EXTERNALIZED FUNCTIONS
  
  HDLC_WRITE_BUF
    Writes data into deframer input buffer.
  
  HDLC_READ_BUF
    Reads data out of deframer output buffer.
  
  HDLC_DEFRAMER_CMD
    Sends a command to deframer.
  
  HDLC_FRAMER_CMD
    Sends a command to framer.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  
  
Copyright (c) 2001, 2002 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/hdlc_drv.c_v   1.0   11 Oct 2002 09:14:10   ubabbar  $
$Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/hdlc_drv.c#2 $ $DateTime: 2008/11/24 21:19:34 $ $Author: sapnab $
   
when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/24/08    scb    Replaced inclusion of hdrasm.h with miscasm.h
10/11/02    usb    Merging file to MM_DATA archive from COMMON
08/09/02    mvl    Added FEATURE_HDLC_HW_ACCEL
05/20/02    vr     Fixed some bugs in hdlc_read_buf and code review changes
12/31/01    sq     Added two new functions: hdlc_deframer_cmd and 
                   hdlc_framer_cmd.
08/20/01    sq     Created.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdlc_drv.h"
#include "miscasm.h"
#include "err.h"
#include "assert.h"


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
                                
===========================================================================*/

#if defined(FEATURE_PPP_HW_ACCEL) || defined(FEATURE_HDLC_HW_ACCEL)
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION    HDLC_WRITE_BUF                                          
    Writes data into deframer/framer input buffer. 
  
DEPENDENCIES
  None.
  
PARAMETERS
  buf is the buffer to be written. 
  dest is the destination to write to - framer or deframer
  length is the number of bytes to write from buf

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdlc_write_buf
( 
  byte *buf, 
  hdlc_hw_buf_type dest, 
  uint16 length
)
{
  int      tmp;
  uint16   len;
  uint32   addr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure buf is a non-zero address
  -------------------------------------------------------------------------*/
  if( buf == NULL)
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
    Get the physical address of the framer/deframer to write to
    Also cap the max bytes transferred to the framer/deframer buffer size
  -------------------------------------------------------------------------*/
  switch (dest)
  {
    case HDLC_FRAMER_DATA_BUF:
      addr = FRAMER_DATA_IN_WW;
      if(length > HDLC_FRAMER_DATA_BUF_SIZE)
      {
        length = HDLC_FRAMER_DATA_BUF_SIZE;
      }
      break;

    case HDLC_DEFRAMER_DATA_BUF:
      addr = DEFRAMER_DATA_IN_WW;
      if(length > HDLC_DEFRAMER_DATA_BUF_SIZE)
      {
        length = HDLC_DEFRAMER_DATA_BUF_SIZE;
      }
      break;

    default:
      ASSERT(0);
      return 0;
  } /* switch (dest) */

  /*-------------------------------------------------------------------------
    If length is less than 4 bytes, simply byte transfer them
  -------------------------------------------------------------------------*/
  if(length < HDLC_ALIGN_SIZE)
  {
    for(tmp = 0; tmp < length; tmp++)
    {
      outp(addr,*buf);
      buf++;
    }
    return length;
  } /* if(length < HDLC_ALIGN_SIZE) */

  /*-------------------------------------------------------------------------
    Byte-transferring data until we read a 4-byte boundary
  -------------------------------------------------------------------------*/
  tmp = HDLC_ALIGN_SIZE - (((uint32)buf) % HDLC_ALIGN_SIZE);
  len = length - tmp;

  while(  tmp   !=  0   )
  {
    outp(addr,*buf);
    buf++;
    tmp--;
  }

  tmp = len % HDLC_ALIGN_SIZE;

  /*-------------------------------------------------------------------------
    Burst write data into the deframer till we hit the final 4-byte boubdary
    in the input buffer
  -------------------------------------------------------------------------*/
  if(len >= HDLC_ALIGN_SIZE) 
  {
    hdrasm_write_burst_data((uint32 *) buf,
                            (uint32 *) addr,
                            len/HDLC_ALIGN_SIZE
                           );
  }

  buf +=  (len - tmp);

  /*-------------------------------------------------------------------------
    Write the residual bytes into the deframer
  -------------------------------------------------------------------------*/
  while(  tmp   !=  0   )
  {
    outp(addr,*buf);
    buf++;
    tmp--;
  }

  return length;
} /* hdlc_write_buf */

/*===========================================================================

FUNCTION    HDLC_READ_BUF                                           
    Reads data out of deframer output buffer. 
  
DEPENDENCIES
  buf must be 4-byte aligned
  
PARAMETERS
  buf is the buffer to be read into. 
  src is the source to read from - framer or deframer
  length is the number of bytes to read

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
)
{
  int      tmp,i;
  uint32   tail_bytes;
  uint32   addr;
    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure buf is a non-zero address
  -------------------------------------------------------------------------*/
  if( buf == NULL)
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
    buf must be 4-byte aligned
  -------------------------------------------------------------------------*/
  ASSERT( (((uint32) buf) % HDLC_ALIGN_SIZE) == 0);

  /*-------------------------------------------------------------------------
    Get the physical address of the framer/deframer to write to
    Also cap the max bytes to read to the framer/deframer buffer size
  -------------------------------------------------------------------------*/
  switch (src)
  {
    case HDLC_FRAMER_DATA_BUF:
      addr = FRAMER_DATA_OUT_RW;
      if(length > HDLC_FRAMER_DATA_BUF_SIZE)
      {
        length = HDLC_FRAMER_DATA_BUF_SIZE;
      }
      break;

    case HDLC_DEFRAMER_DATA_BUF:
      addr = DEFRAMER_DATA_OUT_RW;
      if(length > HDLC_DEFRAMER_DATA_BUF_SIZE)
      {
        length = HDLC_DEFRAMER_DATA_BUF_SIZE;
      }
      break;

    default:
      ASSERT(0);
      return 0;
  } /* switch (src) */

  /*-------------------------------------------------------------------------
    Burst read the data till less than 4 bytes are remaining to be read
  -------------------------------------------------------------------------*/
  hdrasm_read_burst_data((uint32 *) addr,
                         (uint32 *) buf,
                         length/HDLC_ALIGN_SIZE);

  /*-------------------------------------------------------------------------
    Byte read the remaining bytes
  -------------------------------------------------------------------------*/
  tmp = length % HDLC_ALIGN_SIZE;
  if(tmp != 0)
  {
    tail_bytes = inpdw(addr);
    for(i=0; i < tmp; i++)
    {
      (*(buf + length/HDLC_ALIGN_SIZE*HDLC_ALIGN_SIZE + i)) = 
                                    ((byte *) &tail_bytes)[i];
    }
  } /* if(tmp != 0) */

  return length;
} /* hdlc_read_buf */

/*===========================================================================
FUNCTION HDLC_DEFRAMER_CMD

DESCRIPTION
  This function sends a command to the HDLC deframer.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - the command to send

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdlc_deframer_cmd(hdlc_deframer_cmd_type cmd)
{
  switch (cmd) 
  {
    case HDLC_DEFRAMER_RESET:
    case HDLC_DEFRAMER_WRITE_DONE:
      outp(DEFRAMER_CMD_WB,   cmd );
      break;

    default:
      ERR_FATAL("Invalid deframer command",0,0,0); 

  }
} /* hdlc_deframer_cmd */

/*===========================================================================
FUNCTION HDLC_FRAMER_CMD

DESCRIPTION
  This function sends a command to the HDLC framer.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - the command to send

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdlc_framer_cmd(hdlc_framer_cmd_type cmd)
{
  switch (cmd) 
  {
    case HDLC_FRAMER_RESET:
    case HDLC_FRAMER_WRITE_DONE:
    case HDLC_FRAMER_INSERT_FLAG:
    case HDLC_FRAMER_INSERT_CRC:
    case HDLC_FRAMER_DRAIN:
      outp(FRAMER_CMD_WB,   cmd );
      break;

    default:
      ERR_FATAL("Invalid framer command",0,0,0); 
  }
} /* hdlc_framer_cmd */
#endif /* FEATURE_PPP_HW_ACCEL || FEATURE_HDLC_HW_ACCEL */
