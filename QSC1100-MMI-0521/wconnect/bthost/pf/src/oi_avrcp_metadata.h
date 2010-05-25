/**
@file
@internal

*/

/** \addtogroup AVRC_Metadata */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2007 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef __AVRCP_METADATA_H
#define __AVRCP_METADATA_H

#include "oi_avrcp_consts.h"
#include "oi_avrcp.h"

#define OI_AVRCP_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

typedef struct {
    OI_UINT8 eventID;
    OI_UINT32 playbackInterval;
} AVRCP_EVENT_NOTIFICATION;

typedef struct {
    OI_UINT8 *id;
    OI_UINT8 numAttr;
    OI_UINT32 *attrs;
} AVRCP_ELEMENT_ATTRIBUTE_REQUEST;

typedef struct {
    OI_UINT8 attrID;
    OI_UINT8 numValues;
    OI_UINT8 *values;
} AVRCP_ATTRIBUTE_VALUE_TEXT_REQUEST;

typedef struct {
    OI_UINT8 numCharSets;
    OI_UINT16 *charSetIDs;
} AVRCP_INFORM_CHARACTER_SET;

typedef struct {
    OI_UINT8 id;
    OI_UINT8 type;
    
    union {
        OI_UINT8 capabilityID;
        OI_AVRCP_PLAYER_APPLICATION_ATTRIBUTES attribute;
        AVRCP_ATTRIBUTE_VALUE_TEXT_REQUEST attrValueText;
        AVRCP_INFORM_CHARACTER_SET charSetCT; 
        AVRCP_ELEMENT_ATTRIBUTE_REQUEST getElementAttributes;        
        AVRCP_EVENT_NOTIFICATION notification;
        OI_UINT8 batteryStatus;
        OI_UINT8 responsePDU;
    } u;
} OI_AVRCP_COMMAND;


OI_STATUS AVRCPMetadata_CmdInd(OI_BD_ADDR *addr,
                               OI_UINT8 transaction,
                               OI_AVRCP_FRAME *frame);

void AVRCPMetadata_RecvInd(OI_BD_ADDR *addr,
                                OI_UINT8 transaction,
                                OI_AVRCP_FRAME *frame);



#endif

/**@}*/
