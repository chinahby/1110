/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         N V   I T E M    M A N A G E R    IO   S E R V I C E S

GENERAL DESCRIPTION
  This module provides the NV I/O services using EFS items.
  
EXTERNALIZED FUNCTIONS
  nvio_write
    This is the function which writes NV items into the EFS.
  nvio_read
    This is the function which reads NV items from the EFS.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/nv1/nvio.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/05   pc      Creation of NV2
   

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"

#ifdef FEATURE_NV_ITEM_MGR

#include "fs.h"
#include "nvi.h"
#include "msg.h"
#include "crc.h"
#include "err.h"
#include "memory.h"
#include "dog.h"
#include "nvim.h"
#include "fs_public.h"
#include "nv_items.h"

#ifdef FEATURE_NV_RUIM
#include "nvruimi.h"
#endif

#ifdef FEATURE_NV_SUPPORT_FLASH_OTP
#include "flash_otp.h"
#endif

#ifdef FEATURE_NV_SFS_PRIVATE
#include "sfs_private.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                         DATA DEFINITIONS                                */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

extern nvi_item_type            nvi_item;
extern boolean                 nvi_initialized;

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

#ifdef NV_FEATURE_FACTORY_INFO
  /* Factory data block cache for peek/poke access */
  extern byte fact_data[NVIM_FACTORY_DATA_SIZE];
#endif

/* Dynamic pool available in EFS */
extern word                   nvi_memavail_data;

extern boolean  nv_prl_valid_data[NV_MAX_NAMS];
extern word     nv_prl_version_data[NV_MAX_NAMS];

rex_crit_sect_type nv_crit_sect;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                      REX Timers and Queues                              */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_DIAG_WINS
   static boolean                  write_gate_open = TRUE;
#endif


//Add By zzg 2011_12_19
//NV_SID_NID_LOCK_I
static char  nv_sid_nid_lock_pair[127]=
{	
	0x41, 0x7B, 0xff, 0xff, 0x0B, 0x7B, 0xff, 0xff, 0x15, 0x7B, 0xff, 0xff, 0x2B, 0x7B, 0xff, 0xff,
	0x35, 0x7B, 0xff, 0xff, 0x03, 0x7B, 0xff, 0xff, 0x0d, 0x7B, 0xff, 0xff, 0x19, 0x7B, 0xff, 0xff,
	0x2D, 0x7B, 0xff, 0xff, 0x37, 0x7B, 0xff, 0xff, 0x05, 0x7B, 0xff, 0xff, 0x0F, 0x7B, 0xff, 0xff,
	0x21, 0x7B, 0xff, 0xff, 0x2F, 0x7B, 0xff, 0xff, 0x39, 0x7B, 0xff, 0xff, 0x07, 0x7B, 0xff, 0xff,
	0x11, 0x7B, 0xff, 0xff, 0x25, 0x7B, 0xff, 0xff, 0x31, 0x7B, 0xff, 0xff, 0x3B, 0x7B, 0xff, 0xff,
	0x09, 0x7B, 0xff, 0xff, 0x13, 0x7B, 0xff, 0xff, 0x27, 0x7B, 0xff, 0xff, 0x33, 0x7B, 0xff, 0xff,
	0x3F, 0x7B, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   
};
//Add End

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION NVR_TRANSLATE_TO_EXTERNAL

DESCRIPTION
  This function translates a digit from internal 4 bit NVM storage
  to external 8 bit ASCII representation.

DEPENDENCIES
  None.

RETURN VALUE
  The 8 bit ASCII digit value.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL byte 
nvr_translate_to_external ( 
  byte   digit    /* Internal digit representation */
)
{
  if (digit <= 0x09) return(digit + '0');
  else if (digit == 0x0A) return('*');
  else if (digit == 0x0B) return('#');
  else if (digit == 0x0C) return('-');
  else if (digit == 0x0D) return('P');
  else if (digit == 0x0E) return('T');
  else return('L');

} /* nvr_translate_to_external */

/*===========================================================================

FUNCTION NVIO_READ_ITEM

DESCRIPTION
  This function reads an NV item from NV database.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_read_item (
  nv_items_enum_type    item,
  byte   index,
  void   * data_ptr,
  uint32 item_size
)
{
  int   return_size;
  char  f_name[120];
  nv_stat_enum_type status;
  struct fs_stat temp_buf;

  /* Create the name of the item file */
  if(index == 0)
    (void)snprintf(f_name,sizeof(f_name),"/nvm/num/%d",item);
  else
    (void)snprintf(f_name,sizeof(f_name),"/nvm/num/%d_%d",item,index); 
  /* Check if the file is present or not. */
  if(efs_stat(f_name,&temp_buf) == -1) {
    return NV_NOTACTIVE_S;
  }

  if((temp_buf.st_size) != (nvim_op_get_size(item)))
  {
    (void)efs_unlink(f_name);
    return NV_NOTACTIVE_S;
  }
    
  /* Obtain the contents from EFS */
  return_size = efs_get(f_name, data_ptr, item_size);

  if (return_size == -1)
    status = NV_FAIL_S;
  else
    status = NV_DONE_S;

  return status;
} /* End of nvio_read_item*/


/*===========================================================================

FUNCTION NVIO_READ_DIAL

DESCRIPTION
  This procedure processes read requests for speed dial numbers.  It reads
  the NV item and it performs necessary translations and copy from internal 
  format to external format.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the read request.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_dial (
  nv_items_enum_type    item,         /* Item to access */
  byte                  index_range,  /* Maximum allowed range for index */
  byte                  index,        /* speed dial # */
  nv_dial_type          *dial_ptr     /* Pointer to write source buffer */
)
{
  word   i;                           /* Digit index */
  word ndigits;                       /* # of digits being processed */
  nv_stat_enum_type status; /* Status to return to calling procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the number from NVM into internal buffer. */

  if (index >= index_range) {
    return NV_BADPARM_S;
  }

  status = nvio_read_item(
                           item,
                           index,
#ifdef NV_FEATURE_EXTENDED_UI
                           (byte*)&nvi_item.dial.status,    
#else
                           (byte*)&nvi_item.dial.num_digits,
#endif
                           sizeof(nvi_item.dial));
  if(status == NV_DONE_S)
  {
    /* Copy the number of dial digits stored, making  */
    /* sure that if there was an NV error only the    */
    /* maximum number of dialed digits will be read.  */

    dial_ptr->num_digits = MIN(nvi_item.dial.num_digits, NV_MAX_DIAL_DIGITS);

    /* Copy the digits, expanding from two digits per  */
    /* byte into one digits per byte, and translating  */
    /* the digit to external character representation. */

    ndigits = dial_ptr->num_digits;
    for (i = 0; i < ndigits; i++) {

      /* Even digits are copied from least significant digit,  */
      /* odd digits are copied from most significant digit.    */

      if(i%2 == 0){
        dial_ptr->digits[i] = 
          nvr_translate_to_external(
             (byte)(nvi_item.dial.digits[i/2] & 0x0F));
      }
      else{
        dial_ptr->digits[i] = 
          nvr_translate_to_external(
             (byte)((nvi_item.dial.digits[i/2] & 0xF0) >> 4));
      }
    } /* for */

    /* Move name into external type. */
    (void)memcpy((void *) dial_ptr->letters, (void *) nvi_item.dial.letters,
                  NV_MAX_LTRS);
  }

#ifdef NV_FEATURE_EXTENDED_UI
      dial_ptr->status = nvi_item.dial.status;
#endif
  return status;

} /* nvio_read_dial */

/*===========================================================================

FUNCTION NVIO_READ_STDIAL

DESCRIPTION
  This procedure processes read requests for speed dial numbers.  It reads
  the NV item and it performs necessary translations and copy from internal 
  format to external format.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the read request.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_stdial (
  nv_items_enum_type    item,        /* Item to access */
  byte                  index_range, /* Maximum allowed range for index */
  byte                  index,       /* speed dial # */
  nv_stdial_type        *dial_ptr    /* Pointer to write source buffer */
)
{
  word   i;                 /* Digit index */
  nv_stat_enum_type status; /* Status to return to calling procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the number from NV into internal buffer. */

  if (index >= index_range) return NV_BADPARM_S;

  status = nvio_read_item(
                          item,
                          index,
#ifdef NV_FEATURE_EXTENDED_UI
                          (byte*)&nvi_item.stack.status,     
#else
                          (byte*)&nvi_item.stack.num_digits, 
#endif
                          sizeof(nvi_item.stack));

  if(status == NV_DONE_S)
  {
    /* Copy the number of dial digits stored, making  */
    /* sure that if there was an NVM error only the   */
    /* maximum number of dialed digits will be read.  */

    dial_ptr->num_digits = MIN(nvi_item.stack.num_digits, NV_MAX_DIAL_DIGITS);

    /* Copy the digits, expanding from two digits per  */
    /* byte into one digits per byte, and translating  */
    /* the digit to external character representation. */

    for (i = 0; i < dial_ptr->num_digits; i++) {

      /* Even digits are copied from least significant digit,  */
      /* odd digits are copied from most significant digit.    */

      if(i%2 == 0)
        dial_ptr->digits[i] = nvr_translate_to_external((byte)(nvi_item.stack.digits[i/2] & 0x0F));
      else
        dial_ptr->digits[i] = nvr_translate_to_external((byte)((nvi_item.stack.digits[i/2] & 0xF0) >> 4));
    } /* for */

    /* Move name into external type. */
    (void)memcpy((void *) dial_ptr->letters, (void *) nvi_item.stack.letters,
                  NV_MAX_LTRS);
  }

#ifdef NV_FEATURE_EXTENDED_UI
      dial_ptr->status = nvi_item.stack.status;
      dial_ptr->time_stamp = nvi_item.stack.time_stamp;
      dial_ptr->duration = nvi_item.stack.duration;
#endif
  return status;

} /* nvio_read_stdial */


