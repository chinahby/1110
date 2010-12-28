#ifndef OEM_GSM1XCARDHANDLER_SVC_H
#define OEM_GSM1XCARDHANDLER_SVC_H

/*===========================================================================

                           << OEMGSM1xCardHandler_SVC.h >>


GENERAL DESCRIPTION
   AEE OEM SIM Interface header.  This file contains all of the function
   prototypes for routines that are required by the AEE to function with
   SIM.

EXTERNALIZED FUNCTIONS
    OEMGSM1xCardHandler_SVC_Init
    OEMGSM1xCardHandler_SVC_IsSMSInit
    OEMGSM1xCardHandler_SVC_IsSMSEnabled
    OEMGSM1xCardHandler_SVC_GetSMSStatus
    OEMGSM1xCardHandler_SVC_GetNumSMSSlots
    OEMGSM1xCardHandler_SVC_GetSms
    OEMGSM1xCardHandler_SVC_SetSms
    OEMGSM1xCardHandler_SVC_ClearSms
    OEMGSM1xCardHandler_SVC_ClearAllSms
    OEMGSM1xCardHandler_SVC_ReadSMSCAddress
    OEMGSM1xCardHandler_SVC_SetSMSCAddress
    OEMGSM1xCardHandler_SVC_ReadSMSS
    OEMGSM1xCardHandler_SVC_SetSMSS
    OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN
    OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN
    OEMGSM1xCardHandler_SVC_ReadPLMNSelector
    OEMGSM1xCardHandler_SVC_WritePLMNSelector
    OEMGSM1xCardHandler_SVC_ReadAccessClass
    OEMGSM1xCardHandler_SVC_ReadICCId
    OEMGSM1xCardHandler_SVC_ReadIMSI
    OEMGSM1xCardHandler_SVC_ReadMSISDN
    OEMGSM1xCardHandler_SVC_ReadAD
    OEMGSM1xCardHandler_SVC_GetDirPresence
    OEMGSM1xCardHandler_SVC_GetAdn
    OEMGSM1xCardHandler_SVC_SetAdn
    OEMGSM1xCardHandler_SVC_GetNumAdnRec
    OEMGSM1xCardHandler_SVC_GetAdnAlphaSize

INITIALIZATION AND SEQUENCING REQUIREMENTS
   OEMGSM1xCardHandler_SVC_Init needs to be called only once, and must be 
   called before using any of the OEMGSM1xCardHandlerSVC functions. 

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/03   dgb     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "OEMGSM1xCardHandler.h"

/*===========================================================================

                     DEFINITIONS AND CONSTANTS

===========================================================================*/
                                                   
