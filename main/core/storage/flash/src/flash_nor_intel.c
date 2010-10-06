/**********************************************************************
 * flash_nor_intel.c
 *
 * INTEL specific flash support
 *
 * This file implements INTEL specific functions and data
 *
 * Copyright (c) 2005-2008 Qualcomm Incorporated. 
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 **********************************************************************/
 

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/drivers/flash/MSM_FLASH.01.04/flash_nor_intel.c#17 $ $DateTime: 2008/10/28 15:34:32 $ $Author: c_surven $


when         who     what, where, why
--------     ---     ----------------------------------------------------------
2008-10-17   sv      Fix W18 flash issue with bad powerdown 
2008-08-01   sv      Enable W18 NOR flash support 
2008-05-29   sv      Fix error handling, code clean ups
2008-05-21   sv      Fix M18 flash issue with bad powerdown scenario
2008-05-06   sv      Fix status check problem in partial buffer write
2008-05-06   sv      Add partition attribute setting for Half Density mode.
2008-04-17   sv      Fix Lint compiler warning(502) in line 1662
2008-03-11   sv      Modified for M18 half-density mode support 
2007-11-02   op      Fix Lint compiler warning "line 397:'start_wptr' may be 
                     used before being set."
2007-09-28   op      Fix Lint compiler warning.
2007-09-28   op      Add support NOR Sibley Intel
2007-02-28   mm      Incorporate NOR partitioning structural changes
2006-11-20   mm      Integrated onto 1.4 version of flash drivers to meet 
                     SC1x Requirement.
2006-01-23   dp      Cleaned up code and optimized some routines
2005-10-13   dp      Created initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "flash.h"
#include "flash_nor.h"
#include "flash_nor_device.h"
#include "flash_nor_msm.h"
#include "clk.h"
#include "flash_msg.h"

/* Verify the Flash content after Flash Erase/program operations */
#undef FLASH_CHECK

/* Intel CFI commands */
#define  FLASH_READ_ARRAY             0xFF
#define  FLASH_READ_STATUS            0x70
#define  FLASH_CLEAR_STATUS           0x50
#define  FLASH_ERASE_SETUP            0x20
#define  FLASH_UNLOCK_SETUP           0x60
#define  FLASH_CONFIRM                0xD0
#define  FLASH_SUSPEND                0xB0

/* Intel CFI Offsets */
#define  INTEL_PRI_CMDSET_OFFSET      0x13
#define  INTEL_HD_A_HALF_SEG_SIZE_OFFSET (0x10A + 0x35)
#define  INTEL_HD_B_HALF_SEG_SIZE_OFFSET (0x10A + 0x37)


/* Intel CFI Device Specific Values */
#define  INTEL_CMDSET_L18_LO          0x0001
#define  INTEL_CMDSET_L18_HI          0x0000
#define  INTEL_CMDSET_M18_LO          0x0000
#define  INTEL_CMDSET_M18_HI          0x0002
#define  INTEL_CMDSET_W18_LO          0x0003
#define  INTEL_CMDSET_W18_HI          0x0000

#define  INTEL_L18_WORD_WR_SETUP_CMD  0x0040
#define  INTEL_L18_BUF_WR_SETUP_CMD   0x00E8

#define  INTEL_M18_WORD_WR_SETUP_CMD  0x0041
#define  INTEL_M18_BUF_WR_SETUP_CMD   0x00E9

/* Intel Flash Status bits */
#define  FLASH_DWS                   (0x80)    /* Device Write Status */
#define  FLASH_ESS                   (0x40)    /* Erase Suspend Status */
#define  FLASH_ES                    (0x20)    /* Erase Status */
#define  FLASH_PS                    (0x10)    /* Program Status */
#define  FLASH_VPPS                  (0x08)    /* Vpp Status */
#define  FLASH_PSS                   (0x04)    /* Program Suspend Status */
#define  FLASH_BLS                   (0x02)    /* Block Locked Status */

/* Flash device is not ready(ie. busy) until DWS bit is 1 */
#define  FLASH_READY_BIT                     (FLASH_DWS)    

/* If any of these bits are set, there is a write error.
   0x10 = SR.4 - Program Status.
   0x08 = SR.3 - Vpp Status.
   0x02 = SR.1 - Block lock status. */
#define FLASH_WRITE_FAILURE_BITS             (FLASH_PS|FLASH_VPPS|FLASH_BLS)

/* If any of these bits are set, this is an erase error.
   0x20 = SR.5 - Erase Status.
   0x08 = SR.3 - Vpp Status.
   0x02 = SR.1 - Block lock status. */
#define FLASH_ERASE_FAILURE_BITS             (FLASH_ES|FLASH_VPPS|FLASH_BLS)

/* If any of these bits are set, this is an erase suspend error.
   0x20 - SR.5 - Erase Status.
   0x10 = SR.4 - Program Status.
   0x08 = SR.3 - Vpp Status.
   0x04 = SR.2 - Program Suspend Status
   0x02 = SR.1 - Block lock status. */
#define FLASH_ERASE_SUSPEND_FAILURE_BITS     (FLASH_ES|FLASH_PS|FLASH_VPPS| \
                                              FLASH_PSS|FLASH_BLS)
/* If any of these bits are set, before starting an operation , there is 
   an error which was unhandled.

   0x20 - SR.5 - Erase Status.
   0x10 = SR.4 - Program Status.
   0x08 = SR.3 - Vpp Status.
   0x04 = SR.2 - Program Suspend Status
   0x02 = SR.1 - Block lock status. */

#define FLASH_ALL_STATUS_FAILURE_BITS      (FLASH_ES|FLASH_PS|FLASH_VPPS| \
                                           FLASH_PSS|FLASH_BLS)

/* If both of these bits are set after an operation , a fatal 
   error(command sequence error) has occured.
   0x20 - SR.5 - Erase Status.
   0x10 = SR.4 - Program Status. */
#define FLASH_CMD_SEQ_ERR_BITS             (FLASH_PS | FLASH_ES)

/* If any of these bits are set after an operation , a fatal 
   error has occured.
   0x08 = SR.3 - Vpp Status.
   0x02 = SR.1 - Block lock status. */
#define FLASH_FATAL_ERR_BITS             (FLASH_VPPS | FLASH_BLS)


/* The flash response time after which the program bails out */
#define FLASH_OPERATION_BAILOUT_TIME 5000

/* The poll timing is specified in microseconds */
#define FLASH_POLL_TIME 1

/* Maximum number of simultaneous read pointers used by driver clients by
 * invoking read_ptr() API when M18 is being used in half-density mode.
 * Currently only client supported is EFS2. This value is used for sizing a
 * statically allocated buffer utilized by read_ptr() API and should be tuned
 * for specific clients for optimal RAM usage. 
 */
#define FLASH_HD_MAX_READ_PAGES  4

/* Size of each buffer used by read ptr function for M18 device. This value
 * should be equal to or more than the logical page size used by used by the
 * file system.
 */
#define FLASH_HD_READ_PTR_BUF_BSIZE (FLASH_NOR_DEFAULT_PAGE_SIZE)


/* Forward function prototypes */
LOCAL flash_status flash_intel_configure (fsi_device_t self, 
                                          flash_ptr_type baseaddr);

LOCAL flash_status flash_intel_erase_start (flash_ptr_type baseaddr, 
                                            dword offset);

