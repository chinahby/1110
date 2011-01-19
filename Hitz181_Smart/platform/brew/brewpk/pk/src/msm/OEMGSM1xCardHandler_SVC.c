/*===========================================================================

FILE:
  OEMGSM1xCardHandler_SVC.c
                

GENERAL DESCRIPTION:
  OEM REFERENCE SMS SUPPORT ROUTINES

  This file contains a reference implementation of GSM SIM support.

EXTERNALIZED FUNCTIONS:
	See OEMGSM1xCardHandler_SVC.h

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   OEMGSM1xCardHandler_SVC_Init needs to be called only once, and must be called
   before using any of the OEMGSM1xCardHandlerSVC functions. 

Copyright © 2001-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim.h"		            // For uim_cmd_type
#include "msg.h"                    // For MSG_XXXXXX
#include "rex.h"                    // For signals & TCBs

#include "task.h"                   // For other processes TCBs

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/
#if defined(FEATURE_GSM1x)
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"

#include "AEEGSMSMS.h" // For GSMSMS_TON* and GSMSMS_NPI* definitions

#include "OEMGSM1xCardHandler_SVC.h"
#include "OEMGSM1xProv.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/* Size of SIM Service Table to read off the card */
#define OEMGSM1xCardHandler_SERVICETABLE_SIZE    (9)

/* Service Table Flags */
#define OEMGSM1xCardHandler_SERVICETABLE_SMS_STORAGE   0xC0
#define OEMGSM1xCardHandler_SERVICETABLE_SMS_PARAMS    0xC0
#define OEMGSM1xCardHandler_SERVICETABLE_SMS_REPORT    0x30
#define OEMGSM1xCardHandler_SERVICETABLE_MSISDN  0x03
#define OEMGSM1xCardHandler_SERVICETABLE_PLMNSEL 0x30
#define OEMGSM1xCardHandler_SERVICETABLE_ADN_STORAGE   0x0C

/* The following must be synchronized with nvruimi.h */
#define OEMGSM1xCardHandler_EF_BUF_SIZE  180

//Artificial limitation of the number of MSISDN records.
//The card handler only access one at a time, so it can be easily enlarged.
#define OEMGSM1xCardHandler_MAX_MSISDN_RECORDS 20  

#define OEMGSM1xCardHandler_MAX_ADN_BUF_SIZE 255

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

/* Defining chv status(information) for internal use */
typedef struct {
  boolean	chv1_status;       /* is CHV1 required?  */
  byte		chv1cnt_remain;    /* Remaining CHV1 count */
  byte		uchv1cnt_remain;   /* Remaining Unblock CHV1 count */
  byte		chv2cnt_remain;    /* Remaining CHV2 count */
  byte		uchv2cnt_remain;   /* Remaining Unblock CHV2 count */
} uim_chv_status_type;

typedef struct {
  uint8 SMSCount;       // maximum number of SMS messages;
  uint8 SMSMaxSize;     // maximum length of an SMS message
  uint8 PLMNSelMaxSize; // maximum length of the PLMN Select file
  uint8 MSISDNCount;    // maximum number of MSISDN records
  uint8 MSISDNMaxSize;  // maximum length of an MSISDN record
  uint8 ADNCount;
  uint8 ADNMaxSize;
} sim_info;

/*===========================================================================

                            LOCAL/STATIC DATA 

===========================================================================*/

    /* tracking for staggared SIM initialization */
LOCAL uint8                     lastInitSMS = 0;
LOCAL boolean                   SMSInitFlag = FALSE;

    /* Structure holding the SIM information */
LOCAL sim_info                  gSIMInfo = {
                                   MAX_NV_MSGS,                           // SMSCount
                                   OEMGSM1xCardHandler_EF_BUF_SIZE,       // SMSMaxSize
                                   OEMGSM1xCardHandler_EF_BUF_SIZE,       // PLMNSelMaxSize
                                   OEMGSM1xCardHandler_MAX_MSISDN_RECORDS,// MSISDNCount
                                   OEMGSM1xCardHandler_EF_BUF_SIZE,       // MSISDNMaxSize
                                   0,    // No Records.
                                         // This value is read from the SIM after the first
                                         // SIM access and updated after each SIM access.
                                   OEMGSM1xCardHandler_MAX_ADN_BUF_SIZE
                                };

    /* Array holding the status bytes of all the SMS messages (on the SIM) */
LOCAL byte                      simSMSStatus[MAX_NV_MSGS];

    /* SIM service table */
LOCAL byte                      simServiceTable[OEMGSM1xCardHandler_SERVICETABLE_SIZE];

LOCAL rex_tcb_type             *callingThreadTCB = NULL;

LOCAL uim_cmd_type              uimCmd;
LOCAL uim_cmd_type              uimInitCmd;
LOCAL uim_rpt_type              uimRprt;
LOCAL CardHandlerRawSMSType     initMsg;

LOCAL AEECallback               gTimerCb;
LOCAL AEECallback               gSwitchToBrewCb;


/*===========================================================================

                    LOCAL FUNCTION DECLARATIONs

===========================================================================*/

    /* Read the SIM Service Table */
LOCAL boolean   OEMGSM1xCardHandler_SVC_Init_SST(void);

    /* Read the status bytes of all the SMS messages */
LOCAL boolean   OEMGSM1xCardHandler_SVC_Init_SMSStatusCache(void);

    /* Blocking function until a signal from UIM arrives */
LOCAL void      OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady(void);

    /* Call back function, for use by the UIM */
LOCAL void      OEMGSM1xCardHandler_SVC_CBreportFN( uim_rpt_type * report);

    /* Call back function, for use by the UIM */
LOCAL void OEMGSM1xCardHandler_SVC_CBInitFN( uim_rpt_type * report);

    /* Performs an access operation on the SIM */
LOCAL uint32    OEMGSM1xCardHandler_SVC_SimAccess(
                    uim_access_type accessType,
                    uim_items_enum_type efToAccess,
                    word offset,
                    byte* payload, 
                    uint8* payloadLength,
                    sim_info *pSIMInfo);

    /* Select the Master-File (MF) on the SIM */
LOCAL uint32    OEMGSM1xCardHandler_SVC_SelectMF(void);

#ifdef CODE_SECTION_IS_NOT_USED
    /* Returns the CHV status */
LOCAL uim_chv_status_type *
                OEMGSM1xCardHandlerSVCGetCHVstatus(void);
#endif

LOCAL void OEMGSM1xCardHandler_SVC_NextSMSCallback(void *data);

LOCAL void OEMGSM1xCardHandler_SVC_TimerCallback(void *data);

LOCAL void OEMGSM1xCardHandler_SVC_InitSim(void);


/*===========================================================================

                               LOCAL MACROS

===========================================================================*/

