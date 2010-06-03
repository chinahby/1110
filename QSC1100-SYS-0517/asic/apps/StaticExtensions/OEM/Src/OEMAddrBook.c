/*======================================================
FILE:  OEMAddrBook.c

SERVICES:  OEM functions for supporting BREW Address Book Interface

GENERAL DESCRIPTION:
   This file provides the prorotypes of the functions that OEMs must provide in
   order to support the BREW AddressBook interfaces

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMAddrBook.c#4 $
=====================================================*/


//lint -emacro(774,FREEIF) doesn't matter if the if is always true here

#include "OEMFeatures.h"
#include "OEMHeap.h"
#include "OEMDB.h"
#include "OEMAddrBook.h"
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEAddrBook.h"
#include "OEMClassIDs.h"
#include "AEEStdLib.h"
#include "AEEAppletCtl.h"
#include "AEEClassIDs.h"
#include "uiutils.h"
#include "AEEAddrBookUtil.h"

/*====================================================================
SERVICES:  OEM functions for supporting BREW Address Book Interface
           using PBM (Phone Book Manager)

GENERAL DESCRIPTION:
   This file provides the prototypes of the functions that 
   OEMs must provide in order to support the BREW AddressBook 
   interfaces

=======================================================================*/

/* Support for PBM interface */

#include "AEEAddrBookExt.h"
#include "pbmlib.h"
#include "pbm_conversion.h"
#include "err.h"
#include "AppComFunc.h"

//Global Context for AddressBook.
OBJECT(COEMAddrBook)
{
   // Standard stuff
   AEEVTBL(IOEMAddrBook) *pvt;  //lint -esym(754,COEMAddrBook::pvt)

   // Number of references to this interface.
   uint32            m_nRefs;
};


//Struct to hold the callback information from PBM.
typedef struct cb_data_struct
{
  pbm_return_type ret;
  uint16          rec_id;
} OEMADDR_CB_DATA_STRUCT;

//Struct to hold supported category list items
typedef struct _SupportedCat
{
   AEEAddrCat cat;
   char *     pszName;
} SupportedCat;

//Struct to hold supported field list items
typedef struct _SupportedField
{
   AEEAddrFieldID   fieldID;
   char *           pszName;
} SupportedField;

//Struct to hold supported field list items
typedef struct _FieldTypeConversion
{
   pbm_field_type_e_type   pbm_type;
   AEEAddrFieldType        aee_type;
} FieldTypeConversion;


/* Number of address categories supported by this address book */
#ifdef FEATURE_MMGSDI
#ifdef FEATURE_MMGSDI_DUAL_SLOT
#define OEM_ADDRESS_CATEGORY_COUNT  16 // Business, Personal, Speed Dial, 
                                       // ADN/FDN, ADN2/FDN2, SDN[2], MSISDN[2] ,
                                       // MBDN[2] MBN[2]
#else
#define OEM_ADDRESS_CATEGORY_COUNT  10 // Business, Personal, Speed Dial, ADN/FDN, 
                                       // SDN, MSISDN, MBDN, MBN
#endif
#else
#define OEM_ADDRESS_CATEGORY_COUNT  4 // Business, Personal, Speed Dial
#endif

//List of supported categories
static const SupportedCat gSupportedCatList[OEM_ADDRESS_CATEGORY_COUNT] =
{
   { (int)AEE_ADDR_CAT_PERSONAL, "Personal" },
   { (int)AEE_ADDR_CAT_BUSINESS, "Business" },
   { (int)AEE_ADDR_CAT_SPEEDDIAL,"Speed Dial" },
#ifdef FEATURE_MMGSDI
   { (int)AEE_ADDR_CAT_SIM_ADN,    "SIM ADN" },
   { (int)AEE_ADDR_CAT_SIM_FDN,    "SIM FDN" },
   { (int)AEE_ADDR_CAT_SIM_SDN,    "SIM SDN" },
   { (int)AEE_ADDR_CAT_SIM_MSISDN, "SIM MSISDN" },
   { (int)AEE_ADDR_CAT_SIM_MBDN,   "SIM MBDN" },
   { (int)AEE_ADDR_CAT_SIM_MBN,    "SIM MBN" },
#ifdef FEATURE_MMGSDI_DUAL_SLOT
   { (int)AEE_ADDR_CAT_SIM_ADN2,   "SIM 2 ADN" },
   { (int)AEE_ADDR_CAT_SIM_FDN2,   "SIM 2 FDN" },
   { (int)AEE_ADDR_CAT_SIM_SDN2,   "SIM 2 SDN" },
   { (int)AEE_ADDR_CAT_SIM_MSISDN2,"SIM 2 MSISDN" },
   { (int)AEE_ADDR_CAT_SIM_MBDN2,  "SIM 2 MBDN" },
   { (int)AEE_ADDR_CAT_SIM_MBN2,   "SIM 2 MBN" },
#endif
#endif
   { (int)AEE_ADDR_CAT_ECC,"Emergency Numbers" }
};

//List of supported fields
static const SupportedField gSupportedFieldList[] =
{
  //{AEE_ADDRFIELD_NONE,                  "NONE"},
  {AEE_ADDRFIELD_FIRSTNAME,             "FIRSTNAME"},
  {AEE_ADDRFIELD_MIDDLENAME,            "MIDDLENAME"},
  {AEE_ADDRFIELD_LASTNAME,              "LASTNAME"},
  {AEE_ADDRFIELD_NICKNAME,              "NICKNAME"},
  {AEE_ADDRFIELD_NAME,                  "NAME"},
  {AEE_ADDRFIELD_ORG,                   "ORG"},
  {AEE_ADDRFIELD_JOBTITLE,              "JOBTITLE"},
  {AEE_ADDRFIELD_ADDRESS,               "ADDRESS"},
  {AEE_ADDRFIELD_CITY,                  "CITY"},
  {AEE_ADDRFIELD_STATE,                 "STATE"},
  {AEE_ADDRFIELD_COUNTRY,               "COUNTRY"},
  {AEE_ADDRFIELD_ZIPCODE,               "ZIPCODE"},
  {AEE_ADDRFIELD_URL,                   "URL"},
  {AEE_ADDRFIELD_DEPARTMENT,            "DEPARTMENT"},
  {AEE_ADDRFIELD_EMAIL,                 "EMAIL"},
  {AEE_ADDRFIELD_GENDER,                "GENDER"},
  {AEE_ADDRFIELD_PHONE_WORK,            "PHONE_WORK"},
  {AEE_ADDRFIELD_PHONE_HOME,            "PHONE_HOME"},
  {AEE_ADDRFIELD_PHONE_FAX,             "PHONE_FAX"},
  {AEE_ADDRFIELD_PHONE_VOICE,           "PHONE_VOICE"},
  {AEE_ADDRFIELD_PHONE_PREF,            "PHONE_PREF"},
  {AEE_ADDRFIELD_PHONE_CELL,            "PHONE_CELL"},
  {AEE_ADDRFIELD_PHONE_PAGER,           "PHONE_PAGER"},
  {AEE_ADDRFIELD_PHONE_GENERIC,         "PHONE_GENERIC"},
  {AEE_ADDRFIELD_PHONE_OTHER,           "PHONE_OTHER"},
  {AEE_ADDRFIELD_BDAY,                  "BDAY"},
  {AEE_ADDRFIELD_SORTSTRING,            "SORTSTRING"},
  {AEE_ADDRFIELD_NOTES,                 "NOTES"},
  {AEE_ADDRFIELD_SIP_URL,               "SIP_URL"},
  {AEE_ADDRFIELD_DIAL_STRING,           "DIAL_STRING"},
  {AEE_ADDRFIELD_INDEX,                 "INDEX"},
  {AEE_ADDRFIELD_VOICETAG,              "VOICETAG"},
  {AEE_ADDRFIELD_RINGTONE,              "RINGTONE"},
  {AEE_ADDRFIELD_SPEED_DIAL,            "SPEED_DIAL"},
  {AEE_ADDRFIELD_NAMESUFFIX,            "NAMESUFFIX"},
  {AEE_ADDRFIELD_NAMEPREFIX,            "NAMEPREFIX"},
  {AEE_ADDRFIELD_OTHERNAME,             "OTHERNAME"},
  {AEE_ADDRFIELD_PHOTO,                 "PHOTO"},
  {AEE_ADDRFIELD_PHOTOURL,              "PHOTOURL"},
  {AEE_ADDRFIELD_POBOX,                 "POBOX"},
  {AEE_ADDRFIELD_ADDREXTRA,             "ADDREXTRA"},
  {AEE_ADDRFIELD_STREET,                "STREET"},
  {AEE_ADDRFIELD_UID,                   "UID"},
  {AEE_ADDRFIELD_PUBLICKEY,             "PUBLICKEY"},
  {AEE_ADDRFIELD_PUBLICKEYSTRING,       "PUBLICKEYSTRING"},
  {AEE_ADDRFIELD_NUMBER_TYPE,           "NUMBER_TYPE"},
  {AEE_ADDRFIELD_NUMBER_PLAN,           "NUMBER_PLAN"},
  {AEE_ADDRFIELD_DATE,                  "DATE"},
  {AEE_ADDRFIELD_DURATION,              "DURATION"},
  {AEE_ADDRFIELD_CALL_TYPE,             "CALL_TYPE"},
  {AEE_ADDRFIELD_GSM_TONNPI,            "GSM_TONNPI"},
  {AEE_ADDRFIELD_COMPLEX,               "COMPLEX"},
  {AEE_ADDRFIELD_ADDITIONAL_NUMBER,     "ADDITIONAL_NUMBER"},
  {AEE_ADDRFIELD_ANNAME,                "ANNAME"},
  {AEE_ADDRFIELD_ANNAME_ID,             "ANNAME_ID"},
  {AEE_ADDRFIELD_GRPNAME,               "GRPNAME"},
  {AEE_ADDRFIELD_GRPNAME_ID,            "GRPNAME_ID"},
  {AEE_ADDRFIELD_HIDDEN,                "HIDDEN"},
  {AEE_ADDRFIELD_PHONE_VIDEOCALLERID,   "PHONE_VIDEOCALLERID"},
  {AEE_ADDRFIELD_PRIVATE_EMAIL1,        "PRIVATE_EMAIL1"},
  {AEE_ADDRFIELD_PRIVATE_EMAIL2,        "PRIVATE_EMAIL2"},
  {AEE_ADDRFIELD_BUSINESS_EMAIL1,       "BUSINESS_EMAIL1"},
  {AEE_ADDRFIELD_BUSINESS_EMAIL2,       "BUSINESS_EMAIL2"},
  {AEE_ADDRFIELD_BUSINESS_URL,          "BUSINESS_URL"},
  {AEE_ADDRFIELD_PHONE_BUSINESS_FAX,    "PHONE_BUSINESS_FAX"},
  {AEE_ADDRFIELD_BUSINESS_STREET,       "BUSINESS_STREET"},
  {AEE_ADDRFIELD_BUSINESS_CITY,         "BUSINESS_CITY"},
  {AEE_ADDRFIELD_BUSINESS_STATE,        "BUSINESS_STATE"},
  {AEE_ADDRFIELD_BUSINESS_COUNTRY,      "BUSINESS_COUNTRY"},
  {AEE_ADDRFIELD_BUSINESS_POSTALCODE,   "BUSINESS_POSTALCODE"},
  {AEE_ADDRFIELD_BUSINESS_NOTES,        "BUSINESS_NOTES"},
  {AEE_ADDRFIELD_RINGTONE_AUDIO,        "RINGTONE_AUDIO"},
  {AEE_ADDRFIELD_RINGTONE_VIDEO,        "RINGTONE_VIDEO"},
  {AEE_ADDRFIELD_BIRTHDAY,              "BIRTHDAY"}, 
  {AEE_ADDRFIELD_EVENT_UID,             "EVENT_UID"},
  {AEE_ADDRFIELD_DEFAULT_PHONE,         "DEFAULT_PHONE"},
  {AEE_ADDRFIELD_DEFAULT_EMAIL,         "DEFAULT_EMAIL"},
  {AEE_ADDRFIELD_DEFAULT_ADDRESS,       "DEFAULT_ADDRESS"},
  {AEE_ADDRFIELD_DEFAULT_URL,           "DEFAULT_URL"},
  {AEE_ADDRFIELD_DEFAULT_FAX,           "DEFAULT_FAX"},
  {AEE_ADDRFIELD_DISTRIBUTION_LIST,     "DISTRIBUTION_LIST"},
  {AEE_ADDRFIELD_HARDCODED_ECC,         "HARDCODED_ECC"},
  {AEE_ADDRFIELD_NV_ECC,                "NV_ECC"},
  {AEE_ADDRFIELD_SIM1_ECC,              "SIM1_ECC"},
  {AEE_ADDRFIELD_SIM2_ECC,              "SIM2_ECC"},
  {AEE_ADDRFIELD_NETWORK_ECC,           "NETWORK_ECC"},
  {AEE_ADDRFIELD_ECC_SERVICE_CATEGORY,  "ECC_SERVICE_CATEGORY"},
  {AEE_ADDRFIELD_PRIVATEPHONE_VOICETAG, "PRIVATEPHONE_VOICETAG"},
  {AEE_ADDRFIELD_BUSINESSPHONE_VOICETAG, "BUSINESSPHONE_VOICETAG"},
  {AEE_ADDRFIELD_PRIVATEMOBILE_VOICETAG, "PRIVATEMOBILE_VOICETAG"},
  {AEE_ADDRFIELD_BUSINESSMOBILE_VOICETAG, "BUSINESSMOBILE_VOICETAG"},
  {AEE_ADDRFIELD_VIDEOTELEPHONY,        "VIDEOTELEPHONY"},
  {AEE_ADDRFIELD_USEDCOUNT,             "USEDCOUNT"},
  {AEE_ADDRFIELD_BCARD,                 "BCARD"},
  {AEE_ADDRFIELD_GROUP,                 "GROUP"},
  {AEE_ADDRFIELD_DLISTS,                "DLISTS"},
  {AEE_ADDRFIELD_CALL_METHOD,           "CALL_METHOD"},
  {AEE_ADDRFIELD_AOC,                   "AOC"},
  {AEE_ADDRFIELD_PH_UID,                "PH_UID"},
  {AEE_ADDRFIELD_PRIVATE_ADDITIONALINFO,"PRIVATE_ADDITIONALINFO" },
  {AEE_ADDRFIELD_BUSINESS_ADDITIONALINFO,"BUSINESS_ADDITIONALINFO" },
  {AEE_ADDRFIELD_PHONE_VIDEOTELEPHONY,  "PHONE_VIDEOTELEPHONY" },
  {AEE_ADDRFIELD_PREFER_VT,             "PREFER VIDEOTELEPHONY" },
  {AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE,    "APPLICATION DEFINED" },
  {AEE_ADDRFIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE,     "APPLICATION DEFINED" },
  {AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE,   "APPLICATION DEFINED" },
  {AEE_ADDRFIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE,    "APPLICATION DEFINED" },
  {AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE,  "FIELDID_PRIMARY_PHONE"}
};

