#ifndef __QXUTFCONVERTER_H__
#define __QXUTFCONVERTER_H__

/*===========================================================================


                           Q x U T F C O N V E R T 
             
                            H E A D E R  F I L E

FILE:  QxUTFConvert.h

DESCRIPTION

	This header file defines the interface to QxUTFConvert
	QxUTFConvert defines an interface for conversion between various
	transformation formats of Unicode characters.

	Following are the conversions supported as part of QxUTFConvert interface.
		UTF-32 to UTF-16
		UTF-32 to UTF-8
		UTF-16 to UTF-32
		UTF-16 to UTF-8
		UTF-8  to UTF-32
		UTF-8  to UTF-16

	Each of these routines takes pointers to input buffers and output
    buffers.  The input buffers are const.

    Each routine converts the text between *ppSourceStart and pSourceEnd,
    putting the result into the buffer between *ppTargetStart and
    pTargetEnd. Note: the end pointers are *after* the last item: e.g. 
    *(pSourceEnd - 1) is the last item.

    The return result indicates whether the conversion was successful,
    and if not, whether the problem was in the source or target buffers.
    (Only the first encountered problem is indicated.)

    After the conversion, *ppSourceStart and *ppTargetStart are both
    updated to point to the end of last text successfully converted in
    the respective buffers.

    Input parameters:
	ppSourceStart - pointer to a pointer to the source buffer.
		The contents of this are modified on return so that
		it points at the next thing to be converted.
	ppTargetStart - similarly, pointer to pointer to the target buffer.
	pSourceEnd, pTargetEnd - respectively pointers to the ends of the
		two buffers, for overflow checking only.

    These conversion functions take a ConversionFlags argument. When this
    flag is set to strict, both irregular sequences and isolated surrogates
    will cause an error.  When the flag is set to lenient, both irregular
    sequences and isolated surrogates are converted.

    Whether the flag is strict or lenient, all illegal sequences will cause
    an error return. This includes sequences such as: <F4 90 80 80>, <C0 80>,
    or <A0> in UTF-8, and values above 0x10FFFF in UTF-32. Conformant code
    must check for illegal sequences.

    When the flag is set to lenient, characters over 0x10FFFF are converted
    to the replacement character; otherwise (when the flag is set to strict)
    they constitute an error.

    Output parameters:
	The value "sourceIllegal" is returned from some routines if the input
	sequence is malformed.  When "sourceIllegal" is returned, the source
	value will point to the illegal value that caused the problem. E.g.,
	in UTF-8 when a sequence is malformed, it points to the start of the
	malformed sequence.  


PUBLIC METHODS 
	ConversionResult ConvertUTF32toUTF16 ()
	ConversionResult ConvertUTF32toUTF8 ()
	ConversionResult ConvertUTF16toUTF8 ()
	ConversionResult ConvertUTF16toUTF32 ()
	ConversionResult ConvertUTF8toUTF16 ()
	ConversionResult ConvertUTF8toUTF32 ()
PRIVATE METHODS
	Boolean isLegalUTF8 ()
	Boolean isLegalUTF8sequence ()
	
	
INITIALIZATION AND SEQUENCING REQUIREMENTS
	NONE
  



		  (c) COPYRIGHT <2005> QUALCOMM Incorporated.
                      All Rights Reserved.
                    QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/src/QxUTFconverter.h#1 $ 
  $DateTime: 2007/08/20 13:29:56 $ 2005/08/26 15:21:57
  $Author: jmiera $   


when		who			what, where, why
--------	---			----------------------------------------------------
05/17/07        avm                     Fixed Compiler Warnings.
03/15/06        rv			Fixed Compiler warnings
10/11/05	Mwa			Initial Version
			

===========================================================================*/



/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/

#include <stdio.h>
#include <stdlib.h>

/*-------------------------------------------------------------------------
					 Global Constant Definitions
--------------------------------------------------------------------------- */

