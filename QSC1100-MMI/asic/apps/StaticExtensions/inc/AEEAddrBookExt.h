#ifndef __AEEADDRBOOKEXT_H__
#define __AEEADDRBOOKEXT_H__

/*===========================================================================
FILE: AEEAddrBookExt.h

SERVICES: OEM specific extensions to the standard AEEAddrBook interface 

GENERAL DESCRIPTION:
  Declaration for new additional categories that we add in OEM Address book 
  layer which can be used by Applets.

PUBLIC CLASSES AND STATIC FUNCTIONS: 

INITIALIZATION AND SEQUENCING REQUIREMENTS


        Copyright © 2003-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEAddrBookExt.h#28 $
$DateTime: 2007/11/19 14:58:38 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
11/07/07     clm     Add AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE field
10/11/06     jks     Mainlined TrigNx AddrBook extensions
10/02/06     cvs     Added MBN categories.
03/06/06     cvs     Add new fields
02/28/05     cvs     Added MBDN categories.
11/10/05     cvs     Add network ECC numbers
10/18/05     cvs     Add new fields
09/01/05     AT      Added new ECC Category.
08/01/05     cvs     rename field
06/29/05     AT      New constants to match new PBM fields.
03/24/05     cvs     Added Phone Unique ID functionality
02/17/05     PA      Added MSISDN categories.
12/28/04     SUN     Added the Distribution list field
12/15/04     SUN     Added new address fields
12/08/04     SUN     Added Support for Complex Fields
11/15/04     BW      Added AEE_ADD_CAT_SDN and AEE_ADDR_CAT_SDN2
08/20/04     JG      IADDRBOOK_EnumRecInitEx() is defined in BREW 3.1. 
                     Rename the QCT IADDRBOOK_EnumRecInitEx() to 
                     IADDRBOOKEXT_EnumRecInitEx()
06/23/04     PA      Support for enumeration of available contacts
1/20/04      AT      Added all the ADDRFIELD's needed for Java.
09/12/03     adm     Added AEE_ADDR_CAT_INCOMING, AEE_ADDR_CAT_OUTGOING, AEE_ADDR_CAT_MISSED 
                     to map to PBM categories.
09/10/03     adm     Added AEE_ADDR_CAT_SIM_ADN2, AEE_ADDR_CAT_SIM_FDN2, AEE_ADDR_CAT_SIM_BDN2
                     for dual slot support
05/08/03     PA      Added AEE_ADDRFIELD_VOICETAG, ADDRREC flags and
                     EnumRecInitEx.
04/17/03     PA/AT   Removed SPEEDDIAL_SLOTINDEX and added the general purpose 
                     AEE_ADDRFIELD_INDEX
02/06/03     kar     Initial QCT version
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "BREWVersion.h"
#include "AEEAddrBook.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
// Address book categories
typedef enum 
{
  AEE_ADDR_CAT_OEM_MIN = AEE_ADDR_CAT_USER_DEFINED,

// Address book category for Abbreviated Dialing Numbers (on the R-UIM/SIM)
  AEE_ADDR_CAT_SIM_ADN,

// Address book category for Fixed Dialing Numbers (on the R-UIM/SIM)
  AEE_ADDR_CAT_SIM_FDN,

// Address book category for Barred Dialing Numbers (on the R-UIM/SIM)
  AEE_ADDR_CAT_SIM_BDN,

  AEE_ADDR_CAT_SPEEDDIAL,
  AEE_ADDR_CAT_INCOMING,
  AEE_ADDR_CAT_OUTGOING,
  AEE_ADDR_CAT_MISSED,

// Address book category for Abbreviated Dialing Numbers (on the R-UIM2/SIM2)
  AEE_ADDR_CAT_SIM_ADN2,

// Address book category for Fixed Dialing Numbers (on the R-UIM2/SIM2)
  AEE_ADDR_CAT_SIM_FDN2,

// Address book category for Barred Dialing Numbers (on the R-UIM2/SIM2)
  AEE_ADDR_CAT_SIM_BDN2,

// Address book category for Service Dialing Numbers (on the R-UIM/SIM)
  AEE_ADDR_CAT_SIM_SDN,
// Address book category for Service Dialing Numbers (on the R-UIM2/SIM2)
  AEE_ADDR_CAT_SIM_SDN2,
  
// Address book category for MSISDN 
  AEE_ADDR_CAT_SIM_MSISDN,
// Address book category for MSISDN
  AEE_ADDR_CAT_SIM_MSISDN2,
  
// Emergency numbers are stored in this category.
  AEE_ADDR_CAT_ECC,

// Address book category for MBDN
  AEE_ADDR_CAT_SIM_MBDN,
// Address book category for MBDN
  AEE_ADDR_CAT_SIM_MBDN2,

// Address book category for MBN
  AEE_ADDR_CAT_SIM_MBN,
// Address book category for MBN
  AEE_ADDR_CAT_SIM_MBN2,

  AEE_ADDR_CAT_OEM_COUNT,  // Tells us how many OEM categories there are
  AEE_ADDR_CAT_OEM_MAX = 0xFF // Forces type to uint16
} oem_addr_cat_e_type;

/* NOTE: One-to-one mapping with PBM values */
/* Flags Masks */
#define  ADDRREC_CASE_MASK                    0x1
#define  ADDRREC_SEARCHTYPE_MASK              0x6
#define  ADDRREC_DATATYPE_MASK                0x8
#define  ADDRREC_FIELDID_MASK                 0x10
#define  ADDRREC_ENUMERATE_MASK               0x40

