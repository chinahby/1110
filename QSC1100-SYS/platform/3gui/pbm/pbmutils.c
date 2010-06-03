/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM.

  Copyright (c) 2002 - 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbmutils.c#80 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/09   krishna changes for CR 167785
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files during power up.
16/03/09   kcp     resolved Lint errors
03/26/08   cvs     Force NULL termination in strncpy()
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
10/24/07   clm     Fix CR125200.  Record id's from external apps should be
                   unaware of special circular cache indexing.
09/05/07   cvs     Change PBM_FIELD_NUMBER_TYPE and PBM_FIELD_NUMBER_PLAN to look for BYTEs
03/12/07   cvs     Featurize 4 PB set support
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add error message
12/07/06   cvs     Lint fixes
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
07/27/06   slw     Differentiated field info for ADN and SIM groups
07/26/06   cvs     Fix compile errors
07/24/06   cvs     Fixes for unequal size multi pb set support
07/12/06   cvs     Fix compile warnings
07/11/06   slw     Added pointer validity check for pbm_fields_convert_str_to_ucs2()
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
06/14/06   aps     support for ANRC and ANRC1 type records
05/31/06   cvs     Fix support of multiple ANR files
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/28/06   cvs     fix issue where going from international number to not isn't saved.
04/12/06   cvs     better logging
03/24/06   cvs     %x -> 0x%x in log messages
03/14/06   cvs     fix + at beginning of intl numbers
03/10/06   cvs     Lint fixes
02/28/06   cvs     Add support for MBDN
02/24/06   cvs     Move string functions to pbm_conversion.
02/23/06   cvs     rework memory allocation macros
01/19/06   cvs     Emergency number cleanup
01/09/06   cvs     Lint fixes
12/20/05   cvs     Fix reboot (infinite loop) when adding to a full USIM file
11/16/05   cvs     Lint fixes
11/16/05   cvs     Moved pbm_record_id_to_file_id() to pbmutils.c
11/10/05   cvs     Add network ECC numbers
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
09/09/05   cvs     Fix pbm_get_name() sorting if first/last names are separate fields
09/01/05   cvs     match_found(): dummy records don't match
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   AT      Added new Emergency Group for Searches.
08/24/05   cvs     Minor optimizations to pbm_find_name(), make functions public.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/20/05   AT      Fixed order of types when searching mixed datatypes.
08/12/05   AT      Added UCS2 support for strings.
08/04/05   cvs     pbm_allocate_index(): Only check PB_SET if we need to.
08/04/05   cvs     pbm_build_record_from_fields(): Move number/TON handling back
                   to end, after we've seen all fields.
08/02/05   cvs     Fix field corruption in pbm_group_fields(), compile warnings
07/29/05   AT      Added some range checking and the feature PBM_MEM_INIT.
                   PBM_MEM_INIT causes pbm_mem_alloc to initialize memory to
                   0's, and pbm_mem_free to 0xCACACACA.  Helps catch memory
                   being used after being freed.
07/11/05   cvs     Better error messages
06/23/05   cvs     Speed up searching, using qsort() for results
06/10/05   cvs     Fix issue where pbm_build_record_from_fields() put junk
                   in the number field.
05/31/05   cvs     Fix return type of pbm_convert_pbm8_to_gsm7()
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/18/05   AT      Updated the string comparison functions to correctly
                   work with pause characters and wild cards.
04/12/05   AT      Fix LINT errors.
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements, including making the ECC
                   read only.
03/22/05   PA      Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for MSISDN, re-entrant complex field enumeration,
                   greater than enumeration and extended lock support.
01/18/05   PA      Fixed typo in error message
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
12/08/03   PA      Provide EFS field info for PBM_DEFAULT
12/02/03   PA      Record validation utilities
11/19/04   PA      Support non-GSDI builds, code review comments.
11/18/04   PA      Support for complex fields.
11/02/04   PA      Added "999" to list of emergency numbers.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "customer.h"
#ifdef FEATURE_PBM_TASK
#include "cmutil.h"

#include "tmc.h"

#include "pbm.h"
#include "pbmutils.h"
#include "pbmefs.h"
#include "pbmgsdi.h"
#include "pbm_conversion.h"

/* <EJECT> */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/
static uint8 pbm_number_type_to_gsmton[] = {
  0, CM_TON_INTERNATIONAL, CM_TON_NATIONAL, CM_TON_NETWORK_SPECIFIC,
};

static uint8 pbm_number_plan_to_gsmnpi[] = {
  0, CM_NPI_ISDN, CM_NPI_DATA, CM_NPI_TELEX,
  CM_NPI_PRIVATE
};

char pbm_bcd_to_char_mapping[] = {
 '0', '1', '2', '3',
 '4', '5', '6', '7',
 '8', '9', '*', '#',
 /* Time Pause         Wild           Expansion,        Endmark.*/
 PBM_TIMEPAUSE_CHAR, PBM_WILD_CHAR,  PBM_EXPANSION_CHAR, '\0'
};


//lint -emacro(545,OFFSET)  Suspicious use of &.  True enough.
#define OFFSET(struct_type, field)  (((int) &(((struct_type *)4)->field)) - 4)

#define PBM_SUB_FIELD_HEADER_SIZE OFFSET(pbm_sub_field_s_type, buf)

/* Align at 4 bytes */
#define PBM_PAD_BYTES(p)  ((4 - ((uint32)(p) & 0x3)) & 0x03)

#define PBM_FIELD_ALIGN(ptr) ((void *)((ptr) + PBM_PAD_BYTES((ptr))))

#define PBM_NEXT_SUB_FIELD(sub_field)  \
               ( PBM_FIELD_ALIGN((sub_field) + PBM_SUB_FIELD_HEADER_SIZE + \
                                 (sub_field)->data_len) )

/*===========================================================================
                           Memory allocation variables.
===========================================================================*/

/* Critical section used for memory management */
static rex_crit_sect_type pbm_mem_crit_sect;

/* The following structures are used by the phone book manager's best fit
 * memory allocation system.  The current mem_malloc schemes are not well
 * suited for PBM's allocation needs.  In particular, circular files can
 * cause fragmentation issues in the heap resulting in the need for large
 * areas of extra memory.
 */
#ifdef PBM_MEM_HEADERS
#define GOOD_SIGNATURE (unsigned long) 0x5555AAAA
#define IN_USE         (unsigned long) 0xA110CA7E   /* Allocate(d) */
#define NOT_IN_USE     (unsigned long) 0xF4EEF4EE   /* FreeFree    */
#endif

/* These items all help make sure the memory block has not been corrupted.
 * Between the moving signature and stationary allocated flags we should
 * have some confidence if there is a memory overwrite. */

/* The free node is made up of blocks of memory that are not currently
 * allocated.  Each free_node will point to the next free node.  Adjacent
 * free nodes are merged during FREE and ALLOC operations.  The free
 * will join adjacent nodes after the current node.  The ALLOC will
 * join adjacent nodes as it sees them during an ALLOC operation.
 * Each structure begins with the allocated flag and ends with the
 * signature.  This helps ensure the integrity of the data.  If an
 * application writes to freed memory or overwrites its allocated area,
 * one or both of these flags will be corrupt indicating the failure.
 * Technically, size in both the free and used structures and the next
 * pointer in the free structure are all that is necessary.  The other
 * flags allow for error checking.
 */
typedef struct free_node
{
#ifdef PBM_MEM_HEADERS
  unsigned long allocated;  /* Flag indicating if this block is allocated. */
#endif
  unsigned long size;       /* The number of bytes in this free block. */
  struct free_node *next;   /* Pointer to the next free block. */
#ifdef PBM_MEM_HEADERS
  unsigned long signature;  /* Signature of a free block lives in memory
                               that was previously given to the user. */
#endif
} FREE_NODE;

typedef struct used_node
{
#ifdef PBM_MEM_HEADERS
  unsigned long allocated;  /* Flag indicating if this block is allocated. */
#endif
  unsigned short requested; /* Number of bytes originally requested. */
  unsigned short size;      /* The actual number of bytes in this used block. */
#ifdef PBM_MEM_HEADERS
  unsigned long signature;  /* Signature of a used node, not seen by user. */
#endif
#ifdef T_MDM8200
  unsigned long empty;      //this member is introduced to resolve the crash 169091 by aligning the memory to 8 bytes
#endif
} USED_NODE;

#define PBM_MEM_USED_OFFSET sizeof(USED_NODE)

/* Keep some statistics. */
//lint -esym(551, pbm_mem_bytes_requested)  appears not referenced
static unsigned long pbm_mem_bytes_requested = 0;
//lint -esym(551, pbm_mem_bytes_allocated)  appears not referenced
static unsigned long pbm_mem_bytes_allocated = 0;
//lint -esym(551, pbm_mem_blocks_allocated)  appears not referenced
static unsigned long pbm_mem_blocks_allocated = 0;
//lint -esym(551, pbm_mem_max_bytes_allocated)  appears not referenced
static unsigned long pbm_mem_max_bytes_allocated = 0;

static FREE_NODE *pbm_mem_first_available;      /* Start of the free list. */
static unsigned long pbm_mem_last_address;              /* End of memory. */
static unsigned long pbm_mem_first_address;             /* Start of memory. */

__align(4) static uint8 pbm_heap_mem_buffer[PBM_HEAP_MEM_BUFFER_SIZE];  /* Buffer */

/*===========================================================================
FUNCTION pbm_heap_init

DESCRIPTION
  This function initializes the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_heap_init()
{
  rex_init_crit_sect(&pbm_mem_crit_sect);
  pbm_mem_alloc_init(pbm_heap_mem_buffer, sizeof(pbm_heap_mem_buffer));

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_mem_pick_size

DESCRIPTION
  This function enforces the policy of allocating blocks in 16 byte
  steps.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static unsigned long pbm_mem_pick_size(unsigned long orig_req)
{
  return PBM_MEM_MULTIPLE_SIZE*((orig_req + PBM_MEM_MULTIPLE_SIZE - 1)/
                  PBM_MEM_MULTIPLE_SIZE);
}


/*===========================================================================
FUNCTION pbm_mem_alloc

DESCRIPTION
  Search the free list for the best fit block of memory.  If no exact
  size is found, the closest sized block is split to hold the new memory.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void *pbm_mem_alloc(unsigned long size)
{
  unsigned long best_size = 0;  /* What was the size of the best fit */
  unsigned long give_size = 0;  /* What size should we give them. */
  FREE_NODE *best_node = NULL;  /* Pointer to the free node of best fit size */
  FREE_NODE *prev_free = NULL;  /* Pointer to the free node before best_node */
  FREE_NODE *curr_node = NULL;  /* What node are we looking at now. */
  FREE_NODE *last_free = NULL;  /* What was the last node looked at. */
  USED_NODE *allocated = NULL;  /* The memory after it is allocated. */

  /* Set the size to the value corresponding to the policy. */
  give_size = pbm_mem_pick_size(size+sizeof(USED_NODE));
  if (give_size > 0xFFFF)
  {
    PBM_MSG_ERR("Requested size 0x%x greater than allowed", size, 0, 0);
    return NULL;
  }

  /* Memory allocation is a critical resource for PBM. */
  rex_enter_crit_sect(&pbm_mem_crit_sect);

  /* Search through the free list. */
  curr_node = pbm_mem_first_available;
  while (curr_node != NULL)
  {
    /* Check for memory corruption. */
#ifdef PBM_MEM_HEADERS
    if ((curr_node->signature != GOOD_SIGNATURE) ||
        (curr_node->allocated != NOT_IN_USE))
    {
      PBM_MSG_ERR("Corrupted memory at 0x%x 0x%x 0x%x",
                  curr_node, curr_node->signature, curr_node->allocated);
      break;
    }
#endif

    /* First consolodate free nodes if they exist. */
    while ((((unsigned long) curr_node) + curr_node->size) ==
            ((unsigned long) curr_node->next))
    {
      if ((curr_node->size < PBM_MEM_MULTIPLE_SIZE)
#ifdef PBM_MEM_HEADERS
          || (curr_node->next->signature != GOOD_SIGNATURE)
          || (curr_node->next->allocated != NOT_IN_USE)
#endif
         )
      {
        PBM_MSG_ERR("Corrupted memory at 0x%x %d 0x%x",
                    curr_node, curr_node->size, curr_node->next);
        break;
      }
      curr_node->size += curr_node->next->size;
      curr_node->next = curr_node->next->next;
    }

    /* Now that this node is the biggest it can be, check the size. */
    if (curr_node->size >= give_size)
    {
      /* If this is the best we found, remember it. */
      if ((best_node == NULL) || (curr_node->size < best_size))
      {
        best_node = curr_node;  /* This node is the best. */
        prev_free = last_free;  /* Last will have to point around this one */
        best_size = curr_node->size;    /* This is the size of the best. */
      }
    }

    if (best_size == give_size) /* If we are as good as we are going to be. */
      break;                    /* Stop searching. */

    /* We will continue searching. */
    if (((uint32) curr_node->next > (uint32) curr_node + sizeof(FREE_NODE)) ||
        (curr_node->next == NULL))
    {
      last_free = curr_node;         /* This node is now the last node. */
      curr_node = curr_node->next;   /* Current node is next free node. */
    }
    else
    {
      PBM_MSG_ERR("Current node 0x%x next pointer corrupt", curr_node,
                  curr_node->next, 0);
      break;
    }
  }

  /* If we reach here and best_node is NULL, we are out of memory. */
  if (best_node == NULL)
  {
    rex_leave_crit_sect(&pbm_mem_crit_sect);
    PBM_MSG_ERR("Out of memory looking for %d bytes", size, 0, 0);
    return NULL;
  }

  /* If we reach here, we have memory to give. */
  if (((unsigned long) best_node < pbm_mem_first_address) ||
      (((unsigned long) best_node + best_node->size) >
       (unsigned long) pbm_mem_last_address))
  {
    /* Maybe not, somewhere, size goes outside the bounds of memory. */
    rex_leave_crit_sect(&pbm_mem_crit_sect);
    PBM_MSG_ERR("Memory corrupted at 0x%x!", best_node, 0, 0);
    return NULL;
  }

  /* Do we need to split the block? */
  if (best_node->size > give_size)
  {
    FREE_NODE *tmp;

    /* Split the current node into two.  The tmp variable will
     * point to the new free node. */
    tmp = (FREE_NODE *) ((unsigned long) best_node + give_size);
    tmp->size = best_size-give_size;
    tmp->next = best_node->next;
#ifdef PBM_MEM_HEADERS
    tmp->signature = GOOD_SIGNATURE;
    tmp->allocated = NOT_IN_USE;
#endif

    best_node->next = tmp;
  }

  /* Now maintain the free list, either move the start or the next */
  /*lint -esym(613,prev_free)  This is only used on second pass, set prior*/
  if (best_node == pbm_mem_first_available)
    pbm_mem_first_available = best_node->next;
  else
    prev_free->next = best_node->next;

  /* Update the statistics. */
  pbm_mem_bytes_allocated += give_size;
  pbm_mem_bytes_requested += size;
  if (pbm_mem_bytes_allocated > pbm_mem_max_bytes_allocated)
    pbm_mem_max_bytes_allocated = pbm_mem_bytes_allocated;
  pbm_mem_blocks_allocated++;

  rex_leave_crit_sect(&pbm_mem_crit_sect);

  /*lint -e{740}*/
  allocated = (USED_NODE *) best_node;

  /* Now ready the block we are going to give away. */
  allocated->size      = (uint16) give_size;
  allocated->requested = (uint16) size;
#ifdef PBM_MEM_HEADERS
  allocated->signature = GOOD_SIGNATURE;
  allocated->allocated = IN_USE;
#endif
#ifdef PBM_MEM_INIT
  memset((unsigned char *) allocated + PBM_MEM_USED_OFFSET, 0,
         give_size - PBM_MEM_USED_OFFSET);
