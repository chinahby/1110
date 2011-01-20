#ifndef PBMLIB_H
#define PBMLIB_H

/*===========================================================================

           INTERNAL HEADER FILE FOR PHONEBOOK MANAGER LIBRARY

DESCRIPTION
  This is the internal header file for Phonebook Manager library.

 Copyright (c) 2002 - 2007 by QUALCOMM INCORPORATED  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //depot/asic/msmshared/services/pbm/pbmlib.h#45 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
12/26/08   kcp     adding changes for ASUS CR 165600
03/14/08   clm     Add PBM_EVENT_SIM_INIT_DONE event.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
03/16/07   cvs     Add support for error category
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add event for phonebooks becoming ready
11/01/06   cvs     multi pb set fixes
10/03/06   cvs     Add support for MBN
04/05/06   cvs     Emergency category priority fixes
03/21/06   cvs     Add function to get extended phonebook info
03/06/06   cvs     Add new fields
02/28/06   cvs     Add support for MBDN
01/19/06   cvs     Add pbm_emergency_number_cat(), cleanup emergency nums
11/16/05   cvs     Lint fixes
11/10/05   cvs     Add network ECC numbers
10/18/05   cvs     Add new fields
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
08/12/05   AT      Added UCS2 support for strings.
08/01/05   cvs     rename field
07/08/05   cvs     Add new fields
06/29/05   AT      Added error codes, fixed typos, moved ECC field IDs to
                   upper numbers.
06/03/05   cvs     Multiproc cleanup, add metacomments
05/26/05   cvs     API changes to make APIs more multiprocessor friendly
04/08/05   cvs     Add fields
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/23/05   cvs     compilation fixes
03/08/05   PA      Support for PBM notifications
01/20/05   PA      Support for multiple lock types, greater than enumeration,
                   support for MSISDN.
01/24/05   PA      Changed NV_EMERGENCY_NUMBER_START to
                   PBM_NV_EMERGENCY_NUMBER_START
01/20/05   PA      Expose definitions required by ui_base
01/12/05   PA      PBM USIM Support
01/10/05   PA      Added distribution list field.
01/10/05   PA      Added definition for ui_emergency_number
12/16/04   SUN     Added a new PBM field
12/15/04   SUN     Added new Address Fields
12/08/04   PA      Added PBM_FIELD_DIAL_STRING field.
12/03/04   PA      PBM record validation APIs
11/18/04   PA      Support for complex fields and write lock
11/02/04   PA      Support for SDN
09/29/04   PA      Initial revision based on uiupbm.h
===========================================================================*/

#include "cm.h" /* For CM_TON_* and CM_NPI_* */
#include "nv.h" /* For NV_MAX_DIAL_DIGITS */

#define PBM_HARDPAUSE_CHAR      'H'
#define PBM_TIMEPAUSE_CHAR      'T'
#define PBM_WILD_CHAR           '?'
#define PBM_EXPANSION_CHAR      'e'

#define PBM_MAX_NUM_LENGTH                40
#define PBM_MAX_NUM_LENGTH_NO_EXT_RECORD  20 /* max number length if there */
                                             /* is no extension record available */
#define PBM_MAX_TEXT_LENGTH               CM_MAX_ALPHA_LEN_FOR_DISPLAY_INFO

#define PBM_GET_FIRST_AVAILABLE 0xFFFF /* Get first available location */
#define PBM_INVALID_REC_ID      0x0000

/* Note: One-to-one mapping with ADDRREC values */
/* Flag bitmasks to support different search modes */
#define  PBM_CASE_MASK                   0x1
#define  PBM_CASE_INSENSITIVE            0x0
#define  PBM_CASE_SENSITIVE              0x1

#define PBM_SEARCHTYPE_MASK              0x6
#define PBM_SEARCHTYPE_STARTSWITH        0x0
#define PBM_SEARCHTYPE_EXACT             0x2
#define PBM_SEARCHTYPE_CONTAINS          0x4

#define PBM_DATATYPE_MASK                0x8
#define PBM_DATATYPE_STR                 0x0
#define PBM_DATATYPE_BYTE                0x8

#define PBM_FIELDID_MASK                 0x10
#define PBM_FIELDID_NONE                 0x00
#define PBM_FIELDID_PHONE                0x10

#define PBM_CALLHISTORY_MASK             0x20
#define PBM_CALLHISTORY_ONE              0x00
#define PBM_CALLHISTORY_ALL              0x20

#define PBM_ENUMERATE_MASK               0x40
#define PBM_ENUMERATE_ALL                0x00
#define PBM_ENUMERATE_AVAILABLE          0x40

#define PBM_MATCHING_MASK                0x80
#define PBM_MATCHING_NORMAL              0x00
#define PBM_MATCHING_GREATER             0x80

#define PBM_SORT_ORDER_MASK              0x100
#define PBM_SORT_LASTNAME_FIRST          0x000
#define PBM_SORT_LASTNAME_SECOND         0x100

#define PBM_STRTYPE_MASK                 0x200
#define PBM_STRTYPE_UCS2                 0x200
#define PBM_STRTYPE_PBM8                 0x000

#define PBM_TEXT_SIZE_CHARS (PBM_MAX_TEXT_LENGTH+1)              /* Null terminated string */
#define PBM_TEXT_SIZE_BYTES (PBM_TEXT_SIZE_CHARS*sizeof(uint16)) /* size in bytes */
#define PBM_NUM_SIZE       (PBM_MAX_NUM_LENGTH+2)  /* + and null for string */
#define PBM_WORD_SIZE      2
#define PBM_DWORD_SIZE     4
#define PBM_DDWORD_SIZE    8 /* double D-Word: 64 bits */
#define PBM_BYTE_SIZE      1

/* Status values within NV_DIAL_I */
#define NV_NON_SECRET_SD_NUM 0    /* Normal (Non secret) speed dial number */
#define NV_SECRET_SD_NUM     1    /* Secret speed dial number */
#define NV_PRIORITY_SD_NUM   2    /* Priority speed dial number */
#define NV_EMERGENCY_SD_NUM  3    /* Emergency speed dial number */
#define NV_OTASP_SD_NUM      4    /* OTASP speed dial number */
#define NV_OTASP2_SD_NUM     6    /* OTASP-2 (customer-specific) speed dial number */

/* Start of emergency numbers within NV_DIAL_I */
#define PBM_NV_EMERGENCY_NUMBER_START 99

