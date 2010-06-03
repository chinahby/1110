#ifndef MMGSDIUTIL_H
#define MMGSDIUTIL_H
/*===========================================================================


           M M G S D I   U T I L I T Y   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2009 QUALCOMM, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/mmgsdiutil.h#3 $ $DateTime: 2009/04/13 17:44:33 $ $Author: jsharma $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/12/09   js      Explicit cast to void* for ptr argument in mem_free and malloc
03/19/09   js      Support for Enhanced NAA Refresh by file path
11/19/08   js      KW fix for potential null ptr dereferencing
10/07/08   js      Lint Fixes
09/06/08   kk      Changed pkcs15 command handling
08/28/08   sun     If command fails in the original state, then return original
                   error mmgsdi status to client
08/25/08   js      Fixed compilation warning
08/15/08   js      Klockwork fix for MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR
07/17/08   kk      Added support for PKCS 15
06/12/08   srr     Support of MMGSDI Search functionality
06/23/08   sp      Add additional parameter to functions mmgsdi_util_reset_app_data()
                   mmgsdi_util_reset
06/04/08   tml     Added sw to cnf response header and return
                   MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED when malloc macro failed
03/07/08   sun     Added new function to calculate ber length
11/15/07   sun     Added support for NAA refresh
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
05/01/07   sun     Fixed Compile Warning
04/26/07   sp      Added function to get path from enum
03/26/07   sun     Check if additional files are being read during refresh
                   fcn
03/19/07   sun     Fixed Lint Error
03/05/07   nk      Added PIN Validation function
03/02/07   sun     Added Check for file attributes during read/write
01/1/07    sp      Fixed isues with ThinUI build.
01/11/07   sk      Changed return type for mmgsdi_util_access_path_lookup_table()
01/08/07   sun     Get the AppId from the Client ID for All Pin Operations
12/19/06   tml     Add support for path transparent access
12/11/06   tml     Ensure proper event notification for late registered clients
10/26/06   tml     Get Key ref from client ID
10/25/06   tml     Added function to check if the client is trying to access
                   selected application
10/17/06   sun     Do not wait for Card Selection if NV item is set
09/06/06   tml     Added refresh AID notification support
08/25/06   jar     Added support for FEATURE_MMGSDI_CARD_ERROR_INFO
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/18/06   sun     Added orig_pin_op to mmgsdi_util_build_pin_req
08/07/06   sun     Divert GSDI PIN calls to MMGSDI
07/26/06   sun     Added support for Multi USIM App and MMGSDI PIN
06/29/06   sun     Lint Fixes
06/12/06   tml     Externalize functions for session clean-up
05/23/06   tml     lint
05/16/06   tml     Added utility to see if client reg table is free or nots
04/14/06   tml     Added utility function to initialize cache
04/13/06   jar     Added Support for onchip sim
04/05/06   tml     Add NO FILE ENUM and Card Removed support
02/09/06   tml     Added utility functions for conversion and lint fix
02/14/06   nk      Merged for ARR caching and by path changes
12/14/05   tml     MMGSDI Cleanup
11/03/05   tml     Fixed header
09/29/05   tml     Client Deregistration support
08/29/05   jar     Created a Memory allocation routine to help reduce the code
                   size by switching from the MACROS
08/26/05   sun     Added support for BT SAP
08/17/05   pv      added mmgsdi_util_print_data_to_diag function to print
                   data to the diag interface
07/26/05   sst     Added MMGSDIBT support
07/12/05   tml     Session Management and ISIM support
06/16/05   tml     Prototype change for transparent and record access
06/13/05   tml     changed Msg Error to Msg Medium
02/24/05   sk      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


#include "uim.h"
#include "mmgsdi.h"
#include "gsdi_convert.h"
#include "gsdi.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define MMGSDIUTIL_BYTE_SHIFT                      0x08
#define MMGSDIUTIL_MF_HIGH_BYTE                    0x3F
#define MMGSDIUTIL_WORD_SIZE                       4
/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ===========================================================================
   STRUCTURE:      MMGSDI_UIM_FILE_ENUM_TABLE_TYPE

   DESCRIPTION:
     This structure contains the matching pair of a mmgsdi_file_enum_type and
     uim_items_enum_type.
===========================================================================*/
typedef struct
{
  mmgsdi_file_enum_type   mmgsdi_file;
  uim_items_enum_type     uim_file;
} mmgsdi_uim_file_enum_table_type;
/* Enum Type for different MMGSDI/GSDI queue types */

typedef enum {
  GSDI_HIGH_PRI_Q = 0x0,
  GSDI_CMD_Q      = 0x1,
  MMGSDI_CMD_Q    = 0x2,
  MMGSDI_RSP_Q    = 0x3,
  MMGSDI_NO_Q_SVC = 0x7FFFFFFF
} mmgsdi_q_svc_enum_type;


/* Enum type for protocol */

typedef enum
{
  MMGSDI_NO_PROTOCOL = 0,
  MMGSDI_ICC,
  MMGSDI_UICC,
  MMGSDI_MAX_PROTOCOL_ENUM = 0x7FFFFFFF
} mmgsdi_protocol_enum_type;

/* ==========================================================================
TYPE ENUM:    MMGSDI_CHNL_MGT_ID_ENUM_TYPE

DESCRIPTION:  Used internally to distinguish the different request types in
              general way when using general functions.
============================================================================*/
typedef enum {
  MMGSDI_CHNL_MGT_ID_NONE,
  MMGSDI_CHNL_MGT_ID_SEND_DATA
} mmgsdi_chnl_mgt_id_enum_type;

extern const mmgsdi_uim_file_enum_table_type mmgsdi_uim_file_enum_table[MMGSDI_NO_FILE_ENUM];


/*=============================================================================

                          MMGSDI MACROS
               MACRO Definitions used through MMGSDI

=============================================================================*/


/* ===========================================================================-
   MACRO:       MMGSDIUTIL_DEBUG_MSG_ERR
   DESCRIPTION: When MMGSDIUTIL_DEBUG_MSG_ERR is define, an ERROR_FATAL
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are logged to NV for Further Analysis.
   ===========================================================================*/
#ifdef FEATURE_MMGSDI_DEBUG_ERROR
  #define MMGSDIUTIL_DEBUG_MSG_ERR(str_ptr,a,b,c)        \
    ERR_FATAL("MMGSDI: (" str_ptr ")",(a),(b),(c))
#else
  #define MMGSDIUTIL_DEBUG_MSG_ERR(str_ptr,a,b,c)        \
    ERR("MMGSDI: (" str_ptr ")",(a),(b),(c))
#endif /* FEATURE_MMGSDI_DEBUG_ERROR */

/* ===========================================================================-
   MACRO:       MMGSDIUTIL_DEBUG_MSG_ERROR
   DESCRIPTION: When MMGSDIUTIL_DEBUG_MSG_ERROR is define, an ERROR_FATAL
                is called so that the Errors can be caught and further
                analyzed.
                These Errors are not logged to NV.
   ===========================================================================*/
