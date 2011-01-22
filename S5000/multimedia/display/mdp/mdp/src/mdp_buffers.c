/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  MDP Frame Service

  GENERAL DESCRIPTION
  This file implements the MDP Frame Service.  Its main function is to 
  allocate dynamic display buffers. 

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/src/mdp_buffers.c#3 $ 
  $DateTime: 2008/11/06 22:45:00 $ 
  $Author: gauravs $

    when      who        what, where, why
  --------  ---------  ----------------------------------------------------------
  11/07/08  gs         Remove frame dimension macros to be shared in header
  10/07/08  rk         RVCT compiler Warning Fix. 
  07/11/07  sbernard   Creation

============================================================================*/

#include <stdlib.h>
#include "mdp_buffers.h"
#include "err.h"

/*=============================================================================
                           EXTERNAL DEPENDENCIES
=============================================================================*/

// Defined in mdp_drv.c
extern disp_info_type _primary_lcd;
extern disp_info_type _secondary_lcd;
extern disp_info_type _biggest_lcd;

/*=============================================================================
                           PRIVATE DATA TYPES
=============================================================================*/

/* Memory management */
#define MDP_FRAME_MALLOC(size) malloc(size)
#define MDP_FRAME_FREE(ptr) free(ptr)

/* Buffer overflow-check marker tag */
#define MDP_MALLOC_MARKER 0x55

/* Frame node type */
typedef struct frame_node_struct 
{
  q_link_type qLink;    /* MUST be the first item (required by queue.c) */
  mdp_frame_type frame; /* Client frame buffer                          */
  uint8* mallocBuffer;  /* Private frame service data                   */
  uint8* pMarker;
  uint16 markerByteSize;
} frame_node_type;

/* Frame service type */
typedef struct frame_list_struct 
{
  q_type list;
  uint32 totalMemUsage; /* Frame Service data */
} frame_list_type;

/*=============================================================================
                             PRIVATE DATA
=============================================================================*/

/* frame service queue/list */
static frame_list_type qListOfDynamicFrames;

/*=============================================================================
                          PRIVATE PROTOTYPES
=============================================================================*/

static boolean mdp_init_frame_service(void);

static mdp_frame_type* mdp_create_new_frame(uint16 pixelWidth, uint16 pixelHeight, uint16 bpp);

/*=============================================================================
                           API IMPLEMENTATION
=============================================================================*/

/*===========================================================================

FUNCTION mdp_free_frame()

DESCRIPTION
  This function frees/deletes a previously allocated frame.

DEPENDENCIES
  malloc service should have been initialized (call to mdp_init_frame_service)
  
  requires the global queue pointer qListOfDynamicFrames

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mdp_free_frame(mdp_frame_type* pUserFrame)
{
  /* retrieve ptr to frame-service node */
  frame_node_type* pFrameNode = (frame_node_type*)pUserFrame->pParent;

  /* update Frame Service info */
  qListOfDynamicFrames.totalMemUsage -= pUserFrame->byteSize + 
                                        pFrameNode->markerByteSize +
                                        sizeof(frame_node_type);

  /* release heap buffer */
  MDP_FRAME_FREE(pUserFrame->buffer);

  /* remove frame-service node from Frame Service list */
  q_delete(&(qListOfDynamicFrames.list), &(pFrameNode->qLink));

  /* release heap frame */
  MDP_FRAME_FREE(pFrameNode);
}


/*===========================================================================

FUNCTION mdp_check_frames()

DESCRIPTION
  This function checks the dynamic screen buffers for overflow.  It can be
  called to check all buffers at once, or only one buffer for each call in 
  a round-robbin fashion.

DEPENDENCIES
  malloc service should have been initialized (call to mdp_init_frame_service)
  
  requires the global queue pointer qScrBuffers

RETURN VALUE
  boolean => TRUE if no buffer corruption detected, FALSE otherwise

SIDE EFFECTS
  Because this function manages its own round-robin iterator, this is NOT
  a reentrant function and should only be called from the same task.

===========================================================================*/
boolean mdp_check_frames(MDP_BUFFER_TEST_OPTION opt)
{
  frame_node_type* curNode;

  /* get first registered frame */
  curNode = (frame_node_type*)(q_check(&(qListOfDynamicFrames.list)));

  /* traverse the list... */
  while( curNode ) 
  {
    /* check frame for overflow */
    if(mdp_check_frame(&(curNode->frame)) == FALSE) 
    {
      /* OVERFLOW DETECTED */
      return FALSE;
    }

    /* get next registered frame */
    curNode = (frame_node_type*)(q_next(&(qListOfDynamicFrames.list), &(curNode->qLink)));
  }

  return TRUE;
}


