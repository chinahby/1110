#ifndef AEECARDSESSION_H
#define AEECARDSESSION_H

/*============================================================================
FILE:  AEECardSession.h

SERVICES:  AEE ICardSession Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc.

        Copyright © 2006-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/Inc/AEECardSession.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   jk      Added Application Label File for OMH
01/22/08   sun     Added ICardSession_Register API
01/17/08   jk      Added files for OMH Rev C
09/17/07   vs      Added 29 EF definitions in the section "EFs in CDMA DF"
                   for the RUIM feature - OMH (Open Market Handset).                   
07/26/06   tml     lint fixes
06/12/06   sun     Fixed Case
05/18/06   sun     Initial release.
=============================================================================*/


#include "AEE.h"

/**********************************************************************

 ICardSession Interface

*********************************************************************/
#define AEECARDSESSION_PIN_LEN                8

#define AEECARDSESSION_MAX_AUTH_DATA             100


/* EFs */
#define AEECARDSESSION_ICCID                      0x00/* ICCID */
#define AEECARDSESSION_ELP                        0x01/* Extended Language Preference */
#define AEECARDSESSION_DIR                        0x02/* EF DIR for USIM */
#define AEECARDSESSION_ARR                        0x03/* Access rule reference */

  /* EFs in CDMA DF*/
#define AEECARDSESSION_CDMA_CC                    0x100/* Call Count */
#define AEECARDSESSION_CDMA_IMSI_M                0x101/* IMSI_M */
#define AEECARDSESSION_CDMA_IMSI_T                0x102/* IMSI_T */
#define AEECARDSESSION_CDMA_TMSI                  0x103/* TMSI */
#define AEECARDSESSION_CDMA_ANALOG_HOME_SID       0x104/* Analog Home SID */
#define AEECARDSESSION_CDMA_ANALOG_OP_PARAMS      0x105/* Analog Operational Parameters */
#define AEECARDSESSION_CDMA_ANALOG_LOCN_AND_REGN_IND 0x106  /* Analog Locn and Regn Indicators */
#define AEECARDSESSION_CDMA_HOME_SID_NID          0x107/* CDMA Home SID and NID */
#define AEECARDSESSION_CDMA_ZONE_BASED_REGN_IND   0x108/* CDMA Zone Based Regn Indicators */
#define AEECARDSESSION_CDMA_SYS_REGN_IND          0x109/* CDMA System/Network Regn Indicators */
#define AEECARDSESSION_CDMA_DIST_BASED_REGN_IND   0x01A /* CDMA Distance Based Regn Indicators */
#define AEECARDSESSION_CDMA_ACCOLC                0x10B/* Access Overload Class */
#define AEECARDSESSION_CDMA_CALL_TERM_MODE_PREF   0x10C/* Call Termination Mode Preferences */
#define AEECARDSESSION_CDMA_SCI                   0x10D/* Suggested Slot Cycle Index */
#define AEECARDSESSION_CDMA_ANALOG_CHAN_PREF      0x10E/* Analog Channel Preferences */
#define AEECARDSESSION_CDMA_PRL                   0x10F/* Preferred Roaming List */
#define AEECARDSESSION_CDMA_RUIM_ID               0x110/* Removable UIM ID */
#define AEECARDSESSION_CDMA_CDMA_SVC_TABLE        0x111/* CDMA Service Table */
#define AEECARDSESSION_CDMA_SPC                   0x112/* Service Programming Code */
#define AEECARDSESSION_CDMA_OTAPA_SPC_ENABLE      0x113/* OTAPA/SPC-Enable */
#define AEECARDSESSION_CDMA_NAM_LOCK              0x114 /* NAM-Lock */
#define AEECARDSESSION_CDMA_OTASP_OTAPA_FEATURES  0x115/* OTASP-OTAPA Parameters */
#define AEECARDSESSION_CDMA_SERVICE_PREF          0x116/* Service Preferences */
#define AEECARDSESSION_CDMA_ESN_ME                0x117/* ESN_ME */
#define AEECARDSESSION_CDMA_RUIM_PHASE            0x118/* R-UIM Phase */
#define AEECARDSESSION_CDMA_PREF_LANG             0x119/* Preferred Languages */
#define AEECARDSESSION_CDMA_UNASSIGNED_1          0x11A/* Not assigned yet */
#define AEECARDSESSION_CDMA_SMS                   0x11B/* Short Messages */
#define AEECARDSESSION_CDMA_SMS_PARAMS            0x11C/* Short Message Service Parameters */
#define AEECARDSESSION_CDMA_SMS_STATUS            0x11D/* SMS Status */
#define AEECARDSESSION_CDMA_SUP_SVCS_FEATURE_CODE_TABLE 0x11E/* Sup Services Feature Code Table */
#define AEECARDSESSION_CDMA_UNASSIGNED_2          0x11F/* Not assigned yet */
#define AEECARDSESSION_CDMA_HOME_SVC_PVDR_NAME    0x120/* CDMA Home Service Provider Name */
#define AEECARDSESSION_CDMA_UIM_ID_USAGE_IND      0x121/* R-UIM ID usage indicator */
#define AEECARDSESSION_CDMA_ADM_DATA              0x122/* CDMA Administrative Data */
#define AEECARDSESSION_CDMA_MSISDN                0x123/* Mobile Directory Number */
#define AEECARDSESSION_CDMA_MAXIMUM_PRL           0x124/* Max size of the PRL */
#define AEECARDSESSION_CDMA_SPC_STATUS            0x125/* SPC Status */
#define AEECARDSESSION_CDMA_ECC                   0x126/* Emergency Call Codes */
#define AEECARDSESSION_CDMA_3GPD_ME3GPDOPC        0x127/* Operational Capabilities */
#define AEECARDSESSION_CDMA_3GPD_3GPDOPM          0x128/* Operational Mode */
#define AEECARDSESSION_CDMA_3GPD_SIPCAP           0x129/* Simple IP Capabilities */
#define AEECARDSESSION_CDMA_3GPD_MIPCAP           0x12A/* Mobile IP Capabilities */
#define AEECARDSESSION_CDMA_3GPD_SIPUPP           0x12B/* Simple IP User Profile Parameters */
#define AEECARDSESSION_CDMA_3GPD_MIPUPP           0x12C/* Mobile IP User Profile Parameters */
#define AEECARDSESSION_CDMA_3GPD_SIPSP            0x12D/* Simple IP Status Parameters */
#define AEECARDSESSION_CDMA_3GPD_MIPSP            0x12E/* Mobile IP Status Parameters */
#define AEECARDSESSION_CDMA_3GPD_SIPPAPSS         0x12F/* Simple IP PAP SS Parameters */
#define AEECARDSESSION_CDMA_UNASSIGNED_3          0x130/* Reserved */
#define AEECARDSESSION_CDMA_UNASSIGNED_4          0x131/* Reserved */
#define AEECARDSESSION_CDMA_PUZL                  0x132/* Preffered User Zone List */
#define AEECARDSESSION_CDMA_MAXPUZL               0x133/* Maximum PUZL */
#define AEECARDSESSION_CDMA_MECRP                 0x134/* ME-specific Config REquest Param */
#define AEECARDSESSION_CDMA_HRPDCAP               0x135/* HRPD Access Auth Capability Param */
#define AEECARDSESSION_CDMA_HRPDUPP               0x136/* HRPD Access Auth User Profile Param */
#define AEECARDSESSION_CDMA_CSSPR                 0x137/* CUR_SSPR_P_REV */
#define AEECARDSESSION_CDMA_ATC                   0x138/* Acceess Terminal Class */
#define AEECARDSESSION_CDMA_EPRL                  0x139/* Extended Preffered Roaming List */
#define AEECARDSESSION_CDMA_USER_AGENT_STRING     0x13A/* KDDI Agent String */
#define AEECARDSESSION_CDMA_GID2                  0x13B/* KDDI GID2 */
#define AEECARDSESSION_CDMA_GID1                  0x13C/* KDDI GID1 */
#define AEECARDSESSION_CDMA_GID_REFERENCES        0x13D/* KDDI GID References */
#define AEECARDSESSION_CDMA_IMSI_STATUS           0x13E/* KDDI IMSI Status */
#define AEECARDSESSION_CDMA_ME_DOWNLOADABLE_DATA  0x13F/* KDDI ME Downloadable Data */
#define AEECARDSESSION_CDMA_ME_SETTING_DATA       0x140/* KDDI ME Setting Date */
#define AEECARDSESSION_CDMA_ME_USER_DATA          0x141/* KDDI User Data */
#define AEECARDSESSION_CDMA_RESERVED2             0x142/* KDDI Reserved */
#define AEECARDSESSION_CDMA_RESERVED1             0x143/* KDDI Reserved */
#define AEECARDSESSION_CDMA_UIM_SVC_TABLE         0x144/* KDDI UIM Service Table */
#define AEECARDSESSION_CDMA_UIM_FEATURES          0x145/* KDDI UIM Features */
#define AEECARDSESSION_CDMA_UIM_VERSION           0x146/* KDDI UIM Version */
#define AEECARDSESSION_CDMA_BCSMS_CONFIG          0x147/* Broadcast SMS Configuration */
#define AEECARDSESSION_CDMA_BCSMS_PREF            0x148/* Broadcast SMS Preferences */
#define AEECARDSESSION_CDMA_BCSMS_TABLE           0x149/* Broadcast SMS Table */ 
#define AEECARDSESSION_CDMA_BCSMS_PARAMS          0x14A/* Broadcast SMS Paramaters */
#define AEECARDSESSION_CDMA_MMS_NOTIF             0x14B/* MMS Notification */
#define AEECARDSESSION_CDMA_MMS_EXT8              0x14C/* MMS Extension 8 */
#define AEECARDSESSION_CDMA_MMS_ICP               0x14D/* MMS Issuer Connectivity Parameters */
#define AEECARDSESSION_CDMA_MMS_UP                0x14E/* MMS User Preferences */
#define AEECARDSESSION_CDMA_SMS_CAP               0x14F/* SMS Capabilities */
#define AEECARDSESSION_CDMA_3GPD_IPV6CAP          0x150/* IPv6 Capabilities */
#define AEECARDSESSION_CDMA_3GPD_MIPFLAGS         0x151/* Mobile IP Flags */
#define AEECARDSESSION_CDMA_3GPD_TCPCONFIG        0x152/* TCP Configurations */
#define AEECARDSESSION_CDMA_3GPD_DGC              0x153/* Data Generic Configurations */
#define AEECARDSESSION_CDMA_BROWSER_CP            0x154/* Browser Connectivity Parameters */
#define AEECARDSESSION_CDMA_BROWSER_BM            0x155/* Browser Bookmarks */
#define AEECARDSESSION_CDMA_3GPD_SIPUPPEXT        0x156/* Simple IP User Profile Parameters Extension */
#define AEECARDSESSION_CDMA_MMS_CONFIG            0x157/* MMS Configuration */
#define AEECARDSESSION_CDMA_JAVA_DURL             0x158/* Java Download URL */
#define AEECARDSESSION_CDMA_3GPD_MIPUPPEXT        0x159/* Mobile IP User Profile Parameters Extension */ 
#define AEECARDSESSION_CDMA_BREW_DLOAD            0x15A/* BREW Download */
#define AEECARDSESSION_CDMA_BREW_TSID             0x15B/* BREW Teleservice ID */
#define AEECARDSESSION_CDMA_BREW_SID              0x15C/* BREW Subscriber ID */ 
#define AEECARDSESSION_CDMA_LBS_XCONFIG           0x15D/* LBS XTRA Configuration */
#define AEECARDSESSION_CDMA_LBS_XSURL             0x15E/* LBS XTRA Server URLs */
#define AEECARDSESSION_CDMA_LBS_V2CONFIG          0x15F/* LBS V2 Configuration */
#define AEECARDSESSION_CDMA_LBS_V2PDEADDR         0x160/* LBS V2 PDE Address */
#define AEECARDSESSION_CDMA_LBS_V2MPCADDR         0x161/* LBS V2 MPC Address */ 
#define AEECARDSESSION_CDMA_BREW_AEP              0x162/* Application Execution Policy */
#define AEECARDSESSION_CDMA_MODEL                 0x163/* Model Info */
#define AEECARDSESSION_CDMA_RC                    0x164/* Root Certificate */
#define AEECARDSESSION_CDMA_APP_LABELS            0x165/* Application Labels */

