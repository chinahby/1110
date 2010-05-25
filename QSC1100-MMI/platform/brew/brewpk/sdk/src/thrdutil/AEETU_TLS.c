
#include "AEETU_TLS.h"

struct AEETlsSlot
{
   AEETlsSlot *pNext;
   const char *cpszKey;
   void       *pVal;
};

IThread *IThread_Self(void)
{
   AEETls *me;

   GETTLS(me);

   if ((AEETls *)0 == me) {
      return 0;
   }

   return me->piThread;
}

#define STREQ(s1, s2) (((s1) == (s2)) || \
                       (((char *)0 != (s1)) && \
                        ((char *)0 != (s2)) && \
                        !STRCMP((s1),(s2))))

static AEETlsSlot *AEETls_Find(AEETls *me, const char *cpszKey)
{
   AEETlsSlot *ptlssRet;
   
   for (ptlssRet = me->ptlssList;
        (AEETlsSlot *)0 != ptlssRet && !STREQ(ptlssRet->cpszKey,cpszKey);
        ptlssRet = ptlssRet->pNext);
   
   return ptlssRet;
}

int IThread_SetTLS(const char *cpszKey, void *pVal)
{
   AEETls     *me;
   AEETlsSlot *ptlss;

   GETTLS(me);

   if ((AEETls *)0 == me) {
      return EFAILED;
   }

   ptlss = AEETls_Find(me, cpszKey);

   if ((AEETlsSlot *)0 == ptlss) {
      IThread * pIThread = IThread_Self();
      ptlss = (AEETlsSlot *)ITHREAD_Malloc(pIThread,sizeof(AEETlsSlot));
      
      if ((AEETlsSlot *)0 == ptlss) {
         return ENOMEMORY;
      }
      
      ptlss->pNext = me->ptlssList;
      me->ptlssList = ptlss;

      ptlss->cpszKey = cpszKey;
   }
   
   ptlss->pVal = pVal;
   return SUCCESS;
}


int IThread_GetTLS(const char *cpszKey, void **ppVal)
{
   AEETls     *me;
   AEETlsSlot *ptlss;

   GETTLS(me);

   if ((AEETls *)0 == me) {
      return EFAILED;
   }

   ptlss = AEETls_Find(me, cpszKey);

   if ((AEETlsSlot *)0 == ptlss) {
      return EFAILED;
   }

   *ppVal = ptlss->pVal;
   
   return SUCCESS;
   
}
