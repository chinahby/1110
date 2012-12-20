/* =======================================================================
                               qcplayer_oscl_utils.cpp
DESCRIPTION
  Some utilities common across zrex oscl, psos oscl, and qcplayer.

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

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/src/qcplayer_oscl_utils.cpp#5 $
$DateTime: 2009/01/29 06:14:06 $
$Change: 829425 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer_oscl_utils.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
const int ZUtils::npos = -1;

static const char eos = '\0';

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION 
  ZUtils::Find

DESCRIPTION
// 
// Find a pattern in a string, return starting point or npos
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
int ZUtils::Find(const char *s, const char *pat, int i) 
{

  int nsource = strlen(s)+1;
  if (0 <= i && i < nsource)
  {
    int npat=strlen(pat);
    for (int pos=i;s[pos]!=eos;pos++) 
    {
      if (StrncmpI(&s[pos],pat,npat)) 
      {
        return pos;
      }
    }
  }
  return npos;
}

/* ======================================================================
FUNCTION 
  ZUtils::StrncmpI

DESCRIPTION
// case-insensitive strncmp.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool ZUtils::StrncmpI(const char *s1, const char *s2, int n)
{

  for (int i=0;i<n;i++)
  {
    if (s1[i]==eos || s2[i]==eos) return false;
    if (Lower(s1[i]) != Lower(s2[i])) return false;
  }
  return true;
}

/* ======================================================================
FUNCTION 
  ZUtils::Lower

DESCRIPTION
// just like tolower, which isn't available yet in zrex.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
char ZUtils::Lower(const char c)
{
  if ('A' <= c && c <= 'Z')
  {
    return c + 'a' - 'A';
  }
  return c;
}

/* ======================================================================
FUNCTION 
  ZUtils::StrcpyN

DESCRIPTION
// 
// strcpy with truncation.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Returns the length of the string, not including the last null
  terminator.

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
int ZUtils::StrcpyN(char *d, const char *s,int max_length,bool bNullTerm)
{
  int i;
  for (i = 0; i < max_length; i++) 
  {
    d[i] = s[i];
    if ( ! s[i] )
    {
      return i;
    }
  }

  //null term may overwrite the last char
  if (bNullTerm)
  {
    d[max_length-1]='\0';
  }

  return max_length;
}

/* ======================================================================
FUNCTION 
  ZUtils::FindR

DESCRIPTION
// 
// Find a pattern at the end of a string, return starting point or npos
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
int ZUtils::FindR(const char *s, const char *pat) 
{
  return (Find(s,pat,strlen(s)-strlen(pat)));
}

/* ======================================================================
FUNCTION 
  ZUtils::ceil

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
double ZUtils::ceil( double x ) 
{
  long xlong=(long)x;
  return ( ((double)xlong < x) ? (double)(xlong+1) : (double)xlong ); 
}

/* ======================================================================
FUNCTION 
  ZUtils::Init

DESCRIPTION
//////////////////////////////////////////////////////////////////////
//
// Common Init Routine
//
//////////////////////////////////////////////////////////////////////

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
void ZUtils::Init()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "ZUtils::Init");

  InitClock();
}

#ifndef FEATURE_WINCE

/* ======================================================================
FUNCTION:
  ZUtils::readNVItem

DESCRIPTION:
  Read NV Item

INPUT/OUTPUT PARAMETERS:
  item 
  nv_item

RETURN VALUE:
  Returns true on success
  Returns false if it fails
    
SIDE EFFECTS:
  None.
======================================================================*/
bool ZUtils::readNVItem
(
  nv_items_enum_type item,
  nv_item_type *nv_item
)
{

   /* Read RTSP_proxy item from NV.
     Value of item can refer to either Primary proxy NV item
     or failover proxy nv item
    */
   nv_cmd_type   qtv_NV_cmd;

   qtv_NV_cmd.item       = item;
   qtv_NV_cmd.cmd        = NV_READ_F;
   qtv_NV_cmd.data_ptr   = nv_item;
   qtv_NV_cmd.tcb_ptr    = rex_self();
   qtv_NV_cmd.sigs       = QTV_NV_SIG;
   qtv_NV_cmd.done_q_ptr = NULL;

   (void) rex_clr_sigs( rex_self(), QTV_NV_SIG );
   nv_cmd(&qtv_NV_cmd);
   (void) rex_wait( QTV_NV_SIG );
   (void) rex_clr_sigs( rex_self(), QTV_NV_SIG );

   if ( qtv_NV_cmd.status == NV_DONE_S )
   {
      return true;
   } else
   {
      return false;
   }
}
#endif


#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
/* ======================================================================
FUNCTION:
  ZUtils::ThrowException

DESCRIPTION:
  Throws exception 

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
    
SIDE EFFECTS:
   Actual implementation is missing , needs to be properly defined
   Dummy implementation is added to bypass linking errors.
   
======================================================================*/
void ZUtils::ThrowException(int code, unsigned long param)
{
  QTV_USE_ARG1(code);
  QTV_USE_ARG1(param);
}
#endif

