/*===========================================================================

                   P R E F E R R E D   U S E R   Z O N E   L I S T


DESCRIPTION
  This file makes up the PUZL component/module of the SD.

  This file implements the I-683B PUZL and encapsulates all the functionality
  required for SD.

  EXTERNALIZED FUNCTIONS

   puzl_init()
   puzl_find_record_with_sidnid()
   puzl_extract_uz_record()

INITIALIZATION AND SEQUENCING REQUIREMENTS
 SD20 module calls puzl_init() upon nam selection before calling other
 extern functions of PUZL.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/puzl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/19/07   ka      Changes to support 16k PRL
05/09/06   jqi     Lint fixes.
05/04/06   jqi     Lint fixes.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/29/01   SJ      Initial release.

===========================================================================*/

/* ^L<EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */

#ifdef FEATURE_USERZONE
#include "msg.h"
#include "err.h"
#include "bit.h"
#include "nv.h"
#include "db.h"
#include "puzl.h"      /* Definitions for PUZL module */
#include "sddbg.h"
#include "sdi.h"       /* Includes bit unpacking macros */




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if ( (!defined(FEATURE_SD20))   || \
      (!defined(FEATURE_IS2000)) || \
      (!defined(FEATURE_EFS))       \
    )
#error  "User Zone implementation requires SD20, IS2000 & EFS features \
         to be defined"
#endif

/* ^L<EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Define for offset to first uz record in PUZL table
*/
#define PUZLI_FIRST_UZ_RECORD_OFFSET   ((PUZLI_RESERVED_1_LEN) + \
                                        (PUZLI_UZ_TEMP_SUBSC_LEN) +\
                                        (PUZLI_UZ_ORIG_ONLY_LEN) +\
                                        (PUZLI_NUM_UZ_ENTRIES_LEN))

/* Define for invalid values
*/
#define PUZLI_INVLD_OFFSET             ((word)(-1))

/* PUZL value for a wildcard SID
*/
#define PUZLI_WILDCARD_SID             0


/* PUZL value for a wildcard NID
*/
#define PUZLI_WILDCARD_NID             65535

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type to hold information pertaining to the PUZL module.
*/
typedef struct {
  /* Indicates if puzl_init() was successfull, if not
  ** init_err_code has the error reason.
  */
  puzl_error_e_type                init_err_code;

  /* Number of user zone records currently in the PUZL.
  */
  int4                             num_uz_entries;


  /* Bit offset for the next user zone entry after the curr_uz_rec_index.
  */
  puzl_offset_t_type              next_uz_rec_offset;

  /* PUZL RAM image.
  **
  ** The PUZL list is read from NV/EFS and stored here in its bit-packed
  ** format. ** There is one PUZL list per NAM, but only one PUZL list at a
  ** time is in RAM because they are large.
  */
  byte                             puzl[NV_PUZL_MAX_SIZE];

  /* Extracted temporary subscription value for the MS. This indicates
  ** whether or not the MS is permitted to access user zones that offer
  ** temporary subscription.
  ** I-683B specifications  3GPP2 C.S0016-A, Section 3.5.7.
  ** UNUSED FIELD
  */
  puzl_uz_temp_subsc_e_type        uz_temp_subsc;


  /* Extracted for user zone origination type value, this indicates
  ** whether or not the MS is permitted to originate calls only when within
  ** the serving area of currently assigned user zone.
  ** I-683B specifications 3GPP2 C.S0016-A, Section 3.5.7
  ** UNUSED FIELD
  */
  puzl_uz_orig_only_e_type         uz_orig_only;

  /* Current index for which curr_uz_record is valid.
  */
  int4                             curr_uz_rec_index;

  /* Current extracted user zone record, valid only if
  ** curr_uz_rec_index is valid.
  */
  puzl_uz_record_s_type            curr_uz_record;

  /* Pointer to a NV Read callback function. PUZL calls on
  ** this function when it needs to read a NV Item
  */
  puzl_nv_read_f_type             *nv_read_cb_func;

  /* Pointer to a NV PUZL read callback function, called
  ** when PUZL needs to read the puzl from NV
  */
  puzl_read_f_type                *puzl_read_cb_func;

} puzli_info_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*  Local state information of the PUZL module.
**  puzli_info.init_err_code is initialized to PUZL_ERROR_NOT_INITIALIZED
*/
puzli_info_s_type puzli_info;

