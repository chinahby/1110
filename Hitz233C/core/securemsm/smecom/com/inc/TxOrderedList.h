#ifndef __TX_ORDERED_LIST_H__
#define __TX_ORDERED_LIST_H__
/*===========================================================================


T X O R D E R E D L I S T

H E A D E R  F I L E

FILE:  TxOrderedList.h

DESCRIPTION

This header file declares a class TxOrderedList and methods to access ordered 
list functionality. The list class maintains a list of template objects.


PUBLIC CLASSES 
class OrderedListNode
class TxOrderedList

INITIALIZATION AND SEQUENCING REQUIREMENTS
NA

(c) COPYRIGHT <2005> QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

EDIT HISTORY FOR FILE
 $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/TxOrderedList.h#1 $ 
 $DateTime: 2007/07/17 11:00:30 $  
 $Author: jmiera $ 



when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/06   rv       for(int i; ; ) fix
03/29/06   ssm      Renamed TxList methods to match STL
12/29/05   WAG      Moved public function UpdateLastNode,SetFirst to private 
                    function & private function GetLastError,GetlastErrorStr
                    to public function.
12/28/05   WAG      Remove redudant code as index validity in delete_at function
12/27/05   WAG      Check the last node validity in element_at function.
                    Increment value of nSize in InsertElementAt function.
12/26/05   WAG      Added 4 new functions for InsertElementAt,
                    element_at,DeleteElementAt,DeleteElementData 
12/09/05   CP       Incorporated review comments
10/17/05   Siv      Initial Version
===========================================================================*/


/*===========================================================================

INCLUDE FILES

=====================================================================*/
#include "QxTypes.h"


typedef enum ErrorTxOrderedListType{
  ERROR_TX_ORDERED_LIST_NO_ERROR = 0,
  ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS,
  ERROR_TX_ORDERED_LIST_INSUFFICIENT_MEMORY,
  ERROR_TX_ORDERED_LIST_INVALID_NODE,
  ERROR_TX_ORDERED_LIST_NODE_NOT_FOUND,
  ERROR_TX_ORDERED_LIST_OUT_OF_RANGE,
  ERROR_TX_ORDERED_LIST_EMPTY_LIST,
  ERROR_TX_ORDERED_LIST_NO_DUPLICATE,
  ERROR_TX_ORDERED_LIST_SIZER = 0x0FFFFFFF
};

typedef enum SortOrderType{
  SORT_ORDER_MIN = 0,
  SORT_ORDER_NO_ORDER,
  SORT_ORDER_ASCENDING,
  SORT_ORDER_DESCENDING,
  SORT_ORDER_SIZER = 0x0FFFFFFF
};


/*===================================================================

CLASS DEFINITION

CLASS:
OrderedListNode

DESCRIPTION:
Class having different functions to access a single node of a list.

PUBLIC FUNCTIONS & CONSTRUCTORS:

OrderedListNode(const T *nodedata);

QxInt GetIndex();

void SetIndex (QxInt indexvalue);

T *GetNode();

PRIVATE FUNCTION

===================================================================*/
template <class T>
class OrderedListNode{

private:
  T *data;

public:

  OrderedListNode<T> *next;     // next node
  OrderedListNode<T> *prev;     // previous node

  /*==============================================================
  FUNCTION:  OrderedListNode

  DESCRIPTION:
  constructor for OrderedListNode from a template object

  INPUT/OUTPUT PARAMETERS:
  const T *pNodeData  - user defined data structure object.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  ==============================================================*/
  OrderedListNode(const T *pNodeData){
    if (pNodeData == NULL){
      return;
    }
    data = (T *)pNodeData;
    next = NULL;
    prev = NULL;
  }

  /*==============================================================
  FUNCTION:  OrderedListNode

  DESCRIPTION:
  Destructor for OrderedListNode from a template object

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  ==============================================================*/

  ~OrderedListNode(){}

