/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 PMIC DRIVER SERVICES FOR PROC COMM

GENERAL DESCRIPTION
     This file contains functions and variable declarations to support 
  the different voltage regulators inside the Qualcomm Power Manager IC.

INITIALIZATION AND SEQUENCING REQUIREMENTS                            
  None.

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_pcil.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/08   jtn     Move all proc comm functions to pmapp_pcil.c
===========================================================================*/

#ifndef PMAPP_PCIL_H
#define PMAPP_PCIL_H

/**
*  @file  pmapp_pcil.h
   @brief Header file for proc_comm commands hosted in pm. 

   This header file contains the prototype for the function which registers 
   the proc comm commands for power manager.  This file is meant to be internal
   to smem and only used on the modem processor.

   SERVICES: none

      Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
*******************************************************************************/

//------------------------------------------------------------------------------
// Function declarations
//------------------------------------------------------------------------------
/**
   @brief Registers the functions in pm_pcil.c with proc_comm

   Calls smem_proc_comm_register_cmd() for each of the pcil functions
   in pm_pcil.c
   
   @return  TRUE if all functions were sucessfully registered
*/
boolean pm_register_pcil_cmds(void);

#endif /*PM_PCIL_H */
