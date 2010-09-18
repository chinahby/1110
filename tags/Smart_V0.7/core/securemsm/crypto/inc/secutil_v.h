#ifndef SECUTIL_V_H
#define SECUTIL_V_H

#include "customer.h"

#ifdef FEATURE_SEC

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secutil_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
95/21/07   avm     corrected RPC meta comments
04/04/07   avm     defined secutil rnd buf for compatibility with w/1x
02/27/07   df      new function secutil_common_wait()
12/18/06   df      support for dual PD, all rnd funcs now in secrnd
12/13/06   avm     fixed calls to banned API's
12/07/06   avm     Fixed Lint error type 650, 661, 662.
04/21/06   df      added secutil_b64fs_encode(), secutil_get/put_nv_item()
03/27/06   df      more changes to support secrnd task
03/22/06   df      changes to support FEATURE_SECRND
12/12/05   df      include stddef to resolve size_t
10/17/05   df      changed to "on demand" seeding
10/03/05   df      added mutex primitives for csprng
09/16/05   df      Added support for csprng periodic seeding
08/18/05   rv      added secutil_strnicmp (String compare Ignore Case)
11/12/04   rv      changes to correct LINT errors
02/18/04   rwh     Moved some char to long macros from DES code to secutil.h.
                   Added seek_write().
03/24/03   rwh     Provide QXDM output based on a run-time message level.
                   Added macros to handle pointer values between ARM/QDSP.
03/21/03   rwh     Removed a compile warning on an unsigned compare.
03/13/03   rwh     Commented out directives used in custssl.h file.
12/11/02   rwh     Added extern for seccrypt_initialized (CR 26278).
07/25/02   rwh     Modified the random data dump function to use a circular 
                   buffer, moved #def to custssl.h.
06/05/02   om      Added fuzzy string match prototypes
04/18/02   rwh     Added include of secerrno.h.
12/04/01   rwh     Changed log_opaque to log_opaque_level.
12/03/01   rwh     Added prototype for secutil_accept_random_data.
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "secerrno.h"
#include "nv.h"
#ifndef PLATFORM_LTK
#include <stddef.h>               /* resolves size_t */
#endif
#include "dog.h"
#include "rex.h"
#include "ixipc.h"


/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/*lint -e760 */  /*Redundant macro Symbol defined identically at Location */

/* common type for sec wait/ dog petting function */
typedef struct
{
  rex_tcb_type*   tcb;
  rex_timer_type* dog_timer;
  uint32          dog_rpt;
  uint32          dog_rpt_time;
  uint32          dog_rpt_timer_sig;
} sec_wait_type;

/* Byte order macros for ARM */
#ifndef TARGET_OS_UNIX
#define htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))
#define htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))
                  
#define ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))
#define ntohs(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))
        
/* Get the value of the most (or least) significant word of long x. */
#define MSW(x) ((uint16)(((uint32)(x) & 0xffff0000U) >> 16))
#define LSW(x) ((uint16)(((uint32)(x) & 0x0000ffffU)      ))

/* Swap MSW and LSW for ARM-to-QDSP transfers, assuming long word alignment.*/
#define htoql(x) \
        ((uint32)((((uint32)(x) & 0x0000ffffU) << 16) | \
                  (((uint32)(x) & 0xffff0000U) >> 16)))

/* Swap MSW and LSW for QDSP-to-ARM transfers, assuming long word alignment.*/
#define qtohl(x) \
        ((uint32)((((uint32)(x) & 0x0000ffffU) << 16) | \
                  (((uint32)(x) & 0xffff0000U) >> 16)))

#endif /* TARGET_OS_UNIX */

/* Copy 4 consecutive chars to a long. */
#define c2l(c,l)	(/*lint -e616 */l  = ((uint32)(*((c)++)))      , \
			             l |= ((uint32)(*((c)++))) <<  8, \
			             l |= ((uint32)(*((c)++))) << 16, \
			             l |= ((uint32)(*((c)++))) << 24  /*lint -e616 */)