/*==========================================================================

MACRO SERVICETABLE_IS_SMS_STORAGE_ENABLED


DESCRIPTION
  Determines if SMS is active and available according to EFsst

PARAMETERS
  x:  The byte of the SIM Service Table containing the SMS flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SMS services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_SMS_STORAGE_ENABLED( x ) \
  ( OEMGSM1xCardHandler_SERVICETABLE_SMS_STORAGE == ((x[0]) & OEMGSM1xCardHandler_SERVICETABLE_SMS_STORAGE ) )


/*==========================================================================
MACRO SERVICETABLE_IS_ADN_ENABLED


DESCRIPTION
  Determines if ADN is active and available according to EFsst

PARAMETERS
  x:  The byte of the SIM Service Table containing the ADN flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if ADN services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_ADN_STORAGE_ENABLED( x ) \
  ( OEMGSM1xCardHandler_SERVICETABLE_ADN_STORAGE == ((x[0]) & OEMGSM1xCardHandler_SERVICETABLE_ADN_STORAGE ) )


/*==========================================================================
MACRO SERVICETABLE_IS_SMS_PARAM_ENABLED


DESCRIPTION
  Determines if SMSP is active and available according to EFsst

PARAMETERS
  x:  The array of the SIM Service Table containing the SMS flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SMSP services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_SMS_PARAM_ENABLED( x ) \
  ( OEMGSM1xCardHandler_SERVICETABLE_SMS_PARAMS == ( (x[2]) & OEMGSM1xCardHandler_SERVICETABLE_SMS_PARAMS ) )


/*==========================================================================
MACRO SERVICETABLE_IS_SMS_REPORT_ENABLED


DESCRIPTION
  Determines if SMSR is active and available according to EFsst

PARAMETERS
  x   The array of the SIM Service Table containing the SMS flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SMSR services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_SMS_REPORT_ENABLED( x ) \
  (OEMGSM1xCardHandler_SERVICETABLE_SMS_REPORT == ((x[8]) & OEMGSM1xCardHandler_SERVICETABLE_SMS_REPORT) )


/*==========================================================================

MACRO SERVICETABLE_IS_MSISDN_ENABLED


DESCRIPTION
  Determines if MSISDN is active and available according to EFsst

PARAMETERS
  x:  The byte of the SIM Service Table containing the MSISDN flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if MSISDN services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_MSISDN_ENABLED( x ) \
  ( OEMGSM1xCardHandler_SERVICETABLE_MSISDN == ((x[2]) & OEMGSM1xCardHandler_SERVICETABLE_MSISDN) )


/*==========================================================================

MACRO SERVICETABLE_IS_PLMNSEL_ENABLED


DESCRIPTION
  Determines if PLMNSEL is active and available according to EFsst

PARAMETERS
  x   The byte of the SIM Service Table containing the PLMNSEL flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if PLMNSEL services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define SERVICETABLE_IS_PLMNSEL_ENABLED( x ) \
  ( OEMGSM1xCardHandler_SERVICETABLE_PLMNSEL == ((x[1]) & OEMGSM1xCardHandler_SERVICETABLE_PLMNSEL) )


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_Init
 
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
boolean OEMGSM1xCardHandler_SVC_Init(void)
{   
    /* Make sure this function is not called more than once after successful init */
    static boolean IsFirstTime = TRUE;

    CardHandlerRawADNType pAdn;
    uint8                 alphaLength;
    CardHandlerPLMNSelectorListType pPLMNSelectorList;

    if (IsFirstTime == FALSE)
    {
        return TRUE;
    }

    // TODO verify this is BREW/ui thread
    callingThreadTCB = rex_self();

    /* Select the MF */
    if (OEMGSM1xCardHandler_SVC_SelectMF() != UIM_PASS)
    {
       MSG_ERROR("OEMGSM1xCardHandler_SVC_SelectMF failed",0,0,0);
       return FALSE;
    }

    // Currently, the CHV's validation status is not available,
    // so just skip the CHV check, and if the card is not validated 
    // the next card access will fail

    // /* Check the current CHV status */
    //if ( OEMGSM1xCardHandlerSVCGetCHVstatus()->chv1_status != 0 )
    //{   /* CHV1 is ACTIVE! */
    //    MSG_ERROR("CHV1 is active!",0,0,0);
    //    return FALSE;
    //}

    /* Read and save the SIM Service Table.
       This might fail if the card does not have a GSM directory.*/
    if (OEMGSM1xCardHandler_SVC_Init_SST() != UIM_PASS)
    {
        MSG_ERROR("OEMGSM1xCardHandler_SVC_Init_SST failed",0,0,0);

		/* If the card is CHV1 enabled, and was not verified the SST init will fail! */
		/* for now return TRUE, in order to allow future init calls to actually 
		   initialize the code, if the card is CHV1 enabled */
		return TRUE;
    }

    // Initialize the ADN information.
    // The Index value must be zero in order to successfully read a SIM entry
    // when the max number of entries is not yet known.
    // This might fail if the card does not have a GSM directory.
    if(UIM_PASS != OEMGSM1xCardHandler_SVC_GetAdn(0, &pAdn, &alphaLength))
    {
        MSG_ERROR("Failed to initialize EFadn!",0,0,0);
    }

    // Initialize the PLMN information.
    // This might fail if the card does not have a GSM directory.
    if(UIM_PASS != OEMGSM1xCardHandler_SVC_ReadPLMNSelector(&pPLMNSelectorList))
    {
        MSG_ERROR("Failed to initialize EFplmn!",0,0,0);
    }

    // Initialize the SMS Status Cache 
    //
    // SMS may not be enabled on this SIM so don't fail the init
    // if this fails
    (void)OEMGSM1xCardHandler_SVC_Init_SMSStatusCache();

    MSG_HIGH("OEMGSM1xCardHandlerSVC module initialized",0,0,0);

    IsFirstTime = FALSE; // do not allow Init to be performed again
    return TRUE;

    // Rum time validity check
    if ( (GSM1X_UIM_EMPTY == UIM_PASS) || (GSM1X_UIM_EMPTY == UIM_FAIL) )
    {
        MSG_FATAL("Bad GSM1X_UIM_EMPTY value!",0,0,0);
    }

}   /* OEMGSM1xCardHandler_SVC_Init */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_IsSMSInit
 
DESCRIPTION:  Check if SMS has been intialized.  
 
PARAMETERS:
   None
 
