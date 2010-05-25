#ifndef _I_REFERENCE_COUNTABLE_
#define _I_REFERENCE_COUNTABLE_ 


/* =======================================================================
      IReferenceCountable.h

DESCRIPTION
  This module defines the IReferenceCountable interface. It is a subset of the
  IQueryInterface interface in CS, or IUnknown in COM. Reference counting is
  used to allow multiple clients use the same object without copying and without
  knowing when other clients are done with the object, to free the object
  correctly. Refer to a COM book for how to use reference counting.

Portions copyrighted by QUALCOMM Incorporated;
Copyright 2007 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/referencecount/main/latest/inc/ireferencecountable.h#1 $
$DateTime: 2008/05/14 09:40:11 $
$Change: 659468 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtv_msg.h"
#include "qtvsystem.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
class IReferenceCountable
{
public:
  IReferenceCountable() { }

  virtual ~IReferenceCountable() { }
  virtual int32 AddRef() = 0;
  virtual int32 Release() = 0;
};

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* The following macros can be used to implement reference counting.
 * An alternative to implementing reference counting for each concrete
 * implementation of IReferenceCountable would be to use multiple inheritence
 * with a mixin class that does reference counting. We have decided against
 * using a mixin class for the following reasons:
 * 1) The overhead of multiple inheritence.
 * 2) We need to use virtual base classes since the combined class would inherit
 *    from the interface that it implements (which would be a subclass of
 *    IReferenceCountable), and the mixin class which would also inherit
 *    from IReferenceCountable. Virtual base classes are tricky and also
 *    add performance overhead.
 * 3) Using virtual base classes, we would either still have to override 
 *    AddRef() and Release() to call the mixin's implementation, or let the
 *    mixin's implementation override the pure virtual functions automatically,
 *    which VC++ calls "inheritance via dominance" and warns about.
 * 4) The mixin class would have relatively little code so we are not hiding much
 *    anyway.
 */

// Macro that can put in the class declaration to implement reference counting.
// This is for objects that are used on a single thread.
#define IMPLEMENT_IREFERENCECOUNTABLE(cname) \
public: \
  virtual int32 AddRef() \
  { \
    QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_MED, \
    #cname \
    ":%x AddRef() to ref count of %d", (void *) this, m_refCount + 1); \
    return ++m_refCount; \
  } \
  \
  virtual int32 Release() \
  { \
    if (m_refCount <= 0) \
    { \
      QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_ERROR, \
        #cname \
        ":%x Releasing object with ref count = %d!", \
        (void *) this, m_refCount); \
      return 0; \
    } \
    \
    m_refCount--; \
    \
    QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_MED, \
    #cname \
    ":%x Release() to ref count of %d", (void *) this, m_refCount); \
    \
    if (m_refCount == 0) \
    { \
      QTV_Delete(this); \
      /* Don't access m_refCount anymore! */ \
      return 0; \
    } \
    \
    return m_refCount; \
  } \
  \
protected: \
  int32 m_refCount; \
private:

#define ATOMIC_INCREMENT(px) AtomicIncrement(px)
#define ATOMIC_DECREMENT(px) AtomicDecrement(px)

inline int32 AtomicIncrement(int32 *px)
{ 
  int32 count = 0;
  INTLOCK() 
  count = *px + 1;
  *px = count;
  INTFREE()
  return count;
}

inline int32 AtomicDecrement(int32 *px)
{
  int32 count = 0;
  INTLOCK()
  count = *px - 1;
  *px = count;
  INTFREE()
  return count;
}

// Macro that can put in the class declaration to implement reference counting.
// This is for objects that are used on multiple threads.
#define IMPLEMENT_IREFERENCECOUNTABLE_MT(cname) \
public: \
  virtual int32 AddRef() \
  { \
    int32 count = ATOMIC_INCREMENT(&m_refCount); \
    QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_MED, \
    #cname \
    ":%x AddRef() to ref count of %d", (void *) this, count); \
    return count; \
  } \
  \
  virtual int32 Release() \
  { \
    if (m_refCount <= 0) \
    { \
      QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_ERROR, \
        #cname \
        ":%x Releasing object with ref count = %d!", \
        (void *) this, m_refCount); \
      return 0; \
    } \
    \
    int32 count = ATOMIC_DECREMENT(&m_refCount); \
    \
    QTV_MSG_PRIO2(QTVDIAG_DEBUG, QTVDIAG_PRIO_MED, \
    #cname \
    ":%x Release() to ref count of %d", (void *) this, count); \
    \
    if (count == 0) \
    { \
      QTV_Delete(this); \
      /* Don't access m_refCount anymore! */ \
      return 0; \
    } \
    \
    return count; \
  } \
  \
protected: \
  int32 m_refCount; \
  \
private:

// Macro that resets the reference count of an class that implements
// IReferenceCountable using IMPLEMENT_IREFERENCECOUNTABLE or 
// IMPLEMENTS_IREFERENCECOUNTABLE_MT. 
// Note: this macro does not do any locking. Normally it is used in the
// constructor of class, in which the object is not being used anywhere else.
#define RESET_REFCOUNT m_refCount = 0

// To be put in the first few lines of a destructor of a class that 
// implements IReferenceCountable using IMPLEMENT_IREFERENCECOUNTABLE
// or IMPLEMENTS_IREFERENCECOUNTABLE_MT. 
#define NONZERO_REFCOUNT_DESTRUCTOR_WARNING(cname) \
  if (m_refCount != 0) \
  { \
     QTV_MSG_PRIO1(QTVDIAG_DEBUG, QTVDIAG_PRIO_ERROR, \
        #cname \
        ": Deleting object with ref count = %d!", m_refCount); \
  }

#endif /* _I_REFERENCE_COUNTABLE_ */
