/*
  ========================================================================

  FILE:  DocModel.h
  
  SERVICES: 

  GENERAL DESCRIPTION: 

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __DOCMODEL_H__
#define __DOCMODEL_H__

#include "AEEDocModel.h"

int   DocModel_New      (IDocModel **ppo, IShell *piShell, IModule *piModule);
byte* DocModel_Peek     (IDocModel *po, int32 *pcbAvailable, int32 *pnCode);
void  DocModel_Peekable (IDocModel *po, AEECallback *pcb);
void  DocModel_Advance  (IDocModel *po, int32 cbAdvance);


#endif   //__DOCMODEL_H__

