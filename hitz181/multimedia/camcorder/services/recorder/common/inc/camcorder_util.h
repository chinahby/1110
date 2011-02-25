#ifndef CAMCORDER_UTIL_H
#define CAMCORDER_UTIL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     C A M C O R D E R   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains utility funcitons used by all camcorder subsystems.

REFERENCES
  none

EXTERNALIZED FUNCTIONS
  Memory heap tracking system.

Copyright(c) 2004-2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/camcorder_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/06   pavan   Revert Link error cleanup due to regressions uncovered
                   during video editing & cut and merge testing
03/29/06   jdas    Modified function prototype and structure to remove lint error
07/29/05   wyh     llmo routines will take malloc() and free() func pointer
03/16/05   wyh     Added the llmo (linked list memory object)
03/02/05   wyh     Fixed bug in realloc. Added malloc_verify()
02/09/05   wyh     Improved malloc debug. Implement fence and source tracking
01/31/05   wyh     Better define names.
12/29/04   wyh     Created file.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "queue.h"
#include "msg.h"  

/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/


//#define FEATURE_CAMCORDER_MALLOC

#ifdef FEATURE_CAMCORDER_MALLOC

#define CAMCORDER_UTIL_MALLOC_FENCE_SIZE 20
#define CAMCORDER_UTIL_MALLOC_FENCE_MAGIC   0xaa
#define CAMCORDER_UTIL_MALLOC_MODNAMESIZE 30

typedef struct {
  q_link_type    link;       /* link for queue */
  uint8         *ptr;        /* for application */
  uint8         *fence_ptr;  /* ptr plus fence */
  char           module[CAMCORDER_UTIL_MALLOC_MODNAMESIZE+1]; 
                            /* module name of allocator*/
  uint32         linenum;   /* linenum of code where allocated from */
  uint32         nbytes;
  msg_const_type *msg;      /* msg */
} camcorderutil_malloc_queue_element_type;

typedef enum
{
  CAMCORDERUTIL_MALLOC_STATE_IDLE   = 0xabcd,
  CAMCORDERUTIL_MALLOC_STATE_ACTIVE = 0xdcba
}camcorderutil_malloc_state;

typedef enum
{
  CAMUTIL_MALLOC_SUCCESS = 0,
  CAMUTIL_MALLOC_FAILURE = 1
} camcorderutil_malloc_ret_type;

typedef struct{
  camcorderutil_malloc_state    state;
  q_type       allocated_q;
  uint32       tot_allocated;
  uint32       max_heap_usage;  /*historical max heap use*/
  uint32       nmallocs;        /* nr of mallocs */
  char         module[CAMCORDER_UTIL_MALLOC_MODNAMESIZE+1];  /* initializer */
  uint32       linenum;         /* initializer info */
  msg_const_type *msg;            /* message */
} camcorderutil_malloc_context_type;
/* ==========================================================================
FUNCTION camcorderutil_malloc_init
DESCRIPTION
   Initializes a malloc context structure
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
camcorderutil_malloc_context_type *camcorderutil_malloc_init(
  char *module, uint32 linenum, msg_const_type *m1
  );
/* ==========================================================================
FUNCTION camcorderutil_malloc_terminate
DESCRIPTION
   Terminates a malloc context structure and prints out heap usages and
   memory leak information.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
camcorderutil_malloc_ret_type camcorderutil_malloc_terminate(
  camcorderutil_malloc_context_type *context,
  uint32 linenum, char *module, msg_const_type *m1
  );

/* ==========================================================================
FUNCTION camcorderutil_malloc
DESCRIPTION
   Allocate memory in a certain context.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void * camcorderutil_malloc(camcorderutil_malloc_context_type *context,
                      uint32           nbytes_requested,
                      char             *module_name,   /* allocator ID*/
                      uint32           linenum
                     );

/* ==========================================================================
FUNCTION camcorderutil_free
DESCRIPTION
   Free memory allocated in a certain context.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void camcorderutil_free(camcorderutil_malloc_context_type *context,
                        uint32 linenum, char *module, msg_const_type *m1,
                    void *ptr
                   );


camcorderutil_malloc_ret_type camcorderutil_malloc_verify(
  camcorderutil_malloc_context_type *context,
  uint32 linenum, char *module,msg_const_type *m1
  );

/* ==========================================================================
FUNCTION camcorderutil_realloc
DESCRIPTION
   Resize memory allocated in a certain context.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void * camcorderutil_realloc (camcorderutil_malloc_context_type *context,
                              uint32 linenum, char *module,
                              void *ptr,
                              uint32 nbytes
                   );

 
#endif  // ifdef FEATURE_CAMCORDER_MALLOC



/* a linked list memory object
*/

typedef void * (*CCD_MALLOC_FN)( uint32 uSize );
typedef void     (*CCD_FREE_FN)( void *ptr );

typedef struct
{
  uint32      nBytes;     /* nr of bytes allocated in data */
  void       *next;       /* pointer to next packet */
  void       *previous;   /* pointer to previous packet */

  int32      read_idx;   
  int32      write_idx; 

  int32      max_write;

  CCD_MALLOC_FN   mlc;   /* malloc function */
  CCD_FREE_FN     fr;    /* free function   */

} camcorderutil_llmo_header_type;

typedef struct
{
  camcorderutil_llmo_header_type header;
  uint8                         *data;
} camcorderutil_llmo_type;

typedef struct
{
  int32      size;
  int32      param1;
  int32      param2;
  int32      param3;
  int32      param4;
  int32      param5;
}camcorderutil_llmo_data_header_type;


camcorderutil_llmo_type *
camcorderutil_llmo_constructor(uint32 nBytes,CCD_MALLOC_FN mlc,CCD_FREE_FN fr);

void camcorderutil_llmo_destructor(
  camcorderutil_llmo_type *in);

uint8 * 
camcorderutil_llmo_read(
  camcorderutil_llmo_type **in,
  int32 *nBytes,
  camcorderutil_llmo_data_header_type *out_header
  );

/* return 0 for success, 1 for failure */
int
camcorderutil_llmo_write(
  camcorderutil_llmo_type **in,
  int32 nBytes,
  const uint8 *data,
  const camcorderutil_llmo_data_header_type *inp_header
  );


/*
** To write in place in memory, call
**    camcorderutil_llmo_write_inplace_step1()
**    to get target memory pointer and target memory size;
** Then call camcorderutil_llmo_write_inplace_step2()
**    to finalize the write
*/

/*  returns memory pointer
*/
uint8 *camcorderutil_llmo_write_inplace_step1(
  camcorderutil_llmo_type **inout,  
  int32 *nBytes  /* input=desired size, output=actual size */
  );

/* this will expand the size by adding another packet to
** the linked list.
**
** in and nBytes are input/output
*/
void camcorderutil_llmo_write_inplace_step2(
  camcorderutil_llmo_type *in,
  int32  nBytes_written,
  const camcorderutil_llmo_data_header_type *inp_header
  );



#endif  //CAMCORDER_UTIL_H
