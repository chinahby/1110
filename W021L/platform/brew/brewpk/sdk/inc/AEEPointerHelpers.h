#ifndef AEE_POINTERHELPERS_H
#define AEE_POINTERHELPERS_H
/*======================================================
FILE:  AEE_POINTERHelpers.h

SERVICES:  Helper routines for pointer events.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE pointer events

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEComdef.h"
#include "AEEError.h"

//defines
#define AEE_POINTER_DEFAULT_MULTICLICK_TIME  500 // number of milliseconds during which 
                                                 // pointer clicks will be treated as 
                                                 // multi-clicks (e.g. double-click).
#define AEE_POINTER_DELIMITER_SIZE           1  // ',' (comma) delimiter

#define AEE_POINTER_XSTR_OFFSET              0
#define AEE_POINTER_X_PREFIX_SIZE            (sizeof("x=")-1)
#define AEE_POINTER_X_VALUE_SIZE             8  //e.g.: "0000001A" for location 26

// AEE_POINTER_X_SIZE = number of bytes required to represent x-coordinate in dwParam for EVT_POINTER_*
#define AEE_POINTER_X_SIZE  (AEE_POINTER_X_PREFIX_SIZE + AEE_POINTER_X_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_YSTR_OFFSET              AEE_POINTER_X_SIZE
#define AEE_POINTER_Y_PREFIX_SIZE            (sizeof("y=")-1)
#define AEE_POINTER_Y_VALUE_SIZE             8  //e.g.: "FFFFFFFB" for location -5

// AEE_POINTER_Y_SIZE = number of bytes required to represent y-coordinate in dwParam for EVT_POINTER_*
#define AEE_POINTER_Y_SIZE  (AEE_POINTER_Y_PREFIX_SIZE + AEE_POINTER_Y_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_TIME_OFFSET              AEE_POINTER_YSTR_OFFSET + AEE_POINTER_Y_SIZE
#define AEE_POINTER_TIME_PREFIX_SIZE         (sizeof("time=")-1)
#define AEE_POINTER_TIME_VALUE_SIZE          8  //32-bit value in hexadecimal string e.g.: "000065ED"

// AEE_POINTER_TIME_SIZE = number of bytes required to represent time in dwParam for EVT_POINTER_*
#define AEE_POINTER_TIME_SIZE  (AEE_POINTER_TIME_PREFIX_SIZE + AEE_POINTER_TIME_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_CLKCNT_OFFSET            AEE_POINTER_TIME_OFFSET + AEE_POINTER_TIME_SIZE
#define AEE_POINTER_CLKCNT_PREFIX_SIZE       (sizeof("clkcnt=")-1)
#define AEE_POINTER_CLKCNT_VALUE_SIZE        1  //1 digit e.g.: "2"

// AEE_POINTER_CLKCNT_SIZE = number of bytes required to represent click count in dwParam for EVT_POINTER_*
#define AEE_POINTER_CLKCNT_SIZE  (AEE_POINTER_CLKCNT_PREFIX_SIZE + AEE_POINTER_CLKCNT_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_MODIFIERS_OFFSET         AEE_POINTER_CLKCNT_OFFSET + AEE_POINTER_CLKCNT_SIZE
#define AEE_POINTER_MODIFIERS_PREFIX_SIZE    (sizeof("modifiers=")-1)
#define AEE_POINTER_KEY_MODIFIERS_OFFSET     AEE_POINTER_MODIFIERS_OFFSET + AEE_POINTER_MODIFIERS_PREFIX_SIZE
#define AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE 8  //8 hexadecimal characters for 32-bit value. e.g.: "A2004000"
#define AEE_POINTER_MOUSE_MODIFIERS_OFFSET   AEE_POINTER_KEY_MODIFIERS_OFFSET + AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE
#define AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE  4  //4 hexadecimal characters for 16-bit value. e.g.: "1000"
#define AEE_POINTER_EXTRA_MODIFIERS_OFFSET   AEE_POINTER_MOUSE_MODIFIERS_OFFSET + AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE
#define AEE_POINTER_EXTRA_MODIFIERS_VALUE_SIZE  4  //4 hexadecimal characters for 16-bit value. e.g.: "0001"
#define AEE_POINTER_MODIFIERS_VALUE_SIZE     AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE + AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE + AEE_POINTER_EXTRA_MODIFIERS_VALUE_SIZE

// AEE_POINTER_MODIFIERS_SIZE = number of bytes required to represent modifiers in dwParam for EVT_POINTER_*
#define AEE_POINTER_MODIFIERS_SIZE  (AEE_POINTER_MODIFIERS_PREFIX_SIZE + AEE_POINTER_MODIFIERS_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_DISPID_PREFIX_SIZE       (sizeof("dispID=")-1)
#define AEE_POINTER_DISPID_VALUE_SIZE        8 //32-bit AEECLSID in hexadecimal string: "010127d4"

// AEE_POINTER_DISPID_SIZE = number of bytes required to represent display ID in dwParam for EVT_POINTER_*
#define AEE_POINTER_DISPID_SIZE  (AEE_POINTER_DISPID_PREFIX_SIZE + AEE_POINTER_DISPID_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_PTRID_PREFIX_SIZE        (sizeof("ptrID=")-1)
#define AEE_POINTER_PTRID_VALUE_SIZE         2 //2 digits e.g.: "01"

// AEE_POINTER_PTRID_SIZE = number of bytes required to represent pointing device ID in dwParam for EVT_POINTER_*
#define AEE_POINTER_PTRID_SIZE  (AEE_POINTER_PTRID_PREFIX_SIZE + AEE_POINTER_PTRID_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

#define AEE_POINTER_MOUSEBTN_PREFIX_SIZE     (sizeof("mousebtn=")-1)
#define AEE_POINTER_MOUSEBTN_VALUE_SIZE      4 //16-bit value in hexadecimal string e.g.: "000A"

// AEE_POINTER_MOUSEBTN_SIZE = number of bytes required to represent mouse button modifiers in dwParam for EVT_POINTER_*
#define AEE_POINTER_MOUSEBTN_SIZE  (AEE_POINTER_MOUSEBTN_PREFIX_SIZE + AEE_POINTER_MOUSEBTN_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    


// Pointing device modifiers. Sent with dwParam for EVT_POINTER_* events
#define AEE_POINTER_MOUSE_LBUTTON            0x0001   // Left button on mouse
#define AEE_POINTER_MOUSE_RBUTTON            0x0002   // Right button on mouse
#define AEE_POINTER_MOUSE_MBUTTON            0x0004   // Middle button on 3-button mouse

// Pointing device extra modifiers
#define AEE_POINTER_EVENT_DROPPED            0x0001   // Pointer event was dropped due to excessive moves in the queue

// Begin - Private helper functions
/* parses exactly nLen bytes of hexadecimal characters starting from cpc */
static __inline uint32 AEE_POINTER_PARSE_HEX_STR(const char* cpc, int nLen)
{
   uint32 uRet = 0;
   const char* cpcEnd;

#define _PARSE_X_HEXVAL(c) \
    (((unsigned)((c)-'0')<10)?((c)-'0'):(((c)|32)-'a'+10))

   for (cpcEnd = cpc + nLen; cpc < cpcEnd; cpc++) {
      uRet = (uRet << 4) + _PARSE_X_HEXVAL(*cpc);
   }

#undef _PARSE_X_HEXVAL

   return uRet;
}

