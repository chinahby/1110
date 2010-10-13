#ifndef AEEAVRCPTG_PRIV_EXT_DEF_H
#define AEEAVRCPTG_PRIV_EXT_DEF_H
/*=============================================================================
FILE:  AEEAVRCPTG_priv_ext_def.h

SERVICES:  AEE BT AVRCP private defines

GENERAL DESCRIPTION:

===============================================================================
        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.

 =========================================================================== */

#include "AEEAVRCPTG_Priv_Ext.h"


//=============================================================================
// 
//        TYPE DEFINITIONS
// 
//============================================================================= 
#define AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED     4

#define AEEBTAVRCPTG_MAX_NUM_OF_PLAYER_ATTR_VALS   4

#define AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX           8 

#define AEEBTAVRCPTG_MAX_NUM_OF_VALS               (2*AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED)

#define AEEBTAVRCPTG_NUM_OF_COMP_SUPPORTED         2

#define AEEBTAVRCPTG_MAX_NUM_EVENTS_SUPPORTED      8

#define AEEBTAVRCPTG_MEDIA_UPDATE_INTVL           5000   /* In Milliseconds */

#define AEEBTAVRCPTG_TG_BATT_STAT_UPDATE_INTVL    100000 /* In Milliseconds */

#define AEEBTAVRCPTG_LEVEL_NORMAL(x)        ( ((x) >= 50) && ((x) < 250) )
#define AEEBTAVRCPTG_LEVEL_WARNING(x)       ( ((x) >= 10) && ((x) < 50) ) 
#define AEEBTAVRCPTG_LEVEL_CRITICAL(x)      ( (x) < 10 ) 
#define AEEBTAVRCPTG_LEVEL_FULL_CHARGE(x)   ( (x) >= 250 )
//=============================================================================
// 
//       ENUMERATIONS AND CONSTANTS
// 
//============================================================================= 

/*
 * Enum to maintain System status
 */
enum AEEBTAVRCPTgSystemStatusEnum {
   AEEBTAVRCPTG_SYSTEM_POWER_ON,
   AEEBTAVRCPTG_SYSTEM_POWER_OFF,
   AEEBTAVRCPTG_SYSTEM_UNPLUGGED
};

typedef enum AEEBTAVRCPTgSystemStatusEnum AEEBTAVRCPTgSystemStatus;

/**
 *  Enum to maintain the PDU ids of the commands and responses
 */
#define AEEBTAVRCPTG_CAPABILITY_COMMAND_IND                         0x10
#define AEEBTAVRCPTG_LIST_ATTRIBUTES_COMMAND_IND                    0x11
#define  AEEBTAVRCPTG_LIST_ATTRIBUTE_VALUES_COMMAND_IND             0x12
#define AEEBTAVRCPTG_GET_ATTRIBUTE_VALUES_COMMAND_IND               0x13
#define AEEBTAVRCPTG_SET_ATTRIBUTE_VALUES_COMMAND_IND               0x14
#define AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND         0x15
#define AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND   0x16
#define AEEBTAVRCPTG_INFORM_CHARACTER_SET_COMMAND_IND               0x17
#define AEEBTAVRCPTG_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND        0x18
#define AEEBTAVRCPTG_GET_ELEMENT_ATTRIBUTE_COMMAND_IND              0x20
#define AEEBTAVRCPTG_GET_PLAY_STATUS_COMMAND_IND                    0x30
#define AEEBTAVRCPTG_EVENT_NOTIFICATION_COMMAND_IND                 0x31
#define AEEBTAVRCPTG_GROUP_NAVIGATION_COMMAND_IND                   0x1F
#define AEEBTAVRCPTG_ILLEGAL_PDU_ID                                 0xFF



typedef uint32 AEEBTAVRCPTgPduId;

/**
 *  Enum to hold to capabilties id of CT
 */
enum AEEBTAVRCPTgCapIdEnum {
   AEEBTAVRCPTG_CAPABILITY_COMPANY_ID       = 0x2,
   AEEBTAVRCPTG_CAPABILITY_EVENTS_SUPPORTED = 0x3
};

typedef enum AEEBTAVRCPTgCapIdEnum AEEBTAVRCPTgCapId;
//=============================================================================
// 
//       STRUCTURES AND UNIONS
// 
//============================================================================= 
struct AEEBTAVRCPTgCmdId {
   AEEBTAVRCPTgPduId                  cmdPduId;
   uint8                              txn;
   BDAddress                          bdAddr;
   BTResult                           cResult;
};

typedef struct AEEBTAVRCPTgCmdId AEEBTAVRCPTgCmdId;
/**
 * Structure to keep status updates about battery
 */
struct AEEBTAVRCPTgBatStatus {
   AEEBTAVRCPTgContBattStatus battStatus;
   boolean                    statUpdated;
};

typedef struct AEEBTAVRCPTgBatStatus AEEBTAVRCPTgBatStatus;
/**
 * Structure to keep status updates about group navigation
 */
struct AEEBTAVRCPTgGrpNavStatus {
   AEEBTAVRCPTgGroupNavCommandEnum grpNav;
   boolean                         statUpdated;
   boolean                         buttPress;
};