static uint16 aee_to_pbm_field[][2] = 
{
  {AEE_ADDRFIELD_NONE,                PBM_FIELD_NONE},
  {AEE_ADDRFIELD_FIRSTNAME,           PBM_FIELD_FIRSTNAME},
  {AEE_ADDRFIELD_MIDDLENAME,          PBM_FIELD_MIDDLENAME},
  {AEE_ADDRFIELD_LASTNAME,            PBM_FIELD_LASTNAME},
  {AEE_ADDRFIELD_NICKNAME,            PBM_FIELD_NICKNAME},
  {AEE_ADDRFIELD_NAME,                PBM_FIELD_NAME},
  {AEE_ADDRFIELD_ORG,                 PBM_FIELD_ORG},
  {AEE_ADDRFIELD_JOBTITLE,            PBM_FIELD_JOBTITLE},
  {AEE_ADDRFIELD_ADDRESS,             PBM_FIELD_ADDRESS},
  {AEE_ADDRFIELD_CITY,                PBM_FIELD_CITY},
  {AEE_ADDRFIELD_STATE,               PBM_FIELD_STATE},
  {AEE_ADDRFIELD_COUNTRY,             PBM_FIELD_COUNTRY},
  {AEE_ADDRFIELD_ZIPCODE,             PBM_FIELD_ZIPCODE},
  {AEE_ADDRFIELD_URL,                 PBM_FIELD_URL},
  {AEE_ADDRFIELD_DEPARTMENT,          PBM_FIELD_DEPARTMENT},
  {AEE_ADDRFIELD_EMAIL,               PBM_FIELD_EMAIL},
  {AEE_ADDRFIELD_GENDER,              PBM_FIELD_GENDER},
  {AEE_ADDRFIELD_PHONE_WORK,          PBM_FIELD_PHONE_WORK},
  {AEE_ADDRFIELD_PHONE_HOME,          PBM_FIELD_PHONE_HOME},
  {AEE_ADDRFIELD_PHONE_FAX,           PBM_FIELD_PHONE_FAX},
  {AEE_ADDRFIELD_PHONE_VOICE,         PBM_FIELD_PHONE_VOICE},
  {AEE_ADDRFIELD_PHONE_PREF,          PBM_FIELD_PHONE_PREF},
  {AEE_ADDRFIELD_PHONE_CELL,          PBM_FIELD_PHONE_CELL},
  {AEE_ADDRFIELD_PHONE_PAGER,         PBM_FIELD_PHONE_PAGER},
  {AEE_ADDRFIELD_PHONE_GENERIC,       PBM_FIELD_PHONE_GENERIC},
  {AEE_ADDRFIELD_PHONE_OTHER,         PBM_FIELD_PHONE_OTHER},
  {AEE_ADDRFIELD_BDAY,                PBM_FIELD_BDAY},
  {AEE_ADDRFIELD_SORTSTRING,          PBM_FIELD_SORTSTRING},
  {AEE_ADDRFIELD_NOTES,               PBM_FIELD_NOTES},
  {AEE_ADDRFIELD_SIP_URL,             PBM_FIELD_SIP_URL},
  {AEE_ADDRFIELD_DIAL_STRING,         PBM_FIELD_DIAL_STRING},
  {AEE_ADDRFIELD_INDEX,               PBM_FIELD_INDEX},
  {AEE_ADDRFIELD_VOICETAG,            PBM_FIELD_VOICETAG},
  {AEE_ADDRFIELD_RINGTONE,            PBM_FIELD_RINGTONE_AUDIO},
  {AEE_ADDRFIELD_SPEED_DIAL,          PBM_FIELD_SPEED_DIAL},
  {AEE_ADDRFIELD_NAMESUFFIX,          PBM_FIELD_NAMESUFFIX},
  {AEE_ADDRFIELD_NAMEPREFIX,          PBM_FIELD_NAMEPREFIX},
  {AEE_ADDRFIELD_OTHERNAME,           PBM_FIELD_OTHERNAME},
  {AEE_ADDRFIELD_PHOTO,               PBM_FIELD_PHOTO},
  {AEE_ADDRFIELD_PHOTOURL,            PBM_FIELD_PHOTOURL},
  {AEE_ADDRFIELD_POBOX,               PBM_FIELD_POBOX},
  {AEE_ADDRFIELD_ADDREXTRA,           PBM_FIELD_ADDREXTRA},
  {AEE_ADDRFIELD_STREET,              PBM_FIELD_STREET},
  {AEE_ADDRFIELD_UID,                 PBM_FIELD_UID},
  {AEE_ADDRFIELD_PUBLICKEY,           PBM_FIELD_PUBLICKEY},
  {AEE_ADDRFIELD_PUBLICKEYSTRING,     PBM_FIELD_PUBLICKEYSTRING},
  {AEE_ADDRFIELD_NUMBER_TYPE,         PBM_FIELD_NUMBER_TYPE},
  {AEE_ADDRFIELD_NUMBER_PLAN,         PBM_FIELD_NUMBER_PLAN},
  {AEE_ADDRFIELD_DATE,                PBM_FIELD_DATE},
  {AEE_ADDRFIELD_DURATION,            PBM_FIELD_DURATION},
  {AEE_ADDRFIELD_CALL_TYPE,           PBM_FIELD_CALL_TYPE},
  {AEE_ADDRFIELD_GSM_TONNPI,          PBM_FIELD_GSM_TONNPI},
  {AEE_ADDRFIELD_COMPLEX,             PBM_FIELD_COMPLEX},
  {AEE_ADDRFIELD_ADDITIONAL_NUMBER,   PBM_FIELD_ADDITIONAL_NUMBER},
  {AEE_ADDRFIELD_ANNAME,              PBM_FIELD_ANNAME},
  {AEE_ADDRFIELD_ANNAME_ID,           PBM_FIELD_ANNAME_ID},
  {AEE_ADDRFIELD_GRPNAME,             PBM_FIELD_GRPNAME},
  {AEE_ADDRFIELD_GRPNAME_ID,          PBM_FIELD_GRPNAME_ID},
  {AEE_ADDRFIELD_HIDDEN,              PBM_FIELD_HIDDEN},
  {AEE_ADDRFIELD_PHONE_VIDEOCALLERID, PBM_FIELD_PHONE_VIDEOCALLERID},
  {AEE_ADDRFIELD_PRIVATE_EMAIL1,      PBM_FIELD_PRIVATE_EMAIL1},
  {AEE_ADDRFIELD_PRIVATE_EMAIL2,      PBM_FIELD_PRIVATE_EMAIL2},
  {AEE_ADDRFIELD_BUSINESS_EMAIL1,     PBM_FIELD_BUSINESS_EMAIL1},
  {AEE_ADDRFIELD_BUSINESS_EMAIL2,     PBM_FIELD_BUSINESS_EMAIL2},
  {AEE_ADDRFIELD_BUSINESS_URL,        PBM_FIELD_BUSINESS_URL},
  {AEE_ADDRFIELD_PHONE_BUSINESS_FAX,  PBM_FIELD_PHONE_BUSINESS_FAX},
  {AEE_ADDRFIELD_BUSINESS_STREET,     PBM_FIELD_BUSINESS_STREET},
  {AEE_ADDRFIELD_BUSINESS_CITY,       PBM_FIELD_BUSINESS_CITY},
  {AEE_ADDRFIELD_BUSINESS_STATE,      PBM_FIELD_BUSINESS_STATE},
  {AEE_ADDRFIELD_BUSINESS_COUNTRY,    PBM_FIELD_BUSINESS_COUNTRY},
  {AEE_ADDRFIELD_BUSINESS_POSTALCODE, PBM_FIELD_BUSINESS_POSTALCODE},
  {AEE_ADDRFIELD_BUSINESS_NOTES,      PBM_FIELD_BUSINESS_NOTES},
  {AEE_ADDRFIELD_RINGTONE_AUDIO,      PBM_FIELD_RINGTONE_AUDIO},
  {AEE_ADDRFIELD_RINGTONE_VIDEO,      PBM_FIELD_RINGTONE_VIDEO},
  {AEE_ADDRFIELD_BIRTHDAY,            PBM_FIELD_BIRTHDAY}, 
  {AEE_ADDRFIELD_EVENT_UID,           PBM_FIELD_EVENT_UID},
  {AEE_ADDRFIELD_DEFAULT_PHONE,       PBM_FIELD_DEFAULT_PHONE},
  {AEE_ADDRFIELD_DEFAULT_EMAIL,       PBM_FIELD_DEFAULT_EMAIL},
  {AEE_ADDRFIELD_DEFAULT_ADDRESS,     PBM_FIELD_DEFAULT_ADDRESS},
  {AEE_ADDRFIELD_DEFAULT_URL,         PBM_FIELD_DEFAULT_URL},
  {AEE_ADDRFIELD_DEFAULT_FAX,         PBM_FIELD_DEFAULT_FAX},
  {AEE_ADDRFIELD_DISTRIBUTION_LIST,   PBM_FIELD_DISTRIBUTION_LIST},
  {AEE_ADDRFIELD_HARDCODED_ECC,       PBM_FIELD_HARDCODED_ECC},
  {AEE_ADDRFIELD_NV_ECC,              PBM_FIELD_NV_ECC},
  {AEE_ADDRFIELD_SIM1_ECC,            PBM_FIELD_SIM1_ECC},
  {AEE_ADDRFIELD_SIM2_ECC,            PBM_FIELD_SIM2_ECC},
  {AEE_ADDRFIELD_NETWORK_ECC,         PBM_FIELD_NETWORK_ECC},
  {AEE_ADDRFIELD_ECC_SERVICE_CATEGORY,PBM_FIELD_ECC_SERVICE_CATEGORY},
  {AEE_ADDRFIELD_PRIVATEPHONE_VOICETAG,  PBM_FIELD_PRIVATEPHONE_VOICETAG},
  {AEE_ADDRFIELD_BUSINESSPHONE_VOICETAG, PBM_FIELD_BUSINESSPHONE_VOICETAG},
  {AEE_ADDRFIELD_PRIVATEMOBILE_VOICETAG, PBM_FIELD_PRIVATEMOBILE_VOICETAG},
  {AEE_ADDRFIELD_BUSINESSMOBILE_VOICETAG,PBM_FIELD_BUSINESSMOBILE_VOICETAG},
  {AEE_ADDRFIELD_VIDEOTELEPHONY,      PBM_FIELD_VIDEOTELEPHONY},
  {AEE_ADDRFIELD_USEDCOUNT,           PBM_FIELD_USEDCOUNT},
  {AEE_ADDRFIELD_BCARD,               PBM_FIELD_BCARD},
  {AEE_ADDRFIELD_GROUP,               PBM_FIELD_GROUP},
  {AEE_ADDRFIELD_DLISTS,              PBM_FIELD_DLISTS},
  {AEE_ADDRFIELD_CALL_METHOD,         PBM_FIELD_CALL_METHOD},
  {AEE_ADDRFIELD_AOC,                 PBM_FIELD_AOC},
  {AEE_ADDRFIELD_PH_UID,              PBM_FIELD_PH_UID},
  {AEE_ADDRFIELD_PRIVATE_ADDITIONALINFO, PBM_FIELD_PRIVATE_ADDITIONALINFO},
  {AEE_ADDRFIELD_BUSINESS_ADDITIONALINFO,PBM_FIELD_BUSINESS_ADDITIONALINFO},
  {AEE_ADDRFIELD_PHONE_VIDEOTELEPHONY,   PBM_FIELD_PHONE_VIDEOTELEPHONY},
  {AEE_ADDRFIELD_PREFER_VT,           PBM_FIELD_PREFER_VT},
  {AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE,    PBM_FIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE },
  {AEE_ADDRFIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE,     PBM_FIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE },
  {AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE,   PBM_FIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE },
  {AEE_ADDRFIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE,    PBM_FIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE },
  {AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE,  PBM_FIELD_FIELDID_PRIMARY_PHONE}
};

const FieldTypeConversion pbm_to_aee_field_type[] = {
  {PBM_FT_NONE,    AEEDB_FT_NONE},
  {PBM_FT_BYTE,    AEEDB_FT_BYTE},
  {PBM_FT_WORD,    AEEDB_FT_WORD},
  {PBM_FT_DWORD,   AEEDB_FT_DWORD},
  {PBM_FT_UCS2,    AEEDB_FT_STRING},
  {PBM_FT_STRING,  AEEDB_FT_STRING}, 
  {PBM_FT_BINARY,  AEEDB_FT_BINARY},
  {PBM_FT_PHONE,   AEEDB_FT_PHONE},
  {PBM_FT_BITMAP,  AEEDB_FT_BITMAP},
  {PBM_FT_COMPLEX, AEEDB_FT_COMPLEX},
};


// Number of fields supported in the address book
#define OEM_ADDRESS_FIELDS_COUNT     (ARR_SIZE(aee_to_pbm_field)) // Number of fields in any category.
#define OEM_ADDRESS_NAMED_FIELDS_COUNT     (ARR_SIZE(gSupportedFieldList)) // Number of fields that have names.  
																	       // AEE_ADDRFIELD_NONE does not have a name.
																		 
