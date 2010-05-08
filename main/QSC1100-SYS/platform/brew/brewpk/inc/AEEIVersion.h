#ifndef __AEEIVERSION_H__
#define __AEEIVERSION_H__

/*==============================================================================
 *
 * FILE:                 AEEIVersion.h
 *     
 * SERVICES:             IVersion
 *
 * GENERAL DESCRIPTION:  Defines an interface that provides applications a way 
 *                       to dynamically determine the version of the installed 
 *                       components
 *  
 *             Copyright © 2007 QUALCOMM Incorporated 
 *                   All Rights Reserved.
 *                 QUALCOMM Proprietary/GTDR
 *   
 *=============================================================================*/

/*------------------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------------------*/
#include "AEEIQI.h"


/*------------------------------------------------------------------------------
      Definitions and Constants
-------------------------------------------------------------------------------*/

// IVersion Interface ID
#define AEEIID_IVersion      0x0104f5c0

// Version Info IDs
#define AEEVERSID_PRODUCTNAME   0x01052a7c
#define AEEVERSID_VENDORNAME    0x01052a7d 

typedef struct AEEVersionDesc AEEVersionDesc;   
   
struct AEEVersionDesc 
{  
   int  nMajorVer;
   int  nMinorVer;
   int  nSubVer;
   int  nBuildNum;
};

// IVersion Interface
#define INHERIT_IVersion(iname) \
   INHERIT_IQI(iname); \
   int   (*GetVersion)       (iname *po, AEEVersionDesc *pvd, int nVersionDescSize); \
   int   (*GetInfoString)    (iname *po, int nInfoId, char *pszBuf, int nBufLen, int *pnBufLenReq)

AEEINTERFACE_DEFINE(IVersion);


// AddRef
static __inline uint32 IVersion_AddRef(IVersion *po) {
   return AEEGETPVTBL(po, IVersion)->AddRef(po);
}

// Release
static __inline uint32 IVersion_Release(IVersion *po) {
   return AEEGETPVTBL(po, IVersion)->Release(po);
}

// QueryInterface
static __inline int IVersion_QueryInterface(IVersion *po, AEECLSID clsid, void **ppo) {
   return AEEGETPVTBL(po, IVersion)->QueryInterface(po, clsid, ppo);
}

// Get Version 
static __inline int IVersion_GetVersion(IVersion *po, AEEVersionDesc *pvd, int nVersionDescSize) {
   return AEEGETPVTBL(po, IVersion)->GetVersion(po, pvd, nVersionDescSize);
}

// Get Information 
static __inline int IVersion_GetInfoString(IVersion *po, int nInfoId, char *pszBuf, int nBufLen, int *pnBufLenReq) {
    return AEEGETPVTBL(po, IVersion)->GetInfoString(po, nInfoId, pszBuf, nBufLen, pnBufLenReq);
}

// Get Product Name
static __inline int IVersion_GetProductName(IVersion *po, char *pszProdName, int nProdNameLen, int *pnProdNameLenReq) {
    return AEEGETPVTBL(po, IVersion)->GetInfoString(po, AEEVERSID_PRODUCTNAME, pszProdName, nProdNameLen, pnProdNameLenReq);
}

// Get Vendor Name
static __inline int IVersion_GetVendorName(IVersion *po, char *pszVendorName, int nVendorNameLen, int *pnVendorNameLenReq) {
    return AEEGETPVTBL(po, IVersion)->GetInfoString(po, AEEVERSID_VENDORNAME, pszVendorName, nVendorNameLen, pnVendorNameLenReq);
}

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
IVersion Interface and Class IDs

Description: 
   IVersion provides applications a dynamic method of retrieving version information
   from installed components.

Definition:

Identifier                       Definition
----------                       ----------------------------------------------
AEEIID_IVersion:                 Identifies the IVersion interface.

Comments:
   None

See Also:
   None
=============================================================================
AEEVersionDesc

Description:
   This data type defines a data structure used to access the version information
   from the installed components. These attributes indicate major, minor, sub version
   and build number.
   
Definition:
   typedef struct {  
      int  nMajorVer;
      int  nMinorVer;
      int  nSubVer;
      int  nBuildNum;
   } AEEVersionDesc;

Members:
===pre>
   nMajorVer:  Integer value specifying major version of the component.
   nMinorVer:  Integer value specifying minor version of the component.
   nSubVer:    Integer value specifying sub version of the component. 
   nBuildNum:  Integer value specifying build number of the component.
   
===/pre>
Comments:
   None

See Also:
   IVersion_GetVersion()
   
================================================================================
 INTERFACES DOCUMENTATION
================================================================================

IVersion Interface

Description: 
   IVersion provides applications a dynamic method of retrieving the version 
   information from the installed components. 
   
================================================================================

IVersion_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

IVersion_Release()