/* outputs exactly nLen bytes of the hexadecimal value of u into pc,
   returns pc */
static __inline char* AEE_POINTER_MAKE_HEX_STR(char* pc, int nLen, uint32 u)
{
   char* pcChar;
   const char* cpszHexChars = "0123456789ABCDEF";

   for (pcChar = pc + nLen; --pcChar >= pc; ) {
      *pcChar = cpszHexChars[u&0x0f];
      u >>= 4;
   }

   return pc;
}

/* computes the length of the given string.*/
static __inline int AEE_POINTER_STRLEN(const char* s)
{
   const char *sEnd = s;

   if (!*s)
      return 0;

   while (*++sEnd)
      ;

   return sEnd - s;
}

/* compares two memory buffers, byte-wise.*/
static __inline int AEE_POINTER_MEMCMP(const void* p1, const void* p2, int length)
{
   const unsigned char *cpc1 = (const unsigned char *)p1;
   const unsigned char *cpc2 = (const unsigned char *)p2;

   while (length-- > 0) {
      int diff = *cpc1++ - *cpc2++;
      
      if (0 != diff) {
         return diff;
      }
   }
   return 0;
}

/* finds the first occurrence of a substring in a memory buffer.*/
static __inline void* AEE_POINTER_MEMSTR(const char* cpHaystack, const char* cpszNeedle, 
                                          int nHaystackLen)
{
   int nNeedleLen;

   nNeedleLen = AEE_POINTER_STRLEN(cpszNeedle);

   while (nHaystackLen >= nNeedleLen) {
      if (!AEE_POINTER_MEMCMP(cpHaystack, cpszNeedle, nNeedleLen)) {
         return (void*)cpHaystack;
      }
      cpHaystack++;
      nHaystackLen--;
   }
   return 0;
}

