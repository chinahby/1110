/****************************************************************************
**
**  Name        data_types.h
**
**  Function    this file contains common data type definitions used
**              throughout the WIDCOMM Bluetooth code
**
**  Date       Modification
**  -----------------------
**  3/12/99    Create
**  07/27/00   Added nettohs macro for Little Endian
**                                                                         
**  Copyright (c) 1999-2004, WIDCOMM Inc., All Rights Reserved.
**  Proprietary and confidential.
**
*****************************************************************************/

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifndef NULL
#define NULL     0
#endif

#ifndef FALSE
#define FALSE  0
#endif

typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned int    UINT32, *PUINT32;
typedef signed   int    INT32, *PINT32;
typedef signed   char   INT8;
typedef signed   short  INT16;


typedef UINT32          TIME_STAMP;


#ifndef _WINDOWS_VXD
typedef unsigned char   BOOLEAN;

#ifndef TRUE
#define TRUE   (!FALSE)
#endif
#endif

typedef unsigned char   UBYTE;

//#define PACKED
//#define INLINE

#define BIG_ENDIAN FALSE

#define UINT16_LOW_BYTE(x)      ((x) & 0xff)
#define UINT16_HI_BYTE(x)       ((x) >> 8)


#endif
