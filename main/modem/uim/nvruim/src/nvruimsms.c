/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               N V    R - U I M    S M S   S E R V I C E S

GENERAL DESCRIPTION
  Provides I/O services for SMS NVM items on the R-UIM.

EXTERNALIZED FUNCTIONS
  nvruimsms_read
    Read an SMS message or SMSP item on the R-UIM

  nvruimsms_write
    Write an SMS message or SMSP item on the R-UIM

  nvruimsms_free
    Erase an SMS message or SMSP item on the R-UIM

  nvruimsms_buildlist
    Build an SMS control list

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/nvruimsms.c_v   1.4.1.1   13 Nov 2001 18:00:20   hqu  $
$Header: //source/qcom/qct/modem/uim/nvruim/rel/07H1_2/src/nvruimsms.c#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/113/03  wli     Corrected lint errors.
11/13/01   HQ      Changed SMSI_REPLY_OPTION to SMSI_BEARER_REPLY.
05/18/01   ck      Added support to interface with the UIM server
03/19/01   wli     Fixed the bug that caused confusion when choosing to free
                   an entry from either SMS or SMSP.
12/15/00   CK      Removed the debug feature around reading service table 
                   before building the SMS list.
                   Changed the feature define to check for 'FF' as invalid
                   SMSP record per IS-820 .
10/24/00   HQ      Use the renamed conversions functions defined in uasmsu.h.
08/30/00   HQ      Corrected errors in SMS and SMSP read and write operations;
                   use the new function prototype for ui_sms_nv_data().
08/04/00   ms      Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include <memory.h>

#if defined(FEATURE_NV_RUIM) && defined(FEATURE_RUIM_SMS)

#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "auth.h"
#include "nv.h"
#include "nvi.h"
#include "nvruimi.h"
#include "task.h"
#include "uasms.h"
#include "uasmsx.h"
#include "uasmsi.h"
#include "uiusmsl.h"
#include "bit.h"
#include "uasmsu.h"
#include "uixuasms.h"
#include "uissms.h"
#include "nvruimsmsi.h"


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

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                            Local Data                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
static uim_cmd_type cmd;               /* Command buffer for each command. */ 
static uasms_tl_message_type tl_msg;      /* transport layer message
                                             for conversion */
static uasms_client_message_type cl_msg;  /* client message, for 
                                             conversion */
static smsi_msg_type smsi_msg;            /* UI Format message, for 
                                             conversion */
static uasms_raw_bd_type raw_bd_data;     /* Raw Bearer Data of SMS Msg */
static uasms_OTA_message_type sms_msg;    /* OTA format message, for
                                             conversion */
static byte nvruimsms_max_sms;        /* Max SMS & SMSP msg size */
static byte nvruimsms_max_smsp;

static byte nvruimsms_smsp_offset;    /* offest used when storing SMSP 
                                         entries in SMS control list */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

MACRO NVRUIMSMS_SERVICETABLE_ISSMS


DESCRIPTION
  Determines if SMS is active and available according to the CDMA service table

PARAMETERS
  x   The byte of the CDMA Service Table containing the SMS flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SMS services are enabled, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define NVRUIMSMS_SERVICETABLE_ISSMS( x ) \
  ( ( (x) & NVRUIMSMS_SERVICETABLE_SMS_ALLOC) && \
    ( (x) & NVRUIMSMS_SERVICETABLE_SMS_ACTIVE) )


/*===========================================================================

MACRO NVRUIMSMS_SERVICETABLE_ISSMSP


DESCRIPTION
  Determines if SMSP services are active and available according to the CDMA 
  service table

PARAMETERS
  x   The byte of the CDMA Service Table containing the SMSP flags

DEPENDENCIES
  None

RETURN VALUE
  TRUE if SMSP services are enabled and available, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#define NVRUIMSMS_SERVICETABLE_ISSMSP( x ) \
  ( ( (x) & NVRUIMSMS_SERVICETABLE_SMSP_ALLOC) && \
    ( (x) & NVRUIMSMS_SERVICETABLE_SMSP_ACTIVE) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                                                                         */
