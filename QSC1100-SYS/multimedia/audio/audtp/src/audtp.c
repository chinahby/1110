/** 
\file **************************************************************************
*
*                                       A T P    P R O T O C O L    L A Y E R
*
*DESCRIPTION
* This file contains implementation of ATP protocol layer
*
*INITIALIZATION REQUIREMENTS:
*atp_diag_init() must be called to initialise ATP protocol layer.
*atp_diag_init() must be called ~AFTER~ initialization of SOUND TASK
*
*  This file is common to both protocol component on PC and protocol module on AMSS.
*  PC only/phone only code is wrapped with in #ifdef WINPC
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
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audtp/QSC1100/main/latest/src/audtp.c#3 $ $DateTime: 2008/10/22 01:43:49 $ $Author: markande $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
********************************************************************************
*/
/*
      --------------------
      |include files                |
      --------------------
*/
#include "audtp.h"
#ifndef WINPC
  #include "acm_diag.h"
  #include "msg.h"
  #include <stdlib.h>  // needed for malloc() and free()
  #include <string.h>  // needed for memcpy, memset, and memmove
#endif

#if defined(FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL) || defined (WINPC)
/*
      --------------------
      |Constants                  |
      --------------------
*/
const uint8 atp_protocol_version = 1;

/*
      --------------------
      |Function definitions     |
      --------------------
*/
#ifndef WINPC
/**
* FUNCTION : atp_diag_init

* DESCRIPTION :
* Initializes the atp protocol layer.
* All atp protocol layer clients should be initialised from this method.
*
* DEPENDENCIES :None
*
* RETURN VALUE : None
*
* SIDE EFFECTS : None
 */
void atp_diag_init(void)
{
  /**
  Initialize ACM diag dispatcher
  */
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_diag_init start", 0,0,0); 
#endif
  #ifdef FEATURE_AVS_DYNAMIC_CALIBRATION
    acm_diag_init();
  #endif
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_diag_init end", 0,0,0); 
#endif
}/* atp_diag_init */
#endif

/**
* FUNCTION init_atp_header
*
* DESCRIPTION : Initializes ATP frame headr with default values
*
* DEPENDENCIES: atp_frame_ptr should be a valid pointer 
*
* RETURN VALUE: None
*
* SIDE EFFECTS: Sets the atp_frame to default values which is pointed to by atp_frame_ptr .
*/
void init_atp_header
(
  /*atp_frame, which requires header initialization*/
  atp_frame_struct *atp_frame_ptr 
)
{
  if ( atp_frame_ptr == NULL )
    return;
  /** Set protocol Version */
  atp_frame_ptr->header.version = atp_protocol_version;

  /** Set header length , which is of fixed size*/
  atp_frame_ptr->header.header_length = sizeof(atp_frame_header_struct);

/** Turn off all flags */
  atp_frame_ptr->header.flags = 0;
/** Reset frame number to zero */
  atp_frame_ptr->header.frame_number = 0;
/** Reset frame offset to zero */
  atp_frame_ptr->header.frame_offset = 0;
/** Reset frame length to zero */
  atp_frame_ptr->header.frame_length = 0;
/** Reset buffer length to zero */
  atp_frame_ptr->header.buffer_length = 0;
} /* init_atp_header */

/**
* FUNCTION free_atp_frame
*
* DESCRIPTION : Deallocates memory for atp frame data pointer and sets pointers to NULL.
* Then deallocates memory for this frame and sets pointer to NULL
*
* DEPENDENCIES: atp_frame_ptr should be a valid pointer 
*
* RETURN VALUE: None
*
* SIDE EFFECTS: None
*
*/
void free_atp_frame
(
  /*atp_frame pointer which needs to be deallocated*/
  atp_frame_struct* atp_frame_ptr
)
{
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG:free_atp_frame start  %s %d", __FILE__,__LINE__,0); 
#endif
	if(NULL==atp_frame_ptr)
	{
		return;
	}
  /** deallocate data part and set to NULL*/
  if ( atp_frame_ptr->data_ptr != NULL )
  {
    free(atp_frame_ptr->data_ptr);
    atp_frame_ptr->data_ptr = NULL;
  }
  /** free frame */
  if( atp_frame_ptr != NULL )
  {
    free(atp_frame_ptr);
    atp_frame_ptr = NULL;
  }
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG:free_atp_frame end  %s %d", __FILE__,__LINE__,0); 
#endif
}/* free_atp_frame */

/* <EJECT> */
/**
* FUNCTION    : break_buffer_into_frames
*
* DESCRIPTION : Breaks a given buffer into frames and sets their header info. 
*               Creates a doubly linked list of created frames.
*               Input buffer can be deallocated.
*
* DEPENDENCIES: None.
*
* RETURN VALUE: Pointer to the first node of the linked list
*
* SIDE EFFECTS: None.
 */
