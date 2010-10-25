#ifndef AEEIQOSSPEC_H
#define AEEIQOSSPEC_H
/*======================================================
FILE:  AEEIQoSSpec.h

SERVICES:
   BREW QoS Spec Interfaces

GENERAL DESCRIPTION:
	Interface definitions and data structures
   for BREW QoSSpec Interface

   Copyright (c) 2006-2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEIQoSList.h"
#include "AEEIQI.h"
#include "AEEStdDef.h"

#define AEEIID_IQoSSpec    0x0102b6c6

/*
   Quality Of Service Error
*/

#define AEEQOSERR_CODE_SUCCESS   0 /* no error */
#define AEEQOSERR_CODE_UNSPEC    1 /* unspecified error */
#define AEEQOSERR_CODE_MISSING   2
#define AEEQOSERR_CODE_EMPTY     3
#define AEEQOSERR_CODE_INVAL     4
#define AEEQOSERR_CODE_CONFLICT  5 /* conflicting with another option */
#define AEEQOSERR_CODE_TOO_MANY_AUX_FLOWS 6

typedef int32 AEEQoSErrorCode;


#define AEEQOSERR_FIELD_UNSPEC            0
#define AEEQOSERR_FIELD_RX_FLOW_SPEC      1
#define AEEQOSERR_FIELD_TX_FLOW_SPEC      2
#define AEEQOSERR_FIELD_RX_FILTER_SPEC    3
#define AEEQOSERR_FIELD_TX_FILTER_SPEC    4

typedef int32 AEEQoSErrorField;

#define AEEQOSERR_OPT_UNSPEC    (0)

#define AEEQOSERR_INDEX_UNSPEC (-1)

typedef struct AEEQoSError
{
   AEEQoSErrorCode eErrorCode;
   AEEQoSErrorField eField;
   int32 nIndex;
   int32 nOption;
   int32 ext;
} AEEQoSError;

/*
   QoS Modification Mask
*/

#define AEE_QOS_MODIFY_MASK_NONE           0x00
#define AEE_QOS_MODIFY_MASK_RX_FLOW        0x01
#define AEE_QOS_MODIFY_MASK_RX_FILTER      0x02
#define AEE_QOS_MODIFY_MASK_RX_SPEC_ADD    0x04
#define AEE_QOS_MODIFY_MASK_RX_SPEC_DELETE 0x08
#define AEE_QOS_MODIFY_MASK_RX_ALL         0x0F
#define AEE_QOS_MODIFY_MASK_TX_FLOW        0x10
#define AEE_QOS_MODIFY_MASK_TX_FILTER      0x20
#define AEE_QOS_MODIFY_MASK_TX_SPEC_ADD    0x40
#define AEE_QOS_MODIFY_MASK_TX_SPEC_DELETE 0x80
#define AEE_QOS_MODIFY_MASK_TX_ALL         0xF0
#define AEE_QOS_MODIFY_MASK_ALL            0xFF

typedef uint16 AEEQoSModifyMask;