/* Copy 1 to 7 consecutive chars to 2 longs. */
/* NOTE - c is not incremented as per c2l */
#define c2ln(c,l1,l2,n)	{/*lint -e616*/ \
                        c += n;      \
			l1 = l2 = 0; \
			switch (n) { \
			case 7: l2 |= ((uint32)(*(--(c)))) << 16; \
			case 6: l2 |= ((uint32)(*(--(c)))) <<  8; \
			case 5: l2 |= ((uint32)(*(--(c))))      ; \
			case 4: l1 |= ((uint32)(*(--(c)))) << 24; \
			case 3: l1 |= ((uint32)(*(--(c)))) << 16; \
			case 2: l1 |= ((uint32)(*(--(c)))) <<  8; \
			case 1: l1 |= ((uint32)(*(--(c))))      ; \
			default: break;} \
		/*lint +e616 */}

/* Copy a long value to 4 consecutive chars. */
#define l2c(l,c)	(/*lint -e616 */*((c)++) = (uint8)(((l)      ) & 0xff), \
			             *((c)++) = (uint8)(((l) >>  8) & 0xff), \
			             *((c)++) = (uint8)(((l) >> 16) & 0xff), \
			             *((c)++) = (uint8)(((l) >> 24) & 0xff)/*lint +e616 */)

/* Copy 2 long values to 1 to 7 consecutive chars. */
/* NOTE - c is not incremented as per l2c */
#define l2cn(l1,l2,c,n)	{/*lint -e616 */ \
                        c += n; \
			switch (n) { \
			case 7: *(--(c)) = (uint8)(((l2) >> 16) & 0xff); \
			case 6: *(--(c)) = (uint8)(((l2) >>  8) & 0xff); \
			case 5: *(--(c)) = (uint8)(((l2)      ) & 0xff); \
			case 4: *(--(c)) = (uint8)(((l1) >> 24) & 0xff); \
			case 3: *(--(c)) = (uint8)(((l1) >> 16) & 0xff); \
			case 2: *(--(c)) = (uint8)(((l1) >>  8) & 0xff); \
			case 1: *(--(c)) = (uint8)(((l1)      ) & 0xff); \
            default: break;  } \
		/*lint -e616 */}

/* Return the smallest integral value not less than x/y */
#define SECUTIL_CEIL(x, y) \
        ( (((x) % (y)) == 0) ? ((x) / (y)) : (((x) / (y)) + 1) )

/* Macro to set the tracer field of dsm item chains */
#ifdef FEATURE_DSM_MEM_CHK
  #define SECUTIL_DSM_SET_TRACER( msg_ptr, t )  \
      { dsm_item_type* itr = msg_ptr; \
        while ( itr != NULL ) { itr->tracer = t; itr = itr->pkt_ptr; } }
#else
  #define SECUTIL_DSM_SET_TRACER( msg_ptr, t )
#endif

/*===========================================================================
                      secutil rnd buf definition
===========================================================================*/

typedef struct
{
  boolean  done;       /* for compat w/1x */
}secutil_rnd_type;

extern secutil_rnd_type secutil_rnd;

/* end secutil rnd buf definition */

/* This variable is also declared in seccrypti.h */
// extern boolean seccrypt_initialized; 
                                        /* Declared here to avoid circular 
                                        dependencies. */

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


#define SECUTIL_DSM_PUSHDOWN( msg_ptr, src_ptr, len )            \
  if ( dsm_pushdown( &msg_ptr,                                   \
                     (uint8*) src_ptr,                           \
                     len,                                        \
                     DSM_DS_SMALL_ITEM_POOL )                    \
       != len )                                                  \
  {                                                              \
    ERR_FATAL( "PushDown failed", 0, 0, 0 );                     \
  }
  
