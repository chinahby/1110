
/*======================================================
FILE:  AEEOEMExtFor3Part.h

SERVICES:  OEM Extension Interface for three part developer.

GENERAL DESCRIPTION:
   
=====================================================*/

#ifndef __AEE_OEM_EXT_FOR_3PART_H__
#define __AEE_OEM_EXT_FOR_3PART_H__

#include "AEEInterface.h"


//////////////////////////////////////////////////////////////////////////

typedef struct IOEMExtFor3Part IOEMExtFor3Part;

AEEINTERFACE(IOEMExtFor3Part)
{
	uint32  (*AddRef)(IOEMExtFor3Part *pi);
	uint32  (*Release)(IOEMExtFor3Part *pi);
	void (*ShowUnReadPushIcon)(IOEMExtFor3Part *pi, boolean bShowOrHide);
	void (*PlayPushRing)(IOEMExtFor3Part *pi);
	boolean (*IsInIdle)(IOEMExtFor3Part *pi);
};

//////////////////////////////////////////////////////////////////////////

static __inline uint32 IOEMEXTFOR3PART_AddRef(IOEMExtFor3Part *pi)
{
	return AEEGETPVTBL(pi, IOEMExtFor3Part)->AddRef(pi);
}

static __inline uint32 IOEMEXTFOR3PART_Release(IOEMExtFor3Part *pi)
{
	return AEEGETPVTBL(pi, IOEMExtFor3Part)->Release(pi);
}

static __inline void IOEMEXTFOR3PART_ShowUnReadPushIcon(IOEMExtFor3Part *pi, boolean bShowOrHide)
{
	AEEGETPVTBL(pi, IOEMExtFor3Part)->ShowUnReadPushIcon(pi, bShowOrHide);
}

static __inline void IOEMEXTFOR3PART_PlayPushRing(IOEMExtFor3Part *pi)
{
	AEEGETPVTBL(pi, IOEMExtFor3Part)->PlayPushRing(pi);
}

static __inline boolean IOEMEXTFOR3PART_IsInIdle(IOEMExtFor3Part *pi)
{
	return AEEGETPVTBL(pi, IOEMExtFor3Part)->IsInIdle(pi);
}


#endif	// __AEE_OEM_EXT_FOR_3PART_H__