#define AEECARDSESSION_CDMA_HRPD_HRPDUPP          0x166/* HRPD User Profile Parameters */
#define AEECARDSESSION_CDMA_SF_EUIM_ID            0x167/* Short Form EUIMID */

 /* EFs in DF GSM */
#define AEECARDSESSION_GSM_LP                     0x200/* Language Preference */
#define AEECARDSESSION_GSM_IMSI                   0x201/* IMSI */
#define AEECARDSESSION_GSM_KC                     0x202/* Ciphering Key Kc */
#define AEECARDSESSION_GSM_PLMN                   0x203/* PLMN selector */
#define AEECARDSESSION_GSM_HPLMN                  0x204/* HPLMN search period */
#define AEECARDSESSION_GSM_ACM_MAX                0x205/* ACM Maximum value */
#define AEECARDSESSION_GSM_SST                    0x206/* SIM Service table */
#define AEECARDSESSION_GSM_ACM                    0x207/* Accumulated call meter */
#define AEECARDSESSION_GSM_GID1                   0x208/* Group Identifier Level 1 */
#define AEECARDSESSION_GSM_GID2                   0x209/* Group Identifier Level 2 */
#define AEECARDSESSION_GSM_SPN                    0x20A/* Service Provider Name */
#define AEECARDSESSION_GSM_PUCT                   0x20B/* Price Per Unit and currency table */
#define AEECARDSESSION_GSM_CBMI                   0x20C/* Cell broadcast message identifier selection */
#define AEECARDSESSION_GSM_BCCH                   0x20D/* Broadcast control channels */
#define AEECARDSESSION_GSM_ACC                    0x20E/* Access control class */
#define AEECARDSESSION_GSM_FPLMN                  0x20F/* Forbidden PLMNs */
#define AEECARDSESSION_GSM_LOCI                   0x210/* Location information */
#define AEECARDSESSION_GSM_AD                     0x211/* Administrative data */
#define AEECARDSESSION_GSM_PHASE                  0x212/* Phase identification */
#define AEECARDSESSION_GSM_VGCS                   0x213/* Voice Group Call service */
#define AEECARDSESSION_GSM_VGCSS                  0x214/* Voice Group Call service status */
#define AEECARDSESSION_GSM_VBS                    0x215/* Voice Broadcast service  */
#define AEECARDSESSION_GSM_VBSS                   0x216/* Voice Broadcast service status */
#define AEECARDSESSION_GSM_EMLPP                  0x217/* Enhanced multi level pre-emption and priority */
#define AEECARDSESSION_GSM_AAEM                   0x218/* Automatic Answer for eMLPP service */
#define AEECARDSESSION_GSM_CBMID                  0x219/* Cell Broadcast Message id for data dload */
#define AEECARDSESSION_GSM_ECC                    0x21A/* Emergency Call Codes */
#define AEECARDSESSION_GSM_CBMIR                  0x21B/* Cell Broadcast Message id range selection */
#define AEECARDSESSION_GSM_DCK                    0x21C/* De-personalization control keys */
#define AEECARDSESSION_GSM_CNL                    0x21D/* Co-operative network list */
#define AEECARDSESSION_GSM_NIA                    0x21E/* Network's indication of alerting */
#define AEECARDSESSION_GSM_KCGPRS                 0x21F/* GPRS ciphering key */
#define AEECARDSESSION_GSM_LOCIGPRS               0x220/* GPRS location information */
#define AEECARDSESSION_GSM_SUME                   0x221/* Setup Menu elements */
#define AEECARDSESSION_GSM_PLMNWACT               0x222/* PLMN Selector with Access technology */
#define AEECARDSESSION_GSM_OPLMNWACT              0x223/* Operator controlled PLMNWACT */
#define AEECARDSESSION_GSM_HPLMNACT               0x224/* HPLNMN Access technology */
#define AEECARDSESSION_GSM_CPBCCH                 0x225/* CPBCCH information */
#define AEECARDSESSION_GSM_INVSCAN                0x226/* Investigation PLMN Scan */
#define AEECARDSESSION_GSM_RPLMNAT                0x227/* RPLMN  Last used Access Technology */
#define AEECARDSESSION_GSM_VMWI                   0x228/* CPHS: Voice Mail Waiting Indicator */
#define AEECARDSESSION_GSM_SVC_STR_TBL            0x229/* CPHS: Service String Table */
#define AEECARDSESSION_GSM_CFF                    0x22A/* CPHS: Call Forwarding Flag */
#define AEECARDSESSION_GSM_ONS                    0x22B/* CPHS: Operator Name String */
#define AEECARDSESSION_GSM_CSP                    0x22C/* CPHS: Customer Service Profile */
#define AEECARDSESSION_GSM_CPHSI                  0x22D/* CPHS: CPHS Information */
#define AEECARDSESSION_GSM_MN                     0x22E/* CPHS: Mailbox Number */
#define AEECARDSESSION_SIM_7F40_PROP1_DFS         0x22F/* Dynamic Flags Status */
#define AEECARDSESSION_SIM_7F40_PROP1_D2FS        0x230/* Dynamic2 Flag Setting */
#define AEECARDSESSION_SIM_7F40_PROP1_CSP2        0x231/* Customer Service Profile Line2*/
         /* EFs at SoLSA */
#define AEECARDSESSION_GSM_SAI                    0x232/* SoLSA access indicator */
#define AEECARDSESSION_GSM_SLL                    0x233/* SoLSA LSA list */
         /* EFs at MExE Level */
#define AEECARDSESSION_GSM_MEXE_ST                0x234/* MExE Service table */
#define AEECARDSESSION_GSM_ORPK                   0x235/* Operator Root Public Key */
#define AEECARDSESSION_GSM_ARPK                   0x236/* Administrator Root Public Key */
#define AEECARDSESSION_GSM_TPRPK                  0x237/* Third party Root public key */
#define AEECARDSESSION_GSM_MBDN                   0x238/* Mailbox Dialing Number*/
#define AEECARDSESSION_GSM_EXT6                   0x239/* Extension 6*/
#define AEECARDSESSION_GSM_MBI                    0x23A/* Mail Box Identifier*/
#define AEECARDSESSION_GSM_MWIS                   0x23B/* Message Waiting Indication Status*/
#define AEECARDSESSION_GSM_EXT1                   0x23C/* Extension 1*/
#define AEECARDSESSION_GSM_SPDI                   0x23D/* Service Provider Display Information*/
  /* image file */
#define AEECARDSESSION_GSM_IMG                    0x23E
#define AEECARDSESSION_GSM_IMG1INST1              0x23F 
#define AEECARDSESSION_GSM_IMG1INST2              0x241 
#define AEECARDSESSION_GSM_IMG1INST3              0x242
#define AEECARDSESSION_GSM_IMG2INST1              0x243
#define AEECARDSESSION_GSM_IMG2INST2              0x244
#define AEECARDSESSION_GSM_IMG2INST3              0x245
#define AEECARDSESSION_GSM_IMG3INST1              0x246
#define AEECARDSESSION_GSM_IMG3INST2              0x247
#define AEECARDSESSION_GSM_IMG3INST3              0x248
  /* EFs at the telecom DF in GSM and CDMA */