void break_buffer_into_frames
(
  /* pointer to the input buffer */
  byte* buffer_ptr,
  /* bufer length */
  uint32 buffer_length,
  /*linked list of frames start pointer*/
  atp_frame_dblnk_lst_struct** start_node_ptr
)
{
  
	
  /** variable to hold the starting point of the dbl lnk lst.*/
  atp_frame_dblnk_lst_struct *start_ptr = NULL;
  uint32 frame_byte_offset = 0;
  /** initialize frame count to zero */
  uint8 frame_count = 0;
  /** calculate max payload size */
  #ifdef WINPC
  uint16 atp_data_size = DIAG_REQ_PKT_SIZE - sizeof(atp_frame_header_struct);
  #else
  uint16 atp_data_size = DIAG_RES_PKT_SIZE - sizeof(atp_frame_header_struct);
  #endif
  uint16 frame_length = atp_data_size;
  /** Variable to hold the length of the buffer to be processed to break into 
  frames*/
  uint32 remaining_buffer_length = buffer_length;
/** pointer to hold the previous node in the linked list */
  atp_frame_dblnk_lst_struct *prev_node_ptr = NULL;
/** pointer to hold the current node in the linked list */
  atp_frame_dblnk_lst_struct *current_node_ptr = NULL;
/** Initialize the frame count to zero*/
  frame_count = 0;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: break_buffer_into_frames START", 0,0,0); 
#endif	
/**loop while we have some buffer to break*/
  while(remaining_buffer_length > 0 )
  {
    atp_frame_struct* atp_frame_ptr = NULL;
	MSG_MED("ACM_DIAG: break_buffer_into_frames: inside while loop", 0,0,0);	
    /** Create a atp_frame*/
    atp_frame_ptr = (atp_frame_struct *)malloc(sizeof(atp_frame_struct));
	if(NULL==atp_frame_ptr)
	{
		return;
	}
    /** Initialize the header*/
    init_atp_header(atp_frame_ptr);
    /** allocate memory for frame data */
    atp_frame_ptr->data_ptr = (byte *)malloc(atp_data_size);
	/*CLOCKWORK FIX*/
	if(atp_frame_ptr->data_ptr==NULL)
	{
		free(atp_frame_ptr);
		return;
	}
    /**Calculate the current frame size
    *if this is last frame then frame length can be smalled than max frame length
    */
    frame_length = (uint16) ((remaining_buffer_length >= atp_data_size) ? 
                                    atp_data_size : remaining_buffer_length );

    /** Set it data flag for this frame*/
    atp_frame_ptr->header.flags |= FLAG_DATAFRAME_MASK;
    /** If this is the last frame in the buffer, set LAST frame flag also*/
    if( frame_length == remaining_buffer_length )
    {
      //Set LAST frame FLAG
      atp_frame_ptr->header.flags |= FLAG_LASTFRAME_MASK;
    }
    /**Increment frame count*/
    frame_count++;
    /** Set header info for current frame*/
    atp_frame_ptr->header.buffer_length = buffer_length;
    atp_frame_ptr->header.frame_offset = frame_byte_offset;
    atp_frame_ptr->header.frame_number = frame_count;
    atp_frame_ptr->header.frame_length = frame_length;

    /** copy data into frame*/
    memcpy(atp_frame_ptr->data_ptr,buffer_ptr,frame_length);

    /** increment buffer pointer by the amount of data we copied*/
    buffer_ptr += frame_length;

    /** increment offset */
    frame_byte_offset +=frame_length;
    /**Calculate remaining buffer length*/
    remaining_buffer_length -=frame_length;
    current_node_ptr = (atp_frame_dblnk_lst_struct *)
    malloc(sizeof(atp_frame_dblnk_lst_struct));
    if(NULL==current_node_ptr)
	{
		/*CLOCKWORKS FIX*/
		free(atp_frame_ptr);
		free(atp_frame_ptr->data_ptr);
		return;
	}
	/**If this is first node in the linked list, save it as start_ptr*/
    if ( start_ptr == NULL )
    {
      start_ptr = current_node_ptr;
    }
    /**Initialize previous node pointer and next node pointer of current node*/
    current_node_ptr->previous_node_ptr = current_node_ptr->next_node_ptr = NULL;
    /**set the frame pointer of the current node to the atp frame we created just 
    *now 
    */
    current_node_ptr->frame_ptr = atp_frame_ptr;
    /**Set  the previous node pointer of the current node */
    current_node_ptr->previous_node_ptr = prev_node_ptr;
    /**set the next node pointer of the previous node */
    if ( prev_node_ptr != NULL )
    {
      prev_node_ptr->next_node_ptr = current_node_ptr;
    }
    /**Save the current node as previous node . Required to set for next node*/
    prev_node_ptr = current_node_ptr;
  }	
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: break_buffer_into_frames end", 0,0,0); 
#endif	
  *start_node_ptr = start_ptr;
} /* break_buffer_into_frames */

/**
* FUNCTION : initialize_buffer_context

* DESCRIPTION : Initializes buffer context.
* his should be called once before using the buffer for transmission
*
* DEPENDENCIES: None.
*
* RETURN VALUE: None
*
* SIDE EFFECTS: None.
*/
void initialize_buffer_context
(
/**pointer to the buffer context which requires initialization*/
  buffer_context_struct *buf_cntxt_ptr
)
{
  buf_cntxt_ptr->buffer_length = 0;
  buf_cntxt_ptr->buffer_ptr = NULL;
  buf_cntxt_ptr->is_buffer_allocated = FALSE;
  buf_cntxt_ptr->frame_count = 0 ;
} /* initialize_buffer_context */

