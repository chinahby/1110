#ifndef DBL_ERROR_HANDLER_H
#define DBL_ERROR_HANDLER_H

/*===========================================================================

                    D E V I C E    B O O T    L O A D E R 
                         E r r o r  H a n d l e r
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the DBL
  error handler.  It is called in the event a DBL failure is detected.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_error_handler.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Define the DBL ERROR types. This classifies different types of errors 
  encountered in the SBL.
 ---------------------------------------------------------------------------*/
typedef enum
{
  DBL_ERR_NONE,
  DBL_ERR_DATA_FROM_PBL_ALTERED = 0x1000, /* Data coming from PBL has been altered */
  DBL_ERR_FLASH_TYPE_NONE,           /* Failed to detect flash type booting 
                                        from */
  DBL_ERR_FLASH_DATA_WIDTH_INVALID,  /* Data Width of the attached device 
                                        is not valid */  
  DBL_ERR_NO_ENTRY_IN_TABLE,         /* No Entry was found in the table
                                        */
  DBL_ERR_FLASH_PAGE_SIZE_INVALID,   /* Page Size of the attached device is not 
                                        valid */  
  DBL_ERR_IMG_TYPE_INVALID,          /* Image type not supported by DBL */                                        
  DBL_ERR_IMG_SIZE_ZERO,             /* Image to be loaded has size zero */
  DBL_ERR_IMG_SIZE_TOO_BIG,          /* Image size is not within the range
                                        which partition table allows  */
  DBL_ERR_IMG_NOT_FOUND,             /* Failed to find the image in flash */
  DBL_ERR_IMG_NAME_INVALID,          /* Image is not valid */
  DBL_ERR_NO_ENTRY_FOR_IMG,          /* Failed to find the image in flash */
  DBL_ERR_FAILED_TO_COPY_IMG,        /* Failed to copy the image from FLASH */  
  DBL_ERR_FAILED_TO_COPY_IMG_HDR,    /* Failed to copy the image from FLASH */  
  DBL_ERR_FAILED_TO_INIT_FSBL,       /* Failed to init FSBL image */    
  DBL_ERR_PBL_REG_DATA_MISSING,      /* The carry-over PBL register data
                                        is missing */
  DBL_ERR_GEN_CFG_ITEM_MISSING,      /* General configuration item does 
                                        not exist within configuration data */
  DBL_ERR_NOT_GEN_CFG_ITEM,          /* General Configuration item requested 
                                        is not a valid item */
  DBL_ERR_FSBL_HASH_DOES_NOT_MATCH,  /* FSBL's calculated hash does not match with
                                        stored value */
  DBL_ERR_NULL_PTR_PASSED,           /* Function input parameter is NULL */
  DBL_ERR_OSBLHD_NOT_FOUND,          /* OSBL image header not found 
                                        in flash */
  DBL_ERR_NULL_PTR,                  /* Null pointer encountered */
  DBL_ERR_NULL_IMG_HDR_PTR,          /* Null pointer encountered */
  DBL_ERR_NULL_FSBL_CONFIG_IF_PTR,   /* Null pointer encountered */
  DBL_ERR_NULL_HASH_IF_PTR,          /* Null pointer encountered */
  DBL_ERR_NULL_AUTH_IF_PTR,          /* Null pointer encountered */
  DBL_ERR_NULL_DLOAD_IF_PTR,         /* Null pointer encountered */
  DBL_ERR_CLK_SPEED_NOT_SUPPORTED,   /* Clock speed not supported */  
  DBL_ERR_CLK_CFG_FAILED,            /* Failed to configure CLK */  
  DBL_ERR_EBI1_CFG_FAILED,           /* Failed to configure EBI1 */
  DBL_ERR_SMI_CFG_FAILED,            /* Failed to configure SMI */  
  DBL_ERR_NAND_OP_CMD_TYPE_NONE,     /* Failed to detect flash operation 
                                        command */
  DBL_ERR_NAND_TIMEOUT,              /* Timeout occurred while waiting for NAND 
                                        operation to complete */
  DBL_ERR_NAND_READ_FAIL,            /* Page read from NAND failed */
  DBL_ERR_NOR_DETECTION_FAILED,      /* NOR detection failed */
  DBL_ERR_NOR_OP_TYPE_INVALID,       /* Invalid operation on NOR */
  DBL_ERR_PARSER_FAIL,               /* Fail to parse the config data */
  DBL_ERR_FAILED_TO_INIT_PBL_INTERFACE, /* Fail to init PBL interface */
  DBL_ERR_MIBIB_NOT_FOUND,           /* MIBIB page not found in flash */
  DBL_ERR_FAILED_TO_GET_IMG_INFO,    /* Failed to get image related information */
  DBL_ERR_PARTI_TABLE_NOT_FOUND,     /* Partition table not found in flash */
  DBL_ERR_WRG_PARTI_ENTRY_NAME_FMT,  /* Invalid partition entry format */
  DBL_ERR_PARTI_ENTRY_NOT_FOUND,     /* Partition entry not found */
  DBL_ERR_IMG_DEST_PTR_INVALID,      /* Invalid image destination pointer in 
                                        image header */
  DBL_ERR_UNDEF_INSTR,               /* Undefined Instruction exception 
                                        occurred */
  DBL_ERR_SWI,                       /* Software Interrupt (SWI) exception 
                                        occurred */
  DBL_ERR_PREFETCH_ABORT,            /* Prefetch abort exception occurred */
  DBL_ERR_DATA_ABORT,                /* Data abort exception occurred */
  DBL_ERR_RESERVED_HANDLER,          /* Reserved exception occurred */
  DBL_ERR_IRQ,                       /* IRQ exception occurred */
  DBL_ERR_FIQ,                       /* FIQ exception occurred */
  DBL_ERR_IMG_SECURITY_FAIL,         /* Image authentication failed */
  DBL_ERR_OTHERS,                    /* Unsupported flash type and other
                                        unclassified errors */
  DBL_ERR_C_LIB_RT,                  /* C library runtime exception occurred */
  DBL_ERR_IMG_CLOBBER_FAILURE,       /* Image would clobber protected memory */
  DBL_ERR_INVALID_PARAM,             /* Function parameter is invalid */
  DBL_ERR_ENUM            = 0x7FFFFFFF          /* Size enum to 32 bits. */
} dbl_error_type;