/*                             Functions                                   */
/*                                                                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION NVRUIMSMS_READ                    

DESCRIPTION
  Reads an SMS message or SMSP item from the RUIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status nvruimsms_read
(
  nv_cmd_type *nv_cmd_ptr   /* nv data */
)
{
  /* used for parsing SMSP data */
  word offset;
  uns16 parameters;
  dword smsi_parameters = 0;
  byte dcs = 0;
  byte validity_period = 0;
  word chari_size; /* size of character in SMSP address */
  int i; /* loop iterator */
  uasms_status_e_type status_uasms; /* status of uasms conversion routine */

  uim_rpt_status status; /* status of ruim operation */


  /* Set up cmd item */
  cmd.access_uim.num_bytes = NVRUIM_EF_BUF_SIZE;
  cmd.access_uim.data_ptr = nvruim_ef_buffer;
  cmd.access_uim.offset = nv_cmd_ptr->data_ptr->sms.address;

  /* Determine if we're reading an SMS message or SMSP item */
  if (cmd.access_uim.offset <= nvruimsms_smsp_offset)
    cmd.access_uim.item = UIM_CDMA_SMS;
  else
  {
    cmd.access_uim.item = UIM_CDMA_SMS_PARAMS;
    cmd.access_uim.offset -= nvruimsms_smsp_offset;
  }
  cmd.access_uim.rec_mode = UIM_ABSOLUTE;
  /* Read SMS or SMSP data */
  cmd.access_uim.access = UIM_READ_F;
  status = nvruim_access( &cmd );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Translate the message into the format the application desires            */
  if (status == UIM_PASS)
  {
    if( cmd.access_uim.item != UIM_CDMA_SMS_PARAMS )
    {
      /* This is an EFsms message
      */
      /* format SMS message */
      smsi_msg.tag = (smsi_msg_tag_type) nvruim_ef_buffer[0];
      sms_msg.data_len = nvruim_ef_buffer[1];
      memcpy(sms_msg.data,nvruim_ef_buffer+2, UASMS_MAX_LEN);
      sms_msg.format = UASMS_FORMAT_CDMA;

      /* Convert from OTA format to NV packed format */
      status_uasms = uasmsx_decode_CDMA_tl(&sms_msg, &tl_msg, &raw_bd_data);
      if (status_uasms != UASMS_OK_S)
        return UIM_FAIL;
      uasmsx_convert_tl2cl(&tl_msg, &cl_msg);
      cl_msg.raw_bd = raw_bd_data;

      status_uasms = uasmsu_convert_to_smsi(&cl_msg, &smsi_msg);
      if (status_uasms != UASMS_OK_S)
        return UIM_FAIL;
      smsi_msg.tag = (smsi_msg_tag_type) nvruim_ef_buffer[0];
      uasmsu_nv_data(TRUE, &nv_cmd_ptr->data_ptr->sms, &smsi_msg, TRUE );
    }
    else
    {
      /* This is an EFsmsp message
      */
      /* initialise client msg */
      memset(&cl_msg, 0, sizeof(cl_msg));
      cl_msg.tag = UASMS_MO_TEMPLATE;

      /* format SMSP entry */
#ifdef FEATURE_RUIM_DEBUG_SMS_NOT_STRICTFF
#error code not present
#else
      if (nvruim_ef_buffer[0] != NVRUIMSMS_SMSP_EMPTY_CHAR) 
#endif
      {
        /* teleservice ID */
        offset = 16;
        cl_msg.teleservice = (uasms_teleservice_e_type) b_unpackw(nvruim_ef_buffer, 
            offset, 16);
        offset += 16;

        /* Parameter Indicators */
        parameters = b_unpackw(nvruim_ef_buffer, offset, 16);
        offset += 16;

        /* Origination Address */
        if (parameters & NVRUIMSMS_SMSP_ORIGADDR)
        {
          smsi_parameters|=SMSI_ORIG_ADDR;
          offset += 16;
          cl_msg.address.digit_mode = (uasms_digit_mode_e_type)
            b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          cl_msg.address.number_mode = (uasms_number_mode_e_type) 
            b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          if (cl_msg.address.digit_mode)
          {
            cl_msg.address.number_type = (uasms_number_type_e_type) 
              b_unpackb(nvruim_ef_buffer, offset, 3);
            offset += 3;
            if (!cl_msg.address.number_mode)
            {
              cl_msg.address.number_plan = (uasms_number_plan_e_type) 
                b_unpackb(nvruim_ef_buffer, offset, 4);
              offset += 4;
            }
          }
          cl_msg.address.number_of_digits = b_unpackb(nvruim_ef_buffer, offset, 8);
          offset += 8;
          chari_size = (cl_msg.address.digit_mode)?8:4;
          for (i=0; i<cl_msg.address.number_of_digits; i++)
          {
            cl_msg.address.digits[i] = b_unpackb(nvruim_ef_buffer, offset, chari_size);
            offset += chari_size;
          }
          /* byte allign */
          offset = (offset+0x7)&(~0x7);
        }
        else
        {
          /* skip field */
          while (nvruim_ef_buffer[offset/8] == NVRUIMSMS_SMSP_EMPTY_CHAR)
            offset += 8;
          /* Check to see if we're at an Orig. Address record.  If so, 
           * skip it */
          if (nvruim_ef_buffer[offset/8] == NVRUIMSMS_SMSP_ID_ORIGADDR)
          {
            offset += 8;
            offset += nvruim_ef_buffer[offset/8]*8+8;
          }
        }
        
        /* Destination Address */
        if (parameters & NVRUIMSMS_SMSP_DESTADDR)
        {
          smsi_parameters|=SMSI_DEST_ADDR;
          offset += 16;
          cl_msg.address.digit_mode = (uasms_digit_mode_e_type) 
            b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          cl_msg.address.number_mode = (uasms_number_mode_e_type) 
            b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          if (cl_msg.address.digit_mode)
          {
            cl_msg.address.number_type = (uasms_number_type_e_type) 
              b_unpackb(nvruim_ef_buffer, offset, 3);
            offset += 3;
            if (!cl_msg.address.number_mode)
            {
              cl_msg.address.number_plan = (uasms_number_plan_e_type) 
                b_unpackb(nvruim_ef_buffer, offset, 4);
              offset += 4;
            }
          }
          cl_msg.address.number_of_digits = b_unpackb(nvruim_ef_buffer, offset, 8);
          offset += 8;
          chari_size = (cl_msg.address.digit_mode)?8:4;
          for (i=0; i<cl_msg.address.number_of_digits; i++)
          {
            cl_msg.address.digits[i] = b_unpackb(nvruim_ef_buffer, offset, chari_size);
            offset += chari_size;
          }
          /* byte allign */
          offset = (offset+0x7)&(~0x7);
        }
        else
        {
          /* skip field */
          while (nvruim_ef_buffer[offset/8] == NVRUIMSMS_SMSP_EMPTY_CHAR)
            offset += 8;
          /* Check to see if we're at an Orig. Address record.  If so, 
           * skip it */
          if (nvruim_ef_buffer[offset/8] == NVRUIMSMS_SMSP_ID_DESTADDR)
          {
            offset += 8;
            offset += nvruim_ef_buffer[offset/8]*8+8;
          }
        }

        /* Data Coding Scheme */
        if (parameters & NVRUIMSMS_SMSP_DCS)
        {
          dcs = nvruim_ef_buffer[offset/8];
        }
        offset += 8;

        /* Validity Period */
        if (parameters & NVRUIMSMS_SMSP_VALIDPRD)
        {
          smsi_parameters|=SMSI_VALID_REL;
          validity_period=nvruim_ef_buffer[offset/8];
        }
        offset+=8;

        /* Service Category */
        if (parameters & NVRUIMSMS_SMSP_SVCCATEGORY)
        {
          offset+=16;
          cl_msg.service=(uasms_service_e_type) b_unpackw(nvruim_ef_buffer, 
              offset, 16);
          offset+=16;
        }

        /* Origination Subaddress */
        if (parameters & NVRUIMSMS_SMSP_ORIGSUBADDR)
        {
          smsi_parameters|=SMSI_ORIG_SUBADDR;
          offset+=16;
          cl_msg.subaddress.type=(uasms_subaddress_type_e_type) 
            b_unpackb(nvruim_ef_buffer, offset, 3);
          offset+=3;
          cl_msg.subaddress.odd=b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          cl_msg.subaddress.number_of_digits=b_unpackb(nvruim_ef_buffer, offset, 8);
          offset+=8;
          for (i=0; i<cl_msg.subaddress.number_of_digits; i++)
          {
            cl_msg.subaddress.digits[i]=b_unpackb(nvruim_ef_buffer, offset, 8);
            offset+=8;
          }
          /* byte align */
          offset+=4;
        }

        /* Destination Subaddress */
        if (parameters & NVRUIMSMS_SMSP_DESTSUBADDR)
        {
          smsi_parameters|=SMSI_DEST_SUBADDR;
          offset+=16;
          cl_msg.subaddress.type=(uasms_subaddress_type_e_type) 
            b_unpackb(nvruim_ef_buffer, offset, 3);
          offset+=3;
          cl_msg.subaddress.odd=b_unpackb(nvruim_ef_buffer, offset, 1);
          offset++;
          cl_msg.subaddress.number_of_digits=b_unpackb(nvruim_ef_buffer, offset, 8);
          offset+=8;
          for (i=0; i<cl_msg.subaddress.number_of_digits; i++)
          {
            cl_msg.subaddress.digits[i]=b_unpackb(nvruim_ef_buffer, offset, 8);
            offset+=8;
          }
          offset+=4;
        }

        /* Bearer Reply Option */
        if (parameters & NVRUIMSMS_SMSP_BRO)
        {
          smsi_parameters|=SMSI_BEARER_REPLY;
          cl_msg.is_tl_ack_requested=TRUE;
          offset+=24;
        }

        /* Bearer Data */
        if (parameters & NVRUIMSMS_SMSP_BDATA)
        {
          smsi_parameters|=SMSI_BEARER_DATA;
          cl_msg.raw_bd.format=UASMS_FORMAT_CDMA;
          offset+=8;
          cl_msg.raw_bd.bd_len=nvruim_ef_buffer[offset/8];
          offset+=8;
          memcpy(cl_msg.raw_bd.bd_data, nvruim_ef_buffer+(offset/8), 
              cl_msg.raw_bd.bd_len);
        }
      } /* if non-empty SMSP entry */
      else
      {
        return UIM_FAIL;
      }

      /* convert message to NV format */
      status_uasms=uasmsu_convert_to_smsi(&cl_msg, &smsi_msg);
      if (status_uasms != UASMS_OK_S)
        return UIM_FAIL;
      smsi_msg.valid_rel=validity_period;
      smsi_msg.msg_data.user_data.msg_encoding=dcs;
      smsi_msg.sms_param_m|=smsi_parameters;
      uasmsu_nv_data(TRUE, &nv_cmd_ptr->data_ptr->sms, &smsi_msg, TRUE );
    }
  }
  return status;
} /* nvruimsms_read */