/**
* FUNCTION  : copy_frame_to_buffer
* DESCRIPTION : copies a frame on to buffer.
* Takes the offset info from header to find the location to which frame data should copy.
*
* DEPENDENCIES: None.
*
* RETURN VALUE: boolean. TRUE indicates this frame is last frame in the buffer. FALSE indicates
* this is NOT last frame.
*
* SIDE EFFECTS: None.
*/
boolean copy_frame_to_buffer
(
  /* pointer to the address of buffer context */
  buffer_context_struct *buf_cntxt_ptr,
  /* pointer to the frame which needs to be copied */
  atp_frame_struct *frame_ptr
)
{
  byte *dest_loc_ptr = NULL;

#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: copy_frame_to_buffer start", 0,0,0); 
#endif	
  /**If buffer is not created yet, create and initialize */
  if ( buf_cntxt_ptr->is_buffer_allocated == FALSE )
  {
    buf_cntxt_ptr->buffer_ptr = (byte *) malloc(frame_ptr->header.buffer_length);
    memset(buf_cntxt_ptr->buffer_ptr, 0, frame_ptr->header.buffer_length);
    buf_cntxt_ptr->is_buffer_allocated = TRUE ;
    /** assign buffer length*/
    buf_cntxt_ptr->buffer_length = frame_ptr->header.buffer_length;
  }

  /** Calculate destination loaction where to copy the frame*/
  dest_loc_ptr = buf_cntxt_ptr->buffer_ptr + frame_ptr->header.frame_offset;
  /** copy frame on to buffer */
  memcpy( dest_loc_ptr, frame_ptr->data_ptr, frame_ptr->header.frame_length );
  /** Note the frame count */
  if( buf_cntxt_ptr->frame_count < frame_ptr->header.frame_number )
     buf_cntxt_ptr->frame_count = frame_ptr->header.frame_number;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: copy_frame_to_buffer end %s %d", __FILE__,__LINE__,0); 
#endif	
  /** return TRUE if this is last frame else FALSE*/
  if ( frame_ptr->header.flags & FLAG_LASTFRAME_MASK )
    return TRUE;
  else
    return FALSE;

} /* copy_frame_to_buffer */

/**
* FUNCTION    : free_linked_list

* DESCRIPTION : traverse the linked list and free all (frame data, 
                           frame and linked list node )the memory.
*
* DEPENDENCIES: None.
*
* RETURN VALUE: None
*
* SIDE EFFECTS: None.
 */
void free_linked_list
(
  /*start node of the linked list*/
  atp_frame_dblnk_lst_struct *start_ptr
)
{
/**declare and initalise all required pointer for traversing the linked list*/
  atp_frame_dblnk_lst_struct *current_node_ptr=NULL;
  atp_frame_dblnk_lst_struct *prev_node_ptr=NULL;
  atp_frame_struct *frame_ptr = NULL ;
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG:free_linked_list start  %s %d", __FILE__,__LINE__,0); 
#endif
  /** If there are no nodes just return*/
  if ( start_ptr == NULL )
    return;
  /**Initislise the current node to start node*/
  current_node_ptr = start_ptr;
  do
  {
    /** free the atp frame*/
     frame_ptr = current_node_ptr->frame_ptr;
     free_atp_frame(frame_ptr);
     /**Save the current node as previous node to free it later*/
     prev_node_ptr = current_node_ptr;
     /**Get the next node*/
     current_node_ptr = current_node_ptr->next_node_ptr;
     /**free the previous node*/
      if ( prev_node_ptr != NULL )
        free(prev_node_ptr);
   }
   while(current_node_ptr);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG:free_linked_list end  %s %d", __FILE__,__LINE__,0); 
#endif
} /* free_linked_list */

/**
* FUNCTION  : create_getnext_frame
*
* DESCRIPTION : creates a frame and set GetNextFrame flag
*
* DEPENDENCIES: None.
*
* RETURN VALUE: pointer to GNF
*
* SIDE EFFECTS: None.
*/
atp_frame_struct* create_getnext_frame()
{
  atp_frame_struct* atp_frame_ptr = NULL;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG:create_getnext_frame start  %s %d", __FILE__,__LINE__,0); 
#endif 
  /** Create atp_frame*/
  atp_frame_ptr = (atp_frame_struct *)malloc(sizeof(atp_frame_struct));
  if(NULL==atp_frame_ptr)
  {
	  return NULL;
  }
  /** initialize header*/
  init_atp_header(atp_frame_ptr);
/**Set GNF flag*/
  atp_frame_ptr->header.flags |= FLAG_GETNEXTFRAME_MASK;
  /**Set date pointer to NULl*/
  atp_frame_ptr->data_ptr = NULL;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG:create_getnext_frame end  %s %d", __FILE__,__LINE__,0); 
#endif 
  return atp_frame_ptr;	
} /* create_getnext_frame */

/**
* FUNCTION    : atp_create_diag_req_pkt
*
* DESCRIPTION : creates diag request packet and puts the given atp frame into payload
*
* DEPENDENCIES: None.
*
* RETURN VALUE: void
*
* SIDE EFFECTS: None.
 */
void atp_create_diag_req_pkt
(
   /* pointer to atp frame which needs to be put in as pay load*/
   atp_frame_struct* atp_frame_ptr,
   /* pointer to pointer of diag request pakect to be created*/
   diag_pkt_req_struct** atp_diag_pkt
)
{
  //atp frame header size
  uint8 headersize = sizeof(atp_frame_header_struct);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_diag_req_pkt start %s %d", __FILE__,__LINE__,0); 
#endif	
  /**create a diag packet*/
  *atp_diag_pkt = (diag_pkt_req_struct* )malloc(sizeof(diag_pkt_req_struct));
  if(NULL==(*atp_diag_pkt))
  {
	  return;
  }
/**initialize the diag packet memory to zero*/
  memset(*atp_diag_pkt,0,sizeof(diag_pkt_req_struct));
  /**Set diag packet header*/
  /**Set diag command code*/
  (*atp_diag_pkt)->header.cmdCode   = DIAG_EXTEND_CMDCODE;
  /**set susbsystem ID as audio subsystem*/
  (*atp_diag_pkt)->header.subsysId  = AUDIO_SUBSYS_ID;
  /**set the sub system command code */
  (*atp_diag_pkt)->header.subsysCmdCode = SUBSYS_CMDCODE;
  
  //copy atp frame header to payload
  memcpy((*atp_diag_pkt)->payload_req,&atp_frame_ptr->header,headersize);

  //copy frame data to payload
  memcpy((*atp_diag_pkt)->payload_req+headersize,atp_frame_ptr->data_ptr, 
                 atp_frame_ptr->header.frame_length);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_diag_req_pkt end %s %d", __FILE__,__LINE__,0); 
#endif	
  return;
}