typedef struct AEEBTAVRCPTgGrpNavStatus AEEBTAVRCPTgGrpNavStatus;
/**
 * Structure to keep status updates about last command
 */
struct AEEBTAVRCPTgLastCmdInfo {
   AEEBTAVRCPTgPduId              cmdId;
   uint8                          txn;
   AEEBTAVRCPTgCmdResponseStatus  cmdstatus;
};

typedef struct AEEBTAVRCPTgLastCmdInfo AEEBTAVRCPTgLastCmdInfo;
/**
 * Structure to hold all the Avrcp Metadata Command Info from CT
 */
struct AEEBTAVRCPTgExtCmdInfo {
   AEEBTAVRCPTgCmdId              cmdId;
   AEEBTAVRCPTgPlayerAppAttrs     attrId;
   uint8                          trackIndex[AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX]; 
   boolean                        notifyApp;
   union {
      AEEBTAVRCPTgContBattStatus   ctBattStatus;
      AEEBTAVRCPTgCapId            capId;
      struct {
         AEEBTAVRCPTgGroupNavCommandEnum  grpNavCmd;
         boolean                          buttPress; 
      } grpNavInfo;

      struct {
         AEEBTAVRCPTgMediaEventMask eventId;
         uint32                     playbackInterval;
      } notifyInfo;

      struct {
         uint32                     count;
         uint32                     val[AEEBTAVRCPTG_MAX_NUM_OF_VALS]; 
      } pduData;

   } pduInfo;
};

typedef struct AEEBTAVRCPTgExtCmdInfo AEEBTAVRCPTgExtCmdInfo;
/**
 *  Structure to pass the text data from AEE to OEM layer
 */
struct AEEBTAVRCPTgItemTxt {
   uint32    itemId;
   uint32    charset;
   uint8*    itemTxt;
   uint32    itemTxtLen;  
};

typedef struct AEEBTAVRCPTgItemTxt AEEBTAVRCPTgItemTxt;
/**
 * Structure to hold Media information
 */
struct AEEBTAVRCPTgMediaTrackInfo {

   uint8    *trackTitle;
   uint32   trackTitleCharset; 
   uint32   trackTitleLen;

   uint8    *trackArtistName;
   uint32   trackArtistNameCharset;
   uint32   trackArtistNameLen; 

   uint8    *albumName;
   uint32   albumNameCharset;
   uint32   albumNameLen;

   uint8    *trackNum;
   uint32   trackNumLen;

   uint8    *trackTotal;
   uint32   trackTotalLen;

   uint8    *trackGenre;
   uint32   trackGenreCharset;
   uint32   trackGenreLen;

   uint8    *playTime;
   uint32   playTimeLen;
};

typedef struct AEEBTAVRCPTgMediaTrackInfo AEEBTAVRCPTgMediaTrackInfo;

/**
 * Structure to hold Attribute current value information set by app
 */
struct AEEBTAVRCPTgPlayerCurAttrValues {
   AEEBTAVRCPTgPlayerAppAttrs      attrId;
   uint32                          valueId; 
   boolean                         valUpdated;  
};

typedef struct AEEBTAVRCPTgPlayerCurAttrValues AEEBTAVRCPTgPlayerCurAttrValues;
/**
 * Structure to hold Attribute current value information set by CT
 */
struct AEEBTAVRCPTgPlayerCurAttrValSetByCt {
   boolean                          curValStatusUpdatebyCt;
   AEEBTAVRCPTgPlayerCurAttrValues  curValueUpdate;
};

typedef struct AEEBTAVRCPTgPlayerCurAttrValSetByCt AEEBTAVRCPTgPlayerCurAttrValSetByCt;
/**
 *  Structure to maintain status of CT supported charset notification to App
 */
struct AEEBTAVRCPTgCharsetSuppByCt {
   AEEBTAVRCPTgCharsetId        charset;
   boolean                      valUpdated;
};

typedef struct AEEBTAVRCPTgCharsetSuppByCt AEEBTAVRCPTgCharsetSuppByCt;
/**
 * Structure to hold playback information
 */
struct AEEBTAVRCPTgPlaybackInfo {
   AEEBTAVRCPTgPlayStatus playStatus;
   uint32                 curSongPos;
   uint8                  trackIndex[AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX];
};

typedef struct AEEBTAVRCPTgPlaybackInfo AEEBTAVRCPTgPlaybackInfo;
/**
 * Structure to hold Attribute value information
 */
struct AEEBTAVRCPTgAttributeValueInfo {
   uint32     valueId;
   uint32     valueTextCharset; 
   uint8      *valueText;
   uint32     valTextLen;
};

typedef struct AEEBTAVRCPTgAttributeValueInfo AEEBTAVRCPTgAttributeValueInfo;
/**
 * Structure to hold Attribute id information
 */
struct AEEBTAVRCPTgAttributeInfo {
   AEEBTAVRCPTgPlayerAppAttrs      attrId;
   uint32                          attrInfoCharset;
   uint8                           *attrText;
   uint32                          attrTextLen;
   uint8                           numValues;
   AEEBTAVRCPTgAttributeValueInfo  *values;
};

typedef struct AEEBTAVRCPTgAttributeInfo AEEBTAVRCPTgAttributeInfo;
#endif /* AEEAVRCPTG_PRIV_EXT_DEF_H */