/*===========================================================================

FUNCTION NVRUIMSMS_WRITE

DESCRIPTION
  Writes an SMS message or SMSP item to the RUIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status nvruimsms_write
(
  nv_cmd_type *nv_cmd_ptr
)
{
  /* for SMSP parsing */
  word offset=0, offset_sav;
  word chari_size;
  word i;
  word parameters=0;

  uim_rpt_status status; /* status of ruim operation */
  uasms_status_e_type status_uasms; /* status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Translate the message into the format the RUIM desires            */

  /* Convert from NV packed format to OTA format */
  uasmsu_nv_data(FALSE, &nv_cmd_ptr->data_ptr->sms, &smsi_msg, TRUE );

  /* Determine if writing an SMS or SMSP entry */
  if (nv_cmd_ptr->data_ptr->sms.address <= nvruimsms_smsp_offset)
  {
    memset(nvruim_ef_buffer, NVRUIMSMS_SMS_EMPTY_CHAR, NVRUIM_EF_BUF_SIZE);
    status_uasms=uasmsu_convert_from_smsi(&smsi_msg, &cl_msg);
    if (status_uasms != UASMS_OK_S)
      return UIM_FAIL;
    uasmsx_convert_cl2tl(&cl_msg, &tl_msg);
    status_uasms=uasmsx_encode_CDMA_tl(&tl_msg, &cl_msg.raw_bd, &sms_msg);
    if (status_uasms != UASMS_OK_S)
      return UIM_FAIL;

    /* format SMS message */
    nvruim_ef_buffer[0]=(byte) smsi_msg.tag;
    nvruim_ef_buffer[1]=(byte) sms_msg.data_len;

    /* Make sure that the message isn't too long for our buffer or for
     * the card */
    if (sms_msg.data_len+2 > nvruimsms_max_sms)
    {
      MSG_ERROR("SMS too large: %d", sms_msg.data_len+2, 0, 0);
      return UIM_FAIL;
    }

    memcpy(nvruim_ef_buffer+2,sms_msg.data, sms_msg.data_len);
    cmd.access_uim.num_bytes = nvruimsms_max_sms;
    cmd.access_uim.item = UIM_CDMA_SMS;
  }
  else
  {
    /* format SMSP entry */
    status_uasms=uasmsu_convert_from_smsi(&smsi_msg, &cl_msg);
    if (status_uasms != UASMS_OK_S)
      return UIM_FAIL;

    memset(nvruim_ef_buffer, NVRUIMSMS_SMSP_EMPTY_CHAR, NVRUIM_EF_BUF_SIZE);

    /* Teleservice ID */
    nvruim_ef_buffer[0]=0;
    nvruim_ef_buffer[1]=2;
    b_packw(cl_msg.teleservice,nvruim_ef_buffer,offset+16,16);

    /* Skip Parameter Indicators For Now */
    offset+=48;

    /* Origination Address */
    if (smsi_msg.sms_param_m & SMSI_ORIG_ADDR)
    {
      parameters|=NVRUIMSMS_SMSP_ORIGADDR;
      nvruim_ef_buffer[offset/8]=2;
      offset_sav=offset+8; /* write parameter length later */
      offset+=16;
      b_packb(cl_msg.address.digit_mode, nvruim_ef_buffer, offset, 1);
      b_packb(cl_msg.address.number_mode, nvruim_ef_buffer, offset+1, 1);
      offset+=2;
      if (cl_msg.address.digit_mode)
      {
        b_packb(cl_msg.address.number_type, nvruim_ef_buffer, offset, 3);
        offset+=3;
        if (!cl_msg.address.number_mode)
        {
          b_packb(cl_msg.address.number_plan, nvruim_ef_buffer, offset, 4);
          offset+=4;
        }
      }
      b_packb(cl_msg.address.number_of_digits, nvruim_ef_buffer, offset, 8);
      offset+=8;
      chari_size=(cl_msg.address.digit_mode)?8:4;
      for (i=0; i<cl_msg.address.number_of_digits; i++)
      {
        /* Make sure we don't exceed buffer capacity */
        if (offset >= NVRUIM_EF_BUF_SIZE*8)
          return UIM_FAIL;
        b_packb(cl_msg.address.digits[i], nvruim_ef_buffer, offset, chari_size);
        offset+=chari_size;
      }
      /* byte allign */
      offset=(offset+0x7)&(~0x7);
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      /* write parameter length */
      b_packb((byte) ((offset-offset_sav-1)/8), nvruim_ef_buffer, offset_sav, 8);
    }
    else
    {
      nvruim_ef_buffer[offset/8]=NVRUIMSMS_SMSP_EMPTY_CHAR;
      offset+=8;
    }

    /* Destination Address */
    if (smsi_msg.sms_param_m & SMSI_DEST_ADDR)
    {
      parameters|=NVRUIMSMS_SMSP_DESTADDR;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=4;
      offset_sav=offset+8; /* write parameter length later */
      offset+=16;
      /* Make sure we don't exceed buffer capacity */
      if (offset+1 >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.address.digit_mode, nvruim_ef_buffer, offset, 1);
      b_packb(cl_msg.address.number_mode, nvruim_ef_buffer, offset+1, 1);
      offset+=2;
      if (cl_msg.address.digit_mode)
      {
        /* Make sure we don't exceed buffer capacity */
        if (offset >= NVRUIM_EF_BUF_SIZE*8)
          return UIM_FAIL;
        b_packb(cl_msg.address.number_type, nvruim_ef_buffer, offset, 3);
        offset+=3;
        if (!cl_msg.address.number_mode)
        {
          /* Make sure we don't exceed buffer capacity */
          if (offset >= NVRUIM_EF_BUF_SIZE*8)
            return UIM_FAIL;
          b_packb(cl_msg.address.number_plan, nvruim_ef_buffer, offset, 4);
          offset+=4;
        }
      }
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.address.number_of_digits, nvruim_ef_buffer, offset, 8);
      offset+=8;
      chari_size=(cl_msg.address.digit_mode)?8:4;
      for (i=0; i<cl_msg.address.number_of_digits; i++)
      {
        /* Make sure we don't exceed buffer capacity */
        if (offset >= NVRUIM_EF_BUF_SIZE*8)
          return UIM_FAIL;
        b_packb(cl_msg.address.digits[i], nvruim_ef_buffer, offset, chari_size);
        offset+=chari_size;
      }
      /* byte allign */
      offset=(offset+0x7)&(~0x7);
      /* write parameter length */
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb((byte) ((offset-offset_sav-1)/8), nvruim_ef_buffer, offset_sav, 8);
    }
    else
    {
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=NVRUIMSMS_SMSP_EMPTY_CHAR;
      offset+=8;
    }

    /* Data Coding Scheme */
    if (smsi_msg.sms_param_m & SMSI_USER_DATA) 
    {
      parameters |= NVRUIMSMS_SMSP_DCS;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=smsi_msg.msg_data.user_data.msg_encoding;
    }
    else
    {
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=NVRUIMSMS_SMSP_EMPTY_CHAR;
    }
    offset+=8;

    /* Validity Period */
    if (smsi_msg.sms_param_m & SMSI_VALID_REL)
    {
      parameters |= NVRUIMSMS_SMSP_VALIDPRD;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=smsi_msg.valid_rel;
    }
    else
    {
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=NVRUIMSMS_SMSP_EMPTY_CHAR;
    }
    offset+=8;
    
    /* Service Category */
    if (cl_msg.service)
    {
      parameters |= NVRUIMSMS_SMSP_SVCCATEGORY;
      /* Make sure we don't exceed buffer capacity */
      if (offset+8 >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=1;
      nvruim_ef_buffer[offset/8+1]=2;
      b_packw(cl_msg.service, nvruim_ef_buffer, offset+16, 16);
      offset+=32;
    }

    /* Origination Subaddress */
    if (smsi_msg.sms_param_m & SMSI_ORIG_SUBADDR)
    {
      parameters |= NVRUIMSMS_SMSP_ORIGSUBADDR;

      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=3;
      offset_sav=offset+8;
      offset+=16;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.type, nvruim_ef_buffer, offset, 3);
      offset+=3;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.odd, nvruim_ef_buffer, offset, 1);
      offset++;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.number_of_digits, nvruim_ef_buffer, offset, 8);
      offset+=8;
      for (i=0; i<cl_msg.subaddress.number_of_digits; i++)
      {
        /* Make sure we don't exceed buffer capacity */
        if (offset >= NVRUIM_EF_BUF_SIZE*8)
          return UIM_FAIL;
        b_packb(cl_msg.subaddress.digits[i], nvruim_ef_buffer, offset, 8);
        offset+=8;
      }
      /* byte align */
      offset+=4;
    }

    /* Destination Subaddress */
    if (smsi_msg.sms_param_m & SMSI_DEST_SUBADDR)
    {
      parameters |= NVRUIMSMS_SMSP_DESTSUBADDR;

      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=5;
      offset_sav=offset+8;
      offset+=16;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.type, nvruim_ef_buffer, offset, 3);
      offset+=3;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.odd, nvruim_ef_buffer, offset, 1);
      offset++;
      /* Make sure we don't exceed buffer capacity */
      if (offset >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      b_packb(cl_msg.subaddress.number_of_digits, nvruim_ef_buffer, offset, 8);
      offset+=8;
      for (i=0; i<cl_msg.subaddress.number_of_digits; i++)
      {
        /* Make sure we don't exceed buffer capacity */
        if (offset >= NVRUIM_EF_BUF_SIZE*8)
          return UIM_FAIL;
        b_packb(cl_msg.subaddress.digits[i], nvruim_ef_buffer, offset, 8);
        offset+=8;
      }
      /* byte align */
      offset+=4;
    }

    /* Bearer Reply Option */
    if (smsi_msg.sms_param_m & SMSI_BEARER_REPLY)
    {
      parameters |= NVRUIMSMS_SMSP_BRO;
      /* Make sure we don't exceed buffer capacity */
      if (offset+8 >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=6;
      nvruim_ef_buffer[offset/8+1]=1;
      b_packb(smsi_msg.bearer_reply, nvruim_ef_buffer, offset+16, 6);
      offset+=24;
    }

    /* Bearer Data */
    if (smsi_msg.sms_param_m & SMSI_BEARER_DATA)
    {
      parameters|=NVRUIMSMS_SMSP_BDATA;
      /* Make sure we don't exceed buffer capacity */
      if (offset+8 >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      nvruim_ef_buffer[offset/8]=8;
      nvruim_ef_buffer[offset/8+1]=(byte) (cl_msg.raw_bd.bd_len);
      offset+=16;
      /* Make sure we don't exceed buffer capacity */
      if (offset+cl_msg.raw_bd.bd_len*8 >= NVRUIM_EF_BUF_SIZE*8)
        return UIM_FAIL;
      memcpy(nvruim_ef_buffer+(offset/8), cl_msg.raw_bd.bd_data, 
          (byte) cl_msg.raw_bd.bd_len);
      offset+=(word) (cl_msg.raw_bd.bd_len*8);
    }

    /* Write Parameter Indicators */
    b_packw(parameters, nvruim_ef_buffer, 32, 16);
    cmd.access_uim.num_bytes = nvruimsms_max_smsp;
    cmd.access_uim.item = UIM_CDMA_SMS_PARAMS;
    nv_cmd_ptr->data_ptr->sms.address-=nvruimsms_smsp_offset;

    if( offset > nvruimsms_max_smsp*8 )
    {
      MSG_ERROR("SMSP too large: %d", offset, 0, 0);
      return UIM_FAIL;
    }
  }

  /* cmd.access_uim.num_bytes = NVRUIM_EF_BUF_SIZE; */
  cmd.access_uim.data_ptr = nvruim_ef_buffer;
  cmd.access_uim.offset=nv_cmd_ptr->data_ptr->sms.address;

  /* Write data */
  cmd.access_uim.access = UIM_WRITE_F;
  status = nvruim_access( &cmd );
  return status;
} /* nvruimsms_write */


