#ifndef _TLG_GLOBAL_H_
#define _TLG_GLOBAL_H_
/****************************************************************************/
/*!@file  tlg_global.h
 *
 * @brief Global datatypes and configuration information.
 *
 * THIS FILE IS INTENDED TO BE USED BY BOTH HOST SOFTWARE AND FIRMWARE
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.4 $
 *
 * Copyright (c) 2006-2008 Telegent Systems, Inc.  All rights reserved.
 ****************************************************************************/

/*****************************************************************************
* INCLUDES
*****************************************************************************/

/*****************************************************************************
* CONSTANTS
*****************************************************************************/

#ifndef TRUE
#define TRUE            (1)
#endif

#ifndef FALSE
#define FALSE           (0)
#endif

#ifndef NULL
#define NULL            (0)
#endif


/*****************************************************************************
* TYPES
*****************************************************************************/
    typedef unsigned char       uint8_t;
    typedef signed char         int8_t;
    typedef unsigned short      uint16_t;
    typedef signed short        int16_t;

#if    defined (_WIN32_WCE) || defined( WIN32 ) && !defined(__GNUWIN32__)
    typedef unsigned long       uint32_t;
    typedef signed long         int32_t;

#else
    typedef unsigned int        uint32_t;
    typedef signed int          int32_t;

#endif

    typedef uint16_t            bool_t; 




#if    defined (_WIN32_WCE) || defined( WIN32 ) && !defined(__GNUWIN32__)
    typedef unsigned __int64    uint64_t;
    typedef signed __int64      int64_t;
    #pragma warning(disable:4761)
#define  ATTRIBUTE_PACKED

#else
    typedef unsigned long long  uint64_t;
    typedef signed long long    int64_t;

    #define ATTRIBUTE_PACKED __attribute__((packed));

#endif

#include "tlg_defs.h"
#endif /* _TLG_GLOBAL_H_ */