/**
* FUNCTION    : atp_create_frame_from_diag_resp
*
* DESCRIPTION : extracts atp frame from diag resp packet
*
* DEPENDENCIES: None.
*
* RETURN VALUE: void
*
* SIDE EFFECTS: None.
*/
void atp_create_frame_from_diag_resp
(
/**pointer to diag response packet from which to extract atp fraem*/
   diag_pkt_resp_struct* atp_diag_pkt,
/**pointer to pinter where to extrat the atp frame*/
   atp_frame_struct** atp_frame_ptr
)
{
  uint8 headersize = sizeof(atp_frame_header_struct);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_frame_from_diag_resp start %s %d", __FILE__,__LINE__,0); 
#endif	
  /**create atp frame*/
  *atp_frame_ptr = (atp_frame_struct* )malloc(sizeof(atp_frame_struct));
  if(NULL==*atp_frame_ptr)
  {
	  return;
  }
  /**copy header to atp_frame*/
  memcpy(&(*atp_frame_ptr)->header,atp_diag_pkt->payload_resp,headersize);
  /**allocate required memory to hold data*/
  (*atp_frame_ptr)->data_ptr = (byte *) malloc((*atp_frame_ptr)->header.frame_length);

  /**copy frame data*/
  memcpy((*atp_frame_ptr)->data_ptr, atp_diag_pkt->payload_resp+headersize, 
                    (*atp_frame_ptr)->header.frame_length);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_frame_from_diag_resp end %s %d", __FILE__,__LINE__,0); 
#endif
  return;
}

/**
* FUNCTION    : atp_create_frame_from_diag_req
*
* DESCRIPTION : creates atp frame from diag request packet
*
* DEPENDENCIES: None.
*
* RETURN VALUE: void
*
* SIDE EFFECTS: None.
 */
void atp_create_frame_from_diag_req
(
/**pointer to diag request packet from which to extract atp fraem*/
   diag_pkt_req_struct* atp_diag_pkt,
   /**pointer to pinter where to extrat the atp frame*/
   atp_frame_struct** atp_frame_ptr
)
{
  uint8 headersize = sizeof(atp_frame_header_struct);

#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_frame_from_diag_req start %s %d", __FILE__,__LINE__,0); 
#endif
  /**create atp frame*/
  *atp_frame_ptr = (atp_frame_struct* )malloc(sizeof(atp_frame_struct));
  if(NULL==(*atp_frame_ptr))
  {
	  return;
  }
  /**copy header part to atp_frame*/
  memcpy(&(*atp_frame_ptr)->header,atp_diag_pkt->payload_req,headersize);
  (*atp_frame_ptr)->data_ptr = NULL ;
  if ( (*atp_frame_ptr)->header.frame_length > 0 )
  {
  /**allocate required memory to hold data*/
  (*atp_frame_ptr)->data_ptr = (byte *) malloc((*atp_frame_ptr)->header.frame_length);
  if(NULL==((*atp_frame_ptr)->data_ptr))
  {
	  return;
  }
  /**copy frame data*/
    memcpy((*atp_frame_ptr)->data_ptr, atp_diag_pkt->payload_req+headersize, 
                    (*atp_frame_ptr)->header.frame_length);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_create_frame_from_diag_req end %s %d", __FILE__,__LINE__,0); 
#endif
	}   
	return;
}

/**
* FUNCTION    : create_atp_frame
*
* DESCRIPTION : creates atp frame and initializes
*
* DEPENDENCIES: None.
*
* RETURN VALUE: void
*
* SIDE EFFECTS: None.
*/
void create_atp_frame
(
  atp_frame_struct** atp_frame_ptr
)
{
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: create_atp_frame start %s %d", __FILE__,__LINE__,0); 
#endif
  /**create atp frame*/
  *atp_frame_ptr = (atp_frame_struct* )malloc(sizeof(atp_frame_struct));
  if(NULL==(*atp_frame_ptr))
  {
	  return;
  }
  /**initialize its header*/
  init_atp_header(*atp_frame_ptr);
  /**Set data pointer to null*/
  (*atp_frame_ptr)->data_ptr = NULL;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: create_atp_frame end %s %d", __FILE__,__LINE__,0); 
#endif
  return;
  
}/*create_atp_frame*/

#ifdef WINPC

