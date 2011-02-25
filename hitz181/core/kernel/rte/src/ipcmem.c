/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

   S E C U R E   S O C K E T S   L A Y E R   D Y N A M I C   M E M O R Y 


GENERAL DESCRIPTION
  This module implements a simple dynamic memory management mechanism for
  the security services. It's main usage is for the porting of OpenSSL
  certificate parsing and verification routines, so that the memory
  allocation and free can be used as is in the OpenSSL code.

EXTERNALIZED FUNCTIONS
  ipcmem_pool_init() - Initialize the dynamic memory pool for security srv.
  ipcmem_malloc()    - Allocate a block of memory, like malloc.
  ipcmem_free()      - Free the allocated memory, like free.
  ipcmem_realloc()   - Reallocate a block of memory, like realloc
  ipcmem_remalloc()  - Free the existing block, and allocate a new block.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ipcmem_pool_init() has to be called exactly once before using any
  externalized functions provided by this module.

Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/kernel/rte/main/latest/src/ipcmem.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-May-04  rwh     Feature-ized with FEATURE_REX_IPC
                   Adapted for smaller memory configuration.
03/10/13   et      Created module.

===========================================================================*/


/*===========================================================================

             Include Files for Module 

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_REX_IPC

#include "msg.h"
#include "err.h"

#include <string.h>
#include "ipcdefi.h"
#include "ipcmem.h"

/* ------------------------------------------------------------------------*/
/*                       Local Definitions And Data                        */
/* ------------------------------------------------------------------------*/

/* typdef pdsm_pool_info_s is used to store the static pool information. 
** Size of buffer and number of buffers of that size. Size of the buffer
** is the size of free space that can be allocated. It does not include
** the overhead for pointer to the next item, etc.
*/
typedef struct ipcmem_pool_info_s 
{
    uint16     size;        /* Size of buffer */
    uint16     total_num;   /* Total number of buffers of this size */
    uint16     h_watermark; /*lint -e754 High watermark not to exceed. */
    uint16     num_used;    /*lint -e754 Number of buffers being used. */
} ipcmem_pool_info_s_type;  

/* Definition used to indicate the memory size of the buffers. Tune
** with the application. Remember also need to update the 
** IPCMEM_DYN_MEMORY macro.
*/
#ifdef FEATURE_CGPS 
#error code not present
#else
#define    BPOOL_SIZE_00    32
#define    BPOOL_SIZE_01    64
#define    BPOOL_SIZE_02    128
#define    BPOOL_SIZE_03    256
#define    BPOOL_SIZE_04    512
#define    BPOOL_SIZE_05    1024
#define    BPOOL_SIZE_06    2048
#define    BPOOL_SIZE_07    8232
#endif

/* Definition used to indicate the number of buffers of size pool_size
** a pool. Tune with the application. Remember also need to update the
** IPCMEM_DYN_MEMORY macro.
*/
#ifndef FEATURE_IPC_SMALL_MEMORY_POOL
#ifdef FEATURE_CGPS 
#error code not present
#else
#define    BPOOL_NUM_00     100      /* 3200 */
#define    BPOOL_NUM_01     100      /* 6400 */
#define    BPOOL_NUM_02     50       /* 6400 */
#define    BPOOL_NUM_03     30       /* 7680 */
#define    BPOOL_NUM_04     10       /* 5120 */
#define    BPOOL_NUM_05     5        /* 5120 */
#define    BPOOL_NUM_06     2        /* 4096 */
#define    BPOOL_NUM_07     2        /* 8232 */
#endif
#else  /* FEATURE_IPC_SMALL_MEMORY_POOL */
  #define    BPOOL_NUM_00     25
  #define    BPOOL_NUM_01     25
  #define    BPOOL_NUM_02     12
  #define    BPOOL_NUM_03     8
  #define    BPOOL_NUM_04     2
  #define    BPOOL_NUM_05     1
  #define    BPOOL_NUM_06     1
  #define    BPOOL_NUM_07     1
