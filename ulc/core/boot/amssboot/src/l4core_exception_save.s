;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
;                           R E X  A R M   C O R E 
;
; GENERAL DESCRIPTION
;   Capture a "core" dump
;
; EXTERNALIZED FUNCTIONS
;   l4_jettison_core
;
; Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
;*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

;============================================================================
;
;                      EDIT HISTORY FOR MODULE
;
; $Header: //depot/asic/msm6800/drivers/boot/2.1/l4core_exception_save.s#4 $
;
; when      who    what, where, why
; --------  -----  -----------------------------------------------------------
;============================================================================

#include "customer.h"

#ifdef FEATURE_L4_STORE_CRASH_INFO_TO_EFS
#error code not present
#endif

