#ifndef _OI_DEBUGCONTROL_H
#define _OI_DEBUGCONTROL_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_debugcontrol.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 *
 * The debugging level for each module is controlled through this header file.
 *
 * Additional information is available in the @ref debugging_docpage section.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_modules.h"
#include "oi_stddefs.h"
#include "oi_status.h"

/**
 * This is the maximum debug level. At this level, all debug output is enabled (none is
 * suppressed).
 */
#define OI_DEBUG_LEVEL_MAX OI_UINT8_MAX

/**
 * This is the minimum debug level. At this level, all debug output is disabled.
 */
#define OI_DEBUG_LEVEL_OFF  0


/** 
 * This function sets the debug output level for a given module.
 *
 * @param module the module for which to set the debugging level
 * @param level  the debug output level for the specified module
 */
OI_STATUS OI_SetDebugLevel(OI_MODULE module,
                           OI_UINT8 level);

/** 
 * This function sets the debug output level for a given module.
 *
 * @param name   the string name for the module on which to set the debug output level
 * @param level  the debug output level for the specified module
 */
OI_STATUS OI_SetDebugLevelByName(OI_CHAR *name,
                                 OI_UINT8 level);


/**
 * This function gets the current debug level for the specific module, by module number. 
 *
 * @param module number representing the module for which to set debug level
 * @returns level, current debug level for the specified module
 */
OI_UINT8 OI_GetDebugLevel(OI_MODULE module);

#endif /* _OI_DEBUGCONTROL_H */