#endif /* FEATURE_IPC_SMALL_MEMORY_POOL */

/* The high watermark of the buffers is the threshold of buffer utilization
** that merits closer inspection. The threshold is 6/10 of capacity.
*/
#define    BPOOL_HIGH_WM_00  (((BPOOL_NUM_00*6)+9)/10)
#define    BPOOL_HIGH_WM_01  (((BPOOL_NUM_01*6)+9)/10)
#define    BPOOL_HIGH_WM_02  (((BPOOL_NUM_02*6)+9)/10)
#define    BPOOL_HIGH_WM_03  (((BPOOL_NUM_03*6)+9)/10)
#define    BPOOL_HIGH_WM_04  (((BPOOL_NUM_04*6)+9)/10)
#define    BPOOL_HIGH_WM_05  (((BPOOL_NUM_05*6)+9)/10)
#define    BPOOL_HIGH_WM_06  (((BPOOL_NUM_06*6)+9)/10)
#define    BPOOL_HIGH_WM_07  (((BPOOL_NUM_07*6)+9)/10)

/* Pool information table. This table is used at init time
** to create buffer pools. Buffer pools should be in the order of
** increasing sizes. 
*/
static ipcmem_pool_info_s_type ipcmem_pool_info[] = 
{ 
  /* id */  /* size */        /* total_num */ /* high wmark */  /*used*/
  /* 0 */   {BPOOL_SIZE_00,   BPOOL_NUM_00,   BPOOL_HIGH_WM_00,  0},     
  /* 1 */   {BPOOL_SIZE_01,   BPOOL_NUM_01,   BPOOL_HIGH_WM_01,  0},     
  /* 2 */   {BPOOL_SIZE_02,   BPOOL_NUM_02,   BPOOL_HIGH_WM_02,  0},     
  /* 3 */   {BPOOL_SIZE_03,   BPOOL_NUM_03,   BPOOL_HIGH_WM_03,  0},     
  /* 4 */   {BPOOL_SIZE_04,   BPOOL_NUM_04,   BPOOL_HIGH_WM_04,  0},     
  /* 5 */   {BPOOL_SIZE_05,   BPOOL_NUM_05,   BPOOL_HIGH_WM_05,  0},     
  /* 6 */   {BPOOL_SIZE_06,   BPOOL_NUM_06,   BPOOL_HIGH_WM_06,  0},
  /* 7 */   {BPOOL_SIZE_07,   BPOOL_NUM_07,   BPOOL_HIGH_WM_07,  0}     
};


/* Max number of buffer pools */
#define IPCMEM_MAX_NBPOOLS \
    (sizeof(ipcmem_pool_info) / sizeof(ipcmem_pool_info[0]))       

/* Buffer header, used to store the pool id or the pointer to
** the next buffer item + msg_id*/
#define IPCMEM_BUF_HEADER (sizeof(int) + IPC_MSG_OVERHEAD +      \
                           sizeof(((ipc_internal_msg_type*)NULL)->msg_id))

/* Total number of bytes for the pool. Used to static allocate
** the memory as the heap.
   if you change this, also update IPCMEM_BUFFER_IN_POOL_RANGE*/

#define IPCMEM_DYN_MEMORY \
    (   (BPOOL_SIZE_00 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_00 ) \
     +                                                         \
        (BPOOL_SIZE_01 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_01 ) \
     +                                                         \
        (BPOOL_SIZE_02 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_02 ) \
     +                                                         \
        (BPOOL_SIZE_03 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_03 ) \
     +                                                         \
        (BPOOL_SIZE_04 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_04 ) \
     +                                                         \
        (BPOOL_SIZE_05 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_05 ) \
     +                                                         \
        (BPOOL_SIZE_06 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_06 ) \
     +                                                         \
        (BPOOL_SIZE_07 + IPCMEM_BUF_HEADER) * ( BPOOL_NUM_07 ))


