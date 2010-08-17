/*======================================================
FILE:  AEE_OEMDownload.h

SERVICES:  AEE OEM download interface

GENERAL DESCRIPTION:

Download related definitions for the AEE/OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_DOWNLOAD_H)
#define AEE_OEM_DOWNLOAD_H  1

#include "AEEDownload.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// OEM Download Verification
//
// This function is called before each download.  The OEM must fire the callback before the IDOWNLOAD_Acquire
// operation will be completed.  The OEM can use the IDownload instance class instance and the item/price IDs
// to query information about the item if desired.  In addition, the IDownload pointer can be used to
// set the HTTP headers sent to the ADS.  This is performed via the IDOWNLOAD_SetHeaders method.
//
// pUser - User data to be passed to the callback.
// iID - Item ID
// iPrice - Item Price Handle
// pItem - Pointer to information structure for the item
// nErr - Error code.  If non-zero, the download will abort and this error will be reported.
//

extern int                 AEE_AutoInstall(DLITEMID id,DLPRICEID idPrice);
typedef void (*PFNCHECKDLCB)(void * pUser, DLITEMID iID,DLPRICEID iPrice,int nErr);

extern void             OEM_AuthorizeDownload(IDownload * pd, DLITEMID iID, DLPRICEID iPrice,DLItem * pItem,PFNCHECKDLCB pfn, void * pUser);

#if defined(__cplusplus)
}
#endif


/*==================================================================
   INTERFACE  DOCUMENTATION
=======================================================================
AEE_OEMDownload Interface
=======================================================================

Function: AEE_AutoInstall()

Description:

 This function allows the OEM to automatically install an item on the device.
 The function performs the following tasks:
 - Scans the device to see if the specified item is already installed.
 - If the specified item is not installed, displays a "Configuring Apps..." pop-up window.
 - Queries the ADS for information about the item and downloads it.
 - Runs the first application listed for the item.

Prototype:
   int AEE_AutoInstall(DLITEMID id, DLPRICEID idPrice);

Parameters:

  id:  ADS/BDS item identifier
  idPrice: ADS/BDS price handle. If 0 (zero), uses subscription or purchase handle.

Return Value:

  EALREADYLOADED : The specified application is already installed.
  SUCCESS : The application has been run to download the selected item.

Comments:
      None
Side Effects:
      None
See Also:
      None

==================================================================
Function: OEM_AuthorizeDownload()

Description:

 This function is called before each download.  The OEM must fire the callback before the IDOWNLOAD_Acquire().
 operation will be completed.  The OEM can use the IDownload instance class instance and the item/price IDs
 to query information about the item if desired.  In addition, the IDownload pointer can be used to
 set the HTTP headers sent to the ADS.  This is performed via the IDOWNLOAD_SetHeaders() method.

 pUser - user data to be passed to the callback.
 iID - item ID
 iPrice - item price handle
 pItem - pointer to information structure for the item
 nErr - error code.  If nonzero, the download will abort, and this error will be reported.

Prototype:
   void OEM_AuthorizeDownload(IDownload * pd, DLITEMID iID, PFNCHECKDLCB pfn, void * pUser);

Parameters:
 iID:
 pfn:
 pUser
Return Value:  
 None
Comments:     
 None
Side Effects:  
 None
See Also:      
 None

==================================================================*/
#endif // !defined(AEE_OEM_DOWNLOAD_H)
