/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ P P P _ L O G G I N G . C

GENERAL DESCRIPTION

  Contains the necessary routines to log framed or unframed PPP frames.
   
EXTERNALIZED FUNCTIONS

  ppp_log_frames_ext()
    This function logs the PPP-in-HDLC frames. The function iterates 
    through the PPP packet chain, logging each packet in succession.

Copyright (c) 2001-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_logging.c_v   1.3   13 Feb 2003 20:13:54   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_logging.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/13/03     jd    removed old ppp_log_frames implementation, no longer used.
                   ppp_log_frames_ext takes device instead of iface,
                   fixed log code calculation
11/19/02     jd    for MM, include ps_ppp_logging.h instead of dsppplog.h
10/29/02    igt    turned MSG_ERROR() to ERR() and changed text
09/19/02    igt    removed annoying MSG_ERR message.
09/12/02     vr    changed ppp_log_frames_ext to use the defines in 
                   log_codes.h
09/06/02    igt    added extended ppp logging capabilities under
                   FEATURE_DEBUG_PPP_LOG_EXT
02/26/02    sjy    Fixed QCAT bug (Log Record exceeding maximum allowed size)
                   by removing extra log_hdr_type buffer in log_alloc().
06/27/01    sjy    Included "comdef.h" to fix warning during compilation.
06/20/01    sjy    Created file.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_PPP_LOGGING

#include "log.h"
#include "err.h"
#include "log_codes.h"
#include "dsm.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_logging.h"


/*===========================================================================
							
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Payload size for optimal logging capacity at high RX speeds over 
  diagnostic interface.
---------------------------------------------------------------------------*/
#define PPP_MAX_BYTES 186

/*---------------------------------------------------------------------------
  Packet structure definition for extended PPP logging
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_PPP_FRAMES_EXT_C)
  byte          data[ PPP_MAX_BYTES ];  /* PPP payload                     */
LOG_RECORD_END

/*---------------------------------------------------------------------------
  Log entry type for naming compatibility.  
---------------------------------------------------------------------------*/
typedef LOG_PPP_FRAMES_EXT_C_type log_ppp_frames_ext_type;



/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION PPP_LOG_FRAMES_EXT

DESCRIPTION

  This function logs the provided PPP frame (HDLC framed or unframed) on
  the diagnostic interface.

  
  The function iterates through the input dsm chain, which may contain 
  multiple PPP, handling each packets each packet in succession.

  It may be called with framed or unframed ppp data, i.e. before or after 
  the HW/SW framing/unframing has occurred.
  
   
PARAMETERS
  item_ptr      - pointer to the dsm chain to be logged
  flavour       - The characteristics of this packet, 
                  i.e.  framed/unframed, rx/tx
  device        - the device over which the packet is passing
  
  This function constructs the appropriate log code from the ppp_data and
  device parameters.

  LOG CODE RANGE      DIRECTION      FRAMING
  --------------      ---------      -------
  275-290             Receive        HDLC
  291-306             Transmit       HDLC
  307-322             Receive        (none)
  323-338             Transmit       (none)
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ppp_log_frames_ext
( 
  dsm_item_type            *item_ptr,
  pppi_log_flavour_e_type  flavour,
  ppp_dev_enum_type        device
)
{
  log_ppp_frames_ext_type  *log_ptr;   /* pointer to log buffer            */
  byte                     *data_ptr;  /* ptr to buffer to copy the data   */
  uint16                   offset;     /* byte offset into PPP chain       */
  uint16                   num_bytes;  /* # of bytes copied by dsm_extract */
  uint16                   len;        /* length of log frame payload      */
  uint16                   log_code;   /* the log_code for the log packet  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (item_ptr == NULL) 
  {
    /*-----------------------------------------------------------------------
      Log item passed in is NULL, so do nothing.
    -----------------------------------------------------------------------*/
    ERR( "NULL item ptr, frame skipped.", 0, 0, 0 );
    return;
  }
  
  /*-------------------------------------------------------------------------
    Calculate the log code

    The log code is the base log code (275) for the flavour of the logging 
    (rx/tx, framed/unframed) plus the interface.  The log codes are 
    arranged such that there are 16 codes for each flavour to represent 
    up to 16 interfaces.

    NOTE: no range checking is done so incoming parameters MUST be correct.
  -------------------------------------------------------------------------*/
  log_code = LOG_PPP_FRAMES_RESERVED_CODES_BASE_C + (flavour * 16) + device;

  /*-------------------------------------------------------------------------
    Determine length of data packet chain.
  -------------------------------------------------------------------------*/
  offset = 0;
  len = dsm_length_packet (item_ptr);            
  do
  {
    /*-----------------------------------------------------------------------
      Allocate log buffer space from diag.
    -----------------------------------------------------------------------*/
    log_ptr = (log_ppp_frames_ext_type *)
                       log_alloc(log_code, sizeof(log_ppp_frames_ext_type));
    
    if (log_ptr == NULL)
    {
      /*---------------------------------------------------------------------
        No log buffer or logging not on, so do nothing.
      ---------------------------------------------------------------------*/
      return; 
    }  
   
    /*-----------------------------------------------------------------------
      Extract data from item_ptr into log buffer.
    -----------------------------------------------------------------------*/
    data_ptr  = (byte *) log_ptr->data;
    num_bytes = PPP_MAX_BYTES;
    dsm_seek_extract (&item_ptr, &offset, data_ptr, &num_bytes);

    offset   += num_bytes;
    len      -= num_bytes;
    
    /*-----------------------------------------------------------------------
      Set buffer length to number of bytes extracted.
    -----------------------------------------------------------------------*/
    log_shorten (log_ptr, sizeof(log_hdr_type) + num_bytes);
    log_set_length (log_ptr, sizeof(log_hdr_type) + num_bytes);

    /*-----------------------------------------------------------------------
      Commit log buffer.
    -----------------------------------------------------------------------*/
    log_commit (log_ptr);

  } while (len);

} /* ppp_log_frames_ext() */

#endif /* FEATURE_DATA_PS_PPP_LOGGING */