This function is inherited from IQI_Release().

================================================================================

IVersion_QueryInterface()

This function is inherited from IQI_QueryInterface().

================================================================================
IVersion_GetVersion()

Description:
   This method retrieves the version information of the installed component. 

Prototype:
   int IVersion_GetVersion(IVersion *po, 
                           AEEVersionDesc *pvd,
                           int nVersionDescSize);

Parameters:
===pre>
   po                [in]  A pointer to the IVersion object.
   pvd               [out] On return, pvd contains detail version information 
                           of the component. 
   nVersionDescSize  [in] size of AEEVersionDesc structure.
===/pre>

Return Value:
   SUCCESS on success.
   Error code otherwise.

Comments:
  None

Side Effects:
   None

See Also:
   AEEVersionDesc

================================================================================
IVersion_GetInfoString()

Description:
   This method retrieves information like the product name and the vendor name from
   the installed component.

Prototype:
   int IVersion_GetInfoString(IVersion *po, 
                              int nInfoId, 
                              char *pszBuf, 
                              int nBufLen, 
                              int *pnBufLenReq);
Parameters:
===pre>
   po               [in]  A pointer to the IVersion object.
   nInfoId          [in]  Identifies info string to be retrieved. It is specified as 
                          one of the following constants.
            AEEVERSID_PRODUCTNAME    - Use this to retrieve product name.
            AEEVERSID_VENDORNAME     - Use this to retrieve vendor name.

   pszBuf          [out] On return, this will contain requested info string. The string 
                         will be NULL terminated. pszBuf is a pre-allocated buffer to store 
                         info string. 
   nBufLen         [in]  The total size (in bytes) of the pszBuf buffer.
   pnBufLenReq     [out] On return, this will contain total size (in bytes) of the 
                         buffer required to hold requested info string including 
                         NULL termination character.
   
===/pre>

Return Value:
   SUCCESS on success.
   Error code otherwise.

Comments:
   This function should be called once with nBufLen set to zero to determine
   the required amount of space to buffer the requested info.  The caller 
   should then allocate the required amount of memory and call this function
   again to retrieve the value.
   If size of buffer is less than length of info string, then the function will
   fill the buffer with the truncated info string and will NULL terminate the string.

Side Effects: 
   None
See Also:
   None

See Also:
   None
================================================================================
IVersion_GetProductName()

Description:
   This method retrieves the product name of the installed component.

Prototype:
   int IVersion_GetProductName(IVersion *po, 
                               char *pszProdName, 
                               int nProdNameLen, 
                               int *pnProdNameLenReq);
Parameters:
===pre>
   po                [in]  A pointer to the IVersion object.
   pszProdName       [out] On return, this will contain product name. The string 
                           will be NULL terminated. pszProdName is a pre-allocated 
                           buffer to store product name.  
   nProdNameLen      [in]  The total size (in bytes) of the pszProdName buffer.
   pnProdNameLenReq  [out] On return, this will contain total size (in bytes) of the 
                           buffer required to hold product name including the NULL 
                           termination character.
                           
===/pre>

Return Value:
   SUCCESS on success.
   Error code otherwise.

Comments:
   This function should be called once with nProdNameLen set to zero to determine
   the required amount of space to buffer the product name.  The caller 
   should then allocate the required amount of memory and call this function
   again to retrieve the value.
   If size of buffer is less than length of product name, then the function will
   fill the buffer with the truncated product name and will NULL terminate the string.

Side Effects: 
   None
See Also:
   None

See Also:
   None


================================================================================
IVersion_GetVendorName()

Description:
   This method retrieves the vendor name of the installed component.

Prototype:
   int IVersion_GetVendorName(IVersion *po, 
                              char *pszVendorName, 
                              int nVendorNameLen, 
                              int *pnVendorNameLenReq)
Parameters:
===pre>
   po                   [in]  A pointer to the IVersion object.
   pszVendorName        [in]  On return, this will contain vendor name. The string 
                              will be NULL terminated. pszVendorName is a pre-allocated 
                              buffer to store vendor name.  
   nVendorNameLen       [in]  The total size (in bytes) of the pszVendorName buffer.
   pnVendorNameLenReq   [out] On return, this will contain total size (in bytes) of the 
                              buffer required to hold vendor name including the NULL 
                              termination character.
   
===/pre>

Return Value:
   SUCCESS on success.
   Error code otherwise.

Comments:
   This function should be called once with nVendorNameLen set to zero to determine
   the required amount of space to buffer the vendor name.  The caller 
   should then allocate the required amount of memory and call this function
   again to retrieve the value.
   If size of buffer is less than length of vendor name, then the function will
   fill the buffer with the truncated vendor name and will NULL terminate the string.

Side Effects: 
   None
See Also:
   None

See Also:
   None



==============================================================================*/

#endif /* __AEEIVERSION_H__ */
