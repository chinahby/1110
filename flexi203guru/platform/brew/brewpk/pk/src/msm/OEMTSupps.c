/*=============================================================================
FILE:  OEMTSUPPS.c

SERVICES:  OEM functions for supporting BREW SUPPS Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the BREW ISuppsTrans interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

      OEMSUPPS_AddRef            Adds a brew reference to the SUPPS object.
      OEMSUPPS_Release           Removes a brew reference to the SUPPS object.
      OEMSUPPS_QueryInterface    Returns a new interface, not yet supported.
      OEMSupps_ParseString       Checks if a string is a SS command.
      OEMSUPPS_LoadMessage       
      OEMSUPPS_Run               
      OEMSUPPS_GetResult         

        Copyright ?2004-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/platform/brew/brewpk/rel/3.1.5sp/pk/src/msm/OEMTSupps.c#9 $

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/03    at     Initial release.
=============================================================================*/
/*lint -esym(754, ISuppsTrans::pvt) */
/*lint -esym(754, ISuppsTrans_Notifier::pvt) */

#include "OEMFeatures.h"

#include "AEE.h"         // BREW headers.
#include "AEEStdLib.h"
#include "AEE_OEM.h"     // Brew OEM Headers.
#include "AEE_OEMDispatch.h"
#include "AEE_OEMEvent.h"
#include "AEECriticalSection.h" 
#include "OEMTelephone.h"
#include "OEMTSupps.h"
#include "OEMConfig.h"
#include "OEMHeap.h"

#include "cm.h"
#ifdef AEE_SIMULATOR
#define FEATURE_GSM
//#define FEATURE_MMGSDI
#include "cmi.h"
#else
#include "cmutil.h"
#endif

#ifdef FEATURE_MMGSDI
#include "gsdi_exp.h"
#endif

#define __inline 

#ifdef MSG_ERROR 
   #undef MSG_ERROR
#endif
#ifdef MSG_HIGH
   #undef MSG_HIGH
#endif
#ifdef MSG_MED
   #undef MSG_MED
#endif
#ifdef MSG_LOW
   #undef MSG_LOW
#endif

static struct {
   flg bErrorDebugMsg:1;
   flg bHighDebugMsg:1;
   flg bMedDebugMsg:1;
   flg bLowDebugMsg:1;
} gOEMTSuppsDebug = {1,1,1,1};

boolean OEMTSuppsError_IsDebugMsgOn(void);
#define FARF_OEMTSUPPS_ERROR   OEMTSuppsError_IsDebugMsgOn()

boolean OEMTSuppsHigh_IsDebugMsgOn(void);
#define FARF_OEMTSUPPS_HIGH    OEMTSuppsHigh_IsDebugMsgOn()

boolean OEMTSuppsMed_IsDebugMsgOn(void);
#define FARF_OEMTSUPPS_MED     OEMTSuppsMed_IsDebugMsgOn()

boolean OEMTSuppsLow_IsDebugMsgOn(void);
#define FARF_OEMTSUPPS_LOW     OEMTSuppsLow_IsDebugMsgOn()

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 


#define MSG_ERROR( sf, a, b, c )    FARF(OEMTSUPPS_ERROR, (sf, a, b, c))
#define MSG_HIGH( sf, a, b, c )     FARF(OEMTSUPPS_HIGH, (sf, a, b, c))
#define MSG_MED( sf, a, b, c )      FARF(OEMTSUPPS_MED, (sf, a, b, c))
#define MSG_LOW( sf, a, b, c )      FARF(OEMTSUPPS_LOW, (sf, a, b, c))

#ifndef INRANGE
#define INRANGE( val, min, max )  ( (val) >= (min) && (val) <= (max) )
#endif

#ifndef OFFSETOF
#define OFFSETOF(type, field)	(((char*)(&((type *)4)->field))-((char*)4))
#endif   /* offsetof */

#ifndef RECOVER_MEMBER
#define RECOVER_MEMBER(type,member,p) ((type*)(((char*)(p))-OFFSETOF(type,member)))
#endif

#define OEMSUPPS_MAX_BSG_LEN            2
#define OEMSUPPS_MAX_SC_LEN             3
#define OEMSUPPS_MAX_SIA_LEN            20  /*   */
#define OEMSUPPS_MAX_SIB_LEN            4   /* PWD LEN */
#define OEMSUPPS_MAX_SIC_LEN            4   /* PWD LEN */
#define OEMSUPPS_MAX_TEXT_LEN           50
#define OEMSUPPS_MAX_FWD_TO_NUMBER      20
#define OEMSUPPS_CUG_SIZE                2  /*Max Number of CUG Indicators*/

#define OEMSUPPS_MAX_BCD_PHONE_NUM_LEN  27
#define OEMSUPPS_MAX_SUBADDRESS_LEN     22
#define OEMSUPPS_MAX_BC_LEN             8
#define OEMSUPPS_MAX_PIN_LEN            (8+1)
#define OEMSUPPS_MAX_PUK_LEN            (8+1)

#define OEMSUPPS_DEFAULT_ALPHABET                 0x00
#define OEMSUPPS_ALPHABET_MASK                    0xF0
#define OEMSUPPS_USS_DEF_ALPHABET_LANG_UNSPEC     0x0F

#define AEETSUPPS_BM(val) (val << 16)

/*AEE SC Code Mapping to Network Values*/
typedef struct
{
   int   local;   
   int   net_code;
} OEMSUPPSTableType;

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*lint -save -e701*/
/*lint -save -e749*/
/*lint -save -e750*/
typedef enum
{
   OEMSUPPS_ALL_SERVICES                 =0x00 |(AEETSUPPS_BM(NO_CODE)),
/*
** BEARER-SERVICES
*/
  OEMSUPPS_ALL_BEARER_SERVICES           =0x00 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_DATA_CDA_SERVICES         =0x10 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_300BPS               =0x11 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_1200BPS              =0x12 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_1200_75BPS           =0x13 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_2400BPS              =0x14 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_4800BPS              =0x15 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDA_9600BPS              =0x16 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_DATA_CDS_SERVICES         =0x18 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDS_1200BPS              =0x1A |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDS_2400BPS              =0x1C |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDS_4800BPS              =0x1D |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_CDS_9600BPS              =0x1E |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_PAD_ACCESS_CA_SERVICES    =0x20 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_300BPS          =0x21 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_1200BPS         =0x22 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_1200_75BPS      =0x23 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_2400BPS         =0x24 |(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_4800BPS         =0x25|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PAD_ACCESS_CA_9600BPS         =0x26|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_DATA_PDS_SERVICES         =0x28|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_PDS_2400BPS              =0x2C|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_PDS_4800BPS              =0x2D|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_DATA_PDS_9600BPS              =0x2E|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_ALTERNATE_SPEECH_DATA_CDA =0x30|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_ALTERNATE_SPEECH_DATA_CDS =0x38|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_SPEECH_FOLLWOED_BYDATA_CDA=0x40|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_SPEECH_FOLLWOED_BYDATA_CDS=0x48|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_DATA_CIRCUIT_ASYNCHRONOUS =0x50|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_DATA_CIRCUIT_SYNCHRONOUS  =0x58|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_ASYNCHRONOUS_SERVICES     =0x60|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_SYNCHRONOUS_SERVICES      =0x68|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_ALL_PLMN_SPECIFIC_BS          =0xD0|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_1            =0xD1|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_2            =0xD2|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_3            =0xD3|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_4            =0xD4|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_5            =0xD5|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_6            =0xD6|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_7            =0xD7|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_8            =0xD8|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_9            =0xD9|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_A            =0xDA|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_B            =0xDB|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_C            =0xDC|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_D            =0xDD|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_E            =0xDE|(AEETSUPPS_BM(BS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_BS_F            =0xDF|(AEETSUPPS_BM(BS_CODE)),

  OEMSUPPS_ALL_TELESERVICES               =0x0 |(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_SPEECH_TRANSMISSION_SERVCES=0x10|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_TELEPHONY                      =0x11|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_EMERGENCY_CALLS                =0x12|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_SMS_SERVICES               =0x20|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_SHORT_MSG_MT_PP                =0x21|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_SHORT_MSG_MO_PP                =0x22|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_FAX_TRANSMISSION_SERVICES  =0x60|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_FAX_GROUP3_AND_ALTER_SPEECH    =0x61|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_AUTO_FAX_GROUP3                =0x62|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_FAX_GROUP4                     =0x63|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_DATA_TELESERVICES          =0x70|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_TELESERVICES_EXCEPT_SMS    =0x80|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_ALL_PLMN_SPECIFIC_TS           =0xD0|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_1             =0xD1|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_2             =0xD2|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_3             =0xD3|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_4             =0xD4|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_5             =0xD5|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_6             =0xD6|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_7             =0xD7|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_8             =0xD8|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_9             =0xD9|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_A             =0xDA|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_B             =0xDB|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_C             =0xDC|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_D             =0xDD|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_E             =0xDE|(AEETSUPPS_BM(TS_CODE)),
  OEMSUPPS_PLMN_SPECIFIC_TS_F             =0xDF|(AEETSUPPS_BM(TS_CODE))

}OEMSUPPSBasicService;
#endif /* FEATURE_WCDMA || FEATURE_GSM */



#define AEETSUPPS_IS_ALL_CF(x)  ( (x & 0xF0 )== AEETSUPPS_CODE_ALL_FORWARDING_SS ? TRUE: FALSE)
#define AEETSUPPS_IS_RSTRCT(x)  ( (x & 0xF0 )== AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS ? TRUE: FALSE)

#define AEETSUPPS_IS_CF(x)                                                      \
                          ( ((x) == AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS   ||      \
                             (x) == AEETSUPPS_CODE_ALL_CONDFWD_SS  ||           \
                             (x) == AEETSUPPS_CODE_CFNRY)                       \
                             ? TRUE : FALSE )


/*Type of Supplementary Service Operations*/
typedef struct
{
  char *sups_operation;
  AEETSuppsRequestType op_code;
} OEMSUPPS_OPERATION;

OEMSUPPS_OPERATION OEMSUPPS_operation_input[] =
{
  {"**", AEETSUPPS_REQ_REGISTER},
  {"*#", AEETSUPPS_REQ_INTERROGATE},
  {"*",  AEETSUPPS_REQ_ACTIVATE},
  {"##", AEETSUPPS_REQ_ERASE},
  {"#",  AEETSUPPS_REQ_DEACTIVATE},
  { NULL, NULL}
};

typedef struct
{
   char * local_sc;         /* local value of SC */
   byte   net_sc;           /* Network SC */
} OEMSUPPSSCTableType;


/*
** Local Supplementary Service Codes (SC)
*/
#define OEMSUPPS_LOCAL_SC_ALL               ""
#define OEMSUPPS_LOCAL_SC_CF                "002"
#define OEMSUPPS_LOCAL_SC_CFC               "004"
#define OEMSUPPS_LOCAL_SC_CFU               "21"
#define OEMSUPPS_LOCAL_SC_CFB               "67"
#define OEMSUPPS_LOCAL_SC_CFNRY             "61"
#define OEMSUPPS_LOCAL_SC_CFNRC             "62"
#define OEMSUPPS_LOCAL_SC_CLIP              "30"
#define OEMSUPPS_LOCAL_SC_CLIR              "31"
#define OEMSUPPS_LOCAL_SC_COLP              "76"
#define OEMSUPPS_LOCAL_SC_COLR              "77"
#define OEMSUPPS_LOCAL_SC_CW                "43"
#define OEMSUPPS_LOCAL_SC_CNAP              "300"
#define OEMSUPPS_LOCAL_SC_B                 "330"
#define OEMSUPPS_LOCAL_SC_OB                "333"
#define OEMSUPPS_LOCAL_SC_IB                "353"
#define OEMSUPPS_LOCAL_SC_BAOC              "33"
#define OEMSUPPS_LOCAL_SC_BOIC              "331"
#define OEMSUPPS_LOCAL_SC_BOICEXHC          "332"
#define OEMSUPPS_LOCAL_SC_BAIC              "35"
#define OEMSUPPS_LOCAL_SC_BICROAM           "351"
#define OEMSUPPS_LOCAL_BSG_REG_PASSWD       "03"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*Local to Network Mapping*/
const OEMSUPPSSCTableType OEMSUPPSSCConversionTable [] =
{
   { OEMSUPPS_LOCAL_SC_ALL,        all_ss },
   { OEMSUPPS_LOCAL_SC_CF,         allForwardingSS },
   { OEMSUPPS_LOCAL_SC_CFC,        allCondForwardingSS },
   { OEMSUPPS_LOCAL_SC_CFU,        cfu },
   { OEMSUPPS_LOCAL_SC_CFB,        cfb },
   { OEMSUPPS_LOCAL_SC_CFNRY,      cfnry },
   { OEMSUPPS_LOCAL_SC_CFNRC,      cfnrc },
   { OEMSUPPS_LOCAL_SC_CLIP,       clip },
   { OEMSUPPS_LOCAL_SC_CLIR,       clir },
   { OEMSUPPS_LOCAL_SC_COLP,       colp },
   { OEMSUPPS_LOCAL_SC_COLR,       colr },
   { OEMSUPPS_LOCAL_SC_CW,         cw },
   { OEMSUPPS_LOCAL_SC_B,          allCallRestrictionSS },
   { OEMSUPPS_LOCAL_SC_OB,         barringOfOutgoingCalls },
   { OEMSUPPS_LOCAL_SC_IB,         barringOfIncomingCalls },
   { OEMSUPPS_LOCAL_SC_BAOC,       baoc },
   { OEMSUPPS_LOCAL_SC_BOIC,       boic },
   { OEMSUPPS_LOCAL_SC_BOICEXHC,   boicExHC },
   { OEMSUPPS_LOCAL_SC_BAIC,       baic },
   { OEMSUPPS_LOCAL_SC_BICROAM,    bicRoam },
   { OEMSUPPS_LOCAL_SC_CNAP,       cnap }
};
#endif /* FEATURE_WCDMA || FEATURE_GSM */

/*
** LOCAL Basic Service Codes
*/

#define OEMSUPPS_LOCAL_BSG_ALL              ""
#define OEMSUPPS_LOCAL_BSG_ALL_TS           "10"
#define OEMSUPPS_LOCAL_BSG_TELEPHONY        "11"
#define OEMSUPPS_LOCAL_BSG_ALL_DATA         "12"
#define OEMSUPPS_LOCAL_BSG_FAX              "13"
#define OEMSUPPS_LOCAL_BSG_SMS              "16"
#define OEMSUPPS_LOCAL_BSG_ALL_TELE_EX_SMS  "19"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_ALL    "50"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_1      "51"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_2      "51"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_3      "53"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_4      "54"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_5      "55"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_6      "56"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_7      "57"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_8      "58"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_9      "59"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_A      "60"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_B      "61"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_C      "62"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_D      "63"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_E      "64"
#define OEMSUPPS_LOCAL_BSG_PLMN_TELE_F      "65"

#define OEMSUPPS_LOCAL_BSG_ALL_BS           "20"
#define OEMSUPPS_LOCAL_BSG_ALL_ASYNC        "21"
#define OEMSUPPS_LOCAL_BSG_ALL_SYNC         "22"
#define OEMSUPPS_LOCAL_BSG_ALL_DC_SYNC      "24"
#define OEMSUPPS_LOCAL_BSG_ALL_DC_ASYNC     "25"
#define OEMSUPPS_LOCAL_BSG_ALL_DED_PACKET   "26"
#define OEMSUPPS_LOCAL_BSG_ALL_DED_PAD      "27"
#define OEMSUPPS_LOCAL_BSG_12_KBIT          "29"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_ALL    "70"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_1      "71"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_2      "71"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_3      "73"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_4      "74"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_5      "75"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_6      "76"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_7      "77"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_8      "78"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_9      "79"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_A      "80"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_B      "81"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_C      "82"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_D      "83"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_E      "84"
#define OEMSUPPS_LOCAL_BSG_PLMN_BEAR_F      "85"

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

typedef struct
{
   char   *local_bsg;       /* local BSG value */
   int   net_bsg_code;     /* network BSG value */
} OEMSUPPSStrBSGTableType;

#define OEMSUPPS_MAX_ENTRY (sizeof(OEMSUPPSSCConversionTable)/sizeof(OEMSUPPSSCTableType))
/*Local to Network Mapping*/

const OEMSUPPSStrBSGTableType OEMSUPPSStrBSGConversionTable[] = {
   { OEMSUPPS_LOCAL_BSG_ALL,             OEMSUPPS_ALL_SERVICES },
   { OEMSUPPS_LOCAL_BSG_ALL_TS,          OEMSUPPS_ALL_TELESERVICES},
   { OEMSUPPS_LOCAL_BSG_TELEPHONY,       OEMSUPPS_ALL_SPEECH_TRANSMISSION_SERVCES},
   { OEMSUPPS_LOCAL_BSG_ALL_DATA,        OEMSUPPS_ALL_DATA_TELESERVICES},
   { OEMSUPPS_LOCAL_BSG_FAX,             OEMSUPPS_ALL_FAX_TRANSMISSION_SERVICES},
   { OEMSUPPS_LOCAL_BSG_SMS,             OEMSUPPS_ALL_SMS_SERVICES},
   { OEMSUPPS_LOCAL_BSG_ALL_TELE_EX_SMS, OEMSUPPS_ALL_TELESERVICES_EXCEPT_SMS},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_ALL,   OEMSUPPS_ALL_PLMN_SPECIFIC_TS},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_1,     OEMSUPPS_PLMN_SPECIFIC_TS_1},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_2,     OEMSUPPS_PLMN_SPECIFIC_TS_2 },
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_3,     OEMSUPPS_PLMN_SPECIFIC_TS_3},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_4,     OEMSUPPS_PLMN_SPECIFIC_TS_4},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_5,     OEMSUPPS_PLMN_SPECIFIC_TS_5},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_6,     OEMSUPPS_PLMN_SPECIFIC_TS_6},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_7,     OEMSUPPS_PLMN_SPECIFIC_TS_7},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_8,     OEMSUPPS_PLMN_SPECIFIC_TS_8},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_9,     OEMSUPPS_PLMN_SPECIFIC_TS_9},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_A,     OEMSUPPS_PLMN_SPECIFIC_TS_A},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_B,     OEMSUPPS_PLMN_SPECIFIC_TS_B},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_C,     OEMSUPPS_PLMN_SPECIFIC_TS_C},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_D,     OEMSUPPS_PLMN_SPECIFIC_TS_D},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_E,     OEMSUPPS_PLMN_SPECIFIC_TS_E},
   { OEMSUPPS_LOCAL_BSG_PLMN_TELE_F,     OEMSUPPS_PLMN_SPECIFIC_TS_F},
   { OEMSUPPS_LOCAL_BSG_ALL_BS,          OEMSUPPS_ALL_BEARER_SERVICES},
   { OEMSUPPS_LOCAL_BSG_ALL_ASYNC,       OEMSUPPS_ALL_ASYNCHRONOUS_SERVICES},
   { OEMSUPPS_LOCAL_BSG_ALL_SYNC,        OEMSUPPS_ALL_SYNCHRONOUS_SERVICES},
   { OEMSUPPS_LOCAL_BSG_ALL_DC_SYNC,     OEMSUPPS_ALL_DATA_CIRCUIT_SYNCHRONOUS},
   { OEMSUPPS_LOCAL_BSG_ALL_DC_ASYNC,    OEMSUPPS_ALL_DATA_CIRCUIT_ASYNCHRONOUS},
   { OEMSUPPS_LOCAL_BSG_ALL_DED_PACKET,  OEMSUPPS_ALL_DATA_PDS_SERVICES},
   { OEMSUPPS_LOCAL_BSG_ALL_DED_PAD,     OEMSUPPS_ALL_PAD_ACCESS_CA_SERVICES},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_ALL,   OEMSUPPS_ALL_PLMN_SPECIFIC_BS},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_1,     OEMSUPPS_PLMN_SPECIFIC_BS_1 },
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_2,     OEMSUPPS_PLMN_SPECIFIC_BS_2 },
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_3,     OEMSUPPS_PLMN_SPECIFIC_BS_3},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_4,     OEMSUPPS_PLMN_SPECIFIC_BS_4},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_5,     OEMSUPPS_PLMN_SPECIFIC_BS_5},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_6,     OEMSUPPS_PLMN_SPECIFIC_BS_6},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_7,     OEMSUPPS_PLMN_SPECIFIC_BS_7},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_8,     OEMSUPPS_PLMN_SPECIFIC_BS_8},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_9,     OEMSUPPS_PLMN_SPECIFIC_BS_9},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_A,     OEMSUPPS_PLMN_SPECIFIC_BS_A},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_B,     OEMSUPPS_PLMN_SPECIFIC_BS_B},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_C,     OEMSUPPS_PLMN_SPECIFIC_BS_C},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_D,     OEMSUPPS_PLMN_SPECIFIC_BS_D},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_E,     OEMSUPPS_PLMN_SPECIFIC_BS_E},
   { OEMSUPPS_LOCAL_BSG_PLMN_BEAR_F,     OEMSUPPS_PLMN_SPECIFIC_BS_F}
};

