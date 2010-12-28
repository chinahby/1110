#ifndef __QLIST_H__
#define __QLIST_H__
/*===========================================================================


Q L I S T

H E A D E R  F I L E

FILE:  QList.h

DESCRIPTION

This header file declares a class QList and methods to access list 
functionality. The list class maintains a list of template objects.


PUBLIC CLASSES 
class QListNode
class QList
class QOrdList

INITIALIZATION AND SEQUENCING REQUIREMENTS
NA

          (c) COPYRIGHT <2005> QUALCOMM Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE
 $Header: //source/qcom/qct/multimedia/qtv/utils/list/main/latest/inc/qlist.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/


/*===========================================================================

INCLUDE FILES

=====================================================================*/
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "comdef.h"

enum ErrorQListType{
  ERROR_QList_NO_ERROR = 0,
  ERROR_QList_INVALID_PARAMETERS,
  ERROR_QList_INSUFFICIENT_MEMORY,
  ERROR_QList_INVALID_NODE,
  ERROR_QList_NODE_NOT_FOUND,
  ERROR_QList_EMPTY_LIST,
  ERROR_QList_NO_NODE,
  ERROR_QList_OUT_OF_RANGE,
  ERROR_QList_SIZER = 0x0FFFFFFF
};

enum OrderType{
  ASCE_ORDER = 0,
  DESC_ORDER
};

typedef int (*QListCompareFuncType)(void *obj1, void *obj2);

/*===================================================================
CLASS DEFINITION

CLASS: QListNode

DESCRIPTION:
Class having different functions to access a single node of a list.

PUBLIC FUNCTIONS & CONSTRUCTORS:

QListNode(const T *nodedata);

T *GetNode();

PRIVATE FUNCTION
===================================================================*/
template <class T>
class QListNode{

private:
  T data;

public:
  QListNode<T> *next;

  /*==============================================================
  FUNCTION:  QListNode

  DESCRIPTION:
  constructor for QListNode from a template object

  INPUT/OUTPUT PARAMETERS:
  const T *nodedata  - user defined data structure object.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  ==============================================================*/
  QListNode (const T &nodedata);

  /*==============================================================
  FUNCTION: QList

  DESCRIPTION:
  Default destructor for the QListNode class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  virtual ~QListNode(){}

  /*==============================================================
  FUNCTION:  GetNode

  DESCRIPTION:
  Method gets the nodedata for the QListNode.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to the nodedata object.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T &GetData();

}; //End class QListNode


/*================================================================

CLASS DEFINITION

CLASS:
QList

DESCRIPTION:
Class having different functions to maintain a list of QListNodes.

PUBLIC FUNCTIONS & CONSTRUCTORS:

QList();

~QList();

bool push_front (const T &nodedata);

bool push_back (const T &nodedata);

T &pop_front();

T &pop_back();

T &peek_front ();

T &peek_back();

int32 size();

T &get_next ();

T &get_prev ();

bool clear ();

T & search (T &nodeelement);

bool merge (QList<T> *newlist);

bool push_before (const T &nodeelement, const T &nodedata);

bool push_after (const T &nodeelement, const T &nodedata);

bool insert_at(const T &pNodeData, int32 index);

bool delete_at(int32 index);

bool delete_data(T &pNodedata);

T & element_at(int32 Index);

QList<T> *split(const T &nodeelement);

void resize(void);

inline ErrorQListType GetLastError ();

const char *GetLastErrorStr ();

T & operator [](const int i);

bool empty();


PRIVATE FUNCTION

bool SetFirst(QListNode<T> *node);

QListNode<T> *GetFirst ();

QListNode *GetNodeFor (T &nodedata);

bool UpdateLastNode (QListNode<T> *node);

inline void SetError (ErrorQListType errorvalue);


=================================================================*/
template <class T>
class QList
{
public:

