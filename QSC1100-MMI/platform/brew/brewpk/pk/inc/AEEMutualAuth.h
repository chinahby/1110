/*======================================================
                    IMPORTANT NOTICE

               OEM/CARRIER DISTRIBUTION ONLY

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement,
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AEEMutualAuth.h

SERVICES:  AEE IMutualAuth Interface

GENERAL DESCRIPTION:

  This file is provided by QUALCOMM Incorporated to selected carrier partners ONLY.

  It is NOT to be distributed as part of any software development kit.

  It is NOT to be distributed outside of the context specifically noted in licensing
  terms with QUALCOMM.

  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEEMUTUALAUTH_H
#define AEEMUTUALAUTH_H

#include "AEE.h"
#include "AEESource.h"
#include "AEEWeb.h"
#include "AEEWebOpts.h"

#define AEECLSID_IMUTUALAUTH 0x010138a5


//===========================================================================
// Defines
//===========================================================================

//===========================================================================
// MA CLIENT CAPABILITY BITFLAGS
//===========================================================================
//   0x00000001  Current serving mode DOES NOT have a notion of 
//               synchronized time. As a rule, it is assumed
//               that all modes except CDMA do not to support 
//               synchronized time.
//               Dynamically set by BREW. OEM cannot configure this.
//               0:CDMA, 1: All other modes including UMTS & NO SVC.
//
//   0x00000002  Client is capable of receiving Trusted Time from 
//               ADS and would like to make use of this feature if 
//               ADS supports it.	
//               NOT configurable by OEM.      
//               Hard Coded by BREW - set ON for 3.1.4 & later
//
//   0x00000004  Handset has a chip with a real time battery backed 
//               up clock that keeps time if the main handset battery 
//               is removed.	     
//               NOT configurable by OEM.      
//               Hard Coded by BREW - set ON for 3.1.4 & later
//
//   0x00000008  Client is capable of handling the Next Nonce and 
//               would be happy to to make use of this feature if 
//               ADS supports it.	
//               Static value. OEM configures it once, upfront.
//
//   0x00000010  Can the end user set the secure clock? 
//               Must include having the ability to do this via a 
//               secret menu or backdoor.	
//               Static value. OEM configures it once, upfront.
//
//   0x00000020  Can the secure clock be set via NTP?	
//               Static value. OEM configures it once, upfront.
//
//   0x00000040  Can the secure clock be set via ROAP?	
//               Static value. OEM configures it once, upfront.
//
//   0x00000080  Can the secure clock be set via some other means?	
//               Static value. OEM configures it once, upfront.
//
//  0x00000100-0xFFFFFFFF UNUSED    	N/A	
//===========================================================================
//Flag                             Value	                  Source	           
//===========================================================================
#define MA_CLIENT_CAP_MODE_NO_TIME_SYNC       (0x00000001)   // OEM layer API call. Changes dynamically. 
#define MA_CLIENT_CAP_SUPPORTS_NN             (0x00000002)   // Hard coded ON by BREW in 3.14 & later
#define MA_CLIENT_CAP_SUPPORTS_TT             (0x00000004)   // Hard coded ON by BREW in 3.14 & later 
#define MA_CLIENT_CAP_BATTERY_BACKED_RTC      (0x00000008)   // OEM Config Item	  
#define MA_CLIENT_CAP_USER_CLOCK_SET          (0x00000010)   // OEM Config Item	  
#define MA_CLIENT_CAP_NTP_CLOCK_SET           (0x00000020)   // OEM Config Item	  
#define MA_CLIENT_CAP_ROAP_CLOCK_SET          (0x00000040)   // OEM Config Item	  
#define MA_CLIENT_CAP_OTHER_CLOCK_SET         (0x00000080)   // OEM Config Item	  

//
//BREW Mutual Authentication error codes
//
#define AEEMA_ERROR_BASE           0xd00
#define AEEMA_MUTUALAUTH_FAILED    (AEEMA_ERROR_BASE + 0)
#define AEEMA_MAC_FAILED           (AEEMA_ERROR_BASE + 1)
#define AEEMA_BAD_NONCE            (AEEMA_ERROR_BASE + 2)
#define AEEMA_REREG_AND_DELETE     (AEEMA_ERROR_BASE + 3)
#define AEEMA_CERT_INVALID         (AEEMA_ERROR_BASE + 4)
#define AEEMA_INVALID_CRYPTO_SUITE (AEEMA_ERROR_BASE + 5)
#define AEEMA_REREG                (AEEMA_ERROR_BASE + 6)
#define AEEMA_FILE_ERROR           (AEEMA_ERROR_BASE + 7)

// Gap here for MA background app status codes...

#define AEEMA_UPDATE_NN           (AEEMA_ERROR_BASE + 15)  // Not fatal err - generated by ADS, asking client to get new NN
#define AEEMA_UPDATE_TIME         (AEEMA_ERROR_BASE + 16)  // Not fatal err - generated by ADS, asking client to get new time
#define AEEMA_UPDATE_NN_AND_TIME  (AEEMA_ERROR_BASE + 17)  // Not fatal err - generated by ADS, asking client to get new NN&time

#define AEEMA_NO_NN               (AEEMA_ERROR_BASE + 18)  // Fatal err - ADS has asked us to get new NN, but it is not present or bad
#define AEEMA_NO_TIME             (AEEMA_ERROR_BASE + 19)  // Fatal err - ADS has asked us to get new time, but it is not present or bad

#define AEEMA_BAD_CLIENT_TIME     (AEEMA_ERROR_BASE + 20)  // Fatal err - client supplied time is bad even after multiple attempts
#define AEEMA_BAD_CLIENT_NN       (AEEMA_ERROR_BASE + 21)  // Fatal err - client supplied NN is bad even after multiple attempts

#define AEEMA_TIME_REQUEST_FAILED (AEEMA_ERROR_BASE + 22)  // Fatal err code set by ADS to indicate that it failed to supply time update

#define AEEMA_BUSY                (AEEMA_ERROR_BASE + 23)  // Request cannot be completed at this time. 
                                                           // (Most likely reason being that MA state machine is busy)
                                                           // (Most likely reason being that MA state machine is busy)
#define AEEMA_BAD_NNRESP          (AEEMA_ERROR_BASE + 24)   // 3352 bad next nonce (NN) 
#define AEEMA_NOLOCK              (AEEMA_ERROR_BASE + 25)  // Mutex has not been acquired when MA requests are sent. This is an internal error.


// Used by IMA to specify type of svc running on top of MutualAuth - eg Billing Download etc
#define WEBOPT_MUTUALAUTH_SVC_TYPE     WEBOPT_MA_SERVICE_TYPE            

//---------------------------------------------------------------------
// IMutualAuth Interface
//---------------------------------------------------------------------
  

#define INHERIT_IMutualAuth(iname)\
   INHERIT_IQueryInterface(iname);\
   int              (*AdsMutualAuth)(iname *po, char *pszUrl, AEECallback *pcb, boolean bNeedsHMAC, IPeek *pIBody, ISource **pISource, IWebResp **ppIWebResp, int *piErr); \
   int              (*CheckAdsCert)(iname *po, AEECallback *pcb, int *piErr); \
   int              (*GetAdsCert)(iname *po, char *pszUrl, AEECallback *pcb, int *piErr); \
   boolean          (*IsAutoUpgradeSet)(iname *po); \
   boolean          (*IsAdsCertValid)(iname *po); \
   boolean          (*IsClientRegistered)(iname *po); \
   int              (*RegisterClient)(iname *po, char *pszUrl, AEECallback *pcb, int *piErr); \
   int              (*SetAutoUpgrade)(iname *po); \
   int              (*SmsMAToken) (iname *po, byte *pbSmsMsg, uint32 uiLen); \
   int              (*GetDeviceConfigHeader)(iname *po, char *pszHeader, uint32 *puiLen); \
   int              (*MADeviceConfig)(iname *po, char *pszUrl, AEECallback *pcb, IPeek *pIBody, ISource **pISource, IWebResp **ppIWebResp, int *piErr); \
   int              (*SetHostName)(iname *po, const char *pszHost)
typedef struct IMutualAuth IMutualAuth;

AEEINTERFACE(IMutualAuth) {
   INHERIT_IMutualAuth(IMutualAuth);
};


//=============================================================================
// The interface functions.
//=============================================================================

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define IMUTUALAUTH_AddRef(p)                          AEEGETPVTBL((p),IMutualAuth)->AddRef((p))
#define IMUTUALAUTH_Release(p)                         AEEGETPVTBL((p),IMutualAuth)->Release((p))
#define IMUTUALAUTH_QueryInterface(p,i,ppo)            AEEGETPVTBL((p),IMutualAuth)->QueryInterface((p),(i),(ppo))
// >= BREW 2.2 Mutual Auth & License system Interfaces  

#define IMUTUALAUTH_AdsMutualAuth(p, psz, pcb, b, pib, pis, piw, pi)  AEEGETPVTBL((p),IMutualAuth)->AdsMutualAuth((p), (psz), (pcb), (b), (pib), (pis), (piw), (pi))
#define IMUTUALAUTH_CheckAdsCert(p, pcb, err)          AEEGETPVTBL((p),IMutualAuth)->CheckAdsCert((p), (pcb), (err))
#define IMUTUALAUTH_GetAdsCert(p, psz, pcb, err)       AEEGETPVTBL((p),IMutualAuth)->GetAdsCert((p), (psz), (pcb), (err))
#define IMUTUALAUTH_IsAutoUpgradeSet(p)                AEEGETPVTBL((p),IMutualAuth)->IsAutoUpgradeSet((p))
#define IMUTUALAUTH_IsAdsCertValid(p)                  AEEGETPVTBL((p),IMutualAuth)->IsAdsCertValid((p))
#define IMUTUALAUTH_IsClientRegistered(p)              AEEGETPVTBL((p),IMutualAuth)->IsClientRegistered((p))
#define IMUTUALAUTH_RegisterClient(p, psz, pcb, err)   AEEGETPVTBL((p),IMutualAuth)->RegisterClient((p), (psz), (pcb), (err))
#define IMUTUALAUTH_SetAutoUpgrade(p)                  AEEGETPVTBL((p),IMutualAuth)->SetAutoUpgrade((p))
#define IMUTUALAUTH_SmsMAToken(p, pbrm, len)           AEEGETPVTBL((p),IMutualAuth)->SmsMAToken((p), (pbrm), (len))    
#define IMUTUALAUTH_SetHostName(p, psz)                AEEGETPVTBL((p),IMutualAuth)->SetHostName((p), (psz))

#if 1  //OPTIMIZE_MAHEADER_SIZE

#define IMUTUALAUTH_GetDeviceConfigHeader(p, pszh, puil)            AEEGETPVTBL((p),IMutualAuth)->GetDeviceConfigHeader((p), (pszh), (puil))    
#define IMUTUALAUTH_MADeviceConfig(p, psz, pcb, pib, pis, piw, pi)  AEEGETPVTBL((p),IMutualAuth)->MADeviceConfig((p), (psz), (pcb), (pib), (pis), (piw), (pi))

#endif /* OPTIMIZE_MAHEADER_SIZE */


