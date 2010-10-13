#ifndef AEEBTDEVDBGETOBJECT_H
#define AEEBTDEVDBGETOBJECT_H
/*=============================================================================
FILE:         AEEBTDevDBGetObject.h

SERVICES:     GetObject method for Bluetooth Device database

DESCRIPTION:  This header file contains definition of GetObject method for 
              bluetooth device data base 
===============================================================================
Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
#include "AEEBT.h"
#include "AEEBTDevDB.h"

static __inline int IBT_GetDevDB(IBT* po, void** ppDevDB)
{
   return AEEGETPVTBL((po),IBT)->GetObject((po),AEEIID_BT_DEV_DB,
                                           NULL,0,(ppDevDB));
}
/*===============================================================================
IBT_GetDevDB()

Description:
   This function creates an object of type IBTDevDB.

Prototype:
   int IBT_GetDevDB(IBT* po, void** ppDevDB);

Parameters:
   po [i]       : the IBT object
   ppDevDB [o]  : pointer to the created IBTDevDB object if succesful,
                  NULL otherwise

Return Value:
   SUCCESS      : ppDevDB points to a newly created object of type IBTDevDB
   EBADPARM     : invalid input pointer(s)
   ENOMEMORY    : no memory to create object
   EUNSUPPORTED : IBTDevDB class is not supported
   EPRIVLEVEL   : not sufficient privilege level

===============================================================================*/
#endif //AEEBTDEVDBGETOBJECT_H
