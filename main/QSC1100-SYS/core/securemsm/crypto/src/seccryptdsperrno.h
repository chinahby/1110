#ifndef SECCRYPTDSPERRNO_H
#define SECCRYPTDSPERRNO_H
//================================================================
//
//          S E C C R Y P T D S P   E R R O R   C O D E S
//
// DESCRIPTION
//   This is the header file defining error codes returned by
//   qDSP cryptographis routines. These values are shared 
//   between ARM and QDSP.
//   The definitions are from the Security Services aDSP ISOD document.
//
//   NOTE: The error codes have 10 bits available (0x0001 to 0x1FF) .
//
// EXTERNALIZED FUNCTIONS
//   None
//
// INITIALIZATION AND SEQUENCING REQUIREMENTS
//   None
//
// COPYRIGHT NOTICE
//   Copyright (c) 2003 by QUALCOMM, Inc.  All rights reserved.   
//   All data and information contained in or disclosed by this   
//   document is confidental and proprietary information of       
//   QUALCOMM, Inc and all rights therein are expressly reserved. 
//   By accepting this material the recipient agrees that this    
//   material and the information contained therein is held in    
//   confidence and in trust and will not be used, copied,        
//   reproduced in whole or in part, nor its contents revealed in 
//   any manner to others without the express written permission  
//   of QUALCOMM, Inc.                                            
//================================================================
//----------------------------------------------------------------
// FILE NAME:     seccryptdsperrno.h  
// TYPE:          Header file for DSP error return values.
// COMPATABILITY: Compatable with ASCII C and QDSP4 Assembly  
// DESCRIPTION:   Contains ONLY #defines and comments acceptable  
//                to the ARM compiler and Qdsp4 assembler.  
// PROGRAMMER:    Matthew von Damm                                
//----------------------------------------------------------------

//
//================================================================
//
//                      EDIT HISTORY FOR FILE
// $PVCSPath: L:/src/asw/COMMON/vcs/seccryptdsperrno.h_v   1.0   30 Aug 2003 20:19:46   omichael  $
// $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccryptdsperrno.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $
//
// when       who     what, where, why
// --------   ---     --------------------------------------------
// 08/28/03   mvd     Created module.
//
//================================================================

//
//================================================================
//                     Success Code
//================================================================

#define SECCRYPTDSP_SUCCESS                                  0x001

//
//================================================================
//                      Error Codes
//================================================================

// FATAL ERROR The Command Struct addresses don't match          
// the context DMEed in doesn't match the DMA flags.             
#define SECCRYPTDSP_ERROR_CMD_STRUCT_ADDR_DONT_MATCH         0x102

// The DSM items were not formatted as mod the DSP buffer size   
#define SECCRYPTDSP_ERROR_DSM_ITEMS_NOT_FORMATTED_CORRECTLY  0x103

#define SECCRYPTDSP_ERROR_DSM_WRITE_INTERNAL_ERROR           0x104
// Internal error in DSM outupt (write).                         
#define SECCRYPTDSP_ERROR_DSM_WRITE_INTERNAL_ERROR2          0x105
#define SECCRYPTDSP_ERROR_DSM_WRITE_CHAIN_TOO_SMALL          0x106
#define SECCRYPTDSP_ERROR_DSM_CHAIN_EXCEEDS_CMD_MAX          0x107

// FATAL ERROR, The DMAed CMD sem is a negative value,           
#define SECCRYPTDSP_ERROR_CMD_SEM_IS_NEGATIVE                0x108

// FATAL ERROR, The DMAed CMD sem is > the greatest cmd value    
#define SECCRYPTDSP_ERROR_CMD_SEM_IS_OUT_OF_RANGE            0x109

// FATAL ERROR, The hash continue/iterate command was given,     
// but the input num bytes was not mod the block size!!          
#define SECCRYPTDSP_ERROR_HASH_NUM_BYTES_NOT_MOD_BLOCK_SIZE  0x10A

// Means that the format of the pointer to Cmd Struct pointer    
// is not on an even 32 bit boundary like it should be.          
// (needed because of the transfer into memC)                    
#define SECCRYPTDSP_ERROR_PTR_PTR_ADDR_FORMAT_WRONG          0x10B
#define SECCRYPTDSP_ERROR_CMD_STRUCT_ADDR_FORMAT_WRONG       0x10C

// returned when a non-supported modmath command type is seen    
#define SECCRYPTDSP_ERROR_MM_CMD_SEM_IS_NOT_SUPPORTED        0x10D

//
//================================================================
//                    Warning Codes
//================================================================

// Couldn't find context to delete from cache           
#define SECCRYPTDSP_WARNING_CACHE_CTRL_COULD_NOT_FIND_CONTEXT  0x181


#endif /* SECCRYPTDSPERRNO_H */