LOCAL flash_status flash_intel_erase_status (flashi_nor_device *nor_device, 
                                             flash_ptr_type eraseaddr);

LOCAL flash_status flash_intel_suspend (flash_ptr_type wptr);

LOCAL flash_status flash_intel_buffer_write (byte *buffer, 
                                             flash_ptr_type baseaddr, 
                                             dword offset, 
                                             dword count);
LOCAL flash_status flash_intel_word_write(byte *buffer,
                                          flash_ptr_type baseaddr, 
                                          dword offset, 
                                          dword count);

LOCAL flash_status flash_intel_resume (flash_ptr_type eraseaddr);

LOCAL flash_status flash_intel_hd_word_read (flash_ptr_type read_addr, 
                                             byte *buffer, 
                                             dword count);

LOCAL void * flash_intel_hd_read_ptr (flash_ptr_type page_addr, dword count);

LOCAL flash_status flash_intel_hd_is_erased (flash_ptr_type page_address, 
                                              dword page_size);

LOCAL flash_ptr_type __flash_intel_convert_to_hd_addr (flash_ptr_type fd_addr, 
                                                        dword flash_base); 

LOCAL boolean __flash_intel_check_ready_status (volatile word *addr);

/* These functions are located in flash_ram.c. */
extern void flash_intel_copy_cfi_data(volatile word *baseaddr, word *dest, word count);
extern void flash_intel_get_id_codes(volatile word *baseaddr, word *dest);

extern flash_geometry_info flash_geometry_data;

static word flash_word_pgrm_setup_cmd = 0; /* word program set up command */
static word flash_buf_prgm_setup_cmd = 0;  /* buffer program set up command */
static dword flash_write_buf_bsize = 0;     /* device write buf size in bytes */


static word flash_hd_a_half_seg_bsize = 0; /* Size of A-Half part of a Segment*/
static word flash_hd_b_half_seg_bsize = 0; /* Size of B-Half part of a Segment*/

/* Local copy of base address of NOR device */
static dword flash_base_addr = 0;
/* Flag which indicates mode (half or full density) of operation */
static boolean flash_m18_hd_enabled = FALSE;
/* Buffer used by the read ptr() function */
static byte flash_hd_read_buf[FLASH_HD_MAX_READ_PAGES]
                              [FLASH_HD_READ_PTR_BUF_BSIZE];


struct flashi_nor_dev_ops flash_intel_op_functions_L18 =
{
  flash_intel_configure,                    /* Configuration of hardware */
  fsi_nor_device_worded_read,               /* Read operation. */
  flash_intel_word_write,                   /* Worded write for L18 */
  flash_intel_erase_start,                  /* Erase for Intel. */
  flash_intel_erase_status,                 /* Status for Intel. */
  flash_intel_suspend,                      /* Suspend Erase for Intel. */
  flash_intel_resume,                       /* Resume Erase for Intel. */
  NULL,
  NULL,
  NULL
};

struct flashi_nor_dev_ops flash_intel_op_functions_W18 =
{
  flash_intel_configure,                     /* Configuration of hardware */
  fsi_nor_device_worded_read,                /* Read operation. */
  flash_intel_word_write,                    /* Worded write for W18 */
  flash_intel_erase_start,                   /* Erase for Intel. */
  flash_intel_erase_status,                  /* Erase Status for Intel. */
  flash_intel_suspend,                       /* Suspend Erase for Intel. */
  flash_intel_resume,                        /* Resume Erase for Intel. */
  NULL,
  NULL,
  NULL
};

struct flashi_nor_dev_ops flash_intel_op_functions_M18 =
{
  flash_intel_configure,                    /* Configuration of hardware */
  fsi_nor_device_worded_read,               /* Read operation. */
  flash_intel_buffer_write,                 /* Buffered write for M18 */
  flash_intel_erase_start,                  /* Erase for Intel. */
  flash_intel_erase_status,                 /* Status for Intel. */
  flash_intel_suspend,                      /* Suspend Erase for Intel. */
  flash_intel_resume,                       /* Resume Erase for Intel. */
  NULL,
  NULL,
  NULL
};


/* Statically defined geometry sturctures for devices supported in partitioned
 * mode. These are only used for generating system partition table when CFI
 * data is not available. Probe routine uses geometry information from CFI
 * table. 
 */

flash_geometry_info Intel_64W18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_8MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,
  2,
  {
    {127,  65536},
    {  8,   8192},
    {  0,      0},
    {  0,      0},
    {  0,      0},
  },  /*lint !e785*/
  135,
  16,
  {
    {0,  8, 7,  0}, 
    {8,  8, 15, 0}, 
    {16, 8, 23, 0}, 
    {24, 8, 31, 0}, 
    {32, 8, 39, 0}, 
    {40, 8, 47, 0}, 
    {48, 8, 55, 0}, 
    {56, 8, 63, 0}, 
    {64, 8, 71, 0}, 
    {72, 8, 79, 0}, 
    {80, 8, 87, 0}, 
    {88, 8, 95, 0}, 
    {96, 8, 103, 0}, 
    {104, 8,111, 0}, 
    {112, 8, 119, 0}, 
    {120, 15, 134, 0}
  } 
};

flash_geometry_info M36W0R5040U6ZS_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_4MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_NOT_PRESENT,
  2,
  {
    { 63,  65536},
    {  8,   8192},
    {  0,      0},
    {  0,      0},
    {  0,      0},
  },  /*lint !e785*/
  71,
  8,
  {
    {0,  8, 7,  0}, 
    {8,  8, 15, 0}, 
    {16, 8, 23, 0}, 
    {24, 8, 31, 0}, 
    {32, 8, 39, 0}, 
    {40, 8, 47, 0}, 
    {48, 8, 55, 0}, 
    {56,15, 70, 0}
  } 
};

flash_geometry_info L18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_64MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_64,
  3,
  {
    {  4,  32768},
    {510, 131072},
    {  4,  32768},
    {  0,      0},
    {  0,      0},
  },  /*lint !e785*/
  518,
  0
};

flash_geometry_info Intel_128M18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_16MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_1024,
  1,
  {
    {64, 262144},
    {  0,      0},
    {  0,      0},
    {  0,      0},
    {  0,      0}
  },  /*lint !e785*/
  64,
  8,
  {
    { 0,  8,  7,  0},
    { 8,  8,  15,  0},
    { 16,  8, 23,  0},
    { 24,  8, 31,  0},
    { 32,  8, 39,  0},
    { 40,  8, 47,  0},
    { 48,  8, 55,  0},
    { 56,  8, 63,  0}
  }
};

flash_geometry_info Intel_256M18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_32MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_1024,
  1,
  {
    {128, 262144},
    {  0,      0},
    {  0,      0},
    {  0,      0},
    {  0,      0}
  },  /*lint !e785*/
  128,
  8,
  {
    { 0,  16,  15,  0},
    { 16,  16, 31,  0},
    { 32,  16, 47,  0},
    { 48,  16, 63,  0},
    { 64,  16, 79,  0},
    { 80,  16, 95,  0},
    { 96,  16, 111,  0},
    { 112,  16, 127,  0}
  }
};

flash_geometry_info Intel_512M18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_64MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_1024,
  1,
  {
    {256, 262144},
    {  0,      0},
    {  0,      0},
    {  0,      0},
    {  0,      0}
  },  /*lint !e785*/
  256,
  8,
  {
    {  0,  32,  31,  0},
    { 32,  32,  63,  0},
    { 64,  32,  95,  0},
    { 96,  32,  127,  0},
    {128,  32, 159,  0},
    {160,  32, 191,  0},
    {192,  32, 223,  0},
    {224,  32, 255,  0}
  }
};

