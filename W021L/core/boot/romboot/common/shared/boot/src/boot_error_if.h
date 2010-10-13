#ifndef BOOT_ERROR_IF_H
#define BOOT_ERROR_IF_H

/*===========================================================================

                          B o o t   L o a d e r
                         E r r o r  H a n d l e r
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the boot
  error handler interface.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_error_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef void (*boot_error_handler_ptr_type)
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      error             /* Enum that describes error type */
);

/*---------------------------------------------------------------------------
  Define the SBL ERROR types. This classifies different types of errors 
  encountered in the SBL.
 ---------------------------------------------------------------------------*/
typedef enum
{
  BL_ERR_NAND_TIMEOUT = 0x3000,     /* Timeout occurred while waiting for NAND 
                                        operation to complete */
  BL_ERR_NAND_CFG_INVALID,          /* NAND configuration data invalid */
  BL_ERR_FLASH_TYPE_NONE,           /* Failed to detect flash type booting 
                                        from */
  BL_ERR_NULL_PTR_PASSED,           /* Function input parameter is NULL */
  BL_ERR_BLHD_NOT_FOUND,            /* Boot loader header not found */
  BL_ERR_IMG_SIZE_ZERO,             /* Image to be loaded has size zero */
  BL_ERR_IMG_SIZE_TOO_BIG,          /* Image size is not within the range
                                        which partition table allows  */
  BL_ERR_IMG_NOT_FOUND,             /* Failed to find the image in flash */
  BL_ERR_IMG_OVWR_OSBL,             /* Image to be loaded would overwrite
                                        the OSBL memory space */
  BL_ERR_IMG_OVWR_APPSBL,           /* Image to be loaded would overwrite
                                        the APPS boot loader memory space */
  BL_ERR_IMG_OVWR_SMEM,             /* Image to be loaded would overwrite
                                        the shared memory segment */
  BL_ERR_SGMT_TYPE_INVALID,         /* Invalid segment type in ELF image */
  BL_ERR_NULL_PTR,                  /* Null pointer encountered */
  BL_ERR_NO_NONPGD_SEG_FOUND,       /* No non-paged segment found in the
                                        ELF image */
  BL_ERR_MAX_BBTB_TOO_BIG,          /* Bad block table does not fit in 
                                        one block of NAND flash */
  BL_ERR_TOO_MANY_BB,               /* Total bad block exceeds allowed 
                                        upper limit */
  BL_ERR_DM_INIT_FAIL,              /* Data mover initialization failed */
  BL_ERR_NAND_READ_FAIL,            /* Page read from NAND failed */
  BL_ERR_WRG_IMGHD_REQUESTED,       /* Invalid image header requested */
  BL_ERR_DM_CPY_FAIL,               /* Memcpy using Data Mover failed */
  BL_ERR_SEG_CPY_FAIL,              /* ELF Segment copy failed */
  BL_ERR_MULTI_HASH_SEG_FOUND,      /* Multiple hash segment found in
                                        ELF image */
  BL_ERR_NO_HASH_SEG_FOUND,         /* No hash segment found in ELF image */
  BL_ERR_HASH_MATCH_FAIL,           /* Image doesn't match hash segment or
                                        hash segment not present */
  BL_ERR_ELF_PARSE_FAIL,            /* Failed to parse ELF image header */
  BL_ERR_NAND_WRITE_FAIL,           /* Page write to NAND failed */
  BL_ERR_HW_REV_INVALID,            /* Invalid Hardware revision number */
  BL_ERR_CERT_INDEX_INVALID,        /* Commercial x509 certificate selected
                                        for authentication by PUB_KEY_SEL
                                        e-fuse does not exist */
  BL_ERR_IMG_AUTH_CODE_SIZE_ZERO,   /* Image code size to authenticate 
                                        is zero */
  BL_ERR_NULL_ATTEST_CRT_PTR,       /* Attestation Certification pointer 
                                        is NULL */
  BL_ERR_CODE_SIG_VERIFY_FAIL,      /* Signature verification failed */
  BL_ERR_OPER_ON_WRG_FLASH_TYPE,    /* Operation performed on wrong flash type */ 
  BL_ERR_OPER_INVALID_ON_NOR,       /* Invalid operation performed on NOR
                                        flash type */
  BL_ERR_MIBIB_NOT_FOUND,           /* MIBIB page not found in flash */
  BL_ERR_PARTI_TABLE_NOT_FOUND,     /* Partition table not found in flash */
  BL_ERR_WRG_PARTI_ENTRY_NAME_FMT,  /* Invalid partition entry format */
  BL_ERR_PARTI_ENTRY_NOT_FOUND,     /* Partition entry not found */
  BL_ERR_IMG_DEST_PTR_INVALID,      /* Invalid image destination pointer in 
                                        image header */
  BL_ERR_UNDEF_INSTR,               /* Undefined Instruction exception 
                                        occurred */
  BL_ERR_SWI,                       /* Software Interrupt (SWI) exception 
                                        occurred */
  BL_ERR_PREFETCH_ABORT,            /* Prefetch abort exception occurred */
  BL_ERR_DATA_ABORT,                /* Data abort exception occurred */
  BL_ERR_RESERVED_HANDLER,          /* Reserved exception occurred */
  BL_ERR_IRQ,                       /* IRQ exception occurred */
  BL_ERR_FIQ,                       /* FIQ exception occurred */
  BL_ERR_IMG_SECURITY_FAIL,         /* Image authentication failed */
  BL_ERR_OTHERS,                    /* Unsupported flash type and other
                                        unclassified errors */
  BL_ERR_OSBL,                    /* OSBL error */
  BL_ERR_AMSS_ENRTY_POINT_INVALID,  /* Entry Point into AMSS code base is not valid */
  BL_ERR_ROM_DATA_NOT_FOUND,        /* Data passed by ROM does not exist */
  BL_ERR_IMEM_PBL_LIB,               /* IMEM code or PBL library error */
  BL_ERR_C_LIB_RT,                  /* C library runtime exception occurred */
  BL_ERR_ELF_START,                 /* Placeholder for the first ELF error */
  BL_ERR_ELF_NONE = BL_ERR_ELF_START, /* Indicates no ELF error occurred */
  BL_ERR_ELF_NULL_PTR,              /* Null parameter to ELF loader */
  BL_ERR_ELF_INVAL_PARAM,           /* Otherwise invalid param to ELF loader */
  BL_ERR_ELF_FLASH,                 /* Flash-related error in ELF loader */
  BL_ERR_ELF_PARSE,                 /* Parse error in ELF loader */
  BL_ERR_ELF_FORMAT,                /* ELF file format error in ELF loader */
  BL_ERR_ELF_HASH_TABLE_MISSING,    /* Missing ELF hash segment */
  BL_ERR_ELF_HASH_MISMATCH,         /* Hash compare error in ELF loader */
  BL_ERR_ELF_END = BL_ERR_ELF_HASH_MISMATCH, /* Last ELF error placeholder */
  BL_ERR_BOOT_FLASH_ACCESS,         /* Error in the boot flash access module */
  BL_ERR_CLOBBER_PROTECTION_VIOLATION, /* Address range validation failed
                                          and a protected address range 
                                          would be clobbered */
  BL_ERR_FOTA_UPDATE,                /* FOTA Update error*/
  BL_ERR_FAIL_ALLOCATE_SMEM,
  BL_ERR_FAIL_SAVE_TO_SMEM,
  BL_ERR_FLASH_DRIVER,               /* Fatal error within the flash driver */
  BL_ERR_FEATURE_NOT_SUPPORTED,
  BL_ERR_RESOURCE_NOT_AVAILABLE
} bl_error_type;

