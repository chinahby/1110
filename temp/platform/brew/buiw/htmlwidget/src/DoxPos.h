/*======================================================

FILE: DoxPos.h

SERVICES: 
   IDoxPos

GENERAL DESCRIPTION:
   IDoxPos document traversal interface

   Reproduction and/or distribution of this file without the written consent of
   QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef DOXPOS_H
#define DOXPOS_H

#include "AEE.h"
#include "AEEWeb.h"
#include "dox.h"
#include "FormControl.h"
#include "lnode.h"

#include "AEEIID_DOXPOS.BID"
#include "AEEIID_DOXUTIL.BID"


/////////////////////////////////////////////////////////////////////////////
// IDoxUtil interface

typedef struct IDoxUtil IDoxUtil;

#define INHERIT_IDoxUtil(iname) \
   INHERIT_IQueryInterface(iname);\
   int      (*GetIWeb)        (iname *, IWeb **ppiw);\
   int      (*MakeUrl)        (iname *, const char *pszRel, char *pBufOut, int cbBufOut);\
   int      (*Encode)         (iname *, char *pcDest, int cbDest, const AECHAR *pwcSrc,\
                               int cwSrc, int *pnUsed, boolean bTerminate);\
   int      (*Decode)         (iname *, AECHAR *pwcDest, int cwDest, const char *pcSrc,\
                               int cbSrc, int *pcbUsed, boolean bTerminate);\
   void     (*IncrementImageCount) (iname *, boolean bIncrement);\
   int      (*GetImage)       (iname *, int16 idImage, IImage **ppii);\
   uint32   (*GetFlags)       (iname *);\
   AECHAR*  (*GetTempBuffer)  (iname *, int *pnSize)

   

AEEINTERFACE(IDoxUtil) {
   INHERIT_IDoxUtil(IDoxUtil);
};


__inline 
uint32 IDOXUTIL_AddRef(IDoxUtil *p){
   return AEEGETPVTBL(p,IDoxUtil)->AddRef(p);
}

__inline
uint32 IDOXUTIL_Release(IDoxUtil *p) {
   return AEEGETPVTBL(p,IDoxUtil)->Release(p);
}

 __inline 
int IDOXUTIL_QueryInterface(IDoxUtil *p, AEECLSID id, void **ppo) {
   return AEEGETPVTBL(p,IDoxUtil)->QueryInterface(p, id, ppo);
}

__inline
int IDOXUTIL_GetIWeb(IDoxUtil *p, IWeb **ppiw) {
   return AEEGETPVTBL(p,IDoxUtil)->GetIWeb(p, ppiw);
}

__inline
int IDOXUTIL_MakeUrl(IDoxUtil *p, const char *pszRel, char *pBufOut, int cbBufOut) {
   return AEEGETPVTBL(p,IDoxUtil)->MakeUrl(p, pszRel, pBufOut, cbBufOut);
}

__inline
int IDOXUTIL_Encode(IDoxUtil *p, char *pcDest, int cbDest, const AECHAR *pwcSrc,\
                         int cwSrc, int *pnUsed, boolean bTerminate) {
   return AEEGETPVTBL(p,IDoxUtil)->Encode(p, pcDest, cbDest, pwcSrc, cwSrc, pnUsed, bTerminate);
}

__inline
int IDOXUTIL_Decode(IDoxUtil *p, AECHAR *pwcDest, int cwDest, const char *pcSrc,\
                         int cbSrc, int *pcbUsed, boolean bTerminate) {
   return AEEGETPVTBL(p,IDoxUtil)->Decode(p, pwcDest, cwDest, pcSrc, cbSrc, pcbUsed, bTerminate);
}

__inline
void IDOXUTIL_IncrementImageCount(IDoxUtil *p, boolean bIncrement) {
   AEEGETPVTBL(p,IDoxUtil)->IncrementImageCount(p, bIncrement);
}

__inline
int IDOXUTIL_GetImage(IDoxUtil *p, int16 idImage, IImage **ppii) {
   return AEEGETPVTBL(p,IDoxUtil)->GetImage(p, idImage, ppii);
}

__inline
uint32 IDOXUTIL_GetFlags(IDoxUtil *p) {
   return AEEGETPVTBL(p,IDoxUtil)->GetFlags(p);
}

__inline
AECHAR* IDOXUTIL_GetTempBuffer(IDoxUtil *p, int *pnSize) {
   return AEEGETPVTBL(p,IDoxUtil)->GetTempBuffer(p, pnSize);
}


/////////////////////////////////////////////////////////////////////////////
// IDoxPos interface


#define INHERIT_IDoxPos(iname) \
   INHERIT_IQueryInterface(iname);\
   int            (*Clone)          (iname *, IDoxPos **ppNew);\
   boolean        (*NextSibling)    (iname *);\
   boolean        (*PrevSibling)    (iname *);\
   boolean        (*Parent)         (iname *);\
   boolean        (*Child)          (iname *);\
   boolean        (*NextDocOrder)   (iname *, int *pnLevel);\
   boolean        (*PrevDocOrder)   (iname *, int *pnLevel);\
   int            (*GetElementName) (iname *, AECHAR *pwBuf, int cchBuf);\
   int            (*GetContent)     (iname *, AECHAR *pwBuf, int cchBuf, int nchIndex, int *pnUsed);\
   int            (*GetNameByIndex) (iname *, int nIndex, AECHAR *pwBuf, int cchBuf);\
   int            (*GetValueByIndex)(iname *, int nIndex, AECHAR *pwBuf, int cchBuf);\
   int            (*GetAttrValue)   (iname *, const AECHAR *pwszAttr, AECHAR *pwBuf, int cchBuf);\
   int            (*GetAttrInt)     (iname *, const AECHAR *pwszAttr, int nDefault);\
   boolean        (*IsAttrValue)    (iname *, const AECHAR *pwszAttr, const AECHAR *pwszValue);\
   boolean        (*IsElement)      (iname *);\
   boolean        (*IsElementName)  (iname *, const AECHAR *pwszElem);\
   boolean        (*GetObject)      (iname *, IWidget **ppo)
   


/* Methods that fill buffers should be consistent in
   their return values. 
   
   In general, functions that fill buffers with strings will always 
   return the number of characters written, (char or AECHAR as 
   appropriate), including the zero terminator. If error or source string 
   is not found or otherwise not available, the function will return 0. 
   
   To test for the required buffer size to store a source string,
   pass NULL for the buffer pointer and/or 0 for the buffer length.
   The return value is the buffer size in characters to store the 
   requested source string.

   Returning 0 to indicate 'not found' may cause confusion for 
   attribute methods, where constructs such as the following:
      attrib=""
   are valid in HTML. In the above case the attribute is present but
   the string length is zero, but since the zero-terminator is written
   the return value will be 1. The general rule is that when the return
   value is 1, the string is an empty string.
         
   Note that the following construct:
      checked
   is equivalant to:
      checked="checked"
      
   In this case IDOXPOS_GetAttrValue() will return the name of the
   attribute when there is no value, but not when the value is an 
   empty string (""). 

*/


