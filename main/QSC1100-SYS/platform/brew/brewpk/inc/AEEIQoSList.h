#ifndef AEEIQOSLIST_H
#define AEEIQOSLIST_H
/*======================================================
FILE:  AEEIQoSList.h

SERVICES:
   BREW QoS List Interface.

GENERAL DESCRIPTION:
   Interface definition of Quality of service List.

   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEIQI.h"

/*
   Quality Of Service List Interface
*/

#define AEEIID_IQoSList    0x0102b6ca

// Define IQoSList interface methods
#define INHERIT_IQoSList(iname) \
   INHERIT_IQI(iname); \
   int    (*Add)(iname* po, IQI* pIQI); \
   int    (*Remove)(iname* po, IQI* pIQI); \
   uint32 (*Size)(iname* po); \
   int    (*AddAt)(iname* po, uint32 index, IQI* pIQI); \
   int    (*RemoveAt)(iname* po, uint32 index); \
   int    (*GetAt)(iname* po, uint32 index, IQI** ppIQI); \
   int    (*Find)(iname* po, IQI* pIQI); \
   void   (*Clear)(iname* po); \
   int    (*ExtFunc)(iname* po, int nOption, uint32 dwVal)

// declare the actual interface
AEEINTERFACE_DEFINE(IQoSList);


static __inline uint32 IQoSList_AddRef(IQoSList *me)
{
   return AEEGETPVTBL(me,IQoSList)->AddRef(me);
}

static __inline uint32 IQoSList_Release(IQoSList *me)
{
   return AEEGETPVTBL(me,IQoSList)->Release(me);
}

static __inline int IQoSList_QueryInterface(IQoSList *me, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL(me,IQoSList)->QueryInterface(me, id, ppo);
}

static __inline int IQoSList_Add(IQoSList *me, IQI* pIQueryInterface)
{
   return AEEGETPVTBL(me,IQoSList)->Add(me, pIQueryInterface);
}

static __inline int IQoSList_Remove(IQoSList *me, IQI* pIQueryInterface)
{
   return AEEGETPVTBL(me,IQoSList)->Remove(me, pIQueryInterface);
}

static __inline uint32 IQoSList_Size(IQoSList *me)
{
   return AEEGETPVTBL(me,IQoSList)->Size(me);
}

static __inline int IQoSList_AddAt(IQoSList *me, uint32 index, IQI* pIQueryInterface)
{
   return AEEGETPVTBL(me,IQoSList)->AddAt(me, index, pIQueryInterface);
}

static __inline int IQoSList_RemoveAt(IQoSList *me, uint32 index)
{
   return AEEGETPVTBL(me,IQoSList)->RemoveAt(me, index);
}

static __inline int IQoSList_GetAt(IQoSList *me, uint32 index, IQI** ppIQueryInterface)
{
   return AEEGETPVTBL(me,IQoSList)->GetAt(me, index, ppIQueryInterface);
}

static __inline int IQoSList_Find(IQoSList *me, IQI* pIQueryInterface)
{
   return AEEGETPVTBL(me,IQoSList)->Find(me, pIQueryInterface);
}

static __inline void IQoSList_Clear(IQoSList *me)
{
   AEEGETPVTBL(me,IQoSList)->Clear(me);
}