/* The BL error callback function */
typedef void (*bl_error_callback_func_type)
(
  void *data_ptr
);

/* Structure for storing error handler callback data */
typedef struct bl_error_callback_node_type
{
  struct
  {
    bl_error_callback_func_type cb_func;
    void *data_ptr;
  } data;

  struct bl_error_callback_node_type *next_ptr;
} bl_error_callback_node_type;

/* The BL error interface */
typedef struct boot_error_if_type
{
  /*===========================================================================

  FUNCTION  ERROR_HANDLER

  DESCRIPTION
    This function handles boot errors.

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
boot_error_handler_ptr_type error_handler;

  /*===========================================================================

  FUNCTION  INSTALL_CALLBACK

  DESCRIPTION
    Installs a callback that will be executed when an error occurs.

    NOTE: The caller must allocate a node for use in the error callback
          linked list data structure, but does not need to initialize
          the structure, as this function does the initialization of
          the linked list data structure.

  DEPENDENCIES
    None
  
  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*install_callback)
  (
    bl_error_callback_func_type cb_func,  /* The callback function */
    void *data_ptr,       /* Data to pass to the callback function */
    bl_error_callback_node_type *node_ptr /* Data structure allocated by
                                        caller to store linked list node */
  );
} boot_error_if_type;

/* The global boot error interface structure */
extern boot_error_if_type bl_err_if;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO  BL_VERIFY

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, otherwise it will call the
  SBL error handler.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

===========================================================================*/
#define BL_VERIFY( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 bl_err_if.error_handler( __FILE__, __LINE__, \
                                        ( (uint32) ( error_type ) ) ); \
               } \
            } while(0)

/*===========================================================================

MACRO  BL_ERR_FATAL

DESCRIPTION
  This macro calls the error handler.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns.

===========================================================================*/
#define BL_ERR_FATAL( error_type )  \
            do{ \
                bl_err_if.error_handler( __FILE__, __LINE__, \
                                       ( (uint32) error_type ) ); \
              } while (0)

#endif  /* BOOT_ERROR_IF_H */
