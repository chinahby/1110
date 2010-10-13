/*******************************************************************************
********************************************************************************

                  COPYRIGHT 1998-2005 TEGIC COMMUNICATIONS

                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

     This software is supplied under the terms of a license agreement
     or non-disclosure agreement with Tegic Communications and may not
     be copied or disclosed except in accordance with the terms of that
     agreement.

********************************************************************************

     File Name:   xxt9oem.h

     Description: This file contains the definitions specific to any
                  particular T9 embedded environment.
               

     Version:	  7.3                                                       
********************************************************************************
********************************************************************************/

#ifndef T9OEM_H
#define T9OEM_H

/* Don't mangle function names if compiled under C++ */
#if defined(__cplusplus)
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *  Define OEMID
 *----------------------------------------------------------------------------*/
#define T9OEMID 20055


/*----------------------------------------------------------------------------
 *  Define number of bytes in each T9 data element
	For Customer vesrion, these should be defined.
 *----------------------------------------------------------------------------*/
#define T9SYMBOLWIDTH   2   /*Indicates 1 or 2 byte character encoding*/
#define T9MINORWIDTH    2	/*Indicates the target machine's word size in bytes,*/ 
							/*valid values are 1, 2 and 4.*/
     


/*----------------------------------------------------------------------------
 *  Define function and data prefixes for special memory models
 *----------------------------------------------------------------------------*/
/* #define _64K_LIMIT_ */
/* #define T9FARDATA  __far	*/	/* data out side the segment */
/* #define T9FARCALL  __far */		/* function out side of segment */
/* #define T9NEARCALL __near */		/* function within the same segment */
/* #define T9NEARDATA __near */ 	/* data within the same segment */


/*----------------------------------------------------------------------------
 *  Define maximum udb word size
 *  NOTE:  This should not be set to less than 32 or more than 64.
 *----------------------------------------------------------------------------*/

#define T9MAXUDBWORDSIZE    64  /* Max udb word length supported by T9 system */


/*----------------------------------------------------------------------------
 *  Define maximum mdb word size
 *  NOTE: The maximum MDB word size isn't necessarily the same as the maximum
 *       UDB word size.
 *  NOTE:  This should not be set to less than 32 or more than 256.
 *----------------------------------------------------------------------------*/

#define T9MAXMDBWORDSIZE    64



/*----------------------------------------------------------------------------
 *  Define maximum cdb matches
 *----------------------------------------------------------------------------*/
#define T9MAXCDBMATCHES     0x06


/*----------------------------------------------------------------------------
 *	For our external demo, we need latin1 all the time.
 ----------------------------------------------------------------------------*/
#define T9CSLatin1
/* #define T9CSGSM */

/*----------------------------------------------------------------------------
 *	Turn on all the Collating sequences that are not keypad dependent.
 ----------------------------------------------------------------------------*/
#define T9CSGeneralPunctuation   /* Farsi needs zero-width space */
#define T9CSCurrencySymbols      /* Mostly to get the Euro. */

 #define T9CSLatinA        /* 128 extended characters */
 #define T9CSLatinB        /* Next 128 extended characters for Vietnamese*/
 #define T9CSDiacritics    /* Combining diacritic marks, for Vietnamese */

#define T9CSHebrew        /* Collating sequence for Hebrew */ // Gemsea add the define statement on 080819
/* #define T9CSHebrewAlt3 */

 #define T9CSArabic        /* Arabic characters */
/* #define T9CSArabicAlternate */
/* #define T9CSArabicAlt3 */

 #define T9CSCyrillic      /* Cyrillic characters */
 /* #define T9CSCyrillicEven */    /* Cyrillic characters */

/* #define T9CSDevanagari */   /* Devanagari (i.e. Hindi) characters */
 #define T9CSDevanagariAlternate 

/* #define T9CSBengali */     /* Bengali characters */

/* #define T9CSTamil */       /* Tamil characters */

/* #define T9CSGreek  */       /* Greek characters in the U+0380 range */
/* #define T9CSGreekAlphabetic */

 #define T9CSThai          /* Thai characters */
/* #define T9CSThaiAIS */
/* #define T9CSThaiHutchison */
/* #define T9CSThaiAlternate */



/*	Korean */
#if defined(T9KOREAN)
#define T9CSKoreanChun
#endif


/*----------------------------------------------------------------------------
 *	Chinese defaults.
 ----------------------------------------------------------------------------*/

#define T9ALPHABETIC
#define T9CHINESE


#if defined(T9CHINESE)
#define T9CCSPELLBUFMAXSIZE         128 /* Size of Chinese spell buffer   */
#define T9CCKEYBUFMAXSIZE           55  /* Size of Chinese key buffer     */
#define T9CCSELECTPAGEMAXSIZE       9   /* Size of Chinese selection page */
#endif



#if defined(__cplusplus)
}
#endif

#endif /* T9OEM_H */

/*---------------------------------- eof ---------------------------------*/
