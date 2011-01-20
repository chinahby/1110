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


        Copyright ?2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //depot/asic/qsc60x0/apps/StaticExtensions/Inc/AEEAddrBookExt.h#1 $
$DateTime: 2006/03/17 09:59:07 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
                     Rename the QCT IADDRBOOK_EnumRecInitEx() to
                     IADDRBOOKEXT_EnumRecInitEx
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

/* Hack: extend IAddrBook differently */
#include "OEMAddrBook.h"

#if MIN_BREW_VERSION(3,1)
#define IADDRBOOKEXT_EnumRecInitEx(p,cat,field,data,wLen,flags) \
        OEMAddr_EnumRecInitEx(cat, field, data, wLen, flags)
#else
#define IADDRBOOK_EnumRecInitEx(p,cat,field,data,wLen,flags) \
        OEMAddr_EnumRecInitEx(cat, field, data, wLen, flags)
#endif

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

  AEE_ADDR_CAT_OEM_COUNT,  // Tells us how many OEM categories there are
  AEE_ADDR_CAT_OEM_MAX = 0xFF // Forces type to uint16
} oem_addr_cat_e_type;

/* NOTE: One-to-one mapping with PBM values */
/* Flags Masks */
#define  ADDRREC_CASE_MASK                    0x1
#define  ADDRREC_SEARCHTYPE_MASK              0x6
#define  ADDRREC_DATATYPE_MASK                0x8
#define  ADDRREC_FIELDID_MASK                 0x10

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
#define  ADDRREC_FIELDID_PHONE                16

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

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/



#endif
