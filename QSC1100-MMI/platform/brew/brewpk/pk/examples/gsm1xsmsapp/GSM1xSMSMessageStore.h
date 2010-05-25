#ifndef GSM1XSMSMESSAGESTORE_H
#define GSM1XSMSMESSAGESTORE_H

/*===========================================================================

                     S M S M e s s a g e S t o r e . h
                

GENERAL DESCRIPTION
   This file contains prototypes for functions used to store and retrieve 
   SMS messages.
 
EXTERNALIZED FUNCTIONS
   GSM1xSMSMessageStore_ResetMessageStruct
   GSM1xSMSMessageStore_GetNextMessage
   GSM1xSMSMessageStore_GetMessageByID
   GSM1xSMSMessageStore_EraseEntireMBox
   GSM1xSMSMessageStore_EraseMessageFromMBox
   GSM1xSMSMessageStore_EraseOldMessagesFromMBox
   GSM1xSMSMessageStore_GetAddresseeAtIndex
   GSM1xSMSMessageStore_StorePrefs
   GSM1xSMSMessageStore_RestorePrefs

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Database services must be available

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define SET_READ       TRUE
#define DONT_SET_READ  FALSE

#define SMS_MESSAGE_STORE_INDEX_NONE  0xFFFF

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_ResetMessageStruct
  
DESCRIPTION:
   Reset the fields of the message struct.

PROTOTYPE:
   void GSM1xSMSMessageStore_ResetMessageStruct(SMSMessageStruct ms);

PARAMETERS:
   ms [in]: message struct to reset.

RETURN VALUE:
   None.

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_ResetMessageStruct(SMSMessageStruct *ms);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_NumMessagesInMBox
  
DESCRIPTION:
   Returns the number of messages in the specified mailbox.

PROTOTYPE:
   uint16 GSM1xSMSMessageStore_NumMessagesInMBox(SMSApp * pMe, 
					    SMSMBoxType eMBoxType);

PARAMETERS:
   pMe [in]: Pointer to the SMS applet structure.

   eMBoxType [in]: Which mailbox to check.

RETURN VALUE:
   The number of messages in the current mailbox.

SIDE EFFECTS:
   None
=============================================================================*/
uint16 GSM1xSMSMessageStore_NumMessagesInMBox(SMSApp *pMe, SMSMBoxType eMBoxType);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetNextMessage
  
DESCRIPTION:
   Fills in the szNextMsg struct with the next message in the message store.

PROTOTYPE:
   uint16 GSM1xSMSMessageStore_GetNextMessage(SMSApp * pMe,
                                        uint16 *pwID,
                                        SMSMessageStruct *msNextMsg,
                                        boolean bSetRead);
PARAMETERS:
    pMe [in]: Pointer to the SMS applet structure.

   pwID [in,out]: Pointer to ID of the last message, filled with ID of message
   that is returned.

   msNextMsg [out]: Struct into which to place message data.
   
   bSetRead [in]: Whether or not to set the message as having been read

RETURN VALUE:
    TRUE if message returned is valid. FALSE otherwise.

COMMENTS:
    Getting a message from the database doesn't necessarily mean
    the caller wants the message to display its contents, but perhaps to
    iterate through the messages to find a particular type. Therefore, if
    the message is being retreived for this purpose, we shouldn't mark
    the message as having been read.

SIDE EFFECTS:
   Message is marked read if bSetRead is TRUE and the message has never been
   read before.
   
=============================================================================*/
boolean GSM1xSMSMessageStore_GetNextMessage(SMSApp * pMe,
                                       uint16 *pwID,
                                       SMSMessageStruct *msNextMsg, 
                                       boolean bSetRead);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetMessageByID
  
DESCRIPTION:
   Fills in the msMsg struct with the data for the message matching wID.

PROTOTYPE:
   uint16 GSM1xSMSMessageStore_GetMessageByID(SMSApp * pMe,
					 SMSMBoxType eMBoxType,
                                         uint16 wID,
                                         SMSMessageStruct *msMsg,
					 boolean bSetRead);

PARAMETERS:
   pMe [in] : SMS applet data structure

   eMBoxType [in]: Which mailbox to check.

   wID [in]: ID of the message to return.

   msMsg [out]: Struct into which to place message data.
   
   bSetRead [in]: Whether or not to set the message as having been read

RETURN VALUE:
   TRUE if msMsg contains a valid message.  FALSE otherwise.

SIDE EFFECTS:
   Message is marked read if bSetRead is TRUE and the message has never been
   read before.
=============================================================================*/
boolean GSM1xSMSMessageStore_GetMessageByID(SMSApp *pMe, SMSMBoxType eMBoxType, 
				       uint16 wID, SMSMessageStruct *msMsg, 
				       boolean bSetRead);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseEntireMBox

DESCRIPTION:
   Erases all of the messages in the specified mailbox.

