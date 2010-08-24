/*=============================================================================

FILE: QcamDlgRawPreview.c

SERVICES: QCamera Dialog

GENERAL DESCRIPTION: Raw Preview Setting Dialog

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/camera/apps/qcamera/main/latest/src/QcamDlgRawPreview.c#1 $
$DateTime: 2008/12/18 21:50:16 $
$Author: kgelli $
$Change: 808978 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/16/08   KC      Created QcamDlgRawPreview File
=============================================================================*/
//Deal with lint problems in BREW headers
//lint -emacro(740,SPRINTF)
//lint -emacro(740,STRTOWSTR)
//lint -emacro(740,STRLEN)
//lint -emacro(740,WSTRLEN)
//lint -emacro(740,WSTRTOSTR)
//lint -emacro(740,ATOI)
//lint -emacro(740,IDIALOG_GetControl)

//lint -emacro(611,SPRINTF)
//lint -emacro(611,STRTOWSTR)
//lint -emacro(611,STRLEN)
//lint -emacro(611,WSTRLEN)
//lint -emacro(611,WSTRTOSTR)
//lint -emacro(611,ATOI)

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "comdef.h"
#endif

#if defined (FEATURE_APP_QCAMERA) && defined (FEATURE_CAMERA_RAW_PREVIEW_CAPTURE)
#error code not present
#endif //FEATURE_APP_QCAMERA