/* Maximum number of emergency numbers within NV_DIAL_I */
#define PBM_NV_EMERGENCY_NUMBERS 11

/*
** PBM RETURN VALUES
*/

typedef enum {
  PBM_SUCCESS,
  PBM_EOF,
  PBM_ERROR_PH_SIM_REQ,
  PBM_ERROR_NO_SIM,
  PBM_ERROR_SIM_PIN_REQ,
  PBM_ERROR_SIM_PUC_REQ,
  PBM_ERROR_SIM_FAIL,
  PBM_ERROR_PIN2_REQ,
  PBM_ERROR_PUC2_REQ,
  PBM_ERROR_MEM_FULL,
  PBM_ERROR_INDEX_INVALID,
  PBM_ERROR_NOT_FOUND,
  PBM_ERROR_TEXT_TOO_LONG,
  PBM_ERROR_NUMBER_TOO_LONG,
  PBM_ERROR_INVALID_CHAR,
  PBM_ERROR_NOT_READY,
  PBM_ERROR_SYNC,
  PBM_ERROR,
  PBM_ERROR_NOT_AVAILABLE,
  PBM_ERROR_RESTRICTED,
  PBM_ERROR_NOT_SUPPORTED,
  PBM_ERROR_LOCKED,
  PBM_ERROR_INVALID_REC,
  PBM_ERROR_UNIQUE_IDS_CHANGED,
  PBM_ERROR_CAT_NOT_SUPPORTED,
  PBM_ERROR_INVALID_ENUM_FIELD,
  PBM_ERROR_MAX
} pbm_return_type;

typedef pbm_return_type pbm_return_T;  // QCT standard for "new" interface

/* from table 10.81 of 3GPP "Mobile radio interface layer 3 * specification" */
typedef struct {
  int     index; /* Logical index across multiple phonebook sets */
  char    number[PBM_NUM_SIZE]; /* + and null for string */
  uint8   type; /* type of address octet in integer format */
  char    text[PBM_TEXT_SIZE_CHARS]; /* null terminated string */
  /* Following fields added to support USIM */
  uint32  time;  /* Date and time in clock seconds */
  uint32  duration;  /* in seconds, USIM supports up to 0x00FFFFFF */
  boolean is_answered;  /* TRUE = answered, FALSE = not answered */
} pbm_record_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD pbm_record_s_type.text STRING */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef enum {
  PBM_DEFAULT = 0,
  PBM_ADN,                 /* Abbreviated Dialing Number list
                                (SIM/USIM Phonebook) */
  PBM_DS_SM = PBM_ADN,     /* Data Services SIM/USIM Phonebook */
  PBM_DS_MT,               /* Data Services combined NV/SIM Phonebook */
  PBM_FDN,                 /* Fixed Dialing Number list */
  PBM_DS_FD = PBM_FDN,     /* Data Services Fixed Dialing Number list */
  PBM_BDN,                 /* Barred Dialing Number list */

  /* 5 */
  PBM_ECC,                 /* Emergency Call Code (Emergency number list) */
  PBM_DS_EN = PBM_ECC,     /* Data Services Emergency Number list */
  PBM_DIALED,              /* Last Number Dialed list */
  PBM_DS_LD = PBM_DIALED,  /* Data Services Last Dialed Number list */
  PBM_RCVD,                /* Received call list from NV */
  PBM_DS_RC = PBM_RCVD,    /* Data Services Received call list */
  PBM_MISS,                /* Missed call list from NV  */
  PBM_DS_MS = PBM_MISS,    /* Data Services Missed Call list */
  PBM_EFS,                 /* EFS Phonebook */
  PBM_DS_ME = PBM_EFS,     /* Data Services ME phonebook */

  /* 10 */
  PBM_SDL,                 /* Speed Dial List */
  PBM_LND,                 /* SIM/RUIM Last Number Dialed list */
  PBM_OCI,                 /* USIM Outgoing Call Information file.  */
  PBM_ICI,                 /* USIM Incoming Call Information file.  */
  PBM_SDN,                 /* Service Dialling Numbers */

  /* 15 */
  PBM_MSISDN,              /* MSISDN */
  PBM_MBDN,                /* Mailbox dial numbers */
  PBM_MBN,                 /* CPHS Mailbox numbers */
                           /* SIM 2 filenames start here. */
  PBM_ADN2,                /* Abbreviated Dialing Number list */
  PBM_FDN2,                /* Data Services Fixed Dialing Number list */

  /* 20 */
  PBM_BDN2,                /* Barred Dialing Number list */
  PBM_LND2,                /* SIM/RUIM Last Number Dialed list */
  PBM_OCI2,                /* USIM Outgoing Call Information file.  */
  PBM_ICI2,                /* USIM Incoming Call Information file.  */
  PBM_SDN2,                /* Service Dialling Numbers */

  /* 25 */
  PBM_MSISDN2,             /* MSISDN */
  PBM_MBDN2,               /* Mailbox dial numbers */
  PBM_MBN2,                /* CPHS Mailbox numbers */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
  PBM_ECC2,                /* Emergency Call Code (Emergency number list) */
  PBM_ECC_OTHER,           /* Emergency Call Codes for NV,Network & hardcoded */
#endif
  PBM_RESERVED             /* Not used. */
} pbm_device_type;

typedef enum
{
  PBM_CAT_NONE = 0,
  PBM_CAT_PERSONAL,
  PBM_CAT_BUSINESS,
#ifdef CUST_EDITION
  PBM_CAT_VIP,
  PBM_CAT_HOME,
  PBM_CAT_WORK,
  PBM_CAT_FRIEND,
#endif
  PBM_CAT_USER_DEFINED = 0xFF00,

  PBM_CAT_SIM_ADN,
  PBM_CAT_SIM_FDN,
  PBM_CAT_SIM_BDN,
  PBM_CAT_SDL,
  PBM_CAT_INCOMING,
  PBM_CAT_OUTGOING,
  PBM_CAT_MISSED,
  PBM_CAT_SIM_ADN2,
  PBM_CAT_SIM_FDN2,
  PBM_CAT_SIM_BDN2,
  PBM_CAT_SIM_SDN,
  PBM_CAT_SIM_SDN2,
  PBM_CAT_SIM_MSISDN,
  PBM_CAT_SIM_MSISDN2,
  PBM_CAT_SIM_MBDN,
  PBM_CAT_SIM_MBDN2,
  PBM_CAT_SIM_MBN,
  PBM_CAT_SIM_MBN2,
  PBM_CAT_ECC,
  PBM_CAT_COUNTER,
  PBM_CAT_MAX = 0xFFFE, // Makes it uint16
  PBM_CAT_ERROR
} pbm_cat_e_type;

