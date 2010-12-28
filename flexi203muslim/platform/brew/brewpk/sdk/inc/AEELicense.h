#ifndef AEELICENSE_H
#define AEELICENSE_H
/*======================================================
FILE:  AEELicense.h

SERVICES:  AEE ILicense Interface

GENERAL DESCRIPTION:

  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEE.h"
#include "AEEShell.h"

// Basis types that can be selected.

#define BV_UNLIMITED (0xffffffff)   //(dwValue) never expires

enum {
   LT_NONE,             // There is no expiration on this module
   LT_USES,             // Usage counter
   LT_DATE,             // Expiration date
   LT_DAYS,             // Expiration date
   LT_MINUTES_OF_USE,   // Number of minutes remaining
   LT_MAX
};
#ifdef WIN32
typedef unsigned AEELicenseType;
#else
typedef int8 AEELicenseType;
#endif

enum {
   PT_NONE,
   PT_DEMO,
   PT_PURCHASE,
   PT_SUBSCRIPTION,
   PT_UPGRADE,
   PT_AUTOINSTALL,
   PT_MAX
};
#ifdef WIN32
typedef unsigned AEEPriceType;
#else
typedef int8 AEEPriceType;
#endif

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif

typedef PACKED struct  _AEELicenseInfo
{
#if defined(AEE_SIMULATOR)
   byte              pt;
   byte              lt;
#else
   AEEPriceType      pt;
   AEELicenseType    lt;
#endif
   uint32            dwExpire;
} GCCPACKED AEELicenseInfo;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

typedef struct _ILicense ILicense;

QINTERFACE(ILicense)
{
   DECLARE_IBASE(ILicense)

   boolean           (*IsExpired)(ILicense * po);
   AEELicenseType    (*GetInfo)(ILicense * po, uint32 * pdwExpire);
   int               (*SetUsesRemaining)(ILicense * po, uint32 dwCount);
   AEEPriceType      (*GetPurchaseInfo)(ILicense * po, AEELicenseType * plt, uint32 * pdwExpire,uint32 * pdSeq);
};

#define SUR_DECREMENT      (0xfffffffel)
#define SUR_INCREMENT      (0xfffffffdl)

// Standard BREW Methods

#define ILICENSE_AddRef(po)                        GET_PVTBL(po,ILicense)->AddRef(po)
#define ILICENSE_Release(po)                       GET_PVTBL(po,ILicense)->Release(po)
#define ILICENSE_IsExpired(po)                     GET_PVTBL(po,ILicense)->IsExpired(po)
#define ILICENSE_GetInfo(po, pdw)                  GET_PVTBL(po,ILicense)->GetInfo(po, pdw)
#define ILICENSE_SetUsesRemaining(po, dw)          GET_PVTBL(po,ILicense)->SetUsesRemaining(po, dw)
#define ILICENSE_GetPurchaseInfo(po,plt,pdw,pdv)   GET_PVTBL(po,ILicense)->GetPurchaseInfo(po,plt,pdw,pdv)
#define ILICENSE_IncrementUsesRemaining(po)        GET_PVTBL(po,ILicense)->SetUsesRemaining(po, SUR_INCREMENT)
#define ILICENSE_DecrementUsesRemaining(po)        GET_PVTBL(po,ILicense)->SetUsesRemaining(po, SUR_DECREMENT)

#endif
/*=======================================================================
   DATA STRUCTURE DOCUMENTATION
=========================================================================
AEELicenseType

Description:
    This ENUM specifies the license type returned by ILICENSE_GetInfo().

Definition:
    typedef enum {
    LT_NONE,
    LT_USES,
    LT_DATE,
    LT_DAYS,
    LT_MINUTES_OF_USE,
    LT_MAX
    } AEELicenseType;

Members:
    LT_NONE :  There is no expiration on this module.
    LT_USES :  Usage counter.
    LT_DATE :  Expiration date.
    LT_DAYS :  Expiration date.
    LT_MINUTES_OF_USE :  Number of minutes remaining.
    LT_MAX/ :  Not used.

Comments:
    None

See Also:
    ILICENSE_GetInfo()

=============================================================================
AEEPriceType

Description:
    This enum describes the price type returned by ILICENSE_GetPurchaseInfo().

Definition:
    typedef enum {
    PT_NONE,
    PT_DEMO,
    PT_PURCHASE,
    PT_SUBSCRIPTION,
    PT_UPGRADE,
    PT_MAX
    } AEEPriceType;

Members:
    PT_NONE : ILICENSE_GetPurchaseInfo() failed.
    PT_DEMO :  FREE demo.
    PT_PURCHASE :  Purchase.
    PT_SUBSCRIPTION :  Subscription.
    PT_UPGRADE :  Upgrade.
    PT_MAX :  Not used.

Comments:
    None

See Also:
    ILICENSE_GetPurchaseInfo()

=========================================================================
  INTERFACE DOCUMENTATION
=========================================================================

Interface Name: ILicense

Description:

   This interface allows BREW applications to query license information for 
   their application module. The functions in this interface apply only to 
   the currently loaded or executing application module. These functions also 
   allow BREW applications to set the license information on the current module 
   if the license type is set to "Usage Based" (LT_USES).

The following header file is required:~
AEELicense.h


=======================================================================

ILICENSE_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 


=============================================================================

ILICENSE_Release()

Description:
    This function is inherited from IBASE_Release(). 


=======================================================================

ILICENSE_IsExpired()

Description:
    This function returns TRUE if the current module has expired. Expiration 
    is indicated either by a date value or the internal count on the application 
    module that is maintained by the application module using ILICENSE_Decrement 
    or ILICENSE_Increment. If the AEELicenseType is LT_USES, this function always 
    returns FALSE because the expiration is managed from within the application.

Prototype:
    boolean ILICENSE_IsExpired(ILicense * pILicense)

Parameters:
    pILicense  :  Pointer to the ILicense Interface object.

Return Value:
    TRUE: if application module has expired.
    FALSE: if application module has not expired.

Comments:
    None

Version:
     Introduced BREW Client 1.1

See Also:
    None

========================================================================
ILICENSE_GetInfo()

Description:
    This function returns the type of licensing that applies to the currently 
    running application module. It also provides expiration information specific 
    to the type of licensing.
    Applications that are licensed per use (LT_USES) have to call this function
    to determine whether further usage should be permitted. BREW does not enforce 
    restrictions on access to LT_USES applications; this is left up to the 
    application itself.
    Here are the supported licensing types, and the meaning of the associated 
    expiration value for each:
    -    LT_NONE means the license does not expire. There is no associated value.
    -    LT_USES means the license expires after a certain number of uses as 
         defined by the application. Its value is the number of uses remaining.
    -    LT_DATE means the license expires on specified date. Its value is the date/
         time of expiration.
    -    LT_DAYS means the license expires a number of days after downloading. 
         Its value is the time of expiration.
    -    LT_MINUTES_OF_USE means the license expires after application has been
         active a certain number of minutes. Its value is the number of minutes 
         remaining.
    
    In the cases of LT_DAYS and LT_DATE, the expiration value is a time value to
    be compared against GETTIMESECONDS().

    In the case of LT_MINUTES_OF_USE, the remaining minutes value is updated only
    when the application is exited. While the application is running, the value 
    returned by this function does not change. BREW enforces this type of licensing 
    only when the user tries to start the application. If the application expires 
    during the execution of the application, BREW does not take any action at that 
    time, and the application is allowed to continue. The application can, of course, 
    monitor its own activity time while it is running, and handle such expirations 
    as it wishes.

Prototype:
    AEELicenseType ILICENSE_GetInfo
    (
    ILicense * pILicense, 
    uint32 * pdwInfo
    )

Parameters:
    pILicense : [in] : Pointer to the ILicense Interface object.
    pdwInfo : [out] : Expiration information specific to the AEELicenseType returned.

Return Value:
    License type of currently running the application.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    None

========================================================================
ILICENSE_SetUsesRemaining()

Description:
    This function updates the usage count on the application. BREW does 
    not automatically update usage counts based on application startup or 
    any other events. It is the responsibility of the usage-licensed (LT_USES) 
    application to log usages through ILICENSE methods.

Prototype:
    int ILICENSE_SetUsesRemaining(ILicense * pILicense, uint32 dwCount)

Parameters:
    pILicense  :  Pointer to the ILicense Interface object.
    dwCount  :  New usage count.

Return Value:
    SUCCESS: if count is valid.
    EFAILED: if AEELicenseType is not of LT_USES.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    ILICENSE_GetInfo()

========================================================================
ILICENSE_IncrementUsesRemaining()

Description:
    This function increments by one the usage count on the application.

Prototype:
    int ILICENSE_IncrementUsesRemaining(ILicense * pILicense)

Parameters:
    pILicense :  Pointer to the ILicense Interface object.

Return Value:
    SUCCESS: if count is valid.
    EFAILED: if AEELicenseType is not of LT_USES.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    None

========================================================================
ILICENSE_DecrementUsesRemaining()

Description:
    This function decrements by one the usage count on the application.

Prototype:
    int ILICENSE_DecrementUsesRemaining(ILicense * pILicense)

Parameters:
    pILicense :  Pointer to the ILicense Interface object.

Return Value:
    SUCCESS: if count is valid.
    EFAILED: if AEELicenseType is not of LT_USES.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    None

========================================================================
ILICENSE_GetPurchaseInfo()

Description:
   This function returns the manner in which the application was purchased. 
   It optionally fills the license type and expiration. Price types include:
   -    PT_DEMO: Demo version
   -    PT_PURCHASE: Normally purchased version
   -    PT_SUBSCRIPTION: Subscription version
   -    PT_UPGRADE: Upgrade
   -    PT_AUTOINSTALL: Auto Installed

   This function can be used by an application to check if it has been purchased
   as a DEMO. An application can use this function to disable certain features if the
   application has been purchased as a DEMO.

   The license types and their associated expiration values are returned by using
   ILICENSE_GetInfo().

Prototype:
    AEEPriceType ILICENSE_GetPurchaseInfo
    (
    ILicense *pILicense, 
    AEELicenseType * plt,
    uint32 * pdwExpire,
    uint32 * pdwSeq
    )

Parameters:
    pILicense : [in] : Pointer to the ILicense Interface object.
    plt : [out] : Pointer to the license type
    pdwExpire : [out] : Pointer to the expiration value
    pdwSeq : [out] : Pointer to the sequence number of downloaded version

Return Value:
    See Parameters.

Comments:
    None

Version:
    Introduced BREW Client 1.1

See Also:
    None

=====================================================================*/