/*===========================================================================

FUNCTION NVIO_READ_SMS

DESCRIPTION
  This function reads an sms depending upon the address (index) of sms 
  passed into it via the cmd_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_sms (
  nv_cmd_type  *cmd_ptr          /* Command block */
)
{
  int return_size;
  char sms_name[20];
  nv_stat_enum_type   status = NV_DONE_S;
  struct fs_stat temp_buf;

  /* only sms supported at this time */
  if(((cmd_ptr->item != NV_SMS_I) && (cmd_ptr->item != NV_SMS_DM_I))
     || (cmd_ptr->data_ptr->sms.address >= NVI_MAX_SMS_ADDR)) {
    status = NV_BADPARM_S;
  }

  else{
    /* Reset the watchdog timer */
    KICK_WATCHDOG();

    /* Create the name of the file to be read */
    (void) snprintf(sms_name, sizeof(sms_name), "/nvm/sms_%05d",
                                      cmd_ptr->data_ptr->sms.address);

    /* Check if the file present or not. */
    if(efs_stat(sms_name,&temp_buf) == -1) {
      status = NV_NOTACTIVE_S;
    }
    else {

      if(((cmd_ptr->item == NV_SMS_I)&&
            (temp_buf.st_size > (NV_SMS_DATA_SIZ + sizeof(nvi_sms_type))))
         ||((cmd_ptr->item == NV_SMS_DM_I)&&
            (temp_buf.st_size > (NV_SMS_DATA_SIZ + sizeof(nvi_sms_type))))){
        MSG_HIGH ("SMS Buffer not big enough for slot %d",
                        cmd_ptr->data_ptr->sms.address, 0, 0);
        status = NV_FAIL_S;
      }
      else {

        /* Read the contents from EFS */
        return_size = efs_get( sms_name,
                               (byte *)&(cmd_ptr->data_ptr->sms.address),
                               (int)temp_buf.st_size);
        if(return_size == -1) {
          status = NV_FAIL_S;
        }
      }
    }
  }
  return status;
} /* nvio_read_sms */


#ifdef FEATURE_GWSMS
#error code not present
#endif


/*===========================================================================

FUNCTION NVIO_READ_PRL_ITEM

DESCRIPTION
  This function reads a PRL item from NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_read_prl_item (
  int num,
  void * data_ptr,
  int item_size
)
{
  nv_stat_enum_type  status;      /* Function return value */  
  int                fd;
  char               f_name[120];
  struct fs_stat     temp_buf;
  char               *read_buf;
  fs_ssize_t         result = 0;
  fs_size_t          nbytes;
  fs_size_t          bytes_read;

  MSG_MED("size: %d", item_size, 0, 0);

  snprintf(f_name,sizeof(f_name),"/nvm/prl_%d",num);

    /* Check if the file is present or not. */
  if(efs_stat(f_name,&temp_buf) == -1) {
    return NV_NOTACTIVE_S;
  }

  fd = efs_open(f_name, O_RDWR, 0777 );

  nbytes = item_size;
  read_buf = data_ptr;

  efs_read (fd,
            (void *) (read_buf),
            sizeof(boolean));

  efs_lseek(fd,sizeof(boolean),SEEK_CUR);

  bytes_read = 1;

  while (bytes_read < nbytes)
  {
    result = efs_read (fd,
                       (void *) (read_buf + bytes_read),
                       nbytes - bytes_read);
    if (result <= 0)
    break;
    bytes_read += (fs_size_t) result;
  }

  efs_close(fd);

  if (result == -1) {
    status = NV_FAIL_S;
  }
  else status = NV_DONE_S;

  return status;
}


/*===========================================================================

FUNCTION NVIO_READ_RENTAL_TIMER

DESCRIPTION
  This function returns the current value of the rental timer item.  Since
  this item is expected to be updated with very high frequency, it is stored 
  in a way that minimizes EEPROM cell wear.  This unusual storage format 
  calls for a special routine to compute and return the value for reads.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S       if it worked
  NV_NOTACTIVE_S  if the item is uninitialized

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif


/*===========================================================================

FUNCTION NVIO_READ_RENTAL_CNT

DESCRIPTION
  This function returns the current value of the rental count item.  Since
  this item is a fixed-pool double-buffered item, it needs a special read
  routine.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S       if it worked
  NV_NOTACTIVE_S  if the item is uninitialized

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif


#ifdef NV_FEATURE_PRL_ITEMS
/*===========================================================================

FUNCTION NVIO_READ_ROAMING_LIST

DESCRIPTION
  This function reads the roaming list. 

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_read_roaming_list (
  nv_cmd_type  *cmd_ptr          
)
{
  word               item_size;   /* Size of item (in bytes) */
  nv_stat_enum_type  status;      /* Function return value */  
  int                fd;
  char               f_name[120];
  struct fs_stat     temp_buf;
  char               *read_buf;
  fs_ssize_t         result = 0;
  fs_size_t          nbytes;
  fs_size_t          bytes_read;
  boolean            nam_flag;

  /* Check for illegal NAM */
  if(cmd_ptr->data_ptr->roaming_list.nam >= NV_MAX_NAMS) {
    return NV_BADPARM_S;
  }
        
  /* Get its size */
  item_size = NV_ROAMING_LIST_HEADER_SIZE +
                 nv_max_size_of_roaming_list();


  MSG_MED("size: %d", item_size, 0, 0);
 
  snprintf(f_name,sizeof(f_name),"/nvm/prl_%d",cmd_ptr->data_ptr->roaming_list.nam);
  
    /* Check if the file is present or not. */
  if(efs_stat(f_name,&temp_buf) == -1) {
    return NV_NOTACTIVE_S;
  }

  fd = efs_open(f_name, O_RDWR, 0777 );

  nbytes = item_size;
  bytes_read = 0;
  read_buf = (void *) &(cmd_ptr->data_ptr->roaming_list.prl_version);

  efs_lseek(fd,
            sizeof(nam_flag) + sizeof(cmd_ptr->data_ptr->roaming_list.nam),
            SEEK_CUR);

  while (bytes_read < nbytes)
  {
    result = efs_read (fd,
                       (void *) (read_buf + bytes_read),
                       nbytes - bytes_read);
    if (result <= 0)
      break;
    bytes_read += (fs_size_t) result;
  }

  efs_close(fd);

  if (result == -1) {
    status = NV_FAIL_S;
  }
  else status = NV_DONE_S;

  return status;
} /* nvio_read_roaming_list */
#endif