/*===========================================================================

FUNCTION NVRUIMSMS_BUILDLIST

DESCRIPTION
  Builds an SMS control list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status nvruimsms_buildlist
(
  nv_cmd_type *nv_cmd_ptr
)
{
  byte sms_msgs, smsp_msgs; /* Number of SMS and SMSP entries supported */
  int i;
  byte sms_list_idx;
  byte sms_list_counts[MAX_SMS_LIST];

  boolean is_sms, is_smsp;

  uim_rpt_status status; /* status of ruim operation */

  uasms_status_e_type status_uasms; /* status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Determine maximum number of messages supported by client                 */
  
  nvruimsms_smsp_offset=sms_msgs=nv_cmd_ptr->data_ptr->sms.data[
    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS];
  smsp_msgs=nv_cmd_ptr->data_ptr->sms.data[
    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+1];

  /* default values of max msg length */
  nvruimsms_max_sms=0;
  nvruimsms_max_smsp=0;

  sms_list_idx=0;

  for (i=0; i<MAX_SMS_LIST; i++)
    sms_list_counts[i]=0;

  /* Set SMS validation marker */
  nv_cmd_ptr->data_ptr->sms.data[
    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+4]=0xdb;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read CDMA Service Table                                                  */
  
  cmd.access_uim.num_bytes = NVRUIMSMS_SERVICETABLE_SIZE;
  cmd.access_uim.data_ptr = nvruim_ef_buffer;
  cmd.access_uim.offset=0;
  cmd.access_uim.item = UIM_CDMA_CDMA_SVC_TABLE;
  cmd.access_uim.access = UIM_READ_F;

  status=nvruim_access( &cmd );
  is_sms=NVRUIMSMS_SERVICETABLE_ISSMS(nvruim_ef_buffer[0]);
  is_smsp=NVRUIMSMS_SERVICETABLE_ISSMSP(nvruim_ef_buffer[2]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Parse CDMA Service Table                                                 */

  if (status == UIM_PASS)
  {
    if (is_sms)
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      /* Read SMS File                                                        */

      for (cmd.access_uim.offset=1; cmd.access_uim.offset <= sms_msgs;
          cmd.access_uim.offset++)
      {
        /* Read SMS message */
        cmd.access_uim.num_bytes = NVRUIM_EF_BUF_SIZE;
        cmd.access_uim.data_ptr = nvruim_ef_buffer;
        cmd.access_uim.item = UIM_CDMA_SMS;
        cmd.access_uim.rec_mode = UIM_ABSOLUTE;
        cmd.access_uim.access = UIM_READ_F;
        status=nvruim_access( &cmd );
        if (status != UIM_PASS)
          break;
        nvruimsms_max_sms=(byte) (cmd.access_uim.num_bytes_rsp);

        /* Determine number of messages in file */
        if (sms_msgs > cmd.access_uim.num_records_rsp)
          sms_msgs=(byte) (cmd.access_uim.num_records_rsp);

        /* Parse SMS message */
        if (!(nvruim_ef_buffer[0] & NVRUIMSMS_STATUS_USED))
        {
          /* Available space */
          nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=UI_SMS_AVAIL;
          nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
            sms_list_counts[UI_SMS_AVAIL]++;
          //sms_list_idx+=2;
        }
        else if( (nvruim_ef_buffer[0] & NVRUIMSMS_STATUS_MO) || 
                 ! (nvruim_ef_buffer[0] & NVRUIMSMS_STATUS_NOT_READ) )
        {
          /* MO Message or read message */
          nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=UI_SMS_TXT_OLD;
          nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
            sms_list_counts[UI_SMS_TXT_OLD]++;
          //sms_list_idx+=2;
        }
        else
        {
          /* We need to parse the message to determine priority, or if it's a 
           * VCN */
          /* format SMS message */
          sms_msg.data_len=nvruim_ef_buffer[1];
          memcpy(sms_msg.data,nvruim_ef_buffer+2, UASMS_MAX_LEN);
          sms_msg.format=UASMS_FORMAT_CDMA;

          /* Convert from OTA format to SMSI format */
          status_uasms=uasmsx_decode_CDMA_tl(&sms_msg, &tl_msg, &raw_bd_data);
          if (status_uasms != UASMS_OK_S)
            return UIM_FAIL;
          uasmsx_convert_tl2cl(&tl_msg, &cl_msg);
          status_uasms=uasmsu_convert_to_smsi(&cl_msg, &smsi_msg);
          if (status_uasms != UASMS_OK_S)
            return UIM_FAIL;
          
          /* Determine if new message is voice by checking */
          /* SMS teleservice and if number of message      */
          /* parameter is present.                         */
          if ((( smsi_msg.tele_srv == SMSI_VMN_95 ) ||
                ( smsi_msg.tele_srv == SMSI_CMT_91_VOICE_MAIL ) ||
                ( smsi_msg.tele_srv == SMSI_AWI_95 )) &&
              (( smsi_msg.sms_param_m & SMSI_NUM_OF_MSGS) != 0))
          {
            if (!sms_list_counts[UI_SMS_VC])
            {
              nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=UI_SMS_VC;
              nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
                sms_list_counts[UI_SMS_VC]++;
              //sms_list_idx+=2;
            }
          }
          /* Check to see if message urgent */
          else if ((( smsi_msg.sms_param_m & SMSI_PRIORITY) != 0) &&
              ( smsi_msg.msg_data.priority >= UI_SMS_URGENT_IND))
          {
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              UI_SMS_TXT_NEW_URGENT;
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              sms_list_counts[UI_SMS_TXT_NEW_URGENT]++;
            //sms_list_idx+=2;
          }
          /* Message is new, not urgent */
          else
          {
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              UI_SMS_TXT_NEW_NOT_URGENT;
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              sms_list_counts[UI_SMS_TXT_NEW_NOT_URGENT]++;
            //sms_list_idx+=2;
          }
        } /* else */
      } /* for */

      /* Check for error */
      if (status != UIM_PASS)
      {
        MSG_MED("Got an error reading SMS messages from the RUIM...",0,0,0);
        status = UIM_PASS;
        /* Modify number of SMS messages */
        sms_msgs=(byte) (cmd.access_uim.offset-1);
      }
      nv_cmd_ptr->data_ptr->sms.data[sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS]=
        sms_msgs; 
      if (is_smsp)
      {
        /*- - - -- - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

        /* Read SMSP File                                                  */
        sms_list_idx=(byte) (nvruimsms_smsp_offset*2);

        for (cmd.access_uim.offset=1; cmd.access_uim.offset <= smsp_msgs;
            cmd.access_uim.offset++)
        {
          /* Read SMSP entry */
          cmd.access_uim.num_bytes = NVRUIM_EF_BUF_SIZE;
          cmd.access_uim.data_ptr = nvruim_ef_buffer;
          cmd.access_uim.item = UIM_CDMA_SMS_PARAMS;
          cmd.access_uim.rec_mode = UIM_ABSOLUTE;
          cmd.access_uim.access=UIM_READ_F;
          status=nvruim_access( &cmd );
          if (status != UIM_PASS)
            break;
          nvruimsms_max_smsp=(byte) (cmd.access_uim.num_bytes_rsp);

          /* Determine number of messages in file */
          if (smsp_msgs > cmd.access_uim.num_records_rsp)
            smsp_msgs=(byte) (cmd.access_uim.num_records_rsp);

          /* Parse SMSP data into control list */
#ifdef FEATURE_RUIM_DEBUG_SMS_NOT_STRICTFF
#error code not present
#else
          if (nvruim_ef_buffer[0] != NVRUIMSMS_SMSP_EMPTY_CHAR) 
#endif
          {
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=UI_SMS_TXT_OLD;
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              sms_list_counts[UI_SMS_TXT_OLD]++;
          }
          else
          {
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=UI_SMS_AVAIL;
            nv_cmd_ptr->data_ptr->sms.data[sms_list_idx++]=
              sms_list_counts[UI_SMS_AVAIL]++;
          }
        }
        /* Check for error */
        if (status != UIM_PASS)
        {
          MSG_MED("Got an error reading SMSP services from the RUIM...",0,0,0);
          status = UIM_PASS;
          /* Modify number of SMSP entries */
          smsp_msgs=(byte) (cmd.access_uim.offset-1);
        }
        nv_cmd_ptr->data_ptr->sms.data[
          sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+1]=smsp_msgs;
      }
      else
      {
        /* No SMSP entries available.  Indicate that in control list */
        nv_cmd_ptr->data_ptr->sms.data[
          sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+1]=0;
      }
    }
    else
    {
      /* No SMS Services Available.  Indicate that in control list */
      nv_cmd_ptr->data_ptr->sms.data[
        sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS]=0; 
      nv_cmd_ptr->data_ptr->sms.data[
        sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+1]=0;
    }
  }
  nv_cmd_ptr->data_ptr->sms.data[
    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+2]=nvruimsms_max_sms; 
  nv_cmd_ptr->data_ptr->sms.data[
    sizeof(ui_sms_nv_list_type)*MAX_NV_MSGS+3]=nvruimsms_max_smsp;
  return status;
} /* nvruimsms_buildlist */