#define AEECARDSESSION_TELECOM_ADN                0x300/* Abbrev Dialing Numbers */
#define AEECARDSESSION_TELECOM_FDN                0x301/* Fixed dialling numbers */
#define AEECARDSESSION_TELECOM_SMS                0x302/* Short messages */
#define AEECARDSESSION_TELECOM_CCP                0x303/* Capability Configuration Parameters */
#define AEECARDSESSION_TELECOM_ECCP               0x304/* Extended CCP */
#define AEECARDSESSION_TELECOM_MSISDN             0x305/* MSISDN */
#define AEECARDSESSION_TELECOM_SMSP               0x306/* SMS parameters */
#define AEECARDSESSION_TELECOM_SMSS               0x307/* SMS Status */
#define AEECARDSESSION_TELECOM_LND                0x308/* Last number dialled */
#define AEECARDSESSION_TELECOM_SDN                0x309/* Service Dialling numbers */
#define AEECARDSESSION_TELECOM_EXT1               0x30A/* Extension 1 */
#define AEECARDSESSION_TELECOM_EXT2               0x30B/* Extension 2 */
#define AEECARDSESSION_TELECOM_EXT3               0x30C/* Extension 3 */
#define AEECARDSESSION_TELECOM_BDN                0x30D/* Barred Dialing Numbers */
#define AEECARDSESSION_TELECOM_EXT4               0x30E/* Extension 4 */
#define AEECARDSESSION_TELECOM_SMSR               0x30F/* SMS reports */
#define AEECARDSESSION_TELECOM_CMI                0x311/* Comparison Method Information */
#define AEECARDSESSION_TELECOM_SUME               0x312/* Setup Menu elements */
#define AEECARDSESSION_TELECOM_ARR                0x313/* Access Rule reference */

  /* EFs at DF GRAPHICS under DF Telecom */
#define AEECARDSESSIONIMAGE_FILE                  0x314/* Image instance data files */
  /* EFs at the DF PHONEBOOK under  DF Telecom */
#define AEECARDSESSION_TELECOM_PBR                0x315/* Phone book reference file */
#define AEECARDSESSION_TELECOM_CCP1               0x316/* Capability Configuration parameters 1 */
#define AEECARDSESSION_TELECOM_UID                0x317/* Unique Identifier */
#define AEECARDSESSION_TELECOM_PSC                0x318/* Phone book synchronization center */
#define AEECARDSESSION_TELECOM_CC                 0x319/* Change counter */
#define AEECARDSESSION_TELECOM_PUID               0x31A/* Previous Unique Identifier */
  /* EFs of USIM ADF */
#define AEECARDSESSION_USIM_LI                    0x400/* Language Indication */
#define AEECARDSESSION_USIM_IMSI                  0x401/* IMSI */
#define AEECARDSESSION_USIM_KEYS                  0x402/* Ciphering and Integrity keys */
#define AEECARDSESSION_USIM_KEYSPS                0x403/* C and I keys for packet switched domain */
#define AEECARDSESSION_USIM_PLMNWACT              0x404/* User controlled PLMN selector with access tech */
#define AEECARDSESSION_USIM_UPLMNSEL              0x405/* UPLMN selector */
#define AEECARDSESSION_USIM_HPLMN                 0x406/* HPLMN search period */
#define AEECARDSESSION_USIM_ACM_MAX               0x407/* ACM maximum value */
#define AEECARDSESSION_USIM_UST                   0x408/* USIM Service table */
#define AEECARDSESSION_USIM_ACM                   0x409/* Accumulated Call meter */
#define AEECARDSESSION_USIM_GID1                  0x40A/* Group Identifier Level  */
#define AEECARDSESSION_USIM_GID2                  0x40B/* Group Identifier Level 2 */
#define AEECARDSESSION_USIM_SPN                   0x40C/* Service Provider Name */
#define AEECARDSESSION_USIM_PUCT                  0x40D/* Price Per Unit and Currency table */
#define AEECARDSESSION_USIM_CBMI                  0x40E/* Cell Broadcast Message identifier selection */
#define AEECARDSESSION_USIM_ACC                   0x40F/* Access control class */
#define AEECARDSESSION_USIM_FPLMN                 0x410/* Forbidden PLMNs */
#define AEECARDSESSION_USIM_LOCI                  0x411/* Location information */
#define AEECARDSESSION_USIM_AD                    0x412/* Administrative data */
#define AEECARDSESSION_USIM_CBMID                 0x413/* Cell Broadcast msg identifier for data download */
#define AEECARDSESSION_USIM_ECC                   0x414/* Emergency call codes */
#define AEECARDSESSION_USIM_CBMIR                 0x415/* Cell broadcast msg identifier range selection */
#define AEECARDSESSION_USIM_PSLOCI                0x416/* Packet switched location information */
#define AEECARDSESSION_USIM_FDN                   0x417/* Fixed dialling numbers */
#define AEECARDSESSION_USIM_SMS                   0x418/* Short messages */
#define AEECARDSESSION_USIM_MSISDN                0x419/* MSISDN */
#define AEECARDSESSION_USIM_SMSP                  0x41A/* SMS parameters */
#define AEECARDSESSION_USIM_SMSS                  0x41B/* SMS Status */
#define AEECARDSESSION_USIM_SDN                   0x41C/* Service dialling numbers */
#define AEECARDSESSION_USIM_EXT2                  0x41D/* Extension 2 */
#define AEECARDSESSION_USIM_EXT3                  0x41E/* Extension 3 */
#define AEECARDSESSION_USIM_SMSR                  0x41F/* SMS reports */
#define AEECARDSESSION_USIM_ICI                   0x421/* Incoming call information */
#define AEECARDSESSION_USIM_OCI                   0x422/* Outgoing call information */
#define AEECARDSESSION_USIM_ICT                   0x423/* Incoming call timer */
#define AEECARDSESSION_USIM_OCT                   0x424/* Outgoing call timer */
#define AEECARDSESSION_USIM_EXT5                  0x425/* Extension 5 */
#define AEECARDSESSION_USIM_CCP2                  0x426/* Capability Configuration Parameters 2 */
#define AEECARDSESSION_USIM_EMLPP                 0x427/* Enhanced Multi Level Precedence and Priority */
#define AEECARDSESSION_USIM_AAEM                  0x428/* Automatic answer for eMLPP service */
#define AEECARDSESSION_USIM_GMSI                  0x429/* Group identity */
#define AEECARDSESSION_USIM_HIDDENKEY             0x42A/* key for hidden phonebook entries */
#define AEECARDSESSION_USIM_BDN                   0x42B/* Barred dialling numbers */
#define AEECARDSESSION_USIM_EXT4                  0x42C/* Extension 4 */
#define AEECARDSESSION_USIM_CMI                   0x42D/* Comparison Method information */
#define AEECARDSESSION_USIM_EST                   0x42E/* Enabled services table */
#define AEECARDSESSION_USIM_ACL                   0x42F/* Access Point Name Control List */
#define AEECARDSESSION_USIM_DCK                   0x430/* De-personalization Control Keys */
#define AEECARDSESSION_USIM_CNL                   0x431/* Co-operative network list */
#define AEECARDSESSION_USIM_START_HFN             0x432/* Init values for Hyper-frame number */
#define AEECARDSESSION_USIM_THRESHOLD             0x433/* Max value of START */
#define AEECARDSESSION_USIM_OPLMNWACT             0x434/* Operator controlled PLMN sel with access tech */
#define AEECARDSESSION_USIM_OPLMNSEL              0x435/* OPLMN selector */
#define AEECARDSESSION_USIM_HPLMNWACT             0x436/* HPLMN selector with access tech */
#define AEECARDSESSION_USIM_ARR                   0x437/* Access Rule reference */
#define AEECARDSESSION_USIM_RPLMNACT              0x438/* RPLMN last used access tech */
#define AEECARDSESSION_USIM_NETPAR                0x439/* Network parameters */
#define AEECARDSESSION_USIM_VMWI                  0x43A/* CPHS: Voice Mail Waiting Indicator */
#define AEECARDSESSION_USIM_SVC_STR_TBL           0x43B/* CPHS: Service String Table */
#define AEECARDSESSION_USIM_CFF                   0x43C/* CPHS: Call Forwarding Flag */
#define AEECARDSESSION_USIM_ONS                   0x43D/* CPHS: Operator Name String */
#define AEECARDSESSION_USIM_CSP                   0x43E/* CPHS: Customer Service Profile */
#define AEECARDSESSION_USIM_CPHSI                 0x43F/* CPHS: CPHS Information */
#define AEECARDSESSION_USIM_MN                    0x441/* CPHS: Mailbox Number */
  /* EFs of ORANGE DF under USIM ADF */
#define AEECARDSESSION_USIM_7F40_PROP1_DFS        0x442/* Dynamic Flags Status */
#define AEECARDSESSION_USIM_7F40_PROP1_D2FS       0x443/* Dynamic2 Flag Setting */
#define AEECARDSESSION_USIM_7F40_PROP1_CSP2       0x444/* Customer Service Profile Line2*/
  /*EFs at DF PHONEBOOK under USIM ADF */
#define AEECARDSESSION_USIM_PBR                   0x445/* Phone book reference file */
#define AEECARDSESSION_USIM_CCP1                  0x446/* Capability Configuration parameters 1 */
#define AEECARDSESSION_USIM_UID                   0x447/* Unique Identifier */
#define AEECARDSESSION_USIM_PSC                   0x448/* Phone book synchronization center */
#define AEECARDSESSION_USIM_CC                    0x449/* Change counter */
#define AEECARDSESSION_USIM_PUID                  0x44A/* Previous Unique Identifier */
  /*EFs at DF GSM under USIM ADF */