typedef uint16 pbm_field_id_e_type;
#define PBM_FIELD_NONE                     ((pbm_field_id_e_type) 0)
#define PBM_FIELD_FIRSTNAME                ((pbm_field_id_e_type) 1)
#define PBM_FIELD_MIDDLENAME               ((pbm_field_id_e_type) 2)
#define PBM_FIELD_LASTNAME                 ((pbm_field_id_e_type) 3)
#define PBM_FIELD_NICKNAME                 ((pbm_field_id_e_type) 4)
#define PBM_FIELD_NAME                     ((pbm_field_id_e_type) 5)
#define PBM_FIELD_ORG                      ((pbm_field_id_e_type) 6)
#define PBM_FIELD_JOBTITLE                 ((pbm_field_id_e_type) 7)
#define PBM_FIELD_ADDRESS                  ((pbm_field_id_e_type) 8)
#define PBM_FIELD_CITY                     ((pbm_field_id_e_type) 9)
#define PBM_FIELD_STATE                    ((pbm_field_id_e_type) 10)
#define PBM_FIELD_COUNTRY                  ((pbm_field_id_e_type) 11)
#define PBM_FIELD_ZIPCODE                  ((pbm_field_id_e_type) 12)
#define PBM_FIELD_URL                      ((pbm_field_id_e_type) 13)
#define PBM_FIELD_DEPARTMENT               ((pbm_field_id_e_type) 14)
#define PBM_FIELD_EMAIL                    ((pbm_field_id_e_type) 15)
#define PBM_FIELD_GENDER                   ((pbm_field_id_e_type) 16)
#define PBM_FIELD_PHONE_WORK               ((pbm_field_id_e_type) 17)
#define PBM_FIELD_PHONE_HOME               ((pbm_field_id_e_type) 18)
#define PBM_FIELD_PHONE_FAX                ((pbm_field_id_e_type) 19)
#define PBM_FIELD_PHONE_VOICE              ((pbm_field_id_e_type) 20)
#define PBM_FIELD_PHONE_PREF               ((pbm_field_id_e_type) 21)
#define PBM_FIELD_PHONE_CELL               ((pbm_field_id_e_type) 22)
#define PBM_FIELD_PHONE_PAGER              ((pbm_field_id_e_type) 23)
#define PBM_FIELD_PHONE_GENERIC            ((pbm_field_id_e_type) 24)
#define PBM_FIELD_PHONE_OTHER              ((pbm_field_id_e_type) 25)
#define PBM_FIELD_BDAY                     ((pbm_field_id_e_type) 26)

#define PBM_FIELD_SORTSTRING               ((pbm_field_id_e_type) 27)
#define PBM_FIELD_NOTES                    ((pbm_field_id_e_type) 28)

#define PBM_FIELD_SIP_URL                  ((pbm_field_id_e_type) 29)
#define PBM_FIELD_DIAL_STRING              ((pbm_field_id_e_type) 30)
//BREW took 31-33 for entries currently in 0xFFxx.  Leaving a gap
//for maintainability.
#define PBM_FIELD_SPEED_DIAL               ((pbm_field_id_e_type) 34)

#define PBM_FIELD_INDEX                    ((pbm_field_id_e_type) 0xFF01)
#define PBM_FIELD_VOICETAG                 ((pbm_field_id_e_type) 0xFF02)
#define PBM_FIELD_NUMBER_TYPE              ((pbm_field_id_e_type) 0xFF03)
#define PBM_FIELD_NUMBER_PLAN              ((pbm_field_id_e_type) 0xFF04)
#define PBM_FIELD_DATE                     ((pbm_field_id_e_type) 0xFF05)
#define PBM_FIELD_DURATION                 ((pbm_field_id_e_type) 0xFF06)
#define PBM_FIELD_CALL_TYPE                ((pbm_field_id_e_type) 0xFF07)
#define PBM_FIELD_NAMESUFFIX               ((pbm_field_id_e_type) 0xFF08)
#define PBM_FIELD_NAMEPREFIX               ((pbm_field_id_e_type) 0xFF09)
#define PBM_FIELD_OTHERNAME                ((pbm_field_id_e_type) 0xFF0A)
#define PBM_FIELD_PHOTO                    ((pbm_field_id_e_type) 0xFF0B)
#define PBM_FIELD_PHOTOURL                 ((pbm_field_id_e_type) 0xFF0C)
#define PBM_FIELD_POBOX                    ((pbm_field_id_e_type) 0xFF0D)
#define PBM_FIELD_ADDREXTRA                ((pbm_field_id_e_type) 0xFF0E)
#define PBM_FIELD_STREET                   ((pbm_field_id_e_type) 0xFF0F)
#define PBM_FIELD_UID                      ((pbm_field_id_e_type) 0xFF10)
#define PBM_FIELD_PUBLICKEY                ((pbm_field_id_e_type) 0xFF11)
#define PBM_FIELD_PUBLICKEYSTRING          ((pbm_field_id_e_type) 0xFF12)
#define PBM_FIELD_GSM_TONNPI               ((pbm_field_id_e_type) 0xFF13)
#define PBM_FIELD_COMPLEX                  ((pbm_field_id_e_type) 0xFF14)
#define PBM_FIELD_ADDITIONAL_NUMBER        ((pbm_field_id_e_type) 0xFF15)
#define PBM_FIELD_ANNAME                   ((pbm_field_id_e_type) 0xFF16)
#define PBM_FIELD_ANNAME_ID                ((pbm_field_id_e_type) 0xFF17)
#define PBM_FIELD_GRPNAME                  ((pbm_field_id_e_type) 0xFF18)
#define PBM_FIELD_GRPNAME_ID               ((pbm_field_id_e_type) 0xFF19)
#define PBM_FIELD_HIDDEN                   ((pbm_field_id_e_type) 0xFF1A)

#define  PBM_FIELD_PHONE_VIDEOCALLERID           ((pbm_field_id_e_type)0xFF1B)
#define  PBM_FIELD_PRIVATE_EMAIL1                ((pbm_field_id_e_type)0xFF1C)
#define  PBM_FIELD_PRIVATE_EMAIL2                ((pbm_field_id_e_type)0xFF1D)

