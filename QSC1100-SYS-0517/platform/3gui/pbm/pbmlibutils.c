/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY UTILS

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM lib (external interface).

  Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmlibutils.c#18 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
11/16/05   cvs     Moved pbm_record_id_to_file_id() to pbmutils.c
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added support for UCS2 encoding.
04/21/05   cvs     Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK

#include "pbmutils.h"
#include "pbmlib.h"

#ifndef FEATURE_WINCE
  #include "tmc.h"
#else
  //FEATURE_WINCE
  #define mem_malloc(heap, size) malloc (size)
  #define mem_free(heap, ptr)    free(ptr)
#endif


/* <EJECT> */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/

#define PBM_NUM_PB_BITS 5
#define PBM_NUM_LOC_BITS 11
#define PBM_PB_MASK    ((1 << PBM_NUM_PB_BITS) - 1)  /* 0x001F */
#define PBM_LOC_MASK   ((1 << PBM_NUM_LOC_BITS) - 1) /* 0x07FF */

/* The phone book's record number uses the top 5 bits
 * to indicate the phonebook.  Below is the mapping.
 * Use PBM_RESERVED for new phonebooks in future */
static pbm_device_type pbm_pb_bits_to_pb_id[] = {
    PBM_EFS,            /* Phonebook bits 00000 */
    PBM_SDN,            /* Phonebook bits 00001 */
    PBM_ADN,            /* Phonebook bits 00010 */
    PBM_SDN2,           /* Phonebook bits 00011 */
    PBM_FDN,            /* Phonebook bits 00100 */
    PBM_MSISDN,         /* Phonebook bits 00101 */
    PBM_ECC,            /* Phonebook bits 00110 */
    PBM_MSISDN2,        /* Phonebook bits 00111 */
    PBM_SDL,            /* Phonebook bits 01000 */
    PBM_MBDN,           /* Phonebook bits 01001 */
    PBM_DIALED,         /* Phonebook bits 01010 */
    PBM_MBDN2,          /* Phonebook bits 01011 */
    PBM_RCVD,           /* Phonebook bits 01100 */
    PBM_MBN,            /* Phonebook bits 01101 */
    PBM_MISS,           /* Phonebook bits 01110 */
    PBM_MBN2,           /* Phonebook bits 01111 */
    PBM_LND,            /* Phonebook bits 10000 */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    PBM_ECC2,           /* Phonebook bits 10001 */
#else
    PBM_RESERVED,       /* Phonebook bits 10001 */
#endif
    PBM_ICI,            /* Phonebook bits 10010 */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    PBM_ECC_OTHER,      /* Phonebook bits 10011 */
#else
    PBM_RESERVED,       /* Phonebook bits 10011 */
#endif
    PBM_OCI,            /* Phonebook bits 10100 */
    PBM_RESERVED,       /* Phonebook bits 10101 */
    PBM_ADN2,           /* Phonebook bits 10110 */
    PBM_RESERVED,       /* Phonebook bits 10111 */
    PBM_FDN2,           /* Phonebook bits 11000 */
    PBM_RESERVED,       /* Phonebook bits 11001 */
    PBM_LND2,           /* Phonebook bits 11010 */
    PBM_RESERVED,       /* Phonebook bits 11011 */
    PBM_ICI2,           /* Phonebook bits 11100 */
    PBM_RESERVED,       /* Phonebook bits 11101 */
    PBM_OCI2,           /* Phonebook bits 11110 */
    PBM_RESERVED        /* Phonebook bits 11111 */
};

/*===========================================================================
                         FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the global heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields(uint8 **fields_buf_ptr,
                            uint32 *fields_buf_size,
                            uint32 num_fields,
                            const uint32 *sizes)
{
  uint32 i;
  uint32 total_size = 0;
  uint32 mem_offset = 0;
  const uint32 size_one_field = ALIGNED_SIZE(sizeof(pbm_field_s_type));
  /*lint -e{740}*/ /* Definately, an unusual pointer cast. */
  pbm_field_s_type **ptyped_fields_buf_ptr = (pbm_field_s_type **)
                                                              fields_buf_ptr;

  PBM_CHECK_PTR_RET(ptyped_fields_buf_ptr, FALSE);
  PBM_CHECK_PTR_RET(sizes, FALSE);
  PBM_CHECK_PTR_RET(fields_buf_size, FALSE);

  *ptyped_fields_buf_ptr = NULL;
  total_size = num_fields * size_one_field;
  for (i = 0; i < num_fields; i++)
  {
    total_size += ALIGNED_SIZE (sizes[i]);
  }

  *fields_buf_size = total_size;

  if (total_size > 0)
  {
    *ptyped_fields_buf_ptr = mem_malloc(&tmc_heap, total_size); //lint !e826 area too small
    if (*ptyped_fields_buf_ptr == NULL)
    {
      return FALSE;
    }

    mem_offset += num_fields * size_one_field;
    for (i = 0; i < num_fields; i++)
    {
      (*ptyped_fields_buf_ptr)[i].buffer_offset = mem_offset;
      mem_offset += ALIGNED_SIZE(sizes[i]);
      mem_offset -= size_one_field; /* take #bytes from next field to its data */
    }
  }
  MSG_MED("PBM allocated from TMC heap %d bytes 0x%x",
          total_size, fields_buf_ptr ? *fields_buf_ptr : 0, 0);
  return TRUE;
}

