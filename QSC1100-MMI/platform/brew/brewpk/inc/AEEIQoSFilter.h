#ifndef AEEIQOSFILTER_H
#define AEEIQOSFILTER_H
/*===========================================================================

FILE: AEEIQoSFilter.h

SERVICES: AEE Quality of Service Filter definitions


DESCRIPTION: Base level definitions, typedefs, etc. for AEE QoS Filter

   Copyright (c) 2007 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

#include "AEENetAddrTypes.h"
#include "AEEIxOpts.h"

#define AEEIID_IQoSFilter    0x0102b6cc

//**********************************************************************
//
// IQoSFilter Interface
//
//**********************************************************************
typedef xOpt AEEQoSFilterOpt;

//
// IP Version
//
typedef uint32 AEEQOS_IPVersion;

// IP Version values:
#define  AEEQOSFILTER_IP_V4   4
#define  AEEQOSFILTER_IP_V6   6

//
// IPv4 Address, Used for both Source and Destination Addresses.
//
typedef struct
{
   INAddr  addr;
   INAddr  subnet_mask;

} AEEQOSFILTER_IPv4Addr;

//
// IPv4 Type Of Service
//
typedef struct
{
   uint8 val;
   uint8 mask;

} AEEQOSFILTER_IPv4TOS;

//
// IPv6 Address, Used for both Source and Destination Addresses.
//
typedef struct
{
   INAddr6  addr;
   uint8    prefix_len;

} AEEQOSFILTER_IPv6Addr;

//
// IPv6 Traffic Class
//
typedef struct
{
   uint8  val;
   uint8  mask;

} AEEQOSFILTER_IPv6TrafficClass;

//
// IPv6 Flow Label
//
typedef uint32 AEEQOSFILTER_IPv6FlowLabel;

//
// IP Next Protocol
//
typedef uint32 AEEQOSFILTER_IPNextProtocol;

// IP Next Protocol values:
#define  AEEQOSFILTER_IP_NEXT_PROTOCOL_TCP   6
#define  AEEQOSFILTER_IP_NEXT_PROTOCOL_UDP   17

//
// TCP/UDP Port Range, Used for both Source and Destination Ports.
//
typedef struct
{
   INPort    port;
   uint16    range;

} AEEQOSFILTER_Port;


//
// Defines to set various Quality of Service Filter Options
//

// Define for selecting a unique XOPT_VARIABLE value, based on id.
#define AEEQOSFILTEROPT_VARIABLE_UNIQUE(id)  XOPT_VARIABLE_UNIQUE(id)

// For the following option IDs, pVals are 32-bit quantities, no need to copy.
#define AEEQOSFILTEROPT_IP_VERSION                (XOPT_32BIT + 0)

#define AEEQOSFILTEROPT_IP_NEXT_PROTOCOL          (XOPT_32BIT + 1)

#define AEEQOSFILTEROPT_IPV6_FLOW_LABEL           (XOPT_32BIT + 2)

// For the following option IDs, pVals are pointers to opaque, copyable structures,
// the low 12 bits of the id specify length of the option, in bytes.
#define AEEQOSFILTEROPT_IPV4_SRC_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(0) \
                                                   + sizeof(AEEQOSFILTER_IPv4Addr))

#define AEEQOSFILTEROPT_IPV4_DST_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(1) \
                                                   + sizeof(AEEQOSFILTER_IPv4Addr))

#define AEEQOSFILTEROPT_IPV4_TOS                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(2) \
                                                   + sizeof(AEEQOSFILTER_IPv4TOS))

#define AEEQOSFILTEROPT_SRC_PORT                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(3) \
                                                   + sizeof(AEEQOSFILTER_Port))

#define AEEQOSFILTEROPT_DST_PORT                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(4) \
                                                   + sizeof(AEEQOSFILTER_Port))

#define AEEQOSFILTEROPT_IPV6_SRC_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(5) \
                                                   + sizeof(AEEQOSFILTER_IPv6Addr))

#define AEEQOSFILTEROPT_IPV6_DST_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(6) \
                                                   + sizeof(AEEQOSFILTER_IPv6Addr))

#define AEEQOSFILTEROPT_IPV6_TRAFFIC_CLASS        (AEEQOSFILTEROPT_VARIABLE_UNIQUE(7) \
                                                   + sizeof(AEEQOSFILTER_IPv6TrafficClass))

#define AEEQOSFILTEROPT_END                       XOPT_END
#define AEEQOSFILTEROPT_ANY                       XOPT_ANY
#define AEEQOSFILTEROPT_COPYOPTS                  XOPT_COPYOPTS

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IQoSFilter : public IxOpts
{
   virtual int CDECL ExtFunc (int nOption, uint32 dwVal) = 0;
   int CDECL RemoveOptAtIndex(int32 nIndex)
   {
      return RemoveOpt(XOPT_ANY, nIndex);
   }
   int CDECL GetOptAtIndex(int32 nIndex, AEEQoSFilterOpt *pfio)
   {
      return GetOpt(XOPT_ANY, nIndex, pfio);
   }
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Define IQoSFilter interface methods
#define INHERIT_IQoSFilter(iname) \
   INHERIT_IxOpts(iname); \
   int      (*ExtFunc)(iname* po, int nOption, uint32 dwVal)

// declare the actual interface
AEEINTERFACE_DEFINE(IQoSFilter);

static __inline uint32 IQoSFilter_AddRef(IQoSFilter *me)
{
   return AEEGETPVTBL(me,IQoSFilter)->AddRef(me);
}

static __inline uint32 IQoSFilter_Release(IQoSFilter *me)
{
   return AEEGETPVTBL(me,IQoSFilter)->Release(me);
}

static __inline int IQoSFilter_QueryInterface(IQoSFilter *me, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(me,IQoSFilter)->QueryInterface(me, cls, ppo);
}

static __inline int IQoSFilter_AddOpt(IQoSFilter *me, AEEQoSFilterOpt *afio)
{
   return AEEGETPVTBL(me,IQoSFilter)->AddOpt(me, afio);
}

static __inline int IQoSFilter_RemoveOpt(IQoSFilter *me, int32 nOptId)
{
   return AEEGETPVTBL(me,IQoSFilter)->RemoveOpt(me, nOptId, 0);
}

static __inline int IQoSFilter_RemoveOptAtIndex(IQoSFilter *me, int32 nIndex)
{
   return AEEGETPVTBL(me,IQoSFilter)->RemoveOpt(me, (XOPT_ANY), nIndex);
}

static __inline int IQoSFilter_GetOpt(IQoSFilter *me, int32 nOptId, AEEQoSFilterOpt *pfio)
{
   return AEEGETPVTBL(me,IQoSFilter)->GetOpt(me, nOptId, 0, pfio);
}

static __inline int IQoSFilter_GetOptAtIndex(IQoSFilter *me, int32 nIndex, AEEQoSFilterOpt *pfio)
{
   return AEEGETPVTBL(me,IQoSFilter)->GetOpt(me, XOPT_ANY, nIndex, pfio);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEQOS_IPVersion

Description:
  Specifies the IP version

Definition:

  #define  AEEQOSFILTER_IP_V4   4
  #define  AEEQOSFILTER_IP_V6   6

  typedef uint32 AEEQOS_IPVersion;

Members:
   AEEQOSFILTER_IP_V4: IP version 4
   AEEQOSFILTER_IP_V6: IP version 6

Comments:
   Mandatory Option, a QoS filter must contain a single AEEQOS_IPVersion option

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFILTER_IPv4Addr

Description:
   Specifies an IPv4 address (source or destination) filter.

Definition:

   typedef struct
   {
      INAddr  addr;
      INAddr  subnet_mask;

   } AEEQOSFILTER_IPv4Addr;

Members:
   addr: IPv4 address, specified in Network byte order.
   subnet_mask: subnet mask, for selecting a range of IPv4 addresses.
                specified in Network byte order. A subnet mask of
                all 1's (0xffffffff) specify a single address value.

Comments:
   1) If IPv4 address is specified, both addr and subnet_mask must be specified.
   2) In case IPv4 source address option in Tx QoS Filters or IPv4 destination
      address in Rx QoS Filters are specified then the following requirements
      shall be met:
      -  The IPv4 source address in Tx QoS Filters and the IPv4 destination
         address in Rx QoS Filters can only have a single address value.
         Hence subnet_mask must be 0xffffffff.
      -  These address values shall be same as the current IP address assigned
         to the mobile on the IP interface on which the QoS is being requested.
      -  If the IP address on the interface changes e.g., during a network
         handoff, the client that requested the QoS is responsible for updating
         the filters with the new address, or else the QoS may not be able to
         receive the desired treatment.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFILTER_IPv4TOS

Description:
   Specifies an IPv4 Type of Service (TOS) filter.

Definition:

   typedef struct
   {
      uint8 val;
      uint8 mask;

   } AEEQOSFILTER_IPv4TOS;

Members:
   val: specifies the desired value of TOS field's bits.
   mask: specifies which bits in TOS field should be considered for filtering,
         in essence the TOS field is first OR-ed with "mask" and then compared
         to "val". ~
         i.e. a mask of 11111111 refers to packets with the EXACT TOS field value

Comments:
   If IPv4 Type of Service is specified, both val and mask must be specified.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFILTER_IPv6Addr

Description:
   Specifies an IPv6 address (source or destination) filter.

Definition:

   typedef struct
   {
      INAddr6  addr;
      uint8    prefix_len;

   } AEEQOSFILTER_IPv6Addr;      

Members:
   addr: IPv6 address, specified in network byte order.
   prefix_len: prefix length, for selecting a range of IPv6 addresses.
               A prefix length which is equal to 128 specifies a single address value.

Comments:
   1) If IPv6 address is specified, both addr and prefix_len must be specified.
   2) In case IPv6 source address option in Tx QoS Filters or IPv6 destination 
      address in Rx QoS Filters are specified then the following requirements 
      shall be met:
      -  The IPv6 source address in Tx QoS Filters and the IPv6 destination 
         address in Rx QoS Filters can only have a single address value. 
         Hence, the value of prefix_len must be 128.
      -  These address values shall be same as the current IP address assigned 
         to the mobile on the IP interface on which the QoS is being requested.
      -  If the IP address on the interface changes e.g., during a network 
         handoff, the client that requested the QoS is responsible for updating 
         the filters with the new address, or else the QoS may not be able to 
         receive the desired treatment. 

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFILTER_IPv6TrafficClass

Description:
   Specifies an IPv6 Traffic Class filter.

Definition:

   typedef struct
   {
      uint8  val;
      uint8  mask;

   } AEEQOSFILTER_IPv6TrafficClass;


Members:
   val: specifies the desired value of Traffic Class's field.
   mask: specifies which bits in the Traffic Class field should be considered for filtering.
         Should be specified in host byte order.
         In essence, the result of the Traffic Class field And-ed with "mask" is compared 
         to the result of "val" And-ed with "mask". ~
         i.e., a mask of 11111111 refers to packets with a Traffic Class field value
         which is equal to the "val" value.

Comments:
   If IPv6 Traffic Class is specified, both val and mask must be specified.

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
AEEQOSFILTER_IPv6FlowLabel

Description:
   Specifies an IPv6 Flow Label filter.
   Should be specified in Network byte order.

Definition:

   typedef uint32 AEEQOSFILTER_IPv6FlowLabel;

Members:
   None

Comments:
   None

Version:
   Introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================

AEEQOSFILTER_IPNextProtocol

Description:
   Specifies the next protocol higher then IP, i.e TCP, UDP.

Definition:

  #define  AEEQOSFILTER_IP_NEXT_PROTOCOL_TCP   6
  #define  AEEQOSFILTER_IP_NEXT_PROTOCOL_UDP   17

  typedef uint32 AEEQOSFILTER_IPNextProtocol;

Members:
   AEEQOSFILTER_IP_NEXT_PROTOCOL_TCP:
   specifies that the filter will apply only to TCP packets.

   AEEQOSFILTER_IP_NEXT_PROTOCOL_UDP:
   specifies that the filter will apply only to UDP packets.

Comments:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFILTER_Port

Description:
   Specifies a Port (TCP/UDP) range filter.

Definition:

   typedef struct
   {
      INPort    port;
      uint16    range

   } AEEQOSFILTER_Port;

Members:
   port: TCP/UDP port value, specified in network byte order.
   range: Specifies a range of port numbers starting from the value specified
          in port to port+range. ~
          i.e. for a single port value, range shall be set to zero ~
          specified in host byte order.

Comments:
   If Port specified, both port and range must be specified.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQoSFilterOpt

Description:
   Holds an (id, value) pair for AEEQoSFilterOpt.

Definition:
   typedef xOpt AEEQoSFilterOpt;

Members:
   int32  nId: option id, chosen from one of the QoS Filter options
   void *pVal: option value, depends on nId

Comments:
   AEEQoSFilterOpt structures indicate various parameters of an AEEQoSFilter.
   The structures, when passed to AddOpt, are not themselves copied,
   the contents are copied out.

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================
AEEQOSFILTEROPT_

Description:
   The prefix AEEQOSFILTEROPT_ is used for QoS Filter options.
   QoS Filter options specifies a set of IP packet filter rules. ~
   Packets matching these rules are considered part of the QoS flow.

Definition:
   #define AEEQOSFILTEROPT_IP_VERSION                (XOPT_32BIT + 0)

   #define AEEQOSFILTEROPT_IP_NEXT_PROTOCOL          (XOPT_32BIT + 1)

   #define AEEQOSFILTEROPT_IPV6_FLOW_LABEL           (XOPT_32BIT + 2)

   #define AEEQOSFILTEROPT_VARIABLE_UNIQUE(id)       XOPT_VARIABLE_UNIQUE(id)

   #define AEEQOSFILTEROPT_IPV4_SRC_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(0)
                                                      + sizeof(AEEQOSFILTER_IPv4Addr))

   #define AEEQOSFILTEROPT_IPV4_DST_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(1)
                                                      + sizeof(AEEQOSFILTER_IPv4Addr))

   #define AEEQOSFILTEROPT_IPV4_TOS                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(2)
                                                      + sizeof(AEEQOSFILTER_IPv4TOS))

   #define AEEQOSFILTEROPT_SRC_PORT                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(3)
                                                     + sizeof(AEEQOSFILTER_Port))

   #define AEEQOSFILTEROPT_DST_PORT                  (AEEQOSFILTEROPT_VARIABLE_UNIQUE(4)
                                                      + sizeof(AEEQOSFILTER_Port))

   #define AEEQOSFILTEROPT_IPV6_SRC_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(5) \
                                                      + sizeof(AEEQOSFILTER_IPv6Addr))

   #define AEEQOSFILTEROPT_IPV6_DST_ADDR             (AEEQOSFILTEROPT_VARIABLE_UNIQUE(6) \
                                                      + sizeof(AEEQOSFILTER_IPv6Addr))

   #define AEEQOSFILTEROPT_IPV6_TRAFFIC_CLASS        (AEEQOSFILTEROPT_VARIABLE_UNIQUE(7) \
                                                      + sizeof(AEEQOSFILTER_IPv6TrafficClass))

   #define AEEQOSFILTEROPT_END                       XOPT_END

   #define AEEQOSFILTEROPT_ANY                       XOPT_ANY

   #define AEEQOSFILTEROPT_COPYOPTS                  XOPT_COPYOPTS

Members:
   AEEQOSFILTEROPT_IP_VERSION:
   IP version, mandatory option (see AEEQOS_IPVersion)

   AEEQOSFILTEROPT_IP_NEXT_PROTOCOL:
   IP next protocol option (see AEEQOSFILTER_IPNextProtocol)
   must be specified if the dst port and/or src port option is used.

   AEEQOSFILTEROPT_IPV4_SRC_ADDR:
   IPv4 source address option (see AEEQOSFILTER_IPv4Addr)

   AEEQOSFILTEROPT_IPV4_DST_ADDR:
   IPv4 destination address option (see AEEQOSFILTER_IPv4Addr)

   AEEQOSFILTEROPT_IPV4_TOS:
   IPv4 Type of Service option (see AEEQOSFILTER_IPv4TOS)

   AEEQOSFILTEROPT_IPV6_SRC_ADDR:
   IPv6 source address option (see AEEQOSFILTER_IPv6Addr)

   AEEQOSFILTEROPT_IPV6_DST_ADDR:
   IPv6 destination address option (see AEEQOSFILTER_IPv6Addr)

   AEEQOSFILTEROPT_IPV6_TRAFFIC_CLASS:
   IPv6 Traffic Class option (see AEEQOSFILTER_IPv6TrafficClass)

   AEEQOSFILTEROPT_IPV6_FLOW_LABEL:
   IPv6 Flow Label option (see AEEQOSFILTER_IPv6FlowLabel)

   AEEQOSFILTEROPT_SRC_PORT:
   source port range option (see AEEQOSFILTER_Port)

   AEEQOSFILTEROPT_DST_PORT:
   destination port range option (see AEEQOSFILTER_Port)

Comments:
   1. Applications must specify a single AEEQOSFILTEROPT_IP_VERSION option for
      each filter (either AEEQOSFILTER_IP_V4 or AEEQOSFILTER_IP_V6). ~
      Other then that a QoS Filter needs to have at least one more option in
      order to be valid.

   2. Applications must not specify an option more then once.
      AddOpt() will fail for an option that already exists in the IQoSFilter.

   3. IP source address in Tx filters and IP destination address in Rx filters
      should not be specified.

Version:
   Introduced BREW Client 3.1
   IPv6 options introduced BREW Client 3.1.5 SP01

See Also:
   None

=======================================================================
Valid combinations of QoS Filter options

These are the rules for valid combinations of QoS Filter options. 
If these criteria are not met the validation of the QoS Filter will fail, 
and QoS request will not succeed.

Only those options marked with an "X" may be specified for a single QoS Filter.
All marked attributes may be specified, but at least one shall be specified.

===pre>
  QoS Filter Option	                                            Valid combination types
                                                                       I     II     III
  AEEQOSFILTEROPT_IPV4_SRC_ADDR / AEEQOSFILTEROPT_IPV6_SRC_ADDR        X     X      X
  AEEQOSFILTEROPT_IPV4_DST_ADDR / AEEQOSFILTEROPT_IPV6_DST_ADDR        X     X      X
  AEEQOSFILTEROPT_IP_NEXT_PROTOCOL                                     X     X            
  AEEQOSFILTEROPT_SRC_PORT                                             X
  AEEQOSFILTEROPT_DST_PORT                                             X
  AEEQOSFILTEROPT_IPV4_TOS / AEEQOSFILTEROPT_IPV6_TRAFFIC_CLASS        X     X      X   
  AEEQOSFILTEROPT_IPV6_FLOW_LABEL                                                   X
===/pre>

Note that a filter can not contain a mixture of IPv4 and IPv6 fields.
For example, one can not specify both AEEQOSFILTEROPT_IPV4_SRC_ADDR and
AEEQOSFILTEROPT_IPV6_TRAFFIC_CLASS in the same filter.


=======================================================================*/

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQoSFilter

Description:
   IQoSFilter interface encapsulates a Quality of Service IP Filter
   specification. ~
   The IP filter spec, when applied to stream of IP packets, segregates a
   specific IP flow which is characterized by the filter spec parameters, such
   as IP address and TCP/UDP ports. ~
   An IQoSFilter interface instance is obtained via the IEnv_CreateInstance()
   mechanism.

The following header file is required:
   BREW 4.0 - AEEIQoSFilter.h
   BREW 3.1 - AEEQoSFilter.h

=============================================================================

IQoSFilter_AddRef()

Description:
   This method increments the reference count of the IQoSFilter Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IQoSFilter_AddRef(IQoSFilter * po)

Parameters:
   po [in]: Pointer to the IQoSFilter Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSFilter_Release()

=============================================================================

IQoSFilter_Release()


Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQoSFilter_Release(IQoSFilter * po)

Parameters:
   po [in]: Pointer to the IQoSFilter Interface object

Return Value:
   Decremented reference count for the object.

   The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IQoSFilter_AddRef()

==============================================================================

Function: IQoSFilter_QueryInterface()

Description:
   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interface is returned AddRef'ed. The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:
   int IQoSFilter_QueryInterface(IQoSFilter *po, AEECLSID id, void **p)

Parameters:
   po [in]: Pointer to IQoSFilter interface
   id [in]: A globally unique id to identify the entity (interface or data) that
            we are trying to query.
   p [out]: Pointer to the data or interface that we want to retrieve.
            If the interface is not available, this is set to NULL.

