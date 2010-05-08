#ifndef OEM_GSM1XCARDHANDLER_H
#define OEM_GSM1XCARDHANDLER_H

/*===========================================================================

                           << OEMGSM1xCardHandler.h >>


GENERAL DESCRIPTION
   AEE OEM SIM Interface header.  This file contains all of the function
   prototypes for routines that are required by the AEE to function with
   SIM.

EXTERNALIZED FUNCTIONS
    OEMGSM1xCardHandler_Init
    OEMGSM1xCardHandler_IsSMSInit
    OEMGSM1xCardHandler_IsSMSEnabled
    OEMGSM1xCardHandler_GetSMSStatus
    OEMGSM1xCardHandler_GetNumSMSSlots
    OEMGSM1xCardHandler_GetSms
    OEMGSM1xCardHandler_SetSms
    OEMGSM1xCardHandler_ClearSms
    OEMGSM1xCardHandler_ClearAllSms
    OEMGSM1xCardHandler_ReadSMSCAddress
    OEMGSM1xCardHandler_SetSMSCAddress
    OEMGSM1xCardHandler_ReadSMSS
    OEMGSM1xCardHandler_SetSMSS
    OEMGSM1xCardHandler_ReadForbiddenPLMN
    OEMGSM1xCardHandler_WriteForbiddenPLMN
    OEMGSM1xCardHandler_ReadPLMNSelector
    OEMGSM1xCardHandler_WritePLMNSelector
    OEMGSM1xCardHandler_ReadAccessClass
    OEMGSM1xCardHandler_ReadICCId
    OEMGSM1xCardHandler_ReadIMSI
    OEMGSM1xCardHandler_ReadMSISDN
    OEMGSM1xCardHandler_ReadAD
    OEMGSM1xCardHandler_GetDirPresence
    OEMGSM1xCardHandler_GetAdn
    OEMGSM1xCardHandler_SetAdn
    OEMGSM1xCardHandler_GetNumAdnRec
    OEMGSM1xCardHandler_GetAdnAlphaSize


INITIALIZATION AND SEQUENCING REQUIREMENTS
   OEMGSM1xCardHandler_Init needs to be called only once, and must be 
   called before using any of the OEMGSM1xCardHandler functions. 

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/02   eya     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "uim.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

////
// Artificial limitation on the maximum number of SIM SMS messages.
//
// NOTE!
//
// * Use caution when changing this value, nvruimsms.c assumes that:
//
//    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+4 < NV_SMS_DATA_SIZ
//
//  TODO - How does this value relate to the number of SMS messages that can
//         be stored on the SIM (a card-dependent number)?
//
#define MAX_NV_MSGS             31

// SMS message types.
#define OEMGSM1xCardHandler_MSG_UNUSED     0	//    unused (free)
#define OEMGSM1xCardHandler_MSG_READ       1	//    MT message, which was read
#define OEMGSM1xCardHandler_MSG_UNREAD     3	//    unread MT message
#define OEMGSM1xCardHandler_MSG_MO_SENT    5	//    MO message sent
#define OEMGSM1xCardHandler_MSG_MO_UNSENT  7	//    MO message unsent

#define OEMGSM1xCardHandler_NUM_SMS_TYPES  4

#define OEMGSM1xCardHandler_MAX_REPORT_LEN 29

#define OEMGSM1xCardHandler_MAX_PLMN_NUM   64   //    Not standartized, can be enlarged if needed.

#define OEMGSM1xCardHandler_MSISDN_MAX_LENGTH  255 //  Not standartized, can be enlarged if needed.

#define OEMGSM1xCardHandler_AD_MAX_LENGTH    4


#define OEMGSM1xCardHandler_MF_PRESENT          (0x0001)
#define OEMGSM1xCardHandler_CDMA_DF_PRESENT     (0x0002)
#define OEMGSM1xCardHandler_GSM_DF_PRESENT      (0x0004)

/* Add a new failure code, its value must NOT be equal to UIM_FAIL or to UIM_PASS !!! */
#define GSM1X_UIM_EMPTY 0x77

                                                   
/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

typedef struct
{
    uint8 length;  /* indicates 4-bit or 8-bit */
    uint8 ext;
    uint8 nature_of_address;
    uint8 numbering_plan;
    /* Each byte in the following array represents 2 digits, 
        each with 4-bit, of the SMSC address data:
    */
    uint8 digits[ 32 ];

} CardHandlerAddressType;

typedef PACKED struct {
    uint8 lastUsedTP_MR;
    uint8 memCapExcInd;  

} CardHandlerSMSSinfoType;

typedef struct {
   byte data[176];
} CardHandlerRawSMSType;


/////////////////////////////////////

typedef PACKED struct
{
	byte data[12];

} CardHandlerFPLMNListType;