  /*==============================================================
  FUNCTION: QList

  DESCRIPTION:
  Default constructor for the QList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  QList();


  /*==============================================================
  FUNCTION: QList

  DESCRIPTION:
  Default destructor for the QList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  virtual ~QList();  

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
  Gets the number of nodes present in the QList.

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
  The method appends the supplied QList to the existing one.

  INPUT/OUTPUT PARAMETERS:
  newlist - Pointer to QList object.

  RETURN VALUE:
  True if merge is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool merge (QList<T> *newlist);


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
  bool insert_at(const T &pNodeData, int32 index);


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
  QList<T> *split(const T &nodeelement);


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
  inline ErrorQListType GetLastError ();


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
  bool SetFirst(QListNode<T> *node);


  /*==============================================================
  FUNCTION: GetFirst

  DESCRIPTION:
  Returns the QListNode holding the first node data from TxOrderedList.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  QListNode that holds the first node data specified.

  SIDE EFFECTS:
  None
  ==============================================================*/
  QListNode<T> *GetFirst ();

  /*==============================================================
  FUNCTION: GetNodeFor

  DESCRIPTION:
  Returns the QListNode holding the node data from QList.

  INPUT/OUTPUT PARAMETERS:
  nodedata - data object to search for

  RETURN VALUE:
  QListNode that holds the node data specified.

  SIDE EFFECTS:
  If two nodes contains the same node data, only the first occuring 
  QListNode is returned.
  ==============================================================*/
  QListNode<T> *GetNodeFor (T &nodedata);

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
  bool UpdateLastNode (QListNode<T> *node);

public:
  static int CompareObjects (void* obj1, void *obj2){
     return ((int)obj1 - (int)obj2);
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
  inline void SetError (ErrorQListType errorvalue);

public:
  QListCompareFuncType Compare_Func;

private:

  QListNode<T>*    pFirstNode;
  QListNode<T>*    pLastNode;
  QListNode<T>*    pCurNode;
  ErrorQListType error;
  T dummyVal;
  int32 nSize;
};


/*==============================================================
FUNCTION:  QListNode

DESCRIPTION:
constructor for QListNode from a template object

INPUT/OUTPUT PARAMETERS:
const T *nodedata  - user defined data structure object.

RETURN VALUE:
None

SIDE EFFECTS:
==============================================================*/
template <class T>
QListNode<T>::QListNode(const T &pNodeData){
  next = NULL; 
  data = pNodeData;
}


/*==============================================================
FUNCTION:  GetData

DESCRIPTION:
Method gets the nodedata for the QListNode.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Pointer to the nodedata object.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
T &QListNode<T>::GetData(){
  return data;
}


/*==============================================================
FUNCTION: QList

DESCRIPTION:
Default constructor for the QList class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
QList<T>::QList(){

  pFirstNode = NULL;
  pLastNode  = NULL;
  pCurNode   = NULL;
  nSize = 0;

  memset (&dummyVal, '\0', sizeof (T));
  SetError(ERROR_QList_NO_ERROR);
  Compare_Func = QList<T>::CompareObjects;
}


/*==============================================================
FUNCTION: ~QList

DESCRIPTION:
Default destructor for the QList class.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
template <class T>
QList<T>::~QList(){
  QListNode<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode  = pFirstNode;
    pFirstNode = pFirstNode->next;
    delete pTempNode;
  }
  pFirstNode = NULL;
  pLastNode  = NULL;
  pCurNode   = NULL;
  nSize = 0;
  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::push_front (const T &pNodeData)
{
  QListNode<T> *node = new QListNode<T>(pNodeData);
  if (node == NULL){
    SetError (ERROR_QList_INSUFFICIENT_MEMORY);
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
  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::push_back (const T &pNodeData)
{
  // creating list pNode for the object
  QListNode<T> *pNode = new QListNode<T>(pNodeData);
  if (pNode == NULL){
    SetError (ERROR_QList_INSUFFICIENT_MEMORY);
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
  SetError(ERROR_QList_NO_ERROR);
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
T &QList<T>::pop_front()
{
  T &returnVal = dummyVal;
  QListNode<T> *pTempNode = NULL;
  memset (&dummyVal, '\0', sizeof (T));
  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode->next;
    returnVal = pTempNode->GetData();
    delete (pTempNode);
    pTempNode = NULL;
    nSize --;
    SetError(ERROR_QList_NO_ERROR);

    if (pFirstNode == NULL){
      UpdateLastNode(pFirstNode);
    }
    return returnVal;
  }
  else{
    SetError (ERROR_QList_EMPTY_LIST);
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
T &QList<T>::pop_back()
{
  T &pRetVal = dummyVal;
  memset (&dummyVal, '\0', sizeof (T));
  QListNode<T> *pTemp = NULL;
  QListNode<T> *pTempNode = NULL;
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
    pRetVal = pTempNode->GetData();
    delete pTempNode;
    nSize --;
    SetError(ERROR_QList_NO_ERROR);
    return pRetVal;
  }
  SetError (ERROR_QList_EMPTY_LIST);
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
T &QList<T>::peek_front ()
{
  T &retval = dummyVal;
  memset (&dummyVal, '\0', sizeof (T));
  if (pFirstNode != NULL){
    pCurNode = pFirstNode;
    retval = pFirstNode->GetData();
    SetError(ERROR_QList_NO_ERROR);
  }
  else{
    SetError (ERROR_QList_EMPTY_LIST);    
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
T &QList<T>::peek_back()
{
  T &retval = dummyVal;
  memset (&dummyVal, '\0', sizeof (T));
  if (pLastNode != NULL){
    retval = pLastNode->GetData();
    pCurNode = pLastNode;
    SetError(ERROR_QList_NO_ERROR);
  }
  else{
    SetError (ERROR_QList_EMPTY_LIST);    
  }
  return retval;
}


/*==============================================================
FUNCTION: size

DESCRIPTION:
Gets the number of nodes present in the QList.

INPUT/OUTPUT PARAMETERS:
None 

RETURN VALUE:
Number of nodes present in the List as int32

SIDE EFFECTS:
None
==============================================================*/
template <class T>
int32 QList<T>::size() const
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
T &QList<T>::get_next ()
{
  QListNode<T> *node = NULL;
  T &returnVal = dummyVal;
  memset (&dummyVal, '\0', sizeof (T));

  node = pCurNode;
  
  if (node != NULL){
    node = node->next;
    if (node != NULL){
      pCurNode = node;
      SetError(ERROR_QList_NO_ERROR);
      return node->GetData();
    }
    else{
      SetError (ERROR_QList_NO_NODE);
      return returnVal;
    }
  }
  SetError (ERROR_QList_NO_NODE);
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
T &QList<T>::get_prev ()
{
  T &returnVal = dummyVal;

  QListNode<T> *pPreviousNode = NULL;
  QListNode<T> *pTempNode = NULL;
  memset (&dummyVal, '\0', sizeof (T));
  
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
    SetError(ERROR_QList_NO_ERROR);
    pCurNode = pPreviousNode;
    return pPreviousNode->GetData();
  }
  SetError (ERROR_QList_NO_NODE);
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
bool QList<T>::clear ()
{
  QListNode<T> *pTempNode = NULL;
  while (pFirstNode != NULL){
    pTempNode = pFirstNode;
    pFirstNode = pFirstNode ->next;
    //(pTempNode->GetData())->~T();
    delete (pTempNode);
    nSize --;
  }
  pFirstNode = NULL;
  pLastNode = NULL;
  SetError(ERROR_QList_NO_ERROR);
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
T &QList<T>::search (T &pNodeElement)
{
  T &returnVal = dummyVal;

  QListNode<T> *node = NULL;
  memset (&dummyVal, '\0', sizeof (T));
  node = GetNodeFor(pNodeElement);
  if (node != NULL){
    SetError(ERROR_QList_NO_ERROR);
    return node->GetData();
  }
  SetError(ERROR_QList_INVALID_NODE);
  return returnVal;
}


/*==============================================================
FUNCTION: merge

DESCRIPTION:
The method appends the supplied QList to the existing one.

INPUT/OUTPUT PARAMETERS:
newlist - Pointer to QList object to merge.

RETURN VALUE:
True if merge is succesful otherwise FALSE.

SIDE EFFECTS:
nwelist is not freed after the merge operation. Any operation 
on newlist may disturb the main list.
==============================================================*/
template <class T>
bool QList<T>::merge (QList<T> *pNewList)
{
  QListNode<T> *pHeadNode = NULL;

  if (pNewList == NULL){
    SetError(ERROR_QList_INVALID_PARAMETERS);
    return FALSE;
  }
  if ((pHeadNode = pNewList->GetFirst()) == NULL){
    SetError(ERROR_QList_INVALID_PARAMETERS);
    return FALSE;
  }
  if (pFirstNode == NULL){
    pFirstNode = pHeadNode;
    UpdateLastNode(pFirstNode);
    nSize += pNewList->size();
    SetError(ERROR_QList_NO_ERROR);
    return TRUE;
  }
  UpdateLastNode(pLastNode);
  nSize += pNewList->size();
  pLastNode->next = pHeadNode;
  UpdateLastNode(pLastNode);
  pNewList->SetFirst(NULL);

  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::push_before (T &pNodeElement, T &pNodeData)
{
  QListNode<T> *pNode = NULL;
  QListNode<T> *pTemp = NULL;
  QListNode<T> *pTempNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if ((*Compare_Func)(&pTempNode->GetData(), &pNodeElement) == 0)
        break;
      pTemp = pTempNode;
      pTempNode = pTempNode->next;
    }
    if (pTempNode != NULL){
      pNode = new QListNode<T>(pNodeData);
      if (pNode == NULL){
        SetError (ERROR_QList_INSUFFICIENT_MEMORY);
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
      SetError(ERROR_QList_NO_ERROR);
      nSize++;
      return TRUE;
    }
    else{
      SetError(ERROR_QList_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_QList_EMPTY_LIST);
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
bool QList<T>::push_after (T &pNodeElement, T &pNodeData)
{
  QListNode<T> *pTempNode = NULL;
  QListNode<T> *pNode = NULL;

  if(pFirstNode != NULL){
    pTempNode = pFirstNode;
    while(pTempNode != NULL){
      if ((*Compare_Func)(&pTempNode->GetData(), &pNodeElement) == 0)
        break;
      pTempNode = pTempNode->next;
    }
    if(pTempNode != NULL){
      pNode = new QListNode<T>(pNodeData);
      if (pNode == NULL){
        SetError (ERROR_QList_INSUFFICIENT_MEMORY);
        return FALSE;
      }
      pNode->next = pTempNode->next;
      pTempNode->next = pNode;
      UpdateLastNode(pLastNode);
      SetError(ERROR_QList_NO_ERROR);
      nSize++;
      return TRUE;
    }
    else{
      SetError (ERROR_QList_NO_NODE);
      return FALSE;
    }
  }
  SetError(ERROR_QList_EMPTY_LIST);
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
bool QList<T>::insert_at(const T &pNodeData ,int32 index)
{
  QListNode<T> *pTempNode = NULL;
  QListNode<T> *pTempPrev = NULL;
  QListNode<T> *pNode = NULL;

  if(/*(pNodeData == NULL) ||*/ (index < 0) || (index > nSize)){
    SetError (ERROR_QList_INVALID_PARAMETERS);
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

  pNode = new QListNode<T>(pNodeData);
  
  if (pNode == NULL){
    SetError (ERROR_QList_INSUFFICIENT_MEMORY);
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
  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::delete_at(int32 index)
{
  QListNode<T> *pTempNode = NULL;
  QListNode<T> *pTempPrev = NULL;

  if((index < 0) || (index >= nSize)){
    SetError (ERROR_QList_INVALID_PARAMETERS);
    return FALSE;
  } 

  if (pFirstNode == NULL){
    SetError (ERROR_QList_EMPTY_LIST);
    return FALSE;
  }

  pTempNode = pFirstNode;
  pTempPrev = NULL;
  int i;
  for (i = 0; i < index; i++){
    pTempPrev = pTempNode;
    pTempNode = pTempNode->next;

    if (pTempNode == NULL){      
        SetError (ERROR_QList_OUT_OF_RANGE);
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
  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::delete_data(T &pNodeData)
{
  QListNode<T> *pTempNode = NULL;
  QListNode<T> *pTempPrev = NULL;

  if (pFirstNode == NULL){
    SetError (ERROR_QList_EMPTY_LIST);
    return FALSE;
  }
  pTempNode = pFirstNode;
  pTempPrev = NULL;
  
  while(pTempNode != NULL){
    if ((*Compare_Func)(&pTempNode->GetData(), &pNodeData) == 0)
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
      SetError (ERROR_QList_OUT_OF_RANGE);
      return FALSE;
    }
  }
  SetError(ERROR_QList_NO_ERROR);
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
T &QList<T>::element_at(int32 index){

  T &returnVal = dummyVal;
  QListNode<T> *pTempNode = NULL;
  memset (&dummyVal, '\0', sizeof (T));

  if((index < 0) || (index >= nSize)){
    SetError (ERROR_QList_INVALID_PARAMETERS);
    return returnVal;
  } 

  pTempNode = pFirstNode;
  int i;
  for (i = 0; i < index; i++){
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){      
        SetError (ERROR_QList_INVALID_PARAMETERS);
        return returnVal;
    }
  }  

  if(pTempNode != NULL){
    SetError(ERROR_QList_NO_ERROR);
    return pTempNode->GetData();
  }
  else{
    SetError(ERROR_QList_OUT_OF_RANGE);
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
QList<T> *QList<T>::split (const T &pNodeElement)
{
  QListNode<T> *pTemp = NULL; 
  QListNode<T> *pTempNode = NULL; 
  QList<T> *pNewList = NULL;

  if (pFirstNode == NULL){
    SetError(ERROR_QList_EMPTY_LIST);
    return pNewList;
  }

  pNewList = new QList<T>();
  if (pNewList == NULL){
    SetError(ERROR_QList_INSUFFICIENT_MEMORY);
    return pNewList;
  }

  pTempNode = pFirstNode;
  
  
  while (!(*Compare_Func)(&pTempNode->GetData() ,&(T)pNodeElement) == 0){
    pTemp = pTempNode;
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){
      SetError(ERROR_QList_INVALID_PARAMETERS);
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
  SetError(ERROR_QList_NO_ERROR);
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
bool QList<T>::SetFirst(QListNode<T> *pNode){
  pFirstNode = pNode;
  UpdateLastNode(pFirstNode); 
  return TRUE;
}


/*==============================================================
FUNCTION: GetFirst

DESCRIPTION:
Returns the OrderedQListNode holding the first node data from TxOrderedList.

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
OrderedQListNode that holds the first node data specified.

SIDE EFFECTS:
None
==============================================================*/
template <class T>
QListNode<T> *QList<T>::GetFirst (){
  return pFirstNode;
}


/*==============================================================
FUNCTION: GetNodeFor

DESCRIPTION:
Returns the QListNode holding the node data from QList.

INPUT/OUTPUT PARAMETERS:
nodedata - data object to search for

RETURN VALUE:
QListNode that holds the node data specified.

SIDE EFFECTS:
If two nodes contains the same node data, only the first occuring QListNode 
is returned.
==============================================================*/
template <class T>
QListNode<T> *QList<T>::GetNodeFor (T &pNodeData){
  QListNode<T> *pTempNode = NULL;

  if (pFirstNode != NULL){
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if ((*Compare_Func)(&pNodeData ,&pTempNode->GetData()) == 0){
        SetError(ERROR_QList_NO_ERROR);
        return pTempNode;
      }
      pTempNode = pTempNode->next;
    }
    SetError(ERROR_QList_NODE_NOT_FOUND);
  }
  SetError(ERROR_QList_EMPTY_LIST);
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
inline void QList<T>::SetError (ErrorQListType errorvalue){
  error = errorvalue;
}


/*==============================================================
FUNCTION: GetLastError

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
inline ErrorQListType QList<T>::GetLastError (){
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
const char *QList<T>::GetLastErrorStr (){
  const char* rslStr = NULL;
  switch (error){
  case ERROR_QList_NO_ERROR:
    rslStr = "No Error";
    break;
  case ERROR_QList_INVALID_PARAMETERS:
    rslStr = "Error validating input parameters";
    break;
  case ERROR_QList_INSUFFICIENT_MEMORY:
    rslStr = "Error Insufficient memory";
    break;
  case ERROR_QList_INVALID_NODE:
    rslStr = "Error invalid node";
    break;
  case ERROR_QList_NODE_NOT_FOUND:
    rslStr = "Error node not found";
    break;
  case ERROR_QList_EMPTY_LIST:
    rslStr = "Error empty list";
    break;
  case ERROR_QList_NO_NODE:
    rslStr = "Error No Node"; 
    break;
  case ERROR_QList_OUT_OF_RANGE:
    rslStr = "Error Out of range Node";
    break;
  default:
    rslStr = "Error Unknown";
    break;
  }
  return rslStr;
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
bool QList<T>::UpdateLastNode (QListNode<T> *pNode){
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
template <class T>
void QList<T>::resize(void)
{
  QListNode<T> *pTempNode = NULL;

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
bool QList<T>::empty()
{
	if( QList<T>::size() == 0 )
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
T &QList<T>::operator[](const int Index) const {

  T& returnVal = (T&)dummyVal;
  QListNode<T> *pTempNode = NULL;
  memset (&returnVal, '\0', sizeof (T));

  if((Index < 0) || (Index >= nSize)){
    return returnVal;
  } 

  pTempNode = pFirstNode;
  int i;
  for (i = 0; i < Index; i++){
    pTempNode = pTempNode->next;
    if (pTempNode == NULL){      
        return returnVal;
    }
  }  

  if(pTempNode != NULL){
    return pTempNode->GetData();
  }
  else{
    return returnVal;
  }
}

/*=============================================================================
CLASS DEFINITION

CLASS:
QOrdList

DESCRIPTION:
Class having different functions to maintain a ordered list of QListNodes.

PUBLIC FUNCTIONS & CONSTRUCTORS:
QOrdList();
~QOrdList();
bool insert (const T &nodedata);

Note: Compiler doesn't like template derivation, so have to use encapsulation
=============================================================================*/
template <class T>
class QOrdList : public QList<T>
{
public:
  QOrdList(){order = ASCE_ORDER;};
  virtual ~QOrdList();

  void init(OrderType listOrder, QListCompareFuncType func);
  int insert (const T &nodedata);

  // The following functions are delegated to QList<T>
  T &pop_front(){return list.pop_front();}
  T &pop_back(){return list.pop_back();}
  T &peek_front(){return list.peek_front();}
  T &peek_back(){return list.peek_back();}
  int32 size() const{return list.size();}
  T &get_next(){return list.get_next();}
  T &get_prev(){return list.get_prev();}
  bool clear(){return list.clear();}
  T &search (T &nodeelement){return list.search(nodeelement);}
  bool delete_at(int32 index){return list.delete_at(index);}
  bool delete_data(T &pNodedata){return list.delete_data(pNodedata);}
  T &element_at(int32 index){return list.element_at(index);}
  inline ErrorQListType GetLastError(){return list.GetLastError();}
  const char *GetLastErrorStr(){return list.GetLastErrorStr();}
  bool empty(){return list.empty();}
  T & operator [](const int index) const{return list[index];}
private:
   bool push_back (const T &nodeelement)
   {return list.push_back(nodeelement);}

   bool insert_at(const T &nodeelement, int32 index)
   {return list.insert_at(nodeelement, index);}

private:
   OrderType order;
   QList<T> list;
};

template <class T>
QOrdList<T>::~QOrdList(){}

template <class T>
void QOrdList<T>::init(OrderType listOrder, QListCompareFuncType func){
   list.Compare_Func = func;
   order = listOrder;
}

template <class T>
int QOrdList<T>::insert (const T &nodedata){
   int index = 0;
   const T& value = peek_front();
   // compare the nodedata
   int rsl = (*list.Compare_Func)((void*)&value, (void*)&nodedata);
   if (order == DESC_ORDER)
   {
      rsl = (*list.Compare_Func)((void*)&nodedata, (void*)&value);
   }
   // find the right place to insert
   while (GetLastError() == ERROR_QList_NO_ERROR && rsl < 0)
   {
      index++;
      const T& value = get_next();
      rsl = (*list.Compare_Func)((void*)&value, (void*)&nodedata);
      if (order == DESC_ORDER)
      {
         rsl = (*list.Compare_Func)((void*)&nodedata, (void*)&value);
      }
   }
   // found it?
   if (GetLastError() == ERROR_QList_NO_ERROR)
   {
      if (rsl == 0)
      {
         // the content is already in the list
         index = -1;
      }
      // insert here. Error code will be set properly by insert_at()
      else if (!insert_at(nodedata, index))
      {
         index = -1;
      }
   }
   else
   {
      // did not find proper place, apprend to the end of list.
      push_back(nodedata);
   }
   return index;
}

#endif