#endif
  return ((unsigned char *) allocated + PBM_MEM_USED_OFFSET);
}


/*===========================================================================
FUNCTION pbm_mem_free

DESCRIPTION
  Takes the node and frees it.  If the following block can be merged,
  then merge that one.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_mem_free(void *block)
{
  USED_NODE *this_block;        /* The beginning of the alloc block. */
  FREE_NODE *free_block;        /* This block set free. */
  FREE_NODE *prev_free_block;   /* Previous free block. */

  /* Set this_block to the start of the block of memory. */
  this_block = (USED_NODE *) ((uint32) block - PBM_MEM_USED_OFFSET);
  if (((uint32) this_block) < ((uint32) pbm_mem_first_address) ||
      (uint32) this_block > ((uint32) pbm_mem_last_address - sizeof(USED_NODE)))
  {
    PBM_MSG_ERR("Freeing invalid pointer 0x%x (range 0x%x-0x%x", block, pbm_mem_first_address,
        pbm_mem_last_address - sizeof(USED_NODE));
    return;
   }


  /* Ensure the data was not corrupted.  Don't double free. */
  if ((this_block->size >= PBM_MEM_MULTIPLE_SIZE) &&
#ifdef PBM_MEM_HEADERS
      (this_block->signature == GOOD_SIGNATURE) &&
      (this_block->allocated == IN_USE)
#endif
     )
  {
    unsigned long free_size;

    /* Going to set it free. */
    rex_enter_crit_sect(&pbm_mem_crit_sect);

    /* Update the statistics in marking this free. */
    pbm_mem_bytes_requested-=this_block->requested;
    pbm_mem_bytes_allocated-=this_block->size;
    pbm_mem_blocks_allocated--;

    free_size = this_block->size;

    /* The block will now be changed to a free block.  */
    free_block = (FREE_NODE *) (void*) this_block;

    /* Set the flags accordingly. */
    free_block->size      = free_size;
    free_block->next      = NULL;
#ifdef  PBM_MEM_HEADERS
    free_block->signature = GOOD_SIGNATURE;
    free_block->allocated = NOT_IN_USE;
#endif

    /* The free list is sorted, so check if we are going to be first. */
    if (free_block < pbm_mem_first_available)
    {
      /* Yep, first in the list. */
      free_block->next = pbm_mem_first_available;
      pbm_mem_first_available = prev_free_block = free_block;
    }
    else
    {
      /* Nope, was anyone first? */
      if (pbm_mem_first_available == NULL)
      {
        /* List was empty, now something is free. */
        pbm_mem_first_available = free_block;
        prev_free_block = pbm_mem_first_available;
        pbm_mem_first_available->next = NULL;
      }
      else
      {
        /* List was not empty, we are not first.  We will eventually
         * put this block after the prev_free_block.  */
        prev_free_block = pbm_mem_first_available;
        while (prev_free_block->next && (free_block > prev_free_block->next))
          prev_free_block = prev_free_block->next;

        /* prev_free_block now points to the block before this one. */
        free_block->next = prev_free_block->next;
        prev_free_block->next = free_block;
      }
    }

    /* Now consolodate free nodes if they exist. If this node follows a
     * free node, that will be found on the allocation. Because nodes
     * can be freed in any order, there may be any number of blocks
     * to be merged. */
    while ((((unsigned long) prev_free_block) + prev_free_block->size) ==
            ((unsigned long) prev_free_block->next))
    {
      prev_free_block->size += prev_free_block->next->size;
      prev_free_block->next = prev_free_block->next->next;
    }

#ifdef PBM_MEM_INIT
    /* Initialize Freed memory to 0xCACACACA */
    /* If any code is using a freed block, this will help catch it */
    memset((unsigned char *)prev_free_block + sizeof(FREE_NODE), 0xca,
           prev_free_block->size - sizeof(FREE_NODE));
#endif
    rex_leave_crit_sect(&pbm_mem_crit_sect);
  }
  else
  {
    /* Something was wrong with the data. */
#ifdef PBM_MEM_HEADERS
    PBM_MSG_ERR("Unallocated or corrupted memory at 0x%x 0x%x 0x%x",
                this_block, this_block->size, this_block->allocated);
#else
    PBM_MSG_ERR("Unallocated or corrupted memory at 0x%x 0x%x",
                this_block, this_block->size, 0);
#endif
  }
}


/*===========================================================================
FUNCTION pbm_mem_alloc_init

DESCRIPTION
  Initially, the memory buffer is one big empty buffer.  Set the start of
  it to be a FREE_NODE and set the flags accordingly.  Also, initialize our
  pbm_mem_first_available and pbm_mem_last_address variables;

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_mem_alloc_init(unsigned char *buffer, unsigned long size)
{
  pbm_mem_first_available = (FREE_NODE *) buffer; //lint !e826
  pbm_mem_first_available->next = NULL;
  pbm_mem_first_available->size = size;
#ifdef PBM_MEM_HEADERS
  pbm_mem_first_available->allocated = NOT_IN_USE;
  pbm_mem_first_available->signature = GOOD_SIGNATURE;
#endif

  /* Reset our statistics. */
  pbm_mem_bytes_requested     = 0;
  pbm_mem_bytes_allocated     = 0;
  pbm_mem_blocks_allocated    = 0;
  pbm_mem_max_bytes_allocated = 0;

  /* Used for range checks before giving the memory away. */
  pbm_mem_first_address = (unsigned long) buffer;
  pbm_mem_last_address  = (unsigned long) buffer+size;
}


/*===========================================================================*
 *              PHONE BOOK MANAGER INTERNAL UTILITY FUNCTIONS                *
 *              These function are only used internally by PBM               *
 *              and provide general utility services throughout PBM          *
 *===========================================================================*/

/*===========================================================================
FUNCTION MATCH_FIELDS

DESCRIPTION
Local utility function used by match_found() to determine whether the
field id to be searched matches the cache field id.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean match_fields(pbm_field_id_e_type data_field,
                            pbm_field_id_e_type cache_field,
                            uint32 flags,
                            pbm_field_type_e_type field_type)
{
  if ((flags & PBM_FIELDID_MASK) != PBM_FIELDID_PHONE) {
    if ((data_field == PBM_FIELD_NONE) || (data_field == cache_field))
      return TRUE;
    else
      return FALSE;
  } else {
    if (field_type == PBM_FT_PHONE)
      return TRUE;
    else
      return FALSE;
  }
}

/*===========================================================================
FUNCTION MATCH_FOUND

DESCRIPTION
Local utility function used by pbm_enum_rec_init() to determine whether the
given cache entry is a match for the given search criteria. Assumes that
data is null terminated.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean match_found(pbm_cat_e_type category,
                    pbm_field_id_e_type field_id,
                    const void *data_ptr,
                    uint16 data_size,
                    uint32 flags,
                    const pbm_addr_cache_s_type *cache_entry)
{
  int j;
  boolean match = FALSE;
  pbm_field_s_type *sfield;
  pbm_return_type ret;

  PBM_CHECK_PTR_RET(cache_entry, FALSE);

  if (cache_entry->fields == NULL)
  {
    /* We're looking at a dummy entry (deleted from cache, only there in
     * case write fails).  This can't match */
    return FALSE;
  }

  if ( (category == PBM_CAT_NONE) || (cache_entry->category == (uint16) category) )
  {
    if ((data_ptr == NULL) && (field_id == PBM_FIELD_NONE))
    {
      return TRUE;  /* if data is NULL (and any field) , we only look at category */
    }

    for (j = 0; j < cache_entry->num_fields && !match; j++)
    {
      pbm_complex_result_s_type *result = NULL;
      ret = pbm_enum_complex_init(&cache_entry->fields[j], NULL, &result);
      if (ret != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Failed complex field enumeration %d for %d",
                    ret, cache_entry->record_id, 0);
        return FALSE;
      }

      while ((sfield = pbm_enum_complex_next(result)) != NULL)
      {
        if ( match_fields(field_id, sfield->field_id, flags,
                                    sfield->field_type) )
        {
          if (data_ptr == NULL)
          {
            match = TRUE;
            break;
          }

          if (sfield->data_len != 0)
          {
            if ( (flags & PBM_DATATYPE_MASK) == PBM_DATATYPE_STR)
            {
              uint16 num_chars_given;     /* Number of chars in input */
              uint16 num_chars_in_record; /* Number of chars in record */

              /* PBM_DATATYPE_STR */
              boolean case_sensitive =
                ((flags & PBM_CASE_MASK) == PBM_CASE_SENSITIVE);

              /* If the type provided is not UCS2, we need to divide to
               * get the number of characters in the input data. */
              if ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)
                num_chars_given = data_size/2 - 1;
              else
                num_chars_given = data_size - 1;

              /* If the record we found is FT_UCS2, divide to get the
               * number of characters */
              if (sfield->field_type == PBM_FT_UCS2)
                num_chars_in_record = (sfield->data_len / 2) - 1;
              else
                num_chars_in_record = sfield->data_len - 1;

              if ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_CONTAINS)
              {
                /* PBM_SEARCHTYPE_CONTAINS */
                match = (
                    pbm_strstr(
                      pbm_get_field_data_ptr(sfield,0),
                      data_ptr,
                      case_sensitive,
                      (sfield->field_type == PBM_FT_UCS2)?TRUE:FALSE,
                      ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?
                      TRUE:FALSE) != NULL ? TRUE : FALSE);
              }
              else
              {
                /* PBM_SEARCHTYPE_STARTSWITH or PBM_SEARCHTYPE_EXACT */
                if ((flags & PBM_MATCHING_MASK) == PBM_MATCHING_NORMAL)
                {
                  /* PBM_MATCHING_NORMAL */
                  match = (pbm_strncmp(data_ptr, pbm_get_field_data_ptr(sfield,0),
                                       num_chars_given, case_sensitive,
                                       ((flags & PBM_STRTYPE_MASK) ==
                                                        PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                       (sfield->field_type ==
                                                        PBM_FT_UCS2)?TRUE:FALSE)
                                 == 0 ? TRUE : FALSE);
                }
                else
                {
                  /* PBM_MATCHING_GREATER */
                  match = (pbm_strncmp(data_ptr, pbm_get_field_data_ptr(sfield,0),
                                       num_chars_given, case_sensitive,
                                       ((flags & PBM_STRTYPE_MASK) ==
                                                     PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                       (sfield->field_type ==
                                                     PBM_FT_UCS2)?TRUE:FALSE)
                                  == 0 ? TRUE : FALSE);
                }
              }

              if (match && ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_EXACT))
                if (num_chars_in_record != num_chars_given)
                  match = FALSE;
            }
            else
            {
              /* PBM_DATATYPE_BYTE, Always EXACT */
              PBM_CHECK_PTR_RET(sfield, FALSE);
              match = ( memcmp(data_ptr, pbm_get_field_data_ptr(sfield,0),
                                        data_size) == 0 ? TRUE : FALSE );
            }
          } /* if data_len != 0 */
        }  /* if field_id*/
      } /* for each sub field */
      pbm_enum_complex_free(result);
    }  /* for each field */
  }  /* if cat*/

  return match;
}

/*===========================================================================
FUNCTION PBM_NUMBER_MATCH_INT

DESCRIPTION
  Does an exact match of a phone number.  Wild card characters match
  any digits.  All searches end at the end of either string, or a pause
  character in the SIM string.  These matches require the dialed string
  does not contain a wild card or pause character - these routines are
  meant to validate a string which was given to call manager to see if
  the call is allowed.  If fdn_comparison is TRUE, we do a starts with
  match (extra digits are allowed).

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_number_match_int(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len,
                             boolean fdn_comparison)
{
  int index;                    /* Control variable for loop. */
  boolean ret_val = TRUE;       /* Current return value. */

  /* Validate the parameters. */
  if (sim_num_len && (!sim_num || !dialed_num))
    return FALSE;

  /* For each character, compare the sim_num to the dialed_num. */
  for (index = 0; index < sim_num_len; index++)
  {
    if (sim_num && dialed_num)
    {
      /* TIME and HARDPAUSE characters will end the sim string */
      if ((sim_num[index] == PBM_TIMEPAUSE_CHAR) ||
          (sim_num[index] == PBM_HARDPAUSE_CHAR))
      {
        /* Pause characters make our searches stop there. */
        break;
      }

      /* We have more sim digits, but if we are out of dialed digits,
       * then we return that it is not a match. */
      if (dialed_num_len <= index)
      {
        ret_val = FALSE;
        break;
      }

      /* We have sim digits and dialed digits, if the SIM is a wildcard,
       * don't worry about the dialed character.. It matches wildcards. */
      if (sim_num[index] == PBM_WILD_CHAR)
        continue;

      /* If we reach here, the digits must be compared. */
      if (sim_num[index] != dialed_num[index])
      {
        ret_val = FALSE;
        break;
      }
    } else {
      PBM_MSG_ERR("sim_num or dialed num NULL, sim_num 0x%x, dialed_num 0x%x", sim_num, dialed_num, 0);
    }
  }

  if (!fdn_comparison)
  {
    /* At this point, index is the actual sim number length.  Make
     * sure this matches the dialed number length, if not, return
     * FALSE.
     */
    if (dialed_num_len != index)
      ret_val = FALSE;
  }

  return ret_val;
}


/*===========================================================================

FUNCTION: pbm_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII, but checks to make sure
  we don't overwrite memory.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, if we can do this.

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_bcd_to_ascii(__packed const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number)
{
   int bcd_index = 0;
   int ascii_index = 0;
   uint8 asc_1 = 0;
   uint8 asc_2 = 0;

   PBM_CHECK_PTR_RET(bcd_number, FALSE);
   PBM_CHECK_PTR_RET(ascii_number, FALSE);

   if (bcd_len > (PBM_MAX_NUM_LENGTH / 2))
   {
     PBM_MSG_ERR("BCD_LENGTH 0x%x maybe corrupt.", bcd_len, 0, 0);
     return FALSE;
   }

   /*****************************/
   /* International call or not */
   /*****************************/
   if ((ton_npi & CM_TON_MASK) == CM_TON_INTERNATIONAL)
   {
      ascii_number[ascii_index++] = '+';
   }

   /*************************/
   /* Decode BCD into ascii */
   /*************************/
   for( bcd_index = 0; bcd_index <= bcd_len; bcd_index++)
   {
      asc_1 = (bcd_number[bcd_index] & 0x0F);
      asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

      ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_1];
      ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_2];
   }

   /* Null terminate the ascii string */
   if (asc_2 != 0x0f)
   {
     ascii_number[ascii_index] = '\0';
   }
   return TRUE;
}