// Max length of data supported by this address book 
// (e.g. length of a web address)
#define OEM_ADDRESS_MAX_DATA_LEN        256

// Used internally for checks on different parameters.
#define INVALID_GROUP -1
#define SIM_GROUP      0
#define ADN_GROUP      1
#define SDL_GROUP      2
#define EFS_GROUP      3
#define ECC_GROUP      4
#define NUM_GROUPS     5  //lint -esym(750,NUM_GROUPS)

//Global Properties
uint32                      gwProps;

// Flag to indicate if enumeration has been initialized
static boolean              gbEnumInit;

//Helper functions for working with phonebook manager.
static pbm_device_type cat_to_phonebook(AEEAddrCat wCat);
static pbm_cat_e_type cat_to_pbm_cat(AEEAddrCat wCat);
AEEAddrFieldID   OEMAddr_PBMFieldToAEE(pbm_field_id_e_type fid);
AEEAddrFieldType      OEMAddr_PBMFieldTypeToAEE(pbm_field_type_e_type pbmfType);
pbm_field_type_e_type OEMAddr_AEEFieldTypeToPBM(AEEAddrFieldType aeefType);
pbm_field_id_e_type OEMAddr_AEEFieldToPBM(AEEAddrFieldID fID);

//Helper functions for working with this interface.
static boolean  allocate_fields(AEEAddrField **ppFields, int nFieldCount, const int *pSizes);
static int OEMAddrBook_WriteComplexField(AEEAddrFieldComplex **, const AEEAddrField * pItems);
static int OEMAddr_GetComplexField(AEEAddrFieldComplex **pDestAddrFieldComplex, 
                                   const AEEAddrFieldComplex *pSrcAddrFieldComplex);


/* Forward delcarations of functions used to implement the Address Book. */
static uint32 OEMAddr_AddRef(IOEMAddrBook * po);
static uint32 OEMAddr_Release(IOEMAddrBook * po);
static uint16 OEMAddr_EnumNextRec(AEEAddrCat *pcat, AEEAddrField ** ppItems, int *pnItemCount, int *pErr);
static int    OEMAddr_EnumRecInit(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize);
static int    OEMAddr_GetCatCount(void);
static int    OEMAddr_GetCatList(AEEAddrCat *p, int nSize);
static int    OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize);
static int    OEMAddr_GetFieldInfoCount(AEEAddrCat c);
static uint16 OEMAddr_GetNumRecs(void);
static uint16 OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount, int *pErr);
static int    OEMAddr_RecordDelete(uint16 wrecID);
static int    OEMAddr_RecordGetByID(uint16 recID,AEEAddrCat *pCat, AEEAddrField ** ppItems, int *pnItemCount, int *pErr);
static int    OEMAddr_RecordUpdate(uint16 recID, AEEAddrCat cat, AEEAddrField * pItems, int nItemCount, int *pErr);
static int    OEMAddr_RemoveAllRecs(void);
static void   OEMAddr_SetProperties(uint32 dwProps);
static uint32 OEMAddr_GetProperties(void);
static int    OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize);
static int    OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize);

static VTBL(IOEMAddrBook)  gsOEMAddrBookFuncs = { OEMAddr_AddRef,
                                                  OEMAddr_Release,
                                                  OEMAddr_EnumNextRec,
                                                  OEMAddr_EnumRecInit,
                                                  OEMAddr_GetCatCount,
                                                  OEMAddr_GetCatList,
                                                  OEMAddr_GetFieldInfo,
                                                  OEMAddr_GetFieldInfoCount,
                                                  OEMAddr_GetNumRecs,
                                                  OEMAddr_RecordAdd,
                                                  OEMAddr_RecordDelete,
                                                  OEMAddr_RecordGetByID,
                                                  OEMAddr_RecordUpdate,
                                                  OEMAddr_RemoveAllRecs,
                                                  OEMAddr_SetProperties,
                                                  OEMAddr_GetProperties,
                                                  OEMAddr_GetCategoryName,
                                                  OEMAddr_GetFieldName
                                                  };

/*========================================================================
Function: cat_to_phonebook()

Description:
   Helper function that translates from AEEAddrCat to 
   pbm_device_type (phonebook)

Prototype:
   pbm_device_type cat_to_phonebook(AEEAddrCat wCat);

Parameters:
   wCat: [in]: Specifies the address category.

Return Value:
   The phonebook that corresponds to the category
========================================================================*/
static pbm_device_type cat_to_phonebook(AEEAddrCat wCat)
{
  switch (wCat)
  {
  case AEE_ADDR_CAT_NONE:
  case AEE_ADDR_CAT_PERSONAL:
  case AEE_ADDR_CAT_BUSINESS:
    return PBM_EFS;

#ifdef FEATURE_UIM
  case AEE_ADDR_CAT_SIM_ADN:
    return PBM_ADN;
    
  case AEE_ADDR_CAT_SIM_ADN2:
    return PBM_ADN2;
#endif /* FEATURE_UIM */

  case AEE_ADDR_CAT_SIM_FDN:
    return PBM_FDN;

  case AEE_ADDR_CAT_SIM_FDN2:
    return PBM_FDN2;

  case AEE_ADDR_CAT_SIM_SDN:
    return PBM_SDN;

  case AEE_ADDR_CAT_SIM_SDN2:
    return PBM_SDN2;

  case AEE_ADDR_CAT_SIM_MSISDN:
    return PBM_MSISDN;

  case AEE_ADDR_CAT_SIM_MSISDN2:
    return PBM_MSISDN2;

  case AEE_ADDR_CAT_SIM_MBDN:
    return PBM_MBDN;

  case AEE_ADDR_CAT_SIM_MBDN2:
    return PBM_MBDN2;

  case AEE_ADDR_CAT_SIM_MBN:
    return PBM_MBN;

  case AEE_ADDR_CAT_SIM_MBN2:
    return PBM_MBN2;

  case AEE_ADDR_CAT_SPEEDDIAL:  // Speed dial list
    return PBM_SDL;

  case AEE_ADDR_CAT_ECC: /* Emergency Numbers */
    return PBM_ECC;

  default:
    return PBM_EFS;  // Error here instead????
  }
}


/*========================================================================
Function: pbm_error_to_aee()

Description:
   Helper function that translates from PBM errors to AEE errors.

Prototype:
   static int pbm_error_to_aee(pbm_return_type pbmRet);

Parameters:
   pbmRet: [in]: Specifies the PBM error

Return Value:
   The AEE error that corresponds to PBM error specified
========================================================================*/
static int pbm_error_to_aee(pbm_return_type pbmRet)
{
  int ret = EFAILED;

  switch(pbmRet) {
    case PBM_SUCCESS:
    case PBM_EOF:
    case PBM_ERROR_NOT_FOUND:
      ret = AEE_SUCCESS;
      break;

    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_NOT_AVAILABLE:
      ret = ENOSUCH;
      break;

    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_SYNC:
      ret = EITEMBUSY;
      break;

    case PBM_ERROR_RESTRICTED:
      ret = EBADSTATE;
      break;

    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      ret = ENEEDMORE;
      break;

    case PBM_ERROR_MEM_FULL:
      ret = ENOMEMORY;
      break;

    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
    case PBM_ERROR_INVALID_REC:
      ret = EBADPARM;
      break;

    case PBM_ERROR_INVALID_ENUM_FIELD:
      ret = AEE_ADDR_INVALID_ENUM_FIELD;
      break;

    case PBM_ERROR_CAT_NOT_SUPPORTED:
      ret = AEE_ADDR_CAT_NOT_SUPPORTED;
      break;

    default:
      break;
  }

  return ret;
}

/*========================================================================
Function: cat_to_pbm_cat()

Description:
   Helper function that translates from AEEAddrCat to 
   pbm_cat_e_type (phonebook manager categories)

Prototype:
   pbm_cat_e_type cat_to_pbm_cat(AEEAddrCat wCat);

Parameters:
   wCat: [in]: Specifies the address category.

Return Value:
   The phonebook manager category which corresponds to the category
========================================================================*/
static pbm_cat_e_type cat_to_pbm_cat(AEEAddrCat wCat)
{
  switch (wCat)
  {
  case AEE_ADDR_CAT_PERSONAL:
    return PBM_CAT_PERSONAL;
    
  case AEE_ADDR_CAT_BUSINESS:
    return PBM_CAT_BUSINESS;
    
  case AEE_ADDR_CAT_SIM_ADN:
    return PBM_CAT_SIM_ADN;
    
  case AEE_ADDR_CAT_SIM_ADN2:
    return PBM_CAT_SIM_ADN2;
    
  case AEE_ADDR_CAT_SIM_FDN:
    return PBM_CAT_SIM_FDN;
    
  case AEE_ADDR_CAT_SIM_FDN2:
    return PBM_CAT_SIM_FDN2;
    
  case AEE_ADDR_CAT_SIM_SDN:
    return PBM_CAT_SIM_SDN;
    
  case AEE_ADDR_CAT_SIM_SDN2:
    return PBM_CAT_SIM_SDN2;
    
  case AEE_ADDR_CAT_SIM_MSISDN:
    return PBM_CAT_SIM_MSISDN;
    
  case AEE_ADDR_CAT_SIM_MSISDN2:
    return PBM_CAT_SIM_MSISDN2;

  case AEE_ADDR_CAT_SIM_MBDN:
    return PBM_CAT_SIM_MBDN;

  case AEE_ADDR_CAT_SIM_MBDN2:
    return PBM_CAT_SIM_MBDN2;

  case AEE_ADDR_CAT_SIM_MBN:
    return PBM_CAT_SIM_MBN;

  case AEE_ADDR_CAT_SIM_MBN2:
    return PBM_CAT_SIM_MBN2;

  case AEE_ADDR_CAT_SPEEDDIAL:  // Speed dial list
    return PBM_CAT_SDL;

  case AEE_ADDR_CAT_NONE:
    return PBM_CAT_NONE;

  case AEE_ADDR_CAT_ECC:  /* Emergency Numbers */
    return PBM_CAT_ECC;

  default:
    return PBM_CAT_ERROR;  
  }
}

/*========================================================================
Function: category_to_group()

Description:
   Helper function that translates from AEEAddrCat to a group.  The groups
   are defined in this file, which based on the category, contains the fields
   which are required/optional, and their data types.

Prototype:
   int category_to_group(AEEAddrCat wCat);

Parameters:
   wCat: [in]: Specifies the address category.

Return Value:
   The phonebook that corresponds to the category
========================================================================*/
int category_to_group(AEEAddrCat category)
{
  switch (category)
  {
#ifdef FEATURE_UIM
    case AEE_ADDR_CAT_SIM_ADN:
    case AEE_ADDR_CAT_SIM_ADN2:
      return ADN_GROUP;

    case AEE_ADDR_CAT_SIM_FDN:
    case AEE_ADDR_CAT_SIM_SDN:
    case AEE_ADDR_CAT_SIM_MSISDN:
    case AEE_ADDR_CAT_SIM_MBDN:
    case AEE_ADDR_CAT_SIM_MBN:
    case AEE_ADDR_CAT_SIM_FDN2:
    case AEE_ADDR_CAT_SIM_SDN2:
    case AEE_ADDR_CAT_SIM_MSISDN2:
    case AEE_ADDR_CAT_SIM_MBDN2:
    case AEE_ADDR_CAT_SIM_MBN2:
      return SIM_GROUP;
#endif /* FEATURE_UIM */
      
    case AEE_ADDR_CAT_SPEEDDIAL:
      return SDL_GROUP;

    case AEE_ADDR_CAT_NONE:
    case AEE_ADDR_CAT_PERSONAL:
    case AEE_ADDR_CAT_BUSINESS:
      return EFS_GROUP;

    case AEE_ADDR_CAT_ECC:
      return ECC_GROUP;

    default:
      return INVALID_GROUP;
  } // switch
} 

/*========================================================================
Function: OEMAddr_RecordWrite_PBM_CB()

Description:
   This function is called after PBM record_write is called.  It is the
   callback supplied to PBM and thus is called in the PBM context.  This
   callback conveys the results of the write operation.

Prototype:
   void OEMAddr_RecordWrite_PBM_CB(pbm_writecb_data_s_type *cb_data)

Parameters:
   cb_data: [in]: Specifies the results of the PBM_WRITE call.

Return Value:
   None

Comments:
   None

Side Effects:
   Calls UnblockUITask to allow the UI to run again.

See Also:
   None

========================================================================*/
/*lint -save -esym(818,cb_data) suppress could be const */
void OEMAddr_RecordWrite_PBM_CB(pbm_writecb_data_s_type *cb_data)
{
  OEMADDR_CB_DATA_STRUCT *retval_s = NULL;
  if (cb_data)
  {
    retval_s = cb_data->user_data;
    if (retval_s)
    {
      // If the user provided data, this data is an OEMADDR_CB_DATA_STRUCT
      // Fill it in with the info from PBM.
      retval_s->ret    = cb_data->ret;
      retval_s->rec_id = cb_data->rec_id;
    } 
  }

  MSG_HIGH("OEMAddr_RecordWrite_PBM_CB Called with %x %d %d", retval_s,
           retval_s?(int)retval_s->ret:0, retval_s?retval_s->rec_id:0);
  UnblockUITask();
}
/*lint -restore*/

