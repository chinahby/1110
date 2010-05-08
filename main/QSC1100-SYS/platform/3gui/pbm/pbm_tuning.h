#ifndef PBM_TUNING_H
#define PBM_TUNING_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER FUNCTIONS

GENERAL DESCRIPTION
  This file contains the constants that may be useful to customize
  for a particular phonebook implementation.

  Copyright (c) 2005-2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/pbm/pbm_tuning.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/08   cvs     Increase record size for non-EFS enabled PB's
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
03/12/07   cvs     Featurize 4 PB set support
02/09/07   cvs     Support for 4 phonebook sets
11/01/06   cvs     multi pb set fixes
06/12/06   cvs     add feature for 200 entry PB
03/27/06   cvs     Direct featurization to create EFS phonebook
09/22/05   cvs     Initial Revision
===========================================================================*/


#define PBM_MAX_RECORD_SIZE (PBM_MAX_EFS_REC_LEN)

/* These control the number of records and record sizes for the EFS phonebooks */
#if defined(FEATURE_PBM_USE_EFS_PB)
/* enable main EFS phonebook */
  #define PBM_MAX_SPEED_DIALS 99    /* Allow 99 EFS based Speed Dials. */
  #define PBM_MAX_EFS_REC_LEN 1500  /* Allow the records to become large */
  #define PBM_AVG_EFS_REC_LEN PBM_MAX_EFS_REC_LEN /* All records can be of max size,
                                   * used for allocating heap */
  #define PBM_MAX_FIELD_SIZE 256

                                   
  #if defined(FEATURE_PBM_USE_LARGE_EFS_PB)
    #define PBM_MAX_EFS_RECS     500   /* Allow 500 records */
    #define PBM_MAX_CALL_HISTORY 100
  #elif defined(FEATURE_PBM_USE_200_ENTRY_PB)
    #define PBM_MAX_EFS_RECS     200   /* Allow 200 records */
    #define PBM_MAX_CALL_HISTORY 10
  #else
    #define PBM_MAX_EFS_RECS     100   /* Allow 100 records */
    #define PBM_MAX_CALL_HISTORY 10
  #endif
#else /* FEATURE_PBM_USE_EFS_PB */
  #define PBM_MAX_SPEED_DIALS 0     /* No contact App, no EFS speed dials. */
  #define PBM_MAX_EFS_RECS    0     /* Nor EFS based records.              */
  #define PBM_MAX_EFS_REC_LEN 256   /* Max Call history sizes */
  #define PBM_AVG_EFS_REC_LEN PBM_MAX_EFS_REC_LEN /* For allocating heap */
  #define PBM_MAX_FIELD_SIZE  256
  #define PBM_MAX_CALL_HISTORY 10
#endif /* FEATURE_PBM_USE_EFS_PB */

#ifdef FEATURE_MMGSDI
#ifdef FEATURE_PBM_USIM_SUPPORT
  #ifdef FEATURE_USIM_1000_PBM
    #define PBM_MAX_ADN_RECS                  1004
  #else
    #define PBM_MAX_ADN_RECS                  502
  #endif
#else
  #define PBM_MAX_ADN_RECS                    251
#endif /* FEATURE_PBM_USIM_SUPPORT */
  #define PBM_MAX_FDN_RECS                  100
#else
  #define PBM_MAX_ADN_RECS                  0
  #define PBM_MAX_FDN_RECS                  0
#endif

/* These control garbage collection */
#define PBM_MAX_EFS_DELETES       10  /* Number of deletes before doing garbage
                                         collection. */
#define PBM_MAX_DELETE_SIZE       1000 /* When converting old files, if they
                                          are too big, reading the entire
                                          file would take too long. If we try to 
                                          do garbage collection with this many 
                                          deletes, we return PBM_ERROR*/
                                       


/* The following structures are used by the phone book manager's best fit
 * memory allocation system.  The default mem_malloc schemes are not well
 * suited for PBM's allocation needs.  In particular, circular files can
 * cause fragmentation issues in the heap resulting in the need for large 
 * areas of extra memory.  The PBM_MEM_HEADERS define is 
 * helpful to ensure the memory is not corrupted, however, it causes an
 * additional 8K of memory needs.  These are especially useful during testing.
 */
#define PBM_MEM_HEADERS
#define PBM_MEM_MULTIPLE_SIZE   16

#define PBM_TOTAL_FS_ENTRIES  (PBM_MAX_SPEED_DIALS +     \
                               PBM_MAX_EFS_RECS    +     \
                               (3*PBM_MAX_CALL_HISTORY))
#define PBM_TOTAL_UIM_ENTRIES (MAX_UIMS * (              \
                                (PBM_MAX_ADN_RECS +     \
                                 PBM_MAX_FDN_RECS +     \
                                 2*PBM_MAX_CALL_HISTORY \
                                 )))

#define PBM_HEAP_MIN_SIZE        (8*1024)   /* Overhead. */
#ifdef PBM_MEM_HEADERS
/* Figure 1K allocations needing an additional 8 bytes. */
  #define PBM_HEAP_ADDITIONAL_SIZE (8*1024)   /* 8K Additional for debug info */
#else
  #define PBM_HEAP_ADDITIONAL_SIZE 0
#endif

/* Figure out the total size needed. */
#define PBM_HEAP_MEM_BUFFER_SIZE (((                                        \
    PBM_HEAP_MIN_SIZE +                                                     \
    PBM_HEAP_ADDITIONAL_SIZE +                                              \
    (PBM_TOTAL_FS_ENTRIES * PBM_AVG_EFS_REC_LEN ) +                         \
    (PBM_TOTAL_UIM_ENTRIES * 100))/1024+1)*1024)
                         

#endif /* PBM_TUNING_H */