/*=======================================================================
  INTERFACES   DOCUMENTATION
=========================================================================
Interface Name: IQoSList

This interface represents a generic singly linked list.
QoS List allows to store entries that derive from IQI.

In order to use a QoS List, a client should only create a new IQoSList object.

For adding an entry to the end of the list, use IQoSList_Add().

For adding an entry at specific index in the list, use IQoSList_AddAt().

For removing specific entry in the list, use IQoSList_Remove().

For removing an entry from specific index in the list, use IQoSList_RemoveAt().

For retrieving an entry at specific index in the list, use IQoSList_GetAt().

For retrieving the number of the entries in the list, use IQoSList_Size().

For clearing the list, use IQoSList_Clear().

For retrieving the index of specific entry in the list, use IQoSList_Find().

The following header file is required:~
   BREW 4.0 - AEEIQoSList.h
   BREW 3.1 - AEEQoSList.h

=============================================================================

IQoSList_AddRef()
Description:
   This function is inherited from IQI_AddRef().

See Also:
   IQoSList_Release()

=============================================================================

IQoSList_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IQoSList_AddRef()

==============================================================================

IQoSList_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None.

==============================================================================

IQoSList_Add()

Description:
   This function adds new entry at the end of the list.
   The stored element is AddRef()'d.

Prototype:
   int IQoSList_Add(IQoSList *pme, IQI* pIQI);

Parameters:
   pme: the interface pointer
   pIQI: the pointer to the new entry that will be added to the list.

Return Value:
   AEE_SUCCESS: the entry was added correctly to the list.
   AEE_EBADPARM: the entry pointer is not valid.
   AEE_EFAILED: other failure.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSList_AddAt().
=======================================================================

IQoSList_Remove()

Description:
   This function removes the entry pointed by the parameter pIQI,
   the entry will be Released.

Prototype:
   int IQoSList_Remove(IQoSList *pme, IQI* pIQI);

Parameters:
   pme: the interface pointer
   pIQI: the pointer to the entry that will be removed from the list.

Return Value:
   AEE_SUCCESS: the entry was removed from the list.
   AEE_EBADPARM: the entry wasn't not found in the list.
   AEE_EFAILED: other failure.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSList_RemoveAt().
=======================================================================

IQoSList_Size()

Description:
   This function returns the number of the entries in the list.

Prototype:
   uint32 IQoSList_Size(IQoSList* pme);

Parameters:
   pme: the interface pointer

Return Value:
   returns the number of entries in the list.

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None.
=======================================================================

IQoSList_AddAt()

Description:
   This function adds new entry at the index that is specified by "index".
   The stored element is AddRef()'d.

Prototype:
   int IQoSList_AddAt(IQoSList* pme, uint32 index, IQI* pIQI);

Parameters:
   pme: the interface pointer
   index: the index where the entry will be added.
   pIQI: pointer to the entry that will be added to the list.

Return Value:
   AEE_SUCCESS: the entry was added to the list.
   AEE_EBADPARM: the index is not valid.
   AEE_EFAILED: other failure.   

Comments:
   The index is zero based, if the index is greater than or equal to
   the size of the list then the new entry will be added at the end of the list.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSList_Add().
=======================================================================

IQoSList_RemoveAt()

Description:
   This function removes the entry that is specified by "index" 
   from the list, the entry will be Released.

Prototype:
   int IQoSList_RemoveAt(IQoSList* pme, uint32 index);

Parameters:
   pme: the interface pointer
   index: the index of the entry that will be removed from the list.

Return Value:
   AEE_SUCCESS: the entry in index "index" was removed from the list.
   AEE_EBADPARM: the index is not valid.
   AEE_EFAILED: other failure.   

Comments:
   The index is zero based, if the index is greater than or equal to
   the size of the list then EBADPARM is returned.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSList_Remove()
=======================================================================

IQoSList_GetAt()

Description:
   This function returns the entry specified by "index" from the list.

Prototype:
   int IQoSList_GetAt(IQoSList* pme, uint32 index, IQI* pIQI);

Parameters:
   pme: the interface pointer
   index: the index of the entry that will be returned.
   ppIQI: on return points to the requested entry in the list.

Return Value:
   AEE_SUCCESS: the ppIQI points at the entry in the index "index".
   AEE_EMEMPTR: ppIQI pointer is not valid.
   AEE_EBADPARM: the index is not valid.


Comments:
   The index is zero based, if the index is greater than or equal to
   the size of the list then EBADPARM is returned.

Version:
   Introduced BREW Client 3.1

See Also:
   None.
=======================================================================

IQoSList_Find()

Description:
   This function returns the index of an entry in the list.

Prototype:
   int IQoSList_Find(IQoSList* pme, IQI* pIQI);

Parameters:
   pme: the interface pointer   
   pIQI: pointer to the entry that the user wants to find in
   the list.

Return Value:
   -1: if the entry wasn't found in the list.
   >=0: the index of the entry if the entry is found in the list.

Comments:
   The index that will be returned is zero based.

Version:
   Introduced BREW Client 3.1

See Also:
   None.
=======================================================================

IQoSList_Clear()

Description:
   This function clears the list, all the elements in the list will
   be Released.

Prototype:
   void IQoSList_Clear(IQoSList* pme);

Parameters:
   pme: the interface pointer   

Return Value:
   None.
   
Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   None.
=======================================================================*/

#endif      // AEEIQOSLIST_H