/*===========================================================================
Function: allocate_fields

Description:
  Allocate the complete field array memory needed

Prototype:
  boolean allocate_fields(AEEAddrField **ppFields, int nFieldCount, int *pSizes)

Parameters:
  ppFields: [out]: Returns the field array allocated.
  nFieldCount: [in]: Specifies the number of fields
  pSizes: [in]: Specifies the number of byte in each

Return Value:
  TRUE if memory was allocated.  FALSE if not.
===========================================================================*/
static boolean allocate_fields(AEEAddrField **ppFields, int nFieldCount, const int *pSizes)
{
  int i;
  if ((*ppFields = MALLOC((uint32) nFieldCount * sizeof(AEEAddrField))) == NULL)
  {
    MSG_ERROR("Could not allocate %d bytes for %d fields",
              ((uint32) nFieldCount * sizeof(AEEAddrField)), nFieldCount, 0);

    return FALSE;
  }

  for (i = 0; i < nFieldCount; i++)
  {
    if (((*ppFields)[i].pBuffer = MALLOC((uint32) pSizes[i])) == NULL)
    {
      MSG_ERROR("Could not allocate %d bytes for field buffer %d",
                pSizes[i], i, 0);
      for (--i; i >= 0; i--)
      {
        FREEIF((*ppFields)[i].pBuffer);
      } 
      FREEIF(*ppFields);
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
Function: OEMAddrBook_WriteComplexField

Description:
  This function will create a complex field sturcutre from the field
  pointer to in pItems.

Prototype:
  static int OEMAddrBook_WriteComplexField(
          AEEAddrFieldComplex **pDestAddrFieldComplex, AEEAddrField * pItems)

Parameters:
  pDestAddrFieldComplex: [out]: Returns the field array allocated.
  pItems: [in]: The complex field.

Return Value:
  The datasize of the complex field.
===========================================================================*/
static int OEMAddrBook_WriteComplexField(
        AEEAddrFieldComplex **pDestAddrFieldComplex, const AEEAddrField * pItems)
{
  int i;
  int fieldCount = 0;
  AEEAddrField* pAddrField;
  AEEAddrFieldComplex *pAddrFieldComplex;
  
  int dataSize = 0;

  if(pItems == NULL || pItems->pBuffer == NULL)
  {
    MSG_ERROR("Null Data in ComplexField",0,0,0);
    return 0;
  }

  if(*pDestAddrFieldComplex == NULL)
    *pDestAddrFieldComplex= AEEAddrFieldComplex_New();

  if(*pDestAddrFieldComplex == NULL)
  {
    MSG_ERROR("Could not get Complex Field Ptr",0,0,0);
    return 0;
  }
  
  pAddrFieldComplex = (AEEAddrFieldComplex *)pItems->pBuffer;

  fieldCount = AEEAddrFieldComplex_GetSubFieldCount(pAddrFieldComplex);

  pAddrField = MALLOC(sizeof(AEEAddrField));  
  
  for(i=0;i<fieldCount;i++)
  {
    if(AEEAddrFieldComplex_GetSubField(pAddrFieldComplex,i,pAddrField) != AEE_SUCCESS)
    {
      MSG_ERROR("Could not get Sub Field",0,0,0);
      FREEIF(pAddrField);
      return 0;
    }
    
    if ( (pAddrField->fType == AEEDB_FT_PHONE) )
    {
       uint8 temp_str[OEM_ADDRESS_MAX_DATA_LEN];
       if (!pbmconvert_ucs2_to_pbm8((uint16 *) pAddrField->pBuffer,
                                    ((pAddrField->wDataLen)/sizeof(AECHAR))-1,
                                    temp_str, OEM_ADDRESS_MAX_DATA_LEN))
       {
         MSG_ERROR("Could not convert all bytes to output string.\n", 0, 0, 0);
       }

       pAddrField->pBuffer = temp_str;
       pAddrField->wDataLen = (uint16)(STRLEN((char *) temp_str) + 1);
       (void)AEEAddrFieldComplex_AddSubField(pDestAddrFieldComplex,pAddrField);
    }
    else
    {
       (void)AEEAddrFieldComplex_AddSubField(pDestAddrFieldComplex,pAddrField);   
    }

    dataSize = (int)AEEAddrFieldComplex_GetSize(*pDestAddrFieldComplex);
         
  }
  FREEIF(pAddrField);
  return dataSize;
}

/*===========================================================================
Function: OEMAddr_GetComplexField

Description:
  This function looks at complex field coming in and creates a new
  complex field based on it.
  

Prototype:
static int OEMAddr_GetComplexField(AEEAddrFieldComplex **pDestAddrFieldComplex, 
                                   AEEAddrFieldComplex *pSrcAddrFieldComplex)

Parameters:
  pDestAddrFieldComplex: [out]: Returns the field array allocated.
  pSrcAddrFieldComplex:  [in]: The source complex field.

Return Value:
  The datasize of the complex field.
===========================================================================*/
static int OEMAddr_GetComplexField(AEEAddrFieldComplex **pDestAddrFieldComplex, 
                                   const AEEAddrFieldComplex *pSrcAddrFieldComplex)
 {
  int i;
  int fieldCount = 0;
  AEEAddrField* pAddrField;

  if(pSrcAddrFieldComplex == NULL)
  {
    MSG_ERROR("Source Does not have any Complex Data",0,0,0);
    return 0;
  }
  
  if(*pDestAddrFieldComplex == NULL)
     *pDestAddrFieldComplex = AEEAddrFieldComplex_New();

  
  fieldCount = AEEAddrFieldComplex_GetSubFieldCount(pSrcAddrFieldComplex);
  
  pAddrField = MALLOC(sizeof(AEEAddrField));  
  
  for(i=0;i<fieldCount;i++)
  {
    if(AEEAddrFieldComplex_GetSubField(pSrcAddrFieldComplex,i,pAddrField) != AEE_SUCCESS)
    {
      MSG_ERROR("Could not get Sub Field",0,0,0);
      FREEIF(pAddrField);
      FREEIF(pDestAddrFieldComplex);
      return 0;
    }
    
    if ( (pAddrField->fType == AEEDB_FT_PHONE) )
    {
       AECHAR temp_str[OEM_ADDRESS_MAX_DATA_LEN*2 +1];
       (void) STRTOWSTR(pAddrField->pBuffer, temp_str,
                        MIN(pAddrField->wDataLen * sizeof(AECHAR), sizeof(temp_str)));
       pAddrField->pBuffer = temp_str;
       pAddrField->wDataLen = ((uint16)(WSTRLEN(temp_str)+1)*sizeof(AECHAR));
       (void)AEEAddrFieldComplex_AddSubField(pDestAddrFieldComplex, pAddrField);
    
    }
    else
      (void)AEEAddrFieldComplex_AddSubField(pDestAddrFieldComplex, pAddrField);
    
  }
  FREEIF(pAddrField);

  return ((int)AEEAddrFieldComplex_GetSize(*pDestAddrFieldComplex));
   
 }

/*=============================================================================
FUNCTION: OEMAddr_PBMFieldTypeToAEE

DESCRIPTION:
  Convert PBM field type to AEE field types.

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
AEEAddrFieldType OEMAddr_PBMFieldTypeToAEE(pbm_field_type_e_type pbmfType)
{
  int i;

  for (i=0; i<(int)ARR_SIZE(pbm_to_aee_field_type); i++)
  {
    if (pbm_to_aee_field_type[i].pbm_type == pbmfType)
    {
      return pbm_to_aee_field_type[i].aee_type;
    }
  }

  return (AEEAddrFieldType)pbmfType;
}

/*=============================================================================
FUNCTION: OEMAddr_AEEFieldTypeToPBM

DESCRIPTION:
  Convert AEE field type to PBM field types.

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
pbm_field_type_e_type OEMAddr_AEEFieldTypeToPBM(AEEAddrFieldType aeefType)
{
  int i;

  for (i=0; i<(int)ARR_SIZE(pbm_to_aee_field_type); i++)
  {
    if (pbm_to_aee_field_type[i].aee_type == aeefType)
    {
      return pbm_to_aee_field_type[i].pbm_type;
    }
  }

  return (pbm_field_type_e_type)aeefType;
}


/*=============================================================================
FUNCTION: OEMAddr_AEEFieldToPBM

DESCRIPTION:
  Convert AEE field id to PBM field id.

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
pbm_field_id_e_type OEMAddr_AEEFieldToPBM(AEEAddrFieldID fID)
{
  uint32 i;

  for (i = 0; i < ARR_SIZE(aee_to_pbm_field); i++)
  {
    if (aee_to_pbm_field[i][0] == fID)
      return aee_to_pbm_field[i][1];
  }
  MSG_ERROR("Could not Map FieldId from AEE to PBM %d",fID,0,0);
  return PBM_FIELD_NONE;
}

/*=============================================================================
FUNCTION: OEMAddr_PBMFieldToAEE

DESCRIPTION:
  Convert PBM field id to AEE field id.

COMMENTS:

SIDE EFFECTS:

=============================================================================*/
AEEAddrFieldID OEMAddr_PBMFieldToAEE(pbm_field_id_e_type fid)
{
  uint32 i;

  for (i = 0; i < ARR_SIZE(aee_to_pbm_field); i++)
  {
    if (aee_to_pbm_field[i][1] == fid)
      return aee_to_pbm_field[i][0];
  }
  MSG_ERROR("Could not Map FieldId to AEE from PBM %d",fid,0,0);
  return AEE_ADDRFIELD_NONE;
}

/*========================================================================
Function: OEMAddrBook_New()

Description:
   Function called when the AEE layer creates a new object.

Prototype:
   int OEMAddrBook_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)

Parameters:
   pIShell: [in]: Shell pointer from the creating entity.
   ClsId: [in]: Class ID being created. 
   ppObj: [out]: Resulting pointer to the object.

Return Value:
   The phonebook that corresponds to the category
========================================================================*/
int OEMAddrBook_New(IShell * pIShell, AEECLSID ClsId, void ** ppObj)
{
   COEMAddrBook *pNew;
   int Err = EUNSUPPORTED;

   *ppObj = NULL;

   /* Make sure the object is know. */
   if (ClsId == AEECLSID_OEMADDRBOOK)
   {
      // Allocate 
      //lint -save -e740 -e826 unusual ptr ops
      pNew = (COEMAddrBook*)AEE_NewClassEx((IBaseVtbl*)&gsOEMAddrBookFuncs,
                                            sizeof(COEMAddrBook), TRUE);
      //lint -restore
      if (!pNew)
      {
         Err = ENOMEMORY;
      }
      else
      {
         gbEnumInit = FALSE;

         // Return success.
         Err = AEE_SUCCESS;
         *ppObj = (void *) pNew;
      }
   }
   return Err;
}

/*========================================================================
Function: OEMAddr_AddRef

Description:
   Increments the reference count.

Prototype:
   static uint32  OEMAddr_AddRef(IOEMAddrBook * po)

Parameters:
   po: [in]: Object pointer.

Return Value:
   The number of references.
========================================================================*/
static uint32  OEMAddr_AddRef(IOEMAddrBook * po)
{
  //lint -save -e740 -e826 unusual ptr ops
  COEMAddrBook *pMe = (COEMAddrBook *)po;
  //lint -restore

  if (pMe)
    return(++(pMe->m_nRefs));
  else
    return 0;
}

/*========================================================================
Function: OEMAddr_Release

Description:
   Decrements the reference count.  If it reaches 0, free's the associated
   memory.

Prototype:
   static uint32 OEMAddr_Release(IOEMAddrBook * po)

Parameters:
   po: [in]: Object pointer.

Return Value:
   The number of references.
========================================================================*/
static uint32 OEMAddr_Release(IOEMAddrBook * po)
{
  //lint -save -e740 -e826 unusual ptr ops
  COEMAddrBook *pMe = (COEMAddrBook *)po;
  //lint -restore

  if (pMe)
  {
    if(pMe->m_nRefs)
    {
      if(--pMe->m_nRefs == 0)
      {
        sys_free(pMe);
      }
      else
        return pMe->m_nRefs;
    }
  }
  return 0;
}

/*========================================================================
Function: OEMAddr_RecordAdd()

Description:
   This function adds a new record to the address book. The fields to be added into
   this record are passed as parameters to this function

Prototype:
   uint16  OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount,int *pErr)

Parameters:
   cat: [in]: Specifies the address category to which this record belongs to. If concept of
   categories is not supported, this parameter can be ignored.

   pItems: [in]: Pointer to an arry of items that must be added to the record. Each item contains
   information such as: FieldID, DataType, Data, DataLength. For detailed information about
   this structure please refer to AEEAddrBook.h or to the BREW API Reference Guide

   nItemCount: [in]: Number of fields in this record. It also indicates that the array
   pItems contains this many number of fields.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   If record was successfully added, this function must return the recordID of the newly 
   added record:
   If failed, this function must return AEE_ADDR_RECID_NULL and the parameter *pErr must
   contain the error code. A list of error codes can be found in AEEError.h

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
//lint -save -esym(818,pItems)
uint16 OEMAddr_RecordAdd(AEEAddrCat cat, AEEAddrField * pItems, int nItemCount, int *pErr)
{
  uint16            wRecID;
  pbm_return_type   pbmRet;
  OEMADDR_CB_DATA_STRUCT *pbmRet_callback;

  pbm_device_type   pb_id;
  pbm_cat_e_type    pb_cat;
  uint8            *fields_buf;
  pbm_field_s_type *fields_buf_typed; /* a way to access the pbm_field_s_type 
                                       * structures at the beginning of fields_buf */
  uint32            fields_buf_size;
  int               num_fields;
  int               *data_sizes;
  int               i;
  
  MSG_HIGH("OEMAddr_RecordAdd: Cat %d Count %d", cat, nItemCount, 0);
  if (!pItems)
  {
    if (pErr)
      *pErr = EBADPARM;
    return(AEE_ADDR_RECID_NULL);
  }
  if (category_to_group(cat) == INVALID_GROUP)
  {
    if (pErr)
      *pErr = AEE_ADDR_CAT_NOT_SUPPORTED;
    return(AEE_ADDR_RECID_NULL);
  }

  wRecID = 0xFFFF;  // Adding a record.  The record ID will be placed here.
 
  // Translate args. to pbm data types
  pb_id = cat_to_phonebook(cat);
  pb_cat = cat_to_pbm_cat(cat);

  if (pb_cat == PBM_CAT_ERROR) {
    if (pErr)
      *pErr = EBADPARM;
	return AEE_ADDR_RECID_NULL;
  }

  num_fields = nItemCount;

  data_sizes = MALLOC((uint32) num_fields * sizeof(int));
  if (!data_sizes)
  {
    if (pErr)
      *pErr = ENOMEMORY;
    return AEE_ADDR_RECID_NULL;
  }
 
  // Build data_sizes[]
  for (i = 0; i < nItemCount; i++)
  {
    /* If the field id is AEE_ADDRFIELD_INDEX, it is used to obtain the
     * location of the record and the field itself is not stored directly
     * as part of the record. The index can be recovered later from the
     * record id stored as part of each record.
     */
    if (pItems[i].fID == AEE_ADDRFIELD_INDEX) {
      wRecID = pbm_location_to_record_id(pb_id,  *((uint16 *)pItems[i].pBuffer));
    }

    if ( (pItems[i].fType == AEEDB_FT_PHONE) )
    {
      data_sizes[i] = pItems[i].wDataLen/sizeof(AECHAR);
    }
    else if(pItems[i].fType == AEEDB_FT_COMPLEX)
    {
       AEEAddrFieldComplex *complexAddrField = NULL;
       data_sizes[i] = OEMAddrBook_WriteComplexField(&complexAddrField,&pItems[i]);
       if(complexAddrField != NULL){
          AEEAddrFieldComplex_Dtor(complexAddrField);
          complexAddrField = NULL;
       }
    }
    else
    {
      data_sizes[i] = pItems[i].wDataLen;
    }
  }

  if( !pbm_allocate_fields(&fields_buf, &fields_buf_size, (uint32) num_fields, (uint32 *) data_sizes) )
  {
    FREEIF(data_sizes);
    if (pErr)
      *pErr = ENOMEMORY;
    return AEE_ADDR_RECID_NULL;
  }
  fields_buf_typed = (pbm_field_s_type*)fields_buf; //lint !e826 ptr area too small

  // Copy fields in
  for (i = 0; i < nItemCount; i++)
  {
    pbm_field_id_e_type field_id = OEMAddr_AEEFieldToPBM(pItems[i].fID);
   
    if(pItems[i].fType == AEEDB_FT_STRING)           
    {
      pbm_fill_field(&fields_buf_typed[i], field_id, PBM_FT_UCS2,
                     pItems[i].pBuffer, (uint16)data_sizes[i]);   
    }
    else if(pItems[i].fType == AEEDB_FT_PHONE)
    {
      char temp_str[OEM_ADDRESS_MAX_DATA_LEN];
      if (0 == pItems[i].wDataLen)
      {
        temp_str[0] = '\0';
      }
      else
      {
        int len = pItems[i].wDataLen/sizeof(AECHAR);
        (void) WSTRTOSTR(pItems[i].pBuffer, temp_str, MIN(len, sizeof(temp_str)));
      }

      pbm_fill_field(&fields_buf_typed[i], 
                     field_id, 
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                     (byte *)temp_str, 
                     (uint16)data_sizes[i]);  
    }
    else if(pItems[i].fType == AEEDB_FT_COMPLEX)
    {
       AEEAddrFieldComplex *complexAddrField = NULL;
       (void) OEMAddrBook_WriteComplexField(&complexAddrField,&pItems[i]);
       if(complexAddrField != NULL){
         pbm_fill_field(&fields_buf_typed[i], 
                        field_id, 
                        OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                        (byte *)complexAddrField, 
                        (uint16)data_sizes[i]);    
       }
       AEEAddrFieldComplex_Dtor(complexAddrField);
       complexAddrField = NULL;
    }
    else if ((pItems[i].fType == AEEDB_FT_WORD) &&
             (pItems[i].fID == AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE))
    {
      /* Convert primary phone field ID data from addrbook field ID to PBM field ID */
      pbm_field_id_e_type pbm_field_id = OEMAddr_AEEFieldToPBM(*((AEEAddrFieldID*)pItems[i].pBuffer));
      void *pBuffer = &pbm_field_id;

      if (PBM_FIELD_NONE == pbm_field_id)
      {
        ERR("OEMAddr_RecordAdd() FIELDID_PRIMARY_PHONE invalid field id %d.", 
            *((AEEAddrFieldID*)pItems[i].pBuffer), 0, 0);
        if (pErr)
          *pErr = EBADPARM;
        FREEIF(data_sizes);
        pbm_free_fields(&fields_buf,num_fields);
        return AEE_ADDR_RECID_NULL;
      }

      pbm_fill_field(&fields_buf_typed[i], 
                     field_id, 
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType),
                     pBuffer,
                     (uint16)data_sizes[i]);
    }
    else
    {
      pbm_fill_field(&fields_buf_typed[i], 
                     field_id, 
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                     pItems[i].pBuffer, 
                     (uint16)data_sizes[i]);
    }
  }

  FREEIF(data_sizes);

  if((pbmRet = pbm_record_validate(pb_id,wRecID,pb_cat,fields_buf,(int)fields_buf_size,num_fields)) != PBM_SUCCESS)
  {
    ERR("pbm_record_validate() failed with error %d.", pbmRet, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet);
    pbm_free_fields(&fields_buf,num_fields);
    return AEE_ADDR_RECID_NULL;
  }
    

  pbmRet_callback = MALLOC((uint32) sizeof(*pbmRet_callback));
  if (pbmRet_callback == NULL)
  {
    ERR("Could not allocate %d bytes.", sizeof(*pbmRet_callback), 0, 0);
    if (pErr)
      *pErr = ENOMEMORY;
    pbm_free_fields(&fields_buf,num_fields);
    return AEE_ADDR_RECID_NULL;
  }

  ClearBlockUITask();
  pbmRet = pbm_record_write(pb_id, &wRecID, (int)pb_cat, num_fields, fields_buf, (int)fields_buf_size, 
                            OEMAddr_RecordWrite_PBM_CB, pbmRet_callback);

  pbm_free_fields(&fields_buf,num_fields);

  if (pbmRet != PBM_SUCCESS)
  {
    FREEIF(pbmRet_callback);
    ERR("pbm_write() failed with error %d.", pbmRet, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet);
    return AEE_ADDR_RECID_NULL;
  }

  /* Now wait for the callback to occur. */
  BlockUITask();

  if (pbmRet_callback->ret != PBM_SUCCESS)
  {
    ERR("pbm_write() failed with error %d.", pbmRet_callback->ret, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet_callback->ret);
    FREEIF(pbmRet_callback);
    return AEE_ADDR_RECID_NULL;
  }

  FREEIF(pbmRet_callback);
  return wRecID;
}
//lint -restore
   
   
/*========================================================================
Function: OEMAddr_RecordUpdate()

Description:
   This function updates all the fields in the record of given ID. It replaces all the existing
   fields in that record with the fields being passed to this function.
   The recID indicates which record needs to be updated.

Prototype:
   int  OEMAddr_RecordUpdate(uint16 recID,AEEAddrCat cat,AEEAddrField * pItems, int nItemCount,int *pErr)

Parameters:
   recID: [in]: ID of the record whose fields need to be updated.

   cat: [in]: Specifies the address category to which this record belongs to. If concept of
   categories is not supported, this parameter can be ignored.

   pItems: [in]: Pointer to an arry of items that must be added to the record. Each item contains
   information such as: FieldID, DataType, Data, DataLength. For detailed information about
   this structure please refer to AEEAddrBook.h or to the BREW API Reference Guide

   nItemCount: [in]: Number of fields in this record. It also indicates that the array
   pItems contains this many number of fields.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   AEE_SUCCESS: If the record was successfully deleted
   EFAILED: if failed

Comments:
   This function is quite similar to the function OEMAddr_RecordAdd() with the main difference
   being that this function updates all the fields of an already existing record while
   OEMAddr_RecordAdd() adds a new record to the address book.

Side Effects:
   None

See Also:
   OEMAddr_RecordAdd

========================================================================*/
int OEMAddr_RecordUpdate(uint16 recID, 
                         AEEAddrCat cat, 
                         AEEAddrField * pItems, 
                         int nItemCount, 
                         int *pErr)
{
  pbm_return_type  pbmRet;
  OEMADDR_CB_DATA_STRUCT *pbmRet_callback;
  
  pbm_device_type  pb_id;
  pbm_cat_e_type   pb_cat;
  uint8            *fields_buf;
  pbm_field_s_type *fields_buf_typed; /* a way to access the pbm_field_s_type 
                                       * structures at the beginning of fields_buf */
  uint32           fields_buf_size;
  int              num_fields;
  int              *data_sizes;
  int              i;
  uint16           wRecID = recID;
  
  if(!pItems)
  {
    if (pErr)
      *pErr = EBADPARM;
    return EFAILED;
  }
  
  MSG_HIGH("OEMAddr_RecordUpdate: recID %d Cat %d Count %d",
           recID, (int) cat, nItemCount);

  if (recID == AEE_ADDR_RECID_NULL)
  {
    if (pErr)
      *pErr = EBADPARM;
    return EFAILED;
  }

  // Translate args. to pbm data types
  pb_id = cat_to_phonebook(cat);
  pb_cat = cat_to_pbm_cat(cat);
  num_fields = nItemCount;
  data_sizes = MALLOC((uint32) num_fields * sizeof(int));
  if (!data_sizes)
  {
    if (pErr)
      *pErr = ENOMEMORY;
    return EFAILED;
  }
 
  // Build data_sizes[]
  for (i = 0; i < nItemCount; i++)
  {
    /* Skip ADDRFIELD_INDEX */
    if (pItems[i].fID == AEE_ADDRFIELD_INDEX) {
      wRecID = pbm_location_to_record_id(pb_id,  *((uint16 *)pItems[i].pBuffer));
      if (wRecID != recID)
      {
        FREEIF(data_sizes);
        if (pErr)
          *pErr = AEE_ADDR_INVALID_FIELD_VALUE;
        return (EFAILED);
      }
      data_sizes[i] = pItems[i].wDataLen;
    }

    if ( (pItems[i].fType == AEEDB_FT_PHONE) )
    {
      data_sizes[i] = pItems[i].wDataLen/sizeof(AECHAR);
    }
    else if(pItems[i].fType == AEEDB_FT_COMPLEX)
    {     
      AEEAddrFieldComplex *complexAddrField = NULL;
      data_sizes[i] = OEMAddrBook_WriteComplexField(&complexAddrField,&pItems[i]);
      if(complexAddrField != NULL){
         AEEAddrFieldComplex_Dtor(complexAddrField);
         complexAddrField = NULL;
      }
    }
    else
    {
      data_sizes[i] = pItems[i].wDataLen;
    }
  }

  if( !pbm_allocate_fields(&fields_buf, &fields_buf_size, (uint32) num_fields, (uint32 *) data_sizes) )
  {
    FREEIF(data_sizes);
    if (pErr)
      *pErr = ENOMEMORY;
    return EFAILED;
  }
  fields_buf_typed = (pbm_field_s_type*)fields_buf; //lint !e826 ptr conversion

  // Copy fields in
  for (i = 0; i < nItemCount; i++)
  {
    pbm_field_id_e_type field_id = OEMAddr_AEEFieldToPBM(pItems[i].fID);

    if (pItems[i].fType == AEEDB_FT_STRING) 
    {
      pbm_fill_field(&fields_buf_typed[i], field_id, PBM_FT_UCS2, 
                     (byte *)pItems[i].pBuffer, (uint16)data_sizes[i]); 
    }
    else if (pItems[i].fType == AEEDB_FT_PHONE)
    {
      char temp_str[OEM_ADDRESS_MAX_DATA_LEN];
      if (0 == pItems[i].wDataLen) {
        temp_str[0] = '\0';
      } else {
        int len = pItems[i].wDataLen/sizeof(AECHAR);
        (void) WSTRTOSTR(pItems[i].pBuffer, temp_str, MIN(len, sizeof(temp_str)));
      }
      pbm_fill_field(&fields_buf_typed[i], 
                     field_id, 
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                     (byte *)temp_str, 
                     (uint16)data_sizes[i]); 
    }
    else if(pItems[i].fType == AEEDB_FT_COMPLEX)
    {
      AEEAddrFieldComplex *complexAddrField = NULL;
      (void) OEMAddrBook_WriteComplexField(&complexAddrField,&pItems[i]);
      if(complexAddrField != NULL){
        pbm_fill_field(&fields_buf_typed[i], 
                       field_id, 
                       OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                       (byte *)complexAddrField, 
                       (uint16)data_sizes[i]);    
      }
      AEEAddrFieldComplex_Dtor(complexAddrField);
      complexAddrField = NULL;
    }
    else if ((pItems[i].fType == AEEDB_FT_WORD) &&
             (pItems[i].fID == AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE))
    {
      /* Convert primary phone field ID data from addrbook field ID to PBM field ID */
      pbm_field_id_e_type pbm_field_id = OEMAddr_AEEFieldToPBM(*((AEEAddrFieldID*)pItems[i].pBuffer));
      void *pBuffer = &pbm_field_id;

      if (PBM_FIELD_NONE == pbm_field_id)
      {
        ERR("OEMAddr_RecordUpdate() FIELDID_PRIMARY_PHONE invalid field id %d.", 
            *((AEEAddrFieldID*)pItems[i].pBuffer), 0, 0);
        if (pErr)
          *pErr = EBADPARM;
        FREEIF(data_sizes);
        pbm_free_fields(&fields_buf,num_fields);
        return EFAILED;
      }

      pbm_fill_field(&fields_buf_typed[i], 
                     field_id,  
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                     pBuffer,
                     (uint16)data_sizes[i]);
    }
    else
    {
      pbm_fill_field(&fields_buf_typed[i], 
                     field_id, 
                     OEMAddr_AEEFieldTypeToPBM(pItems[i].fType), 
                     pItems[i].pBuffer, 
                     (uint16)data_sizes[i]);
    }
  }

  FREEIF(data_sizes);
  
  if((pbmRet = pbm_record_validate(pb_id,wRecID,pb_cat,fields_buf,(int)fields_buf_size,num_fields)) != PBM_SUCCESS)
  {
    ERR("pbm_record_validate() failed with error %d.", pbmRet, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet);
    pbm_free_fields(&fields_buf,num_fields);
    return EFAILED;
  }
  
  pbmRet_callback = MALLOC((uint32) sizeof(*pbmRet_callback));
  if (pbmRet_callback == NULL)
  {
    ERR("Could not allocate %d bytes.", sizeof(*pbmRet_callback), 0, 0);
    if (pErr)
      *pErr = ENOMEMORY;
    pbm_free_fields(&fields_buf,num_fields);
    return EFAILED;
  }

  ClearBlockUITask();
  pbmRet = pbm_record_write(pb_id, &wRecID, (uint16)pb_cat, num_fields, fields_buf, 
                            (int)fields_buf_size, OEMAddr_RecordWrite_PBM_CB, 
                            pbmRet_callback);
  pbm_free_fields(&fields_buf,num_fields);
  
  if (pbmRet != PBM_SUCCESS)
  {
    ERR("pbm_write() failed with error %d.", pbmRet, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet);
    FREEIF(pbmRet_callback);
    return EFAILED;
  }
  
  /* Now wait for the callback to occur. */
  BlockUITask();

  if (pbmRet_callback->ret != PBM_SUCCESS)
  {
    ERR("pbm_write() failed with error %d.", pbmRet_callback->ret, 0, 0);
    if (pErr)
      *pErr = pbm_error_to_aee(pbmRet_callback->ret);
    FREEIF(pbmRet_callback);
    return EFAILED;
  }

  FREEIF(pbmRet_callback);
  return AEE_SUCCESS;
}
   
   
/*========================================================================
Function: OEMAddr_RecordGetByID()

Description:
   This function retrieves information of a record with given record ID. This function
   returns information about all the fields in this record. This function does NOT
   delete the record from the address book. It just reads the record and returns 
   information (data) of that record to the caller.

Prototype:
   int   OEMAddr_RecordGetByID(uint16 recID,AEEAddrCat *pcat,AEEAddrField ** ppItems, int *pnItemCount,int *pErr)

Parameters:
   recID: [in]: ID of the record whose fields need to be retrieved and returned

   pCat: [in/out]: On input, this is a valid pointer to AEEAddrCat. On return, this pointer
   points to the address category to which the record belongs to. 

   ppItems: [in/out]: Pointer for passing info about the fields. Memory for *ppItems must
   be allocated by the OEMs while implementing this function. This memory will be freed
   by the caller (BREW). Memory must be allocated by the OEMs using the function
   MALLOC(). 

   pnItemCount: [in/out]: On input, this is a valid pointer to an integer. On return,
   this contains the count of the number of fields present in this record. 
   It also indicates that the array  *ppItems contains this many number of fields on return.

   pErr: [out]: If any error occurs, the error code must be placed into this pointer before
   returning from this function. OEMs must check for this parameter being NULL before storing
   the error value into this.

Return Value:
   AEE_SUCCESS: If the record information was successfully retrieved
   EFAILED: if failed. In this case, *pErr must contain the exact error code.

Comments:
   Note that memory for *ppItems must be allocated by implementer of this function. This
   memory will be released by the caller.

Side Effects:
   None

See Also:
   None

========================================================================*/
int OEMAddr_RecordGetByID(uint16 recID,AEEAddrCat *pCat, 
                          AEEAddrField ** ppItems, 
                          int *pnItemCount, 
                          int *pErr)
{
  pbm_return_type       pbmRet;
  int i;

  pbm_device_type       pb_id;
  pbm_field_s_type      *fields_buf;
  uint32                fields_size;
  int                   *dataSizes = NULL;
  int                   nItemCount;
  int                   ret = AEE_SUCCESS;
  

  if( !pCat || !ppItems || !pnItemCount )
  {
    if (pErr)
      *pErr = EBADPARM;
    return EFAILED;
  }

  MSG_HIGH("OEMAddr_RecordGetByID: recID %d", recID, 0, 0);

  if (recID == AEE_ADDR_RECID_NULL)
  {
    if (pErr)
      *pErr = EBADPARM;
    return EFAILED;
  }

  pb_id = pbm_record_id_to_phonebook(recID);

  fields_size = (uint32)pbm_calculate_fields_size_from_id(recID);
  if (fields_size == 0)
  {
    if (pErr)
      *pErr = pbm_error_to_aee(PBM_ERROR_NOT_FOUND);
    return EFAILED;
  }

  fields_buf = MALLOC(fields_size);
  if (fields_buf == NULL)
  {
    if (pErr)
      *pErr = ENOMEMORY;
    return EFAILED;
  }

  do
  {
    pbmRet = pbm_record_read(recID, pCat, pnItemCount, (uint8*)fields_buf, fields_size);
    if (pbmRet != PBM_SUCCESS)
    {
      ERR("pbm_record_read() failed with error %d.", pbmRet, 0, 0);
      if (pErr)
        *pErr = pbm_error_to_aee(pbmRet);
      ret = EFAILED;
      break;
    }

    /* For SDL phonebook, we need to create a new field with index obtained 
       from the record id */
    if (pb_id == PBM_SDL)
      nItemCount = (*pnItemCount)++;
    else
      nItemCount = *pnItemCount;

    if ((dataSizes = MALLOC((uint32) *pnItemCount * sizeof(int))) == NULL)
    {
      if (pErr)
        *pErr = ENOMEMORY;
      ret =  EFAILED;
      break;
    }

    // Determine field data sizes 
    for (i = 0; i < nItemCount; i++)
    {
      AEEAddrFieldID fieldID = OEMAddr_PBMFieldToAEE(fields_buf[i].field_id);
      // If PBM_FT_STRING or PBM_FT_PHONE, need to convert to AECHAR.  
      if ( (fields_buf[i].field_type == PBM_FT_STRING) ||
           (fields_buf[i].field_type == PBM_FT_PHONE) )
      {
        dataSizes[i] = fields_buf[i].data_len * sizeof(AECHAR);
      }
      else if(fieldID == AEE_ADDRFIELD_COMPLEX)
      {
        AEEAddrFieldComplex *complexAddrField = NULL;
        dataSizes[i] = OEMAddr_GetComplexField(&complexAddrField,pbm_get_field_data_ptr(fields_buf,i));
        if(complexAddrField != NULL){
           AEEAddrFieldComplex_Dtor(complexAddrField);
           complexAddrField = NULL;
        }
      }
      else
      {
        dataSizes[i] = fields_buf[i].data_len;
      }
    }

    if (pb_id == PBM_SDL)
      dataSizes[i] = sizeof(uint16);

    if (!allocate_fields(ppItems, *pnItemCount, dataSizes))
    {
      FREEIF(dataSizes);
      if (pErr)
        *pErr = ENOMEMORY;
      ret =  EFAILED;
      break;
    }

    // Copy field structure info into MALLOCed area
    for (i = 0; i < nItemCount; i++)
    {
      (*ppItems)[i].fID = OEMAddr_PBMFieldToAEE(fields_buf[i].field_id);
      (*ppItems)[i].fType = OEMAddr_PBMFieldTypeToAEE(fields_buf[i].field_type);
      (*ppItems)[i].wDataLen = (uint16)dataSizes[i];

      if((*ppItems)[i].fID == AEE_ADDRFIELD_INDEX)
      {
        (*ppItems)[i].fID = AEE_ADDRFIELD_INDEX;
        (*ppItems)[i].fType = AEEDB_FT_WORD;
        (*ppItems)[i].wDataLen = sizeof(uint16);
        *((uint16 *)((*ppItems)[i].pBuffer)) = 
                                  (uint16)pbm_record_id_to_location(recID);
      }
      else if ((fields_buf[i].field_type == PBM_FT_STRING)  ||
               (fields_buf[i].field_type == PBM_FT_PHONE))
      {
        // If PBM_FT_STRING or PBM_FT_PHONE, need to convert to AECHAR (UCS2). 
        if (!pbmconvert_pbm8_to_ucs2(
                                (uint8 *) pbm_get_field_data_ptr(fields_buf,i),
                                (uint16) fields_buf[i].data_len - 1,
                                (uint16 *) (*ppItems)[i].pBuffer,
                                (uint16) dataSizes[i]/2))
        {
          MSG_ERROR("String from PBM not completely converted to UCS2.  %d %d",
                    fields_buf[i].data_len, dataSizes[i], 0);
        }
      }
      else if((*ppItems)[i].fID == AEE_ADDRFIELD_COMPLEX)
      {
        AEEAddrFieldComplex *complexAddrField = NULL;
        (void) OEMAddr_GetComplexField(&complexAddrField,pbm_get_field_data_ptr(fields_buf,i));
        if(complexAddrField != NULL){
           if ((*ppItems)[i].pBuffer != NULL) {
              FREE((*ppItems)[i].pBuffer);
           }
           (*ppItems)[i].pBuffer = (void*) complexAddrField;
           complexAddrField = NULL; // No _Dtor
        }
      }
      else if (((*ppItems)[i].fType == AEEDB_FT_WORD) &&
               ((*ppItems)[i].fID == AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE))
      {
        /* Convert primary phone field ID data from addrbook field ID to PBM field ID */
        *((uint16 *)((*ppItems)[i].pBuffer)) = 
          (uint16)OEMAddr_PBMFieldToAEE(*((pbm_field_id_e_type*)pbm_get_field_data_ptr(fields_buf,i)));
      }
      else
      {
        (void) MEMCPY( (*ppItems)[i].pBuffer, 
                pbm_get_field_data_ptr(fields_buf,i), 
                (uint32) dataSizes[i] );
      }
    }

    if (pb_id == PBM_SDL) {
      (*ppItems)[i].fID = AEE_ADDRFIELD_INDEX;
      (*ppItems)[i].fType = AEEDB_FT_WORD;
      (*ppItems)[i].wDataLen = sizeof(uint16);
      *((uint16 *)((*ppItems)[i].pBuffer)) = 
                                (uint16)pbm_record_id_to_location(recID);
    }
 
  } while (0);
  
  FREEIF(dataSizes);
  FREEIF(fields_buf);

  return ret;
}

/*========================================================================

Function: OEMAddr_RecordDelete()

Description:
   This function deletes a record of the specified record ID from the address book.

Prototype:
   int   OEMAddr_RecordDelete(uint16 recID)

Parameters:
   recID: [in]: ID of the record to be deleted from the address book.

Return Value:
   AEE_SUCCESS: If the record was successfully deleted
   EFAILED: if failed

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
int OEMAddr_RecordDelete(uint16 wrecID)
{
  pbm_return_type pbmRet;
  pbm_device_type pb_id;
  OEMADDR_CB_DATA_STRUCT *pbmRet_callback;

  MSG_HIGH("OEMAddr_RecordDelete: wrecID %d", wrecID, 0, 0);

  if (wrecID == AEE_ADDR_RECID_NULL)
  {
    return EFAILED;
  }

  pb_id = pbm_record_id_to_phonebook(wrecID);  

  pbmRet_callback = MALLOC((uint32) sizeof(*pbmRet_callback));
  if (pbmRet_callback == NULL)
  {
    ERR("Could not allocate %d bytes.", sizeof(*pbmRet_callback), 0, 0);
    return EFAILED;
  }

  ClearBlockUITask();
  pbmRet = pbm_record_write(pb_id, &wrecID, 0, 0, NULL, 0,
                            OEMAddr_RecordWrite_PBM_CB, pbmRet_callback);

  if (pbmRet != PBM_SUCCESS)
  {
    ERR("pbm_write() failed with error %d.", pbmRet, 0, 0);
    FREEIF(pbmRet_callback);
    return pbm_error_to_aee(pbmRet);
  }

  BlockUITask();

  if (pbmRet_callback->ret != PBM_SUCCESS)
  {
    ERR("pbm_write() failed with error %d.", pbmRet_callback->ret, 0, 0);
    FREEIF(pbmRet_callback);
    return pbm_error_to_aee(pbmRet);
  }

  FREEIF(pbmRet_callback);
  return AEE_SUCCESS;
}


/*========================================================================

Function: OEMAddr_GetNumRecs()

Description:
   This function returns the total number of records found in the address book.

Prototype:
   uint16   OEMAddr_GetNumRecs(void);

Parameters:
   None

Return Value:
   Count indicating the total number of records currently found in the address book.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
uint16   OEMAddr_GetNumRecs(void)
{
  uint16 ret_val = pbm_get_num_recs(PBM_DEFAULT);

  //Get Count from PBM.  PBM_DEFAULT indicates both EFS, SDL, and ADN phonebooks
  MSG_HIGH("OEMAddr_GetNumRecs returning %d", ret_val, 0, 0);
  return ( ret_val );
}

/*========================================================================
Function: OEMAddr_RemoveAllRecs(void)

Description:
   This function deletes all records from the address book.

Prototype:
   int   OEMAddr_RemoveAllRecs(void)

Parameters:
   None

Return Value:
   AEE_SUCCESS: If all the records were deleted
   EUNSUPPORTED: if this function is not supported
   EFAILED: if failed. 

Comments:
   Since this is a sensitive operaiton, OEMs can decide not to support it. If this
   function is not supported, the value EUNSUPPORTED must be returned from this function.

Side Effects:
   None

See Also:
   None

========================================================================*/
int OEMAddr_RemoveAllRecs(void)
{
  // Include SIM card and EFS and Speed Dial
  MSG_HIGH("OEMAddr_RemoveAllRecs: returning EUNSUPPORTED", 0, 0, 0);
  return EUNSUPPORTED;
}


/*========================================================================
Function: OEMAddr_EnumRecInit()

Description:
   This function is used for searching the address book for specific records. It is
   also used for sequentially retrieving all the records in the database.
   This fundtion initializes the enumeration of records in the address book
   based on a specific search criteria.Once enumeration has been initialized, the function
   OEMAddr_EnumNextRec() will be used to iterate through the records that match
   this search criteria.

Prototype:
   int   OEMAddr_EnumRecInit(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize)

Parameters:
   wCategory: [in]: Category type to be matched. If this is set to AEE_ADDR_CAT_NONE, 
   it should be ignored.

   wFieldID: [in]: AEEAddrFieldID to be matched. If this is set to AEE_ADDRFIELD_NONE, 
   it should be ignored.
   It is likely that the OEMs do not allow searching for records on this field ID (ex: searching
   for records based on EMAIL may not be allowed). In this case, return EFAILED. 
   and IADDRBOOK_EnumNextFieldsInfo().
   
   pData: [in]: if non-null, it specifies the actual data that must be matched. 
   If NULL, it is ignored. For ex: if wFieldID is set to AEE_ADDRFIELD_NAME, pData will contain
   the actual name to be matched.  If wCategory is set to AEE_ADDRCAT_ENHANCED_ENUM
   and wFieldId is set to AEE_ADDRFIELD_ENHANCED_ENUM, then this field points
   to an AEEAddrEnumRecInfo structure.

   
   wDataSize: [in]: Specifies the size of pData

Return Value:
   AEE_SUCCESS: If enumeration was successfully initialized.
   EFAILED: Enumeration failed. 

Comments:
   This function can also be used to enumerate through all
   the records in the database by specifying AEE_ADDR_CAT_NONE for the category parameter and
   AEE_ADDRFIELD_NONE for the field paramter.

    When wCategory is set to AEE_ADDRCAT_ENHANCED_ENUM and wFieldId is set to
    AEE_ADDRFIELD_ENHANCED_ENUM, then pData points to an AEEAddrEnumRecInfo
    structure.  This allows the caller to specify a number of search match
    criteria for the record enumeration, including:
    - A list of categories to search
    - A list of fields to search
    - A list of data types to search
    - Specific data to match
    - Data match criteria such as case sensitivity and exact or partial string matches.


Side Effects:
   None

See Also:
   OEMAddr_EnumNextRec

========================================================================*/
int OEMAddr_EnumRecInit(AEEAddrCat wCategory, 
                        AEEAddrFieldID wFieldID, 
                        void *pData, 
                        uint16 wDataSize)
{
  return OEMAddr_EnumRecInitEx(wCategory, wFieldID, pData, wDataSize, 0);
}

/*========================================================================
Function: OEMAddr_EnumRecInitEx()

Description:
   This function is used for searching the address book for specific records. It is
   also used for sequentially retrieving all the records in the database.
   This fundtion initializes the enumeration of records in the address book
   based on a specific search criteria.Once enumeration has been initialized, the function
   OEMAddr_EnumNextRec() will be used to iterate through the records that match
   this search criteria.

Prototype:
   int   OEMAddr_EnumRecInitEx(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize, uint32 flags)

Parameters:
   wCategory: [in]: Category type to be matched. If this is set to AEE_ADDR_CAT_NONE, 
   it should be ignored.

   wFieldID: [in]: AEEAddrFieldID to be matched. If this is set to AEE_ADDRFIELD_NONE, 
   it should be ignored.
   It is likely that the OEMs do not allow searching for records on this field ID (ex: searching
   for records based on EMAIL may not be allowed). In this case, return EFAILED. 
   and IADDRBOOK_EnumNextFieldsInfo().
   
   pData: [in]: if non-null, it specifies the actual data that must be matched. 
   If NULL, it is ignored. For ex: if wFieldID is set to AEE_ADDRFIELD_NAME, pData will contain
   the actual name to be matched.
   
   wDataSize: [in]: Specifies the size of pData

   flags: Specify the type of search to be performed. 
          Default (0x0) is case insensitive, starts with, string match
          with FIELD_NONE. 
          Bit 0:    0   - ADDRREC_CASE_INSENSITIVE
          Bit 0:    1   - ADDRREC_CASE_SENSITIVE
          Bit 1-2:  0   - ADDRREC_SEARCHTYPE_STARTSWITH
          Bit 1-2:  1   - ADDRREC_SEARCHTYPE_EXACT
          Bit 1-2:  10  - ADDRREC_SEARCHTYPE_CONTAINS
          Bit 3:    0   - ADDRREC_DATATYPE_STR
          Bit 3:    1   - ADDRREC_DATATYPE_BYTE
          Bit 4:    0   - ADDRREC_FIELDID_NONE
          Bit 4:    1   - ADDRREC_FIELDID_PHONE (Any type of phone)

Return Value:
   AEE_SUCCESS: If enumeration was successfully initialized.
   EFAILED: Enumeration failed. 

Comments:
   This function can also be used to enumerate through all
   the records in the database by specifying AEE_ADDR_CAT_NONE for the category parameter and
   AEE_ADDRFIELD_NONE for the field paramter.


Side Effects:
   None

See Also:
   OEMAddr_EnumNextRec

========================================================================*/
int OEMAddr_EnumRecInitEx(AEEAddrCat wCategory, 
                        AEEAddrFieldID wFieldID, 
                        void *pData, 
                        uint16 wDataSize,
                        uint32 flags)
{
  pbm_return_type pbmRet;
  pbm_device_type pb_id;
  pbm_cat_e_type pb_cat;
  void *pDataSrch = pData;
  uint16 wDataSizeSrch = wDataSize;
  uint32 pbm_flags = 0;

  MSG_HIGH("OEMAddr_EnumRecInitEx: Cat %d Field %d Flags %x", wCategory, wFieldID, flags);
  //Free data from previous enum

  gbEnumInit = TRUE;
   
  if (wCategory == AEE_ADDR_CAT_NONE)
  {
    pb_id = PBM_DEFAULT; // ???? How else would we show multiple phonebooks?
    pb_cat = PBM_CAT_NONE;
  }
  else
  {
    pb_id = cat_to_phonebook(wCategory);
    pb_cat = cat_to_pbm_cat(wCategory);
  }

  /* Or in the flags that are bitwise eqivalent. */
  pbm_flags = (flags & (ADDRREC_CASE_MASK |
                        ADDRREC_SEARCHTYPE_MASK |
                        ADDRREC_DATATYPE_MASK |
                        ADDRREC_FIELDID_MASK |
                        ADDRREC_ENUMERATE_MASK));

  /* Add in the fact we are UCS2 aware. */
  pbm_flags |= PBM_STRTYPE_UCS2;

  pbmRet = pbm_enum_rec_init(pb_id, (int)pb_cat, OEMAddr_AEEFieldToPBM(wFieldID), 
                                        pDataSrch, wDataSizeSrch, pbm_flags);

  if (pbmRet != PBM_SUCCESS)
  {
    ERR("pbm_enum_rec_init() failed with error code %d.", pbmRet, 0, 0);
    return pbm_error_to_aee(pbmRet);
  }

  return AEE_SUCCESS;
}

/*========================================================================
Function: OEMAddr_EnumNextRec()

Description:
   This function returns the information about the next record based on the search criteria
   specified in most recent call to OEMAddr_EnumRecInit()

Prototype:
   uint16   OEMAddr_EnumNextRec(AEEAddrCat *cat,AEEAddrField ** ppItems, int *nItemCount,int *pErr);

Parameters:
   cat: [in/out]: On return, if the next record was found, this contains the address category of
   that next record.

   ppItems: [in/out]: On return, if the next record was found, *ppItems contains the 
   list of address fields found in that next record.
   
   nItemCount: [in/out]: On return, if the next record was found, *nItemCount contains the 
   number of address fields found in that next record.

  *pErr: [in/put: On return, contains the error code if an error occurred.


Return Value:
   If next record was successfully found, this function must return the recordID of the newly 
   found record. Also, it must fill up the incoming parameters with the contents of the
   newly found record. 
   If the end of the enumeration has been reached or no more records are found, 
   this function must return AEE_ADDR_RECID_NULL. .

Comments:
   Once the end of the enumeration has been reached,
   the index must NOT be reset to point to the begining of the enumeration. This means, once
   the end has been reached, all subsequent calls to this function will continue to return
   AEE_ADDR_RECID_NULL. The caller must call OEMAddr_EnumRecInit() to re-initialize the
   search criteria

Side Effects:
   None

See Also:
   OEMAddr_EnumRecInit

============================================================================= */
uint16 OEMAddr_EnumNextRec(AEEAddrCat *pcat, 
                           AEEAddrField ** ppItems, 
                           int *pnItemCount, 
                           int *pErr)
{
  pbm_return_type pbmRet;
  int i;
  int nItemCount;

  uint16 rec_id;
  pbm_field_s_type      *fields_buf = NULL;
  uint32                 fields_size;
  int *dataSizes = NULL;
  boolean fail = FALSE;

  /************************************/
  MSG_HIGH("OEMAddr_EnumNextRec: %x", gbEnumInit, 0, 0);

  *pErr = AEE_SUCCESS;

  //Check if Enum initialized 
  if(!gbEnumInit)
    return(AEE_ADDR_RECID_NULL);

  if(!pcat || !ppItems || !pnItemCount)
  {
    if(pErr)
      *pErr = EBADPARM;
    return(AEE_ADDR_RECID_NULL);
  }

  //Get the next record that matches the given search criteria
  do
  {
    pbmRet = pbm_enum_next_rec_id(&rec_id);

    if (pbmRet != PBM_SUCCESS)
    {
      if ((pbmRet != PBM_EOF) && (pbmRet != PBM_ERROR_NOT_FOUND))
        ERR("pbm_enum_next_rec_id() failed with error code %d.", pbmRet, 0, 0);
      if (pErr)
        *pErr = pbm_error_to_aee(pbmRet);

      fail = TRUE;
      break;
    }

    // If no records left 
    if (rec_id == AEE_DB_RECID_NULL)
    {
      gbEnumInit = FALSE;
      if (pErr)
        *pErr = pbm_error_to_aee(pbmRet);

      // no more records
      fail = TRUE;
      break;
    }

    //read the actual record
    fields_size = (uint32)pbm_calculate_fields_size_from_id(rec_id);
    fields_buf = MALLOC(fields_size);

    if (fields_buf == NULL)
    {
      if (pErr)
        *pErr = ENOMEMORY;
      fail = TRUE;
      break;
    }

    pbmRet = pbm_record_read(rec_id, pcat, pnItemCount, (uint8*)fields_buf, fields_size);
    if (pbmRet != PBM_SUCCESS)
    {
      ERR("pbm_enum_next_rec_id() failed with error code %d.", pbmRet, 0, 0);
      if (pErr)
        *pErr = pbm_error_to_aee(pbmRet);

      fail = TRUE;
      break;
    }

    /* For SDL phonebook, we need to create a new field with index obtained
      from the record id */
    if (*pcat == (int)PBM_CAT_SDL)
      nItemCount = (*pnItemCount)++;
    else
      nItemCount = *pnItemCount;

    if ( ( dataSizes = MALLOC((uint32) *pnItemCount * sizeof(int)) ) == NULL )
    {
      if (pErr)
        *pErr = ENOMEMORY;

      fail = TRUE;
      break;
    }

    // Determine field data sizes 
    for (i = 0; i < nItemCount; i++)
    {
      // If PBM_FT_STRING or PBM_FT_PHONE, need to convert to AECHAR.  
      if ( (fields_buf[i].field_type == PBM_FT_STRING) ||
           (fields_buf[i].field_type == PBM_FT_PHONE) )
      {
        dataSizes[i] = fields_buf[i].data_len * sizeof(AECHAR);
      }
      else
      {
        dataSizes[i] = fields_buf[i].data_len;
      }
    }

    if (*pcat == (int)PBM_CAT_SDL)
      dataSizes[i] = sizeof(uint16);

    if (!allocate_fields(ppItems, *pnItemCount, dataSizes))
    {
      if (pErr)
        *pErr = ENOMEMORY;
      fail = TRUE;
      break;
    }

    // Copy field structure info into MALLOCed area
    for (i = 0; i < nItemCount; i++)
    {
      (*ppItems)[i].fID = OEMAddr_PBMFieldToAEE(fields_buf[i].field_id);
      (*ppItems)[i].fType = OEMAddr_PBMFieldTypeToAEE(fields_buf[i].field_type);
      (*ppItems)[i].wDataLen = (uint16)dataSizes[i];

      // If PBM_FT_STRING or PBM_FT_PHONE, need to convert to AECHAR.  This is
      // the same as UCS2.
      if ((fields_buf[i].field_type == PBM_FT_STRING) ||
          (fields_buf[i].field_type == PBM_FT_PHONE))
      {
        if (!pbmconvert_pbm8_to_ucs2(
                                 (uint8 *) pbm_get_field_data_ptr(fields_buf,i),
                                 (uint16) fields_buf[i].data_len - 1,
                                 (uint16 *) (*ppItems)[i].pBuffer,
                                 (uint16) dataSizes[i]/2))
        {
          MSG_ERROR("String from PBM not completely converted to UCS2.  %d %d",
                    fields_buf[i].data_len, dataSizes[i], 0);
        }
      }
      else if (((*ppItems)[i].fType == AEEDB_FT_WORD) &&
               ((*ppItems)[i].fID == AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE))
      {
        /* Convert primary phone field ID data from addrbook field ID to PBM field ID */
        *((uint16 *)((*ppItems)[i].pBuffer)) = 
          (uint16)OEMAddr_PBMFieldToAEE(*((pbm_field_id_e_type*)pbm_get_field_data_ptr(fields_buf,i)));
      }
      else
      {
        (void) MEMCPY( (*ppItems)[i].pBuffer, 
                       pbm_get_field_data_ptr(fields_buf,i), 
                       (uint32) dataSizes[i] );
      }
    }

    if (*pcat == (int)PBM_CAT_SDL)
    {
      (*ppItems)[i].fID = AEE_ADDRFIELD_INDEX;
      (*ppItems)[i].fType = AEEDB_FT_WORD;
      (*ppItems)[i].wDataLen = sizeof(uint16);
      *((uint16 *)((*ppItems)[i].pBuffer)) =
      (uint16)pbm_record_id_to_location(rec_id);
    }

    FREEIF(dataSizes);

    if ( AEE_SUCCESS != *pErr )
    {
      fail = TRUE;
      break;
    }
  } while (0);

  FREEIF(dataSizes);
  FREEIF(fields_buf);

  if (fail)
  {
    return AEE_ADDR_RECID_NULL;
  }
  else 
  {
    return rec_id;
  }
}


/*========================================================================
Function: OEMAddrBook_GetCatCount()

Description:
   This function returns the number of address-categories supported by the
   address book. Examples of address-categories are: PERSONAL, BUSINESS, etc.
   Each record in the address book will (may) belong to a specific address category.
   If the concept of categories are not supported in the address book, this function
   must return 0.

Prototype:
   int   OEMAddr_GetCatCount(void)

Parameters:
   None

Return Value:
   Number of categories supported

Comments:
   Its perfectly valid to return 0 to this function if the address book does not
   support the concept of categories for each record

Side Effects:
   None

See Also:
   None

========================================================================*/
int   OEMAddr_GetCatCount(void)
{
  MSG_HIGH("OEMAddr_GetCatCount returning %d", OEM_ADDRESS_CATEGORY_COUNT, 0, 0);
 
  return(OEM_ADDRESS_CATEGORY_COUNT);
}


/*========================================================================
Function: OEMAddr_GetCatList()

Description:
   This function returns information about all the address categories supported
   by the handset address book. This function is called only if OEMAddr_GetCatCount()
   returned a value other than 0.

Prototype:
   int   OEMAddr_GetCatList(AEEAddrCat *p, int nSize);

Parameters:
   p: [in/out]: Pointer (allocated by the caller) that can hold information about the
   address categories.
   nSize: Numnber of AEEAddrCat elements that can fit into the array pointed to by p.

Return Value:
   AEE_SUCCESS: If successful. Even if nSize is less than the total number of categories
   supported, this function must return AEE_SUCCESS as long as nSize > 0.

   EFAILED: if failed

Comments:
   The categories must be converted from OEM list to the AEE values before returning. A list
   of pre-defined AEEAddressCategories can be found in AEEAddrBook.h. OEMs can add their
   own categories too.

Side Effects:
   None

See Also:
   None

========================================================================*/
int   OEMAddr_GetCatList(AEEAddrCat *p, int nSize)
{
  int i = 0;

  if(!p || !nSize)
    return(EFAILED);

#define OEMADDR_SET_AND_CHECK_FOR_END(cat)  \
  p[i++] = (cat);                             \
  if (i >= nSize)                           \
    return AEE_SUCCESS;
  
  OEMADDR_SET_AND_CHECK_FOR_END(AEE_ADDR_CAT_PERSONAL);
  OEMADDR_SET_AND_CHECK_FOR_END(AEE_ADDR_CAT_BUSINESS);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SPEEDDIAL);
#ifdef FEATURE_MMGSDI
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_ADN);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_ADN2);
#endif
  
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_FDN);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_SDN);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MSISDN);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MBDN);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MBN);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_FDN2);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_SDN2);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MSISDN2);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MBDN2);
  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_SIM_MBN2);
