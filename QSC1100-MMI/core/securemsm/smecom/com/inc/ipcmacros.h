#ifndef IPCMACROS_H
#define IPCMACROS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               I P C    M A C R O S    S E R V I C E S
                             
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/ipcmacros.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/06    gr     Fixed a problem with the definition of the STRUCT macro 
12/07/05    ws     Removed semicolons and implemented more review comments
12/02/05    ws     Implemented Review comments
11/29/05    ws     Initial Revision.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>
#include <memory.h>
#include "rtedef.h"

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*===========================================================================
              DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
              DEFINITIONS AND TYPES DECLARATIONS
===========================================================================*/

/*   Type of function that should be passed into CUSTOM
*/
typedef void (ipcmacros_custom_f_type) (
   void             *req,
   uint32            a
);

/*   Type of function that should be passed into RETURN_CUSTOM
*/
typedef void (ipcmacros_return_custom_f_type) (
   uint32            b,   
   void             *rsp
);
  
#define PREP_MSG( req, id, handle )                \
   (req)->msg_id = (ipc_msg_id_type) (id);         \
   (req)->object_handle = (uint32) (handle)

#define UINT8( req, a ) \
   (req)->a = (uint8) (a)

#define UINT16( req, a ) \
   (req)->a = (uint16) (a)

#define UINT32( req, a ) \
   (req)->a = (uint32) (a)

#define INT8( req, a ) \
   (req)->a = (int8) (a)

#define INT16( req, a ) \
   (req)->a = (int16) (a)

#define INT32( req, a ) \
   (req)->a = (int32) (a)

#define LONG( req, a ) \
   (req)->a = (long) (a)

#define ENUM( req, a ) \
   (req)->a = (uint32) (a)

#define BOOL( req, a ) \
   (req)->a = (boolean) (a)

#define CHAR_ARRAY( req, a, size ) \
   (void) ipc_memcpy( (char*) (req)->a, (char*) (a), (uint32)(size) )

#define STRING( req, a ) \
   (void) ipc_strcpy( (char*) (req)->a, (const char*) (a) )

#define CONST_STRING( req, a ) \
   (void) ipc_strcpy( (char*) (req)->a, (const char*) (a) ) 

#define STRUCT_ARRAY( req, a, size ) \
   (void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size)*sizeof( a ) )
   
#define CONST_STRUCT_ARRAY( req, a, size ) \
   (void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size)*sizeof( a ) )

#define POINTER( req, a ) \
   (void) ipc_memcpy( (void*) &((req)->a), (void*) a, (size_t) sizeof(*a))

#define TYPED_POINTER( req, a, size ) \
   (void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size) )

#define CONST_POINTER( req, a ) \
   (void) ipc_memcpy( (void*) &((req)->a), (void*) a, (size_t) sizeof(*a))

#define CONST_VOID_POINTER( req, a, size ) \
   (void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size) )

#define OPAQUE_PTR( req, a ) \
   (req)->a = (uint32) (a)

#define CONST_OPAQUE_PTR( req, a ) \
   (req)->a = (uint32) (a)

#define TYPED_OPAQUE_PTR( req, a, size ) \
   (void) ipc_memcpy( (req)->a, (a), (size) )

/* func should be of type ipcmacros_custom_f_type
*/
#define CUSTOM( req, a, func ) \
   func( (void*) (req), (uint32) (a) )

#define STRUCT( req, a ) \
   (void) ipc_memcpy( (void*) &((req)->a), (void*) (&a), (size_t) sizeof( a ) )

#define VOID( req, a )
#define CALLBACK( req, a )



#define RETURN_BOOL( b, rsp ) \
   (b) = (boolean) (rsp)->b

/* func should be of type ipcmacros_return_custom_f_type
*/
#define RETURN_CUSTOM( b, rsp, func ) \
   func( (uint32) (b), (void*) (rsp) )

#define RETURN_VOID

#define RETURN_UINT8( b, rsp ) \
   (b) = (uint8) (rsp)->b

#define RETURN_UINT16( b, rsp ) \
   (b) = (uint16) (rsp)->b

#define RETURN_UINT32( b, rsp ) \
   (b) = (uint32) (rsp)->b

#define RETURN_INT8( b, rsp ) \
   (b) = (int8) (rsp)->b

#define RETURN_INT16( b, rsp ) \
   (b) = (int16) (rsp)->b

#define RETURN_INT32( b, rsp ) \
   (b) = (int32) (rsp)->b

#define RETURN_ENUM( b, rsp ) \
   (b) = (uint32) (rsp)->b
   
#define RETURN_CHAR_ARRAY( b, rsp, size ) \
   (void) ipc_memcpy( (void*) (b), (void*) (rsp)->b, (size_t) (size) )

#define RETURN_STRING( b, rsp ) \
   (void) ipc_strcpy( (char*) (b), (const char*) (rsp)->b )
  
#define RETURN_POINTER( b, rsp, size ) \
   (void) ipc_memcpy( (void*) (b), (void*) (rsp)->b, (size_t) (size) )

#define RETURN_OPAQUE_PTR( b, rsp ) \
   (b) = (uint32) (rsp)->b

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IPCMACROS_H */