/* Structure which holds buffer pool information.
*/
typedef struct ipcmem_bpool {
    size_t bpsize;                   /* Size of this pool */
    int    *bpnext;                  /* Pointer to free buffer list */
} ipcmem_bpool_type;

/* Memory pools storage allocation */
static char  ipcmem_heap[IPCMEM_DYN_MEMORY];  /*lint !e413 !e30 */

/* Allocation index for heap */
static unsigned int   alloc_idx = 0;

/* Buffer pool table */
static ipcmem_bpool_type bptab[ IPCMEM_MAX_NBPOOLS ];

/* Current number of pools 
*/
static int    nbpools=0;                    


/*===========================================================================

FUNCTION IPCMEM_GETMEM

DESCRIPTION
  This function is used to get RAM space. This interface is done so that
  if REX supports getmem() in future the interfaces will remain same.
  Currently it just returns the pointer to statically allocated buffer 
  space.

RETURN VALUE
  The address of the memory block available.

DEPENDENCIES

===========================================================================*/
static char *ipcmem_getmem( size_t size )
{
  char *buffer = NULL;

  if ( (alloc_idx + size) <= IPCMEM_DYN_MEMORY )  /*lint !e413 */
  {
    /* return pointer to statically allocated buffer space */
    buffer = &ipcmem_heap[alloc_idx];
    alloc_idx += size;
  }

  return buffer;
} /* End ipcmem_getmem( ) */


/*===========================================================================

FUNCTION IPCMEM_MAKE_POOL

DESCRIPTION
  This function is used to make Dynamic buffer pools, to be used by the
  Security Services.

RETURN VALUE
  TRUE, if the memory pool is made successfully.
  FALSE, if it runs out of the free memory space.

DEPENDENCIES

===========================================================================*/
static boolean ipcmem_make_pool(size_t size, int num)
{
  int    poolid;
  char   *where;

  size = (size + 3) & ~3;  /* Round up to 4 byte boundary */

  /* Now get memory for this pool. IPCMEM_BUF_HEADER is added so that enough space
  ** is allowed for storing pool id + meta-data required (see ipc_internal_msg_type).
  */
  if (  (where = ipcmem_getmem( (size + IPCMEM_BUF_HEADER)*num) ) == NULL )  /*lint !e413 !e737 */
  {
    /* Ran out of memory, return error */
    ERR_FATAL( "Running out memory for size %d num %d", size, num, 0 );
    return FALSE;   /*lint !e527 valid for other macro defines*/
  }
    
  /* memory allocation was a Success, go ahead and create a pool
  ** of buffers.
  */
  /* INTLOCK(); */
    
  /* Increment the num of pools being currently used.
  */
  poolid = nbpools++;             
    
  /* Point to the first available buffer in this pool.
  */
  bptab[poolid].bpnext = (int *)where;   /*lint !e826 */
  
  /* Indicate the size of buffer allocated.
  */
  bptab[poolid].bpsize = size;
   
  /* Add size so that pool id can be stored along with the buffer meta-data.
  */
  size += IPCMEM_BUF_HEADER;
    
  /* Now link all the buffers together, last buffer will be linked
  ** at the end of this loop.
  */
  for (num--; num > 0; num--, where += size )
  {
    *( (int *) where) = size +(int)where;   /*lint !e737 !e826 !e713 */
  }
  *( (int *) where) = (int)NULL;   /*lint !e826  Last buffer in the pool */
    
  /*  INTFREE(); */

  return TRUE;

} /* End ipcmem_make_pool( ) */