#define OEMSSUPPS_MAX_STR_BSG_ENTRY (sizeof(OEMSUPPSStrBSGConversionTable)/sizeof(OEMSUPPSStrBSGTableType))

const OEMSUPPSTableType OEMSUPPSBSGConversionTable[] = {
   { AEETSUPPS_BASIC_SERVICE_ALL,                      OEMSUPPS_ALL_SERVICES },
   { AEETSUPPS_BASIC_SERVICE_ALL_TELESERVICES,         OEMSUPPS_TELEPHONY} ,
   { AEETSUPPS_BASIC_SERVICE_ALL_TELESERVICES,         OEMSUPPS_ALL_TELESERVICES},
   { AEETSUPPS_BASIC_SERVICE_ALL_SPEECH_TRANSMISSION,  OEMSUPPS_ALL_SPEECH_TRANSMISSION_SERVCES},
   { AEETSUPPS_BASIC_SERVICE_ALL_FAX,                  OEMSUPPS_ALL_FAX_TRANSMISSION_SERVICES},
   { AEETSUPPS_BASIC_SERVICE_ALL_SMS,                  OEMSUPPS_ALL_SMS_SERVICES},
   { AEETSUPPS_BASIC_SERVICE_ALL_BEARER,               OEMSUPPS_ALL_BEARER_SERVICES},
   { AEETSUPPS_BASIC_SERVICE_ALL_ASYNCHRONOUS,         OEMSUPPS_ALL_ASYNCHRONOUS_SERVICES},
   { AEETSUPPS_BASIC_SERVICE_ALL_SYNCHRONOUS,          OEMSUPPS_ALL_SYNCHRONOUS_SERVICES}
   
};
#define OEMSSUPPS_MAX_BSG_ENTRY (sizeof(OEMSUPPSBSGConversionTable)/sizeof(OEMSUPPSTableType))

const OEMSUPPSTableType OEMSUPPSCMCmdConversionTable[]=
{
  {AEETSUPPS_REQ_REGISTER,          CM_SUPS_CMD_REGISTER},
  {AEETSUPPS_REQ_ERASE,             CM_SUPS_CMD_ERASE},
  {AEETSUPPS_REQ_ACTIVATE,          CM_SUPS_CMD_ACTIVATE},
  {AEETSUPPS_REQ_DEACTIVATE,        CM_SUPS_CMD_DEACTIVATE},
  {AEETSUPPS_REQ_INTERROGATE,       CM_SUPS_CMD_INTERROGATE},
  {AEETSUPPS_REQ_REGISTER_PASSWD,   CM_SUPS_CMD_REG_PASSWORD},
  {AEETSUPPS_IND_ENTER_PASSWD,      CM_SUPS_CMD_GET_PASSWORD_RES},
  {AEETSUPPS_IND_USS,               CM_SUPS_CMD_PROCESS_USS},
  {AEETSUPPS_IND_USS_NOTIFY,        CM_SUPS_CMD_USS_NOTIFY_RES}
};

#define OEMSSUPPS_MAX_CMD (sizeof(OEMSUPPSCMCmdConversionTable)/sizeof(OEMSUPPSTableType))
#endif /* FEATURE_WCDMA || FEATURE_GSM */

typedef struct OEMSUPPS_NOTIFIER_notify_cb
{
  uint32 mask;
  AEETSuppsEventData user_data;
} OEMSUPPS_NOTIFIER_NOTIFY_CB;



#define OEMSUPPS_MAX_CB    5        /* Max pending event callbacks */

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static AEECriticalSection gcmCriticalSection;
#define OEMSUPPS_MUTEX_LOCK() AEECriticalSection_Enter(&gcmCriticalSection);
#define OEMSUPPS_MUTEX_UNLOCK() AEECriticalSection_Leave(&gcmCriticalSection);
#endif

/*Event node used to send messages from CM task to BREW dispatcher.*/
typedef struct OEMSuppsEvent 
{
  AEECallback          cb;         
  boolean              bInUse;
   cm_sups_event_e_type event;   /*CM Events or CM Errors*/
   OEMSuppsTrans*       pTrans;
   cm_sups_info_s_type  cm_sups_info;
   ACONTEXT*            pac;
} OEMSuppsEvent;


typedef struct {
   AEETSuppsMessage  *pMsg;
   uint32            dwExtra;
   uint32            dwNeeded;
   byte*             pEnd;
} SuppsBuffer;



/*A string can be broken down on into a set of tokens based on the regular
  expression*/
typedef int (*OEMSUPPS_PF_CHECK_TOKEN)(int ntokens, const char *tokens[],SuppsBuffer *);
typedef struct token_input
{
  const char *reg_exp;                /* Quasi-regular expression. */
  OEMSUPPS_PF_CHECK_TOKEN check_fn;      /* Function to be called. */
} OEMSUPPS_TOKEN_INPUT;


