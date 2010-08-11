/*======================================================
FILE:  AEE_OEMComdef.h

SERVICES:  AEE OEM common definitions

GENERAL DESCRIPTION:

Common definitions for the AEE/OEM interface

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_COMDEF_H)
#define AEE_OEM_COMDEF_H  1

#if defined(__cplusplus)
extern "C" {
#endif

// Defines for unlimited max files and max space (for static apps and for dynamic apps
// that do not have values set either in MIF or by OEM)
#define  MAX_FILES_UNLIMITED  (0xffff)
#define  MAX_SPACE_UNLIMITED  (0xffffffff)


// Defines for minimum values for wMaxFiles and dwMaxSpace. OEMs cannot set wMaxFiles
// and dwMaxSpace in CFGI_MODULE_FSLIMIT to values below these.
#define  MAX_FILES_MINIMUM    (7)
#define  MAX_SPACE_MINIMUM    (1024 * 20)


// This is a special key code value.  It should be sent by the OEM
// as desired.  It will not be sent to the apps and will close all apps
// other than AEECLSID_APPMANAGER

#define AVK_EXIT   0x100

// Note - BREW directed SMS messages follow the following format:
//
// //BREW:<CLSID>:<Text>
//
// BREW SMS Prefix

#define BREW_SMS_PREFIX       "//BREW:"
#define BREW_SMS_PREFIX_LEN   (sizeof(BREW_SMS_PREFIX)-1)

#define WStrLen(p)                           WSTRLEN(p)
#define WStrSize(p)                          WSTRSIZE(p)

// 
// Copyright Timeout
//
#define COPYRIGHT_TIMEOUT        (2000)

// Helpful typedefs.
// OEMCONTEXT - Denotes an opaque hi-level structure, pointer, etc.
// OEMINSTANCE - Is used to demonte an opaque handle to a particular instance of
// data.
// OEMAPPCONTEXT - Denotes an opaque handle for OEM Context data for an 
// application context.

typedef void *                   OEMCONTEXT;
typedef void *                   OEMINSTANCE;
typedef void *                   OEMAPPCONTEXT;
typedef struct _AppContext       ACONTEXT;
typedef ACONTEXT *               PACONTEXT;
typedef struct _ModuleContext    MCONTEXT;
typedef MCONTEXT *               PMCONTEXT;

#if defined(__cplusplus)
}
#endif

#endif // !defined(AEE_OEM_COMDEF_H)