/* finds the first occurrence of a substring in a string.*/
static __inline void* AEE_POINTER_STRSTR(const char* cpszHaystack, const char* cpszNeedle)
{
   if (0 == AEE_POINTER_STRLEN(cpszNeedle)) { /* strstr(0,"") => 0, shouldn't crash */
      return (char*)cpszHaystack;
   }
   return AEE_POINTER_MEMSTR(cpszHaystack, cpszNeedle, AEE_POINTER_STRLEN(cpszHaystack));
}

/* copies a block of memory from one buffer to another.*/
static __inline void* AEE_POINTER_MEMMOVE(void* pTo, const void* pFrom, unsigned int nLen)
{
   char* pDest = (char *)pTo;
   const char* pSrc = (const char *)pFrom;

   if (nLen == 0 || pTo == pFrom) {
      return pTo; // nothing to be done
   }

   if (pSrc > pDest) {
      for (; nLen > 0; nLen--) {
         *pDest++ = *pSrc++;
      }
   } else {
      for (pDest = pDest + nLen, pSrc = pSrc + nLen; nLen > 0; nLen--) {
         *--pDest = *--pSrc;
      }
   }
   return pTo;
}
// End - Private helper functions



/////////////////////////////////////////////////////////////////////////////
//EVT_POINTER_* public helper functions
static __inline int AEE_POINTER_GET_X(const char* pszdwParam)
{
   return (int)AEE_POINTER_PARSE_HEX_STR(pszdwParam + AEE_POINTER_XSTR_OFFSET + AEE_POINTER_X_PREFIX_SIZE,
                                          AEE_POINTER_X_VALUE_SIZE);
}

static __inline int AEE_POINTER_GET_Y(const char* pszdwParam)
{
   return (int)AEE_POINTER_PARSE_HEX_STR(pszdwParam + AEE_POINTER_YSTR_OFFSET + AEE_POINTER_Y_PREFIX_SIZE,
                                          AEE_POINTER_Y_VALUE_SIZE);
}

static __inline int AEE_POINTER_GET_XY(char* pszdwParam, int *x, int *y)
{
   *x = AEE_POINTER_GET_X(pszdwParam);
   *y = AEE_POINTER_GET_Y(pszdwParam);

   return SUCCESS;
}

static __inline int AEE_POINTER_SET_XY(char* pszDest, int nDestSize, int x, int y)
{
   if (!pszDest || (AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE > nDestSize)) {
      return EBADPARM;
   }

   (void)AEE_POINTER_MEMMOVE((void *)pszDest, (const void *)"x=00000000,y=00000000,", AEE_POINTER_X_SIZE+AEE_POINTER_Y_SIZE);
   (void)AEE_POINTER_MAKE_HEX_STR(pszDest+AEE_POINTER_X_PREFIX_SIZE, AEE_POINTER_X_VALUE_SIZE, x);
   (void)AEE_POINTER_MAKE_HEX_STR(pszDest+AEE_POINTER_YSTR_OFFSET+AEE_POINTER_Y_PREFIX_SIZE, AEE_POINTER_Y_VALUE_SIZE, y);
   return SUCCESS;
}