#define SECUTIL_DSM_PUSHDOWN_TAIL( msg_ptr, src_ptr, len )       \
  if ( dsm_pushdown_tail( &msg_ptr,                              \
                          (uint8*) src_ptr,                      \
                          len,                                   \
                          DSM_DS_SMALL_ITEM_POOL )               \
       != len )                                                  \
  {                                                              \
    ERR_FATAL( "PushDownTail failed", 0, 0, 0 );                 \
  }


/* SEC_STRLCPY and SEC_STRLCAT macros are defined to move to using strlcpy and strlcat 
   when they are available in a target build. For now, we will just use strncpy and strncat
   instead. */
#ifdef STRLCPY_SUPPORTED

#define SEC_STRLCPY   strlcpy
#define SEC_STRLCAT   strlcat

#else

#define SEC_STRLCPY   sec_strlcpy
#define SEC_STRLCAT  sec_strlcat

size_t sec_strlcat(char* pcDst, const char* pszSrc, size_t nDestSize);
size_t sec_strlcpy(char* pcDst, const char* pszSrc, size_t nDestSize);

#endif /* STRLCPY_SUPPORTED */

/*==========================================================================
 FUNCTION SECUTIL_COMMON_WAIT
  
 DESCRIPTION 
   This function performs the SEC task waiting function. This function
   will not return until a signal from the requested mask is set.  To 
   ensure that the watchdog stays happy, this function internally makes
   sure that the watchdog is petted periodically.
   
   Note that if the task just uses rex_wait instead of secrnd_wait, then
   the task could wait on a signal for a long time, without ever petting
   the watchdog.  The watchdog expects all tasks to report in periodically
   (indicating good software operation); failure for this task to pet the
   watchdog will cause the phone to reset.  So each task must make its own
   specific wait routine, so that it takes care to pet the watchdog
   periodically.
   
 DEPENDENCIES 
   This function can be called from any sec task - known tasks as of now -
   sec_task, secrnd_task, seccryptarm_task, sfs_svc_task, etc.

 RETURN VALUE 
   A signal mask respresenting the signals which are currently set.  At
   least one signal in the requested mask will be set.  Any other signals
   which are set, which may or may not be spec'd in the signal mask, are
   also returned.
  
 SIDE EFFECTS
   None
===========================================================================*/
ixipc_sig_type secutil_common_wait
(
rex_sigs_type *requested_mask,              /* mask of signals to wait for */
uint32        *kxipc_signals,
sec_wait_type *wait
);

/*===========================================================================
FUNCTION SECUTIL_INET_ADDR

DESCRIPTION
  This function converts an Internet Address in dot notation
  into a word32 value in network order.

DEPENDENCIES
  None
  
PARAMETERS
  dot_addr_ptr - Pointer to string containing IP address

RETURN VALUE
  IP address in network order
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint32 secutil_inet_addr
(
  const char* dot_addr_ptr
);


/*===========================================================================

FUNCTION SECUTIL_BYTE_COMPARE

DESCRIPTION
  This function ...

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 secutil_byte_compare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint16  byte_len
);

/*===========================================================================

FUNCTION SECUTIL_IS_DIRECTORY

DESCRIPTION
  This function checks to see if dirname is a directory in efs

DEPENDENCIES
  None
  
PARAMETERS
  dirname the dir to check for existance

RETURN VALUE
  TRUE or 1 if dir exists, FALSE or 0 otherwise
  
SIDE EFFECTS
  None
===========================================================================*/
boolean secutil_is_directory(char *dirname);

#ifndef FEATURE_SECRND
/*===========================================================================

FUNCTION SECUTIL_PROCESS_CSPRNG_SIG

DESCRIPTION     
  seed the csprng

PARAMETERS

DEPENDENCIES    

RETURN VALUE

SIDE EFFECTS  

===========================================================================*/
void secutil_process_csprng_sig(void);
#endif