/************************************************************************
                  Internal Functions
************************************************************************/
static int OEMSUPPS_ChkUSSD(int num_tokens, const char *tokens[], SuppsBuffer *pBuf);
static int OEMSUPPS_ChkChangePin(int num_tokens, const char *tokens[], SuppsBuffer *pBuf);
static int OEMSUPPS_ChkUnblockPin(int num_tokens, const char *tokens[], SuppsBuffer *pBuf);
static int OEMSUPPS_ChkIMEI(int num_tokens, const char *tokens[], SuppsBuffer *pBuf);
static int OEMSUPPS_ChkSS(int num_tokens, const char *tokens[], SuppsBuffer *pBuf);
static int OEMSUPPS_RegexTokenize (const char *in_str, OEMSUPPS_TOKEN_INPUT *inputs, const char *tokens[], void *pBuf);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static int OEMSUPPS_ChangePin(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_UnblockPin(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_IMEI(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_NetworkIndReq(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_RegisterSS(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_EraseSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_ActivateSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_DeactivateSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_InterrogateSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_RegPwd (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_USSReq (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg);
static int OEMSUPPS_PwdRes (OEMSuppsTrans* pTrans, AEETSuppsResponse *ss_resp);
static int OEMSUPPS_USSNotifyRes (OEMSuppsTrans* pTrans, AEETSuppsResponse *ss_resp);
static int OEMSUPPS_USSRes (OEMSuppsTrans* pTrans, AEETSuppsResponse *ss_resp);
static int OEMSUPPS_ConvertBSGToNW( AEETSuppsMessage *pMsg);
static int OEMSUPPS_ConvertBSGToLocal( int basic_service);  
static uint32 OEMSUPPS_BCDToNumber(const byte *pBcdNumber,uint32  bcdLength,char *pAddress);
static int OEMSUPPS_Abort(OEMSuppsTrans* pTrans, AEETSuppsMessage* pMsg);
static __inline void OEMSUPPS_GetCompletedData(AEETSuppsStatus *pStatus, 
                                               const cm_sups_info_s_type* sups_info);

#endif 

typedef struct OEMSupps {
   OEMTelephone*     pOEMTelephone;
   OEMSuppsTrans*    pTrans;
   uint32            uRefs;
   byte              bTransId;
   OEMSuppsEvent     acb[OEMSUPPS_MAX_CB];/*Event Callbacks for NI*/
   AEECallback*      pcbList;
   cm_client_id_type cm_id;
}OEMSupps;

struct OEMTSupps {
   OEMSupps*      pSys;
   OEMSuppsEvent  evt;     
   AEECallback*   pcbApp;  /* Application call back of ongoing transaction. */
};

static struct OEMSupps* gpOEMSupps;

extern int OEMTelephone_SuppsReg(OEMTelephone* me, 
                                 cm_sups_event_f_type* pfnEvent,
                                 cm_sups_cmd_err_f_type* pfnCmdErr,
                                 cm_client_id_type* pId);
extern void OEMTelephone_Notify(AEETNotifyInfo *pData);


/* The tokens we handle. */
OEMSUPPS_TOKEN_INPUT OEMSUPPS_regex_input[] = {
  {"(\\d\\d)",OEMSUPPS_ChkUSSD},
  {"(\\d)",OEMSUPPS_ChkUSSD},
  { "\\*\\*04(2*)\\*{(\\d+)\\*(\\d+)\\*(\\d+)}#", OEMSUPPS_ChkChangePin },
  { "\\*\\*05(2*)\\*{(\\d+)\\*(\\d+)\\*(\\d+)}#", OEMSUPPS_ChkUnblockPin },
  { "\\*#06#", OEMSUPPS_ChkIMEI },
  {"([*#]+)(\\d+){\\*([\\+\\d]*)\\*(\\d+)\\*(\\d+)\\*(\\d+)}#",OEMSUPPS_ChkSS},
  { NULL, NULL }
};

/* Maximum number of tokens; this value corresponds to the maximum possible tokens
** by any of the strings above. */
#define OEMSUPPS_MAX_TOKENS 6

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/* List of functions that can Start an SS operation 
 * Note that the order of these functions is dependant upon the defines
 * above. 
 */
int (*OEMSuppsReqOp[])(OEMSuppsTrans*, AEETSuppsMessage*) =
{
  OEMSUPPS_ChangePin,            /* AEETSUPPS_REQ_CHANGE_PIN  */
  OEMSUPPS_UnblockPin,           /* AEETSUPPS_REQ_UNBLOCK_PIN */
  OEMSUPPS_IMEI,                 /* AEETSUPPS_REQ_DEVICE_IMEI  */
  OEMSUPPS_RegisterSS,           /* AEETSUPPS_REQ_REGISTER*/
  OEMSUPPS_EraseSS,              /* AEETSUPPS_REQ_ERASE*/
  OEMSUPPS_ActivateSS,           /* AEETSUPPS_REQ_ACTIVATE*/
  OEMSUPPS_DeactivateSS,         /* AEETSUPPS_REQ_DEACTIVATE*/
  OEMSUPPS_InterrogateSS,        /* AEETSUPPS_REQ_INTERROGATE*/
  OEMSUPPS_RegPwd,               /* AEETSUPPS_REQ_REGISTER_PASSWD*/
  OEMSUPPS_USSReq,               /* AEETSUPPS_REQ_USS*/
  OEMSUPPS_NetworkIndReq,        /* AEETSUPPS_REQ_NETWORK_IND*/
  OEMSUPPS_ChangePin,            /* AEETSUPPS_REQ_CHANGE_PIN2*/
  OEMSUPPS_UnblockPin,           /* AEETSUPPS_REQ_UNBLOCK_PIN2*/
};

/*Functions used to respond to the SS requests made by the newtork*/
int (*OEMSuppsRespOp[])(OEMSuppsTrans*, AEETSuppsResponse*) =
{
  OEMSUPPS_PwdRes,               /* AEETSUPPS_IND_ENTER_PASSWD*/
   OEMSUPPS_PwdRes,               /* AEETSUPPS_IND_ENTER_NEW_PASSWD*/
   OEMSUPPS_PwdRes,               /* AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN*/
   NULL,                          /* AEETSUPPS_IND_FWD_CHECK */
   OEMSUPPS_USSRes,               /* AEETSUPPS_IND_USS  */
  OEMSUPPS_USSNotifyRes,         /* AEETSUPPS_IND_USS_NOTIFY */
};

#endif

/***********************************************************************
 * D A T A    O B J E C T S
 ***********************************************************************/
static struct OEMSuppsEvent* niEvent = NULL;       /* Network indication */

/*===========================================================================
FUNCTION OEMSupps_ParseString

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  tokens global is updated.
===========================================================================*/
int OEMSupps_ParseString(const char *pszString, AEETSuppsMessage *pMsg, uint32 *pdwSize)
{
  int  nErr = SUCCESS;
  int  nLen;
  SuppsBuffer theBuf;
  const char *OEMSUPPS_tokens[OEMSUPPS_MAX_TOKENS];

  /* There is no string to parse or nothing to respond to */
  if( NULL == pszString || NULL == pdwSize || 0 == (nLen = STRLEN(pszString)) || NULL == pMsg ) {
     return EBADPARM;
  }

  /* Message buffer must be atleast sizeof(AEETSuppsMessage) */
  if( *pdwSize < sizeof(AEETSuppsMessage) ) {
     *pdwSize = sizeof(AEETSuppsMessage);
     return EBUFFERTOOSMALL;
  }

  /* String is larger than the known maximum */
  if( nLen > AEET_MAX_DIGITS_LENGTH ) {
     return EINVALIDFORMAT;
  }

  theBuf.pMsg = pMsg;
  theBuf.dwExtra = (*pdwSize - sizeof(AEETSuppsMessage));   /* Extra space in the buffer */
  theBuf.dwNeeded = 0;                                      /* To start with we donot need any more space */
  theBuf.pEnd = (byte*)pMsg + sizeof(AEETSuppsMessage);

  /*Tokenize the String*/
  nErr = OEMSUPPS_RegexTokenize(pszString, OEMSUPPS_regex_input, OEMSUPPS_tokens, &theBuf);

  *pdwSize += theBuf.dwNeeded;                           /* Add the needed size to the presented size */

  return nErr;
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
static int OEMSUPPSTable_XlateLocal(const OEMSUPPSTableType* pTable, int nSize, int nEntry)
{
   int i;
   int nRet = 0;

   for (i = 0; i < nSize; i++) {
      if(pTable[i].local == nEntry){
         nRet = pTable[i].net_code;
         break;
      }
   }

   return nRet;
}

/*===========================================================================
===========================================================================*/
static boolean OEMSupps_IsBasicRequest(AEETSuppsMessage* pMsg)
{
   boolean bRet = FALSE;

   if (AEETSUPPS_MSG_REQUEST == pMsg->type) {
      if (AEETSUPPS_REQ_DEVICE_IMEI < pMsg->data.request.operation
          && pMsg->data.request.operation < AEETSUPPS_REQ_NETWORK_IND) {
         bRet = TRUE;
      }
   }

   return bRet;
}
#endif

/*===========================================================================
===========================================================================*/
static void OEMSupps_Cancel(AEECallback *pcb)
{
   OEMSuppsTrans* pTrans = (OEMSuppsTrans*)pcb->pReserved;
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   AEETSuppsMessage* pMsg = (AEETSuppsMessage*)pcb->pCancelData;
#endif

   MSG_HIGH("==>OEMTSups_Cancel tid : %d", pTrans->m_transId, 0, 0);
   pTrans->pOEMTSupps->pcbApp = (AEECallback*)0;
   pcb->pfnCancel = 0;
   pcb->pReserved = 0;
   pcb->pCancelData = 0;

   /* Cancel the event if queued to dispatcher */
   CALLBACK_Cancel(&pTrans->pOEMTSupps->evt.cb);

   if (AEETSUPPS_STATE_RUNNING == pTrans->m_transState) {
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
      /* @@@ TODO : This does not abort GSDI operations */
      if (OEMSupps_IsBasicRequest(pMsg)) {
         OEMSUPPS_Abort(pTrans, pMsg);
      }
#endif
      pTrans->m_transState = AEETSUPPS_STATE_DONE;
}
}

/*===========================================================================
===========================================================================*/
static void OEMSupps_MessageDone(OEMSuppsTrans* pTrans)
{
   AEECallback *pcb = pTrans->pOEMTSupps->pcbApp;
   MSG_HIGH("==>OEMSupps_MessageDone tid : %d", pTrans->m_transId, 0, 0);
   AEE_ResumeCallback(pcb, pTrans->pOEMTSupps->evt.pac);
}

/*=============================================================================
=============================================================================*/
static void OEMSupps_CmdErr(OEMSuppsTrans* pTrans)
{

   MSG_HIGH("==>OEMSupps_CmdErr tid : %d", pTrans->m_transId, 0, 0);
   /* Send the response to application */
   pTrans->m_transState = AEETSUPPS_STATE_DONE;

   pTrans->m_result.status = AEETSUPPS_STATUS_COMPLETE;
   pTrans->m_result.data.Result.success = FALSE;
   pTrans->m_result.data.Result.result_type = AEETSUPPS_SS_ERROR_INFO;
   pTrans->m_result.data.Result.data.ss_error = AEETSUPPS_ERR_UNRECOGNISED_ERROR;

   OEMSupps_MessageDone(pTrans);
}

/*=============================================================================
=============================================================================*/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static void OEMSupps_MessageErr(OEMSuppsTrans* pTrans, uint32 uError)
{

   MSG_HIGH("==>OEMSupps_MessageErr tid : %d", pTrans->m_transId, 0, 0);
   /* Send the response to application */
   pTrans->m_transState = AEETSUPPS_STATE_DONE;

   pTrans->m_result.status = AEETSUPPS_STATUS_COMPLETE;
   pTrans->m_result.data.Result.success = FALSE;
   pTrans->m_result.data.Result.result_type = AEETSUPPS_SS_ERROR_INFO;
   pTrans->m_result.data.Result.data.ss_error = uError;
}
#endif

/*===========================================================================
===========================================================================*/
static void OEMSupps_ReleaseStatus(AEETSuppsStatus* pStatus)
{
   if (NULL == pStatus) {
      return;
   }

   if (AEETSUPPS_STATUS_IND == pStatus->status) {
      if (AEETSUPPS_IND_USS == pStatus->data.NetworkInd.ind_type
          || AEETSUPPS_IND_USS_NOTIFY == pStatus->data.NetworkInd.ind_type) {
         FREEIF(pStatus->data.NetworkInd.ind_data.data);
      }
   }
   else if (AEETSUPPS_STATUS_COMPLETE == pStatus->status) {
      switch (pStatus->data.Result.result_type) {
      case AEETSUPPS_FWD_INFO: {
         uint32 u;
         AEETSuppsForwardingInfo* pInfo = &pStatus->data.Result.data.fwd_info;

         for (u = 0; u < pInfo->feature_list.length; u++) {
            FREEIF(pInfo->feature_list.fwd_feature[u].fwd_to_number.data);
            FREEIF(pInfo->feature_list.fwd_feature[u].fwd_to_subaddress.data);
            FREEIF(pInfo->feature_list.fwd_feature[u].long_fwd_to_number.data);
         }
         break;
      }
      case AEETSUPPS_FWD_FEATURE_LIST_INFO: {
         AEETSuppsFwdFeatureList* pFwdList = &pStatus->data.Result.data.fwd_feature_list;
         uint32 u;

         for (u = 0; u < pFwdList->length; u++) {
            FREEIF(pFwdList->fwd_feature[u].fwd_to_number.data);
            FREEIF(pFwdList->fwd_feature[u].long_fwd_to_number.data);
            FREEIF(pFwdList->fwd_feature[u].fwd_to_subaddress.data);
         }
         
         break;
      }
      case AEETSUPPS_CALL_BARRING_INFO:
         break;
      case AEETSUPPS_CLI_RESTRICTION_INFO:
         break;
      case AEETSUPPS_CUG_INFO: {
         AEETSuppsData* pCUGInfo = &pStatus->data.Result.data.cug_info;

         FREEIF(pCUGInfo->data);

         break;
      }
      case AEETSUPPS_BSG_LIST_INFO:
         break;
      case AEETSUPPS_USS_DATA_INFO: {
         AEETSuppsData* pUSSData = &pStatus->data.Result.data.uss_data_info;

         FREEIF(pUSSData->data);
         break;
      }
      case AEETSUPPS_USSD_DATA_INFO:{
         AEETSuppsData* pUSSData = &pStatus->data.Result.data.ussd_data_info;

         FREEIF(pUSSData->data);
         break;
      }
      case AEETSUPPS_SS_DATA_INFO:
         break;
      case AEETSUPPS_SS_STATUS_INFO:
         break;
      case AEETSUPPS_SS_ERROR_INFO:
         break;
      case AEETSUPPS_CC_CAUSE_INFO:
         break;
      }
   }
}

/*===========================================================================
FUNCTION OEMSupps_SendMessage

DESCRIPTION
  This function is called to start the SS running.  Based on the info in the 
  AEETSuppsMessage, the Supplementary Service Operation is started
  If it successfully is started, the event is sent out, otherwise
  it returns an error which this function returns.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
int OEMTSupps_SendMessage(OEMTSupps* pme, AEETSuppsMessage *pMsg, AEECallback *pcb)
{
   OEMSuppsTrans* pTrans = pme->evt.pTrans;
   int nErr = AEE_SUCCESS;

   if( !OEMTelephone_IsModeGW(OEMTelephone_GetSystemMode(0)) ) {
      return AEET_ERR_OP_NOT_ALLOWED;
   }

   if ((AEECallback*)0 != pme->pcbApp) {
      return EALREADY;
   }

   pme->pcbApp = pcb;
   pcb->pfnCancel = OEMSupps_Cancel;
   pcb->pCancelData = (void *)pMsg;
   pcb->pReserved = (void*)pTrans;

   MSG_HIGH("==>OEMTSupps_SendMessage tid : %d", pTrans->m_transId, 0, 0);

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   if (AEETSUPPS_MSG_REQUEST == pMsg->type) {
      if (OEMSupps_IsBasicRequest(pMsg)) {
         nErr = OEMSUPPS_ConvertBSGToNW(pMsg);
         if (AEE_SUCCESS != nErr) {
            OEMSupps_MessageErr(pTrans, AEETSUPPS_ERR_BADLY_STRUCTURED_COMPONENT);
            return SUCCESS;
         }
      }

      (void) OEMSuppsReqOp[pMsg->data.request.operation](pTrans, pMsg);
   }
   else if (AEETSUPPS_MSG_RESPONSE == pMsg->type) {
      int (*pfn)(OEMSuppsTrans* pTrans, AEETSuppsResponse* pR);
      pfn = OEMSuppsRespOp[pTrans->m_result.data.NetworkInd.ind_type];
      if (NULL != pfn) {
         (void) pfn(pTrans, &pMsg->data.response);
      }
      else {
         OEMSupps_MessageErr(pTrans, AEETSUPPS_ERR_BADLY_STRUCTURED_COMPONENT);
      }
   }
   else {
      OEMSupps_MessageErr(pTrans, AEETSUPPS_ERR_UNRECOGNISED_COMPONENT);
   }
#else
   nErr = EUNSUPPORTED;
   OEMSupps_CmdErr(pTrans);
#endif

   return nErr;
}

/*===========================================================================
===========================================================================*/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static __inline
void OEMSupps_Notify(AEETNotifyInfo* pNotify, OEMSuppsEvent* pEvt)
{
   niEvent = pEvt;
   MSG_HIGH("==>OEMSupps_Notify", 0, 0, 0);
   OEMTelephone_Notify(pNotify);
   niEvent = NULL;
}
#endif

/*===========================================================================
===========================================================================*/
static __inline
void OEMSuppsEvt_Free(OEMSuppsEvent* pEvt)
{
   /* Cancel the event if queued to dispatcher */
   CALLBACK_Cancel(&pEvt->cb);
   pEvt->bInUse = FALSE;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static __inline
int OEMSupps_New_DecodedGSMAlphaBuf(void** ppDest, uint32* puLen,
                                    const void* pSrc, uint32 uSrc)
{
   uint32 src_chars = (uSrc * 8) / 7;
   char *uss_data = MALLOC(src_chars+1);

   if ((char*)0 == uss_data) {
      return ENOMEMORY;
   }

   /* convert gsm alphabet to ascii */
   cm_util_gsm_alphabet_to_ascii((byte*)uss_data, (byte*)pSrc, 
                                 (byte)((7 * (src_chars >> 3)) 
                                  + (src_chars & 0x07)));

   *ppDest = uss_data;
   *puLen  = src_chars;

   return AEE_SUCCESS;
}
#endif

/*===========================================================================
** Handle Application initiated message
**    This runs in application context.
===========================================================================*/
static void OEMSuppsEvt_HandleAI(OEMSuppsEvent* pEvt)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   cm_sups_info_s_type* sups_info;
   cm_sups_event_e_type sups_event;
   OEMSuppsTrans *pTrans;

   sups_info = &pEvt->cm_sups_info;
   sups_event = pEvt->event;
   pTrans = pEvt->pTrans;

   MSG_HIGH("==> OEMSuppsEvt_HandleAI Evt 0x%X tid %d", sups_event, pTrans->m_transId, 0);
   /*Process the CM event.*/
   MSG_HIGH("SUPS Ref %d Code %d", sups_info->ss_ref, sups_info->ss_code, 0);
   switch (sups_event){
   /*--------------------------Error Events------------------------------------*/
   case CM_SUPS_EVENT_NONE:
      OEMSupps_CmdErr(pTrans);
      break;
      /*--------------------------Started Events------------------------------------*/
      /*For all the started events. Wait for the CONF event from CM*/
   case CM_SUPS_EVENT_REGISTER:
   case CM_SUPS_EVENT_ACTIVATE:
   case CM_SUPS_EVENT_DEACTIVATE:
   case CM_SUPS_EVENT_ERASE:
   case CM_SUPS_EVENT_INTERROGATE:
   case CM_SUPS_EVENT_REG_PASSWORD:
   case CM_SUPS_EVENT_PROCESS_USS:
      /* do nothing */
      break;

      /*-------------------------Confirmation Events----------------------------------*/
      /*For all the Completed Events, the state is changed to AEETSUPPS_STATE_DONE. Send all
      **the information in the right format to the client*/

   case CM_SUPS_EVENT_REGISTER_CONF:
   case CM_SUPS_EVENT_ACTIVATE_CONF:
   case CM_SUPS_EVENT_DEACTIVATE_CONF:
   case CM_SUPS_EVENT_ERASE_CONF:
   case CM_SUPS_EVENT_INTERROGATE_CONF:
   case CM_SUPS_EVENT_REG_PASSWORD_CONF:
   case CM_SUPS_EVENT_PROCESS_USS_CONF:
      OEMSupps_ReleaseStatus(&pTrans->m_result);
      OEMSUPPS_GetCompletedData(&pTrans->m_result,sups_info);
      pTrans->m_transState = AEETSUPPS_STATE_DONE;
      OEMSupps_MessageDone(pTrans);
      break;


      /*---------------------------NW Init Events------------------------------*/
      /*Network Initiated Events. A response is required from the client for each
         **of these events*/

   case CM_SUPS_EVENT_FWD_CHECK_IND: {

      OEMSupps_ReleaseStatus(&pTrans->m_result);
      pTrans->m_result.status = AEETSUPPS_STATUS_IND;
      pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_FWD_CHECK;
      pTrans->m_transState = AEETSUPPS_STATE_NW_INDICATION;

      OEMSupps_MessageDone(pTrans);
      break;
   }

   case CM_SUPS_EVENT_USS_NOTIFY_IND: {

      OEMSupps_ReleaseStatus(&pTrans->m_result);
      pTrans->m_result.status = AEETSUPPS_STATUS_IND;
      pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_USS_NOTIFY;

      /*Convert the Sups data to AECHAR*/
      if (OEMSUPPS_DEFAULT_ALPHABET 
          == (sups_info->uss_data.uss_data_coding_scheme & OEMSUPPS_ALPHABET_MASK)) {

         if (AEE_SUCCESS != OEMSupps_New_DecodedGSMAlphaBuf(
            (void**)&pTrans->m_result.data.NetworkInd.ind_data.data,
            &pTrans->m_result.data.NetworkInd.ind_data.length,
            sups_info->uss_data.ussData,
            sups_info->uss_data.size)) {
            pTrans->m_result.data.NetworkInd.ind_data.data = NULL;
            pTrans->m_result.data.NetworkInd.ind_data.length = 0;
         }
      }

      pTrans->m_transState = AEETSUPPS_STATE_NW_INDICATION;

      OEMSupps_MessageDone(pTrans);
      break;
   }

   case CM_SUPS_EVENT_USS_IND: {

      OEMSupps_ReleaseStatus(&pTrans->m_result);
      pTrans->m_result.status = AEETSUPPS_STATUS_IND;
      pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_USS;

      /*Change the current state to AEETSUPPS_STATE_NW_INDICATION-> App needs to respond to this event*/
      pTrans->m_transState = AEETSUPPS_STATE_NW_INDICATION;

      if (OEMSUPPS_DEFAULT_ALPHABET 
          == (sups_info->uss_data.uss_data_coding_scheme & OEMSUPPS_ALPHABET_MASK)){

        if (AEE_SUCCESS != OEMSupps_New_DecodedGSMAlphaBuf(
           (void**)&pTrans->m_result.data.NetworkInd.ind_data.data,
           &pTrans->m_result.data.NetworkInd.ind_data.length,
           sups_info->uss_data.ussData,
           sups_info->uss_data.size)) {
           pTrans->m_result.data.NetworkInd.ind_data.data = NULL;
           pTrans->m_result.data.NetworkInd.ind_data.length = 0;
        }
      }

      OEMSupps_MessageDone(pTrans);
      break;
   }

   case CM_SUPS_EVENT_GET_PASSWORD_IND: {

      OEMSupps_ReleaseStatus(&pTrans->m_result);
      pTrans->m_result.status = AEETSUPPS_STATUS_IND;
      pTrans->m_transState = AEETSUPPS_STATE_NW_INDICATION;

      /*Get the Pwd Info*/
      if (enterPW == sups_info->guidance_info) {
         pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_ENTER_PASSWD;
      }
      else if (enterNewPW == sups_info->guidance_info) {
         pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_ENTER_NEW_PASSWD;
      }
      else if (enterNewPW_Again == sups_info->guidance_info) {
         pTrans->m_result.data.NetworkInd.ind_type = AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN;
      }

      OEMSupps_MessageDone(pTrans);
      break;
   }

   /*---------------------------Response to NW events-----------------------*/
   /*Response sent from the Client to network initiated events*/

   case CM_SUPS_EVENT_USS_NOTIFY_RES:
   case CM_SUPS_EVENT_USS_RES:
   case CM_SUPS_EVENT_GET_PASSWORD_RES:
      pTrans->m_result.status = AEETSUPPS_STATUS_COMPLETE;
      pTrans->m_transState = AEETSUPPS_STATE_DONE;
      pTrans->m_result.data.Result.result_type = AEETSUPPS_NO_INFO;
      pTrans->m_result.data.Result.success = TRUE;
      OEMSupps_MessageDone(pTrans);
      break;

   /*---------------------------Release Events------------------------------*/

   case CM_SUPS_EVENT_RELEASE_USS_IND:
   case CM_SUPS_EVENT_RELEASE:
      pTrans->m_transState = AEETSUPPS_STATE_DONE;
      OEMSupps_MessageDone(pTrans);
      break;

   default:
      MSG_ERROR("unhandled SUPS Evt 0x%X", sups_event, 0, 0);
      break;
   }
#endif
}

/*===========================================================================
** Handle Network Indication
**    Send the AEETNotifyInfo and Free the Event buffer.
===========================================================================*/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static void OEMSuppsEvt_HandleNI(OEMSuppsEvent* pEvt)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   cm_sups_info_s_type* sups_info;
   cm_sups_event_e_type sups_event;

   sups_info = &pEvt->cm_sups_info;
   sups_event = pEvt->event;

   MSG_HIGH("==> OEMSuppsEvt_HandleNI Evt 0x%X tid %d", sups_event, sups_info->invoke_id, 0);
   /*Process the NI event.*/
   MSG_HIGH("SUPS Ref %d Code %d", sups_info->ss_ref, sups_info->ss_code, 0);
   switch (sups_event){

      /*---------------------------NW Init Events------------------------------*/
      /*Network Initiated Events. A response is required from the client for each
         **of these events*/

   case CM_SUPS_EVENT_FWD_CHECK_IND: {
      AEETNotifyInfo NotifyInfo;

      NotifyInfo.event = AEET_EVENT_SUPPS_NETWORK_IND;
      NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_FWD_CHECK;
      NotifyInfo.event_data.supps.transaction_id = sups_info->invoke_id;

      OEMSupps_Notify(&NotifyInfo, pEvt);

      break;
   }

   case CM_SUPS_EVENT_USS_NOTIFY_IND: {
      AEETNotifyInfo NotifyInfo;

      NotifyInfo.event = AEET_EVENT_SUPPS_NETWORK_IND;
      NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_USS_NOTIFY;
      NotifyInfo.event_data.supps.transaction_id = sups_info->invoke_id;

      OEMSupps_Notify(&NotifyInfo, pEvt);

      break;
   }

   case CM_SUPS_EVENT_USS_IND: {
      AEETNotifyInfo NotifyInfo;

      NotifyInfo.event = AEET_EVENT_SUPPS_NETWORK_IND;
      NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_USS;
      NotifyInfo.event_data.supps.transaction_id = sups_info->invoke_id;

      OEMSupps_Notify(&NotifyInfo, pEvt);

      break;
   }

   case CM_SUPS_EVENT_GET_PASSWORD_IND: {
      AEETNotifyInfo NotifyInfo;

      NotifyInfo.event = AEET_EVENT_SUPPS_NETWORK_IND;
      NotifyInfo.event_data.supps.transaction_id = sups_info->invoke_id;

      /*Get the Pwd Info*/
      if (enterPW == sups_info->guidance_info) {
         NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_ENTER_PASSWD;
      }
      else if (enterNewPW == sups_info->guidance_info) {
         NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_ENTER_NEW_PASSWD;
      }
      else if (enterNewPW_Again == sups_info->guidance_info) {
         NotifyInfo.event_data.supps.ind_type = AEETSUPPS_IND_ENTER_NEW_PASSWD_AGAIN;
      }

      OEMSupps_Notify(&NotifyInfo, pEvt);

      break;
   }

   default:
      MSG_ERROR("unhandled SUPS Evt 0x%X", sups_event, 0, 0);
      break;
   }
#endif 

   OEMSuppsEvt_Free(pEvt);
}

/*===========================================================================
===========================================================================*/
static __inline
OEMSuppsEvent* OEMSuppsEvt_Alloc(OEMSupps* pme)
{
   int i;
   OEMSuppsEvent *pEvt = 0;

   OEMSUPPS_MUTEX_LOCK();
   for (i = 0; i < OEMSUPPS_MAX_CB; i++) {
      if (FALSE == pme->acb[i].bInUse) {
         pEvt = &pme->acb[i];
         pEvt->bInUse = TRUE;
         /*Initialize AEECallback pointer */
         CALLBACK_Init(&pEvt->cb, OEMSuppsEvt_HandleNI, pEvt);
         break;
      }
   }
   OEMSUPPS_MUTEX_UNLOCK();

   return pEvt;
}

/*===========================================================================
===========================================================================*/
static __inline
void OEMSuppsEvt_Dispatch(OEMSuppsEvent* pEvt)
{
   AEE_ResumeCallback(&pEvt->cb, pEvt->pac);
}
#endif

/*===========================================================================
===========================================================================*/
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
static __inline
OEMSuppsEvent* OEMSuppsEvt_FromAEECallback(AEECallback* pcb)
{
   if (pcb) {
      return RECOVER_MEMBER(OEMSuppsEvent, cb, pcb);
   } 
   else {
      return NULL;
   }
}

/*===========================================================================
===========================================================================*/
static
void OEMSuppsEvt_Cancel(AEECallback* pcb)
{
   OEMSuppsEvent* pEvt = OEMSuppsEvt_FromAEECallback(pcb);
   OEMSupps* pme = (OEMSupps*)pcb->pCancelData;
   AEECallback** ppcb;

   MSG_HIGH("==> OEMSuppsEvt_Cancel id: 0x%X", pEvt->pTrans->m_transId, 0, 0);

   OEMSUPPS_MUTEX_LOCK();
   pcb->pCancelData = 0;
   pcb->pfnCancel = NULL;

   ppcb = &pme->pcbList;
   while(*ppcb) {

      if (*ppcb == pcb) {
         *ppcb = pcb->pNext;
         break;
      }
      ppcb = &(*ppcb)->pNext;
   }

   OEMSUPPS_MUTEX_UNLOCK();
}
#endif

/***********************************************************************
                 * INTERNAL      FUNCTIONS*
 ***********************************************************************/
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*===========================================================================
FUNCTION  OEMSUPPS_GetSSError 

DESCRIPTION
  Converts the CM SS Error into SuppSvc SS Error

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Add command in UI command buffer
===========================================================================*/
AEETSuppsErrorType OEMSUPPS_GetSSError
(
  const ie_cm_ss_error_s_type *pIError
)
{
  AEETSuppsErrorType      ss_error = AEETSUPPS_ERR_NO_ERR;
  if(pIError->present)
  {
    ss_error = (AEETSuppsErrorType)pIError->error_code | AEETSUPPS_BM(pIError->error_code_tag);
    MSG_HIGH("Received SS Error %d from CM",ss_error,0,0);
  }
  return ss_error;
}/*OEMSUPPS_GetSSError*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetCCCause 

DESCRIPTION
  Converts the CM CC Cause into ICM CC Cause

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Add command in UI command buffer
===========================================================================*/
static __inline
AEETEndCause OEMSUPPS_GetCCCause
(
  const ie_cm_cc_cause_T *pICause
)
{
  if(!pICause->present){
    MSG_HIGH("CC Cause Info with no Info",0,0,0);
    return AEET_END_UNAVAILABLE;
  }

  MSG_HIGH("CC Cause Value %d",pICause->cause_value,0,0);
  return ((AEETEndCause)pICause->cause_value | 0x0100);
}

     
/*===========================================================================
FUNCTION  OEMSUPPS_GetSSStatus

DESCRIPTION
  Converts the CM SS Status into SuppSvc SS Status

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
boolean OEMSUPPS_GetSSStatus
(
  const ie_ss_status_s_type *pIssStatus,
  AEETSuppsOpStatus        *pOssStatus
)
{
  if(pIssStatus->present)
  {
     pOssStatus->ss_active      = pIssStatus->cm_ss_active;
     pOssStatus->ss_operative   = pIssStatus->cm_ss_operative;
     pOssStatus->ss_provisioned = pIssStatus->cm_ss_provisioned;
     pOssStatus->ss_registered  = pIssStatus->cm_ss_registered;
     MSG_HIGH("Received SS Status from CM",0,0,0);
     return TRUE;
  }
  return FALSE;
}/*OEMSUPPS_GetSSStatus*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetCallBarringInfo

DESCRIPTION
  Converts the CM Call Barring Info into SuppSvc Call Barring Info

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
 
===========================================================================*/
static __inline
boolean OEMSUPPS_GetCallBarringInfo
(
  const ie_call_barring_info_s_type *pICallBarring,
  AEETSuppsCallBarringInfo         *pOCallBarring
)
{
  uint32 i;
   if(!pICallBarring->present)
     return FALSE;

  MSG_HIGH("Received Call Barring Info from CM",0,0,0);

  /*Supplementary Service Code*/
  if(pICallBarring->cm_ss_code.present)
    pOCallBarring->ss_code = (AEETSuppsCodeType)pICallBarring->cm_ss_code.ss_code;

  /*Info Length*/
  pOCallBarring->length = MIN(pICallBarring->call_barring_info_length,
                              AEETSUPPS_CALL_BARRING_LIST_SIZE);


  for(i=0;i<pOCallBarring->length;i++)
  {
    /*Get the Basic Service Type*/
    if(pICallBarring->call_barring_feature_list[i].basic_service.present)
    {
      pOCallBarring->call_barring_feature_list[i].basic_service =
         (AEETSuppsBasicServiceType)(AEETSUPPS_BM(pICallBarring->call_barring_feature_list[i].basic_service.code_type) |
                                      pICallBarring->call_barring_feature_list[i].basic_service.bs_code);

    pOCallBarring->call_barring_feature_list[i].basic_service=
      (AEETSuppsBasicServiceType)OEMSUPPS_ConvertBSGToLocal(pOCallBarring->call_barring_feature_list[i].basic_service);
    }
    // Get the SS status as well
    (void)OEMSUPPS_GetSSStatus(&pICallBarring->call_barring_feature_list[i].cm_ss_status,
                             &pOCallBarring->call_barring_feature_list[i].ss_status);
  }
  return TRUE;
}/*OEMSUPPS_GetCallBarringInfo*/


/*===========================================================================
FUNCTION  OEMSUPPS_GetFwdInfo

DESCRIPTION
  Converts the Fwd Info into SuppSvc Fwd  Info

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
boolean OEMSUPPS_GetFwdInfo(const ie_forwarding_info_s_type* pIfwdInfo, 
                            AEETSuppsForwardingInfo* pOfwdInfo)
{
  uint32 i;
  (void)MEMSET(pOfwdInfo, 0, sizeof(AEETSuppsForwardingInfo));

  if (!pIfwdInfo->present)
  {
    return FALSE;
  }

  MSG_HIGH("Received Fwd Info from CM",0,0,0);
  if(pIfwdInfo->cm_ss_code.present)
    pOfwdInfo->ss_code = (AEETSuppsCodeType) pIfwdInfo->cm_ss_code.ss_code;
  
  pOfwdInfo->feature_list.length=
                MIN(pIfwdInfo->forwarding_feature_list.forwarding_feature_list_length,AEETSUPPS_FWD_FEATURE_LIST_SIZE);

  /*Get the Info for all*/
  for(i=0;i<pOfwdInfo->feature_list.length;i++)
  {
  /*Basic Service*/
    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].basic_service.present)
    {
      pOfwdInfo->feature_list.fwd_feature[i].basic_service
       =(AEETSuppsBasicServiceType)(AEETSUPPS_BM(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].basic_service.code_type) |
                                     pIfwdInfo->forwarding_feature_list.forwarding_feature[i].basic_service.bs_code);

    /*Convert to Local Values*/
    pOfwdInfo->feature_list.fwd_feature[i].basic_service=
        (AEETSuppsBasicServiceType)OEMSUPPS_ConvertBSGToLocal(pOfwdInfo->feature_list.fwd_feature[i].basic_service);
    }

  /*Get the SS Status*/
    (void)OEMSUPPS_GetSSStatus(&pIfwdInfo->forwarding_feature_list.forwarding_feature[i].ss_status,
                             &pOfwdInfo->feature_list.fwd_feature[i].ss_status);


  /*Get the forward to Number*/
    pOfwdInfo->feature_list.fwd_feature[i].fwd_to_number.length =0;

    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[0].present)
    {
      pOfwdInfo->feature_list.fwd_feature[i].fwd_to_number.data = 
             (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->feature_list.fwd_feature[i].fwd_to_number.length
            =OEMSUPPS_BCDToNumber((byte*)pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[0].data,
                                    pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[0].length,
                                    (char*)pOfwdInfo->feature_list.fwd_feature[i].fwd_to_number.data);
    }

    pOfwdInfo->feature_list.fwd_feature[i].long_fwd_to_number.length =0;

    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[1].present)
    {
      pOfwdInfo->feature_list.fwd_feature[i].long_fwd_to_number.data = 
             (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->feature_list.fwd_feature[i].long_fwd_to_number.length=
         OEMSUPPS_BCDToNumber(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[1].data,
                                pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[1].length,
                                pOfwdInfo->feature_list.fwd_feature[i].long_fwd_to_number.data);

    }

    pOfwdInfo->feature_list.fwd_feature[i].fwd_to_subaddress.length = 0;

    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_subaddress.present)
    {
      pOfwdInfo->feature_list.fwd_feature[i].fwd_to_subaddress.data = 
             (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->feature_list.fwd_feature[i].fwd_to_subaddress.length = 
          OEMSUPPS_BCDToNumber(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_subaddress.data,
                                 pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarded_to_subaddress.length,
                                 pOfwdInfo->feature_list.fwd_feature[i].fwd_to_subaddress.data);
    }

  /*Get the forwarding options*/
    pOfwdInfo->feature_list.fwd_feature[i].fwd_option.fwd_reason  = AEETSUPPS_FWD_REASON_FWD_REASON_NONE;

    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarding_options.present)
    {
       pOfwdInfo->feature_list.fwd_feature[i].fwd_option.fwd_reason 
          =(AEETSuppsFwdReasonType)pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarding_options.forwarding_reason;
       pOfwdInfo->feature_list.fwd_feature[i].fwd_option.notification_to_cp
          = pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarding_options.notification_to_cp;
       pOfwdInfo->feature_list.fwd_feature[i].fwd_option.notification_to_fp
          = pIfwdInfo->forwarding_feature_list.forwarding_feature[i].forwarding_options.notification_to_fp;
    }

  /*Get the timer value*/
    if(pIfwdInfo->forwarding_feature_list.forwarding_feature[i].no_reply_condition_timer.present)
    {
      pOfwdInfo->feature_list.fwd_feature[i].timer_value =
          pIfwdInfo->forwarding_feature_list.forwarding_feature[i].no_reply_condition_timer.timer_value;
    }

  }
 return TRUE;
}/*OEMSUPPS_GetFwdInfo*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetFwdFeatureListInfo

DESCRIPTION
  Converts the Fwd Info into SuppSvc Fwd  Info

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
boolean OEMSUPPS_GetFwdFeatureListInfo
(  
   const ie_forwarding_feature_list_s_type *pIfwdInfo,
   AEETSuppsFwdFeatureList                *pOfwdInfo
)
{ 
  uint32 i;
  (void)MEMSET(pOfwdInfo, 0, sizeof(AEETSuppsFwdFeatureList));

  if (!pIfwdInfo->present)
  {
    return FALSE;
  }

  MSG_HIGH("Received Fwd Feature List Info from CM",0,0,0);
  
  pOfwdInfo->length=
                MIN(pIfwdInfo->forwarding_feature_list_length,AEETSUPPS_FWD_FEATURE_LIST_SIZE);

  /*Get the Info for all*/
  for(i=0;i<pOfwdInfo->length;i++)
  {
  /*Basic Service*/
    if(pIfwdInfo->forwarding_feature[i].basic_service.present)
    {
      pOfwdInfo->fwd_feature[i].basic_service
       =(AEETSuppsBasicServiceType)(AEETSUPPS_BM(pIfwdInfo->forwarding_feature[i].basic_service.code_type) |
                                     pIfwdInfo->forwarding_feature[i].basic_service.bs_code);

    /*Convert to Local Values*/
    pOfwdInfo->fwd_feature[i].basic_service=
      (AEETSuppsBasicServiceType)OEMSUPPS_ConvertBSGToLocal(pOfwdInfo->fwd_feature[i].basic_service);
    }

  /*Get the SS Status*/
    (void)OEMSUPPS_GetSSStatus(&pIfwdInfo->forwarding_feature[i].ss_status,
                             &pOfwdInfo->fwd_feature[i].ss_status);


  /*Get the forward to Number*/
    pOfwdInfo->fwd_feature[i].fwd_to_number.length = 0;

    if(pIfwdInfo->forwarding_feature[i].forwarded_to_number[0].present)
    {
      pOfwdInfo->fwd_feature[i].fwd_to_number.data
               = (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->fwd_feature[i].fwd_to_number.length = 
        OEMSUPPS_BCDToNumber(pIfwdInfo->forwarding_feature[i].forwarded_to_number[0].data,
                               pIfwdInfo->forwarding_feature[i].forwarded_to_number[0].length,
                             pOfwdInfo->fwd_feature[i].fwd_to_number.data);
    }

    pOfwdInfo->fwd_feature[i].long_fwd_to_number.length = 0;

    if(pIfwdInfo->forwarding_feature[i].forwarded_to_number[1].present)
    {
      pOfwdInfo->fwd_feature[i].long_fwd_to_number.data
               = (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->fwd_feature[i].long_fwd_to_number.length = 
        OEMSUPPS_BCDToNumber(pIfwdInfo->forwarding_feature[i].forwarded_to_number[1].data,
                               pIfwdInfo->forwarding_feature[i].forwarded_to_number[1].length,
                               pOfwdInfo->fwd_feature[i].long_fwd_to_number.data);
    }

    pOfwdInfo->fwd_feature[i].fwd_to_subaddress.length = 0;

    if(pIfwdInfo->forwarding_feature[i].forwarded_to_subaddress.present)
    {
      pOfwdInfo->fwd_feature[i].fwd_to_subaddress.data
               = (char*)MALLOC(OEMSUPPS_MAX_FWD_TO_NUMBER);

      pOfwdInfo->fwd_feature[i].fwd_to_subaddress.length = 
        OEMSUPPS_BCDToNumber(pIfwdInfo->forwarding_feature[i].forwarded_to_subaddress.data,
                               pIfwdInfo->forwarding_feature[i].forwarded_to_subaddress.length,
                               pOfwdInfo->fwd_feature[i].fwd_to_subaddress.data);
    }


  /*Get the forwarding options*/
   pOfwdInfo->fwd_feature[i].fwd_option.fwd_reason =AEETSUPPS_FWD_REASON_FWD_REASON_NONE;

   if(pIfwdInfo->forwarding_feature[i].forwarding_options.present){
      pOfwdInfo->fwd_feature[i].fwd_option.fwd_reason = 
           (AEETSuppsFwdReasonType)pIfwdInfo->forwarding_feature[i].forwarding_options.forwarding_reason;

      pOfwdInfo->fwd_feature[i].fwd_option.notification_to_cp =
           pIfwdInfo->forwarding_feature[i].forwarding_options.notification_to_cp;

      pOfwdInfo->fwd_feature[i].fwd_option.notification_to_fp =
           pIfwdInfo->forwarding_feature[i].forwarding_options.notification_to_fp;

   }

  /*Get the timer value*/
    if(pIfwdInfo->forwarding_feature[i].no_reply_condition_timer.present)
    {
      pOfwdInfo->fwd_feature[i].timer_value =
          pIfwdInfo->forwarding_feature[i].no_reply_condition_timer.timer_value;
    }

  }
 return TRUE;
}/*OEMSUPPS_GetFwdFeatureListInfo*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetCliRestrictInfo

DESCRIPTION
  Converts the Cli Restrict Info into SuppSvc Cli Restrict Info

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
 
===========================================================================*/
static __inline
boolean OEMSUPPS_GetCliRestrictInfo
(
  const cli_restriction_info_T   *pICliRestrict,
  AEETSuppsCliRestrictInfo      *pOCliRestrict
)
{
  if(!pICliRestrict->present)
  {
    return FALSE;
  }

  MSG_HIGH("Received Cli Restrict Info from CM",0,0,0);

  (void)OEMSUPPS_GetSSStatus(&pICliRestrict->cm_ss_status,
                             &pOCliRestrict->ss_status);

  if(pICliRestrict->cm_cli_restriction_option.present)
  {
    pOCliRestrict->cli_restriction_option = (AEETSuppsCliRestrictOptionType)pICliRestrict->cm_cli_restriction_option.option;
  }
  return TRUE;
}/*OEMSUPPS_GetCliRestrictInfo*/


/*===========================================================================
FUNCTION  OEMSUPPS_GetUSSData

DESCRIPTION
  Converts the USS Data Info into SuppSvc USS Data Info

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline
boolean OEMSUPPS_GetUSSData
(
   const cm_uss_data_e_type   ussDataType,
   const uss_data_s_type     *pIUSSData,
   AEETSuppsData            *pOUSSData
)
{

  MSG_HIGH("Received USS Data from CM",0,0,0);

  /*Check the data coding scheme*/
  if( ( pIUSSData->uss_data_coding_scheme & OEMSUPPS_ALPHABET_MASK ) ==
                   OEMSUPPS_DEFAULT_ALPHABET )
  {
     if ( ussDataType == CM_USS_PHASE2_TYPE )
     {
        if (AEE_SUCCESS != OEMSupps_New_DecodedGSMAlphaBuf(
           (void**)&pOUSSData->data, &pOUSSData->length,
           pIUSSData->ussData, pIUSSData->size)) {

           return FALSE;
        }
      }
      else if ( ussDataType == CM_USS_PHASE1_TYPE )
      {
         uint16 dest_bytes = pIUSSData->size;

       //lint -esym(737, pOUSSData->length)  

     /*simply Store the data*/
         pOUSSData->data = (char*)MALLOC(dest_bytes+1);
         if ((char*)0 == pOUSSData->data) {
            return FALSE;
         }
         pOUSSData->length = dest_bytes;
         (void)MEMMOVE(pOUSSData->data, pIUSSData->ussData, dest_bytes);
      }
   }
   return TRUE;
}/*OEMSUPPS_GetUSSData*/
/*===========================================================================
FUNCTION  OEMSUPPS_GetBSGListInfo

DESCRIPTION
  Gets the Completed Event Data when the Completed event is received

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
boolean OEMSUPPS_GetBSGListInfo(const basic_service_group_list_s_type* pIBsgList,
  AEETSuppsBasicServiceGroupListInfo   *pOBsgList)
{
  int i;

  ZEROAT(pOBsgList);

  if (!pIBsgList->present)
  {
    return FALSE;
  }
  pOBsgList->length = pIBsgList->list_length;

  for(i=0;i<pOBsgList->length;i++)
  {
    if(pIBsgList->list[i].present)
    {
      pOBsgList->bsg_list[i] = OEMSUPPS_ConvertBSGToLocal(pIBsgList->list[i].bs_code \
        | (AEETSUPPS_BM(pIBsgList->list[i].code_type )));
    }
  }
  return TRUE;
} /*OEMSUPPS_GetBSGListInfo*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetCUGInfo

DESCRIPTION
  Gets the Completed Event Data when the Completed event is received

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
 boolean OEMSUPPS_GetCUGInfo
( 
  const ie_cug_info_s_type *pICUGInfo, 
  AEETSuppsData  *pOCUGInfo
)
{
  int i;
  char *pData;
  if(!pICUGInfo->present)
  {
    return FALSE;
  }

  pOCUGInfo->length = pICUGInfo->cug_feature_list_length * OEMSUPPS_CUG_SIZE;
  pOCUGInfo->data = (char*)MALLOC(pOCUGInfo->length);
  if ((char*)0 == pOCUGInfo->data) {
     return FALSE;
  }

  pData = pOCUGInfo->data;
  for(i=0;i<pICUGInfo->cug_feature_list_length;i++){
     MEMMOVE(pData, (char*)pICUGInfo->cug_subscription_list[i].cug_index, 
             OEMSUPPS_CUG_SIZE);
     pData += OEMSUPPS_CUG_SIZE;
  }

  return TRUE;
}/*OEMSUPPS_GetCUGInfo*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetSSData

DESCRIPTION
  Converts the CM SS Data into Supps SS Data

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  
===========================================================================*/
static __inline
boolean OEMSUPPS_GetSSData
(
  const ie_cm_ss_data_s_type *pIssStatusData,
  AEETSuppsDataInfo          *pOssStatusData
)
{
   if (pIssStatusData->present) {

      pOssStatusData->length = pIssStatusData->length;
      if (pIssStatusData->cm_ss_code.present) {
         pOssStatusData->ss_code = (AEETSuppsCodeType)pIssStatusData->cm_ss_code.ss_code;
      }

      (void)OEMSUPPS_GetSSStatus(&pIssStatusData->cm_ss_status, &pOssStatusData->ss_status);

      (void)OEMSUPPS_GetBSGListInfo((basic_service_group_list_s_type*)&pIssStatusData->basic_service_group_list,
                                    &pOssStatusData->basic_service_group_list);

      pOssStatusData->ss_subscription_option.option_type 
         = (AEETSuppsCliOptionType)pIssStatusData->ss_subscription_option.option_type;
      if (pIssStatusData->ss_subscription_option.option.clir_restriction_option.present) {
         pOssStatusData->ss_subscription_option.option.cli_restrict
            = (AEETSuppsCliRestrictOptionType)pIssStatusData->ss_subscription_option.option.clir_restriction_option.option;
      }

      return TRUE;
   }

   return FALSE;
}/*OEMSUPPS_GetSSData*/

/*===========================================================================
FUNCTION  OEMSUPPS_GetCompletedData

DESCRIPTION
  Gets the Completed Event Data when the Completed event is received

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS

===========================================================================*/
static __inline 
void OEMSUPPS_GetCompletedData(AEETSuppsStatus *pStatus, 
                               const cm_sups_info_s_type* sups_info)
{
  pStatus->status  = AEETSUPPS_STATUS_COMPLETE;

  pStatus->data.Result.result_type = sups_info->conf_type;
  pStatus->data.Result.success = sups_info->ss_success;
  

  MSG_HIGH("Received Conf: %d from CM",sups_info->conf_type,0,0);

  /*Based on the confirmation type ,convert the CM data into App Data*/
  switch(sups_info->conf_type)
  {
    case FWD_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_FWD_INFO;
      pStatus->data.Result.data.fwd_info.ss_code = (AEETSuppsCodeType)sups_info->ss_code; 
      if(!OEMSUPPS_GetFwdInfo(&sups_info->forwarding_info,
                             &pStatus->data.Result.data.fwd_info)) 
      {
         pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      } 
     break;


     case FWD_FEATURE_LIST_INFO:
       pStatus->data.Result.result_type = AEETSUPPS_FWD_FEATURE_LIST_INFO;
      if(!OEMSUPPS_GetFwdFeatureListInfo(&sups_info->fwd_feature_list,
                                           &pStatus->data.Result.data.fwd_feature_list))
      {
         pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      }
      break;

    case CALL_BARRING_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_CALL_BARRING_INFO;
      pStatus->data.Result.data.call_barring_info.ss_code = 
               (AEETSuppsCodeType)sups_info->ss_code;

      if( !OEMSUPPS_GetCallBarringInfo(&sups_info->call_barring_info,
                                      &pStatus->data.Result.data.call_barring_info))
      {
         pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      }
      break;

    case CLI_RESTRICTION_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_CLI_RESTRICTION_INFO;
      if(!OEMSUPPS_GetCliRestrictInfo(&sups_info->cli_restriction,
                                        &pStatus->data.Result.data.cli_restriction))
      {
          pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      }
      break;

    case CUG_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_CUG_INFO;
      (void)OEMSUPPS_GetCUGInfo(&sups_info->cug_info, 
                            &pStatus->data.Result.data.cug_info); 
      break;

    case BSG_LIST_INFO :
       pStatus->data.Result.result_type = AEETSUPPS_BSG_LIST_INFO;
      (void)OEMSUPPS_GetBSGListInfo(&sups_info->bsg_list, 
                                &pStatus->data.Result.data.bsg_list_info); 
      break;

    case USS_DATA_INFO:
      pStatus->data.Result.result_type =AEETSUPPS_USS_DATA_INFO;
      if(!OEMSUPPS_GetUSSData( sups_info->uss_data_type,
                                &sups_info->uss_data,
                                &pStatus->data.Result.data.uss_data_info))
      {
         pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      }
      break;

    case USSD_DATA_INFO:
       pStatus->data.Result.result_type = AEETSUPPS_USSD_DATA_INFO;
       if(!OEMSUPPS_GetUSSData( sups_info->uss_data_type,
                                  &sups_info->uss_data,
                                  &pStatus->data.Result.data.ussd_data_info))
        {
            pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
        }
        break;

      case SS_DATA_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_SS_DATA_INFO;
      if (!OEMSUPPS_GetSSData(&sups_info->ss_data, &pStatus->data.Result.data.ss_data)) {
         pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      }
      break;

    case SS_STATUS_INFO:
        pStatus->data.Result.result_type = AEETSUPPS_SS_STATUS_INFO;
       (void)OEMSUPPS_GetSSStatus(&sups_info->ss_status,
                              &pStatus->data.Result.data.ss_status);
     break;
    
   
    case SS_ERROR_INFO:
       pStatus->data.Result.result_type = AEETSUPPS_SS_ERROR_INFO;
       pStatus->data.Result.data.ss_error
          = OEMSUPPS_GetSSError(&sups_info->ss_error);
      break;

    case UNKNOWN_CONF_INFO:
       pStatus->data.Result.result_type = AEETSUPPS_UNKNOWN_CONF_INFO;
      break;
    
    case CC_CAUSE_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_CC_CAUSE_INFO;
      pStatus->data.Result.data.cc_cause =  (uint32)OEMSUPPS_GetCCCause(&sups_info->cc_cause);

     break;

    case NO_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_NO_INFO;
      MSG_HIGH("No Info Received",0,0,0);
      break;

    case NEW_PWD_INFO:
      pStatus->data.Result.result_type = AEETSUPPS_NEW_PASSWD_INFO;
      STRLCPY(pStatus->data.Result.data.ss_new_pwd,
              sups_info->ss_new_password.ss_password, MIN((MAX_PWD_CHAR+1),
                                                          AEETSUPPS_MAX_PASSWD_LEN));
      break;
    default:
      pStatus->data.Result.success = FALSE;
      MSG_HIGH("%d Conf Type Ignored",sups_info->conf_type,0,0);
      break; 
  }
}/*OEMSUPPS_GetCompletedData*/

/*=============================================================================
FUNCTION: OEMSUPPS_CMEventCB

DESCRIPTION:  
This function is called in the DMSS Context. It will save all the necessary 
information and switch to BREW context to process each of the CM events. This
CB only deals with the SUPS related events. It is this callback function that 
is registered with CM whenever ISuppsTrans is initialized.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMSUPPS_CMEventCB(cm_sups_event_e_type sups_event, 
                               const cm_sups_info_s_type* sups_info)
{
   OEMSuppsEvent  *pEvt;

  MSG_HIGH("Received Sups Event 0x%X from DMSS CM", sups_event, 0, 0);

   if (NULL == gpOEMSupps) {
      MSG_ERROR("Unexpected Supps Event Callback from CM", 0, 0, 0);
    return;
  }

  OEMSUPPS_MUTEX_LOCK();

   switch (sups_event) {
      /* Network Initiated Transaction */
   case CM_SUPS_EVENT_FWD_CHECK_IND:
   case CM_SUPS_EVENT_USS_NOTIFY_IND:
   case CM_SUPS_EVENT_USS_IND:
   case CM_SUPS_EVENT_GET_PASSWORD_IND:
   /*Get the event call back pointer from ISuppsTrans*/
      pEvt = OEMSuppsEvt_Alloc(gpOEMSupps);
      break;

      /* Mobile Initiated Responses */
   case CM_SUPS_EVENT_USS_NOTIFY_RES:
   case CM_SUPS_EVENT_USS_RES:
   case CM_SUPS_EVENT_GET_PASSWORD_RES: {
      AEECallback* pcb = gpOEMSupps->pcbList;
      OEMSuppsEvent* pEvtTemp;

      /* match sups_info->invoke_id */
      pEvt = 0;
      while (pcb) {
         pEvtTemp = OEMSuppsEvt_FromAEECallback(pcb);
         if (sups_info->invoke_id == pEvtTemp->pTrans->m_transId) {
            pEvt = pEvtTemp;
            break;
         }
         pcb = pcb->pNext;
      }

      break;
   }

      /* Mobile Initiated Requests */
   case CM_SUPS_EVENT_REGISTER_CONF:
   case CM_SUPS_EVENT_ACTIVATE_CONF:
   case CM_SUPS_EVENT_DEACTIVATE_CONF:
   case CM_SUPS_EVENT_ERASE_CONF:
   case CM_SUPS_EVENT_INTERROGATE_CONF:
   case CM_SUPS_EVENT_REG_PASSWORD_CONF:
   case CM_SUPS_EVENT_PROCESS_USS_CONF: {
      AEECallback* pcb = gpOEMSupps->pcbList;
      OEMSuppsEvent* pEvtTemp;

      /* match sups_info->ss_ref */
      pEvt = 0;
      while (pcb) {
         pEvtTemp = OEMSuppsEvt_FromAEECallback(pcb);
         if (sups_info->ss_ref == pEvtTemp->pTrans->m_transId) {
            pEvt = pEvtTemp;
            break;
         }
         pcb = pcb->pNext;
      }

      break;
   }

   default:
      MSG_ERROR("Unhandled Supps Event 0x%X", sups_event, 0, 0);
      pEvt = 0;  /* bail out */
      break;
   }

   /*Copy the event data*/
   if(0 != pEvt) {
      pEvt->event = sups_event;
      (void)MEMCPY(&pEvt->cm_sups_info, sups_info, sizeof(cm_sups_info_s_type));
   }

   OEMSUPPS_MUTEX_UNLOCK();

   if (0 != pEvt) {
      /* @@@ TODO : a potential crash if OEMSupps_Release() occurs now. 
      ** Gaurding with Supps_Mutex has a potential for deadlock. */
      OEMSuppsEvt_Dispatch(pEvt);
   }
   else {
      MSG_ERROR("Out of event buffer", 0, 0, 0);
   }

  return;
}

