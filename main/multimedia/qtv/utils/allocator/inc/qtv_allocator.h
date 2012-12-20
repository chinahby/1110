#ifndef QTV_ALLOCATOR_H
#define QTV_ALLOCATOR_H
/* ===========================================================================

 @file QTV_Allocator.h

 @brief Definition of allocator classes which allow STL to use QTV-defined
 virtual memory management schemes.

 Copyright 2007 QUALCOMM Incorporated, All Rights Reserved.

 ===========================================================================*/

/*============================================================================
                             Edit History

 $Header: //source/qcom/qct/multimedia/qtv/utils/allocator/main/latest/inc/qtv_allocator.h#5 $
 $DateTime: 2009/03/18 21:44:35 $
 $Change: 866593 $

 ===========================================================================*/

// ============================================================================
//
//                   INCLUDE FILES FOR MODULE
//
// ============================================================================
#include <stddef.h>
#include "qtvInternalDefs.h"
#include "qtvsystem.h"
#ifndef UINT_MAX
#define UINT_MAX  0xffffffffU
#endif

// ============================================================================
//
//                      DATA DECLARATIONS
//
// ============================================================================

// ----------------------------------------------------------------------------
// Constant / Define Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Type Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Global Constant Data Declarations
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------

// ============================================================================
//                        Macro Definitions
// ============================================================================

// ============================================================================
//                        Class & Function Declarations
// ============================================================================

// Simple QTV Allocator, uses QTV_New and QTV_Delete
//
// As with all allocators, users of this class may be disappointed by the
// seperation of construction and allocation, but I beg you, resist the urge
// to add 'combination' methods to this interface!  It is complete and minimal
// as it stands.
//
// A shameless cut & paste in which Scott Meyers discusses complete & minimal:
//
// Bill Venners: One guideline in Effective C++ is, "Strive for interfaces
// that are minimal and complete." How do you know when an interface i
// s complete? How do you determine what to leave in and what to leave out?
//
// Scott Meyers: A class is a manifestation of some concept. You have something
// in mind. You want to let people talk about that something. You have some
// idea of the operations that make sense. You have some idea of what people
// might reasonably want to do.
//   The class is complete when people can do everything that you as a designer
// can envision they might reasonably want to do, though they don't necessarily
// have to be able to do it in a convenient fashion. If you want to make things
// convenient, add some non-member functions that wrap a bunch of calls to
// member functions. The class is complete, however, when clients can do
// everything you envision people might reasonably want to do.
//
// Bill Venners: But what if I can envision 1000 things?
//
// Scott Meyers: And they're all reasonable?
//
// Bill Venners: Yeah. Or, maybe they aren't but I think they are.
// How do I know if 500 of those things really aren't necessary?
// How do I decide what is necessary?
//
// Scott Meyers: Necessary is easier than reasonable. A member function is
// unnecessary if it can be implemented in terms of the other member functions.
// You can provide unlimited functionality. I don't care about that.
// The question is whether or not it goes inside the class. If you have a
// member function that can be implemented in terms of other member functions,
// it is not necessary.
//   A minimal interface throws out all the unnecessary functions. The
// unnecessary functions might simply become non-member functions. In that case,
// clients can still use the functions, they just don't call them with member
// function syntax.
//   It's difficult for me to envision a class that has even 100 member
// functions, none of which are redundant. For example, one of the more
// embarrassing classes in the standard C++ library is the String class.
// The String class was designed by committee, and it shows. String has
// about 113 member function names, including overloading. I say member
// function names because many of them are member function templates, which
// means the number of member functions is literally unlimited. You could
// easily trim that down to 25 names, move everything else outside the class,
// and not lose any functionality. That's just a matter of a design that
// went completely crazy.
//   If you can come up with more than say 25 or 30 member functions, that
// strongly suggests you have probably merged more than one concept into a
// single class. You should probably think about splitting that class
// into pieces.
template <typename T>
class QTV_Allocator
{
public:
   // types & constants ------------------------------------------------------

   // Allocators do not follow IPA naming conventions.  It must be this way
   // in order for STL to find what it needs.
   typedef size_t    size_type;
   typedef ptrdiff_t difference_type;
   typedef T*        pointer;
   typedef const T*  const_pointer;
   typedef T&        reference;
   typedef const T&  const_reference;
   typedef T         value_type;

   // STL uses rebind structs to produce compatible types from a given type.
   // For instance, if it's given 'allocator<int> a' and needs an allocator<float>,
   // it will create one with the declaration
   //    'a.rebind<float>::other b'
   template <typename U>
   struct rebind
   {
      typedef QTV_Allocator<U> other;
   };

   // construction & destruction ---------------------------------------------

   // Allocators are expected to be stateless.  This implies only two methods
   // of construction:  default and copy.
   QTV_Allocator( void );
   template <typename U> QTV_Allocator( const QTV_Allocator<U>& );

   ~QTV_Allocator( void );

   // As with everything in STL, copy assignment is mandatory.
   template <typename U> QTV_Allocator& operator=( const QTV_Allocator<U>& );

   // BEGIN public API -------------------------------------------------------

   // Allocators must provide the 'address' operator in lieu of unary &.
   pointer address(reference r) const;
   const_pointer address(const_reference r) const;

   // Allocate memory.
   //
   // This should be:
   // template <class U> pointer allocate(size_type n,
   //                                     allocator<U>::const_pointer hint = 0);
   // but ADS120 is not smart enough to figure out the nested type.
   pointer allocate(size_type n, const void* hint = 0);

   // Releases memory.
   void deallocate(pointer p, size_type n);

   // Returns the largest number of bytes this allocator could allocate.
   size_type max_size(void);