#endif
#endif /* FEATURE_MMGSDI */

  OEMADDR_SET_AND_CHECK_FOR_END((int)AEE_ADDR_CAT_ECC); // read only emergency numbers
  
  
  MSG_HIGH("OEMAddr_GetCatList returned %d of %d categories",
           i, nSize, 0);
  return(AEE_SUCCESS);
}


/*========================================================================
Function: OEMAddr_GetFieldInfoCount()

Description:
   This function returns the number of types of fields supported for the give category.
   If the concept of categories is not supported, this function must just
   return the total number of types of fields supported for each record in the address book.
   Examples of fields are: NAME, WORK_NUM, FAX_NUM, URL, ADDRESS, etc.

Prototype:
   int   OEMAddr_GetFieldInfoCount(AEEAddrCat c)

Parameters:
   c: [in]: Address Category for which the number of fields supported is to be
   returned.

Return Value:
   Number of types of fields supported for the given address category.

Comments:
   None

Side Effects:
   None

See Also:
   OEMAddr_GetFieldInfo()

========================================================================*/
int   OEMAddr_GetFieldInfoCount(AEEAddrCat c)
{
  pbm_return_type pbmRet;
  int num;
  pbm_device_type pb_id;
  pbm_cat_e_type pb_cat;

  if (c == AEE_ADDR_CAT_NONE)
  {
    pb_id = PBM_DEFAULT; // ???? How else would we show multiple phonebooks?
    pb_cat = PBM_CAT_NONE;
  }
  else
  {
    pb_id = cat_to_phonebook(c);
    pb_cat = cat_to_pbm_cat(c);
  }

  pbmRet = pbm_get_field_info_count(pb_id,pb_cat,&num);
  if(pbmRet != PBM_SUCCESS)
  {
    ERR("Could not get field Count pb_id %d pb_cat %d",pb_id,pb_cat,0);
    return 0;
  }
  MSG_HIGH("OEMAddr_GetFieldInfoCount: Category %d pb_id %d Return %d", c, pb_id, num);
  return num;
}