/*===========================================================================
FUNCTION  OEMSUPPS_CmdCb

DESCRIPTION
  Called by Call manager to indicate status of SUPS command

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  Add command in UI command buffer
===========================================================================*/
static void OEMSUPPS_CmdCb(void* data_block_ptr, cm_sups_cmd_e_type cmd,
                           cm_sups_cmd_err_e_type cmd_err)
{
   OEMSuppsTrans* pTrans = (OEMSuppsTrans*)data_block_ptr;

   if (NULL == pTrans || NULL == gpOEMSupps) {
      MSG_ERROR("Unexpected Supps CMD callback from CM", 0, 0, 0);
      return;
   }

  if (cmd_err != CM_SUPS_CMD_ERR_NOERR) {
      OEMSuppsEvent  *pEvt = &pTrans->pOEMTSupps->evt;

      MSG_HIGH("==> OEMSUPPS_CmdCb cmd: 0x%X, err: 0x%X", cmd, cmd_err, 0);

      if(0 == pEvt) {
         MSG_ERROR("==> OEMSUPPS_CmdCb No Callbacks Available", 0, 0, 0);
         return;
      }

      pEvt->event = CM_SUPS_EVENT_NONE;
      OEMSuppsEvt_Dispatch(pEvt);
   }
}
#endif /* FEATURE_WCDMA || F_GSM */

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/