/************************************************************************************
Business tab fields
************************************************************************************/
#define  PBM_FIELD_BUSINESS_EMAIL1               ((pbm_field_id_e_type)0xFF1E)
#define  PBM_FIELD_BUSINESS_EMAIL2               ((pbm_field_id_e_type)0xFF1F)
#define  PBM_FIELD_BUSINESS_URL                  ((pbm_field_id_e_type)0xFF20)
#define  PBM_FIELD_PHONE_BUSINESS_FAX            ((pbm_field_id_e_type)0xFF21)


#define  PBM_FIELD_BUSINESS_STREET               ((pbm_field_id_e_type)0xFF22)
#define  PBM_FIELD_BUSINESS_CITY                 ((pbm_field_id_e_type)0xFF23)
#define  PBM_FIELD_BUSINESS_STATE                ((pbm_field_id_e_type)0xFF24)
#define  PBM_FIELD_BUSINESS_COUNTRY              ((pbm_field_id_e_type)0xFF25)
#define  PBM_FIELD_BUSINESS_POSTALCODE           ((pbm_field_id_e_type)0xFF26)
#define  PBM_FIELD_BUSINESS_NOTES                ((pbm_field_id_e_type)0xFF27)


/************************************************************************************
Personal tab fields
************************************************************************************/
#define  PBM_FIELD_RINGTONE_AUDIO                ((pbm_field_id_e_type)0xFF28)
#define  PBM_FIELD_RINGTONE_VIDEO                ((pbm_field_id_e_type)0xFF29)
#define  PBM_FIELD_BIRTHDAY                      ((pbm_field_id_e_type)0xFF2A)
#define  PBM_FIELD_EVENT_UID                     ((pbm_field_id_e_type)0xFF2B)



/************************************************************************************
Fields Used to store default information
************************************************************************************/
#define  PBM_FIELD_DEFAULT_PHONE                 ((pbm_field_id_e_type)0xFF2C)
#define  PBM_FIELD_DEFAULT_EMAIL                 ((pbm_field_id_e_type)0xFF2D)
#define  PBM_FIELD_DEFAULT_ADDRESS               ((pbm_field_id_e_type)0xFF2E)
#define  PBM_FIELD_DEFAULT_URL                   ((pbm_field_id_e_type)0xFF2F)
#define  PBM_FIELD_DEFAULT_FAX                   ((pbm_field_id_e_type)0xFF30)
#define  PBM_FIELD_DISTRIBUTION_LIST             ((pbm_field_id_e_type)0xFF31)


/************************************************************************************
Additional Fields
************************************************************************************/
#define  PBM_FIELD_PRIVATEPHONE_VOICETAG         ((pbm_field_id_e_type)0xFF32)
#define  PBM_FIELD_BUSINESSPHONE_VOICETAG        ((pbm_field_id_e_type)0xFF33)
#define  PBM_FIELD_PRIVATEMOBILE_VOICETAG        ((pbm_field_id_e_type)0xFF34)
#define  PBM_FIELD_BUSINESSMOBILE_VOICETAG       ((pbm_field_id_e_type)0xFF35)
#define  PBM_FIELD_VIDEOTELEPHONY                ((pbm_field_id_e_type)0xFF36)
#define  PBM_FIELD_USEDCOUNT                     ((pbm_field_id_e_type)0xFF37)
#define  PBM_FIELD_BCARD                         ((pbm_field_id_e_type)0xFF38)
#define  PBM_FIELD_GROUP                         ((pbm_field_id_e_type)0xFF39)
#define  PBM_FIELD_DLISTS                        ((pbm_field_id_e_type)0xFF3A)
#define  PBM_FIELD_CALL_METHOD                   ((pbm_field_id_e_type)0xFF3B)
#define  PBM_FIELD_AOC                           ((pbm_field_id_e_type)0xFF3C)

/************************************************************************************
Field for phone unique ID
************************************************************************************/
#define  PBM_FIELD_PH_UID                        ((pbm_field_id_e_type)0xFF3D)

/************************************************************************************
Field for different emergency numbers
************************************************************************************/
#define PBM_FIELD_HARDCODED_ECC                  ((pbm_field_id_e_type) 0xFF3E)
#define PBM_FIELD_NV_ECC                         ((pbm_field_id_e_type) 0xFF3F)
#define PBM_FIELD_SIM1_ECC                       ((pbm_field_id_e_type) 0xFF40)
#define PBM_FIELD_SIM2_ECC                       ((pbm_field_id_e_type) 0xFF41)
#define PBM_FIELD_ECC_SERVICE_CATEGORY           ((pbm_field_id_e_type) 0xFF42)

/* more efs fields */
#define  PBM_FIELD_PRIVATE_ADDITIONALINFO        ((pbm_field_id_e_type) 0xFF43)
#define  PBM_FIELD_BUSINESS_ADDITIONALINFO       ((pbm_field_id_e_type) 0xFF44)

/* videotelephony stuff */
#define  PBM_FIELD_PHONE_VIDEOTELEPHONY          ((pbm_field_id_e_type) 0xFF45)
#define  PBM_FIELD_PREFER_VT                     ((pbm_field_id_e_type) 0xFF46)

/* emergency numbers from network */
#define  PBM_FIELD_NETWORK_ECC                   ((pbm_field_id_e_type) 0xFF47)

/* Call technology type */
#define  PBM_FIELD_TECH_TYPE                     ((pbm_field_id_e_type) 0xFF48)

/* Primary Phone ID */
#define  PBM_FIELD_FIELDID_PRIMARY_PHONE         ((pbm_field_id_e_type) 0xFF49)

/************************************************************************************
Fields for UIOneRefUI Contact App
************************************************************************************/
#define  PBM_FIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE  ((pbm_field_id_e_type) 0xFF50)
#define  PBM_FIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE   ((pbm_field_id_e_type) 0xFF51)
#define  PBM_FIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE ((pbm_field_id_e_type) 0xFF52)
#define  PBM_FIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE  ((pbm_field_id_e_type) 0xFF53)

/* Call technology types */
enum {
  PBM_TECH_TYPE_VOICE,
  PBM_TECH_TYPE_VIDEO,
  PBM_TECH_TYPE_DATA
};
typedef uint16 pbm_tech_type_e_type;

/* apitrim -include */
enum
{
  PBM_FT_NONE,
  PBM_FT_BYTE,
  PBM_FT_WORD,
  PBM_FT_DWORD,
  PBM_FT_STRING,
  PBM_FT_BINARY,
  PBM_FT_PHONE,
  PBM_FT_BITMAP,
  PBM_FT_COMPLEX,
  PBM_FT_UCS2
};