typedef unsigned long   UTF32;	   /* at least 32 bits */
typedef unsigned short	UTF16;	 /* at least 16 bits */
typedef unsigned char	  UTF8;	     /* typically 8 bits */
typedef unsigned char	  Boolean;   /* 0 or 1 */


#define UNI_SUR_HIGH_START  (UTF32)0xD800		// UTF16 high surrogate start value
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF		// UTF16 high surrogate end value
#define UNI_SUR_LOW_START   (UTF32)0xDC00		// UTF16 low surrogate start value
#define UNI_SUR_LOW_END     (UTF32)0xDFFF		// UTF32 low surrogate end value
#define FALSE	                     0				// set boolean 0 as false						
#define TRUE	                     1				// set boolean 1 as true 
#define ENCODE_BIT_SHIFT           6		    // 6 bit shifter for encoding.

// Some fundamental constants  from UTF specification 

#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD		// Default character setting.
#define UNI_MAX_BMP          (UTF32)0x0000FFFF		// Maximum Binary Multilingal Plan.
#define UNI_MAX_UTF16        (UTF32)0x0010FFFF		// Maximum UTF16 character.
#define UNI_MAX_UTF32        (UTF32)0x7FFFFFFF		// Maximum UTF32 character.
#define UNI_MAX_LEGAL_UTF32  (UTF32)0x0010FFFF		// LEGAL UTF32

// Conversion Result
typedef enum {
	UTF_CONV_RES_OK,				      // conversion successful 
	UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end 
	UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for conversion
	UTF_CONV_RES_SRC_ILLEGAL,		  // source sequence is illegal or malformed 
	UTF_CONV_RES_SIZZER = 0x0FFFFFFF	
		
} ConversionResult;

// Flag setting.
typedef enum {							
	UTF_CONV_FLAG_MIN,					// Flag min value.
	UTF_CONV_FLAG_STRICT,				// it is set: both irregular sequence &
										          // isolated surrogates will cause an error.
	UTF_CONV_FLAG_LENIENT,			// it is set: both irregular sequece &
										          // isolated surrogates are converted.
	UTF_CONV_FLAG_SIZER = 0x0FFFFFFF	// Flag max value.

} ConversionFlagType;


/*--------------------------------------------------------------------------
				     Static Data Definitions
 ---------------------------------------------------------------------------*/

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */

static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */

static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
		     0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/*===========================================================================

                           CLASS DEFINITION

CLASS:
	QxUTFConvert


DESCRIPTION:
	Following are the conversions supported as part of QxUTFConvert class. 

	Conversion UTF32toUTF16 
	Conversion UTF32toUTF8 
	Conversion UTF16toUTF8 
	Conversion UTF16toUTF32 
	Conversion UTF8toUTF16 
	Conversion UTF8toUTF32 


PUBLIC FUNCTIONS 

	ConversionResult ConvertUTF32toUTF16 ()
	ConversionResult ConvertUTF32toUTF8 ()
	ConversionResult ConvertUTF16toUTF8 ()
	ConversionResult ConvertUTF16toUTF32 ()
	ConversionResult ConvertUTF8toUTF16 ()
	ConversionResult ConvertUTF8toUTF32 ()


PRIVATE FUNCTION
	
	Boolean isLegalUTF8
	

===========================================================================*/

class QxUTFConvert
{
public:

    //------------------------------------------------------------------------
    //                           PUBLIC TYPES
    //------------------------------------------------------------------------

	static const int   halfShift  = 10;			/* used for shifting by 10 bits */
	static const UTF32 halfBase = 0x0010000UL;	/* Max value of UTF16 */
	static const UTF32 halfMask = 0x3FFUL;		/* use for masking of bit*/


	//------------------------------------------------------------------------
    //                           PUBLIC METHODS
	// ------------------------------------------------------------------------

	/* Constructor for QxUTFConvert  
	 *	This function creates QxUTFConvert object	
	 */