static __inline
int OEMSupps_New(OEMSupps** ppOEMSupps)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)

   OEMSupps* pme = gpOEMSupps;
   int nErr;

   if (NULL != pme) {
      pme->uRefs++;
      *ppOEMSupps = pme;
      return SUCCESS;
   }

   pme = (OEMSupps*)sys_malloc(sizeof(OEMSupps));
   if (NULL == pme) {
      return ENOMEMORY;
   }

   nErr = OEMTelephone_Open(&pme->pOEMTelephone);
   if (AEE_SUCCESS != nErr) {
      sys_free(pme);
      return nErr;
   }

   (void)OEMTelephone_SuppsReg(pme->pOEMTelephone, OEMSUPPS_CMEventCB, 
                               NULL /* Ignore errors from other clients */, &pme->cm_id);
   pme->uRefs = 1;
   *ppOEMSupps = pme;
   gpOEMSupps = pme;
   return AEE_SUCCESS;
#else
   return EUNSUPPORTED;
#endif // defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
}

static __inline
uint32 OEMSupps_Release(OEMSupps* pme)
{
   uint32 dwRet = --pme->uRefs;

   if (0 == dwRet) {
      int i;

      if (gpOEMSupps == pme) {
         gpOEMSupps = NULL;
      }

      for (i = 0; i < OEMSUPPS_MAX_CB; i++) {
         if (TRUE == pme->acb[i].bInUse) {
            OEMSuppsEvt_Free(&pme->acb[i]);
         }
      }

      OEMTelephone_SuppsReg(pme->pOEMTelephone, NULL, NULL, NULL);

      OEMTelephone_Close(pme->pOEMTelephone);

      sys_free(pme);
   }

   return dwRet;
}

int OEMSuppsT_New(OEMSupps** ppOEMSupps){
   return OEMSupps_New(ppOEMSupps);
}

uint32 OEMSuppsT_Release(OEMSupps* pOEMSupps)
{
   return OEMSupps_Release(pOEMSupps);
}

int OEMTSupps_New(AEETSuppsTransId nTransId, OEMSuppsTrans* pTrans,
                  OEMTSupps** ppOEMTSupps)
{
   int nErr;
   OEMTSupps* pme = MALLOCREC(OEMTSupps);

   MSG_HIGH("==>OEMTSups_New", 0, 0, 0);

   if ((OEMTSupps*)0 == pme) {
        return ENOMEMORY;
     }

   nErr = OEMSupps_New(&pme->pSys);
   if (AEE_SUCCESS != nErr) {
      goto bail;
   }

   if (AEET_SUPPSVC_TRANSID_NEW == nTransId) {
      pTrans->m_transId = ++pme->pSys->bTransId;
      if (0 == pTrans->m_transId) {
         pTrans->m_transId = ++pme->pSys->bTransId;
      }
   }
   else {

      // Find the transaction and initialize evt
      if (niEvent && niEvent->cm_sups_info.invoke_id == nTransId) {

         /* copy sups_info */
         (void)MEMCPY(&pme->evt.cm_sups_info, &niEvent->cm_sups_info, 
                      sizeof(cm_sups_info_s_type));
         /* copy sups_event */
         pme->evt.event = niEvent->event;
         pTrans->m_transId = nTransId;
         MSG_HIGH("==>OEMTSups_New tid : %d", nTransId, 0, 0);
      }
      else {
         nErr = ENOSUCH;
         goto bail;
      }
   }

   pme->evt.pac = AEE_GetAppContext();
   pme->evt.pTrans = pTrans;
   pTrans->pOEMTSupps = pme;
   CALLBACK_Init(&pme->evt.cb, OEMSuppsEvt_HandleAI, &pme->evt);

   MSG_HIGH("==>OEMTSups_New tid : %d", pTrans->m_transId, 0, 0);

   *ppOEMTSupps = pme;

   return AEE_SUCCESS;
bail:
   if ((OEMSupps*)0 != pme->pSys) {
      OEMSupps_Release(pme->pSys);
   }
   FREE(pme);
   return nErr;
}

void OEMTSupps_Delete(OEMTSupps* pme)
{
   MSG_HIGH("==>OEMTSups_Delete tid : %d", pme->evt.pTrans->m_transId, 0, 0);

   /* Cancel the event if queued to dispatcher */
   CALLBACK_Cancel(&pme->evt.cb);
   OEMSupps_ReleaseStatus(&pme->evt.pTrans->m_result);
   OEMSupps_Release(pme->pSys);
   FREE(pme);
}

/*===========================================================================
FUNCTION OEMSUPPS_RegExCharsMatch 

DESCRIPTION
  This function is called to compare two characters against eachother,
  but handles the special characters associated with the near-regular
  expressions.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean OEMSUPPS_RegExCharsMatch(boolean special_char, char match_char, char src)
{
  if (special_char)     /* Do we treat this special */
  {
    switch (match_char)
    {
      case 'd': /* Currently, we only handle digits as special characters */
        return (src >= '0' && src <= '9');
      default:
        MSG_MED("Unknown special character %d\n", match_char, 0, 0);
        return FALSE;
    }
  }
  else
  {
    /* Normal characters are just compared. */
    return (src == match_char);
  }
}
/* <EJECT> */
/*===========================================================================
FUNCTION OEMSUPPS_RegexToMatch

DESCRIPTION
  This function is called to compare a token (group of characters)
  to the current portion of the regular expression.  This function
  will handle the cases when there are choices of characters or multiple
  characters as part of the token.  Upon completion, it moves the input
  pointers past the token in the input and regular expression strings.

  The formating understood includes:

  [] to indicate a choice of multiple characters that my match.
  \d indicates digit.
  * indicates 0 or more.
  + indicates 1 or more.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean OEMSUPPS_RegexToMatch( const char **src_ptr, const char **regex_ptr)
{
  boolean special_char = FALSE;  /* Are we processing a special character */
  boolean multi_char = FALSE;    /* Does this token have multiple characters */
  boolean choice_exists = FALSE; /* Is there a choice of characters */
  int min_num_chars_to_match;   /* Number of characters necessary (0 or 1) */
  char match_char;              /* The character we are trying to match. */
  const char *start_of_regex = *regex_ptr;  /* Save the start of the current regex */
  const char *end_reg_ptr;            /* Save the end of the current regex. */
  const char *tmp_reg_ptr;            /* Work variable. */
  int num_chars_matched = 0;    /* How long is the matching string. */

  end_reg_ptr = start_of_regex+1; /* End initially is the start + 1 */
  switch (*start_of_regex)
  {
    case '[':                   /* We are going to have a choice. */
      choice_exists = TRUE;
      start_of_regex++;         /* The characters to match start next */

      /* Find the end of the regular expression */
      while ((*end_reg_ptr != ']') && (*end_reg_ptr != '\0'))
      {
        end_reg_ptr++;
      }

      if (*end_reg_ptr != ']')
      {
        MSG_ERROR("Error in regular expression; no closing ]", 0, 0, 0);
        return FALSE;
      }

      end_reg_ptr++;    /* End points to either the start of the next or the
                           repeat character */
      break;
    case '\\':          /* Escaped character? */
      end_reg_ptr++;    /* The regular expression takes an extra character. */
      break;
    default:
      break;
  }

  /* Now we check the character afterwards to see if it specifies a count. */
  switch (*(end_reg_ptr))
  {
    case '*':   /* Specifies 0 or more */
      multi_char = TRUE;
      min_num_chars_to_match = 0;
      end_reg_ptr++;
      break;
    case '+':   /* Specifies 1 or more */
      multi_char = TRUE;
      min_num_chars_to_match = 1;
      end_reg_ptr++;
      break;
    default:    /* No repeat counts */
      multi_char = FALSE;
      min_num_chars_to_match = 1;
  }

  tmp_reg_ptr = start_of_regex;         /* Initialize work variable */
  do {
    match_char = *tmp_reg_ptr;          /* We are searching for this char */

    // Check for escaped characters.
    if (match_char == '\\')             /* Unless it is escaped. */
    {
      match_char = *++tmp_reg_ptr;      /* Get the char after the backslash */
      switch (match_char)
      {
        case 'd':                       /* D is the only special char we know */
          special_char = TRUE;
          break;
        default:
          special_char = FALSE;
          break;
      }
    }

    /* Now check to see if the match_char from the regular expression matches
     * the current character in the input string. */
    if (OEMSUPPS_RegExCharsMatch(special_char, match_char,
          *((*src_ptr) + num_chars_matched)))
    {
      /* We have a match, mark that we did. */
      num_chars_matched++;
      tmp_reg_ptr = start_of_regex;
    }
    else
    {
      /* It doesn't match.  Either we are done matching to this regular
       * expression or we have more choices. */
      if (choice_exists)
      {
        tmp_reg_ptr++;
        if (*tmp_reg_ptr == ']') /* No more choices, we are done. */
          break;
      }
      else
        break;
    }
  } while (multi_char || choice_exists);

  /* Now to see if we are actually matching what was specified. */
  if (num_chars_matched >= min_num_chars_to_match)
  {
    /* Yep, move things beyond this regular expression */
    *src_ptr += num_chars_matched;
    *regex_ptr = end_reg_ptr;
    return TRUE;
  }
  else
  {
    /* Nope.  */
    return FALSE;
  }
}

/*===========================================================================
FUNCTION OEMSUPPS_RegexTokenize

DESCRIPTION
  This function is called to compare a series of regular expressions
  to a single input_string.  If the strings match, we call the function
  associated with the regular expression.  This function should process
  the string and return the command to be executed.  The caller can then
  call the appropriate function to handle the request.

  The regular expression formating understood by this function is:
    () to indicate save this part in one of the tokens.
    NOTE: the tokens are not NULL terminated by this function.
    The input string is preserved unless the called function modifies them.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static int OEMSUPPS_RegexTokenize (const char *in_str, 
                                   OEMSUPPS_TOKEN_INPUT *inputs, 
                                   const char *tokens[], void *pMsg)
{
  int ntokens;          /* Number of tokens we have saved. */
  const char *current_in;     /* Current place in the input string. */
  const char *tmp_ptr;        /* Work variable when we may need to save it. */
  const char *current_regex;  /* Current place of the regular expression */
  OEMSUPPS_TOKEN_INPUT *curr_ptr = inputs; /* Current regular express */
  boolean found_match = FALSE; /* Used to loop across multiple reg_exps */
  boolean optional_match = FALSE; /* Marks this part of the regex as optional */
  boolean do_save = FALSE; /* Should we save this if it matches. */

  /* While we need to search and haven't found it. */
  while ((found_match == FALSE) && (curr_ptr->reg_exp != NULL))
  {
    /* New expression, init our variables. */
    current_in = in_str;                /* Start at beginning of source */
    current_regex = curr_ptr->reg_exp;  /* Start at beginning of regexp */
    found_match = TRUE;                 /* Assume we will match. */
    ntokens = 0;                        /* We haven't saved anything yet. */
    optional_match = FALSE;
    do_save = FALSE;

    /* While we are looking good. */
    while ((found_match) && (*current_regex != '\0'))
    {
      switch (*current_regex) /* Check if we handle the regular expression */
      {
        case '{':               /* this portion of the string is optional */
          optional_match = TRUE;
          current_regex++;
          break;
        case '}':               /* end the string is optional */
          optional_match = FALSE;
          current_regex++;
          break;
        case '(':               /* Save the current token. */
          do_save = TRUE;
          current_regex++;
          break;
        case ')':               /* Really a no-op since its up to the
                                   parsing function to mark the end. */
          current_regex++;
          break;
        default:
          /* We don't handle the character, so see if the token matches */
          tmp_ptr = current_in;
          found_match = OEMSUPPS_RegexToMatch(&current_in, &current_regex);
          if (found_match)
          {
            if (do_save)
            {
              tokens[ntokens++] = tmp_ptr;
            }
          }
          else if (optional_match)
          {
            // If we didn't find a match, it may mean we are in an
            // optional part of the expression.  Skip to the end of
            // the optional part now and resume comparing there.
            while ((*current_regex++ != '}') && (*current_regex != '\0'))
            {
              if (*current_regex == '\\') current_regex++;
              current_regex++;
            }
            found_match = TRUE;
          }
          do_save = FALSE;
          break;
      }
    }

    /* Done checking regular expression... if everything matches... */
    if (found_match && (*current_in == '\0'))
    {
      /* Everything matched, call the function associated with it. */
      return ((*(curr_ptr->check_fn))(ntokens, tokens, pMsg));
    }

    /* We don't have a match, check next regular expression. */
    found_match = FALSE;
    curr_ptr++;
  }

  /* Return an indication that we did not find a match. */
  return AEET_ERR_SUPPS_STRING_NO_MATCH;
}

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
/*===========================================================================
FUNCTION OEMSUPPS_ConvertBSGToNW

DESCRIPTION
  Converts the basic service from a local value to a network value
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
 None
===========================================================================*/
static int OEMSUPPS_ConvertBSGToNW( AEETSuppsMessage *pMsg)
{
   int n, nErr;

   /*Exception - Register Passwd does not have a basic service*/
   if(pMsg->data.request.operation == AEETSUPPS_REQ_REGISTER_PASSWD) {
      return SUCCESS;
   }

   if (AEETSUPPS_BASIC_SERVICE_NETWORK_FORMAT < (unsigned)pMsg->data.request.basic_service) {
      return SUCCESS;
   }

   nErr = EFAILED;

   for( n = 0; n < OEMSSUPPS_MAX_BSG_ENTRY; n++ ) {
      if( OEMSUPPSBSGConversionTable[n].local == pMsg->data.request.basic_service) {
         pMsg->data.request.basic_service  = OEMSUPPSBSGConversionTable[n].net_code;
         nErr = SUCCESS;
         break;
      }
   }

   return nErr;
}