static __inline int AEE_POINTER_SET_XY_OFFSET(char* pszdwParam, int ndwParamSize, int nXOffset, int nYOffset)
{
   int x, y;

   if (AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE > ndwParamSize) {
      return EBADPARM;
   }
   
   x = AEE_POINTER_GET_X(pszdwParam) + nXOffset;
   y = AEE_POINTER_GET_Y(pszdwParam) + nYOffset;

   AEE_POINTER_SET_XY(pszdwParam, ndwParamSize, x, y);

   return SUCCESS;
}

static __inline int AEE_POINTER_GET_TIME(const char* pszdwParam)
{
   return (int)AEE_POINTER_PARSE_HEX_STR(pszdwParam + AEE_POINTER_TIME_OFFSET + AEE_POINTER_TIME_PREFIX_SIZE,
                                          AEE_POINTER_TIME_VALUE_SIZE);
}

static __inline int AEE_POINTER_GET_CLICKCOUNT(const char* pszdwParam)
{
   return (int)AEE_POINTER_PARSE_HEX_STR(pszdwParam + AEE_POINTER_CLKCNT_OFFSET + AEE_POINTER_CLKCNT_PREFIX_SIZE,
                                          AEE_POINTER_CLKCNT_VALUE_SIZE);
}

static __inline AEECLSID AEE_POINTER_GET_DISPID(const char* pszdwParam)
{
   char* pcDispIDMarker = 0;

   pcDispIDMarker = (char *)AEE_POINTER_STRSTR(pszdwParam, "dispID");
   if ((char *)0 == pcDispIDMarker) {
      return 0;
   }

   return (AEECLSID)AEE_POINTER_PARSE_HEX_STR(pcDispIDMarker + AEE_POINTER_DISPID_PREFIX_SIZE,
                                          AEE_POINTER_DISPID_VALUE_SIZE);
}

static __inline int AEE_POINTER_GET_VALUE(const char *pszdwParam, char *pname, char *pvalue, int nSize)
{
   char *pcValueOffset = pname;
   int nCopied = 0;
   int nNameLen = AEE_POINTER_STRLEN(pname);
      
   if (!pszdwParam || !pname || !pvalue || (nSize <= 1) ) {
      return EBADPARM;
   }

   while (pcValueOffset) {
      pcValueOffset = (char *)AEE_POINTER_STRSTR(pszdwParam, pname);
      if (!pcValueOffset)
         return EBADPARM;
      pcValueOffset = pcValueOffset + nNameLen;
      if (pcValueOffset && '=' == *pcValueOffset) {
         pcValueOffset++;
         break;
      }
   }   

   //copy the value until delimeter ',' is reached or until max buffer size-1
   while (pcValueOffset && (*pcValueOffset != ',') && (nCopied < nSize) )
   {
      *pvalue++ = *pcValueOffset++;
      nCopied++;
   };
   *pvalue = '\0'; //null terminate
    
   return SUCCESS;
}

static __inline uint32 AEE_POINTER_GET_KEY_MODIFIERS(const char* pszdwParam)
{
   if (!pszdwParam)
      return 0;

   return AEE_POINTER_PARSE_HEX_STR(pszdwParam+AEE_POINTER_MODIFIERS_OFFSET+AEE_POINTER_MODIFIERS_PREFIX_SIZE, AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE);
}

static __inline boolean AEE_POINTER_IS_KEY_MODIFIER_SET(const char* pszdwParam, uint32 dwModifier)
{
   if (!pszdwParam)
      return FALSE;

   return (dwModifier == (dwModifier & AEE_POINTER_GET_KEY_MODIFIERS(pszdwParam)));
}

static __inline uint16 AEE_POINTER_GET_MOUSE_MODIFIERS(const char* pszdwParam)
{
   if (!pszdwParam)
      return 0;

   return (uint16)AEE_POINTER_PARSE_HEX_STR(pszdwParam+AEE_POINTER_MODIFIERS_OFFSET+AEE_POINTER_MODIFIERS_PREFIX_SIZE+AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE,
                                             AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE);
}