typedef uint8 pbm_field_type_e_type;

typedef struct
{
  pbm_field_id_e_type field_id;
  pbm_field_type_e_type field_type;
  uint32 buffer_offset;  /* # bytes from beginning of this field to beginning of data */
  uint16 data_len;
} pbm_field_s_type;

/* apitrim -end-include */

typedef struct {
  pbm_field_id_e_type   field_id;        /* Field ID */
  int                   num_items;       /* Maximum number within a record */
  int                   max_length;      /* Maximum length of data */
  pbm_field_type_e_type field_type;      /* Field type */
  boolean               field_search;    /* Allow search for data? */
  boolean               field_cat_search;/* -"- within specified category? */
} pbm_field_info_s_type;

/* Complex data type item
 * This structure maps directly to AEEDBComplex */
typedef PACKED struct
{
   pbm_field_id_e_type   field_id;   /* Field id */
   uint16                data_len;   /* Number of bytes in buf[] */
   pbm_field_type_e_type field_type; /* Field type */
   uint8                 buf[1];     /* Variable number of bytes of data */
} pbm_sub_field_s_type;

/* Complex data type
 * This structure maps directly to AEEDBSubfield */
typedef PACKED struct
{
   uint16   num_subfields; /* Number of pbm_sub_field_s_type */
   uint8    version;       /* Version of the complex type. */
   uint8    byReserved;
   uint8    buf[1];        /* Variable number of bytes
                            * containing pbm_sub_field_s_type structs */
} pbm_complex_field_s_type;

typedef enum {
  PBM_LOCK_NOT_INIT,        /* Not set */
  PBM_W_LOCK_BLOCKING,      /* Other tasks will be blocked on the lock */
  PBM_W_LOCK_NON_BLOCKING,  /* Writes from other tasks will return an error */
  PBM_RW_LOCK_BLOCKING,     /* No read/writes from other tasks (blocks) */
  PBM_RW_LOCK_NON_BLOCKING  /* No read/writes from other tasks (non-blocking) */
} pbm_lock_type_e_type;

typedef enum {
  PBM_EVENT_REC_ADD,                  /* A new record was added (data: rec_id) */
  PBM_EVENT_REC_UPDATE,               /* Record was updated (data: rec_id) */
  PBM_EVENT_REC_DELETE,               /* Record was deleted (data: rec_id) */
  PBM_EVENT_REC_FAILED,               /* Record operation failed (data: rec_id) */
  PBM_EVENT_PB_REFRESH_START,         /* Phonebook refresh was started (data: pb_id) */
  PBM_EVENT_PB_REFRESH_DONE,          /* Phonebook refresh done (data: pb_id) */
  PBM_EVENT_PB_READY,                 /* Phonebook is now ready (data: pb_id) */
  PBM_EVENT_LOCKED,                   /* PBM is locked (data: lock_type) */
  PBM_EVENT_UNLOCKED,                 /* PBM is unlocked (data: lock_type) */
  PBM_EVENT_PH_UNIQUE_IDS_VALIDATED,  /* Phone Unique IDs were validated (data: none) */
  PBM_EVENT_SIM_INIT_DONE             /* PBM SIM is done initializing (data: sim_id) */
} pbm_notify_event_e_type;

typedef union {
  uint16               rec_id;    /* Record ID */
  pbm_device_type      pb_id;     /* Phonebook id */
  pbm_lock_type_e_type lock_type; /* Lock type */
  uint16               sim_id;    /* 0 for slot 1, add 1 for each additional slot*/
} pbm_event_data_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ IF ( _DISC_ == PBM_EVENT_REC_ADD ||
         _DISC_ == PBM_EVENT_REC_UPDATE ||
         _DISC_ == PBM_EVENT_REC_DELETE ||
         _DISC_ == PBM_EVENT_REC_FAILED )
    pbm_event_data_u_type.rec_id */
/*~ IF ( _DISC_ == PBM_EVENT_PB_REFRESH_START ||
         _DISC_ == PBM_EVENT_PB_REFRESH_DONE ||
         _DISC_ == PBM_EVENT_PB_READY )
    pbm_event_data_u_type.pb_id */
/*~ IF ( _DISC_ == PBM_EVENT_LOCKED ||
         _DISC_ == PBM_EVENT_UNLOCKED )
     pbm_event_data_u_type.lock_type */
/*~ IF ( _DISC_ == PBM_EVENT_SIM_INIT_DONE )
     pbm_event_data_u_type.sim_id */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct {
  pbm_notify_event_e_type  event; /* event */
  pbm_event_data_u_type    data;  /* event data */
} pbm_notify_data_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD pbm_notify_data_s_type.data DISC pbm_notify_data_s_type.event */
#endif /* FEATURE_HTORPC_METACOMMENTS */
typedef struct {
  pbm_return_type  ret;       /* return value of the operation */
  uint16           rec_id;    /* record id this callback applies to */
  void            *user_data; /* passed in by the caller, if supported */
} pbm_writecb_data_s_type;

typedef void (*PBM_EVENT_FUNC)(boolean ready);
typedef void (*PBM_WRITE_COMPAT_CB_FUNC)(pbm_return_type ret);

typedef void (*PBM_WRITE_CB_FUNC)(pbm_writecb_data_s_type *cb_data);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM cb_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef void (*PBM_NOTIFY_FUNC)(void *user_data,  pbm_notify_data_s_type *notify_data);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM notify_data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef void (*PBM_FILE_INFO_CB_FUNC) ( pbm_return_type ret,
                                        pbm_device_type pb_id,
                                        int records_used,
                                        int number_of_records,
                                        int text_len);

typedef struct {
  int records_used;         /* number of records used */
  int number_of_records;    /* total number of records */
  int text_len;             /* length of text fields */
  int number_len;           /* length of number fields, not including + or NULL */
} pbm_extended_fileinfo_s_type;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  #define MAX_UIMS        2
#else
  #define MAX_UIMS        1
#endif

typedef struct {
  boolean pbm_efs_init_done;         /* initialization of phone mmemory */
  boolean pbm_sim_init_done[MAX_UIMS];    /* initialization of sim mmemory*/
} pbm_init_info_s_type;

extern pbm_init_info_s_type pbm_init_info;


