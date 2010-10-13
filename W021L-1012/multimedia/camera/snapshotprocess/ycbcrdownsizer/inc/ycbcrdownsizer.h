#ifndef YCBCR_DOWNSIZER_H
#define YCBCR_DOWNSIZER_H

/*=============================================================================
FILE:       YCbCr_downsizer.h

SERVICES:   YCBCR DOWNSIZER MODULE INTERFACE

DESCRIPTION:
   This file provides the necessary interfaces for the ycbcr_downsizer. 

LIMITATIONS:
   

INITIALIZATION AND SEQUENCING REQUIREMENTS:

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/ycbcrdownsizer/main/latest/inc/ycbcrdownsizer.h#1 $ $Author: karenp $

=============================================================================*/
/* <EJECT> */
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/10/07   sigu    ARM side changes for integrating the incall downsizer for 6260 target.
08/07/06   sigu    Initial version


=============================================================================*/


/*-----------------------------------------------------------------------------
      Include Files
-----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "ycbcrdownsizerqdsp.h"
#include "camsensor.h"

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*-----------------------------------------------------------------------------
      Type Declarations
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
      Defines for ycbcr_downsizer catagories
-----------------------------------------------------------------------------*/
/*Define the input and output format for ycbcr downsizer*/
#define YDS_H2V1 0x00
#define YDS_H2V2 0x01
/*
** Define the message type for the client callback 
*/
typedef enum
{
  YCBCR_DOWNSIZER_SUCCESS_MSG,  
  YCBCR_DOWNSIZER_NO_FEATURE_MSG,
  YCBCR_DOWNSIZER_INVALID_CMD_MSG,
  YCBCR_DOWNSIZER_ABORT_MSG,
  YCBCR_DOWNSIZER_OUT_OF_MEMORY_MSG,
  YCBCR_DOWNSIZER_FAIL_QDSP_MSG,
  YCBCR_DOWNSIZER_QDSP_RESET
} ycbcr_downsizer_msg_type;

/*
** Define the return type for the client
*/
typedef enum
{
  YCBCR_DOWNSIZER_CMD_ACCEPT_RET,  
  YCBCR_DOWNSIZER_CMD_REJECT_RET
} ycbcr_downsizer_ret_type;

/*
** Define the processing type for the DSP callback 
*/
typedef enum
{
  YCBCR_DOWNSIZER_DOWNLOAD_OP,
  YCBCR_DOWNSIZER_START_OP,
  YCBCR_DOWNSIZER_IDLE_OP,
  YCBCR_DOWNSIZER_PROCESS_ABORT_OP,
  YCBCR_DOWNSIZER_ABORT_OP,
  YCBCR_DOWNSIZER_DONE_OP,
  YCBCR_DOWNSIZER_DISABLE_OP,
  YCBCR_DOWNSIZER_MAX_OP
} ycbcr_downsizer_operation_type;

/*
** Define a ycbcr_downsizer callback msg type
*/
typedef struct
{
   int32             client_id;    /* client id */
   ycbcr_downsizer_msg_type     status;       /* ycbcr_downsizer status */
} ycbcr_downsizer_cbmsg_type;


/*============================================================================
                        ABSTRACT DATA TYPES
============================================================================*/
typedef unsigned int ycbcr_downsizer_image_paramType;

typedef __packed struct ycbcr_downsizer_image_type
{
  ycbcr_downsizer_image_paramType           height                 : 14;
  unsigned int                              reserved1              : 2;
  ycbcr_downsizer_image_paramType           width                  : 14;
  unsigned int                              reserved2              : 2;
} ycbcr_downsizer_image_type;

typedef __packed struct ycbcr_downsizer_lines_of_frag
{
  ycbcr_downsizer_image_paramType           frag1                  : 14;
  unsigned int                              reserved3              : 2;
  ycbcr_downsizer_image_paramType           frag0                  : 14;
  unsigned int                              reserved4              : 2;
} ycbcr_downsizer_lines_of_frag;

typedef __packed struct ycbcr_downsizer_row_increment_type
{
  ycbcr_downsizer_image_paramType           CbCr_increment         : 14;
  unsigned int                              reserved5              : 2;
  ycbcr_downsizer_image_paramType           Y_increment            : 14;
  unsigned int                              reserved6              : 2;
} ycbcr_downsizer_row_increment_type;

typedef __packed struct ycbcr_downsizer_fragments_n_format_type
{
  ycbcr_downsizer_image_paramType           format                 : 4;
  unsigned int                              reserved8              : 12;
  ycbcr_downsizer_image_paramType           num_fragments          : 2;
  unsigned int                              reserved7              : 14;
} ycbcr_downsizer_fragments_n_format_type;

typedef uint32 *ycbcr_downsizer_AddressType;

typedef struct 
{
	ycbcr_downsizer_AddressType address;
} ycbcr_downsizer_address_type;


/*
** QDSP message processing callback function structure
*/
typedef void (* ycbcr_downsizer_cb_type) (ycbcr_downsizer_cbmsg_type msg);