/*
   Quality Of Service Spec Interface
*/

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IQoSSpec : public IQI
{
   virtual int CDECL SetRxFlowSpec (IQoSList* pIQoSFlowList) = 0;
   virtual int CDECL SetTxFlowSpec (IQoSList* pIQoSFlowList) = 0;
   virtual int CDECL SetRxFilterSpec (IQoSList* pIQoSFilterList) = 0;
   virtual int CDECL SetTxFilterSpec (IQoSList* pIQoSFilterList) = 0;
   virtual int CDECL GetRxFlowSpec (IQoSList** ppIQoSFlowList) = 0;
   virtual int CDECL GetTxFlowSpec (IQoSList** ppIQoSFlowList) = 0;
   virtual int CDECL GetRxFilterSpec (IQoSList** ppIQoSFilterList) = 0;
   virtual int CDECL GetTxFilterSpec (IQoSList** ppIQoSFilterList) = 0;
   virtual int CDECL ExtFunc (int nOption, uint32 dwVal) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Define IQoSSpec interface methods
#define INHERIT_IQoSSpec(iname) \
   INHERIT_IQI(iname); \
   /* set accessors */ \
   int   (*SetRxFlowSpec)(iname* po, IQoSList* pIQoSFlowList); \
   int   (*SetTxFlowSpec)(iname* po, IQoSList* pIQoSFlowList); \
   int   (*SetRxFilterSpec)(iname* po, IQoSList* pIQoSFilterList); \
   int   (*SetTxFilterSpec)(iname* po, IQoSList* pIQoSFilterList); \
   /* get accessors */ \
   int   (*GetRxFlowSpec)(iname* po, IQoSList** ppIQoSFlowList); \
   int   (*GetTxFlowSpec)(iname* po, IQoSList** ppIQoSFlowList); \
   int   (*GetRxFilterSpec)(iname* po, IQoSList** ppIQoSFilterList); \
   int   (*GetTxFilterSpec)(iname* po, IQoSList** ppIQoSFilterList); \
   /* Extension Function to allow future enhancement to the interface without
   breaking its binary compatibility. */ \
   int   (*ExtFunc)(iname* po, int nOption, uint32 dwVal)

   // declare the actual interface
   AEEINTERFACE_DEFINE(IQoSSpec);


static __inline uint32 IQoSSpec_AddRef(IQoSSpec *me)
{
   return AEEGETPVTBL(me,IQoSSpec)->AddRef(me);
}

static __inline uint32 IQoSSpec_Release(IQoSSpec *me)
{
   return AEEGETPVTBL(me,IQoSSpec)->Release(me);
}

static __inline int IQoSSpec_QueryInterface(IQoSSpec *me, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(me,IQoSSpec)->QueryInterface(me, cls, ppo);
}

static __inline int IQoSSpec_SetRxFlowSpec(IQoSSpec *me, IQoSList* pQoSFlowList)
{
   return AEEGETPVTBL(me,IQoSSpec)->SetRxFlowSpec(me, pQoSFlowList);
}

static __inline int IQoSSpec_SetTxFlowSpec(IQoSSpec *me, IQoSList* pQoSFlowList)
{
   return AEEGETPVTBL(me,IQoSSpec)->SetTxFlowSpec(me, pQoSFlowList);
}

static __inline int IQoSSpec_SetRxFilterSpec(IQoSSpec *me, IQoSList* pIQoSFilterList)
{
   return AEEGETPVTBL(me,IQoSSpec)->SetRxFilterSpec(me, pIQoSFilterList);
}

static __inline int IQoSSpec_SetTxFilterSpec(IQoSSpec *me, IQoSList* pIQoSFilterList)
{
   return AEEGETPVTBL(me,IQoSSpec)->SetTxFilterSpec(me, pIQoSFilterList);
}

static __inline int IQoSSpec_GetRxFlowSpec(IQoSSpec *me, IQoSList** ppIQoSFlowList)
{
   return AEEGETPVTBL(me,IQoSSpec)->GetRxFlowSpec(me, ppIQoSFlowList);
}

static __inline int IQoSSpec_GetTxFlowSpec(IQoSSpec *me, IQoSList** ppIQoSFlowList)
{
   return AEEGETPVTBL(me,IQoSSpec)->GetTxFlowSpec(me, ppIQoSFlowList);
}

static __inline int IQoSSpec_GetRxFilterSpec(IQoSSpec *me, IQoSList** ppIQoSFilterList)
{
   return AEEGETPVTBL(me,IQoSSpec)->GetRxFilterSpec(me, ppIQoSFilterList);
}

static __inline int IQoSSpec_GetTxFilterSpec(IQoSSpec *me, IQoSList** ppIQoSFilterList)
{
   return AEEGETPVTBL(me,IQoSSpec)->GetTxFilterSpec(me, ppIQoSFilterList);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEQoSError

Description:
   AEEQoSError can be used by the client to identify an erroneous QoS parameter,
   in case such a parameter caused a QoS request to fail.

   When a QoS request (either IQoSSession_Open() or IQoSSession_ModifyEx()) fails
   due to an erroneous parameter in the QoS Spec, a special error value
   is returned (AEE_NET_EQOS_SPEC_INVALID_OPT), and an AEEQoSError instance
   is built internally.

   The client can now call IQoSSession_GetError() to get the AEEQoSError,
   and find out which parameter in the request QoSSpec was erroneous.
   AEEQoSError has an error code, and also pin points the error. For example,
   it may specify that option ID 123 inside filter #0 of the Rx flow spec
   contains an invalid value.

   If an AEEQoSError member is not applicable, it is marked as “unspecified”.
   For example, if Rx Flow #2 is empty, then option ID is unspecified.

Definition:
   typedef struct AEEQoSError
   {
      AEEQoSErrorCode eErrorCode;
      AEEQoSErrorField eField;
      int32 nIndex;
      int32 nOption;
      int32 ext;
   }
   AEEQoSError;

Members:
   eErrorCode: type of error
   eField:     ID of erroneous component (Rx/Tx Flow/Filter spec),
               or unspecified (AEEQOSERR_FIELD_UNSPEC)
   nIndex:     list index of erroneous item (for example, Rx filter #3),
               or unspecified (AEEQOSERR_INDEX_UNSPEC)
   nOption:    ID of erroneous option, or unspecified (AEEQOSERR_OPT_UNSPEC)
   ext:        extended information - reserved for future use.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   IQoSSession_GetError()

=======================================================================
AEEQoSErrorCode

Description:
   Type of error in an AEEQoSError struct.

Definition:
   #define AEEQOSERR_CODE_SUCCESS   0
   #define AEEQOSERR_CODE_UNSPEC    1
   #define AEEQOSERR_CODE_MISSING   2
   #define AEEQOSERR_CODE_EMPTY     3
   #define AEEQOSERR_CODE_INVAL     4
   #define AEEQOSERR_CODE_CONFLICT  5
   #define AEEQOSERR_CODE_TOO_MANY_AUX_FLOWS 6

   typedef int32 AEEQoSErrorCode;


Members:
   AEEQOSERR_CODE_SUCCESS: No error
   AEEQOSERR_CODE_UNSPEC:  Unspecified error
   AEEQOSERR_CODE_MISSING: Mandatory parameter is missing
   AEEQOSERR_CODE_EMPTY:   Data structure is empty, while it must not be empty
   AEEQOSERR_CODE_INVAL:   Invalid value
   AEEQOSERR_CODE_CONFLICT: Parameter conflicts with another parameter
   AEEQOSERR_CODE_TOO_MANY_AUX_FLOWS: Too many auxiliary flows are specified
                                      (more than MAX_UINT8)

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEQoSError

=======================================================================
AEEQoSErrorField

Description:
   Identification of erroneous component (Rx/Tx Flow/Filter spec)
   in an AEEQoSError struct.

Definition:
   #define AEEQOSERR_FIELD_UNSPEC            0
   #define AEEQOSERR_FIELD_RX_FLOW_SPEC      1
   #define AEEQOSERR_FIELD_TX_FLOW_SPEC      2
   #define AEEQOSERR_FIELD_RX_FILTER_SPEC    3
   #define AEEQOSERR_FIELD_TX_FILTER_SPEC    4

   typedef int32 AEEQoSErrorField;

Members:
   AEEQOSERR_FIELD_UNSPEC:          Unspecified - error is not related to
                                    any specific component of the QoS spec.
   AEEQOSERR_FIELD_RX_FLOW_SPEC:    Error is in Rx Flow Spec.
   AEEQOSERR_FIELD_TX_FLOW_SPEC:    Error is in Tx Flow Spec.
   AEEQOSERR_FIELD_RX_FILTER_SPEC:  Error is in Rx Filter Spec.
   AEEQOSERR_FIELD_TX_FILTER_SPEC:  Error is in Tx Filter Spec.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEQoSError

======================================================================

AEEQoSModifyMask

Description:
   This data type is a bitmask which is used as a parameter to
   IQoSSession_ModifyEx() in order to specify the combination of
   QoS Specification blocks to be modified.

Definition:
   #define AEE_QOS_MODIFY_MASK_NONE           0x00
   #define AEE_QOS_MODIFY_MASK_RX_FLOW        0x01
   #define AEE_QOS_MODIFY_MASK_RX_FILTER      0x02
   #define AEE_QOS_MODIFY_MASK_RX_SPEC_ADD    0x04
   #define AEE_QOS_MODIFY_MASK_RX_SPEC_DELETE 0x08
   #define AEE_QOS_MODIFY_MASK_RX_ALL         0x0F
   #define AEE_QOS_MODIFY_MASK_TX_FLOW        0x10
   #define AEE_QOS_MODIFY_MASK_TX_FILTER      0x20
   #define AEE_QOS_MODIFY_MASK_TX_SPEC_ADD    0x40
   #define AEE_QOS_MODIFY_MASK_TX_SPEC_DELETE 0x80
   #define AEE_QOS_MODIFY_MASK_TX_ALL         0xF0
   #define AEE_QOS_MODIFY_MASK_ALL            0xFF

   typedef uint16 AEEQoSModifyMask;

Members:
   AEE_QOS_MODIFY_MASK_NONE:           None
   AEE_QOS_MODIFY_MASK_RX_FLOW:        Modify Rx Flow block
   AEE_QOS_MODIFY_MASK_RX_FILTER:      Modify Rx Filter block
   AEE_QOS_MODIFY_MASK_RX_SPEC_ADD:    Add QoS Specification in Rx direction
   AEE_QOS_MODIFY_MASK_RX_SPEC_DELETE: Delete QoS Specification in Rx direction
   AEE_QOS_MODIFY_MASK_RX_ALL:         All Rx modification options
   AEE_QOS_MODIFY_MASK_TX_FLOW:        Modify Tx Flow block
   AEE_QOS_MODIFY_MASK_TX_FILTER:      Modify Tx Filter block
   AEE_QOS_MODIFY_MASK_TX_SPEC_ADD     Add QoS Specification in Tx direction
   AEE_QOS_MODIFY_MASK_TX_SPEC_DELETE: Delete QoS Specification in Tx direction
   AEE_QOS_MODIFY_MASK_RX_ALL:         All Tx modification options
   AEE_QOS_MODIFY_MASK_ALL:            All modification options

Comments:
   None.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   IQoSSession_ModifyEx()

=======================================================================*/

/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQoSSpec

This interface represents a complete QoS specification. It is used for specifying
the flow spec and filter spec for each direction on which QOS is requested.

QoS specification is composed of four lists of specs:
- List of Rx Flow spec
- List of Tx Flow spec
- List of Rx Filter spec
- List of Tx Filter spec

The two lists of flow specs contains IQoSFlow objects and specify the treatment
required for an IP flow for the Rx direction and for the Tx direction
respectively.
The order of IQoSFlow objects in these lists is significant. The first object
is the requested / most suitable flow for the client while the last object is
the minimum required flow by the client. All other objects are auxiliary
flows in decreasing order of desirability in between the requested flow and
the minimum required flow.
If only one object exists in the list,it is handled as the requested flow.
In any case where more than a single object exists in the list, in order to
specify that no minimum flow is required, the last object of the list must
be set to an empty flow. (an empty flow is a flow with no QoS options.
see AEEQoSFlow.h).
Rx/Tx Flow lists cannot be empty.

The two lists of filter specs contains IQoSFilter objects and specify the IP
flow itself for the Rx direction and for the Tx direction.
The order of IQoSFilter objects in these lists is not significant.
Rx/Tx Filter lists cannot be empty.

IQoSSpec provides Set and Get accessors to each one of the above four lists.

IQoSSpec object is created empty.
When setting any of the spec lists (through the Set methods), IQoSSpec doesn't
copy the given list but rather stores a reference to it.
The given IQoSList object replaces the stored one and IQoSSpec takes
responsibility of it. This means that the stored IQoSList is Release()'d and
the given IQoSList is AddRef()'d.

The following header file is required:~
   BREW 4.0 - AEEIQoSSpec.h
   BREW 3.1 - AEEQoSSpec.h

=============================================================================

IQoSSpec_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IQoSSpec_Release()

=============================================================================

IQoSSpec_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IQoSSpec_AddRef()

==============================================================================

IQoSSpec_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

=======================================================================

IQoSSpec_SetRxFlowSpec()

Description:
   This function sets the list of Rx Flow specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the given IQoSList object but rather stores a
   reference to it.
   The given IQoSList object replaces the stored one, if one actually exists.
   The stored IQoSList is Release()'d and the given IQoSList is AddRef()'d.
   If no IQoSList object is given (pIQoSFlowList==NULL), the stored list is
   removed.

   No validation is done on the content of the given IQoSList object.
   Validation is only done later when the object is used in operations such
   as IQoSSession_Open() and IQoSSession_ModifyEx().

Prototype:
   int IQoSSpec_SetRxFlowSpec(IQoSSpec* po, IQoSList* pIQoSFlowList)

Parameters:
   po: the interface pointer
   pIQoSFlowList: list of IQoSFlow objects to be used as the list of Rx Flow
                  specs. May be NULL.
                  The order of IQoSFlow objects in the list is significant. The
                  first object is the most suitable flow for the client
                  while the last object is the minimum required flow
                  by the client. If only one object exists in the list,
                  it is considered to be the requested flow (no minimum required flow).

Return Value:
   AEE_SUCCESS: if successfully set

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_GetRxFlowSpec()
   IQoSSession_Open()
   IQoSSession_ModifyEx()

=======================================================================

IQoSSpec_SetTxFlowSpec()

Description:
   This function sets the list of Tx Flow specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the given IQoSList object but rather stores a
   reference to it.
   The given IQoSList object replaces the stored one, if one actually exists.
   The stored IQoSList is Release()'d and the given IQoSList is AddRef()'d.
   If no IQoSList object is given (pIQoSFlowList==NULL), the stored list is
   removed.

   No validation is done on the content of the given IQoSList object.
   Validation is only done later when the object is used in operations such
   as IQoSSession_Open() and IQoSSession_ModifyEx().

Prototype:
   int IQoSSpec_SetTxFlowSpec(IQoSSpec* po, IQoSList* pIQoSFlowList)

Parameters:
   po: the interface pointer
   pIQoSFlowList: list of IQoSFlow objects to be used as the list of Tx Flow
                  specs. May be NULL.
                  The order of IQoSFlow objects in the list is significant. The
                  first object is the most suitable flow for the client
                  while the last object is the minimum required flow
                  by the client. If only one object exists in the list,
                  it is considered to be the requested flow (no minimum required flow).

Return Value:
   AEE_SUCCESS: if successfully set

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_GetTxFlowSpec()
   IQoSSession_Open()
   IQoSSession_ModifyEx()

=======================================================================

IQoSSpec_SetRxFilterSpec()

Description:
   This function sets the list of Rx Filter specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the given IQoSList object but rather stores a
   reference to it.
   The given IQoSList object replaces the stored one, if one actually exists.
   The stored IQoSList is Release()'d and the given IQoSList is AddRef()'d.
   If no IQoSList object is given (pIQoSFilterList==NULL), the stored list is
   removed.

   No validation is done on the content of the given IQoSList object.
   Validation is only done later when the object is used in operations such
   as IQoSSession_Open() and IQoSSession_ModifyEx().

Prototype:
   int IQoSSpec_SetRxFilterSpec(IQoSSpec* po, IQoSList* pIQoSFilterList)

Parameters:
   po: the interface pointer
   pIQoSFilterList: list of IQoSFilter objects to be used as the list of
                    Rx Filter specs. May be NULL.
                    The order of IQoSFilter objects in the list is
                    insignificant.

Return Value:
   AEE_SUCCESS: if successfully set

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_GetRxFilterSpec()
   IQoSSession_Open()
   IQoSSession_ModifyEx()

=======================================================================

IQoSSpec_SetTxFilterSpec()

Description:
   This function sets the list of Tx Filter specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the given IQoSList object but rather stores a
   reference to it.
   The given IQoSList object replaces the stored one, if one actually exists.
   The stored IQoSList is Release()'d and the given IQoSList is AddRef()'d.
   If no IQoSList object is given (pIQoSFilterList==NULL), the stored list is
   removed.

   No validation is done on the content of the given IQoSList object.
   Validation is only done later when the object is used in operations such
   as IQoSSession_Open() and IQoSSession_ModifyEx().

Prototype:
   int IQoSSpec_SetTxFilterSpec(IQoSSpec* po, IQoSList* pIQoSFilterList)

Parameters:
   po: the interface pointer
   pIQoSFilterList: list of IQoSFilter objects to be used as the list of
                    Tx Filter specs. May be NULL.
                    The order of IQoSFilter objects in the list is
                    insignificant.

Return Value:
   AEE_SUCCESS: if successfully set

Comments:
   None.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_GetTxFilterSpec()
   IQoSSession_Open()
   IQoSSession_ModifyEx()

=======================================================================

IQoSSpec_GetRxFlowSpec()

Description:
   This function gets the list of Rx Flow specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the stored IQoSList object but rather supplies a
   reference to it.

Prototype:
   int IQoSSpec_GetRxFlowSpec(IQoSSpec* po, IQoSList** ppIQoSFlowList)

Parameters:
   po: the interface pointer
   ppIQoSFlowList: returns the stored list of Rx Flow specs. If the IQoSSpec
                   object doesn't currently contain a list of Rx Flow specs,
                   *ppIQoSFlowList is set to NULL.

Return Value:
   AEE_SUCCESS: The list was returned successfully.
   AEE_EBADPARM: ppIQoSFlowList is NULL.

Comments:
   If the client wishes to use the returned IQoSList object beyond the scope of
   this function call, it should AddRef() it.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_SetRxFlowSpec()

=======================================================================

IQoSSpec_GetTxFlowSpec()

Description:
   This function gets the list of Tx Flow specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the stored IQoSList object but rather supplies a
   reference to it.

Prototype:
   int IQoSSpec_GetTxFlowSpec(IQoSSpec* po, IQoSList** ppIQoSFlowList)

Parameters:
   po: the interface pointer
   ppIQoSFlowList: returns the stored list of Tx Flow specs. If the IQoSSpec
                   object doesn't currently contain a list of Tx Flow specs,
                   *ppIQoSFlowList is set to NULL.

Return Value:
   AEE-SUCCESS: The list was returned successfully.
   AEE_EBADPARM: ppIQoSFlowList is NULL.

Comments:
   If the client wishes to use the returned IQoSList object beyond the scope of
   this function call, it should AddRef() it.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_SetTxFlowSpec()

=======================================================================

IQoSSpec_GetRxFilterSpec()

Description:
   This function gets the list of Rx Filter specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the stored IQoSList object but rather supplies a
   reference to it.

Prototype:
   int IQoSSpec_GetRxFilterSpec(IQoSSpec* po, IQoSList** ppIQoSFilterList)

Parameters:
   po: the interface pointer
   ppIQoSFilterList: returns the stored list of Rx Filter specs. If the IQoSSpec
                     object doesn't currently contain a list of Rx Filter specs,
                     *ppIQoSFilterList is set to NULL.

Return Value:
   AEE_SUCCESS: The list was returned successfully.
   AEE_EBADPARM: ppIQoSFilterList is NULL.

Comments:
   If the client wishes to use the returned IQoSList object beyond the scope of
   this function call, it should AddRef() it.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_SetRxFilterSpec()

=======================================================================

IQoSSpec_GetTxFilterSpec()

Description:
   This function gets the list of Tx Filter specs that is stored in the IQoSSpec
   object.
   This function doesn't copy the stored IQoSList object but rather supplies a
   reference to it.

Prototype:
   int IQoSSpec_GetTxFilterSpec(IQoSSpec* po, IQoSList** ppIQoSFilterList)

Parameters:
   po: the interface pointer
   ppIQoSFilterList: returns the stored list of Tx Filter specs. If the IQoSSpec
                     object doesn't currently contain a list of Tx Filter specs,
                     *ppIQoSFilterList is set to NULL.

Return Value:
   AEE_SUCCESS: The list was returned successfully.
   AEE_EBADPARM: ppIQoSFilterList is NULL.

Comments:
   If the client wishes to use the returned IQoSList object beyond the scope of
   this function call, it should AddRef() it.

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSSpec_SetTxFilterSpec()

=======================================================================*/

#endif      // AEEIQOSSPEC_H