/*===========================================================================
=============================================================================
=============================================================================
================================ LOCAL FUNCTIONS ============================
=============================================================================
=============================================================================
===========================================================================*/

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_traverse_uz_ohp_sid_and_nid_records()

DESCRIPTION
 This function will traverse and validate the OHP sid and nid type item

DEPENDENCIES
  offset_ptr starts after ohp type field.

RETURN VALUE
   PUZL_ERROR_NO_ERR - offset_ptr points to next uz record,
                       uz_rec_ptr is extracted and valid
  !PUZL_ERROR_NO_ERR - offset_ptr and uz_rec_ptr are not valid

SIDE EFFECTS
  None.

===========================================================================*/
static puzl_error_e_type      puzli_traverse_uz_ohp_sid_and_nid_records(

       puzl_offset_t_type    *offset_ptr,
       /* starting offset, should start immediately after ohp type,
       ** after traversing, offset is set to beginning of next
       ** uz record, offset cannot be null.
       */

       puzl_uz_record_s_type  *uz_rec_ptr,
       /* traversed record placed in uz_rec_ptr.
       */

       byte                   *puzl_ptr,
       /* pointer to the PUZL table in the bit-packed form.
       */

       int4                   ohp_rec_index
       /* user zone Record index to be updated if extract was good
       */
)
{
  word n_items = 0;
  int k = 0;
  puzl_offset_t_type offset;
  puzl_uz_type_sp_fields_s_type *uz_type_info_ptr;

  SD_ASSERT( offset_ptr != NULL );
  SD_ASSERT( uz_rec_ptr != NULL );
  SD_ASSERT( puzl_ptr != NULL );

  offset = *offset_ptr;

  uz_type_info_ptr = &uz_rec_ptr->uz_type_info[ohp_rec_index];

  /* Extract the N_ITEMS from sid and nid type
  */
  n_items = SD_B_UNPACKW( puzl_ptr,
                       offset,
                       PUZLI_N_ITEMS_LEN
                     );
  uz_type_info_ptr->uz_ohp.uz_ohp_type_fields.ohp_sid_nid_items \
      .num_uz_ohp_items  = n_items;
  offset += PUZLI_N_ITEMS_LEN;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract each of sid and nid items
  */
  for( k = 0; k < n_items ; k++ )
  {
     uz_type_info_ptr->uz_ohp.uz_ohp_type_fields.ohp_sid_nid_items \
         .uz_ohp_sid_nid_entry[k].nid
                                  = SD_B_UNPACKW( puzl_ptr,
                                               offset,
                                               PUZLI_NID_LEN
                                             );
     offset += PUZLI_NID_LEN;

     uz_type_info_ptr->uz_ohp.uz_ohp_type_fields.ohp_sid_nid_items
         .uz_ohp_sid_nid_entry[k].hysteresis
                                 = SD_B_UNPACKB( puzl_ptr,
                                              offset,
                                              PUZLI_HYSTERESIS_LEN
                                            );
     offset += PUZLI_HYSTERESIS_LEN;
  } /* for ( n_items ) */

  /* Update the offset_ptr to return
  */
  *offset_ptr = offset;
  return PUZL_ERROR_NO_ERR;

} /* puzli_traverse_uz_ohp_sid_and_nid_records()*/


/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_traverse_uz_ohp_records()

DESCRIPTION
  This function will traverse and validate one UZ OHP record

DEPENDENCIES
  offset_ptr starts with the UZ Type Specific Fields.

RETURN VALUE
  PUZL_ERROR_NO_ERR - offset_ptr points to next uz record,
                      uz_rec_ptr is extracted and valid
  !PUZL_ERROR_NO_ERR - offset_ptr and uz_rec_ptr are not valid

SIDE EFFECTS
  None.