/**
* FUNCTION    : atp_perform_handshake
*
* DESCRIPTION : Performs hand shake with RX.
* Should be called each time before sending a buffer from PC.
*
* DEPENDENCIES: None.
*
* RETURN VALUE: void
*
* SIDE EFFECTS: None.
*/
boolean atp_perform_handshake
(
/**pointer to the atp session*/
  atp_pc_context_struct *context_ptr
 )
{
  //Cariable to hold the diag request packet, which is wraper of hand shake frame
  diag_pkt_req_struct* atp_diag_req_pkt = NULL;
  //Received diag response in response to hand shake
  diag_pkt_resp_struct atp_diag_resp_pkt;
  //pointer to hand shake frame
  atp_frame_struct *atp_hsk_frame_ptr = NULL;
  //pointer to response frame for hand shake frame
  atp_frame_struct *atp_resp_frame_ptr=NULL;
  //flag to indicate if packet is sent successfully
  boolean is_pkt_sent = TRUE;
  //flag to indicate is hand shake is success
  boolean is_hsk_success = TRUE;
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_perform_handshake start  %s %d", __FILE__,__LINE__,0); 
#endif
  /**Create a ATP frame*/
  create_atp_frame(&atp_hsk_frame_ptr);
  /**Set hand shake frame*/
  atp_hsk_frame_ptr->header.flags |= FLAG_HANDSHAKE_MASK;
  /**Create a diag request paket*/
  atp_create_diag_req_pkt(atp_hsk_frame_ptr, &atp_diag_req_pkt);
/**free the hand shake frame, as we have copied it to diag request packet*/
  free_atp_frame(atp_hsk_frame_ptr);
  if( atp_diag_req_pkt != NULL )
  {
    /**Send the diag request packet*/
    is_pkt_sent = context_ptr->send_atp_frame_ptr(atp_diag_req_pkt,&atp_diag_resp_pkt);
    free(atp_diag_req_pkt);
  }
  
  if ( FALSE == is_pkt_sent )
  {
    /**if failed to send the packet, return error */
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: failed to send the packet  %s %d", __FILE__,__LINE__,0); 
#endif
    return FALSE;
  }
  else
  {
    /**Extract atp frame from diag response packet*/
    atp_create_frame_from_diag_resp(&atp_diag_resp_pkt,&atp_resp_frame_ptr);
    /**if RX acknowleded the hand shake it send a frame GetNextFrame flag set*/
    if( (atp_resp_frame_ptr->header.flags & FLAG_GETNEXTFRAME_MASK) == 0 )
      is_hsk_success = FALSE;
    else
    is_hsk_success = TRUE;
    
    free_atp_frame(atp_resp_frame_ptr);
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_perform_handshake success to send the packet  %s %d", __FILE__,__LINE__,0); 
#endif
    return is_hsk_success;
  }
}

/**
* FUNCTION : atp_send_buffer

* DESCRIPTION :
* Send a request buffer to target.
* If buffer sent successfully, returns the response through the call back 
*function: receive_resp_buffer_ptr of context_ptr.
*
* DEPENDENCIES : send_atp_frame_ptr memeber in context_ptr must be pointing to 
*                           appropriate function.

* RETURN VALUE : If sent successfully returns TRUE or FALSE

* SIDE EFFECTS : Frees buffer_ptr and resets the pointer to NULL.
 */
