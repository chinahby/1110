#include "AEE_OEM.h"
#include "OEMConfig.h"
#include "OEMAuth.h"
#include "AEEFile.h"
#include "AEEStdLib.h"


#define CFGI_OEMAUTH_RUIM_CARDID  (CFGI_MAX + 121)


/*============================================================
OEM stub function to return f(NN) based upon cardID. It 
generates two different responses using two differnet algorimthms.
The algorithm will be picked based on RUIM card setting (cardid).

This function will be implemented by OEMs
============================================================*/
void OEMAuth_GetChallengeResponse(OEMAuth_Challenge* pChallenge,  AEECallback *pcb)
{
   uint32      i;
   IShell     *pIShell = AEE_GetShell();
   uint32      cardid = 0;
   int nErr;

   pChallenge->nResult = EBADSTATE;

   nErr = OEM_GetConfig(CFGI_OEMAUTH_RUIM_CARDID, &cardid, sizeof(cardid));
   DBGPRINTF("OEMAUTH RUIM card id: %d (%d)", nErr, cardid);

   if (SUCCESS == nErr) {   
	   //Two algoritms to simulate two differnt RUIM cards
      //In both, response's length is same as challenge's length

	   switch (cardid) {
	      uint32 nextByte;
	      case 1:     //reverse byte order in response
            //see if length of the passed buffer is enough
            if(pChallenge->nResponseLen < pChallenge->nChallengeLen) {
               pChallenge->nResult = ENEEDMORE;
            } else {
	            for(i=0; i < (pChallenge->nChallengeLen); i++) {
	               pChallenge->pResponse[i] = pChallenge->pChallenge[pChallenge->nChallengeLen-1-i];
	            }    
	            pChallenge->nResult = AEE_SUCCESS;
            }
            pChallenge->nResponseLen = pChallenge->nChallengeLen;   
	         break;

	      case 2:  // each byte is bitwise OR of its corresponding byte in challenge and its next byte
            //see if length of the passed buffer is enough
            if(pChallenge->nResponseLen < pChallenge->nChallengeLen) {
               pChallenge->nResult = ENEEDMORE;
            } else {
	            for(i=0; i < (pChallenge->nChallengeLen); i++) {
	               nextByte = (i+1)%pChallenge->nChallengeLen;
	               pChallenge->pResponse[i] = pChallenge->pChallenge[i] | pChallenge->pChallenge[nextByte];
	            }
	            pChallenge->nResult = AEE_SUCCESS;
            }
            pChallenge->nResponseLen = pChallenge->nChallengeLen;
	         break;

	      default: // card not initialized 
	         pChallenge->nResponseLen = 0;
	         break;
	   }
   }
      
   //Issue client's callback
   ISHELL_Resume(pIShell, pcb);
}