/*===========================================================================

FUNCTION IPCMEM_BUFFER_IN_POOL_RANGE

DESCRIPTION
  This function checks if a given buffer pointer belongs to the memory pool
  indicated by pool id

RETURN VALUE
  If buffer is the range of buffers for the given pool, 1 is returned
  Otherwise, 0 is returned.

DEPENDENCIES

===========================================================================*/
static int ipcmem_buffer_in_pool_range(unsigned int buffer, int poolid)
{
   static const unsigned int buffer_sizes[IPCMEM_MAX_NBPOOLS] = {
      BPOOL_SIZE_00, BPOOL_SIZE_01, BPOOL_SIZE_02, BPOOL_SIZE_03, 
      BPOOL_SIZE_04, BPOOL_SIZE_05, BPOOL_SIZE_06, BPOOL_SIZE_07
   };

   static const unsigned int num_buffers[IPCMEM_MAX_NBPOOLS] = {
      BPOOL_NUM_00, BPOOL_NUM_01, BPOOL_NUM_02, BPOOL_NUM_03,
      BPOOL_NUM_04, BPOOL_NUM_05, BPOOL_NUM_06, BPOOL_NUM_07

   };

   static unsigned int pool_range_end[IPCMEM_MAX_NBPOOLS]= {0};
   
   int buffer_found_in_range = 0;

   /* do this only once */
   if(0 == pool_range_end[0]) 
   {
      unsigned int i = 0;
      
      pool_range_end[0] = (unsigned int) (ipcmem_heap + ((buffer_sizes[0] + IPCMEM_BUF_HEADER)*num_buffers[0]));
      
      /* iteratively determine start index for each buffer pool */
      for(i = 1; i < IPCMEM_MAX_NBPOOLS; ++i)
      {
         pool_range_end[i] = pool_range_end[i-1] + 
                               ((buffer_sizes[i] + IPCMEM_BUF_HEADER)*num_buffers[i]);
      }
   }

   /* ensure we are dealing with valid buffer and pool */
   if(buffer && (poolid >= 0) && (poolid < IPCMEM_MAX_NBPOOLS))
   {
      /* range starts at end of prior pool if (poolid > 0), else it starts at ipcmem_heap */
      unsigned int pool_start = poolid ? pool_range_end[poolid - 1] : (unsigned int)(&(ipcmem_heap[0]));
      buffer_found_in_range = (int)((pool_start <= buffer) && (buffer < pool_range_end[poolid]));
   }

   return buffer_found_in_range; 
}  /* End ipcmem_buffer_in_pool_range( ) */


/*===========================================================================

FUNCTION IPCMEM_POOL_INIT

DESCRIPTION
  This function is used to initialize the Dynamic buffer pools. It creates
  the memory pool based on the different pool id (size) and number of 
  buffers for this pool id, as defined in ipcmem_pool_info. As such, these
  numbers should be tuned to achieve minimum yet sufficient memory usage.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from SEC_TASK

===========================================================================*/
void ipcmem_pool_init ( void )
{
  unsigned int i;

  MSG_LOW( "Total RAM use: %d", IPCMEM_DYN_MEMORY, 0, 0 );

  for ( i=0; i < IPCMEM_MAX_NBPOOLS; i++ )
  {
    /* check if the size or num of buffer is > 0 */
    if ( (ipcmem_pool_info[i].total_num != 0) && 
         (ipcmem_pool_info[i].size != 0) )
    {
      /* Now make pools of size and num specified in pool information data 
      ** structure. */
      if ( FALSE == ipcmem_make_pool((int)ipcmem_pool_info[i].size, 
		                       (int)ipcmem_pool_info[i].total_num))
      {
        break;  /* Break out, can't make buffer pools */
      } /* If make_pool() failed to create pool */
    } /* End if number and size of pool to create != 0 */
  } /* End for all the buffer pools to be created */

} /* End pdsm_pool_init() */