boolean atp_send_buffer
( 
  byte* buffer_ptr /**< pointer to the input buffer */,
  uint32 buffer_length /**< bufer length */,
  atp_pc_context_struct *atp_pc_context_ptr/**< ponter to ATP session(on PC side) */
 )
{
  diag_pkt_req_struct* atp_current_diag_req_pkt_ptr = NULL;
  diag_pkt_resp_struct atp_diag_resp_pkt;
  boolean is_pkt_sent = TRUE;
  boolean is_last_frame_received = FALSE;
  atp_frame_struct *atp_resp_frame_ptr=NULL;
  atp_frame_struct *atp_gnf_frame_ptr=NULL;
  /** 
  Initialize response buffer context
  */
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_send_buffer start  %s %d", __FILE__,__LINE__,0); 
#endif
  initialize_buffer_context(&atp_pc_context_ptr->resp_buf_cntxt);
 /**
  *Break the request buffer into atp frames and create a linked list.
  *Get the start node of the created linked list.
  */
  break_buffer_into_frames(buffer_ptr, buffer_length,
                                               &atp_pc_context_ptr->start_node_ptr);
  /** Free the request buffer */
  if(buffer_ptr)
  {
    free(buffer_ptr);
    buffer_ptr = NULL;
  }
/** We are going to start sending the frames from linked list.
*So assign the start node as current node
*/
  atp_pc_context_ptr->current_node_ptr = atp_pc_context_ptr->start_node_ptr;
/** If there are no nodes in linked list just return FALSE (error)*/
  if ( atp_pc_context_ptr->current_node_ptr == NULL )
  {
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
	  return FALSE;
  }
/** Perform hand shake with RX to inform that we are going to start 
transmission 
of fresh request buffer*/
  if ( FALSE == atp_perform_handshake(atp_pc_context_ptr) )
  {
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
	  return FALSE;
  }
/** Wrap the first atp_frame into  a diag packet */
atp_create_diag_req_pkt(atp_pc_context_ptr->current_node_ptr->frame_ptr,
                                          &atp_current_diag_req_pkt_ptr);
  /** Start traversing the linked list to send all atp_frames one by one*/
  while( 1 )
  {
    /** Send atp frame through the call back function of application layer*/
    /** If the packet is send successfully, we receive response atp frame 
    wrapped in diag packet*/
    is_pkt_sent = atp_pc_context_ptr->send_atp_frame_ptr(atp_current_diag_req_pkt_ptr,
                                                                              
&atp_diag_resp_pkt);
    /** free the diag packet*/
    free(atp_current_diag_req_pkt_ptr);
    /** Check the return code of send_atp_frame_ptr(). 
    *If failed to send the packet, return FALSE (error)
    */
    if ( FALSE == is_pkt_sent )
	{
#ifdef AUDTP_DEGUB_PRINT 
		MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
      return FALSE;
	}
    /** No error? unwrap the diag packet to get response atp frame*/
    atp_create_frame_from_diag_resp(&atp_diag_resp_pkt,&atp_resp_frame_ptr);
    /** If the current frame is not the last frame... */
    if( atp_pc_context_ptr->current_node_ptr->next_node_ptr != NULL )
    {
      /** Expect a atp frame with GNF(Gent Next Frame) flag set*/
      if( !(atp_resp_frame_ptr->header.flags & FLAG_GETNEXTFRAME_MASK) )
      {
        /**.If we havn't received a GNF frame, its an error return FALSE 
        (erro)*/
        free_atp_frame(atp_resp_frame_ptr);
#ifdef AUDTP_DEGUB_PRINT 
		MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
        return FALSE;
      }
      /** If we received a frame with GNF flag set, send next frame and
      *increment the current node pointer to next node in the lined list
      */
      atp_pc_context_ptr->current_node_ptr = 
                    atp_pc_context_ptr->current_node_ptr->next_node_ptr;
      /** wrap the next atp frame into diag packet*/
      atp_create_diag_req_pkt(atp_pc_context_ptr->current_node_ptr->frame_ptr, 
                                                &atp_current_diag_req_pkt_ptr);
    }
    else
    {
      /** If we have sent all frames from request buffer, expect a frame from 
      response buffer. i.e a frame with Data flag set.*/
      if( !(atp_resp_frame_ptr->header.flags & FLAG_DATAFRAME_MASK) )
      {
        /** If we hav not received a Data frame, return FALSE (error) */
        free_atp_frame(atp_resp_frame_ptr);.
#ifdef AUDTP_DEGUB_PRINT 
			MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
        return FALSE;
      }
      /** If yes, copy frame to response buffer */
      is_last_frame_received = copy_frame_to_buffer(
                  &atp_pc_context_ptr->resp_buf_cntxt, 
                  atp_resp_frame_ptr);
      free_atp_frame(atp_resp_frame_ptr);
      /** If we received a data frame.with LAST flag set, give the response 
      buffer to application layer*/
        if( is_last_frame_received == TRUE )
        {
          atp_pc_context_ptr->receive_resp_buffer_ptr(
                      atp_pc_context_ptr->resp_buf_cntxt.buffer_ptr,
                      atp_pc_context_ptr->resp_buf_cntxt.buffer_length);
          /** clear the request buffer and reinitialize the response buffer 
context*/
          free(atp_pc_context_ptr->resp_buf_cntxt.buffer_ptr);
          initialize_buffer_context(&atp_pc_context_ptr->resp_buf_cntxt);
#ifdef AUDTP_DEGUB_PRINT 
			MSG_MED("ACM_DIAG: atp_send_buffer returning true  %s %d", __FILE__,__LINE__,0); 
#endif
          return TRUE;
        }
        else
        {
          /** Else  request next frame from RX */
          /** create GNF frame */
          create_atp_frame(&atp_gnf_frame_ptr);
          /** Set GNF flag*/
          atp_gnf_frame_ptr->header.flags |= FLAG_GETNEXTFRAME_MASK;
          /** Prepare the diag packet with the next frame we need to send*/
          atp_create_diag_req_pkt(atp_gnf_frame_ptr, 
                                                      &atp_current_diag_req_pkt_ptr);
          /** free the atp frame*/
          free_atp_frame( atp_gnf_frame_ptr );
        }
      }
  }/*while*/  
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_send_buffer returning FALSE  %s %d", __FILE__,__LINE__,0); 
#endif
  return FALSE;
}/*atp_send_buffer*/

#else
/**
* FUNCTION : atp_receive_atp_frame

* DESCRIPTION :
*This is the main part of RX algorithm
* Receives an atp frame and copies on to buffer.
*If all frame are received calls the callback function of the application layer 
*to give the request buffer and to recive the response buffer. Then sends the 
*response buffer to TX.
*
* DEPENDENCIES : receive_req_buffer_ptr memeber in context_ptr must be pointing to 
* appropriate function, which can process the request buffer and can provide response buffer

* RETURN VALUE : If sent successfully returns TRUE or FALSE

* SIDE EFFECTS : None
 */
