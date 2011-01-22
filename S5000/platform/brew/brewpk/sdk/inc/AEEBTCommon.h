#ifndef AEEBTCOMMON_H
#define AEEBTCOMMON_H
/*=============================================================================
FILE:         AEEBTCommon.h

SERVICES:     IBT common definitions

DESCRIPTION:  This header file contains all definitions shared by all IBT 
              interfaces.

===============================================================================
        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEE.h"
#include "../../inc/AEEBTDef.h"

/*=============================================================================
 Helper Functions
=============================================================================*/
static __inline boolean AEEBT_LMP_FEATURE_SUPPORTED (uint32 bit, 
                                                     BTLMPFeatures* pFeatures)
{
   return ((pFeatures->bytes[bit/8] & (1 << (bit%8))) ? TRUE : FALSE);
}

/*=============================================================================
 BTEventData
=============================================================================*/
typedef union
{
   BDAddress            bdAddr;
   BTConnEvData         connEv;
   BTDevUpdateEvData    devUpdateEv;
   // more types to be added to this union
}  BTEventData;

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTEventData

Description:
  Union containing all data associated with the event of interest.
  
Definition:
   typedef union
   {
      BDAddress            bdAddr;
      BTConnEvData         connEv;
      BTDevUpdateEvData    devUpdateEv;
   }  BTEventData;

Members:
   bdAddr       : Bluetooth device address of the device
   connEv       : Information about SDP Service class of remote device
   devUpdateEv  : Information about the modified field in the device database

See Also:
   BDAddress
   BTConnEvData
   BTDevUpdateEvData
==============================================================================

==============================================================================
  MACROS  DOCUMENTATION
==============================================================================
AEEBT_LMP_FEATURE_SUPPORTED()

Description:
   This inline function returns TRUE if the feature bit FB is set in the 
   8-byte features bitmask.
   
Prototype:
   AEEBT_LMP_FEATURE_SUPPORTED (uint32 FB, BTLMPFeatures* pFeatures)
   
Parameters:
   FB                    : Any one of the LMP feature
   pFeatures             : LMP feautre array.
   
Comments:
   The possible values for LMP features are
   AEEBT_LMP_FEAT_3_SLOT_PKTS_B         0
   AEEBT_LMP_FEAT_5_SLOT_PKTS_B         1
   AEEBT_LMP_FEAT_ENCRYPT_B             2
   AEEBT_LMP_FEAT_SLOT_OFFSET_B         3
   AEEBT_LMP_FEAT_TIMING_ACCURACY_B     4
   AEEBT_LMP_FEAT_SWITCH_B              5
   AEEBT_LMP_FEAT_HOLD_MODE_B           6
   AEEBT_LMP_FEAT_SNIFF_MODE_B          7
   AEEBT_LMP_FEAT_PARK_MODE_B           8
   AEEBT_LMP_FEAT_RSSI_B                9
   AEEBT_LMP_FEAT_AUTO_DATA_RATE_B      10
   AEEBT_LMP_FEAT_SCO_LINK_B            11
   AEEBT_LMP_FEAT_HV2_PKTS_B            12
   AEEBT_LMP_FEAT_HV3_PKTS_B            13
   AEEBT_LMP_FEAT_U_LAW_LOG_B           14
   AEEBT_LMP_FEAT_A_LAW_LOG_B           15
   AEEBT_LMP_FEAT_CVSD_B                16
   AEEBT_LMP_FEAT_PAGING_SCHEME_B       17
   AEEBT_LMP_FEAT_POWER_CTRL_B          18
   AEEBT_LMP_FEAT_TRANS_SCO_DATA_B      19
   AEEBT_LMP_FEAT_ESCO_LINK_B           31
   AEEBT_LMP_FEAT_AFH_CAPABLE_SLAVE_B   35
   AEEBT_LMP_FEAT_AFH_CLASS_SLAVE_B     36
   AEEBT_LMP_FEAT_AFH_CAPABLE_MASTER_B  43
   AEEBT_LMP_FEAT_AFH_CLASS_MASTER_B    44
 
Return values:
  TRUE if LMP feature FB is present in pFeatures array.

=============================================================================*/
#endif // AEEBTCOMMON_H
