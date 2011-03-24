/* Zi8types.h
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */

/* Note: This file is ANSI C
**
** These type definitions are used in the API and internally within the
** eZiText Core Library. 
**
** All of these definitions will generally remain the same regardless
** of platform.
**
*/
#ifndef _ZI8_TYPES_H
#define _ZI8_TYPES_H

typedef void                    ZI8VOID;
typedef unsigned char           ZI8UCHAR;
typedef unsigned char ZI8FAR *  PZI8UCHAR;
typedef unsigned short          ZI8WCHAR;
typedef unsigned short ZI8FAR * PZI8WCHAR;
typedef unsigned char           ZI8BOOL;
typedef unsigned char ZI8FAR *  PZI8BOOL;
typedef unsigned short          ZI8USHORT;
typedef unsigned short ZI8FAR * PZI8USHORT; 
typedef long                    ZI8LONG;
typedef long *                  PZI8LONG;
typedef unsigned long           ZI8ULONG;
typedef unsigned long ZI8FAR *  PZI8ULONG;
typedef short                   ZI8SHORT;
typedef short ZI8FAR *          PZI8SHORT;
typedef void ZI8FAR *           PZI8VOID;

#define ZI8ROMDATA              const
#define ZI8ROMPOINTER           ZI8_HUGE * 
#define ZI8RAMPOINTER           ZI8_HUGE * 

#endif /* #ifndef _ZI8_TYPES_H */