flash_geometry_info Intel_1024M18_geometry =
{
  FLASH_INTEL_FAMILY,
  FLASH_SIZE_128MB,
  FLASH_XIFACE_16,
  FLASH_WBUF_1024,
  1,
  {
    {512, 262144},
    {  0,      0},
    {  0,      0},
    {  0,      0},
    {  0,      0}
  },  /*lint !e785*/
  512,
  8,
  {
    {  0,  64,  63,  0},
    { 64,  64,  127,  0},
    { 128,  64, 191,  0},
    { 192,  64, 255,  0},
    { 256,  64, 319,  0},
    { 320,  64, 383,  0},
    { 384,  64, 447,  0},
    { 448,  64, 511,  0}
  }
};


 /* The fsi_device structure will have minimal configuration information such 
  * as number of manufacture codes, actual manufacture codes, byte offset to 
  * EFS2 area, size in words of EFS2 and the necessary function pointers. The
  * other data such as number of sectors and the sector layout, etc are 
  * computed at run time depending on the flash used. So they have intialized 
  * value set as INIT_USING_CFI_AT_RUNTIME
  */  
flashi_nor_device Intel_PH28F256L18B85 = {
  "INTEL PH28F256L18B85",
  2,                             /* # of codes to match */    
  { 0x89, 0x8810 },              /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_PAIRED_BITS,  /* L18 uses MLC array */
  0x0,
  64,
  INIT_USING_CFI_AT_RUNTIME,
  &L18_geometry,
  &flash_intel_op_functions_L18
};

flashi_nor_device Intel_64W18_ADMux = {
  "INTEL 64 W18 ADMux",
  2,                             /* # of codes to match */    
  { 0x20, 0x88C0 },              /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,       /* W18 uses SLC array */
  0x0,
  0,
  INIT_USING_CFI_AT_RUNTIME,
  &Intel_64W18_geometry,
  &flash_intel_op_functions_W18
};

flashi_nor_device M36W0R5040U6ZS = 
{
  "NUMONYX M36W0R5040U6ZS",
  2,                             /* # of codes to match */    
  { 0x20, 0x8828 },              /* Manufacture codes. */
  0,
  FS_DEVICE_WRITES_SIMPLE,       /* W18 uses SLC array */
  0x0,
  0,
  INIT_USING_CFI_AT_RUNTIME,
  &M36W0R5040U6ZS_geometry,
  &flash_intel_op_functions_W18
};

flashi_nor_device Intel_1024M18_ADMux =
{
  "INTEL 1024 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x88B1 },              /* Manufacture codes. */
  ( 0 | FLASHI_DEV_CTRL_OPTS_HD_MODE ),
  FS_DEVICE_WRITES_PAIRED_BITS,
  0x0,
  1024,
  INIT_USING_CFI_AT_RUNTIME,
  &Intel_1024M18_geometry,
  &flash_intel_op_functions_M18
};

flashi_nor_device Intel_512M18_ADMux =
{
  "INTEL 512 M18 ADMux",
  2,                             /* # of codes to match */    
  { 0x89, 0x8881 },              /* Manufacture codes. */
  ( 0 | FLASHI_DEV_CTRL_OPTS_HD_MODE ),
  FS_DEVICE_WRITES_PAIRED_BITS,
  0x0,
  1024,
  INIT_USING_CFI_AT_RUNTIME,
  &Intel_512M18_geometry,
  &flash_intel_op_functions_M18
};



/*===========================================================================

FUNCTION __flash_intel_buffer_write

DESCRIPTION
  Write blocks of flash_write_buf_bsize bytes to an Intel part using 
  the buffer write algorithm.

DEPENDENCIES
  buffer points to a word aligned address
  dest_bptr points to a word aligned address
  count is even and multiple of flash_write_buf_bsize
  
RETURN VALUE
  FLASH_SUCCESS   -  If write completed normally
  FLASH_FAILURE   -  If write operation failed

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status __flash_intel_buffer_write(volatile byte *dest_bptr,
                                              const byte *buffer, 
                                              dword byte_count)
{
  dword loop_count;
  volatile word  *start_wptr;
  volatile word  *dest_wptr  = (volatile word *)dest_bptr;
  volatile word  *wptr = (volatile word *) buffer;
  word dest_addr_jump_boffset = 8;
  flash_status status = FLASH_SUCCESS;

  start_wptr = dest_wptr;
  *start_wptr = FLASH_READ_STATUS;

  if(__flash_intel_check_ready_status(start_wptr) != TRUE)
  {
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: buffer write - device not ready", 0, 0, 0);
  }
  else if(((*start_wptr) & FLASH_ALL_STATUS_FAILURE_BITS) != 0)
  {      
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: buffer write - error value in SR val = 0x%X", \
                     *start_wptr, 0, 0);
  }


  for (; byte_count; byte_count -= flash_write_buf_bsize)
  {
    start_wptr = dest_wptr;
    *dest_wptr = flash_buf_prgm_setup_cmd;
    if(__flash_intel_check_ready_status(start_wptr) != TRUE)
    {
      status = FLASH_FAILURE;
      FLASH_ERR_FATAL ("fni: buffer write - write setup timed out", 0, 0, 0);
    }
   
    /* write the number of words */
    *dest_wptr = (word)((flash_write_buf_bsize >> 1) - 1);
    if (flash_m18_hd_enabled == TRUE)
    {
      /* If we crossed over the upper half(A Half) of the segment, skip rest
       * of current segment(B Half) by adjusting the pointer to the beginning
       * of next segment. Calculate the jump offset here.
       */
      dest_addr_jump_boffset += (flash_hd_b_half_seg_bsize >> 1);
    }
    /* Some loop unrolling to speed up */
    for (loop_count = flash_write_buf_bsize >> 4; loop_count != 0 ; 
         loop_count--)
    {
      dest_wptr[0] = wptr[0];
      dest_wptr[1] = wptr[1];
      dest_wptr[2] = wptr[2];
      dest_wptr[3] = wptr[3];

      dest_wptr[4] = wptr[4];
      dest_wptr[5] = wptr[5];
      dest_wptr[6] = wptr[6];
      dest_wptr[7] = wptr[7];      

      dest_wptr += dest_addr_jump_boffset;
      wptr += 8;
      KICK_DOG_AND_CHECK_DATA();
    }

    *start_wptr = FLASH_CONFIRM;

    /* Wait for the write to complete.  When it does, the Write
     State Machine Status bit will go from Busy to Ready. */
    
    *start_wptr = FLASH_READ_STATUS;

    if (__flash_intel_check_ready_status(start_wptr) != TRUE)
    {
      status = FLASH_FAILURE;
      FLASH_ERR_FATAL ("fni: buffer write - write confirm timed out ", 0, 0, 0);
    }
    
    if (((*start_wptr) & FLASH_WRITE_FAILURE_BITS) != 0)
    {
      status = FLASH_FAILURE;
      if ((((*start_wptr) & FLASH_CMD_SEQ_ERR_BITS) == FLASH_CMD_SEQ_ERR_BITS) ||
          ((*start_wptr) & FLASH_FATAL_ERR_BITS)) 
      {
        FLASH_ERR_FATAL ("fni: buffer write - fatal error occured ", 0, 0, 0);
      }
      
      *start_wptr = FLASH_CLEAR_STATUS;
      TPRINTF(3, ("fni: buff write, bad status: 0x%02x",
                *start_wptr, 0, 0));
      break;
    } // End if Flash Write fail
  }// end for ; byte_count

  /* set flash back to array mode */
  *start_wptr = FLASH_READ_ARRAY; 
  return status;
}

