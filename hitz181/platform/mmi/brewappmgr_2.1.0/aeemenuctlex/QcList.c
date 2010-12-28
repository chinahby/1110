/*===========================================================================
FILE:  QcList.c

SERVICES:  list object implementation

GENERAL DESCRIPTION: 

PUBLIC CLASSES AND STATIC FUNCTIONS: IMenuCtl

Copyright © 2008 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

========================================================================== */

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //brewapps/bam2/HEAD/aeemenuctlex/QcList.c#2 $

===========================================================================*/
#include "QcList.h"


static boolean QcListFreeObj(void ** ppObj);

static QcNode* QcListMakeNode(void* pdata)
{
  QcNode* pnode;

  pnode = (QcNode*)MALLOC(sizeof(QcNode));
  if(NULL != pnode)
  {
    pnode->m_pNext = NULL;
    pnode->m_pData = pdata;
  }
  return pnode;
}

QcList* QcListMake()
{
  QcList* plist = NULL;

  plist = (QcList*)MALLOC(sizeof(QcList));
  if(NULL != plist)
  {
    MEMSET(plist, 0, sizeof(QcList));
  }
  else
  {
    DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
  }
  return plist;
}

boolean QcListFreeObj(void ** ppObj)
{
  if (ppObj && *ppObj)
  {
    FREE(*ppObj);
    *ppObj = NULL;
    return TRUE;
  }
  return FALSE;
}

void QcListFree(QcList** pplist)
{
  if ( pplist && *pplist )
  {
    QcList* plist = *pplist;
    QcNode * pnode = plist->m_Head;

    while (pnode != NULL)
    {
      QcNode* pnext = pnode->m_pNext;
      if(plist->m_pFreeDataCB)
      {
        // execute cb to free user data
        plist->m_pFreeDataCB(&pnode->m_pData);
      }
      (void)QcListFreeObj((void**)&pnode);
      pnode = pnext;
    }
    (void)QcListFreeObj((void**)pplist);
  }
}

void QcListRegisterFreeNodeCB(QcList* plist, FreeDataCB pfunc)
{
  if(plist)
  {
    plist->m_pFreeDataCB = pfunc;
  }
}

boolean QcListAddNode(QcList* plist, void* pdata)
{
  QcNode* pnode;
  if(NULL == plist)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm");
    return FALSE;
  }

  pnode = QcListMakeNode(pdata);
  if(NULL == pnode)
  {
    DBGPRINTF_ERROR("MCEX: failed to allocate memory.");
    return FALSE;
  }

  if(NULL == plist->m_Head)
  {
    plist->m_Head = pnode;
  }
  else
  {
    QcNode  *curNode = plist->m_Head;
    while(curNode->m_pNext) 
    {
      curNode = curNode->m_pNext;
    }
    curNode->m_pNext = pnode;
  }

  return TRUE;
}

boolean QcListRemoveNode(QcList* plist, void* pdata)
{
  //boolean  bRet = FALSE;
  QcNode  *pnode;
  QcNode  *pnext;

  if(NULL == plist || NULL == pdata)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm");
  }

  pnode = plist->m_Head;
  if(pnode && pnode->m_pData == pdata)
  {
    // this is the head node
    QcNode* ptmp = pnode;
    plist->m_Head = pnode->m_pNext;
    // delete pnode
    if(plist->m_pFreeDataCB)
    {
      // execute cb to free user data
      plist->m_pFreeDataCB(&ptmp->m_pData);
    }
    (void)QcListFreeObj((void**)&ptmp);
    return TRUE;
  }

  while (pnode != NULL)
  {
    pnext = pnode->m_pNext;
    if(pnext && pnext->m_pData == pdata)
    {
      QcNode* ptmp = pnext;
      // bypass pnext
      pnode->m_pNext = pnext->m_pNext;
      // delete pnext
      if(plist->m_pFreeDataCB)
      {
        // execute cb to free user data
        plist->m_pFreeDataCB(&ptmp->m_pData);
      }
      (void)QcListFreeObj((void**)&ptmp);
      return TRUE;
    }
    pnode = pnext;
  }
 
  return FALSE;
}

QcNode* QcListGetFirstNode(QcList* plist)
{
  if(NULL == plist)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm");
    return FALSE;
  }
  return plist->m_Head;
}

QcNode* QcListGetNextNode(QcNode* pnode)
{
  if(NULL == pnode)
  {
    DBGPRINTF_ERROR("MCEX: invalid parm");
    return FALSE;
  }

  return pnode->m_pNext;
}