/*===========================================================================

FUNCTION NVIO_READ

DESCRIPTION
  This procedure processes NVM read requests.  It checks to make sure
  that a valid item has been requested, it reads the NVM item using EFS
  services and it performs the translation from internal NVM format to
  external format. If the item being read requires no special processing,
  it is passed on to nvimio_io().

DEPENDENCIES
  None.

RETURN VALUE
  Status of read operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvio_read (
  nv_cmd_type  *cmd_ptr           /* Pointer to Command block */
)
{
  nv_stat_enum_type status;  /* Status to return to calling procedure */
  dword   local_esn_chksum;    /* buffer for esn chksum */
  dword   local_esn;
  uint16  array_size;
  byte index; /* Specific index of an array */
#ifdef FEATURE_NV_SFS_PRIVATE
  sfs_priv_errno_enum_type sfs_ret = 0;
#endif

#ifdef FEATURE_NV_SUPPORT_FLASH_OTP
  FlashOTP_ReturnType otp_ret_val;
  FlashOTP_ProtectOp_ReturnType otp_prot_ret_val;
  byte  local_imei[NV_UE_IMEI_SIZE];
  int i;
#endif
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If item code is out of range return with bad parameter status. */

  if (cmd_ptr->item >= NV_MAX_I) {
    return NV_BADPARM_S;
  }
  else {                 

#ifdef FEATURE_NV_RUIM

    /* If the R-UIM supports the item,
     * return the status else use "regular" NVM.
     */
    if ( nvruim_read(cmd_ptr, &status) == NV_RUIM_SUPPORTS_ITEM )
      return status;
#endif

    switch (cmd_ptr->item) {

    /* read up data item, do special processing if required */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* This item is a special case for writes, so it has no generic path in
       * nvimio_io(). Thus we have to handle the read of this item specially. 
       */
      case NV_ESN_I:

        status = nvio_read_item(NV_ESN_I,/* file handle */
                                0,
                                (byte *)&cmd_ptr->data_ptr->esn, /* data ptr */
                                sizeof(nvi_item.esn)); /* data count */
        break; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Since NV_ESN_ME is a shadow variable for ESN in EFS, return the value 
         of ESN in EFS.
       */

      case NV_ESN_ME_I:

        status = nvio_read_item(NV_ESN_I,/* file handle */
                                0,
                                (byte *)&cmd_ptr->data_ptr->esn_me, /* data ptr */
                                sizeof(nvi_item.esn)); /* data count */

        break; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ESN checksum read returns the status of */
     /* the checksum, NOT the checksum itself.  */

      case NV_ESN_CHKSUM_I:

        /* First read the ESN itself into a local buffer. */
        status = nvio_read_item(NV_ESN_I,/* file handle */
                                0,
                                (byte*)&local_esn,          /* data ptr */
                                sizeof(local_esn)); /* data count */

        if(status != NV_DONE_S)
          break;

        /* Next read the ESN checksum into a local buffer. */
        status = nvio_read_item(NV_ESN_CHKSUM_I,   /* file handle */
                                0,
                                &local_esn_chksum,   /* data ptr */
                                sizeof(local_esn_chksum)); /* data count */

        if(status != NV_DONE_S) 
          break;
        /* Next compute the ESN checksum and compare against  */
        /* the stored checksum.  Return its status to caller. */

        if (crc_30_calc((byte *)&local_esn,
                        sizeof(local_esn) * 8) == local_esn_chksum) {
          cmd_ptr->data_ptr->esn_chksum.chksum = NV_VALID_ESN_CHKSUM;
        } 
        else {
          cmd_ptr->data_ptr->esn_chksum.chksum = ~NV_VALID_ESN_CHKSUM;
        }
        status = NV_DONE_S;
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Since NV_MEID_ME is a shadow variable for MEID in EFS, return the value 
         of MEID in EFS.
       */

      case NV_MEID_ME_I:

        status = nvio_read_item(NV_MEID_I,/* file handle */
                                0,
                                (byte *)cmd_ptr->data_ptr, /* data ptr */
                                nvim_op_get_size(cmd_ptr->item)); /* data count */
        break; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_NV_SUPPORT_FLASH_OTP

      case NV_UE_IMEI_I:

        /* Check whether OTP area is programmed */
        otp_prot_ret_val = flash_otp_status();

        /* If the protection is not active, then return NV data */
        if ((otp_prot_ret_val == FLASH_OTP_PROTECTION_INACTIVE) ||
            (otp_prot_ret_val == FLASH_OTP_NOT_SUPPORTED)) {
          /* Read from NV */
          status = nvio_read_item(cmd_ptr->item,
                                  0,
                                  (byte *)cmd_ptr->data_ptr,
                                  nvim_op_get_size(cmd_ptr->item));
          return status;
        }
        else {
          /* Read from OTP Area */
          otp_ret_val = flash_otp_operation(FLASH_OTP_OP_IMEI_READ,
                                          (uint8 *)local_imei);

          if (otp_ret_val != FLASH_OTP_IO_SUCCESS) {
             MSG_HIGH ("OTP Read failure",0,0,0);
             status = NV_FAIL_S;
             return status;
          }
          /* Read from NV */
          status = nvio_read_item(cmd_ptr->item,
                                  0,
                                  (byte *)cmd_ptr->data_ptr,
                                  nvim_op_get_size(cmd_ptr->item));

          if ( (status != NV_DONE_S) && (status != NV_NOTACTIVE_S) ) {
            MSG_HIGH ("NV Read failure for IMEI",0,0,0);
            return status;
          }

          /* If the IMEI got wiped out, then set it from OTP area */
          if ( status == NV_NOTACTIVE_S )
          {
            for (i=0; i<NV_UE_IMEI_SIZE; i++)
              cmd_ptr->data_ptr->ue_imei.ue_imei[i] = local_imei[i];

            status = nvio_write(cmd_ptr);

            /* Turn back on permanent file attribute */
            if (status == NV_DONE_S) {
              MSG_HIGH ("IMEI successfully propagated from OTP to NV.",0,0,0);
              return status;
            }

            MSG_HIGH ("IMEI could not propagate from OTP to NV.",0,0,0);
            return (NV_FAIL_S);
          }

          /* Compare the contents of OTP and NV */
          if (memcmp(local_imei, (byte *)cmd_ptr->data_ptr->ue_imei.ue_imei,
                     sizeof(local_imei))==0)
          {
            /* Do nothing */
            MSG_HIGH ("OTP and NV contents are same.",0,0,0);
            return status;
          }
          else
          {
            /* Return Failure */
            MSG_HIGH ("IMEI does not match between OTP and NV",0,0,0);
            return (NV_FAIL_S);
          }
        }
        break;

#elif defined (FEATURE_NV_SFS_PRIVATE)

      case NV_UE_IMEI_I:
        sfs_ret = sfs_priv_imei_read((uint8 *)cmd_ptr->data_ptr, 
                                            nvim_op_get_size(cmd_ptr->item));
        if(sfs_ret == SFS_PRIV_INACTIVE) 
        {
          return NV_NOTACTIVE_S;
        }
        else if(sfs_ret == SFS_PRIV_SUCCESS) 
        {
          return NV_DONE_S;
        }
        else
        {
          return NV_FAIL_S;
        }

#else
      case NV_UE_IMEI_I:
        status =  nvio_read_item(cmd_ptr->item,
                                 0,
                                 (byte *)cmd_ptr->data_ptr,
                                 nvim_op_get_size(cmd_ptr->item));
#endif
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* This item is a special case for writes, so it has no generic path in
       * nvimio_io(). Thus we have to handle the read of this item here 
       * rather than in nvimio_io().
       */
      case NV_VERNO_MAJ_I:
        status = nvio_read_item(cmd_ptr->item, /* item ID */
                              0,
                              (byte*)&cmd_ptr->data_ptr->verno_maj, /* data ptr */
                              sizeof(nvi_item.verno_maj)); /* data count */
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* This item is a special case for writes, so it has no generic path in
       * nvimio_io(). Thus we have to handle the read of this item here 
       * rather than in nvimio_io().
       */
      case NV_VERNO_MIN_I:
        status = nvio_read_item(NV_VERNO_MIN_I,  /* item ID */
                              0,
                              (byte*)&cmd_ptr->data_ptr->verno_min, /* data ptr */
                              sizeof(nvi_item.verno_min));   /* data count */
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Some invariant offset items are handled as a special case,
         because they will always have a valid entry, but if their 
         feature is not turned on, then you can't read, write or build
         them */
      case NV_PAP_PASSWORD_I:
        #ifdef NV_FEATURE_PAP
          status = nvio_read_item(cmd_ptr->item,
                                  0,
                                  (byte *)cmd_ptr->data_ptr,
                                  nvim_op_get_size(cmd_ptr->item));
        #else
          status = NV_BADPARM_S;
        #endif
        break;        
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_UP_KEY_I:
        #ifdef NV_FEATURE_UP
          index = *((byte *)cmd_ptr->data_ptr);
          if (index >= NV_UP_LINK_INFO_TABLE_SIZE)
            status = NV_BADPARM_S;
          else
            status = nvio_read_item(cmd_ptr->item,
                                    index,
                                    ((byte *)cmd_ptr->data_ptr)+sizeof(index),
                                    nvim_op_get_size(cmd_ptr->item));
        #else
          status = NV_BADPARM_S;
        #endif
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_BD_ADDR_I:
        #if (defined(FEATURE_BT) || defined(FEATURE_BT_QSOC))
          status = nvio_read_item(cmd_ptr->item,
                                  0,
                                  (byte *)cmd_ptr->data_ptr,
                                  nvim_op_get_size(cmd_ptr->item));
        #else
          status = NV_BADPARM_S;
        #endif
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DIAL_I:
        status = nvio_read_dial(NV_DIAL_I,
                                NV_MAX_SPEED_DIALS,
                                cmd_ptr->data_ptr->dial.address,
                                &cmd_ptr->data_ptr->dial);
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_STACK_I:
        status = nvio_read_stdial(NV_STACK_I,
                                  NV_MAX_STACK_DIALS,
                                  cmd_ptr->data_ptr->stack.address,
                                  &cmd_ptr->data_ptr->stack);
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_REDIAL
      case NV_REDIAL_I:
        status = nvio_read_dial(NV_DIAL_I,
                                NV_MAX_SPEED_DIALS + NVI_REDIAL_EXTENSIONS,
                                NV_MAX_SPEED_DIALS,
                                &cmd_ptr->data_ptr->dial);
        break;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SMS_I:
      case NV_SMS_DM_I:
        status = nvio_read_sms(cmd_ptr);
        break;
/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_FACTORY_INFO
      case NV_FACTORY_INFO_I:  
        /* Copy from the static block, latest factory data */
        memcpy((void *) cmd_ptr->data_ptr->fact_info,
               fact_data, 
               (word) NV_FACTORY_INFO_SIZ);
        status = NV_DONE_S;
        break;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_PRL_ITEMS
  #ifdef NV_FEATURE_IS683A_PRL
      case NV_ROAMING_LIST_683_I:
  #else
      case NV_ROAMING_LIST_I:
  #endif
      status = nvio_read_roaming_list(cmd_ptr);
      break;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_NV_SUPPORT_FLASH_OTP
      case NV_DEVICE_SERIAL_NO_I:
         if (flash_otp_operation(FLASH_OTP_OP_SERIALNO_READ,
                       (byte *)cmd_ptr->data_ptr) != FLASH_OTP_IO_SUCCESS) {
           return NV_FAIL_S;
         }
         else {
           return NV_DONE_S;
         }
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_GWSMS
#error code not present
#endif

      default:

        /* Check if there's an actual entry */
        if (nvim_op_get_presence(cmd_ptr->item)) {
          array_size = nvim_op_get_array_size(cmd_ptr->item);
          if (array_size == 0)  /* If not an array, 1 means an array of 1 */
            status = nvio_read_item(cmd_ptr->item,
                                  0,
                                  (byte *)cmd_ptr->data_ptr, 
                                  nvim_op_get_size(cmd_ptr->item));
          else /* More than one item */ {
            index = *((byte *) cmd_ptr->data_ptr);
            if (index >= array_size) {
              status = NV_BADPARM_S;
            }
            else {
              status = nvio_read_item(cmd_ptr->item,
                                    index,
                                    ((byte *)cmd_ptr->data_ptr)+sizeof(index), 
                                    nvim_op_get_size(cmd_ptr->item));
            }
          }
        }
        else  /* Not a valid entry */ {
          MSG_HIGH("nvio_read item %d not for this target", cmd_ptr->item, 0, 0);
          status = NV_BADPARM_S;
        }
    /* End read switch and return status. */
    } /* switch */
    return status;
  } /* else */
} /* nvio_read */