#define AEECARDSESSION_USIM_KC                    0x44B/* GSM ciphering key Kc */
#define AEECARDSESSION_USIM_KCGPRS                0x44C/* GPRS ciphering key */
#define AEECARDSESSION_USIM_CPBCCH                0x44D/* CPBCCH information */
#define AEECARDSESSION_USIM_INVSCAN               0x44E/* Investigation scan */
  /*EFs at DF MEXe under USIM ADF */
#define AEECARDSESSION_USIM_MEXE_ST               0x44F/* MExE Service table */
#define AEECARDSESSION_USIM_ORPK                  0x450/* Operator Root Public Key */
#define AEECARDSESSION_USIM_ARPK                  0x451/* Administrator Root Public Key */
#define AEECARDSESSION_USIM_TPRPK                 0x452/* Third party Root public key */
#define AEECARDSESSION_USIM_MBDN                  0x453/* Mailbox Dialing Number*/
#define AEECARDSESSION_USIM_EXT6                  0x454/* Extension 6 */
#define AEECARDSESSION_USIM_MBI                   0x455/* Mailbox Identifier*/
#define AEECARDSESSION_USIM_MWIS                  0x456/* Message Waiting Indication Status*/
#define AEECARDSESSION_USIM_SPDI                  0x457/* Service Provider Display Information*/
#define AEECARDSESSION_USIM_EHPLMN                0x458/* Equivalent HPLMN */
  /* EFs of DCS1800 DF */
#define AEECARDSESSIONDCS1800_IMSI                0x459/* Imsi */
#define AEECARDSESSIONDCS1800_KC                  0x45A/* Ciphering Key Kc */
#define AEECARDSESSIONDCS1800_PLMN                0x45B/* PLMN selector */
#define AEECARDSESSIONDCS1800_SST                 0x45C/* SIM Service table */
#define AEECARDSESSIONDCS1800_BCCH                0x45F/* Broadcast control channels */
#define AEECARDSESSIONDCS1800_ACC                 0x460/* Access control class */
#define AEECARDSESSIONDCS1800_FPLMN               0x461/* Forbidden PLMNs */
#define AEECARDSESSIONDCS1800_LOCI                0x462/* Location information */
#define AEECARDSESSIONDCS1800_AD                  0x463/* Administrative data */
  /* EFs selected by path at the telecom DF in GSM and CDMA */
#define AEECARDSESSION_TELECOM_IAP                0x464/* Index Administration File */
#define AEECARDSESSION_TELECOM_PBC                0x465/* Phone Book Control */
#define AEECARDSESSION_TELECOM_GRP                0x466/* Grouping File */
#define AEECARDSESSION_TELECOM_AAS                0x467/* Additional Number Alpha String */
#define AEECARDSESSION_TELECOM_GAS                0x468/* Grouping Information Alpha String */
#define AEECARDSESSION_TELECOM_ANR                0x469/* Additional Number */
#define AEECARDSESSION_TELECOM_SNE                0x46A/* Secondary Name Entry */
#define AEECARDSESSION_TELECOM_EMAIL              0x46B/* Email */
#define AEECARDSESSION_TELECOM_ANRA               0x46C/* Additional Number A */
#define AEECARDSESSION_TELECOM_ANRB               0x46D/* Additional Number B */
#define AEECARDSESSION_TELECOM_ANR1               0x46E/* Additional Number C */
#define AEECARDSESSION_TELECOM_ANRA1              0x46F/* Additional Number A1 */
#define AEECARDSESSION_TELECOM_ANRB1              0x470/* Additional Number B1 */
#define AEECARDSESSION_TELECOM_ADN1               0x471/* Abbreviated Dialing Numbers 1 */
#define AEECARDSESSION_TELECOM_PBC1               0x472/* Phone Book Control 1 */
#define AEECARDSESSION_TELECOM_GRP1               0x473/* Grouping File 1 */
#define AEECARDSESSION_TELECOM_SNE1               0x474/* Secondary Name Entry 1 */
#define AEECARDSESSION_TELECOM_UID1               0x475/* Unique Identifier 1 */
#define AEECARDSESSION_TELECOM_EMAIL1             0x476/* Email 1 */
#define AEECARDSESSION_TELECOM_IAP1               0x477/* Index Administration File 1 */
  /* EFs selected by path of USIM ADF */
#define AEECARDSESSION_USIM_ADN                   0x478/* Abbreviated Dialing Numbers */
#define AEECARDSESSION_USIM_IAP                   0x479/* Index Administration File */
#define AEECARDSESSION_USIM_PBC                   0x47A/* Phone Book Control */
#define AEECARDSESSION_USIM_EXT1                  0x47B/* Extension 1 */
#define AEECARDSESSION_USIM_GRP                   0x47C/* Grouping File */
#define AEECARDSESSION_USIM_AAS                   0x47D/* Additional Number Alpha String */
#define AEECARDSESSION_USIM_AAS1                  0x47E/* Additional Number Alpha String 1 */
#define AEECARDSESSION_USIM_GAS                   0x47F/* Grouping Information Alpha String */
#define AEECARDSESSION_USIM_GAS1                  0x480/* Grouping Information Alpha String */
#define AEECARDSESSION_USIM_ANR                   0x481/* Additional Number */
#define AEECARDSESSION_USIM_SNE                   0x482/* Secondary Name Entry */
#define AEECARDSESSION_USIM_EMAIL                 0x483/* Email */
#define AEECARDSESSION_USIM_ANRA                  0x484/* Additional Number A */
#define AEECARDSESSION_USIM_ANRB                  0x485/* Additional Number B */
#define AEECARDSESSION_USIM_ANRC                  0x486/* Additional Number C */
#define AEECARDSESSION_USIM_ANR1                  0x487/* Additional Number 1 */
#define AEECARDSESSION_USIM_ANRA1                 0x488/* Additional Number A1 */
#define AEECARDSESSION_USIM_ANRB1                 0x489/* Additional Number B1 */
#define AEECARDSESSION_USIM_ANRC1                 0x48A/* Additional Number C1 */
#define AEECARDSESSION_USIM_ADN1                  0x48B/* Abbreviated Dialing Numbers 1 */
#define AEECARDSESSION_USIM_PBC1                  0x48C/* Phone Book Control 1 */
#define AEECARDSESSION_USIM_GRP1                  0x48D/* Grouping File 1 */
#define AEECARDSESSION_USIM_SNE1                  0x48E/* Secondary Name Entry 1 */
#define AEECARDSESSION_USIM_UID1                  0x48F/* Unique Identifier 1 */
#define AEECARDSESSION_USIM_EMAIL1                0x490/* Email 1 */
#define AEECARDSESSION_USIM_IAP1                  0x491/* Index Administration File 1 */
  /* EFs of WIM DF */
#define AEECARDSESSIONWIM_ODF                     0x492/* WIM ODF */
#define AEECARDSESSIONWIM_TOKEN_INFO              0x493/* Generic token info and capabilities */
#define AEECARDSESSIONWIM_UNUSED_SPACE            0x494/* Unused Space */
  /* EFs of ORANGE DF */
#define AEECARDSESSIONORANGE_DFS                  0x495/* Dynamic Flags Status */
#define AEECARDSESSIONORANGE_D2FS                 0x496/* Dynamic2 Flag Setting */
#define AEECARDSESSIONORANGE_CSP2                 0x497/* Customer Service Profile Line2*/
#define AEECARDSESSIONORANGE_PARAMS               0x498/* Welcome Message*/
#define AEECARDSESSION_GSM_CINGULAR_ACT_HPLMN     0x499/*Acting HPLMN*/
#define AEECARDSESSION_GSM_CINGULAR_SPT_TABLE     0x49A/*Support Table*/
#define AEECARDSESSION_GSM_PNN                    0x49B/* PLMN Network Name */
#define AEECARDSESSION_GSM_OPL                    0x49C/* Operator List */
#define AEECARDSESSION_USIM_PNN                   0x49D/* CPHS: PLMN Network Name */
#define AEECARDSESSION_USIM_OPL                   0x49E/* CPHS: Operator List */


#define AEECARDSESSION_EF_FILE_NAME               1
#define AEECARDSESSION_DF_FILE_NAME               2
#define AEECARDSESSION_FILE_PATH                  3


#define AEECARDSESSION_FILE_ID_LEN                2

/* This indicates if the file is MF, DF or EF */
#define  AEECARDSESSION_FTYPE_MASTER_FILE         1
#define  AEECARDSESSION_FTYPE_DEDICATED_FILE      2            
#define  AEECARDSESSION_FTYPE_ELEMENTARY_FILE     3

/* This indicates the EF structure */
#define  AEECARDSESSION_FSTRUCTURE_TRANSPARENT    4
#define  AEECARDSESSION_FSTRUCTURE_LINEAR         5
#define  AEECARDSESSION_FSTRUCTURE_CYCLIC         6
 
/* The various PIN operations that ICardSession supports */
#define AEECARDSESSION_PIN_VERIFY_OP      1
#define AEECARDSESSION_PIN_ENABLE_OP      2
#define AEECARDSESSION_PIN_DISABLE_OP     3
#define AEECARDSESSION_PIN_UNBLOCK_OP     4
#define AEECARDSESSION_PIN_CHANGE_OP      5

/* The below #defines provide the various PIN status information 
** that is applicable to all the different types of PINs.  
** The status of PIN does not include an action resulting from a 
** PIN operation, e.g., PIN_UNBLOCKED, PIN_CHANGED.  
*/
#define AEECARDSESSION_PIN_NOT_INITIALIZED            0
#define AEECARDSESSION_PIN_ENABLED_VERIFIED           1
#define AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED       2
#define AEECARDSESSION_PIN_DISABLED                   3
#define AEECARDSESSION_PIN_BLOCKED                    4
#define AEECARDSESSION_PIN_PERM_BLOCKED               5


