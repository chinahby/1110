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


        Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEAddrBook.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
// Address book category for Abbreviated Dialing Numbers (on the R-UIM/SIM)
#ifdef FEATURE_RUIM_PHONEBOOK
#define  AEE_ADDR_CAT_RUIM_ADN        (AEE_ADDR_CAT_USER_DEFINED+1)
#endif /* FEATURE_RUIM_PHONEBOOK */

#ifdef FEATURE_FDN
// Address book category for Fixed Dialing Numbers (on the R-UIM/SIM)
#define  AEE_ADDR_CAT_RUIM_FDN        (AEE_ADDR_CAT_USER_DEFINED+3)
#endif /* FEATURE_FDN */

// Address book category for the Speed Dial List
#define  AEE_ADDR_CAT_SPEEDDIAL       (AEE_ADDR_CAT_USER_DEFINED+2)

//AEE_ADDRFIELD_SEARCH_PHONE_NUM field should be used only for search
//purpose and it does not store meaningful field. When some one searches on
//this field it will see if the search string apppears anywhere in 
//the phone number in any phonenumber type. (like strstr)
#define  AEE_ADDRFIELD_SEARCH_PARTIAL_PHONE_NUM (AEE_ADDRFIELD_USER_DEFINED+1)

//AEE_ADDRFIELD_SEARCH_PHONE_NUM field should be used only for search
//purpose and it does not store meaningful field. When some one searches on
//this field it will see if the exact string matches the phone number
//in any phonenumber type. (like strcmp)
#define  AEE_ADDRFIELD_SEARCH_EXACT_PHONE_NUM (AEE_ADDRFIELD_USER_DEFINED+2)

//AEE_ADDRFIELD_SEARCH_PARTIAL_NAME field should be used only for search
//purpose and it does not store meaningfulfield. When some one searches on
//this field it will match name partially with all the contact names
#define  AEE_ADDRFIELD_SEARCH_PARTIAL_NAME (AEE_ADDRFIELD_USER_DEFINED+3)

//AEE_ADDRFIELD_SPEEDDIAL_SLOTINDEX  field will be used to save the speed dial
//slot index. Range is from 2 to 99. 1 is reserved for voice mail
#define  AEE_ADDRFIELD_SPEEDDIAL_SLOTINDEX (AEE_ADDRFIELD_USER_DEFINED+4)

//AEE_ADDRFIELD_PHONE_SPEEDDIAL field will be used  to save the speed dial 
//phone number.
#define  AEE_ADDRFIELD_PHONE_SPEEDDIAL     (AEE_ADDRFIELD_USER_DEFINED+5)

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
            
#endif
