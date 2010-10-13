/*=============================================================================

FILE: OEMMangoBrewStubs.c

GENERAL DESCRIPTION: 
      These are stub functions which are added to resolve compile errors
      araised after removing some libraries which not required for Mango.

        Copyright © 1999-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "assert.h"
#include "aeeerror.h"
#include "AEEStdLib.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEE_OEMFile.h"

const AEEConstFile *  gpBREWConstFiles[] = {NULL};

void AEENetList_DrawStat(IDisplay *piDisp)
{
   return;
}

#ifndef CUST_EDITION
int Static_New(IShell * pShell,AEECLSID cls, void **ppif)
{
   ASSERT(FALSE);
   return(EUNSUPPORTED);
}

int ADialog_New(IShell * ps,AEECLSID cls, void **ppif)
{
   ASSERT(FALSE);
   return(EUNSUPPORTED);
}

boolean ADialog_HandleEvent(IDialog * po, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   ASSERT(FALSE);
   return(FALSE);
}

boolean ADialog_Prompt(IDialog * po,AEEPromptInfo * pi)
{
   ASSERT(FALSE);
   return(FALSE);
}

void ADialog_PromptEnableScrolling(IDialog * po, boolean bEnable)
{
   ASSERT(FALSE);
}

int ADialog_Load(IDialog * po, const char * pszResFile, uint16 wID,DialogInfo * pInfo)
{
   ASSERT(FALSE);
   return(EUNSUPPORTED);
}
#endif
int rsa(const uint8 input[], const int inputSize, 
        uint8 output[], int *outputSize,
        const uint8 exponent[], const int exponentSize,
        const uint8 modulus[], const int modulusSize)
{
   ASSERT(FALSE);
   return(EUNSUPPORTED);
}

void AEESecurity_AccumulateTimeForFIPS186(boolean bActualTime)
{
   return;
}

void AEESecurity_AccumulateCreateInstanceRandomnessForFIPS186(AEECLSID clsid, void **ppObj)
{
   return;
}

void AEEDebug_VPrintf(int nLevel, const char *cpszFile, int nLine, 
                      const char *cpszFormat, va_list ap)
{
   return;
}

int Clipboard_New(IShell * ps,AEECLSID cls, void **ppif)
{
   return(ENOMEMORY);
}

void Clipboard_Init(IShell * ps)
{
   return;
}

void AEEDebug_LogEvent(AEEEvent evt, AEECLSID cls, uint32 pl)
{
   return;
}

#ifndef CUST_EDITION
uint32 ParagraphAlignment(AECHAR *pszText, int nChars)
{
      return (IDF_ALIGN_LEFT);
}
#endif

void OEMDebug_Trigger(unsigned uDebugTriggerNum)
{
   return;
}

int AEEFIFO_New(IShell* ps, AEECLSID cls, void** ppif)
{
   return ECLASSNOTSUPPORT;
}