   // Constructs an object in provided memory.
   //
   // In STL, allocation and construction are seperate events.  Construction
   // is always to be done by copy semantics, so be warned.
   void construct(pointer p, const_reference val);

   // Destroys an object.
   // Just like construction, destruction and deallocation are seperate.
   void destroy(pointer p);

   // STL demands that allocators be equality testable.
   bool operator==( const QTV_Allocator& ) const;
   bool operator!=( const QTV_Allocator& ) const;

   // END public API -------------------------------------------------------
private:
   // types & constants ------------------------------------------------------

   // member data ------------------------------------------------------------

   // methods ----------------------------------------------------------------
};

// Because the compiler does not support export templates (RVCT Compiler and
// Libraries Guide, C.3), the function definitions must be in scope wherever
// they are used.  I do not want to produce a thousand-line header, though,
// so I am instead #including the c file here.


// ============================================================================
//                        Class & Function Definitions
// ============================================================================

/// ---------------------------------------------------------------------------
/// Default construction.
///
/// Since QTV_Allocator is stateless, the create/destroy/copy functions
/// are going to be dull.
/// ---------------------------------------------------------------------------
template <typename T>
QTV_Allocator<T>::QTV_Allocator( void )
{
}

/// ---------------------------------------------------------------------------
/// Copy construction.
/// ---------------------------------------------------------------------------
template <typename T> template <typename U>
QTV_Allocator<T>::QTV_Allocator( const QTV_Allocator<U>& )
{
}

/// ---------------------------------------------------------------------------
/// Destruction.
/// ---------------------------------------------------------------------------
template <typename T>
QTV_Allocator<T>::~QTV_Allocator( void )
{
}

/// ---------------------------------------------------------------------------
/// Assignment.
/// ---------------------------------------------------------------------------
template <typename T> template <typename U>
QTV_Allocator<T>& QTV_Allocator<T>::operator=( const QTV_Allocator<U>& )
{
   return *this;
}

// ----------------------------------------------------------------------------
// Address-of operator
//
// @param[in] r The reference.
// @return the address of the referenced item.
// ----------------------------------------------------------------------------
template <typename T>
#ifdef PLATFORM_LTK
#error code not present
#else
QTV_Allocator<T>::pointer QTV_Allocator<T>::address(reference r) const
#endif
{
   return &r;
}

// ----------------------------------------------------------------------------
// Address-of operator
//
// @param[in] r The reference.
// @return the address of the referenced item.
// ----------------------------------------------------------------------------
template <typename T>
#ifdef PLATFORM_LTK
#error code not present
#else
QTV_Allocator<T>::const_pointer QTV_Allocator<T>::address(const_reference r) const
#endif
{
   return &r;
}

// ----------------------------------------------------------------------------
// Allocate memory.
//
// This should be:
// template <class U> pointer allocate(size_type n,
//                                     allocator<U>::const_pointer hint = 0);
// but ADS120 is not smart enough to figure out the nested type.
//
// @param[in] n The number of items to allocate.
// @param[in] hint A pointer 'hint' to the allocator, to be interpreted as
//                 saying 'I want the memory allocated the same way THIS
//                 memory was.'  We ignore hints.
// @return A pointer to the new memory.
// ----------------------------------------------------------------------------
template <typename T>
#ifdef PLATFORM_LTK
#error code not present
#else
QTV_Allocator<T>::pointer QTV_Allocator<T>::allocate( size_type n, const void* /* hint */ )
#endif
{
   return ( pointer )QTV_Malloc( n * sizeof( T ) );
}

// ----------------------------------------------------------------------------
// Releases memory.
//
// @param[in] p Pointer to the memory to be freed.
// @param[in] n The number of objects in the referenced memory, from which
//              the size of the memory block can be determined.
// ----------------------------------------------------------------------------
template <typename T>
void QTV_Allocator<T>::deallocate( pointer p, size_type /* n */ )
{
   QTV_Free( p );
}

// ----------------------------------------------------------------------------
// Returns the largest number of bytes this allocator could allocate.
// ----------------------------------------------------------------------------
template <typename T>
#ifdef PLATFORM_LTK
#error code not present
#else
QTV_Allocator<T>::size_type QTV_Allocator<T>::max_size(  void )
#endif
{
   return UINT_MAX;
}

// ----------------------------------------------------------------------------
// Constructs an object in provided memory.
//
// In STL, allocation and construction are seperate events.  Construction
// is always to be done by copy semantics, so be warned.
//
// @param[in] p The pointer in which construction is to take place.
// @param[in] val The value to be copied.
// ----------------------------------------------------------------------------
template <typename T>
void QTV_Allocator<T>::construct( pointer p, const_reference val )
{
   ::new (p) T( val );
}

// ----------------------------------------------------------------------------
// Destroys an object.
//
// Just like construction, destruction and deallocation are seperate.
//
// @param[in] p The object to be destroyed.
// ----------------------------------------------------------------------------
template <typename T>
void QTV_Allocator<T>::destroy( pointer p )
{
   p->T::~T();
}

// ----------------------------------------------------------------------------
// Equality operator.
// ----------------------------------------------------------------------------
template <typename T>
bool QTV_Allocator<T>::operator==( const QTV_Allocator& ) const
{
   // Since allocators are stateless, being of the same type is enough
   // to match.
   return true;
}

// ----------------------------------------------------------------------------
// Inequality operator.
// ----------------------------------------------------------------------------
template <typename T>
bool QTV_Allocator<T>::operator!=( const QTV_Allocator& ) const
{
   return false;
}

#endif // QTV_ALLOCATOR_H

