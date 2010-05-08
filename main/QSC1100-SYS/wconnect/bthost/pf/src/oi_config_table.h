#ifndef _OI_CONFIG_TABLE_H
#define _OI_CONFIG_TABLE_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_config_table.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
 * This file provides the interface to the configuration table.
 * 
 * Associated with each module defined in oi_modules.h is a 
 * configuration structure type definition.
 *     
 *  Naming conventions:
 *  - Configuration type definition is OI_CONFIG_<module> 
 *    (e.g., OI_CONFIG_MEMMGR, OI_CONFIG_L2CAP).
 *  - Default configuration structure name is oi_default_config_<module>
 *    (e.g., oi_default_config_MEMMGR, oi_default_config_L2CAP)
 * 
 *   
 *  
 * Do not call the function OI_ConfigTable_Init() unluess you are sure that you
 * know what you are doing. It should only be called as part of a global, reset/restart 
 * process. Initialization sets the all configuration pointers to their default values.
 * This function should be called before initializing any profiles or core protocol stack 
 * and support module components. This function is defined in oi_bt_stack_init.h with the 
 * other stack initialization prototypes.
 * 
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_modules.h"
                            
extern void* OI_ConfigTable_GetConfig(OI_MODULE module) ;
extern void  OI_ConfigTable_SetConfig(void *configPtr, OI_MODULE module) ;

/**
 *   Configuration table access:
 *
 *    This macro returns a pointer to the configuration structure for the module.
 *    Callers must allow for the possibility that the pointer is NULL.
 *    
 *    Specify module name without the OI_MODULE_ prefix. For example, call
 *    OI_CONFIG_TABLE_GET (DEVMGR), not OI_CONFIG_TABLE_GET (OI_MODULE_DEVMGR).
 *        
 *    The result is a pointer to module's configuration structure
 */
#define OI_CONFIG_TABLE_GET(module)              \
        ((OI_CONFIG_##module*)(OI_ConfigTable_GetConfig(OI_MODULE_##module)))
        
/**
 * This macro is provided primarily to enable backward compatibility with applications
 * written with previous versions of the BLUEmagic 3.0 APIs. Normally, configuration 
 * structures are compiled into the application by defining a the variable 
 * oi_default_config_<module>. 
 * 
 * This macro updates the configuration table entry for the specified module with the specified 
 * pointer. The caller must guarantee the integrity of both the pointer and the data being pointed 
 * to. The configuration structure may not change in any way while the module is 'in use'. 
 * If you cannot be sure when a module is 'in use' or not, do not change the configuration.
 * 
 *    Specify module name without the OI_MODULE_ prefix. For example, call
 *    OI_CONFIG_TABLE_GET (DEVMGR), not OI_CONFIG_TABLE_GET (OI_MODULE_DEVMGR).
 *        
 * 
 * 
 * Usage notes:
 * To set the configuration for a module, you should define a static const structure of the 
 * appropriate type and then use OI_CONFIG_TABLE_PUT() to update the configuration table
 * with your structure.
 * 
 * The configuration table retains a pointer to your structure, so your structure must persist
 * and the contents of your structure may not be changed after it has been put into the table.    
 * 
 * If should you wish to change one or more individual elements in a structure while retaining the
 * default values for other elements, you should
 * 
 *     - 1. use OI_CONFIG_TABLE_GET() to get a reference to the current configuration
 *     - 2. copy the current configuration into a local, static structure
 *     - 3. modify desired elements in the local structure
 *     - 4. use OI_CONFIG_TABLE_PUT() to put a reference to your local structure into the config table. 
 *  
 */
     
#define OI_CONFIG_TABLE_PUT(configPtr, module)   \
        (OI_ConfigTable_SetConfig(configPtr, OI_MODULE_##module))

/** Validate the parameters in the configuration table. */
void OI_ConfigTable_Validate(void);

/*****************************************************************************/
#endif /* _OI_CONFIG_TABLE_H */

