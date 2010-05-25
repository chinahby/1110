#ifndef BYTE_ORDER_UTILS_H
#define BYTE_ORDER_UTILS_H
/* =======================================================================
                               byte_order_utils.h
DESCRIPTION
  This file defines functions providing byte ordering utility.
  
  Implementor's note: These four functions are all identical in their 
  implementation except for the ifdefs.

  We could probably move all the common code into one function and call 
  it from each of the four?
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/endian/main/latest/inc/byte_order_utils.h#1 $
$DateTime: 2008/05/08 11:03:24 $
$Change: 656211 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  little_endian_to_host

DESCRIPTION
   Convert little endian to host format.

   This function takes a buffer of data which is assumed to be in little endian order and 
   rearranges it to the native order of the machine running the code. If the machine is a
   little endian machine, nothing is done.
   \param data A pointer to the input/output buffer
   \param size The number of bytes in the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
#if defined(BYTE_ORDER_BIG_ENDIAN)
inline void little_endian_to_host(char *data, uint32 size)
{
  char tmp;
  char *ptr1, *ptr2;

  ptr1 = data; ptr2 = data + size - 1;

  while ( ptr1 < ptr2 )
  {
    tmp  = *ptr1;
    *ptr1++ = *ptr2;
    *ptr2-- = tmp;
  }
}
#else
#define little_endian_to_host(a,b)
#endif  

/* ======================================================================
FUNCTION 
  host_to_little_endian

DESCRIPTION
   Convert host to little endian format.

   This function takes a buffer of data which is assumed to be in the host's native order and 
   rearranges it to the little endian format. If the machine is a little endian machine, nothing is done.
   \param data A pointer to the input/output buffer
   \param size The number of bytes in the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
#if defined(BYTE_ORDER_BIG_ENDIAN)
inline void host_to_little_endian(char *data, unsigned int size)
{
  char tmp;
  char *ptr1, *ptr2;

  ptr1 = data; ptr2 = data + size - 1;

  while ( ptr1 < ptr2 )
  {
    tmp  = *ptr1;
    *ptr1++ = *ptr2;
    *ptr2-- = tmp;
  }
}
#else
#define host_to_little_endian(a,b)
#endif  


/* ======================================================================
FUNCTION 
  big_endian_to_host

DESCRIPTION
   Convert big endian to host format.

   This function takes a buffer of data which is assumed to be in big endian order and 
   rearranges it to the native order of the machine running the code. If the machine is a
   big endian machine, nothing is done.
   \param data A pointer to the input/output buffer
   \param size The number of bytes in the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
#if defined(BYTE_ORDER_LITTLE_ENDIAN)
inline void big_endian_to_host(char *data, unsigned int size)
{
  char tmp;
  char *ptr1, *ptr2;

  ptr1 = data; ptr2 = data + size - 1;

  while ( ptr1 < ptr2 )
  {
    tmp  = *ptr1;
    *ptr1++ = *ptr2;
    *ptr2-- = tmp;
  }
}
#else
#define big_endian_to_host(a,b)
#endif  


/* ======================================================================
FUNCTION 
  host_to_big_endian

DESCRIPTION
   Convert host to big endian format.

   This function takes a buffer of data which is assumed to be in native host order and 
   rearranges it to big endian format. If the machine is a big endian machine, nothing is done.
   \param data A pointer to the input/output buffer
   \param size The number of bytes in the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
#if defined(BYTE_ORDER_LITTLE_ENDIAN)
inline void host_to_big_endian(char *data, unsigned int size)
{
  char tmp;
  char *ptr1, *ptr2;

  ptr1 = data; ptr2 = data + size - 1;

  while ( ptr1 < ptr2 )
  {
    tmp  = *ptr1;
    *ptr1++ = *ptr2;
    *ptr2-- = tmp;
  }
}
#else
#define host_to_big_endian(a,b)
#endif  

#endif