/*===========================================================================
FUNCTION __flash_intel_partial_buffer_write

DESCRIPTION
  Write even number of bytes (less than flash_write_buf_bsize to an Intel 
  part using the buffer write algorithm.

DEPENDENCIES
  buffer points to a word aligned address
  dest_bptr points to a word aligned address
  count is even 
  
RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_FAILURE      -  If write operation failed

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status __flash_intel_partial_buffer_write(volatile byte *dest_bptr,
                                                      const byte *buffer, 
                                                      dword byte_count)
{
  dword loop_count;
  volatile word  *start_wptr;
  volatile word  *dest_wptr  = (volatile word *)dest_bptr;
  volatile word  *wptr = (volatile word *) buffer;
  flash_status status = FLASH_SUCCESS;

  start_wptr = dest_wptr;

  *start_wptr = FLASH_READ_STATUS;  
  if(__flash_intel_check_ready_status(start_wptr) != TRUE)
  {
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: buffer write - device not ready", 0, 0, 0);
  }
  else if(((*start_wptr) & FLASH_ALL_STATUS_FAILURE_BITS) != 0)
  {      
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: buffer write - error value in SR val = 0x%X", \
                         *start_wptr, 0, 0);
  }
  else
  {  
    *dest_wptr = flash_buf_prgm_setup_cmd;
    if(__flash_intel_check_ready_status(dest_wptr) != TRUE)
    {
      status = FLASH_FAILURE; 
      FLASH_ERR_FATAL ("fni: Timed out on partial write setup\n", 0, 0, 0);      
    }
    else
    {
      /* write the number of words */
      *dest_wptr = (word)((byte_count >> 1) - 1);                                
      for (loop_count = (byte_count >> 1); loop_count != 0 ; loop_count--)
      {
        *(dest_wptr++)  = *(wptr++);
        /* If we crossed over to the upper half of the segment, skip rest of
         * current segment by adjusting the pointer to the beginning of next
         * segment.
         */
        if ( (flash_m18_hd_enabled == TRUE) && 
             (((uint32)dest_wptr & (flash_hd_a_half_seg_bsize - 1)) == 0) )
        {
          dest_wptr += flash_hd_b_half_seg_bsize >> 1;
        }
      }

      *start_wptr = FLASH_CONFIRM;
  
      /* Wait for the write to complete.  When it does, the Write
         State Machine Status bit will go from Busy to Ready. */
      
      *start_wptr = FLASH_READ_STATUS;

      if(__flash_intel_check_ready_status(start_wptr) != TRUE)
      {
        status = FLASH_FAILURE;
        FLASH_ERR_FATAL ("fni: Timed out on partial write confirm\n", 0, 0, 0);
      
      }
      else
      { 
        if (((*start_wptr) & FLASH_WRITE_FAILURE_BITS) != 0)
        {
          status = FLASH_FAILURE;
          if ((((*start_wptr) & FLASH_CMD_SEQ_ERR_BITS) == 
            FLASH_CMD_SEQ_ERR_BITS) || ((*start_wptr) & FLASH_FATAL_ERR_BITS))
          {
            FLASH_ERR_FATAL ("fni: buffer write - fatal error occured ", 0, 0, 0);
          }

          TPRINTF(3, ("fni: partial buff write, bad status: 0x%02x",
                  *start_wptr, 0, 0));
          *start_wptr = FLASH_CLEAR_STATUS;
        }
      }
    }
  }
  *start_wptr = FLASH_READ_ARRAY; 

  return status;
}


/*===========================================================================

FUNCTION flash_intel_word_write

DESCRIPTION
  Programs words to an Intel part using the word program algorithm.

DEPENDENCIES
  buffer points to a word aligned address
  dest_bptr points to a word aligned address
  count is even 
  
RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_FAILURE      -  If write operation failed

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status flash_intel_word_write(byte *buffer, 
                                   flash_ptr_type baseaddr, 
                                   dword offset, 
                                   dword count)
{
  dword loop_count;
  volatile word *dest_wptr;
  volatile word *start_wptr;
  volatile word  *wptr = (volatile word *) buffer;
  flash_status status = FLASH_SUCCESS;

#ifdef FLASH_CHECK
  dword size = count;
  byte *source = buffer;
#endif

  TPRINTF (3, ("fni: Write offset 0x%x \n", offset));

  /* Base address of operation. */
  dest_wptr = (((volatile word *) baseaddr) + BYTE_TO_WORD_OFFSET(offset));

  start_wptr = dest_wptr;

  *dest_wptr = FLASH_READ_STATUS;  
  
  if(__flash_intel_check_ready_status(dest_wptr) != TRUE)
  {
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: word write - device not ready", 0, 0, 0);
  }
  else if(((*dest_wptr) & FLASH_ALL_STATUS_FAILURE_BITS) != 0)
  {      
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: word write - error value in SR val = 0x%X", \
                         *dest_wptr, 0, 0);
  }
  
  /* Make sure Flash base byte pointer, buffer and byte_count are 
     word aligned / even This is an optimization for EFS2 calls. */
  if( ((dword)dest_wptr & 1)  || 
      ((dword)buffer & 1)     || 
      (count & 1))
  {
    /* We should not come here as all EFS2 calls are currently word aligned
       and only even number of bytes */
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: misaligned/odd byte writes requested", 0, 0, 0);
  }

  for (loop_count = (count >> 1); loop_count != 0 ; loop_count--)
  {
    if((loop_count % 16) == 0)
    {
      KICK_DOG_AND_CHECK_DATA();
    }
    /* Program Setup command */

    *dest_wptr = flash_word_pgrm_setup_cmd;
    
    if(__flash_intel_check_ready_status(dest_wptr) != TRUE) 
    {
      status = FLASH_FAILURE;
      FLASH_ERR_FATAL ("fni: word write - setup timed out", 0, 0, 0);
    }
    /* Data to be written */
    *(dest_wptr)  = *(wptr);

    /* Check the status register for Flash Ready indicator */

    if(__flash_intel_check_ready_status(dest_wptr) != TRUE)
    {
      status = FLASH_FAILURE;
      FLASH_ERR_FATAL ("fni: word write - write timed out", 0, 0, 0);
    }
    /* increment the counter to the next word */
    dest_wptr++;
    wptr++;
  }
  
 /* Check the operation status  */
  dest_wptr--;
  if ((*dest_wptr & FLASH_WRITE_FAILURE_BITS) != 0)
  {
    if ((((*dest_wptr) & FLASH_CMD_SEQ_ERR_BITS) == FLASH_CMD_SEQ_ERR_BITS) ||
        ((*dest_wptr) & FLASH_FATAL_ERR_BITS))
    {
      status = FLASH_FAILURE;
      FLASH_ERR_FATAL ("fni: word write - fatal error occured ", 0, 0, 0);
    }

    TPRINTF(3, ("fni: write, bad status: 0x%02x",
            *dest_wptr, 0, 0));

    /* Set the return status to FAILURE as program error has occured */
    status = FLASH_FAILURE;

    /* Clear the status register */
    *start_wptr = FLASH_CLEAR_STATUS;
    *dest_wptr = FLASH_CLEAR_STATUS;
  }

  *start_wptr = FLASH_READ_ARRAY;
  *dest_wptr = FLASH_READ_ARRAY;