/* Flags Fields */
/* Default (0x0) is case insensitive, starts with, string match 
   with FIELD_NONE */

#define  ADDRREC_CASE_INSENSITIVE             0
#define  ADDRREC_CASE_SENSITIVE               1

#define  ADDRREC_SEARCHTYPE_STARTSWITH        0
#define  ADDRREC_SEARCHTYPE_EXACT             2
#define  ADDRREC_SEARCHTYPE_CONTAINS          4

#define  ADDRREC_DATATYPE_STR                 0 
#define  ADDRREC_DATATYPE_BYTE                8   

#define  ADDRREC_FIELDID_NONE                 0
#define  ADDRREC_FIELDID_PHONE                0x10

#define  ADDRREC_ENUMERATE_ALL                0
#define  ADDRREC_ENUMERATE_AVAILABLE          0x40

/* AEE_ADDRFIELD_INDEX field can be used to specify the index of the record 
   within the address book. */

#if !MIN_BREW_VERSION(3,1) // BREW 3.1 includes these things already
#define  AEE_ADDRFIELD_INDEX       (AEE_ADDRFIELD_USER+0x01)
/* Used to store a voice tag ID provided by PureVoice */
#define  AEE_ADDRFIELD_VOICETAG    (AEE_ADDRFIELD_USER+0x02)
#endif

#define  AEE_ADDRFIELD_NUMBER_TYPE (AEE_ADDRFIELD_USER+0x03)
#define  AEE_ADDRFIELD_NUMBER_PLAN (AEE_ADDRFIELD_USER+0x04)
#define  AEE_ADDRFIELD_DATE        (AEE_ADDRFIELD_USER+0x05)
#define  AEE_ADDRFIELD_DURATION    (AEE_ADDRFIELD_USER+0x06)
#define  AEE_ADDRFIELD_CALL_TYPE   (AEE_ADDRFIELD_USER+0x07)
#if !MIN_BREW_VERSION(3,1) // BREW 3.1 includes these things already
#define  AEE_ADDRFIELD_NAMESUFFIX  (AEE_ADDRFIELD_USER+0x08)
#define  AEE_ADDRFIELD_NAMEPREFIX  (AEE_ADDRFIELD_USER+0x09)
#define  AEE_ADDRFIELD_OTHERNAME   (AEE_ADDRFIELD_USER+0x0A)
#define  AEE_ADDRFIELD_PHOTO       (AEE_ADDRFIELD_USER+0x0B)
#define  AEE_ADDRFIELD_PHOTOURL    (AEE_ADDRFIELD_USER+0x0C)
#define  AEE_ADDRFIELD_POBOX       (AEE_ADDRFIELD_USER+0x0D)
#define  AEE_ADDRFIELD_ADDREXTRA   (AEE_ADDRFIELD_USER+0x0E)
#define  AEE_ADDRFIELD_STREET      (AEE_ADDRFIELD_USER+0x0F)
#define  AEE_ADDRFIELD_UID         (AEE_ADDRFIELD_USER+0x10)
#define  AEE_ADDRFIELD_PUBLICKEY   (AEE_ADDRFIELD_USER+0x11)
#define  AEE_ADDRFIELD_PUBLICKEYSTRING (AEE_ADDRFIELD_USER+0x12)
#endif
#define  AEE_ADDRFIELD_GSM_TONNPI  (AEE_ADDRFIELD_USER+0x13)
#define  AEE_ADDRFIELD_COMPLEX     (AEE_ADDRFIELD_USER+0x14)
#define  AEE_ADDRFIELD_ADDITIONAL_NUMBER   (AEE_ADDRFIELD_USER+0x15)
#define  AEE_ADDRFIELD_ANNAME      (AEE_ADDRFIELD_USER+0x16)
#define  AEE_ADDRFIELD_ANNAME_ID   (AEE_ADDRFIELD_USER+0x17)
#define  AEE_ADDRFIELD_GRPNAME     (AEE_ADDRFIELD_USER+0x18)
#define  AEE_ADDRFIELD_GRPNAME_ID  (AEE_ADDRFIELD_USER+0x19)
#define  AEE_ADDRFIELD_HIDDEN      (AEE_ADDRFIELD_USER+0x1A)

