/* ZiShaping.h 
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */
#ifndef _ZI_SHAPING_H_
#define _ZI_SHAPING_H_

#ifndef _ZI8_CFG_H

/* platform related definition */
#if defined(NC30)
#define ZI8FAR                 far
#elif defined(_C166)
#define ZI8FAR                 huge
#else
#define ZI8FAR
#endif

#if 1 && defined(_WIN32_WCE) && !defined(_WIN32_WCE_LIB)
#define _SMARTPHONE_DLL_
#endif

#ifdef __SYMBIAN32__

#define ZI8_NO_GLOBAL_VARIABLES
#define ZI8_EXPORT_C EXPORT_C           /* EXPORT_C and IMPORT_C are declared */
#define ZI8_IMPORT_C IMPORT_C           /* by Symbian OS in e32def.h */

#else /* #ifdef __SYMBIAN32__ */

#ifdef _SMARTPHONE_DLL_

#ifdef _SMARTPHONE_DLL_EXPORT_

#define ZI8_IMPORT_C __declspec(dllexport)
#define ZI8_EXPORT_C

#else /* #ifdef _SMARTPHONE_DLL_EXPORT_ */

#define ZI8_IMPORT_C __declspec(dllimport)
#define ZI8_EXPORT_C

#endif /* #else    #ifdef _SMARTPHONE_DLL_EXPORT_ */

#else /* #ifdef _SMARTPHONE_DLL_EXPORT_ */

#if defined(ZI8_NO_GLOBAL_VARIABLES) && (defined(WIN32) || defined(WIN32DLL))  /* if manually set elsewhere */
/* for Windows DLL exports at least, but not guaranteed to work for other OSes */
/* which may required custom ZI8_EXPORT_C statements                           */
#define ZI8_EXPORT_C __declspec(dllexport)
#define ZI8_IMPORT_C __declspec(dllexport)
#else
/* Engine is statically linked */
#define ZI8_EXPORT_C
#define ZI8_IMPORT_C
#endif

#endif /* #else    #ifdef _SMARTPHONE_DLL_EXPORT_ */

#endif /* #else    #ifdef __SYMBIAN32__ */

#endif/* #ifndef _ZI8_CFG_H*/


#ifndef _ZI8_TYPES_H
typedef unsigned char           ZI8UCHAR;
typedef unsigned char ZI8FAR *  PZI8UCHAR;
typedef unsigned short          ZI8USHORT;
typedef short ZI8FAR *          PZI8SHORT;
typedef unsigned short ZI8FAR * PZI8USHORT; 
typedef unsigned short ZI8FAR * PZI8WCHAR;
typedef unsigned short          ZI8WCHAR;
typedef short                   ZI8SHORT;
typedef void                    ZI8VOID;
typedef unsigned long           ZI8ULONG;
typedef unsigned char           ZI8BOOL;
#endif

#ifndef _ZI8_API_H
#define ZI8_LANG_AR                    19
#define ZI8_LANG_FA                    36
#define ZI8_LANG_UR                    43

#define ZI8TRUE                        1
#define ZI8FALSE                       0
#define ZI8NULL                        0
#endif

#define ErrNo                       0
#define ErrLengthNull               1
#define ErrLength0                  2
#define ErrStringNull               3
#define ErrStringEmpty              4
#define ErrLangID                   5
#define ErrInvalidUniode            6

#define MedialBase    0xF200    /*User's Unicoe area: */
#define MaxBias       0x0F      /* MedialBase+0 to MedialBase+MaxBias */

#define MaxVowelNum                 8

#ifdef __cplusplus

extern "C"{
#endif

/*Note: All character of the input string must be in Unicode*/
  ZI8_IMPORT_C  ZI8SHORT ZiStringShape(  ZI8UCHAR uLangID, PZI8USHORT uLen, PZI8WCHAR wString);
  ZI8_IMPORT_C  ZI8SHORT ZiStringShapeEx(ZI8UCHAR uLangID, PZI8USHORT uLen, PZI8WCHAR wString, 
                                      PZI8USHORT pOrgToSha, PZI8USHORT pShaToOrg);

  ZI8_IMPORT_C ZI8WCHAR ZiISO8859_6ToUnicode(ZI8UCHAR ch);
  ZI8_IMPORT_C ZI8UCHAR ZiUnicodeToISO8859_6(ZI8WCHAR wch);


  ZI8_IMPORT_C ZI8ULONG ZiGetVersion( PZI8UCHAR pVerMajor, PZI8UCHAR pVerMinor, PZI8UCHAR pVerSubminor);
  ZI8_IMPORT_C ZI8WCHAR ZiGetBuildID( void );
  ZI8_IMPORT_C ZI8WCHAR ZiGetOEMID( void );

#ifdef __cplusplus
}
#endif
#endif