/*=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IMutualAuth

Description:
   This interface provides access to the BREW Mutual authentication
   mechanism. It is for use ONLY by QUALCOMM, Handset Manufacturer and
   selected partners.

=======================================================================*/

/*===========================================================================

FUNCTION: IMUTUALAUTH_AdsMutualAuth

DESCRIPTION
	This function performs the mutual authentication between the client and ADS.

    Client header
    name=BREWCLIENT
    value= SPDRsequence AU defined below, B64 encoded

    SEQUENCE[AU]
         BLOB8[CST] AES  (aes128/rsa1024/sha1 == 1)
         BLOB8[CRT] certificate serial #
         BLOB8[DN]  certificate DN
         BLOB32[N]nonce value
         BLOB8[PAD] key encrypted with RSA for the cert public key
         BLOB8[ES]  data bytes encrypted as determined by the CST tag

    the databytes in the ES tag get decrypted and then passed into the SPDR 
    parser expecting

    SEQUENCE[CD]   
            BLOB32[TZ]local time offset
            BLOB8[SID]SID
            BLOB32[PL]Platformid
            BLOB8[LN] Current language
            BLOB8[AP] Brew API version
            BLOB8[MIN]min
            BLOB32[ESN] esn 
            BLOB8[RND] Rand value used to compute BKey
            BLOB8[#AK]akeybytes
            BLOB8[#BK]bkeybytes
            BLOB8[RID]ruid value
            
            BLOB8[B??] 0-n client credentials sent "as is"
            BLOB8[B??] 0-n client credentials sent HMAC'ed                       

    The message is authenticated by a 20 byte SHA1 HMAC that is appended to
    the end of the SPDR structure.

    The response header will be as follows

    name=X-BREWADS
    value= (nonce ecrypted with session key) B64 encoded,  
    if successfull value = 1 if unsuccessful

PROTOTYPE:   
    int IMUTUALAUTH_AdsMutualAuth(IMutualAuth  *po, 
                                   char        *pszUrl, 
                                   AEECallback *pcb, 
                                   boolean      bNeedsHMAC,
                                   IPeek        *pIBody,
                                   ISource     **ppISource,
                                   IWebResp    **ppIWebResp,
                                   int         *piErr)
PARAMETERS:
	po:       [in]:     IMutualAuth context. 
    pszUrl    [in]:     Ads Url
    pcb:      [in]:     Ptr to AEE callback. 
    bNeedsHMAC[in]:     HMAC of response required?
    pIBody    [in]:     IPeek of request body (created by IDownload)
    ppISource [in/out]: ISource of response
    ppIWebResp[in/out]: IWebResp of response
    piErr     [in/out]: address of error return code 
DEPENDENCIES
  none

RETURN VALUE
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION: IMUTUALAUTH_CheckAdsCert

DESCRIPTION
	This function retrieves and chains the locally stored ADS cert. 
    
PROTOTYPE:
    int IMUTUALAUTH_CheckAdsCert(IMutualAuth *po, AEECallback *pcb, int *piErr)
PARAMETERS:
	po:   [in]: IMutualAuth context. 
	pcb:  [in]: Ptr to AEE callback. 
    piErr:[in]: address of error return code 	
DEPENDENCIES
  none

RETURN VALUE
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
    none
===========================================================================*/
/*===========================================================================

FUNCTION: IMUTUALAUTH_GetAdsCert

DESCRIPTION
	The ADS cert is retrieved via HTTP, and then validated, chained and stored.

PROTOTYPE:
    int IMUTUALAUTH_GetAdsCert(IMutualAuth *po, char *pszUrl, AEECallback *pcb, int *piErr)
PARAMETERS:
	po:   [in]: IMutualAuth context. 
    pszUrl[in]: Cert Request URL
	pcb:  [in]: Ptr to AEE callback. 
    piErr:[in]: address of error return code 	
DEPENDENCIES
  none

RETURN VALUE
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
    none
===========================================================================*/
/*===========================================================================

FUNCTION IMUTUALAUTH_IsAutoUpgradeSet

DESCRIPTION
   Have we made the 1-way transition to the new MA scheme?

PROTOTYPE:
    boolean IMUTUALAUTH_IsAutoUpgradeSet(IMutualAuth *po)
PARAMETERS:
	po  [in]: IMutualAuthcontext.     
   
DEPENDENCIES
   none
RETURN VALUE
   boolean - TRUE  - use 3.0 (or higher) auth protocol
             FALSE - use pre 3.0 auth protocol
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION IMUTUALAUTH_IsAdsCertValid

DESCRIPTION
   Is the ADS cert valid. That is, it is present, and has it been successfully
   chained to the embedded brew root, using the appropriate criteria.

PROTOTYPE:
    boolean IMUTUALAUTH_IsAdsCertValid(IMutualAuth *po)
PARAMETERS:
	po:       [in]: IMutualAuth context.     
    
DEPENDENCIES
   none
RETURN VALUE
   boolean - TRUE  - cert is valid
             FALSE - cert is invalid, or is not present
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION IMUTUALAUTH_IsClientRegistered

DESCRIPTION
   Is the client registered, or is he required to register/re-register?

PROTOTYPE:
    boolean IMUTUALAUTH_IsClientRegistered(IMutualAuth *po)
PARAMETERS:
	po:       [in]: IMutualAuth context.     
    
DEPENDENCIES
   none
RETURN VALUE
   boolean - TRUE  - client is registered with ADS
             FALSE - client is not yet registered, or is required to re-reg
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION: IMUTUALAUTH_RegisterClient

DESCRIPTION
	This function performs the registration of the handset with the ADS.
    The caller is notified of the result via a callback.

PROTOTYPE:
    IMUTUALAUTH_RegisterClient(IMutualAuth *po, char *pszUrl, AEECallback *pcb, int *piErr)
PARAMETERS:
	po    [in]: IMutualAuth context. 
    pszUrl[in]: Ads URL
	pcb   [in]: Ptr to AEE callback. 
    piErr [in]: address of error return code 
DEPENDENCIES
  Client must have a valid ADS/carrier cert. If not, an error is returned.

RETURN VALUE
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION IMUTUALAUTH_SetAutoUpgrade

DESCRIPTION
   Writes the auto upgrade status to file. This is a 1-off deal.
   Once we move to 3.0 MA, there is no going back. 
   No API available to remove this file.

PROTOTYPE:
    int IMUTUALAUTH_SetAutoUpgrade(IMutualAuth *po)
PARAMETERS:
	po: [in]: IMutualAuth context.     
DEPENDENCIES
   None
RETURN VALUE
   iStatus - SUCCESS
             AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
  See description above
===========================================================================*/
/*===========================================================================

FUNCTION:      IMUTUALAUTH_GetDeviceConfigHeader

DESCRIPTION:   This routine generates a base64, SPDR formatted data blob
               containing the Device configuration data

               Format:

               Header name: "X-BREWDC"

               Contents:

                SEQUENCE[CD]
                    BLOB32[CID]Carrier ID
                    BLOB32[TZ]local time offset
                    BLOB8[SID]SID
                    BLOB32[PL]Platformid
                    BLOB8[LN] Current language
                    BLOB8[AP] Brew API version
                    BLOB8[MIN]MIN
                    BLOB8[DIF] Device Flags
                    BLOB8[MDN]Mobile Directory Number
                    BLOB8[FWR]Firmware Version
                    
DEPENDENCIES:  none

RETURN VALUE:  
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS:  

===========================================================================*/
/*===========================================================================

FUNCTION:      IMUTUALAUTH_MADeviceConfig

DESCRIPTION:   This routine performs an HTTP request with the ADS.
               The request contains the "Device Config" HTTP header.
               The format of the header is detailed below.

               Motivation for introducing this functionality is to reduce
               the performance overhead in browsing the Catalog using 
               full MA.

               Format:

               Header name: "X-BREWDC"

               Contents:

                SEQUENCE[CD]
                    BLOB32[CID]Carrier ID
                    BLOB32[TZ]local time offset
                    BLOB8[SID]SID
                    BLOB32[PL]Platformid
                    BLOB8[LN] Current language
                    BLOB8[AP] Brew API version
                    BLOB8[MIN]MIN
                    BLOB8[DIF] Device Flags
                    BLOB8[MDN]Mobile Directory Number
                    BLOB8[FWR]Firmware Version
                    
DEPENDENCIES:  none

RETURN VALUE:  
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS:  

===========================================================================*/
/*===========================================================================

FUNCTION: IMUTUALAUTH_SmsMAToken

DESCRIPTION
	Handles processing of the SMS MA token used for re-registration.
    The routine is non blocking & all processing is completed in the 
    caller context.

PROTOTYPE:
    int IMUTUALAUTH_SmsMAToken (IMutualAuth *pMe, byte *pbSmsMsg, uint32 uiLen)
PARAMETERS:
	pMe       [in]: IMutualAuth context. 
	pbSmsMsg  [in]: SMS payload
    uiLen     [in]: Length of payload
DEPENDENCIES
  
RETURN VALUE
    int iStatus - SUCCESS
                  AEEMA_MUTUALAUTH_FAILED
SIDE EFFECTS
  none
===========================================================================*/
/*===========================================================================

FUNCTION: IMUTUALAUTH_SetHostName

DESCRIPTION
	Set the hostname that BREWMA will be connected to. Moreover, the hostname 
   will be used to name the certficate file, autoupgrade file and 
   re-register file.

PROTOTYPE:
    int IMUTUALAUTH_SetHostName (IMutualAuth *pMe, const char *pszHost)
PARAMETERS:
	pMe       [in]: IMutualAuth context. 
	pszHost   [in]: The hostname to connect to
DEPENDENCIES
  
RETURN VALUE
    int iStatus - SUCCESS
                  ENOMEMORY
		  EBADPARM
SIDE EFFECTS
  none
===========================================================================*/

#endif /* AEEMUTUALAUTH_H */