/*===========================================================================

FUNCTION: pbm_ascii_to_bcd

DESCRIPTION
  Convert the phone number from ASCII to BCD, but checks to make sure
  we don't overwrite memory. Updates number type of bcd number if the ascii
  number starts with a '+'.

DEPENDENCIES
  Boolean, if we can do this.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_ascii_to_bcd(byte *ascii_number, uint8 type, byte *bcd_number)
{
  int i, j;
  char temp[PBM_MAX_NUM_LENGTH+2] = "";  /* extra + and NULL */
  uint8 bcd_index = BCD_NUM;          /* Index into output bcd_number */
  int num_len = 0;

  PBM_CHECK_PTR_RET(bcd_number, FALSE);
  PBM_CHECK_PTR_RET(ascii_number, FALSE);

  num_len = (int) strlen((char *)ascii_number);
  if (num_len > PBM_MAX_NUM_LENGTH + 1)
  {
     PBM_MSG_ERR("Number is too long (%d)", num_len, 0, 0);
     return FALSE;
  }

  /* Null record for deletion */
  if (!num_len)
  {
    bcd_number[BCD_LEN] = 0;
    return TRUE;
  }

  /* Translate or skip over invalid characters */
  for (i = 0, j = 0; i < num_len; i++)
  {
    if (ISDIGIT(ascii_number[i]))
    {
      temp[j++] = ascii_number[i] & 0x0F;
    }
    else
    {
      switch (ascii_number[i])
      {
        case PBM_TIMEPAUSE_CHAR:
        case PBM_HARDPAUSE_CHAR:
          temp[j++] = PBM_TIMEPAUSE_BCD;
          break;

        case PBM_WILD_CHAR:
          temp[j++] = PBM_WILD_BCD;
          break;

        case PBM_EXPANSION_CHAR:
          temp[j++] = PBM_EXPANSION_BCD;
          break;

        case '#':
          temp[j++] = PBM_HASH_BCD;
          break;

        case '*':
          temp[j++] = PBM_STAR_BCD;
          break;

        default:
          /* Skipping '+' and everything else */
          break;
      }
    }
  }

  if (!j)
  {
    PBM_MSG_ERR("Invalid number (%s)", ascii_number, 0, 0);
    PBM_MSG_ERR("Invalid number", 0, 0, 0);
    return FALSE;
  }

  /* Odd number of digits must have the 0x0f at the end. */
  if (j & 0x01) temp[j++] = 0x0f;

  /* Now that temp has the bcd codes in natural order... Squish them together
   * and reverse the order per bcd coding.
   */
  for (i = 0; i < j; i+=2)
    bcd_number[bcd_index++] = ((char)(temp[i+1] << 4)) | temp[i];

  /* Update Number type/NPI byte */
  bcd_number[BCD_NT_NPI] = 0x80 | type;

  if (ascii_number[0] == '+')
  {
    bcd_number[BCD_NT_NPI] |= CM_TON_INTERNATIONAL;
  }
  else
  {
    bcd_number[BCD_NT_NPI] &= ~CM_TON_INTERNATIONAL;
  }

  /* Update length byte */
  bcd_number[BCD_LEN] = (bcd_index - 1);

  return TRUE;
}

/*===========================================================================
FUNCTION PBM_BCD_TO_BIN

DESCRIPTION
  Converts time quantities from bcd to binary.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bcd_to_bin( byte bcd )
{
  return ( (bcd >> 4) + (bcd & 0x0F) * 10 );
}

/*===========================================================================
FUNCTION PBM_BIN_TO_BCD

DESCRIPTION
  Converts time quantities from binary to bcd.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bin_to_bcd( byte bin )
{
  return ( bin/10 + ((bin % 10) << 4) );
}

/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_FIELDS

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  passed in.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_size_from_fields( const pbm_field_s_type *fields, int num_fields)
{
  int total_size=0;
  int i;

  PBM_CHECK_PTR_RET(fields, 0);

  if (num_fields > 0)
  {
    total_size = (int) ALIGNED_SIZE ((uint32) num_fields * sizeof(pbm_field_s_type));
    for (i = 0; i < num_fields; i++)
    {
      total_size += ALIGNED_SIZE ((fields)[i].data_len);
    }
  }

  return total_size;
}


/*===========================================================================
FUNCTION PBM_FREE_FIELDS_INTERNAL

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields_internal.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields_internal( pbm_field_s_type **fields, int num_fields)
{
  PBM_CHECK_PTR_RET(fields, VOID);

  if ((num_fields > 0) && (*fields != NULL))
    PBM_MEM_FREEIF(*fields);
  *fields = NULL;
}


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS_INTERNAL

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields_internal(pbm_field_s_type **fields_buf_ptr, uint32 num_fields, const uint32 *sizes)
{
  uint32 i;
  uint32 total_size = 0;
  uint32 mem_offset = 0;
  const uint32 size_one_field = sizeof(pbm_field_s_type);

  PBM_CHECK_PTR_RET(fields_buf_ptr, FALSE);
  PBM_CHECK_PTR_RET(sizes, FALSE);

  *fields_buf_ptr = NULL;
  total_size = ALIGNED_SIZE (num_fields * size_one_field);
  for (i = 0; i < num_fields; i++)
  {
    total_size += ALIGNED_SIZE (sizes[i]);
  }

  if (total_size > 0)
  {
    PBM_MEM_ALLOC(*fields_buf_ptr, total_size);
    if (*fields_buf_ptr == NULL)
    {
      return FALSE;
    }

    mem_offset = ALIGNED_SIZE(num_fields * size_one_field);
    for (i = 0; i < num_fields; i++)
    {
      // Each offset is from the start of the particular field
      // to the data associated with this field.
      // Thus, the first offset points to the start of the
      // data part of the buffer, and then each subsequent offset
      // is incremented by the size of the previous items data minus
      // the size of the previous items field header.  By doing
      // this, each field can be handled in a self contained way, and
      // accessing the data does not require knowledge of the entire
      // field memory buffer.
      (*fields_buf_ptr)[i].buffer_offset = mem_offset;
      mem_offset += ALIGNED_SIZE(sizes[i]);
      mem_offset -= size_one_field; /* take #bytes from next field to its data */
    }

    MSG_MED("PBM: blocks:%d max used:%d of %d",
        pbm_mem_blocks_allocated, pbm_mem_max_bytes_allocated,
        pbm_mem_last_address - pbm_mem_first_address);
    if (pbm_mem_bytes_allocated != 0)
    {
      MSG_MED("PBM: requested:%d alloc:%d = %d%",
          pbm_mem_bytes_requested, pbm_mem_bytes_allocated,
          pbm_mem_bytes_requested * 100 / pbm_mem_bytes_allocated);
    }
  }
  return TRUE;
}

/*===========================================================================
FUNCTION PBM_COPY_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the destination field array
  and copies the data from the source field array.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_fields(pbm_field_s_type **dst,
                        pbm_field_s_type const * const * src,
                        uint32 num_fields)
{
  uint32 total_size = 0;

  PBM_CHECK_PTR_RET(dst, FALSE);
  PBM_CHECK_PTR_RET(src, FALSE);
  PBM_CHECK_PTR_RET(*src, FALSE);

  total_size = (uint32) pbm_calculate_size_from_fields(*src, (int) num_fields);

  PBM_MEM_ALLOC(*dst, total_size);
  if (*dst == NULL)
  {
    return FALSE;
  }

  memcpy (*dst, *src, total_size);

  return TRUE;
}

/*===========================================================================
FUNCTION PBM_BUILD_NUM_TEXT_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_record_s_type record expected by
  the old software.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_num_text_record_from_fields(pbm_field_s_type *fields, int item_count,
                                           int index, pbm_num_text_record_s_type *record)
{
  int i;
  int filled_in_number = -1; /* Remember if we put a number in there. */
  char number_copy[sizeof(record->number)]; /* copy of number to store */
  uint32 number_size = 0;       /* Size of what will be stored. */
  uint32 offset = 0;
  pbm_return_type ret;
  pbm_field_s_type *sfield;

  PBM_CHECK_PTR2_RET(fields, record, VOID);

  /* Fill in the record with default values. */
  memset(number_copy, 0, sizeof(number_copy));
  memset(record, 0, sizeof(pbm_record_s_type));
  record->type = 0x80 | CM_TON_UNKNOWN | CM_NPI_ISDN ;

  /* Get the records_index. */
  record->index = index;
  for (i = 0; i < item_count; i++)
  {
    pbm_complex_result_s_type *result = NULL;
    ret = pbm_enum_complex_init(&fields[i], NULL, &result);
    if (ret != PBM_SUCCESS)
    {
      PBM_MSG_ERR("Failed complex field enumeration %d", ret , 0, 0);
      return;
    }

    while ((sfield = pbm_enum_complex_next(result)) != NULL)
    {
      PBM_CHECK_PTR_RET(sfield, VOID);
      switch(sfield->field_id)
      {
        case PBM_FIELD_NAME:
          memset(record->text, 0, sizeof(record->text));
          if (sfield->field_type == PBM_FT_UCS2)
          {
            memcpy(record->text, pbm_get_field_data_ptr(sfield,0),
                   (sfield->data_len >= sizeof(record->text))?
                   sizeof(record->text) - 1:sfield->data_len);
          }
          else
          {
            if (sfield->field_type != PBM_FT_STRING)
            {
              PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                  sfield->field_type, i, PBM_FT_STRING);
            }
            if (!pbmconvert_pbm8_to_ucs2(pbm_get_field_data_ptr(sfield,0),
                                         sfield->data_len - 1,
                                         record->text,
                                         ARR_SIZE(record->text)))
            {
              PBM_MSG_ERR("Could not convert entire string.", 0, 0, 0);
            }
          }
          break;
        case PBM_FIELD_PHONE_WORK:
        case PBM_FIELD_PHONE_HOME:
        case PBM_FIELD_PHONE_FAX:
        case PBM_FIELD_PHONE_VOICE:
        case PBM_FIELD_PHONE_PREF:
        case PBM_FIELD_PHONE_CELL:
        case PBM_FIELD_PHONE_PAGER:
        case PBM_FIELD_PHONE_GENERIC:
        case PBM_FIELD_PHONE_OTHER:
        case PBM_FIELD_HARDCODED_ECC:
        case PBM_FIELD_NV_ECC:
        case PBM_FIELD_SIM1_ECC:
        case PBM_FIELD_SIM2_ECC:
        case PBM_FIELD_NETWORK_ECC:
          if (((int) sfield->field_id > filled_in_number) &&
              (filled_in_number != PBM_FIELD_PHONE_GENERIC))
          {
            if (sfield->field_type != PBM_FT_PHONE)
              sfield->field_type = PBM_FT_PHONE;

            filled_in_number = (int) sfield->field_id;
            number_size = sfield->data_len;
            /* copy the number, so it doesn't get overwritten later */
            memset(number_copy, 0, sizeof(number_copy));
            memcpy(number_copy, pbm_get_field_data_ptr(sfield,0),
                   sizeof(number_copy));
          }
          break;
        case PBM_FIELD_GSM_TONNPI:
          if (sfield->field_type != PBM_FT_BYTE)
          {
            PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                sfield->field_type, i, PBM_FT_BYTE);
          }
          record->type = *((byte *) pbm_get_field_data_ptr(sfield,0));
          break;
        case PBM_FIELD_NUMBER_TYPE:
          if (sfield->field_type != PBM_FT_BYTE)
          {
            PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                sfield->field_type, i, PBM_FT_BYTE);
          }
          if (*(uint8 *)pbm_get_field_data_ptr(sfield,0) < ARR_SIZE(pbm_number_type_to_gsmton))
            record->type |= pbm_number_type_to_gsmton[*(uint8 *)pbm_get_field_data_ptr(sfield,0)];
          break;
        case PBM_FIELD_NUMBER_PLAN:
          if (sfield->field_type != PBM_FT_BYTE)
          {
            PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                sfield->field_type, i, PBM_FT_BYTE);
          }
          if (*(uint8 *)pbm_get_field_data_ptr(sfield,0) < ARR_SIZE(pbm_number_plan_to_gsmnpi))
            record->type |= pbm_number_plan_to_gsmnpi[*(uint8 *)pbm_get_field_data_ptr(sfield,0)];
          break;
#ifdef FEATURE_PBM_USIM_SUPPORT
        case PBM_FIELD_DATE:
          if (sfield->field_type != PBM_FT_DWORD)
          {
            PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                sfield->field_type, i, PBM_FT_DWORD);
          }
          record->time = *((uint32 *) pbm_get_field_data_ptr(sfield,0));
          break;
        case PBM_FIELD_DURATION:
          if (sfield->field_type != PBM_FT_DWORD)
          {
            PBM_MSG_ERR("Wrong field type found 0x%x at index %d expected 0x%x",
                sfield->field_type, i, PBM_FT_DWORD);
          }
          record->duration = *((uint32 *) pbm_get_field_data_ptr(sfield,0));
          break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
        case PBM_FIELD_INDEX:
          if (*((uint16 *)pbm_get_field_data_ptr(sfield,0)) != index)
          {
            int field_index = *(uint16 *)pbm_get_field_data_ptr(sfield,0);
            PBM_MSG_ERR("Given index %d does not match with index in fields"
                        "%d", index, field_index, 0);

          }
          break;

        default:    /* Ignore other fields. */
          break;
      }
    } /* for each sub field */
    pbm_enum_complex_free(result);
  } /* for each field */

  memset(record->number, 0, sizeof(record->number));
  /* if we filled something in... */
  if (filled_in_number != -1)
  {
    if (filled_in_number == PBM_FIELD_PHONE_GENERIC)
    {
      if (((record->type & CM_TON_INTERNATIONAL) == CM_TON_INTERNATIONAL) &&
          (number_copy[0] != '+'))
      {
        record->number[0] = '+';
        offset++;
      }
    }

    /* MAX_DIALED_DIGITS, plus One for the '+', and one for the NULL */
    if (number_size > (sizeof(record->number) - 2))
      number_size = (sizeof(record->number) - 2);

    memcpy(record->number + offset, number_copy, number_size);

    if (record->number[0] == '+')
      record->type |= CM_TON_INTERNATIONAL;
  }

  return;
}

/*===========================================================================
FUNCTION PBM_BUILD_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_record_s_type record expected by
  the old software.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_fields(pbm_field_s_type *fields, int item_count,
                                  int index, pbm_record_s_type *record)
{
  pbm_num_text_record_s_type new_record;

  PBM_CHECK_PTR2_RET(fields, record, VOID);

  pbm_build_num_text_record_from_fields(fields, item_count, index, &new_record);

  /* Fill in the record with default values. */
  memset(record, 0, sizeof(pbm_record_s_type));

  /* Copy in the fields from new_record to record */
  record->index = new_record.index;
  memcpy(record->number, new_record.number, sizeof(record->number));
  record->type = new_record.type;
  (void) pbmconvert_ucs2_to_pbm8(new_record.text,
                                 (uint16) pbmutils_wstrlen(new_record.text),
                                 (uint8 *) record->text, sizeof(record->text));
#ifdef FEATURE_PBM_USIM_SUPPORT
  record->time = new_record.time;
  record->duration = new_record.duration;
  record->is_answered = new_record.is_answered;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  return;
}

