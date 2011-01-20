#ifndef _OI_UNICODE_H
#define _OI_UNICODE_H
/**
 * @file
 *
 * This file provides utilities for handling unicode strings.
 *
 */

/** \addtogroup Misc Miscellaneous APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_stddefs.h"

/*
 * A de facto standard for unicode strings is to prefix the string with a
 * non-printing space character to indicate the endian-ness of the string.
 *
 * When used in this way the non-printing space is referred to as the byte order
 * indicator. The unicode conversion and comparison functions below check (and
 * possibly ignore) the byte order indicator.
 */

#define OI_UNICODE_BYTE_ORDER_INDICATOR_BE  ((OI_CHAR16) 0xFFFE)
#define OI_UNICODE_BYTE_ORDER_INDICATOR_LE  ((OI_CHAR16) 0xFEFF)

/**
 * OI_Utf8ToUtf16()
 *
 * @param src       Pointer to a UTF-8 string.
 *
 * @param srcLen    Number of bytes to convert from the source string.
 *
 * @param dst       Pointer to a buffer for holding the resulting UTF-16
 *                  string.
 *
 * @param dstLen    Size of the destination buffer in 16-bit words.
 *
 * @return          OI_OK on success, OI_UNICODE_* on error.
 *
 * This function converts a UTF-8 string to UTF-16.  It is recommended that
 * the destination buffer be at least twice as many bytes in size as the
 * source string.  Due to multi-byte encoding schemes of UTF-8 the number of
 * 16-bit words in the resulting UTF-16 string will not necessarily correspond
 * to the number of bytes in the UTF-8 source string.  Note that the
 * destination string is guaranteed to be null terminated even if the source is
 * not.
 */
OI_STATUS OI_Utf8ToUtf16(const OI_UTF8 *src, OI_INT srcLen,
                         OI_UTF16 *dst, OI_INT dstLen);


/**
 * OI_Utf8ToUtf16()
 *
 * @param src       Pointer to a UTF-16 string.
 *
 * @param srcLen    Number of 16-bit words to convert from the source string.
 *
 * @param dst       Pointer to a buffer for holding the resulting UTF-18
 *                  string.
 *
 * @param dstLen    Size of the destination buffer in bytes.
 *
 * @return          OI_OK on success, OI_UNICODE_* on error.
 *
 * This function converts a UTF-16 string to UTF-8.  It is recommended that
 * the destination buffer be approximately twice as many bytes as the number
 * of 16-bit words in the source string.  Due to multi-byte encoding schemes
 * of UTF-8 the number of bytes in the resulting UTF-8 string will not
 * necessarily correspond to the number of 16-bit words in the UTF-16 source
 * string.  Note that the destination string is guaranteed to be null
 * terminated even if the source is not.
 */
OI_STATUS OI_Utf16ToUtf8(const OI_UTF16 *src, OI_INT srcLen,
                         OI_UTF8 *dst, OI_INT dstLen);


/**
 * OI_StrLenUtf16()
 *
 * @param str    A UTF-16 string to measure.
 *
 * @return       Number of 16-bit words in the UTF-16 string.
 *
 * This function returns the number of 16-bit words used by the string, not
 * the number of Unicode characters as some Unicode characters may occupy
 * multiple 16-bit words.  In other words, it operates the same way that the
 * standard C function strlen() would on UTF-8 strings (which also have
 * multibyte characters).
 */
OI_INT OI_StrLenUtf16(const OI_UTF16 *str);


/**
 * OI_StrcmpUtf16()
 *
 * @param str1   A UTF-16 string for comparison.
 *
 * @param str2   A UTF-16 string for comparison.
 *
 * @return       0 if identical, -1 if str1 < str2, 1 if str1 > str2.
 *
 * This function compares 2 UTF-16 strings in the same manner that the
 * standard C function strcmp() would compare UTF-8 strings.  That is, it does
 * an exact 16-bit word for 16-bit word compare rather than check for possible
 * eqivalent character encodings for those characters that may have different,
 * but equivalent encodings.
 */
OI_INT OI_StrcmpUtf16(const OI_UTF16 *str1,
                      const OI_UTF16 *str2);


/**
 * OI_StrcmpUtf16()
 *
 * @param str1   A UTF-16 string for comparison.
 *
 * @param str2   A UTF-16 string for comparison.
 *
 * @param len    Max number of 16-bit words to compare.
 *
 * @return       0 if identical, -1 if str1 < str2, 1 if str1 > str2.
 *
 * This function compares 2 UTF-16 strings in the same manner that the
 * standard C function strncmp() would compare UTF-8 strings.  That is, it
 * does an exact 16-bit word for 16-bit word compare rather than check for
 * possible eqivalent character encodings for those characters that may have
 * different, but equivalent encodings.
 */
OI_INT OI_StrncmpUtf16(const OI_UTF16 *str1,
                       const OI_UTF16 *str2,
                       OI_UINT16 len);


