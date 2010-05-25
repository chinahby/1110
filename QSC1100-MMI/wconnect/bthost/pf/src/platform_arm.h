#ifdef FEATURE_BT_EXTPF_AV

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#ifndef _PLATFORM_ARM_H
#define _PLATFORM_ARM_H

#undef CLIP_INT16
#define CLIP_INT16(a) do { if (a < -32768) { a = -32768; } if (a  > 32767) { a = 32767; } } while (0)

/* This is currently disabled until I verify it still works */
#if 0
#undef DCT2_8
void asm_dct_aan(SBC_BUFFER_T*,OI_INT32*);
#define DCT2_8(dst, src) asm_dct_aan(dst, src)
#endif

/** GCC-specific optimization */
#ifdef GCC

#undef DIVIDE
#define DIVIDE _divide(a,b)

#undef COPY4WORDS_BACK
#define COPY4WORDS_BACK(_dest, _src)                \
    asm volatile (                                  \
              "ldmdb %0!, { r0-r3 }\n\t"            \
              "stmdb %1!, { r0-r3 }"                \
              : "+&r"(_src), "+&r"(_dest)           \
              : "0"(_src), "1"(_dest)               \
              : "r0", "r1", "r2", "r3", "memory")



#endif /* GCC */
    
#ifdef GHS_ARM7 /* Greenhills-specific optimization */

asm long long smull(a, b)
{
%con a %con b
    mov r0, a
    mov r1, b
    smull r0, r1, r0, r1
%con a %reg b
    mov r2, a
    smull r0, r1, r2, b
%con a %mem b
    mov r2, a
    ldr r3, b
    smull r0, r1, r2, r3
%reg a %con b
    mov r3, b
    smull r0, r1, a, r3
%reg a %reg b
    smull r0, r1, a, b
%reg a %mem b
    ldr r3, b
    smull r0, r1, a, r3
%mem a %con b
    ldr r2, a
    mov r3, b
    smull r0, r1, r2, r3
%mem a %reg b
    ldr r2, a
    smull r0, r1, r2, b
%mem a %mem b
    ldr r2, a
    ldr r3, b
    smull r0, r1, r2, r3
}

asm long asm_smull(a,b,t)
{
%reg a %reg b %reg t
    smull r0, t, r2, r3
    mov r0, r0 lsl 16
    orr r0, r0, t lsr 16
}
//    smull r1, r0, u, v      /* r0 := (u*v)[63:32] r1 := (u*v)[31:0] */
//    mov r0, r0 lsl 16   /* r0 := (u*v)[47:32] << 16     -- no overflow since u is 16-bit */
//    orr r0, r0, r1 lsr 16   /* r0 := ((u*v)[47:32] << 16) | (u*v)[31:16]) */

asm void asm_load4(a, b, c, d, addr)
{
%reg a % reg b %reg c %reg d %reg addr
}

#include <stdio.h>
#include <arm_ghs.h>

long _asm_mul_16s_32s_hi(OI_INT16 _x, OI_INT32 _y)
{
    register OI_INT32 x,y,t;
    x = _x;
    y = _y;
    return asm_smull(x, y, t);
}

long _asm_mul_32s_32s_hi(OI_INT32 _x, OI_INT32 _y)
{
    long long s;
    s = smull(_x, _y);
    s >>= 32;
    return (long)s;
}


#undef MUL_16S_32S_HI
#define MUL_16S_32S_HI(_x, _y)  _asm_mul_16s_32s_hi(_x, _y)

#undef MUL_32S_32S_HI
#define MUL_32S_32S_HI(_x, _y) _asm_mul_32s_32s_hi(_x, _y)

#endif /* Greenhills */

#endif /* _PLATFORM_ARM_H */

#endif /* FEATURE_BT_EXTPF_AV */