/*===========================================================================

FUNCTION NVRUIMSMS_FREE

DESCRIPTION
  Erases an SMS message or SMSP item

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uim_rpt_status nvruimsms_free
(
  nv_cmd_type *nv_cmd_ptr
)
{
  uim_rpt_status status; /* status of ruim operation */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmd.access_uim.data_ptr = nvruim_ef_buffer;
  cmd.access_uim.offset   = nv_cmd_ptr->data_ptr->sms.address;
  cmd.access_uim.rec_mode = UIM_ABSOLUTE;

  /* Determine if we're reading an SMS message or SMSP item */
  if( cmd.access_uim.offset <= nvruimsms_smsp_offset )
  {
    /* format an empty SMS message */
    nvruim_ef_buffer[0]=0;
    cmd.access_uim.num_bytes = nvruimsms_max_sms;

    cmd.access_uim.item = UIM_CDMA_SMS;
  }
  else
  {
    /* format an empty SMSP entry */
    memset(nvruim_ef_buffer, NVRUIMSMS_SMSP_EMPTY_CHAR, NVRUIM_EF_BUF_SIZE);
    cmd.access_uim.num_bytes = nvruimsms_max_smsp;

    cmd.access_uim.item = UIM_CDMA_SMS_PARAMS;
    cmd.access_uim.offset -= nvruimsms_smsp_offset;
  }

  
  /* Send the command to free the entry
  */
  cmd.access_uim.access = UIM_WRITE_F;
  status=nvruim_access( &cmd );

  return status;

} /* nvruimsms_free */

#endif /* FEATURE_NV_RUIM && FEATURE_RUIM_SMS */
