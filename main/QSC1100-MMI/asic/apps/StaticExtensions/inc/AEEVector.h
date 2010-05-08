/*===========================================================================
FILE: AEEVector.h

SERVICES: IVector Interface

GENERAL DESCRIPTION:
  IVector implements a growable array of objects. Like an array, it contains 
  components that can be accessed using an integer index.  However, the size 
  of a Vector can grow or shrink as needed to accommodate adding and 
  removing items after the Vector has been created.

PUBLIC CLASSES AND STATIC FUNCTIONS: 
   IVector

INITIALIZATION AND SEQUENCING REQUIREMENTS


        Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEVector.h#1 $
$DateTime: 2002/11/19 09:07:29 $
  $Author: pmukunda $
  $Change: 29134 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
12/03/2001   mjv     Added IVector_Sort()
11/15/2001   mjv     Made file conform to TSG coding standards

=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef __IVECTOR_H__
#define __IVECTOR_H__

#if !defined(BREW_STATIC_APP)
#error BREW_STATIC_APP define
   // Use the .bid file if compiling as dynamic module 
   #include "IVector.bid"		
#else
//   #define AEECLSID_VECTOR    	0x01011e62
    #include "OEMClassIDs.h"   
#endif


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

typedef struct _IVector IVector;

typedef int (*IVectorSortCB)(void *item1,
                             void *item2);


QINTERFACE(IVector)
{
   DECLARE_IBASE(IVector)

   int (*Init)(IVector *iv, uint32 initialCapacity);
   int (*InitEx)(IVector *iv, uint32 initialCapacity, uint32 capacityIncrement);
   int (*AddElement)(IVector *iv, void *object);
   uint32 (*GetCapacity)(IVector *iv);
   int (*EnsureCapacity)(IVector *iv, uint32 minCapacity);
   void *(*ElementAt)(IVector *iv, uint32 index);
   boolean (*IsEmpty)(IVector *iv);
   boolean (*GetList)(IVector *iv, void **objectList, uint32 size);
   void *(*BuildBuffer)(IVector *iv, uint32 elementSize);
   void (*RemoveAllElements)(IVector *iv);
   void *(*RemoveElementAt)(IVector *iv, uint32 index);
   uint32 (*RemoveElement)(IVector *iv, void *object);
   int (*InsertElementAt)(IVector *iv, void *object, uint32 index);
   boolean (*SetElementAt)(IVector *iv, void *object, uint32 index);
   uint32 (*Size)(IVector *iv);
   int (*TrimToSize)(IVector *iv);
   void (*Sort)(IVector *iv, IVectorSortCB cmpFn);
};


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

#define IVector_AddRef(p)                 GET_PVTBL(p,IVector)->AddRef(p)
#define IVector_Release(p)                GET_PVTBL(p,IVector)->Release(p)
#define IVector_Init(p, ic)               GET_PVTBL(p,IVector)->Init(p, ic)
#define IVector_InitEx(p, ic, ci)         GET_PVTBL(p,IVector)->InitEx(p, ic, ci)
#define IVector_AddElement(p,o)           GET_PVTBL(p,IVector)->AddElement(p, o)
#define IVector_GetCapacity(p)            GET_PVTBL(p,IVector)->GetCapacity(p)
#define IVector_EnsureCapacity(p, mc)     GET_PVTBL(p,IVector)->EnsureCapacity(p, mc)
#define IVector_ElementAt(p, i)           GET_PVTBL(p,IVector)->ElementAt(p, i)
#define IVector_IsEmpty(p)                GET_PVTBL(p,IVector)->IsEmpty(p)
#define IVector_GetList(p, ol, s)         GET_PVTBL(p,IVector)->GetList(p, ol, s)
#define IVector_BuildBuffer(p, es)        GET_PVTBL(p,IVector)->BuildBuffer(p, es)
#define IVector_RemoveAllElements(p)      GET_PVTBL(p,IVector)->RemoveAllElements(p)
#define IVector_RemoveElementAt(p, i)     GET_PVTBL(p,IVector)->RemoveElementAt(p, i)
#define IVector_RemoveElement(p, o)       GET_PVTBL(p,IVector)->RemoveElement(p, o)
#define IVector_InsertElementAt(p, o, i)  GET_PVTBL(p,IVector)->InsertElementAt(p, o, i)
#define IVector_SetElementAt(p, o, i)     GET_PVTBL(p,IVector)->SetElementAt(p, o, i)
#define IVector_Size(p)                   GET_PVTBL(p,IVector)->Size(p)
#define IVector_TrimToSize(p)             GET_PVTBL(p,IVector)->TrimToSize(p)
#define IVector_Sort(p,fn)                GET_PVTBL(p,IVector)->Sort(p,fn)

/*=====================================================================

  INTERFACE  DOCUMENTATION

=======================================================================


Interface Name: IVector


==============================================================================

Method: IVector_Init()

Description:
  Constructs an empty vector with the specified initial capacity.  

Prototype:
   int IVector_Init(IVector *iv,
                    uint32   initialCapacity);

Return Value:
   If succesful, return SUCCESS
   If failed,
        ENOMEMORY - not enough memory


==============================================================================

Method: IVector_InitEx()

Description:
   Constructs an empty vector with the specified initial capacity and
   capacity increment

Prototype:
   int IVector_Init(IVector *iv,
                    uint32   initialCapacity,
                    uint32   capacityIncrement);

Return Value:
   If succesful, return SUCCESS
   If failed,
        ENOMEMORY - not enough memory


==============================================================================

Method: IVector_AddElement()

Description:
   Adds the specified component to the end of this vector, increasing 
   its size by one. The capacity of this vector is increased if its 
   size becomes greater than its capacity. 
 

Prototype:
   int IVector_AddElement(IVector *iv,
                          void    *object);

Return Value:
   SUCCESS   - on success
   ENOMEMORY - not enough memory

==============================================================================

Method: IVector_GetCapacity()

Description:

Prototype:
   uint32 IVector_GetCapacity(IVector *iv);

Return Value:
   The current capacity of this vector. 

==============================================================================

Method: IVector_EnsureCapacity()

Description:
   Increases the capacity of this vector, if necessary, to ensure that it 
   can hold at least the number of components specified by the minimum 
   capacity argument.
  
Prototype:
   int IVector_EnsureCapacity(IVector *iv,
                              uint32   minCapacity);

Return Value:
   SUCCESS   - success
   ENOMEMORY - not enough memory

   
==============================================================================

Method: IVector_ElementAt()

Description:

Prototype:
   void *IVector_ElementAt(IVector *iv,
                           uint32   index)

Return Value:
   The component at the specified index


==============================================================================

Method: IVector_IsEmpty()

Description:
   Tests if this vector has no components

Prototype:
   boolean IVector_IsEmpty(IVector *iv);

Return Value:
   None


==============================================================================

Method: IVector_GetList()

Description:
   Copy the list of object pointers into objectList.  The size parameter
   indicates the size of the objectList buffer in bytes.  This must be
   greater than or equal to the value returned by the Size() method

Prototype:
   boolean IVector_GetList(IVector *iv,
                           void  **objectList, 
                           uint32  size);

Return Value:
   FALSE if size is too small or objectList is NULL


==============================================================================

Method: IVector_BuildBuffer()

Description:
   This method assumes that every object stored in the vector is the
   same size (which is indicated by the parameter elementSize).  The method
   will return an array of vector elements in a newly allocated block of 
   memory.  The caller is responsible for FREEing the memory.

Prototype:
   void *IVector_BuildBuffer(IVector *iv,
                             uint32   elementSize);

Return Value:
   NULL on error


==============================================================================

Method: IVector_RemoveAllElements()

Description:
   Removes all components from this vector and sets its size to zero.

   Note that the caller is responsible for FREEing each component
   if they have been dynamically allocated.

Prototype:
   void IVector_RemoveAllElements(IVector *iv);

Return Value:
   None


==============================================================================

Method: IVector_RemoveElementAt()

Description:
   Deletes the component at the specified index. Each component in this 
   vector with an index greater or equal to the specified index is 
   shifted downward to have an index one smaller than the value it had 
   previously.  The index must be a value greater than or equal to 0 and 
   less than the current size of the vector. 

   NOTE!  The current implementation of IVector makes this function run in
          O(n) time

Prototype:
   void * IVector_RemoveElementAt(IVector *iv,
                                  uint32   index);


Return Value:
   NULL       -- if index is out of range
   otherwise  -- the pointer to the component that was just removed


==============================================================================

Method: IVector_RemoveElement()

Description:
   Deletes any component that contains the specified object.  
   
   NOTE! This method runs in O(nm) time, where n is the number of
         components in the vector and m is the number of times the 
         specified object occurs in the vector.

Prototype:
   uint32    IVector_RemoveElement(IVector *iv,
                                   void    *object);


Return Value:
   the number of components removed 


==============================================================================

Method: IVector_InsertElementAt()

Description:
   Inserts the specified object as a component in this vector at 
   the specified index. Each component in this vector with an index 
   greater or equal to the specified index is shifted upward to have 
   an index one greater than the value it had previously.  The index 
   must be a value greater than or equal to 0 and less than or equal 
   to the current size of the vector. 

   NOTE!  The current implementation of IVector makes this function run in
          O(n) time

Prototype:
   int IVector_InsertElementAt(IVector *iv,
                               void    *object, 
                               uint32   index);

Return Value:
   SUCCESS   - success
   ENOMEMORY - not enough memory
   EFAILED   - index is out of range


==============================================================================

Method: IVector_SetElementAt()

Description:
   Sets the component at the specified index of this vector to 
   be the specified object. The previous component at that position 
   is discarded.  The index must be a value greater than or equal to 
   0 and less than the current size of the vector. 

Prototype:
   boolean IVector_SetElementAt(IVector *iv,
                                void    *object, 
                                uint32   index);

Return Value:
   FALSE if index is out of range


==============================================================================

Method: IVector_Size()

Description:

Prototype:
   uint32 IVector_Size(IVector *iv)

Return Value:
   The number of components in this vector


==============================================================================

Method: IVector_TrimToSize()

Description:
   Trims the capacity of this vector to be the vector's current size

Prototype:
   int IVector_TrimToSize(IVector *iv)

Return Value:
   SUCCESS - on success


==============================================================================

Method: IVector_Sort()

Description:
   Sorts the vector using the supplied comparator function

Prototype:
   void IVector_Sort(IVector *iv, IVectorSortCB cmpFn)

Return Value:
   None

=============================================================================*/

#endif