/*
** Define the ycbcr_downsizer data specification
*/
typedef struct 
{
/*Input image configrations */
	ycbcr_downsizer_image_type              in_image ;                      /*Input image width & height configrations*/
	ycbcr_downsizer_row_increment_type      row_in_increment;               /*Input image row increment*/
	ycbcr_downsizer_fragments_n_format_type in_frag_n_format ;              /*Number of fragments & the format for the input image*/
	ycbcr_downsizer_lines_of_frag           in_lines_frag0_1 ;              /*Number of lines in the input fragment 0 & 1*/
	ycbcr_downsizer_lines_of_frag           in_lines_frag2_3 ;              /*Number of lines in the input fragment 2 & 3*/
	ycbcr_downsizer_address_type            in_Yaddress_frag0 ;             /*Input image Y address for fragment 0*/
	ycbcr_downsizer_address_type            in_CbCraddress_frag0;           /*Input image CbCr address for fragment 0*/
	ycbcr_downsizer_address_type            in_Yaddress_frag1 ;             /*Input image Y address for fragment 1*/
	ycbcr_downsizer_address_type            in_CbCraddress_frag1 ;          /*Input image CbCr address for fragment 1*/
	ycbcr_downsizer_address_type            in_Yaddress_frag2 ;             /*Input image Y address for fragment 2*/
	ycbcr_downsizer_address_type            in_CbCraddress_frag2 ;          /*Input image CbCr address for fragment 2*/
	ycbcr_downsizer_address_type            in_Yaddress_frag3;              /*Input image Y address for fragment 3*/
	ycbcr_downsizer_address_type            in_CbCraddress_frag3;           /*Input image CbCr address for fragment 3*/

/*Output image configrations */
	ycbcr_downsizer_image_type              out_image ;                     /*Output image width & height configrations*/
	ycbcr_downsizer_row_increment_type      row_out_increment;              /*Output image row increment*/
	ycbcr_downsizer_fragments_n_format_type out_frag_n_format;              /*Number of fragments & the format for the output image*/
	ycbcr_downsizer_lines_of_frag           out_lines_frag0_1;              /*Number of lines in the output fragment 0 & 1*/
	ycbcr_downsizer_lines_of_frag           out_lines_frag2_3;              /*Number of lines in the output fragment 2 & 3*/
	ycbcr_downsizer_address_type            out_Yaddress_frag0;             /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_CbCraddress_frag0;          /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_Yaddress_frag1;             /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_CbCraddress_frag1;          /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_Yaddress_frag2;             /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_CbCraddress_frag2;          /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_Yaddress_frag3;             /*Output image Y address for fragment 0*/
	ycbcr_downsizer_address_type            out_CbCraddress_frag3;          /*Output image Y address for fragment 0*/
} ycbcr_downsizer_data_type;


/*
** Define the ycbcr_downsizer info specification
*/
typedef struct ycbcr_downsizer_cmd_info_type
{
  int32                       client_id;                                  /* client id */
  ycbcr_downsizer_cb_type     cb_fun;                                     /* call back to application */
  boolean                     ycbcr_downsizer_enable;                     /* enable or disable ycbcr */
  ycbcr_downsizer_data_type   data;                                       /* Data to ycbcr downsizer*/
} ycbcr_downsizer_cmd_info_type;

/*===========================================================================*/
/* FUNCTION DEFINITIONS                                                      */
/*===========================================================================*/
/*=============================================================================

Function: YCBCR_DOWNSIZER_PROCESS_DATA

Description: This function will apply ycbcr_downsizer on data provided.

Return Value: 

  YCBCR_DOWNSIZER_CMD_ACCEPT_RET : ycbcr_downsizer.h command is accepted. 
                       Caller must wait for callback before freeing any memory 
                       location accessible by cmd pointer.
  
  YCBCR_DOWNSIZER_CMD_REJECT_RET : command is NOT accepted, caller is responsible to check
                       this status and perform clean up. NO callback will
                       come from ycbcr_downsizer.
   
  Comments: The caller must not free or modify data located in the input buffer
          until callback from ycbcr_downsizer is received.

  Side Effects: None.

=============================================================================*/

extern ycbcr_downsizer_ret_type ycbcr_downsizer_process_data ( ycbcr_downsizer_cmd_info_type * cmd);

/*=============================================================================

Function: YCBCR_DOWNSIZER_ABORT

Description: This function will try to abort the current ycbcr_downsizer session.

Return Value:

  YCBCR_DOWNSIZER_CMD_ACCEPT_RET : ycbcr_downsizer command is accepted. Caller must wait for callback
                       before freeing any memory location accessible by cmd
                       pointer.
  
  YCBCR_DOWNSIZER_CMD_REJECT_RET : command is NOT accepted, caller is responsible to check
                       this status and perform clean up. NO callback will
                       come from ycbcr_downsizer.
   
Comments: The caller must not free or modify data located in the input buffer
          until callback from ycbcr_downsizer is received.

Side Effects: Attempting to abort ycbcr_downsizer in the middle of the process may corrupt
              the data.

=============================================================================*/

extern ycbcr_downsizer_ret_type ycbcr_downsizer_abort ( void );

#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */

#endif /* YCBCR_DOWNSIZER_H */
