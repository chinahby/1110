/*=============================================================================

FILE: OEMFS.c

SERVICES: OEM Functions for Embedded File System

GENERAL DESCRIPTION:
A sample implementation of the OEMFS functions declared in
OEMFS.h that are used in the implementation of AEEFile.

PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMFS_GetNativePath
   OEMFS_GetMaxPathLen

INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"
#include "fs.h"
#include "OEMFSPath_priv.h"
#include "AEEFile.h"
#include "AEEStdLib.h"

#if defined(FEATURE_EFS_FTL)
#include "AEECLASSID_FTL.bid"
#endif /* #if defined(FEATURE_EFS_FTL) */
#define AEEFS_ODM_USER_DIR     "fs:/user/"

#if !defined(AEEFS_FTL_DIR)
#define AEEFS_FTL_DIR  "fs:/shared/ftl/"
#endif /* #if !defined(AEEFS_FTL_DIR) */

// The following value setting is based on two factors
// 1. Usage of QCT EFS v1.0
// 2. Bytes being added to directory name field in the DMSS fs_ops.c file
//    inside the routine fsi_ops_convert_dirname().  Locate the number
//    of bytes that are being added to the passed in directory name length
//    in the routine fsi_ops_convert_dirname() before it performs a check
//    to see if the directory name is > FS_FILENAME_MAX_LENGTH.  Subtract
//    the same number of bytes here to come up with the value of OEMFS_MAX_PATH
// The line of code below is written assuming that 3 bytes are being added
// to the directory name in fsi_ops_convert_dirname().

#define OEMFS_MAX_PATH (FS_FILENAME_MAX_LENGTH - 3)

#define AEEFS_USER_RINGERS_DIR  "fs:/user/sound/ringer/"

#undef CSTRLEN
#define CSTRLEN(x)      (sizeof(x) - 1)

typedef struct OEMFSGNPMap
{
   const char *cpszBREW;
   const char *cpszOEM;
   unsigned    bAutoCreate:1;
} OEMFSGNPMap;

/*
  OEMFSGNPMap entries must be listed in order from most specific
  to least specific.

  If OEMFSGNPMap is re-defined, OEMFS_GetMaxPathLength() should
  be examined for correctness.
*/
static const OEMFSGNPMap gapmBrewToOEM[] = {
   /*   from dir                to dir    */
#if defined(FEATURE_MMC)
   { AEEFS_CARD0_DIR           , OEMFS_CARD0_DIR  , FALSE },
#else /* #if defined(FEATURE_MMC) */
   { AEEFS_CARD0_DIR           , OEMFS_CARD0_DIR  , FALSE },
#endif /* #if defined(FEATURE_MMC) */
#if (defined(FEATURE_USB_HOST_MASS_STORAGE) && defined(FEATURE_USB_HOST))
   { AEEFS_MOD_DIR"26108", OEMFS_UHMS1_DIR, FALSE  },
   { AEEFS_MOD_DIR"26109", OEMFS_UHMS2_DIR, FALSE  },
#else /* #if (defined(FEATURE_USB_HOST_MASS_STORAGE) && defined(FEATURE_USB_HOST)) */
   { AEEFS_MOD_DIR"26108", 0, FALSE  },
   { AEEFS_MOD_DIR"26109", 0, FALSE  },
#endif /* #if (defined(FEATURE_USB_HOST_MASS_STORAGE) && defined(FEATURE_USB_HOST)) */
   { AEEFS_RINGERS_DIR         , OEMFS_RINGERS_DIR, TRUE  },
   { AEEFS_USER_RINGERS_DIR    , OEMFS_RINGERS_DIR, FALSE },
#if defined(FEATURE_EFS_FTL)
   { AEEFS_FTL_DIR             , OEMFS_FTL_DIR  , FALSE },
#else /* #if defined(FEATURE_EFS_FTL) */
   { AEEFS_FTL_DIR             , 0              , FALSE },
#endif /* #if defined(FEATURE_EFS_FTL) */
#ifdef FEATURE_ODM_UI
   { AEEFS_ODM_USER_DIR        , OEMFS_ODM_BREW_USER_DIR, TRUE },
#else /* #if defined(FEATURE_ODM_UI) */
   { AEEFS_ODM_USER_DIR             , 0              , FALSE },
#endif /* #if defined(FEATURE_ODM_UI) */
   { AEEFS_ROOT_DIR            , OEMFS_BREW_DIR   , TRUE  },
   { 0                         , 0                , 0     },
};


