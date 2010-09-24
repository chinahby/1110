#ifndef __TXLISTREF_H__
#define __TXLISTREF_H__
/*===========================================================================


T X L I S T

H E A D E R  F I L E

FILE:  TxListRef.h

DESCRIPTION

This header file declares a class TxListRef and methods to access list 
functionality. The list class maintains refs to objects


PUBLIC CLASSES 
class ListNodeRef
class TxListRef

INITIALIZATION AND SEQUENCING REQUIREMENTS
NA

          (c) COPYRIGHT <2005> QUALCOMM Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE 
$Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/TxListRef.h#1 $ 
$DateTime: 2007/07/17 11:00:30 $  
$Author: jmiera $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/06   rv       for(int i; ; ) fix
03/29/06   ssm     Renamed methods to match STL
12/29/05   NV       Moved public function UpdateLastNode,SetFirst to private 
                    function & private function GetLastError,GetlastErrorStr
                    to public function.
12/28/05   NV       Remove redudant code as index validity in delete_at function
12/27/05   NV       Added 4 new functions for InsertElementAt,
                    element_at,DeleteElementAt,DeleteElementData 
12/09/05   CP       Incorporated review comments
10/17/05   Siv      Initial Version


===========================================================================*/


/*===========================================================================

INCLUDE FILES

=====================================================================*/
#include "comdef.h"



typedef enum ErrorTxListRefType{
  ERROR_TXLISTREF_NO_ERROR = 0,
  ERROR_TXLISTREF_INVALID_PARAMETERS,
  ERROR_TXLISTREF_INSUFFICIENT_MEMORY,
  ERROR_TXLISTREF_INVALID_NODE,
  ERROR_TXLISTREF_NODE_NOT_FOUND,
  ERROR_TXLISTREF_EMPTY_LIST,
  ERROR_TXLISTREF_NO_NODE,
  ERROR_TXLISTREF_OUT_OF_RANGE,
  ERROR_TXLISTREF_SIZER = 0x0FFFFFFF
};

/*===================================================================

CLASS DEFINITION

CLASS:
ListNodeRef

DESCRIPTION:
Class having different functions to access a single node of a list.

PUBLIC FUNCTIONS & CONSTRUCTORS:

ListNodeRef(const T *nodedata);

int32 GetIndex();

void SetIndex (int32 indexvalue);

T *GetNode();

PRIVATE FUNCTION

===================================================================*/
template <class T>
class ListNodeRef{

private:
  T *data;

public:
  ListNodeRef<T> *next;

  /*==============================================================
  FUNCTION:  ListNodeRef

  DESCRIPTION:
  constructor for ListNodeRef from a template object

  INPUT/OUTPUT PARAMETERS:
  const T *nodedata  - user defined data structure object.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  ==============================================================*/
  ListNodeRef(const T *nodedata);

    /*==============================================================
  FUNCTION: TxListRef

  DESCRIPTION:
  Default destructor for the ListNodeRef class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  ~ListNodeRef(){}

  /*==============================================================
  FUNCTION:  GetNode

  DESCRIPTION:
  Method gets the nodedata for the ListNodeRef.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to the nodedata object.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T *GetData();

}; //End class ListNodeRef


/*================================================================

CLASS DEFINITION

CLASS:
TxListRef

DESCRIPTION:
Class having different functions to maintain a list of ListNodeRefs.

PUBLIC FUNCTIONS & CONSTRUCTORS:

TxListRef();

~TxListRef();

bool push_front (const T *nodedata);

bool push_back (const T *nodedata);

T *pop_front();

T *pop_back();

T *peek_front ();

T *peek_back();

int32 GetSize();

T *get_next (T *currentnode);

T *GetPrev (T *currentnode);

bool Clear ();

T * search (T *nodeelement);

bool Merge (TxListRef<T> *newlist);

bool PushBefore (const T *nodeelement, const T *nodedata);

bool PushAfter (const T *nodeelement, const T *nodedata);

bool insert_at(T *pNodeData, int32 index);

bool delete_at(int32 index);

bool delete_data(T *pNodedata);

T * element_at(int32 Index);

TxListRef<T> *Split(const T *nodeelement);

void Resize(void);

inline ErrorTxListRefType GetLastError ();

const QxChar *GetLastErrorStr ();


PRIVATE FUNCTION

bool SetFirst(ListNodeRef<T> *node);

ListNodeRef<T> *GetFirst ();

ListNodeRef *GetNodeFor (T *nodedata);

bool UpdateLastNode (ListNodeRef<T> *node);

inline void SetError (ErrorTxListRefType errorvalue);


=================================================================*/
template <class T>
class TxListRef
{
public:

  /*==============================================================
  FUNCTION: TxListRef

  DESCRIPTION:
  Default constructor for the TxListRef class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  TxListRef();

  /*==============================================================
  FUNCTION: TxListRef

  DESCRIPTION:
  Default destructor for the TxListRef class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  ~TxListRef();  

  /*==============================================================
  FUNCTION: push_front

  DESCRIPTION:
  Insers the nodedata object at the begining of the list.

  INPUT/OUTPUT PARAMETERS:
  nodedata  - pointer to data object that need to be inserted at the 
  begining of the list.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  bool push_front (const T *nodedata);


  /*==============================================================
  FUNCTION: push_back

  DESCRIPTION:
  Inserts the nodedata object at the end of the list.

  INPUT/OUTPUT PARAMETERS:
  nodedata  - pointer to data object that need to be inserted at the 
  end of the list.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  bool push_back (const T *nodedata);


  /*==============================================================
  FUNCTION: pop_front

  DESCRIPTION:
  Removes the first nodedata object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to data object that is removed from the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T *pop_front();


  /*==============================================================
  FUNCTION: pop_back

  DESCRIPTION:
  Removes the last node object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to data object that is removed from the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T *pop_back();


  /*==============================================================
  FUNCTION: peek_front

  DESCRIPTION:
  Returns the first nodedata object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to data object that is present at the begining of the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T *peek_front ();


  /*==============================================================
  FUNCTION: peek_back

  DESCRIPTION:
  Returns the last nodedata from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to data object that is present at the end of the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T *peek_back();



  /*==============================================================
  FUNCTION: GetSize

  DESCRIPTION:
  Gets the number of nodes present in the TxListRef.

  INPUT/OUTPUT PARAMETERS:
  None 

  RETURN VALUE:
  Number of nodes present in the List as int32

  SIDE EFFECTS:
  None
  ==============================================================*/
  int32 GetSize();


  /*==============================================================
  FUNCTION: get_next

  DESCRIPTION:
  Gets the next node from the node specified.

  INPUT/OUTPUT PARAMETERS:
  currentnode - reference node whose next node is required. 

  RETURN VALUE:
  Pointer to node that is placed after the passed node.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T *get_next (T *currentnode);


  /*==============================================================
  FUNCTION: GetPrev

  DESCRIPTION:
  Gets the previous node from the node specified.

  INPUT/OUTPUT PARAMETERS:
  currentnode - reference node whose previous node is required. 

  RETURN VALUE:
  Pointer to node that is placed before the passed node.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T *GetPrev (T *currentnode);


  /*==============================================================
  FUNCTION: Clear

  DESCRIPTION:
  Clears all the nodes in the list.

  INPUT/OUTPUT PARAMETERS:
  None 

  RETURN VALUE:
  TRUE if all the nodes are freed else FALSE.

  SIDE EFFECTS:
  Memory allocated for userdefined objects need to be freed apprpriately.
  ==============================================================*/
  bool Clear ();


  /*==============================================================
  FUNCTION: Search

  DESCRIPTION:
  searches for the node.

  INPUT/OUTPUT PARAMETERS:
  nodeelement - node element to search for.

  RETURN VALUE:
  Pointer to node element. NULL on failure.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T * Search (T *nodeelement);


  /*==============================================================
  FUNCTION: Merge

  DESCRIPTION:
  The method appends the supplied TxListRef to the existing one.

  INPUT/OUTPUT PARAMETERS:
  newlist - Pointer to TxListRef object.

  RETURN VALUE:
  True if merge is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool Merge (TxListRef<T> *newlist);

  /*==============================================================
  FUNCTION: Push Before

  DESCRIPTION:
  The method pushes an data before a specified element.

  INPUT/OUTPUT PARAMETERS:
  Nodelelement - The element before which the data is to be inserted.
  Nodedata - The data which is to be inserted in the list.

  RETURN VALUE:
  True if push is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool PushBefore (T *nodeelement, T *nodedata);

  /*==============================================================
  FUNCTION: Push After

  DESCRIPTION:
  The method pushes an data after a specified element.

  INPUT/OUTPUT PARAMETERS:
  Nodelelement - The element after which the data is to be inserted.
  Nodedata - The data which is to be inserted in the list.

  RETURN VALUE:
  True if push is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool PushAfter (T *nodeelement, T *nodedata);

  /*==============================================================
  FUNCTION: insert_at

  DESCRIPTION:
  The method inserts data at particular index

  INPUT/OUTPUT PARAMETERS:
  Index - The index at data is to be inserted.
  pNodedata - The data which is to be inserted in the list.

  RETURN VALUE:
  True if Insert is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool insert_at(T *pNodeData, int32 index);

  /*==============================================================
  FUNCTION: delete_at

  DESCRIPTION:
  The method deletes data in list at particular index

  INPUT/OUTPUT PARAMETERS:
  Index - index at which data is to be deleted

  RETURN VALUE:
  True if Delete is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool delete_at(int32 index);

  /*==============================================================
  FUNCTION: delete_data

  DESCRIPTION:
  The method deletes data in list

  INPUT/OUTPUT PARAMETERS:
  nodedata - The data which is to be deleted in the list.

  RETURN VALUE:
  True if Delete is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool delete_data(T *pNodedata);

  /*==============================================================
  FUNCTION: element_at

  DESCRIPTION:
  The method gives list data at particular index

  INPUT/OUTPUT PARAMETERS:
  Index - The index at which data is needed


  RETURN VALUE:
  List Data at particular Index

  SIDE EFFECTS:
  None
  ==============================================================*/
  T * element_at(int32 Index);