/* The below #defines provide the various PIN ID that application will have access to.
** PIN 1 and PIN 2 will be application specific in USIM, CSIM and other UICC applications
** PIN 1 and PIN 2 will translate to CHV1 and CHV2 in SIM and RUIM
** PIN Universal will translate to Universal PIN in UICC and Global PIN in SIM/RUIM 
*/
#define AEECARDSESSION_PIN_1          1
#define AEECARDSESSION_PIN_2          2
#define AEECARDSESSION_PIN_UNIVERSAL  3
#define AEECARDSESSION_PIN_SAP        4
#define AEECARDSESSION_PHONE_CODE     5

/* SUB_SIM equates to regular SIM provisioning information
** SUB_RUIM equates to regular RUIM provisioning information
** SUB_UICC equates to USIM, CSIM etc provisioning information, where an AID will have to provide for the subscription information
*/
#define AEECARDSESSION_SUB_SIM        0x1
#define AEECARDSESSION_SUB_RUIM       0x2
#define AEECARDSESSION_SUB_UICC       0x4

/* Maximum number of applications */
#define AEECARDSESSION_MAX_APPS 10

/* Maximum length for AID and Label */
#define AEECARDSESSION_MAX_APP_ID_LEN       16
#define AEECARDSESSION_MAX_APP_LABEL_LEN    33

typedef struct
{
  uint8  *pData;
  int32   nLen;
}AEECardSessionData;

/* This structure contains the Application information

   dwTech      - AEECARDSESSION_SUB_SIM or
                 AEECARDSESSION_SUB_RUIM or
                 AEECARDSESSION_SUB_UICC
   nAidLen     - Length of the Aid (applicable if UICC)
   aAid        - Aid (applicable if UICC)
   aLabel      - Label (applicable if UICC)
*/
typedef struct
{
  uint32 dwTech;
  int32  nAidLen;
  uint8  aAid[AEECARDSESSION_MAX_APP_ID_LEN];
  AECHAR aLabel[AEECARDSESSION_MAX_APP_LABEL_LEN];
}AEECardSessionAppData;

/* This structure is the command status data structure that ICardSession
    will populate upon processing the Send command 
*/
typedef struct
{ 
  int           nCmdStatus;    /* SUCCESS if there are data returned
                                   by the card */
}AEECardSessionCmdStatus;

/*Read Structure for the Transparent Files
  pReadData contains the raw byte format of the file as defined in the 11.14 or 31.102 
  specifications 
*/
typedef struct
{  
  int                nCmdStatus;    /* SUCCESS if there are data returned by the card*/
  AEECardSessionData *pReadData; 
  uint32             dwOffset;
  uint32             dwFileName;
}AEECardSessionReadTpStatus;

/*Read Structure for the Record Based Files
  pReadData contains the raw byte format of the file as defined in the 11.14 or 31.102 
  specifications 
*/
typedef struct
{   
  int                nCmdStatus;    /* SUCCESS if there are data returned by the card*/
  AEECardSessionData *pReadData; 
  uint32             dwRecNum;
  uint32             dwFileName;
}AEECardSessionReadRecStatus;

/*Write Structure for the Transparent Files*/
typedef struct
{   
  int                nCmdStatus;    /* SUCCESS if there are data returned by the card*/
  uint32             dwOffset;
  uint32             dwFileName;
}AEECardSessionWriteTpStatus;

/*Write Structure for the Record Based Files*/
typedef struct
{    
  int                nCmdStatus;    /* SUCCESS if there are data returned by the card*/
  uint32             dwRecNum;
  uint32             dwFileName;
}AEECardSessionWriteRecStatus;

/* sAuts: AUTS returned in case of AUTS error
   sIk:   Integrity Key
   sCk:   Cipherying Key
   sRes:  RES 
*/
typedef struct
{   
  int                nCmdStatus;    /* SUCCESS if there are data returned by the card */
  AEECardSessionData sAuts;
  AEECardSessionData sIk;
  AEECardSessionData sCk;
  AEECardSessionData sRes;
}AEECardSessionAuthStatus;

/* The Pin Operation Status returns the PIN Status and 
** the Number of Retries for the requested PIN. 

DESCRIPTION:        
   nCmdStatus     - SUCCESS/EFAILED/EUNSUPPORTED
   dwPinOp        - AEECARDSESSION_PIN_VERIFY_OP
                    AEECARDSESSION_PIN_ENABLE_OP
                    AEECARDSESSION_PIN_DISABLE_OP
                    AEECARDSESSION_PIN_UNBLOCK_OP
                    AEECARDSESSION_PIN_CHANGE_OP
   dwPinID        - AEECARDSESSION_PIN_1
                    AEECARDSESSION_PIN_2
                    AEECARDSESSION_PIN_UNIVERSAL
                    AEECARDSESSION_PIN_SAP
                    AEECARDSESSION_PHONE_CODE
   dwPinStatus    - AEECARDSESSION_PIN_ENABLED_VERIFIED, 
                    AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED,
                    AEECARDSESSION_PIN_DISABLED,
                    AEECARDSESSION_PIN_BLOCKED,
                    AEECARDSESSION_PIN_PERM_BLOCKED,
                    AEECARDSESSION_PIN_NOT_INITIALIZED
   dwNumPinRetries- Number of retries left for PIN
   dwNumPukRetries- Number of retries for PUK
*/
typedef struct
{  
  int32              nCmdStatus;  /* SUCCESS if there are data returned by the card*/
  uint32             dwPinOp; 
  uint32             dwPinID;
  uint32             dwPinStatus;
  uint32             dwNumPinRetries;
  uint32             dwNumPukRetries;
}AEECardSessionPinOpStatus;

/* The Pin Information returns the PIN Status and the Number of Retries 
** for all the PINs relevant to the particular ICardSession Object.
   
DESCRIPTION:
   nCmdStatus                  - SUCCESS/EFAILED/EUNSUPPORTED
   dwPin1Status, dwPuk1Status,
   dwPinUniversalStatus        - AEECARDSESSION_PIN_ENABLED_VERIFIED, 
                                 AEECARDSESSION_PIN_ENABLED_NOT_VERIFIED,
                                 AEECARDSESSION_PIN_DISABLED,
                                 AEECARDSESSION_PIN_BLOCKED,
                                 AEECARDSESSION_PIN_PERM_BLOCKED,
                                 AEECARDSESSION_PIN_NOT_INITIALIZED
   dwPin1NumRetries, dwPuk1NumRetries,
   dwPin2NumRetries, dwPuk2NumRetries,
   dwPinUniversalNumRetries, dwPukUniversalNumRetries
                               - Number of retries left for the PINs and PUKs
*/
typedef struct
{   
  int                nCmdStatus;  /* SUCCESS if there are data returned by the card*/
  uint32             dwPin1Status;
  uint32             dwPin1NumRetries;
  uint32             dwPuk1NumRetries;
  uint32             dwPin2Status;
  uint32             dwPin2NumRetries;
  uint32             dwPuk2NumRetries;
  uint32             dwPinUniversalStatus;
  uint32             dwPinUniversalNumRetries;
  uint32             dwPukUniversalNumRetries;
}AEECardSessionCardAllPinsInfo;

/* The Tech indicates whether a GSM/CDMA or UMTS should be selected.
** In case of UICC subscription, pAID can be used to identify the 
** CSIM Application ID or the USIM Application ID for the channel.
   NOTE: 
   In a single technology solution, subscription should only be set for 
   the default channel ICardSession interface.
   In a multiple technologies solution, there are 2 possible cases.
   1)   Card where subscription supports AID is limited to 0 or 1 technology, 
      e.g., in a USIM + RUIM, GSM + RUIM solution.  
      In such a case, subscription should only be set for the default channel
      ICardSession interface and a bitmask value for dwTech is applicable
   2)   Card where subscriptions support AID with > 1 technologies, 
      e.g., in a CSIM solution.  In such a case, 
      subscription needs to be set separately for the different technologies 
      with different ICardSession interfaces that communicate with default or 
      1 non-default channel.  
      The bitmask value for dwTech is applicable only between technologies 
      that has 1 AID + other technologies that do not require the specification of AID. 

DESCRIPTION:
   dwTech    -AEECARDSESSION_SUB_SIM/AEECARDSESSION_SUB_RUIM/AEECARDSESSION_SUB_UICC 
   pAid      -Application ID, set when dwTech == AEECARDSESSION_SUB_UICC
*/
typedef struct
{ 
  uint32                  dwTech;   
  AEECardSessionAppData  *pAid;
} AEECardSessionSubData;

/* This structure indicates how ICardSession should interpret the
** File parameter in the calling function 

DESCRIPTION:
   dwFileIndication-AEECARDSESSION_EF_FILE_NAME/ AEECARDSESSION_DF_FILE_NAME/ AEECARDSESSION_FILE_PATH 
   dwFileName      -File name when dwFileIndication == AEECARDSESSION_FILE_NAME
   sAid            - application pointer in hex value Applicable to UICC only.
                     According to ETSI spec, the AID is separated into 2 parts: 
                     RID (Registered applicaiton provider IDentifier (5 bytes)
                       to indicate whether it is ETSI or 3GPP
                     PIX (Proprietary application Idneitifer eXtension)  (7-11 bytes)
                       digit1-4: Application Code -> Identification of the card application  (byte 6th and 7th)
                       digit5-8: Country Code -> Indicate the country of the appication providier of 
                                 the ETSI or EG standarized application  (byte 8th, 9th)
                       digit9-14: Application provider code  (byte 10th to 12th)
                       digit15-22: Application provider field (Optional)  (byte 13th to 16th)
   sPath           - Path location per spec 31.102,11.11, IS820
                     e.g., IMSI on a GSM card: 3F007F206F07 
*/   
typedef struct 
{   
  uint32              dwFileIndication;  
  uint32              dwFileName;        
  AEECardSessionData  sAid;             
  AEECardSessionData  sPath;            
}
AEECardSessionFilePathType;