static __inline boolean AEE_POINTER_IS_MOUSE_MODIFIER_SET(const char* pszdwParam, uint16 wModifier)
{
   if (!pszdwParam)
      return FALSE;

   return (wModifier == (wModifier & AEE_POINTER_GET_MOUSE_MODIFIERS(pszdwParam)));
}

static __inline boolean AEE_POINTER_IS_EVENT_DROPPED(const char* pszdwParam)
{
   if (!pszdwParam)
      return FALSE;

   return (AEE_POINTER_EVENT_DROPPED == (AEE_POINTER_EVENT_DROPPED & (uint16)AEE_POINTER_PARSE_HEX_STR(pszdwParam+AEE_POINTER_EXTRA_MODIFIERS_OFFSET, AEE_POINTER_EXTRA_MODIFIERS_VALUE_SIZE)));
}


/*=====================================================================
MACROS DOCUMENTATION
=======================================================================
AEE_POINTER_GET_X()

Description:
   This helper function extracts the value for the pointing device's 
   x-coordinate. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   The x-coordinate is relative to the app frame coordinates.

Prototype:
   int AEE_POINTER_GET_X(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   x-coordinate of the event as a signed integer.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_Y()
   AEEEvent description for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, 
   and EVT_POINTER_STALE_MOVE.


=======================================================================

AEE_POINTER_GET_Y()

Description:
   This helper function extracts the value for the pointing device's 
   y-coordinate. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   The y-coordinate is relative to the app frame coordinates.

Prototype:
   int AEE_POINTER_GET_Y(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   y-coordinate of the event as a signed integer.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X()
   AEEEvent description for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, 
   and EVT_POINTER_STALE_MOVE.

=======================================================================
AEE_POINTER_GET_XY()

Description:
   This helper function extracts the value for the pointing device's 
   x & y coordinates. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   The x & y coordinates are relative to the app frame coordinates.

Prototype:
    int AEE_POINTER_GET_XY(char* pszdwParam, int *x, int *y)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.
   x: Pointer to buffer receiving x-coordinate
   y: Pointer to buffer receiving y-coordinate

Return Value:
   SUCCESS upon reading the values.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X(), AEE_POINTER_GET_Y()
   AEEEvent description for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, 
   and EVT_POINTER_STALE_MOVE.

=======================================================================

AEE_POINTER_SET_XY()

Description:
   This helper function sets the value for the pointing device's 
   x and y coordinates. This can used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   Using this function to modify the x and y coordinates will ensure 
   that EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events are generated and handled properly by BREW.

Prototype:
   int AEE_POINTER_SET_XY(char* pszDest, int nDestSize, int x, int y)

Parameters:
   pszDest: Buffer to put the formatted string into. Should be at least 
            as big as AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE.
   nDestSize: Size of pszDest in bytes.
   x: X-coordinate of pointing device.
   y: Y-coordinate of pointing device.

Return Value:
   SUCCESS - X and Y coordinates formatted and written to pszDest successfully.
   EBADPARM - invalid parameter passed to the function.

Comments:
   pszDest is not NUL-terminated by this function.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X()
   AEE_POINTER_GET_Y()
   AEE_POINTER_SET_XY_OFFSET()

=======================================================================

AEE_POINTER_SET_XY_OFFSET()

Description:
   This helper function adds an offset to the values of the pointing device's 
   x and y coordinates. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   Using this function to modify the x and y offsets will ensure that 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events are generated and handled properly by BREW.

Prototype:
   int AEE_POINTER_SET_XY_OFFSET(char* pszDest, int nDestSize, int nXOffset, 
                                 int nYOffset)

Parameters:
   pszDest: Buffer to put the formatted string into. Should be at least 
            as big as AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE.
   nDestSize: Size of pszDest in bytes.
   nXOffset: Offset to be added to X-coordinate of pointing device.
   nYOffset: Offset to be added to Y-coordinate of pointing device.

Return Value:
   SUCCESS - X and Y coordinates modified in pszDest successfully.
   EBADPARM - invalid parameter passed to the function.

Comments:
   pszDest is not NUL-terminated by this function.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X()
   AEE_POINTER_GET_Y()
   AEE_POINTER_SET_XY_OFFSET()

=======================================================================

AEE_POINTER_GET_TIME()

Description:
   This helper function extracts the value for the timestamp of the event.
   This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   This is the value returned by GETUPTIMEMS() when the event was recevied by 
   BREW from the driver.

Prototype:
   int AEE_POINTER_GET_TIME(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   Time-stamp of the event as a signed integer.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEEEvent description for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, 
   and EVT_POINTER_STALE_MOVE.

=======================================================================

AEE_POINTER_GET_CLICKCOUNT()

Description:
   This helper function extracts the value for the number of clicks or a pointer
   event. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   The click count is a running count of the number of pointer clicks received 
   during the OEM-specified duration. For example, if the timeout duration is 
   500 milliseconds and two EVT_POINTER_DOWN events are received within 500 
   milliseconds of each other, then AEE_POINTER_GET_CLICKCOUNT will return 2 
   for the second EVT_POINTER_DOWN event. This can be typically used to identify 
   double-click events.

Prototype:
   int AEE_POINTER_GET_CLICKCOUNT(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   Number of clicks recorded since last click interval as a signed integer.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   None

=======================================================================

AEE_POINTER_GET_DISPID()

Description:
   This helper function extracts the value of the display screen's AEECLSID. 
   This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   The AEECLSID returned is the ID of the display on which the 
   EVT_POINTER_* event occured.

Prototype:
   AEECSLID AEE_POINTER_GET_DISPID(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   AEECLSID of display screen.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   None

=======================================================================
AEE_POINTER_GET_VALUE()

Description:
	This functions returns any generic value based upon the given name. 
	This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   This can be used to extract a non default OEM/App specific values such 
   as pressure, color, etc. Although default values such as coordinates and 
   time can be obtained via this fucntions, its more efficient to obtain 
   them by their specific helper functions.

Prototype:
   int AEE_POINTER_GET_VALUE(const char *pszdwParam, char *pname, char *pvalue, int nSize)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.
   pname: String representing the name of the value to be retrieved               
   pvalue: Buffer to hold the value
   nSize: Size of buffer pointed to by pvalue. 
	
Return Value:
   SUCCESS upon finding the value corresponding to name else EBADPARM.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X(), AEE_POINTER_GET_Y(), AEE_POINTER_GET_XY(), 
   AEE_POINTER_GET_TIME(), and AEE_POINTER_GET_DISPID()
   AEEEvent description for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE 
   and EVT_POINTER_STALE_MOVE.

=======================================================================

AEE_POINTER_GET_KEY_MODIFIERS()

Description:
   This helper function finds the modifier bits of keys pressed when the 
   EVT_POINTER_* event was delivered. This should be used in conjunction 
   with EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events. See key modifier bits defined in AEEVCodes.h

Prototype:
   uint32 AEE_POINTER_GET_KEY_MODIFIERS(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   ORed value of all key modifier bits that are set.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_IS_KEY_MODIFIER_SET().
   KB_LSHIFT, KB_RSHIFT and other key modifiers.

=======================================================================

AEE_POINTER_IS_KEY_MODIFIER_SET()

Description:
   This helper function finds if a particular key modifier bit was set when
   EVT_POINTER_* event was delivered. This should be used in conjunction 
   with EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events. See key modifier bits defined in AEEVCodes.h

Prototype:
   boolean AEE_POINTER_IS_KEY_MODIFIER_SET(const char* pszdwParam, uint32 dwModifier)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

   dwModifier: The uint32 modifier value for the key modifier bits.

Return Value:
   TRUE if dwModifier was set, FALSE otherwise.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_KEY_MODIFIERS().
   KB_LSHIFT, KB_RSHIFT and other key modifiers.

=======================================================================

AEE_POINTER_GET_MOUSE_MODIFIERS()

Description:
   This helper function finds the modifier bits of mouse buttons, if supported,
   that were pressed when the EVT_POINTER_* event was delivered. This 
   should be used in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, 
   EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 

Prototype:
   uint16 AEE_POINTER_GET_MOUSE_MODIFIERS(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   ORed value of all mouse modifier bits that are set.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_IS_MOUSE_MODIFIER_SET().
   AEE_POINTER_MOUSE_LBUTTON, AEE_POINTER_MOUSE_RBUTTON and AEE_POINTER_MOUSE_MBUTTON.

=======================================================================

AEE_POINTER_IS_MOUSE_MODIFIER_SET()

Description:
   This helper function finds if a particular mouse modifier bit was set, 
   if supported, when the EVT_POINTER_* event was delivered. This 
   should be used in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, 
   EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events.

Prototype:
   boolean AEE_POINTER_IS_MOUSE_MODIFIER_SET(const char* pszdwParam, uint16 wModifier)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

   wModifier: The uint16 modifier value for the mouse modifier bits.

Return Value:
   TRUE if wModifier was set, FALSE otherwise.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_MOUSE_MODIFIERS().
   AEE_POINTER_MOUSE_LBUTTON, AEE_POINTER_MOUSE_RBUTTON and AEE_POINTER_MOUSE_MBUTTON.

=======================================================================

AEE_POINTER_IS_EVENT_DROPPED()

Description:
   This helper function finds if a pointer move event was dropped by BREW 
   due to an overflowing event queue. A pointer move event may be dropped by 
   BREW if there are more move events generated than BREW can handle. This 
   should be used in conjunction with EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events to detect dropped events.

Prototype:
   boolean AEE_POINTER_IS_EVENT_DROPPED(const char* pszdwParam)

Parameters:
   pszdwParam: The null-terminated single-byte-character string received 
               as dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, 
               EVT_POINTER_MOVE or EVT_POINTER_STALE_MOVE event.

Return Value:
   TRUE if a move event was dropped, FALSE otherwise.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_EVENT_DROPPED

======================================================================*/


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEE_POINTER_DEFAULT_MULTICLICK_TIME