/*===========================================================================
FUNCTION PBM_FREE_FIELDS

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields( uint8 **fields, int num_fields)
{
  PBM_CHECK_PTR_RET(fields, VOID);

  if ((num_fields > 0) && (*fields != NULL)) {
    MSG_MED("PBM freed from TMC heap 0x%x", *fields, 0, 0);
    mem_free(&tmc_heap,*fields);
  } else {
    PBM_MSG_ERR("PBM not freeing memory, num fields %d, ptr 0x%x", num_fields, *fields, 0);
  }
  *fields = NULL;

}

/*===========================================================================
FUNCTION PBM_GET_FIELD_DATA_PTR

DESCRIPTION
  This function is just a shortcut return the pointer to the data buffer for
  a given field number.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void * pbm_get_field_data_ptr(const void *field_buf,
                              int field_num)
{
  uint8 *ptr;
  uint32 offset;
  pbm_field_s_type *ptyped_field_buf = (pbm_field_s_type*)field_buf;

  PBM_CHECK_PTR_RET(ptyped_field_buf, NULL);

  offset = ptyped_field_buf[field_num].buffer_offset;

  ptr = (uint8*)&(ptyped_field_buf[field_num]) + offset;

  return (void*)ptr;
}

/*===========================================================================
FUNCTION PBM_FILL_FIELD

DESCRIPTION
  This function is just a shortcut to fill in the four different fields
  which make up the field structure.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_fill_field(pbm_field_s_type *field, pbm_field_id_e_type field_id,
     pbm_field_type_e_type field_type, const byte *buffer, uint16 data_len)
{
  void *field_data_ptr;

  PBM_CHECK_PTR_RET(field, VOID);
  PBM_CHECK_PTR_RET(buffer, VOID);

  if (!data_len)
  {
    PBM_MSG_ERR("Invalid data len passed", 0, 0, 0);
    return;
  }

  field->field_id   = field_id;
  field->field_type = field_type;

  field_data_ptr = pbm_get_field_data_ptr(field,0);
  memcpy(field_data_ptr, buffer, data_len);
  field->data_len   = data_len;
}

/*==========================================================================*
 *                      PBM Record ID UTILITIES                             *
 *==========================================================================*/

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_LOCATION

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_location(uint16 record_id)
{
  /* Return lower 11 bits of record ID*/
  return (record_id & PBM_LOC_MASK);
}


/*===========================================================================
FUNCTION PBM_LOCATION_TO_RECORD_ID

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_record_id(pbm_device_type pb_id, int location)
{
  uint16 pb_code = 0;
  uint16 i;

  for (i = 0; i < (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
  {
    if (pbm_pb_bits_to_pb_id[i] == pb_id)
    {
      pb_code = i;
      break;
    }
  }

  if (i >= (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id))
  {
    PBM_MSG_ERR("Invalid pbm_device_type for conversion: %d.", pb_id, 0, 0);
  }

  /* Put pb_id in upper 5 bits and location in lower 11 bits*/
  return ( ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
           ((uint16) location & PBM_LOC_MASK) );
}


/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PHONEBOOK

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_record_id_to_phonebook(uint16 record_id)
{
  int phonebook;
  pbm_device_type retval;

  /* Get upper 5 bits of record ID*/
  phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

  retval = pbm_pb_bits_to_pb_id[phonebook];
  if (retval == PBM_RESERVED)
  {
    PBM_MSG_ERR("Unknown phonebook code %d.", phonebook, 0, 0);
    retval = PBM_EFS;
  }

  return retval;
}

/*===========================================================================
FUNCTION PBM_PB_ID_TO_SIM_ID

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_slot_type pbm_pb_id_to_sim_id(pbm_device_type pb_id)
{
  switch (pb_id)
  {
    case PBM_ADN:
    case PBM_FDN:
    case PBM_LND:
    case PBM_SDN:
    case PBM_MSISDN:
    case PBM_MBN:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_ICI:
    case PBM_OCI:
    case PBM_MBDN:
#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC:
#endif
      return PBM_SLOT_1;
    case PBM_ADN2:
    case PBM_FDN2:
    case PBM_LND2:
    case PBM_SDN2:
    case PBM_MSISDN2:
    case PBM_MBN2:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC2:
#endif
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_ICI2:
    case PBM_OCI2:
    case PBM_MBDN2:
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return PBM_SLOT_2;
    default:
      PBM_MSG_ERR("pb_id_to_sim_id called with %d (an invalid device)", pb_id, 0, 0);
      return PBM_NUM_SLOTS;
  }
}

/*===========================================================================
FUNCTION pbmutils_wstrlen;

DESCRIPTION
  int pbmutils_wstrlen(const uint16 *ucs2_str);

  ucs2_str:   [in]   The UCS2 string to count the characters.

  This function counts the number of UCS2 characters in a string.  The
  string must be terminated by 0x0000.

DEPENDENCIES
  None.

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmutils_wstrlen(const uint16 *ucs2_str)
{
  uint32 retval = 0;                  /* Value to be returned. */
  if (ucs2_str)
    while (*ucs2_str++) { retval++; } /* Increment retval until 0x0000 */
  return retval;                      /* Return what we counted. */
}

#endif /* FEATURE_PBM_TASK */