/*===========================================================================

                     TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_Init

DESCRIPTION:  Initialize the OEMGSM1xCardHandlerSVC module.

PARAMETERS:
   None

RETURN VALUE:
   TRUE if successful, otherwise FALSE

COMMENTS:
   Needs to be called only once, and must be called before using
   any of the OEMGSM1xCardHandlerSVC functions.

SIDE EFFECTS:
   Accessing the SIM (caching the SMS message status bytes)

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_Init(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_IsSMSInit

DESCRIPTION: Returns whether SMS functionality has completed initialization 
             or not

PARAMETERS:
   None

RETURN VALUE:
   TRUE if initialized, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_IsSMSInit(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_IsSMSEnabled

DESCRIPTION: Returns whether SMS functionality is enabled in SIM Service Table

PARAMETERS:
   None

RETURN VALUE:
   TRUE if initialized, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_IsSMSEnabled(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetSMSStatus

DESCRIPTION: Returns status byte for an SMS slit in the SIM.

PARAMETERS:
   index:    slot to query
   *pStatus: pointer to variable to receive the status

RETURN VALUE:
   UIM_PASS if the SIM could be read and the index was valid, otherwise UIM_FAIL

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_GetSMSStatus(uint8 index, uint8 *pStatus);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetNumSMSSlots

DESCRIPTION: Returns number of SMS slots on the card

PARAMETERS:
   None

RETURN VALUE:
   number on SMS slots on the card

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_SVC_GetNumSMSSlots(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetSms

DESCRIPTION:  Retrieves the contents of the specified SIM SMS message

PARAMETERS:
   index: which SIM SMS message to read
   *pMsg: message content (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_GetSms(uint16 index, CardHandlerRawSMSType *pMsg);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSms

DESCRIPTION: Set an SIM SMS message to the specified contents

PARAMETERS:
   index: the SIM SMS message to set
   *pMsg: message content (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetSms(uint16 index, CardHandlerRawSMSType *pMsg);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ClearSms

DESCRIPTION: Delete the specified SIM SMS message

PARAMETERS:
   index: the SIM SMS message to delete

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ClearSms(uint16 index);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ClearAllSms

DESCRIPTION:  Delete all the SIM SMS messages

PARAMETERS:
   None

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ClearAllSms(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadSMSCAddress

DESCRIPTION:  Reads the SMS Center address

PARAMETERS:
   index:	The SMSC entry to read
   smscAddress:	The address of the SMS Centre

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred
            GSM1X_UIM_EMPTY if the field is empty

COMMENTS:
	Use index == 0 to get the DEFAULT SMSC

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadSMSCAddress(uint16 index, CardHandlerAddressType* pSmscAddress);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSMSCAddress

DESCRIPTION:  Set the SMS Center address

PARAMETERS:
   index:	    The SMSC entry to Set
   smscAddress: The address of the SMS Centre

RETURN VALUE:
   uint32:  UIM_PASS on success
			   UIM_FAIL if an error occurred

COMMENTS:
	Use index == 0 to get the DEFAULT SMSC

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetSMSCAddress(uint16 index, const CardHandlerAddressType* pSmscAddress);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadSMSS

DESCRIPTION:  Reads the EFsmss file

PARAMETERS:
   smssInfo:	The content of the EFsmss
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadSMSS(CardHandlerSMSSinfoType* pSmssInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSMSS

DESCRIPTION:  Writes the EFsmss file

PARAMETERS:
   pSmssInfo:	Pointer to the new content of the EFsmss

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetSMSS(CardHandlerSMSSinfoType* pSmssInfo);

/* ##################################################################################  

        New SIM support for Europa
		
 ##################################################################################  */

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadPLMNSelector

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadPLMNSelector(CardHandlerPLMNSelectorListType* pPlmnSelectorInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadPLMNSelector

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_WritePLMNSelector(CardHandlerPLMNSelectorListType* pPlmnSelectorInfo);




/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadAccessClass

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadAccessClass(CardHandlerAaccessClassBufType * pAccInfo);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadICCId

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadICCId(CardHandlerICCIdBufType * pICCId);



/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadIMSI

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadIMSI(CardHandlerImsiBufType * pImsiInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadMSISDN

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadMSISDN
(
    CardHandlerMSISDNBufType  *msisdn_buffer, 
    int                        record_number, 
    uint8                     *alphaLength
);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadAD

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadAD(CardHandlerADBufType * ad_buffer);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetDirPresence

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_GetDirPresence(CardHandlerDirPresenceBitMaskType * dirPresenceBitMask);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetAdn

DESCRIPTION:  Retrieves the contents of the specified SIM ADN record

PARAMETERS:
   index: which SIM ADN record to read
   *pAdn: record content (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_GetAdn
(
    uint16                index,
    CardHandlerRawADNType *pRec,
    uint8                 *alphaLength
);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetAdn

DESCRIPTION: Set an SIM ADN record to the specified contents

PARAMETERS:
   index: the SIM ADN record to set
   *pAdn: record content (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetAdn(uint16 index, CardHandlerRawADNType *pRec);

 
/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetNumAdnRec

DESCRIPTION:  

Returns the number of ADN records on the SIM.

OEMGSM1xCardHandler_SVC_Init must have been called prior to using 
this function.


PARAMETERS:
   None

RETURN VALUE:
   The number of ADN records on the SIM.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_SVC_GetNumAdnRec(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetAdnAlphaSize

DESCRIPTION:  

Returns the length in bytes of the alpha identifier in EFadn.

OEMGSM1xCardHandler_SVC_Init must have been called prior to using 
this function.


PARAMETERS:
   None

RETURN VALUE:
   The length of the alpha identifier in EFadn on the SIM.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_SVC_GetAdnAlphaSize(void);


#endif	/* OEM_GSM1XCARDHANDLER_SVC_H */