typedef PACKED struct
{
	byte data[OEMGSM1xCardHandler_MAX_PLMN_NUM * 3]; //3 bytes for each PLMN

} CardHandlerPLMNSelectorListType;

typedef PACKED struct {
  
  byte imsi[9];

} CardHandlerImsiBufType;

typedef struct {
   
	byte data[OEMGSM1xCardHandler_MSISDN_MAX_LENGTH];

} CardHandlerMSISDNBufType;


typedef struct {
   
	byte data[OEMGSM1xCardHandler_AD_MAX_LENGTH];

} CardHandlerADBufType;


typedef PACKED struct {
  
	byte data[2];

} CardHandlerAaccessClassBufType;


typedef PACKED struct {
  
	byte data[10];

} CardHandlerICCIdBufType;

typedef uint16 CardHandlerDirPresenceBitMaskType;

typedef struct {
   byte data[255];
} CardHandlerRawADNType;


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_Init

DESCRIPTION:  Initialize the OEMGSM1xCardHandler module.

PARAMETERS:
   None

RETURN VALUE:
   TRUE if successful, otherwise FALSE

COMMENTS:
   Needs to be called only once, and must be called before using
   any of the OEMGSM1xCardHandler functions.

SIDE EFFECTS:
   Accessing the SIM (caching the SMS message status bytes)

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_Init(void);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_IsSMSInit

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
boolean OEMGSM1xCardHandler_IsSMSInit(void);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_IsSMSEnabled

DESCRIPTION: Returns whether SMS functionality is enabled in SIM Service Table

PARAMETERS:
   None

RETURN VALUE:
   TRUE if initialized, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_IsSMSEnabled(void);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetSMSStatus

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
uint32 OEMGSM1xCardHandler_GetSMSStatus(uint8 index, uint8 *pStatus);

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
uint8 OEMGSM1xCardHandler_GetNumSMSSlots(void);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetSms

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
uint32 OEMGSM1xCardHandler_GetSms(uint16 index, CardHandlerRawSMSType *pMsg);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSms

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
uint32 OEMGSM1xCardHandler_SetSms(uint16 index, CardHandlerRawSMSType *pMsg);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ClearSms

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
uint32 OEMGSM1xCardHandler_ClearSms(uint16 index);

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
uint32 OEMGSM1xCardHandler_ClearAllSms(void);

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
uint32 OEMGSM1xCardHandler_ReadSMSCAddress(uint16 index, CardHandlerAddressType* pSmscAddress);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSMSCAddress

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
uint32 OEMGSM1xCardHandler_SetSMSCAddress(uint16 index, const CardHandlerAddressType* pSmscAddress);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadSMSS

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
uint32 OEMGSM1xCardHandler_ReadSMSS(CardHandlerSMSSinfoType* pSmssInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetSMSS

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
uint32 OEMGSM1xCardHandler_SetSMSS(CardHandlerSMSSinfoType* pSmssInfo);

/* ##################################################################################  

        New SIM support for Europa
		
 ##################################################################################  */

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadForbiddenPLMN

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
uint32 OEMGSM1xCardHandler_ReadForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_WriteForbiddenPLMN

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
uint32 OEMGSM1xCardHandler_WriteForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadPLMNSelector

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
uint32 OEMGSM1xCardHandler_ReadPLMNSelector(CardHandlerPLMNSelectorListType* pPlmnSelectorInfo);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadPLMNSelector

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
uint32 OEMGSM1xCardHandler_WritePLMNSelector(CardHandlerPLMNSelectorListType* pPlmnSelectorInfo);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadAccessClass

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadAccessClass(CardHandlerAaccessClassBufType * pAccInfo);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadICCId

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadICCId(CardHandlerICCIdBufType * pICCId);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadIMSI

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadIMSI(CardHandlerImsiBufType * pImsiInfo);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadMSISDN

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadMSISDN
(
    CardHandlerMSISDNBufType  *msisdn_buffer, 
    int                        record_number, 
    uint8                     *alphaLength
);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_ReadAD

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_ReadAD(CardHandlerADBufType * ad_buffer);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetDirPresence

DESCRIPTION:  

PARAMETERS:
   None

RETURN VALUE:
   None

COMMENTS:
   

SIDE EFFECTS:
   

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_GetDirPresence(CardHandlerDirPresenceBitMaskType * dirPresenceBitMask);


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_GetAdn

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
uint32 OEMGSM1xCardHandler_GetAdn
(
    uint16                index,
    CardHandlerRawADNType *pRec,
    uint8                 *alphaLength
);

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SetAdn

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
uint32 OEMGSM1xCardHandler_SetAdn(uint16 index, CardHandlerRawADNType *pRec);

 
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
uint8 OEMGSM1xCardHandler_GetNumAdnRec(void);


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
uint8 OEMGSM1xCardHandler_GetAdnAlphaSize(void);


#endif	/* OEM_GSM1XCARDHANDLER_H */
