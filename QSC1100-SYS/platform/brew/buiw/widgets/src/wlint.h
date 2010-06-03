/*
  ========================================================================

  FILE:                 wlint.h
  
  SERVICES:             lint utilities

  GENERAL DESCRIPTION:  Macros, inlines, constants and other things to
                        help make widgets code lint compliant.

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#ifndef _WLINT_H_
#define _WLINT_H_

// macro to make lint shut up about unused parameters
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

// clean casting macro
#define CAST(t, exp)	((t)(void*)(exp))

#endif //_WLINT_H_