  /*==============================================================
  FUNCTION:  GetNode

  DESCRIPTION:
  Method gets the nodedata for the OrderedListNode.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  Pointer to the nodedata object.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T *GetData(){
    return data;
  }

  /*==============================================================
  FUNCTION:  SetNode

  DESCRIPTION:
  Method sets the nodedata for the OrderedListNode.

  INPUT/OUTPUT PARAMETERS:
  pNodeData - Pointer to the nodedata object.

  RETURN VALUE:
  None.

  SIDE EFFECTS:
  None
  ==============================================================*/
  void SetData(const T *pNodeData){
    data = (T*)pNodeData;
  }

};    //OrderedListNode Class


/*================================================================

CLASS DEFINITION

CLASS:
TxOrderedList

DESCRIPTION:
Class having different functions to maintain a list of OrderedListNodes.

PUBLIC FUNCTIONS & CONSTRUCTORS:

TxOrderedList(SortOrderType orderType = SORT_ORDER_ASCENDING);

~TxOrderedList();

T *pop_front();

T *pop_back();

T *peek_front ();

T *peek_back();

QxInt GetSize();

T *get_next (T *currentnode);

T *GetPrev (T *currentnode);

bool Clear ();

bool RemoveDuplicates();

bool Merge (TxOrderedList *newlist);

T *search (T *nodeelement);

TxOrderedList<T> *Split (const T *pNodeElement);

bool Push(T *nodeelement, T *nodedata);

bool SetSortOrder ();

bool InsertElementAt(T *pNodeData, QxInt index);

bool DeleteElementAt(QxInt index);

bool DeleteElementData(T *pNodeData);

T * element_at(QxInt index);

inline ErrorTxOrderedListType GetLastError ();

const QxChar *GetlastErrorStr ();

PRIVATE FUNCTION

bool SetFirst(OrderedListNode<T> *pListNode);

OrderedListNode<T> *GetFirst ();

void Resize(void);

OrderedListNode<T> *GetNodeFor (T *pNodeData);

bool UpdateLastNode (OrderedListNode<T> *pNode);

inline void SetError (ErrorTxOrderedListType errValue);

bool InSortOrder(T *pNodeData, OrderedListNode<T> *pTempNode);

bool OrderList();

bool SwapListItems(OrderedListNode<T> *pSourceNode, 
                    OrderedListNode<T> *pDestinationNode);




=================================================================*/
template <class T>
class TxOrderedList
{

public:

  /*==============================================================
  FUNCTION: TxOrderedList

  DESCRIPTION:
  Default constructor for the TxOrderedList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  TxOrderedList(SortOrderType orderType = SORT_ORDER_ASCENDING){
    pFirstNode = NULL;
    pLastNode = NULL;
    nSize = 0;
    sortingOrderType = orderType;
    SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
  }
  /*==============================================================
  FUNCTION: TxOrderedList

  DESCRIPTION:
  Default Destructor for the TxOrderedList class.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None.
  ==============================================================*/
  ~TxOrderedList(){
    OrderedListNode<T> *pTempNode = NULL;
    while (pLastNode != NULL){
      pTempNode = pLastNode;
      pLastNode = pLastNode->prev;
      delete(pTempNode);
    }
    pFirstNode = NULL;
    pLastNode = NULL;
    nSize = 0;
    return;
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
  T *pop_front(){
    T *pretNodeData = NULL;
    OrderedListNode<T> *pTempNode = NULL;
    if (pFirstNode != NULL){
      pTempNode = pFirstNode;
      pFirstNode = pFirstNode->next;
      if (pFirstNode != NULL){
        pFirstNode->prev = NULL;
      }
      pretNodeData = pTempNode->GetData();
      delete (pTempNode);
      nSize--;
      if (pFirstNode == NULL){
        UpdateLastNode (pFirstNode);
      }
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      return pretNodeData;
    }
    else{
      SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
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
   T *pop_back(){
   
      T *pRetData = NULL;
      OrderedListNode<T> *pTempListNode = NULL;

      pTempListNode = pLastNode;
      if (pTempListNode!= NULL){
         pLastNode = pLastNode->prev;
         if (pLastNode != NULL){
            pLastNode->next = NULL;
         } else {
            pFirstNode = NULL; // If last node is NULL means, list has only one node
         }
         UpdateLastNode (pLastNode);
         pRetData = pTempListNode->GetData();
         delete (pTempListNode);
         nSize--;
         SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
         return pRetData;
      }

      SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
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
  T *peek_front (){
    T *pRetData;
    if (pFirstNode != NULL){
      pRetData = pFirstNode->GetData();
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      return pRetData;
    }
    else{
      SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return NULL;
    }
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
  T *peek_back(){
    UpdateLastNode (pLastNode);
    if (pLastNode != NULL){
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      return pLastNode->GetData();
    }
    SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
    return NULL;
  }



  /*==============================================================
  FUNCTION: GetSize

  DESCRIPTION:
  Gets the number of nodes present in the TxOrderedList.

  INPUT/OUTPUT PARAMETERS:
  None 

  RETURN VALUE:
  Number of nodes present in the List as QxInt

  SIDE EFFECTS:
  None
  ==============================================================*/
  QxInt GetSize(){
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
  T *get_next (T *currentnode){
    OrderedListNode<T> *pListNode = NULL;
    if (currentnode == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return NULL;
    }
    pListNode = GetNodeFor (currentnode);
    if (pListNode != NULL){
      pListNode = pListNode->next;
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      if (pListNode != NULL){
        return pListNode->GetData();
      }
    }
    SetError (ERROR_TX_ORDERED_LIST_INVALID_NODE);
    return NULL;
  }


  /*==============================================================
  FUNCTION: GetPrev

  DESCRIPTION:
  Gets the previous node from the node specified.

  INPUT/OUTPUT PARAMETERS:
  pNodeElement - reference node whose previous node is required. 

  RETURN VALUE:
  Pointer to node that is placed before the passed node.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T *GetPrev (T *pNodeElement){
    OrderedListNode<T> *pTempNode = NULL;
    if (pNodeElement == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return NULL;
    }
    if (pNodeElement != NULL){
      pTempNode = GetNodeFor(pNodeElement);
      pTempNode = pTempNode->prev;    
      if (pTempNode != NULL){
        SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
        return pTempNode->GetData();
      }
      else{
        SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
        return NULL;
      }
    }
    else{
      SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return NULL;
    }
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
  bool Clear (){
    OrderedListNode<T> *pTempNode = NULL;
    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      pFirstNode = pTempNode->next;
      (pTempNode->GetData())->~T();
      delete (pTempNode);
      pTempNode = pFirstNode;
      nSize--;
    }
    pFirstNode = NULL;
    pLastNode = NULL;
    SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
    return TRUE;
  }



  /*==============================================================
  FUNCTION: RemoveDuplicates

  DESCRIPTION:
  Removes duplicate nodes present in the list. The nodes are compared with 
  operator ==. For user defined data types, it is advised to overload 
  == operator. 

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  TRUE if the nodes are freed else FALSE.

  SIDE EFFECTS:
  Memory allocated for userdefined object need to be freed apprpriately.
  ==============================================================*/
  bool RemoveDuplicates(){
    T *pFirstValue = NULL;
    T *pSecondValue = NULL;
    OrderedListNode<T> *pTempNode1 = NULL;
    OrderedListNode<T> *pTempNode2 = NULL;
    OrderedListNode<T> *pTempNode = NULL;
    OrderedListNode<T> *pTempPrevious = NULL;
    OrderedListNode<T> *pTempNext = NULL;

    bool duplicateFound = FALSE;

    if (pFirstNode == NULL){
      SetError(ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return FALSE;
    }
    if (pFirstNode->next == NULL){
      // Only one node exists in the list
      SetError(ERROR_TX_ORDERED_LIST_NO_DUPLICATE);
      return TRUE;
    }
    pTempNode1 = pFirstNode;
    while (pTempNode1 != NULL){
      pTempNode2 = pTempNode1->next;
      pFirstValue = pTempNode1->GetData();
      if (pFirstValue == NULL){ // Is this a possible case??
        continue;
      }
      while(pTempNode2 != NULL){
        pSecondValue = pTempNode2->GetData();
        if (pSecondValue == NULL){
          pTempNode2 = pTempNode2->next; // Is this a possible case??
          continue;
        }

        pTempNode = pTempNode2;
        pTempNode2 = pTempNode2->next; 
        if (*pFirstValue == *pSecondValue){
          pTempPrevious = pTempNode->prev;
          pTempNext = pTempNode->next;
          pTempPrevious->next = pTempNext;
          if (pTempNext != NULL){
            pTempNext->prev = pTempPrevious; // What happens if pTempNext is last node
          }
          nSize --;
          delete (pTempNode);
          duplicateFound = TRUE;
        }
      }
      pTempNode1 = pTempNode1->next;
    }
    if (duplicateFound){
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
    }
    else{
      SetError(ERROR_TX_ORDERED_LIST_NO_DUPLICATE);
    }
    UpdateLastNode (pFirstNode);  
    return TRUE;
  }


  /*==============================================================
  FUNCTION: Merge

  DESCRIPTION:
  The method appends the supplied TxList to the existing one.

  INPUT/OUTPUT PARAMETERS:
  pListToMerge - Pointer to TxList object.

  RETURN VALUE:
  True if merge is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool Merge (TxOrderedList<T> *pListToMerge){
    OrderedListNode<T> *pNewHeadNode = NULL;
    if (pListToMerge == NULL){
      SetError(ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return FALSE;
    }
    pNewHeadNode = pListToMerge->GetFirst();
    if (pNewHeadNode == NULL){
      SetError(ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return FALSE;
    }
    if (pFirstNode == NULL){
      pFirstNode = pNewHeadNode;
      nSize = pListToMerge->GetSize();
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      return TRUE;
    }
    UpdateLastNode (pLastNode);
    if (pLastNode != NULL){
      pLastNode->next = pNewHeadNode;
      pNewHeadNode->prev = pLastNode;
    }
    nSize += pListToMerge->GetSize();
    UpdateLastNode (pLastNode);

    pListToMerge->SetFirst(NULL);
    pListToMerge->UpdateLastNode(NULL);

    SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
    return TRUE;
  }


  /*==============================================================
  FUNCTION: Search

  DESCRIPTION:
  searches for the node.

  INPUT/OUTPUT PARAMETERS:
  pNodeElement - node element to search for.

  RETURN VALUE:
  Pointer to node element. NULL on failure.

  SIDE EFFECTS:
  None
  ==============================================================*/
  T * Search (T *pNodeElement){
    OrderedListNode<T> *pListNode = NULL;
    pListNode = GetNodeFor(pNodeElement);
    if (pListNode != NULL){
      SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
      return pListNode->GetData();
    }
    SetError(ERROR_TX_ORDERED_LIST_INVALID_NODE);
    return NULL;
  }


  /*==============================================================
  FUNCTION: Split

  DESCRIPTION:
  Splits the list into two starting from a specified node element.

  INPUT/OUTPUT PARAMETERS:
  pNodeElement - The node element from where the Split is to take place.

  RETURN VALUE:
  True if Split is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  TxOrderedList<T> *Split (const T *pNodeElement)
  {
    OrderedListNode<T> *pTemp = NULL; 
    OrderedListNode<T> *pTempNode = NULL; 
    TxOrderedList<T> *pNewListAfterSplit = NULL;

    if (pNodeElement == NULL){
      SetError(ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return NULL;
    }
    pNewListAfterSplit = new TxOrderedList<T>();
    if (pFirstNode == NULL){
      SetError(ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return NULL;
    }

    if (pNewListAfterSplit == NULL){
      SetError(ERROR_TX_ORDERED_LIST_INSUFFICIENT_MEMORY);
      return NULL;
    }
    pTempNode = pFirstNode;
    while (!(*(pTempNode->GetData()) == *pNodeElement)){
      pTemp = pTempNode;
      pTempNode = pTempNode->next;
      if (pTempNode == NULL){
        SetError(ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
        return NULL;
      }
    }
    if (pTempNode == pFirstNode){
      pFirstNode = NULL;
    }
    pTemp->next = NULL;
    pLastNode = pTemp;
    if (pLastNode != NULL){
      pLastNode->next = NULL;
    }
    pNewListAfterSplit->UpdateLastNode(pTemp);
    pNewListAfterSplit->SetFirst(pTempNode);
    Resize();
    pNewListAfterSplit->Resize();
    return pNewListAfterSplit;
  }


  /*==============================================================
  FUNCTION: Push

  DESCRIPTION:
  Inserts an element in between the list.

  INPUT/OUTPUT PARAMETERS:
  pNodeData - The data to be inserted.

  RETURN VALUE:
  True if Push is successful FALSE otherwise.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool Push(T *pNodeData){
    OrderedListNode<T> *pTempNode = NULL; 
    OrderedListNode<T> *pNode = NULL;
    OrderedListNode<T> *pTempPreviousNode = NULL;
    bool bOrderFound = FALSE;

    if (pNodeData == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return FALSE;
    }
    pNode = new OrderedListNode<T>(pNodeData);
    if (pNode == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INSUFFICIENT_MEMORY);
      return FALSE;
    }
    if (pFirstNode == NULL){
      pFirstNode = pNode;
      pFirstNode->next = NULL;
      pFirstNode->prev = NULL;
      pLastNode = pNode;
      nSize++;
      UpdateLastNode (pLastNode);
      return TRUE;
    }

    pTempNode = pFirstNode;
    while (pTempNode != NULL){
      if (InSortOrder(pNodeData, pTempNode)){
        if (pTempNode != NULL){
          pTempPreviousNode = pTempNode->prev;
          pTempNode->prev = pNode;
          pNode->next = pTempNode;
          pNode->prev = pTempPreviousNode;
          if (pTempPreviousNode == NULL){
            pFirstNode = pNode;
            pFirstNode->prev = NULL;
          }
          else{
            pTempPreviousNode->next = pNode;
          }
          bOrderFound = TRUE;
        }
        break;
      }
      pTempNode = pTempNode->next;
    }
    if (bOrderFound == FALSE){
      pLastNode->next = pNode;
      pNode->prev = pLastNode;
    } 
    nSize ++;
    UpdateLastNode (pLastNode);
    return TRUE;
  }


  /*==============================================================
  FUNCTION: SetSortOrder

  DESCRIPTION:
  This function sets the order of sort as ascending or descending.

  INPUT/OUTPUT PARAMETERS:
  1 for Ascending and 2 for descending.

  RETURN VALUE:
  True if SetSortOrder is succesful otherwise FALSE.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool SetSortOrder (SortOrderType orderType){
    sortingOrderType = orderType;
    if (!OrderList()){
      return FALSE;
    }
    return TRUE;
  }

  /*==============================================================
  FUNCTION: InsertElementAt

  DESCRIPTION:
  Insert the node at respective index.

  INPUT/OUTPUT PARAMETERS:
  pNodeData - reference node element to inserting.
  index - respective index

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool InsertElementAt(T *pNodeData, QxInt index){
    OrderedListNode<T> *pTempNode = NULL;
    OrderedListNode<T> *pTempPrev = NULL;
    OrderedListNode<T> *pNode = NULL;

    // QxInt NodeCount = 0;
    if((NULL == pNodeData) || (index < 0) || (index > nSize) ){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
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
    
    pNode = new OrderedListNode<T>(pNodeData);
    if (pNode == NULL){    
      SetError(ERROR_TX_ORDERED_LIST_INSUFFICIENT_MEMORY);
      return FALSE;
    }
    if (pTempPrev == NULL){
      pNode->next = pFirstNode;
      pFirstNode->prev = pNode;
      pNode->prev = NULL;
      pFirstNode = pNode;      
    }
    else{      
      pTempPrev->next = pNode;      
      pNode->next = pTempNode;
      pNode->prev = pTempPrev;
      if (pTempNode != NULL){
        pTempNode->prev = pNode;
      }
      UpdateLastNode(pLastNode);
    }
    nSize++;
    return TRUE;
  }

  /*==============================================================
  FUNCTION: DeleteElementAt

  DESCRIPTION:
  Delete the node at respective index.

  INPUT/OUTPUT PARAMETERS:
  index - respective index.

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  Index should start from 0
  ==============================================================*/
  bool DeleteElementAt(QxInt index){
    OrderedListNode<T> *pTempNode = NULL;
    OrderedListNode<T> *pTempPrev = NULL;

    if((index < 0) || (index >= nSize)){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return FALSE;
    }
    pTempNode = pFirstNode;
    pTempPrev = NULL;  

	int i;
    for (i = 0; i < index; i++){
      pTempPrev = pTempNode;
      pTempNode = pTempNode->next;      
    }    
    
    if (pTempPrev == NULL){
      pFirstNode = pTempNode->next;
      if (pLastNode == pTempNode){
        UpdateLastNode (pFirstNode);
      }
      else{
        pFirstNode->prev = NULL;
      }
      nSize--;
      delete pTempNode;
    }
    else{  
      if (pTempNode != NULL){
        pTempPrev->next = pTempNode->next; 
        if( pTempNode->next != NULL){    
          pTempNode->next->prev = pTempPrev;  
        }   
        if (pLastNode == pTempNode){
          UpdateLastNode (pTempPrev);
        }
        nSize--;
        delete pTempNode;
      } 
    }
    return TRUE;
  }

  /*==============================================================
  FUNCTION: DeleteElementData

  DESCRIPTION:
  Delete NodeData

  INPUT/OUTPUT PARAMETERS:
  pNodeData - reference node element to delete node Data.

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool DeleteElementData(T *pNodeData){
    OrderedListNode<T> *pTempNode = NULL;
    OrderedListNode<T> *pTempPrev = NULL;

    if(pNodeData == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return FALSE;
    } 
    if (pFirstNode == NULL){
      SetError (ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return FALSE;
    }
    pTempNode = pFirstNode;
    pTempPrev = NULL;

    while(pTempNode != NULL)
    {
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
      else{
        pFirstNode->prev = NULL;
      }
      nSize--;
      delete pTempNode;

    }
    else {
      if (pTempNode != NULL){
        pTempPrev->next = pTempNode->next; 
        
        if( pTempNode->next != NULL){    
          pTempNode->next->prev = pTempPrev;  
        }   
        if (pLastNode == pTempNode){
          UpdateLastNode (pTempPrev);
        }   
        nSize--;
        delete pTempNode;
      }else{
        SetError (ERROR_TX_ORDERED_LIST_OUT_OF_RANGE);
        return FALSE;
      }
    }  
    return TRUE;
  }

  /*==============================================================
  FUNCTION: element_at

  DESCRIPTION:
  Get the Data at respective index.

  INPUT/OUTPUT PARAMETERS:
  index - respective index.

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  Index value should be start from 0
  ==============================================================*/
  T * element_at(QxInt index){
    OrderedListNode<T> *pTempNode = NULL; 

    if((index < 0) || (index >= nSize)){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);      
      return NULL;
    }
    pTempNode = pFirstNode;

    for (int i = 0; i < index; i++){    
      pTempNode = pTempNode->next;
      if (pTempNode == NULL){         
          SetError (ERROR_TX_ORDERED_LIST_INVALID_NODE);
          return NULL;
        }      
    }
    if (pTempNode != NULL){
      return pTempNode->GetData();      
    }
    else{
      SetError (ERROR_TX_ORDERED_LIST_OUT_OF_RANGE);
      return NULL;
    }
  }
                                 

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
  inline ErrorTxOrderedListType GetLastError (){
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
  const QxChar *GetLastErrorStr (){
    switch (error){
      case ERROR_TX_ORDERED_LIST_NO_ERROR:
        return "No Error";
        break;
      case ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS:
        return "Error validating input parameters";
        break;
      case ERROR_TX_ORDERED_LIST_INSUFFICIENT_MEMORY:
        return "Error Insufficient memory";
        break;
      case ERROR_TX_ORDERED_LIST_INVALID_NODE:
        return "Error invalid node";
        break;
      case ERROR_TX_ORDERED_LIST_NODE_NOT_FOUND:
        return "Error node not found";
        break;
      case ERROR_TX_ORDERED_LIST_OUT_OF_RANGE:
        return "Error out of range node";
        break;
      case ERROR_TX_ORDERED_LIST_EMPTY_LIST:
        return "Error empty list";
        break;
      case ERROR_TX_ORDERED_LIST_NO_DUPLICATE:
        return "Error No Duplicate";
        break;
      default:
        return "Error Unknown";
        break;
    }
    return "Error Unknown";
  }


private:

   /*==============================================================
  FUNCTION: SetFirst

  DESCRIPTION:
  The method Sets the header node for the list.

  INPUT/OUTPUT PARAMETERS:
  pListNode - List node to be updated as the first node.

  RETURN VALUE:
  True if head node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool SetFirst(OrderedListNode<T> *pListNode){
    pFirstNode = pListNode;
    if (pFirstNode != NULL){
      pFirstNode->prev = NULL;
    }
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
  OrderedListNode<T> *GetFirst (){
    return pFirstNode;
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
  void Resize(void){
    OrderedListNode<T> *pTempNode = NULL;

    nSize = 0;
    pTempNode = pFirstNode;

    while (pTempNode != NULL){
      pTempNode = pTempNode->next;
      nSize ++;
    }
    return;	
  }

  /*==============================================================
  FUNCTION: GetNodeFor

  DESCRIPTION:
  Returns the OrderedListNode holding the node data from TxOrderedList.

  INPUT/OUTPUT PARAMETERS:
  pNodeData - data object to search for

  RETURN VALUE:
  OrderedListNode that holds the node data specified.

  SIDE EFFECTS:
  If two nodes contains the same node data, only the first occuring 
  OrderedListNode is returned.
  ==============================================================*/
  OrderedListNode<T> *GetNodeFor (T *pNodeData){
    OrderedListNode<T> *pTempNode = NULL;
    if (pNodeData == NULL){
      SetError (ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
      return NULL;
    }
    if (pFirstNode != NULL){
      pTempNode = pFirstNode;
      while (pTempNode != NULL){
        if (*pNodeData == *(pTempNode->GetData())){
          SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
          return pTempNode;
        }
        pTempNode = pTempNode->next;
      }
      if (pTempNode == NULL){
        SetError(ERROR_TX_ORDERED_LIST_NODE_NOT_FOUND);
      }
    }
    else{
      SetError(ERROR_TX_ORDERED_LIST_EMPTY_LIST);
    }
    return NULL;
  }

  /*==============================================================
  FUNCTION: UpdateLastNode

  DESCRIPTION:
  Updates the last node of the list.

  INPUT/OUTPUT PARAMETERS:
  pNode - reference node element to start upating the last node.

  RETURN VALUE:
  TRUE if the last node is updated succesfully.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool UpdateLastNode (OrderedListNode<T> *pNode){
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
  FUNCTION: SetError

  DESCRIPTION:
  Sets the error value.

  INPUT/OUTPUT PARAMETERS:
  errValue - error value to be set.

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
  ==============================================================*/
  inline void SetError (ErrorTxOrderedListType errValue){
    error = errValue;
  }

  /*==============================================================
  FUNCTION: InSortOrder

  DESCRIPTION:
  Checks if the data and the list node are as per the sort order.

  INPUT/OUTPUT PARAMETERS:
  pNodeData - pointer to list node data element.
  pTempNode - pointer to node element to compare with.

  RETURN VALUE:
  TRUE if nodedata and data in node are as per sort order.

  SIDE EFFECTS:
  None
  ==============================================================*/
  bool InSortOrder(T *pNodeData, OrderedListNode<T> *pTempNode){
    if (pTempNode == NULL){
      return FALSE;
    }
    switch (sortingOrderType){
      case SORT_ORDER_NO_ORDER:
        return FALSE;
        break;
      case SORT_ORDER_DESCENDING:
        return (*pTempNode->GetData() < *pNodeData);
        break;
      case SORT_ORDER_ASCENDING:
        return (*pTempNode->GetData() > *pNodeData);
        break;
      default:
      return FALSE;
    }
  }


  /*==============================================================
  FUNCTION: OrderList

  DESCRIPTION:
  List elements are rearanged as per the sorting order.

  INPUT/OUTPUT PARAMETERS:
  None

  RETURN VALUE:
  TRUE if list is orderd succesfully.

  SIDE EFFECTS:
  If an error occurs in between the ordering, the original order is not 
  restored. 
  ==============================================================*/
  bool OrderList(){
    OrderedListNode<T> *pTempNode1 = NULL;
    OrderedListNode<T> *pTempNode2 = NULL;
    OrderedListNode<T> *pTempNode = NULL;
    OrderedListNode<T> *pTempNext = NULL;

    if (pFirstNode == NULL){
      SetError(ERROR_TX_ORDERED_LIST_EMPTY_LIST);
      return FALSE;
    }
    pTempNode1 = pFirstNode;
    while (pTempNode1->next != NULL){
      pTempNode2 = pTempNode1->next;
      while(pTempNode2 != NULL){
        pTempNode = pTempNode2;
        if (!InSortOrder(pTempNode1->GetData(), pTempNode)){
          SwapListItems (pTempNode1, pTempNode);
        }
        pTempNode2 = pTempNode2->next;
      }
      pTempNode1 = pTempNode1->next;
    }
    SetError(ERROR_TX_ORDERED_LIST_NO_ERROR);
    return TRUE;
  }


  /*==============================================================
  FUNCTION: SwapListItems

  DESCRIPTION:
  Swaps the node data  for the specified nodes.

  INPUT/OUTPUT PARAMETERS:
  pSourceNode - source node
  pDestinationNode - destination node

  RETURN VALUE:
  TRUE if operation is succesfully.

  SIDE EFFECTS:
  None. 
  ==============================================================*/
  bool SwapListItems(OrderedListNode<T> *pSourceNode,
    OrderedListNode<T> *pDestinationNode){
      T *tempdata = NULL;
      if ((pSourceNode == NULL) || (pDestinationNode == NULL)){
        //Set error Invalid parameters
        SetError(ERROR_TX_ORDERED_LIST_INVALID_PARAMETERS);
        return FALSE;
      }
      tempdata = pSourceNode->GetData();
      pSourceNode->SetData(pDestinationNode->GetData());
      pDestinationNode->SetData(tempdata);
      return TRUE;
    }



private:

  OrderedListNode<T> *pFirstNode;     // first node of list
  OrderedListNode<T> *pLastNode;      // last node of list
  QxInt nSize;                        // list size
  SortOrderType sortingOrderType;     // sorting order

  ErrorTxOrderedListType error;       // error

};    //TxOrderedList class

#endif