/*===========================================================================

FUNCTION NVIO_TRANSLATE_TO_INTERNAL

DESCRIPTION
  This function translates a digit frome external representation to
  internal 4 bit NVM storage representation.

DEPENDENCIES
  None.

RETURN VALUE
  The 4 bit digit value.

SIDE EFFECTS
  None.

===========================================================================*/

static byte 
nvio_translate_to_internal ( 
  byte   digit    /* External digit representation */
)
{
  if ((digit >= '0') && (digit <= '9')) return(digit - '0');
  else if (digit == '*' ) return(0x0A);
  else if (digit == '#' ) return(0x0B);
  else if (digit == '-' ) return(0x0C);
  else if (digit == 'P' ) return(0x0D);
  else if (digit == 'T' ) return(0x0E);
  else if (digit == 'L' ) return(0x0F);
  else {
    ERR("Illegal dial digit",0,0,0);
    return(0x0C);
  }

} /* nvio_translate_to_internal */


/********************** TEMPORARY ******************************************/
/************* Remove this function after cross-update is removed **********/
/*===========================================================================

FUNCTION NVT_TRANSLATE_TO_EXTERNAL

DESCRIPTION
  This function translates a digit frome internal 4 bit NVM storage
  to external 8 bit ASCII representation.

DEPENDENCIES
  None.

RETURN VALUE
  The 8 bit ASCII digit value.

SIDE EFFECTS
  None.

===========================================================================*/

static byte nvt_translate_to_external
( 
  byte   digit    /* Internal digit representation */
)
{
  if ((digit > 0) && (digit <= 0x09)) return(digit + '0');
  else if (digit == 0x0A) return('0');
  else if (digit == 0x0B) return('*');
  else if (digit == 0x0C) return('#');
  else return(' ');

} /* nvt_translate_to_external */
/********************** TEMPORARY ******************************************/


/*===========================================================================

FUNCTION NVT_TRANSLATE_TO_INTERNAL

DESCRIPTION
  This function translates a digit frome external representation to
  internal 4 bit NVM storage representation.

DEPENDENCIES
  None.

RETURN VALUE
  The 4 bit digit value.

SIDE EFFECTS
  None.

===========================================================================*/

static byte 
nvt_translate_to_internal ( 
  byte   digit    /* External digit representation */
)
{
  if ((digit > '0') && (digit <= '9')) return(digit - '0');
  else if (digit == '0') return(0x0A);
  else if (digit == '*') return(0x0B);
  else if (digit == '#') return(0x0C);
  else return(0x0F);

} /* nvt_translate_to_internal */

/*===========================================================================

FUNCTION NVIO_WRITE_ITEM

DESCRIPTION
  This function writes an NV item to NV database.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  Or the failure status from the lower levels.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_write_item ( 
  nv_items_enum_type    item,
  byte   index,
  void   * data_ptr,
  uint32 item_size
) 
{
  int               return_size;
  boolean           item_present;
  char              f_name[120];
  nv_stat_enum_type status;
  struct fs_stat    temp_buf;


  item_present = nvim_op_get_presence(item);

  if(item_present) {
    /* Creating file name for array item*/
    if(index == 0)
      (void)snprintf(f_name,sizeof(f_name),"/nvm/num/%d",item);  
    else
      (void)snprintf(f_name,sizeof(f_name),"/nvm/num/%d_%d",item,index);

    /* Check if the file is present or not. */
    if(efs_stat(f_name,&temp_buf) != -1) {
      if((temp_buf.st_size) != (nvim_op_get_size(item))) {
        (void)efs_unlink(f_name);
      }
    }

    return_size = efs_put(f_name, data_ptr, item_size, 
                          O_CREAT|O_AUTODIR, 0777);
    if(return_size >= 0)
      status = NV_DONE_S;
    else
      status = NV_FAIL_S;
  }
  else {
    ERR("nvio_write %d not for this target", (int)item, 0, 0);
    status = NV_BADPARM_S;  
  }
  return status;
} /* End of nvio_write_item*/

    
/*===========================================================================

FUNCTION NVIO_WRITE_ESN

DESCRIPTION
  This function processed a write command to the NV_ESN_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_esn(
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  nv_stat_enum_type status;   /* Function return value */
  dword local_esn;            /* tmp buffer for esn */
  dword local_esn_chksum;     /**/                                  
  /* Read the current ESN value */
  status = nvio_read_item(NV_ESN_I,/* file handle */
                         0,
                         &local_esn,          /* data ptr */
                         sizeof(local_esn)); /* data count */
        
  if (status == NV_FAIL_S){
    return status;
  }
        
#if 0        
  /* The ESN may only be written once */
  if ((local_esn != 0) &&
      (local_esn != 0xFFFFFFFF) && (status != NV_NOTACTIVE_S)){
    /* The file is now considered read-only */
    return NV_READONLY_S;
  }
#endif

  /* Write the ESN, the ESN checksum, and the ESN valid flag. */
  status = nvio_write_item(NV_ESN_I,  /* Item name */
                          0, /* index */
                          (byte*)&cmd_ptr->data_ptr->esn, /* data ptr */
                          sizeof(nvi_item.esn));  /* data count */

  if (status == NV_DONE_S)
  {
    local_esn_chksum = crc_30_calc((byte *)&cmd_ptr->data_ptr->esn,
                                      sizeof(nvi_item.esn) * 8);

    status = nvio_write_item(NV_ESN_CHKSUM_I,  /* file handle */
                            0, /* file position */
                            (byte*)&local_esn_chksum, /* data ptr */
                            sizeof(local_esn_chksum));  /* data count */

  }
        
  return status;
}


