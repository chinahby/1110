/*! =========================================================================

                       S I O R S 2 3 2   T R A C E . C
DESCRIPTION
  This file contains code require to log the flow of the driver code
  in a circular buffer.  This is used to assist debugging.
  
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

 \file siors232_trace.c
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/07   rh      Spin off the trace feature into a new file
                               
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "msm.h"
#include "comdef.h"
#include "sio.h"
#include "siors232_trace.h"


#ifdef FEATURE_SIO_DEBUG_TRACE

/*! \def TIMETEST_PORT 
 *  \brief Used if code trace message is forwarded to the time trace port */
/*! \def TIMETEST_SEND(x) 
 *  \brief Macro to send the data out of time test port */
#define TIMETEST_PORT           0xF9410002
#define TIMETEST_SEND(x) outpw (TIMETEST_PORT, x)

/*! \var   siors_trace 
 *  \brief Debug trace variable that stores rx chars, count and code path */
siors_trace_type siors_trace;


/*===========================================================================
 
FUNCTION SIORS_LOG_CODE

DESCRIPTION
   This function logs the code path

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void siors_log_code(uint16 id, uint8 port) 
{
#ifdef SIORS_TEST_LOG_DATA_ON_TIMETEST_PORT
  TIMETEST_SEND(id);
#else
#ifdef SIORS_TRACE_LOG_FILTER
  if(port != SIORS_TRACE_LOG_FILTER){
	  return;
  }
#endif

  siors_trace.code[siors_trace.code_cntr].sio_debug_val=(id) | (port << 24); 
  siors_trace.code[siors_trace.code_cntr++].timestamp= (uint32)timetick_get(); 
  if (siors_trace.code_cntr == SIORS_CODE_TRACE_SIZE)
  {
    siors_trace.code_cntr = 0;
  }
#endif
}

#ifdef FEATURE_SIO_EXTENDED_DEBUG_TRACE
/*===========================================================================
 
FUNCTION SIORS_LOG_RCVD_CHARS

DESCRIPTION
   This function logs the characters received

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void siors_log_rcvd_chars(byte* ptr, uint32 len, uint8 port)
{
  int i;
  uint32 timestamp = timetick_get();

#ifdef SIORS_TRACE_LOG_FILTER
  if(port != SIORS_TRACE_LOG_FILTER){
	  return;
  }
#endif

  for(i=0;i<len;i++)  
  {
    if(siors_trace.rcvd_chars_cntr == SIORS_CODE_TRACE_SIZE)
    {
      siors_trace.rcvd_chars_cntr = 0;
    }
    siors_trace.rcvd_chars[siors_trace.rcvd_chars_cntr].sio_debug_val 
                                      = (*ptr++) | (port << 24);
    siors_trace.rcvd_chars[siors_trace.rcvd_chars_cntr++].timestamp 
                                      = timestamp;
  }
}


/*===========================================================================
 
FUNCTION SIORS_LOG_CHAR_COUNT_RCVD

DESCRIPTION
   This function logs the number of characters received

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void siors_log_char_count_rcvd(uint32 id, uint8 port)
{
#ifdef SIORS_TRACE_LOG_FILTER
  if(port != SIORS_TRACE_LOG_FILTER){
	  return;
  }
#endif

  siors_trace.num_rcvd_chars[siors_trace.num_rcvd_chars_cntr].sio_debug_val 
                                      = (id & 0x00ffffff) | (port << 24);
  siors_trace.num_rcvd_chars[siors_trace.num_rcvd_chars_cntr++].timestamp 
                                      = (uint32)timetick_get();
  if ( siors_trace.num_rcvd_chars_cntr == SIORS_TRACE_SIZE )
  {
    siors_trace.num_rcvd_chars_cntr = 0;
  }
}


/*===========================================================================
 
FUNCTION SIORS_LOG_CHAR_COUNT_TX

DESCRIPTION
   This function logs the number of characters transmitted.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void siors_log_char_count_tx(uint32 id, uint8 port)
{
#ifdef SIORS_TRACE_LOG_FILTER
  if(port != SIORS_TRACE_LOG_FILTER){
	  return;
  }
#endif

  siors_trace.num_tx_chars[siors_trace.num_tx_chars_cntr].sio_debug_val 
                                      = (id & 0x00ffffff) | (port << 24);
  siors_trace.num_tx_chars[siors_trace.num_tx_chars_cntr++].timestamp 
                                      = (uint32)timetick_get();
  if ( siors_trace.num_tx_chars_cntr == SIORS_TRACE_SIZE )
  {
    siors_trace.num_tx_chars_cntr = 0;
  }
}

/*===========================================================================
 
FUNCTION SIORS_LOG_TXED_CHARS

DESCRIPTION
   This function logs the characters transmitted

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void siors_log_txed_chars(byte* ptr, uint32 len, uint8 port)
{
  int i;
  uint32 timestamp = timetick_get();

#ifdef SIORS_TRACE_LOG_FILTER
  if(port != SIORS_TRACE_LOG_FILTER){
	  return;
  }
#endif

  for(i=0;i<len;i++)  
  {
    if(siors_trace.txed_chars_cntr == SIORS_CODE_TRACE_SIZE)
    {
      siors_trace.txed_chars_cntr = 0;
    }
    siors_trace.txed_chars[siors_trace.txed_chars_cntr].sio_debug_val 
                                      = (*ptr++) | (port << 24);
    siors_trace.txed_chars[siors_trace.txed_chars_cntr++].timestamp 
                                      = timestamp;
  }
}

#endif /* Extended trace functions */

#endif /* ifdef FEATURE_SIO_DM_TRACE */