#ifdef FEATURE_MMGSDI_DEBUG_ERROR
  #define MMGSDIUTIL_DEBUG_MSG_ERROR(str_ptr,a,b,c)            \
    ERR_FATAL("MMGSDI: (" str_ptr ")",(a),(b),(c))
#else
  #define MMGSDIUTIL_DEBUG_MSG_ERROR(str_ptr,a,b,c)            \
    MSG_ERROR("MMGSDI: (" str_ptr ")",(a),(b),(c))
#endif /* FEATURE_MMGSDI_DEBUG_ERROR */


/* ===========================================================================
   MACRO:       MMGSDIUTIL_TMC_MEM_FREE
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will also assign the Pointer to NULL.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_TMC_MEM_FREE) */
#define MMGSDIUTIL_TMC_MEM_FREE(p_mem)                          \
  if ( (p_mem) == NULL )                                        \
  {                                                             \
    MMGSDIUTIL_DEBUG_MSG_ERR("Attempt to free NULL PTR",0,0,0); \
  }                                                             \
  else                                                          \
  {                                                             \
    MSG_MED("Mem Free 0x%x", (p_mem), 0, 0);                    \
    mmgsdi_free((void*)(p_mem));                                \
    (p_mem) = NULL;                                             \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_TMC_MEM_FREE_NULL_OK
   DESCRIPTION: Will Free the TMC Memory and peform the necessary checks for
                NULL.  It will not Error or print out a message if the
                pointer is NULL.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_TMC_MEM_FREE_NULL_OK) */
#define MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(p_mem)               \
  if ( (p_mem) != NULL )                                     \
  {                                                          \
    MSG_MED("Mem Free 0x%x", (p_mem), 0, 0);                 \
    mmgsdi_free((void *)(p_mem));                            \
    (p_mem) = NULL;                                          \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NULL
   DESCRIPTION: Will check the paramter provided and return
                MMGSDI_INCORRECT_PARAMS if the pointer is NULL
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_NULL) */
#define MMGSDIUTIL_RETURN_IF_NULL(p_var)                           \
  if ( (p_var) == NULL  )                                          \
  {                                                                \
    MMGSDIUTIL_DEBUG_MSG_ERR("Attempt to use NULL POINTER",0,0,0); \
    return MMGSDI_INCORRECT_PARAMS;                                \
  }


/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_STATUS_IF_NULL
   DESCRIPTION: Will check the paramter provided and return
                the given mmgsdi_status.
   ===========================================================================*/