/*===========================================================================

FUNCTION IPCMEM_ALLOCATE

DESCRIPTION
    This function is used for dynamic buffer allocation. It searches
    in the buffer pool corresponding to the size requested. If no more
    free space in this pool id, it searches for the next higher sized
    memory pool. At the most two pools are searched for a free buffer
    to avoid memory waistage.

RETURN VALUE
    If successful, a pointer to the allocated memory block is returned.
    Otherwise, a NULL pointer is returned.

DEPENDENCIES

===========================================================================*/
void *ipcmem_allocate( size_t size )
{
   int i;

  ipcmem_bpool_type   *pool_ptr;  /* pointer to pool table */
  char entry         = NULL;      /* Indicate failure to get free buffer */
  int  *buffer       = NULL;      /* Buffer pointer to return to user */

  /*-----------------------------------------------------------------------*/
  /* Do sanity check */
  if (size == 0)
  {
    MSG_ERROR( "ipcmem_malloc of size 0", 0, 0, 0 );
    return ((void *)NULL);
  }

  for (i=0, pool_ptr = &bptab[0]; i < nbpools; pool_ptr++,i++)
  {
    /* Check if pool buffer size fits the requested buffer size 
    */
    if (pool_ptr->bpsize >= size)
    {
      /* Find a free buffer */
      if (pool_ptr->bpnext != NULL)
      { 
#ifdef FEATURE_SEC_MEM_TUNING
        if ( ipcmem_pool_info[i].h_watermark <=
             ipcmem_pool_info[i].num_used )
        {
          MSG_ERROR( "Secmem pool [%d] exceed high watermark", i, 0, 0 );
        }
        ipcmem_pool_info[i].num_used++;
#endif /* FEATURE_SEC_MEM_TUNING */

        buffer = pool_ptr->bpnext;
        pool_ptr->bpnext = (int *)*buffer; /* Remove this buffer and link
                                           ** the next free buffer in Q.
                                           */

        *buffer++ = i;  /* Store the poolid in allocated buffer and 
                           skip over by sizeof(int) to stand at IPC meta-data*/
        
        /* skip over IPC meta-data to give user access only to requested size */
        return ((void*)IPC_MSG_INT_TO_EXT(buffer)); 
      }
      else /* No free buffers in this pool */
      {
         /* If it fails to get buffer in next higher size
         ** pool, then we simply return with NULL pointer.
         */
         if (( i == nbpools - 1 ) || ( entry != 0 ))
         { 
           /* Failure in next higher pool */
           ERR_FATAL("No free memory pool for size %d", size, 0, 0);
           return ((void *)NULL); /*lint !e527 valid for other macro defines*/
         }
         entry++;    /*lint !e734*//* Indicate that pool has been found
                          ** and there are no free buffers in pool.
                          */
      } /* End else  if(bpnext!=NULL) */
    }/* End if pool buffer size > size requested */
        
  }/* End For all the buffer pools */

  return ((void *)buffer);
} /* End ipcmem_allocate() */


/*===========================================================================

FUNCTION IPCMEM_DEALLOCATE

DESCRIPTION
  This function is used for deallocating the memory. It also checks for 
  the possible memory corruptions.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void ipcmem_deallocate( void *buf )
{
  int poolid;  
  int *buffer;

  /* Check if poolid is proper in user given buffer pointer.
  */
  if ( buf == NULL)
  {
    return;
  }

  if ( ((char*)buf < ipcmem_heap)
       ||
       ((char*)buf > ipcmem_heap+IPCMEM_DYN_MEMORY)) /*lint !e413 */
  {
     ERR_FATAL("Buffer is not in Pool 0x%04x", buf, 0, 0);
  }

  /* recover IPC meta-data from user buf pointer */
  buffer = (int *)(IPC_MSG_EXT_TO_INT(buf));
  
  /* extract the poolid from buffer pointer by skipping back sizeof(int)
  */
  poolid = *(--buffer);

  /* If there is an error in pool id return error.
  */
  if( (poolid < 0) || (poolid >= nbpools) || !ipcmem_buffer_in_pool_range((unsigned int)buffer, poolid))
  {
    /* Indicate an error here, this can happen only when user is 
    ** trying to deallocate a wrong buffer or the pool id in buffer
    ** got corrupted.
    */
    ERR_FATAL("Error in free memory poolid %d", poolid, 0, 0);
  }
  else
  {
    /* Now link the buffer back to the free pool.
    */
    *buffer = (int)bptab[poolid].bpnext;
    bptab[poolid].bpnext = buffer;

#ifdef FEATURE_SEC_MEM_TUNING
    ipcmem_pool_info[poolid].num_used--;
#endif /* FEATURE_SEC_MEM_TUNING */
  } /* End Else when the pool id is proper. */
} /* End ipcmem_deallocate( ) */