typedef void (*PBM_EXTENDED_FILE_INFO_CB_FUNC) ( pbm_return_type ret,
                                                 pbm_device_type pb_id,
                                                 pbm_extended_fileinfo_s_type *info);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM info POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef void (*PBM_FIND_CB_FUNC) ( pbm_return_type ret, pbm_record_s_type *rec);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ PARAM rec POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

extern PBM_EVENT_FUNC ds_ready_fn;
/*===========================================================================
FUNCTION PBM_CLEAR_CALL_HISTORY

DESCRIPTION
  Clears all call history entries

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_clear_call_history(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_clear_call_history */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_IS_INIT_COMPLETED

DESCRIPTION
will let the client know whether the init process of PBM is comlpeted or not


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
extern boolean pbm_is_init_completed(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_is_init_completed */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_ENUM_REC_INIT

DESCRIPTION
Initializes searches of the address book for specific records

Does search to set up for subsequent calls to pbm_enum_next_rec(), which
iterates through the records that match the given criteria.  Search results
are stored in static variables and are overwritten by the next call to
pbm_rec_init().

Searching rules are as follows:
1)  Only records with the given category are matched.  If PBM_CAT_NONE is
    specified, then all categories are matched.
2)  Only records that contain at least one instance of a field with the
    given field ID are matched.  If PBM_FIELD_NONE is specified, then
    this criterion is ignored.
3)  Only records with data in the specified field that "matches" the data
    specified by data_ptr are matched.  If data_ptr is NULL, then this
    criterion is ignored.

  What "matches" means depends on the setting of flags:
    a.  PBM_CASE_SENSITIVE_MATCH and PBM_CASE_INSENSITIVE_MATCH control
        case-sensitivity.
    b.  PBM_EXACT_MATCH and PBM_STARTS_WITH_MATCH control how much of
        data specified by data_ptr needs to be the same to be considered
        a match.
  Example:  pbm_rec_init(PBM_EFS, PBM_CAT_NONE, PBM_FIELD_NONE, "joh", 3,
                         PBM_CASE_INSENSITIVE_MATCH | PBM_STARTS_WITH_MATCH)
            would match on the names "John Smith" and "Johann Bach"

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_enum_rec_init(pbm_device_type pb_id,
                               uint16 category,
                               pbm_field_id_e_type field_id,
                               const uint8 *data_ptr,
                               uint16 data_size,
                               uint32 flags);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_enum_rec_init */
/*~ PARAM data_ptr VARRAY LENGTH data_size */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_ENUM_NEXT_REC_ID

DESCRIPTION
Retrieves the next record ID that matches the search criteria
specified in the most recent call to pbm_enum_rec_init().

Only one search at a time is supported.  This can allow situations where
one application has called pbm_enum_rec_init() and is in the process of
calling pbm_enum_next_rec_id() a few times.  Then another application calls
pbm_enum_rec_init() and changes the search criteria and resets the search
before the first application is done.

When the end of the enumeration is reached, the index is not reset to the
beginning of the enumeration.  All subsequent calls to this function must
continue to return PBM_EOF.  The caller must call pbm_enum_rec_init() to
reinitialize the search criteria.

DEPENDENCIES
pbm_enum_rec_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_next_rec_id(uint16 *rec_id_ptr);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_enum_next_rec_id */
/*~ PARAM OUT rec_id_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, and the text length for the specified phonebook.

  Data returned in proc_func.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_file_info (pbm_device_type pb_id,
                                      PBM_FILE_INFO_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_file_info */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_EXTENDED_FILE_INFO

DESCRIPTION
  Through GSDI will return the total number of records, the number of records
  in use, the text length, and the max phone number length for the specified phonebook.

  Data returned in proc_func.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_extended_file_info (pbm_device_type pb_id,
                                               PBM_EXTENDED_FILE_INFO_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_extended_file_info */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FIND_LOCATION

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_find_location (pbm_device_type pb_id,
                                int index,
                                pbm_record_s_type * data,
                                PBM_FIND_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_find_location */
/*~ PARAM OUT data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FIND_NAME_NEXT

DESCRIPTION
  Continues reading from linked list to determine if additional calls
  to the SIM are needed.  If so, call pbm_sim_read again.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_find_name_next( void );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_find_name_next */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FIND_NAME

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "name".  If no match is found PBM_ERROR_NOT_FOUND is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_find_name (pbm_device_type pb_id,
                            char *name,
                            pbm_record_s_type * data,
                            PBM_FIND_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_find_name */
/*~ PARAM name STRING */
/*~ PARAM OUT data POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FIND_NUMBER

DESCRIPTION
  This function will call the function defined in proc_func for each matching
  occurance of string "number".  If no match is found PBM_EOF is returned
  otherwise each matching record is returned, until EOF is reached.

  Data returned in proc_func.  The record pointer passed as a parameter in
  proc_func is only valid for the duration of the callback.  If clients want
  to use the data after the callback terminates, they need to make a copy.

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_find_number (pbm_device_type pb_id,
                              const byte *number, int loc_number_len,
                              pbm_record_s_type * record,
                              PBM_FIND_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_find_number */
/*~ PARAM number VARRAY LENGTH loc_number_len */
/*~ PARAM OUT record POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_GET_NUM_RECS

DESCRIPTION
Gets the total number of records in the given phone book.

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
extern uint16 pbm_get_num_recs(pbm_device_type pb_id);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_get_num_recs */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_ID

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  based on the record ID.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_fields_size_from_id( uint16 rec_id );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_calculate_fields_size_from_id */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_RECORD_READ

DESCRIPTION
Searches the cache appropriate to the given phonebook and retrieves
information contained in a specified record, returning success/failure
indication.  Does not delete the record from the address book.  If no
record is found, PBM_ERROR_NOT_FOUND is returned.

The layout of data_buf when it is returned is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_read(uint16            rec_id,
                                uint16           *category_ptr,
                                int              *num_fields_ptr,
                                uint8            *data_buf,
                                uint32            data_buf_size);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_record_read */
/*~ PARAM OUT category_ptr POINTER */
/*~ PARAM OUT num_fields_ptr POINTER */
/*~ PARAM OUT data_buf VARRAY data_buf_size LENGTH data_buf_size */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_RECORD_WRITE

DESCRIPTION
Add/updates/deletes a record in the address book (both EFS and SIM).  This is
the new interface that supports many different fields per contact.

To add a record:
1)  Set rec_id = 0xFFFF (and rec_id_ptr=&rec_id).  After a synchronous call has
    completed, look here for the assigned record ID for the new record.
2)  Set items_ptr equal to an array of fields for the new record
3)  Set item_count equal to the length of above array
    To update a record, do the same as when adding a record (above), except set
    rec_id to the record ID of the record to be updated.  Note that the fields
    given in items_list will replace all fields in the current record, i.e. the
    current fields will be deleted and replaced, not added to.