/* 
DESCRIPTION:
   bAnd           -Indicate if the following access key condition is an "AND"
                   or "OR" operation
   bAlways        - Access is always
   bNever         - Access is never
   bPin1          - Access is allowed after Pin1 verification
   bPin2          - Access is allowed after Pin2 verification
   bPinUniversal  - Access is allowed after universal Pin verification
*/   

typedef struct 
{  
  flg        bAnd:1;             
  flg        bAlways:1;     
  flg        bNever:1;                  
  flg        bPin1:1;        
  flg        bPin2:1;        
  flg        bPinUniversal:1;    
}
AEECardSessionAccessCondType;

/* This structure indicates protection indication of the various
** access condition.  The value of each condition is defined
** per 102.221, 11.11, IS820

DESCRIPTION
   sRead         - Access condition for Read operation
   sWrite        - Access condition for Write operation
   sIncrease     - Access condition for Increase operation
   sInvalidate   - Access condition for Invalidate/Deactivate operation
   sRehabilitate - Access condition for Rehabilitate/Activate operation
 */
typedef struct 
{    
  AEECardSessionAccessCondType     sRead;         
  AEECardSessionAccessCondType     sWrite;        
  AEECardSessionAccessCondType     sIncrease;     
  AEECardSessionAccessCondType     sInvalidate;   
  AEECardSessionAccessCondType     sRehabilitate; 
}
AEECardSessionFileProtectionType;


/* This structure indicates the Get File Attribute Response
   for Transparent Files

DESCRIPTION
   sFileProtection    - File security access condition
   bFileInvalidated   - Indicate if Read/write operation is allowed
   bRWWhenInvalidated - Read/Write Allowed when the file is invalidate/deactivate
*/
typedef struct 
{
  flg                              bFileInvalidated:1;                      
  flg                              bRWWhenInvalidated:1;
  AEECardSessionFileProtectionType sFileProtection; 
}
AEECardSessionTpFileStatus;


/* This structure indicates the Get File Attribute Response
   for Cyclic Files

DESCRIPTION
   bCyclicIncreaseAllowed - increase operation is allowed or not for the file
   sFileProtection -File security access condition
   bFileInvalidated - Indicate if Read/write operation is allowed
   bRWWhenInvalidated - Read/Write Allowed when the file is invalidate/deactivate
*/
typedef struct 
{
  flg                              bCyclicIncreaseAllowed:1;
  flg                              bFileInvalidated:1;                      
  flg                              bRWWhenInvalidated:1;
  AEECardSessionFileProtectionType sFileProtection; 
}
AEECardSessionCyclicFileStatus;


/* This structure indicates the Get File Attribute Response
   for Linear Fixed Files

DESCRIPTION
   dwRecordLen        - length of record for cyclic and linear file. 
   dwNumberRecords    - number of records                    
   sFileProtection    -File security access condition
   bFileInvalidated   - Indicate if Read/write operation is allowed
   bRWWhenInvalidated - Read/Write Allowed when the file is invalidate/deactivate
*/
typedef struct 
{
  uint32                           dwRecordLen;
  uint32                           dwNumberRecords; 
  flg                              bFileInvalidated:1;                      
  flg                              bRWWhenInvalidated:1;
  AEECardSessionFileProtectionType sFileProtection; 
}
AEECardSessionlinearFileStatus;

/* This structure indicates the Get File Attribute Response
** nCmdStatus: Indicates if the Get File Attribute was successful or not
** nSlotID: Indicates the slot that the command is performed on
** sFileAttribData: The FileAttribData returned by a successful Get
**                  File Attribute command 

DESCRIPTION
   nCmdStatus     - SUCCESS/EFAILED/EUNSUPPORTED
   u8SlotID       - slot ID of the operation
   u8FileID       - File ID as specified in spec 31.102 
   dwFileType     - MF, DF or EF
   dwFileStructure- Cyclic, linear or transparent EF 
   dwFileSize     - size of the file or director
*/
typedef struct 
{    
  int                              nCmdStatus;                           
  uint8                            u8SlotID;                             
  uint8                            u8FileID[ AEECARDSESSION_FILE_ID_LEN];         
  uint32                           dwFileType;                               
  uint32                           dwFileStructure;                          
  uint32                           dwFileSize;      
  union
  {
    AEECardSessionTpFileStatus       sTpFileAttrStatus;
    AEECardSessionCyclicFileStatus   sCyclicFileStatus;
    AEECardSessionlinearFileStatus   sLinearFileStatus;
  }file;
      
}
AEECardSessionFileAttribStatus;

typedef struct ICardSession ICardSession;

AEEINTERFACE(ICardSession)
{
   INHERIT_IQueryInterface(ICardSession);

   int   (*Open)                 (ICardSession                  *p, 
                                  AEECardSessionCmdStatus       *pStatus,
                                  const AEECardSessionData      *pData,
                                  AEECallback                   *pUserCB);

   int   (*Close)                (ICardSession                  *p, 
                                  AEECardSessionCmdStatus       *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*Authenticate)         (ICardSession                  *p,
                                  const AEECardSessionData      *pAuthData,
                                  AEECardSessionAuthStatus      *pAuthStatus,
                                  AEECallback                   *pUserCB);

   int   (*ReadTransparent)      (ICardSession                  *p,
                                  uint32                         dwFileName,
                                  uint32                         dwOffset,
                                  uint32                         dwReqLen,
                                  AEECardSessionReadTpStatus    *pReadStatus,
                                  AEECallback                   *pUserCB);

   int   (*ReadRecord)           (ICardSession                  *p,
                                  uint32                         dwFileName,
                                  uint32                         dwRecNum,
                                  uint32                         dwReqLen,
                                  AEECardSessionReadRecStatus   *pReadStatus,
                                  AEECallback                   *pUserCB);

   int   (*WriteTransparent)     (ICardSession                  *p,
                                  uint32                         dwFileName,
                                  uint32                         dwOffset,
                                  const AEECardSessionData      *pWriteData,
                                  AEECardSessionWriteTpStatus   *pWriteStatus,
                                  AEECallback                   *pUserCB);

   int   (*WriteRecord)          (ICardSession                  *p,
                                  uint32                         dwFileName,
                                  uint32                         dwRecNum,
                                  const AEECardSessionData      *pWriteData,
                                  AEECardSessionWriteRecStatus  *pWriteStatus,
                                  AEECallback                   *pUserCB);

   int   (*GetFileAttr)          (ICardSession                  *p,
                                  AEECardSessionFilePathType     sFilePath, 
                                  AEECardSessionFileAttribStatus*pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*GetCardAllPinsInfo)   (ICardSession                  *p, 
                                  AEECardSessionCardAllPinsInfo *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*VerifyPin)            (ICardSession                  *p, 
                                  uint32                         dwPinID,
                                  const AEECardSessionData      *pPin, 
                                  AEECardSessionPinOpStatus     *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*EnablePin)            (ICardSession                  *p, 
                                  uint32                         dwPinID,
                                  const AEECardSessionData      *pPin, 
                                  AEECardSessionPinOpStatus     *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*DisablePin)           (ICardSession                  *p, 
                                  uint32                         dwPinID,
                                  const AEECardSessionData      *pPin, 
                                  AEECardSessionPinOpStatus     *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*UnblockPin)           (ICardSession                  *p, 
                                  uint32                         dwPinID,
                                  const AEECardSessionData      *pPuk, 
                                  const AEECardSessionData      *pNewPin, 
                                  AEECardSessionPinOpStatus     *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*ChangePin)            (ICardSession                  *p, 
                                  uint32                         dwPinID,
                                  const AEECardSessionData      *pOldPin, 
                                  const AEECardSessionData      *pNewPin, 
                                  AEECardSessionPinOpStatus     *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*SelectSubscription)   (ICardSession                  *p, 
                                  const AEECardSessionSubData   *pSub,
                                  AEECardSessionCmdStatus       *pCmdStatus,
                                  AEECallback                   *pUserCB);

   int   (*Register)             (ICardSession                    *p,
                                  AEECardSessionCmdStatus         *pStatus,
                                  AEECallback                     *pUserCB);

};

/* The following defines make for easier naming of the function */

static __inline int ICARDSESSION_AddRef(ICardSession *p) 
{
   return AEEGETPVTBL((p),ICardSession)->AddRef(p);
}

static __inline int ICARDSESSION_Release(ICardSession *p) 
{
   return AEEGETPVTBL((p),ICardSession)->Release(p);
}

/*=============================================================================
FUNCTION: ICARDSESSION_QueryInterface

DESCRIPTION:  

PARAMETERS:
   *p  :  [Input] ICardSession interface pointer
    id :  [Input] Class ID that application wants to obtain a query interface on
  **ppo:  [Input/Output] Return interface pointer for the Class ID provided in
                        input parameter "id"

PROTOTYPE:

  int   ICARDSESSION_QueryInterface(
                                    ICardSession     *p, 
                                    AEECLSID          id, 
                                    void             **ppo);

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None

=============================================================================*/
static __inline int ICARDSESSION_QueryInterface
  (ICardSession     *p, 
   AEECLSID          id, 
   void             **ppo)
{
   return AEEGETPVTBL((p),ICardSession)->QueryInterface(p,
                                                        id,
                                                        ppo);
}

