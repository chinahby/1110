#ifndef HEADER_MF_DRM_CRYPTOTYPES_H
#define HEADER_MF_DRM_CRYPTOTYPES_H

// ============================================================================
// FILE: mf_drm_cryptotypes.h
// 
// DESCRIPTION:
//    Typedefs and shared basic data structures required for the drm csw lib.
// 
//               (c) COPYRIGHT 2004 QUALCOMM Incorporated.
//                          All Rights Reserved.
//                       QUALCOMM Proprietary/GTDR
// ============================================================================

/* =======================================================================
                           Perforce Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/src/mf_drm_cryptotypes.h#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

// ----------------------------------------------------------------------------
// Include files 
// ----------------------------------------------------------------------------


#ifdef __cplusplus
extern "C" {
#endif
// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------


#define MF_CRYPTOTYPES_CIPHER_BLOCK_SIZE 16


typedef unsigned char  CRYPTOTYPES_UINT8;
typedef   signed char  CRYPTOTYPES_INT8;
typedef unsigned short CRYPTOTYPES_UINT16;
typedef   signed short CRYPTOTYPES_INT16;
typedef unsigned int   CRYPTOTYPES_UINT32;
typedef   signed int   CRYPTOTYPES_INT32;

// ----------------------------------------------------------------------------
// Type Declarations
// ----------------------------------------------------------------------------
    typedef struct
    {
        CRYPTOTYPES_UINT8 key[MF_CRYPTOTYPES_CIPHER_BLOCK_SIZE];
    } MFProgramKey;


    typedef struct
    {
        CRYPTOTYPES_UINT8 data[MF_CRYPTOTYPES_CIPHER_BLOCK_SIZE];
    } MFIvData;

// ----------------------------------------------------------------------------
// Global Constant Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Global Data Declarations
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // HEADER_MF_DRM_CRYPTOTYPES_H 