#ifdef FLASH_CHECK
  if(status == FLASH_SUCCESS)
  {

    dword i;
    volatile byte *part_base = ((volatile byte *) baseaddr + offset);

    for (i = 0; i < size; i++)
    {
      if(part_base[i] != source[i])
      {
        TPRINTF (3, ("fni: Wr Vrfy Fail : Status 0x%x \n", status));
        TPRINTF (3, ("fni: Wr Failed : blk 0x%x addr 0x%x \n", part_base, i));
        FLASH_ERR_FATAL("Write verify failed", 0, 0, 0);
      }

      if ((i % 16)==0)
      {
        KICK_DOG_AND_CHECK_DATA();
      }
    }
  }
#endif

  return status;
}


/*===========================================================================

FUNCTION intel_probe

DESCRIPTION
  Detect the presence of an Intel flash device.

DEPENDENCIES
  None

RETURN VALUE
  NULL           - No Intel device detected.
  otherwise      - Pointer to flashi_nor_device

SIDE EFFECTS
  None

===========================================================================*/
flashi_nor_device *intel_probe (volatile word *baseaddr)
{
  word codes[4];
  flashi_nor_device const **dev;
  word saved_int;

  /* Perform the ARM specific probe without interrupt */
  INTLOCK_SAV (saved_int);

  /* Call function to go get the codes from the device */
  flash_intel_copy_cfi_data(baseaddr, codes, 2);
  /* Retrieve Command set IDs from CFI Data */
  flash_intel_copy_cfi_data(baseaddr+INTEL_PRI_CMDSET_OFFSET, codes+2, 2);
  INTFREE_SAV (saved_int);

  /*  See if we can find a match for the MFG and part ID */
  for (dev = intel_parts; *dev != NULL; dev++)
  {
    if ((codes[0] == (*dev)->codes[0]) && 
        (codes[1] == (*dev)->codes[1]))
    {
      /* Match primary vendor cmd set id codes to determine cmd set to use */
      if ((codes[2] == INTEL_CMDSET_M18_LO) &&
         (codes[3] == INTEL_CMDSET_M18_HI))
      {
        flash_word_pgrm_setup_cmd = INTEL_M18_WORD_WR_SETUP_CMD;
        flash_buf_prgm_setup_cmd = INTEL_M18_BUF_WR_SETUP_CMD;
      }
      else if ( ((codes[2] == INTEL_CMDSET_L18_LO) &&
                 (codes[3] == INTEL_CMDSET_L18_HI)) ||
                ((codes[2] == INTEL_CMDSET_W18_LO) && 
                 (codes[3] == INTEL_CMDSET_W18_HI)))
      {
        flash_word_pgrm_setup_cmd = INTEL_L18_WORD_WR_SETUP_CMD;
        flash_buf_prgm_setup_cmd = INTEL_L18_BUF_WR_SETUP_CMD;
      }
      break;
    }
  }

  /* 
   * If we did not find a match, the dev pointer is holding NULL
   * which indicates to the calling function that we did not
   * find a match, otherwise, it points to the device table for
   * the type of flash part we identified.
   */
  return (flashi_nor_device *) *dev;
}

/*===========================================================================

FUNCTION flash_intel_erase

DESCRIPTION
  Initiate erase operation for an Intel part.

DEPENDENCIES
  fs_dev_base must be set.

RETURN VALUE
  FLASH_SUCCESS            -  If erase operation was successfully initiated
  FLASH_FAILURE            -  If erase operation could not otherwise be started

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status flash_intel_erase_start (flash_ptr_type baseaddr, 
                                          dword offset)
{
  volatile word *eraseaddr;
  flash_status status = FLASH_SUCCESS;
  
  /* If we made it here, then the component is now sitting in read
     mode.  Our offset is in bytes from the beginning of NV, get a
     pointer to the real data. */
  eraseaddr = (volatile word *)(baseaddr + BYTE_TO_WORD_OFFSET(offset));

  /* Adjust sector address if device is operating in half-density mode */
  if (flash_m18_hd_enabled == TRUE)
  {
    eraseaddr = __flash_intel_convert_to_hd_addr ((flash_ptr_type)eraseaddr,
                                              flash_base_addr);
  }
 
  *eraseaddr = FLASH_READ_STATUS;
  if(__flash_intel_check_ready_status(eraseaddr) != TRUE)
  {
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: erase start - device not ready", 0, 0, 0);
  }
  else if(((*eraseaddr) & FLASH_ALL_STATUS_FAILURE_BITS) != 0)
  {   
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: erase start - error value in SR, val = 0x%X", \
                         *eraseaddr, 0, 0);   
  }

  *eraseaddr = FLASH_ERASE_SETUP;
  *eraseaddr = FLASH_CONFIRM;

  return status;
}

/*===========================================================================

FUNCTION flash_intel_erase_status

DESCRIPTION
  Erase status for Intel components.

DEPENDENCIES
  The device must be in the erasing state.

RETURN VALUE
  FLASH_SUCCESS             - The erase is finished, component is not in
                            read state
  FLASH_OP_NOT_COMPLETE     - The erase is still happening.
  FLASH_FAILURE             - Something went wrong with the erase.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status flash_intel_erase_status (flashi_nor_device *nor_device, 
                                             flash_ptr_type eraseaddr)
{
  word value;
  flash_status status = FLASH_SUCCESS;

  /* Adjust address if device is operating in half-density mode */
  if (flash_m18_hd_enabled == TRUE)
  {
    eraseaddr = __flash_intel_convert_to_hd_addr (eraseaddr, flash_base_addr);
  }
  *eraseaddr = FLASH_READ_STATUS;
  value = *eraseaddr;

  if ((value & FLASH_READY_BIT) == 0)
  {
    status = FLASH_OP_NOT_COMPLETE ;
  }
  else
  {  
    *eraseaddr = FLASH_READ_ARRAY;   
    if ((value & FLASH_ERASE_FAILURE_BITS) != 0)
    {
      status = FLASH_FAILURE;
      
      if (((value & FLASH_CMD_SEQ_ERR_BITS) == FLASH_CMD_SEQ_ERR_BITS) ||
          (value & FLASH_FATAL_ERR_BITS))
      {
        FLASH_ERR_FATAL ("fni: buffer write - fatal error occured ", 0, 0, 0);
      }
      /* Erase failed.  Clear the status register. */
      *eraseaddr = FLASH_CLEAR_STATUS;
    
      TPRINTF (3, ("Erase Status Failure : 0x%x \n", *eraseaddr));
    }
  #ifdef FLASH_CHECK
    else
    {
      dword i;
    #if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
      extern dword curr_erase_sector_bsize;
      dword size = curr_erase_sector_bsize >> 1;
    #else
      dword size = flash_nor_find_size_for_block_addr (nor_device, 
        (dword)eraseaddr);       
      size >>= 1;
    #endif

      for (i = 0; i < size; i++)
      {
        if(eraseaddr[i] != 0xFFFF)
        {
          status = FLASH_FAILURE;
          FLASH_ERR_FATAL("Erase verify failed", 0, 0, 0);
        }
        if ((i % 16)==0)
        {
          KICK_DOG_AND_CHECK_DATA();
        }
      }
    }
  #endif
  }
  return status;
}