typedef struct IDoxPos IDoxPos;

AEEINTERFACE(IDoxPos) {
   INHERIT_IDoxPos(IDoxPos);
};


#define IDOXPOS_AddRef(p)                       AEEGETPVTBL((p),IDoxPos)->AddRef(p)
#define IDOXPOS_Release(p)                      AEEGETPVTBL((p),IDoxPos)->Release(p)
#define IDOXPOS_QueryInterface(p,i,ppo)         AEEGETPVTBL((p),IDoxPos)->QueryInterface((p),(i),(ppo)) 
#define IDOXPOS_Clone(p,ppn)                    AEEGETPVTBL((p),IDoxPos)->Clone((p),(ppn))
#define IDOXPOS_NextSibling(p)                  AEEGETPVTBL((p),IDoxPos)->NextSibling(p)
#define IDOXPOS_PrevSibling(p)                  AEEGETPVTBL((p),IDoxPos)->PrevSibling(p)
#define IDOXPOS_Parent(p)                       AEEGETPVTBL((p),IDoxPos)->Parent(p)
#define IDOXPOS_Child(p)                        AEEGETPVTBL((p),IDoxPos)->Child(p)
#define IDOXPOS_NextDocOrder(p,pn)              AEEGETPVTBL((p),IDoxPos)->NextDocOrder((p),(pn))
#define IDOXPOS_PrevDocOrder(p,pn)              AEEGETPVTBL((p),IDoxPos)->PrevDocOrder((p),(pn))
#define IDOXPOS_GetElementName(p,pw,c)          AEEGETPVTBL((p),IDoxPos)->GetElementName((p),(pw),(c))
#define IDOXPOS_GetContent(p,pw,cch,ndx,pu)     AEEGETPVTBL((p),IDoxPos)->GetContent((p),(pw),(cch),(ndx),(pu))
#define IDOXPOS_GetNameByIndex(p,ndx,pw,cch)    AEEGETPVTBL((p),IDoxPos)->GetNameByIndex((p),(ndx),(pw),(cch))
#define IDOXPOS_GetValueByIndex(p,ndx,pw,cch)   AEEGETPVTBL((p),IDoxPos)->GetValueByIndex((p),(ndx),(pw),(cch))
#define IDOXPOS_GetAttrValue(p,pa,pw,cch)       AEEGETPVTBL((p),IDoxPos)->GetAttrValue((p),(pa),(pw),(cch))
#define IDOXPOS_GetAttrInt(p,pa,n)              AEEGETPVTBL((p),IDoxPos)->GetAttrInt((p),(pa),(n))
#define IDOXPOS_IsAttrValue(p,pa,pv)            AEEGETPVTBL((p),IDoxPos)->IsAttrValue((p),(pa),(pv)) 
#define IDOXPOS_IsElement(p)                    AEEGETPVTBL((p),IDoxPos)->IsElement(p)
#define IDOXPOS_IsElementName(p,pe)             AEEGETPVTBL((p),IDoxPos)->IsElementName((p),(pe)) 
#define IDOXPOS_GetObject(p,pi)                 AEEGETPVTBL((p),IDoxPos)->GetObject((p),(pi))  
#define IDOXPOS_MakeURL(p,pa,pc,cch)            AEEGETPVTBL((p),IDoxPos)->MakeURL((p),(pa),(pc),(cch))

