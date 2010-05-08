#ifndef _OI_ASSERT_H
#define _OI_ASSERT_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_assert.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
  This file provides macros and functions for compile-time and run-time assertions.
  
  When the OI_DEBUG preprocessor value is defined, the macro OI_ASSERT is compiled into
  the program, providing for a runtime assertion failure check. 
  C_ASSERT is a macro that can be used to perform compile time checks.
*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifdef OI_DEBUG
#error code not present
#else


#define OI_ASSERT(condition) 
#define OI_ASSERT_FAIL(msg) 
 
#endif


/**
   C_ASSERT() can be used to perform many compile-time assertions: type sizes, field offsets, etc.
   An assertion failure results in compile time error C2118: negative subscript.
   Unfortunately, this elegant macro doesn't work with GCC, so it's all commented out
   for now. Perhaps later..... 
*/

#ifndef C_ASSERT
// #define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
// #define C_ASSERT(e)
#endif


/*****************************************************************************/
#endif /* _OI_ASSERT_H */

