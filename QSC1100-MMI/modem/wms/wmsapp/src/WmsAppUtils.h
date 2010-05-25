#ifndef WMSAPPUTILS_H
#define WMSAPPUTILS_H
/*=============================================================================

FILE: WMSAppUtils.C

    U S E R   I N T E R F A C E   S M S   D I S P L A Y   S E R V I C E

GENERAL DESCRIPTION
  Contains utlity functions for formatting the data displayed and used by the
  WMS application.



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsAppUtils.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/08   HN      Added check for buffer overflow in WMSAPPU_ITOA()
07/14/08   HN      Added check for buffer overflow in WMSAPPU_FormatNumberType(),
                   WMSAPPU_FormatNumberPlan(), WMSAPPU_FormatAddress(),
                   WMSAPPU_FormatPID() and WMSAPPU_FormatDCS().  
06/27/08   PMD     Cleaned up Feature Flags in Code
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Mainlined 637A.
04/21/06   HQ      Added support for CDMA Read Ack message.
01/26/06   PMD     Fixed Compile Errors by making pMe as const
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast 
01/07/05   PMD     Added Comments for various functions.
01/07/05   PMD     Added WMSAPPU_MEMCPY, WMSAPPU_FREE and WMSAPPU_MALLOC Functions.
11/22/04   VK      Added GW_ToUnicode and GW_FromUnicode functions. 
10/16/04   PMD     Added WMSAPPU_LoadImageFromBuffer function
10/16/04   PMD     Change Prototype for WMSAPPU_DeleteBroadcastMessage
08/05/04   Rex     Featuerize IWMSDIAG
06/10/04   PMD     Fixed Featurization Issue
05/14/04   Rex	    Added support for WMSAPPU_EventCbSmsNotify and WMSAPPU_GetCbMsgIndex
04/08/04   PMD     Added support for Delete All GW CB Services Function
03/31/04   PMD/Rex Moved WMSAPPU_IsMsgActive and WMSAPPU_UpdateIcons.
03/22/04   PMD     Added Function WMSAPPU_DeleteAllBroadcastMessage
03/22/04   PMD     Added Function WMSAPPU_CheckIfUpdate()
03/16/04   AP      Added support for the user app
02/14/04   Rex     Remove compile warning in code
02/03/04   PMD     Removed FEATURE_SMS_SEGMENT
01/29/04   Rex     Added function ConvertBCDToInt
01/20/04   PMD     Added WMSAPPU_ITOA()
01/14/04   PMD     Added Function to Segment / Concat Large Messages
01/09/04   PMD     Removed WMSAPPU_CauseCodeEnumToString under GW Feature
01/06/04   Rex     Cause Code Display Changes
11/21/03   PMD     Added GSM Cell Broadcast Support
11/06/03   Rex     Status Report Display Changes
09/16/03   PMD     Removed WMSAPPMN_ConvertUINT8ToAscii from under FEATURE_CDSMS
07/29/03   AP      Modified included files to generate the correct dependencies.
07/07/03   PMD     Changes to remove Lint Errors
06/26/03   SUN     Added support for all 160 characters
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
05/20/03   SUN     Fixed Validity Display
05/17/03   PMD     Added function ConvertUINT8ToAscii
05/09/03   SUN     Added Support for class summary
04/03/03   SUN/AP  Clean up.
03/17/03   sun     Featurized the file under FEATURE_WMS_APP
03/04/03   AP      Made HexChar available for both modes.
02/27/03   AP      Clean up.
02/14/03   sun     Initial Version
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifdef FEATURE_WMS_APP
#include "target.h"
#include "comdef.h"
#if defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS)
#include "WmsApp.h"

#define WMSAPPU_FREE(ptr) {FREEIF(ptr); ptr = NULL;}

/*===========================================================================

                      PUBLIC FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION WMSAPPU_WriteToBuffer

DESCRIPTION
  Helper function to write to a bounded-buffer. If write leads to overflow the
  buffer is left untouched.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_WriteToBuffer
( char* to_buf,
  int to_buf_size,
  char* from_buf,
  int from_buf_size,
  int *offset
);


/*===========================================================================
FUNCTION WMSAPPU_MAPINDEX

DESCRIPTION
  Maps the scroll line of the desired message to the index of the
  message list.

RETURN
  Index value of the selected message.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_message_index_type WMSAPPU_MapIndex
(
  const CWMSAPP *pMe,
  wms_memory_store_e_type memStore,
  int scrLine
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATNUMBERTYPE

DESCRIPTION
  Formats number type of address.

RETURN
  i, the number of characters to display the number type.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatNumberType
(
  const wms_address_s_type          * addr,
  uint8                       * buf,
  int                      buf_size
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATNUMBERPLAN

DESCRIPTION
  Formats number plan of address.

RETURN
  i, the number of lines needed to display the number plan.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatNumberPlan
(
  const wms_address_s_type          * addr,
  uint8                       * buf,
  int                      buf_size
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATADDRESS

DESCRIPTION
  Formats address field of message.

RETURN
  i, the number of characters needed to display an address and its parameters

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAddress
(
  const CWMSAPP *pMe,
  boolean               digits_only,
  const wms_address_s_type    * addr,
  uint8                 * buf,
  int                buf_size
);

/*===========================================================================
FUNCTION WMSAPPU_HEXCHAR

DESCRIPTION
  Converts decimal to ascii char representation.

RETURN
  table[n]: the ascii value.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 WMSAPPU_HexChar
(
  uint8 n
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATTIMESTAMP

DESCRIPTION
  Formats timestamp value of the message.

RETURN
  i, the number of lines needed to display the timestamp.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatTimestamp
(
  const CWMSAPP               *pMe,
  wms_timestamp_s_type  * time,
  byte                  * disp,
  int                disp_size
);

/*===========================================================================
FUNCTION WMSAPPU_GETMSGCOUNT

DESCRIPTION
  Gets the message count for the outbox or inbox.

RETURN
  count, the number of messages in the particular box.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int WMSAPPU_GetMsgCount
(
  const CWMSAPP *pMe,
  wmsapp_box_type box_type,
  wms_memory_store_e_type mem_store
);


/*===========================================================================
FUNCTION WMSAPPU_ConvertUINT8ToASCII

DESCRIPTION
  Converts a Uint8 integer (0-255) to appropriate ASCII Text
  Used for retrival of Files given uint8 number
  Returns Number of Characters in the New ASCII Text

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPU_ConvertUINT8ToASCII
(
  uint8 number,                 /* uint8 number to convert */
  char *txt                     /* Ascii output string */
);


