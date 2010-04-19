#ifndef AEEFOTA_H
#define AEEFOTA_H

/*============================================================================
FILE: AEEFOTA.h

SERVICES:  IFOTA interface definition

GENERAL DESCRIPTION: 
   This file provides definitions for FOTA services
   available to application developers.

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
#include "AEE.h"

#include "FOTA.bid"

#define AEEIID_FOTAFILE 0x0102cf00

#define FOTA_AGENT_UPDATE_PACKAGE  1
#define FOTA_AGENT_BITMAPS         2
#define FOTA_FS_UPDATE_PACKAGE     3


#define INHERIT_IFOTA(iname) \
   INHERIT_IQueryInterface(iname); \
   int    (* Open)(iname* me, int openType); \
   int32  (* Read)(iname* me, void *pBuffer, uint32 dwCount); \
   uint32 (* Write)(iname* me, const void *pBuffer, uint32 dwCount); \
   int    (* Commit)(iname *me, void* pvOpenType); \
   int    (* GetStatus)(iname *me, char *pcStatus); \
   int    (* SetStatus)(iname *me, char cStatus)


AEEINTERFACE_DEFINE(IFOTA);

#define IFOTA_AddRef(p)                AEEGETPVTBL(p,IFOTA)->AddRef(p)
#define IFOTA_Release(p)               AEEGETPVTBL(p,IFOTA)->Release(p)
#define IFOTA_QueryInterface(p,i,p2)   AEEGETPVTBL(p,IFOTA)->QueryInterface(p,i,p2)
#define IFOTA_Open(p,type)             AEEGETPVTBL(p,IFOTA)->Open(p,type)
#define IFOTA_Read(p,b,c)              AEEGETPVTBL(p,IFOTA)->Read(p,b,c)
#define IFOTA_Write(p,b,c)             AEEGETPVTBL(p,IFOTA)->Write(p,b,c)
#define IFOTA_Commit(p,b)              AEEGETPVTBL(p,IFOTA)->Commit(p,b)
#define IFOTA_GetStatus(p,type)        AEEGETPVTBL(p,IFOTA)->GetStatus(p,type)
#define IFOTA_SetStatus(p,type)        AEEGETPVTBL(p,IFOTA)->SetStatus(p,type)

/*=================================================================================
   INTERFACE DOCUMENTATION
===================================================================================

IFOTA Interface

IFOTA is an interface that allows the ability for OEMs to read/write/commit 
data to the FOTA partition. This extension is meant to help OEMs design 
their own FOTA (Firmware Over The Air) mechanism using BREW. 

This extension, by itself, does not provide a full-fledged FOTA support in BREW.

===================================================================================

IFOTA_AddRef()

Description:
    This function is inherited from IQI_AddRef(). 
	
See Also:
    IQueryInterface
    IFOTA_Release()

=================================================================================

IFOTA_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

See Also:
    IQueryInterface

=================================================================================

IFOTA_Release()

Description:
    This function is inherited from IQI_Release(). 
	
See Also:
    IQueryInterface
    IFOTA_AddRef()
=================================================================================
IFOTA_Open()

Description:
   This function is similar to IFILE_Open. 
   Opens the requested type of FOTA partition. It currently supports the open 
   types of FOTA_AGENT_UPDATE_PACKAGE, FOTA_AGENT_BITMAPS, 
   FOTA_FS_UPDATE_PACKAGE


Prototype:
   int  IFOTA_Open(IFOTA* me, int openType)


Parameters:
   me: [in] Pointer to the IFOTA object
   openType: [in] type of open 
   
Return Value:

   SUCCESS: on success

   Other BREW error codes on failures


See Also:
    

=================================================================================

IFOTA_Read()

Description:
    This function is similar to IFILE_Read. Reads the requested number of bytes

Prototype:
   int32  IFOTA_Read(IFOTA* me, void *pBuffer, uint32 dwCount)

Parameters:
   me: [in] Pointer to IFOTA object
   pBuffer: [out] Buffer to read into
   dwCount: [in]  size of the read buffer

Return Value:

   Number of bytes read : If successful

   0 (zero) : If unsuccessful


See Also:
    

=================================================================================
IFOTA_Write()

Description:
    This function is similar to IFILE_Write.  Writes the given number of bytes from 
    the given buffer to FOTA partition

Prototype:
    uint32 IFOTA_Write(IFOTA* me, const void *pBuffer, uint32 dwCount)

Parameters:
   me: [in] Pointer to the IFOTA object
   pBuffer: [in] Buffer to write from
   dwCount: [in]  number of bytes to write

Return Value:

   Number of bytes written : If successful

   0 (zero) : If unsuccessful


See Also:
    

=================================================================================

IFOTA_Commit()

Description:
    This function commits the previously written data to the FOTA partition.  
    (For example, Call it after writing the update package, 
    so that the FOTA update can be performed on the next boot.)

Prototype:
   int IFOTA_Commit(IFOTA *me, void* pvOpenType)

Parameters:
   me: [in] Pointer to the IFOTA object
   pvOpenType: [in] corresponds to the openType in IFOTA_Open 

Return Value:

   SUCCESS : On success

   Other BREW error codes


See Also:
=================================================================================
IFOTA_GetStatus()

Description:
    Get the FOTA status information

Prototype:
    int IFOTA_GetStatus(IFOTA* me, char *pcStatus)

Parameters:
   me: [in] Pointer to the IFOTA object
   pcStatus: [out] where to put the status

Return Value:

   SUCCESS: when status is updated

   BREW error code otherwise


See Also:
    
=================================================================================
IFOTA_SetStatus()

Description:
    Set the FOTA status information

Prototype:
    int IFOTA_SetStatus(IFOTA* me, char cStatus)

Parameters:
   me: [in] Pointer to the IFOTA object
   cStatus: [in] status to be set

Return Value:

   SUCCESS: when status is updated

   BREW error code otherwise


See Also:
    
    
=================================================================================*/



#endif //AEEFOTA_H