/*===========================================================================

FUNCTION NVIO_WRITE_ROAMING_LIST

DESCRIPTION
  This function writes the roaming list. Handled as a special case in the 
  item manager.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_roaming_list (
  nv_cmd_type  *cmd_ptr          
)
{
  word               item_size;   /* Size of item (in bytes) */
  nv_stat_enum_type  status;      /* Function return value */  
  int                fd;
  char               f_name[120];
  fs_ssize_t         result = 0;
  fs_size_t          nbytes;
  fs_size_t          bytes_written;
  char               *write_buf;
  boolean            true_flag   = TRUE;
  
  /* Check for illegal NAM */
  if(cmd_ptr->data_ptr->roaming_list.nam >= NV_MAX_NAMS)
  {
    return NV_BADPARM_S;
  }


  /* Get its size */
  item_size = NV_ROAMING_LIST_HEADER_SIZE +
                 nv_max_size_of_roaming_list();

  MSG_MED("size: %d", item_size, 0, 0);

  snprintf(f_name,sizeof(f_name),"/nvm/prl_%d",cmd_ptr->data_ptr->roaming_list.nam);

  fd = efs_open(f_name, O_RDWR | O_CREAT , 0777 );

  nbytes = item_size;
  bytes_written = 0;

  /*
  memcpy(((byte*)write_buf), &true_flag, sizeof(true_flag));
  memcpy(((byte*)write_buf)+sizeof(true_flag), 
         (byte*)&(cmd_ptr->data_ptr->roaming_list.prl_version), 
         item_size-sizeof(true_flag));
*/

  write_buf = (void *)&(cmd_ptr->data_ptr->roaming_list.prl_version);

  efs_write (fd,
             (const void *) &(true_flag),
             sizeof(true_flag));

  efs_lseek(fd, sizeof(true_flag), SEEK_CUR);

  while (bytes_written < nbytes) {
    result = efs_write (fd,
                        (const void *) (write_buf + bytes_written),
                        nbytes - bytes_written);
    if (result <= 0)
      break;
    bytes_written += (fs_size_t) result;
  }
  
  if (result == -1) {
    status = NV_FAIL_S;
  }
  else
    status = NV_DONE_S;

  nv_prl_version_data[cmd_ptr->data_ptr->roaming_list.nam] = 
          cmd_ptr->data_ptr->roaming_list.prl_version;
  nv_prl_valid_data[cmd_ptr->data_ptr->roaming_list.nam] = 
          cmd_ptr->data_ptr->roaming_list.valid;

  efs_close(fd);
  return status;
}  /* nvio_write_roaming_list */



/*===========================================================================

FUNCTION NVIO_WRITE_DIAL

DESCRIPTION
  This function processes write requests for speed dial number.  It performs
  translations necessary from external to internal format and then attempts
  to write the NVM item using EFS services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_dial (
  nv_items_enum_type    item,             /* Item to access */
  byte                  index_range,      /* Maximum allowed range for index */
  byte                  index,            /* speed dial # */
  nv_dial_type          *dial_ptr         /* Pointer to write source buffer */
)
{
  word               i;                   /* Index through speed dial numbers */
  byte               ndigits;             /* # of digits to process */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for illegal offset into the array */
  if (index >= index_range) return NV_BADPARM_S;

  /* First copy the digits count from external buffer to internal */
  /* buffer, making sure that only the maximum number allowed can */
  /* be stored.  When copying translate digits to internal type.  */

  nvi_item.dial.num_digits = MIN(dial_ptr->num_digits, NV_MAX_DIAL_DIGITS);
  ndigits = nvi_item.dial.num_digits;
  for (i = 0; i < ndigits; i++) {

    /* Copy the digits, compressing from one digit per byte into two */
    /* digits per byte.  Even bytes are stored in least significant  */
    /* digit, odd bytes in most significant digit.                   */

    if ((i%2) == 0) {
      nvi_item.dial.digits[i/2] = 
         nvio_translate_to_internal(dial_ptr->digits[i]);
    } else {
      nvi_item.dial.digits[i/2] |= 
         (byte)(nvio_translate_to_internal(dial_ptr->digits[i]) << 4);
    }
  } /* for */

  /* Copy the name to internal type. */
    (void)memcpy((void *) nvi_item.dial.letters, (void *) dial_ptr->letters,
                  NV_MAX_LTRS);

#ifdef NV_FEATURE_EXTENDED_UI
  nvi_item.dial.status = dial_ptr->status;
#endif

  /* Write out item. */
  return  nvio_write_item(
                                  item,
                                  index,
#ifdef NV_FEATURE_EXTENDED_UI
                                  (byte*)&nvi_item.dial.status,     
#else
                                  (byte*)&nvi_item.dial.num_digits, 
#endif
                                  sizeof(nvi_item.dial));

} /* nvio_write_dial */


/*===========================================================================

FUNCTION NVIO_WRITE_STDIAL

DESCRIPTION
  This function processes write requests for stack dial number.  It performs
  translations necessary from external to internal format and then attempts
  to write the NV item using EFS services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_stdial (
  nv_items_enum_type    item,        /* Item to access */
  byte                  index_range, /* Maximum allowed range for index */
  byte                  index,       /* speed dial # */
  nv_stdial_type *dial_ptr  /* Pointer to external write source buffer */
)
{
  word                  i;           /* Index through stack dial numbers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the index is within the range */
  if (index >= index_range) return NV_BADPARM_S;
  
  /* First copy the digits count from external buffer to internal */
  /* buffer, making sure that only the maximum number allowed can */
  /* be stored.  When copying translate digits to internal type.  */

  nvi_item.stack.num_digits = MIN(dial_ptr->num_digits, NV_MAX_DIAL_DIGITS);
  for (i = 0; i < nvi_item.stack.num_digits; i++) {

    /* Copy the digits, compressing from one digit per byte into two */
    /* digits per byte.  Even bytes are stored in least significant  */
    /* digit, odd bytes in most significant digit.                   */

    if ((i%2) == 0) {
      nvi_item.stack.digits[i/2] = nvio_translate_to_internal(dial_ptr->digits[i]);
    } else {
      nvi_item.stack.digits[i/2] |= (byte)(nvio_translate_to_internal(dial_ptr->digits[i]) << 4);
    }
  } /* for */

  /* Copy the name to internal type. */
  (void)memcpy((void *) nvi_item.stack.letters, (void *) dial_ptr->letters,
                NV_MAX_LTRS);

#ifdef NV_FEATURE_EXTENDED_UI
  nvi_item.stack.status = dial_ptr->status;
  nvi_item.stack.time_stamp = dial_ptr->time_stamp;
  nvi_item.stack.duration = dial_ptr->duration;
#endif

  /* Write out item. */
  return nvio_write_item(
                                 item,
                                 index,
#ifdef NV_FEATURE_EXTENDED_UI
                                 (byte*)&nvi_item.stack.status,
#else
                                 (byte*)&nvi_item.stack.num_digits,
#endif
                                 sizeof(nvi_item.stack));

} /* nvio_write_stdial */


/*===========================================================================

FUNCTION NVIO_WRITE_SMS

DESCRIPTION
  This function writes to an sms EFS file.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  NV_BADPARM_S if requested bytes are out of range for NVM

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_sms (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  int                 return_size;
  char                sms_name[20];
  nv_stat_enum_type   status = NV_DONE_S;

  /* only sms supported at this time */
  if(((cmd_ptr->item != NV_SMS_I) && (cmd_ptr->item != NV_SMS_DM_I)) ||
     (cmd_ptr->data_ptr->sms.address >= NVI_MAX_SMS_ADDR) ||
     (cmd_ptr->data_ptr->sms.length > NV_SMS_DATA_SIZ)) {
    status = NV_BADPARM_S;
  }
  else {
    /* Reset the watchdog timer */
    KICK_WATCHDOG();

    (void)snprintf(sms_name, sizeof(sms_name), "/nvm/sms_%05d",
                                       cmd_ptr->data_ptr->sms.address);

    /* Unlink the file before writing to it as the size may be different */
    if (cmd_ptr->cmd == NV_REPLACE_F)
      (void)efs_unlink(sms_name);

    return_size = efs_put( sms_name,
                     (byte*)&(cmd_ptr->data_ptr->sms.address),
                     (sizeof(nvi_sms_type) + cmd_ptr->data_ptr->sms.length),
                      O_CREAT, 0777);

    if(return_size == -1) {
      MSG_HIGH("Write failed for SMS item address %d",
                  cmd_ptr->data_ptr->sms.address, 0, 0);
      status = NV_FAIL_S;
    }
  }
  return status;
} /* nvio_write_sms */


#ifdef FEATURE_GWSMS
#error code not present
#endif


