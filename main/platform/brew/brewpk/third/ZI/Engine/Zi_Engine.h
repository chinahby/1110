#ifndef _OEM_ZI_ENGINE_HEADER_H_ // {
#define _OEM_ZI_ENGINE_HEADER_H_
/*=============================================================================


=============================================================================*/
#include "AEE.h"
#include "AEEText.h"
#define AEECLSID_OEMIMMGR    	(0x0101278e)
#define MAX_ELEMENT_BUFFER_SIZE         20 

// System context options
#define IMMCTX_EMAIL          (0x0001)          // Match words for email type applications
#define IMMCTX_NAME           (0x0002)          // Match words for default name setting
#define IMMCTX_SURNAME        (0x0004)          // Match words for surnames, if supported
#define IMMCTX_GIVENNAME      (0x0008)          // Match words for given names, if supported
#define IMMCTX_PUNCTUATION    (0x0010)          // Match punctuation, if supported
#define IMMCTX_COMMON         (0x0020)          // Match only common items, if supported
#define IMMCTX_NODUPE         (0x0040)          // Match only one character per entry, if supported

#define IMMCTX_ALL            (0x007F)          // All Ctx Options
#define IMMCTX_LAST           (IMMCTX_NODUPE)   // Last Ctx Option

// Key press result options
#define IMMKEY_DEFAULT        (0x0000)          // System default
#define IMMKEY_MATCHONE       (0x0001)          // Match Key only
#define IMMKEY_MATCHMANY      (0x0002)          // Match other expected entries not on this key also

#define IMMKEY_LAST           (IMMKEY_MATCHMANY)// Last Key Option

// IMMgr Options
#define IMGRO_MAXCANDIDATE    (0x0001)          // pData = uint16 *, default value = 64
#define IMGRO_SYSTEMCONTEXT   (0x0002)          // pData = uint16 * of IMMCTX_*, default value is system dependant
#define IMGRO_KEYOPTIONS      (0x0003)          // pData = uint16 * of IMMKEY_*, default value is system dependant
typedef struct IIMMgr IIMMgr;

AEEINTERFACE(IIMMgr) 
{   
   INHERIT_IQueryInterface(IIMMgr);
   
   boolean  (*HandleEvent)(IIMMgr * po, AEEEvent evt, uint16 wp, uint32 dwp);
   void     (*Reset)(IIMMgr * po);
   int      (*Initialise)(IIMMgr * po);
   int      (*EnumModeInit)(IIMMgr * po);
   int      (*EnumNextMode)(IIMMgr * po, AEETextInputMode * pmode);
   int      (*SetInputMode)(IIMMgr * po, AEETextInputMode mode);
   int      (*GetInputMode)(IIMMgr * po, AEETextInputMode * pmode);
   int      (*SetOpt)(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
   int      (*GetOpt)(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
   int      (*GetCompositionCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   int      (*GetCompleteCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   boolean  (*IsCompleteCandidate)(IIMMgr * po, const AECHAR * pBuff);
   int      (*GetData)(IIMMgr * po, AECHAR * pBuff, uint16 * pwBuffSize);
   int      (*SetData)(IIMMgr * po, const AECHAR * pBuff);
   int      (*GetAssociatedCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   int      (*GetCompositionCount)(IIMMgr * po, int * pnElements);
};

#define IIMMGR_AddRef(p)                                 AEEGETPVTBL((p),IIMMgr)->AddRef((p))
#define IIMMGR_Release(p)                                AEEGETPVTBL((p),IIMMgr)->Release((p))
#define IIMMGR_QueryInterface(p,clsid,pp)                AEEGETPVTBL((p),IIMMgr)->QueryInterface((p),(clsid),(pp))
#define IIMMGR_HandleEvent(p,ec,wp,dw)                   AEEGETPVTBL((p),IIMMgr)->HandleEvent((p),(ec),(wp),(dw))
#define IIMMGR_Reset(p)                                  AEEGETPVTBL((p),IIMMgr)->Reset((p))
#define IIMMGR_Initialise(p)                             AEEGETPVTBL((p),IIMMgr)->Initialise((p))
#define IIMMGR_EnumModeInit(p)                           AEEGETPVTBL((p),IIMMgr)->EnumModeInit((p))
#define IIMMGR_EnumNextMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->EnumNextMode((p),(m))
#define IIMMGR_SetInputMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->SetInputMode((p),(m))
#define IIMMGR_GetInputMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->GetInputMode((p),(m))
#define IIMMGR_SetOpt(p,o,d,s)                           AEEGETPVTBL((p),IIMMgr)->SetOpt((p),(o),(d),(s))
#define IIMMGR_GetOpt(p,o,d,s)                           AEEGETPVTBL((p),IIMMgr)->GetOpt((p),(o),(d),(s))
#define IIMMGR_GetCompositionCandidates(p,b,n,f,pn,pm)   AEEGETPVTBL((p),IIMMgr)->GetCompositionCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_GetCompleteCandidates(p,b,n,f,pn,pm)      AEEGETPVTBL((p),IIMMgr)->GetCompleteCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_IsCompleteCandidate(p,b)                  AEEGETPVTBL((p),IIMMgr)->IsCompleteCandidate((p),(b))
#define IIMMGR_GetData(p,b,s)                            AEEGETPVTBL((p),IIMMgr)->GetData((p),(b),(s))
#define IIMMGR_SetData(p,b)                              AEEGETPVTBL((p),IIMMgr)->SetData((p),(b))
#define IIMMGR_GetAssociatedCandidates(p,b,n,f,pn,pm)    AEEGETPVTBL((p),IIMMgr)->GetAssociatedCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_GetCompositionCount(p,n)                  AEEGETPVTBL((p),IIMMgr)->GetCompositionCount((p),(n))
#endif // } !_OEM_IMM_HEADER_H_