Description:
   BREW's default number of milliseconds during which pointer clicks 
   will be treated as multi-clicks (e.g. double-click). This is an OEM-
   configurable item.

Definition:
   #define AEE_POINTER_DEFAULT_MULTICLICK_TIME   500

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_CLICKCOUNT

=======================================================================

AEE_POINTER_X_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   x-coordinate name-value pair in string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 11 bytes are required for "x=0000001A,"

Definition:
   #define AEE_POINTER_DELIMITER_SIZE           1  // ',' (comma) delimiter
   #define AEE_POINTER_X_PREFIX_SIZE            (sizeof("x=")-1)
   #define AEE_POINTER_X_VALUE_SIZE             8  //e.g.: "0000001A" for location 26
   #define AEE_POINTER_X_SIZE  (AEE_POINTER_X_PREFIX_SIZE + AEE_POINTER_X_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X
   AEE_POINTER_SET_XY

=======================================================================

AEE_POINTER_Y_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   y-coordinate name-value pair in string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 11 bytes are required for "y=FFFFFFFB,"

Definition:
   #define AEE_POINTER_DELIMITER_SIZE           1  // ',' (comma) delimiter
   #define AEE_POINTER_Y_PREFIX_SIZE            (sizeof("y=")-1)
   #define AEE_POINTER_Y_VALUE_SIZE             8  //e.g.: "FFFFFFFB" for location -5
   #define AEE_POINTER_Y_SIZE  (AEE_POINTER_Y_PREFIX_SIZE + AEE_POINTER_Y_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_X
   AEE_POINTER_SET_XY