/*===========================================================================

FUNCTION NVIO_WRITE_DIR_NUMBER

DESCRIPTION
  This function performs the "cross-update" between NV_DIR_NUMBER_I and
  NV_DIR_NUMBER_PCS_I.  That is, a write to either item is translated
  and copied to the other item as well.  THIS IS A TEMPORARY FUNCTION
  ONLY USED TO EASE THE TRANSITION FROM THE OLD STRUCTURE TO THE NEW.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_dir_number (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  nv_stat_enum_type status;     /* Status to return to calling procedure */
  nv_cmd_type       local_cmd;
  nv_cmd_type       *local_cmd_ptr = &local_cmd;
  nv_item_type      local_item;
  byte              i, index;
  byte              pcs_index, gem_index;
  

  /* First, update the specified nv item */
  index = *((byte *)cmd_ptr->data_ptr);

  if (index >= NV_MAX_NAMS)
  {
    status = NV_BADPARM_S;
  }
  else
  {
    status = nvio_write_item(cmd_ptr->item,
                            index,
                            ((byte *)cmd_ptr->data_ptr) + sizeof(index),
                            nvim_op_get_size(cmd_ptr->item));
  }

  if (status != NV_DONE_S) return status;

  /* Set up generic command buffer parameters */
  local_cmd.cmd         = NV_WRITE_F;
  local_cmd.tcb_ptr     = NULL;
  local_cmd.sigs        = 0;
  local_cmd.done_q_ptr  = NULL;
  local_cmd.data_ptr    = &local_item;
  local_cmd.status      = NV_DONE_S;

  /* Now do the cross update */
  if (cmd_ptr->item == NV_DIR_NUMBER_I) {
    local_cmd.item = NV_DIR_NUMBER_PCS_I;
    local_item.mob_dir_number.nam = cmd_ptr->data_ptr->dir_number.nam;
    for (i=0; i<NV_DIR_NUMB_SIZ; ++i) {
      local_item.mob_dir_number.digitn[i] = 
         nvt_translate_to_internal
            (cmd_ptr->data_ptr->dir_number.dir_number[i]);
    }      
    local_item.mob_dir_number.n_digits = NV_DIR_NUMB_SIZ;
  }
  else {
    if (cmd_ptr->data_ptr->mob_dir_number.n_digits > 15) {
      return NV_BADPARM_S;
    }
    local_cmd.item = NV_DIR_NUMBER_I;
    local_item.dir_number.nam = cmd_ptr->data_ptr->mob_dir_number.nam;
    for (i=0; i<NV_DIR_NUMB_SIZ; ++i) {
      local_item.dir_number.dir_number[i] = 0x20;  /* ASCII blank */
    }

    /* If n_digits is nonzero, translate and copy the digits; otherwise */
    /* we've already blanked out all the digits                         */
    if (cmd_ptr->data_ptr->mob_dir_number.n_digits != 0) {
      pcs_index = MAX(0,(cmd_ptr->data_ptr->mob_dir_number.n_digits-10)); 
      gem_index = MAX(0, (10-cmd_ptr->data_ptr->mob_dir_number.n_digits));
      while ((gem_index < NV_DIR_NUMB_SIZ)&&(pcs_index < NV_DIR_NUMB_PCS_SIZ)) {
        local_item.dir_number.dir_number[gem_index++] = 
           nvt_translate_to_external
              (cmd_ptr->data_ptr->mob_dir_number.digitn[pcs_index++]);
      }
    }
  }     

   /* Finally, update the "other" nv item */
   index = *((byte *)local_cmd_ptr->data_ptr);
   if (index >= NV_MAX_NAMS)
   {
     status = NV_BADPARM_S;
   }
   else
   {
     status = nvio_write_item(local_cmd_ptr->item,
                             index,
                             ((byte *)local_cmd_ptr->data_ptr) + sizeof(index),
                             nvim_op_get_size(local_cmd_ptr->item));
 }

  return status;

} /* nvio_write_dir_number */


/*===========================================================================

FUNCTION NVIO_WRITE_RENTAL_TIMER

DESCRIPTION
  This function writes an arbitrary value to the rental timer item.  
  Since this item is expected to be updated with very high frequency,
  it is stored in a way that minimizes EEPROM cell wear.  This unusual
  storage format calls for a special routine to write and read the
  value.
  
  The NV "increment" command would normally be used to update this item;
  this routine should only be used to initialize the item to some desired
  specific value.
  
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif


/*===========================================================================

FUNCTION NVIM_INCREMENT

DESCRIPTION
  This function processes NVM item "increment" requests.  Currently, only
  the rental timer item can use this operation.  This special processing
  is needed because the rental timer item is an unusually high wear item,
  expected to be written very frequently.

DEPENDENCIES
  None.

RETURN VALUE
  Status of the increment operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvim_increment (
  nv_cmd_type  *cmd_ptr          /* Command block */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
#ifndef NV_FEATURE_RENTAL_ITEMS
  return NV_BADPARM_S;
#else
#error code not present
#endif
} /* nvim_increment */


/*===========================================================================

FUNCTION NVIO_WRITE_RENTAL_CNT

DESCRIPTION
  This function writes a value to the rental count item.  This special
  function is needed because this item is a fixed-pool double-buffered
  item.
  
DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S    if it worked
  other        if a called function returns a failure status

SIDE EFFECTS
  None

===========================================================================*/
#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif



/*===========================================================================

FUNCTION NVIO_WRITE_PRL_ITEM

DESCRIPTION
  This function writes a PRL item into NVM directory

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked
  NV_FAIL_S if it did not work.

SIDE EFFECTS

===========================================================================*/
nv_stat_enum_type 
nvio_write_prl_item (
  int num,
  void * data_ptr,
  int item_size
)
{
  nv_stat_enum_type  status;      /* Function return value */  
  int                fd;
  char               f_name[120];
  fs_ssize_t         result = 0;
  fs_size_t          nbytes;
  fs_size_t          bytes_written;
  char               *write_buf;


  MSG_MED("size: %d", item_size, 0, 0);

  snprintf(f_name,sizeof(f_name),"/nvm/prl_%d",num);

  fd = efs_open(f_name, O_RDWR | O_CREAT, 0777 );

  nbytes = item_size;
  bytes_written = 0;
  write_buf = data_ptr;

  while (bytes_written < nbytes) {
    result = efs_write (fd,
    (const void *) (write_buf + bytes_written),
    nbytes - bytes_written);
    if (result <= 0)
      break;
    bytes_written += (fs_size_t) result;
  }

  if (result == -1) {
    status = NV_FAIL_S;
  }
  else
    status = NV_DONE_S;

  efs_close(fd);

  return status;
}


#if (defined(FEATURE_BT) || defined(FEATURE_BT_QSOC))
/*===========================================================================

FUNCTION NVIO_WRITE_BD_ADDR

DESCRIPTION
  This function processed a write command to the NV_BD_ADDR_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_bd_addr (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  int i;
  nv_stat_enum_type status;
  nv_bd_addr_type local_bd_addr;  /* tmp buffer for bd_addr */
  boolean all_zero_write_value;
  boolean bd_addr_empty = TRUE;
        
  /* Read the current Bluetooth addr. value */
  status = nvio_read_item(cmd_ptr->item,
                         0,
                         (byte*)&local_bd_addr, /* data ptr */
                         nvim_op_get_size(cmd_ptr->item));

  if ((status != NV_DONE_S) && (status != NV_NOTACTIVE_S))
  {
    return status;
  }
        
  /* Bluetooth addr can only be written once */
  if (status == NV_DONE_S) {
    for (i=0; i<NV_BD_ADDR_SIZE; i++) {
      if (local_bd_addr.bd_addr[i] != 0) {
        bd_addr_empty = FALSE;  
      }
    }
    if (!bd_addr_empty)
    {
      /* The file is now considered read-only */
      return NV_READONLY_S;
    } else {

      /* The PST may try to write a value of 0 one or more times,
       simply return success if the current BD_ADDR value is 0 */
      all_zero_write_value = TRUE;
      for (i=0; i<NV_BD_ADDR_SIZE; i++) {
        if (cmd_ptr->data_ptr->bd_addr.bd_addr[i] != 0) {
          all_zero_write_value = FALSE;
        }
      }

      if (all_zero_write_value) 
      {
        return NV_DONE_S;
      }
    }
  }
         
  status = nvio_write_item(cmd_ptr->item,
                          0,
                          (byte*)cmd_ptr->data_ptr,
                          nvim_op_get_size(cmd_ptr->item));
     
  return status;
}
#endif

#ifdef FEATURE_RFCAL_VERSION
/*===========================================================================

FUNCTION NVIO_WRITE_RF_CAL_DATE

DESCRIPTION
  This function processed a write command to the NV_RF_CAL_DATE_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_rf_cal_date (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  nv_stat_enum_type status;
  dword local_rf_cal_date;  /* tmp buffer for rf_cal_date */
        
  /* Read the current rf_cal_date */
  status = nvio_read_item(cmd_ptr->item,
                         0,
                         &local_rf_cal_date, /* data ptr */
                         nvim_op_get_size(cmd_ptr->item));

  if ((status != NV_DONE_S) && (status != NV_NOTACTIVE_S))
  {
    return status;
  }
        
  /* rf_cal_date can only be written once */
  if (status == NV_DONE_S) {
    if (local_rf_cal_date != 0) {
      /* The file is now considered read-only */
      return NV_READONLY_S;
    }
    else {
      /* The PST may try to write a value of 0 one or more times,
       simply return success if the current rf_cal_date value is 0 */
      if (cmd_ptr->data_ptr->rf_cal_date == 0) {
        return NV_DONE_S;
      }
    }
  }
         
  status = nvio_write_item(cmd_ptr->item,
                          0,
                          (byte*)cmd_ptr->data_ptr,
                          nvim_op_get_size(cmd_ptr->item));
     
  return status;
}
#endif