/*===========================================================================
FUNCTION PBM_BUILD_FIELDS_FROM_RECORD

DESCRIPTION
  This function is used for the legacy API.  It takes a pbm_record_s_type
  record and builds a record that contains an array of fields expected by
  the new interface.

===========================================================================*/
pbm_addr_cache_s_type * pbm_build_fields_from_record (pbm_device_type pb_id,
                                          pbm_record_s_type *record)
{
  pbm_addr_cache_s_type *node;
  uint32 field_lengths[6];            /* Sim records have at most 5 fields */
  uint32 build_field_lengths[6];      /* build_field_lengths are the lengths actually to build */
  PB_CACHE *cache = pbm_pb_id_to_cache(pb_id);
  uint16 index = 0;
  uint16 location = 0;
  uint16 *temp_ucs2_str;
  int j = 0;

  memset(field_lengths, 0, sizeof(field_lengths));
  memset(build_field_lengths, 0, sizeof(build_field_lengths));

  PBM_CHECK_PTR2_RET(record, cache, NULL);

  PBM_MEM_ALLOC(node, sizeof(pbm_addr_cache_s_type));
  if (node)
  {
    memset((byte *)node, 0, sizeof(pbm_addr_cache_s_type));
    switch (pb_id)
    {
      case PBM_ADN:
        node->category = (uint16) PBM_CAT_SIM_ADN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_ADN2:
        node->category = (uint16) PBM_CAT_SIM_ADN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;

      case PBM_SDN:
        node->category = (uint16) PBM_CAT_SIM_SDN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_SDN2:
        node->category = (uint16) PBM_CAT_SIM_SDN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;

      case PBM_MSISDN:
        node->category = (uint16) PBM_CAT_SIM_MSISDN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MSISDN2:
        node->category = (uint16) PBM_CAT_SIM_MSISDN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;

#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_MBDN:
        node->category = (uint16) PBM_CAT_SIM_MBDN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MBDN2:
        node->category = (uint16) PBM_CAT_SIM_MBDN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
#endif /* FEATURE_PBM_USIM_SUPPORT*/

      case PBM_MBN:
        node->category = (uint16) PBM_CAT_SIM_MBN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_MBN2:
        node->category = (uint16) PBM_CAT_SIM_MBN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;

      case PBM_FDN:
        node->category = (uint16) PBM_CAT_SIM_FDN;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_FDN2:
        node->category = (uint16) PBM_CAT_SIM_FDN2;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;

      case PBM_SDL:
        node->category = (uint16) PBM_CAT_SDL;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_EFS:
        node->category = (uint16) PBM_CAT_NONE;
        node->num_fields = PBM_SIM_NUM_FIELDS;
        break;
      case PBM_DIALED:
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_OCI:
      case PBM_OCI2:
#endif /* FEATURE_PBM_USIM_SUPPORT */
      case PBM_LND:
      case PBM_LND2:
        node->category = (uint16) PBM_CAT_OUTGOING;
        node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
        break;
      case PBM_RCVD:
#ifdef FEATURE_PBM_USIM_SUPPORT
      case PBM_ICI:
      case PBM_ICI2:
#endif /* FEATURE_PBM_USIM_SUPPORT */
        node->category = (uint16) PBM_CAT_INCOMING;
        node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
        break;
      case PBM_MISS:
        node->category = (uint16) PBM_CAT_MISSED;
        node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
        break;
      default:
        PBM_MSG_ERR("Unknown category.  Searches will be interesting. %d", pb_id, 0, 0);
        node->num_fields = PBM_SIM_NUM_FIELDS;
        node->category = (uint16) PBM_CAT_NONE;
        break;
    }

    /* Allocate 3 fields for an ADN record.
     * 1 for the Name.          ( Type string. )
     * 1 for the GSM_NUMBER     ( Type string. )
     * 1 for the GSM_TON_NPI    ( Type Byte. )
     */
    field_lengths[0] = strlen(record->text);
    if (field_lengths[0] > PBM_MAX_TEXT_LENGTH) field_lengths[0] = PBM_MAX_TEXT_LENGTH;
    if (field_lengths[0])
    {
      field_lengths[0]++; /* Room for the NULL. */
      field_lengths[0] *= sizeof(uint16); /* Make room for UCS2 characters. */
    }

    field_lengths[1] = strlen(record->number);
    if (field_lengths[1] > PBM_MAX_NUM_LENGTH) field_lengths[1] = PBM_MAX_NUM_LENGTH;
    if (field_lengths[1])
      field_lengths[1]++; /* Room for the NULL. */

    field_lengths[2] = 1;       /* GSM_TON_NPI is 1 byte always. */

    if (record->index != -1)
      field_lengths[3] = 2;     /* Index */

    if (node->num_fields == 6)
    {
      field_lengths[4] = 4;     /* The DATE is stored as a DWORD */
      field_lengths[5] = 4;     /* The DURATION is also stored as a DWORD */
    }

    for (j = 0; j < node->num_fields; j++)
    {
      if (field_lengths[j] > 0)
        build_field_lengths[index++] = field_lengths[j];
    }

    if (pbm_allocate_fields_internal(&node->fields, (uint32) index, build_field_lengths) == FALSE)
    {
      PBM_MEM_FREEIF(node);
      return NULL;
    }

    if (record->index == -1)
      node->record_id = PBM_GET_FIRST_AVAILABLE;
    else
    {
      location = (uint16) record->index;
      node->record_id = pbm_location_to_record_id(pb_id, location);
    }

    index = 0;

    if (field_lengths[0] > 0)
    {
      PBM_MEM_ALLOC(temp_ucs2_str, field_lengths[0]);
      if (temp_ucs2_str)
      {
        if (!pbmconvert_pbm8_to_ucs2((uint8 *) record->text,
                                     (uint16) strlen(record->text),
                                     temp_ucs2_str,
                                     (uint16) field_lengths[0]/sizeof(uint16)))
        {
          PBM_MSG_ERR("Could not convert entire string.", 0, 0, 0);
        }
        pbm_fill_field(&node->fields[index++], PBM_FIELD_NAME, PBM_FT_UCS2,
            (byte *) temp_ucs2_str, (uint16) field_lengths[0]);

        PBM_MEM_FREEIF(temp_ucs2_str);
      }
    }

    if (field_lengths[1] > 0)
      pbm_fill_field(&node->fields[index++], PBM_FIELD_PHONE_GENERIC, PBM_FT_PHONE,
          (byte *) record->number, (uint16) field_lengths[1]);

    pbm_fill_field(&node->fields[index++], PBM_FIELD_GSM_TONNPI,
        PBM_FT_BYTE, (byte *) &record->type, (uint16) field_lengths[2]);

    if (record->index != -1)
    {
      pbm_fill_field(&node->fields[index++], PBM_FIELD_INDEX,
          PBM_FT_WORD, (byte *) &location, (uint16) field_lengths[3]);
    }

#ifdef FEATURE_PBM_USIM_SUPPORT
    if (node->num_fields == 6)
    {
      pbm_fill_field(&node->fields[index++], PBM_FIELD_DATE,
        PBM_FT_DWORD, (byte *) &record->time, (uint16) field_lengths[4]);
      pbm_fill_field(&node->fields[index++], PBM_FIELD_DURATION,
        PBM_FT_DWORD, (byte *) &record->duration, (uint16) field_lengths[5]);
    }
#endif /* FEATURE_PBM_USIM_SUPPORT */
    node->num_fields = index;
  }

  return node;
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_build_record_from_usim_fields

DESCRIPTION
  This function adds to a record from USIM fields.  It expects to either be
  sent 1 field, or multiple consecutive group ID fields.


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_usim_fields(uint16 adn_rec_id,
                                       const pbm_field_s_type *fields,
                                       int num_fields,
                                       pbm_sim_record_s_type *record)
{
  int i;

  PBM_CHECK_PTR_RET(fields, VOID);
  PBM_CHECK_PTR_RET(record, VOID);

  switch (fields[0].field_id)
  {
    case PBM_FIELD_OTHERNAME:
    case PBM_FIELD_EMAIL:
      memcpy(record->data.text.text, pbm_get_field_data_ptr(fields,0),
             MIN(fields[0].data_len, sizeof(record->data.text.text)));
      record->data.text.adn_record_id = adn_rec_id;
      if (num_fields > 1) {
        PBM_MSG_ERR("Too many fields sent to pbm_build_record_from_usim_fields() %d %d",
                    num_fields, fields[0].field_id, 0);
      }
      break;

    case PBM_FIELD_ADDITIONAL_NUMBER:
      record->data.anr.anr_id = 0;
      record->data.anr.adn_record_id = adn_rec_id;
      record->data.anr.type = 0x80 | CM_TON_UNKNOWN | CM_NPI_ISDN;
      memcpy(record->data.anr.number, pbm_get_field_data_ptr(fields,0),
             MIN(PBM_NUM_SIZE, fields[0].data_len));
      if (record->data.anr.number[0] == '+')
        record->data.anr.type |= CM_TON_INTERNATIONAL;
      if (num_fields > 1) {
        PBM_MSG_ERR("Too many fields sent to pbm_build_record_from_usim_fields() %d %d",
                    num_fields, fields[0].field_id, 0);
      }
      break;

    case PBM_FIELD_GRPNAME_ID:
      for ( i = 0; i < num_fields; i++) {
        PBM_CHECK_PTR_RET(fields,VOID);
        record->data.grp.group_id[i] = *((uint8 *)pbm_get_field_data_ptr(fields,i));
        if (fields[i].field_id != PBM_FIELD_GRPNAME_ID)
        {
          PBM_MSG_ERR("Not a groupname field %d %d %d",
                      i, fields[i].field_id, num_fields);
        }
      }

      break;

    default:
      break;
  }
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/* <EJECT> */

/*===========================================================================
FUNCTION CHECK_PB_READY

DESCRIPTION
Checks if the phonebook is ready or not and returns the appropriate PBM
return value. Any value other than PBM_SUCCESS means the phonebook is not
available for use.

Checking for PBM_DEFAULT returns an ERROR

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type check_pb_ready(pbm_device_type pb_id)
{
  pbm_return_type ret;
  PB_CACHE *cache;

  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
    return PBM_ERROR;

  switch(cache->status)
  {
    case PBM_STATUS_READY:
      ret = PBM_SUCCESS;
      break;

    case PBM_STATUS_NOT_INIT:
      if (pbm_action_requires_sim(pb_id))
        ret = PBM_ERROR_NO_SIM;
      else
        ret = PBM_ERROR_NOT_READY;
      break;

    case PBM_STATUS_SYNC:
      if (rex_self() == &pbm_tcb)
        ret = PBM_SUCCESS;
      else
        ret = PBM_ERROR_SYNC;
      break;

    case PBM_STATUS_PIN_REQ: ret = PBM_ERROR_SIM_PIN_REQ; break;
    case PBM_STATUS_PUC_REQ: ret = PBM_ERROR_SIM_PUC_REQ; break;
    case PBM_STATUS_PIN2_REQ: ret = PBM_ERROR_PIN2_REQ; break;
    case PBM_STATUS_PUC2_REQ: ret = PBM_ERROR_PUC2_REQ; break;
    case PBM_STATUS_SIM_FAIL: ret = PBM_ERROR_SIM_FAIL; break;
    case PBM_STATUS_NOT_AVAILABLE: ret = PBM_ERROR_NOT_AVAILABLE; break;
    case PBM_STATUS_RESTRICTED: ret = PBM_ERROR_RESTRICTED; break;

    case PBM_STATUS_NOT_READY:
    default:
      ret = PBM_ERROR_NOT_READY;
      break;
  }

  return ret;
}

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_file_id_to_pb_id(pbm_slot_type sim_id,
                                     pbm_file_device_e_type file_id)
{
  pbm_device_type pb_id = PBM_DEFAULT;

  switch (file_id)
  {
    case PBM_FILE_ADN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_ADN : PBM_ADN2);
      break;

    case PBM_FILE_FDN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_FDN : PBM_FDN2);
      break;

    case PBM_FILE_LND:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_LND : PBM_LND2);
      break;

    case PBM_FILE_SDN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_SDN : PBM_SDN2);
      break;

    case PBM_FILE_MSISDN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_MSISDN : PBM_MSISDN2);
      break;

    case PBM_FILE_MBN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_MBN : PBM_MBN2);
      break;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_ICI:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_ICI : PBM_ICI2);
      break;

    case PBM_FILE_OCI:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_OCI : PBM_OCI2);
      break;

    case PBM_FILE_MBDN:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_MBDN : PBM_MBDN2);
      break;
#endif /* FEATURE_PBM_USIM_SUPPORT */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_FILE_ECC:
      pb_id = (sim_id == PBM_SLOT_1 ? PBM_ECC : PBM_ECC2);
      break;
#endif
    default:
      PBM_MSG_ERR("pbm_file_id_to_pb_id called for invalid file %d", file_id, 0, 0);
      break;
  }

  return pb_id;
}

/*===========================================================================
FUNCTION PBM_FILE_CIRCULAR

DESCRIPTION
  This function returns TRUE if the init file is circular, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
boolean pbm_file_circular(pbm_file_device_e_type file_id)
{
  switch(file_id)
  {
    case PBM_FILE_LND:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_OCI:
    case PBM_FILE_ICI:
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return TRUE;

    default:
      return FALSE;
  }
}
#endif /* FEATURE_MMGSDI */

/* <EJECT> */
/*==============================================================================

FUNCTION NAME pbm_action_requires_sim

DESCRIPTION
   This function is called to determine if an action on a given pb_id
   requires the SIM to be fully initialized.  The purpose of this call
   is to prohibit reads and writes to memory while the SIM is still
   initializing.

RETURN VALUE
  TRUE: If the action would require the SIM.
  FALSE: The pb_id does not touch the UIM.

DEPENDENCIES
  None
==============================================================================*/
boolean pbm_action_requires_sim(pbm_device_type pb_id)
{
  switch (pb_id)
  {
#ifdef FEATURE_MMGSDI
    case PBM_DEFAULT:
#endif
    case PBM_ADN:                 /* Abbreviated Dialing Number list */
    case PBM_FDN:                 /* Fixed Dialing Number list */
    case PBM_LND:                 /* SIM/RUIM Last Number Dialed list */
    case PBM_SDN:                 /* Service Dialling Number */
    case PBM_MSISDN:              /* MSISDN */
    case PBM_MBN:                 /* MBN */
    case PBM_ADN2:                /* Abbreviated Dialing Number list */
    case PBM_FDN2:                /* Data Services Fixed Dialing Number list */
    case PBM_LND2:                /* SIM/RUIM Last Number Dialed list */
    case PBM_SDN2:                /* Service Dialling Number */
    case PBM_MSISDN2:             /* MSISDN */
    case PBM_MBN2:                /* MBN */
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC:                 /* Emergency Call Code (Emergency number list) */
    case PBM_ECC2:                /* Emergency Call Code (Emergency number list) */
#endif
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_OCI:                 /* USIM Outgoing Call Information file.  */
    case PBM_ICI:                 /* USIM Incoming Call Information file.  */
    case PBM_OCI2:                /* USIM Outgoing Call Information file.  */
    case PBM_ICI2:                /* USIM Incoming Call Information file.  */
    case PBM_MBDN:                /* MBDN */
    case PBM_MBDN2:               /* MBDN */
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return TRUE;
    default:
      return FALSE;
  }
}

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache type associated with a given file id