	QxUTFConvert() {}


	 /* Desctructor for QxUTFConvert  
	  *	This function delete the QxUTFConvert object 
	  */

	~QxUTFConvert(){}

/*========================================================================= 

FUNCTION:  QxUTFConvert::ConvertUTF32toUTF16

DESCRIPTION
	This method converts the input buffer of UTF32 formatted to 
	output buffer of UTF16 formatted.

INPUT PARAMETERS
	UTF32 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF32 *pSourceEnd	: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type.
	

OUTPUT PARAMETERS
	UTF16 ** ppTargetStart: Pointer to pointer to the target buffer.
	UTF16 *pTargetEnd	: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following.
		UTF_CONV_RES_OK,				// conversion successful. 
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for 
										   conversion. 
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed. 
		
SIDE EFFECTS:
	N/A

========================================================================*/

	ConversionResult ConvertUTF32toUTF16 (
		const UTF32** ppSourceStart, const UTF32* pSourceEnd, 
		UTF16** ppTargetStart, UTF16* pTargetEnd, ConversionFlagType bFlags);

/*==============================================================
FUNCTION:  QxUTFConvert::ConvertUTF16toUTF32

DESCRIPTION
	This method converts the input buffer of UTF16 formatted to 
	output buffer of UTF32 formatted.

INPUT PARAMETERS
	UTF16 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF16 *pSourceEnd	: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type
	

OUTPUT PARAMETERS
	UTF32 ** ppTargetStart: Pointer to pointer to the target buffer.
	UTF32 *pTargetEnd	: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following.
		UTF_CONV_RES_OK,				// conversion successful.
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for 
										// conversion. 
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed. 

SIDE EFFECTS:
	N/A

==============================================================*/

	ConversionResult ConvertUTF16toUTF32 (
		const UTF16** ppSourceStart, const UTF16* pSourceEnd, 
		UTF32** ppTargetStart, UTF32* pTargetEnd, ConversionFlagType bFlags);

/*===================================================================
FUNCTION:  QxUTFConvert::ConvertUTF16toUTF8

DESCRIPTION
	This method converts the input buffer of UTF16 formatted to 
	output buffer of UTF8 formatted.

INPUT PARAMETERS
	UTF16 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF16 *pSourceEnd	: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type.
	

OUTPUT PARAMETERS
	UTF8 ** ppTargetStart	: Pointer to pointer to the target buffer.
	UTF8 *pTargetEnd		: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following
		UTF_CONV_RES_OK,				// conversion successful. 
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for 
										// conversion. 
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed. 

SIDE EFFECTS:
	N/A

===================================================================*/

	ConversionResult ConvertUTF16toUTF8 (
		const UTF16** ppSourceStart, const UTF16* pSourceEnd, 
		UTF8** ppTargetStart, UTF8* pTargetEnd, ConversionFlagType bFlags);
	
/*==============================================================
FUNCTION:  QxUTFConvert::ConvertUTF8toUTF16

DESCRIPTION
	This method converts the input buffer of UTF8 formatted to 
	output buffer of UTF16 formatted.

INPUT PARAMETERS
	UTF8 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF8 *pSourceEnd		: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type
	

OUTPUT PARAMETERS
	UTF16 ** ppTargetStart: Pointer to pointer to the target buffer.
	UTF16 *pTargetEnd	: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following.
		UTF_CONV_RES_OK,				// conversion successful. 
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for
										// conversion.
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed.

SIDE EFFECTS:
	N/A

==============================================================*/