#define  AEE_ADDRFIELD_PHONE_VIDEOCALLERID  (AEE_ADDRFIELD_USER + 0x1B)
#define  AEE_ADDRFIELD_PRIVATE_EMAIL1       (AEE_ADDRFIELD_USER + 0x1C)
#define  AEE_ADDRFIELD_PRIVATE_EMAIL2       (AEE_ADDRFIELD_USER + 0x1D)

/************************************************************************************
Business tab fields
************************************************************************************/
#define  AEE_ADDRFIELD_BUSINESS_EMAIL1   (AEE_ADDRFIELD_USER + 0x1E)
#define  AEE_ADDRFIELD_BUSINESS_EMAIL2   (AEE_ADDRFIELD_USER + 0x1F)
#define  AEE_ADDRFIELD_BUSINESS_URL      (AEE_ADDRFIELD_USER + 0x20)
#define  AEE_ADDRFIELD_PHONE_BUSINESS_FAX (AEE_ADDRFIELD_USER + 0x21)
#define  AEE_ADDRFIELD_BUSINESS_STREET    (AEE_ADDRFIELD_USER + 0x22)
#define  AEE_ADDRFIELD_BUSINESS_CITY      (AEE_ADDRFIELD_USER + 0x23)
#define  AEE_ADDRFIELD_BUSINESS_STATE     (AEE_ADDRFIELD_USER + 0x24)
#define  AEE_ADDRFIELD_BUSINESS_COUNTRY   (AEE_ADDRFIELD_USER + 0x25)
#define  AEE_ADDRFIELD_BUSINESS_POSTALCODE (AEE_ADDRFIELD_USER + 0x26)
#define  AEE_ADDRFIELD_BUSINESS_NOTES     (AEE_ADDRFIELD_USER + 0x27)

/************************************************************************************
Personal tab fields
************************************************************************************/
//AEE_ADDRFIELD_VOICETAG
#define  AEE_ADDRFIELD_RINGTONE_AUDIO     (AEE_ADDRFIELD_USER + 0x28)
#define  AEE_ADDRFIELD_RINGTONE_VIDEO     (AEE_ADDRFIELD_USER + 0x29)

//#define  AEE_ADDRFIELD_BIRTHDAY           (AEE_ADDRFIELD_USER + 0x2A)
#define  AEE_ADDRFIELD_EVENT_UID          (AEE_ADDRFIELD_USER + 0x2B)

/************************************************************************************
Fields Used to store default information
************************************************************************************/
#define  AEE_ADDRFIELD_DEFAULT_PHONE       (AEE_ADDRFIELD_USER + 0x2C)
#define  AEE_ADDRFIELD_DEFAULT_EMAIL       (AEE_ADDRFIELD_USER + 0x2D)
#define  AEE_ADDRFIELD_DEFAULT_ADDRESS     (AEE_ADDRFIELD_USER + 0x2E)
#define  AEE_ADDRFIELD_DEFAULT_URL         (AEE_ADDRFIELD_USER + 0x2F)
#define  AEE_ADDRFIELD_DEFAULT_FAX         (AEE_ADDRFIELD_USER + 0x30)
#define  AEE_ADDRFIELD_DISTRIBUTION_LIST   (AEE_ADDRFIELD_USER + 0x31)

/************************************************************************************
Additional Fields
************************************************************************************/
#define  AEE_ADDRFIELD_PRIVATEPHONE_VOICETAG         (AEE_ADDRFIELD_USER + 0x32)
#define  AEE_ADDRFIELD_BUSINESSPHONE_VOICETAG        (AEE_ADDRFIELD_USER + 0x33)
#define  AEE_ADDRFIELD_PRIVATEMOBILE_VOICETAG        (AEE_ADDRFIELD_USER + 0x34)
#define  AEE_ADDRFIELD_BUSINESSMOBILE_VOICETAG       (AEE_ADDRFIELD_USER + 0x35)
#define  AEE_ADDRFIELD_VIDEOTELEPHONY                (AEE_ADDRFIELD_USER + 0x36)
#define  AEE_ADDRFIELD_USEDCOUNT                     (AEE_ADDRFIELD_USER + 0x37)
#define  AEE_ADDRFIELD_BCARD                         (AEE_ADDRFIELD_USER + 0x38)
//#define  AEE_ADDRFIELD_GROUP                         (AEE_ADDRFIELD_USER + 0x39)
#define  AEE_ADDRFIELD_DLISTS                        (AEE_ADDRFIELD_USER + 0x3A)
#define  AEE_ADDRFIELD_CALL_METHOD                   (AEE_ADDRFIELD_USER + 0x3B)
#define  AEE_ADDRFIELD_AOC                           (AEE_ADDRFIELD_USER + 0x3C)