SIDE EFFECTS
===========================================================================*/
pbm_cache_type_e_type pbm_file_id_to_cache_type(pbm_file_device_e_type file_id)
{
  switch (file_id)
  {
    case PBM_FILE_ADN:
    case PBM_FILE_FDN:
    case PBM_FILE_SDN:
    case PBM_FILE_LND:
    case PBM_FILE_MSISDN:
    case PBM_FILE_MBN:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_OCI:
    case PBM_FILE_ICI:
    case PBM_FILE_MBDN:
    case PBM_FILE_ADN1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ADN2:
    case PBM_FILE_ADN3:
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return PBM_CACHE_TYPE_PB;
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_FILE_ECC:
      return PBM_CACHE_TYPE_ECC;
#endif

    case PBM_FILE_EXT1:
    case PBM_FILE_EXT2:
    case PBM_FILE_EXT3:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_EXT5:
    case PBM_FILE_EXT6:
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return PBM_CACHE_TYPE_EXT;
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_EMAIL:
    case PBM_FILE_SNE:
    case PBM_FILE_GRP:
    case PBM_FILE_ANR:
    case PBM_FILE_ANRA:
    case PBM_FILE_ANRB:
    case PBM_FILE_ANRC:
    case PBM_FILE_EMAIL1:
    case PBM_FILE_SNE1:
    case PBM_FILE_GRP1:
    case PBM_FILE_ANR1:
    case PBM_FILE_ANRA1:
    case PBM_FILE_ANRB1:
    case PBM_FILE_ANRC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_EMAIL2:
    case PBM_FILE_SNE2:
    case PBM_FILE_GRP2:
    case PBM_FILE_ANR2:
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRC2:
    case PBM_FILE_EMAIL3:
    case PBM_FILE_SNE3:
    case PBM_FILE_GRP3:
    case PBM_FILE_ANR3:
    case PBM_FILE_ANRA3:
    case PBM_FILE_ANRB3:
    case PBM_FILE_ANRC3:
#endif
      return PBM_CACHE_TYPE_USIM;

    case PBM_FILE_IAP:
    case PBM_FILE_IAP1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_IAP2:
    case PBM_FILE_IAP3:
#endif
      return PBM_CACHE_TYPE_INDEX;

    case PBM_FILE_PSC:
    case PBM_FILE_PUID:
    case PBM_FILE_CC:
      return PBM_CACHE_TYPE_SYNC1;

    case PBM_FILE_UID:
    case PBM_FILE_UID1:
    case PBM_FILE_PBC:
    case PBM_FILE_PBC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_UID2:
    case PBM_FILE_UID3:
    case PBM_FILE_PBC2:
    case PBM_FILE_PBC3:
#endif
      return PBM_CACHE_TYPE_SYNC2;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    default:
      PBM_MSG_ERR("Unknown file id %d", file_id, 0, 0);
      return PBM_CACHE_TYPE_UNKNOWN;
  }
}

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given file id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
void *pbm_file_id_to_cache(pbm_slot_type sim_id,
                           pbm_file_device_e_type file_id)
{
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_pb_set_type pb_set;
#endif /* FEATURE_PBM_USIM_SUPPORT */

  PBM_VALIDATE_SLOT_RET(sim_id, NULL);

  switch(file_id)
  {
    case PBM_FILE_ADN:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_ADN1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ADN2:
    case PBM_FILE_ADN3:
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return &adn_cache[sim_id];

    case PBM_FILE_FDN:
      return &fdn_cache[sim_id];

    case PBM_FILE_SDN:
      return &sdn_cache[sim_id];

    case PBM_FILE_LND:
      return &lnd_cache[sim_id];

    case PBM_FILE_MSISDN:
      return &msisdn_cache[sim_id];

    case PBM_FILE_MBN:
      return &mbn_cache[sim_id];

    case PBM_FILE_EXT1:
      return &ext1_cache[sim_id];

    case PBM_FILE_EXT2:
      return &ext2_cache[sim_id];

    case PBM_FILE_EXT3:
      return &ext3_cache[sim_id];
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_FILE_ECC:
      return &ecc_sim_cache[sim_id];
#endif

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_OCI:
      return &oci_cache[sim_id];

    case PBM_FILE_ICI:
      return &ici_cache[sim_id];

    case PBM_FILE_MBDN:
      return &mbdn_cache[sim_id];

    case PBM_FILE_EXT5:
      return &ext5_cache[sim_id];

    case PBM_FILE_EXT6:
      return &ext6_cache[sim_id];

    case PBM_FILE_PSC:
      return &psc_cache[sim_id];

    case PBM_FILE_PUID:
      return &puid_cache[sim_id];

    case PBM_FILE_CC:
      return &cc_cache[sim_id];

    case PBM_FILE_EMAIL:
    case PBM_FILE_EMAIL1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_EMAIL2:
    case PBM_FILE_EMAIL3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &email_cache[sim_id][pb_set];

    case PBM_FILE_SNE:
    case PBM_FILE_SNE1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_SNE2:
    case PBM_FILE_SNE3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &sne_cache[sim_id][pb_set];

    case PBM_FILE_GRP:
    case PBM_FILE_GRP1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_GRP2:
    case PBM_FILE_GRP3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &grp_cache[sim_id][pb_set];

    case PBM_FILE_ANR:
    case PBM_FILE_ANR1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ANR2:
    case PBM_FILE_ANR3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &anr_cache[sim_id][PBM_USIM_SAMEFILE_INDEX(pb_set, 0)];

    case PBM_FILE_ANRA:
    case PBM_FILE_ANRA1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRA3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &anr_cache[sim_id][PBM_USIM_SAMEFILE_INDEX(pb_set, 1)];

    case PBM_FILE_ANRB:
    case PBM_FILE_ANRB1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRB3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &anr_cache[sim_id][PBM_USIM_SAMEFILE_INDEX(pb_set, 2)];

    case PBM_FILE_ANRC:
    case PBM_FILE_ANRC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ANRC2:
    case PBM_FILE_ANRC3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &anr_cache[sim_id][PBM_USIM_SAMEFILE_INDEX(pb_set, 3)];

    case PBM_FILE_IAP:
    case PBM_FILE_IAP1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_IAP2:
    case PBM_FILE_IAP3:
#endif
      pb_set = pbm_file_id_to_pb_set(file_id);
      PBM_CHECK_PBSET_RET(pb_set, NULL);
      return &adn_m_usim_cache[sim_id][pb_set];

    case PBM_FILE_UID:
    case PBM_FILE_UID1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_UID2:
    case PBM_FILE_UID3:
#endif
      return &uid_cache[sim_id];

    case PBM_FILE_PBC:
    case PBM_FILE_PBC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_PBC2:
    case PBM_FILE_PBC3:
#endif
      return &pbc_cache[sim_id];
#endif /* FEATURE_PBM_USIM_SUPPORT */
    default:
      PBM_MSG_ERR("Unknown file id %d", file_id, 0, 0);
      return NULL;
  }
}

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_SET

DESCRIPTION
  Returns the proper pb set based on the given file id.

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_file_id_to_pb_set(pbm_file_device_e_type file_id)
{
  switch(file_id)
  {
    case PBM_FILE_ADN:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_EMAIL:
    case PBM_FILE_SNE:
    case PBM_FILE_GRP:
    case PBM_FILE_ANR:
    case PBM_FILE_ANRA:
    case PBM_FILE_ANRB:
    case PBM_FILE_ANRC:
    case PBM_FILE_IAP:
    case PBM_FILE_UID:
    case PBM_FILE_PBC:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
       case PBM_FILE_ECC:
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return PBM_USIM_PB_ONE;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_ADN1:
    case PBM_FILE_EMAIL1:
    case PBM_FILE_SNE1:
    case PBM_FILE_GRP1:
    case PBM_FILE_ANR1:
    case PBM_FILE_ANRA1:
    case PBM_FILE_ANRB1:
    case PBM_FILE_ANRC1:
    case PBM_FILE_IAP1:
    case PBM_FILE_UID1:
    case PBM_FILE_PBC1:
      return PBM_USIM_PB_TWO;

#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ADN2:
    case PBM_FILE_EMAIL2:
    case PBM_FILE_SNE2:
    case PBM_FILE_GRP2:
    case PBM_FILE_ANR2:
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRC2:
    case PBM_FILE_IAP2:
    case PBM_FILE_UID2:
    case PBM_FILE_PBC2:
      return PBM_USIM_PB_THREE;

    case PBM_FILE_ADN3:
    case PBM_FILE_EMAIL3:
    case PBM_FILE_SNE3:
    case PBM_FILE_GRP3:
    case PBM_FILE_ANR3:
    case PBM_FILE_ANRA3:
    case PBM_FILE_ANRB3:
    case PBM_FILE_ANRC3:
    case PBM_FILE_IAP3:
    case PBM_FILE_UID3:
    case PBM_FILE_PBC3:
      return PBM_USIM_PB_FOUR;
#endif
#endif /* FEATURE_PBM_USIM_SUPPORT */

    default:
      PBM_MSG_ERR("Unexpected file id %d", file_id, 0, 0);
      return PBM_USIM_PB_INVALID;
  }
}

/*===========================================================================
FUNCTION PBM_LOCATION_TO_ADN_REC_ID

DESCRIPTION
  Returns the ADN record id given the sim id, phonebook set and location
  of ADN record.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_adn_rec_id(pbm_slot_type sim_id,
                                  pbm_pb_set_type pb_set,
                                  uint8 index)
{
  pbm_device_type pb_id;
  PB_CACHE *pb_cache;
  int location;

  PBM_VALIDATE_SLOT_RET(sim_id, PBM_INVALID_REC_ID);

  PBM_CHECK_PBSET_RET(pb_set, PBM_INVALID_REC_ID);

  if (index == 0xFF)
    return PBM_INVALID_REC_ID;

  pb_id = pbm_file_id_to_pb_id(sim_id, PBM_FILE_ADN);
  pb_cache = pbm_pb_id_to_cache(pb_id);
  PBM_CHECK_PTR_RET(pb_cache, PBM_INVALID_REC_ID);

  if (!pb_cache->num_of_records || !pb_cache->num_files ||
      index > pb_cache->num_of_records)
  {
    PBM_MSG_ERR("Invalid values cache 0x%x, index %d, files %d",
                 pb_cache, index, pb_cache->num_files);
    return PBM_INVALID_REC_ID;
  }

  location = index;
#ifdef FEATURE_PBM_USIM_SUPPORT
#ifdef FEATURE_USIM_1000_PBM
  if (pb_set >= PBM_USIM_PB_FOUR)
    location += (pb_cache->records_in_pb_set[2]);
  if (pb_set >= PBM_USIM_PB_THREE)
    location += (pb_cache->records_in_pb_set[1]);
#endif
  if (pb_set >= PBM_USIM_PB_TWO)
    location += (pb_cache->records_in_pb_set[0]);
#endif /* FEATURE_PBM_USIM_SUPPORT */

  return pbm_location_to_record_id(pb_id, location);
}

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_FIELD_ID

DESCRIPTION
  Returns the field id stored within the given file device.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_file_id_to_field_id(pbm_file_device_e_type file_id)
{
  switch (file_id)
  {
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_FILE_EMAIL:
    case PBM_FILE_EMAIL1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_EMAIL2:
    case PBM_FILE_EMAIL3:
#endif
      return PBM_FIELD_EMAIL;

    case PBM_FILE_SNE:
    case PBM_FILE_SNE1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_SNE2:
    case PBM_FILE_SNE3:
#endif
      return PBM_FIELD_OTHERNAME;

    case PBM_FILE_GRP:
    case PBM_FILE_GRP1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_GRP2:
    case PBM_FILE_GRP3:
#endif
      return PBM_FIELD_GRPNAME_ID;

    case PBM_FILE_ANR:
    case PBM_FILE_ANRA:
    case PBM_FILE_ANRB:
    case PBM_FILE_ANRC:
    case PBM_FILE_ANR1:
    case PBM_FILE_ANRA1:
    case PBM_FILE_ANRB1:
    case PBM_FILE_ANRC1:
#ifdef FEATURE_USIM_1000_PBM
    case PBM_FILE_ANR2:
    case PBM_FILE_ANRA2:
    case PBM_FILE_ANRB2:
    case PBM_FILE_ANRC2:
    case PBM_FILE_ANR3:
    case PBM_FILE_ANRA3:
    case PBM_FILE_ANRB3:
    case PBM_FILE_ANRC3:
#endif
      return PBM_FIELD_ADDITIONAL_NUMBER;
#endif /* FEATURE_PBM_USIM_SUPPORT */
    default:
      return PBM_FIELD_NONE;
  }
}
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_CHECK

DESCRIPTION
  This function does all the initial error checking for a record write
  operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
//lint -e{715} suppress unused parameters (cat, proc_func)
pbm_return_type pbm_record_write_check(pbm_device_type pb_id,
                                  const uint16 *rec_id_ptr,
                                  uint16 cat,
                                  const pbm_field_s_type *fields,
                                  int num_fields,
                                  PBM_WRITE_CB_FUNC proc_func)
{
  PB_CACHE *cache;
  boolean deletion;
  pbm_return_type ret;

  if (num_fields < 0)
    return PBM_ERROR;

  /* Validate the parameters. */
  PBM_CHECK_PTR_RET(rec_id_ptr, PBM_ERROR);

  /* The ECC is currently read only. */
  if (pb_id == PBM_ECC)
  {
    return PBM_ERROR_NOT_SUPPORTED;
  }

  /* We have the cache, make sure this one is ready to go. */
  if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS) {
    return ret;
  }

  /* Get cache */
  if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
  {
    return PBM_ERROR;
  }

  /* We can only write to the first available record for circular files. */
  /* SIM records, I can only write to the top of the circular file, for
   * EFS records, we currently assume the last record written is the
   * latest when initializing the stack, thus, only the first available
   * there can be written.  This is also true with deleting records,
   * but deleting the "FIRST_AVAILABLE", the oldest records will be
   * pushed out of the circular file.  This is true both for the SIM
   * and for EFS.
   */
  deletion = ((num_fields == 0 || fields == NULL) ? TRUE : FALSE);

  if (*rec_id_ptr == PBM_GET_FIRST_AVAILABLE)
  {
    if (deletion && !cache->circular)
    {
      return PBM_ERROR_INDEX_INVALID;
    }
  }
  else
  {
    if (cache->circular)
    {
      return PBM_ERROR_INDEX_INVALID;
    }
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_COMPLEX_GET_SUB_FIELD

DESCRIPTION
  This function obtains the sub field for a given index. Index in this case
  is 0-based.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_sub_field_s_type *pbm_complex_get_sub_field(pbm_complex_field_s_type *cfield,
                                                int index)
{
  int i;
  pbm_sub_field_s_type *sub_field;

  PBM_CHECK_PTR_RET(cfield, NULL);

  if (index >= cfield->num_subfields)
    return NULL;

  sub_field = PBM_FIELD_ALIGN(cfield->buf);

  for (i = 0; i < index; ++i)
    sub_field = PBM_NEXT_SUB_FIELD(sub_field);

  return sub_field;
}

/*===========================================================================
FUNCTION PBM_COMPLEX_UNWRAP

DESCRIPTION
  This function converts a complex field into its constituent simple fields and
  adds it to the enumeration list.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_complex_unwrap(
                                  pbm_complex_field_s_type *complex_field,
                                  pbm_complex_result_s_type *result)
{
  pbm_return_type ret;
  pbm_field_s_type *field;
  pbm_sub_field_s_type *sub_field;
  int i;

  PBM_CHECK_PTR_RET(complex_field, PBM_ERROR);
  PBM_CHECK_PTR_RET(result, PBM_ERROR);

  sub_field = PBM_FIELD_ALIGN(complex_field->buf);

  for ( i = 0; i < complex_field->num_subfields; i++)
  {
    if (sub_field->field_type == PBM_FT_COMPLEX)
    {
      ret = pbm_complex_unwrap(PBM_FIELD_ALIGN(sub_field->buf), result);
      if (ret != PBM_SUCCESS)
      {
        PBM_MSG_ERR("Unable to unwrap 0x%x", PBM_FIELD_ALIGN(sub_field->buf),
                                           0, 0);
        return ret;
      }
    }
    else
    {
      if (result->enumerator >= result->num_fields)
        return PBM_ERROR;

      field = &result->fields[result->enumerator++];

      field->field_id = sub_field->field_id;
      field->field_type = sub_field->field_type;
      field->data_len = sub_field->data_len;
      memcpy(pbm_get_field_data_ptr(field,0), (uint8*)sub_field->buf, sub_field->data_len);
    }

    sub_field = PBM_NEXT_SUB_FIELD(sub_field);
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_COMPLEX_NUM_SUB_FIELDS

DESCRIPTION
  This function returns the total number of sub fields within a complex
  field. If there are complex fields within complex fields, only the sub
  fields (or the leaves) are counted.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
static int pbm_complex_num_sub_fields(pbm_complex_field_s_type *complex_field)
{
  int num = 0;
  int i;
  pbm_sub_field_s_type *sub_field;

  PBM_CHECK_PTR_RET(complex_field, 0);

  sub_field = PBM_FIELD_ALIGN(complex_field->buf);

  for (i = 0; i < complex_field->num_subfields; i++)
  {
    if (sub_field->field_id == PBM_FT_COMPLEX)
      num += pbm_complex_num_sub_fields(PBM_FIELD_ALIGN(sub_field->buf));
    else
      num++;
    sub_field = PBM_NEXT_SUB_FIELD(sub_field);
  }

  return num;
}

/*===========================================================================
FUNCTION PBM_COMPLEX_FILL_SIZES

DESCRIPTION
  This function fills a sizes array, so that a normal fields structure can
  be created with pbm_allocate_fields().

RETURN VALUE
  number of filled fields.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
static int pbm_complex_fill_sizes(uint32 *sizes,
                                  int max_num_sub_fields,
                                  pbm_complex_field_s_type *complex_field)
{
  int i;
  pbm_sub_field_s_type *sub_field;
  int filled;
  int total_filled = 0;

  PBM_CHECK_PTR_RET(sizes, 0);
  PBM_CHECK_PTR_RET(complex_field, 0);

  sub_field = PBM_FIELD_ALIGN(complex_field->buf);

  for (i = 0; (i < complex_field->num_subfields) && (max_num_sub_fields > 0); i++)
  {
    if (sub_field->field_type == PBM_FT_COMPLEX)
    {
      filled = pbm_complex_fill_sizes(sizes,
                                      max_num_sub_fields,
                                      (pbm_complex_field_s_type *)sub_field->buf); //lint !e826 suspicious ptr conversion
      sizes += filled;
      max_num_sub_fields -= filled;
      total_filled += filled;
    }
    else
    {
      *sizes = sub_field->data_len;
      sizes++;
      max_num_sub_fields--;
      total_filled++;
    }

    sub_field = PBM_NEXT_SUB_FIELD(sub_field);
  }

  return total_filled;
}

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_INIT

DESCRIPTION
  This function initializes enumeration of a complex field. If there are
  complex fields within complex fields, all of them are converted into
  individual PBM fields.

  If the input field is a simple field, the simple field is returned as
  enumeration.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_complex_init(const pbm_field_s_type *field,
                                      int *pnum_fields,
                                      pbm_complex_result_s_type **presult)
{
  int num_sub_fields;
  int returned_val;
  pbm_return_type ret;
  pbm_complex_result_s_type *result = NULL;
  uint32 *sizes;

  PBM_CHECK_PTR_RET(field, PBM_ERROR);
  PBM_CHECK_PTR_RET(presult, PBM_ERROR);

  *presult = NULL;

  PBM_MEM_ALLOC(result, sizeof(pbm_complex_result_s_type));
  PBM_CHECK_PTR_RET(result, PBM_ERROR);
  memset(result, 0, sizeof(pbm_complex_result_s_type));

  /* Get number of fields */
  if (field->field_type == PBM_FT_COMPLEX)
  {
    num_sub_fields = pbm_complex_num_sub_fields(
            (pbm_complex_field_s_type *) pbm_get_field_data_ptr(field,0));
    PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
    if (sizes == NULL)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }

    returned_val = pbm_complex_fill_sizes(sizes,
                                 num_sub_fields,
                                 (pbm_complex_field_s_type *)
                                          pbm_get_field_data_ptr(field,0));
    if (returned_val != num_sub_fields)
    {
      PBM_MSG_ERR("pbm_complex_fill_sizes() didn't fill all subfields %d %d",
                  returned_val, num_sub_fields, 0);
      PBM_MEM_FREEIF(sizes);
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }
  }
  else
  {
    num_sub_fields = 1;

    PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
    if (sizes == NULL)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }

    *sizes = field->data_len;
  }

  result->num_fields = num_sub_fields;

  /* Allocate memory */
  if (!pbm_allocate_fields_internal(&result->fields, (uint32) num_sub_fields,
                                    sizes))
  {
    PBM_MEM_FREEIF(sizes);
    pbm_enum_complex_free(result);
    return PBM_ERROR;
  }

  PBM_MEM_FREEIF(sizes);

  /* Read fields */
  if (field->field_id == PBM_FIELD_COMPLEX)
  {
    ret = pbm_complex_unwrap((pbm_complex_field_s_type *)
                             pbm_get_field_data_ptr(field,0), result);
    if (ret != PBM_SUCCESS)
    {
      pbm_enum_complex_free(result);
      return PBM_ERROR;
    }
  }
  else
  {
    pbm_fill_field(&result->fields[result->enumerator++],
                   field->field_id,
                   field->field_type,
                   pbm_get_field_data_ptr(field,0),
                   field->data_len);
  }

  if (pnum_fields)
    *pnum_fields = num_sub_fields;

  result->enumerator = 0;

  *presult = result;

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_NEXT