/*========================================================================

Function: OEMAddr_SetProperties()

Description:
   This function sets the properties for the address book.

Prototype:
   void OEMAddr_SetProperties(uint32 dwProps);

Parameters:
   dwProps: [in]: Properties

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
void OEMAddr_SetProperties(uint32 dwProps)
{
  MSG_HIGH("Unsupported OEMAddr_SetProperties %d", dwProps, 0, 0);
  gwProps = dwProps;
}

/*========================================================================

Function: OEMAddr_GetProperties()

Description:
   This function gets the properties for the address book.

Prototype:
   uint32 OEMAddr_GetProperties();

Parameters:
   None
   
Return Value:
   Properties

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================*/
uint32 OEMAddr_GetProperties(void)
{
  MSG_HIGH("Unsupported OEMAddr_GetProperties %d", gwProps, 0, 0);
  return gwProps;
}

/*========================================================================
Function: OEMAddr_GetFieldInfo()

Description:
   This function returns detiled information about each field-type supported
   for the given category. This function is generally invoked righ after the
   function OEMAddr_GetFieldInfoCount().

Prototype:
   int   OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize)

Parameters:
   c: [in]: Address Category for which the field info is to be returned
   pf: [in/size]: Pointer to an arry of AEEAddrFieldInfo structs (allocated by caller)
   where information needs to be returned by this function
   nSize: [in]: Number of AEEAddrFieldInfo elements that can fit into the array pointed
   to by pf

Return Value:
   AEE_SUCCESS: If this function was able to successfuly send information about the fields.
   Even if nSize is less than the total number of fields supported, 
   this function must return AEE_SUCCESS as long as nSize > 0.
   EFAILED: If failed.

Comments:
   AEEAddrFieldInfo contains detailed information about the fields. It contains information
   such as FieldID, maximum number of fields of this ID supported in each record, etc.
   Detailed information of this structure can be found in AEEAddrBook.h and in the
   BREW API Reference Guide.

Side Effects:
   None

See Also:
   None

========================================================================*/
int OEMAddr_GetFieldInfo(AEEAddrCat c, AEEAddrFieldInfo *pf, int nSize)
{
  pbm_device_type pb_id;
  pbm_cat_e_type pb_cat;
  pbm_return_type pbmRet;
  pbm_field_info_s_type *pbmFieldInfo;
  int size; 
  int i;

  if(pf == NULL)
  {
    MSG_ERROR("AddrField Info is NULL",0,0,0);
    return EFAILED;
  }

  MSG_HIGH("OEMAddr_GetFieldInfo: Cat %d size %d", c, nSize, 0);
  if (c == AEE_ADDR_CAT_NONE)
  {
    pb_id = PBM_DEFAULT; // ???? How else would we show multiple phonebooks?
    pb_cat = PBM_CAT_NONE;
  }
  else
  {
    pb_id = cat_to_phonebook(c);
    pb_cat = cat_to_pbm_cat(c);
  }

  //Allocate fieldInfo
  size = nSize * (int)sizeof(pbm_field_info_s_type);
  pbmFieldInfo = MALLOC((uint32)size);
  pbmRet = pbm_get_field_info(pb_id,pb_cat,pbmFieldInfo,size);

  if(pbmRet != PBM_SUCCESS)
  {
     ERR("pbm_get_field_info() failed with error code %d.", pbmRet, 0, 0);
     FREEIF(pbmFieldInfo);
     return pbm_error_to_aee(pbmRet);
  }

  //convert to AEEAddrFieldInfo
  for (i = 0; i < nSize; i++)
  {
    pf[i].fieldID = OEMAddr_PBMFieldToAEE(pbmFieldInfo[i].field_id);
    pf[i].nMaxItems = pbmFieldInfo[i].num_items;           /* Maximum number within a record */
    pf[i].fType = OEMAddr_PBMFieldTypeToAEE(pbmFieldInfo[i].field_type);              /* Field type */
    pf[i].bFieldSearch = pbmFieldInfo[i].field_search;     /* Allow search for data? */
    pf[i].bFieldCatSearch= pbmFieldInfo[i].field_cat_search;/* -"- within specified category? */ 
  }

  FREEIF(pbmFieldInfo);
  return AEE_SUCCESS;

}

