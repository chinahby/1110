/*
  ========================================================================

  FILE:          AEETextFormatter.h

  SERVICES:      String composition interface

  PUBLIC CLASSES:ITextFormatter

  DESCRIPTION:   This file describes an interface of language dependent
                 data formatting routines.  These can be used to 
                 compose strings of text according to a specific
                 set of culture specific formatting rules.
  ========================================================================
  ========================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef __AEETEXTFORMATTER__
#define __AEETEXTFORMATTER__

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEStdLib.h"

/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/

// uniquely identify the ITextFormatter interface.
#include "bid\AEECLSID_TEXTFORMATTER.bid"
#include "bid\AEEIID_TEXTFORMATTER.bid"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// ITextFormatter parameters
#define ITEXTFORMATTER_PARM_ILOCALE       1  // language code of default locale
#define ITEXTFORMATTER_PARM_LANG          2  // language code of default language
#define ITEXTFORMATTER_PARM_YEAROFFSET    3  // offset to add to years when computing dates (Buddhist vs. Julian)
// others ...

// ITEXTFORMATTER_FormatText return codes
#define ITEXTFORMATTER_PARTIALSUCCESS     -1

//
// ITextFormatter class
//

#define INHERIT_ITextFormatter(iname)\
   INHERIT_IQueryInterface (iname);\
   int    (*SetParm)       (iname *po, int16 nParmID, int32 p1);\
   int    (*GetParm)       (iname *po, int16 nParmID, int32 *pp1);\
   int    (*FormatText)    (iname *po, AECHAR *bufOut, uint32 *pdwBufSize, const AECHAR *pszFormatString, const char *pszTypeString, ...)

AEEINTERFACE_DEFINE(ITextFormatter);

#define ITEXTFORMATTER_AddRef(p)                      AEEGETPVTBL((p),ITextFormatter)->AddRef((p))
#define ITEXTFORMATTER_Release(p)                     AEEGETPVTBL((p),ITextFormatter)->Release((p))
#define ITEXTFORMATTER_QueryInterface(p,i,o)          AEEGETPVTBL((p),ITextFormatter)->QueryInterface((p),(i),(o))
#define ITEXTFORMATTER_SetParm(p,i,v)                 AEEGETPVTBL((p),ITextFormatter)->SetParm((p),(i),(v))
#define ITEXTFORMATTER_GetParm(p,i,v)                 AEEGETPVTBL((p),ITextFormatter)->GetParm((p),(i),(v))
#define ITEXTFORMATTER_FormatText(p,args)             AEEGETPVTBL((p),ITextFormatter)->FormatText args

// Helper Macros
#define ITEXTFORMATTER_SetLocale(p,l)                 ITEXTFORMATTER_SetParm(p, ITEXTFORMATTER_PARM_ILOCALE,      (int32)l)
#define ITEXTFORMATTER_SetLang(p,l)                   ITEXTFORMATTER_SetParm(p, ITEXTFORMATTER_PARM_LANG,         (int32)l)
#define ITEXTFORMATTER_SetYearOffset(p,o)             ITEXTFORMATTER_SetParm(p, ITEXTFORMATTER_PARM_YEAROFFSET,   (int32)o)

#define ITEXTFORMATTER_GetLocale(p,l)                 ITEXTFORMATTER_GetParm(p, ITEXTFORMATTER_PARM_ILOCALE,      (int32 *)l)
#define ITEXTFORMATTER_GetLang(p,pl)                  ITEXTFORMATTER_GetParm(p, ITEXTFORMATTER_PARM_LANG,         (int32 *)pl)
#define ITEXTFORMATTER_GetYearOffset(p,po)            ITEXTFORMATTER_GetParm(p, ITEXTFORMATTER_PARM_YEAROFFSET,   (int32*)po)

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================

ITextFormatter Parameters

Description:
   These parameters allow the interal behavior of the ITextFormatter object 
   to be changed.  They are used in the ITEXTFORMATTER_SetParm() and 
   ITEXTFORMATTER_GetParm() APIs.

ITEXTFORMATTER_PARM_ILOCALE
   The default ILocale object used by this ITextFormatter object.  The caller
   may retrieve or set the internal ILocale object used by the ITextFormatter 
   with this parameter.

ITEXTFORMATTER_PARM_LANG
   The caller may both inquire as to the default language an ITextFormatter
   uses and set it.  ITEXTFORMATTER_SetParm() will return an error if it is called
   with a lanague code that is not supported on the device.  Four character
   strings such as "enus" and "engb" are expected.

ITEXTFORMATTER_PARM_YEAROFFSET
   Specifies an offset to be applied to the year field of any date generated
   by the ITextFormatter object.  Allows for Julian, Buddhist or other dates.

===================================================================================

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

ITextFormatter Interface

Description: 
  ITextFormatter provides applications with a way to format strings in
  a language-aware way.
   
==============================================================================
Function: ITEXTFORMATTER_AddRef()

This function is inherited from IQI_AddRef().

==============================================================================
Function: ITEXTFORMATTER_Release()

This function is inherited from IQI_Release().

==============================================================================
Function: ITEXTFORMATTER_QueryInterface()

This function is inherited from IQI_QueryInterface().

==============================================================================
Function: ITEXTFORMATTER_SetParm()

Description:
   This function sets the certain ITextFormatter parameters.

Prototype:
    int ITEXTFORMATTER_SetParm(
       ITextFormatter *po,
       int16 nParmID,
       int32 p1
    );

Parameters:
    po:        Pointer to the ITextFormatter Interface.
    nParmID:   Parameter Id
    p1:        Depends on the nParmID parameter

Return Value:
    SUCCESS: Successful. Operation is completed.
    EBADPARM: Bad parm
    EFAILED: General failure

Comments:
    See ITextFormatter Parameters Description for parameter details.

See Also:
    ITEXTFORMATTER_GetParm()

=============================================================================

Function: ITEXTFORMATTER_GetParm()

Description:
    This function gets certain ITextFormatter parameters.

Prototype:
    int ITEXTFORMATTER_GetParm(
       ITextFormatter *po,
       int16 nParmID,
       int32 *pp1
    );

Parameters:
    po:        Pointer to the ITextFormatter Interface.
    nParmID:   Parameter Id
    pp1:        Depends on the nParmID parameter

Return Value:
    SUCCESS: Successful. Operation is completed.
    EBADPARM: Bad parm
    EFAILED: General failure

Comments:
    See ITextFormatter Parameters Description for parameter details.
  
See Also:
    ITEXTFORMATTER_SetParm()

===========================================================================
Function: ITEXTFORMATTER_FormatText()

Description:

   This routine takes a source string consisting of alternating fixed text
   and indexed placeholders and a list of values and composes a result string
   containing the original text intermixed with values formatted appropriately
   for either the default language of the device or the language the caller
   specifies in the format string.

   It is left to the caller to allocate the buffer for the result.  If the
   buffer passed to this routine is too small for the formatting, the 
   formatting will be done as completely as possible and a
   ITEXTFORMATTER_PARTIALSUCCESS error code will be returned. The caller
   may pass a NULL buffer to determine the length of the formatted result.


   __The Type String__

   ITEXTFORMATTER_FormatText() is provided both a format string and a type
   string.  The type string lists the types of the arguments in order that
   are included as variable arguments.  The type string is a string of single
   byte characters and takes the following form:

   [arg1Type][;arg2Type]...[;argNType]

   The types that the ITextFomatter interface understands are the same
   as those that ILocale can process via it's ILOCALE_FormatItem() method.
   These include but may not be limited to the following characters:

   "d"   a signed integer value
   "u"   an unsigned integer value
   "c"   a single character
   "C"   a single AECHAR
   "s"   a string of characters
   "S"   a string of AECHARs
   "p"   a generic pointer value
   "j"   a JulianType date structure
   "J"   a pointer to a JulianType date structure

   For example, if one were to call ITEXTFORMATTER_FormatText() with
   uint32, AECHAR string and julian type arguments, the type string
   would be "u;S;j".  If one were to call this routine with a
   pointer to a julian date structure, a signed integer value and
   a string of characters, the type string would be "J;d;s".

   If the type string includes an unsupported argument type, the
   ITEXTFORMATTER_FormatText() routine will return an error.

   Please refer to the ILocale documentation for valid Argument Types for
   more details.



   __The Format String__

   The caller provides pszFormatString, a string containing text and indexed
   placeholders, that indicates how the string is to be composed.  The
   format string is a string of AECHARs and may be as simple as a single
   character or more complex to format strings for display purposes.
   
   Indexed Place Holders in the pszFormatString reference the arguments in the
   argument list by index and provide additional formatting information such as
   format strings, locale-dependent translations and printf-like modifers that 
   limit the size or specify padding.  Each format item takes the following
   form:

   {index[,alignment][[;language]:formatString]}

   The matching braces ("{" and "}") are required.  To include either of these 
   characters in the fixed text, specify two opening braces "{{" or two closing
   braces "}}".


   Index Component

   The mandatory index component is a number starting from 1 that identifies
   a corresponding argment in the variable list of arguments passed in.  A
   format item such as {1} refers to the first argument in the list, one 
   such as {2} refers to the second item in the list, and so on.  Each format 
   can refer to any parameter.  The parameters may be referred to more than 
   once, and may be referred to in any order.  If an index is specified outside 
   of the range of parameters, ITEXTFORMATTER_FormatText() will return an error.

   For example, the following code:

      char pszNoLike[] = {"do not"};
      char pszSam[] = {"Sam I am"};
      char pszColor[] = {"green");

      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "I {1} like {3} eggs and ham.  I {1} like them, {2}.",
         "s;s;s;",  pszNoLike, pszSam, pszColor));

   produces the following string:
      
      I do not like green eggs and ham.  I do not like them, Sam I am.


   Alignment Component

   The optional alignment component is a signed integer that specifies the 
   minimum and/or maximum width of the formatted data as well as the 
   alignment  within that space.  The alignment component takes the form of

   [-][min][.max]

   '-' indicates padding to the right. 'min' is the minimum number of 
   characters to be inserted.  'max' is the maximum number of characters 
   to be inserted.  This allows for printf like alignment as well as 
   truncation.

   If the length of the formatted data is less than the 'min' value, then
   the formatted data is padded with spaces to the right if the alignment
   value is positive to the left if the alignment value is negative.

   If the length of the formatted data is greater than the 'max' value, 
   the string is truncated to fit in the allowed space.

   Otherwise the required number of bytes are inserted.

   Consider the following examples:

      char pszName[] = "John";
      char pszTitle[] = "Grand PoohBah of all Things Great and Small";

      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Name  = |{1,10}|",  "s", pszName));
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Name  = |{1,-10}|",  "s", pszName));

      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Title = |{1,10}|",  "s", pszTitle));
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Title = |{1,-10}|",  "s", pszTitle));

   Could be used like printf to generate:

      Name  = |      John|
      Name  = |John      |
      Title = |Grand PoohBah of all Things Great and Small|
      Title = |Grand PoohBah of all Things Great and Small|


   While 

      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Name  = |{1,.10}|",  "s", pszName));
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Title = |{1,.10}|",  "s", pszTitle));

   Would be used to truncate like this:

      Name  = |John|
      Title = |Grand Pooh|

   Combinations could be used to do more complicated things like:

      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Name  = |{1,10.10}|",  "s", pszName));
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Title = |{1,10.10}|",  "s", pszTitle));
         
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Name  = |{1,-10.10}|",  "s", pszName));
      ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
         "Title = |{1,-10.10}|",  "s", pszTitle));

      Name  = |      John|
      Title = |Grand Pooh|

      Name  = |John      |
      Title = |Grand Pooh|


   The Language Component

   The 'language' component can be included along with the format string 
   component to force the ITEXTFORMATTER_FormatText() function to apply a 
   specific language's formatting rules while formatting the item.  
   For example, an applicaiton may want to compose the following string:

   "5 cans of Spam will cost you $18.85 or £10.00"

   Assuming the default language of the device is US English or the program 
   has called ITEXTFORMATTER_SetLang(po, 'enus'), the following call can be 
   used to generate that string:

   ITEXTFORMATTER_FormatText(pitf, (pitf, bufOut, &bufSize, 
      "5 cans of Spam will cost you {1:C} or {2;engb:C}."
      "d;d;", dwPriceDollars, dwPricePounds);

   The Language Component is a sequence of four characters consisting of the 
   ISO 639-1 language code (2 characters) followed by the ISO 3166 A2 country 
   code (2 characters).  If no country code is needed to specify the language
   (Korean, for example), two periods are used in place of the country code.
   'enus', 'engb' and 'ko..' are valid examples of language codes. This follows 
   the same format as BREW's Language Codes defined in AEELngCode.h, except 
   periods are used instead of spaces.


   Format String Component

   The optional formatString component consists of standard or custom format 
   specifiers. These, along with the argument's type from the list of type 
   descriptors are used to format the item.  The formatString may specify an 
   exact format for an item or a more ambiguous format string.  If an exact 
   format string is specified it is used, providing the argument type is 
   approrpriate.  If a general formatting string is specified,
   it is passed to the appropriate ILocale object which returns an appropriate
   pattern or does the actual language-specific string composition.

   If a format string is specified that is incompatible with the argument type, 
   this routine will coninute composition and insert nothing into the formatted
   string.  Additionally, this routine will return ITEXTFORMATTER_PARTIALSUCCESS.  
   For example,  if {1:C} is specified, but the first parameter is a JulianType 
   rather than an integer, the format item will be ignored and the error code
   will be returned.

   ITextFormatter relies on ILocale to provide custom string formatting
   and format patterns for different types of data.  Any specifier defined
   by ILocale may be used.  Refer to AEELocale.h for the supported

   
   Custom Numeric Format Strings
   Custom Date Format Strings

   Future versions of this interface may support custom format strings for both
   numeric and date values.  These format specifiers will conform the the
   format pattern strings defined in AEELocale.h.
   
   Note, If you wish to use a custom format string, adjust the ILocale object 
   used by ITextFormatter to provide the pattern you desire before calling
   ITEXTFORMATTER_FormatString().  See the ILocale documentation on how
   to do this.

Prototype:
   uint32 ITEXTFORMATTER_FormatText(
      ITextFormatter *po, 
     (ITextFormatter *po, 
      AECHAR *bufOut, 
      uint32 *pdwBufSize, 
      const AECHAR *pszFormatString,
	   const char *pszTypeString,
   	va_list list));

Parameters:
   po                [in]     pointer to the whatever interface object.  
   bufOut            [in]     buffer to fill with composed strin
   pdwBufSize        [in/out] size of buffer in bytes on input, size of formatted string on output
   pszFormatString   [in]     a string containing text and format specifiers
   pszTypeString     [in]     a string containing a semicolon-separated list of type descriptors
   list              [in]     list of arguments used for formatting.  
                              Argument type must match the data in the pszTypeString parameter.

Return Value:
===pre>
   SUCCESS        (0)
      ITEXTFORMATTER_FormatText() successfully formatted the text.

   EFAILED        (1)
      An error was encountered during text formatting.  Formatting was aborted.

   ITEXTFORMATTER_PARTIALSUCCESS (-1)
      Either the buffer specified was too small, an index in the format string was
      out of range, or a format code wasn't handled.  Formatting was done as
      completely as possible.
===/pre>

Comments:
   The types listed in the pszTypeString parameter MUST MATCH the types of the
   arguments in the va_list.

Usage:

   AECHAR pawFmtString[] = "Today, {5:D}, you can purchase {2,2} cans of {1} for {3:C} or {4:C;engb}."
   int dwLen = ITEXTFORMATTER_FormatText(pitf, 
                                        (pitf, 
                                         NULL, 
                                         0, 
                                         pawFmtString,
	                                      "s;d;d;d;j",
	                                      pszProductName,
	                                      nNumCans,
	                                      dwPriceDollars,
	                                      dwPricePounds,
	                                      date));

   AECHAR *pawText = (AECHAR *)MALLOC(dwLen);

   dwResult = ITEXTFORMATTER_FormatText(pitf, 
                                       (pitf, 
                                        pawText, 
                                        &dwLen, 
                                        pawFmtString,
	                                     "s;d;d;d;j",
	                                     pszProductName,
	                                     nNumCans,
	                                     dwPriceDollars,
	                                     dwPricePounds,
	                                     date));
   
   Displaying the text in pawText would produce:

   Today, November 10, 2004, you can purchase 12 cans of Spam for $18.95 or £10.00.

Side Effects: 
   None

See Also:
   ITEXTFORMATTER_FormatText() return codes
   ILocale
   
===========================================================================

*/
#endif /* __AEETEXTFORMATTER__ */