#ifdef FEATURE_RFCAL_VERSION
/*===========================================================================

FUNCTION NVIO_WRITE_RF_CAL_DAT_FILE

DESCRIPTION
  This function processed a write command to the NV_RF_CAL_DAT_FILE_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_rf_cal_dat_file (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  int i;
  nv_stat_enum_type status;
  byte local_rf_cal_dat_file[NV_QC_SERIAL_NUM_LEN];  /* tmp buffer for rf_cal_dat_file */
  boolean all_zero_write_value;
  boolean empty_value = TRUE;
        
  /* Read the current rf_cal_dat_file. value */
  status = nvio_read_item(cmd_ptr->item,
                         0,
                         &local_rf_cal_dat_file, /* data ptr */
                         nvim_op_get_size(cmd_ptr->item));

  if ((status != NV_DONE_S) && (status != NV_NOTACTIVE_S))
  {
    return status;
  }
        
  /* rf_cal_dat_file can only be written once */
  if (status == NV_DONE_S) {
    for (i=0; i<NV_QC_SERIAL_NUM_LEN; i++) {
      if (local_rf_cal_dat_file[i] != 0) {
        empty_value = FALSE;  
      }
    }
    if (!empty_value)
    {
      /* The file is now considered read-only */
      return NV_READONLY_S;
    } else {

      /* The PST may try to write a value of 0 one or more times,
       simply return success if the current rf_cal_dat_file value is 0 */
      all_zero_write_value = TRUE;
      for (i=0; i<NV_QC_SERIAL_NUM_LEN; i++) {
        if (cmd_ptr->data_ptr->rf_cal_dat_file[i] != 0) {
          all_zero_write_value = FALSE;
        }
      }

      if (all_zero_write_value) 
      {
        return NV_DONE_S;
      }
    }
  }
   
  status = nvio_write_item(cmd_ptr->item,
                          0,
                          (byte*)cmd_ptr->data_ptr,
                          nvim_op_get_size(cmd_ptr->item));

  return status;
}
#endif

/*===========================================================================
 
FUNCTION NVIO_WRITE_MEID
 
DESCRIPTION
  This function processed a write command to the NV_MEID_I item.
 
DEPENDENCIES
  None.
 
RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 
 
SIDE EFFECTS
  None
 
===========================================================================*/
 
LOCAL nv_stat_enum_type 
nvio_write_meid (
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
  nv_stat_enum_type status;
  qword local_meid;             /* temporary buffer for meid */
  local_meid[0] = 0;
  local_meid[1] = 0;

  /* Read the current MEID value */
  status = nvio_read_item(cmd_ptr->item,
                          0,
                          &local_meid, /* data ptr */
                          nvim_op_get_size(cmd_ptr->item));

  if((status != NV_DONE_S)&&(status != NV_NOTACTIVE_S)) {
    MSG_HIGH (" MEID Read was successful. ",0,0,0);
    return status;
  }
#if 0
  if (!(((local_meid[0] == 0) && (local_meid[1] == 0)) ||
         ((local_meid[0] == 0xFFFFFFFF) 
          && (local_meid[1] == 0x00FFFFFF)))) {
    /* The file is now considered read-only */
    MSG_HIGH (" MEID is neither all 0s nor all Fs. ",0,0,0);
    return NV_READONLY_S;
  }
#endif
  status = nvio_write_item(cmd_ptr->item,
                           0,
                           (byte*)cmd_ptr->data_ptr,
                           nvim_op_get_size(cmd_ptr->item));

  return status;
}
   

    
/*===========================================================================

FUNCTION NVIO_WRITE_IMEI

DESCRIPTION
  This function processed a write command to the NV_UE_IMEI_I item.

DEPENDENCIES
  None.

RETURN VALUE
  NV_DONE_S if it worked error status code if it failed. 

SIDE EFFECTS
  None

===========================================================================*/

LOCAL nv_stat_enum_type 
nvio_write_imei
(
  nv_cmd_type         *cmd_ptr       /* Command block */
)
{
    int i;
  nv_stat_enum_type status = NV_FAIL_S;
    nv_ue_imei_type local_ue_imei;  /* tmp buffer for ue_imei */
    boolean all_zero_write_value;
    boolean ue_imei_empty = TRUE;
#ifdef FEATURE_NV_SFS_PRIVATE
  sfs_priv_errno_enum_type sfs_ret = 0;
#endif
    
#if defined (FEATURE_NV_SFS_PRIVATE)
  /* If IMEI can't be read, call it "not active" */
  sfs_ret = sfs_priv_imei_read((uint8 *) &local_ue_imei, 
                        nvim_op_get_size(cmd_ptr->item));
  if ( sfs_ret == SFS_PRIV_INACTIVE )
  {
    status = NV_NOTACTIVE_S;
  } 
  else if( sfs_ret == SFS_PRIV_SUCCESS)
  {
    status = NV_DONE_S;
  }
  else
  {
    status = NV_FAIL_S;
  }
#else
    /* Read the current IMEI value */
    status = nvio_read_item(cmd_ptr->item,
                            0,
                            (byte*)&local_ue_imei, /* data ptr */
                             nvim_op_get_size(cmd_ptr->item));
#endif

    if ((status != NV_DONE_S) && (status != NV_NOTACTIVE_S))
    {
      return status;
    }

  /* IMEI has been written once */
  if (status == NV_DONE_S) 
  {
    for (i=0; i<NV_UE_IMEI_SIZE; i++) 
    {
      if (local_ue_imei.ue_imei[i] != 0) 
      {
          ue_imei_empty = FALSE;  
        }
      }
      if (!ue_imei_empty)
      {
        /* The file is now considered read-only */
      status = NV_READONLY_S;
    } 
    else 
    {
        /* The PST may try to write a value of 0 one or more times,
         simply return success if the current IMEI value is 0 */
        all_zero_write_value = TRUE;
      for (i=0; i<NV_UE_IMEI_SIZE; i++) 
      {
        if (cmd_ptr->data_ptr->ue_imei.ue_imei[i] != 0) 
        {
            all_zero_write_value = FALSE;
          }
        }

        if (all_zero_write_value) 
        {
        status = NV_DONE_S;
      }
    }
  } /* IMEI has been written once */

#ifdef FEATURE_NV_SFS_PRIVATE
  sfs_ret = sfs_priv_imei_write((uint8 *)cmd_ptr->data_ptr,
                                nvim_op_get_size(cmd_ptr->item));
  /* If IMEI can't be written, call it "read-only" */
  if ( sfs_ret == SFS_PRIV_READONLY ) 
  {
    /* SFS sees that Qfuse is blown so readonly */
    status = NV_READONLY_S;      
  } 
  else if( sfs_ret == SFS_PRIV_SUCCESS)
  {
    /* SFS write succeeded */
    status = NV_DONE_S;
  }
  else
  {
    /* There is a failure to write at the SFS layer*/
    status = NV_FAIL_S;
  }
#else
  /* IMEI has not been written yet */
  else 
  {
    status = nvio_write_item(cmd_ptr->item,
                             0,
                             (byte*)cmd_ptr->data_ptr,
                             nvim_op_get_size(cmd_ptr->item));
  } /* IMEI has not been written yet */
#endif

    return status;
}


/*===========================================================================

FUNCTION NVIO_WRITE

DESCRIPTION
  This function processes NVM write requests.  The function checks
  to make sure that a valid item has been requested, it performs any
  translations necessary from external format to internal NVM format
  and writes the item using EFS write services.

DEPENDENCIES
  None.

RETURN VALUE
  Status of write operation.

SIDE EFFECTS
  None.

===========================================================================*/