=======================================================================

AEE_POINTER_TIME_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   time-stamp name-value pair in string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 14 bytes are required for "time=000065ED,"

Definition:
   #define AEE_POINTER_DELIMITER_SIZE           1  // ',' (comma) delimiter
   #define AEE_POINTER_TIME_PREFIX_SIZE         (sizeof("time=")-1)
   #define AEE_POINTER_TIME_VALUE_SIZE          8  //32-bit value in hexadecimal string e.g.: "000065ED"
   #define AEE_POINTER_TIME_SIZE  (AEE_POINTER_TIME_PREFIX_SIZE + AEE_POINTER_TIME_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_TIME

=======================================================================

AEE_POINTER_CLKCNT_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   click count name-value pair in string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 9 bytes are required for "clkcnt=2,"

Definition:
   #define AEE_POINTER_DELIMITER_SIZE           1  // ',' (comma) delimiter
   #define AEE_POINTER_CLKCNT_PREFIX_SIZE       (sizeof("clkcnt=")-1)
   #define AEE_POINTER_CLKCNT_VALUE_SIZE        1  //1 digit e.g.: "2"
   #define AEE_POINTER_CLKCNT_SIZE  (AEE_POINTER_CLKCNT_PREFIX_SIZE + AEE_POINTER_CLKCNT_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_CLICKCOUNT