/*===========================================================================

FUNCTION IPCMEM_REALLOC

DESCRIPTION
  This function changes the size of the block pointed to by ptr to
  size bytes and returns a pointer to the (possibly moved) block. 
  The contents are unchanged up to the lesser of the new and old sizes.
  If ptr is a null pointer, this function behaves like malloc() for 
  the specified size. If size is zero and ptr is not a null pointer, 
  the object it points to is freed.

RETURN VALUE
  If successful, a pointer to the re-allocated memory block is returned.
  Otherwise, a NULL pointer is returned.

DEPENDENCIES

===========================================================================*/
void *ipcmem_realloc( void *buf, size_t size )
{
   int poolid;
   void *buffer        = buf;   /* Buffer pointer to return to user */

   /* ----------------------------------------------------------------------*/

   /* If the buf is NULL, ipcmem_realloc behaves likes the ipcmem_malloc */
   if ( buf == NULL)
   {
      buffer = ipcmem_allocate(size);
   }
   else 
   {
      if (size == 0)
      {
         /* If size is zero and ptr is not a null pointer, just free 
         ** the block
         */
         ipcmem_deallocate(buf);
         buffer = (void *)NULL;
      }
      else
      {

         /* extract the poolid from buffer pointer.
         */
         poolid = *((int *)(IPC_MSG_EXT_TO_INT(buf)) - 1);

         /* If there is an error in pool id return error.
         */
         if( poolid < 0 || poolid >= nbpools )
         {
            /* Indicate an error here, this can happen only when user is
            ** trying to deallocate a wrong buffer or the pool id in buffer
            ** got corrupted.
            */
            ERR_FATAL("Error in realloc memory poolid %d", poolid, 0, 0);
            buffer = (void *)NULL;  /*lint !e527 valid for other macro defines*/
         }
#if 0
         else if ( ((poolid == 0) && 
                    (size < bptab[poolid].bpsize))
                   ||
                   ((poolid > 0) && 
                    (bptab[poolid-1].bpsize < size) && 
                    (size <= bptab[poolid].bpsize))
                   )
         {
            /* If the size fits in the current block, and there is no better
            ** way to alloc the memory, just return the current pointer.
            */
            buffer = buf;
         }
#endif
         else if (size <= bptab[poolid].bpsize)
         {
            /* If the size fits in the current block, just return the current
            ** pointer.
            */
            buffer = buf;
         }
         else
         {
            /* Do malloc first, and copy the contents from the old block to the
            ** new block, then free the old block. If the new block is shortened, 
            ** bytes are discarded off the end. If the block is lengthened, the 
            ** new bytes added are not initialized and will have garbage values.
            */
            buffer = ipcmem_allocate(size);

            if (buffer != NULL)
            {
               size_t bytes_to_copy = (size < bptab[poolid].bpsize) ? size : bptab[poolid].bpsize;
               bytes_to_copy += (IPCMEM_BUF_HEADER - sizeof(int)); /* each buffer has the same overhead, except the pool id */

               /* convert from EXT to INT to ensure that any meta-data associated also gets copied */
               (void)memcpy((void *)(IPC_MSG_EXT_TO_INT(buffer)), 
                                    (const void *)(IPC_MSG_EXT_TO_INT(buf)), 
                                    bytes_to_copy);
               ipcmem_deallocate(buf);
            }
         }
      }
   }

   return (buffer);
} /* End ipcmem_realloc() */

#endif /* FEATURE_REX_IPC */