// other macros
#define IDOXPOS_GetContentSize(p)      IDOXPOS_GetContent((p),NULL,0,0,NULL)
#define IDOXPOS_ResetPos(p)            while (IDOXPOS_PrevDocOrder(me,NULL))


IDoxPos * IDoxPos_New(Dox *pdox, int pos/*, LNode *pnHead*/);


/*
==============================================================================

IDOXPOS_Clone()

Description:
   This function creates a copy of the current IDoxPos object. The new
   IDoxPos object may be used for place holding while traversing a
   sub-section of the document.

Prototype:
   int IDOXPOS_Clone(
      IDoxPos *p, 
      IDoxPos **ppNew
   );

Parameters:
   p: Pointer to the interface
   ppNew : Address of memory that will receive a pointer to the new IDoxPos

Return Value:
   SUCCESS if function succeeded.
   ENOMEMORY if function failed due to insufficient memory.

Comments:  
   None

Side Effects: 
   None

==============================================================================

IDOXPOS_Next()

Description:
   Moves the IDoxPos position to the next sibling relative to the current position

Prototype:
   boolean IDOXPOS_Next(IDoxPos *p)

Parameters:
   p: Pointer to the interface

Return Value:
   TRUE if navigation successful (next sibling exists), otherwise FALSE;

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_Prev()

Description:
   Moves the IDoxPos position to the previous sibling relative to the current position

Prototype:
   boolean IDOXPOS_Prev(IDoxPos *p)

Parameters:
   p: Pointer to the interface

Return Value:
   TRUE if navigation successful (previous sibling exists), otherwise FALSE;

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_Parent()

Description:
   Moves the IDoxPos position to the parent relative to the current position

Prototype:
   boolean IDOXPOS_Parent(IDoxPos *p)

Parameters:
   p: Pointer to the interface

Return Value:
   TRUE if navigation successful (parent exists), otherwise FALSE;

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_Child()

Description:
   Moves the IDoxPos position to the first child relative to the current position

Prototype:
   boolean IDOXPOS_Child(IDoxPos *p)

Parameters:
   p: Pointer to the interface

Return Value:
   TRUE if navigation successful (child exists), otherwise FALSE;

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_NextInOrder()

Description:
   Moves the IDoxPos position to the next in-order position

Prototype:
   boolean IDOXPOS_NextInOrder(
      IDoxPos *p,
      int *pnLevel
   );

Parameters:
   p: Pointer to the interface
   pnLevel : [in/out] Pointer to integer value which will be incremented or
             decremented at document hierarchy level changes. This parameter
             may be set to NULL if level information is not needed.

Return Value:
   TRUE if navigation successful, otherwise FALSE (at end of document)

Comments:  
   None

Side Effects: 
   None



==============================================================================

IDOXPOS_PrevInOrder()

Description:
   Moves the IDoxPos position to the previous in-order position. 

Prototype:
   boolean IDOXPOS_PrevInOrder(
      IDoxPos *p,
      int *pnLevel
   );

Parameters:
   p: Pointer to the interface
   pnLevel : [in/out] Pointer to integer value which will be incremented or
             decremented at document hierarchy level changes. This parameter
             may be set to NULL if level information is not needed.

Return Value:
   TRUE if navigation successful, otherwise FALSE (at beginning of document)

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_GetElementName()

Description:
   If the current position points to an element, this function fills a 
   buffer with the name of the element. 
   
Prototype:
   int IDOXPOS_GetElementName(
      IDoxPos *p, 
      AECHAR *pwBuf, 
      int cchBuf
   );

Parameters:
   p: Pointer to the interface
   pwBuf : [out] Address of buffer to receive the element name string
   cchBuf : [in] sizeof receive buffer in AECHARs

Return Value:

   Returns 0 if the IDoxPos object is not pointing to an element.
   
   If pwBuf != NULL and cchBuf != 0, returns the number of AECHARs
   written to the destination buffer.

   If pwBuf == NULL or cchBuf == 0, returns the required buffer size
   to store the element name string.
   
   The returned string will always be zero-terminated even if buffer 
   is not large enough to hold the entire name string.

Comments:  
   None

Side Effects: 
   None

See Also:
   Macros IDOXPOS_IsElement(), IDOXPOS_IsContent()


==============================================================================

IDOXPOS_GetContent()

Description:
   Retrieves the text of the content data at the offset. This function
   may be called iteratively with a small buffer size to retrieve all of the text.
   The 'nchIndex' parameter is used to offset past portions of the content data
   already read. The number of document character consumed is return in pnUsed.

Prototype:
   int IDOXPOS_GetContent(
      IDoxPos *p
      AECHAR *pwBuf, 
      int cchBuf, 
      int nchIndex,
      int *pnUsed
   );

Parameters:
   p: Pointer to the interface
   pwBuf : [out] Address of buffer receiving content data text.
   cchBuf : [in] Size of pwBuf (in AECHARs). 
   nchIndex : [in] Offset (in document chars) from start of content data at which to return text.
   pnUsed : [out] Number of document chars processed. This number should be added to the 
                  last 'nchIndex' value for the next call to IDOXPOS_GetContent

Return Value:
   Returns 0 if the IDoxPos is not pointing to content data, (i.e., is pointing
   to an element), or if nchIndex indexes past the end of the content data.

   If pwBuf != NULL and cchBuf != 0, returns the number of AECHARs written
   to the destination buffer. 
   
   If pwBuf == NULL or cchBuf == 0, returns the length of the content data, 
   in AECHARs, from 'nchIndex'. 
   
   To determine the buffer size required to store the entirety of the content 
   data, make the following call:
   
      int nSize = IDOXPOS_GetContent(p,NULL,0,0,NULL);
   
   This function does NOT zero-terminate the returned content data string.

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_GetNameByIndex()

Description:
   This function is used to enumerate attributes at the current element and
   retrieve the name component. The IDoxPos must be pointing to an element 
   for this function to succeed.

Prototype:
   int IDOXPOS_GetNameByIndex(
      IDoxPos *p, 
      int nIndex, 
      AECHAR *pwBuf, 
      int cchBuf
   );

Parameters:
   p : Pointer to the interface
   nIndex : [in] index of attribute to retrieve. 
   pwBuf : [out] Address of buffer receiving the attribute name string
   cchBuf : [in] sizeof pwBuf in AECHARs

Return Value:
   Returns 0 if the IDoxPos object is not pointing to an element or
   attribute does not exist at 'nIndex'.
   
   If pwBuf != NULL and cchBuf != 0, returns the number of AECHARs
   written to the destination buffer.

   If pwBuf == NULL or cchBuf == 0, returns the required buffer size
   to store the attribute name string.
   
   The returned string will always be zero-terminated even if 
   the buffer is not large enough to hold the entire string.

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_GetValueByIndex()

Description:
   This function is used to enumerate attributes at the current element and
   retrieve the value component. The IDoxPos must be pointing to an element 
   for this function to succeed.

Prototype:
   int IDOXPOS_GetValueByIndex(
      IDoxPos *p, 
      int nIndex, 
      AECHAR *pwBuf, 
      int cchBuf
   );

Parameters:
   p : Pointer to the interface
   nIndex : [in] index of attribute to retrieve. 
   pwBuf : [out] Address of buffer receiving the attribute value string
   cchBuf : [in] sizeof pwBuf in AECHARs

Return Value:
   Returns 0 if the IDoxPos object is not pointing to an element or
   attribute does not exist at 'nIndex'.
   
   If pwBuf != NULL and cchBuf != 0, returns the number of AECHARs
   written to the destination buffer.

   If pwBuf == NULL or cchBuf == 0, returns the required buffer size
   to store the attribute value string.
   
   The returned string will always be zero-terminated even if 
   the buffer is not large enough to hold the entire string.

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_GetAttrValue

Description:
   This function is used to return the value string of a given attribute.
   The IDoxPos must be pointing to an element for this function to succeed.

Prototype:
   int IDoxPos_GetAttrValue(
      IDoxPos *me, 
      const AECHAR *pwszAttr, 
      AECHAR *pwBuf, 
      int cchBuf
   );

Parameters:
   p : Pointer to the interface
   pwszAttr : [in] pointer to attribute name string. 
   pwBuf : [out] Address of buffer receiving the attribute value string
   cchBuf : [in] sizeof pwBuf in AECHARs

Return Value:
   Returns 0 if the IDoxPos object is not pointing to an element or
   attribute matching pwszAttr not found.
   
   If pwBuf != NULL and cchBuf != 0, returns the number of AECHARs
   written to the destination buffer.

   If pwBuf == NULL or cchBuf == 0, returns the required buffer size
   to store the attribute value string.
   
   The returned string will always be zero-terminated even if 
   the buffer is not large enough to hold the entire string.

Comments:  
   None

Side Effects: 
   None


==============================================================================

IDOXPOS_GetAttrInt

Description:
   This function is used to return an integer value of a given attribute. If
   attribute does not exist or IDoxPos is not pointing at an element, this
   function returns the 'nDefault' value.

Prototype:
   int IDoxPos_GetAttrValue(
      IDoxPos *me, 
      const AECHAR *pwszAttr, 
      int nDefault
   );

Parameters:
   p : Pointer to the interface
   pwszAttr : [in] pointer to attribute name string. 
   nDefault : [in] default value returned if attribute value not found

Return Value:
   Returns integer value of attribute of value of 'nDefault' 
   if the attribute was not found.

Comments:  
   None

Side Effects: 
   None

==============================================================================

Macro:
IDOXPOS_IsElement()

Description:
   This macros test if IDoxPos is pointing to an element.

Prototype:
   boolean IDOXPOS_IsElement(IDoxPos *p);

Parameters:
   p : Pointer to the interface

Return Value:
   TRUE is IDoxPos is pointing to an element, otherwise, FALSE
   
Comments:  
   None

Side Effects: 
   None


==============================================================================

Macro:
IDOXPOS_IsContent()

Description:
   This macros test if IDoxPos is pointing to a content data.

Prototype:
   boolean IDOXPOS_IsContent(IDoxPos *p);

Parameters:
   p : Pointer to the interface

Return Value:
   TRUE is IDoxPos is pointing to a content data, otherwise, FALSE
   
Comments:  
   None

Side Effects: 
   None

*/



#endif   //DOXPOS_H