	ConversionResult ConvertUTF8toUTF16 (
		const UTF8** ppSourceStart, const UTF8* pSourceEnd, 
		UTF16** ppTargetStart, UTF16* pTargetEnd, ConversionFlagType bFlags);

/*=========================================================================
FUNCTION:  QxUTFConvert::ConvertUTF8toUTF32

DESCRIPTION
	This method converts the input buffer of UTF8 formatted to 
	output buffer of UTF32 formatted.

INPUT PARAMETERS
	UTF8 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF8 *pSourceEnd		: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type.
	

OUTPUT PARAMETERS
	UTF32 ** ppTargetStart: Pointer to pointer to the target buffer.
	UTF32 *pTargetEnd	: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following.
		UTF_CONV_RES_OK,				// conversion successful. 
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for 
										// conversion. 
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed. 

SIDE EFFECTS:
	N/A

==============================================================================*/
	
	ConversionResult ConvertUTF8toUTF32 (
		const UTF8** ppSourceStart, const UTF8* pSourceEnd, 
		UTF32** ppTargetStart, UTF32* pTargetEnd, ConversionFlagType bFlags);

/*==============================================================
FUNCTION:  QxUTFConvert::ConvertUTF32toUTF8

DESCRIPTION
	This method converts the input buffer of UTF32 formatted to 
	output buffer of UTF8 formatted.

INPUT PARAMETERS
	UTF32 **ppSourceStart	: Pointer to a pointer to the source buffer. The 
						  content of this are modified on return so that
						  it points at the next thing to be converted.
	UTF32 *pSourceEnd	: Pointer to the end of source buffer. For 
						  overflow checking only.
	ConversionFlagType	: Conversion Flag type
	

OUTPUT PARAMETERS
	UTF8 ** ppTargetStart	: Pointer to pointer to the target buffer.
	UTF8 *pTargetEnd		: Pointer to the end of target buffer. For
						  overflow checking only.
	

RETURN VALUE 
	ConversionResult	: ConversionResult type set to one of following.
		UTF_CONV_RES_OK,				// conversion successful. 
		UTF_CONV_RES_SRC_EXHAUSTED,		// partial character in source, but hit end. 
		UTF_CONV_RES_DEST_EXHAUSTED,	// insufficient room in target buffer for 
										// conversion.
		UTF_CONV_RES_SRC_ILLEGAL,		// source sequence is illegal or malformed. 

SIDE EFFECTS:
	N/A

==============================================================*/
	ConversionResult ConvertUTF32toUTF8 (
		const UTF32** ppSourceStart, const UTF32* pSourceEnd, 
		UTF8** ppTargetStart, UTF8* pTargetEnd, ConversionFlagType bFlags);
	
	



	/*------------------------------------------------------------------------
                               PRIVATE METHODS
	 ------------------------------------------------------------------------*/
private:

/*==============================================================
FUNCTION:  QxUTFConvert::isLegalUTF8

DESCRIPTION
  Utility routine to tell whether a sequence of bytes is legal UTF-8.
  This must be called with the length pre-determined by the first byte.
  If not calling this from ConvertUTF8to*, then the length can be set by:
  length = trailingBytesForUTF8[*source]+1;
  and the sequence is illegal right away if there aren't that many bytes
  available. If presented with a length > 4, this returns false.  The Unicode
  definition of UTF-8 goes up to 4-byte sequences.	

INPUT PARAMETERS
	UTF8 *source	: Pointer to source buffer.
	sourceLength	: source length.

RETURN VALUE:
	boolean value(T/F).

SIDE EFFECTS:
	N/A

==============================================================*/
	
	Boolean isLegalUTF8(const UTF8 *pSource, int length);
	
/*==============================================================
FUNCTION:  QxUTFConvert::isLegalUTF8Sequence

DESCRIPTION
	Exported function to return whether a UTF-8 sequence is legal or not.
	This is not used here; 

INPUT PARAMETERS:
	UTF8 *source	: pointer to source buffer.
	UTF8 *pSourceEnd	: pointer to the end of source buffer.

RETURN VALUE:
	boolean value (T/F).

SIDE EFFECTS:
	N/A

==========================================================================*/

	Boolean isLegalUTF8Sequence(const UTF8 *pSource, const UTF8 *pSourceEnd);			


};

#endif		// __QXUTFCONVERTER_H__

