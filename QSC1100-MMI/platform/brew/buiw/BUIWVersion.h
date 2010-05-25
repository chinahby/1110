#ifndef _BUIWVERSION_H_
#define _BUIWVERSION_H_
/*=====================================================================
FILE:  BUIWVersion.h

SERVICES:  Compile time definition of BREW UI Widgets version

GENERAL DESCRIPTION:
   BREW UI Widgets compile time version definitions and compile time macros
   for checking minimum version.


        Copyright © 2000-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#define BUIW_MAJ_VER          1
#define BUIW_MIN_VER          4
#define BUIW_SUB_VER          3 
#define BUIW_BUILD            468068
#define BUIW_BUILD_NUMBER     1,4,3,468068
#define BUIW_BUILD_STRING     "1.4.3.468068"


#define MIN_BUIW_VERSION(maj,min) \
        (BUIW_MAJ_VER > maj || (BUIW_MAJ_VER == maj && BUIW_MIN_VER >= min))

#define MIN_BUIW_VERSIONEx(maj,min,sub) \
        (BUIW_MAJ_VER > maj \
         || (BUIW_MAJ_VER == maj \
             && (BUIW_MIN_VER > min || (BUIW_MIN_VER == min && BUIW_SUB_VER >= sub))))

#endif // _BUITVERSION_H_


/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
    BUIW_MAJ_VER

Description:
    BREW UI Widgets Major Version Number

Definition:
    This macro provides information about the Major Version Number of the BREW UI Widgets build.
    BREW follows a 4 digit naming conventon:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintainance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BUIW_MIN_VER
    BUIW_SUB_VER

=============================================================================

    BUIW_MIN_VER

Description:
    BREW UI Widgets Minor Version Number

Definition:
    This macro provides information about the Minor Version Number of the BREW UI Widgets Build.
    BREW follows a 4 digit naming conventon:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintainance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BUIW_MAJ_VER
    BUIW_MIN_VER

=============================================================================

    BUIW_SUB_VER

Description:
    BREW UI Widgets Sub-Version Number

Definition:
    This macro provides information about the Sub-Version Number of the BREW UI Widgets Version.
    BREW follows a 4 digit naming conventon:
    w.x.y.z;
    w - Major Version Number
    x - Minor Version Number
    y - Sub-Version Number. Also called maintainance release version number
    z - Actual Build Number

Members:
    None

Comments:
    None

See Also:
    BUIW_MAJ_VER
    BUIW_MIN_VER
============================================================================*/
