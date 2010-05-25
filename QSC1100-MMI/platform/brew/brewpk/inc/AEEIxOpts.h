#ifndef AEEIXOPTS_H
#define AEEIXOPTS_H
/*======================================================
      Copyright © 2004, 2007 QUALCOMM Incorporated
              All Rights Reserved.
           QUALCOMM Confidential and Proprietary
=====================================================

DESCRIPTION:

        IxOpts is an interface for manipulating a container of options.  It
        provides retrieval/transaction/utility APIs for services needing to
        store a tree of ordered information.  Items can be integers, strings,
        variable length structures, interfaces, etc.

=====================================================*/
#include "AEEIQI.h"

#define AEEIID_IxOpts (0x0102c269)


/* =======================================================================
   Type Declarations
   ======================================================================= */

/*
  ||
  || xOpt
  ||
  || used to hold an id-value pair, for Get/SetOpt
  ||
*/
typedef struct xOpt
{
   int32  nId;   /* from below defines */
   void  *pVal;  /* depends on optname */
} xOpt;

/*
  ||
  || xOpt option ids
  ||
  || xOpt ids fall in ranges that are designed to keep the pVal data opaque
  ||  while helping xOpts deal with copying the data if necessary.
  ||
  || During processing, the data in options passed to GetResponse
  ||  are not copied by default, they must stay valid until the GetResponse
  ||  callback fires.  Data passed as options may be copied by default,
  ||  so it needn't be kept around, unless you specifically turn off
  ||  COPYOPTS on the IWeb.
  ||
*/
/*
  || pVals are reserved, as are ids, up to 0x0ffff
*/
#define XOPT_RESERVED       0
#define XOPT_RESERVED_LAST  0x0ffff
#define XOPT_END            0 /* no pVal, ignore rest, used for terminating
                                   a vector of xOpts */
#define XOPT_ANY            1 /* no pVal, matches any opt, used for
                                   enumeration */
#define XOPT_REMOVED        2 /* not used */
#define XOPT_COPYOPTS       3 /* (boolean) if set to TRUE, causes the
                                   IxOpts to copy current and future xOpt
                                   values.  Unfortunately, it is almost a
                                   one-way street, since only an empty
                                   IxOpts may be changed from copy to
                                   non-copy */

/*
   || pVals are null-terminated strings, valid ids up to 0x01ffff
*/
#define XOPT_SZ             0x00010000
#define XOPT_SZ_LAST        0x0001ffff

/*
  || pVals are 32-bit quantities, no need to copy, valid ids up to
  ||  0x02ffff
*/
#define XOPT_32BIT            0x00020000
#define XOPT_32BIT_LAST       0x0002ffff

/*
  || pVals are pointers to interfaces, which are AddRef'd to copy
  || valid ids up to 0x03ffff
*/
#define XOPT_INTERFACE      0x00030000
#define XOPT_INTERFACE_LAST 0x0003ffff

/*
  || Following xOpts are pointers to interfaces, which are AddRef'd to copy
  || valid ids up to 0x03ffff
*/
/* (XOPT_INTERFACE+1) is already taken as WEBOPT_BODY defined AEEWeb.h */
#define XOPT_DEFAULTS       (XOPT_INTERFACE+2) /* (IxOpts *), used for
                                                      chaining IxOpts */



/*
  || pVals are pointers to opaque, but copyable structures,
  || the first two bytes give the length of the following data.
  || The length is little-endian, low byte first.
*/
#define XOPT_VARBUFFER      0x00040000
#define XOPT_VARBUFFER_LAST 0x0004ffff


/*
  || pVals are pointers to opaque ASN.1, but copyable structures
  || The length of the object comes from the ASN.1 length of the
  || object which starts at the second byte of the data. The
  || first byte is the type (it doesn't matter what the type of
  || the object is). The length is in the standard ASN.1 length
  || format. (At present there is a 64Kb limit on the length).
  || To derive the total length of the object, take the ASN.1
  || length of the object, add one for the type and then the
  || length of the length. Note this is for DER & BER encoded
  || objects. Note that it is a good idea to check the
  || length and be sure it is valid before adding such options,
  || especially if copy opts is set. If the length is wilde
  || a malloc of a large amount of memory may be attempted
  || and/or a copy off the end of a buffer may be attempted.
*/
#define XOPT_ASN1      0x00050000
#define XOPT_ASN1_LAST 0x0005ffff