===========================================================================*/
static puzl_error_e_type      puzli_traverse_uz_ohp_records(

       puzl_offset_t_type    *offset_ptr,
       /* starting offset, should start from uz record boundary,
       ** after traversing, offset is set to beginning of next
       ** uz record, offset cannot be null.
       */

       puzl_uz_record_s_type  *uz_rec_ptr,
       /* traversed record placed in uz_rec_ptr.
       */

       byte             *puzl_ptr
       /* pointer to the PUZL table in the bit-packed form.
       */

)
{
  int j;
  puzl_uz_type_sp_fields_s_type *uz_type_info_ptr;
  puzl_offset_t_type offset;
  puzl_error_e_type ret_err = PUZL_ERROR_NO_ERR;

  SD_ASSERT( offset_ptr != NULL );
  SD_ASSERT( uz_rec_ptr != NULL );
  SD_ASSERT( puzl_ptr != NULL );

  offset = *offset_ptr;

  /* Extract and validate each of uz subtypes.
  */
  for ( j = 0; j < uz_rec_ptr->num_uz_type_elem; j++ )
  {
    uz_type_info_ptr = &uz_rec_ptr->uz_type_info[j];

    /*  Validate uz subtype field, should be only supported
    **  type.
    */
    uz_type_info_ptr->uz_ohp.uz_ohp_type =
               (puzl_uz_ohp_e_type)SD_B_UNPACKB( puzl_ptr,
                                            offset,
                                            PUZLI_OHP_TYPE_LEN
                                          );
    offset += PUZLI_OHP_TYPE_LEN;

    switch (uz_type_info_ptr->uz_ohp.uz_ohp_type)
    {
      case PUZL_UZ_OHP_SID_AND_NID:
        if ((ret_err =
             puzli_traverse_uz_ohp_sid_and_nid_records( &offset,
                                                        uz_rec_ptr,
                                                        puzl_ptr,
                                                        j
                                                      ))
             != PUZL_ERROR_NO_ERR )
        {
          return ret_err;
        }
        break;

      case PUZL_UZ_OHP_SID_ONLY:
      case PUZL_UZ_OHP_SID_AND_BASE_ID:
      case PUZL_UZ_OHP_BASE_ST_LAT_AND_LONG:
      case PUZL_UZ_OHP_UZ_CENTER_PT_AND_RAD:
      case PUZL_UZ_OHP_MAX:
      default:
        /* Other types not supported currently
        */
        SD_MSG_HIGH( "Invalid ohp type %d, offset %d uz subtype index %d",
                     uz_type_info_ptr->uz_ohp.uz_ohp_type , offset, j );
        return PUZL_ERROR_INVALID_UZ_RECORD;
    }
  } /* for (num_elements) */

  /* Update the offset_ptr to be returned
  */
  *offset_ptr = offset;
  return PUZL_ERROR_NO_ERR;

} /* puzli_traverse_uz_ohp_records() */



/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_traverse_uz_record()

DESCRIPTION
  This function will traverse the user zone record and extract the uz record,
  given the offset. If any of the fields fail the validation, the appropriate
  error is returned, and the current offset_ptr becomes invalid.

DEPENDENCIES
  None.

RETURN VALUE
  PUZL_ERROR_NO_ERR - offset_ptr points to next uz record,
                      uz_rec_ptr is extracted and valid.
  !PUZL_ERROR_NO_ERR - offset_ptr and uz_rec_ptr are not valid.

SIDE EFFECTS
  None.