boolean atp_receive_atp_frame
(
/**pointer to the atp session*/
  atp_phone_context_struct *context_ptr,
  /**pointer to the atp frame. which needs to be processed in the give atp 
  context*/
  atp_frame_struct *atp_frame_ptr
)
{
  /*flag to indicate whether the received frame is last frame in the request 
      buffer*/
  boolean is_last_frame = FALSE;
  /*pointer to the response buffer*/
  byte* resp_buf_ptr = NULL ; 
  /*variable to hold response buffer length*/
  uint32 resp_buf_length=0; 
  /*pointer to the response frame created inresponse to the atp_frame-ptr*/
  atp_frame_struct *atp_resp_frame_ptr=NULL;
  uint8 atp_frame_header_length = sizeof(atp_frame_header_struct);
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame start  %s %d", __FILE__,__LINE__,0); 
#endif
  /**We received a handshake frame, so drop all the frames from previous session
    *and get ready for new session.
  */
  if( (atp_frame_ptr->header.flags & FLAG_HANDSHAKE_MASK) != 0 )
  {
    /** we are expecting a frame with GNF flag */
    if ( context_ptr->is_next_frame_gnf == TRUE )
    {
      context_ptr->is_next_frame_gnf = FALSE;
      /**we have some response frames left in previous session free them*/
      free_linked_list(context_ptr->resp_start_node_ptr);
    }
    else
    {
      /**We were receiving frames from previous session request buffer, so free 
      *the request buffer*/
      if ( context_ptr->req_buf_cntxt.buffer_ptr != NULL )
      free(context_ptr->req_buf_cntxt.buffer_ptr);
      /**initialise the response buffer context*/
      initialize_buffer_context(&context_ptr->req_buf_cntxt);
    }/*hand shake if*/
    /** inform TX that we are ready to receive new request buffer */
    /**create GNF frame and it is our response frame for hand shake frame*/
    create_atp_frame(&context_ptr->atp_current_resp_frame_ptr);
    /**Set GNF flag*/
    context_ptr->atp_current_resp_frame_ptr->header.flags |=FLAG_GETNEXTFRAME_MASK ;
#ifdef AUDTP_DEGUB_PRINT 
		MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   TRUE %s %d", __FILE__,__LINE__,0); 
#endif
    return TRUE;
  }   
  /**If we have not received a hand shake frame.
    * we have left some frames in response buffer so we are expecting a GNF frame
    */
  if ( context_ptr->is_next_frame_gnf == TRUE )
  {
    /**If  we have not received a GNF frame so its an error  because still we have 
      *some frame is response buffer*/
    if( (atp_frame_ptr->header.flags & FLAG_GETNEXTFRAME_MASK) == 0 )
    {
#ifdef AUDTP_DEGUB_PRINT 
	MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   FALSE %s %d", __FILE__,__LINE__,0); 
#endif
		return FALSE;
    }
    /** move to next node pointer to send 
    *If we don't have frames means some unexpected error*/
    if( context_ptr->resp_current_node_ptr == NULL )
    {
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   FALSE %s %d", __FILE__,__LINE__,0); 
#endif
		return FALSE;
    }
    context_ptr->resp_current_node_ptr = context_ptr->resp_current_node_ptr->next_node_ptr;
    /** if this is the last frame in response, note that we are not expecting a GNF 
    *frame 
    */
    if ( context_ptr->resp_current_node_ptr->next_node_ptr == NULL )
    {
      context_ptr->is_next_frame_gnf = FALSE;
      /**create response frame*/
      create_atp_frame(&atp_resp_frame_ptr);
      /**copy header info to response frame*/
       memcpy(atp_resp_frame_ptr,
                    &context_ptr->resp_current_node_ptr->frame_ptr->header,
                    atp_frame_header_length);
      /**create memory for data */
     atp_resp_frame_ptr->data_ptr = 
                  (byte *)malloc(atp_resp_frame_ptr->header.frame_length);
      /**copy data to response frame*/
     memcpy(atp_resp_frame_ptr->data_ptr,
                    context_ptr->resp_current_node_ptr->frame_ptr->data_ptr,
                    atp_resp_frame_ptr->header.frame_length
                    );
      /** set response frame pointer as current node frame pointer*/
      context_ptr->atp_current_resp_frame_ptr= atp_resp_frame_ptr;
      //Since this is the last frame we can free the response buffer linked list
      free_linked_list(context_ptr->resp_start_node_ptr);
    }
    else
    {
      /**current node is not the last node in response buffer linked list so 
        *next frame should be GNF frame
      */
      context_ptr->is_next_frame_gnf = TRUE;
      /** set response frame pointer as current node frame pointer*/
      context_ptr->atp_current_resp_frame_ptr= context_ptr->resp_current_node_ptr->frame_ptr;
    }
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   TRUE %s %d", __FILE__,__LINE__,0); 
#endif
    return TRUE;
  }
  else
  {
    /**we are expecting a data frame
    *we have not received a data frame so its an error*/
    if( (atp_frame_ptr->header.flags & FLAG_DATAFRAME_MASK) == 0 )
	{
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   FALSE %s %d", __FILE__,__LINE__,0); 
#endif
		return FALSE;
	}
    /**copy frame to our buffer*/
    is_last_frame = copy_frame_to_buffer(&context_ptr->req_buf_cntxt,atp_frame_ptr);        

    if ( is_last_frame )
    {
      /**we received last frame*/
      /**send request buffer to ACM*/
      if ( context_ptr->receive_req_buffer_ptr == NULL )
	  {
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   FALSE %s %d", __FILE__,__LINE__,0); 
#endif
		  return FALSE;
	  }
      context_ptr->receive_req_buffer_ptr(context_ptr->req_buf_cntxt.buffer_ptr,
      context_ptr->req_buf_cntxt.buffer_length, &resp_buf_ptr,&resp_buf_length);
      /**free the request buffer*/
      free(context_ptr->req_buf_cntxt.buffer_ptr);
      initialize_buffer_context(&context_ptr->req_buf_cntxt);
      /**break the response buffer into frames*/
      break_buffer_into_frames(resp_buf_ptr,resp_buf_length,
      &context_ptr->resp_start_node_ptr);
      /**free the response buffer*/
      free(resp_buf_ptr);
      /**set current node pointer to start pointer*/
      context_ptr->resp_current_node_ptr = context_ptr->resp_start_node_ptr;
      if ( context_ptr->resp_current_node_ptr->next_node_ptr == NULL )
        context_ptr->is_next_frame_gnf = FALSE;
      else
        context_ptr->is_next_frame_gnf = TRUE;
      /**set response frame pointer as current node frame pointer*/
      context_ptr->atp_current_resp_frame_ptr = context_ptr->resp_current_node_ptr->frame_ptr;
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   TRUE %s %d", __FILE__,__LINE__,0); 
#endif
      return TRUE;
    }
    else
    {/**this is not last frame so request next frame*/
      /**create GNF frame and it is our response frame*/
      create_atp_frame(&context_ptr->atp_current_resp_frame_ptr);
      context_ptr->atp_current_resp_frame_ptr->header.flags |=FLAG_GETNEXTFRAME_MASK ;
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   TRUE %s %d", __FILE__,__LINE__,0); 
#endif
      return TRUE;
    }
  }/*else if( context_ptr->is_next_frame_gnf == TRUE )*/
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_receive_atp_frame returning   FALSE %s %d", __FILE__,__LINE__,0); 
#endif
  return FALSE;
}/*atp_receive_atp_frame*/
#endif

