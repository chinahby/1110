/*=============================================================================

                OS  S e c o n d a r y   B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  This header file contains the routines to read the UID from the NAND
  One-Time-Programmable region.
  
EXTERNALIZED FUNCTIONS
  osbl_get_wm_uuid

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash_nand_otp_uid.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

           
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "osbl_error_handler.h"
#include "flash.h"
#include "flash_nand_msm.h"
#include "osbl_flash_nand_otp_uid.h"
#include "osbl_flash.h"
#include "smem.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Local definitions for flash UID format in OTP page 0. */

/* Number of bytes for signature in spare area. */
#define FLASH_OTP_SIG_BYTE_SIZE 2
/* Default number of signature sets in the spare area. */
#define FLASH_OTP_SIG_NUM_SETS 4
/* First byte of the signature. */
#define FLASH_OTP_SIG_BYTE1 0x55
/* Second byte of the signature. */
#define FLASH_OTP_SIG_BYTE2 0xAA

/* Flash UID in OTP page 0 is 16 bytes or 128 bits. */
#define FLASH_OTP_ID_BYTE_SIZE 16
/* Size of UID set is twice size of UID, includes UID and UID complement. */
#define FLASH_OTP_ID_SET_BYTE_SIZE ( 2 * FLASH_OTP_ID_BYTE_SIZE )
/* Default number of sets of UID and UID complement in UID Area 1. */
#define FLASH_OTP_ID_NUM_SETS 8
/* WM UUID size is equal to flash device UID size. */
#define OSBL_WM_UUID_BYTE_SIZE FLASH_OTP_ID_BYTE_SIZE


/* Internal structure used by UUID routines to define the OTP page format
   for parsing the UID. */
typedef struct
{
  boolean has_sig;    /* Is there a 0x55-0xAA signature in the spare data? */
  boolean x8_compat;  /* Is the device 16-bit but the OTP page is programmed
                         to be compatible with an 8-bit interface? */
  boolean num_uid_sets;  /* Total number of UID sets in Unique ID Area 1. */
  boolean num_sig_sets;  /* Total number of signature sets in spare area. */
} osbl_otp_formats_type;

/* Internal structure used by UUID routines to link a set of OTP page read
   parameters with a corresponding OTP page format for parsing the UID. */
typedef struct
{
  /* Set of OTP page read parameters for a flash device. */
  struct flash_otp_op_params params;

  /* Index into the OTP page format table for the flash device. */
  uint32 format_idx;
} osbl_otp_device_type;



/* Array of OTP page formats for known flash devices, for parsing UID. */
static const osbl_otp_formats_type osbl_otp_formats[] =
{
  /* Samsung, Format 0 */
  {
    TRUE,                  /* 0x55-0xAA signature exists in spare area. */
    FALSE,                 /* OTP page programmed using native device width. */
    FLASH_OTP_ID_NUM_SETS, /* OTP Unique ID Area 1 has default # of sets. */
    FLASH_OTP_SIG_NUM_SETS /* OTP signature area has default # of sets. */
  },
  /* Micron, Format 1 */
  {
    FALSE,                 /* 0x55-0xAA signature missing in spare area. */
    FALSE,                 /* OTP page programmed using native device width. */
    FLASH_OTP_ID_NUM_SETS, /* OTP Unique ID Area 1 has default # of sets. */
    FLASH_OTP_SIG_NUM_SETS /* OTP signature area has default # of sets. */
  },
  /* Toshiba x16, Format 2 */
  {
    TRUE,                  /* 0x55-0xAA signature exists in spare area. */
    TRUE,                  /* OTP page programmed using 8-bit interface. */
    FLASH_OTP_ID_NUM_SETS, /* OTP Unique ID Area 1 has default # of sets. */
    FLASH_OTP_SIG_NUM_SETS /* OTP signature area has default # of sets. */
  }
  /* Additional OTP page formats can be appended here at the end of array */
};
  
/* Table of OTP page read parameters for known devices.  As these parameters
   are subject to special vendor licensing agreements, no actual device
   parameters are provided by Qualcomm Inc. and instead only dummy example
   parameters are provided to illustrate the interface. The dummy example
   parameters are enclosed with the OSBL_OTP_DEVICES_DUMMY_EXAMPLE_ONLY
   conditional statement.  OEMs need to add entries to the array that 
   include the correct OTP cmd sequences. */
