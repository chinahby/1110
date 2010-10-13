#ifndef __TXLIST_H__
#define __TXLIST_H__
/*===========================================================================


T X L I S T

H E A D E R  F I L E

FILE:  TxList.h

DESCRIPTION

This header file declares a class QxList and methods to access list 
functionality. The list class maintains a list of template objects.


PUBLIC CLASSES 
class ListNode
class TxList

INITIALIZATION AND SEQUENCING REQUIREMENTS
NA

          (c) COPYRIGHT <2005> QUALCOMM Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE
 $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/TxList.h#2 $ 
 $DateTime: 2007/12/11 18:43:30 $  
 $Author: rajeevg $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/06   rv       for(int i; ; ) fix
03/30/06   ssm      Fixed bug where it was not updating pCurNode
03/29/06   ssm      Renamed TxList methods to match STL
03/27/06   ssm      Including [] operator from Rishi as this is need by 
                    IxDRMPermissionsContainer
02/27/06   rv       include 'malloc.h' only for LTK
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
#include "QxTypes.h"


typedef enum ErrorTxListType{
  ERROR_TXLIST_NO_ERROR = 0,
  ERROR_TXLIST_INVALID_PARAMETERS,
  ERROR_TXLIST_INSUFFICIENT_MEMORY,
  ERROR_TXLIST_INVALID_NODE,
  ERROR_TXLIST_NODE_NOT_FOUND,
  ERROR_TXLIST_EMPTY_LIST,
  ERROR_TXLIST_NO_NODE,
  ERROR_TXLIST_OUT_OF_RANGE,
  ERROR_TXLIST_SIZER = 0x0FFFFFFF
}ErrorTxListType_T;


/*===================================================================

CLASS DEFINITION

CLASS:
ListNode

DESCRIPTION:
Class having different functions to access a single node of a list.

PUBLIC FUNCTIONS & CONSTRUCTORS:

ListNode(const T *nodedata);

int32 GetIndex();

void SetIndex (int32 indexvalue);

T *GetNode();

PRIVATE FUNCTION

===================================================================*/
template <class T>
class ListNode{

private:
  T data;

public:
  ListNode<T> *next;

  /*==============================================================
  FUNCTION:  ListNode

  DESCRIPTION:
  constructor for ListNode from a template object

  INPUT/OUTPUT PARAMETERS:
  const T *nodedata  - user defined data structure object.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  ==============================================================*/
  ListNode (const T &nodedata);

  /*==============================================================
  FUNCTION: TxList

  DESCRIPTION:
  Default destructor for the ListNode class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  ~ListNode(){}

  /*==============================================================
  FUNCTION:  GetNode

  DESCRIPTION:
  Method gets the nodedata for the ListNode.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to the nodedata object.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T &GetData();

}; //End class ListNode


/*================================================================

CLASS DEFINITION

CLASS:
TxList

DESCRIPTION:
Class having different functions to maintain a list of ListNodes.

PUBLIC FUNCTIONS & CONSTRUCTORS:

TxList();

~TxList();

bool push_front (const T &nodedata);

bool push_back (const T &nodedata);

T &pop_front();

T &pop_back();

T &peek_front ();

T &peek_back();

int32 size();

T &get_next (T &currentnode);

T &get_prev (T &currentnode);

bool clear ();

T & search (T &nodeelement);

bool merge (TxList<T> *newlist);

bool push_before (const T &nodeelement, const T &nodedata);

bool push_after (const T &nodeelement, const T &nodedata);

bool insert_at(T &pNodeData, int32 index);

bool delete_at(int32 index);

bool delete_data(T &pNodedata);

T & element_at(int32 Index);

TxList<T> *split(const T &nodeelement);

void resize(void);

inline ErrorTxListType GetLastError ();

const QxChar *GetLastErrorStr ();

T & operator [](const int i);

bool empty();


PRIVATE FUNCTION

bool SetFirst(ListNode<T> *node);

ListNode<T> *GetFirst ();

ListNode *GetNodeFor (T &nodedata);

bool UpdateLastNode (ListNode<T> *node);

inline void SetError (ErrorTxListType errorvalue);


=================================================================*/
template <class T>
class TxList
{
public:

  /*==============================================================
  FUNCTION: TxList

  DESCRIPTION:
  Default constructor for the TxList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  TxList();


  /*==============================================================
  FUNCTION: TxList

  DESCRIPTION:
  Default destructor for the TxList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  ~TxList();  

  /*==============================================================
  FUNCTION: push_front

  DESCRIPTION:
  Insers the nodedata object at the begining of the list.

  INPUT/OUTPUT PARAMETERS:
  nodedata  - reference to data object that need to be inserted at the 
  begining of the list.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  bool push_front (const T &nodedata);


  /*==============================================================
  FUNCTION: push_back

  DESCRIPTION:
  Inserts the nodedata object at the end of the list.

  INPUT/OUTPUT PARAMETERS:
  nodedata  - reference to data object that need to be inserted at the 
  end of the list.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  bool push_back (const T &nodedata);


  /*==============================================================
  FUNCTION: pop_front

  DESCRIPTION:
  Removes the first nodedata object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Reference to data object that is removed from the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T &pop_front();


  /*==============================================================
  FUNCTION: pop_back

  DESCRIPTION:
  Removes the last node object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Reference to data object that is removed from the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T &pop_back();


  /*==============================================================
  FUNCTION: peek_front

  DESCRIPTION:
  Returns the first nodedata object from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Reference to data object that is present at the begining of the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T &peek_front ();


  /*==============================================================
  FUNCTION: peek_back

  DESCRIPTION:
  Returns the last nodedata from the list.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Reference to data object that is present at the end of the list.

  SIDE EFFECTS:
  None.
  ==============================================================*/
  T &peek_back();


  /*==============================================================
  FUNCTION: size

  DESCRIPTION:
  Gets the number of nodes present in the TxList.

  INPUT/OUTPUT PARAMETERS:
  None 

  RETURN VALUE:
  Number of nodes present in the List as QxInt

  SIDE EFFECTS:
  None
  ==============================================================*/
  int32 size() const;


  /*==============================================================
  FUNCTION: get_next

  DESCRIPTION:
  Gets the next node from the node specified.

  INPUT/OUTPUT PARAMETERS:
  currentnode - reference node whose next node is required. 

  RETURN VALUE:
  Reference to node that is placed after the passed node.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T &get_next ();


  /*==============================================================
  FUNCTION: get_prev

  DESCRIPTION:
  Gets the previous node from the node specified.

  INPUT/OUTPUT PARAMETERS:
  currentnode - reference node whose previous node is required. 

  RETURN VALUE:
  Reference to node that is placed before the passed node.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T &get_prev ();


  /*==============================================================
  FUNCTION: clear

  DESCRIPTION:
  Clears all the nodes in the list.

  INPUT/OUTPUT PARAMETERS:
  None 

  RETURN VALUE:
  TRUE if all the nodes are freed else FALSE.

  SIDE EFFECTS:
  Memory allocated for userdefined objects need to be freed apprpriately.
  ==============================================================*/
  bool clear ();


  /*==============================================================
  FUNCTION: search

  DESCRIPTION:
  searches for the node.

  INPUT/OUTPUT PARAMETERS:
  nodeelement - node element to search for.

  RETURN VALUE:
  Reference to node element. NULL on failure.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T &search (T &nodeelement);


  /*==============================================================
  FUNCTION: merge

  DESCRIPTION:
  The method appends the supplied TxList to the existing one.

  INPUT/OUTPUT PARAMETERS:
  newlist - Pointer to TxList object.

  RETURN VALUE:
  True if merge is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool merge (TxList<T> *newlist);


  /*==============================================================
  FUNCTION: push_before

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
  bool push_before (T &nodeelement, T &nodedata);


  /*==============================================================
  FUNCTION: push_after

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
  bool push_after (T &nodeelement, T &nodedata);


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
  bool insert_at(T &pNodeData, int32 index);


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
  bool delete_data(T &pNodedata);


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
  T &element_at(int32 Index);


  /*==============================================================
  FUNCTION: split

  DESCRIPTION:
  The method splits the list starting from a specified element.

  INPUT/OUTPUT PARAMETERS:
  Nodelelement - The element from where split occurs.

  RETURN VALUE:
  True if split is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  TxList<T> *split(const T &nodeelement);


  /*==============================================================
  FUNCTION: resize

  DESCRIPTION:
  Updates the size of the list.

  INPUT/OUTPUT PARAMETERS:
  none

  RETURN VALUE:
  none

  SIDE EFFECTS:
  None
  ==============================================================*/
  void resize(void);


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
  inline ErrorTxListType GetLastError ();


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

