#ifndef AEE64STRUCTS_H
#define AEE64STRUCTS_H
/*===========================================================================
FILE:  AEE64structs.h

SERVICES: 64 bit definitions and conversions.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for 64 bit data types.

Copyright © 2006-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        INCLUDES
===========================================================================*/

#include "AEEStdDef.h"

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

typedef struct {
  uint32 a[2]; /* members should not be accessed directly */
} uint64struct;

typedef struct {
  uint32 a[2]; /* members should not be accessed directly */
} int64struct;

typedef struct {
  uint32 a[2]; /* members should not be accessed directly */
} doublestruct;

static __inline uint64 uint64struct_to_uint64(uint64struct us) {
  union {
     uint64 u;
     uint64struct us;
  } uus;

  uus.us = us;
  return uus.u;
}

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

static __inline uint64struct uint64struct_from_uint64(uint64 u) {
  union {
     uint64 u;
     uint64struct us;
  } uus;

  uus.u = u;
  return uus.us;
}

static __inline int64 int64struct_to_int64(int64struct is) {
  union {
     int64 i;
     int64struct is;
  } iis;

  iis.is = is;
  return iis.i;
}

static __inline int64struct int64struct_from_int64(int64 i) {
  union {
     int64 i;
     int64struct is;
  } iis;

  iis.i = i;
  return iis.is;
}

static __inline double doublestruct_to_double(doublestruct ds) {
  union {
     double d;
     doublestruct ds;
  } dds;

  dds.ds = ds;
  return dds.d;
}

static __inline doublestruct doublestruct_from_double(double d) {
  union {
     double d;
     doublestruct ds;
  } dds;

  dds.d = d;
  return dds.ds;
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

uint64struct

Description:
   This structure serves as an intermediary when passing uint64 data
   types through an interface.

Definition:

   typedef struct {
      uint32 a[2];
   } uint64struct;

Members:

   a[2] : Contents of uint64


Comments:
   The members of this structure must never be modified directly.  
   Instead, one of the conversion functions should be used to move
   data into and out of this structure.

See Also:
   uint64struct_to_uint64()
   uint64struct_from_uint64()
   
=======================================================================

int64struct

Description:
   This structure serves as an intermediary when passing int64 data
   types through an interface.

Definition:

   typedef struct {
     uint32 a[2];
   } int64struct;

Members:

   a[2] : Contents of int64


Comments:
   The members of this structure must never be modified directly.  
   Instead one of the conversion functions should be used to move
   data into and out of this structure.

See Also:
   int64struct_to_int64()
   int64struct_from_int64()
   
=======================================================================
   
   doublestruct

Description:
   This structure serves as an intermediary when passing double data
   types through an interface.

Definition:

   typedef struct {
      uint32 a[2];
   } doublestruct;

Members:

   a[2] : Contents of double


Comments:
   The members of this structure must never be modified directly.  
   Instead one of the conversion functions should be used to move
   data into and out of this structure.

See Also:
   doublestruct_to_double()
   doublestruct_from_double()

=======================================================================
MACROS DOCUMENTATION
=======================================================================

uint64struct_to_uint64()

Description:
   Converts from uint64struct to uint64.

Prototype:
    uint64 uint64struct_to_uint64(uint64struct us)

Parameters:
   us : Value to be converted

Return Value:
    Converted uint64

Comments:
    None

Side Effects:
    None


   

See Also:
   uint64struct
   uint64struct_from_uint64()
    
=======================================================================

uint64struct_from_uint64()

Description:
   Converts from uint64 to uint64struct.

Prototype:
    uint64struct uint64struct_from_uint64(uint64 u)

Parameters:
    u : Value to be converted

Return Value:
    Converted uint64struct

Comments:
    None

Side Effects:
    None


   

See Also:
   uint64struct
   uint64struct_to_uint64()
=======================================================================

int64struct_to_int64()

Description:
   Converts from int64struct to int64.

Prototype:
    int64 int64struct_to_int64(int64struct is)

Parameters:
   is : Value to be converted

Return Value:
    Converted int64

Comments:
    None

Side Effects:
    None


   

See Also:
   int64struct
   int64struct_from_int64()
    
=======================================================================

int64struct_from_int64()

Description:
   Converts from int64 to int64struct.

Prototype:
    int64struct int64struct_from_int64(int64 i)

Parameters:
    i : Value to be converted

Return Value:
    Converted int64struct

Comments:
    None

Side Effects:
    None


   

See Also:
   int64struct
   int64struct_to_int64()
=======================================================================

doublestruct_to_double()

Description:
   Converts from doublestruct to double.

Prototype:
    double doublestruct_to_double(doublestruct ds)

Parameters:
   ds : Value to be converted

Return Value:
    Converted double

Comments:
    None

Side Effects:
    None


   

See Also:
   doublestruct
   doublestruct_from_double()
    
=======================================================================

doublestruct_from_double()

Description:
   Converts from double to doublestruct.

Prototype:
    doublestruct doublestruct_from_double(double d)

Parameters:
    d : Value to be converted

Return Value:
    Converted doublestruct

Comments:
    None

Side Effects:
    None


   

See Also:
   doublestruct
   doublestruct_to_double()
=====================================================================*/

#endif // AEE64STRUCTS_H