DESCRIPTION
  This function obtains the next sub field within a complex field, converts it
  to a normal PBM field and returns it.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_s_type *pbm_enum_complex_next(pbm_complex_result_s_type *result)
{
  if (!result)
    return NULL;

  if (result->enumerator >= result->num_fields)
    return NULL;
  else
    return &result->fields[result->enumerator++];
}

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_FREE

DESCRIPTION
  This function frees the memory allocated to store the search result.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_enum_complex_free(pbm_complex_result_s_type *result)
{
  PBM_CHECK_PTR_RET(result, VOID);

  PBM_MEM_FREEIF(result->fields);
  result->num_fields = 0;
  result->enumerator = 0;

  PBM_MEM_FREEIF(result);
}

/*===========================================================================
FUNCTION PBM_CAT_TO_PHONEBOOK

DESCRIPTION
  This function returns the phonebook associated with a given category.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_cat_to_phonebook(pbm_cat_e_type cat)
{
  switch(cat)
  {
    case PBM_CAT_BUSINESS:
    case PBM_CAT_PERSONAL:
      return PBM_EFS;

    case PBM_CAT_SIM_ADN:
      return PBM_ADN;

    case PBM_CAT_SIM_FDN:
      return PBM_FDN;

    case PBM_CAT_SIM_BDN:
      return PBM_BDN;

    case PBM_CAT_SDL:
      return PBM_SDL;

    case PBM_CAT_INCOMING:
      return PBM_RCVD;

    case PBM_CAT_OUTGOING:
      return PBM_DIALED;

    case PBM_CAT_MISSED:
      return PBM_MISS;

    case PBM_CAT_SIM_ADN2:
      return PBM_ADN2;

    case PBM_CAT_SIM_FDN2:
      return PBM_FDN2;

    case PBM_CAT_SIM_BDN2:
      return PBM_BDN2;

    case PBM_CAT_SIM_SDN:
      return PBM_SDN;

    case PBM_CAT_SIM_SDN2:
      return PBM_SDN2;

    case PBM_CAT_SIM_MSISDN:
      return PBM_MSISDN;

    case PBM_CAT_SIM_MSISDN2:
      return PBM_MSISDN2;

    case PBM_CAT_SIM_MBN:
      return PBM_MBN;

    case PBM_CAT_SIM_MBN2:
      return PBM_MBN2;

    case PBM_CAT_ECC:
      return PBM_ECC;

#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_CAT_SIM_MBDN:
      return PBM_MBDN;

    case PBM_CAT_SIM_MBDN2:
      return PBM_MBDN2;
#endif /* FEATURE_PBM_USIM_SUPPORT*/

    default:
      PBM_MSG_ERR("Unexpected category %d", cat, 0, 0);
      return PBM_DEFAULT;
  }
}

/*===========================================================================
FUNCTION PBM_PB_ID_TO_FIELDGROUP

DESCRIPTION
  This function returns the field info group corresponding to this phonebook.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_pb_id_to_fieldgroup(pbm_device_type pb_id)
{
  switch (pb_id)
  {
    case PBM_FDN:
    case PBM_BDN:
    case PBM_SDN:
    case PBM_MSISDN:
    case PBM_MBN:
    case PBM_FDN2:
    case PBM_BDN2:
    case PBM_SDN2:
    case PBM_MSISDN2:
    case PBM_MBN2:
#ifdef FEATURE_PBM_USIM_SUPPORT
  case PBM_MBDN:
  case PBM_MBDN2:
#endif /* FEATURE_PBM_USIM_SUPPORT*/
      return SIM_GROUP;

    case PBM_ADN:
    case PBM_ADN2:
      return ADN_GROUP;

    case PBM_DIALED:
    case PBM_RCVD:
    case PBM_MISS:
    case PBM_LND:
    case PBM_LND2:
#ifdef FEATURE_PBM_USIM_SUPPORT
    case PBM_OCI:
    case PBM_ICI:
    case PBM_OCI2:
    case PBM_ICI2:
#endif /* FEATURE_PBM_USIM_SUPPORT */
      return CALLHISTORY_GROUP;

    case PBM_EFS:
    case PBM_DEFAULT:
      return EFS_GROUP;

    case PBM_SDL:
      return SDL_GROUP;

    case PBM_ECC:
#ifdef FEATURE_ECC_HANDLING_AT_PBM
    case PBM_ECC2:
#endif
      return ECC_GROUP;

    default:
      PBM_MSG_ERR("Unexpected phonebook device %d", pb_id, 0, 0);
      return INVALID_GROUP;
  }
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_IS_USIM_FIELD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_field(pbm_field_id_e_type fid)
{
  switch (fid)
  {
    case PBM_FIELD_ADDITIONAL_NUMBER:
    case PBM_FIELD_GRPNAME_ID:
    case PBM_FIELD_OTHERNAME:
    case PBM_FIELD_EMAIL:
      return TRUE;

    default:
      return FALSE;
  }
}

/*===========================================================================
FUNCTION pbm_is_usim_required

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_required(uint16 rec_id, const pbm_field_s_type *fields,
                             int num_fields)
{
  int i;
  pbm_device_type pb_id;

  if (!fields || !num_fields)
    return FALSE;

  pb_id = pbm_record_id_to_phonebook(rec_id);

  if (pbm_action_requires_sim(pb_id))
  {
    for (i = 0; i < num_fields; i++)
      if (pbm_is_usim_field(fields[i].field_id))
        return TRUE;
  }

  return FALSE;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_get_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_get_usim_index(uint16 rec_id,
                              const pbm_field_s_type *fields, int num_fields,
                              uint8 *index_array)
{
  PB_USIM_CACHE *usim_cache;
  PB_CACHE *pb_cache;
  int i, j;
  boolean *cache_in_use;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  int location;
  int adn_index = 0;
  int index;
  int num_grp_ids = 0;
  PB_USIM_CACHE *group_cache = NULL;
  boolean ret = TRUE;
  pbm_pb_set_type pb_set;

  PBM_CHECK_PTR_RET(index_array, FALSE);

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  PBM_CHECK_PTR_RET(pb_cache, FALSE);

  pb_set = pbm_record_id_to_pb_set(rec_id);
  PBM_CHECK_PBSET_RET(pb_set, FALSE);
  m_usim_cache = pb_cache->m_usim_cache[pb_set];
  PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

  PBM_MEM_ALLOC(cache_in_use,
                (uint32) (m_usim_cache->num_caches+1) * sizeof(boolean));
  PBM_CHECK_PTR_RET(cache_in_use, FALSE);
  memset(cache_in_use, 0,
         (uint32)(m_usim_cache->num_caches+1) * sizeof(boolean));

  location = pbm_record_id_to_location(rec_id);
  adn_index = pbm_pb_record_index_to_fileloc(pb_cache, location);
  if (adn_index > pb_cache->num_of_records)
  {
    PBM_MSG_ERR("Invalid adn_index %d based on location %d", adn_index,
                location, pb_cache->num_of_records);
    PBM_MEM_FREEIF(cache_in_use);
    return FALSE;
  }

  for (i = 0; i < num_fields; i++)
  {
    PBM_CHECK_PTR_RET(fields,FALSE);
    if (pbm_is_usim_field(fields[i].field_id))
    {
      if (fields[i].field_id == PBM_FIELD_GRPNAME_ID)
      {
        if (group_cache)
        {
          if (++num_grp_ids <= group_cache->record_length)
            continue;
          else
          {
            group_cache = NULL;
            num_grp_ids = 0;
          }
        }
      }

      j = 0;
      index = 0;
      //go through the caches, and find the first one that is not
      //in use, and is valid for this type of data
      while (((usim_cache = pbm_get_next_usim_cache(rec_id,
                              fields[i].field_id, &j)) != NULL) &&
             cache_in_use[usim_cache->order])
        NO_ACTION

      if (usim_cache != NULL)
      {
        if (usim_cache->mapping_type == GSDI_MAPPING_TYPE_1)
        {
          index = adn_index;
        }
        else
        {
          if (m_usim_cache->index_array[usim_cache->order][adn_index])
            index = m_usim_cache->index_array[usim_cache->order][adn_index];
          else
            index = pbm_usim_get_first_available(usim_cache);
        }

        if (index)
        {
          index_array[usim_cache->order] = (uint8) index;
          cache_in_use[usim_cache->order] = TRUE;
        }
      }

      if (!index)
      {
        /* Unable to find space for this field */
        ret = FALSE;
        break;
      }

      PBM_CHECK_PTR_RET(usim_cache, FALSE);
      if (pbm_file_id_to_field_id(usim_cache->file_id) ==
                                                    PBM_FIELD_GRPNAME_ID)
      {
        group_cache = usim_cache;
        num_grp_ids++;
      }

    } /* if usim field */
  } /* for each field in current record */

  if (!ret)
  {
    PBM_MSG_ERR("Returning False from pbm_get_usim_index", 0, 0, 0);
  }

  PBM_MEM_FREEIF(cache_in_use);

  return ret;
}

/*===========================================================================
FUNCTION PBM_GET_NEXT_USIM_CACHE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
PB_USIM_CACHE *pbm_get_next_usim_cache(uint16 rec_id,
                                  pbm_field_id_e_type field_id,
                                  int *index)
{
  int i;
  pbm_pb_set_type pb_set;
  PB_CACHE *pb_cache;
  uint16 location;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_USIM_CACHE *usim_cache = NULL;

  PBM_CHECK_PTR_RET(index, NULL);
  if (*index < 0)
  {
    PBM_MSG_ERR("Invalid index %d", *index, 0, 0);
    return NULL;
  }

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  PBM_CHECK_PTR_RET(pb_cache, NULL);

  location =  pbm_record_id_to_location(rec_id);
  pb_set = pbm_pb_record_index_to_pb_set(pb_cache,location);
  PBM_CHECK_PBSET_RET(pb_set, NULL);

  m_usim_cache = pb_cache->m_usim_cache[pb_set];
  if (!m_usim_cache)
    return NULL;

  for (i = *index + 1; i <= m_usim_cache->num_caches; i++)
  {
    usim_cache = m_usim_cache->usim_cache_array[i];
    if (!usim_cache)
      continue;

    if (pbm_file_id_to_field_id(usim_cache->file_id) == field_id)
    {
      *index = i;
      return usim_cache;
    }
  }

  return NULL;
}

/*===========================================================================
FUNCTION PBM_GROUP_FIELDS

DESCRIPTION
  Group fields together. Currently we care only about grouping group ids
  together.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} record can be const, but stuff pointed to by record isn't */
