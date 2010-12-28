/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*

  J O I N T   P H O T O G R A P H E R S  E X P E R T S   G R O U P   (JPEG)   
                             D E C O D E R

GENERAL DESCRIPTION
  This module contains functions to decode Joint Photographers Expert Group 
  (JPEG) image files.

EXTERNALIZED FUNCTIONS
  NONE

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/stillimage/jpeg6k/jpegdecoder/main/latest/inc/jpeg_mrk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/06   vma     Fixed all Lint errors
11/28/05   arv     Fixed Lint errors - Phase 1
06/10/05   arv     Initial version

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/


#ifdef DMSS

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */
#include "msg.h"                /* Message logging/reporting services      */
#endif




#ifdef FEATURE_JPEG_DECODER_REV2
#include "jpegd.h"
#endif

#ifdef FEATURE_JPEG_DECODER


#include "jpegi.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/



/* Forward declaration */
uint8 clamp(int32 i);
void recedeImagePtr(void);
JPEGD_STATUS_MSG jpegd_interpretDQTMarker(void);
JPEGD_STATUS_MSG jpegd_interpretSOFMarker(void);
JPEGD_STATUS_MSG jpegd_skipMarker(void);
JPEGD_STATUS_MSG jpegd_readDRIMarker(void);
int32 jpegd_readNextMarker(void);
int32 jpegd_parseMarkers(void);
JPEGD_STATUS_MSG jpegd_findSOIMarker(void);
void jpegd_correctBuffer(void);
JPEGD_STATUS_MSG jpegd_findSOFMarker(void);
int32 read_dht_marker(void);





//Already present declarations
uint32 fetchBits( int32, boolean );
uint8 fetchByte(void);

//int32 jpegd_initializeScan(void);
JPEGD_STATUS_MSG jpegd_interpretSOSMarker(void);
//void MSG_ERROR (char *, uint32, uint32, uint32);




#endif /* FEATURE_JPEG_DECODER */