static const osbl_otp_device_type osbl_otp_devices[] = 
{
#ifdef OSBL_OTP_DEVICES_DUMMY_EXAMPLE_ONLY
  /* [DUMMY EXAMPLE] Samsung K9K2G08R0A */
  {
    {
      { 0xEC, 0xAA },  /* dev_id[] */
      2,               /* num_id_bytes */
      { 0x00, 0x00 }, /* otp_cmd[] */
      2,               /* num_cmd_bytes */
      FALSE            /* disable_ecc (ignored by API) */
    },
    0                  /* Use Samsung format 0 */
  },
  /* [DUMMY EXAMPLE] Samsung K9F1208Q0A */
  {
    {
      { 0xEC, 0x36 },  /* dev_id[] */
      2,               /* num_id_bytes */
      { 0xFF, 0xFF, 0xFF, 0xFF },  /* otp_cmd[] */
      4,               /* num_cmd_bytes */
      FALSE            /* disable_ecc (ignored by API) */
    },
    0                  /* Use Samsung format 0 */
  },
  /* [DUMMY EXAMPLE] Toshiba TH58DDM92F1XGJ5 */
  {
    {
      { 0x98, 0x46 },  /* dev_id[] */
      2,               /* num_id_bytes */
      { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 }, /* otp_cmd[] */
      6,               /* num_cmd_bytes */
      FALSE            /* disable_ecc (ignored by API) */
    },
    2                  /* Use Toshiba format 2 */
  },
#endif /* OSBL_OTP_DEVICES_DUMMY_EXAMPLE_ONLY */
/* Add entries that include the correct OTP cmd sequences here */
};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION OSBL_PARSE_AND_VALIDATE_OTP_UID

DESCRIPTION
  This function parses the UID from page 0 in the flash device OTP
  redundant block, using the UID format passed in as a parameter.  If 
  the UID is successfully parsed, then it is stored in the destination
  pointer passed as a parameter.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if UID was parsed successfully, FALSE otherwise