/*======================================================================= 
Function: OEMAddr_GetCategoryName()

Description: 
   This method is used to get the AECHAR string name of a category.

Prototype:
   int   OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize);
   
Parameters:
   c: [in]: Address category for which the name in string format is returned.
   pszName: [in/out]: On input this is the buffer where the category name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.  On return, the buffer will contain the category name.
   pnSize: [in/out]: On input contains the size of pszName. On return, it 
   contains the size filled.

Return Value: 
   AEE_SUCCESS: If successful
   EBADPARM:    If bad parameters are passed in
   AEE_ADDR_CAT_NOT_SUPPORTED: If the category is not supported.
   AEE_ADDRBOOK_NOCATSUPPORT:  If the device does not support categories.

Comments:  
   None

Side Effects: 
   None

See Also: 
   OEMAddr_GetCatCount
   OEMAddr_GetCatList
   OEMAddr_GetFieldName   
=======================================================================*/
static int OEMAddr_GetCategoryName(AEEAddrCat c, AECHAR *pszName, int *pnSize)
{
   int i;
   int n;
   char *psz = NULL;

   if(!pnSize)
      return EBADPARM;

   MSG_HIGH("OEMAddr_GetCategoryName: Cat %d", c, 0, 0);
   // Search for the passed category
   for(i = 0; i < OEM_ADDRESS_CATEGORY_COUNT; i++)
   {
      if(gSupportedCatList[i].cat == c)
      {
         psz = gSupportedCatList[i].pszName;
         break;
      }
   }

   // If found, determine the name or buffer size required.
   if(psz)
   {
      n = (int)(sizeof(AECHAR)*(STRLEN(psz) + 1));
      if( pszName )
      {
         *pnSize = MIN(*pnSize, n);
         (void)STRTOWSTR(psz, pszName, *pnSize);
      }
      else
      {
         *pnSize = n;
      }
      return AEE_SUCCESS;
   }

   // If we fall through to here, the category was not found.
   return AEE_ADDR_CAT_NOT_SUPPORTED;
}