/*===========================================================================
FUNCTION OEMSUPPS_ConvertBSGToLocal

DESCRIPTION
  Converts the basic service from a local value to a network value
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
 None
===========================================================================*/
static int OEMSUPPS_ConvertBSGToLocal( int basic_service )
{
   int n = 0;

   for( n = 0; n < OEMSSUPPS_MAX_BSG_ENTRY; n++ ) {

      if( OEMSUPPSBSGConversionTable[n].net_code == basic_service) {
         
         return(OEMSUPPSBSGConversionTable[n].local);
      }
   }
   
   return n;
}

/*===========================================================================
FUNCTION ISuppsTrans_NumberToBCD

DESCRIPTION
  This function is called to convert the number into a BCD format

DEPENDENCIES
  None.

RETURN VALUE
  Length of the BCD Number

SIDE EFFECTS
  Converts the number into a BCD format
===========================================================================*/
static int OEMSUPPS_NumberToBCD(const byte *src, uint32 uLen, byte *dest)
{
  cm_num_s_type number;
  uint8 bcd_len;
   uint8 bLen = (uint8)uLen;    /* @@@ TODO : Change this to accept uint32 */

   MSG_LOW("OEMSUPPS_NumberToBCD Called len %d",bLen,0,0);

   if(src == NULL || dest == NULL) {
    MSG_ERROR("Bad Pointer",0,0,0);
    return 0;
  }


  /*Default Values*/
  number.pi            = 0;
  number.si            = 0;
  number.digit_mode    = CM_DIGIT_MODE_8BIT_ASCII;
   number.number_type   = 0x80 | CM_NPI_UNKNOWN;
   number.number_plan   = CM_NPI_ISDN;

   if (src[0] == '+' ) {
    number.number_type = CM_NPI_ISDN;
      (void)MEMCPY( number.buf, (src + 1), bLen -1);
      number.len = bLen - 1 ;
   }
  else {
      (void)MEMCPY( number.buf, (src ), bLen );
      number.len = bLen  ;
   }

  /*Convert number to BCD*/
  cm_util_number_to_bcd( (const cm_num_s_type*)&number, dest);

   bcd_len  = 2 + (1 + number.len)/2;    // (LENGTH + TON + NPI + digits)

  MSG_HIGH("Returned length %d", bcd_len, 0, 0);
  return bcd_len;
}

/*===========================================================================
FUNCTION OEMSUPPS_BCDToNumber

DESCRIPTION
  This function is called to convert the BCD Number number into an ascii format

DEPENDENCIES
  None.

RETURN VALUE
  Length of the Ascii Number

SIDE EFFECTS
  Converts the BCD number into ascii format
===========================================================================*/
static uint32 OEMSUPPS_BCDToNumber
(
  const byte *pBcdNumber,
  uint32  uLength,
  char *pAddress
)
{
  byte *asciiNumber = (byte*)MALLOC(CM_CALLED_PARTY_BCD_NO_LENGTH);
  byte *pBuffer = (byte*)MALLOC(CM_CALLED_PARTY_BCD_NO_LENGTH);
  uint32 len =0;
  byte bcdLength = (byte)uLength;   /* TODO : make it work on uint32 and no MALLOCs*/

  MSG_LOW("OEMSUPPS_BCDToNumber Called",0,0,0);

  /*Check for invalid values*/
  if(pBcdNumber == NULL || pAddress == NULL || asciiNumber == NULL || pBuffer == NULL)
  {
    MSG_ERROR("Bad Pointer",0,0,0);
    FREEIF(asciiNumber);
    FREEIF(pBuffer);
    return 0;
  }

  (void)MEMSET (pBuffer, 0xFF, sizeof(pBuffer));
  pBuffer[0] = bcdLength;
  (void)MEMCPY( &pBuffer[1],
           pBcdNumber,
           bcdLength);

  /*Convert to ASCII values*/
  cm_util_bcd_to_ascii((byte *)pBuffer, asciiNumber );

  /*Save string to ouput param*/
  if( asciiNumber[0] != '\0' )
  {
    len = (byte)STRLEN((char *)asciiNumber);
    (void)STRCPY( pAddress, (char*)asciiNumber);
  }

  FREE(asciiNumber);
  FREE(pBuffer);
  return len;
}
#endif 

#if defined (FEATURE_MMGSDI) || defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
static size_t strnchrcpy(char *pszDest, const char *pszSrc, 
                         const char *pszTerm, size_t nSize)
{
   size_t nRet;

   if( !nSize ) {
      return nSize;
   }

   nRet = nSize;

   while( nSize > 1 && *pszSrc && !STRCHR( pszTerm, *pszSrc ) ) {
      *pszDest++ = *pszSrc++;
      nSize--;
   }

   *pszDest = '\0';        /* NULL terminate */
   nRet -= nSize;

   return nRet;
}
#endif 