RETURN VALUE:
   TRUE if initialized, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_IsSMSInit(void)
{
   return SMSInitFlag;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_IsSMSEnabled
 
DESCRIPTION:  Check if SMS use is enabled.  
 
PARAMETERS:
   None
 
RETURN VALUE:
   TRUE if enabled, otherwise FALSE

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_IsSMSEnabled(void)
{
   return (SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable)) ? TRUE : FALSE;
}


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
uint32 OEMGSM1xCardHandler_SVC_GetSMSStatus(uint8 index, uint8 *pStatus)
{
   if((SMSInitFlag == TRUE) && (index < gSIMInfo.SMSCount))
   {
      COPY_TO_USER(pStatus, &simSMSStatus[index], sizeof(*pStatus));
      return UIM_PASS;
   }
   else
   {
      return UIM_FAIL;
   }
}


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
uint8 OEMGSM1xCardHandler_SVC_GetNumSMSSlots(void)
{
   return gSIMInfo.SMSCount;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetSms

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
uint32 OEMGSM1xCardHandler_SVC_GetSms(uint16 index, CardHandlerRawSMSType *pMsg)
{
    uim_rpt_status  status;
    byte efBuffer[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8           payloadLength;

    status = UIM_FAIL;
    payloadLength = gSIMInfo.SMSMaxSize;

    if ( index >=  gSIMInfo.SMSCount ) 
    {
        MSG_ERROR("Received an out of range SMS index (%d)!",index,0,0);
        return UIM_FAIL;
    }

    /* Check if SMS is available in the SIM Service Table */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   /* Access The SMS File */

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,     /* Operation (Read/Write) */
                                                   UIM_TELECOM_SMS,/* EF to access */
                                                   index + 1,      /* Record # to access */
                                                   efBuffer,       /* Where to put the data */
                                                   &payloadLength, /* Size of data buff (in & out) */
                                                   &gSIMInfo       /* SIM Info struct */
                                                   );
        if ( status != UIM_PASS )
        {   /* SIM access failed */
            MSG_ERROR("Could not read from EFsms file!",0,0,0);
        } 
        else 
        {
            COPY_TO_USER(pMsg->data, efBuffer, MIN(payloadLength,sizeof(pMsg->data)));
        }
    }
    else
    {
        MSG_ERROR("SMS SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_GetSms */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSms

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
uint32 OEMGSM1xCardHandler_SVC_SetSms(uint16 index, CardHandlerRawSMSType *pMsg)
{
    uim_rpt_status status = UIM_FAIL;
        
    if ( index >= gSIMInfo.SMSCount ) 
    {
        MSG_ERROR("Received an out of range SMS index (%d)!",index,0,0);
        return UIM_FAIL;
    }
  
    /* Check if SMS is available in the SIM Service Table */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   
        CardHandlerRawSMSType rawBuffer;
        uint8 sizeOf = sizeof(pMsg->data);

        if (gSIMInfo.SMSMaxSize != sizeOf)
        {
            MSG_ERROR("Invalid size for pMsg->data",0,0,0);
            return UIM_FAIL;
        }

        COPY_FROM_USER(&rawBuffer.data, pMsg->data, sizeOf);

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,    /* Operation (Read/Write) */
                                                   UIM_TELECOM_SMS,/* EF to access */
                                                   index + 1,      /* Record # to access */
                                                   rawBuffer.data, /* Buffer to write */
                                                   &sizeOf,        /* Size of data buff (in & out) */
                                                   &gSIMInfo       /* SIM Info struct */
                                                   );
        if ( status != UIM_PASS )
        {   /* SIM access failed */
            MSG_ERROR("Could not write to EFsms file!",0,0,0);
        } 
        else
        {   /* Update the SMS Status Cache */
            simSMSStatus[index] = rawBuffer.data[0];
        }
    }
    else
    {
        MSG_ERROR("SMS SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_SetSms */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ClearSms

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
uint32 OEMGSM1xCardHandler_SVC_ClearSms(uint16 index)
{
    uim_rpt_status    status;
    byte payload[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8             payloadLength;

    status        = UIM_FAIL;
    payloadLength = gSIMInfo.SMSMaxSize;

    if ( index >= gSIMInfo.SMSCount ) 
    {
        MSG_ERROR("Received an out of range SMS index (%d)!",index,0,0);
        return UIM_FAIL;
    }
  
    /* Check if SMS is available in the SIM Service Table */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   
        /* Create the empty SMS message */
        payload[0] = 0x00;
        memset(payload + 1, 0xFF, gSIMInfo.SMSMaxSize - 1 );

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,    /* Operation (Read/Write) */
                                                   UIM_TELECOM_SMS,/* EF to access */
                                                   index + 1,      /* Record # to access */
                                                   payload,        /* Buffer to write */
                                                   &payloadLength, /* Size of data buff (in & out) */
                                                   &gSIMInfo       /* SIM Info struct */
                                                   );
        if ( status != UIM_PASS )
        {   /* SIM access failed */
            MSG_ERROR("Could not write to EFsms file!",0,0,0);
        }
        else
        {   /* Update the SMS Status Cache */
            simSMSStatus[index] = 0;
        }
    }
    else
    {
        MSG_ERROR("SMS SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_ClearSms */


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
uint32 OEMGSM1xCardHandler_SVC_ClearAllSms(void)
{
    int            msgInd;
    uim_rpt_status status;

    status = UIM_FAIL;
    
    for ( msgInd = 0 ; msgInd < gSIMInfo.SMSCount ; msgInd++ )
    {
        status = OEMGSM1xCardHandler_SVC_ClearSms(msgInd);
        if ( status != UIM_PASS )
        {
            MSG_ERROR("Problem erasing SMS message # %d",msgInd,0,0);
            break;
        }
    }
    return status;
    
}   /* OEMGSM1xCardHandler_SVC_ClearAllSms */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadSMSCAddress

DESCRIPTION:  Reads the SMS Center address

PARAMETERS:
   index: The SMSC entry to read
   smscAddress:	The address of the SMS Centre

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred (field can not be read)
            GSM1X_UIM_EMPTY if an error occurred (field is empty)

COMMENTS:
	Use index == 0 to get the DEFAULT SMSC

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadSMSCAddress(uint16 index, CardHandlerAddressType* pSmscAddress)
{
    uim_rpt_status   status;
    byte payload[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8 payloadLength = sizeof(payload);
    static CardHandlerAddressType smscAddr;
    byte* ptr;
    int i;

    status        = UIM_FAIL;
    payloadLength = sizeof(payload);

    /* Check if SMSP is available in the SIM Service Table */
    if ( SERVICETABLE_IS_SMS_PARAM_ENABLED(simServiceTable) )
    {   /* Read The SMS File */
        status = 
             OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                               UIM_TELECOM_SMSP,/* EF to access */
                                               index + 1,       /* Record # to access */
                                               payload,         /* Buffer to read */
                                               &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
                                               );
        if ( status == UIM_PASS )
        {   
            ptr = payload + payloadLength - 15;
            // check in "Parameter Indicators" field if the 
            // "TS-Service Centre Address" field is present.
            // and the smsc address is not empty
            MSG_HIGH("SCADDR: ptr[0] = %x ptr[1] = %x (%x)",ptr[0],ptr[1],*(ptr-13));
            if ( ((*(ptr-13) & 0x02) == 0) && (ptr[0] != 0xFF ))
            {
                smscAddr.length = ptr[0];
                smscAddr.ext = (ptr[1] & 0x80) >> 7;
                smscAddr.nature_of_address = (ptr[1] & 0x70) >> 4;
                smscAddr.numbering_plan = (ptr[1] & 0x0f);

                for (i = 0; i < ptr[0]-1; i++)
                {
                    smscAddr.digits[i] = ptr[2+i];
                }

                COPY_TO_USER(pSmscAddress, &smscAddr, sizeof(*pSmscAddress));
            }
            else
            {   /* No centre address / address is empty */
                return GSM1X_UIM_EMPTY;
            }
        }
        else
        {   /* SIM access failed */
            MSG_ERROR("Could not read from EFsmsp!",0,0,0);
        }
    }
    else
    {   /* SMSP not in service table */
        MSG_ERROR("SMSP SIM access not allowed in service table!",0,0,0);
    }

    return status;

}   /* OEMGSM1xCardHandler_SVC_ReadSMSCAddress */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSMSCAddress

DESCRIPTION:  Set the SMS Center address

PARAMETERS:
   index:  The SMSC entry to Set
   smscAddress: The address of the SMS Centre

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:
	Use index == 0 to get the DEFAULT SMSC

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetSMSCAddress(uint16 index, const CardHandlerAddressType* pSmscAddress)
{
    uim_rpt_status  status;
    byte payload[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8           payloadLength;
    static CardHandlerAddressType smscAddr;
    byte* ptr;
    byte* parameter_indicator_ptr;
    int i;

    status        = UIM_FAIL;
    payloadLength = sizeof(payload);
    
    /* Validate input parameters - refer to GSM04.11 section 8.2.5.1 */
    if(pSmscAddress == NULL)
    {
        MSG_ERROR("Null input pointer to OEMGSM1xCardHandler_SVC_SetSMSCAddress!",0,0,0);
        return status;
    }

    COPY_FROM_USER(&smscAddr, pSmscAddress, sizeof(smscAddr));
     
	if( (smscAddr.length            > 11) ||
        (smscAddr.length            < 2 ) ||
		(smscAddr.ext              != 1 ) ||
		(smscAddr.nature_of_address > 7 ) ||
		(smscAddr.numbering_plan    > 15)   )
		
	{
		MSG_ERROR("Bad input parameter to OEMGSM1xCardHandler_SVC_SetSMSCAddress!",0,0,0);
        return status;
    }

    /* Check if SMSP is available in the SIM Service Table */
    if ( SERVICETABLE_IS_SMS_PARAM_ENABLED(simServiceTable) )
    {   /* Read The SMS File */
        status = 
             OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                               UIM_TELECOM_SMSP,/* EF to access */
                                               index + 1,       /* Record # to access */
                                               payload,         /* Buffer to read */
                                               &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
                                               );
        if ( status != UIM_PASS )
        {   
           MSG_ERROR("OEMGSM1xCardHandler_SVC_SimAccess failed",0,0,0);
           return status;
        }
    }
    else
    {
       MSG_ERROR("SMSP SIM access not allowed in service table!",0,0,0);
       return status;
    }

    ptr = payload + payloadLength - 15;
    parameter_indicator_ptr = payload + payloadLength - 28; 

    // zero the default SC address
    memset(ptr,0,12);

    ptr[0] = smscAddr.length;
    ptr[1] = smscAddr.ext << 7;
    ptr[1] |= smscAddr.nature_of_address << 4;
    ptr[1] |= smscAddr.numbering_plan;

    parameter_indicator_ptr[0] &= 0xFD;  /* Indicate that SC addr is there!  */

    for (i=0; i < smscAddr.length -1; i++)
    {
       ptr[2+i] = smscAddr.digits[i];
    }

    return OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,      /* Operation (Read/Write) */
                                             UIM_TELECOM_SMSP,/* EF to access */
                                             index + 1,       /* Record # to access */
                                             payload,         /* Buffer to read */
                                             &payloadLength,  /* Size of data buff (in & out) */
                                             &gSIMInfo        /* SIM Info struct */
                                             );

}   /* OEMGSM1xCardHandler_SVC_ReadSMSCAddress */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadSMSS

DESCRIPTION:  Reads the EFsmss file

PARAMETERS:
   pSmssInfo: The content of the EFsmss

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadSMSS(CardHandlerSMSSinfoType* pSmssInfo)
{
    uim_rpt_status  status;
    byte           *payload;
    uint8           payloadLength;

    CardHandlerSMSSinfoType smssInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&smssInfo;
    payloadLength = sizeof(smssInfo);

    /* Check the Service Table */
    /* EFsms & EFsmss are both either existing or not */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   /* Read The SMS File */
        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                                   UIM_TELECOM_SMSS,/* EF to access */
                                                   0,               /* offset to access */
                                                   payload,         /* Buffer to write */
                                                   &payloadLength,  /* Size of data buff (in & out) */
                                                   NULL             /* SIM Info struct not required */
                                                   );
        if ( status == UIM_PASS )
        {
            COPY_TO_USER((byte*)pSmssInfo, (byte*)&smssInfo, sizeof(*pSmssInfo));
        }
        else
        { /* SIM access failed */
            MSG_ERROR("Could not read from EFsmss!",0,0,0);
        }
    }
    else
    {   /* EFsmss not allowed in service table */
        MSG_ERROR("SMSS SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_ReadSMSS */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetSMSS

DESCRIPTION:  Writes the EFsmss file

PARAMETERS:
   pSmssInfo: The new content of the EFsmss

RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_SetSMSS(CardHandlerSMSSinfoType* pSmssInfo)
{
    uim_rpt_status  status;
    byte           *payload;
    uint8           payloadLength;

    CardHandlerSMSSinfoType smssInfo;

    status        = UIM_FAIL;
    payload       = (byte*)pSmssInfo;
    payloadLength = sizeof(*pSmssInfo);

    /* Check the Service Table */
    /* EFsms & EFsmss are both either existing or not */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   
        COPY_FROM_USER((byte*)&smssInfo, (byte*)pSmssInfo, sizeof(smssInfo));

        /* Read The SMS File */
        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,     /* Operation (Read/Write) */
                                                   UIM_TELECOM_SMSS,/* EF to access */
                                                   0,               /* offset to access */
                                                   payload,         /* Buffer to write */
                                                   &payloadLength,  /* Size of data buff (in & out) */
                                                   NULL             /* SIM Info struct not required */
                                                   );
        if ( status != UIM_PASS )
        {   /* SIM access failed */
            MSG_ERROR("Could not read from EFsmss!",0,0,0);
        }
    }
    else
    {   /* EFsmss not allowed in service table */
        MSG_ERROR("SMSS SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_SetSMSS */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_Init_SST
 
DESCRIPTION:  
    Read the content of the EFsst file from DFgsm and save it for future use.
 
PARAMETERS:
   None
 
RETURN VALUE:
   boolean : TRUE - Success
             FALSE - Failure

=============================================================================*/
boolean OEMGSM1xCardHandler_SVC_Init_SST(void)
{
    uim_rpt_status status;
    uint8          payloadLength;

    payloadLength = sizeof(simServiceTable);

    memset(simServiceTable, 0 , sizeof(simServiceTable));

    status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,     /* Operation (Read/Write) */
                                               UIM_GSM_SST,    /* EF to access */
                                               0,              /* offset to access */
                                               simServiceTable,/* Buffer to read */
                                               &payloadLength, /* Size of data buff (in & out) */
                                               NULL            /* SIM Info struct not required */
                                               );

    if ( status != UIM_PASS)
    {
        MSG_ERROR("Could not read EFsst file!",0,0,0);
    }

    return (status == UIM_PASS);

}   /* OEMGSM1xCardHandler_SVC_Init_SST */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_Init_SMSStatusCache
 
DESCRIPTION:  Reads all SMS messages from the sim. After reading a message
    will parse its status header (1st byte) and store it in a cache.

PARAMETERS:
   None
 
RETURN VALUE:
   boolean : TRUE - Success
             FALSE - Failure

COMMENTS:
    Should be called only once from withing OEMGSM1xCardHandler_SVC_Init.
    Should be updated after every call to SetSMS / ClearSMS 

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL boolean OEMGSM1xCardHandler_SVC_Init_SMSStatusCache(void)
{
    uim_rpt_status status;
    uint8 i;

    lastInitSMS = 0;
    status = UIM_FAIL;

    for (i=0; i<MAX_NV_MSGS; i++)
    {
       simSMSStatus[i] = 0;
    }

    /* Check the Service Table */
    if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
    {   /* Read The SMS File */

        // This call should set the correct value for gSIMInfo.SMSCount
        status = OEMGSM1xCardHandler_SVC_GetSms(lastInitSMS,&initMsg);

        if ( status == UIM_PASS)
        {
            simSMSStatus[0] = initMsg.data[0];
            lastInitSMS++;

            // Set the timer for 10 seconds            
            CALLBACK_Init(
               &gTimerCb,
               OEMGSM1xCardHandler_SVC_TimerCallback, 
               (void *)&lastInitSMS
               );

            AEE_SetSysTimerCallback(10*1000, &gTimerCb);
        }
        else
        {
            MSG_ERROR("Could not read EFsms",0,0,0);
            gSIMInfo.SMSCount = 0;
        }

    }   /* SMS Enabled */        
    else
    {
       SMSInitFlag = TRUE;
    }
    return (status == UIM_PASS);
    
}   /* OEMGSM1xCardHandler_SVC_Init_SMSStatusCache */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_CBInitFN
 
DESCRIPTION:  Called when initializing SMS Status Cache.

PARAMETERS:
   report: the result of the command
 
RETURN VALUE:
   boolean : TRUE - Success
             FALSE - Failure

COMMENTS:
    Should be called only once from withing OEMGSM1xCardHandler_SVC_Init.
    Should be updated after every call to SetSMS / ClearSMS 

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_CBInitFN(uim_rpt_type * report)
{
    MSG_LOW("Starting CBInitFN ...",0,0,0);

    /* Just a sanity check */
    if( report->rpt_type != UIM_ACCESS_R )
    {
        MSG_FATAL("Expected UIM_ACCESS_R report, got %d report",report->rpt_type,0,0);
        return;
    }

    if ( report->rpt_status == UIM_PASS)
    {
       MSG_FATAL("Set simSMSStatus[%d] of %d (%d)",
          lastInitSMS, gSIMInfo.SMSCount, initMsg.data[0]);

       simSMSStatus[lastInitSMS] = initMsg.data[0];
       lastInitSMS++;

        // Since we are not blocking we can switch to the UI thread context 
        // in order to execute the timer.

       CALLBACK_Init(
           &gSwitchToBrewCb,
           OEMGSM1xCardHandler_SVC_NextSMSCallback, 
          (void *)&lastInitSMS
          );

        AEE_SYS_RESUME(&gSwitchToBrewCb);
    }
    else
    {
        MSG_ERROR("Could not access an EF file!",0,0,0);        
        gSIMInfo.SMSCount = lastInitSMS;

        // Set the flag because we have at least 1 entry
        SMSInitFlag = TRUE;
    }

    MSG_LOW("Ending CBInitFN ...",0,0,0);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_NextSMSCallback
 
DESCRIPTION:  Called when initializing SMS Status Cache and switching context
              to BREW (UI task) from the UIM task

PARAMETERS:
   data: should be the address of lastInitSMS
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_NextSMSCallback(void *data)
{
    uint32 IntervalTime = 1000;

    CALLBACK_Init(
          &gTimerCb,
          OEMGSM1xCardHandler_SVC_TimerCallback, 
          data
          );

    AEE_SetSysTimerCallback(IntervalTime, &gTimerCb);
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_TimerCallback
 
DESCRIPTION:  Called when initializing SMS Status Cache.

PARAMETERS:
   data: should be the address of lastInitSMS
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_TimerCallback(void *data)
{
   if (data == &lastInitSMS && 
       SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
   {
      if (lastInitSMS < gSIMInfo.SMSCount)
      {
         uim_rpt_status status;
         uint8          payloadLength;

         status = UIM_FAIL;
         payloadLength = sizeof(initMsg);

         /* Check if SMS is available in the SIM Service Table */
         if ( SERVICETABLE_IS_SMS_STORAGE_ENABLED(simServiceTable) )
         {   /* Access The SMS File */
             OEMGSM1xCardHandler_SVC_InitSim();
         }
         else
         {
             MSG_ERROR("SMS SIM access not allowed in service table!",0,0,0);
             SMSInitFlag = TRUE;
         }
      }
      else
      {
         MSG_FATAL("SMSInitFlag = TRUE",0,0,0);
         SMSInitFlag = TRUE;
      }
   }
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_InitSim
 
DESCRIPTION: Performs an access operation (read/write) to/from the SIM
     
 
PARAMETERS:
    accessType:     Read/Write
    efToAccess:     EF to be accessed
    offset:         Record number / offset to access
    payload:        Buffer to read / write
    payloadLength:  How much to read / write
 
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_InitSim( void )
{
    MSG_LOW("Starting InitSim...",0,0,0);

    /* Build the uim_cmd common header */
    uimInitCmd.access_uim.hdr.command = UIM_ACCESS_F;   /* "Access" an EF */
    uimInitCmd.access_uim.hdr.cmd_hdr.task_ptr = NULL;  /* Don't signal any task */
    uimInitCmd.access_uim.hdr.cmd_hdr.sigs = 0;         /* Don't signal any task */
    uimInitCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;/* Don't release to a Q */
    uimInitCmd.access_uim.hdr.rpt_function = 
                                OEMGSM1xCardHandler_SVC_CBInitFN;/* CB function for the status */
    uimInitCmd.access_uim.hdr.protocol = UIM_GSM;       /* Use GSM protocol (GSM sim) */
    uimInitCmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;/* Always report */
    
    /* Build the uim_cmd proprietery fields */
    uimInitCmd.access_uim.item = UIM_TELECOM_SMS;       /* EF to access */
    uimInitCmd.access_uim.access = UIM_READ_F;          /* Read / Write */
    uimInitCmd.access_uim.offset = lastInitSMS + 1;     /* Offset / record # */
    uimInitCmd.access_uim.rec_mode = UIM_ABSOLUTE;      /* Absolute mode 4 records */
    uimInitCmd.access_uim.data_ptr = &initMsg;          /* Payload to read/write */
    uimInitCmd.access_uim.num_bytes = sizeof(initMsg);  /* Length of payload */

    /* Send the command to the SIM */
    uim_cmd( &uimInitCmd );

    MSG_LOW("Ending InitSim...",0,0,0);
}   /* OEMGSM1xCardHandler_SVC_InitSim */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SimAccess
 
DESCRIPTION: Performs an access operation (read/write) to/from the SIM
     
 
PARAMETERS:
    accessType:     Read/Write
    efToAccess:     EF to be accessed
    offset:         Record number / offset to access
    payload:        Buffer to read / write
    payloadLength:  How much to read / write
 
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL uint32 OEMGSM1xCardHandler_SVC_SimAccess(
    uim_access_type accessType,
    uim_items_enum_type efToAccess,
    word offset,
    byte* payload, 
    uint8* payloadLength,
    sim_info *pSIMInfo
    )
{
    uim_rpt_status   status;
    uint8            respLen;
    uint8            numRecords;

    status = UIM_FAIL;

    MSG_LOW("Starting SimAccess...",0,0,0);

    MSG_HIGH("SIM access,type=%d EF=%d offset=%d",accessType,efToAccess,offset);

    /* Build the uim_cmd common header */
    uimCmd.access_uim.hdr.command = UIM_ACCESS_F;   /* "Access" an EF */
    uimCmd.access_uim.hdr.cmd_hdr.task_ptr = NULL;  /* Don't signal any task */
    uimCmd.access_uim.hdr.cmd_hdr.sigs = 0;         /* Don't signal any task */
    uimCmd.access_uim.hdr.cmd_hdr.done_q_ptr = NULL;/* Don't release to a Q */
    uimCmd.access_uim.hdr.rpt_function = 
                                OEMGSM1xCardHandler_SVC_CBreportFN;/* CB function for the status */
    uimCmd.access_uim.hdr.protocol = UIM_GSM;       /* Use GSM protocol (GSM sim) */
    uimCmd.access_uim.hdr.options = UIM_OPTION_ALWAYS_RPT;/* Always report */
    
    /* Build the uim_cmd proprietery fields */
    uimCmd.access_uim.item = efToAccess;            /* EF to access */
    uimCmd.access_uim.access = accessType;          /* Read / Write */
    uimCmd.access_uim.offset=offset;                /* Offset / record # */
    uimCmd.access_uim.rec_mode  = UIM_ABSOLUTE;     /* Absolute mode 4 records */
    uimCmd.access_uim.data_ptr = payload;           /* Payload to read/write */
    uimCmd.access_uim.num_bytes = *payloadLength;   /* Length of payload */

    /* Send the command to the SIM */
    uim_cmd( &uimCmd );

    /* Block here till the SIM access is done */
    OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady();

    /* Just a sanity check */
    if( uimRprt.rpt_type != UIM_ACCESS_R )
    {
        MSG_FATAL("Expected UIM_ACCESS_R report, got %d report",uimRprt.rpt_type,0,0);
    }

    /* Save important info. */
    status = uimRprt.rpt_status;
    respLen = uimCmd.access_uim.num_bytes_rsp;
    numRecords = uimCmd.access_uim.num_records_rsp;

    if ( status != UIM_PASS)
    {
        MSG_ERROR("Could not access an EF file!",0,0,0);        
    }
    else
    {
        /* Return the respLen */
        *payloadLength = respLen;

        /* On access to EFsms, update the count & rec size */
        if ( pSIMInfo && (accessType == UIM_READ_F) && ( efToAccess == UIM_TELECOM_SMS) ) 
        {
            pSIMInfo->SMSMaxSize = respLen;
            pSIMInfo->SMSCount = (numRecords > MAX_NV_MSGS) ? MAX_NV_MSGS : numRecords;
        }
		/* On access to EFplmnsel, update the count */
		else if ( pSIMInfo && (accessType == UIM_READ_F) && ( efToAccess == UIM_GSM_PLMN) ) 
        {
            pSIMInfo->PLMNSelMaxSize = respLen;          
		}
		/* On access to EFmsisdn, update the count */
		else if ( pSIMInfo && (accessType == UIM_READ_F) && ( efToAccess == UIM_TELECOM_MSISDN) ) 
        {
            pSIMInfo->MSISDNMaxSize = respLen;          
	    pSIMInfo->MSISDNCount   = numRecords; 
	}
        /* If first access to EFadn, update the count & rec size */
        else if ( pSIMInfo && (accessType == UIM_READ_F) && ( efToAccess == UIM_TELECOM_ADN) ) 
        {
            pSIMInfo->ADNMaxSize = respLen;
            pSIMInfo->ADNCount = numRecords;
        } /* If first access to EFsmsr, update the count */
    }
	
    MSG_LOW("Ending SimAccess...",0,0,0);

    return status;    

}   /* OEMGSM1xCardHandler_SVC_SimAccess */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SelectMF
 
DESCRIPTION:  Performs a SELECT op on the SIM's MF
        
PARAMETERS:
   None
 
RETURN VALUE:
   uint32:  UIM_PASS on success
			UIM_FAIL if an error occurred

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL uint32 OEMGSM1xCardHandler_SVC_SelectMF(void)
{
    uim_rpt_status                  status;

    status = UIM_FAIL;

    /* Build the uim_cmd common header */
    uimCmd.select.hdr.command = UIM_SELECT_F;
    uimCmd.select.hdr.cmd_hdr.task_ptr = NULL;
    uimCmd.select.hdr.cmd_hdr.sigs = 0;
    uimCmd.select.hdr.cmd_hdr.done_q_ptr = NULL;
    uimCmd.select.hdr.rpt_function = OEMGSM1xCardHandler_SVC_CBreportFN;
    uimCmd.select.hdr.protocol = UIM_GSM;
    uimCmd.select.hdr.options = UIM_OPTION_ALWAYS_RPT;
    
    /* Build the uim_cmd proprietery fields */
    uimCmd.select.file_type = UIM_MF;    
    /* Send the command to the SIM */
    uim_cmd( &uimCmd );

    /* Block here till the SIM access is done */
    OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady();

    if( uimRprt.rpt_type != UIM_SELECT_R )
    {
        MSG_FATAL("Expected UIM_SELECT_R report, got %d report",uimRprt.rpt_type,0,0);
    }

    /* Save the status */
    status = uimRprt.rpt_status;
    
    if ( status != UIM_PASS)
    {
        MSG_ERROR("Could not select MF !",0,0,0);        
    } 
    return status;

}   /* OEMGSM1xCardHandler_SVC_SelectMF */


#ifdef CODE_SECTION_IS_NOT_USED

/*=============================================================================
FUNCTION:  OEMGSM1xCardHandlerSVCGetCHVstatus
 
DESCRIPTION:  Performs a STATUS op on the SIM, return the SIM's CHV status.
 
PARAMETERS:
   None
 
RETURN VALUE:
   uim_chv_status_type: NULL - failure

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL uim_chv_status_type* OEMGSM1xCardHandlerSVCGetCHVstatus(void)
{
    static uim_chv_status_type      uim_chv_status;
    uim_rpt_type                    rprt;

    /* Build the uim_cmd common header */
    uimCmd.hdr.command = UIM_STATUS_F;
    uimCmd.hdr.cmd_hdr.task_ptr = NULL;
    uimCmd.hdr.cmd_hdr.sigs = 0;
    uimCmd.hdr.cmd_hdr.done_q_ptr = NULL;
    uimCmd.hdr.rpt_function = OEMGSM1xCardHandler_SVC_CBreportFN;
    uimCmd.hdr.protocol = UIM_GSM;
    uimCmd.hdr.options = UIM_OPTION_ALWAYS_RPT;
    /* Send the command to the SIM */
    uim_cmd( &uimCmd );
    /* Block here till the SIM access is done */
    OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady();
    /* Sanity Check */
    if( uimRprt.rpt_type != UIM_STATUS_R )
    {
        MSG_FATAL("Expected UIM_STATUS_R report, got %d report",uimRprt.rpt_type,0,0);
        return NULL;
    }

    /* Save the entire report for future use */
    rprt = uimRprt;    
    
    if ( (UIM_STATUS_R == rprt.rpt_type) &&
         (UIM_PASS == rprt.rpt_status))
    {
         uim_sel_mf_df_rsp_type *data;

         data = (uim_sel_mf_df_rsp_type*)rprt.rpt.status.data;

         // Check CHV1 'enabled' status (Note that the bit comes back from the UIM inverted) 
         uim_chv_status.chv1_status = (data->file_char & 0x80) ? 0 : 1;

         // Remaining CHV1 count
         uim_chv_status.chv1cnt_remain = data->chv1_status & 0x0f;
         //* Remaining Unblock CHV1 count 
         uim_chv_status.uchv1cnt_remain = data->uchv1_status & 0x0f;
         // Remaining CHV2 count 
         uim_chv_status.chv2cnt_remain = data->chv2_status & 0x0f;
         //* Remaining Unblock CHV2 count 
         uim_chv_status.uchv2cnt_remain = data->uchv2_status & 0x0f;
    } 
    else 
    {
         MSG_ERROR("UIM STATUS command failed", 0, 0 ,0);
         memset(&uim_chv_status, 0, sizeof(uim_chv_status));
         return NULL;
    }

    return &uim_chv_status;

}   /* OEMGSM1xCardHandlerSVCGetCHVstatus */

#endif //#CODE_SECTION_IS_NOT_USED


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady
 
DESCRIPTION:  Blocks until will be signaled
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady(void)
{
    /* Clear and wait to be woken (by the CB function */
    rex_clr_sigs(callingThreadTCB,OEMGSM1XPROV_UI_SIG_FOR_UIM);
    rex_wait( OEMGSM1XPROV_UI_SIG_FOR_UIM );
    rex_clr_sigs(callingThreadTCB,OEMGSM1XPROV_UI_SIG_FOR_UIM);

}   /* OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_CBreportFN
 
DESCRIPTION:  Being called by the uim task when the process of the uim_cmd
    is done. Will wake up the sleeping task (UI) by signaling it
 
PARAMETERS:
   None
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
LOCAL void OEMGSM1xCardHandler_SVC_CBreportFN( uim_rpt_type * report)
{
    MSG_LOW("Starting CBreportFN...",0,0,0);

    /* Save the report for future use */
    uimRprt = *report;

    MSG_MED("GSM1xUIM CBFN report type=%d sts=%d",report->rpt_type,report->rpt_status,0);
    MSG_MED("GSM1xUIM CBFN report sw1=%d sw2=%d",report->sw1,report->sw2,0);

    /* Wake up the blocking function (OEMGSM1xCardHandler_SVC_WaitUntilReportIsReady) */
    rex_set_sigs( callingThreadTCB, OEMGSM1XPROV_UI_SIG_FOR_UIM);
    
    MSG_LOW("Ending CBreportFN...",0,0,0);
    
}   /* OEMGSM1xCardHandler_SVC_CBreportFN */


/* ##################################################################################  

        New SIM support for Europa
		
 ##################################################################################  */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN

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
uint32 OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo)
{
    uim_rpt_status   status;
    byte            *payload;
    uint8            payloadLength;

    CardHandlerFPLMNListType fplmnInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&fplmnInfo;
    payloadLength = sizeof(fplmnInfo);

    // must make sure that all the buffer is zero, especially the parts that are not read!
	MEMSET((void*)&fplmnInfo, 0, sizeof(fplmnInfo));

    /* Read The FPLMN File */
	status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
										       UIM_GSM_FPLMN,   /* EF to access */
										       0,               /* offset to access */
										       payload,         /* Buffer to write */
										       &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
										       );
	if ( status == UIM_PASS )
    {
        COPY_TO_USER((byte*)pFplmnInfo, (byte*)&fplmnInfo, sizeof(*pFplmnInfo));
    }
    else
    {   /* SIM access failed */
		MSG_ERROR("Could not read from EFfplmn!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN

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
uint32 OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN(CardHandlerFPLMNListType* pFplmnInfo)
{
    uim_rpt_status   status;
    byte            *payload;
    uint8            payloadLength;

    CardHandlerFPLMNListType fplmnInfo;
    
    status        = UIM_FAIL;
    payload       = (byte*)&fplmnInfo;
    payloadLength = sizeof(fplmnInfo);

    COPY_FROM_USER((byte*)&fplmnInfo, (byte*)pFplmnInfo, sizeof(fplmnInfo));
     
    /* Read The FPLMN File */
	status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,     /* Operation (Read/Write) */
										       UIM_GSM_FPLMN,   /* EF to access */
										       0,               /* offset to access */
										       payload,         /* Buffer to write */
										       &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
										       );
	if ( status != UIM_PASS )
	{   /* SIM access failed */
		MSG_ERROR("Could not write to EFfplmn!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadPLMNSelector

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

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_ReadPLMNSelector(CardHandlerPLMNSelectorListType * pPlmnSelectorInfo)
{
    uim_rpt_status   status;
    byte           * payload ;
    uint8            payloadLength;

    CardHandlerPLMNSelectorListType plmnSelectorInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&plmnSelectorInfo;
    payloadLength = sizeof(plmnSelectorInfo);

	// must make sure that all the buffer is zero, especially the parts that are not read!
	MEMSET((void*)&plmnSelectorInfo, 0, sizeof(plmnSelectorInfo));

	/* Check if PLMN Selector is available in the SIM Service Table */
	if ( SERVICETABLE_IS_PLMNSEL_ENABLED(simServiceTable) )
	{
        /* Read The PLMN Selector File */
		status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
											       UIM_GSM_PLMN,    /* EF to access */
											       0,               /* offset to access */
											       payload,         /* Buffer to write */
											       &payloadLength,  /* Size of data buff (in & out) */
                                                   &gSIMInfo        /* SIM Info struct */
											       );
	    if ( status == UIM_PASS )
	    { 
            COPY_TO_USER((byte*)pPlmnSelectorInfo, (byte*)&plmnSelectorInfo, sizeof(*pPlmnSelectorInfo));
	    }
        else
        {
            /* SIM 	access failed */
	        MSG_ERROR("Could not read from EFplmn!",0,0,0);            
        }
	}
	else
	{
		MSG_ERROR("PLMN SIM access not allowed in service table!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_WritePLMNSelector

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

=============================================================================*/
uint32 OEMGSM1xCardHandler_SVC_WritePLMNSelector(CardHandlerPLMNSelectorListType * pPlmnSelectorInfo)
{
    uim_rpt_status   status;
    byte            *payload;
    uint8            payloadLength;

    CardHandlerPLMNSelectorListType plmnSelectorInfo;
    
    status        = UIM_FAIL;
    payload       = (byte*)&plmnSelectorInfo;
    payloadLength = MIN( sizeof(plmnSelectorInfo), gSIMInfo.PLMNSelMaxSize);

    COPY_FROM_USER((byte*)&plmnSelectorInfo, (byte*)pPlmnSelectorInfo, sizeof(plmnSelectorInfo));

	/* Check if PLMN Selector is available in the SIM Service Table */
	if ( SERVICETABLE_IS_PLMNSEL_ENABLED(simServiceTable) )
	{
        /* Read The PLMN Selector File */
		status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,      /* Operation (Read/Write) */
											       UIM_GSM_PLMN,    /* EF to access */
											       0,               /* offset to access */
											       payload,         /* Buffer to write */
											       &payloadLength,  /* Size of data buff (in & out) */
                                                   &gSIMInfo        /* SIM Info struct */
											       );
		if ( status != UIM_PASS )
		{   /* SIM 	access failed */
			MSG_ERROR("Could not read from EFplmn!",0,0,0);
		}

	}
	else
	{
		MSG_ERROR("PLMN SIM access not allowed in service table!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadAccessClass

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
uint32 OEMGSM1xCardHandler_SVC_ReadAccessClass(CardHandlerAaccessClassBufType * pAccInfo)
{
    uim_rpt_status   status;
    byte            *payload;
    uint8            payloadLength;

    CardHandlerAaccessClassBufType accInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&accInfo;
    payloadLength = sizeof(accInfo);

        /* Read The Access Control File */
	status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
										       UIM_GSM_ACC,     /* EF to access */
										       0,               /* offset to access */
										       payload,         /* Buffer to write */
										       &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
										       );
	if ( status == UIM_PASS )
    {
        COPY_TO_USER((byte*)pAccInfo, (byte*)&accInfo, sizeof(*pAccInfo));
    }
    else 
    {   /* SIM access failed */
		MSG_ERROR("Could not read from EFacc!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadICCId

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
uint32 OEMGSM1xCardHandler_SVC_ReadICCId(CardHandlerICCIdBufType * pICCId)
{
    uim_rpt_status     status;
    byte              *payload;
    uint8              payloadLength;

    CardHandlerICCIdBufType iccId;

    status          = UIM_FAIL;
    payload         = (byte*)&iccId;
    payloadLength   = sizeof(iccId);

        /* Read The Access Control File */
	status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
										       UIM_ICCID,       /* EF to access */
										       0,               /* offset to access */
										       payload,         /* Buffer to write */
										       &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
										       );
	if ( status == UIM_PASS )
    {
        COPY_TO_USER((byte*)pICCId, (byte*)&iccId, sizeof(*pICCId));
    }
    else 
	{   /* SIM access failed */
		MSG_ERROR("Could not read from EFiccid!",0,0,0);
	}

	return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadIMSI

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
uint32 OEMGSM1xCardHandler_SVC_ReadIMSI(CardHandlerImsiBufType * pImsiInfo)
{
    uim_rpt_status    status;
    byte             *payload;
    uint8             payloadLength;

    CardHandlerImsiBufType imsiInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&imsiInfo;
    payloadLength = sizeof(imsiInfo);

    /* Read The IMSI File */
    status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
										       UIM_GSM_IMSI,    /* EF to access */
										       0,               /* offset to access */
										       payload,         /* Buffer to write */
										       &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
										       );
    if ( status == UIM_PASS )
    {
        COPY_TO_USER((byte*)pImsiInfo, (byte*)&imsiInfo, sizeof(*pImsiInfo));  
    }
    else
    {   /* SIM access failed */
        MSG_ERROR("Could not read from EFimsi!",0,0,0);
    }

    return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadMSISDN

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
uint32 OEMGSM1xCardHandler_SVC_ReadMSISDN
(
    CardHandlerMSISDNBufType  *msisdn_buffer, 
    int                        record_number, 
    uint8                     *alphaLength
)
{
    uim_rpt_status  status;
    byte            efBuffer[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8           payloadLength;

    uint8           alphaLengthResult;

    status         = UIM_FAIL;
    payloadLength  = gSIMInfo.MSISDNMaxSize;

    if ( record_number >= gSIMInfo.MSISDNCount ) 
    {
        MSG_ERROR("Received an out of range MSISDN index (%d)!",record_number,0,0);
        return UIM_FAIL;
    }

    if ( (msisdn_buffer == NULL) || (alphaLength == NULL))
    {
        MSG_ERROR("Out parameter is NULL!",0,0,0);
        return UIM_FAIL;
    }

    /* Check if MSISDN is available in the SIM Service Table */
    if ( SERVICETABLE_IS_MSISDN_ENABLED(simServiceTable) )
    {   /* Access The MSISDN File */

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,     /* Operation (Read/Write) */
                                                   UIM_TELECOM_MSISDN,/* EF to access */
                                                   record_number + 1, /* Record # to access */
                                                   efBuffer,       /* Where to put the data */
                                                   &payloadLength, /* Size of data buff (in & out) */
                                                   &gSIMInfo       /* SIM Info struct */
                                                   );
        if ( status == UIM_PASS )
        {            
            COPY_TO_USER(msisdn_buffer->data, efBuffer, MIN(payloadLength,sizeof(msisdn_buffer->data)));

            alphaLengthResult = payloadLength - 14;/* There are 14 bytes following the alpha identifier */
        }
        else
        {   /* SIM access failed */
            MSG_ERROR("Could not read from EFmsisdn file!",0,0,0);

            alphaLengthResult = 0;
        }   

        COPY_TO_USER(alphaLength, &alphaLengthResult, sizeof(*alphaLength));
    }
    else
    {
        MSG_ERROR("MSISDN SIM access not allowed in service table!",0,0,0);
    }

    return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_ReadAD

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
uint32 OEMGSM1xCardHandler_SVC_ReadAD(CardHandlerADBufType *pADInfo)
{
    uim_rpt_status    status;
    byte             *payload;
    uint8             payloadLength;

    CardHandlerADBufType adInfo;

    status        = UIM_FAIL;
    payload       = (byte*)&adInfo;
    payloadLength = sizeof(adInfo);

    /* Read The IMSI File */
    status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                               UIM_GSM_AD,      /* EF to access */
                                               0,               /* offset to access */
                                               payload,         /* Buffer to write */
                                               &payloadLength,  /* Size of data buff (in & out) */
                                               NULL             /* SIM Info struct not required */
                                               );
    if ( status == UIM_PASS )
    {
        COPY_TO_USER(pADInfo, &adInfo, sizeof(*pADInfo));
    }
    else
    {   /* SIM access failed */
        MSG_ERROR("Could not read from EFad!",0,0,0);
    }

    return status;
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetDirPresence

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
uint32 OEMGSM1xCardHandler_SVC_GetDirPresence
(
    CardHandlerDirPresenceBitMaskType  *dirPresenceBitMask
)
{
    uint32                              status;
    CardHandlerDirPresenceBitMaskType   uimDirIndicator;
    
    uimDirIndicator = 0;
       
    /* Attempt a mandatory field in MF, if we succeed, DFgsm must be there.*/
    {
        CardHandlerICCIdBufType  ICCId;
        byte* payload         = (byte*)&ICCId;
        uint8 payloadLength   = sizeof(ICCId);

	    /* Read The Access Control File */
	    status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
										           UIM_ICCID,       /* EF to access */
										           0,               /* offset to access */
										           payload,         /* Buffer to write */
										           &payloadLength,  /* Size of data buff (in & out) */
                                                   NULL             /* SIM Info struct not required */
										           );
	    if (UIM_PASS == status)
		{  
            uimDirIndicator |= OEMGSM1xCardHandler_MF_PRESENT;
        }
    }

    {
        CardHandlerADBufType    adBuf;
        byte                   *payload = (byte*)&adBuf;
        uint8                   payloadLength = sizeof(adBuf);

        /* Attempt a mandatory field in DFgsm, if we succeed, DFgsm must be there.*/
        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                                   UIM_GSM_AD,      /* EF to access */
                                                   0,               /* offset to access */
                                                   payload,         /* Buffer to write */
                                                   &payloadLength,  /* Size of data buff (in & out) */
                                                   NULL             /* SIM Info struct not required */
                                                   );
        if(UIM_PASS == status)
        {  
           uimDirIndicator |= OEMGSM1xCardHandler_GSM_DF_PRESENT;
        }

        /* Attempt a mandatory field in DFcdma, if we succeed, DFcdma must be there.*/
        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,      /* Operation (Read/Write) */
                                                   UIM_CDMA_UIM_ID_USAGE_IND,      /* EF to access */
                                                   0,               /* offset to access */
                                                   payload,         /* Buffer to write */
                                                   &payloadLength,  /* Size of data buff (in & out) */
                                                   NULL             /* SIM Info struct not required */
                                                   );
        if(UIM_PASS == status)
        {  
           uimDirIndicator |= OEMGSM1xCardHandler_CDMA_DF_PRESENT;
        }
    }

    COPY_TO_USER(dirPresenceBitMask, &uimDirIndicator, sizeof(*dirPresenceBitMask));

    return UIM_PASS;    
}


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_GetAdn

DESCRIPTION:  Retrieves the contents of the specified SIM ADN record

PARAMETERS:
   index: SIM ADN record to read (zero based index)
   *pAdn: record content   (status byte + data)
   *alphaLength: points to the memory location to receive the length
                 of "alpha-identitier".  This information is needed
                 in order to parse the rest of ADN, 
                 since "alpha-identitfier" is a variable-length field
                 that is located in the beginning of ADN record.

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
uint32 OEMGSM1xCardHandler_SVC_GetAdn
(
   uint16                 index,
   CardHandlerRawADNType *pAdn,
   uint8                 *alphaLength
)
{
    uim_rpt_status  status;
    byte efBuffer[OEMGSM1xCardHandler_EF_BUF_SIZE];
    uint8           payloadLength;
    uint8           alphaLengthResult;
   
    status        = UIM_FAIL;
    payloadLength = gSIMInfo.ADNMaxSize;

    if ( alphaLength == NULL) 
    {
	   MSG_ERROR("Out parameter alphaLength is NULL!",0,0,0);
	   return UIM_FAIL;
    }

    if ( (index != 0) && (index >= gSIMInfo.ADNCount) ) 
    {
        MSG_ERROR("Received an out of range ADN index (%d)!",index,0,0);
        return UIM_FAIL;
    }

    /* Check if ADN is available in the SIM Service Table */
    if ( SERVICETABLE_IS_ADN_STORAGE_ENABLED(simServiceTable) )
    {   /* Access The ADN File */

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_READ_F,     /* Operation (Read/Write) */
                                                   UIM_TELECOM_ADN,/* EF to access */
                                                   index + 1,      /* Record # to access */
                                                   efBuffer,       /* Where to put the data */
                                                   &payloadLength, /* Size of data buff (in & out) */
                                                   &gSIMInfo
                                                   );
        if ( status == UIM_PASS )
        {
            COPY_TO_USER(pAdn->data, efBuffer, MIN(payloadLength,sizeof(pAdn->data)));

			alphaLengthResult = payloadLength - 14;/*There are 14 bytes following the alpha identifier */
        }
        else 
        {   /* SIM access failed */
            MSG_ERROR("Could not read from EFadn file!",0,0,0);
			alphaLengthResult = 0;
        } 

        COPY_TO_USER(alphaLength, &alphaLengthResult, sizeof(*alphaLength));
    }
    else
    {
        MSG_ERROR("ADN SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_GetAdn */


/*=============================================================================
FUNCTION:  OEMGSM1xCardHandler_SVC_SetAdn

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
uint32 OEMGSM1xCardHandler_SVC_SetAdn(uint16 index, CardHandlerRawADNType *pAdn)
{
    CardHandlerRawADNType adnIndo;
    uim_rpt_status status = UIM_FAIL;
        
    if ( index >= gSIMInfo.ADNCount ) 
    {
        MSG_ERROR("Received an out of range ADN index (%d)!",index,0,0);
        return UIM_FAIL;
    }

    COPY_FROM_USER(&adnIndo, pAdn, sizeof(adnIndo));
  
    /* Check if ADN is available in the SIM Service Table */
    if ( SERVICETABLE_IS_ADN_STORAGE_ENABLED(simServiceTable) )
    {   
        uint8 sizeOf = MIN(gSIMInfo.ADNMaxSize, sizeof(adnIndo.data));

        status = OEMGSM1xCardHandler_SVC_SimAccess(UIM_WRITE_F,    /* Operation (Read/Write) */
                                                   UIM_TELECOM_ADN,/* EF to access */
                                                   index + 1,      /* Record # to access */
                                                   adnIndo.data,   /* Buffer to write */
                                                   &sizeOf,        /* Size of data buff (in & out) */
                                                   &gSIMInfo
                                                   );
        if ( status != UIM_PASS )
        {   /* SIM access failed */
            MSG_ERROR("Could not write to EFadn file!",0,0,0);
        } 
	}
    else
    {
        MSG_ERROR("ADN SIM access not allowed in service table!",0,0,0);
    }
    return status;

}   /* OEMGSM1xCardHandler_SVC_SetAdn */


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
uint8 OEMGSM1xCardHandler_SVC_GetNumAdnRec()
{
    return gSIMInfo.ADNCount;
}


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
uint8 OEMGSM1xCardHandler_SVC_GetAdnAlphaSize()
{
    if (gSIMInfo.ADNMaxSize > 14)
    {
        return gSIMInfo.ADNMaxSize - 14;
    }
    else
    {
        return 0;
    }
}

#endif /* FEATURE_GSM1x */