/************************************************************************************
Field for phone unique ID
************************************************************************************/
#define  AEE_ADDRFIELD_PH_UID                        (AEE_ADDRFIELD_USER + 0x3D)

/************************************************************************************
Field for different emergency numbers
************************************************************************************/
#define AEE_ADDRFIELD_HARDCODED_ECC                  (AEE_ADDRFIELD_USER + 0x3E)
#define AEE_ADDRFIELD_NV_ECC                         (AEE_ADDRFIELD_USER + 0x3F)
#define AEE_ADDRFIELD_SIM1_ECC                       (AEE_ADDRFIELD_USER + 0x40)
#define AEE_ADDRFIELD_SIM2_ECC                       (AEE_ADDRFIELD_USER + 0x41)
#define AEE_ADDRFIELD_ECC_SERVICE_CATEGORY           (AEE_ADDRFIELD_USER + 0x42)

/* more user fields */
#define  AEE_ADDRFIELD_PRIVATE_ADDITIONALINFO        (AEE_ADDRFIELD_USER + 0x43)
#define  AEE_ADDRFIELD_BUSINESS_ADDITIONALINFO       (AEE_ADDRFIELD_USER + 0x44)

#define  AEE_ADDRFIELD_PHONE_VIDEOTELEPHONY          (AEE_ADDRFIELD_USER + 0x45)
#define  AEE_ADDRFIELD_PREFER_VT                     (AEE_ADDRFIELD_USER + 0x46)

/* Emergency numbers from network */
#define AEE_ADDRFIELD_NETWORK_ECC                    (AEE_ADDRFIELD_USER + 0x47)

#ifndef AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE
/* Maybe defined in BREW SDK AEEAddrBook.h */
/* Primary Phone ID */
#define AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE          (AEE_ADDRFIELD_USER + 0x48)
#endif /* AEE_ADDRFIELD_FIELDID_PRIMARY_PHONE */
/************************************************************************************
Fields for UIOneRefUI Contact App
************************************************************************************/
#define  AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_FIELD_ATTRIBUTE   (AEE_ADDRFIELD_USER + 0x50)
#define  AEE_ADDRFIELD_APP_DEFINED_STRING_FIELD_ATTRIBUTE    (AEE_ADDRFIELD_USER + 0x51)
#define  AEE_ADDRFIELD_APP_DEFINED_BOOLEAN_RECORD_ATTRIBUTE  (AEE_ADDRFIELD_USER + 0x52)
#define  AEE_ADDRFIELD_APP_DEFINED_STRING_RECORD_ATTRIBUTE   (AEE_ADDRFIELD_USER + 0x53) 


#ifdef FEATURE_RUIM_PHONEBOOK
//Index of ID in cache from where the RUIM ADN records will start.
//All the IDs below this number will be used for addresses in 
//EFS address book.
#define AEE_RUIM_ADN_INDEX_START  0x4000

#endif /*FEATURE_RUIM_PHONEBOOK*/

#ifdef FEATURE_FDN
//Index of ID in cache from where the RUIM FDN records will start.
#define AEE_RUIM_FDN_INDEX_START  0x8000
#endif /* FEATURE_FDN */

// Used to mask off all the special R-UIM bits in the record ID
#define AEE_RUIM_INDEX_MASK       0xC000

//Special id for the phone number which is on RUIM. This id will be 
//used to OR with the original field id to store temporarily before
//shown on the menuitem
#define AEEDB_FT_RUIM_PHONE   0x8000
            
#ifdef FEATURE_FDN
// Special wild card character that matches any character. 
//
// Used for matching FDNs against a dialed number.  The function
// nvruim_phbook_ascii_to_bcd() in nvruim.c will map this character
// to the BCD digit 0x0D.  0x0D is defined as the "Wild" value 
// in TS 02.07 [3]
#define AEE_RUIM_FDN_WILDCARD_AECHAR  ( (AECHAR) 'P' )
#endif /* FEATURE_FDN */
            

