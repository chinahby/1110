#ifndef IXDEVICEID_H
#define IXDEVICEID_H

/*===========================================================================

                     S E C U R I T Y    S E R V I C E S
   
                           H E A D E R  F I L E

FILE:  IxDeviceID.h

DESCRIPTION
  The IxDeviceID class provides methods to get/set various device ids.

EXTERNALIZED FUNCTIONS : 

   GetHW_ID

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxDeviceID.h#3 $
  $DateTime: 2008/02/06 10:40:29 $
  $Author: glennf $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/07    ws     Initial Revision
===========================================================================*/


/*===========================================================================
                           
                                INCLUDE FILES

============================================================================*/

#include "comdef.h"
#include "IxErrno.h"
#include "stdlib.h"

#define IXDEVICEID_HWID_MAX_LEN               16

// Definitions for "C" files
#ifndef __cplusplus

typedef struct
{
   uint32 dummy;
} IxDeviceID;

IxErrnoType     Call_IxDeviceID_GetHW_ID (char **ppszHW_ID );


#else // __cplusplus
// Definitions for "C++" files

/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

class IxDeviceID
{

public:

   /*------------------------------------------------------------------------
                                PUBLIC TYPES
   ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
                               PUBLIC METHODS
  ------------------------------------------------------------------------*/

  /*****
   *  Method Name: IxDeviceID ()
   *
   *  Description: Default Constructor
   *
   *  Dependencies: None
   *
   *  Parameters:  None
   *
   *  Return Value: None
   *
   *****/
   IxDeviceID();

  // Destructor
  ~IxDeviceID();

  /*****
   *  Method Name: Init ()
   *
   *  Description: Initialize the Device ID
   *
   *  Dependencies: None
   *
   *  Parameters:  NULL terminated string representing the HW ID.
   *
   *  Return Value: boolean
   *
   *****/
   static IxErrnoType Init( char *HwId );

  /*****
   *  Method Name: Init ()
   *
   *  Description: Initialize the Device ID
   *
   *  Dependencies: None
   *
   *  Parameters:  None... IxDeviceID will figure it out.
   *
   *  Return Value: boolean
   *
   *****/
   static IxErrnoType Init( );

  /*****
   *  Method Name: GetHW_ID ()
   *  
   *  Description: This method retrieves the HW_ID of the device.
   *    
   *   
   *  Dependencies: None
   *
   *  Parameters: 
   *    ppszHW_ID - pointer to NULL terminated string representing HW ID. The 
   *    
   *  Comments:
   *    The memory for pszHW_ID belongs to the object and will remain valid for
   *    the lifetime of the object. The caller must make a copy if it is required
   *    after the object is deleted.
   *    IT IS INVALID FOR THE CALLER TO ATTEMPT TO FREE THIS MEMORY.
   * 
   *  Return Value: 
   *    IxErrnoType  - E_SUCCESS or error number in case of failure
   *  
   *****/
   static IxErrnoType GetHW_ID ( char **ppszHW_ID );

private:
   /*------------------------------------------------------------------------
                                PRIVATE TYPES
   ------------------------------------------------------------------------*/

   /*------------------------------------------------------------------------
                                PRIVATE DATA
   ------------------------------------------------------------------------*/
   static char*                      __esn;
   static boolean                    __initialized;

  /*------------------------------------------------------------------------
                               PRIVATE METHODS
   ------------------------------------------------------------------------*/
};

#endif // #ifdef _cplusplus

#endif