===========================================================================*/
static puzl_error_e_type      puzli_traverse_uz_record(

       puzl_offset_t_type     *offset_ptr,
       /* starting offset, should start from uz record boundary,
       ** after traversing, offset is set to beginning of next
       ** uz record, offset cannot be null.
       */

       puzl_uz_record_s_type  *uz_rec_ptr,
       /* traversed record placed in uz_rec_ptr.
       */

       byte                   *puzl_ptr
       /* pointer to the PUZL table in the bit-packed form.
       */

)
{
  puzl_offset_t_type offset;
  int i;
  puzl_error_e_type ret_err = PUZL_ERROR_NO_ERR;

  SD_ASSERT( offset_ptr != NULL );
  SD_ASSERT( uz_rec_ptr != NULL );
  SD_ASSERT( puzl_ptr != NULL );

  offset = *offset_ptr;

  /* Extract UZ_PRIORITY field, unused but...
  */
  uz_rec_ptr->uz_priority = SD_B_UNPACKB( puzl_ptr,
                                       offset,
                                       PUZLI_UZ_PRIORITY_LEN
                                     );
  offset += PUZLI_UZ_PRIORITY_LEN;

  /* Extract UZ_ID field.
  */
  uz_rec_ptr->uz_id = SD_B_UNPACKW( puzl_ptr,
                                 offset,
                                 PUZLI_UZ_ID_LEN
                               );
  offset += PUZLI_UZ_ID_LEN;

  /* Extract UZ_SID field.
  */
  uz_rec_ptr->uz_sid = SD_B_UNPACKW( puzl_ptr,
                                  offset,
                                  PUZLI_UZ_SID_LEN
                                );
  offset += PUZLI_UZ_SID_LEN;


  /* Extract user zone name.
  */
  uz_rec_ptr->uz_name.uz_name_encoding =
                       SD_B_UNPACKB( puzl_ptr,
                                  offset,
                                  PUZLI_UZ_NAME_ENCODING_LEN
                                );
  offset += PUZLI_UZ_NAME_ENCODING_LEN;

  uz_rec_ptr->uz_name.uz_name_n_octets =
                       SD_B_UNPACKB( puzl_ptr,
                                  offset,
                                  PUZLI_N_OCTETS_LEN
                                );
  offset += PUZLI_N_OCTETS_LEN;


  /* Extract the user zone name.
  */
  for (i=0; i < uz_rec_ptr->uz_name.uz_name_n_octets ; i++)
  {
    uz_rec_ptr->uz_name.uz_name[i] = SD_B_UNPACKB( puzl_ptr,
                                                offset,
                                                PUZLI_UZ_NAME_OCTET_LEN
                                               );
    offset += PUZLI_UZ_NAME_OCTET_LEN;
  }

  /* Extract the ACTIVE_FLAG field, no validation required.
  */
  uz_rec_ptr->active_flag = (puzl_uz_act_flag_e_type)
                             SD_B_UNPACKB( puzl_ptr,
                                        offset,
                                        PUZLI_ACTIVE_FLAG_LEN
                                      );
  offset += PUZLI_ACTIVE_FLAG_LEN;


  /* Extract PRIORITY_CONTROL field, unused but...
  */
  uz_rec_ptr->priority_control = (puzl_uz_pri_cntrl_e_type)
                                  SD_B_UNPACKB( puzl_ptr,
                                             offset,
                                             PUZLI_PRIORITY_CONTROL_LEN
                                           );
  offset += PUZLI_PRIORITY_CONTROL_LEN;

  /* Extract NOTIFICATION_FLAG field, unused but...
  */
  uz_rec_ptr->notification_flag = (puzl_uz_notif_flag_e_type)
                                   SD_B_UNPACKB( puzl_ptr,
                                              offset,
                                              PUZLI_NOTIFICATION_FLAG_LEN
                                            );
  offset += PUZLI_NOTIFICATION_FLAG_LEN;


  /* Extract UZ_REVISION field, unused but...
  */
  uz_rec_ptr->uz_revision = SD_B_UNPACKB( puzl_ptr,
                                       offset,
                                       PUZLI_UZ_REVISION_LEN
                                     );
  offset += PUZLI_UZ_REVISION_LEN;

  /* Extract and validate UZ_TYPE field.
  */
  uz_rec_ptr->uz_type = (puzl_uz_e_type)
                         SD_B_UNPACKB( puzl_ptr,
                                    offset,
                                    PUZLI_UZ_TYPE_LEN
                                  );
  offset += PUZLI_UZ_TYPE_LEN;

  /* Extract and validate uz subtype length field.
  */
  uz_rec_ptr->num_uz_type_elem = SD_B_UNPACKW( puzl_ptr,
                                            offset,
                                            PUZLI_NUM_ELEMENTS_LEN
                                          );
  offset += PUZLI_NUM_ELEMENTS_LEN;

  switch( uz_rec_ptr->uz_type )
  {
    case PUZL_UZ_MS_SP_RF_COV_BASED_OHP_DET:
      if((ret_err = puzli_traverse_uz_ohp_records(&offset,
                                                    uz_rec_ptr,
                                                    puzl_ptr))
          != PUZL_ERROR_NO_ERR)
      {
        return ret_err;
      }
      break;

    case PUZL_UZ_BRDCST_RF_COV_BASED:
    case PUZL_UZ_BRDCST_GEO_BASED:
    case PUZL_UZ_MS_SP_GEO_BASED:
    case PUZL_UZ_MS_SP_GEO_BASED_SEP_CAR:
    case PUZL_UZ_MAX:
    default:
      /* Other types not supported currently.
      */
      SD_MSG_ERROR( " Invalid uz_type %d, offset %d ", uz_rec_ptr->uz_type,
                      offset, 0 );
      return PUZL_ERROR_UNSUPPORTED_UZ_TYPE;
  }

  /* Each uz record ends with padding of 0-7 bits,
  ** so Move to the next byte boundary,
  ** RESERVED 0-7 bits.
  */
  if ( (offset % 8) != 0 )
  {
    offset += ( 8 - (offset % 8) );
  }

  *offset_ptr = offset;
  return PUZL_ERROR_NO_ERR;

} /* puzli_traverse_uz_record() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_validate()

DESCRIPTION
  This function defined to be static, will be called to validate the PUZL
  table. The following validations will be performed on the PUZL.
     - PUZL has atleast one user zone record
     - Each user zone record has valid uz_type field
     - Each user zone subtype has valid ohp type

DEPENDENCIES
  None.

RETURN VALUE
  PUZL_ERROR_NO_ERR - PUZL was validated without errors
  !PUZL_ERROR_NO_ERR - PUZL validate failed with some error as defined
                       by puzl_error_e_type

SIDE EFFECTS
  None.

===========================================================================*/
static puzl_error_e_type    puzli_validate(

       byte                 *puzl_ptr
       /* pointer to the PUZL table in the bit-packed form.
       */
)
{
   puzl_offset_t_type offset = 0;
   int i;

   SD_ASSERT( puzl_ptr != NULL );

   offset += PUZLI_RESERVED_1_LEN;

   /* Extract the UZ_TEMP_SUBSC field
   */
   puzli_info.uz_temp_subsc = (puzl_uz_temp_subsc_e_type)
                               SD_B_UNPACKB( puzl_ptr,
                                          offset,
                                          PUZLI_UZ_TEMP_SUBSC_LEN
                                        );
   offset += PUZLI_UZ_TEMP_SUBSC_LEN;

   /* Extract the UZ_ORIG_ONLY field
   */
   puzli_info.uz_orig_only =  (puzl_uz_orig_only_e_type)
                               SD_B_UNPACKB( puzl_ptr,
                                          offset,
                                          PUZLI_UZ_ORIG_ONLY_LEN
                                        );
   offset += PUZLI_UZ_ORIG_ONLY_LEN;

   /* Extract the NUM_UZ_ENTRIES.
   */
   puzli_info.num_uz_entries = (int) SD_B_UNPACKW( puzl_ptr,
                                                 offset,
                                                 PUZLI_NUM_UZ_ENTRIES_LEN
                                               );
   offset += PUZLI_NUM_UZ_ENTRIES_LEN;


   if ( puzli_info.num_uz_entries == 0 )
   {
     /* there should be atleast one uz record
     */
     SD_MSG_ERROR( " Num of UZ records = 0 ", 0, 0,0 );
     return PUZL_ERROR_INV_NUM_OF_RECORDS;
   }

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /* Validate each of the user zone record.
   */
   for ( i = 0; i < puzli_info.num_uz_entries; i++ )
   {
     /* using puzli_info.curr_uz_record as scratch buffer
     ** traverse the current uz record checking for errors
     */
     puzli_info.init_err_code =
                      puzli_traverse_uz_record( &offset,
                                                &puzli_info.curr_uz_record,
                                                puzl_ptr
                                              );
     if ( puzli_info.init_err_code != PUZL_ERROR_NO_ERR )
     {
       SD_MSG_ERROR( "puzli_validate() failed in index %d", i, 0, 0);
       return puzli_info.init_err_code;
     }
   } /*  for ( num_uz_entries ) */

   return PUZL_ERROR_NO_ERR;

} /* puzli_validate() */

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_ohp_is_sidnid_match()