/*===========================================================================
FUNCTION OEMSUPPS_ChkChangePin

DESCRIPTION
  This function is called to determine if the parameters entered are good
  for changing the PIN.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChkChangePin(int num_tokens, const char *tokens[], SuppsBuffer *pBuf)
{
#ifdef FEATURE_MMGSDI
   AEETSuppsMessage* pMsg;
   AEETSuppsChangePassword *pPass;
   int    nLen;

   /* The expression specifying this string has multiple formats. */
   /* However, only the complete format executes the command. */

   if( num_tokens != 4 ) {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   /* **04[2]*<old_pin>*<new_pin>*<new_pin># legal. */
   /* Token0     1          2         3*/

   pMsg = pBuf->pMsg;
   pPass = &pMsg->data.request.req.passwd;

   /* Copy the old PIN. */
   nLen = strnchrcpy( pPass->szCurPasswd, tokens[1], "*", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify old  PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   nLen = strnchrcpy( pPass->szNewPasswd, tokens[2], "*", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify new PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   nLen = strnchrcpy( pPass->szNewPasswdRepeated, tokens[3], "#", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify new PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   pMsg->type = AEETSUPPS_MSG_REQUEST;
   if (*((char *)tokens[0]) == '2') { 
      pMsg->data.request.operation = AEETSUPPS_REQ_CHANGE_PIN2;
   }
   else {
      pMsg->data.request.operation = AEETSUPPS_REQ_CHANGE_PIN;
   }

   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION OEMSUPPS_PinCmdCb

DESCRIPTION
  The callback Function for GSDI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static __inline
AEETSuppsPinStatus OEMSupps_PinStatusFrom_gsdi(gsdi_returns_T status)
{
   AEETSuppsPinStatus pinStatus;

   switch (status) {
   case GSDI_SUCCESS:
      pinStatus = AEETSUPPS_PIN_SUCCESS;
      break;
   case GSDI_INCOMPAT_PIN_STATUS:
      pinStatus =  AEETSUPPS_PIN_ERR_INCOMPATIBLE;
      break;
   case GSDI_CODE_BLOCKED:
      pinStatus = AEETSUPPS_PIN_ERR_BLOCKED;
      break;
   case GSDI_INCORRECT_CODE:
      pinStatus = AEETSUPPS_PIN_ERR_INCORRECT;
      break;
   default:
      pinStatus = AEETSUPPS_PIN_ERR_UNKNOWN;
   }

   return pinStatus;
}

static void OEMSUPPS_PinDone(OEMSuppsTrans* pTrans, AEETSuppsPinStatus pinStatus)
{
  /* Send the response to application */
   pTrans->m_transState = AEETSUPPS_STATE_DONE;

   pTrans->m_result.status = AEETSUPPS_STATUS_COMPLETE;
   pTrans->m_result.data.Result.success
      = (AEETSUPPS_PIN_SUCCESS == pinStatus)?TRUE:FALSE;
   pTrans->m_result.data.Result.result_type = AEETSUPPS_PIN_INFO;
   pTrans->m_result.data.Result.data.pin_status = pinStatus;
   OEMSupps_MessageDone(pTrans);
}

static void OEMSUPPS_PinCmdCb(gsdi_cnf_T *retval)
{
   AEETSuppsPinStatus pinStatus;
   OEMSuppsTrans *pTrans = (OEMSuppsTrans*)retval->change_pin_cnf.message_header.client_ref;

   pinStatus = OEMSupps_PinStatusFrom_gsdi(retval->change_pin_cnf.message_header.gsdi_status);
   OEMSUPPS_PinDone(pTrans, pinStatus);
}

#ifdef FEATURE_MMGSDI_DUAL_SLOT
static __inline 
gsdi_slot_id_type gsdi_GetActiveSlot(void)
{
   gsdi_slot_id_type slot_id;
   gsdi_card_apps_T card_apps;

   gsdi_get_apps_available(&card_apps);
 
   if (card_apps.slot1_present 
       && (card_apps.apps_enabled & GSDI_GSM_SIM_MASK 
           ||card_apps.apps_enabled & GSDI_USIM_APP_MASK)
       && (card_apps.operational_slot & GSDI_GSM_SLOT_1_MASK 
           || card_apps.operational_slot & GSDI_USIM_SLOT_1_MASK )) {
      slot_id = GSDI_SLOT_1;
   }
   else if (card_apps.slot2_present 
            && (card_apps.apps2_enabled & GSDI_GSM_SIM_MASK 
                || card_apps.apps2_enabled & GSDI_USIM_APP_MASK)
            && (card_apps.operational_slot & GSDI_GSM_SLOT_2_MASK 
                || card_apps.operational_slot & GSDI_USIM_SLOT_2_MASK)) {
      slot_id = GSDI_SLOT_2;
   }
   else {
      slot_id = GSDI_SLOT_NONE;
   }

   return slot_id;
}
#endif

static __inline
boolean gsdi_IsCardActive(void)
{
   boolean bRet;
   gsdi_card_apps_T card_apps;

   gsdi_get_apps_available(&card_apps);

   if(card_apps.slot1_present && (card_apps.apps_enabled & GSDI_GSM_SIM_MASK ||
                                  card_apps.apps_enabled & GSDI_USIM_APP_MASK)) {
      bRet = TRUE;
      MSG_MED("Slot 1 is active",0,0,0);
   }
   else {
      bRet = FALSE;
   }

   return bRet;
}

static __inline
AEETSuppsPinStatus gsdi_ChangePin(gsdi_pin_ids_T pin_id, char* old_pin, 
                              char* new_pin, gsdi_async_cb cb, uint32 ctxt)
{
   gsdi_returns_T gsdi_retval;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
   gsdi_slot_id_type slot_id = gsdi_GetActiveSlot();

   gsdi_retval = (slot_id == GSDI_SLOT_NONE) ? GSDI_ERROR 
                 : gsdi2_change_pin (slot_id, pin_id, old_pin, new_pin,
                                     NULL, ctxt, cb);
#else
   byte num_retry_left;  /* Variable so GSDI can say whats left. */

   gsdi_retval = gsdi_IsCardActive() ? gsdi_change_pin(pin_id, old_pin, new_pin,
                                                       (byte *)&num_retry_left, 
                                                       ctxt, cb)
                                     : GSDI_ERROR;
#endif

   return OEMSupps_PinStatusFrom_gsdi(gsdi_retval);
}
#endif /*FEATURE_MMGSDI*/

/*===========================================================================
FUNCTION OEMSUPPS_ChangePin

DESCRIPTION
  This function is called when the parameters entered are good and
  we need to attempt to change the PIN.


DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChangePin(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{
#ifdef FEATURE_MMGSDI
   AEETSuppsPinStatus pinStatus;
   gsdi_pin_ids_T pin_id;

  if (MEMCMP(pMsg->data.request.req.passwd.szNewPasswd, 
             pMsg->data.request.req.passwd.szNewPasswdRepeated, 
             AEETSUPPS_MAX_PASSWD_LEN)) {
     /* Indicate asynchronous done */
      OEMSUPPS_PinDone(pTrans, AEETSUPPS_PIN_ERR_INCOMPATIBLE);
     return AEETSUPPS_PIN_ERR_INCOMPATIBLE;
  }

  if (AEETSUPPS_REQ_CHANGE_PIN2 == pMsg->data.request.operation) {
     pin_id = GSDI_PIN2;
  }
   else {
      pin_id = GSDI_PIN1;
   }

   pTrans->m_transState = AEETSUPPS_STATE_RUNNING;

   pinStatus = gsdi_ChangePin(pin_id, (char*)pMsg->data.request.req.passwd.szCurPasswd,
                                     (char *) pMsg->data.request.req.passwd.szNewPasswd, 
                              OEMSUPPS_PinCmdCb, (uint32)pTrans);
   if (AEETSUPPS_PIN_SUCCESS != pinStatus){
      OEMSUPPS_PinDone(pTrans, pinStatus);
   }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
}
#endif

/*===========================================================================
FUNCTION OEMSUPPS_ChkUnblockPin

DESCRIPTION
  This function is called to determine if the parameters entered are good
  for unblocking the PIN.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChkUnblockPin(int num_tokens, const char *tokens[], SuppsBuffer *pBuf)
{
#ifdef FEATURE_MMGSDI
   AEETSuppsMessage *pMsg;
   AEETSuppsChangePassword *pPass;
   int    nLen;

   /* The expression specifying this string has multiple formats. */
   /* However, only the complete format executes the command. */

   if( num_tokens != 4 ) {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   /* **05[2]*<old_pin>*<new_pin>*<new_pin># legal. */
   /* Token0     1          2         3*/

   pMsg = pBuf->pMsg;
   pPass = &pMsg->data.request.req.passwd;

   /* Copy the old PIN. */
   nLen = strnchrcpy( pPass->szCurPasswd, tokens[1], "*", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify old  PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   nLen = strnchrcpy( pPass->szNewPasswd, tokens[2], "*", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify new PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   nLen = strnchrcpy( pPass->szNewPasswdRepeated, tokens[3], "#", AEETSUPPS_MAX_PASSWD_LEN );
   /* Verify new PIN */
   if( !INRANGE( nLen, 4, GSDI_PIN_LEN ) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   pMsg->type = AEETSUPPS_MSG_REQUEST;
   if (*((char *)tokens[0]) == '2') {
      pMsg->data.request.operation = AEETSUPPS_REQ_UNBLOCK_PIN2; 
   }
   else {
      pMsg->data.request.operation = AEETSUPPS_REQ_UNBLOCK_PIN; 
   }

   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)

#ifdef FEATURE_MMGSDI
static AEETSuppsPinStatus gsdi_UnblockPin(gsdi_pin_ids_T pin_id, char* old_pin, 
                                      char* new_pin, gsdi_async_cb cb, uint32 ctxt)
{
   gsdi_returns_T gsdi_retval;

   #ifdef FEATURE_MMGSDI_DUAL_SLOT
   gsdi_slot_id_type slot_id = gsdi_GetActiveSlot();

   gsdi_retval = (slot_id == GSDI_SLOT_NONE) ? GSDI_ERROR
      : gsdi2_unblock_pin(slot_id, pin_id, old_pin, 
                          new_pin, NULL, ctxt, cb);
   #else
   byte num_retry_left; /* Variable so GSDI can say whats left. */

   gsdi_retval = gsdi_IsCardActive() ? gsdi_unblock_pin(pin_id, old_pin, new_pin, 
                                                        (byte *)&num_retry_left, 
                                                        ctxt, cb)
   : GSDI_ERROR;
   #endif
   return OEMSupps_PinStatusFrom_gsdi(gsdi_retval);
}
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION OEMSUPPS_UnblockPin

DESCRIPTION
  This function is called when the parameters entered are good and
  we need to attempt to unblock the PIN.

  The input parameters are as follows:
  tokens[0] = PIN_ID
  tokens[1] = puk
  tokens[2] = new_pin
  tokens[3] = verify_new_pin

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_UnblockPin(OEMSuppsTrans* pTrans, AEETSuppsMessage* pMsg)
{
#ifdef FEATURE_MMGSDI
   AEETSuppsPinStatus pinStatus;
   gsdi_pin_ids_T     pin_id;

  if (MEMCMP(pMsg->data.request.req.passwd.szNewPasswd, 
             pMsg->data.request.req.passwd.szNewPasswdRepeated, 
             AEETSUPPS_MAX_PASSWD_LEN)) {
      /* Indicate asynchronous done */
      OEMSUPPS_PinDone(pTrans, AEETSUPPS_PIN_ERR_INCOMPATIBLE);
    return AEETSUPPS_PIN_ERR_INCOMPATIBLE;
  }
      
  if (AEETSUPPS_REQ_UNBLOCK_PIN2 == pMsg->data.request.operation) {
     pin_id = GSDI_PIN2;
  }
   else {
      pin_id = GSDI_PIN1;
   }

   pTrans->m_transState = AEETSUPPS_STATE_RUNNING;

   pinStatus = gsdi_UnblockPin(pin_id, (char*)pMsg->data.request.req.passwd.szCurPasswd,
                                     (char *) pMsg->data.request.req.passwd.szNewPasswd, 
                               OEMSUPPS_PinCmdCb, (uint32)pTrans);

   if (AEETSUPPS_PIN_SUCCESS != pinStatus) {
     /* Indicate asynchronous done */
      OEMSUPPS_PinDone(pTrans, pinStatus);
   }

  return SUCCESS;
#else
  return EUNSUPPORTED;
#endif
}
#endif

/*===========================================================================
FUNCTION OEMSUPPS_ChkIMEI

DESCRIPTION
  This function is called to verify the parameters of the IMEI display.
  Since there are no parameters, this function simply returns the command
  number for IMEI.

DEPENDENCIES
  None.

RETURN VALUE
  Command number.

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChkIMEI(int num_tokens, const char *tokens[], SuppsBuffer *pBuf)
{

   if( num_tokens != 0 ) {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   pBuf->pMsg->type = AEETSUPPS_MSG_REQUEST;
   pBuf->pMsg->data.request.operation = AEETSUPPS_REQ_DEVICE_IMEI;
   return SUCCESS;
}

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
/*===========================================================================
FUNCTION OEMSUPPS_NetworkIndReq

DESCRIPTION
  This function is called to get the Network Indication.

DEPENDENCIES
  None.

RETURN VALUE
  Command number.

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_NetworkIndReq(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{
   OEMSuppsEvent* pEvt = &pTrans->pOEMTSupps->evt;

   OEMSuppsEvt_Dispatch(pEvt);
   return SUCCESS;
}

/*===========================================================================
FUNCTION OEMSUPPS_IMEI

DESCRIPTION
  This function is called to get the IMEI to be displayed.

DEPENDENCIES
  None.

RETURN VALUE
  Command number.

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_IMEI(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{
  int len;
   int nErr;
   IShell* piShell = AEE_GetShell();
   AEECLSID clsApp;

  MSG_LOW("OEMSUPPS_IMEI Called",0,0,0);

   clsApp = AEE_GetAppContextCls(pTrans->pOEMTSupps->evt.pac);

   nErr = ISHELL_AppIsInGroup(piShell, clsApp, AEECLSID_HWIDS);
   if (AEE_SUCCESS != nErr) {
      nErr = ISHELL_AppIsInGroup(piShell, clsApp, AEECLSID_GSMCREDENTIALS);
   }

   if (AEE_SUCCESS != nErr) {
      OEMSupps_MessageErr(pTrans, AEETSUPPS_ERR_INSUFFICIENT_PRIVILEGE);
      goto bail;
   }

   len = sizeof(pTrans->m_result.data.Result.data.imei);
   nErr = OEM_GetDeviceInfoEx(AEE_DEVICEITEM_IMEI, 
                              pTrans->m_result.data.Result.data.imei,
                              &len);
   if (AEE_SUCCESS != nErr) {
      OEMSupps_MessageErr(pTrans, AEETSUPPS_ERR_UNRECOGNISED_ERROR);
      goto bail;
   }

   pTrans->m_transState = AEETSUPPS_STATE_DONE;
   pTrans->m_result.status = AEETSUPPS_STATUS_COMPLETE;
   pTrans->m_result.data.Result.result_type = AEETSUPPS_DEVICE_IMEI;
   pTrans->m_result.data.Result.success = TRUE;

 bail:
   OEMSupps_MessageDone(pTrans);
  return SUCCESS;
}
#endif

boolean OEMSUPPS_IsDigit(char c)
{
  return (c >= '0' && c <= '9');
}



/*===========================================================================
FUNCTION OEMSUPPS_ChkSS

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Command number.

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChkSS(int num_tokens, const char *tokens[], SuppsBuffer *pBuf)
{
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
   char szWorkBuf[AEET_MAX_DIGITS_LENGTH];
   int i;
   const char *ss_str;
   OEMSUPPS_OPERATION         *current_ss_operation = OEMSUPPS_operation_input;
   uint32 str_len;
   boolean ss_match = FALSE;
   int ss_operation;
   char bsg[OEMSUPPS_MAX_SIA_LEN + 1];
   char sia[OEMSUPPS_MAX_SIA_LEN + 1];
   char sib[OEMSUPPS_MAX_SIB_LEN + 1];
   char sic[OEMSUPPS_MAX_SIC_LEN + 1];
   char ss_code[OEMSUPPS_MAX_SC_LEN + 1];
   AEETSuppsMessage*    sups_start = (AEETSuppsMessage*) pBuf->pMsg;
   int nErr = AEE_SUCCESS;
   int nToken = 0;

   if(num_tokens <= 0) {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   if( 0 == STRNCMP(tokens[0], "*31#", 4) || 0 == STRNCMP(tokens[0], "#31#", 4) ) {
      return AEET_ERR_SUPPS_STRING_BAD_PARAM;
   }

   ss_str = tokens[0];
   str_len = (uint8)STRLEN(ss_str);

   strnchrcpy( szWorkBuf, tokens[nToken++], "0123456789", AEET_MAX_DIGITS_LENGTH );

   /* STEP 1: Find the SS operation */
   ss_operation = AEETSUPPS_REQ_REGISTER;
   while(current_ss_operation->sups_operation != NULL) {
      if( 0 == STRCMP( szWorkBuf, current_ss_operation->sups_operation) ) {
         ss_operation = current_ss_operation->op_code;
         break;
      }
      current_ss_operation++;
   }

   if( num_tokens > nToken ) {
      strnchrcpy( szWorkBuf, tokens[nToken], "#*", AEET_MAX_DIGITS_LENGTH );
   }
   else {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   if( 0 == STRCMP( szWorkBuf, OEMSUPPS_LOCAL_BSG_REG_PASSWD ) ) {
      ss_operation = AEETSUPPS_REQ_REGISTER_PASSWD;
      nToken++;
   }
   else if( NULL == current_ss_operation->sups_operation ) {
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   sups_start->type = AEETSUPPS_MSG_REQUEST;
   sups_start->data.request.operation = ss_operation;

   if ( ss_operation <= AEETSUPPS_REQ_REGISTER_PASSWD ) {
      /* By now the operation must have been identified */
      /* STEP 2: Find the SI fields */
      if( num_tokens > nToken ) {
         strnchrcpy( ss_code, tokens[nToken++], "#*", sizeof(ss_code) );
      }
      else {
         ss_code[0] = '\0';
      }

      if( num_tokens > nToken ) {
         strnchrcpy( sia, tokens[nToken++], "#*", sizeof(sia) );
      }
      else {
         sia[0] = '\0';
      }

      if( num_tokens > nToken ) {
         strnchrcpy( sib, tokens[nToken++], "#*", sizeof(sib) );
      }
      else {
         sib[0] = '\0';
      }

      if( num_tokens > nToken ) {
         strnchrcpy( sic, tokens[nToken++], "#*", sizeof(sic) );
      }
      else {
         sic[0] = '\0';
      }

      /* by now SIA, SIB, SIC fields must have been separated out */
      /* STEP 3: Convert SC to network SC and update the
                sups_user_data field */
      ss_match = FALSE;
      for( i = 0; i < OEMSUPPS_MAX_ENTRY; i++ ){
         if( 0 == STRNCMP( OEMSUPPSSCConversionTable[i].local_sc, ss_code, OEMSUPPS_MAX_SC_LEN ) ) {
            sups_start->data.request.code = OEMSUPPSSCConversionTable[i].net_sc;
            ss_match = TRUE;
            break;
         }
      }

      /* by now the translation of SC must be complete */
      /* STEP 4: Check for BSG . This may be in SIA, SIB or SIC */
      if (ss_match) {
         if( (sups_start->data.request.operation != AEETSUPPS_REQ_REGISTER_PASSWD)
             && (AEETSUPPS_IS_ALL_CF(sups_start->data.request.code) || AEETSUPPS_IS_RSTRCT(sups_start->data.request.code)) ) {
            /* BSG is in SIB */
            (void)STRCPY( bsg, sib );
         }
         else if( sups_start->data.request.code == AEETSUPPS_CODE_CW ) {
            /* BSG is in  SIA */
            (void)STRCPY( bsg, sia );
         }
         else {
            bsg[0] = '\0';
         }

         /* by now BSG must be identified */
         /* STEP 5: Convert BSG to net BSG */
         sups_start->data.request.basic_service = AEETSUPPS_BASIC_SERVICE_ALL;

         /* special case */
         if ( sups_start->data.request.operation != AEETSUPPS_REQ_REGISTER_PASSWD) {
            /*  BSG - Basic Service Group :
                     BS  = Bearer Service
                     TS  = Tele Service
            */
            ss_match = FALSE;
            for( i = 0; i < OEMSSUPPS_MAX_STR_BSG_ENTRY; i++ ) {
               if( 0 == STRNCMP( OEMSUPPSStrBSGConversionTable[i].local_bsg, bsg, OEMSUPPS_MAX_BSG_LEN ) ) {
                  sups_start->data.request.basic_service  = OEMSUPPSStrBSGConversionTable[i].net_bsg_code;
                  ss_match = TRUE;
                  break;
               }
            }
         }
      }

      /* STEP 6:  Convert DN, if any */
      if ( ss_match) {
         if( ( sups_start->data.request.code & 0xF0 ) == AEETSUPPS_CODE_ALL_FORWARDING_SS ) {
            if(sups_start->data.request.operation == AEETSUPPS_REQ_REGISTER && sia[0] == '\0') {
               return AEET_ERR_SUPPS_STRING_BAD_PARAM;
            }

            if( 0 != (sups_start->data.request.req.reg.data.length = STRLEN(sia)+1) ) {

               if( sups_start->data.request.req.reg.data.length > pBuf->dwExtra ) {
                  /* Insufficient memory */
                  pBuf->dwNeeded = sups_start->data.request.req.reg.data.length - pBuf->dwExtra;
                  nErr = EBUFFERTOOSMALL;
               }
               else {
                  /* There is sufficient memory at the end - allocate */
                  sups_start->data.request.req.reg.data.data = (char*)pBuf->pEnd;
                  pBuf->pEnd += sups_start->data.request.req.reg.data.length;
                  pBuf->dwExtra -= sups_start->data.request.req.reg.data.length;

                  (void)STRLCPY(sups_start->data.request.req.reg.data.data, sia,
                                sups_start->data.request.req.reg.data.length);
               }
            }
         }
      }

      /* STEP 7: Check is activation is also registration? */
      if( sups_start->data.request.operation == AEETSUPPS_REQ_ACTIVATE ) {
         if( sups_start->data.request.req.reg.data.length > 0 )
            sups_start->data.request.operation = AEETSUPPS_REQ_REGISTER;
         else if( sups_start->data.request.req.reg.nr_timer != 0 )
            sups_start->data.request.operation = AEETSUPPS_REQ_REGISTER;
      }

      /* STEP 8: Set the NR timer */
      if( (sups_start->data.request.operation == AEETSUPPS_REQ_REGISTER ) && AEETSUPPS_IS_CF(sups_start->data.request.code)) {

         if( sic[0] != '\0' )
            sups_start->data.request.req.reg.nr_timer = (uint8)ATOI(sic);
         else
            sups_start->data.request.req.reg.nr_timer = 0;
      }

      /* STEP 9: copy the password */
      if ( ss_match) {
         if( AEETSUPPS_IS_RSTRCT(sups_start->data.request.code)
             && sups_start->data.request.operation != AEETSUPPS_REQ_REGISTER_PASSWD) {

            (void)STRLCPY(sups_start->data.request.req.passwd.szCurPasswd, sia,
                          AEETSUPPS_MAX_PASSWD_LEN);
         }
      }

      /* copy old_pwd, new_pwd, new_pwd_again */
      if( sups_start->data.request.operation == AEETSUPPS_REQ_REGISTER_PASSWD) {
         if((sia[0] == '\0' || sib[0] == '\0' || sic[0] == '\0' )) {
            ss_match = FALSE;
         }
         else {
            /*OLD Passwd*/
            (void)STRLCPY((char*)sups_start->data.request.req.passwd.szCurPasswd, (char*)sia,
                          AEETSUPPS_MAX_PASSWD_LEN);

            /*New Passwd*/
            (void)STRLCPY((char*)sups_start->data.request.req.passwd.szNewPasswd, (char*)sib,
                          AEETSUPPS_MAX_PASSWD_LEN);

            /*New Passwd Again*/
            (void)STRLCPY((char*)sups_start->data.request.req.passwd.szNewPasswdRepeated,(char*)sic,
                         AEETSUPPS_MAX_PASSWD_LEN);
         }
      }
   }

   if ( !ss_match ) { /* check if it is an Unstructured SS data */
      if( ( str_len <= AEETSUPPS_MAX_DATA_LEN ) && ( ss_str[str_len - 1] == '#' ) ) {

         sups_start->data.request.operation = AEETSUPPS_REQ_USS;
         ss_match = TRUE;
         sups_start->data.request.req.ussd.length = str_len+1;
         if( str_len > pBuf->dwExtra ) {
            pBuf->dwNeeded = (str_len+1) - pBuf->dwExtra;
            nErr = EBUFFERTOOSMALL;
         }
         else {
            sups_start->data.request.req.ussd.data = (char*)pBuf->pEnd;
            pBuf->pEnd += sups_start->data.request.req.ussd.length;
            pBuf->dwExtra -= sups_start->data.request.req.ussd.length;
            (void)STRLCPY(sups_start->data.request.req.ussd.data, ss_str,
                          sups_start->data.request.req.ussd.length);
         }
      }
   }

   if(ss_match && ss_str[str_len - 1] == '#') {
      MSG_LOW("Valid SS String",0,0,0);
      return nErr;
   }
#endif /* FEATURE_WCDMA || FEATURE_GSM */

   return AEET_ERR_SUPPS_STRING_NO_MATCH;
}

static int IsThereAVoiceCall(void)
{
   ICall    *pCall;
   ACONTEXT *pac;
   int      nErr;
   ICallMgr *pCallMgr;

   pac = AEE_EnterAppContext(0);

   nErr = AEE_CreateInstance(AEECLSID_CALLMGR, (void**)&pCallMgr);
   if (SUCCESS == nErr) {
      nErr = ICALLMGR_GetActiveVoiceCall(pCallMgr, &pCall);

      if( SUCCESS == nErr ) {
         ICALL_Release(pCall);
      }
      ICALLMGR_Release(pCallMgr);
   }

   AEE_LeaveAppContext(pac);

   return nErr;
}

/*===========================================================================
FUNCTION OEMSUPPS_ChkUSSD

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Command number.

SIDE EFFECTS
  Plays with the display
===========================================================================*/
static int OEMSUPPS_ChkUSSD(int num_tokens, const char *tokens[], SuppsBuffer *pBuf)
{
   uint8 str_len;
   boolean bValid = FALSE;
   const char *ss_str;
   AEETSuppsMessage *pMsg = pBuf->pMsg;

   if(num_tokens == 0){
      return AEET_ERR_SUPPS_STRING_NO_MATCH;
   }

   str_len = (uint8)STRLEN(tokens[0]);
   ss_str  = tokens[0];

   /* ALL 1 digit and 2 digit numbers [EXCEPT CALLINDEPENDENT SUPS]are USSD if there is a call up
   ** If not in a call then all 2 digit numbers not starting  with 1 are USSDs*/

   if( str_len > 0  && str_len <= 2 ) {
      if( SUCCESS == IsThereAVoiceCall() ||
          ( str_len == 2 && ss_str[0] != '1' ) ){
         bValid = TRUE;
      }
   }

   /*Fill the structure if required*/ 
   if( bValid ) {

      pMsg->type = AEETSUPPS_MSG_REQUEST;
      pMsg->data.request.operation = AEETSUPPS_REQ_USS;
      pMsg->data.request.req.ussd.length = str_len+1;

      if(pMsg->data.request.req.ussd.length > pBuf->dwExtra) {
         pBuf->dwNeeded = (str_len+1) - pBuf->dwExtra;
         return EBUFFERTOOSMALL;
      }

      pMsg->data.request.req.ussd.data = (char *)(pBuf->pEnd);
      pBuf->pEnd += pMsg->data.request.req.ussd.length;
      pBuf->dwExtra -= pMsg->data.request.req.ussd.length;
      (void)STRLCPY(pMsg->data.request.req.ussd.data, tokens[0],
                   pMsg->data.request.req.ussd.length);

      return SUCCESS;
   }

   return AEET_ERR_SUPPS_STRING_NO_MATCH;
}


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
static __inline 
void OEMSupps_Enqueue_Transaction(OEMSuppsTrans* pTrans)
{
   OEMSupps* pme = pTrans->pOEMTSupps->pSys;
   OEMSuppsEvent* pEvt = &pTrans->pOEMTSupps->evt;

   OEMSUPPS_MUTEX_LOCK();

   CALLBACK_Cancel(&pEvt->cb);
   pEvt->cb.pfnCancel = OEMSuppsEvt_Cancel;
   pEvt->cb.pCancelData = pme;
   pEvt->cb.pNext = pme->pcbList;
   pme->pcbList = &pEvt->cb;
   pTrans->m_transState = AEETSUPPS_STATE_RUNNING;

   OEMSUPPS_MUTEX_UNLOCK();
}

static __inline 
boolean OEMSupps_cm_sups_cmd_register(OEMSuppsTrans* pTrans,
                                      cm_reg_ss_params_s_type* reg_cmd_params)
{

   OEMSupps_Enqueue_Transaction(pTrans);

   return cm_sups_cmd_register(OEMSUPPS_CmdCb, pTrans, 
                               pTrans->pOEMTSupps->pSys->cm_id, 
                               reg_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_erase(OEMSuppsTrans* pTrans,
                                   cm_erase_ss_params_s_type *erase_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_erase(OEMSUPPS_CmdCb, pTrans,
                            pTrans->pOEMTSupps->pSys->cm_id,
                            erase_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_activate(OEMSuppsTrans* pTrans,
                                      cm_act_ss_params_s_type *act_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_activate(OEMSUPPS_CmdCb, pTrans,
                               pTrans->pOEMTSupps->pSys->cm_id,
                               act_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_deactivate(OEMSuppsTrans* pTrans,
                                        cm_deact_ss_params_s_type* deact_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_deactivate(OEMSUPPS_CmdCb, pTrans,
                                 pTrans->pOEMTSupps->pSys->cm_id,
                                 deact_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_interrogate(OEMSuppsTrans* pTrans, 
                                         cm_interr_ss_params_s_type* interr_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_interrogate(OEMSUPPS_CmdCb, pTrans,
                                 pTrans->pOEMTSupps->pSys->cm_id,
                                 interr_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_reg_password(OEMSuppsTrans* pTrans,
                                          cm_reg_pwd_params_s_type* reg_pwd_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_reg_password(OEMSUPPS_CmdCb, pTrans,
                                   pTrans->pOEMTSupps->pSys->cm_id,
                                   reg_pwd_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_process_uss(OEMSuppsTrans* pTrans,
                                          cm_uss_params_s_type* process_uss_cmd_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_process_uss(OEMSUPPS_CmdCb, pTrans,
                                   pTrans->pOEMTSupps->pSys->cm_id,
                                   process_uss_cmd_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_get_password_res(OEMSuppsTrans* pTrans,
                                          cm_get_pwd_res_params_s_type* get_pwd_res_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_get_password_res(OEMSUPPS_CmdCb, pTrans,
                                   pTrans->pOEMTSupps->pSys->cm_id,
                                   get_pwd_res_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_uss_res(OEMSuppsTrans* pTrans,
                                          cm_uss_res_params_s_type* uss_res_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_uss_res(OEMSUPPS_CmdCb, pTrans,
                                   pTrans->pOEMTSupps->pSys->cm_id,
                                   uss_res_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_uss_notify_res(OEMSuppsTrans* pTrans,
                                          cm_uss_notify_res_params_s_type* uss_notify_res_params)
{
   OEMSupps_Enqueue_Transaction(pTrans);
   return cm_sups_cmd_uss_notify_res(OEMSUPPS_CmdCb, pTrans,
                                   pTrans->pOEMTSupps->pSys->cm_id,
                                   uss_notify_res_params);
}

static __inline
boolean OEMSupps_cm_sups_cmd_release(OEMSuppsTrans* pTrans,
                                     cm_release_ss_params_s_type* release_cmd_params)
{
   return cm_sups_cmd_release(NULL, NULL,
                              pTrans->pOEMTSupps->pSys->cm_id,
                              release_cmd_params);
}

/*===========================================================================
FUNCTION OEMSUPPS_RegisterSS

DESCRIPTION
    Send a RegisterSS request

DEPENDENCIES
    ISUPSSVC and CM interface must be initialized

SIDE EFFECTS
  None
===========================================================================*/
static int OEMSUPPS_RegisterSS(OEMSuppsTrans*pTrans, AEETSuppsMessage *pMsg)
{
  cm_reg_ss_params_s_type       *reg_cmd_parms 
     = (cm_reg_ss_params_s_type*)MALLOC(sizeof(cm_reg_ss_params_s_type));

  MSG_HIGH("OEMSUPPS_RegisterSS Called",0,0,0);

  if(reg_cmd_parms == NULL) {
    MSG_ERROR("No Resources in ISuppsTransSVC",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the register SS request */

  /* ss_ref */
  reg_cmd_parms->ss_ref   = (uint8)pTrans->m_transId;

  /* service code */
  reg_cmd_parms->ss_code  = (uint8)pMsg->data.request.code;

  /* basic service group information */
  reg_cmd_parms->basic_service.bs_code = pMsg->data.request.basic_service & 0xff;
  reg_cmd_parms->basic_service.code_type = pMsg->data.request.basic_service >> 16; /*BS_CODE or TS_CODE*/
  reg_cmd_parms->basic_service.present = \
    (reg_cmd_parms->basic_service.code_type == NO_CODE )? FALSE:TRUE;

  /* forwarded to number*/
  if(pMsg->data.request.req.reg.data.length != 0)
  {
    char *address = (char*)MALLOC(pMsg->data.request.req.reg.data.length + 1);

    (void)STRCPY(address, pMsg->data.request.req.reg.data.data);

    (void)OEMSUPPS_NumberToBCD((byte*)address, pMsg->data.request.req.reg.data.length, (byte*)address);

    reg_cmd_parms->fwd_to_num.length = address[0];
    (void)MEMCPY (reg_cmd_parms->fwd_to_num.data,
            &address[1],
            address[0]);

    reg_cmd_parms->fwd_to_num.present = TRUE;
    FREE(address);
  }
  else
    reg_cmd_parms->fwd_to_num.present = FALSE;

  /* forwarded to subaddress */
  reg_cmd_parms->fwd_to_subaddr.present = FALSE;

  /* no reply condition timer */
  if(pMsg->data.request.req.reg.nr_timer != 0 )
  {
    reg_cmd_parms->nr_timer.present     = TRUE;
    reg_cmd_parms->nr_timer.timer_value = (uint8)pMsg->data.request.req.reg.nr_timer;
  }
  else
    reg_cmd_parms->nr_timer.present = FALSE;

  /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_register(pTrans, reg_cmd_parms) == FALSE)
  {
    /*Either reg_cmd_params is NULL or no more resources in cm*/
    FREE(reg_cmd_parms);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  FREE(reg_cmd_parms);
  return SUCCESS;
}
/*===========================================================================

FUNCTION OEMSUPPSSVC_ERASESS

DESCRIPTION
  Send an EraseSS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_EraseSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{
  cm_erase_ss_params_s_type    *erase_cmd_parms 
     = (cm_erase_ss_params_s_type*)MALLOC(sizeof(cm_erase_ss_params_s_type));

  MSG_HIGH("OEMSUPPS_EraseSS Called",0,0,0);

  if(erase_cmd_parms == NULL)
  {
    MSG_ERROR("No Resources in ISuppsTransSVC",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the Erase SS request */

  erase_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* Service code */
  erase_cmd_parms->ss_code = (uint8)pMsg->data.request.code;

  /* basic service group information */
  erase_cmd_parms->basic_service.bs_code = pMsg->data.request.basic_service & 0xff;
  erase_cmd_parms->basic_service.code_type = pMsg->data.request.basic_service >> 16; /*BS_CODE or TS_CODE*/
  erase_cmd_parms->basic_service.present = \
    (erase_cmd_parms->basic_service.code_type == NO_CODE )? FALSE:TRUE;

  /* send the command to the CM command queue */
  if(OEMSupps_cm_sups_cmd_erase(pTrans, erase_cmd_parms) == FALSE)
  {
    /*no more resources in cm*/
    FREE(erase_cmd_parms);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }
  FREE(erase_cmd_parms);

  return SUCCESS;

}

/*===========================================================================

FUNCTION OEMSUPPS_ActivateSS

DESCRIPTION
  Send an ActivateSS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_ActivateSS(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg )
{
  cm_act_ss_params_s_type     *act_cmd_parms 
     = (cm_act_ss_params_s_type*)MALLOC(sizeof(cm_act_ss_params_s_type));

  MSG_HIGH("OEMSUPPS_ActivateSS Called",0,0,0);

  if(act_cmd_parms == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the Activate SS request */
  act_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* ss code */
  
  act_cmd_parms->ss_code = (uint8)pMsg->data.request.code;

   /* basic service group information */
  act_cmd_parms->basic_service.bs_code = pMsg->data.request.basic_service & 0xff;
  act_cmd_parms->basic_service.code_type = pMsg->data.request.basic_service >> 16; /*BS_CODE or TS_CODE*/
  act_cmd_parms->basic_service.present = \
    (act_cmd_parms->basic_service.code_type == NO_CODE )? FALSE:TRUE;

  /* send the command to the CM command queue */
   if (OEMSupps_cm_sups_cmd_activate(pTrans, act_cmd_parms) == FALSE)
   {
     /*no more resources in cm*/
     FREE(act_cmd_parms);
     MSG_ERROR("No Resources in CM",0,0,0);
     OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
     return EFAILED;
   }
   FREE(act_cmd_parms);
   return SUCCESS;
}
/*===========================================================================

FUNCTION OEMSUPPS_DeactivateSS

DESCRIPTION
  Send a DeactivateSS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_DeactivateSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{
  cm_deact_ss_params_s_type     *deact_cmd_parms  
     = (cm_deact_ss_params_s_type*)MALLOC(sizeof(cm_deact_ss_params_s_type));

  MSG_HIGH("OEMSUPPS_DeactivateSS Called",0,0,0);

  if(deact_cmd_parms == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the Deactivate SS request */
  deact_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* ss code */
  deact_cmd_parms->ss_code = (uint8)pMsg->data.request.code;

   /* basic service group information */
  deact_cmd_parms->basic_service.bs_code = pMsg->data.request.basic_service & 0xff;
  deact_cmd_parms->basic_service.code_type = pMsg->data.request.basic_service >> 16; /*BS_CODE or TS_CODE*/
  deact_cmd_parms->basic_service.present = \
    (deact_cmd_parms->basic_service.code_type == NO_CODE )? FALSE:TRUE;

  /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_deactivate(pTrans, deact_cmd_parms) == FALSE) {
    FREE(deact_cmd_parms);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }
  FREE(deact_cmd_parms);
  return SUCCESS;
}
/*===========================================================================

FUNCTION OEMSUPPS_InterrogateSS

DESCRIPTION
  Send an InterrogateSS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_InterrogateSS (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{

  cm_interr_ss_params_s_type       *interr_cmd_parms 
     = (cm_interr_ss_params_s_type*)MALLOC(sizeof(cm_interr_ss_params_s_type));

  MSG_HIGH("OEMSUPPS_InterrogateSS Called",0,0,0);

  if(interr_cmd_parms == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the Interrogate SS request */
  interr_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* ss code */
  interr_cmd_parms->ss_code = (uint8)pMsg->data.request.code;

   /* basic service group information */
  interr_cmd_parms->basic_service.bs_code = pMsg->data.request.basic_service & 0xff;
  interr_cmd_parms->basic_service.code_type = pMsg->data.request.basic_service >> 16; /*BS_CODE or TS_CODE*/
  interr_cmd_parms->basic_service.present = \
    (interr_cmd_parms->basic_service.code_type == NO_CODE )? FALSE:TRUE;


  /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_interrogate(pTrans, interr_cmd_parms) == FALSE) {
    FREE(interr_cmd_parms);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }
  FREE(interr_cmd_parms);
  return SUCCESS;

}
/*===========================================================================

FUNCTION OEMSUPPS_RegPwd

DESCRIPTION
  Send a RegisterPassword SS request

DEPENDENCIES


SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_RegPwd(OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg )
{
  cm_reg_pwd_params_s_type       *reg_pwd_cmd_parms 
     = (cm_reg_pwd_params_s_type*)MALLOC(sizeof(cm_reg_pwd_params_s_type));

  MSG_HIGH("OEMSUPPS_RegPwd Called",0,0,0);

  if(reg_pwd_cmd_parms == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the request */
  reg_pwd_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* SS code */
  reg_pwd_cmd_parms->ss_code.present = TRUE;

  reg_pwd_cmd_parms->ss_code.ss_code = (uint8)pMsg->data.request.code;

  /* Old Password */
  if( pMsg->data.request.req.passwd.szCurPasswd[0] != '\0')
  {
    STRCPY(reg_pwd_cmd_parms->ss_old_pwd.ss_password, pMsg->data.request.req.passwd.szCurPasswd);
    reg_pwd_cmd_parms->ss_old_pwd.present = TRUE;
  }
  else
    reg_pwd_cmd_parms->ss_old_pwd.present = FALSE;


  /* New Password */
  if( pMsg->data.request.req.passwd.szNewPasswd[0] != '\0')
  {
    (void)STRCPY(reg_pwd_cmd_parms->ss_new_pwd.ss_password, pMsg->data.request.req.passwd.szNewPasswd);
        
    reg_pwd_cmd_parms->ss_new_pwd.present = TRUE;

  }
  else
    reg_pwd_cmd_parms->ss_new_pwd.present = FALSE;

  /* New Password Again*/
  if( pMsg->data.request.req.passwd.szNewPasswdRepeated[0] != '\0')
  {
    (void)STRCPY(reg_pwd_cmd_parms->ss_new_pwd_again.ss_password, pMsg->data.request.req.passwd.szNewPasswdRepeated);
    
    reg_pwd_cmd_parms->ss_new_pwd_again.present = TRUE;
  }
  else
    reg_pwd_cmd_parms->ss_new_pwd_again.present = FALSE;

  
  /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_reg_password(pTrans, reg_pwd_cmd_parms) == FALSE) {
    FREE(reg_pwd_cmd_parms);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }
  FREE(reg_pwd_cmd_parms);
  return SUCCESS;
}

/*===========================================================================

FUNCTION OEMSUPPS_USSReq

DESCRIPTION
  Send a ProcessUnstructured SS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_USSReq (OEMSuppsTrans* pTrans, AEETSuppsMessage *pMsg)
{

  cm_uss_params_s_type    *uss_cmd_parms;
  byte                    num_bytes;
  int                     num_bits = 0;

  MSG_HIGH("OEMSUPPS_USSReq Called",0,0,0);

  uss_cmd_parms = (cm_uss_params_s_type*)MALLOC(sizeof(cm_uss_params_s_type));
  if(uss_cmd_parms == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }



  /* set all the fields of the Process USS request */
  uss_cmd_parms->ss_ref = (uint8)pTrans->m_transId;

  /* USS Data */

  uss_cmd_parms->uss_data.present = TRUE;

  uss_cmd_parms->uss_data.uss_data_coding_scheme = OEMSUPPS_USS_DEF_ALPHABET_LANG_UNSPEC;

  (void)STRCPY((char*)uss_cmd_parms->uss_data.ussData,
               pMsg->data.request.req.ussd.data);

  num_bits = (int)( STRLEN( (char *)(uss_cmd_parms->uss_data.ussData) ) * 7 ) ;

  num_bytes = (byte)num_bits / 8;

  if( (  num_bits % 8 ) != 0 )
    num_bytes ++;

  uss_cmd_parms->uss_data.size = num_bytes;

  /* send the command to the CM command queue */
  if( OEMSupps_cm_sups_cmd_process_uss(pTrans, uss_cmd_parms) == FALSE) {
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
  }
  FREE(uss_cmd_parms);
  return SUCCESS;
}

/*===========================================================================

FUNCTION OEMSUPPS_Abort

DESCRIPTION
  Send a ReleaseSS request

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_Abort(OEMSuppsTrans* pTrans, AEETSuppsMessage* pMsg)
{
  cm_release_ss_params_s_type  *release_cmd_parms = (cm_release_ss_params_s_type*)MALLOC(sizeof(cm_release_ss_params_s_type));

   MSG_HIGH("OEMSUPPS_Abort Called",0,0,0);

   if(NULL == release_cmd_parms) {
    /*No resources*/
      MSG_ERROR("Out of memory",0,0,0);
      return ENOMEMORY;
   }

   /* get the ss operation to be released */
   if (AEETSUPPS_MSG_REQUEST == pMsg->type ) {
      const OEMSUPPSTableType Xlate_Req[]= {
         {AEETSUPPS_REQ_REGISTER,          registerSS},
         {AEETSUPPS_REQ_ERASE,             eraseSS},
         {AEETSUPPS_REQ_ACTIVATE,          activateSS},
         {AEETSUPPS_REQ_DEACTIVATE,        deactivateSS},
         {AEETSUPPS_REQ_INTERROGATE,       interrogateSS},
         {AEETSUPPS_REQ_REGISTER_PASSWD,   registerPassword},
         {AEETSUPPS_REQ_USS,               processUnstructuredSS_Data}, };

         release_cmd_parms->ss_oper = OEMSUPPSTable_XlateLocal(Xlate_Req, ARR_SIZE(Xlate_Req),
                                                               pMsg->data.request.operation);
   }
   else {
      static const OEMSUPPSTableType Xlate_Ind[]= {
         {AEETSUPPS_IND_ENTER_PASSWD,      getPassword},
         {AEETSUPPS_IND_FWD_CHECK,         forwardCheckSS_Indication},
         {AEETSUPPS_IND_USS,               processUnstructuredSS_Request},
         {AEETSUPPS_IND_USS_NOTIFY,        processUnstructuredSS_Request}
      };

      release_cmd_parms->ss_oper = OEMSUPPSTable_XlateLocal(Xlate_Ind, ARR_SIZE(Xlate_Ind), 
                                                            pTrans->m_result.data.NetworkInd.ind_type);
   }

   release_cmd_parms->ss_ref = (uint8)pTrans->m_transId;
  /* send the command to the CM */
   if (0 != release_cmd_parms->ss_oper) {
      (void)OEMSupps_cm_sups_cmd_release(pTrans, release_cmd_parms);
   }

  FREE(release_cmd_parms);
  return SUCCESS;
}
/*===========================================================================

FUNCTION OEMSUPPS_PwdRes

DESCRIPTION
  Send a Get Password Response

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_PwdRes(OEMSuppsTrans* pTrans, AEETSuppsResponse* ss_resp)
{
  cm_get_pwd_res_params_s_type       *get_pwd_res 
     = (cm_get_pwd_res_params_s_type*)MALLOC(sizeof(cm_get_pwd_res_params_s_type));

  MSG_HIGH("OEMSUPPS_PwdRes Called",0,0,0);

  if(get_pwd_res == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* Invoke ID */
  get_pwd_res->invoke_id = (uint8)pTrans->m_transId;

  /* Password */
  (void)STRCPY(get_pwd_res->ss_pwd.ss_password, ss_resp->response_data.data);
  get_pwd_res->ss_pwd.present = TRUE;

  /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_get_password_res(pTrans, get_pwd_res) == FALSE)
  {
    FREE(get_pwd_res);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }
  FREE(get_pwd_res);
  return SUCCESS;
}

/*===========================================================================

FUNCTION OEMSUPPS_USSRes

DESCRIPTION
  Send an Unstructured SS response

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_USSRes(OEMSuppsTrans* pTrans, AEETSuppsResponse* ss_resp)
{
  cm_uss_res_params_s_type        *uss_res =(cm_uss_res_params_s_type*) MALLOC(sizeof(cm_uss_res_params_s_type));
  byte num_bytes;

  MSG_HIGH("OEMSUPPS_USSRes Called",0,0,0);
  
  if(uss_res == NULL) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /* set all the fields of the USS Response*/
  if( ss_resp->response_type == AEETSUPPS_RES_USS_OK )
  {
    /* uss data */
    uss_res->uss_res_data.data_type = CM_USS_RES_SS_DATA;

    uss_res->uss_res_data.data.uss_data.present = TRUE;
    uss_res->uss_res_data.data.uss_data.uss_data_coding_scheme =
                                              OEMSUPPS_USS_DEF_ALPHABET_LANG_UNSPEC;

    /*lint -esym(713, ss_resp->length) */

    /* convert the response from ascii to def alphabet */
    num_bytes = cm_util_ascii_to_gsm_alphabet( (byte *)uss_res->uss_res_data.data.uss_data.ussData,
                                               (byte *)ss_resp->response_data.data,
                                               (byte)ss_resp->response_data.length );

    /* size */
    uss_res->uss_res_data.data.uss_data.size = num_bytes;
  }
  else
  {
    /* ss_error */

    uss_res->uss_res_data.data_type = CM_USS_RES_SS_ERROR;

    uss_res->uss_res_data.data.cm_ss_error.present = TRUE;

    uss_res->uss_res_data.data.cm_ss_error.error_code_tag = ERROR_CODE_TAG;

    if( ss_resp->response_type == AEETSUPPS_RES_USS_UNKNOWN_ALPHABET )
    {
      uss_res->uss_res_data.data.cm_ss_error.error_code = unknownAlphabet;
    }
    else if ( ss_resp->response_type == AEETSUPPS_RES_USS_BUSY_DISP )
    {
      uss_res->uss_res_data.data.cm_ss_error.error_code = ussd_Busy;
    }
    uss_res->uss_res_data.data.cm_ss_error.parameter.cm_called_barred_error.present = FALSE;

  }

  /* invocation id */
  uss_res->invoke_id = (uint8)pTrans->m_transId;

 /* send the command to the CM command queue */
  if (OEMSupps_cm_sups_cmd_uss_res(pTrans, uss_res) == FALSE) {
    FREE(uss_res);
    MSG_ERROR("No Resources in CM",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  FREE(uss_res);
  return SUCCESS;
}
/*===========================================================================

FUNCTION OEMSUPPS_USSNotifyRes

DESCRIPTION
  Send an Unstructured SS Notify response

DEPENDENCIES

SIDE EFFECTS
  None.

===========================================================================*/
static int OEMSUPPS_USSNotifyRes(OEMSuppsTrans* pTrans, AEETSuppsResponse* ss_resp)
{
  cm_uss_notify_res_params_s_type        *uss_notify_res 
     = (cm_uss_notify_res_params_s_type*)MALLOC(sizeof(cm_uss_notify_res_params_s_type));

  MSG_HIGH("OEMSUPPS_USSNotifyRES Called",0,0,0);

  if(!uss_notify_res) {
    /*No resources*/
    MSG_ERROR("No Resources in ISuppsTrans",0,0,0);
    OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
    return EFAILED;
  }

  /*Send Appropriate Error Messages is response from Client is not OK*/
  if( ss_resp->response_type == AEETSUPPS_RES_USS_UNKNOWN_ALPHABET )
  {
    uss_notify_res->ss_error.present = TRUE;

    uss_notify_res->ss_error.error_code = unknownAlphabet;

    uss_notify_res->ss_error.error_code_tag = ERROR_CODE_TAG;
  }
  else if (ss_resp->response_type == AEETSUPPS_RES_USS_BUSY_DISP )
  {

    uss_notify_res->ss_error.present = TRUE;

    uss_notify_res->ss_error.error_code = ussd_Busy;

    uss_notify_res->ss_error.error_code_tag = ERROR_CODE_TAG;
  }

  /*Send OK Response*/
  else if ( ss_resp->response_type == AEETSUPPS_RES_USS_OK )
  {
   uss_notify_res->ss_error.present = FALSE;

  }
  
  /* invoke-id */
  uss_notify_res->invoke_id = (uint8)pTrans->m_transId;

  if (OEMSupps_cm_sups_cmd_uss_notify_res(pTrans, uss_notify_res) == FALSE) {
     FREE(uss_notify_res);
     OEMSupps_CmdErr(pTrans); /* Indicate asynchronous done */
     return EFAILED;
  }
  FREE(uss_notify_res);
  return SUCCESS;
}
#endif

boolean OEMTSuppsError_IsDebugMsgOn()
{
   return gOEMTSuppsDebug.bErrorDebugMsg;
}

void OEMTSuppsError_EnableDebugMsg(boolean bOn)
{
   gOEMTSuppsDebug.bErrorDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMTSuppsHigh_IsDebugMsgOn()
{
   return gOEMTSuppsDebug.bHighDebugMsg;
}

void OEMTSuppsHigh_EnableDebugMsg(boolean bOn)
{
   gOEMTSuppsDebug.bHighDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMTSuppsMed_IsDebugMsgOn()
{
   return gOEMTSuppsDebug.bMedDebugMsg;
}

void OEMTSuppsMed_EnableDebugMsg(boolean bOn)
{
   gOEMTSuppsDebug.bMedDebugMsg = (bOn ? TRUE : FALSE);
}

boolean OEMTSuppsLow_IsDebugMsgOn()
{
   return gOEMTSuppsDebug.bLowDebugMsg;
}

void OEMTSuppsLow_EnableDebugMsg(boolean bOn)
{
   gOEMTSuppsDebug.bLowDebugMsg = (bOn ? TRUE : FALSE);
}
