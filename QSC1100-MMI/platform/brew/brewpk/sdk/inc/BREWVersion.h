#ifndef _BREWVERSION_H_
#define _BREWVERSION_H_
/*=====================================================================
FILE:  BREWVersion.h

SERVICES:  Compile time definition of BREW version

GENERAL DESCRIPTION:
   BREW compile time version definitions and compile time macros
   for checking minimum version.


Copyright © 2000-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================================*/

#define BREW_MAJ_VER          3
#define BREW_MIN_VER          1
#define BREW_SUB_VER          5
#define BREW_BUILD            189
#define BREW_BUILD_NUMBER     3,1,5,189
#define BREW_BUILD_STRING     "3.1.5.189"
#define BREW_UPDATE_VERSION   "SP03"

#define MIN_BREW_VERSION(maj,min) \
        (BREW_MAJ_VER > maj || (BREW_MAJ_VER == maj && BREW_MIN_VER >= min))

#define MIN_BREW_VERSIONEx(maj,min,sub) \
        (BREW_MAJ_VER > maj \
         || (BREW_MAJ_VER == maj \
             && (BREW_MIN_VER > min || (BREW_MIN_VER == min && BREW_SUB_VER >= sub))))

#define MIN_BREW_BUILD(maj,min,sub,bld) \
        (BREW_MAJ_VER > maj || \
         (BREW_MAJ_VER == maj && \
          (BREW_MIN_VER > min || \
           (BREW_MIN_VER == min && \
            (BREW_SUB_VER > sub || \
             (BREW_SUB_VER == sub && BREW_BUILD >= bld))))))



#endif // _BREWVERSION_H_


/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
    BREW_MAJ_VER

Description:
    BREW Major Version Number

Definition:
    This macro provides information about the Major Version Number of the BREW build.
This allows BREW Apps to find out at compile time, what BREW version they are compiling
against.
    BREW follows a 4 digit naming convention:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintenance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BREW_MIN_VER
    BREW_SUB_VER

=============================================================================

    BREW_MIN_VER

Description:
    BREW Minor Version Number

Definition:
    This macro provides information about the Minor Version Number of the BREW Build.
This allows BREW Apps to find out at compile time, what BREW version they are compiling
against.
    BREW follows a 4 digit naming convention:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintenance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BREW_MAJ_VER
    BREW_MIN_VER

=============================================================================

    BREW_SUB_VER

Description:
    BREW Major Version Number

Definition:
    This macro provides information about the Sub-Version Number of the BREW Version.
This allows BREW Apps to find out at compile time, what BREW version they are compiling
against.
    BREW follows a 4 digit naming convention:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintenance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BREW_MAJ_VER
    BREW_MIN_VER

=============================================================================

BREW_UPDATE_VERSION

Description:
BREW Upadte (Patch) Information

Definition:
This macro provides information about the Patches applied to the BREW Version.

The patch string is maintained as a comma separated list of patches applied to
the system.
e.g: "P1,P2,FP!,P5"

Members:
None

Comments:
None

See Also:

============================================================================*/