uint16 OEMFS_GetMaxPathLength(void)
{
   const OEMFSGNPMap *ppm;
   uint16 uMaxPath;

   for (uMaxPath = 0, ppm = gapmBrewToOEM; (char *)0 != ppm->cpszBREW; ppm++) {
      uint16 uLen;

      /* skip empty maps */
      if ((char *)0 == ppm->cpszOEM) {
         continue;
      }
      /*
         Length we're returning is the largest fs:/ path we'll ever make.

         An fs:/ path is constructed by taking some EFS filename, stripping
         an EFS prefix, and prepending an fs:/ prefix.

         Run through the table, subtracting length of EFS part,
         adding length of fs:/ part, see which length is longest...

         The +1 in the expression below reflects the fact that the AEEFS_
         defines include a trailing slash, where the OEMFS_ defines do not.
       */

      uLen = OEMFS_MAX_PATH - (STRLEN(ppm->cpszOEM)+1) + STRLEN(ppm->cpszBREW);

      uMaxPath = MAX(uLen, uMaxPath);
   }

   return uMaxPath;
}

boolean OEMFS_IsAutoCreate(const char *cpszIn)
{
   const OEMFSGNPMap *ppm;

   for (ppm = gapmBrewToOEM; (char *)0 != ppm->cpszBREW; ppm++) {
      const char *cpszTail;

      if ((char *)0 == ppm->cpszOEM) {
         continue;
      }
      cpszTail = SPLITPATH(cpszIn,ppm->cpszOEM);

      if ((char *)0 != cpszTail && '\0' == cpszTail[0]) {
         return ppm->bAutoCreate;
      }
   }

   return FALSE;
}


AEE_EXPORTS int OEMFS_GetNativePath(const char *cpszIn, char *pszOut, int *pnOutLen)
{
   const OEMFSGNPMap *ppm;
   int                nErr = EBADFILENAME;

   for (ppm = gapmBrewToOEM; (char *)0 != ppm->cpszBREW; ppm++) {
      const char *cpszTail = SPLITPATH(cpszIn,ppm->cpszBREW);

      if ((char *)0 != cpszTail) {
         if ((char *)0 != ppm->cpszOEM) { /* path explicitly unsupported */
            nErr = MAKEPATH(ppm->cpszOEM, cpszTail, pszOut, pnOutLen);

            /* trim any trailing slash for EFS1 */
            if (SUCCESS == nErr && (char *)0 != pszOut) {
               char *pc = pszOut+STRLEN(pszOut)-1;
               if ('/' == *pc) {
                  *pc = 0;
               }
            }
         }
         break;
      }
   }
   return nErr;
}


int OEMFS_GetBREWPath(const char *cpszIn, char *pszOut, int *pnOutLen)
{
   const OEMFSGNPMap *ppm;
   int                nErr = EBADFILENAME;

   if (cpszIn && ('/' == cpszIn[0])) {
      cpszIn += 1; // guard against some device returning / with path.
   }

   for (ppm = gapmBrewToOEM; (char *)0 != ppm->cpszBREW; ppm++) {

      /* unless path explicitly unsupported */
      if ((char *)0 != ppm->cpszOEM) {

         const char *cpszTail = SPLITPATH(cpszIn, ppm->cpszOEM);

         if ((char *)0 != cpszTail) {
            nErr = MAKEPATH(ppm->cpszBREW, cpszTail, pszOut, pnOutLen);
            break;
         }
      }
   }

   return nErr;
}