DESCRIPTION
  This function defined to be static will be called to perform the match on
  given "sid" and "nid". It does by traversing the "n_items" of the user
  zone type specific "SID and NID" OHP type fields.

  Note: While matching sid or nid, wildcard matching would be done, if sid or
        nid is wildcard.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - a match was found for sid/nid in this uz_record
  FALSE - no match was found

SIDE EFFECTS
  None.

===========================================================================*/
static boolean                      puzli_ohp_is_sidnid_match(

       puzl_uz_record_s_type        *uz_rec_ptr,
       /* pointer to the extract user zone record.
       */

       word                         sid,
       /* sid with which to find the matching user zone record.
       */

       word                         nid,
       /* nid with which to find the matching user zone record.
       */

       boolean                      *is_wildcard_match
       /* Return value, indicates if it was a wildcard match
       */
)
{
  puzl_uz_type_sp_fields_s_type *uz_type_info_ptr;
  int i;
  int j;
  int n_items;


  SD_ASSERT( uz_rec_ptr != NULL );
  SD_ASSERT( is_wildcard_match != NULL );

  /* set wildcard match to FALSE.
  */
  *is_wildcard_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* do sid matching taking care of wildcards.
  */
  if (!(sid == uz_rec_ptr->uz_sid ||
      uz_rec_ptr->uz_sid == PUZLI_WILDCARD_SID))
  {
      /* SID match failed, return.
      */
      return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SID match passed, check for matching nids.
  */
  for (i=0;i< uz_rec_ptr->num_uz_type_elem; i++)
  {
    uz_type_info_ptr = &uz_rec_ptr->uz_type_info[i];

    if ( uz_type_info_ptr->uz_ohp.uz_ohp_type != PUZL_UZ_OHP_SID_AND_NID )
    {
      /* Skip if it is not PUZL_UZ_OHP_SID_AND_NID
      */
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Check for NID match
    */
    n_items = uz_type_info_ptr->uz_ohp.uz_ohp_type_fields. \
         ohp_sid_nid_items.num_uz_ohp_items;

    for (j=0; j < n_items ; j++)
    {
      /* Do wildcard nid matching.
      */
      if (nid ==  uz_type_info_ptr->uz_ohp.uz_ohp_type_fields. \
            ohp_sid_nid_items.uz_ohp_sid_nid_entry[j].nid ||
          uz_type_info_ptr->uz_ohp.uz_ohp_type_fields.ohp_sid_nid_items. \
             uz_ohp_sid_nid_entry[j].nid
              == PUZLI_WILDCARD_NID)
      {
        /* Nid match occured.
        */
        if (uz_rec_ptr->uz_sid == PUZLI_WILDCARD_SID ||
            uz_type_info_ptr->uz_ohp.uz_ohp_type_fields.ohp_sid_nid_items. \
               uz_ohp_sid_nid_entry[j].nid
                   == PUZLI_WILDCARD_NID
           )
        {
          /* It was a wildcard match.
          */
          *is_wildcard_match = TRUE;
        }
        return TRUE;
      }
    }
  }
    /* No nid matches found.
    */
  return FALSE;
} /* puzli_ohp_is_sidnid_match() */


/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzli_read()

DESCRIPTION
  This function defined to be static will be called to read the PUZL from
  the NVRAM. If the read was successful, it initializes the puzl in puzli_info.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - NVRAM read of PUZL was successful
  FALSE - NVRAM read of PUZL failed.

SIDE EFFECTS
  puzl in puzli_info initialized, if NVRAM read suceeded.

===========================================================================*/
static boolean              puzli_read(

       byte                 nam,
       /* NAM for which to read the PUZL.
       */

       byte                 *puzl_buf_ptr,
       /* Pointer to the PUZL table buffer in RAM
       */

       int32                puzl_buf_size
       /* Size of the PUZL table buffer in RAM
       */

)
{
  int32  size = puzl_buf_size;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read into RAM the PUZL that is associated with the specified NAM. If read
  ** operation fails, return FALSE.
  */
  if( puzli_info.puzl_read_cb_func == NULL ||
      !puzli_info.puzl_read_cb_func(nam, puzl_buf_ptr, &size ) )
  {
    SD_MSG_HIGH( "PUZL read failed!, NAM=%d",nam,0,0 );
    return FALSE;
  }

  /* Check if PUZL table size is invalid.
  */
  if (size > puzl_buf_size )
  {
    SD_MSG_HIGH(" Invalid PUZL size %d", size, 0, 0);
    return FALSE;
  }
  return TRUE;

} /* puzli_read() */

/* ^L<EJECT> */
/*---------------------------------------------------------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
-------------------------------- EXTERN FUNCTIONS ---------------------------
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_init()

DESCRIPTION
  This is the first function to be called on PUZL module, before any other
  can be called. This function reads the PUZL from NVRAM and validates it.

DEPENDENCIES
  None.

RETURN VALUE
  return indicates whether the PUZL module initialization was successful
  or the cause for failure.

SIDE EFFECTS
  None.
===========================================================================*/
puzl_error_e_type      puzl_init(

       byte                   nam,
         /* NAM with which to initialize the PUZL component.
         */

       puzl_nv_read_f_type    *nv_read_cb_func,
         /* Pointer to a NV Read callback function.
         */

       puzl_read_f_type       *puzl_read_cb_func
         /* Pointer to a NV PUZL read callback function.
         */
)
{
  nv_item_type              buf;
  nv_item_type              *item_ptr               = (nv_item_type*) &buf;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the state variables in puzli_info{}.
  */
  puzli_info.init_err_code      = PUZL_ERROR_NOT_INITIALIZED;
  puzli_info.num_uz_entries     = 0;
  puzli_info.curr_uz_rec_index  = PUZL_INVLD_UZ_ENTRY_INDEX;
  puzli_info.next_uz_rec_offset = PUZLI_INVLD_OFFSET;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the callback functions
  */
  puzli_info.nv_read_cb_func    = nv_read_cb_func;
  puzli_info.puzl_read_cb_func  = puzl_read_cb_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether PUZL is disabled.
  */
  item_ptr->puzl_enabled.nam = nam;

  if( (puzli_info.nv_read_cb_func == NULL) ||
      (!puzli_info.nv_read_cb_func(NV_PUZL_ENABLED_I, item_ptr)) ||
      (!item_ptr->puzl_enabled.enabled ))
  {
    puzli_info.init_err_code =  PUZL_ERROR_NOT_ENABLED;
    SD_MSG_HIGH( " PUZL not enabled or NV read cb func not registered",
                   0, 0, 0 );
    return puzli_info.init_err_code;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the PUZL from NV.
  */

  if ( !puzli_read( nam, puzli_info.puzl, (int32)sizeof( puzli_info.puzl )  ) )
  {
    /* PUZL read failed, return error.
    */
    puzli_info.init_err_code = PUZL_ERROR_NVRAM_READ_FAILED;
    SD_MSG_HIGH(" PUZL NVRAM read failed ", 0, 0, 0 );
    return puzli_info.init_err_code;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate the PUZL to ensure no errors, puzli_validate() will
  ** initialize the state variables in puzli_info{}.
  */

  puzli_info.init_err_code = puzli_validate( (byte *)puzli_info.puzl );

  SD_MSG_LOW(" puzli_validate() returned %d" , puzli_info.init_err_code, 0, 0);

  return puzli_info.init_err_code;

} /* puzl_init() */

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_get_init_err_code

DESCRIPTION
  This function returns the initialization error code

DEPENDENCIES
  None.

RETURN VALUE
  PUZL_ERROR_NO_ERR => PUZL successfully initialized
  !PUZL_ERROR_NO_ERR => PUZL init failed

SIDE EFFECTS
  None.

===========================================================================*/
puzl_error_e_type             puzl_get_init_err_code(
        void
)
{
  return puzli_info.init_err_code;
}


/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_find_record_with_sidnid()

DESCRIPTION
  This extern function finds matching user zone record given the sid and nid.

DEPENDENCIES
  puzl_find_record_with_sidnid() & puzl_extract_uz_record() are not re-entrant,
  so call these functions from the same thread only.

RETURN VALUE
  PUZL_INVLD_UZ_ENTRY_INDEX if no match found,
  Any other value - match was found.

SIDE EFFECTS
  None.
===========================================================================*/
int4          puzl_find_record_with_sidnid(

       word                 sid,
       /* Sid for which user zone record is to be searched.
       */

       word                 nid
       /* Nid for which user zone record is to be searched.
       */
)
{
  puzl_uz_record_s_type *uz_rec_ptr = &puzli_info.curr_uz_record;
  boolean wildcard_match = FALSE;
  int i;
  int4 wildcard_match_index = PUZL_INVLD_UZ_ENTRY_INDEX;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* perform precond checks.
  */
  if ( puzli_info.init_err_code != PUZL_ERROR_NO_ERR )
  {
    SD_MSG_LOW(" PUZL module not initialized successfully", 0, 0, 0);
    return PUZL_INVLD_UZ_ENTRY_INDEX;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for ( i = 0; i < puzli_info.num_uz_entries; i++ )
  {
    if ( puzl_extract_uz_record( i ) == NULL )
    {
       SD_MSG_LOW( " puzl_extract_uz_record() failing \
                      for valid index %d, unrecoverable ",
                      i, 0, 0);
       return PUZL_INVLD_UZ_ENTRY_INDEX;
    }

    if ( uz_rec_ptr->uz_type == PUZL_UZ_MS_SP_RF_COV_BASED_OHP_DET &&
        puzli_ohp_is_sidnid_match( uz_rec_ptr,
                                    sid,
                                    nid,
                                    &wildcard_match
                                  ))
    {
      if ( !wildcard_match )
      {
        /* Explicit Match found return the uz entry index.
        */
        SD_MSG_LOW( "Explicit Match found for sid %d nid %d at index %i ",
                    sid, nid, i
                  );
        return i;
      }
      /* It was wildcard match, store the index,
      ** may be used if no explicit match is found.
      */
      wildcard_match_index = i;
    }
  } /* for ( num_uz_entries ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there was a wildcard match
  */
  if ( wildcard_match_index != PUZL_INVLD_UZ_ENTRY_INDEX )
  {
    SD_MSG_LOW( "wildcard Match found for sid %d nid %d at index %i ",
                  sid, nid, i
                );
    return wildcard_match_index;
  }

  SD_MSG_LOW( "Match not found for sid %d nid %d ",
              sid, nid, 0);
  return PUZL_INVLD_UZ_ENTRY_INDEX;

} /* puzl_find_record_with_sidnid() */

/* ^L<EJECT> */
/*===========================================================================

FUNCTION
  puzl_extract_uz_record()

DESCRIPTION
  This extern function extracts the user zone record given the entry index.

DEPENDENCIES
  puzl_find_record_with_sidnid() & puzl_extract_uz_record() are not re-entrant,
  so call these functions from the same thread only.

RETURN VALUE
  !NULL - extract was successful
  == NULL - extract failed

SIDE EFFECTS
  None.
===========================================================================*/
puzl_uz_record_s_type*         puzl_extract_uz_record(

       int4                    index
       /* user zone entry index whose record has to be retrieved.
       */
)
{
  puzl_offset_t_type offset    = puzli_info.next_uz_rec_offset;
  int4 uz_index  = puzli_info.curr_uz_rec_index;
  puzl_error_e_type err_code = PUZL_ERROR_NO_ERR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* perform precond checks.
  */
  if ( puzli_info.init_err_code != PUZL_ERROR_NO_ERR )
  {
    SD_MSG_LOW("PUZL module not initialized successfully", 0, 0, 0);
    return NULL;
  }


  /* precond: index should be valid.
  */
  if ( index >= puzli_info.num_uz_entries  ||
       index < 0 )
  {
    SD_MSG_LOW("Invalid index passed in", 0, 0, 0);
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check if it is already extracted.
  */
  if ( uz_index != PUZL_INVLD_UZ_ENTRY_INDEX &&
       index == uz_index )
  {
    /* already extracted, return true.
    */
    SD_MSG_LOW(" Uz rec already extracted", 0,0,0);
    return &puzli_info.curr_uz_record;
  }

  if ( index < uz_index  ||
       uz_index == PUZL_INVLD_UZ_ENTRY_INDEX )
  {
    /* reset the current offset and uz_entry_index to first entry.
    */
    uz_index = 0;
    offset = PUZLI_FIRST_UZ_RECORD_OFFSET;
    err_code = puzli_traverse_uz_record( &offset,
                                          &puzli_info.curr_uz_record,
                                          (byte *) puzli_info.puzl
                                         );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( ; err_code == PUZL_ERROR_NO_ERR && uz_index < index ; uz_index++)
  {
    err_code = puzli_traverse_uz_record( &offset,
                                         &puzli_info.curr_uz_record,
                                         (byte *)puzli_info.puzl
                                       );
  }

  if ( err_code != PUZL_ERROR_NO_ERR )
  {
    puzli_info.curr_uz_rec_index = PUZL_INVLD_UZ_ENTRY_INDEX;
    puzli_info.next_uz_rec_offset = PUZLI_INVLD_OFFSET;
    return NULL;
  }

  /* Save the search context.
  */
  puzli_info.curr_uz_rec_index = index;
  puzli_info.next_uz_rec_offset = offset;
  return &puzli_info.curr_uz_record;
} /* puzl_extract_uz_record() */

#else /* FEATURE_USERZONE */
/*lint -esym(765, puzl_dummy) */ /* external could be made static */
/*lint -esym(714, puzl_dummy) */ /* The function is not referenced */
/*===========================================================================
FUNCTION puzl_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word puzl_dummy( word dummy_input )
{
  return dummy_input;
}

#endif /* FEATURE_USERZONE */