Return Value:
   AEE_SUCCESS: on success
   AEE_ECLASSNOTSUPPORT: if class ID is not supported

Comments:
   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFilter_AddOpt()

Description:
   Append the list of AEEQoSFilterOpts to the current QoS Filter option set

Prototype:
  int IQoSFilter_AddOpt(IQoSFilter *po, AEEQoSFilterOpt *apwoz)

Parameters:
  po: the interface pointer
  apwoz: array of AEEQoSFilterOpt structs, terminated by a AEEQoSFilterOpt with
         nId set to AEEQOSFILTEROPT_END

Return Value:
   AEE_SUCCESS: options were added successfully
   AEE_ENOMEMORY: could not allocated enough memory
   AEE_EBADITEM: on of the items either:
            (1) has same option ID as an existing option in the QoS Filter.
            (2) conflicts with another option in the QoS Filter.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFilter_RemoveOpt()

Description:
   Removes the AEEQoSFilterOpt matching nOptId

Prototype:
   int IQoSFilter_RemoveOpt(IQoSFilter *po, int32 nOptId)

Parameters:
   po:  the interface pointer
   nOptId: id of the option to remove from the options list

Return Value:
   AEE_SUCCESS: if the option was removed successfully
   AEE_EFAILED: if the specified AEEQoSFilterOpt couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFilter_RemoveOptAtIndex()