/*===========================================================================

FUNCTION mdp_check_frame()

DESCRIPTION
  This function checks the passed-in frame buffer for overflow.

DEPENDENCIES
  frame service should have been initialized (call to mdp_init_frame_service)
  
  requires the global queue pointer qScrBuffers

RETURN VALUE
  boolean => TRUE if no buffer corruption detected, FALSE otherwise

SIDE EFFECTS
  Because this function manages its own round-robin iterator, this is NOT
  a reentrant function and should only be called from the same task.

===========================================================================*/
boolean mdp_check_frame( mdp_frame_type* pUserFrame )
{
  /* cursor-check position */
  uint16 pos;
  frame_node_type* pFrameNode;
  uint8*           marker;

  /* retrieve ptr to frame-service node */
  pFrameNode = (frame_node_type*)pUserFrame->pParent;

  /* retrieve start address of marker */
  marker = pFrameNode->pMarker;

  /* check marker integrity... */
  for(pos = 0; pos < pFrameNode->markerByteSize; pos++ ) 
  {
    if(marker[pos] != MDP_MALLOC_MARKER)
    {
      /** MEMORY CORRUPTION DETECTED! **/
      /* reset marker */
      memset(marker, MDP_MALLOC_MARKER, pFrameNode->markerByteSize);

      /* report corruption! */
      return FALSE;
    }
  }

  /* gone that far -> all is good! */
  return TRUE;
}


/*===========================================================================

FUNCTION mdp_create_primary_frame()

DESCRIPTION
  This function is a wrapper to mdp_malloc and creates a heap frame for
  the primary display based on a BPP format.

DEPENDENCIES
 none

RETURN VALUE
  pointer to frame

SIDE EFFECTS
  none

===========================================================================*/
mdp_frame_type* mdp_create_primary_frame(disp_bpp_type bpp)
{
  uint16 uBpp;

  if(bpp == DISP_8BPP)
    uBpp = 8;
  else if (bpp == DISP_18BPP)
    uBpp = 18;
  else 
    uBpp = 16; /* default: DISP_16BPP */

  return mdp_create_new_frame(_primary_lcd.disp_width, _primary_lcd.disp_height, uBpp);
}


/*===========================================================================

FUNCTION mdp_create_secondary_frame()

DESCRIPTION
  This function is a wrapper to mdp_malloc and creates a heap frame for
  the secondary display based on a BPP format.

DEPENDENCIES
 none

RETURN VALUE
  pointer to frame

SIDE EFFECTS
  none

===========================================================================*/
mdp_frame_type* mdp_create_secondary_frame(disp_bpp_type bpp)
{
  uint16 uBpp;

  if ( bpp == DISP_8BPP )
    uBpp = 8;
  else if ( bpp == DISP_18BPP )
    uBpp = 18;
  else
    uBpp = 16; /* default: DISP_16BPP */

  return mdp_create_new_frame(_secondary_lcd.disp_width, _secondary_lcd.disp_height, uBpp);
}


/*===========================================================================

FUNCTION mdp_create_largest_frame()

DESCRIPTION
  This function is a wrapper to mdp_malloc and creates a heap frame 
  based on a BPP format that will accomodate both the primary and secondary
  display.

DEPENDENCIES
 none

RETURN VALUE
  pointer to frame

SIDE EFFECTS
  none

===========================================================================*/
mdp_frame_type* mdp_create_largest_frame(disp_bpp_type bpp)
{
  uint16 uBpp;

  if(bpp == DISP_8BPP)
    uBpp = 8;
  else if (bpp == DISP_18BPP)
    uBpp = 18;
  else
    uBpp = 16; /* default: DISP_16BPP */

  return mdp_create_new_frame(_biggest_lcd.disp_width, _biggest_lcd.disp_height, uBpp);
}