  /*==============================================================
  FUNCTION: Split

  DESCRIPTION:
  The method splits the list starting from a specified element.

  INPUT/OUTPUT PARAMETERS:
  Nodelelement - The element from where split occurs.

  RETURN VALUE:
  True if split is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  TxListRef<T> *Split(const T *nodeelement);


  /*==============================================================
  FUNCTION: Resize

  DESCRIPTION:
  Updates the size of the list.

  INPUT/OUTPUT PARAMETERS:
  none

  RETURN VALUE:
  none

  SIDE EFFECTS:
  None
  ==============================================================*/
  void Resize(void);


  /*==============================================================
  FUNCTION: GetLastError

  DESCRIPTION:
  Gets the error value that was last set.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Error value that was last set.

  SIDE EFFECTS:
  None
  ==============================================================*/
  inline ErrorTxListRefType GetLastError ();


  /*==============================================================
  FUNCTION: GetlastErrorStr

  DESCRIPTION:
  Gets the character string description for the last occured error.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  character string description for the last occured error.

  SIDE EFFECTS:
  None
  ==============================================================*/
  const char *GetLastErrorStr ();
 
private:

  /*==============================================================
  FUNCTION: SetFirst

  DESCRIPTION:
  The method Sets the header node for the list.

  INPUT/OUTPUT PARAMETERS:
  node - List node to be updated as the first node.

  RETURN VALUE:
  True if head node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool SetFirst(ListNodeRef<T> *node);

  /*==============================================================
  FUNCTION: GetFirst

  DESCRIPTION:
  Returns the ListNodeRef holding the first node data from TxOrderedList.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  ListNodeRef that holds the first node data specified.

  SIDE EFFECTS:
  None
  ==============================================================*/
  ListNodeRef<T> *GetFirst ();

  /*==============================================================
  FUNCTION: GetNodeFor

  DESCRIPTION:
  Returns the ListNodeRef holding the node data from TxListRef.

  INPUT/OUTPUT PARAMETERS:
  nodedata - data object to search for

  RETURN VALUE:
  ListNodeRef that holds the node data specified.

  SIDE EFFECTS:
  If two nodes contains the same node data, only the first occuring 
  ListNodeRef is returned.
  ==============================================================*/
  ListNodeRef<T> *GetNodeFor (T *nodedata);

  /*==============================================================
  FUNCTION: UpdateLastNode

  DESCRIPTION:
  Updates the last node of the list.

  INPUT/OUTPUT PARAMETERS:
  node - reference node element to start upating the last node.

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool UpdateLastNode (ListNodeRef<T> *node);


  /*==============================================================
  FUNCTION: SetError

  DESCRIPTION:
  Sets the error value.

  INPUT/OUTPUT PARAMETERS:
  errorvalue - error value to be set.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
  ==============================================================*/
  inline void SetError (ErrorTxListRefType errorvalue);

private:

  ListNodeRef<T> *pFirstNode;
  ListNodeRef<T> *pLastNode;
  ErrorTxListRefType error;
  int32 nSize;

};


/*==============================================================
FUNCTION:  ListNodeRef

DESCRIPTION:
constructor for ListNodeRef from a template object

INPUT/OUTPUT PARAMETERS:
const T *nodedata  - user defined data structure object.

RETURN VALUE:
None

SIDE EFFECTS:
==============================================================*/
template <class T>
ListNodeRef<T>::ListNodeRef(const T *pNodeData){
  if (pNodeData == NULL){
    return;
  }
  data = (T *)pNodeData;
  next = NULL;
}

/*==============================================================
FUNCTION:  GetData

DESCRIPTION:
Method gets the nodedata for the ListNodeRef.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to the nodedata object.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T *ListNodeRef<T>::GetData(){
  return data;
}


/*==============================================================
FUNCTION: TxListRef

DESCRIPTION:
Default constructor for the TxListRef class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
TxListRef<T>::TxListRef(){
  pFirstNode = NULL;
  pLastNode = NULL;
  nSize = 0;
  SetError(ERROR_TXLIST_NO_ERROR);
}


/*==============================================================
FUNCTION: ~TxListRef

DESCRIPTION:
Default destructor for the TxListRef class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
TxListRef<T>::~TxListRef(){
  ListNodeRef<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode->next;
    delete(pTempNode);
  }
  pFirstNode = NULL;
  pLastNode = NULL;
  nSize = 0;
  return;
}


/*==============================================================
FUNCTION: push_front

DESCRIPTION:
Insers the nodedata object at the begining of the list.

INPUT/OUTPUT PARAMETERS:
nodedata  - pointer to data object that need to be inserted at the begining 
of the list.

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
bool TxListRef<T>::push_front (const T *pNodeData)
{
  if (pNodeData == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  }
  ListNodeRef<T> *node = new ListNodeRef<T>(pNodeData);
  if (node == NULL){
    SetError (ERROR_TXLISTREF_INSUFFICIENT_MEMORY);
    return FALSE;
  }
  if (pFirstNode == NULL){
    pFirstNode = node;
    UpdateLastNode(pFirstNode);
  }else{
    node->next = pFirstNode;
    pFirstNode = node;
  }
  nSize ++;
  SetError(ERROR_TXLISTREF_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: push_back

DESCRIPTION:
Inserts the nodedata object at the end of the list.

INPUT/OUTPUT PARAMETERS:
nodedata  - pointer to data object that need to be inserted at the end of 
the list.

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
bool TxListRef<T>::push_back (const T *pNodeData)
{
  if (pNodeData == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  }
  //creating list pNode for the object
  ListNodeRef<T> *pNode = new ListNodeRef<T>(pNodeData);
  if (pNode == NULL){
    SetError (ERROR_TXLISTREF_INSUFFICIENT_MEMORY);
    return FALSE;
  }

  if (pFirstNode == NULL){
    pFirstNode = pNode;
    UpdateLastNode(pFirstNode);
  }
  else{
    pLastNode->next = pNode;
    UpdateLastNode(pNode);
  }
  nSize++;
  SetError(ERROR_TXLISTREF_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: pop_front

DESCRIPTION:
Removes the first nodedata object from the list.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to data object that is removed from the list.

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
T *TxListRef<T>::pop_front()
{
  T *pRetVal = NULL;
  ListNodeRef<T> *pTempNode = NULL;
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode->next;
    pRetVal = pTempNode->GetData();
    delete (pTempNode);
    pTempNode = NULL;
    nSize --;
    SetError(ERROR_TXLISTREF_NO_ERROR);

    if (pFirstNode == NULL){
      UpdateLastNode(pFirstNode);
    }
    return pRetVal;
  }
  else{
    SetError (ERROR_TXLISTREF_EMPTY_LIST);
    return NULL;
  }
}


/*==============================================================
FUNCTION: pop_back

DESCRIPTION:
Removes the last node object from the list.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to data object that is removed from the list.

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
T *TxListRef<T>::pop_back()
{
  T *pRetVal = NULL;
  ListNodeRef<T> *pTemp = NULL;
  ListNodeRef<T> *pTempNode = NULL;
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode->next != NULL){
      pTemp = pTempNode;
      pTempNode = pTempNode->next;
    }
    if (pTemp != NULL){
      pTemp->next = NULL;
    }
    else{
      pFirstNode = NULL; // If there is only one node in the list
    }
    UpdateLastNode(pTemp);
    pRetVal = pTempNode->GetData();
    delete pTempNode;
    nSize --;
    SetError(ERROR_TXLISTREF_NO_ERROR);
    return pRetVal;
  }
  else{
    SetError (ERROR_TXLISTREF_EMPTY_LIST);
    return NULL;
  }
  return NULL;
}


/*==============================================================
FUNCTION: peek_front

DESCRIPTION:
Returns the first nodedata object from the list.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to data object that is present at the begining of the list.

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
T *TxListRef<T>::peek_front ()
{
  T *retval = NULL;
  if (pFirstNode != NULL){
    retval = pFirstNode->GetData();
    SetError(ERROR_TXLISTREF_NO_ERROR);
  }
  else{
    SetError (ERROR_TXLISTREF_EMPTY_LIST);    
  }
  return retval;
}


/*==============================================================
FUNCTION: peek_back

DESCRIPTION:
Returns the last nodedata from the list.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to data object that is present at the end of the list.

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
T *TxListRef<T>::peek_back()
{
  T *retval = NULL;
  if (pLastNode != NULL){
    retval = pLastNode->GetData();
    SetError(ERROR_TXLISTREF_NO_ERROR);
  }
  else{
    SetError (ERROR_TXLISTREF_EMPTY_LIST);    
  }
  return retval;
}

/*==============================================================
FUNCTION: GetSize

DESCRIPTION:
Gets the number of nodes present in the TxListRef.

INPUT/OUTPUT PARAMETERS:
None 

RETURN VALUE:
Number of nodes present in the List as int32

SIDE EFFECTS:
None
==============================================================*/
template <class T>
int32 TxListRef<T>::GetSize()
{
  return nSize;
}


/*==============================================================
FUNCTION: get_next

DESCRIPTION:
Gets the next node from the node specified.

INPUT/OUTPUT PARAMETERS:
currentnode - reference node whose next node is required. 

RETURN VALUE:
Pointer to node that is placed after the passed node.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T *TxListRef<T>::get_next (T *pCurrentNode)
{
  ListNodeRef<T> *node = NULL;
  T *tempnode = NULL;
  if (pCurrentNode == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return NULL;
  }
  node = GetNodeFor (pCurrentNode);
  if (node != NULL && node->next != NULL ){
    node = node->next;
    SetError(ERROR_TXLISTREF_NO_ERROR);
    return node->GetData();
  }
  SetError (ERROR_TXLISTREF_NO_NODE);
  return NULL;
}


/*==============================================================
FUNCTION: GetPrev

DESCRIPTION:
Gets the previous node from the node specified.

INPUT/OUTPUT PARAMETERS:
currentnode - reference node whose previous node is required. 

RETURN VALUE:
Pointer to node that is placed before the passed node.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T *TxListRef<T>::GetPrev (T *pCurrentNode)
{
  ListNodeRef<T> *pPreviousNode = NULL;
  ListNodeRef<T> *pTempNode = NULL;
  if (pCurrentNode == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return NULL;
  }
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if (pCurrentNode == pTempNode->GetData()){
        break;
      }
      pPreviousNode = pTempNode;
      pTempNode = pTempNode->next;
    }
  }
  if (pPreviousNode != NULL){
    SetError(ERROR_TXLISTREF_NO_ERROR);
    return pPreviousNode->GetData();
  }
  SetError (ERROR_TXLISTREF_NO_NODE);
  return NULL;
}


/*==============================================================
FUNCTION: Clear

DESCRIPTION:
Clears all the nodes in the list.

INPUT/OUTPUT PARAMETERS:
None 

RETURN VALUE:
TRUE if all the nodes are freed else FALSE.

SIDE EFFECTS:
Memory allocated for userdefined objects need to be freed apprpriately.
==============================================================*/
template <class T>
bool TxListRef<T>::Clear ()
{
  ListNodeRef<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode ->next;
    (pTempNode->GetData())->~T();
    delete (pTempNode);
    nSize --;
  }
  pFirstNode = NULL;
  pLastNode = NULL;
  SetError(ERROR_TXLISTREF_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: Search

DESCRIPTION:
searches for the node data.

INPUT/OUTPUT PARAMETERS:
nodeelement - node element to search for.

RETURN VALUE:
Pointer to node element. NULL on failure.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T * TxListRef<T>::Search (T *pNodeElement)
{
  ListNodeRef<T> *node = NULL;
  node = GetNodeFor(pNodeElement);
  if (node != NULL){
    SetError(ERROR_TXLISTREF_NO_ERROR);
    return node->GetData();
  }
  SetError(ERROR_TXLISTREF_INVALID_NODE);
  return NULL;
}


/*==============================================================
FUNCTION: Merge

DESCRIPTION:
The method appends the supplied TxListRef to the existing one.

INPUT/OUTPUT PARAMETERS:
newlist - Pointer to TxListRef object to merge.

RETURN VALUE:
True if merge is succesful otherwise FALSE.

SIDE EFFECTS:
nwelist is not freed after the merge operation. Any operation 
on newlist may disturb the main list.
==============================================================*/
template <class T>
bool TxListRef<T>::Merge (TxListRef<T> *pNewList)
{
  ListNodeRef<T> *pHeadNode = NULL;
  if (pNewList == NULL){
    SetError(ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  }
  if ((pHeadNode = pNewList->GetFirst()) == NULL){
    SetError(ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  }
  if (pFirstNode == NULL){
    pFirstNode = pHeadNode;
    UpdateLastNode(pFirstNode);
    nSize += pNewList->GetSize();
    SetError(ERROR_TXLISTREF_NO_ERROR);
    return TRUE;
  }
  UpdateLastNode(pLastNode);
  nSize += pNewList->GetSize();
  pLastNode->next = pHeadNode;
  UpdateLastNode(pLastNode);
  pNewList->SetFirst(NULL);

  SetError(ERROR_TXLISTREF_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: Push Before

DESCRIPTION:
The method pushes an data before a specified element.

INPUT/OUTPUT PARAMETERS:
Nodelelement - The element before which the data is to be inserted.
Nodedata - The data which is to be inserted in the list.

RETURN VALUE:
True if push is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
bool TxListRef<T>::PushBefore (T *pNodeElement, T *pNodeData)
{
  if ((pNodeData == NULL) || (pNodeElement == NULL)){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  } 

  ListNodeRef<T> *pNode = new ListNodeRef<T>(pNodeData);

  ListNodeRef<T> *pTemp = NULL;
  ListNodeRef<T> *pTempNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if (*(pTempNode->GetData()) == *pNodeElement){
        break;
      }
      pTemp = pTempNode;
      pTempNode = pTempNode->next;
    }
    if (pTempNode != NULL){
      if (pFirstNode == pTempNode){
        pNode->next = pFirstNode;
        pFirstNode = pNode;
      }
      else{
        pTemp->next = pNode;
        pNode->next = pTempNode;
      }	
      SetError(ERROR_TXLISTREF_NO_ERROR);
      return TRUE;
    }
    else{
      SetError(ERROR_TXLISTREF_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_TXLISTREF_EMPTY_LIST);
  return FALSE;
}

/*==============================================================
FUNCTION: Push After

DESCRIPTION:
The method pushes an data after a specified element.

INPUT/OUTPUT PARAMETERS:
Nodelelement - The element after which the data is to be inserted.
Nodedata - The data which is to be inserted in the list.

RETURN VALUE:
True if push is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
bool TxListRef<T>::PushAfter (T *pNodeElement, T *pNodeData)
{
  ListNodeRef<T> *pNodeAfter = new ListNodeRef<T>(pNodeElement);
  ListNodeRef<T> *pNode = new ListNodeRef<T>(pNodeData);

  if ((pNodeData == NULL) || (pNodeElement == NULL)){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  }
  ListNodeRef<T> *pTempNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if (*(pTempNode->GetData()) == *pNodeElement){
        break;
      }
      pTempNode = pTempNode->next;
    }
    if(pTempNode != NULL){
      pNode->next = pTempNode->next;
      pTempNode->next = pNode;
      UpdateLastNode(pLastNode);
      SetError(ERROR_TXLISTREF_NO_ERROR);
      return TRUE;
    }
    else{
      SetError (ERROR_TXLISTREF_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_TXLISTREF_EMPTY_LIST);
  return FALSE;
}

/*==============================================================
FUNCTION: insert_at

DESCRIPTION:
The method inserts data at particular index

INPUT/OUTPUT PARAMETERS:
Index - The index at data is to be inserted.
pNodedata - The data which is to be inserted in the list.

RETURN VALUE:
True if Insert is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
bool TxListRef<T>::insert_at(T *pNodeData ,int32 index)
{
  ListNodeRef<T> *pTempNode = NULL;
  ListNodeRef<T> *pTempPrev = NULL;
  ListNodeRef<T> *pNode = NULL;

  int32 NodeCount = 0;

  if((pNodeData == NULL) || (index < 0) || (index > nSize)){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  } 

  pTempNode = pFirstNode;
  pTempPrev = NULL;

  int i;
  for (i = 0; i < index; i++){
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;

    if (pTempNode == NULL){
       break;
    }    
  }

  pNode = new ListNodeRef<T>(pNodeData);
  
  if (pNode == NULL){
    SetError (ERROR_TXLISTREF_INSUFFICIENT_MEMORY);
    return FALSE;
  }
  
  if (pTempPrev == NULL){
    pNode->next = pFirstNode;
    pFirstNode = pNode;
  }
  else{
    pTempPrev->next = pNode;
    pNode->next = pTempNode;
    UpdateLastNode(pLastNode);
  }
  nSize++;
  return FALSE;
}

/*==============================================================
FUNCTION: delete_at

DESCRIPTION:
The method deletes data in list at particular index

INPUT/OUTPUT PARAMETERS:
Index - index at which data is to be deleted

RETURN VALUE:
True if Delete is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
bool TxListRef<T>::delete_at(int32 index)
{
  ListNodeRef<T> *pTempNode = NULL;
  ListNodeRef<T> *pTempPrev = NULL;

  if((index < 0) || (index >= nSize)){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  } 

  if (pFirstNode == NULL){
    SetError (ERROR_TXLISTREF_EMPTY_LIST);
    return FALSE;
  }

  pTempNode = pFirstNode;
  pTempPrev = NULL;

  int i;
  for (i = 0; i < index; i++){
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;

    if (pTempNode == NULL){      
        SetError (ERROR_TXLISTREF_OUT_OF_RANGE);
        return FALSE;
    }    
  }
  
  if (pTempPrev == NULL){
    if (pTempNode != NULL){
      pFirstNode = pTempNode->next;
      if (pLastNode == pTempNode){
        UpdateLastNode (pFirstNode);
      }
      nSize--;
      delete pTempNode;
    }
  }
  else{
    if (pTempNode != NULL){
      pTempPrev->next = pTempNode->next;
      if (pLastNode == pTempNode){
        UpdateLastNode (pTempPrev);
      }
      nSize--;
      delete pTempNode;
    }
  }
  return FALSE;
}


/*==============================================================
FUNCTION: delete_data

DESCRIPTION:
The method deletes data in list

INPUT/OUTPUT PARAMETERS:
nodedata - The data which is to be deleted in the list.

RETURN VALUE:
True if Delete is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/

template <class T> 
bool TxListRef<T>::delete_data(T *pNodeData)
{
  ListNodeRef<T> *pTempNode = NULL;
  ListNodeRef<T> *pTempPrev = NULL;

  if(pNodeData == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return FALSE;
  } 

  if (pFirstNode == NULL){
    SetError (ERROR_TXLISTREF_EMPTY_LIST);
    return FALSE;
  }

  pTempNode = pFirstNode;
  pTempPrev = NULL;
  
  while(pTempNode != NULL){
    if (*(pTempNode->GetData()) == *pNodeData){
      break;
    }
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;
  }
  
  if (pTempPrev == NULL){
    pFirstNode = pTempNode->next;
    if (pLastNode == pTempNode){
      UpdateLastNode (pFirstNode);
    }
    nSize--;
    delete pTempNode;
  }
  else{
    if (pTempNode != NULL){
      pTempPrev->next = pTempNode->next;
      if (pLastNode == pTempNode){
        UpdateLastNode (pTempPrev);
      }
      nSize--;
      delete pTempNode;
    }
    else{
      SetError (ERROR_TXLISTREF_OUT_OF_RANGE);
      return FALSE;
    }
  }
  return TRUE;
}

/*==============================================================
FUNCTION: element_at

DESCRIPTION:
The method gives list data at particular index

INPUT/OUTPUT PARAMETERS:
Index - The index at which data is needed 

RETURN VALUE:
List Data at particular Index

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
T * TxListRef<T>::element_at(int32 index){

  ListNodeRef<T> *pTempNode = NULL;
  ListNodeRef<T> *pTempPrev = NULL;
  ListNodeRef<T> *pNode = NULL;

  int32 NodeCount = 0;
  if((index < 0) || (index >= nSize)){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return NULL;
  } 

  pTempNode = pFirstNode;
  pTempPrev = NULL;

  int i;
  for (i = 0; i < index; i++){
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;

    if (pTempNode == NULL){      
        SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
        return NULL;
    }
  }  
  
  if(pTempNode != NULL){
    return pTempNode->GetData();
  }
  else{
    SetError(ERROR_TXLISTREF_OUT_OF_RANGE);
    return NULL;
  }


}


/*==============================================================
FUNCTION: Split

DESCRIPTION:
The method splits the list starting from a specified element.

INPUT/OUTPUT PARAMETERS:
Nodelelement - The element from where split occurs.

RETURN VALUE:
True if split is succesful otherwise FALSE.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
TxListRef<T> *TxListRef<T>::Split (const T *pNodeElement)
{
  ListNodeRef<T> *pTemp = NULL; 
  ListNodeRef<T> *pTempNode = NULL; 
  TxListRef<T> *pNewList = NULL;

  if (pNodeElement == NULL){
    SetError(ERROR_TXLISTREF_INVALID_PARAMETERS);
    return NULL;
  }

  pNewList = new TxListRef<T>();
  if (pFirstNode == NULL){
    SetError(ERROR_TXLISTREF_EMPTY_LIST);
    return pNewList;
  }

  if (pNewList == NULL){
    SetError(ERROR_TXLISTREF_INSUFFICIENT_MEMORY);
    return pNewList;
  }

  pTempNode = pFirstNode;
  while (!(*(pTempNode->GetData()) == *pNodeElement)){
    pTemp = pTempNode;
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){
      SetError(ERROR_TXLISTREF_INVALID_PARAMETERS);
      return pNewList;
    }
  }
  if (pTempNode == pFirstNode){
    pFirstNode = NULL;
  }

  pNewList->UpdateLastNode(pLastNode);

  pLastNode = pTemp;
  if (pLastNode != NULL){
    pLastNode->next = NULL;
  }
  Resize();
  pNewList->SetFirst(pTempNode);
  pNewList->Resize();
  return pNewList;
}

/*==============================================================
FUNCTION: SetFirst

DESCRIPTION:
The method Sets the header node for the list.

INPUT/OUTPUT PARAMETERS:
node - List node to be updated as the first node.

RETURN VALUE:
True if head node is updated succesfully.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
bool TxListRef<T>::SetFirst(ListNodeRef<T> *pNode){
  pFirstNode = pNode;
  UpdateLastNode(pFirstNode); 
  return TRUE;
}


/*==============================================================
FUNCTION: GetFirst

DESCRIPTION:
Returns the OrderedListNodeRef holding the first node data from TxOrderedList.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
OrderedListNodeRef that holds the first node data specified.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
ListNodeRef<T> *TxListRef<T>::GetFirst (){
  return pFirstNode;
}

/*==============================================================
FUNCTION: GetNodeFor

DESCRIPTION:
Returns the ListNodeRef holding the node data from TxListRef.

INPUT/OUTPUT PARAMETERS:
nodedata - data object to search for

RETURN VALUE:
ListNodeRef that holds the node data specified.

SIDE EFFECTS:
If two nodes contains the same node data, only the first occuring ListNodeRef 
is returned.
==============================================================*/
template <class T>
ListNodeRef<T> *TxListRef<T>::GetNodeFor (T *pNodeData){
  ListNodeRef<T> *pTempNode = NULL;
  if (pNodeData == NULL){
    SetError (ERROR_TXLISTREF_INVALID_PARAMETERS);
    return NULL;
  }
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if (pNodeData == pTempNode->GetData()){
        SetError(ERROR_TXLISTREF_NO_ERROR);
        return pTempNode;
      }
      pTempNode = pTempNode->next;
    }
    SetError(ERROR_TXLISTREF_NODE_NOT_FOUND);
  }
  else{
    SetError(ERROR_TXLISTREF_EMPTY_LIST);
  }
  return NULL;
}


/*==============================================================
FUNCTION: SetError

DESCRIPTION:
Sets the error value.

INPUT/OUTPUT PARAMETERS:
errorvalue - error value to be set.

RETURN VALUE:
None

SIDE EFFECTS:
None
==============================================================*/
template <class T>
inline void TxListRef<T>::SetError (ErrorTxListRefType errorvalue){
  error = errorvalue;
}


/*==============================================================
FUNCTION: GetError

DESCRIPTION:
Gets the error value.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Error value that was last set.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
inline ErrorTxListRefType TxListRef<T>::GetLastError (){
  return error;
}


/*==============================================================
FUNCTION: GetLastErrorStr

DESCRIPTION:
Gets the character string description for the last occured error.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
character string description for the last occured error.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
const char *TxListRef<T>::GetLastErrorStr (){
  switch (error){
  case ERROR_TXLISTREF_NO_ERROR:
    return "No Error";
    break;
  case ERROR_TXLISTREF_INVALID_PARAMETERS:
    return "Error validating input parameters";
    break;
  case ERROR_TXLISTREF_INSUFFICIENT_MEMORY:
    return "Error Insufficient memory";
    break;
  case ERROR_TXLISTREF_INVALID_NODE:
    return "Error invalid node";
    break;
  case ERROR_TXLISTREF_NODE_NOT_FOUND:
    return "Error node not found";
    break;
  case ERROR_TXLISTREF_EMPTY_LIST:
    return "Error empty list";
    break;
  case ERROR_TXLISTREF_NO_NODE:
    return "Error No Node"; 
    break;
  case ERROR_TXLISTREF_OUT_OF_RANGE:
    return "Error Out of range Node";
    break;
  default:
    return "Error Unknown";
    break;
  }
  return "Error Unknown";
}


/*==============================================================
FUNCTION: UpdateLastNode

DESCRIPTION:
Updates the last node of the list.

INPUT/OUTPUT PARAMETERS:
node - reference node element to start upating the last node.

RETURN VALUE:
TRUE if the last node is updated succesfully.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
bool TxListRef<T>::UpdateLastNode (ListNodeRef<T> *pNode){
  if ((pNode == NULL) && (pFirstNode != NULL)){
    pNode = pFirstNode;
  }
  if (pNode == NULL){
    pLastNode = NULL;
  }
  else{
    while (pNode->next != NULL){
      pNode = pNode->next;
    }
    pLastNode = pNode;
  }
  return TRUE;
}


/*==============================================================
FUNCTION: Resize

DESCRIPTION:
Updates the size of the list.

INPUT/OUTPUT PARAMETERS:
none

RETURN VALUE:
none

SIDE EFFECTS:
None
==============================================================*/
template <class T>
void TxListRef<T>::Resize(void)
{
  ListNodeRef<T> *pTempNode = NULL;

  nSize = 0;
  pTempNode = pFirstNode;

  while (pTempNode != NULL){
    pTempNode = pTempNode->next;
    nSize ++;
  }
  return;	
}


#endif