/*===========================================================================

FUNCTION flash_intel_buffer_write

DESCRIPTION
  Write a block of even number of bytes to an Intel part.

DEPENDENCIES
  The device must not be in erasing state.
  fs_dev_base must be set.
  buffer and offset are word aligned. byte_count is even multiple

RETURN VALUE
  FLASH_SUCCESS      -  If write completed normally
  FLASH_FAILURE      -  If write operation failed

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status flash_intel_buffer_write (byte *buffer, 
                                             flash_ptr_type baseaddr, 
                                             dword offset, 
                                             dword count)
{
  volatile byte *wptr;
  flash_status status = FLASH_SUCCESS;
  dword partial_bcount;
#ifdef FLASH_CHECK
  dword size = count;
  byte *source = buffer;
#endif

  
  /* Base address of operation. */
  wptr = ((volatile byte *) baseaddr + offset);

  /* Adjust address if device is operating in half-density mode */
  if (flash_m18_hd_enabled == TRUE)
  {
    wptr = (byte *)__flash_intel_convert_to_hd_addr ((flash_ptr_type)wptr, 
                                                      flash_base_addr);
  }

  TPRINTF(3, ("fni: write addr = 0x%x, size = %u bytes \n",
                  wptr, count));

  /* Make sure flash base byte pointer, buffer and byte_count are word
   * aligned. This is an optimization assuming that only word aligned
   * requests will be made. */
  if( ((dword)wptr & 1)   || 
      ((dword)buffer & 1) || 
      (count & 1))
  {
    /* We should not come here as all file system calls are currently word
     * aligned with only even number of bytes */
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("flash_nor_intel: misaligned/odd byte writes requested",
      0, 0, 0);
  }
  
  if (count >= flash_write_buf_bsize)
  {
    /* Handle even bytes more than buffer write size bytes */
    partial_bcount = count & ~(flash_write_buf_bsize - 1);

    TPRINTF(3, ("fni: buff_write addr = 0x%x, size = %u bytes \n",
                  wptr, partial_bcount));
    status = __flash_intel_buffer_write(wptr, buffer, partial_bcount);    
    TPRINTF (3, ("Write : offset 0x%x \n", wptr));
    count -= partial_bcount;
    if(flash_m18_hd_enabled == TRUE)
    {
      wptr += partial_bcount << 1;
    }
    else
    {
      wptr += partial_bcount;
    }
    buffer += partial_bcount;
  }

  if ((count > 0) && (status == FLASH_SUCCESS))
  {
    /* Handle even bytes less than buffer write size bytes */
    TPRINTF(3, ("fni: pbuff_write addr = 0x%x, size = %u bytes \n",
                  wptr, count));
    status = __flash_intel_partial_buffer_write (wptr, buffer, count);
  }

#ifdef FLASH_CHECK
  if(status == FLASH_SUCCESS)
  {
    dword i;
    volatile byte *check_addr = ((volatile byte *) baseaddr + offset);
    dword byte_compare_offset = 0;
    
    /* Adjust address if device is operating in half-density mode */
    if (flash_m18_hd_enabled == TRUE)
    {
      check_addr = \
       (byte *)__flash_intel_convert_to_hd_addr ((flash_ptr_type)check_addr,
                                                  flash_base_addr);
    }
    size = count;
    for (i = 0; i < size; i++)
    {
      if(check_addr[byte_compare_offset++] != source[i])
      {
        status = FLASH_FAILURE;
        FLASH_ERR_FATAL("Write verify failed",0,0,0);
      }

      /* If the address is in the B half of the segment and m18 half-density
       * mode is enabled then adjust it to point to the A half of next segment. 
       */
      if ((flash_m18_hd_enabled == TRUE) && ((((uint32)check_addr + 
           byte_compare_offset) & (flash_hd_a_half_seg_bsize - 1)) == 0))
      {
        byte_compare_offset += flash_hd_b_half_seg_bsize;
      }
      if ((i % 16) == 0)
      {
        KICK_DOG_AND_CHECK_DATA();
      }
    }
  }
#endif
  TPRINTF(3, ("fni: Return Status for start addr ( 0x%x ) = %d \n",
                  wptr, status));
  return status;
} /* flash_intel_buffer_write */

/*===========================================================================
FUNCTION flash_intel_suspend

DESCRIPTION
  Suspend an erase operation on Intel.  

DEPENDENCIES
  The device be in erasing state.
  fs_dev_base must be set.
  
RETURN VALUE
  FLASH_SUCCESS          - The erase has completed.
  FLASH_OP_NOT_COMPLETE  - The erase was suspended, reads may be done.
  FLASH_FAILURE          - The suspend failed, or an erase was not in
                           progress.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status flash_intel_suspend (flash_ptr_type wptr)
{
  flash_status status = FLASH_SUCCESS;

  /* Adjust address if device is operating in half-density mode */
  if (flash_m18_hd_enabled == TRUE)
  {
    wptr = __flash_intel_convert_to_hd_addr (wptr, flash_base_addr);
  }
  
/* Issue the suspend erase command. */
  *wptr = FLASH_SUSPEND;
  *wptr = FLASH_READ_STATUS;
  
  if(__flash_intel_check_ready_status(wptr) != TRUE)
  {
    status = FLASH_FAILURE;
    FLASH_ERR_FATAL ("fni: suspend - timed out ", 0, 0, 0);
  }
  else
  {
    /* 0x40 - SR.6 - Erase Suspend Status 
     0 - Erase suspend not in effect
     1 - Erase suspend in effect */
    if ((*wptr & FLASH_ESS) == 0)
    {
      status = FLASH_SUCCESS;
    }
    else
    {
      if ((*wptr & FLASH_ERASE_SUSPEND_FAILURE_BITS) != 0)
      {
        if (((*wptr & FLASH_CMD_SEQ_ERR_BITS) == FLASH_CMD_SEQ_ERR_BITS) ||
          (*wptr & FLASH_FATAL_ERR_BITS))
        {
          status = FLASH_FAILURE;
          FLASH_ERR_FATAL ("fni: suspend - fatal error occured ", 0, 0, 0);
        }

       *wptr = FLASH_CLEAR_STATUS;
        TPRINTF(3, ("fni: invalid status: (0x%x)\n",
          *wptr, 0, 0));
        status = FLASH_FAILURE;
      }
      else
      {
        status = FLASH_OP_NOT_COMPLETE ;
      }
    }
  }
  
  *wptr = FLASH_READ_ARRAY;
  return status;
}

/*===========================================================================
FUNCTION flash_intel_resume

DESCRIPTION
  Resume a suspended erase.

DEPENDENCIES
  fs_dev_base must be set.
  
RETURN VALUE
  FLASH_SUCCESS             - The erase has been resumed.
  FLASH_FAILURE             - An error occurred.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL flash_status flash_intel_resume (flash_ptr_type eraseaddr)
{
  /* Adjust address if device is operating in half-density mode */
  if (flash_m18_hd_enabled == TRUE)
  {
    eraseaddr = __flash_intel_convert_to_hd_addr (eraseaddr, flash_base_addr);
  }
  KICK_DOG_AND_CHECK_DATA();
  /* Issue the resume command. */
  *eraseaddr = FLASH_CONFIRM;
  *eraseaddr = FLASH_READ_STATUS;
  
  return FLASH_SUCCESS;
}