To delete a record:
1)  Set rec_id to the record ID of the record to be deleted.
2)  Set data_buf = NULL.
3)  Other fields are not used.

When making an asynchronous call, a success return value indicates that
the command has been successfully queued. The actual result of the operation
is given in the first argument to the callback function proc_func(). The
resulting record id is returned synchronously within rec_id_ptr and along
with the proc_func callback.

The layout of data_buf when it is sent is that it starts with
*num_fields_ptr number of pbm_field_s_type structures.  After those structures,
is the data for those fields.  pbm_get_field_data_pointer() can get pointers
to the actual field data.


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_record_write( pbm_device_type         pb_id,
                                         uint16                 *rec_id_ptr,
                                         uint16                  cat,
                                         int                     num_fields,
                                         const uint8            *data_buf,
                                         int                     data_buf_size,
                                         PBM_WRITE_CB_FUNC       proc_func,
                                         void                   *user_data);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_record_write */
/*~ PARAM INOUT rec_id_ptr POINTER */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_REG_EVENT_CB

DESCRIPTION
  Allows the user to initialize the ds_ready_fn to be called during init.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
extern pbm_return_type pbm_reg_event_cb ( PBM_EVENT_FUNC user_fn );
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_reg_event_cb */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_WRITE

DESCRIPTION
  Writes the given record to the phone book specified.  If proc_func is not NULL
  that is called when the write is complete.

  Note: Now this is just a wrapper for pbm_record_write() that preserves the legacy
  interface of one callback function with one argument to it.