PROTOTYPE:
   boolean GSM1xSMSMessageStore_EraseEntireMBox(SMSApp * pMe, 
					   SMSMBoxType eMBoxType);

PARAMETERS:
   pMe [in]: Pointer to the SMS applet structure; 
    
   eMBoxType [in]: Which mailbox to completely erase.

RETURN VALUE:
   TRUE if successful.  FALSE otherwise.

SIDE EFFECTS:
   Will reset the message count upon success
=============================================================================*/
boolean GSM1xSMSMessageStore_EraseEntireMBox(SMSApp *pMe, SMSMBoxType eMBoxType);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseMessageFromMBox
  
DESCRIPTION:
   Erases the indicated message from the specified mailbox.

PROTOTYPE:
   boolean GSM1xSMSMessageStore_EraseMessageFromMBox(SMSApp *pMe,
						SMSMBoxType eMBoxType, 
						uint16 wID);

PARAMETERS:
    pMe [in]: Pointer the SMS applet structure

   eMBoxType [in]: Indicates from which mailbox to erase the message.

   wID [in]: ID of the message to erase.

RETURN VALUE:
   TRUE if successful.  FALSE otherwise.

SIDE EFFECTS:
   Will decrement the message count upon success.
=============================================================================*/
boolean GSM1xSMSMessageStore_EraseMessageFromMBox(SMSApp *pMe, 
					     SMSMBoxType eMBoxType, uint16 wID);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_EraseOldMessageFromMBox
  
DESCRIPTION:
   Erases the indicated message from the specified mailbox.

PROTOTYPE:
   boolean GSM1xSMSMessageStore_EraseOldMessagesFromInbox(SMSApp *pMe)

PARAMETERS:
    pMe [in]: Pointer the SMS applet structure

   eMBoxType [in]: Indicates from which mailbox to erase the message.

   wID [in]: ID of the message to erase.

RETURN VALUE:
   TRUE if successful.  FALSE otherwise.

SIDE EFFECTS:
   Will decrement the message count upon success.
=============================================================================*/
void GSM1xSMSMessageStore_EraseOldMessagesFromInbox(SMSApp *pMe);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_GetAddresseeAtIndex
  
DESCRIPTION:
   Fills in szBuf with the data for addressee for the message
   wMsgID in mailbox eMBoxType.

PROTOTYPE:
   void GSM1xSMSMessageStore_GetAddresseeAtIndex(SMSApp * pMe,
                                            SMSMBoxType eMBoxType,
                                            uint16 wMsgID,
                                            AECHAR *szBuf,
                                            uint16 wBufSize)

PARAMETERS:
    pMe [in]: Pointer the applet data structure.

   eMBoxType [in]: Which mailbox to check.

   wMsgID [in]: ID of the message.

   szBuf [out]: Addressee data.

   wBufSize [in]: Size of szBuf.

RETURN VALUE:
   None

COMMENTS:
   None

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_GetAddresseeAtIndex(SMSApp *pMe, SMSMBoxType eMBoxType, 
					 uint16 wMsgID, AECHAR *szBuf, uint16 wBufSize);

// Preferences support functions

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_StorePrefs
  
DESCRIPTION:
   Save the current preferences to the preferences file.

PROTOTYPE:

PROTOTYPE:
   void GSM1xSMSMessageStore_StorePrefs(SMSApp *pMe);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_StorePrefs(SMSApp *pMe);

/*=============================================================================
FUNCTION: GSM1xSMSMessageStore_RestorePrefs
  
DESCRIPTION:
   Restore the current preferences from the preferences file.

PROTOTYPE:

PROTOTYPE:
   void GSM1xSMSMessageStore_RestorePrefs(SMSApp *pMe);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSMessageStore_RestorePrefs(SMSApp *pMe);


/*=============================================================================
FUNCTION:  GSM1xSMSMessageStore_GetMaxSIMMsgs
 
DESCRIPTION: Returns the total number of messages allowed on the SIM card
 
PARAMETERS:
   *pMe: SMS Applet Pointer
 
RETURN VALUE:
   uint8: 

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 GSM1xSMSMessageStore_GetMaxSIMMsgs(SMSApp *pMe);

/*=============================================================================
FUNCTION:  GSM1xSMSMessageStore_MatchSMS
 
DESCRIPTION: Finds a matching SMS message for an incoming status report message
 
PARAMETERS:
   *pMe:      [in]  Pointer to the SMSApp instance
   MR:        [in]  Message reference to match
   *pIndex    [out] Pointer to index to set with matching SMS
   store:     [in]  Storage to search (i.e. NVRAM, SIM)
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None

SEE ALSO:

=============================================================================*/
boolean GSM1xSMSMessageStore_MatchSMS
(
   SMSApp *pMe, 
   uint8 MR, 
   const GSMSMSAddress *pRA, 
   uint16 *pIndex, 
   GSMSMSStorageType store
);

#endif // #ifndef
