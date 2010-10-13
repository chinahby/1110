#include "customer.h"

   EXPORT OEMMemCache_ArchClean
   
#ifdef T_MSM7500
   IMPORT memory_barrier
   IMPORT mmu_invalidate_instruction_cache
   IMPORT dcache_clean_all
#endif

   PRESERVE8
   AREA OEMMEMCACHEASM, CODE, READONLY
   CODE32

MMU_WRITEBACK_CACHE  EQU     1
MMU_FLUSH_DATACACHE  EQU     2
MMU_FLUSH_INSTCACHE  EQU     4

;=======================================================================
;
;=======================================================================
OEMMemCache_ArchClean
   AND    r3, r2, #MMU_WRITEBACK_CACHE
   CMP    r3, #0
   BEQ    EraseCache

SyncCache
#ifdef T_MSM7500
   STMFD  sp!, {lr, r5}
   LDR    r5, =dcache_clean_all
   BLX    r5
   LDR    r5, =memory_barrier
   BLX    r5
   LDMFD  sp!, {lr, r5}
#else
   MRC    p15, 0, pc, c7, c14, 3
   BNE    SyncCache
#endif
   B      CheckICache

EraseCache
   AND    r3, r2, #MMU_FLUSH_DATACACHE
   CMP    r3, #0
   BEQ    CheckICache

EraseDCache
   MOV    r0, #0
   MCR    p15, 0, r0, c7, c6, 0

CheckICache
   AND    r3, r2, #MMU_FLUSH_INSTCACHE
   CMP    r3, #0
   BEQ    FlushCache_Ret

EraseICache
   MOV    r0, #0 
#ifdef T_MSM7500 
   STMFD  sp!, {lr, r5}
   LDR    r5, =mmu_invalidate_instruction_cache
   BLX    r5
   LDMFD  sp!, {lr, r5}
#else
   MCR    p15, 0, r0, c7, c5, 0
#endif

FlushCache_Ret
   MOV    r0, #0  ; return SUCCESS
   BX     lr

   END
