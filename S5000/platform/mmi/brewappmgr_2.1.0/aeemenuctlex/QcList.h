/*=============================================================================
FILE: QcList.h

SERVICES: this file defines a list util interfaces and objects
 
GENERAL DESCRIPTION: 

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //brewapps/bam2/HEAD/aeemenuctlex/QcList.h#2 $

===========================================================================*/
#ifndef QCLIST_H
#define QCLIST_H
#include "AEEStdLib.h"


typedef void (*FreeDataCB)(void*);

typedef struct _QcNode
{
  struct _QcNode *m_pNext;
  void   *m_pData;
}QcNode;

typedef struct _QcList
{
  QcNode* m_Head;
  FreeDataCB m_pFreeDataCB;
}QcList;

QcList* QcListMake(void);
void QcListFree(QcList** pplist);
// if pfunc is not NULL, pfunc will be called when removing a node.
void QcListRegisterFreeNodeCB(QcList* plist, FreeDataCB pfunc);

boolean QcListAddNode(QcList* plist, void* pdata);
boolean QcListRemoveNode(QcList* plist, void* pdata);

QcNode* QcListGetFirstNode(QcList* plist);
QcNode* QcListGetNextNode(QcNode* pnode);

#endif //QCLIST_H