/*==========================================================================

FUNCTION SECUTIL_STR_TOLOWER

DESCRIPTION
  This function converts all characters in a string into lower case.

DEPENDENCIES
  None
  
PARAMETERS
  str  - Pointer to the string to be converted

RETURN VALUE
  A pointer to the converted string

DEPENDENCIES

==========================================================================*/
extern char* secutil_str_tolower
(
  char          *str
);
#if 0
/*==========================================================================

FUNCTION SECUTIL_STRSTRNC

DESCRIPTION
  This function looks for a token in a given string while ignoring
  the case of ASCII characters. It does not alter the passed in
  strings.

PARAMETERS
  string    - String to search in
  substring - Token to try to find in string

RETURN VALUE
  Pointer to the matching string, or NULL if no match was found.

DEPENDENCIES
  None.

==========================================================================*/
char* secutil_strstrnc
(
  char* string,
  char* substring
);

/*==========================================================================

FUNCTION SECUTIL_STRMATCH

DESCRIPTION
  This function gives a measure of how well two strings match. The matching 
  criteria is fuzzy, i.e. the function will try to match any substring
  of the token in the search string.
  If relative_match is TRUE, the result is the best match in percent
  of the original token size, if FALSE, the result is the size of the
  largest matching sub-token in characters.

PARAMETERS
  str1_ptr       - String to match with
  str2_ptr       - Same as str1_ptr
  ignore_case    - Set to TRUE to ignore character casing
  relative_match - Set to true to return the matching degree in percent

RETURN VALUE
  Measure of the matching degree.

DEPENDENCIES
  None.

==========================================================================*/
int secutil_strmatch
(
  char*  str1_ptr,
  char*  str2_ptr,
  int    ignore_case,
  int    relative_match
);

/*==========================================================================

FUNCTION SECUTIL_STRMATCH_LIST

DESCRIPTION
  This function finds the best relative match of a token in a list
  of strings.

PARAMETERS
  str_list_size - Number of search strings in list
  str_list      - Pointer to list of search strings
  token_ptr     - Token string to match against
  match_ptr     - Pointer set by func to best-matching index in above list
  ignore_case   - Set to TRUE to ignore character casing
  minimum_match_level - Minimum matching level in percent for acceptance

RETURN VALUE
  Security Services error number to indicate execution outcome.
  (see secerrno.h for details)

  Possible error conditions:
  E_INVALID_ARG   - Any parameter passed in is invalid
  E_FAILURE       - No acceptable match could be found

DEPENDENCIES
  None.

==========================================================================*/
secerrno_enum_type secutil_strmatch_list
(
  int     str_list_size,
  char*   str_list[],
  char*   token_ptr,
  int*    match_ptr,
  int     ignore_case,
  int     minimum_match_level
);

#endif /* #if 0 */
/*===========================================================================
FUNCTION SECUTIL_DSM_INSERT()

DESCRIPTION
 Copy data from a user-provided buffer to a DSM item, starting at
 'offset' bytes from start of dsm item and copying no more than 'len'
 bytes. Return actual number of bytes copied

DEPENDENCIES
  None

PARAMETERS
  packet_ptr - Pointer to packet to copy data to
  offset     - Offset this number of bytes from packet head. 
  buf        - Pointer to source buffer
  len        - Length in bytes to be inserted 

RETURN VALUE
  returns  0 if nothing

SIDE EFFECTS
  None
===========================================================================*/
uint16 secutil_dsm_insert
(
  dsm_item_type*  packet_ptr,
  uint16          offset,
  void*           buf,
  uint16          len
);