/*=========================================================================
FUNCTION
  WMSAPPU_ConvertBCDToInt

DESCRIPTION
  Convert a BCD to Integer.
  Valid BCD values altogether are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
=========================================================================*/
uint8 WMSAPPU_ConvertBCDToInt
(
  const uint8 bcd
);


/*===========================================================================
FUNCTION WMSAPPU_ITOA

DESCRIPTION
  Converts any Integer to appropriate ASCII Text
  Returns Number of Characters in the New ASCII Text

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 WMSAPPU_ITOA
(
  int number,
  char *string,
  int size,
  int base
);

/*===========================================================================
FUNCTION WMSAPPU_CauseCodeEnumToString

DESCRIPTION
  Maps the Cause Value to appropriate Text String

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
char* WMSAPPU_CauseCodeEnumToString
(
 const wms_submit_report_info_s_type *pRptInfo
);

/*===========================================================================
FUNCTION WMSAPPU_ConvertHexToAscii

DESCRIPTION
  Converts any Hex to appropriate ASCII Text

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte WMSAPPU_ConvertHexToAscii
(
  byte num                 /* Hexadecimal Number to convert */
);

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION WMSAPPU_FORMATTELESERVICE

DESCRIPTION
  Formats teleservice param of the message.

RETURN
  i, the number of characters needed to display the teleservice.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatTeleservice
(
  wms_teleservice_e_type  teleservice,
  uint8                    *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATCDMADCS

DESCRIPTION
  Formats DCS value of the message.

RETURN
  i, the number of characters needed to display the DCS.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatCDMADcs
(
  wms_user_data_encoding_e_type  dcs,
  uint8                          *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATPRIORITY

DESCRIPTION
  Formats priority value of the message.

RETURN
  i, the number of characters needed to display the priority.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatPriority
(
  wms_priority_e_type  priority,
  uint8                *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATPRIVACY

DESCRIPTION
  Formats privacy value of the message.

RETURN
  i, the number of characters needed to display the privacy.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatPrivacy
(
  wms_privacy_e_type  privacy,
  uint8               *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATACKINFO

DESCRIPTION
  Formats ack info value(s) of the message.

RETURN
  i, the number of characters needed to display the ack info.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAckInfo
(
  uint32  mask,
  boolean tl_ack,
  boolean del_ack,
  boolean user_ack,
  boolean read_ack,
  uint8   *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATLANGUAGE

DESCRIPTION
  Formats language value of the message.

RETURN
  i, the number of characters needed to display the language.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatLanguage
(
  wms_language_e_type  language,
  uint8                *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATDISPLAYMODE

DESCRIPTION
  Formats display mode value of the message.

RETURN
  i, the number of characters needed to display the display mode.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatDisplayMode
(
  wms_display_mode_e_type  display_mode,
  uint8                    *buf
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATBDTYPE

DESCRIPTION
  Formats bearer data message type value of the message.

RETURN
  i, the number of characters needed to display the BD type.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatBDType
(
  wms_bd_message_type_e_type  bd_type,
  uint8                       *buf
);

/*===========================================================================
FUNCTION WMSAPPU_ConvertToUnicode

DESCRIPTION
  Converts the user data to unicode.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies user data and data length.
===========================================================================*/
void WMSAPPU_ConvertToUnicode
(
  wms_client_bd_s_type *bd_data
);

/*===========================================================================
FUNCTION WMSAPPU_ConvertFromUnicode

DESCRIPTION
  Converts the user data from unicode.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies user data and data length.
===========================================================================*/
void WMSAPPU_ConvertFromUnicode
(
  wms_client_bd_s_type *bd_data
);

/*===========================================================================
FUNCTION WMSAPPU_FORMATALERTMODE

DESCRIPTION
  Formats alert mode value of the message.

RETURN
  i, the number of characters needed to display the alert mode.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAlertMode
(
  wms_alert_mode_e_type  alert_mode,
  uint8                *buf
);

/*===========================================================================
FUNCTION IS_NON_STD_EMAIL

DESCRIPTION
  Checks whether the address is using the non-stanard e-mail address format.
  Returns TRUE if so.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_IsNonStdEmail
(
  boolean             is_dtmf,
  const uint8         *n_buf,
  const uint8         n_length
);

/*===========================================================================
FUNCTION WMSAPPU_GetNonStdEmail

DESCRIPTION
  Gets the value of the non standard e-mail address. Returns length.

DEPENDENCIES
  None

SIDE EFFECTS
  Changes the value of email_addr directly.
===========================================================================*/
uint8 WMSAPPU_GetNonStdEmail
(
  const uint8 *user_data,
  uint8 *email_addr
);

/*===========================================================================
FUNCTION WMSAPPU_PUTNONSTDEMAIL

DESCRIPTION
  Puts the non-std email address in the user data porition of the message.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies userData directly.
===========================================================================*/
void WMSAPPU_PutNonStdEmail
(
  const uint8 *emailAddr,
  CWMSAPP      *pMe,
  wms_cdma_user_data_s_type *userData
);

/*===========================================================================
FUNCTION WMSAPPU_REMOVENONSTDEMAIL

DESCRIPTION
  Removes the non-std email from the user data of the message.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies the user data and possibly the mask of clBd.
===========================================================================*/
void WMSAPPU_RemoveNonStdEmail
(
  const CWMSAPP *pMe,
  wms_client_bd_s_type  *clBd
);

#ifdef FEATURE_CDSMS_BROADCAST
/*===========================================================================
FUNCTION WMSAPPU_BCSORTSVCS

DESCRIPTION
  Sorts the Broadcast SMS services into their respective languages and assigns
  status to a particular languge if all services are selected, partially
  selected or all services are not selected.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies pMe directly.
===========================================================================*/
void WMSAPPU_BCSortSvcs
(
  CWMSAPP *pMe
);
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

/*===========================================================================
FUNCTION WMSAPPU_SplitUserData

DESCRIPTION
  Splits the Long User Data into Multiple segments.
  ** This function allocates memory for the Split Message.
     It is upto the Calling function to FREE the Memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  TRUE: Successful Segmentation
  FALSE: Segmentation Failed
===========================================================================*/
boolean WMSAPPU_SplitUserData
(
  const CWMSAPP                   * pMe,             /* INPUT */
  const wmsapp_user_data_s_type   * origUserData,    /* INPUT */
  uint8                     * numSegments,     /* OUTPUT */
  wmsapp_user_data_s_type  ** splitUserDataPtr /* OUTPUT */
);

/*===========================================================================
FUNCTION WMSAPPU_ConcatUserData

DESCRIPTION
  Concatenates the Segmented User Data to a single User Data.
  ** This function allocates memory for the HugeMessage.
     It is upto the Calling function to FREE the Memory.

ASSUMPTIONS:
  splitUserDataPtr has all the segments contiguously sorted from
  segment #1 to the last segment
  All segments have same encoding.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  TRUE: Successful Concatenation
  FALSE: Concatenation Failed
===========================================================================*/
boolean WMSAPPU_ConcatUserData
(
  const CWMSAPP                   * pMe,              /* INPUT */
  const uint8                       numSegments,      /* INPUT */
  const wmsapp_user_data_s_type   * splitUserDataPtr, /* INPUT */
  wmsapp_user_data_s_type   ** pHugeUserDataPtr /* OUTPUT */
);

/*===========================================================================

FUNCTION: WMSAPPU_IsMsgActive

DESCRIPTION
   Checks if there are any new messages

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_IsMsgActive
(
  const CWMSAPP *pMe
);

/*===========================================================================
FUNCTION WMSAPPU_UPDATEICONS

DESCRIPTION
  Updates the envelope icon to reflect the current status.
  For text messages : New urgent messages are reflected with a blinking
  envelope icon.  New non urgent messages are reflected with a solid on
  envelope icon.  No new messages is reflected by turning off the envelope
  icon.

  For Voice Mail : Urgent voice mail is reflected with a blinking envelope  icon.
  Non urgent voice mail is reflected with a solid on envelope icon. No voice
  mail ( no message or a count of 0 ) is reflected by turning the envelope icon
  off.

DEPENDENCIES
  pMe->m_smslist must be current
  WMSAPPU_IsMsgActive

SIDE EFFECTS
 None

===========================================================================*/
void WMSAPPU_UpdateIcons
(
  const CWMSAPP *pMe
);

#if defined(FEATURE_GWSMS_BROADCAST) && defined(FEATURE_IWMSDIAG)
#error code not present
#endif /* defined(FEATURE_GWSMS_BROADCAST) && defined(FEATURE_IWMSDIAG) */

/*===========================================================================
FUNCTION: WMSAPPU_LoadImageFromBuffer

DESCRIPTION
   Loads the IImage Pointer from an Image Buffer Provided

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
IImage *WMSAPPU_LoadImageFromBuffer
(
  const CWMSAPP *pMe,
  void    *pvBuff, 
  uint32   dwSize
);

/*===========================================================================
FUNCTION: WMSAPPU_MEMCPY

DESCRIPTION
   Copies Memory from Input to Output

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_MEMCPY
(
  const void *input, 
  uint32 input_size, 
  void *output,
  uint32 output_size
);

/*===========================================================================
FUNCTION: WMSAPPU_MALLOC

DESCRIPTION
   Allocates Memory

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_MALLOC
(
  void **ptr, 
  uint32 size
);

#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS)*/
#endif /*FEATURE_WMSAPP*/
#endif /* WMSAPPUtils_H */