void pbm_group_fields(pbm_addr_cache_s_type *record)
{
  int i;
  pbm_field_s_type field;
  uint32 offset;
  const uint32 size_one_field = ALIGNED_SIZE(sizeof(pbm_field_s_type));
  int num_grp_id = 0;

  if (!record)
    return;

  for ( i = 0; i < record->num_fields; i++)
  {
    if (record->fields[i].field_id == PBM_FIELD_GRPNAME_ID)
    {
      /* if the field needs to move... */
      if (i != num_grp_id) {
        /* swap fields */
        field = record->fields[i];
        record->fields[i] = record->fields[num_grp_id];
        record->fields[num_grp_id] = field;

        /* adjust buffer offsets */
        offset = (uint32) (i - num_grp_id) * size_one_field;
        record->fields[i].buffer_offset -= offset;
        record->fields[num_grp_id].buffer_offset += offset;
      }

      num_grp_id++;
    }
  }
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_pb_record_index_to_pb_set(const PB_CACHE *cache, int index)
{
  int i;
  int records_accumulator = 0;
  pbm_pb_set_type pb_set = PBM_USIM_PB_INVALID;

  PBM_CHECK_PTR_RET(cache, PBM_USIM_PB_INVALID);

  if ((index > cache->num_of_records) || (index < 1))
  {
    PBM_MSG_ERR("pbm_pb_record_index_to_pb_set out of bounds index %d, max %d",
                (index), (cache)->num_of_records, 0);
  }

  for (i=0; i<cache->num_files; i++)
  {
    records_accumulator += cache->records_in_pb_set[i];
    if (index <= records_accumulator) /* 1 based numbering */
    {
      pb_set = (pbm_pb_set_type) i;
      break;
    }
  }

  if (pb_set < PBM_USIM_PB_ONE || pb_set >= PBM_MAX_NUM_PB_SETS)
  {
    PBM_MSG_ERR("pb_set out of bounds index %d, max %d",
                pb_set, PBM_MAX_NUM_PB_SETS, 0);
    pb_set = PBM_USIM_PB_INVALID;
  }

  return pb_set;
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given SYNC2 index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_sync2_record_index_to_pb_set(const PB_SYNC2_CACHE *cache, int index)
{
  int i;
  int records_accumulator = 0;
  pbm_pb_set_type pb_set;

  pb_set = PBM_USIM_PB_INVALID;

  PBM_CHECK_PTR_RET(cache, PBM_USIM_PB_INVALID);

  if ((index > cache->num_of_records) || (index < 1))
  {
    PBM_MSG_ERR("pbm_sync2_record_index_to_pb_set out of bounds index %d, max %d",
                (index), (cache)->num_of_records, 0);
  }

  for (i=0; i<cache->num_files; i++)
  {
    records_accumulator += cache->records_in_pb_set[i];
    if (index <= records_accumulator) /* 1 based numbering */
    {
      pb_set = (pbm_pb_set_type) i;
      break;
    }
  }

  PBM_CHECK_PBSET_RET(pb_set, PBM_USIM_PB_INVALID);

  return pb_set;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
mmgsdi_file_enum_type pbm_pb_record_index_to_filename(const PB_CACHE *cache, int index)
{
  pbm_pb_set_type pb_set;
  mmgsdi_file_enum_type filename = MMGSDI_NO_FILE_ENUM;

  PBM_CHECK_PTR_RET(cache, MMGSDI_NO_FILE_ENUM);

  if (cache->num_of_records && cache->num_files && index)
  {
    pb_set = pbm_pb_record_index_to_pb_set(cache, index);
    if ((pb_set >= PBM_USIM_PB_ONE) && (pb_set < PBM_MAX_NUM_PB_SETS))
    {
      filename = cache->filenames[pb_set];
    }
  }

  return filename;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
mmgsdi_file_enum_type pbm_sync2_record_index_to_filename(const PB_SYNC2_CACHE *cache, int index)
{
  pbm_pb_set_type pb_set;
  mmgsdi_file_enum_type filename = MMGSDI_NO_FILE_ENUM;

  PBM_CHECK_PTR_RET(cache, MMGSDI_NO_FILE_ENUM);

  if (cache->num_of_records && cache->num_files && index)
  {
    pb_set = pbm_sync2_record_index_to_pb_set(cache, index);
    if ((pb_set >= PBM_USIM_PB_ONE) && (pb_set < PBM_MAX_NUM_PB_SETS))
    {
      filename = cache->filenames[pb_set];
    }
  }

  return filename;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif //FEATURE_MMGSDI

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_pb_record_index_to_fileloc(const PB_CACHE *cache, int index)
{
#ifdef FEATURE_PBM_USIM_SUPPORT
  pbm_pb_set_type pb_set;
#endif /* FEATURE_PBM_USIM_SUPPORT */
  int location;

  PBM_CHECK_PTR_RET(cache, index);

  location = index;
#ifdef FEATURE_PBM_USIM_SUPPORT
  pb_set = pbm_pb_record_index_to_pb_set(cache, index);
#ifdef FEATURE_USIM_1000_PBM
  if (pb_set >= PBM_USIM_PB_FOUR)
  {
    location -= cache->records_in_pb_set[2];
  }
  if (pb_set >= PBM_USIM_PB_THREE)
  {
    location -= cache->records_in_pb_set[1];
  }
#endif
  if (pb_set >= PBM_USIM_PB_TWO)
  {
    location -= cache->records_in_pb_set[0];
  }
#endif /* FEATURE_PBM_USIM_SUPPORT */

  return location;
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_sync2_record_index_to_fileloc(const PB_SYNC2_CACHE *cache, int index)
{
  pbm_pb_set_type pb_set;
  int location;

  PBM_CHECK_PTR_RET(cache, index);

  location = index;
  pb_set = pbm_sync2_record_index_to_pb_set(cache, index);

#ifdef FEATURE_USIM_1000_PBM
  if (pb_set >= PBM_USIM_PB_FOUR)
  {
    location -= cache->records_in_pb_set[2];
  }
  if (pb_set >= PBM_USIM_PB_THREE)
  {
    location -= cache->records_in_pb_set[1];
  }
#endif
  if (pb_set >= PBM_USIM_PB_TWO)
  {
    location -= cache->records_in_pb_set[0];
  }

  return location;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif //FEATURE_MMGSDI

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PB_SET

DESCRIPTION
  Map a record ID to a phonebook set

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_record_id_to_pb_set(uint16 rec_id)
{
  PB_CACHE *pb_cache;
  uint16 location;
  pbm_pb_set_type pb_set;

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  location = pbm_record_id_to_location(rec_id);

  PBM_CHECK_PTR_RET(pb_cache, PBM_USIM_PB_INVALID);

  pb_set = pbm_pb_record_index_to_pb_set(pb_cache, location);
  PBM_CHECK_PBSET_RET(pb_set, PBM_USIM_PB_INVALID);

  return pb_set;
}

/*===========================================================================
FUNCTION pbm_record_id_to_fileloc

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_fileloc(uint16 rec_id)
{
  PB_CACHE *pb_cache;
  uint16 location;
  int index = 0;

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  location = pbm_record_id_to_location(rec_id);

  PBM_CHECK_PTR_RET(pb_cache, 0);

  index = pbm_pb_record_index_to_fileloc(pb_cache, location);
  if (index < 0)
  {
    PBM_MSG_ERR("Invalid record id %d", rec_id, 0, 0);
    return 0;
  }

  return (uint16)index;
}

#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_fill_adn_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_fill_adn_sfi(uint16 rec_id, byte *buffer)
{
  PB_CACHE             *pb_cache;
  PB_MASTER_USIM_CACHE *m_usim_cache;
  pbm_pb_set_type       pb_set = pbm_record_id_to_pb_set(rec_id);
  int                   location = pbm_record_id_to_location(rec_id);
  int                   index = 0;

  PBM_CHECK_PTR_RET(buffer, PBM_ERROR);

  PBM_CHECK_PBSET_RET(pb_set, PBM_ERROR);

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

  m_usim_cache = pb_cache->m_usim_cache[pb_set];
  PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

  index = pbm_pb_record_index_to_fileloc(pb_cache, location);

  buffer[0] = (uint8) m_usim_cache->adn_sfi;
  buffer[1] = (uint8)index;

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_allocate_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_index(uint16 rec_id, const pbm_field_s_type *fields,
                                   int num_fields, uint8 **prev_index,
                                   uint8 **curr_index)
{
  PB_CACHE *cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  PB_MASTER_USIM_CACHE *m_usim_cache;
  pbm_addr_cache_s_type *prev_rec;
  uint8 *curr = NULL;
  uint8 *prev = NULL;
  pbm_pb_set_type pb_set;

  PBM_CHECK_PTR_RET(prev_index, FALSE);
  PBM_CHECK_PTR_RET(curr_index, FALSE);

  PBM_CHECK_PTR_RET(cache, FALSE);

  if (pbm_is_usim_required(rec_id, fields, num_fields))
  {
    pb_set = pbm_record_id_to_pb_set(rec_id);
    PBM_CHECK_PBSET_RET(pb_set, FALSE);

    m_usim_cache = cache->m_usim_cache[pb_set];

    PBM_CHECK_PTR_RET(m_usim_cache, FALSE); // Perhaps there is no USIM.

    PBM_MEM_ALLOC(curr, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
    if (!curr)
    {
      PBM_MSG_ERR("Unable to allocate current index array", 0, 0, 0);
      return FALSE;
    }
    memset(curr, 0, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
  }

  prev_rec = cache->pb_cache_array[pbm_record_id_to_location(rec_id)];
  if (prev_rec &&
      pbm_is_usim_required(rec_id, prev_rec->fields, prev_rec->num_fields))
  {
    pb_set = pbm_record_id_to_pb_set(rec_id);
    PBM_CHECK_PBSET_RET(pb_set, FALSE);

    m_usim_cache = cache->m_usim_cache[pb_set];

    PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

    PBM_MEM_ALLOC(prev, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
    if (!prev)
    {
      PBM_MSG_ERR("Unable to allocate current index array", 0, 0, 0);
      PBM_MEM_FREEIF(curr);
      return FALSE;
    }
    memset(prev, 0, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
  }

  *curr_index = curr;
  *prev_index = prev;

  return TRUE;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION pbm_free_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_index(uint8 *prev_index, uint8 *curr_index)
{
  PBM_MEM_FREEIF(prev_index);
  PBM_MEM_FREEIF(curr_index);
}

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
/*===========================================================================
FUNCTION pbm_copy_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_usim_index(uint16 rec_id,
                         uint8 *index_array)
{
  int i;
  int index = pbm_record_id_to_fileloc(rec_id);
  PB_MASTER_USIM_CACHE *m_usim_cache;
  PB_CACHE *pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  pbm_pb_set_type pb_set;

  PBM_CHECK_PTR_RET(index_array, FALSE);

  PBM_CHECK_PTR_RET(pb_cache, FALSE);
  pb_set = pbm_record_id_to_pb_set(rec_id);
  PBM_CHECK_PBSET_RET(pb_set, FALSE);
  m_usim_cache = pb_cache->m_usim_cache[pb_set];
  PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

  memset(index_array, 0, (uint32)(m_usim_cache->num_caches+1) * sizeof(uint8));

  for (i = 1; i <= m_usim_cache->num_caches; i++)
    if (m_usim_cache->usim_cache_array[i]) /* if cache exists */
      index_array[i] = m_usim_cache->index_array[i][index];

  return TRUE;
}

/*===========================================================================
FUNCTION pbm_gsm_modified

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsm_modified(uint16 rec_id)
{
  PB_CACHE *pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  uint16 location = pbm_record_id_to_location(rec_id);
  PB_SYNC2_CACHE *sync2_cache;
  boolean ret = FALSE;

  if (pb_cache && pb_cache->m_sync_cache &&
      (sync2_cache = pb_cache->m_sync_cache->pbc_cache) != NULL)
  {
    if (sync2_cache->data_array &&
        (sync2_cache->data_array[location] & PBM_PBC_MODIFIED_MASK))
      ret = TRUE;
  }

  return ret;
}

/*===========================================================================
FUNCTION pbm_validate_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_validate_sfi(uint16 rec_id, uint8 sfi)
{
  boolean ret = TRUE;
  PB_CACHE *pb_cache;
  pbm_pb_set_type pb_set;

  pb_cache = pbm_pb_id_to_cache(pbm_record_id_to_phonebook(rec_id));
  pb_set = pbm_record_id_to_pb_set(rec_id);
  PBM_CHECK_PBSET_RET(pb_set, FALSE);
  if (pb_cache && pb_cache->m_usim_cache[pb_set])
  {
    if (pb_cache->m_usim_cache[pb_set]->adn_sfi != sfi)
    {
      PBM_MSG_ERR("SFI does not match for record %d, sfi %d", rec_id, sfi, 0);
      ret = FALSE;
    }
  }

  return ret;
}
#endif /* FEATURE_PBM_USIM_SUPPORT */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION pbm_check_write_lock

DESCRIPTION
  Returns error if pbm write is locked and non-blocking, or
  blocks till pbm write is unlocked in case of blocking lock.

  If this function returns PBM_SUCCESS, we have already acquired
  pbm_write_lock_crit_sect.

DEPENDENCIES
  Caller should leave pbm_write_lock_crit_sect in case of success.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_check_write_lock()
{
  rex_tcb_type *self = rex_self();

  /* If lock is already locked in blocking way, we block at next line */
  rex_enter_crit_sect(&pbm_write_lock_crit_sect);

  /* Return error if lock is already locked in non-blocking way */
  if (g_lock.lock_owner && g_lock.lock_owner != self)
  {
    PBM_MSG_HIGH("Write is locked", 0, 0, 0);
    rex_leave_crit_sect(&pbm_write_lock_crit_sect);
    return PBM_ERROR_LOCKED;
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_check_read_lock

DESCRIPTION
  Returns error if pbm read is locked and non-blocking, or
  blocks till pbm read is unlocked in case of a blocking lock.

  If this function returns PBM_SUCCESS, we have already acquired
  pbm_crit_sect.

DEPENDENCIES
  Caller should leave pbm_read_lock_crit_sect in case of success.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_check_read_lock()
{
  rex_tcb_type *self = rex_self();

  /* If lock is already locked in blocking way, we block at next line */
  rex_enter_crit_sect(&pbm_read_lock_crit_sect);

  /* Return error if lock is already locked in non-blocking way */
  if (g_lock.lock_owner && g_lock.lock_owner != self)
  {
    PBM_MSG_HIGH("Read is locked", 0, 0, 0);
    rex_leave_crit_sect(&pbm_read_lock_crit_sect);
    return PBM_ERROR_LOCKED;
  }

  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_wstrncpy

DESCRIPTION
  uint16 *pbm_wstrncpy(uint16 *dest, const uint16 *src, int size)

  dest: [out]  The resulting string is placed in dest.
  src:  [in]   The string to copy is placed here.
  size: [in]   The number of UCS2 characters of src to at most copy.

  This is the UCS2 version of the strncpy function.  It will copy
  src to dest until either a NULL character is encountered or
  size characters have been copied.  The resulting string is
  NULL terminated if there was one encountered within the first
  size characters.  Otherwise, the resulting string is not NULL terminated.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
uint16 *pbm_wstrncpy(uint16 *dest, const uint16 *src, int size)
{
  uint16 *dest_ret = dest;

  PBM_CHECK_PTR_RET(dest, dest_ret);
  PBM_CHECK_PTR_RET(src, dest_ret);

  while (size)
  {
    size--;
    *dest = *src;
    if (*dest == 0)
    {
      break;
    }
    dest++;
    src++;
  }

  //force NULL termination
  if (size == 0)
  {
    *(dest-1) = 0;
  }

  return dest_ret;
}

/*===========================================================================
FUNCTION pbm_wstrncat

DESCRIPTION
  uint16 *pbm_wstrncat(uint16 *dest, uint16 *src, int size)

  dest: [out]  The resulting string is placed in dest.
  src:  [in]   The string to copy is placed here.
  size: [in]   The number of UCS2 characters of src to at most copy.

  This is the UCS2 version of the strncat function.  It will append
  src to dest until either a NULL character is encountered or
  size characters have been copied.  The resulting string is
  NULL terminated if there was one encountered within the first
  size characters.  Otherwise, the resulting string is not NULL terminated.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
uint16 *pbm_wstrncat(uint16 *dest, const uint16 *src, int size)
{
  uint16 *dest_ret = dest;

  PBM_CHECK_PTR_RET(dest,dest_ret);
  while (*dest != 0) dest++;  /* Move to end of dest string. */
  (void) pbm_wstrncpy(dest, src, size); /* Copy src to this part of dest. */

  return dest_ret;
}

/*===========================================================================
FUNCTION PBM_GET_NAME

DESCRIPTION
Extracts the name field from the cache based on a priority.
The priority levels are listed in decreasing order in the array name_field_sort.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_name(const pbm_addr_cache_s_type *cache,
                             boolean pbm_sort_last_first,
                             uint16 *nameBuff, int buffSize)
{
  int j;
  int priority_index;
  void *name = NULL;
  boolean name_is_wstr = TRUE;
  uint16  name_size = 0;
  uint16 *lastname = NULL;
  uint16 *firstname = NULL;
  boolean firstname_wstr = FALSE;
  boolean lastname_wstr = FALSE;
  int priority;
  pbm_field_id_e_type *name_field_sort;
  int name_field_sort_size;
  static pbm_field_id_e_type name_field_sort_efs[] = {
#ifndef FEATURE_ODM_UI
    PBM_FIELD_NAME,
#endif // FEATURE_ODM_UI
    PBM_FIELD_LASTNAME,
    PBM_FIELD_FIRSTNAME,
    PBM_FIELD_NICKNAME,
    PBM_FIELD_MIDDLENAME,
    PBM_FIELD_ORG,
    PBM_FIELD_EMAIL,
    PBM_FIELD_PRIVATE_EMAIL1,
    PBM_FIELD_PRIVATE_EMAIL2,
    PBM_FIELD_BUSINESS_EMAIL1,
    PBM_FIELD_BUSINESS_EMAIL2,
    PBM_FIELD_PHONE_HOME,
    PBM_FIELD_PHONE_WORK,
    PBM_FIELD_PHONE_GENERIC,
    PBM_FIELD_PHONE_OTHER,
    PBM_FIELD_PHONE_CELL,
    PBM_FIELD_PHONE_VOICE,
    PBM_FIELD_PHONE_FAX,
    PBM_FIELD_PHONE_BUSINESS_FAX,
    PBM_FIELD_PHONE_PREF,
    PBM_FIELD_PHONE_PAGER,
    PBM_FIELD_PHONE_VIDEOCALLERID,
#ifdef FEATURE_ODM_UI
    PBM_FIELD_NAME
#endif // FEATURE_ODM_UI
  };

  static pbm_field_id_e_type name_field_sort_sim[] = {
    PBM_FIELD_NAME,
    PBM_FIELD_PHONE_GENERIC,
#ifdef FEATURE_PBM_USIM_SUPPORT
    PBM_FIELD_ADDITIONAL_NUMBER,
    PBM_FIELD_OTHERNAME,
    PBM_FIELD_EMAIL
#endif /* FEATURE_PBM_USIM_SUPPORT */
  };

  if(cache == NULL)
    return PBM_ERROR;

  PBM_CHECK_PTR_RET(nameBuff, PBM_ERROR);

  if (pbm_record_id_to_phonebook(cache->record_id) == PBM_EFS)
  {
    name_field_sort = name_field_sort_efs;
    name_field_sort_size = ARR_SIZE(name_field_sort_efs);
  }
  else
  {
    name_field_sort = name_field_sort_sim;
    name_field_sort_size = ARR_SIZE(name_field_sort_sim);
  }
  //always assign to one less than array size as
  //to avoid lint error at name_field_sort[priority]
  priority = name_field_sort_size - 1;

  for (j = 0; j < cache->num_fields; j++)
  {
    if (cache->fields[j].field_id == PBM_FIELD_LASTNAME)
    {
      lastname = (uint16 *)pbm_get_field_data_ptr(cache->fields, j);
      lastname_wstr = (cache->fields[j].field_type == PBM_FT_UCS2)?TRUE:FALSE;
    }
    else if (cache->fields[j].field_id == PBM_FIELD_FIRSTNAME)
    {
      firstname = (uint16 *)pbm_get_field_data_ptr(cache->fields, j);
      firstname_wstr = (cache->fields[j].field_type == PBM_FT_UCS2)?TRUE:FALSE;
    }

    for (priority_index = 0; priority_index <= priority;
         priority_index++)
    {
      if (cache->fields[j].field_id == name_field_sort[priority_index])
      {
        break;
      }
    }

    /* Assign name to the field that appears earliest in name_field_sort.
     * If no fields are present in name_field_sort, use the first string or
     * phone field */
    if ((priority_index <= priority) &&
        ((cache->fields[j].field_type == PBM_FT_STRING) ||
         (cache->fields[j].field_type == PBM_FT_UCS2) ||
         (cache->fields[j].field_type == PBM_FT_PHONE)))
    {
      name = (void *) pbm_get_field_data_ptr(cache->fields, j);
      name_is_wstr = (cache->fields[j].field_type==PBM_FT_UCS2);
      name_size = cache->fields[j].data_len;
      priority = priority_index;
    }
  }


  /* Special handling if last name is present. */
  if ((priority < name_field_sort_size) &&
      (name_field_sort[priority] == PBM_FIELD_LASTNAME) &&
      ((lastname != NULL) && (firstname != NULL)))
  {
    int initial_len;
    uint16 *initial;
    uint16 *afterwards;
    boolean initial_wstr = FALSE;
    boolean after_wstr = FALSE;


    if (pbm_sort_last_first)
    {
      initial = lastname;
      initial_wstr = lastname_wstr;
      afterwards = firstname;
      after_wstr = firstname_wstr;
    }
    else
    {
      initial = firstname;
      initial_wstr = firstname_wstr;
      afterwards = lastname;
      after_wstr = lastname_wstr;
    }

    if (initial_wstr)
    {
      initial_len = (int) pbmutils_wstrlen(initial);
      (void) pbm_wstrncpy(nameBuff, initial, buffSize);
    }
    else
    {
      initial_len = (int) strlen((char *) initial);
      (void) pbmconvert_pbm8_to_ucs2((uint8 *) initial, (uint16) initial_len,
                                     nameBuff, (uint16) buffSize);
    }

    if (initial_len < buffSize) /* Copy "afterwards" if there is space */
    {
      if (after_wstr)
      {
        (void) pbm_wstrncat(nameBuff, afterwards,  buffSize-initial_len);
      }
      else
      {
        (void) pbmconvert_pbm8_to_ucs2((uint8 *) afterwards,
                                       (uint16) strlen((char *) afterwards),
                                       nameBuff + initial_len,
                                       (uint16) (buffSize - initial_len));
      }
    }
    nameBuff[buffSize-1] = '\0';

    return PBM_SUCCESS;
  }

  if (name != NULL)
  {
    if (name_is_wstr) {
      (void) pbm_wstrncpy(nameBuff, (uint16 *) name, buffSize);
    }
    else
    {
      if (!pbmconvert_pbm8_to_ucs2((uint8 *) name, name_size - 1,
                                   nameBuff, (uint16) buffSize))
      {
        PBM_MSG_ERR("Could not convert entire string, %d %d",
                    name_size, buffSize, 0);
      }
    }

    return PBM_SUCCESS;
  }

  return PBM_ERROR;
}



/*===========================================================================
FUNCTION PBM_NOTIFY

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_notify(pbm_notify_data_s_type *notify_data)
{
  pbm_notify_node_s_type *curr = pbm_notify_list;

  while (curr)
  {
    if (curr->notify_func)
    {
      (*curr->notify_func)(curr->user_data, notify_data);
    }
    else
    {
      PBM_MSG_ERR("Invalid entry %p found in notify list", curr, 0, 0);
      break;
    }

    curr = curr->next;
  }
}

/*===========================================================================
FUNCTION PBM_APPEND_PHONE_UID_FIELD

DESCRIPTION
  This function is used to append the field PBM_FIELD_PH_UID to a field list with a new
  phone unique ID. ph_unique_id is an input parameter,

DEPENDENCIES
  Must be called from within pbm_crit_sect critical section.

SIDE EFFECTS
  Frees the existing field list. New memory is allocated for the appended field list, and the pointer
  to it is returned in the parameter.

COMMENTS

SEE ALSO
===========================================================================*/

pbm_return_type pbm_append_phone_uid_field(uint64 ph_unique_id, pbm_field_s_type **ppfields,
   uint16 *pnum_fields)
{
  pbm_return_type nRet = PBM_SUCCESS;
  int i;
  uint16 new_num_fields;
  pbm_field_s_type *new_fields = NULL;
  uint32 *new_sizes = NULL;

  PBM_CHECK_PTR_RET(ppfields, PBM_ERROR);
  PBM_CHECK_PTR_RET(pnum_fields, PBM_ERROR);
  PBM_CHECK_PTR_RET(*ppfields, PBM_ERROR);

  /* The only way to do this is to copy the entire field array to a new field array
   * with the appended field. */

  new_num_fields = *pnum_fields + 1;

  do
  {
    /* Create array of field sizes to allocate the fields from */
    PBM_MEM_ALLOC(new_sizes, new_num_fields*sizeof(uint32));
    PBM_CHECK_PTR_BREAK(new_sizes, nRet, PBM_ERROR);

    for (i = 0; i < *pnum_fields; i++)
    {
      new_sizes[i] = (*ppfields)[i].data_len;
    }

    new_sizes[i] = PBM_DDWORD_SIZE;

    /* now allocate the field structure, based on the sizes */
    if (!pbm_allocate_fields_internal(&new_fields, new_num_fields, new_sizes))
    {
      nRet = PBM_ERROR;
      break;
    }

    /* copy over the fields */
    for (i = 0; i < *pnum_fields; i++)
    {
      pbm_fill_field(&new_fields[i], (*ppfields)[i].field_id, (*ppfields)[i].field_type,
         pbm_get_field_data_ptr(*ppfields,i), (*ppfields)[i].data_len);
    }

    /* Fill in the UID field */
    pbm_fill_field(&new_fields[i], PBM_FIELD_PH_UID, PBM_FT_BINARY,
       (byte *) &ph_unique_id, sizeof(ph_unique_id));

    /* free the old fields */
    pbm_free_fields_internal(ppfields, *pnum_fields);

    /* set pointers to the new fields */
    *ppfields = new_fields;
    *pnum_fields = new_num_fields;
  }
  while (FALSE); /*lint !e717 */

  PBM_MEM_FREEIF(new_sizes);

  if (nRet != PBM_SUCCESS) {
    if (new_fields) {
      pbm_free_fields_internal(&new_fields, new_num_fields);
    }
  }

  return nRet;
}

/*===========================================================================
FUNCTION PBM_FIND_PHONE_UID_FIELD

DESCRIPTION
  This function locates the PH UID field from a field list and returns the
  index to the field. Returns TRUE on found, FALSE if not found.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/

boolean pbm_find_phone_uid_field(const pbm_field_s_type *fields, int num_fields, int *index)
{
  int i;

  if (!fields)
  {
    return FALSE;
  }

  /* Locate PH_UID field in update field list. */
  for (i = 0; i < num_fields; i++)
  {
    if (fields[i].field_id == PBM_FIELD_PH_UID)
    {
      if (index)
      {
        *index = i;
      }
      return TRUE;
    }
  }

  return FALSE;
}

/*===========================================================================
FUNCTION pbm_fields_convert_str_to_ucs2

DESCRIPTION
  boolean pbm_fields_convert_str_to_ucs2(uint8   **to_fields_buf_ptr,
                                         int     *to_fields_buf_size,
                                         const   uint8  *from_fields_buf_ptr,
                                         uint32  num_fields)

  to_fields_buf_ptr:   [out]   newly allocated record with all
                               PBM_FT_STRING values stored as PBM_FT_UCS2 type
  to_fields_buf_size:  [out]   size of the newly allocated record
  from_fields_buf_ptr: [in]    record which needs to be converted
  num_fields:          [in]    number of fields

  This function takes a given record and copies over the fields to a
  new buffer. If any field of type PBM_FT_STRING is encountered, they
  are copied over as PBM_FT_UCS2. If no PBM_FT_STRING type is found,
  no new buffer is allocated. The calling function is responsible for
  freeing the memory allocated for the new buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if conversion was successful

SIDE EFFECTS
  None.
===========================================================================*/
boolean pbm_fields_convert_str_to_ucs2(uint8         **to_fields_buf_ptr,
                                       uint32        *to_fields_buf_size,
                                       const  uint8  *from_fields_buf_ptr,
                                       uint32        num_fields)
{
  pbm_field_s_type **ptyped_to_fields_buf_ptr;
  pbm_field_s_type *ptyped_from_fields_buf_ptr;
  uint32 *data_sizes;
  boolean bstring_type_exists = FALSE;
  uint32 i;

  PBM_CHECK_PTR_RET(to_fields_buf_ptr, FALSE);
  PBM_CHECK_PTR_RET(to_fields_buf_size, FALSE);
  PBM_CHECK_PTR_RET(from_fields_buf_ptr, FALSE);

  /*lint -e{740} Unusual Pointer Cast*/
  ptyped_to_fields_buf_ptr = (pbm_field_s_type **)to_fields_buf_ptr;
  /*lint -e{826} Suspicious pointer-to-pointer conversion*/
  ptyped_from_fields_buf_ptr = (pbm_field_s_type *)from_fields_buf_ptr;



  data_sizes = mem_malloc(&tmc_heap,(uint32) num_fields * sizeof(int));

  for( i = 0; i < num_fields; i++) {
    if (ptyped_from_fields_buf_ptr[i].field_type == PBM_FT_STRING) {
      data_sizes[i] = ptyped_from_fields_buf_ptr[i].data_len * 2;
      bstring_type_exists = TRUE;
    } else {
      data_sizes[i] = ptyped_from_fields_buf_ptr[i].data_len;
    }
  }

  /* if FT_STRING type field was never encountered, then simply return
     after setting the out pointer value to NULL*/
  if ( !bstring_type_exists ) {
    *to_fields_buf_ptr = NULL;
    mem_free(&tmc_heap, data_sizes);
    return TRUE;
  }

  if(!pbm_allocate_fields(to_fields_buf_ptr,
                          to_fields_buf_size,
                          num_fields,
                          data_sizes)) {
    mem_free(&tmc_heap, data_sizes);
    return FALSE;
  }

  /* now that the new record fields have been allocated, we need to
     copy data */
  for ( i = 0; i < num_fields; i++) {
    if (ptyped_from_fields_buf_ptr[i].field_type == PBM_FT_STRING) {
      uint16 temp_wstr[PBM_MAX_NUM_LENGTH];
      if(!pbmconvert_pbm8_to_ucs2(pbm_get_field_data_ptr(&(ptyped_from_fields_buf_ptr[i]), 0),
                                  ptyped_from_fields_buf_ptr[i].data_len,
                                  temp_wstr,
                                  PBM_MAX_NUM_LENGTH)) {
        mem_free(&tmc_heap, data_sizes);
        return FALSE;
      }
      pbm_fill_field(&((*ptyped_to_fields_buf_ptr)[i]),
                     ptyped_from_fields_buf_ptr[i].field_id,
                     PBM_FT_UCS2,
                     (byte *)temp_wstr, (uint16 )data_sizes[i]);
    } else {
      pbm_fill_field(&((*ptyped_to_fields_buf_ptr)[i]),
                     ptyped_from_fields_buf_ptr[i].field_id,
                     ptyped_from_fields_buf_ptr[i].field_type,
                     pbm_get_field_data_ptr(&ptyped_from_fields_buf_ptr[i], 0),
                     (uint16 )data_sizes[i]);
    }

  }
  mem_free(&tmc_heap, data_sizes);
  return TRUE;
}

#ifdef FEATURE_MMGSDI
/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_FILE_ID

DESCRIPTION
  This function will find the file id corresponding to the given record id.

SIDE EFFECTS
===========================================================================*/
pbm_file_device_e_type pbm_record_id_to_file_id(uint16 rec_id)
{
  pbm_device_type pb_id = pbm_record_id_to_phonebook(rec_id);
  int location = pbm_record_id_to_location(rec_id);
  PB_CACHE *pb_cache;
  pbm_pb_set_type pb_set;

  pb_cache = pbm_pb_id_to_cache(pb_id);
  if (!pb_cache)
  {
    PBM_MSG_ERR("Invalid rec id %d", rec_id, 0, 0);
    return PBM_FILE_MAX_REQUESTS;
  }

  pb_set = pbm_pb_record_index_to_pb_set(pb_cache, location);
  PBM_CHECK_PBSET_RET(pb_set, PBM_FILE_MAX_REQUESTS);

  return pb_cache->file_ids[pb_set];
}
#endif /* FEATURE_MMGSDI */

#endif /* FEATURE_PBM_TASK */