/*======================================================================= 
Function: OEMAddr_GetFieldName()

Description: 
   This method is used to get the AECHAR string name of a field.

Prototype:
   int OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
   
Parameters:
   f: [in]: AEEAddrFieldID for which the name in string format is returned.
   pszName: [in/out]: On input this is the buffer where the field name will
   be placed.  If NULL, this indicates the caller is querying the size.
   required.
   pnSize: [in/out]: On input contains the size of pszName. On return, it 
   contains the size filled.

Return Value: 
   AEE_SUCCESS: If successful
   EBADPARM:    If bad parameters are passed in
   AEE_ADDR_INVALID_FIELD_ID: If the field is not supported.

Comments:  
   None

Side Effects: 
   None

See Also: 
   OEMAddr_GetFieldInfoCount
   OEMAddr_GetFieldInfo
   OEMAddr_GetCategoryName
=======================================================================*/
static int OEMAddr_GetFieldName(AEEAddrFieldID f, AECHAR *pszFieldName, int *pnSize)
{
   int i;
   int n;
   char *psz = NULL;

   if(!pnSize)
      return EBADPARM;

   MSG_HIGH("OEMAddr_GetFieldName: Field %d", f, 0, 0);
   // Search for the passed field. 
   for(i = 0; i < (int)OEM_ADDRESS_NAMED_FIELDS_COUNT ; i++)
   {
      if(gSupportedFieldList[i].fieldID == f)
      {
         psz = gSupportedFieldList[i].pszName;
         break;
      }
   }

   // If found, determine the name or buffer size required.
   if(psz)
   {
      n = (int)(sizeof(AECHAR)*(STRLEN(psz) + 1));
      if( pszFieldName )
      {
         *pnSize = MIN(*pnSize, n);
         (void)STRTOWSTR(psz, pszFieldName, *pnSize);
      }
      else
      {
         *pnSize = n;
      }
      return AEE_SUCCESS;
   }

   // If we fall through to here, the field was not found.
   return AEE_ADDR_INVALID_FIELD_ID;
}

/*===========================================================================
FUNCTION OEMAddr_HaveUIDsChanged

DESCRIPTION
  This function is used to see if the phone Unique ID's have changed.
  If the unique ID changes aren't validated (eg if EFS was wiped), writes 
  will fail until someone validates the unique ID's. 
  (If FEATURE_PBM_UNIQUE_ID is on)

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  OEMAddr_ValidateUIDChanges
===========================================================================*/
boolean OEMAddr_HaveUIDsChanged(void)
{
  return pbm_have_uids_changed();
}

/*===========================================================================
FUNCTION OEMAddr_ValidateUIDChanges

DESCRIPTION
  This function is used to acknowledge that the unique IDs have changed.
  If the unique ID changes aren't validated (eg if EFS was wiped), writes 
  will fail until someone validates the unique ID's. 
  (If FEATURE_PBM_UNIQUE_ID is on)

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  OEMAddr_HaveUIDsChanged
===========================================================================*/
void OEMAddr_ValidateUIDChanges(void)
{
  pbm_return_type ret_val;

  ret_val = pbm_validate_uid_changes();
  if (PBM_SUCCESS != ret_val) {
    MSG_ERROR("pbm_validate_uid_changes failed, code %d",ret_val,0,0);
  }
}