/**
 * The unicode encodings that this implementation understands.  UCS-2 is stored in
 * OI_CHAR16s and may specify OI_UNICODE_UTF16_LE or OI_UNICODE_UTF16_BE as the
 * encoding if needed.
 */
typedef enum {
    OI_UNICODE_UNKNOWN = 0,
    OI_UNICODE_UTF8,
    OI_UNICODE_UTF16_LE,
    OI_UNICODE_UTF16_BE
} OI_UNICODE_ENCODING;

/**
 * Structure for representing Pascal-style strings.  This is used to hold UTF encoded
 * unicode strings.
 */
typedef struct {
    OI_BYTE *p;   /**< Pointer to an array of bytes containing the string. */
    OI_INT sz;    /**< The number of bytes at p, not the number of characters. */
} OI_PSTR;


/*
 * UTF encoded unicode string functions
 */

/**
 * @param s1  A UTF encoded string
 * @param e1  The encoding of s1
 * @param s2  An ASCII string
 */
OI_INT OI_PStrcmp(const OI_PSTR *s1,
                  OI_UNICODE_ENCODING e1,
                  const OI_CHAR *s2);

/*
 * This is only valid for US-ASCII characters.  The case of other locales is
 * not known.
 */
OI_INT OI_PStrcmpInsensitive(const OI_PSTR *s1,
                             OI_UNICODE_ENCODING e1,
                             const OI_CHAR *s2);

/**
 * Compare the first len characters of s2 against s1.
 *
 * @param s1  A UTF encoded string
 * @param e1  The encoding of s1
 * @param s2  An ASCII string
 * @param len The number of characters in s2 to compare.
 */
OI_INT OI_PStrncmp(const OI_PSTR *s1,
                   OI_UNICODE_ENCODING e1,
                   const OI_CHAR *s2,
                   OI_UINT16 len);

/**
 * @param s1  A UTF encoded string
 * @param e1  The encoding of s1
 * @param s2  A UTF encoded string
 * @param e2  The encoding of s1
 */
OI_INT OI_PStrcmp2(const OI_PSTR *s1,
                   OI_UNICODE_ENCODING e1,
                   const OI_PSTR *s2,
                   OI_UNICODE_ENCODING e2);

/**
 * Return the byte index of the beginning of s2 in s1 or -1 if not found.
 *
 * @param s1        The string to search.
 * @param e1        The encoding of s1.
 * @param s2        The string to search for.
 * @param startPos  The character position in s1 to begin searching at.
 *
 * @return  The byte position in s1 of the match, or -1 if no match.
 */
OI_INT OI_PStrIndexOf(const OI_PSTR *s1,
                      OI_UNICODE_ENCODING e1,
                      const OI_CHAR *s2,
                      OI_INT startPos);

/**
 * Return the byte index of the beginning of the last instance of s2 in s1
 * or -1 if not found.
 *
 * @param s1        The string to search.
 * @param e1        The encoding of s1.
 * @param s2        The string to search for.
 * @param startPos  The character position in s1 to begin searching at.
 *
 * @return  The byte position in s1 of the match, or -1 if no match.
 */
OI_INT OI_PStrLastIndexOf(const OI_PSTR *s1,
                          OI_UNICODE_ENCODING e1,
                          const OI_CHAR *s2,
                          OI_INT startPos);

/**
 * Return the number of characters (not the number of bytes) in s1.
 */
OI_INT OI_PStrLen(const OI_PSTR *s1,
                  OI_UNICODE_ENCODING e1);

/**
 * Convert a UTF encoded string to UCS-2.
 *
 * @param s1  The UCS-2 destination string
 * @param s2  The UTF source string
 * @param e2  The encoding of s2
 */
void OI_PStrToUStr(OI_CHAR16 *s1,
                   const OI_PSTR *s2,
                   OI_UNICODE_ENCODING e2);

#endif /* _OI_UNICODE_H */



#ifdef OI_DEPRECATED

/**
 * Macro for a UCS-2 unicode string initialization.
 *
 * OI_CHAR16 *unicodeString = OI_UNICODE(L"a unicode string");
 * @deprecated
 */
#define OI_UNICODE(s)  (OI_CHAR16*) (s)


/*
 * UCS-2 unicode string functions
 */

/**
 * @deprecated
 */
OI_INT OI_UStrLen(const OI_CHAR16 *ustr) ;

/**
 * @deprecated
 */
OI_INT OI_UStrcmp(const OI_CHAR16 *ustr1,
                  const OI_CHAR16 *ustr2);

/**
 * @deprecated
 */
OI_INT OI_UStrncmp(const OI_CHAR16 *ustr1,
                   const OI_CHAR16 *ustr2,
                   OI_UINT16 len);

/**
 * @deprecated
 */
void OI_UStrToStr(OI_CHAR *str,
                  const OI_CHAR16 *ustr);

/**
 * @deprecated
 */
void OI_StrToUStr(OI_CHAR16 *ustr,
                  const OI_CHAR *str);

#endif
/**@}*/