/*
   || pVals are null-terminated AECHAR strings, valid ids up to 0x06ffff
*/
#define XOPT_WSZ       0x00060000
#define XOPT_WSZ_LAST  0x0006ffff


/*
  || pVals are pointers to opaque, but copyable structures,
  || the low 12 bits of the id specify length of the option, in bytes
  || valid ids up to 0x1ffff000
*/
#define XOPT_VARIABLE       0x10000000
#define XOPT_VARIABLE_LAST  0x1ffff000
#define XOPT_VARIABLE_INC   0x00001000

/*Macro for selecting a unique XOPT_VARIABLE value, based on id.
  IDs are unique per subsystem (but not across subsystems
  Define for selecting a unique XOPT_VARIABLE value, based on id.*/
#define XOPT_VARIABLE_UNIQUE(id)       (XOPT_VARIABLE + (id << 12))

/*=====================================================================
  function declarations and inline function definitions
  ===================================================================== */

/*
  ||
  || The IxOpts base class
  ||
  || IxOpts is intended to lend some power and flexibility to
  ||   implementations.  For example, IWebOpts, IWeb and IWebResponse
  ||   descend from it.
  ||
*/

#if defined(AEEINTERFACE_CPLUSPLUS)

struct IxOpts : public IQI
{
   virtual int CDECL AddOpt(xOpt *apItems) = 0;
   virtual int CDECL RemoveOpt(int32 nOptId, int32 nIndex) = 0;
   virtual int CDECL GetOpt(int32 nOptId, int32 nIndex, xOpt *pItem) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define INHERIT_IxOpts(iname)\
   INHERIT_IQI(iname);\
   int (*AddOpt)(iname *po, xOpt *apItems); \
   int (*RemoveOpt)(iname *po, int32 nOptId, int32 nIndex); \
   int (*GetOpt)(iname *po, int32 nOptId, int32 nIndex, xOpt *pItem)


/*  declare the actual  interface */
AEEINTERFACE_DEFINE(IxOpts);


static __inline uint32 IxOpts_AddRef(IxOpts *pif)
{
   return AEEGETPVTBL(pif,IxOpts)->AddRef(pif);
}

static __inline uint32 IxOpts_Release(IxOpts *pif)
{
   return AEEGETPVTBL(pif,IxOpts)->Release(pif);
}

static __inline int IxOpts_QueryInterface(IxOpts *pif, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(pif,IxOpts)->QueryInterface(pif, cls, ppo);
}

static __inline int IxOpts_AddOpt(IxOpts *pif, xOpt *apItems)
{
   return AEEGETPVTBL(pif,IxOpts)->AddOpt(pif, apItems);
}

static __inline int IxOpts_RemoveOpt(IxOpts *pif,  int32 nOptId, int32 nIndex)
{
   return AEEGETPVTBL(pif,IxOpts)->RemoveOpt(pif, nOptId, nIndex);
}

static __inline int IxOpts_GetOpt(IxOpts *pif, int32 nOptId, int32 nIndex, xOpt *pItem)
{
   return AEEGETPVTBL(pif,IxOpts)->GetOpt(pif,  nOptId,  nIndex, pItem);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

#define XOPT_GETVARBUFFERLENGTH(pbuf)            ((uint16)(((unsigned char *)(pbuf))[0] + (((unsigned char *)(pbuf))[1] << 8)))
/*Gives the length of XOPT_VARBUFFER*/
#define XOPT_GETVARBUFFERDATA(pbuf)              ((unsigned char *)((unsigned char*)(pbuf) + sizeof(uint16)))
/*Gives the data in XOPT_VARBUFFER*/

// provide a stub implementation of memmove() to keep 
// this header free of external dependencies
static __inline void *__ixopts_memmove(void* pTo, const void* cpFrom, int nLen)
{
   char* pcTo = (char*)pTo;
   const char* cpcFrom = (const char*)cpFrom;

   if (pcTo < cpcFrom) {
      /* run forwards */
      while (nLen-- > 0) {
         *pcTo++ = *cpcFrom++;
      }
   } else if (pcTo > cpcFrom) { 
      /* run backwards */
      pcTo += nLen;
      cpcFrom += nLen;

      while (nLen-- > 0) {
         *--pcTo = *--cpcFrom;
      }
   }
   return pTo;
}

#define XOPT_SETVARBUFFER(pbuf,pdata,len)        ((unsigned char*)(pbuf))[0] = (unsigned char)((len) & 0x00ff), \
                                                    ((unsigned char*)(pbuf))[1] = (unsigned char)((len) >> 8), \
                                                    __ixopts_memmove(((unsigned char*)(pbuf) + sizeof(uint16)),(pdata),(len))

/*Sets the XOPT_VARBUFFER with the data and length provided.*/

/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

xOpt

Description:
   Holds an id, value pair for IxOpts

Definition:
   typedef struct xOpt
   {
      int32        nId;
      void        *pVal;
   } xOpt;

Members:
   nId: option id, chosen from one of the XOPT_* defines.

   pVal: option value, depends on nId

Comments:
   xOpt structures are the basic building block for talking to an
   interface using IxOpts.  The structures, when passed to AddOpt, are
   not themselves copied, the contents are copied out

See Also:
   IxOpts_AddOpt()

=======================================================================

*/

/*
=======================================================================
   INTERFACES DOCUMENTATION
=======================================================================

IxOpts Interface

Description: xOpt management API, used to manipulate
        configuration for a service such as IWeb.  IxOpts represents a
        stack or list of options, which may be single- or multi-valued.
        IxOpt does not enforce semantics of single- or multi-valued
        options.  Access to the options stack is dictated by the order
        in which the options are added.

For example:
===pre>

   A list built with:
        AddOpt({OPT1, x})            ==> push {OPT1, x}
        AddOpt({OPT1, y})            ==> push {OPT1, y}
        AddOpt({OPT1, z})            ==> push {OPT1, z}
        RemoveOpt(OPT1, 0)           ==> remove 0th OPT1 (which is {OPT1, z})
        AddOpt({OPT2, b},{OPT2, a}}) ==> push {OPT2, a}, then push {OPT2, b}

   Yields answers:
        GetOpt(OPT1, 0) ==> find 0th OPT1 => {OPT1, y}
        GetOpt(OPT1, 1) ==> find 1st OPT1 => {OPT1, x}
        GetOpt(OPT1, 2) ==> find 2nd OPT1 => AEE_EFAILED
        GetOpt(OPT1, 3) ==> find 3rd OPT1 => AEE_EFAILED
        GetOpt(OPT2, 1) ==> find 1st OPT2 => {OPT2, a}
===/pre>
   IxOpts is the base class of IWeb and IWebResp, which use IxOpts
   methods to communicate all kinds of stuff.


   IxOpt nOptIds


   The xOpt ids fall in ranges that are designed to keep the pVal data opaque
     while helping the service deal with copying the data if necessary.

   Depending on the underlying service, the data passed as options may or
   may not be copied.  If it is not copied, the data must stay valid as specified
   for the service. Check the specific service which uses IxOpts
   for the default copy option.

   Here are the ranges and to-date defined XOPT id values and ranges:

    - RESERVED range: pVals are reserved, as are ids, up to 0x0ffff, used
                   for control of the IxOpts, markers in internal lists

       XOPT_RESERVED:       0
       XOPT_RESERVED_LAST:  0x0ffff

       XOPT_END: - no pVal, used for terminating a vector of xOpts

       XOPT_ANY: - no pVal, matches any opt, used for enumeration

       XOPT_REMOVED: - no pVal, means skip or ignore, is deleted

       XOPT_COPYOPTS: - (boolean) if set to TRUE, causes the IxOpts
                  to copy current and future xOpt values.
                  Unfortunately, it's almost a one-way street, since
                  only an empty IxOpts may be changed from copy to
                  non-copy. XOPT_COPYOPTS is set to true by default.

    - SZ RANGE: pVals are null-terminated strings, valid ids up to 0x01ffff

        XOPT_SZ:            0x00010000
        XOPT_SZ_LAST:       0x0001ffff


    - 32-bit range: pVals 32-bit quantities, no need to (actually can't)
                        copy, valid ids up to 0x02ffff

        XOPT_32BIT:           0x00020000
        XOPT_32BIT_LAST:       0x0002ffff

    - BREW Interface range: pVals are pointers to interfaces, which are
                        AddRef'd to copy valid ids up to 0x03ffff

        XOPT_INTERFACE:      0x00030000
        XOPT_INTERFACE_LAST: 0x0003ffff

        One XOPT_INTERFACE is reserved for the IxOpts interface: XOPT_DEFAULTS.
        XOPT_DEFAULTS can be used to add an IxOpt with a default set of values
        to another IxOpt.
        
    - variable size range:  pVals are pointers to opaque, but copyable
                       structures, the low 12 bits of the id specify length
                       of the option, in bytes. Valid ids up to 0x1ffff000,
                       spaced by 0x1000.

        XOPT_VARIABLE:       0x10000000
        XOPT_VARIABLE_LAST:  0x1ffff000

    - variable buffer range: pVals are pointers to opaque, but
                 copyable structures, the first two bytes give the
                 length of the following data. The length is
                 little-endian, low byte first.  Valid ids up to
                 0x0004ffff.

        XOPT_VARBUFFER:      0x00040000
        XOPT_VARBUFFER_LAST: 0x0004ffff

    - ASN.1 range: pVals are pointers to opaque ASN.1, but copyable
                 structures. The length of the object comes from the
                 ASN.1 length of the object which starts at the second
                 byte of the data. The first byte is the type (it
                 doesn't matter what the type of the object is). The
                 length is in the standard ASN.1 length format. (At
                 present there is a 64Kb limit on the length).  To
                 derive the total length of the object, take the ASN.1
                 length of the object, add one for the type and then
                 the length of the length. Note this is for DER & BER
                 encoded objects. Note that it is a good idea to check
                 the length and be sure it is valid before adding such
                 options, especially if copy opts is set. If the
                 length is wilde a malloc of a large amount of memory
                 may be attempted and/or a copy off the end of a
                 buffer may be attempted.  Valid ids up to 0x0005ffff.

        XOPT_ASN1:      0x00050000
        XOPT_ASN1_LAST: 0x0005ffff

    - WSZ RANGE: pVals are null-terminated AECHAR strings, valid ids up to
                0x06ffff

        XOPT_WSZ:             0x00060000
        XOPT_WSZ_LAST:        0x0006ffff


Required header files:

   AEEIxOpts.h

See Also:
   IWeb, IWebOpts

=======================================================================

IxOpts_AddRef()

Description:
   This function is inherited from IQI_AddRef().

=======================================================================

IxOpts_Release()

Description:
   This function is inherited from IQI_Release().

=======================================================================

IxOpts_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

=======================================================================

IxOpts_AddOpt()

Description:
   Adds the list of xOpt's to the current option set.

Prototype:

    int IxOpts_AddOpt(IxOpts *pIxOpts, xOpt *apItems);

Parameters:

  pIxOpts: the interface pointer

  apItems: array of xOpt structs, terminated by a xOpt with
                   nId set to XOPT_END

Return Value:
   AEE_SUCCESS if the options could be added successfully
   AEE_ENOMEMORY if enough memory couldn't be allocated

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

IxOpts_RemoveOpt()

Description:
   remove the nIndex'th xOpt matching nOptId

Prototype:

   int IxOpts_RemoveOpt(IxOpts *pif, int32 nOptId, int32 nIndex);

Parameters:

  pif    :  the interface pointer

  nOptId : id of the option to remove from the options list

  nIndex : which (if there are multiple xOpts of the same id) to
                  remove nIndex counts from most recently AddOpt()ed option

Return Value:

   AEE_SUCCESS: if the options could be added successfully
   AEE_EFAILED: if the specified xOpt couldn't be found

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

IxOpts_GetOpt()

Description:

   Find a xOpt in the list of xOpts in pIxOpts matching nOptId.  There
   may be multiple matches (either in the case of a multi-valued
   xOpt, or in the case of overrides).  In this case nIndex
   specifies how far into the list of xOpts that match nOptId to
   look.  pItem is filled in with the matching xOpt contents.
   nIndex is zero-based.

Prototype:

   int IxOpts_GetOpt(IxOpts *pIxOpts, int32 nOptId, int32 nIndex,
                       xOpt *pItem)

Parameters:

  pIxOpts:  the interface pointer

  nOptId:  id of the option to find in the options list, XOPT_ANY
                 returns the nIndexth option, if there is one...

  nIndex:  which (if there are multiple xOpts of the same id) to get
                   nIndex counts from most recently AddOpt()ed option, it's
                   zero-based

  pItem:  where to stick the found xOpt

Return Value:
   AEE_SUCCESS: if the options could be added successfully
   AEE_EFAILED: if the specified xOpt couldn't be found
   AEE_EBADPARM: if pItem is NULL

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

*/

#endif /* AEEIXOPTS_H */
