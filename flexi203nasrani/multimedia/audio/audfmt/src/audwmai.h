#ifndef AUDWMAI_H
#define AUDWMAI_H
/*===========================================================================

    W I N D O W S   M E D I A   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all internal definitions necessary to interface
  RealAudio streams with AUDFMT Services.

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audwmai.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/05    pl     Added error_status to audwma_parse_ctl_type. 
02/14/05    hz     Initial Version check in code base.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_WMA
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                       RealAudio Stream Declarations

===========================================================================*/
/* State of the ping-pong buffers */
typedef enum {
  AUDWMA_DATA_STATE_LOADING,/* Both ping-pong buffers are loading             */
  AUDWMA_DATA_STATE_READY,  /* Data ready, other buffer might still be loading*/
  AUDWMA_DATA_STATE_FLUSH,  /* Flushing current data, and any pending Read ops*/
  AUDWMA_DATA_STATE_ERROR   /* Data error                                     */
} audwma_data_state_type;

/* This structure contains all the information on a particular file
** that is being parsed.
*/
/* Input data buffer */
typedef struct {
  uint8               *data;     /* Data buffer                      */
  uint32              start;     /* Position in file of buffer       */
  uint16              length;    /* Buffer data length               */
  boolean             loading;   /* TRUE == Buffer is being loaded   */
} audwma_buf_type;

/* This structure contains all the information on a particular file
** that is being parsed. Two ping-pong style buffers are used, so as to allow
** one buffer to be loaded, while the other is being parsed.
*/
typedef struct {
  snd_af_access_type      *handle;        /* Info to callback client          */

  audwma_buf_type         buf1;           /* ping-pong buffer 1               */
  audwma_buf_type         buf2;           /* ping-pong buffer 2               */

  audwma_buf_type         *curr_buf;      /* buffer being currently used      */
  uint32                  file_location;  /* Location in file being processed */
  uint32                  file_len;       /* File Length                      */
  uint32                  track_length;
  audwma_data_state_type  state;
  audmain_status_type     error_status;   
  snd_wma_format_spec_type spec;
} audwma_parse_ctl_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Function Prototypes
**---------------------------------------------------------------------------
*/

#endif /* FEATURE_WMA */
#endif /* AUDWMAI_H */