/*---------------------------------------------------------------------------
  Definitions of MAX_FILENAME_SIZE to be logged in on SBL error
---------------------------------------------------------------------------*/
#define MAX_FILENAME_SIZE 20 /* 20 * 4 = 80 chars */ 

/*---------------------------------------------------------------------------
  Define the SBL ERROR Log entry types. 
  ---------------------------------------------------------------------------*/
typedef struct dbl_err_log_entry
{
  uint32 error_type;
  uint32 line;
  uint32 filename[MAX_FILENAME_SIZE];
}dbl_err_log_entry_type;

typedef struct dbl_err_count
{
  uint32 error_count;
}dbl_err_count_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO  DBL_VERIFY

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
#define DBL_VERIFY( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 dbl_error_handler( __FILE__, __LINE__, error_type);\
               } \
            } while(0)

/*===========================================================================

MACRO  DBL_VERIFY_RET

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, it will return the error_type 
  supplied.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

===========================================================================*/
#define DBL_VERIFY_RET( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 return error_type;\
               } \
            } while(0)

/*===========================================================================

MACRO  DBL_ERR_FATAL

DESCRIPTION
  This macro calls the SBL error handler.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns.

===========================================================================*/
#define DBL_ERR_FATAL( error_type ) \
            do { \
              dbl_error_handler( __FILE__, __LINE__, error_type);\
            } while(0)

/*===========================================================================

FUNCTION  DBL_ERROR_HANDLER

DESCRIPTION
  This function is the error handler for the DBL. 
  This function is also shared by exception handler.

DEPENDENCIES
  Exception handler
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                  defined by __FILE__ */ 
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      dbl_error         /* Enum that describes error type */
);

#endif  /* DBL_ERROR_HANDLER_H */