  /*==============================================================
  FUNCTION: empty

  DESCRIPTION:
  Returns 'true' if the list is empty

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  boolean
  
  SIDE EFFECTS:
  None
  ==============================================================*/
  bool empty();


  /*==============================================================
  FUNCTION: operator []

  DESCRIPTION:
  Returns the element at specified index 

   INPUT/OUTPUT PARAMETERS:
  Index - The index at which data is needed


  RETURN VALUE:
  List Data at particular Index

  SIDE EFFECTS:
  None
  ==============================================================*/
  T & operator [](const int Index) const;

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
  bool SetFirst(ListNode<T> *node);


  /*==============================================================
  FUNCTION: GetFirst

  DESCRIPTION:
  Returns the ListNode holding the first node data from TxOrderedList.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  ListNode that holds the first node data specified.

  SIDE EFFECTS:
  None
  ==============================================================*/
  ListNode<T> *GetFirst ();

  /*==============================================================
  FUNCTION: GetNodeFor

  DESCRIPTION:
  Returns the ListNode holding the node data from TxList.

  INPUT/OUTPUT PARAMETERS:
  nodedata - data object to search for

  RETURN VALUE:
  ListNode that holds the node data specified.

  SIDE EFFECTS:
  If two nodes contains the same node data, only the first occuring 
  ListNode is returned.
  ==============================================================*/
  ListNode<T> *GetNodeFor (T &nodedata);

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
  bool UpdateLastNode (ListNode<T> *node);

  bool CompareObjects (T &obj1, T &obj2){
    return (obj1 == obj2);
  }

public:
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
  inline void SetError (ErrorTxListType errorvalue);

private:

  ListNode<T>*    pFirstNode;
  ListNode<T>*    pLastNode;
  ListNode<T>*    pCurNode;
  ErrorTxListType error;
  T dummyVal;
  int32 nSize;
};


/*==============================================================
FUNCTION:  ListNode

DESCRIPTION:
constructor for ListNode from a template object

INPUT/OUTPUT PARAMETERS:
const T *nodedata  - user defined data structure object.

RETURN VALUE:
None

SIDE EFFECTS:
==============================================================*/
template <class T>
ListNode<T>::ListNode(const T &pNodeData){
  next = NULL; 
  data = pNodeData;
}


/*==============================================================
FUNCTION:  GetData

DESCRIPTION:
Method gets the nodedata for the ListNode.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to the nodedata object.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T &ListNode<T>::GetData(){
  return data;
}


/*==============================================================
FUNCTION: TxList

DESCRIPTION:
Default constructor for the TxList class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
TxList<T>::TxList(){

  pFirstNode = NULL;
  pLastNode  = NULL;
  pCurNode   = NULL;
  nSize = 0;

  //memset (&dummyVal, '\0', sizeof (T));
  SetError(ERROR_TXLIST_NO_ERROR);
}


/*==============================================================
FUNCTION: ~TxList

DESCRIPTION:
Default destructor for the TxList class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
TxList<T>::~TxList(){
  ListNode<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode  = pFirstNode;
    pFirstNode = pFirstNode->next;
    delete(pTempNode);
  }
  pFirstNode = NULL;
  pLastNode  = NULL;
  pCurNode   = NULL;
  nSize = 0;
  SetError(ERROR_TXLIST_NO_ERROR);
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
bool TxList<T>::push_front (const T &pNodeData)
{
  ListNode<T> *node = new ListNode<T>(pNodeData);
  if (node == NULL){
    SetError (ERROR_TXLIST_INSUFFICIENT_MEMORY);
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
  SetError(ERROR_TXLIST_NO_ERROR);
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
bool TxList<T>::push_back (const T &pNodeData)
{
  // creating list pNode for the object
  ListNode<T> *pNode = new ListNode<T>(pNodeData);
  if (pNode == NULL){
    SetError (ERROR_TXLIST_INSUFFICIENT_MEMORY);
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
  SetError(ERROR_TXLIST_NO_ERROR);
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
T &TxList<T>::pop_front()
{
  T &returnVal = dummyVal;
  ListNode<T> *pTempNode = NULL;
//  memset (&dummyVal, '\0', sizeof (T));
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode->next;
    returnVal = pTempNode->GetData();
    delete (pTempNode);
    pTempNode = NULL;
    nSize --;
    SetError(ERROR_TXLIST_NO_ERROR);

    if (pFirstNode == NULL){
      UpdateLastNode(pFirstNode);
    }
    return returnVal;
  }
  else{
    SetError (ERROR_TXLIST_EMPTY_LIST);
    return returnVal;
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
T &TxList<T>::pop_back()
{
  T &pRetVal = dummyVal;
//  memset (&dummyVal, '\0', sizeof (T));
  ListNode<T> *pTemp = NULL;
  ListNode<T> *pTempNode = NULL;
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while ( pTempNode != NULL && pTempNode->next != NULL){
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
    nSize --;
    SetError(ERROR_TXLIST_NO_ERROR);
    return pTempNode->GetData();
  }
  SetError (ERROR_TXLIST_EMPTY_LIST);
  return pRetVal;
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
T &TxList<T>::peek_front ()
{
  T &retval = dummyVal;
//  memset (&dummyVal, '\0', sizeof (T));
  if (pFirstNode != NULL){
    pCurNode = pFirstNode;
    SetError(ERROR_TXLIST_NO_ERROR);
    return pFirstNode->GetData();
  }
  SetError (ERROR_TXLIST_EMPTY_LIST);
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
T &TxList<T>::peek_back()
{
  T &retval = dummyVal;
//  memset (&dummyVal, '\0', sizeof (T));
  if (pLastNode != NULL){
    retval = pLastNode->GetData();
    pCurNode = pLastNode;
    SetError(ERROR_TXLIST_NO_ERROR);
  }
  else{
    SetError (ERROR_TXLIST_EMPTY_LIST);    
  }
  return retval;
}


/*==============================================================
FUNCTION: size

DESCRIPTION:
Gets the number of nodes present in the TxList.

INPUT/OUTPUT PARAMETERS:
None 

RETURN VALUE:
Number of nodes present in the List as int32

SIDE EFFECTS:
None
==============================================================*/
template <class T>
int32 TxList<T>::size() const
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
T &TxList<T>::get_next ()
{
  ListNode<T> *node = NULL;
  T &returnVal = dummyVal;
//  memset (&dummyVal, '\0', sizeof (T));

  node = pCurNode;
  
  if (node != NULL){
    node = node->next;
    if (node != NULL){
      pCurNode = node;
      SetError(ERROR_TXLIST_NO_ERROR);
      return node->GetData();
    }
    else{
      SetError (ERROR_TXLIST_NO_NODE);
      return returnVal;
    }
  }
  SetError (ERROR_TXLIST_NO_NODE);
  return returnVal;
}


/*==============================================================
FUNCTION: get_prev

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
T &TxList<T>::get_prev ()
{
  T &returnVal = dummyVal;

  ListNode<T> *pPreviousNode = NULL;
  ListNode<T> *pTempNode = NULL;
//  memset (&dummyVal, '\0', sizeof (T));
  
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if ( pTempNode == pCurNode )
        break;
      pPreviousNode = pTempNode;
      pTempNode = pTempNode->next;
    }
  }
  if (pPreviousNode != NULL){
    SetError(ERROR_TXLIST_NO_ERROR);
    pCurNode = pPreviousNode;
    return pPreviousNode->GetData();
  }
  SetError (ERROR_TXLIST_NO_NODE);
  return returnVal;
}


/*==============================================================
FUNCTION: clear

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
bool TxList<T>::clear ()
{
  ListNode<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode ->next;
    //(pTempNode->GetData())->~T();
    delete (pTempNode);
    nSize --;
  }
  pFirstNode = NULL;
  pLastNode = NULL;
  SetError(ERROR_TXLIST_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: search

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
T &TxList<T>::search (T &pNodeElement)
{
  T &returnVal = dummyVal;

  ListNode<T> *node = NULL;
//  memset (&dummyVal, '\0', sizeof (T));
  node = GetNodeFor(pNodeElement);
  if (node != NULL){
    SetError(ERROR_TXLIST_NO_ERROR);
    return node->GetData();
  }
  SetError(ERROR_TXLIST_INVALID_NODE);
  return returnVal;
}


/*==============================================================
FUNCTION: merge

DESCRIPTION:
The method appends the supplied TxList to the existing one.

INPUT/OUTPUT PARAMETERS:
newlist - Pointer to TxList object to merge.

RETURN VALUE:
True if merge is succesful otherwise FALSE.

SIDE EFFECTS:
nwelist is not freed after the merge operation. Any operation 
on newlist may disturb the main list.
==============================================================*/
template <class T>
bool TxList<T>::merge (TxList<T> *pNewList)
{
  ListNode<T> *pHeadNode = NULL;

  if (pNewList == NULL){
    SetError(ERROR_TXLIST_INVALID_PARAMETERS);
    return FALSE;
  }
  if ((pHeadNode = pNewList->GetFirst()) == NULL){
    SetError(ERROR_TXLIST_INVALID_PARAMETERS);
    return FALSE;
  }
  if (pFirstNode == NULL){
    pFirstNode = pHeadNode;
    UpdateLastNode(pFirstNode);
    nSize += pNewList->size();
    SetError(ERROR_TXLIST_NO_ERROR);
    return TRUE;
  }
  UpdateLastNode(pLastNode);
  nSize += pNewList->size();
  pLastNode->next = pHeadNode;
  UpdateLastNode(pLastNode);
  pNewList->SetFirst(NULL);

  SetError(ERROR_TXLIST_NO_ERROR);
  return TRUE;
}


/*==============================================================
FUNCTION: push_before

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
bool TxList<T>::push_before (T &pNodeElement, T &pNodeData)
{
  ListNode<T> *pNode = NULL;
  ListNode<T> *pTemp = NULL;
  ListNode<T> *pTempNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if (CompareObjects (pTempNode->GetData(), pNodeElement))
        break;
      pTemp = pTempNode;
      pTempNode = pTempNode->next;
    }
    if (pTempNode != NULL){
      pNode = new ListNode<T>(pNodeData);
      if (pNode == NULL){
        SetError (ERROR_TXLIST_INSUFFICIENT_MEMORY);
        return FALSE;
      }
      if (pFirstNode == pTempNode){
        pNode->next = pFirstNode;
        pFirstNode = pNode;
      }
      else{
        pTemp->next = pNode;
        pNode->next = pTempNode;
      }	
      SetError(ERROR_TXLIST_NO_ERROR);
      nSize++;
      return TRUE;
    }
    else{
      SetError(ERROR_TXLIST_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_TXLIST_EMPTY_LIST);
  return FALSE;
}


/*==============================================================
FUNCTION: push_after

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
bool TxList<T>::push_after (T &pNodeElement, T &pNodeData)
{
  ListNode<T> *pTempNode = NULL;
  ListNode<T> *pNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if (CompareObjects(pTempNode->GetData(), pNodeElement))
        break;
      pTempNode = pTempNode->next;
    }
    if(pTempNode != NULL){
      pNode = new ListNode<T>(pNodeData );
      if (pNode == NULL){
        SetError (ERROR_TXLIST_INSUFFICIENT_MEMORY);
        return FALSE;
      }
      pNode->next = pTempNode->next;
      pTempNode->next = pNode;
      UpdateLastNode(pLastNode);
      SetError(ERROR_TXLIST_NO_ERROR);
      nSize++;
      return TRUE;
    }
    else{
      SetError (ERROR_TXLIST_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_TXLIST_EMPTY_LIST);
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
bool TxList<T>::insert_at(T &pNodeData ,int32 index)
{
  ListNode<T> *pTempNode = NULL;
  ListNode<T> *pTempPrev = NULL;
  ListNode<T> *pNode = NULL;

  if(/*(pNodeData == NULL) ||*/ (index < 0) || (index > nSize)){
    SetError (ERROR_TXLIST_INVALID_PARAMETERS);
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

  pNode = new ListNode<T>(pNodeData);
  
  if (pNode == NULL){
    SetError (ERROR_TXLIST_INSUFFICIENT_MEMORY);
    return FALSE;
  }
  
  if (pTempPrev == NULL){
    pNode->next = pFirstNode;
    pFirstNode = pNode;
    UpdateLastNode(pFirstNode);
  }
  else{
    pTempPrev->next = pNode;
    pNode->next = pTempNode;
    UpdateLastNode(pLastNode);
  }
  nSize++;
  SetError(ERROR_TXLIST_NO_ERROR);
  return TRUE;
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
bool TxList<T>::delete_at(int32 index)
{
  ListNode<T> *pTempNode = NULL;
  ListNode<T> *pTempPrev = NULL;

  if((index < 0) || (index >= nSize)){
    SetError (ERROR_TXLIST_INVALID_PARAMETERS);
    return FALSE;
  } 

  if (pFirstNode == NULL){
    SetError (ERROR_TXLIST_EMPTY_LIST);
    return FALSE;
  }

  pTempNode = pFirstNode;
  pTempPrev = NULL;
  int i;
  for (i = 0; i < index; i++){
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;

    if (pTempNode == NULL){      
        SetError (ERROR_TXLIST_OUT_OF_RANGE);
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
  SetError(ERROR_TXLIST_NO_ERROR);
  return TRUE;
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
bool TxList<T>::delete_data(T &pNodeData)
{
  ListNode<T> *pTempNode = NULL;
  ListNode<T> *pTempPrev = NULL;

  if (pFirstNode == NULL){
    SetError (ERROR_TXLIST_EMPTY_LIST);
    return FALSE;
  }
  pTempNode = pFirstNode;
  pTempPrev = NULL;
  
  while(pTempNode != NULL){
    if (CompareObjects(pTempNode->GetData(), pNodeData))
      break;
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
      SetError (ERROR_TXLIST_OUT_OF_RANGE);
      return FALSE;
    }
  }
  SetError(ERROR_TXLIST_NO_ERROR);
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
T &TxList<T>::element_at(int32 index){

  T &returnVal = dummyVal;
  ListNode<T> *pTempNode = NULL;
//  memset (&dummyVal, '\0', sizeof (T));

  if((index < 0) || (index >= nSize)){
    SetError (ERROR_TXLIST_INVALID_PARAMETERS);
    return returnVal;
  } 

  pTempNode = pFirstNode;
  int i;
  for (i = 0; i < index; i++){
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){      
        SetError (ERROR_TXLIST_INVALID_PARAMETERS);
        return returnVal;
    }
  }  

  if(pTempNode != NULL){
    SetError(ERROR_TXLIST_NO_ERROR);
    return pTempNode->GetData();
  }
  else{
    SetError(ERROR_TXLIST_OUT_OF_RANGE);
    return returnVal;
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
TxList<T> *TxList<T>::split (const T &pNodeElement)
{
  ListNode<T> *pTemp = NULL; 
  ListNode<T> *pTempNode = NULL; 
  TxList<T> *pNewList = NULL;

  if (pFirstNode == NULL){
    SetError(ERROR_TXLIST_EMPTY_LIST);
    return pNewList;
  }

  pNewList = new TxList<T>();
  if (pNewList == NULL){
    SetError(ERROR_TXLIST_INSUFFICIENT_MEMORY);
    return pNewList;
  }

  pTempNode = pFirstNode;
  
  
  while (!CompareObjects(pTempNode->GetData() ,(T)pNodeElement)){
    pTemp = pTempNode;
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){
      SetError(ERROR_TXLIST_INVALID_PARAMETERS);
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
  resize();
  pNewList->SetFirst(pTempNode);
  pNewList->resize();
  SetError(ERROR_TXLIST_NO_ERROR);
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
bool TxList<T>::SetFirst(ListNode<T> *pNode){
  pFirstNode = pNode;
  UpdateLastNode(pFirstNode); 
  return TRUE;
}


/*==============================================================
FUNCTION: GetFirst

DESCRIPTION:
Returns the OrderedListNode holding the first node data from TxOrderedList.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
OrderedListNode that holds the first node data specified.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
ListNode<T> *TxList<T>::GetFirst (){
  return pFirstNode;
}


/*==============================================================
FUNCTION: GetNodeFor

DESCRIPTION:
Returns the ListNode holding the node data from TxList.

INPUT/OUTPUT PARAMETERS:
nodedata - data object to search for

RETURN VALUE:
ListNode that holds the node data specified.

SIDE EFFECTS:
If two nodes contains the same node data, only the first occuring ListNode 
is returned.
==============================================================*/
template <class T>
ListNode<T> *TxList<T>::GetNodeFor (T &pNodeData){
  ListNode<T> *pTempNode = NULL;

  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if (CompareObjects (pNodeData ,pTempNode->GetData())){
        SetError(ERROR_TXLIST_NO_ERROR);
        return pTempNode;
      }
      pTempNode = pTempNode->next;
    }
    SetError(ERROR_TXLIST_NODE_NOT_FOUND);
  }
  SetError(ERROR_TXLIST_EMPTY_LIST);
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
inline void TxList<T>::SetError (ErrorTxListType errorvalue){
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
inline ErrorTxListType TxList<T>::GetLastError (){
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
const char *TxList<T>::GetLastErrorStr (){
  switch (error){
  case ERROR_TXLIST_NO_ERROR:
    return "No Error";

  case ERROR_TXLIST_INVALID_PARAMETERS:
    return "Error validating input parameters";

  case ERROR_TXLIST_INSUFFICIENT_MEMORY:
    return "Error Insufficient memory";

  case ERROR_TXLIST_INVALID_NODE:
    return "Error invalid node";

  case ERROR_TXLIST_NODE_NOT_FOUND:
    return "Error node not found";
 
  case ERROR_TXLIST_EMPTY_LIST:
    return "Error empty list";

  case ERROR_TXLIST_NO_NODE:
    return "Error No Node"; 

  case ERROR_TXLIST_OUT_OF_RANGE:
    return "Error Out of range Node";

  default:
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
bool TxList<T>::UpdateLastNode (ListNode<T> *pNode){
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
void TxList<T>::resize(void)
{
  ListNode<T> *pTempNode = NULL;

  nSize = 0;
  pTempNode = pFirstNode;

  while (pTempNode != NULL){
    pTempNode = pTempNode->next;
    nSize ++;
  }
  return;	
}


/*==============================================================
FUNCTION: empty

DESCRIPTION:
Returns 'true' if the list is empty

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
boolean

SIDE EFFECTS:
None
==============================================================*/
template <class T> 
bool TxList<T>::empty()
{
	if( TxList<T>::size() == 0 )
		return true;
	else
		return false;
}
/*==============================================================
FUNCTION: operator[]

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
T &TxList<T>::operator[](const int Index) const {

  ListNode<T> *pTempNode = NULL;
//  memset (&returnVal, '\0', sizeof (T));

  if((Index < 0) || (Index >= nSize)){
    return *((T*) &dummyVal);
  } 

  pTempNode = pFirstNode;
  int i;
  for (i = 0; i < Index; i++){
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){      
        return *((T*) &dummyVal);
    }
  }  

  if(pTempNode != NULL){
    return pTempNode->GetData();
  }
  else{
    return *((T*) &dummyVal);
  }
}

#endif
