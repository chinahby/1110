#ifndef GSM1XSMSUTIL_H
#define GSM1XSMSUTIL_H
/*===========================================================================

                           S M S U t i l . h
                

GENERAL DESCRIPTION
   This file contains prototypes for utility procedures for common functions.

EXTERNALIZED FUNCTIONS
   GSM1xSMSUtil_ExtractTitle
   GSM1xSMSUtil_SecsToDateString
   GSM1xSMSUtil_InitMenuIcons
   GSM1xSMSUtil_SetMenuIcon
   GSM1xSMSUtil_ShowBusyIcon
   GSM1xSMSUtil_GSMTime2Secs

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

Copyright (c) 2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEGSMSMS.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#undef ASSERT
#define ASSERT_FATAL(x) 
#define ASSERT_NOT_REACHABLE
#define ASSERT(x)

////
// This macro is used at the start of a function to make Lint suppress
// 'not referenced' message for the specified parameter.  
//
// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSUtil_SecsToDateString

DESCRIPTION:
   Convert dwSecs to a string displaying the date.

PROTOTYPE:
   void GSM1xSMSUtil_SecsToDateString(SMSApp *pMe, uint32 dwTime,
                                       AECHAR *szBuf, int wBufLen);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

   dwTime [in]: Date in seconds form.

   szBuf [out]: String where date will be placed.

   wBufLen [in]: Length of szBuf string.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSUtil_SecsToDateString(SMSApp *pMe, uint32 dwTime,
              AECHAR *szBuf, int wBufLen);


/*=============================================================================
FUNCTION: GSM1xSMSUtil_AddBitmapMenuItem
  
DESCRIPTION:
   This function encapsulates the code needed to add a menu item with an icon
   to the current menu.

PROTOTYPE:
   boolean GSM1xSMSUtil_AddBitmapMenuItem(SMSApp   *pMe,
                                     IMenuCtl *pMenu,
                                     AECHAR   *szMenuText,
                                     uint16    wTextResID,
                                     uint16    wImageResID,
                                     AEEFont   wFont,
                                     uint32    dwData);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

   pMenu [in]: Pointer to menu to which to add the item.

   szMenuText [in]: String containing menu text.  If this is a NULL pointer or
      an empty string the menu text will be obtained from the string resource
      with id wTextResID.

   wTextResID [in]: ID for command item.  If szMenuText is NULL or an empty
      string this will also be used as the resource ID for the menu text.

   wImageResID [in]: Resource ID for the menu image.
   
   wFont [in]: Font type for the text of menu item

   dwData [in]: The 32bit parameter to be sent to the handler

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
boolean GSM1xSMSUtil_AddBitmapMenuItem(SMSApp   *pMe,
                              IMenuCtl *pMenu,
                              AECHAR   *szMenuText,
                              uint16    wTextResID,
                              uint16    wImageResID,
                              AEEFont   wFont,
                              uint32    dwData);

/*=============================================================================
FUNCTION: GSM1xSMSUtil_DecodeMBoxAndID
  
DESCRIPTION:
    Extracts the message ID and mailbox value from an encoded 32 bit
    number.

PROTOTYPE:
   void GSM1xSMSUtil_DecodeMBoxAndID (uint32 dwCode,
             SMSMBoxType * eMBoxType, uint16 *wID);

PARAMETERS:
    wdCode [in]: Value with encoded mailbox value and message ID
    
    eMBoxType [out]: Pointer to variable to store mailbox value
    
    wID [out]: Pointer to variable to store message ID


RETURN VALUE:
   None

COMMENTS:
    The message ID is encoded in the lower 16 bits and the mailbox value is 
    in the upper 16 bits.
   
SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSUtil_DecodeMBoxAndID (uint32 dwCode, 
               SMSMBoxType *eMBoxType, uint16 *wID);

/*===========================================================================

FUNCTION
   ExtractTitle

DESCRIPTION
   Updates the title field depending upon the mail box type.
   
   RUIM Box and Inbox: This function extracts the title (to be displayed on the 
   screen) from the From field (if there is any).
   
   Outbox : Extracts the title based on the outgoing number (To field)

DEPENDENCIES
   None

PARAMETERS
   pMe [in]: Pointer to the SMS applet structure
   eMBoxType [in]: Type of mail box to indicate which mail box is making this
                   request to extract title.
   pInMsg [in/out]: Pointer to SMS message structure

RETURN VALUE
   None

SIDE EFFECTS
   Depending upon the mail box type, if there is any From/To field present in 
   the received SMS message, then the pInMsg Title field is updated.
===========================================================================*/
void GSM1xSMSUtil_ExtractTitle ( SMSApp* pMe, SMSMBoxType eMBoxType,
                            SMSMessageStruct* pInMsg );

/*=============================================================================

FUNCTION:  GSM1xSMSUtil_InitMenuIcons
 
DESCRIPTION: Adds IDB_NONSELECT_ICON images to every menu item in the
             specified menu.
 
PARAMETERS:
   *pm: pointer to IMenuCtl instance
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_InitMenuIcons(IMenuCtl *pm);

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_SetMenuIcon
 
DESCRIPTION:
 
PARAMETERS:
   *pm:    pointer to IMenuCtl instance
   listId: list ID of the item to set
   bSel:   indicates whether to select or unselect the icon
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_SetMenuIcon(IMenuCtl   *pm, 
                 uint16      listId, 
                 boolean     bSel);

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_ShowBusyIcon
 
DESCRIPTION: Sets or clears the busy icon
 
PARAMETERS:
   *ps: pointer to IShell instance
   pd: pointer to IDisplay instance
   rc: pointer to AEERect structure
   bClear: indicates whether to set or clear the busy icon
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc, boolean bClear);

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_GSMTime2Secs
 
DESCRIPTION: Convert from GSM time format to seconds
 
PARAMETERS:
   gsmtime: the GSM time to convert to seconds
 
RETURN VALUE:
   The time in seconds

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int GSM1xSMSUtil_GSMTime2Secs(GSMSMSTime *gsmtime);

#endif // SMSUTIL_H
