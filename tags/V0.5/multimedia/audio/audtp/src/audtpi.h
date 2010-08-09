#ifndef AUDTPI_H
#define AUDTPI_H

/** 
\file ***************************************************************************
*
*                                   A T P I    H E A D E R    F I L E
*
*DESCRIPTION
* This header file contains all the definitions that are internal to ATP protocol module
* This file is common to both PC and phone.
* PC only/phone only code is wrapped with in #ifdef WINPC
*
*Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*******************************************************************************
*/
/* <EJECT> */
/**
\file ***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audtp/QSC1100/main/latest/src/audtpi.h#1 $ $DateTime: 2008/06/30 02:34:23 $ $Author: prashant $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
*
*******************************************************************************
*/

/*
      --------------------
      |include files                |
      --------------------
 */
#ifdef WINPC
  #define _WIN32_WINNT 0x0500
  #include <windows.h>
  #include <stdio.h>
#endif

/** Definitions for byte, word, etc. */
#include "comdef.h" 


/*
      --------------------
      | Macros and type defs |
      --------------------
*/


/** Bit mask to identify ATP frame type */
#define FLAG_HANDSHAKE_MASK 0x1 /**< Bit mask to identify Handshake frame */
#define FLAG_LASTFRAME_MASK 0x2 /**< Bit mask to identify Last frame in the buffer */
#define FLAG_GETNEXTFRAME_MASK 0x4 /**< Bit mask to identify Get Next Frame  */
#define FLAG_DATAFRAME_MASK 0x8 /**< Bit mask to identify that it is a data frame */

#define DIAG_EXTEND_CMDCODE 75 /**< Diag command code for extended mode*/
#define AUDIO_SUBSYS_ID     17	 /**< Audio subsystem ID */
#define SUBSYS_CMDCODE      2000  /**< Command code for ACM with in audio subsystem */

/**
* Structure definition for ATP frame Header
*/
typedef struct atp_frame_header_tag
{
  uint8  version; /**< version of the protocol */
  uint8  header_length;	/**< ATP frame header length(i.e this structure length)*/
  uint8  frame_number; /**< Identifies the number of the frame in a buffer*/
  /**
  * Bit flags to identify frame type
  * Bit0:HSK, Bit1:LAST, Bit2:GNF, Bit3:Data
  */
  uint8  flags;
  uint32 frame_offset;/**< offset position of the frame in buffer*/
  /** Length of the frame.
  *Should not exceed DIAG_REQ_PKT_SIZE/DIAG_RES_PKT_SIZE
  */
  uint16 frame_length;	
  uint32 buffer_length; /**< Total buffer size */
} atp_frame_header_struct;

/**
* Structure definition for ATP frame
*/
typedef struct atp_frame_tag
{
  atp_frame_header_struct header; /**< Frame header*/
  byte *data_ptr; /**< Frame data*/	
}atp_frame_struct;

/**
* Structure definition for doubly linked list node of type atp frame.
*/
typedef struct atp_frame_dblnk_lst_tag
{
  atp_frame_struct *frame_ptr;/**< pointer to hold atp frame*/
  struct atp_frame_dblnk_lst_tag *previous_node_ptr;/**< pointer to previous node */
  struct atp_frame_dblnk_lst_tag *next_node_ptr;/**< pointer to next node */
}atp_frame_dblnk_lst_struct;

/**
* Structure definition to hold a buffer context<br>
* This buffer context is used at RX side to accumulate atp frames of a buffer
*/
typedef struct buffer_context_tag
{
  boolean is_buffer_allocated;/**< Indicates if buffer is allocated*/
  uint8 frame_count;/**< Keeps track of number of frames in the buffer*/
  byte *buffer_ptr;/**< pointer to the buffer allocated*/
  uint32 buffer_length;/**< size of the buffer*/
}buffer_context_struct;

#endif
