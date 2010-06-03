#ifndef __DATASTATSAPP_H_
#define __DATASTATSAPP_H_

/*=============================================================================

FILE: DataStatsApp.h

SERVICES: Data Diagnostics Applet

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2002 - 2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
						EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/DataStats/DataStatsApp.h#25 $
$DateTime: 2007/06/20 12:12:33 $
$Author: sanjinij $
$Change: 498942 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/15/07   sj       Added support for two flags im_cn flag and dhcp_pcscf flag
02/23/07   jas      Removed unused include file
09/15/06   sj       Featurized pcscf addr changes with FEATURE_IMS
08/08/06   sj       Added support for PCSCF address
08/02/05   sun      Added support for IPV6
05/02/05   RI       Added QoS parameters.
08/23/04   ak       Call id to be in sync with ICM
07/12/04   sun      Added support for 16 profiles and to edit profile name
05/13/04   sun      Fixed the RX TX Display
01/19/04   ram      Use brew extension to set PDP profiles.
12/01/03   ram      Lint related changes.
11/20/03   ram      DDTM Support and remove HDR_ABORT_PPP support
11/18/03   ram      Code cleanup
12/11/03   ram      Endable PDP Profile settings
29/10/03   ram      Integration with ICM and code-cleanup.
07/24/03   ram      Created Data Diagnostics Header File

=============================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE 

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "DATASTATSAPP.BID"
#include "datastatsapp_res.h"

 
#include "AppComFunc.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEImage.h"
#include "AEECM.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
#include "AEEPDP.h"
#endif

#define MAX_NUM_DATA_CALLS 13
#define FLASH_TIME 5000
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



#define MAX_HANDLE_LEN 32
#define MAX_SECURITY_LEN 15
#define MAX_ERROR_STR_LEN 64

//PPP FAX ETC
#define MAX_CALL_SUMMARY_LEN 32 


// SERVICE OPTION6
#define MAX_SYSTEM_INFORMATION_LENGTH 32 

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
// Type of field selected to edit.
#define IP_ADDRESS 0
#define PRIMARY_DNS 1
#define SECONDARY_DNS 2
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)


#ifndef _lint
/*******
 * The following bit of ugliness is needed to allow string pasting of __LINE__ in the
 * following macros.  We have to onvert __LINE__ from an integer value to a string.  
 * We use # to stringify, but it has to be a parameter to use that operator.  But, if
 * we only use a single function then it stringifies the keyword (__FILE__) instead of
 * the value of __FILE__!
 *******/
#define __MAKESTRING2__(x) #x
#define __MAKESTRING1__(x) __MAKESTRING2__(x) 

#ifdef T_WINNT
#define DATASTATS_ERR(x) MSG_HIGH(x " " __FILE__ "(" __MAKESTRING1__(__LINE__) ")", 0, 0, 0)
#define PRINT_EVENT(x) MSG_MED(#x " " __FILE__ "(" __MAKESTRING1__(__LINE__) ")", 0, 0, 0)
#else
#define DATASTATS_ERR(x) MSG_HIGH(x " " __MODULE__ "(" __MAKESTRING1__(__LINE__) ")", 0, 0, 0)
#define PRINT_EVENT(x) MSG_MED(#x " " __MODULE__ "(" __MAKESTRING1__(__LINE__) ")", 0, 0, 0)
#endif /* T_WINNT */
#else
#define DATASTATS_ERR(x) MSG_HIGH(x, 0, 0, 0)
#define PRINT_EVENT(x) MSG_MED(#x, 0, 0, 0)
#endif
/*===========================================================================

TYPE DECLARATIONs

===========================================================================*/

typedef boolean (*PFNDLGHANDLER) (
                                 void *pUser,
                                 AEEEvent evt,
                                 uint16 w,
                                 uint32 dw );
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
typedef struct _AECHARIP
{
    AECHAR IP1[4];
    AECHAR IP2[4];
    AECHAR IP3[4];
    AECHAR IP4[4];
} AECharIP;
#endif

#define EVT_DATASTATS_DLG_RESUME 0x1000

typedef struct _CDataStatsApp 
{
	AEEApplet		 a;
	AEEDeviceInfo	 m_deviceInfo;
	AEERect			 m_rc;				// Applet rectangle

  //Applet specific data.
  ICM               *m_pICM;               // instance of ICM

  AEECMCallInfo *m_pEndedCalls[MAX_NUM_DATA_CALLS];

  //Active Call related data
  AEECMCallID call_id;   	
  uint32	p_mTxHistoryBytes;
  uint32	p_mRxHistoryBytes;
  boolean bHistoryCountSet; //TODO:remove these boolean by setting the value to non-valid?
  uint32 nLastTimeStamp; // Last time when counters were updated.
  char *szSystemInfo;// The mode dependant System information string - "Service Option -6"
  AECHAR *szError; //Error message

  uint32     m_dwRxTotalBytes;
  uint32     m_dwTxTotalBytes;
  uint32     m_dwTxLastCount;
  uint32     m_dwRxLastCount;
  

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)

  IPDP *m_pIPDP;    
  // Profile related data.
  // nItemId for the currently selected profile.
  uint8 selectedProfile; 

  // Title for the Dialog, e.g. "Data Profile 1"
  AECHAR *profileText; 

  // Used to display the IP address in the edit boxes.
  // IP, DNS Primary or DNS Secondary
  int ipTypeSelected;
  AEEPDPType m_pdpType;
  AEEPDPType m_oldPdpType;
  uint32 nIp;
  AEEPDPUMTSPDPAddr nIpv6;
  AEEPDPUMTSPDPAddr nDnsPrimary;
  AEEPDPUMTSPDPAddr nDnsSecondary;
  AEEPDPAuthType nSecurity;

  //Password related.
  byte      *password;/*Passw/secret string */
  byte      *username;/* Username string    */
  // APN String
  AECHAR *apn; 

  // Dialog Tab Control.
  DlgTabCtl *pTabController;
  AEEPDPQOSType  nQoSType;        /* QoS Type - GPRS/UMTS */
  AEEPDPUMTSQOS  m_UMTSQoSReq;    /* QoS UMTS requested   */
  AEEPDPUMTSQOS  m_UMTSQoSMin;    /* QoS UMTS minimum     */
  AEEPDPGPRSQOS  m_GPRSQoSReq;    /* QoS GPRS requested   */
  AEEPDPGPRSQOS  m_GPRSQoSMin;    /* QoS GPRS minimum     */
#ifdef FEATURE_DATA_IMS
  boolean   m_pcscf_addr;         /* Request for pcscf address */
  boolean   m_dhcp_pcscf_addr;    /* Request for pcscf using dhcp */
  boolean m_im_cn_flag;           /* IMCN (IP Multimedia Core Network) Flag */
#endif //FEATURE_DATA_IMS
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)

#ifdef FEATURE_DDTM_CNTL
  AEECMDDTMPref currentDDTM;
#endif
  
  boolean bStaleSummary;
  //Splash screen related.
  boolean splashDrawn;
  int m_cxWidth, m_cyHeight;
  IImage *pSplash;

} CDataStatsApp;

#endif // ifdef __DATASTATSAPP_H_