/*===========================================================================
FUNCTION flash_intel_configure

DESCRIPTION
  Do any necessary initialization.

DEPENDENCIES
  Must be done before any operations are carried out on the flash hardware.
  fs_dev_base must be set.
  
RETURN VALUE
  FLASH_SUCCESS             - Success
  FLASH_FAILURE             - An error occurred.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL flash_status flash_intel_configure (fsi_device_t self, 
                                          flash_ptr_type probe_addr)
{
  flash_status status = FLASH_SUCCESS;
  flashi_nor_device *nor_device = &(self->nor_dev_info);

  flash_base_addr = (dword)nor_device->base_addr;
 
  status = flash_nor_geometry_init(nor_device, probe_addr);

  if(status != FLASH_SUCCESS)
  {
    return status;
  }
  /* Get the write buffer wize from geometry data and store it in a
   * global which will be used by other routines in the file 
   */
  flash_write_buf_bsize  = (1 << ((dword)nor_device->geometry->write_buf_size));

#if defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)
  {
    uint32 i, j;
    uint32 num_sectors;
    volatile byte *sectorptr;
    flash_ptr_type  wptr;
    
    sectorptr = (volatile byte *)flash_base_addr;

    /* Unlock the sectors for programming */
    for(j = 0; ((j < MAX_FLASH_REGIONS) ); j++)
    {
      if (flash_geometry_data.blk_regions[j].numblocks <= 0)
      {
        break;
      }
      num_sectors = flash_geometry_data.blk_regions[j].numblocks;
    
      /* Unlock all sectors in the NV area */
      for (i = 0; i < num_sectors; i++)
      {
        wptr = (volatile word *)sectorptr;

        /* Check if the flash has been left in any suspend state. 
         If so resume the suspend state and reset the status */
        *wptr = FLASH_CLEAR_STATUS;                

        KICK_DOG_AND_CHECK_DATA();
        
        /* Unlock sector */
        *wptr = FLASH_UNLOCK_SETUP;
        *wptr = FLASH_CONFIRM;
        *wptr = FLASH_READ_ARRAY;
 
        /* update pointer for next sector */
        sectorptr = sectorptr + \
          flash_geometry_data.blk_regions[j].blocksize_bytes;
        
        *wptr = FLASH_CONFIRM;
        *wptr = FLASH_READ_STATUS;
         
         if(__flash_intel_check_ready_status(wptr) != TRUE)
         {
           FLASH_ERR_FATAL ("fni: configure - resume timed out", 0, 0, 0);
         }
         else if(((*wptr) & FLASH_ERASE_FAILURE_BITS) != 0)
         {      
           FLASH_ERR_FATAL ("fni: configure - resume failed SR val = 0x%X", \
                         *wptr, 0, 0);
         }
        *wptr = FLASH_READ_ARRAY; 
      }
    }
  }
#elif !defined(FLASH_NOR_PARTI_TBL)
  {
    flash_ptr_type  wptr;
    uint32 value = 0;
    uint32 flash_blk_size = 0;

  #if defined(FLASH_NOR_HD_MODE)
    if ( ((nor_device->dev_ctrl_opts) & FLASHI_DEV_CTRL_OPTS_HD_MODE) ==
          FLASHI_DEV_CTRL_OPTS_HD_MODE )
    {
      self->partition_attributes |= (uint32)FLASH_PARTITION_SLC_READWRITE;

      /* Adjust write buffer size to be half the size in full density mode */
      flash_write_buf_bsize /= 2; 

      /* Retrieve the A-Half size for HD mode */
      flash_intel_copy_cfi_data(probe_addr + INTEL_HD_A_HALF_SEG_SIZE_OFFSET, 
		                &flash_hd_a_half_seg_bsize, 1);
      /* Retrieve the B-Half size for HD mode */
      flash_intel_copy_cfi_data(probe_addr + INTEL_HD_B_HALF_SEG_SIZE_OFFSET, 
		                &flash_hd_b_half_seg_bsize, 1);
      /* Validate that the segment data obtained is non-zero */
      if ((flash_hd_a_half_seg_bsize == 0) ||
          (flash_hd_b_half_seg_bsize == 0))
      {    
        return FLASH_FAILURE;
      }	
      
      /* Update the device specific function pointers */
      nor_device->ops->read_dev = flash_intel_hd_word_read;
      nor_device->ops->read_ptr = flash_intel_hd_read_ptr;
      nor_device->ops->is_erased = flash_intel_hd_is_erased;
      flash_m18_hd_enabled = TRUE;
    }
  #endif

    flash_blk_size = flash_nor_find_size_for_block_addr (&(self->nor_dev_info), 
      (dword)FLASH_NOR_EFS2_START_BYTE_OFFSET);
    
    if (flash_blk_size == 0)
    {
      return FLASH_FAILURE;
    }    	
    
    for( value = (uint32) FLASH_NOR_EFS2_START_BYTE_OFFSET; 
         value < (uint32) FLASH_NOR_EFS2_END_BYTE_OFFSET;
         value = value + flash_blk_size
       )
    {
      wptr = (flash_ptr_type) value;

      /* Check if the flash has been left in any suspend state. 
         If so resume the suspend state and reset the status */
      *wptr = FLASH_CLEAR_STATUS;                

      KICK_DOG_AND_CHECK_DATA();
    
      /* Unlock sector */
      *wptr = FLASH_UNLOCK_SETUP;
      *wptr = FLASH_CONFIRM;
      *wptr = FLASH_READ_ARRAY;
    }
  }         

#else /* #if !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) && 
       * defined(FLASH_NOR_PARTI_TBL) */
  #if defined(FLASH_NOR_HD_MODE)
    #error NOR Half Density mode feature not supported in partition mode 
  #endif
#endif /* #if !defined(BUILD_JFLASH) && !defined(BUILD_ARMPRG) && 
        * defined(FLASH_NOR_PARTI_TBL) */

  return status;
}

/*===========================================================================

FUNCTION flash_intel_hd_word_read

DESCRIPTION
  Read a arbitrary length of bytes from a word interface device.  This
  function is specific to Intel MLC devices operating in half-density mode.

DEPENDENCIES
  None

RETURN VALUE
  FLASH_SUCCESS             - Success.
  FLASH_FAILURE             - An error occured.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL flash_status
flash_intel_hd_word_read (flash_ptr_type read_ptr, byte *buffer, dword count)
{
  dword read_addr = 0; 
#ifdef DO_READ_WITH_MEMMOVE
  dword num_bytes_to_copy = 0;
#else
  volatile byte *bptr;
#endif /* DO_READ_WITH_MEMMOVE */
  
  /* Adjust address since device is operating in half-density mode */
  read_addr = (dword)__flash_intel_convert_to_hd_addr (read_ptr, 
                                                       flash_base_addr);
 
#ifdef DO_READ_WITH_MEMMOVE
  
  num_bytes_to_copy =  ((flash_hd_a_half_seg_bsize - (read_addr % 
    flash_hd_a_half_seg_bsize )) < count ) ? (flash_hd_a_half_seg_bsize - 
    (read_addr % flash_hd_a_half_seg_bsize )): count;

  (void)memmove (buffer, (byte *) read_addr, num_bytes_to_copy);
  count -= num_bytes_to_copy;
  read_addr += (num_bytes_to_copy + flash_hd_b_half_seg_bsize);
  buffer += num_bytes_to_copy;
  while(count > 0)
  {
    num_bytes_to_copy = (count > flash_hd_a_half_seg_bsize ) ? 
      flash_hd_a_half_seg_bsize : count;
    (void)memmove (buffer, (byte *) read_addr, num_bytes_to_copy);
    count -= num_bytes_to_copy;
    read_addr += (flash_hd_a_half_seg_bsize + flash_hd_b_half_seg_bsize);
    buffer += num_bytes_to_copy; 
  }
  
