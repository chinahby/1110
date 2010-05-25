#ifndef __QXTYPES_H__
#define __QXTYPES_H__

/*===========================================================================
*
*                               Q X  T Y P E S
*             
*                            H E A D E R  F I L E
*
* FILE:  QxTypes.h
*
* DESCRIPTION :
*
* This header file contains general types and macros that are of use
* to all modules.
*
* DEFINED TYPES:
*
*       Name      Definition
*       -------   --------------------------------------------------------
*       QxByte    8  bit unsigned value
*
*       QxInt8    8  bit unsigned value
*       QxUInt16  16 bit unsigned value
*       QxUInt32  32 bit unsigned value
*
*       QxInt8    8  bit signed value
*       QxInt16   16 bit signed value 
*       QxInt32   32 bit signed value
*
*
* DEFINED CONSTANTS:
*
*       Name      Definition
*       -------   --------------------------------------------------------
*       TRUE      Asserted boolean condition (Logical 1)
*       FALSE     Deasserted boolean condition (Logical 0)
* 
*       ON        Asserted condition
*       OFF       Deasserted condition
*
*       NULL      Pointer to nothing
*
*
* Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/QxTypes.h#2 $ 
  $DateTime: 2008/04/07 18:38:18 $ 2005/09/07 
  $Author: rajeevg $ Madhav Chinthala  


when		who			what, where, why
--------	---			----------------------------------------------------
09/07/05	MC			Initial Version
11/29/05	MC			Added some more definations
			
===========================================================================*/

/* ------------------------------------------------------------------------
*  Constants
*  ------------------------------------------------------------------------*/


#define TRUE	1   /* Boolean true value. */
#define FALSE	0   /* Boolean false value. */

#define ON		1   /* On value. */
#define OFF		0   /* Off value. */

#ifndef NULL
#define NULL	'\0'
#endif

#define SUCCESS		0
#define FAILED		-1


// Typedef's for data types
typedef signed int			  QxInt;
typedef unsigned int		  QxUInt;

typedef float             QxFloat;
typedef long              QxLong;
typedef double            QxDouble;

typedef signed short		  QxInt16;
typedef unsigned short		QxUInt16;
typedef signed long int		QxInt32;
typedef unsigned long int	QxUInt32;

typedef const char			  QxCChar;
typedef char				      QxChar;

typedef signed char			  QxInt8;
typedef unsigned char		  QxUInt8;

typedef unsigned char		  QxByte;

//typedef QxUInt16          QxWord;

// MACROS

#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif


#endif // __QXTYPES_H__

