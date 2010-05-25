/* =======================================================================
 			bool_array.h
DESCRIPTION
  This module defines an array of booleans.
  
EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/boolarray/main/latest/inc/bool_array.h#1 $
$DateTime: 2008/05/12 09:35:19 $
$Change: 657861 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef BOOL_ARRAY_H
#define BOOL_ARRAY_H


/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */


/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  This class defines an array of booleans

DESCRIPTION
  This class stores the information for the telop element.

========================================================================== */
template <uint32 max_array_size> class BoolArray {
public:

/* ======================================================================
FUNCTION:
  BoolArray::BoolArray

DESCRIPTION:
  Creates a new BoolArray object

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
  BoolArray() { size = max_array_size; 
  elements = (max_array_size+31)/32;};

/* ======================================================================
FUNCTION:
  BoolArray::~BoolArray

DESCRIPTION:
  Destroys a BoolArray object

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
  ~BoolArray() {};

/* ======================================================================
FUNCTION:
  BoolArray::set_value

DESCRIPTION:
  Sets the element at index to value

INPUT/OUTPUT PARAMETERS:
  index
  value

RETURN VALUE:
  Returns true if successful, false if there's an error.

SIDE EFFECTS:
  None.
======================================================================*/
  bool set_value(int index, bool value) {

    if (index < 0 || index >= size) {
      return false;
    }

    int array_index = (index)/32;
    int word_index = index % 32;
    uint32 mask = 1 << word_index;

    if (value) {
      array[array_index] |= mask;
    } else {
      array[array_index] &= (~mask);
    }

    return true;
  }


/* ======================================================================
FUNCTION:
  BoolArray::get_value

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  index

RETURN VALUE:
  Returns the value at index.

SIDE EFFECTS:
  None.
======================================================================*/
  bool get_value(int index) const{
    if (index < 0 || index >= size) {
      return false;
    }

    int array_index = (index)/32;
    int word_index = index % 32;
    uint32 mask = 1 << word_index;

    return ((array[array_index] & mask) != 0);
  }

/* ======================================================================
FUNCTION:
  BoolArray::operator[] 

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  index

RETURN VALUE:
  Returns the value at index.

SIDE EFFECTS:
  None.
======================================================================*/

  bool operator[] (int index) const{return get_value(index);};

 private:
  int size;
  int elements;
  uint32 array[(max_array_size+31)/32];

};

#endif