#else /* not DO_READ_WITH_MEMMOVE */

  /* The ARM processor doesn't handle misaligned data accesses, so we
     must account for that here. */

  /* The easy case is if the alignment of the two is the same. */
  if (((dword) buffer & 1) == (read_addr & 1))
  {
    /* Is there a "byte" at the beginning? */
    if ((read_addr & 1) == 1)
    {
      *buffer = *((volatile byte *) read_addr);
      buffer++;
      read_addr++;
      count--;
      if ((read_addr & (flash_hd_a_half_seg_bsize - 1)) == 0)
      {
        read_addr += flash_hd_b_half_seg_bsize;
      }
    }

    /* Copy over any appropriately aligned data. */
    
    while (count > 1)
    {
      *((word *) buffer) = *((word*)read_addr);  /*lint !e826*/
      buffer += 2;
      read_addr   += 2;
      count  -= 2;
      if ((read_addr & (flash_hd_a_half_seg_bsize - 1)) == 0 )
      {
          read_addr += flash_hd_b_half_seg_bsize;
      }
    }

    /* There may be one additional byte to copy. */
    if (count)
    {
      *buffer = *((byte *) read_addr);   
    }
  }
  /* Otherwise, the two sections of data are misaligned.  There are
     two options, either copy the data in byte mode, or perform word
     operations and scramble the bytes around manually.  Because both
     RAM and the component are 16 bits wide, we gain much by doing the
     scrambling. */
  else
  {
    /* Simple byte copy. */
    bptr = (byte *) (read_addr);
    while (count-- > 0)
    {
      *buffer++ = *bptr++;
      if (((dword)bptr & (flash_hd_a_half_seg_bsize - 1)) == 0 )
      {
          bptr += flash_hd_b_half_seg_bsize;
      }
    }
  }

#endif /* not DO_READ_WITH_MEMMOVE */

  return FLASH_SUCCESS;
}
/* END of flash_intel_hd_word_read*/

/***********************************************************************
FUNCTION      flash_intel_hd_read_ptr

DESCRIPTION   This function returns an address pointer to a page. The
              function is specific to Intel MLC devices operating in
              half-density mode. Since pages are not stored in contigious
              memory locations this function reads the page data into
              a buffer and returns the address of the buffer.

DEPENDENCIES  None

RETURN VALUE  Pointer to the beginning of page data in local buffer

SIDE EFFECTS  None
**********************************************************************/
void *
flash_intel_hd_read_ptr (flash_ptr_type page_addr, dword count)
{
  uint8 *data_ptr = 0;
  static byte read_ptr_buf_index = 0; 
  
  /* Initialize the buffer pointer */ 
  data_ptr = flash_hd_read_buf[read_ptr_buf_index++];
  
  /* Copy the page into buffer */
  flash_intel_hd_word_read(page_addr, data_ptr, count);

  /* Wrap buffer page index around if it is the last page in the buffer */  
  read_ptr_buf_index &= (FLASH_HD_MAX_READ_PAGES - 1);

  return (void *)data_ptr;
}
/*END of flash_intel_hd_read_ptr */


/***********************************************************************
FUNCTION      flash_intel_hd_is_erased

DESCRIPTION   This function checks if every byte within a given address
              range is erased(0xFF). The function is specific to Intel
              MLC devices operating in half-density mode.

DEPENDENCIES  None

RETURN VALUE  FLASH_SUCCESS if page is erased
              FLASH_FAILURE if page is not erased, or any error occurs
                            during the check
**********************************************************************/
flash_status
flash_intel_hd_is_erased (flash_ptr_type page_address, dword page_size)
{
  dword *rdptr = NULL;
  dword loop_var = 0;
  dword read_count;
  flash_status status = FLASH_SUCCESS;

  /* Calculate read count by dividing page_size by 4 to compare dwords
   * and the double it to adjust for half-density mode. */ 
  read_count = page_size >> 1; 

  /* Adjust address since device is operating in half-density mode */
  rdptr = (dword *)__flash_intel_convert_to_hd_addr (page_address,
                                                     flash_base_addr);
  
  for (loop_var = 0; loop_var < read_count; loop_var++)
  {
    /* Check if any bit is programmed to 0, if so return error */ 
    if ((~(rdptr[loop_var] & 0xFFFFFFFF)) != 0)
    {
      status = FLASH_FAILURE;
      break;
    }
  }

  return status;
} /* END of flash_intel_hd_is_page_erased */

/***********************************************************************
FUNCTION      __flash_intel_convert_to_hd_addr

DESCRIPTION   This is an internal function used to convert full density
              addresses to half-density mode addresses. The function is
              specific to Intel MLC devices operating in half-density
              mode.

DEPENDENCIES  None

RETURN VALUE  Half-density mode address
**********************************************************************/
LOCAL flash_ptr_type
__flash_intel_convert_to_hd_addr (flash_ptr_type fd_addr, dword flash_dev_base) 
{  
  dword offset_from_flashbase;
  flash_ptr_type hd_addr = 0;
  /*
   *     Algorithm for half-density mode address calculation
   *    -----------------------------------------------------
   *
   * For a device with segment size of 32 bytes determine offset from
   * the flash device base. Calculate half-density offset corresponding
   * to this full-density offset into the device.
   *
   * Half-Density (HD) offset calculation:
   *
   *     HD offset =  ABS(FD offset/16) * 32 + (FD Addr % 16)
   *
   * Add the flash dev base to the offset obtained above get the final 
   * half-density address location in flash. Bit-wise operators are used
   * instead of multiplicative ones for performance optimization.
   */
  offset_from_flashbase = (dword)fd_addr - flash_dev_base; 

  hd_addr = (flash_ptr_type)(((offset_from_flashbase & ~
    ((dword)(flash_hd_a_half_seg_bsize - 1))) << 1) + 
    (offset_from_flashbase & (flash_hd_a_half_seg_bsize - 1)) + 
    flash_dev_base);  
    

  return hd_addr;
} /* END of __flash_intel_convert_to_hd_addr */


/***********************************************************************
FUNCTION      __flash_intel_check_ready_status

DESCRIPTION   This is an internal function used to check if the flash 
              device is ready or not.The function polls for the status 
              ready bit and returns TRUE if the device is ready within 
              the timeout period.

DEPENDENCIES  None

RETURN VALUE  TRUE   - Device Ready
              FALSE  - Device Not Ready/ Timeout occured
**********************************************************************/

LOCAL boolean 
__flash_intel_check_ready_status(volatile word *addr)
{    
  uint32 time_count = 0;
  boolean ready_status = FALSE;

  do
  {
    if(((*addr) & FLASH_READY_BIT) == FLASH_READY_BIT)
    {
      ready_status = TRUE;
      break;
    }
    time_count++;
    clk_busy_wait(FLASH_POLL_TIME);
  } while (time_count < FLASH_OPERATION_BAILOUT_TIME);
  
  return ready_status;
} /* END of __flash_intel_check_ready_status */