/**
* FUNCTION : atp_create_diag_resp_pkt
* 
* DESCRIPTION : creates a diag resp packet from atp frame
* 
* DEPENDENCIES: None.
* 
* RETURN VALUE: None
* 
* SIDE EFFECTS: None.
*/
void atp_create_diag_resp_pkt
(
/** frame to be wrapped in response diag packet */
  atp_frame_struct* atp_frame_ptr,
/** created diag response packet */
  diag_pkt_resp_struct** atp_diag_pkt
)
{
  uint8 headersize = sizeof(atp_frame_header_struct);
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_create_diag_resp_pkt START %s %d", __FILE__,__LINE__,0); 
#endif
  /** Create diag response packet*/
  (*atp_diag_pkt) = (diag_pkt_resp_struct* )malloc(sizeof(diag_pkt_resp_struct));
  if(NULL==(*atp_diag_pkt))
  {
	  return;
  }
  /**Initialise the memeory*/
  memset(*atp_diag_pkt,0,sizeof(diag_pkt_resp_struct));
  /**set diag response packet header info*/
  (*atp_diag_pkt)->header.cmdCode   = DIAG_EXTEND_CMDCODE;
  (*atp_diag_pkt)->header.subsysId  = AUDIO_SUBSYS_ID;
  (*atp_diag_pkt)->header.subsysCmdCode = SUBSYS_CMDCODE;
  
  /** copy atp header info*/
  memcpy(&(*atp_diag_pkt)->payload_resp[0],&atp_frame_ptr->header,headersize);
  /**copy frame data*/
  memcpy(&(*atp_diag_pkt)->payload_resp[headersize],atp_frame_ptr->data_ptr, atp_frame_ptr->header.frame_length);
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: atp_create_diag_resp_pkt END %s %d", __FILE__,__LINE__,0); 
#endif
  return;
}
#ifndef WINPC
/**
* FUNCTION : atp_receive_diag_pkt
*
* DESCRIPTION :
* Receive a request diag packet from acm diag dispatcher (avsACMdiag_cmd())
*.and return a response diag packet.
*
* DEPENDENCIES : receive_req_buffer_ptr memeber in atp_phone_context_ptr must be 
*pointing to.appropriate function, which can process the request buffer and 
*provide a response buffer.
*
* RETURN VALUE : If sent successfully returns TRUE or FALSE
*
* SIDE EFFECTS : Frees buffer_ptr and resets the pointer to NULL.
 */
void atp_receive_diag_pkt
(
/** pointer to the diag packet which contains the atp frame from request buffer*/
  diag_pkt_req_struct *atp_diag_req_pkt_ptr,
/** pointer to the diag packet which contains the atp frame from response buffer*/
  diag_pkt_resp_struct **atp_diag_rsp_pkt_ptr,
/** pointer to the phone context to whome request to be sent and resposne 
received*/
  atp_phone_context_struct *atp_phone_context_ptr
)
{
//A temporary variable to hold atp frame extracted from request diag packet
  atp_frame_struct* atp_frame_ptr;
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG:atp_receive_diag_pkt:START  %s %d", __FILE__,__LINE__,0); 
#endif
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG:atp_receive_diag_pkt:Create a atp frame from diag request packet  %s %d", __FILE__,__LINE__,0); 
#endif
/** Create a atp frame from diag request packet */
  atp_create_frame_from_diag_req(atp_diag_req_pkt_ptr, &atp_frame_ptr);
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG:atp_receive_diag_pkt:Create a atp frame from diag request packet is done %s %d", __FILE__,__LINE__,0); 
#endif
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG: Giving ATP frame to rx layer", 0,0,0); 
#endif
/** Send extracted atp frame to RX algorithm, which accumulates all the 
request frames to construct a request buffer.*/
  atp_receive_atp_frame(atp_phone_context_ptr, atp_frame_ptr);
/** free the extracted atp frame*/
  free_atp_frame(atp_frame_ptr);
/** if response atp frame is ready...*/
  if ( atp_phone_context_ptr->atp_current_resp_frame_ptr != NULL )
  {
  /** create a diag packet which wraps the response frame*/
    atp_create_diag_resp_pkt
    (
      atp_phone_context_ptr->atp_current_resp_frame_ptr,
      atp_diag_rsp_pkt_ptr
    );
/**free the response frame*/
    free_atp_frame(atp_phone_context_ptr->atp_current_resp_frame_ptr);
  }
  else
  {
/**else return NULL as response diag packet*/
    *atp_diag_rsp_pkt_ptr = NULL;
  }
#ifdef AUDTP_DEGUB_PRINT 
 MSG_MED("ACM_DIAG:atp_receive_diag_pkt:END  %s %d", __FILE__,__LINE__,0); 
#endif
}
#endif

#endif /*FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL || WINPC*/