PARAMETERS:
  record  if record.index == -1, write to the first empty record
          if record.number is NULL, delete entry record.index

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write ( pbm_device_type pb_id,
                         pbm_record_s_type * record,
                         PBM_WRITE_COMPAT_CB_FUNC proc_func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_write */
/*~ PARAM INOUT record POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_FILL_FIELD

DESCRIPTION
  This function is just a shortcut to fill in the four different fields
  which make up the field structure.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_fill_field(pbm_field_s_type *field,
    pbm_field_id_e_type field_id, pbm_field_type_e_type field_type,
    const byte *buffer, uint16 data_len);

/*===========================================================================
FUNCTION PBM_GET_FIELD_DATA_PTR

DESCRIPTION
  This function is just a shortcut return the pointer to the data buffer for
  a given field number.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void * pbm_get_field_data_ptr(const void *field_buf,
                              int field_num);

/*===========================================================================
FUNCTION PBM_FREE_FIELDS

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields( uint8 **fields, int num_fields);

/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields(uint8 **fields_buf_ptr,
                            uint32 *fields_buf_size,
                            uint32 num_fields,
                            const uint32 *sizes);


/*===========================================================================
FUNCTION PBM_LOCATION_TO_RECORD_ID

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_record_id(pbm_device_type pb_id, int location);

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_LOCATION

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_location(uint16 record_id);

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PHONEBOOK

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_record_id_to_phonebook(uint16 record_id);

/*===========================================================================
FUNCTION PBM_WRITE_LOCK

DESCRIPTION
  This function is used to lock further writes to PBM. Returns error if unable
  to acquire lock ie pbm_write_lock itself is always non-blocking. The lock type
  specifies whether other tasks are blocked by this lock or not. If lock type is
  blocking, other tasks are blocked till the task who owns the lock unlocks pbm
  write. If the lock type is unblocking, writes from other tasks will result in
  an error.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write_lock(pbm_lock_type_e_type type,
                               void *user_data,
                               PBM_WRITE_CB_FUNC func);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_write_lock */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_WRITE_UNLOCK

DESCRIPTION
  This function is used to unlock PBM writes.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_write_unlock(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_write_unlock */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_GET_FIELD_INFO_COUNT

DESCRIPTION
  This function returns the number of PBM fields supported by the specified
  PBM phonebook or category.

PROTOTYPE:
  pbm_return_type pbm_get_field_info_count(pbm_device_type pb_id, pbm_cat_e_type cat,
                                           int *num)
PARAMETERS:
  pb_id       [in]: Phonebook device.
  cat         [in]: Category for which information is requested.
  num        [out]: Number of fields supported by this category.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_field_info_count(pbm_device_type pb_id, pbm_cat_e_type cat,
                                         int *num);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_get_field_info_count */
/*~ PARAM OUT num POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_GET_FIELD_INFO

DESCRIPTION
  This function returns field information associated with each PBM field
  supported by the specified category

PROTOTYPE:
  pbm_return_type pbm_get_field_info(pbm_device_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size)
PARAMETERS:
  pb_id       [in]: The phonebook device for which information is requested
  cat         [in]: Category for which information is requested.
  pf         [out]: Array of field info which is filled up by PBM
  size        [in]: Size of pf array in bytes.

RETURN VALUE:
  PBM_SUCCESS         : If successful
  PBM_ERROR_NOT_READY : If requested phonebook is not ready
  PBM_ERROR           : Otherwise

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_field_info(pbm_device_type pb_id, pbm_cat_e_type cat,
                                   pbm_field_info_s_type *pf, int size);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_get_field_info */
/*~ PARAM OUT pf VARRAY (size/sizeof(pbm_field_info_s_type))
    LENGTH (size/sizeof(pbm_field_info_s_type)) */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_RECORD_VALIDATE

DESCRIPTION
  This function checks whether a given record is valid.

  The checks include the following.
    - Check whether fields that are required are included. For ADN or FDN phonebooks
      there should be a non-NULL number string. For other phonebooks there should be
      a non-NULL number OR non-NULL name string.
    - Check whether the maximum number of a particular field or the maximum size of
      a particular field or the maximum size of the entire record is exceeded.
    - Check whether all the fields are supported for this phonebook and whether the
      types of fields match the field id.

PROTOTYPE:
  pbm_return_type pbm_record_validate(pbm_device_type pb_id, uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields);
PARAMETERS:
  pb_id       [in]: Phonebook device
  rec_id      [in]: Record id.
  cat         [in]: Category of the record
  pfields     [in]: buffer with Array of fields containing data (same struct as
                    pbm_record_read() and pbm_record_write()
  num_fields  [in]: Number of fields.

RETURN VALUE:
  PBM_ERROR_INDEX_INVALID   : If the record contains an INDEX field and if it is
                              invalid or the record id is not valid.
  PBM_ERROR_TEXT_TOO_LONG   : If name is too long.
  PBM_ERROR_NUMBER_TOO_LONG : If number is too long.
  PBM_ERROR_NOT_READY       : If the phone book requested has not been initialized.
  PBM_ERROR_NO_SIM          : If the corresponding UIM is not present.
  PBM_ERROR_NOT_AVAILABLE   : If the corresponding UIM file is not available
  PBM_ERROR_RESTRICTED      : If UIM file is not available due to restriction
  PBM_ERROR_INVALID_REC     : If the record is not valid for this phonebook due
                              to other reasons.
  PBM_SUCCESS               : If record is valid


DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_validate(pbm_device_type pb_id,
                                    uint16 rec_id,
                                    pbm_cat_e_type cat,
                                    uint8 *data_buf,
                                    int data_buf_size,
                                    int num_fields);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_record_validate */
/*~ PARAM data_buf VARRAY LENGTH data_buf_size */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_CLEAR_PHONEBOOK

DESCRIPTION
  Clears all entries within a given phonebook

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_clear_phonebook(pbm_device_type pb_id);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_clear_phonebook */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_EMERGENCY_NUMBER

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.

DEPENDENCIES
  uipbm.h
  nv.h

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_emergency_number(const byte *num, byte len);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_emergency_number */
/*~ PARAM num VARRAY LENGTH len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_EMERGENCY_NUMBER_CAT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.  If it is, the
  category is returned in ecc_category_ptr.  If the number has no category, it is
  set to 0.

DEPENDENCIES
  uipbm.h
  nv.h

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_emergency_number_cat(const byte *num, byte len, uint8 *ecc_category_ptr);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_emergency_number_cat */
/*~ PARAM num VARRAY LENGTH len */
/*~ PARAM OUT ecc_category_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_NUMBER_MATCH

DESCRIPTION
  Does an exact match of a phone number.  Ignoring any non-dialed characters.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_number_match(const byte *sim_num,    byte sim_num_len,
                         const byte *dialed_num, byte dialed_num_len);

/*===========================================================================
FUNCTION PBM_FDN_NUMBER_MATCH

DESCRIPTION
  The rules for FDN is that the digits in the SIM must match, but they
  allow digits at the end.

DEPENDENCIES
  None.

SIDE EFFECTS
===========================================================================*/
boolean pbm_fdn_number_match(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len);

/*===========================================================================
FUNCTION PBM_NOTIFY_REGISTER

DESCRIPTION
  This function is used to register a callback function that is used to notify
  PBM events. The user data given as input will be passed on when the
  notification callback is called.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_unregister
===========================================================================*/
pbm_return_type pbm_notify_register(PBM_NOTIFY_FUNC func, void *user_data);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_notify_register */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_NOTIFY_UNREGISTER

DESCRIPTION
  This function is used to cancel the notification function registered with PBM.

  The function pointer and user data provided as input should match the
  registered function pointer and user data.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_register
===========================================================================*/
pbm_return_type pbm_notify_unregister(PBM_NOTIFY_FUNC func, void *user_data);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_notify_unregister */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_HAVE_UIDS_CHANGED

DESCRIPTION
  This function is used to see if the phone Unique ID's have changed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_validate_uid_changes
===========================================================================*/
boolean pbm_have_uids_changed(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_have_uids_changed */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION PBM_VALIDATE_UID_CHANGES

DESCRIPTION
  This function is used to acknowledge that the unique IDs have changed.

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_have_uids_changed
===========================================================================*/
pbm_return_type pbm_validate_uid_changes(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pbm_validate_uid_changes */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION pbmutils_ucs2_to_pbm8

DESCRIPTION
  boolean pbmutils_ucs2_to_pbm8(uint16 *ucs2_str, uint16 ucs_str_len,
                                uint8 *pbm8_str, uint16 pbm8_str_len)
  *ucs2_str:    [in]  Array of UCS2 characters to convert to PBM8.
  ucs_str_len:  [in]  Length of the array of UCS2 characters.
  *pbm8_str:    [out] Array where translation is saved.
  pbm8_str_len: [in]  Length of results string.

  This function will convert a UCS2 string to PBM8.  It will do its
  best to convert the entire string, but will fill unrecognized characters
  with the UNDEFINED_CHAR define octet (Non-blocking space).

  The function will leave the resulting string as null terminated,
  truncating the source if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates every character in ucs2_string exists in the resulting
       PBM8 string.
  FALSE if some character could not be translated or we ran out of room.

SIDE EFFECTS
  None.
===========================================================================*/
#define PBM_UNDEFINED_CHAR 0xA0 /* Non Blocking SPACE fills chars we
                                 * couldn't convert */
boolean pbmutils_ucs2_to_pbm8(const uint16 *ucs2_str, uint16 ucs_str_len,
                              uint8 *pbm8_str, uint16 pbm8_str_len);

/*===========================================================================
FUNCTION pbmutils_pbm8_to_ucs2

DESCRIPTION
  boolean pbmutils_pbm8_to_ucs2( const uint8 *pbm8_str,  uint16 pbm8_str_len,
                                 uint16 *ucs2_str, uint16 ucs2_str_len)

  pbm8_str:     [in]  PBM8 string to convert.
  pbm8_str_len: [in]  Length of said string.
  ucs2_str:     [out] Buffer filled with the translation.
  ucs2_str_len: [in]  Size of resulting buffers.  Note, the actual size
                      of the memory must be 2 times this value since each
                      item is 16 bits large.

  This function will convert a PBM8 string to UCS2.  The function will leave
  the resulting string as null terminated, truncating the source if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE indicates every character in pbm8_string exists in the resulting
       UCS2 string.
  FALSE if we ran out of room.

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbmutils_pbm8_to_ucs2( uint8 *pbm8_str,  uint16 pbm8_str_len,
                               uint16 *ucs2_str, uint16 ucs2_str_len);


/*===========================================================================
FUNCTION pbmutils_wstrlen;

DESCRIPTION
  uint32 pbmutils_wstrlen(uint16 *ucs2_str);

  ucs2_str:   [in]   The UCS2 string to count the characters.

  This function counts the number of UCS2 characters in a string.  The
  string must be terminated by 0x0000.

DEPENDENCIES
  None.

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmutils_wstrlen(const uint16 *ucs2_str);
#endif /* PBMLIB_H */
