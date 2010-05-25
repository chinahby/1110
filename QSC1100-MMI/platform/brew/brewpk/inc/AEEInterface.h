#ifndef AEEINTERFACE_H
#define AEEINTERFACE_H /* #ifndef AEEINTERFACE_H */
/*======================================================

FILE:      AEEInterface.h

SERVICES:  Macros and typedefs necessary to declare a
           BREW-style interface

GENERAL DESCRIPTION:

  BREW's COM-style binary interface specification is
  a little cumbersome to craft in C, these macros
  and types make BREW headers a little cleaner.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    AEEINTERFACE, et al.

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

/* the type of an interface's method vtable */
#define AEEVTBL(iname) iname##Vtbl

/* shortcut for declaring an abstract interface */
#define AEEINTERFACE(iname) \
           typedef struct AEEVTBL(iname) AEEVTBL(iname); \
           struct AEEVTBL(iname)

/* macro for retrieving the vtable from an instance of an interface */
#define AEEGETPVTBL(p,iname)  (*((AEEVTBL(iname) **)((void *)p)))

/* Define an AEE interface vtable (Assumes INHERIT_iname exists for iname) */
#define AEEVTBL_DEFINE(iname)\
            AEEINTERFACE(iname) {\
               INHERIT_##iname(iname);\
            }

/* Define an AEE interface (Assumes INHERIT_iname exists for iname) */
#define AEEINTERFACE_DEFINE(iname)\
            typedef struct iname iname;\
            AEEVTBL_DEFINE(iname)

/*
=======================================================================
MACROS DOCUMENTATION
=======================================================================
AEEVTBL()

Description:
    helps declare the standard type of a BREW interface's method
      vtable

Definition:

   #define AEEVTBL(iname) iname##Vtbl

Parameters:
    iname: the interface name for which the vtable type name
            should be generatedl

Evaluation Value:
    the token iname##Vtbl

=======================================================================
AEEINTERFACE()

Description:
    The standard way to declare the type of a new interface. Defines
      an opaque type, and the method vtable's struct header.
      Designed to be used like so:
===pre>
      AEEINTERFACE(IFoo) {
           uint32 (*AddRef)(IFoo*);
           uint32 (*Release)(IFoo*);
           int    (*QueryInterface)(IFoo *, AEECLSID, void **);
      };
===/pre>

Definition:
   #define AEEINTERFACE(iname) \
              typedef struct AEEVTBL(iname) AEEVTBL(iname); \
              struct AEEVTBL(iname)

Parameters:
    iname: the interface name for which the method vtable
           type header should be should be generated

Comments:
   This macro is rarely used by itself, it's more commonly used
     only as part of AEEVTBL_DEFINE().

=======================================================================
AEEVTBL_DEFINE()

Description:
    Uses an INHERIT_iname() macro and AEEINTERFACE() to define a
      method vtable type for an interface.
   Designed to be used like so:
===pre>
      #define INHERIT_IFoo(iname) \
         uint32 (*AddRef)(iname*);\
         uint32 (*Release)(iname*);\
         int    (*QueryInterface)(iname *, AEECLSID, void **)

      AEEVTBL_DEFINE(IFoo);
===/pre>

Definition:
    #define AEEVTBL_DEFINE(iname)\
                AEEINTERFACE(iname) {\
                   INHERIT_##iname(iname);\
                }

Parameters:
    iname: the interface name for which the method vtable
         type should be should be defined

Comments:
  This macro is rarely used by itself.  It is more commonly used as
     part of AEEINTERFACE_DEFINE().
  It can be useful when a compiler (like gcc) generates an error
    on duplicate typedefs, and the object definition needs to be
    separate from the vtable definition.
  This macro assumes the existence of INHERIT_##iname() #define

=======================================================================
AEEINTERFACE_DEFINE()

Description:
   Uses an INHERIT_iname() macro and AEEINTERFACE() to define a
     standard BREW interface.

   Designed to be used like so:
===pre>
      #define INHERIT_IFoo(iname) \
         uint32 (*AddRef)(iname*);\
         uint32 (*Release)(iname*);\
         int    (*QueryInterface)(iname *, AEECLSID, void **)

      AEEINTERFACE_DEFINE(IFoo);
===/pre>

Definition:

   #define AEEINTERFACE_DEFINE(iname)\
               typedef struct iname iname;\
               AEEVTBL_DEFINE(iname)

Parameters:
    iname: the interface name for which the struct and method vtable
          types should be defined.

Comments:
  This macro assumes the existence of INHERIT_##iname() #define

=======================================================================
*/

#endif /* #ifndef AEEINTERFACE_H */
