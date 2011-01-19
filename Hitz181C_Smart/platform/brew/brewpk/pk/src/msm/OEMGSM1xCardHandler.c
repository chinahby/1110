/*===========================================================================

FILE:
  OEMGSM1xCardHandler.c
                

GENERAL DESCRIPTION:
  OEM REFERENCE SMS SUPPORT ROUTINES

  This file contains a reference implementation of GSM SIM support.

EXTERNALIZED FUNCTIONS:
	See OEMGSM1xCardHandler.h

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   OEMGSM1xCardHandler_Init needs to be called only once, and must be called before using
   any of the OEMGSM1xCardHandler functions. 

Copyright © 2001-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMGSM1xCardHandler_SVC.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                            LOCAL/STATIC DATA 

===========================================================================*/


/*===========================================================================

                    LOCAL FUNCTION DECLARATIONs

===========================================================================*/

/*===========================================================================

                               LOCAL MACROS

===========================================================================*/


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_Init
 
DESCRIPTION:  Initialize the OEM SMS layer.  
 
PARAMETERS:
   None
 
RETURN VALUE:
   TRUE if successful, otherwise FALSE

COMMENTS:
   This only needs to be called once, and must be called before using
   any of the OEM SMS functions.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_Init(void)
{
   return OEMGSM1xCardHandler_SVC_Init();
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_IsSMSInit
=============================================================================*/
boolean OEMGSM1xCardHandler_IsSMSInit(void)
{
   return OEMGSM1xCardHandler_SVC_IsSMSInit();
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_IsSMSEnabled
=============================================================================*/
boolean OEMGSM1xCardHandler_IsSMSEnabled(void)
{
   return OEMGSM1xCardHandler_SVC_IsSMSEnabled();
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetSMSStatus

DESCRIPTION: Returns status byte for an SMS slot in the SIM.

PARAMETERS:
   index:    slot to query
   *pStatus: pointer to variable to receive the status

RETURN VALUE:
   UIM_PASS if the SIM could be read and the index was valid, otherwise UIM_FAIL

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_GetSMSStatus(uint8 index, uint8 *pStatus)
{
   return OEMGSM1xCardHandler_SVC_GetSMSStatus(index, pStatus);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetNumSMSSlots

DESCRIPTION: Returns number of SMS slots on the card

PARAMETERS:
   None

RETURN VALUE:
   number on SMS slots on the card

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_GetNumSMSSlots(void)
{
   return OEMGSM1xCardHandler_SVC_GetNumSMSSlots();
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetSms

DESCRIPTION:  Retrieves the contents of the specified SIM SMS message

PARAMETERS:
   index: SIM SMS message to read (zero based index)
   *pMsg: message content   (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
   Exactly MIN(simSMSMaxSize,sizeof(CardHandlerRawSMSType)) bytes will be read
   from the SIM. If the SMS message is shorter then the above figure, unused
   bytes will be flagged with 0xFF

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_GetSms(uint16 index, CardHandlerRawSMSType *pMsg)
{
   return OEMGSM1xCardHandler_SVC_GetSms(index, pMsg);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSms

DESCRIPTION: Set a SIM SMS message to the specified contents

PARAMETERS:
   index: the SIM SMS message to set (zero based index)
   *pMsg: message content   (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
    Exactly simSMSMaxSize bytes (176 Bytes) will be written to the SIM. The 
    function assumes that any un-used bytes trailing the actual content of 
    the SMS message within the pMsg will be marked with 0xFF

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SetSms(uint16 index, CardHandlerRawSMSType *pMsg)
{
   return OEMGSM1xCardHandler_SVC_SetSms(index, pMsg);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ClearSms

DESCRIPTION: Delete the specified SIM SMS message

PARAMETERS:
   index: the SIM SMS message to delete (zero based index)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
    An empty SMS message has 0 as a status and the rest is trailing 0xFFs

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ClearSms(uint16 index)
{
   return OEMGSM1xCardHandler_SVC_ClearSms(index);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ClearAllSms

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
uint32 OEMGSM1xCardHandler_ClearAllSms(void)
{
   return OEMGSM1xCardHandler_SVC_ClearAllSms();
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadSMSCAddress

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
uint32 OEMGSM1xCardHandler_ReadSMSCAddress(uint16 index, CardHandlerAddressType* pSmscAddress)
{
   return OEMGSM1xCardHandler_SVC_ReadSMSCAddress(index, pSmscAddress);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSMSCAddress

DESCRIPTION:  Set the SMS Center address

PARAMETERS:
   index:	The SMSC entry to Set
   smscAddress:	The address of the SMS Centre
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
	Use index == 0 to get the DEFAULT SMSC

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SetSMSCAddress(uint16 index, const CardHandlerAddressType* pSmscAddress)
{
   return OEMGSM1xCardHandler_SVC_SetSMSCAddress(index, pSmscAddress);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadSMSS

DESCRIPTION:  Reads the EFsmss file

PARAMETERS:
   pSmssInfo:	The content of the EFsmss

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadSMSS(CardHandlerSMSSinfoType* pSmssInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadSMSS(pSmssInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSMSS

DESCRIPTION:  Writes the EFsmss file

PARAMETERS:
   pSmssInfo:	The new content of the EFsmss
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SetSMSS(CardHandlerSMSSinfoType* pSmssInfo)
{
   return OEMGSM1xCardHandler_SVC_SetSMSS(pSmssInfo);
}

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadForbiddenPLMN

DESCRIPTION:  
   Read EFfplmn field from DFgsm.

PARAMETERS:
   pFplmnInfo pointer to the buffer to receive the contents of EFfplmn file.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN(pFplmnInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_WriteForbiddenPLMN

DESCRIPTION:  
   Read EFfplmn field from DFgsm.

PARAMETERS:
   pFplmnInfo pointer to the buffer to receive the contents of EFfplmn file.


RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_WriteForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo)
{
   return OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN(pFplmnInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadPLMNSelector

DESCRIPTION:  
   Retrieve the contents of EFplmnsel file in DFgsm.

PARAMETERS:
   pPlmnSelectorInfo - pointer to the memory buffer to receive
   the contents of EFplmnsel field.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadPLMNSelector(CardHandlerPLMNSelectorListType * pPlmnSelectorInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadPLMNSelector(pPlmnSelectorInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_WritePLMNSelector

DESCRIPTION:  
   Update the contents of EFplmnsel file in DFgsm with the specified
   buffer.

PARAMETERS:
   pPlmnSelectorInfo - pointer to the memory buffer that contains
   new value for EFplmnsel.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:
								   forbidden
=============================================================================*/
uint32 OEMGSM1xCardHandler_WritePLMNSelector(CardHandlerPLMNSelectorListType * pPlmnSelectorInfo)
{
   return OEMGSM1xCardHandler_SVC_WritePLMNSelector(pPlmnSelectorInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadAccessClass

DESCRIPTION:  
   Retrieve the contents of EFaccolc file in DFgsm. 

PARAMETERS:
   Pointer to the buffer to receive the contents of EFaccolc.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadAccessClass(CardHandlerAaccessClassBufType * pAccInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadAccessClass(pAccInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadICCId

DESCRIPTION:  
   Retrieve the value of EFiccid from DFtelecom.

PARAMETERS:
   pICCId points to the memory location to receive the value
   of EFiccid.

RETURN VALUE:
   Completion status.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadICCId( CardHandlerICCIdBufType  *pICCId)
{
   return OEMGSM1xCardHandler_SVC_ReadICCId(pICCId);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadIMSI

DESCRIPTION:  
   Retrieve the contents of EFimsi file from DFgsm.

PARAMETERS:
   pImsiInfo points to the memory location to receive the contents
   of EFimsi file.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadIMSI(CardHandlerImsiBufType * pImsiInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadIMSI(pImsiInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadMSISDN

DESCRIPTION:  
   Retrieve the value of EFmsisdn file in DFgsm.

PARAMETERS:
   msisdn_buffer points to the memory location to receive the specified
                 MSISDN record
   index         specifies the index of MSISDN record to be retrieved.
   alphaLength   points to the memory location to receive the length
                 of "alpha-identitier".  This information is needed
                 in order to parse the rest of MSIDSN, 
                 since "alpha-identitfier" is a variable-length field
                 that is located in the beginning of MSISDN record.
   
RETURN VALUE:
   Completion status.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadMSISDN
(
    CardHandlerMSISDNBufType  *msisdn_buffer, 
    int                        record_number, 
    uint8                     *alphaLength
)
{
   return OEMGSM1xCardHandler_SVC_ReadMSISDN(msisdn_buffer, record_number, alphaLength);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadAD

DESCRIPTION:  
   Retrieve the contents of the EFad file from DFgsm.  

PARAMETERS:
   pADInfo points to the memory location to receive to contents
   of EFad file.

RETURN VALUE:
   Completion status

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadAD(CardHandlerADBufType * pADInfo)
{
   return OEMGSM1xCardHandler_SVC_ReadAD(pADInfo);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetDirPresence

DESCRIPTION:  
   Retrieve the presence of GSM and CDMA directories on the supplied
   identity card. 

PARAMETERS:
   dirPresenceBitMask points the memory location to receive the bitmask
   for the directories that are present on the identity card.

RETURN VALUE:
   Completion status.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_GetDirPresence
(
    CardHandlerDirPresenceBitMaskType  *dirPresenceBitMask
)
{
    return OEMGSM1xCardHandler_SVC_GetDirPresence(dirPresenceBitMask);
}

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetAdn

DESCRIPTION:  Retrieves the contents of the specified SIM ADN record

PARAMETERS:
   index: SIM ADN record to read (zero based index)
   *pAdn: record content   (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
   Exactly MIN(simADNMaxSize,sizeof(CardHandlerRawADNType)) bytes will be read
   from the SIM. If the ADN record is shorter then the above figure, unused
   bytes will be flagged with 0xFF.

   On first access (Initialization) this function should be called with index == 0
   in order to read a sim entry and find out how many entries are in the SIM.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_GetAdn
(
   uint16                 index,
   CardHandlerRawADNType *pAdn,
   uint8                 *alphaLength
)
{
    return OEMGSM1xCardHandler_SVC_GetAdn(index, pAdn, alphaLength);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetAdn

DESCRIPTION: Set a SIM ADN record to the specified contents

PARAMETERS:
   index: the SIM ADN record to set (zero based index)
   *pAdn: record content   (status byte + data)

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
    Exactly simADNMaxSize bytes (176 Bytes) will be written to the SIM. The 
    function assumes that any un-used bytes trailing the actual content of 
    the ADN record within the pAdn will be marked with 0xFF

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SetAdn(uint16 index, CardHandlerRawADNType *pAdn)
{
    return OEMGSM1xCardHandler_SVC_SetAdn(index, pAdn);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetNumAdnRec

DESCRIPTION:  

Returns the number of ADN records on the SIM.

OEMGSM1xCardHandler_Init must have been called prior to using 
this function.


PARAMETERS:
   None

RETURN VALUE:
   The number of ADN records on the SIM.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_GetNumAdnRec()
{
    return OEMGSM1xCardHandler_SVC_GetNumAdnRec();
}

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetAdnAlphaSize

DESCRIPTION:  

Returns the length in bytes of the alpha identifier in EFadn.

OEMGSM1xCardHandler_Init must have been called prior to using 
this function.


PARAMETERS:
   None

RETURN VALUE:
   The length of the alpha identifier in EFadn on the SIM.

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint8 OEMGSM1xCardHandler_GetAdnAlphaSize()
{
    return OEMGSM1xCardHandler_SVC_GetAdnAlphaSize();
}
#endif /* FEATURE_GSM1x */