/*lint -emacro(774,MMGSDIUTIL_RETURN_STATUS_IF_NULL) */
#define MMGSDIUTIL_RETURN_STATUS_IF_NULL(p_var,mmgsdi_status)      \
  if ( (p_var) == NULL  )                                          \
  {                                                                \
    MMGSDIUTIL_DEBUG_MSG_ERR("Attempt to use NULL POINTER",0,0,0); \
    return mmgsdi_status;                                          \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR
   DESCRIPTION: Allocation TMC memory according to the input size
                Check if memory allocation is successful or not
                memset the memory location to 0x00
                MMGSDI_ERROR is returned if we cannot get memory allocated
   INPUT:       p_mem: pointer to be allocated
                size: size of the allocation
   ===========================================================================*/
/*lint -emacro(506,MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR) */
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR) */
/*lint -emacro(732,MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR) */
#define MMGSDIUTIL_RETURN_IF_TMC_MEM_MALLOC_ERROR(p_mem, ptr_size)        \
  if ( (ptr_size) > 0 )                                                   \
  {                                                                       \
    (p_mem) = mmgsdi_malloc((ptr_size));                                  \
    if ( (p_mem) == NULL )                                                \
    {                                                                     \
      MMGSDIUTIL_DEBUG_MSG_ERR("TMC Heap Space (size = 0x%x) Not avail",  \
                               (ptr_size), 0, 0);                         \
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;                          \
    }                                                                     \
    MSG_MED("Mem Malloc 0x%x, size=0x%x", (p_mem), (ptr_size), 0);        \
    memset((void *)p_mem, 0x00, (ptr_size));                              \
  }                                                                       \
  else if ( ((signed)ptr_size) < 0 )                                      \
  {                                                                       \
    MMGSDIUTIL_DEBUG_MSG_ERR("Incorrect param (size = %d)",               \
                             (ptr_size), 0, 0);                           \
    return MMGSDI_ERROR;                                                  \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    MMGSDIUTIL_DEBUG_MSG_ERR("Zero ptr size in malloc (size = %d)",       \
                             (ptr_size), 0, 0);                           \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE
   DESCRIPTION: Allocation TMC memory according to the input size
                Check if memory allocation is successful or not
                memset the memory location to 0x00
   INPUT:       p_mem: pointer to be allocated
                size: size of the allocation
  ===========================================================================*/
/*lint -emacro(506,MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE) */
/*lint -emacro(774,MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE) */
/*lint -emacro(732,MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE) */
#define MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(p_mem, ptr_size, status)   \
  status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;                            \
  if ( (ptr_size) > 0 ) /*lint !e774 */                                   \
  {                                                                       \
    (p_mem) = mmgsdi_malloc((ptr_size));                                  \
    if ( (p_mem) == NULL )                                                \
    {                                                                     \
      MMGSDIUTIL_DEBUG_MSG_ERR("TMC Heap Space (size = 0x%x) Not avail",  \
                               (ptr_size), 0, 0);                         \
    }                                                                     \
    else                                                                  \
    {                                                                     \
      MSG_MED("Mem Malloc 0x%x, size=0x%x", (p_mem), (ptr_size), 0);      \
      memset((void *)(p_mem), 0x00, (ptr_size));                          \
      status = MMGSDI_SUCCESS;                                            \
    }                                                                     \
  }



/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE
   DESCRIPTION: Will return MMGSDI_INCORRECT_PARAMS if var does not fall
                within [min, max]
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(Var,Min,Max)                        \
  if ( (Var) < (Min)  )                                                       \
  {                                                                           \
    MMGSDIUTIL_DEBUG_MSG_ERR("Less than MIN: 0x%x < 0x%x, var,min",           \
                               (Var),(Min),0);                                \
    return MMGSDI_INCORRECT_PARAMS;                                           \
  }                                                                           \
  if ( (Var) > (Max) )                                                        \
  {                                                                           \
    MMGSDIUTIL_DEBUG_MSG_ERR("Greater than MAX: 0x%x > 0x%x, var,max",        \
                               (Var),(Max),0);                                \
    return MMGSDI_INCORRECT_PARAMS;                                           \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_EXCEEDS
   DESCRIPTION: Will check the paramter provided and return
                MMGSDI_INCORRECT_PARAMS the var exceed the maximum allowed
   ===========================================================================*/
#define MMGSDIUTIL_RETURN_IF_EXCEEDS(Var,Max)                                 \
  if ( (Var) > (Max) )                                                        \
  {                                                                           \
    MMGSDIUTIL_DEBUG_MSG_ERR("Greater than MAX: 0x%x > 0x%x, var,max",        \
                              (Var),(Max),0);                                 \
    return MMGSDI_INCORRECT_PARAMS;                                           \
  }

/* ===========================================================================
   MACRO:       MMGSDIUTIL_RETURN_IF_NEGATIVE
   DESCRIPTION: Will check the paramter provided and return
                MMGSDI_INCORRECT_PARAMS if the var underflow
   ===========================================================================-*/
#define MMGSDIUTIL_RETURN_IF_NEGATIVE(Var)                                    \
  if ( (Var) < 0 )                                                            \
  {                                                                           \
    MMGSDIUTIL_DEBUG_MSG_ERR(" Negative var: 0x%x", (Var), 0, 0);             \
    return MMGSDI_INCORRECT_PARAMS;                                           \
  }



/* ===========================================================================
   MACRO:       MMGSDIUTIL_SET_MIN
   DESCRIPTION: Will set var to the lesser of val1 or val2
   ===========================================================================-*/
#define MMGSDIUTIL_SET_MIN(var,val1,val2) \
  if ( (val1) < (val2) )                  \
    (var) = (val1);                       \
  else                                    \
    (var) = (val2);


/* ===========================================================================-
   MACRO:       MMGSDIUTIL_RETURN_IF_SLOT_INVALID
   DESCRIPTION: Used to check if the slot is valid or not
   INPUT:       slot:  Slot to be validated
   ===========================================================================-*/
/*lint -emacro(506,MMGSDIUTIL_RETURN_IF_SLOT_INVALID) */
/*lint -emacro(774,MMGSDIUTIL_RETURN_IF_SLOT_INVALID) */
#define MMGSDIUTIL_RETURN_IF_SLOT_INVALID(slot,include_auto)        \
  switch ( (slot) ) {                                               \
  case MMGSDI_SLOT_1:                                               \
  case MMGSDI_SLOT_2:                                               \
    break;                                                          \
  case MMGSDI_SLOT_AUTOMATIC:                                       \
    if (include_auto) /*lint !e774 */                               \
      break;                                                        \
    else {                                                          \
      MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Slot: 0x%x", (slot), 0, 0); \
      return MMGSDI_INCORRECT_PARAMS;                               \
    }                                                               \
  default:                                                          \
    MMGSDIUTIL_DEBUG_MSG_ERR("Invalid Slot: 0x%x", (slot), 0, 0);   \
    return MMGSDI_INCORRECT_PARAMS;                                 \
  }

/* ===========================================================================-
   MACRO:       MMGSDIUTIL_RETURN_IF_CLIENT_INVALID
   DESCRIPTION: client id is valid or not
   INPUT:       client id
   ===========================================================================-*/
#define MMGSDIUTIL_RETURN_IF_CLIENT_INVALID(cid)         \
  if ( mmgsdi_util_is_valid_client_id((cid)) == MMGSDI_ERROR )  \
  {                                                      \
    MSG_ERROR("INVALID Client ID 0x%x",client_id,0,0);   \
    return MMGSDI_INCORRECT_PARAMS;                      \
  }                                                      \

/* ===========================================================================
   MACRO:       MMGSDIUTIL_CHK_REFRESH_FILE_PTRS
   DESCRIPTION: Validates that only one of the file enum or file path ptr
                is valid.
   ===========================================================================*/
#define MMGSDIUTIL_CHK_REFRESH_FILE_PTRS(file_enum_ptr, file_path_ptr, status)\
  if((file_enum_ptr != NULL) && (file_path_ptr != NULL))                      \
  {                                                                           \
    MSG_ERROR("Both file enum & path not supported enum_p=0x%x path_p=0x%x",  \
              file_enum_ptr,file_path_ptr,0);                                 \
    status = MMGSDI_NOT_SUPPORTED;                                            \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    status = MMGSDI_SUCCESS;                                                  \
  }                                                                           \


/* ===========================================================================-
   MACRO:       MMGSDIUTIL_RETURN_ERR_IF_WORD_ALIGNED_VAR_OUT_OF_BOUNDS
   DESCRIPTION: Checks if the variable is not word aligned
   INPUT:       Param 1 - Non word aligned val, Param 2 - Word Aligned Value
   ===========================================================================-*/
#define MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(non_aligned, aligned)  \
  if(!((aligned == non_aligned) ||                                            \
      (aligned == non_aligned +                                               \
                  (MMGSDIUTIL_WORD_SIZE - non_aligned % MMGSDIUTIL_WORD_SIZE))\
      ))                                                                      \
  {                                                                           \
    MSG_ERROR("Var out of bounds not_aligned_val=%d, aligned_val=%d",         \
              non_aligned, aligned, 0);                                       \
    return MMGSDI_ERROR;                                                      \
  }                                                                           \

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_TO_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding uim_items_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_ERROR:            Unable to convert to UIM items


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_uim_items_enum(
  mmgsdi_file_enum_type mmgsdi_file,
  uim_items_enum_type  *uim_file_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_FROM_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the uim_items_enum_type
     and returns the corresponding mmgsdi_file_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_from_uim_items_enum (
  uim_items_enum_type    uim_file,
  mmgsdi_file_enum_type *mmgsdi_file_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_LEN

   DESCRIPTION:
     This function return the total len - offset of the cache item

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_len(
  mmgsdi_slot_id_enum_type  slot_id,
  const mmgsdi_access_type *file_access_ptr,
  mmgsdi_len_type          *data_len_ptr,
  mmgsdi_len_type           offset
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_ITEM_LEN

   DESCRIPTION:
     This function return the cache item len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_item_len(
  uim_items_enum_type   sim_filename,
  mmgsdi_tech_enum_type tech,
  boolean               pref_slot,
  mmgsdi_len_type       data_offset,
  mmgsdi_len_type       *total_len_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_UIM_ITEMS_ENUM

   DESCRIPTION:
     This function returns the uim_items_enum_type based on the access method
     and the service_table_use boolean
     MMGSDI_EF_ENUM_ACCESS:
       - return the corresponding uim_items_enum_type if there is a match
       - return UIM_NO_SUCH_FILE if there is no match
     MMGSDI_BY_PATH_ACCESS:
       1) When this function is called because it wants to find out the
          existence of corresponding path in the
          if ( service_table_use == TRUE )
            if ( path == {MF} {DF} or {ADF} )
              return UIM_NO_SUCH_FILE
            if ( path == EF )
              return corresponding uim_items_enum_type if there is a match
       2) When this function is called because it is used for non service
          table check, e.g., populating the item in the uim command for
          Access, Select functions, the following uim_items_enum_type will
          be returned
          if ( service_table_use == FALSE )
            if ( path == {MF} {DF} or {ADF} )
              return UIM_NO_SUCH_FILE
            if ( path == EF )
              return UIM_EF_BY_PATH

   DEPENDENCIES:
     None

   LIMITATIONS:
     DIRECTORY_ENUM_ACCESS is currently not being supported

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_determine_uim_items_enum (
  const mmgsdi_access_type  *access_ptr,
  uim_items_enum_type       *uim_file_ptr,
  boolean                    service_table_use
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_EF_ENUM_DIR_LEVELS

   DESCRIPTION:
     This function determines the directory level for the EF enum type
     Result will be populated into the passed in pointer

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_determine_ef_enum_df_levels(
  mmgsdi_file_enum_type       file_enum,
  uim_dir_type               *uim_dir_level_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_UIM_DIR_TYPE

   DESCRIPTION:
     This function determines the uim directory type for MF, DF, ADF and EFs

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.


   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_determine_uim_dir_type (
  const mmgsdi_access_type   *mmgsdi_access_ptr,
  uim_dir_type               *uim_dir_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DETERMINE_PROTOCOL

   DESCRIPTION:
     This function determines which card technology (UICC or ICC)

   DEPENDENCIES:
     gsdimain must be finished initialization.

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_determine_protocol(
  mmgsdi_protocol_enum_type   * protocol_type_ptr      /* UICC or ICC */
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_RESPONSE_PTR

   DESCRIPTION:
     This function frees the dynamically allocated pointer in the
     mmgsdi cnf type structure

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_free_response_ptr(
  mmgsdi_cnf_type * response_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CLIENT_REQUEST_TABLE_INDEX_FREE

   DESCRIPTION:
     This function checks if the index location is free or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_request_table_index_free(
  int32    index,
  boolean *is_free_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_FREE_INDEX

   DESCRIPTION:
     This function returns the index location of the client request info table
     that is currently available

   DEPENDENCIES:
     None

   LIMITATIONS:
     The number of entry to the table is limited to the size of the
     mmgsdi task cmd buffer

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          A valid and free index is found.
     MMGSDI_ERROR:            Unable to find a free index.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_free_index(
  int32 * index_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function populate the info to the client request table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Assignment successful
     MMGSDI_ERROR:            Assignment fail.
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_populate_client_request_table_info(
  int32                               index,
  const mmgsdi_request_header_type  * request_hdr_ptr,
  uim_cmd_type                      * uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type * extra_param_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_FREE_CLIENT_REQUEST_TABLE_INFO_INDEX

   DESCRIPTION:
     This function free the client request and uim rsp data pointers
     and set the len of the rsp data data to zero

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Free was successful


   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_free_client_request_table_index(
  int32 index
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INFO

   DESCRIPTION:
     This function return pointers to the information in the client request
     table at the corresponding index location.  The pointers to client request
     and uim rsp data returned will be pointing to the location that the
     table information is storing
     data_len is a pointer that needs to be a valid memory location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Successful retrieval of information
     MMGSDI_ERROR:            Unable to retrieve information
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_info(
  int32                                  index,
  mmgsdi_request_header_type            *request_hdr_ptr,
  uim_cmd_type                         **uim_cmd_ptr,
  mmgsdi_client_req_extra_info_type    **extra_param_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_COPY_ACCESS_TYPE

   DESCRIPTION:
     This function copy access type information from one buffer to another.
     This will dynamically allocation memory if required

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_copy_access_type(
  mmgsdi_access_type       *dest_access_ptr,
  const mmgsdi_access_type *src_access_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_WRITE_CACHE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And write data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, all the caches will be
     updated
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_write_cache(
  mmgsdi_slot_id_enum_type   slot_id,
  const mmgsdi_access_type   *file_access_ptr,
  mmgsdi_data_type           data,
  mmgsdi_len_type            offset,
  gsdi_data_from_enum_type   data_fr_card
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CACHE_SET_DEFAULT_INIT_VALUE

   DESCRIPTION:
     This function set the init flag to true, and initialize data value to
     0 with the "existing" total len

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cache_set_default_init_value(
  uim_items_enum_type       sim_filename,
  mmgsdi_tech_enum_type     tech,
  boolean                   pref_slot
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_WRITE_CACHE_ITEM

   DESCRIPTION:
     This function writes to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_write_cache_item(
  uim_items_enum_type       sim_filename,
  mmgsdi_tech_enum_type     tech,
  boolean                   pref_slot,
  mmgsdi_len_type           data_len,
  mmgsdi_len_type           data_offset,
  const uint8 *             data_ptr,
  gsdi_data_from_enum_type  data_fr_card
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_USIM_SPEC_VERSION

   DESCRIPTION:
     This function checks the usim specification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_check_usim_spec_version (
  mmgsdi_access_type *mmgsdi_access_ptr
);


/* ===========================================================================
   FUNCTION:      MMSDI_UTIL_ACCESS_PATH_LOOKUP_TABLE

   DESCRIPTION:
     This function check if the path look up data enum was initizlied or not
     and copied the path using "mmgsdi util copy path lookup table". The
     initialization information is populated in the address ie
     is_ef_init_in_table_ptr provided  by the calling function.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_access_path_lookup_table (
  const mmgsdi_access_type *mmgsdi_file_ptr,
  int32                    *path_len_ptr,
  word                    **path_array_ptr,
  boolean                  *is_ef_init_in_table_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_FILE_OK_IN_SVC_TABLE

   DESCRIPTION:
     This function based on the application available on the card,
     check if the file is allowed by the service table indication or not

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     int32:
     0 FALSE (Not Initialized)
     1 TRUE  (Initialized)
     255 (GSDI_NOT_IN_LKUP_TABLE)

   SIDE EFFECTS:

===========================================================================*/
boolean mmgsdi_util_is_file_ok_in_svc_table (
  const mmgsdi_slot_id_enum_type  *mmgsdi_slot_ptr,
  const mmgsdi_access_type        *mmgsdi_access_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SLOT_PREF

   DESCRIPTION:
     This function looks at the technology type and determines if the slot
     passed in is the operational slot for that particular technology.
     If it is, the function returns TRUE, else, it returns FALSE.
     The function also populates the new slot pointer to the correct slot info.
     This is required for automatic slot mode since the uim request will require
     the specification of the slot info

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_NOT_SUPPORTED:    Path not found in the lookup table

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_slot_pref (
  const mmgsdi_slot_id_enum_type *slot_ptr,
  mmgsdi_tech_enum_type           tech,
  boolean                        *pref_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE

   DESCRIPTION:
     This function converts the mmgsdi file enum type to uim enum type.
     And read data to different caches based on the applications available
     on the card by calling the corresding util write cache for each
     technology

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          Copy Success
     MMGSDI_ERROR:            Unable to Copy
     MMGSDI_INCORRECT_PARAMS: Null input pointer

   SIDE EFFECTS:
     If the same EF is presented in more than 1 cache, it will return the data
     from the first cache
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache(
  mmgsdi_slot_id_enum_type   slot_id,
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_data_type          *data_ptr,
  mmgsdi_len_type            offset
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_READ_CACHE_ITEM

   DESCRIPTION:
     This function reads to the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_read_cache_item (
  uim_items_enum_type   sim_filename,
  mmgsdi_tech_enum_type tech,
  boolean               pref_slot,
  mmgsdi_len_type       requested_data_len,
  mmgsdi_len_type       data_offset,
  mmgsdi_len_type *     actual_data_len_ptr,
  uint8  *              data_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_UIM_REPORT_RSP

   DESCRIPTION:
     This function is used to queue a mmgsdi_uim_report_rsp in the event that a
     command is not sent down the the UIM (ex, error before command is sent, or
     a read that comes from cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type
     MMGSDI_SUCCESS          - Command dequeued, or no commands pending
     MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
     MMGSDI_ERROR            - Other unknown failure

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_uim_report_rsp(
  int32                   index,
  mmgsdi_cmd_enum_type    mmgsdi_cmd,
  mmgsdi_return_enum_type error_status
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CLEAR_FILE_FROM_CACHE

   DESCRIPTION:
     This function will clear the indicated files from the cache

   DEPENDENCIES:
     NONE

   LIMITATIONS:
     None

   RETURN VALUE:
     None

   SIDE EFFECTS:
     None
===========================================================================*/
void mmgsdi_util_clear_file_from_cache(
  uint8                       num_files,
  const uim_items_enum_type * sim_filenames_ptr,
  mmgsdi_slot_id_enum_type    slot
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_POPULATE_CNF_RSP_HEADER

   DESCRIPTION:
     This function populate the confirmation header information

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_populate_cnf_rsp_header(
  const void*                         cnf_ptr,
  mmgsdi_response_header_type       * rsp_hdr_ptr,
  const mmgsdi_request_header_type  * req_hdr_ptr,
  mmgsdi_cnf_enum_type                cnf_cmd,
  boolean                             sw_present,
  mmgsdi_sw_status_type               status_word
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_SLOT

   DESCRIPTION:
     This function converts the mmgsdi slot type into uim slot type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_slot(
  mmgsdi_slot_id_enum_type  mmgsdi_slot,
  uim_slot_type            *uim_slot_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server (
    uim_cmd_type *cmd_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_UPDATE_STATE_VARIABLE

   DESCRIPTION:
     This function updates the state variable

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_update_state_variable(
  const mmgsdi_event_data_type *event_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SEND_CMD_TO_UIM_SERVER_SYNCH

   DESCRIPTION:
     This function populate header information for uim_cmd_type and sends
     the command to uim module and wait synchronously for the reply

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_send_cmd_to_uim_server_synch (
    uim_cmd_type *cmd_ptr
);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_NOTIFY_CLIENT_EVT_EXT

   DESCRIPTION:
     This function send notification event to registered clients

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_notify_client_evt_ext(
  mmgsdi_event_data_type *event_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_NOTIFY_CLIENT_EVT

   DESCRIPTION:
     This function send notification event to registered clients

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_notify_client_evt(
  mmgsdi_event_data_type *event_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_ERROR_EVT

   DESCRIPTION:
     This function populate card error event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_card_error_evt(
  mmgsdi_slot_id_enum_type slot,
  mmgsdi_event_data_type  *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_REMOVED_EVT

   DESCRIPTION:
     This function populate card removed event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_card_removed_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_INSERTED_EVT

   DESCRIPTION:
     This function populate card inserted event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_card_inserted_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_CARD_INIT_COMPLETED_EVT

   DESCRIPTION:
     This function populate card init completed event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_card_init_completed_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_ILLEGAL_CARD_EVT

   DESCRIPTION:
     This function populate illegal event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_illegal_card_evt(
  mmgsdi_slot_id_enum_type     slot,
  mmgsdi_event_data_type      *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_PIN_EVT

   DESCRIPTION:
     This function populate pin event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_pin_evt(
  mmgsdi_client_id_type       client_id,
  mmgsdi_slot_id_enum_type    slot,
  mmgsdi_pin_enum_type        pin,
  mmgsdi_pin_status_enum_type status,
  mmgsdi_event_data_type     *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_TP_DL_EVT

   DESCRIPTION:
     This function populate terminal profile download event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_tp_dl_evt(
  mmgsdi_slot_id_enum_type         slot,
  boolean                          support_tp,
  mmgsdi_event_data_type          *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_FDN_EVT

   DESCRIPTION:
     This function populate FDN event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_fdn_evt(
  mmgsdi_slot_id_enum_type         slot,
  boolean                          fdn_enable,
  mmgsdi_event_data_type          *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SWITCH_SLOT_EVT

   DESCRIPTION:
     This function populate switch slot event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_switch_slot_evt(
  boolean                          start,
  mmgsdi_event_data_type          *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SESSION_CLOSE_EVT

   DESCRIPTION:
     This function populate session close event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_session_close_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_client_id_type      session_id,
  mmgsdi_static_data_type    app_id,
  mmgsdi_event_data_type    *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_SELECT_AID_EVT

   DESCRIPTION:
     This function populate select aid event for notification

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_select_aid_evt(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_event_data_type    *evt_info_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_BUILD_AND_NOTIFY_EVT_FROM_STATE

   DESCRIPTION:
     This function builds the event to client who registered late based on
     the current state of the card.
     Multiple events may be sent out based on what is the currently state
     of MMGSDI

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_and_notify_evt_from_state(
  mmgsdi_slot_id_enum_type            slot,
  int32                               client_id_table_index
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CURRENT_AWAIT_CMD

   DESCRIPTION:
     This function returns the index of the command that has been sent to UIM
     for the longest time

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_current_await_cmd (
    int  *index_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_MORE_PENDING_COMMAND

   DESCRIPTION:
     This function returns the index of the command that has been sent to UIM
     for the longest time

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_more_pending_command (
  boolean *more_pending_cmd_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_CLIENT_ID_VALID

   DESCRIPTION:
     This function checks if the client ID is valid or not

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_client_id_valid (
  mmgsdi_client_id_type client_id
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_FIND_CLIENT_ID

   DESCRIPTION:
     This function checks if the client ID is valid and return the index
     location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_find_client_id (
  mmgsdi_client_id_type client_id,
  int32                 *index
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_AND_ALLOC_REQ_TYPE_LEN

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cal_and_alloc_req_type_len(
  void               **msg_ptr,
  mmgsdi_cmd_enum_type cmd,
  int32                var_len,
  int32               *mem_size_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CAL_ALIGN_SIZE

   DESCRIPTION:
     This function calculates the aligned size

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_cal_align_size(
  int32                orig_size,
  int32               *aligned_size_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CLIENT_ID_INDEX

   DESCRIPTION:
     This function checks if the client ID is valid and return the index
     location

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_id_index (
  mmgsdi_client_id_type  client_id,
  int32                 *client_index_ptr,
  int32                 *session_index_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_ALIGN_MEM

   DESCRIPTION:
     This function calculates the total memory size required for the command
     type, allocate the memory for the command pointer, and properly assigning
     the pointer of member of the command type if applicable.
     This function also takes care of the word alignment for pointer assignment

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_align_mem(
  void                *msg_ptr,
  mmgsdi_cmd_enum_type cmd
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_ALLOC_AND_POPULATE_EXTRA_PARAM

   DESCRIPTION:
     This function malloc the mmgsdi_client_req_extra_info_type pointer and
     populate the data.  If there is any error encountered in this function,
     the function wil free the allocated extra parameter pointer before
     returning error to the calling function

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_alloc_and_populate_extra_param(
  mmgsdi_cmd_enum_type                cmd_type,
  const void                         *cmd_ptr,
  mmgsdi_client_req_extra_info_type **extra_param_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CHANNEL_ID

   DESCRIPTION:
     This function returns the channel ID that corresponds to the passed in
     client id

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_channel_id
(
  mmgsdi_client_id_type  client_id,
  mmgsdi_channel_id_type *channel_id_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_CLIENT_ID

   DESCRIPTION:
     This function checks if the client ID provided is in the registrating
     table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_is_valid_client_id (
  mmgsdi_client_id_type client_id
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UIM_REPORT

   DESCRIPTION:
     This function is called when UIM report back command response

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     none

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_uim_report (
  uim_rpt_type *report_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_AID

   DESCRIPTION:
     This function returns the AID that corresponds to the passed in
     client id. Applicable to UICC only.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_static_data_type * mmgsdi_util_get_aid(
  mmgsdi_client_id_type  client_id
);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_APP_INFO

   DESCRIPTION:
     This function returns the APP Info Pointer that corresponds to the passed in
     client id.
     The function is applicable to both UICC and ICC. The caller the function
     needs to specify which app_type they need. If either is fine, then the caller
     needs to pass in MMGSDI_APP_NONE.

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

==========================================================================*/
mmgsdi_int_app_info_type * mmgsdi_util_get_app_info (
  mmgsdi_client_id_type  client_id,
  mmgsdi_app_enum_type   app_type
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_CHANNEL

   DESCRIPTION:
     This function convert mmgsdi channel type to uim channel type

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_channel(
  mmgsdi_channel_id_type mmgsdi_channel,
  uim_channel_type      *uim_channel
);


/* ===========================================================================
     FUNCTION:      MMGSDI_UTIL_QUEUE_MMGSDI_SAP_REPORT_RSP

     DESCRIPTION:
       This function is used to queue a mmgsdi_sap_report_rsp in the event that a
       command is not sent down the the BT

     DEPENDENCIES:
       NONE

     LIMITATIONS:
       None

     RETURN VALUE:
       mmgsdi_return_enum_type
       MMGSDI_SUCCESS          - Command dequeued, or no commands pending
       MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
       MMGSDI_ERROR            - Other unknown failure

     SIDE EFFECTS:
       None
  ===========================================================================*/
  mmgsdi_return_enum_type mmgsdi_util_queue_mmgsdi_sap_report_rsp(
    int32                    index,
    mmgsdi_cmd_enum_type     mmgsdi_cmd_enum,
    mmgsdi_return_enum_type  cmd_status
  );


/* ===========================================================================
     FUNCTION:      MMGSDI_UTIL_GET_CLIENT_REQUEST_TABLE_INDEX

     DESCRIPTION:
       This function is used to retrieve the index of the request type from the
       client table

     DEPENDENCIES:
       NONE

     LIMITATIONS:
       None

     RETURN VALUE:
       mmgsdi_return_enum_type
       MMGSDI_SUCCESS          - Command dequeued, or no commands pending
       MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
       MMGSDI_ERROR            - Other unknown failure

     SIDE EFFECTS:
       None
  ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_client_request_table_index(
  int32* index_ptr,
  mmgsdi_cmd_enum_type request_type
);


/*===========================================================================
FUNCTION MMGSDI_UTIL_PRINT_DATA_TO_DIAG

DESCRIPTION
  Generic Function used to print any data out onto diag.  The function takes
  a length and a data pointer and prints the data in hex format.  This
  function breaks each line to approximately 70 chars, so that the data
  would not be missed out when looking at the diag messages.

PARAMETERS
message: Message to be prefixed before each set of data
req_data: uint8 pointer to the data to be printed in hex
req_data_len: Length in bytes of the data that needs to be printed

DEPENDENCIES
  None

RETURN VALUE
  void
SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_util_print_data_to_diag(
  char*        message,
  const uint8* req_data,
  uint16       req_data_len
);


/*===========================================================================
FUNCTION MMGSDI_UTIL_ALLOC

DESCRIPTION
  Generic Function used to allocate the memory requested.  Memory is always
  allocated from tmc_heap.

  Memory when obtained will always be memset to 0.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED:  Indication that the heap has been
                                         exhausted.
    MMGSDI_INCORRECT_PARAMS:             Indication that pointer provided
                                         is not NULL.
                                         -- or --
                                         size is not greater than 0.
SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_alloc(
  void ** mem_p,
  int32  size
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_ADD_CACHE_RECORD

   DESCRIPTION:
     This function check if the data requested to be added already exist in
     the cache or not, if not, it will add the data to the cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_add_cache_record(
  mmgsdi_access_type      access,
  mmgsdi_len_type         rec_num,
  const mmgsdi_data_type *data_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CACHE_RECORD

   DESCRIPTION:
     This function get the data from the record cache if a match is found

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_cache_record(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num,
  mmgsdi_data_type     *data_ptr
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DELETE_CACHE_RECORD

   DESCRIPTION:
     This function delete the record cache requested by the caller

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_delete_cache_record(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_DELETE_ALL_CACHE_RECORD

   DESCRIPTION:
     This function delete the entire record cache

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_delete_all_cache_record( void );


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_GET_CACHE_RECORD_LEN

   DESCRIPTION:
     This function will determine if the file and record are cached and return
     the record len in the pointer provided

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_cache_record_len(
  mmgsdi_access_type    access,
  mmgsdi_len_type       rec_num,
  mmgsdi_len_type      *rec_len_p
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CONVERT_UIM_BY_PATH_ENUM

   DESCRIPTION:
     This function checks if the uim enum is those that has a different mapping
     in GSM than USIM, e.g., UIM_TELECOM_ADN,
     this function converts the uim enum to the right enum, e.g.,
     UIM_TELECOM_BY_PATH_ADN

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_uim_by_path_enum(
  uim_items_enum_type                  *uim_file_items
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_BY_PATH_UIM_ENUM

   DESCRIPTION:
     This function check if the uim enum is under BY_PATH_START_EF, and convert
     to a corresponding enum, if the enum can't be found within the
     BY_PATH_START_EF, it will update the file_enum to UIM_EF_BY_PATH

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_find_by_path_uim_enum(
  uim_items_enum_type                  *uim_file_items
);


/*===========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_UIM_PATH

DESCRIPTION
  This function is used to convert mmgsdi_access_type to uim path for
  MMGSDI_BY_PATH_ACCESS

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_convert_to_uim_path(
  uim_items_enum_type                      orig_uim_file_items,
  const mmgsdi_access_type                *mmgsdi_access_ptr,
  const mmgsdi_uim_additional_params_type *add_path_ptr,
  uim_items_enum_type                     *update_uim_file_items_ptr,
  word                                    *uim_path_array
);


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_CHANNEL_ID_VALID

DESCRIPTION
  This function is used to verify that the channel ID provided is a
  valid channel.

  The only acceptable values are:
  channel_id greater than or equal to 0

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_channel_id_valid(
  int32 channel_id
);


/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_LEN_VALID

DESCRIPTION
  This function is used to verify the length of the data meets the following
  requirements:

  1. Data must be greater than 0
  2. Data must be less than GSDI_MAX_DATA_BLOCK_LEN

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_len is valid for configuration
            FALSE: data_len is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_len_valid(
  int32 data_len
);

/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_DATA_BUFFER_VALID

DESCRIPTION
  This function is used to verify the buffer used is valid.

  1. Buffer Pointer Can not be NULL

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  data_buffer is valid for request
            FALSE: data_buffer is not valid for request

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_data_buffer_valid(
  const byte * data_buffer
);

/*===========================================================================
FUNCTION MMGSDI_CHNL_MGT_IS_SLOT_VALID

DESCRIPTION
  This function is used to verify the slot id received in the message.
  All channel management procedures require that the correct slot
  is provided in the request.

  The only acceptable values are:
  GSDI_SLOT_1 -> 0x01
  GSDI_SLOT_2 -> 0x02

DEPENDENCIES
  None

RETURN VALUE
  boolean:  TRUE:  Slot is valid for configuration
            FALSE: Slot is not valid for configuration

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_chnl_mgt_is_slot_valid(
  mmgsdi_slot_id_enum_type slot
);


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_SESSION_CLOSE_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function calls corresponding function cleanup session information,
     and notify client based on the corresponding client_index and session_index
     into the mmgsdi_client_id_reg_table

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:
     The Session information for the indices will be cleanup, which include
     freeing up any relevant information for the session

===========================================================================*/
void mmgsdi_util_session_close_cleanup_and_notify(
  int client_index,
  int session_index
);


/* ===========================================================================
   FUNCTION:      MMGSDI_UTIL_CLOSE_ALL_SESSIONS_CLEANUP_AND_NOTIFY

   DESCRIPTION:
     This function goes through all the entries in the client id reg table,
     perform cleanup and notify the corresponding client

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:

===========================================================================*/
void mmgsdi_util_close_all_sessions_cleanup_and_notify(void);


/*===========================================================================
  FUNCTION MMGSDI_UTIL_CMP_AID

DESCRIPTION
  This function is used to compare two app ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS
                            MMGSDI_ERROR

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_cmp_aid(
  const mmgsdi_aid_type *aid1_ptr,
  const mmgsdi_aid_type *aid2_ptr
);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_USIM_APPS

DESCRIPTION
  This function is used to get the number of usim apps.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
uint32 mmgsdi_util_get_usim_apps(void);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_BUILD_PIN_REQ

DESCRIPTION
  This function is used to compare two app ids.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_pin_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_pin_enum_type               pin_id,
  mmgsdi_pin_operation_enum_type     pin_op,
  mmgsdi_pin_operation_enum_type     orig_pin_op,
  mmgsdi_protocol_enum_type          protocol
);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_BUILD_GET_FILE_ATTR_REQ

DESCRIPTION
  This function builds a get file attribute request and sends it to mmgsdi.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_build_get_file_attr_req(
  const mmgsdi_request_header_type  *req_header_ptr,
  mmgsdi_access_type                 access_type,
  mmgsdi_client_data_type            client_ref
);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_GET_KEY_REF

DESCRIPTION
  This function gets the chv based on the PIN ID and protocol
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_retrun_enum_type
SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_util_get_key_ref(
  mmgsdi_client_id_type     client_id,
  mmgsdi_pin_enum_type      pin,
  uim_chv_type             *chv
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_INTERNAL_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands generated by MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_internal_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_GET_SEL_APP

DESCRIPTION
  This function returns the currently selected app

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
 mmgsdi_int_app_info_type * mmgsdi_util_get_sel_app(void);


/* ==========================================================================
FUNCTION MMGSDI_UTIL_GET_SEL_APP_PIN_PTR

DESCRIPTION
  This function gets the PIN Ptr for the given PIN ID for the default APP

DEPENDENCIES
  mmgsdi_util_get_sel_app

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_app_pin_info_type

SIDE EFFECTS
  NONE
==========================================================================*/
mmgsdi_app_pin_info_type * mmgsdi_util_get_sel_app_pin_ptr(
  gsdi_pin_ids_T pin_id
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_FIND_APP_PIN_INDEX

DESCRIPTION
  This function finds the index into one of the foolowing tables -
  1. mmgsdi_app_pin1_table
  2. mmgsdi_app_pin2_table
  3. mmgsdi_app_univ_pin_table

  It first finds if the key reference already exists in the table,
  If  not, then it looks for the first empty slot.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
 mmgsdi_return_enum_type mmgsdi_util_find_app_pin_index(
   const mmgsdi_app_pin_info_type  *app_pin_table,
   mmgsdi_key_ref_values_enum_type  key_ref,
   uint32                          *index
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_RESET_APP_DATA

DESCRIPTION
  This function resets all the app related data including PIN

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_reset_app_data(
  boolean reselect_app
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_RESET

DESCRIPTION
  This function reset the global data of gsdi and mmgsdi based on the refresh mode

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_reset(
  mmgsdi_refresh_mode_enum_type refresh_mode,
  mmgsdi_slot_id_enum_type      slot_id,
  boolean                       reselect_app
);


/* ===========================================================================
FUNCTION MMGSDI_UTIL_SET_PIN_INFO

DESCRIPTION
  This function sets the pin info from the src pin info
DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void mmgsdi_util_set_pin_info(
  mmgsdi_pin_info_type           *dest,
  const mmgsdi_app_pin_info_type *src
);


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_CB

DESCRIPTION
  This function, defined as mmgsdi_callback_type, will handle the internal
  commands  generated by MMGSDI

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_gsdi_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);


/* ==========================================================================
FUNCTION MMGSDI_UTIL_STORE_PIN

DESCRIPTION
  This function, will store the pin for the PIN ID. It will set the
  valid_pin flag to FALSE. Once the operation succeeds the flag will
  be converted to TRUE.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_store_pin(
  mmgsdi_pin_enum_type           pin_id,
  mmgsdi_static_data_type        pin_data
);


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_FIND_AID_INDEX

   DESCRIPTION:
     This function returns an index location to the mmgsdi_slot_data_type
     if a matching AID is found

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uint32: index location of the AID
             MMGSDI_MAX_APP_INFO if not found

   SIDE EFFECTS:
     None
============================================================================*/
uint32 mmgsdi_util_find_aid_index(
  mmgsdi_slot_id_enum_type slot_id,
  mmgsdi_aid_type          aid
);


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_IS_VALID_AID_INDEX

   DESCRIPTION:
     This function returns whether the AID index is valid or not

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     TRUE if valid index
     FALSE if invalid index

   SIDE EFFECTS:
     None
============================================================================*/
boolean mmgsdi_util_is_valid_aid_index(
  uint32 aid_index
);


/* ============================================================================
   FUNCTION:      MMGSDI_UTIL_WAIT_FOR_CARD_SELECT

   DESCRIPTION:
     This function finds out if we need to wait for Card Selection from
     Clients

   DEPENDENCIES:
     NV

   LIMITATIONS:
     None

   RETURN VALUE:
     TRUE if MMGSDI needs to wait
     FALSE if MMGSDI does not need to wait

   SIDE EFFECTS:
     None
============================================================================*/
boolean mmgsdi_util_wait_for_card_select(void);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SEL_AID

   DESCRIPTION:
     This function checks if the client ID is intended for the select AID.
     If client ID points to default channel, return TRUE
     If client ID points to a non default channel but the AID matches the
     default channel application, return TRUE

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_util_is_sel_aid(
  mmgsdi_client_id_type   client_id
);


/*===========================================================================
FUNCTION MMMGSDI_UTIL_CAN_PROCEED_TO_POST_PIN1

DESCRIPTION
  This function checks the pin1 and univ pin status and the replacement
  condition to determine if post pin1 procedure is allowed or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  Can proceed with Post PIN1 process
  FALSE: Cannot proceed with Post PIN1 process

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_util_can_proceed_to_post_pin1(
  mmgsdi_slot_id_enum_type slot
);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTI_PRESET_RETRIES

   DESCRIPTION:
     This function sets the pin retires to 3 and the unblock
     retries to 10

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     None

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_preset_retries(mmgsdi_slot_id_enum_type);


/*==========================================================================
FUNCTION MMGSDI_UTIL_GSDI_SET_RETRIES

DESCRIPTION
  This function will set the number of retries for all PIN
  functions

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
 NONE

SIDE EFFECTS
  NONE
==========================================================================*/
void mmgsdi_util_gsdi_set_retries(
  mmgsdi_pin_enum_type            pin_id,
  mmgsdi_int_app_info_type       *sel_app_ptr,
  uint8                          *retries_ptr,
  mmgsdi_num_retry_enum_type      retry_type
);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_RESET_PIN_STATUS

   DESCRIPTION:
     This function resets the pin status

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
void mmgsdi_util_reset_pin_status(void);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_IS_SEL_APP_PIN_CACHED

   DESCRIPTION:
     This function checks to see if selected app's pin is cached

   DEPENDENCIES:
     None

   LIMITATIONS:


   RETURN VALUE:
     boolean

   SIDE EFFECTS:

==========================================================================*/
boolean mmgsdi_util_is_sel_app_pin_cached(void);


/* ==========================================================================
   FUNCTION:      MMGSDI_UTIL_CHECK_FILE_ATTR

   DESCRIPTION:
     This function checks the file attributes

   DEPENDENCIES:
     gsdi_ef_access_conditions_check

   LIMITATIONS:
     Checks are Limited to Read and write only, for all others, it will return
     SUCCESS

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     File Read/Write will be blocked if the security conditions have not
     been met.

==========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_check_file_attr(
  void                 * req_ptr,
  mmgsdi_cmd_enum_type   cmd_type
);


/*===========================================================================
   FUNCTION mmgsdi_util_is_pin_valid

   DESCRIPTION:
     This function will check the pin meets the minimum length requirement with
     valid data

   PARAMS:
    pin_ptr:          Will contain the code of the PIN.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
      boolean:  TRUE  PIN is valid
                FALSE PIN is not valid

   SIDE EFFECTS:

 ===========================================================================*/
boolean mmgsdi_util_is_pin_valid(const uint8 * pin_ptr);


/*===========================================================================
   FUNCTION mmgsdi_util_get_file_path_from_enum

   DESCRIPTION:
     This function gets the file path for a given file enum value.
     This function is based on the uim function to get the path.

   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_get_file_path_from_enum(uim_items_enum_type file_enum,
                                                            word *path,
                                                            const word *defined_path);


/*===========================================================================
   FUNCTION mmgsdi_util_is_sel_app

   DESCRIPTION:
     This function compares the aid with the given aid and finds out if the
     given aid is the same as default app
   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
boolean mmgsdi_util_is_sel_app(mmgsdi_aid_type aid);


/*===========================================================================
   FUNCTION mmgsdi_util_get_session_ptr

   DESCRIPTION:
     This function returns the session ptr for the given client id
   PARAMS:

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
mmgsdi_session_info_type*  mmgsdi_util_get_session_ptr(
  int32                  client_index,
  mmgsdi_aid_type        aid
);


#ifdef FEATURE_MMGSDI_MBMS
#error code not present
#endif /*FEATURE_MMGSDI_MBMS*/


/*===========================================================================
   FUNCTION mmgsdi_util_print_file_information

   DESCRIPTION:
     This function print file information.
   PARAMS:
     file_access: File Information
     is_error_msg: boolean variable to print message as MSG_ERROR.

   DEPENDENCIES:
     None

   LIMITATIONS:

   RETURN VALUE
    None
 ===========================================================================*/
void mmgsdi_util_print_file_information (
  mmgsdi_access_type file_access,
  boolean is_error_msg
);


#ifdef FEATURE_MMGSDI_PKCS15
#error code not present
#endif /* FEATURE_MMGSDI_PKCS15 */


/* ==========================================================================
FUNCTION MMGSDI_UTIL_CONVERT_TO_REFRESH_FILE_PATHS

DESCRIPTION
  This function shall accept byte buffer that has list of file paths.
  Each file path begins from MF, hence byte value 0x 3F shall be used
  as a delimiter. The individual file paths shall be stored in the
  array of mmgsdi_path_type. For example
  Input buffer: 0x02 0x3F 0x00 0x2F 0x02 0x3F 0x00 0x7F 0x10 0x6F 0xD2
                 |
                First byte is number of files
  Should give output -
  file_path_pptr[0].path_buf = {0x3F 0x00 0x2F 0x02}
  file_path_pptr[0].path_len = 2
  file_path_pptr[1].path_buf = {0x3F 0x00 0x7F 0x10 0x6F 0xD2}
  file_path_pptr[1].path_len = 3
  num_files = 2

DEPENDENCIES
  None

LIMITATIONS
  The function shall only consider the number of files as mentioned in the
  first byte of the input data_ptr. If the actual buffer has more files than
  the number mentioned in the first byte, the logic in the function will
  ignore the rest of the files.

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  The calling function should free the memory allocated to file_path_pptr.
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_util_convert_to_refresh_file_paths(
  uint32                 data_len,
  const uint8            *data_ptr,
  mmgsdi_path_type       **file_path_pptr,
  uint8                  *num_files_ptr
);


/*===========================================================================
  FUNCTION mmgsdi_util_malloc_and_copy_file_path_array

  DESCRIPTION:
    This function shall allocate memory for the destination file path
    array and will copy file paths from source array

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE
    MMGSDI_SUCCESS                     => Successful file copy operation
    MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED => Malloc Error
    MMGSDI_ERROR                       => For all other failure scenarios


  SIDE EFFECTS:
    The calling function should free the allocated memory
 ===========================================================================*/
mmgsdi_return_enum_type mmgsdi_util_malloc_and_copy_file_path_array(
  mmgsdi_path_type           **dst_file_path_pptr,
  mmgsdi_path_type           *src_file_path_ptr,
  uint32                     num_files
);
#endif /* MMGSDIUTIL_H */