/*=============================================================================
                         PRIVATE IMPLEMENTATION
=============================================================================*/

/*===========================================================================

FUNCTION mdp_init_frame_service()

DESCRIPTION
  This function initializes the specialized memory allocation service.
  
  As the MDP driver now uses dynamic allocation for its screen buffers,
  a simple service was built around malloc to provide additional services, 
  such as memory overflow check.  To do so, requires some memory management
  structures that need to be initialized. 

DEPENDENCIES
  none.

RETURN VALUE
  boolean => TRUE if successful, FALSE otherwise

SIDE EFFECTS
  none.

===========================================================================*/
boolean mdp_init_frame_service()
{
  static boolean inited = FALSE;

  /* only initialize once... */
  if(inited == TRUE)
    return TRUE;

  /* first time call, carry on... */
  inited = TRUE;

  /* initialize Frame Service */
  q_init( &(qListOfDynamicFrames.list));
  qListOfDynamicFrames.totalMemUsage = 0;

  return TRUE;
}


/*===========================================================================

FUNCTION mdp_create_new_frame()

DESCRIPTION
  This function provides dynamic allocation of memory-aligned screen buffers
  as required by the MDP.

DEPENDENCIES
  none.

RETURN VALUE
  boolean => TRUE if successful, FALSE otherwise

SIDE EFFECTS
  none.

===========================================================================*/
mdp_frame_type* mdp_create_new_frame(uint16 pixelWidth, uint16 pixelHeight, uint16 bpp)
{
  static boolean firstCall = TRUE;
  frame_node_type* pFrameNode;
  uint8* pBuffer;
  uint16  alignOffset;
  uint32 bufferByteSize, 
  markerByteSize;

  /* initialize service if first time called */
  if(firstCall == TRUE) 
  {
    mdp_init_frame_service();
    firstCall = FALSE;
  }

  /* check requested screen size */
  if((pixelWidth == 0)  || (pixelWidth > MAX_WB_WIDTH) || 
     (pixelHeight == 0) || (pixelHeight > MAX_WB_HEIGHT))
  {
    return NULL;
  }

  /* Allocate Screen Buffer Heap Memory */
  /* screen buffer size in bytes */
  bufferByteSize = SCR_BYTE_SIZE(pixelWidth, pixelHeight, bpp);

  /* marker size (one line) in bytes */
  markerByteSize = SCR_BYTE_SIZE(pixelWidth, 1, bpp);

  /* request enough heap memory for working buffer + marker + alignment offset */
  if((pBuffer = (uint8*)MDP_FRAME_MALLOC( bufferByteSize + markerByteSize + 4)) == NULL)
    return NULL;

  /* compute offset to get working buffer aligned with divide-by-4 address */
  alignOffset = (4 - ((uint32)pBuffer % 4)) % 4;

  /* Register buffer with Frame-Service */
  /* create service data-structure */
  if((pFrameNode = (frame_node_type*)MDP_FRAME_MALLOC(sizeof(frame_node_type))) == NULL)
  {
    MDP_FRAME_FREE(pBuffer);
    return NULL;
  }

  /* frame data init */
  pFrameNode->frame.buffer = pBuffer + alignOffset;
  pFrameNode->frame.byteSize = bufferByteSize;
  pFrameNode->frame.pxWidth = pixelWidth;
  pFrameNode->frame.pxHeight = pixelHeight;
  pFrameNode->frame.bpp = (disp_bpp_type)bpp;
  pFrameNode->frame.pParent = pFrameNode;

  /* frame-service data init */
  pFrameNode->mallocBuffer = pBuffer;
  pFrameNode->pMarker = pFrameNode->frame.buffer + bufferByteSize;
  pFrameNode->markerByteSize = markerByteSize;

  /* initialize marker */
  memset(pFrameNode->pMarker, MDP_MALLOC_MARKER, pFrameNode->markerByteSize);

  /* register new frame with Frame Service */
  q_put( &(qListOfDynamicFrames.list), q_link(pFrameNode, &(pFrameNode->qLink)) );
  qListOfDynamicFrames.totalMemUsage += pFrameNode->frame.byteSize + 
                                        pFrameNode->markerByteSize +
                                        sizeof(frame_node_type);

  return &(pFrameNode->frame);
}