nv_stat_enum_type 
nvio_write (
  nv_cmd_type  *cmd_ptr          /* Command block */
)
{
  nv_stat_enum_type status;     /* Status to return to calling procedure */
  uint16 array_size; /* Array size of the item */
  byte   index; /* Specific index of an array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Check that the item code is within range. If it is not */
  /* then exit with fail status, else switch on item type.  */

  if (cmd_ptr->item >= NV_MAX_I) {
    return NV_BADPARM_S;
  }
  else 
  {

#ifdef NV_FEATURE_DIAG_WINS
    /* If write request is from diag, close gate to all other writers */
    if ((write_gate_open) || (cmd_ptr->tcb_ptr == &diag_tcb))
    {
      if (cmd_ptr->tcb_ptr == &diag_tcb)
      {
         write_gate_open = FALSE;
      }
#endif      

#ifdef FEATURE_NV_RUIM

      /* If the R-UIM supports the item,
       * return the status else use "regular" NVM.
       */
      if ( nvruim_write(cmd_ptr, &status) == NV_RUIM_SUPPORTS_ITEM )
#ifdef CUST_EDITION
       if(status == NV_DONE_S)
#endif
        return status;

#endif

      switch (cmd_ptr->item)
      {

      /* For each requested parameter if the parameter is fixed then */
      /* write it via its pointer from the external requestor's data */
      /* buffer to an EFS file. Any required                         */
      /* translation from external to internal format is performed   */
      /* as part of this write.                                      */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ESN_I:
        status = nvio_write_esn(cmd_ptr);
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ESN_ME_I:
        status = NV_READONLY_S;      
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_ESN_CHKSUM_I:
     
        /* The ESN checksum cannot be written from external   */
        /* request.  This request is denied and the NV task   */
        /* initiates transition of the DMSS to offline state  */
        /* (not yet implemented).                             */
#if 0
        status = NV_READONLY_S;      
#else
        status = NV_DONE_S;
#endif
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_VERNO_MAJ_I:

        /* Major version can be written only internally, directly */
        /* from nv_init.  So we do not allow writing it here.     */

        status = NV_READONLY_S;      
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_VERNO_MIN_I:

        /* Minor version can be written only internally, directly */
        /* from nv_init.  So we do not allow writing it here.     */

        status = NV_READONLY_S;      
        break;
                     
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DIAL_I:
          status = nvio_write_dial(NV_DIAL_I,
                                 NV_MAX_SPEED_DIALS,
                                 cmd_ptr->data_ptr->dial.address,
                                 &cmd_ptr->data_ptr->dial);
        break;
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_STACK_I:
          status = nvio_write_stdial(NV_STACK_I,
                                 NV_MAX_STACK_DIALS,
                                 cmd_ptr->data_ptr->stack.address,
                                 &cmd_ptr->data_ptr->stack);
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_NV_REDIAL_ITEM
      case NV_REDIAL_I:
        status = nvio_write_dial(NV_DIAL_I,
                                 NV_MAX_SPEED_DIALS + NVI_REDIAL_EXTENSIONS,
                                 NV_MAX_SPEED_DIALS,
                                 &cmd_ptr->data_ptr->dial);
        break;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_SMS_I:
      case NV_SMS_DM_I:
        status = nvio_write_sms(cmd_ptr);
        break;

/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_DIR_NUMBER_I:
      case NV_DIR_NUMBER_PCS_I :
        status = nvio_write_dir_number(cmd_ptr);
        break;

/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef NV_FEATURE_RENTAL_ITEMS
#error code not present
#endif

/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case NV_A_KEY_I:
      case NV_SSD_A_I:
      case NV_SSD_B_I:
        
        /* Write the item */
        index = *((byte *)cmd_ptr->data_ptr);
        if (index >= NV_MAX_NAMS) {
          status = NV_BADPARM_S;
        }
        else   /* Valid index */ {
          status = nvio_write_item(cmd_ptr->item,
                                   index,
                                 ((byte *)cmd_ptr->data_ptr) + sizeof(index),
                                   nvim_op_get_size(cmd_ptr->item));
        }
        
      break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_PAP_PASSWORD_I:
        #ifdef NV_FEATURE_PAP
           status = nvio_write_item(cmd_ptr->item,
                                    0,
                                    (byte *)cmd_ptr->data_ptr,
                                    nvim_op_get_size(cmd_ptr->item));
        #else
           status = NV_BADPARM_S;
        #endif
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_UP_KEY_I:
        #ifdef NV_FEATURE_UP

          index = *((byte *) cmd_ptr->data_ptr);
          if (index >= NV_UP_LINK_INFO_TABLE_SIZE) {
            status = NV_BADPARM_S;
          }
          else /* Valid index */ {
            status = nvio_write_item(cmd_ptr->item,
                                     index,
                                  ((byte *)cmd_ptr->data_ptr) + sizeof(index),
                                     nvim_op_get_size(cmd_ptr->item));
          }

        #else
          status = NV_BADPARM_S;
        #endif
        break;
                                  
/*- - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
/*  Roaming List is handled differently in the item manager (not a generic case) */
#ifdef NV_FEATURE_PRL_ITEMS

  #ifdef NV_FEATURE_IS683A_PRL 
      case NV_ROAMING_LIST_683_I:
        status = nvio_write_roaming_list(cmd_ptr);
        break;
  #else    
      case NV_ROAMING_LIST_I: 
        status = nvio_write_roaming_list(cmd_ptr);
        break;
  #endif
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_BD_ADDR_I:
        #if (defined(FEATURE_BT) || defined(FEATURE_BT_QSOC))
          status = nvio_write_bd_addr(cmd_ptr);
        #else
          status = NV_BADPARM_S;
        #endif
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_UE_IMEI_I:
        status = nvio_write_imei(cmd_ptr);
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_GWSMS
#error code not present
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_RF_CAL_DATE_I:
        #ifdef FEATURE_RFCAL_VERSION
          status = nvio_write_rf_cal_date(cmd_ptr);
        #else
          status = NV_BADPARM_S;
        #endif
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_RF_CAL_DAT_FILE_I:
        #ifdef FEATURE_RFCAL_VERSION
          status = nvio_write_rf_cal_dat_file(cmd_ptr);
        #else
          status = NV_BADPARM_S;
        #endif
        break;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_NV_SUPPORT_FLASH_OTP

      case NV_DEVICE_SERIAL_NO_I:
        if (flash_otp_operation(FLASH_OTP_OP_SERIALNO_WRITE,
                        (byte *)cmd_ptr->data_ptr) != FLASH_OTP_IO_SUCCESS) {
          return NV_FAIL_S;
        }
        else {
          return NV_DONE_S;
        }
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_MEID_I:
        status = nvio_write_meid(cmd_ptr);
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_MEID_ME_I:
        status = NV_READONLY_S;      
        break;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case NV_MS_SMS_AUTOSAVE_I:
        status = nvio_write_item(cmd_ptr->item, 
                                  0,
                                  (byte *)cmd_ptr->data_ptr, 
                                  nvim_op_get_size(cmd_ptr->item));
        break;

	  //Add By zzg 2012_02_02
#if defined(FEATURE_VERSION_W208S)
	case NV_MIN2_I:
	{		
		// Encode the first three digits (IS-95A 6.3.1.1)
		int i;				
		int digit1;
		int digit2;
		
		int  wstrPrefix1[]={1,5,8};
		int  wstrPrefix2[]={1,9,9};

				uint16 min_fix1 = 0;
		uint16 min_fix2 = 0;		
		
		for (i = 0; i < 3; i++) 
		{
			digit1 = wstrPrefix1[i];
			digit2 = wstrPrefix2[i];
			
			if (0 == digit1) 
			{
				digit1 = 10;
			}

			if (0 == digit2) 
			{
				digit2 = 10;
			}			
			
			min_fix1 = (uint16) (min_fix1 * 10) + digit1;			
			min_fix2 = (uint16) (min_fix2 * 10) + digit2;
		}

		min_fix1 -= 111;	
		min_fix2 -= 111;

		
		if ((min_fix1 != cmd_ptr->data_ptr->min2.min2[1])
			&&(min_fix2 != cmd_ptr->data_ptr->min2.min2[1]))
		{
			break;
		}
		//else do nothing, don't break, go to default blow	
	}
#endif
	  //Add End

      default:
        /* All other items get generic treatment, if they have 
            a valid entry */
        if (nvim_op_get_presence(cmd_ptr->item)) {
          array_size = nvim_op_get_array_size(cmd_ptr->item);

          if (array_size == 0) {
            status = nvio_write_item(cmd_ptr->item,
                                     0,
                                     (void *) cmd_ptr->data_ptr,
                                     nvim_op_get_size(cmd_ptr->item));
          }
          else /* More than one item */ {
            index = *((byte *) cmd_ptr->data_ptr);
            if (index >= array_size) {
              status = NV_BADPARM_S;
            }
            else {
              //MSG_MED("Data after: %.4x", *(((byte *) cmd_ptr->data_ptr) + sizeof(index)), 0, 0);
              status = nvio_write_item(cmd_ptr->item,
                                       index,
                                       ((byte *)cmd_ptr->data_ptr)+sizeof(index),
                                       nvim_op_get_size(cmd_ptr->item));
            }
          }
        }
        else   /* Not a valid entry */ {
          ERR("nvw_write %d not for this target", cmd_ptr->item, 0, 0);
          status = NV_BADPARM_S;
        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      }/* switch */
      return status;
#ifdef NV_FEATURE_DIAG_WINS
    }
    else
    {
      /* If write gate is closed, tell callers write succeeded even though */
      /* it didn't really (avoids recovery processing in client code)      */
      return NV_DONE_S;
    }
#endif  
  } /* if */
} /* nvio_write */
                   
#endif /* FEATURE_NV_ITEM_MGR */