/*===========================================================================

                    FUNCTION DECLARATIONs

===========================================================================*/

int OEMAddr_EnumRecInitEx(AEEAddrCat wCategory, AEEAddrFieldID wFieldID, void *pData, uint16 wDataSize,uint32 flags);

boolean OEMAddr_HaveUIDsChanged(void);

void OEMAddr_ValidateUIDChanges(void);

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

////
// Structure to hold the record info
//

typedef struct 
{
   int nRecordsInUse;   // Number of records in use for category
   int nRecordsFree;    // Number of free records for category
   int nTextLength;     // Length of the text for this category
   int nNumberLength;   // Length of the number for this category
} AEEAddrCatRecInfo;


#define INHERIT_IAddrBookExt(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*GetRecInfo)(iname *po,AEEAddrCat wCategory,AEEAddrCatRecInfo *pCatRecInfo) \
 
AEEINTERFACE_DEFINE(IAddrBookExt);

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

/*=============================================================================
FUNCTION: OEMAddrBookExt_GetRecInfo

DESCRIPTION:  Retrieve extended record information pertaining to the phonebook
              category.

PARAMETERS:
   po          [in]: Pointer to the IAddrBookExt object
   wCategory   [in]: Address book category
   pCatRecInfo[out]: Record information for the specified category

RETURN VALUE:
   int: BREW error code

COMMENTS:
   IAddrBookExt is instantiated using AEECLSID_ADDRBOOKEXT defined in the 
   "AddrBookExt.bid" header.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline int IADDRBOOKEXT_GetRecInfo(IAddrBookExt *po,
                                            AEEAddrCat wCategory,
                                            AEEAddrCatRecInfo *pCatRecInfo)
{
   return AEEGETPVTBL((po),IAddrBookExt)->GetRecInfo(po,wCategory,pCatRecInfo);
}


/*=============================================================================
FUNCTION: IADDRBOOKEXT_EnumRecInitEx

DESCRIPTION: This function is used for searching the address book for specific records. It is
   also used for sequentially retrieving all the records in the database.
   This fundtion initializes the enumeration of records in the address book
   based on a specific search criteria.Once enumeration has been initialized, the function
   OEMAddr_EnumNextRec() will be used to iterate through the records that match
   this search criteria.

PARAMETERS:
   po          [in]: Pointer to the IAddrBookExt object

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

RETURN VALUE:
   AEE_SUCCESS: If enumeration was successfully initialized.
   EFAILED: Enumeration failed. 

COMMENTS:
   IAddrBookExt is instantiated using AEECLSID_ADDRBOOKEXT defined in the 
   "AddrBookExt.bid" header.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static __inline int IADDRBOOKEXT_EnumRecInitEx(IAddrBook *po,
                                               AEEAddrCat wCategory, 
                                               AEEAddrFieldID wFieldID, 
                                               void *pData, 
                                               uint16 wDataSize,
                                               uint32 flags)
{
   return OEMAddr_EnumRecInitEx(wCategory, wFieldID, pData, wDataSize, flags);

}


/*===========================================================================
FUNCTION IADDRBOOKEXT_HaveUIDsChanged

DESCRIPTION
  This function is used to see if the phone Unique ID's have changed.
  If the unique ID changes aren't validated (eg if EFS was wiped), writes 
  will fail until someone validates the unique ID's. 
  (If FEATURE_PBM_UNIQUE_ID is on)

PARAMETERS:
   po          [in]: Pointer to the IAddrBookExt object

RETURN VALUE:
  TRUE: if UIDs have changed
  FALSE: if UIDs have not changed

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  IADDRBOOKEXT_ValidateUIDChanges
=============================================================================*/
static __inline boolean IADDRBOOKEXT_HaveUIDsChanged(IAddrBook *po)
{
   return OEMAddr_HaveUIDsChanged();
}


/*===========================================================================
FUNCTION IADDRBOOKEXT_ValidateUIDChanges

DESCRIPTION
  This function is used to acknowledge that the unique IDs have changed.
  If the unique ID changes aren't validated (eg if EFS was wiped), writes 
  will fail until someone validates the unique ID's. 
  (If FEATURE_PBM_UNIQUE_ID is on)

PARAMETERS:
   po          [in]: Pointer to the IAddrBookExt object

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  IADDRBOOKEXT_HaveUIDsChanged
=============================================================================*/
static __inline void IADDRBOOKEXT_ValidateUIDChanges(IAddrBook *po)
{
   OEMAddr_ValidateUIDChanges();
}

#endif