Description:
   Removes the AEEQoSFilterOpt at nIndex

Prototype:

   int IQoSFilter_RemoveOptAtIndex(IQoSFilter *po, int32 nIndex)

Parameters:
   po:  the interface pointer
   nIndex: id of the option to remove from the options list

Return Value:
   AEE_SUCCESS: if the option was removed successfully
   AEE_EFAILED: if the specified index couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================

Function: IQoSFilter_GetOpt()

Description:
   Find an AEEQoSFilterOpt, in the list of AEEQoSFilterOpts in po, matching nOptId.
   pwo is filled in with a the matching AEEQoSFilterOpt contents.

Prototype:
   int IQoSFilter_GetOpt(IQoSFilter *po, int32 nOptId, AEEQoSFilterOpt *pwo)

Parameters:
   po :  the interface pointer
   nOptId: id of the option to find in the options list
   pwo [out]: place holder for the requested AEEQoSFilterOpt

Return Value:
   AEE_SUCCESS: if the option was retrieved successfully
   AEE_EFAILED: if the specified AEEQoSFilterOpt couldn't be retrieved

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=============================================================================

Function: IQoSFilter_GetOptAtIndex()

Description:
   Find the AEEQoSFilterOpt, in the list of AEEQoSFilterOpts in po, at nIndex.
   pwo is filled in with a the matching AEEQoSFilterOpt contents.

Prototype:
   int IQoSFilter_GetOptAtIndex(IQoSFilter *po, int32 nIndex, AEEQoSFilterOpt *pwo)

Parameters:
   po :  the interface pointer
   nIndex: Option Index.
   pwo [out]: place holder for the requested AEEQoSFilterOpt

Return Value:
   AEE_SUCCESS: if the option was retrieved successfully
   AEE_EFAILED: if the specified index couldn't be found

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   None

=======================================================================*/

#endif  // AEEIQOSFILTER_H