===========================================================================*/
static boolean osbl_parse_and_validate_otp_uid
(
  struct fs_device_data *nand_if,  /* Pointer to the public interface */
  const uint8 *page_buf,           /* The OTP page 0 data read from device */
  uint8 *uid_ptr,                  /* Destination pointer to store UID */
  const osbl_otp_formats_type *format  /* Format definition of OTP page */
)
{
  const uint8 *work_buf;
  int set, i;
  boolean success = FALSE;
  uint32 page_size;

  /* Amount to shift addresses by if the device has a 16-bit interface
     but the OTP page was programmed for compatibility with an 8-bit
     interface.  If 8-bit compatibility is not used then no shift is
     necessary. */
  uint32 x8_compat_shft = 0;
  /* Mask to use when reading 32-bit words from OTP page.  For 8-bit
     interface compatibility on 16-bit device, every other byte is masked. */
  uint32 x8_compat_mask = 0xFFFFFFFF;

  OSBL_VERIFY( nand_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Retrieve the page size of our flash device */
  page_size = nand_if->page_size( nand_if );

  if ( format->x8_compat )
  {
    /* If the OTP page was programmed on a 16-bit device with compatibility
       for an 8-bit interface, then mask off the upper byte of each
       16-bit word, and double the offset where we look for each byte
       in the Unique ID area and for the signature in the spare area. */
    x8_compat_shft = 1;
    x8_compat_mask = 0x00FF00FF;
  }

  /* First if the OTP page 0 format has a signature, then verify that the
     signature exists. */
  if ( format->has_sig )
  {
    /* Search the spare area for the OTP signature.  Verify that the signature
       is correct for one of the possible signature sets in the spare area. */
    for ( set = 0, work_buf = &page_buf[page_size]; 
          set < format->num_sig_sets; 
          ++set, work_buf += ( FLASH_OTP_SIG_BYTE_SIZE << x8_compat_shft ) )
    {
      /* If one of the signature sets matches, then the signature area
         has been verified. */
      if ( ( work_buf[0<<x8_compat_shft] == FLASH_OTP_SIG_BYTE1 ) && 
           ( work_buf[1<<x8_compat_shft] == FLASH_OTP_SIG_BYTE2 ) )
      {
        success = TRUE;
        break;
      }
    }

    /* Verify that we found the signature. */
    OSBL_VERIFY( success, BL_ERR_OSBL );
  }

  /* Second search the page data for an ID number area that satisfies the
     complimentary relationship. Verify that the complimentary relationship
     is valid for one of the possible UID sets in UID Area 1. */
  for ( set = 0, work_buf = page_buf; 
        set < format->num_uid_sets; 
        ++set, work_buf += ( FLASH_OTP_ID_SET_BYTE_SIZE << x8_compat_shft ) )
  {
    /* When the OTP page is programmed for 8-bit interface compatibility
       on an 8-bit device, then only every-other byte has UID data, then the
       size of each UID set in UID Area 1 is effectively doubled. */
    const uint32 *area_ptr = (const uint32*) work_buf;
    const uint32 *comp_ptr = 
      (const uint32*) &work_buf[FLASH_OTP_ID_BYTE_SIZE<<x8_compat_shft];
    success = TRUE;

    /* Check the complementary relationship for the current UID and UID
       complement set.  For efficiency perform comparison by 32-bit words
       rather than byte-wise. */
    for ( i = 0; area_ptr + i < comp_ptr; i++ )
    {
      /* Stop checking as soon as the complementary relationship is wrong. 
         If we are using 8-bit compatibility mode then compat_mask will 
         ignore every other byte. */
      if ( ( ( area_ptr[i] ^ ~comp_ptr[i] ) & x8_compat_mask ) != 0 )
      {
        success = FALSE;
        break;
      }
    }

    /* Stop searching if we found an area that satisfies the complementary
       relationship. */
    if ( success )
    {
      break;
    }
  }

  /* Verify that we found an ID number area satisfying the complimentary
     relationship. */
  if ( set < format->num_uid_sets )
  {
    /* Copy the UUID from the OTP page data into provided buffer. */
    if ( x8_compat_shft == 0 )
    {
      memcpy( uid_ptr, work_buf, FLASH_OTP_ID_BYTE_SIZE );
    }
    else
    {
      /* If we are using 8-bit compatibility mode then we can't memcpy
         the data, we instead have to copy byte-by-byte. */
      for ( i = 0; i < FLASH_OTP_ID_BYTE_SIZE; i++ )
      {
        uid_ptr[i] = work_buf[i<<x8_compat_shft];
      }
    }

    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* osbl_parse_and_validate_otp_uid() */

/*===========================================================================

FUNCTION OSBL_GET_WM_UUID

DESCRIPTION
  This function retrieves page 0 from the flash redundant block in the 
  OTP area, extracts and verifies the flash device UID (Unique IDentifier),
  and stores the value in shared memory for use by Windows Mobile as the UUID
  (Universally Unique IDentifier).

DEPENDENCIES
  NAND device must exist for boot or EFS.

RETURN VALUE
  None

===========================================================================*/
void osbl_get_wm_uuid( void )
{
  /* Pointer to the NAND flash driver interface */
  struct fs_device_data *nand_if;
  /* Pointer to store WM UUID in shared memory */
  uint8 *uuid_ptr;
  /* Make and device ID of the flash device */
  uint8 maker_id, dev_id;
  /* Index of OTP device parameters to use */
  uint32 otp_dev_num;
  /* Total number of OTP page read parameters defined in array */
  const uint32 otp_dev_limit = 
    sizeof(osbl_otp_devices)/sizeof(osbl_otp_devices[0]);
  boolean success;
  int rc;

  nand_if = boot_flash_dev_get_nand_if();

  OSBL_VERIFY( nand_if != NULL, BL_ERR_NULL_PTR );

  /* First get the Maker and Device ID of the flash device */
  maker_id = (uint8) nand_if->device_maker_id( nand_if );
  dev_id = (uint8) nand_if->device_id( nand_if );

  /* Now search to see if we have OTP page read parameters for the device */
  for ( otp_dev_num = 0; otp_dev_num < otp_dev_limit; otp_dev_num++ )
  {
    /* If the Maker and Device IDs match, we have OTP page read parameters 
       for the device */
    if ( maker_id == osbl_otp_devices[otp_dev_num].params.dev_id[0] && 
         dev_id == osbl_otp_devices[otp_dev_num].params.dev_id[1] )
    {
      break;
    }
  }

  /* Make sure we found the device in the paramters array */
  OSBL_VERIFY( otp_dev_num < otp_dev_limit, BL_ERR_OSBL );

  /* Set ECC generation state, Unique ID OTP data will always be read
     with ECC state disabled. */
  nand_if->set_ecc_state( nand_if, FLASH_NAND_ECC_DISABLED );

  /* Read the Unique ID data from flash OTP redundant block page 0, 
     using the OTP page read paramters for our device. */
  rc = nand_if->read_otp_page_and_spare( nand_if, 0, bl_page_buf, NULL, 0,
                            (void*) &osbl_otp_devices[otp_dev_num].params );
  OSBL_VERIFY( rc == FS_DEVICE_DONE, BL_ERR_BOOT_FLASH_ACCESS );

  /* Restore ECC generation state */
  nand_if->set_ecc_state( nand_if, FLASH_NAND_ECC_ENABLED );

  /* Allocate the WM UUID storage location in shared memory */
  uuid_ptr = (uint8*) smem_alloc( SMEM_WM_UUID, OSBL_WM_UUID_BYTE_SIZE );
  OSBL_VERIFY( uuid_ptr != NULL, BL_ERR_NULL_PTR );

  /* Now parse the UID from the flash OTP page and store the result in
     the location pointer to in shared memory.  Use the OTP page format
     associated with our device to parse the UID data. */
  success = osbl_parse_and_validate_otp_uid( nand_if, bl_page_buf, uuid_ptr,
             &osbl_otp_formats[osbl_otp_devices[otp_dev_num].format_idx] );
  
  /* Fail if we were not able to parse the UID data from the OTP page */
  OSBL_VERIFY( success, BL_ERR_OSBL );
  
} /* osbl_get_wm_uuid() */


