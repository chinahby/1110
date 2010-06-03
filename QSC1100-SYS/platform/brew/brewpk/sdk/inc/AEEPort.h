/*=================================================================================

FILE: AEEPort.h

SERVICES:  
   IPort interface.

GENERAL DESCRIPTION:
   Bidirectional IO interface derived from ISource.
           

         Copyright © 1999-2002 QUALCOMM Incorporated.
                   All Rights Reserved.
                 QUALCOMM Proprietary/GTDR
=================================================================================*/
#ifndef _AEEPORT_H_
#define _AEEPORT_H_

#include "AEE.h"
#include "AEESource.h"
#include "AEEIOCtl.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../../inc/AEEIPort.h"

#define AEEPORT_CLOSED    IPORT_END
#define AEEPORT_ERROR     IPORT_ERROR
#define AEEPORT_WAIT      IPORT_WAIT

#define IPORT_AddRef(p)                   AEEGETPVTBL((p),IPort)->AddRef((p))
#define IPORT_Release(p)                  AEEGETPVTBL((p),IPort)->Release((p))
#define IPORT_QueryInterface(p,i,ppo)     AEEGETPVTBL((p),IPort)->QueryInterface((p),(i),(ppo))
#define IPORT_Read(p,pc,cb)               AEEGETPVTBL((p),IPort)->Read((p),(pc),(cb))
#define IPORT_Readable(p,pcb)             AEEGETPVTBL((p),IPort)->Readable((p),(pcb))
#define IPORT_GetLastError(p)             AEEGETPVTBL((p),IPort)->GetLastError((p))
#define IPORT_Write(p,pc,cb)              AEEGETPVTBL((p),IPort)->Write((p),(pc),(cb))
#define IPORT_Writeable(p,pcb)            AEEGETPVTBL((p),IPort)->Writeable((p),(pcb))
#define IPORT_IOCtl(p,opt,v)              AEEGETPVTBL((p),IPort)->IOCtl((p),(opt),(v))
#define IPORT_Close(p)                    AEEGETPVTBL((p),IPort)->Close((p))
#define IPORT_Open(p,pname)               AEEGETPVTBL((p),IPort)->Open((p),(pname))


typedef struct {
   int nWrote;
   const void *pIOVec;
   uint16 wIOVCount;
} PortWriteV;
#define AEEPORT_IOCTL_WRITEV BREWIOCTL_R(BREWIOCTL_ANYPORT, 1, PortWriteV)

static __inline int IPORT_WriteV(IPort *me, const void *pvIOVec, uint16 wIOVCount)
{
   int nErr;
   PortWriteV pwv;
   
   pwv.pIOVec = pvIOVec;
   pwv.wIOVCount = wIOVCount;
   
   nErr = IPORT_IOCtl(me, AEEPORT_IOCTL_WRITEV, (uint32)&pwv);
   if (SUCCESS != nErr) {
      typedef struct { /* an IOVec */
         const char* pc;
         uint16      wLen;
      } PortWriteV_IOVec;
      const PortWriteV_IOVec *pVec, *pEnd;
      
      for (pwv.nWrote = 0, 
             pVec = (const PortWriteV_IOVec *)pvIOVec,
             pEnd = pVec + wIOVCount;
           pVec < pEnd; pVec++) {
         nErr = IPORT_Write(me, pVec->pc, pVec->wLen);
         if (nErr < 0) {
            if (0 == pwv.nWrote) {
               // Only return error if we did not write anything whatsoever
               pwv.nWrote = nErr;
            }
            break;
         }
         pwv.nWrote += nErr;
         if (nErr != pVec->wLen) {
            break;// partial write, probably time to quit
         }
      }
   }

   return pwv.nWrote;      
}

#define IPORT_ReadableEx(p,pcb,pfn,pv)    CALLBACK_Init((pcb),(pfn),(pv)); \
                                          IPORT_Readable((p), (pcb))

#ifdef __cplusplus
}
#endif


#endif  //_AEEPORT_H_

