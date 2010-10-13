#ifndef SIORS232_TRACE_H
#define SIORS232_TRACE_H

/*! =========================================================================

                       S I O R S 2 3 2   T R A C E . H
DESCRIPTION
  This file contains the code to implement a debug trace buffer for
  all UART devices
   
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

 \file siors232_trace.h
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/07   rh      Spin off the trace feature into a new file
                               
===========================================================================*/

/*===========================================================================
                         Constant Defines.
===========================================================================*/

/* Debug trace defines */
#ifdef FEATURE_SIO_DEBUG_TRACE

/*! \def   SIORS_TRACE_SIZE
 *  \brief Number of entries of the trace buffer for extended trace function */
/*! \def   SIORS_TX_RX_CODE_TRACE_SIZE
 *  \brief Number of entries of the trace buffer for code trace function */
#define SIORS_TRACE_SIZE 2048
#define SIORS_CODE_TRACE_SIZE 10000

#endif


/*===========================================================================
                         DATA DECLARATIONS
===========================================================================*/

#ifdef FEATURE_SIO_DEBUG_TRACE
/*! Simple structure to store debug information with time stemp */
typedef struct
{
  uint32 sio_debug_val;			/*!< Debug code */
  uint32 timestamp;				/*!< Time stemp value */
} trace_uint32;

/*! A structure to contain all the debug trace information */
typedef struct
{
  trace_uint32 code[SIORS_CODE_TRACE_SIZE];
  uint32       code_cntr;
#ifdef FEATURE_SIO_EXTENDED_DEBUG_TRACE
  trace_uint32 rcvd_chars[SIORS_CODE_TRACE_SIZE];
  uint32       rcvd_chars_cntr;
  trace_uint32 num_rcvd_chars[SIORS_TRACE_SIZE];
  uint32       num_rcvd_chars_cntr;
  trace_uint32 num_sent_chars[SIORS_TRACE_SIZE];
  uint32       num_sent_chars_cntr;
  trace_uint32 txed_chars[SIORS_CODE_TRACE_SIZE];
  uint32       txed_chars_cntr;
  trace_uint32 num_tx_chars[SIORS_TRACE_SIZE];
  uint32       num_tx_chars_cntr;
#endif /* Extended trace functions */
} siors_trace_type;

#endif


#ifdef FEATURE_SIO_DEBUG_TRACE
/*! =========================================================================
 
FUNCTION SIORS_LOG_CODE

DESCRIPTION
   This function logs the code path

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param id Code ID to be logged
 \param port Extra port id information to be logged along 
===========================================================================*/
void siors_log_code(uint16 id, uint8 port);  

#ifdef FEATURE_SIO_EXTENDED_DEBUG_TRACE
/*! =========================================================================
 
FUNCTION SIORS_LOG_CHAR_COUNT_RCVD

DESCRIPTION
   This function logs the number of characters received

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param id Count of character to be logged
===========================================================================*/
void siors_log_char_count_rcvd(uint32 id, uint8 port);

/*! =========================================================================
 
FUNCTION SIORS_LOG_RCVD_CHARS

DESCRIPTION
   This function logs the characters received

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param ptr Pointer to the character buffer
 \param len Length of valid data in the buffer
===========================================================================*/
void siors_log_rcvd_chars(byte* ptr, uint32 len, uint8 port);

/*! =========================================================================
 
FUNCTION SIORS_LOG_CHAR_COUNT_SENT

DESCRIPTION
   This function logs the number of characters sent up to the upper layers.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param id number of character sent to media layer
===========================================================================*/
void siors_log_char_count_sent(uint32 id, uint8 port);

/*! =========================================================================
 
FUNCTION SIORS_LOG_CHAR_COUNT_TX

DESCRIPTION
   This function logs the number of characters transmitted.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param id number of character actually being transmitted
===========================================================================*/
void siors_log_char_count_tx(uint32 id, uint8 port);

/*! =========================================================================
 
FUNCTION SIORS_LOG_TXED_CHARS

DESCRIPTION
   This function logs the characters begin transmitted

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

 \param ptr Pointer to the character buffer
 \param len Length of valid data in the buffer
===========================================================================*/
void siors_log_txed_chars(byte* ptr, uint32 len, uint8 port);
#endif /* Extended trace functions */
#endif /* FEATURE_SIO_DEBUG_TRACE */


/* Remap the trace MACRO to nothing if trace is not used */
/* Number of less used log is removed to reduce CPU load */
#ifdef FEATURE_SIO_DEBUG_TRACE

#define SIORS_TRACE_CODE(id, port)       siors_log_code((id), (port))

#ifdef FEATURE_SIO_EXTENDED_DEBUG_TRACE
#define SIORS_TRACE_RCVD_CHARS(ptr,len,port)  siors_log_rcvd_chars((ptr),(len),(port))
#define SIORS_TRACE_CHAR_COUNT_RCVD(id,port)  siors_log_char_count_rcvd((id),(port))
#define SIORS_TRACE_CHAR_COUNT_SENT(id,port)  siors_log_char_count_sent((id),(port))
#define SIORS_TRACE_TXED_CHARS(ptr,len,port)  siors_log_txed_chars((ptr),(len),(port))
#define SIORS_TRACE_TX_CHAR_COUNT(id,port)    siors_log_char_count_tx((id),(port))
#else
#define SIORS_TRACE_RCVD_CHARS(ptr,len,port) 
#define SIORS_TRACE_CHAR_COUNT_RCVD(id,port) 
#define SIORS_TRACE_CHAR_COUNT_SENT(id,port) 
#define SIORS_TRACE_TXED_CHARS(ptr,len,port)
#define SIORS_TRACE_TX_CHAR_COUNT(id,port)
#endif /* Extended trace functions */

#else

#define SIORS_TRACE_CODE(id, port)            
#define SIORS_TRACE_RCVD_CHARS(ptr,len,port) 
#define SIORS_TRACE_CHAR_COUNT_RCVD(id,port) 
#define SIORS_TRACE_CHAR_COUNT_SENT(id,port) 
#define SIORS_TRACE_TXED_CHARS(ptr,len,port)
#define SIORS_TRACE_TX_CHAR_COUNT(id,port)

#endif

#endif /* Include flag */