/*=============================================================================
FUNCTION: ICARDSESSION_Open

DESCRIPTION:  
      Opens a new session given the APPID 
      
PROTOTYPE:
   int ICARDSESSION_Open(
                        ICardSession             *p, 
                        AEECardSessionCmdStatus  *pStatus,
                        const AEECardSessionData *pData,
                        AEECallback              *pUserCB)

PARAMETERS:
    *p       :  [Input] ICardSession interface pointer
    *pStatus :  [Input/Output] Cmd Status. The Session ID will be returned in this Cmd Status if 
                the command is successful.
    *pData   :  [Input] Data to be provided by the client (Application ID as defined by
                in various specification, e.g., USIM Spec: 31.102, ISIM Spec: 31.103 for the
                exact encoding of the Application ID
                According to ETSI spec, the AID is separated into 2 parts: 
                RID (Registered applicaiton provider IDentifier (5 bytes)
                  to indicate whether it is ETSI or 3GPP
                PIX (Proprietary application Idneitifer eXtension)  (7-11 bytes)
                  digit1-4: Application Code -> Identification of the card application  (byte 6th and 7th)
                  digit5-8: Country Code -> Indicate the country of the appication providier of 
                            the ETSI or EG standarized application  (byte 8th, 9th)
                  digit9-14: Application provider code  (byte 10th to 12th)
                  digit15-22: Application provider field (Optional)  (byte 13th to 16th)
    *pUserCB :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  The ICardSession pointer will be associated with the Slot where the Open
  was performed

SEE ALSO:
  ICARDSESSION_Close

=============================================================================*/
static __inline int ICARDSESSION_Open
  (ICardSession             *p, 
   AEECardSessionCmdStatus  *pStatus,
   const AEECardSessionData *pData,
   AEECallback              *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->Open(p,
                                            pStatus,
                                            pData, 
                                            pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_Close

DESCRIPTION:  
      Closes the session that was opened using ICARDSESSION_Open
      
PROTOTYPE:
    int ICARDSESSION_Close(
                          ICardSession            *p, 
                          AEECardSessionCmdStatus *pCmdStatus,
                          AEECallback             *pUserCB)

PARAMETERS:
    *p         :  [Input] ICardSession interface pointer
    *pCmdStatus:  [Input/Output] Cmd Status. The Session ID will be returned 
                   in this Cmd Status if the command is successful.
    *pUserCB   :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  The corresponding Session will be closed, the ICardSession pointer will not be
  associates with any slot or channel

SEE ALSO:
  ICARDSESSION_Open

=============================================================================*/
static __inline int ICARDSESSION_Close
  (ICardSession            *p, 
   AEECardSessionCmdStatus *pCmdStatus,
   AEECallback             *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->Close(p,
                                             pCmdStatus,
                                             pUserCB);
}


/*=============================================================================
FUNCTION: ICARDSESSION_Authenticate

DESCRIPTION:  
      App can run authenticate algorithms on the card by providing all the 
      appropriate data.
      
PROTOTYPE:
    int ICARDSESSION_Authenticate(
                                  ICardSession             *p,
                                  AEECardSessionData       *pAuthData,
                                  AEECardSessionAuthStatus *pAuthStatus,
                                  AEECallback              *pUserCB)

PARAMETERS:
    *p           :  [Input] ICardSession interface pointer
    *pAuthData   :  [Input] Data required to do the authentication
    *pAuthStatus :  [Input/Output] Response for the Command
    *pUserCB     :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  None

=============================================================================*/
static __inline int ICARDSESSION_Authenticate
  (ICardSession             *p,
   AEECardSessionData       *pAuthData,
   AEECardSessionAuthStatus *pAuthStatus,
   AEECallback              *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->Authenticate(p,
                                                    pAuthData, 
                                                    pAuthStatus,
                                                    pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_ReadTransparent

DESCRIPTION:  
      Reads a Transparent File
      
PROTOTYPE:
   int ICARDSESSION_ReadTransparent(
                                    ICardSession               *p,
                                    uint32                      dwFileName,
                                    uint32                      dwOffset,
                                    uint32                      dwReqLen,
                                    AEECardSessionReadTpStatus *pReadStatus,
                                    AEECallback                *pUserCB)

PARAMETERS:
    *p           :  [Input] ICardSession interface pointer
    dwFileName   :  [Input] Name of the file to be read, 
                    e.g., AEECARDSESSION_ICCID, 
                          AEECARDSESSION_CDMA_IMSI_M,
                          AEECARDSESSION_GSM_IMSI,
                          AEECARDSESSION_USIM_KC
                    Please refer to 11.11, 31.102 for detail information
    dwOffset     :  [Input] Offset into the file
    dwReqLen     :  [Input] Length to be read
    *pReadStatus :  [Input/Output] Return Data
    *pUserCB     :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  ICARDSESSION_ReadRecord

=============================================================================*/
static __inline int ICARDSESSION_ReadTransparent
  (ICardSession               *p,
   uint32                      dwFileName,
   uint32                      dwOffset,
   uint32                      dwReqLen,
   AEECardSessionReadTpStatus *pReadStatus,
   AEECallback                *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->ReadTransparent(p,
                                                       dwFileName,
                                                       dwOffset,
                                                       dwReqLen,
                                                       pReadStatus,
                                                       pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_ReadRecord

DESCRIPTION:  
      Reads a record based file
      
PROTOTYPE:
    int ICARDSESSION_ReadRecord(
                                ICardSession                 *p,
                                uint32                        dwFileName,
                                uint32                        dwRecNum,
                                uint32                        dwReqLen,
                                AEECardSessionReadRecStatus  *pReadStatus,
                                AEECallback                  *pUserCB)

PARAMETERS:
    *p           :  [Input] ICardSession interface pointer
    dwFileName   :  [Input] Name of the file to be read
                    e.g., AEECARDSESSION_DIR, 
                          AEECARDSESSION_TELECOM_ADN,
                          AEECARDSESSION_USIM_ADN
                    Please refer to 11.11, 31.102 for detail information
    dwRecNum     :  [Input] Record #
    dwReqLen     :  [Input] Length to be read
    *pReadtatus  :  [Input/Output] Return Data
    *pUserCB     :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  ICARDSESSION_ReadTransparent

=============================================================================*/
static __inline int ICARDSESSION_ReadRecord
  (ICardSession                 *p,
   uint32                        dwFileName,
   uint32                        dwRecNum,
   uint32                        dwReqLen,
   AEECardSessionReadRecStatus  *pReadStatus,
   AEECallback                  *pUserCB)
{ 
   return AEEGETPVTBL(p,ICardSession)->ReadRecord(p,
                                                  dwFileName,
                                                  dwRecNum,
                                                  dwReqLen,
                                                  pReadStatus,
                                                  pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_WriteTransparent

DESCRIPTION:  
      Writes to a Transparent File
     
PROTOTYPE:
    int ICARDSESSION_WriteTransparent(
                                      ICardSession                 *p,
                                      uint32                        dwFileName,
                                      uint32                        dwOffset,
                                      const AEECardSessionData     *pWriteData,
                                      AEECardSessionWriteTpStatus  *pWriteStatus,
                                      AEECallback                  *pUserCB)

PARAMETERS:
    *p            :  [Input] ICardSession interface pointer
    dwFileName    :  [Input] Name of the file to be read
    dwOffset      :  [Input] Offset into the file
    *WriteData    :  [Input] Data to be written.  Data should be in the 
                     raw byte format of the file as defined in the 11.14 or 
                     31.102 specifications 
    *pWriteStatus :  [Input/Output] Data to be written and return Cmd Status
    *pUserCB      :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  ICARDSESSION_WriteRecord

=============================================================================*/
static __inline int ICARDSESSION_WriteTransparent
  (ICardSession                 *p,
   uint32                        dwFileName,
   uint32                        dwOffset,
   const AEECardSessionData     *pWriteData,
   AEECardSessionWriteTpStatus  *pWriteStatus,
   AEECallback                  *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->WriteTransparent(p,
                                                       dwFileName,
                                                       dwOffset,
                                                       pWriteData,
                                                       pWriteStatus,
                                                       pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_WriteRecord

DESCRIPTION:  
      Writes to a record based file
      
PROTOTYPE:
    int ICARDSESSION_WriteRecord(
                                ICardSession                 *p,
                                uint32                        dwFileName,
                                uint32                        dwRecNum,
                                const AEECardSessionData     *pWriteData,
                                AEECardSessionWriteRecStatus *pWriteStatus,
                                AEECallback                  *pUserCB)

PARAMETERS:
    *p            :  [Input] ICardSession interface pointer
    dwFileName    :  [Input] Name of the file to be read
    dwRecNum      :  [Input] Record #
    *pWriteData   :  [Input] Data to be written.  Data should be in the 
                     raw byte format of the file as defined in the 11.14 or 
                     31.102 specifications 
    *pWriteStatus :  [Input/Output] Data to be written and return Cmd Status
    *pUserCB      :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None

SIDE EFFECTS:
  None

SEE ALSO:
  ICARDSESSION_WriteTransparent

=============================================================================*/
static __inline int ICARDSESSION_WriteRecord
  (ICardSession                 *p,
   uint32                        dwFileName,
   uint32                        dwRecNum,
   const AEECardSessionData     *pWriteData,
   AEECardSessionWriteRecStatus *pWriteStatus,
   AEECallback                  *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->WriteRecord(p,
                                                  dwFileName,
                                                  dwRecNum,
                                                  pWriteData,
                                                  pWriteStatus,
                                                  pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_GetFileAttr

DESCRIPTION:  
      This method provides the service to get the Attribute information on a
     file or directory on the SIM/USIM

PROTOTYPE:

  int ICARDSESSION_GetFileAttributes(
                                    ICardSession                  *p,
                                    AEECardSessionFilePathType     sFilePath, 
                                    AEECardSessionFileAttribStatus*pCmdStatus,
                                    AEECallback                   *pUserCB);

Parameters:
  *p         : [Input] ICardSession interface pointer
  sFilePath  : [Input] Indicates the file of interest
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

Return Value:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  Data read is returned in Asynchronous fashion.  In the pUserCB, ICardSession
  will populate pCmdStatus with the file attribute information

Side Effects:
   None

See Also:
   None

=============================================================================*/
static __inline int ICARDSESSION_GetFileAttr
(ICardSession                   *p,
 AEECardSessionFilePathType      sFilePath, 
 AEECardSessionFileAttribStatus *pCmdStatus,
 AEECallback                    *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->GetFileAttr(p,
                                                   sFilePath,
                                                   pCmdStatus,
                                                   pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_GetCardAllPinsInfo

DESCRIPTION:  
  Get All the Card PIN information relating to the ICardSession object
 
PROTOTYPE:
  int ICARDSESSION_GetCardAllPinsInfo(
                   ICardSession                  *p, 
                   AEECardSessionCardAllPinsInfo *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  None

See Also:
  ICARDSESSION_VerifyPin
  ICARDSESSION_EnablePin
  ICARDSESSION_DisablePin
  ICARDSESSION_UnblockPin
  ICARDSESSION_ChangePin

=============================================================================*/
static __inline int ICARDSESSION_GetCardAllPinsInfo
( ICardSession                  *p, 
  AEECardSessionCardAllPinsInfo *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->GetCardAllPinsInfo(p,
                                                         pCmdStatus,
                                                         pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_VerifyPin

DESCRIPTION:  
  Verify the PIN for the particular application accessed by the 
  ICardSession object.
 
PROTOTYPE:
  int ICARDSESSION_VerifyPin(
                   ICardSession                  *p, 
                   uint32                         dwPinID,
                   const AEECardSessionData      *pPin, 
                   AEECardSessionPinOpStatus     *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  dwPinID    : [Input] PIN ID to which the verify data is associated with
  *pPin      : [Input] Pin value.  According to 11.14 and 31.102, both ICC and
               UICC's should have PIN of length [4,8]
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  None

See Also:
  ICARDSESSION_GetCardAllPinsInfo
  ICARDSESSION_EnablePin
  ICARDSESSION_DisablePin
  ICARDSESSION_UnblockPin
  ICARDSESSION_ChangePin

=============================================================================*/
static __inline int ICARDSESSION_VerifyPin
( ICardSession                  *p, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->VerifyPin(p,
                                                dwPinID,
                                                pPin,
                                                pCmdStatus,
                                                pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_EnablePin

DESCRIPTION:  
  Enable the PIN for the particular application accessed by the 
  ICardSession object.
 
PROTOTYPE:
  int ICARDSESSION_EnablePin(
                   ICardSession                  *p, 
                   uint32                         dwPinID,
                   const AEECardSessionData      *pPin, 
                   AEECardSessionPinOpStatus     *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  dwPinID    : [Input] PIN ID to which the enable data is associated with
  *pPin      : [Input] Pin value.  According to 11.14 and 31.102, both ICC and
               UICC's should have PIN of length [4,8]
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  None

See Also:
  ICARDSESSION_GetCardAllPinsInfo
  ICARDSESSION_VerifyPin
  ICARDSESSION_DisablePin
  ICARDSESSION_UnblockPin
  ICARDSESSION_ChangePin

=============================================================================*/
static __inline int ICARDSESSION_EnablePin
( ICardSession                  *p, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->EnablePin(p,
                                                dwPinID,
                                                pPin,
                                                pCmdStatus,
                                                pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_DisablePin

DESCRIPTION:  
  Disable the PIN for the particular application accessed by the 
  ICardSession object.
 
PROTOTYPE:
  int ICARDSESSION_DisablePin (
                   ICardSession                  *p, 
                   uint32                         dwPinID,
                   const AEECardSessionData      *pPin, 
                   AEECardSessionPinOpStatus     *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  dwPinID    : [Input] PIN ID to which the disable data is associated with
  *pPin      : [Input] Pin value.  According to 11.14 and 31.102, both ICC and
               UICC's should have PIN of length [4,8]
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  None

See Also:
  ICARDSESSION_GetCardAllPinsInfo
  ICARDSESSION_VerifyPin
  ICARDSESSION_EnablePin
  ICARDSESSION_UnblockPin
  ICARDSESSION_ChangePin

=============================================================================*/
static __inline int ICARDSESSION_DisablePin
( ICardSession                  *p, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->DisablePin(p,
                                                 dwPinID,
                                                 pPin,
                                                 pCmdStatus,
                                                 pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_UnblockPin

DESCRIPTION:  
  Unblock the PIN for the particular application accessed by the ICardSession object.  
  The New Pin value will also need to be provided in this operation.
 
PROTOTYPE:
  int ICARDSESSION_UnblockPin (
                   ICardSession                  *p, 
                   uint32                         dwPinID,
                   const AEECardSessionData      *pPuk, 
                   const AEECardSessionData      *pNewPin, 
                   AEECardSessionPinOpStatus     *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  dwPinID    : [Input] PIN ID to which the unblock data is associated with
  *pPuk      : [Input] Puk value to unblock.  According to 11.14 and 31.102, both ICC and
               UICC's should have PUK of length 8
  *pNewPin   : [Input] New Pin value.  According to 11.14and 31.102, both ICC and
               UICC's should have PIN of length [4,8]
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  The sNewPin value will be treated as the Pin value once a successful 
  unblock occurs

See Also:
  ICARDSESSION_GetCardAllPinsInfo
  ICARDSESSION_VerifyPin
  ICARDSESSION_EnablePin
  ICARDSESSION_DisablePin
  ICARDSESSION_ChangePin

=============================================================================*/
static __inline int ICARDSESSION_UnblockPin
( ICardSession                  *p, 
  uint32                         dwPinID,
  const AEECardSessionData      *pPuk, 
  const AEECardSessionData      *pNewPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->UnblockPin(p,
                                                 dwPinID,
                                                 pPuk,
                                                 pNewPin,
                                                 pCmdStatus,
                                                 pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_ChangePin

DESCRIPTION:  
  Change the PIN for the particular application accessed by the ICardSession object.  
  The New Pin value will also need to be provided in this operation.
 
PROTOTYPE:
  int ICARDSESSION_ChangePin (
                   ICardSession                  *p, 
                   uint32                         dwPinID,
                   const AEECardSessionData      *pOldPin, 
                   const AEECardSessionData      *pNewPin, 
                   AEECardSessionPinOpStatus     *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  dwPinID    : [Input] PIN ID to which the change data is associated with
  *pOldPin   : [Input] Original Pin value.  According to 11.14and 31.102, 
               both ICC and UICC's should have PIN of length [4,8]
  *pNewPin   : [Input] New Pin value to change to.  According to 11.14and 31.102, 
               both ICC and UICC's should have PIN of length [4,8]
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  The sNewPin value will be treated as the Pin value once a successful change occurs

See Also:
  ICARDSESSION_GetCardAllPinsInfo
  ICARDSESSION_VerifyPin
  ICARDSESSION_EnablePin
  ICARDSESSION_DisablePin
  ICARDSESSION_UnblockPin

=============================================================================*/
static __inline int ICARDSESSION_ChangePin
( ICardSession                  *p, 
  uint32                         dwPinID,
  const AEECardSessionData      *pOldPin, 
  const AEECardSessionData      *pNewPin, 
  AEECardSessionPinOpStatus     *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->ChangePin(p,
                                                dwPinID,
                                                pOldPin,
                                                pNewPin,
                                                pCmdStatus,
                                                pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_SelectSubscription

DESCRIPTION:  
  The function selects the subscription.  This function is applicable to all 
  technologies.  
  This function should only be called when a technology initialization is 
  required or when a technology switch is required.
  i.e., UMTS App1 -> UMTS App2 is not applicable without a SIM INSERTED Card Event
  i.e., GSM -> CDMA is applicable

PROTOTYPE:
  int ICARDSESSION_SelectSubscription(
                   ICardSession                  *p, 
                   const AEECardSessionSubData   *pSub,
                   AEECardSessionCmdStatus       *pCmdStatus,
                   AEECallback                   *pUserCB);
     
PARAMETERS:
  *p         : [Input] ICardSession interface pointer
  *pSub      : [Input] Technology to which the subscription will be selected.
  *pCmdStatus: [Input/Output] Command response status
  *pUserCB   : [Input/Output] User Callback

RETURN VALUE:
  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

Comments:
  None

Side Effects:
  None

See Also:
  None

=============================================================================*/
static __inline int ICARDSESSION_SelectSubscription
( ICardSession                  *p, 
  const AEECardSessionSubData   *pSub,
  AEECardSessionCmdStatus       *pCmdStatus,
  AEECallback                   *pUserCB)
{
  return AEEGETPVTBL(p,ICardSession)->SelectSubscription(p,
                                                         pSub,
                                                         pCmdStatus,
                                                         pUserCB);
}

/*=============================================================================
FUNCTION: ICARDSESSION_Register

DESCRIPTION:
  Registers for Client ID with MMGSDI.
  Every app will need to call this API after creating an new instance of ICardSession
  Every app will need to do a client registration of its own.
  

PROTOTYPE:
  int   ICARDSESSION_Register(ICardSession                  *p,
                              AEECardSessionCmdStatus       *pStatus,
                              AEECallback                   *pUserCB)

PARAMETERS:
    *p       :  [Input] ICardSession interface pointer
    *pStatus :  [Input/Output] Cmd Status. The Session ID will be returned in this Cmd Status if 
                the command is successful.
    *pUserCB :  [Input/Output] App Callback          

RETURN VALUE:
  SUCCESS - Client ID Registration request was succssfuly queued
  EBADPARM -  Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  None
  
DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static __inline int ICARDSESSION_Register
  (ICardSession             *p, 
   AEECardSessionCmdStatus  *pStatus,
   AEECallback              *pUserCB)
{
   return AEEGETPVTBL(p,ICardSession)->Register(p,
                                                pStatus,
                                                pUserCB);
}

#endif    /* #define AEECARDSESSION_H */

