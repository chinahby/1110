/*===========================================================================

FILE: OEMRegistry.c

SERVICES: OEM Registry related implementation.

DESCRIPTION
   This file implements IMedia for BREW MultiMedia format. All 
   media formats like MIDI, MP3, QCP must be derived from this class.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "OEMRegistry.h"

#if defined(FEATURE_SECMSM_APPS)
#include "QMIMETypeMagic.h"
#endif

#if defined(FEATURE_BREW_MULTIMEDIA)
#include "OEMMedia.h"
#endif // defined(FEATURE_BREW_MULTIMEDIA)

/*======================================================================= 
Function: OEMRegistry_DetectType

Description: 
   Given data in a buffer or the name of an object, this function detects 
   the MIME type. 
   
   This function is typically used to get the handler associated with the data type. 
   For example, if the data represents standard MIDI format, then this 
   function returns the MIME "audio/mid". Using the MIME type, you can query 
   Shell registry to obtain the handler (Class ID) of type AEECLSID_MEDIA.

Prototype:

   int OEMRegistry_DetectType(IShell * po, const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME);

Parameters:
   po [in]:          Pointer to the IShell object
   cpBuf [in]:       Buffer containing the data whose type needs to be determined
   pdwSize [in/out]: On input: Size of data in pBuf, unless pBuf is NULL, then ignored
                     On output: number of additional data bytes needed to perform type detection
   cpszName [in]:    Name of the object whose type needs to be determined (may be null, if unknown).
   pcpszMIME [out]:  MIME string returned to caller, on return, filled with a pointer to a constant string (do not free)

Return Value: 
   SUCCESS:    Data type is detected and MIME is returned
   ENOTYPE:    There is no type associated with this data
   EBADPARM:   Wrong input data (parameter(s))
   ENEEDMORE:  Need more data to perform type detection. *pdwSize contains the the required
               number of additional bytes.
   EUNSUPPORTED: Type detection for the specified input is not supported

Comments: None
Side Effects: None
See Also: None
========================================================================*/
int OEMRegistry_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME)
{
   int      nRet = SUCCESS;
   uint32   dwMaxBytes = 0; // the max number of additional bytes needed to enable type detection.
   boolean  bNeedMore = FALSE;

   // If the size parameter is NULL, we can't even return the need for more
   // data, so just fail.

   if (!pdwSize)
      return EBADPARM;

   // If we don't have any data with which to do the type detection, or if we
   // don't have an output buffer with which to report the MIME type, we'll 
   // proceed with calling the type detection routines, and let them return
   // the actual data size required.  We will then return that value with a
   // return code of ENEEDMORE.

   // Check if data is IMedia type...
#if defined(FEATURE_BREW_MULTIMEDIA)
   nRet = OEMMedia_DetectType(cpBuf, pdwSize, cpszName, pcpszMIME);
   switch (nRet)
   {
     case SUCCESS:
       /* If we found the MIME type, just return it. */
       *pdwSize = 0;
       return SUCCESS;

     case ENEEDMORE:
       // If we need more data, note that we need more and the amount needed 
       // and continue with any other type detection schemes.
       bNeedMore = TRUE;
       if (*pdwSize > dwMaxBytes)
       {
         dwMaxBytes = *pdwSize;
       }
       break;

     case ENOTYPE:
       // Fallthrough to the next MIME type detection scheme
       break;

     case EBADPARM:
     case EUNSUPPORTED:
     default:
       // Something actually failed, so just give up.
       return nRet;
   }
#endif // defined(FEATURE_BREW_MULTIMEDIA)

#if defined(FEATURE_SECMSM_APPS)
   nRet = QMIMETypeMagic_DetectMIMEType(cpBuf, pdwSize, cpszName, pcpszMIME);
   switch (nRet)
   {
     case SUCCESS:
       /* If we found the MIME type, just return it. */
       *pdwSize = 0;
       return SUCCESS;

     case ENEEDMORE:
       // If we need more data, note that we need more and the amount needed 
       // and continue with any other type detection schemes.
       bNeedMore = TRUE;

       if (*pdwSize > dwMaxBytes)
       {
         dwMaxBytes = *pdwSize;
       }
       break;

     case ENOTYPE:
       // Fallthrough to the next MIME type detection scheme
       break;

     case EBADPARM:
     case EUNSUPPORTED:
     default:
       // Something actually failed, so just give up.
       return nRet;
   }
#endif

   // If we expand this any further to include more type detection methods,
   // we should convert this into a table of detection methods and loop
   // through the table.

   // At this point, one of the following is true:
   //  1) bNeedMore is true, indicating that at least one detection method could 
   //     determine the type with more data.  (If so, dwMaxBytes holds the maximum
   //     number of bytes required to determine the type.)
   //  2) All of the detection methods returned ENOTYPE, so we cannot determine
   //     the type, even with more data.
   if (bNeedMore == TRUE)
   {
     // We didn't get it, but we could with more data
     *pdwSize = dwMaxBytes;
     return ENEEDMORE;
   }
   else
   {
     // We couldn't determine the type.
     *pdwSize = 0;
     return ENOTYPE;
   }
}