/*===========================================================================
FUNCTION  SECDSM_SEEK_WRITE()

DESCRIPTION
  This function will seek offset bytes into the item that is passed in.  If
  'buf' is not NULL it will copy 'len' bytes from it. If there are not 'len'
  bytes to receive all the butes from offset until the end of the item chain
  will be written.
  If the seek or the extract tries go beyond the end of the item chain the
  function will return FALSE, and neither item_head_ptr nor offset_ptr
  will be modified.  Otherwise item_head_ptr and offset_ptr will be set
  to indicate the write end point, the next available location for a 
  subsequent write.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to extract from 
  offset_ptr - Pointer to integer off bytes to offset into packet
  buf - Buffer to put extracted bytes into
  len - Pointer to integer to put number of bytes extracted

RETURN VALUE
  TRUE:  We have successfully seeked to the right byte, and if we were doing
         a write we copied ALL of the bytes indicated by 'len.'
           pkt_head_ptr: set to the item in which the write ended
           offset_ptr: set to the offset in the item in which the write ended
           len: if bytes were written this is set to the number that were

  FALSE: We have seeked past the end of the packet without finding the byte
         we are interesed in or we have seeked to the right place but not
         succeeded in copying all of the bytes.  Niether 'pkt_head_ptr' or
         'offset_ptr' are modified.  However:
             len: will be changed to reflect the number of bytes that were
                  copied


                                ***WARNING***
  Successful completion of this call WILL modify the first two arguments, and
  in all cases the last two arguments MAY be modified.
  

SIDE EFFECTS
  None
===========================================================================*/
boolean secdsm_seek_write
(
  dsm_item_type **pkt_head_ptr,
  uint16 *offset_ptr,
  void *buf,
  uint16 *len
);

/*===========================================================================
FUNCTION  SECUTIL_STRNICMP

DESCRIPTION
 compares not more than n characters (characters that follow a null
 character are not compared) from the array pointed to by s1 to the array
 pointed to by s2 ignoring the case.
 
DEPENDENCIES
  None

PARAMETERS
    str1 
    str2    2 Strings that have to be compared.
    n       # of bytes to be compared
    
RETURN VALUE
    an integer greater than, equal to, or less than zero, according
	as the string pointed to by s1 is greater than, equal to, or
	less than the string pointed to by s2.

SIDE EFFECTS
  None
===========================================================================*/
int secutil_strnicmp (const char* s1, const char* s2, size_t n);

/*===========================================================================

FUNCTION SECUTIL_B64FS_ENCODE

DESCRIPTION
  Base64 encode the source string and output to the destination string.
  The base64 encoding is modified to use only FS-compatible characters.

DEPENDENCIES
  None

PARAMETERS
  src_ptr            - Pointer to source string
  src_len            - Length of source string
  des_ptr            - Pointer to destination string

RETURN VALUE
  Length of destination string.

SIDE EFFECTS
  None
===========================================================================*/
size_t secutil_b64fs_encode( uint8 *src_ptr, size_t src_len, uint8 *des_ptr );

/*===========================================================================

FUNCTION SECUTIL_GET_NV_ITEM()

DESCRIPTION
  Read the requested item from NV.
  
DEPENDENCIES
  This task is started; NV task is started.

PARAMETERS
  item              - ID of item to get
  data_ptr          - Pointer to buffer to receive item value
  data_len          - Length of buffer to receive item value

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking until the NV item is read.
===========================================================================*/
secerrno_enum_type secutil_get_nv_item
( 
  nv_items_enum_type             item,           /* Item to access */
  uint8*                         data_ptr,
  uint16                         data_len
);

/*===========================================================================

FUNCTION SECUTIL_PUT_NV_ITEM()

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.
  
DEPENDENCIES
  This task is started; NV task is started.

PARAMETERS
  item              - ID of item to put
  data_ptr          - Pointer to data source 

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking until the NV item is written.
===========================================================================*/
secerrno_enum_type secutil_put_nv_item
( 
  nv_items_enum_type             item,           /* Item to access */
  uint8*                         data_ptr
);

#endif /* FEATURE_SEC */

#endif /* SECUTIL_V_H */
/*lint +e760 */