=======================================================================

AEE_POINTER_MODIFIERS_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   modifiers name-value pair in the string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 27 bytes are required for "modifiers=a2001000000000001,"

Definition:
   #define AEE_POINTER_MODIFIERS_PREFIX_SIZE    (sizeof("modifiers=")-1)
   #define AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE 8  //8 hexadecimal characters for 32-bit value. e.g.: "A2004000"
   #define AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE  4  //4 hexadecimal characters for 16-bit value. e.g.: "1000"
   #define AEE_POINTER_EXTRA_MODIFIERS_VALUE_SIZE  4  //4 hexadecimal characters for 16-bit value. e.g.: "0001"
   #define AEE_POINTER_MODIFIERS_VALUE_SIZE        AEE_POINTER_KEY_MODIFIERS_VALUE_SIZE + AEE_POINTER_MOUSE_MODIFIERS_VALUE_SIZE + AEE_POINTER_EXTRA_MODIFIERS_VALUE_SIZE

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_KEY_MODIFIERS()
   AEE_POINTER_GET_MOUSE_MODIFIERS()
   AEE_POINTER_IS_EVENT_DROPPED()

=======================================================================

AEE_POINTER_DISPID_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   display ID name-value pair in the string pointed to by dwParam for 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE 
   events. e.g.: 16 bytes are required for "dispID=010127d4,"

Definition:
   #define AEE_POINTER_DISPID_PREFIX_SIZE       (sizeof("dispid=")-1)
   #define AEE_POINTER_DISPID_VALUE_SIZE        8 //32-bit AEECLSID in hexadecimal string: "010127d4"
   #define AEE_POINTER_DISPID_SIZE  (AEE_POINTER_DISPID_PREFIX_SIZE + AEE_POINTER_DISPID_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_DISPID()

=======================================================================

AEE_POINTER_PTRID_SIZE

Description:
   The number of bytes required to represent the delimiter-separated 
   pointing device ID name-value pair in the string pointed to by 
   dwParam for EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events. e.g.: 9 bytes are required for "ptrID=01,"

Definition:
   #define AEE_POINTER_PTRID_PREFIX_SIZE        (sizeof("ptrID=")-1)
   #define AEE_POINTER_PTRID_VALUE_SIZE         2 //2 digits e.g.: "01"
   #define AEE_POINTER_PTRID_SIZE  (AEE_POINTER_PTRID_PREFIX_SIZE + AEE_POINTER_PTRID_VALUE_SIZE + AEE_POINTER_DELIMITER_SIZE)    

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   None.

=======================================================================

AEE_POINTER_MOUSE_LBUTTON

Description:
   The modifier bit for left button on mouse device, if supported. This
   should be used in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, 
   EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events.

Definition:
   #define AEE_POINTER_MOUSE_LBUTTON         0x0001   // Left button on mouse

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_MOUSE_MODIFIERS()

=======================================================================

AEE_POINTER_MOUSE_RBUTTON

Description:
   The modifier bit for right button on mouse device, if supported.  This
   should be used in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, 
   EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events.

Definition:
   #define AEE_POINTER_MOUSE_RBUTTON         0x0002   // Right button on mouse

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_MOUSE_MODIFIERS()

=======================================================================

AEE_POINTER_MOUSE_MBUTTON

Description:
   The modifier bit for middle button on mouse device, if supported.  This
   should be used in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, 
   EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events.

Definition:
   #define AEE_POINTER_MOUSE_MBUTTON         0x0004   // Middle button on 3-button mouse

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_MOUSE_MODIFIERS()

=======================================================================

===========================================================================*/

#endif    // AEE_POINTERHELPERS_H
