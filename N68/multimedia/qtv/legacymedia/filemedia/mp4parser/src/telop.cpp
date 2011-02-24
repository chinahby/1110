/* =======================================================================
                               telop.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/telop.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT

#include "telop.h"

/* ==========================================================================

                 DATA DEFINITIONS AND DECLARATIONS

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* ==========================================================================

                        MACRO DECLARATIONS

========================================================================== */

/* ==========================================================================

                   CLASS/FUNCTION DEFINITIONS

========================================================================== */

/* ======================================================================
FUNCTION 
  TelopElement :: TelopElement

DESCRIPTION
  Constructor for the TelopElement.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
TelopElement :: TelopElement ()
{
  oWrapFlag = 0;
  numSubStrings = 0;
  beginTime = 0;
  duration = 0;
  telopSize = 0;
}

/* ======================================================================
FUNCTION 
  TelopElement :: TelopElement

DESCRIPTION
  Copy Constructor for the TelopElement.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
TelopElement :: TelopElement (TelopElement & srcTelop, int32 * constructionresult)
{
  int32 i;
  SubStrings * src, * des=NULL;

  if(constructionresult)
  {
    * constructionresult = SUCCESSFUL;

    this -> oWrapFlag       = srcTelop . oWrapFlag;
    this -> numSubStrings   = srcTelop . numSubStrings;
    this -> beginTime       = srcTelop . beginTime;
    this -> duration        = srcTelop . duration;
    this -> telopSize       = srcTelop . telopSize;

    for ( i = 0; i < srcTelop . numSubStrings; i ++ )
    {
      src = srcTelop . subStringVector . ElementAt (i);
      
      if(!src)
      {
        break;
      }
            
      des = (SubStrings*)QTV_Malloc(sizeof(SubStrings));
      
      if ( des )
      {
        memset(des, 0, sizeof(SubStrings));
        this -> subStringVector += des;
      }
      else
      {
        * constructionresult = ERROR_IN_ALLOCATING_MEMORY_FOR_SUBSTRINGS;
        break;
      }

      des ->oLineFeed = src ->oLineFeed;
      des ->oUnderLine = src ->oUnderLine;
      des ->oReversal = src ->oReversal;
      des ->oLinking = src ->oLinking;
      des ->sizeofTextSampleInBytes = src ->sizeofTextSampleInBytes;
      des ->fontColor = src ->fontColor;
      des ->linkSize = src ->linkSize;

      if(src ->sizeofTextSampleInBytes)
      {
        des ->textSubString = (char*)QTV_Malloc(src ->sizeofTextSampleInBytes + 1);

        if ( des ->textSubString )
        {
          (void)std_strlcpy(des ->textSubString, src ->textSubString,(src ->sizeofTextSampleInBytes + 1));
        }
        else
        {
          * constructionresult = ERROR_IN_ALLOCATING_MEMORY_FOR_STRING_MEMBER;
          QTV_Free(des);
          break;
        }
      }
      if ( src ->oLinking )
      {
        des ->linkValue = (char*)QTV_Malloc(src ->linkSize + 1);

        if ( des -> linkValue )
        {
          (void)std_strlcpy (des ->linkValue, src ->linkValue,(src ->linkSize + 1));
        }
        else
        {
          * constructionresult = ERROR_IN_ALLOCATING_MEMORY_FOR_LINKVALUE;
          QTV_Free(des ->textSubString);
          QTV_Free(des);
          break;
        }
      }
      else
        des ->linkValue = 0;
    }
  } 
}

/* ======================================================================
FUNCTION 
  TelopElement :: GetWrapFlag

DESCRIPTION
  Returns Wrap Flag of a TelopElement.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool TelopElement :: GetWrapFlag ()const /*made constant */
{
  return oWrapFlag;
}

/* ======================================================================
FUNCTION 
  TelopElement :: ~TelopElement

DESCRIPTION
  Destructor of TelopElement

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
TelopElement :: ~TelopElement ()
{
  int32 i;
  SubStrings * substring;

  for ( i = 0; i < subStringVector .GetLength (); i ++ ) /*lint !e1551 */
  {
    substring = GetSubStringStructAt (i);/*lint !e1551 */

    if ( substring ->textSubString )
    {
      QTV_Free(substring ->textSubString);
    }

    if ( substring ->linkValue )
    {
      QTV_Free(substring ->linkValue);
    }

    QTV_Free(substring);
  }

  subStringVector .Clear ();/*lint !e1551 */
}

#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
