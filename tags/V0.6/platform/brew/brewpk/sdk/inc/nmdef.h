#ifndef __NMDEF_H
#define __NMDEF_H

#ifndef  BITPOS_EFL_AC
#define  BITPOS_EFL_AC     18
#endif

#ifndef  BITMASK_EFL_AC
#define  BITMASK_EFL_AC    (1 << BITPOS_EFL_AC)
#endif

#if   defined(_M_IX86) && (_MSC_VER >= 1300)
//
// X86
//
#define  EFLAGS_AC_ON()       {                                         \
                              _asm pushfd                               \
                              _asm or dword ptr [esp], BITMASK_EFL_AC   \
                              _asm popfd                                \
                              }
                              
#define  EFLAGS_AC_OFF()      {                                         \
                              _asm pushfd                               \
                              _asm and dword ptr [esp], ~BITMASK_EFL_AC \
                              _asm popfd                                \
                              }
                              
#define  SAVE_EFL(s)          {                                            \
                                 _asm pushfd                               \
                                 _asm mov s, eax                           \
                                 _asm btr dword ptr [esp], BITPOS_EFL_AC   \
                                 _asm sbb s, eax                           \
                                 _asm and s, BITMASK_EFL_AC                \
                                 _asm popfd                                \
                              }
#define  RESTORE_EFL(s)       {                                            \
                                 _asm pushfd                               \
                                 _asm xchg eax, dword ptr [esp]            \
                                 _asm and  eax, ~BITMASK_EFL_AC            \
                                 _asm or   eax, s                          \
                                 _asm xchg eax, dword ptr [esp]            \
                                 _asm popfd                                \
                              }
#define  AC_BIT_ON(f)         {f |= BITMASK_EFL_AC;}
#define  AC_BIT_OFF(f)        {f &= ~BITMASK_EFL_AC;}
                              
#define  NO_MISALIGN_BEGIN    {unsigned _____efl_saved; SAVE_EFL(_____efl_saved)
#define  NO_MISALIGN_END      RESTORE_EFL(_____efl_saved)}

#ifndef  DO_STACKCHECK
#pragma runtime_checks( "", off )
#endif

#else
//
// Non-X86
//

#define  EFLAGS_AC_ON()          {}
#define  EFLAGS_AC_OFF()         {}
#define  SAVE_EFL(s)             {}
#define  RESTORE_EFL(s)          {}
#define  NO_MISALIGN_BEGIN       {}
#define  NO_MISALIGN_END         {}

#endif   // #if defined(_M_IX86) && (_MSC_VER >= 1300) 

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 110000)
#define  DWORD_ALIGN  __align(4)
#define  QWORD_ALIGN  __align(8)
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define  DWORD_ALIGN  __declspec(align(4))
#define  QWORD_ALIGN  __declspec(align(8))
#else
#define  DWORD_ALIGN
#define  QWORD_ALIGN
#endif

#define  NM_SWITCH_BEGIN(i)      NO_MISALIGN_BEGIN; switch(i)
#define  NM_SWITCH_END           NO_MISALIGN_END
#define  NM_CASE(c)              case (c): RESTORE_EFL(_____efl_saved)
#define  NM_CASE_FALL_THROUGH(c) case (c):
#define  NM_DEFAULT              default: RESTORE_EFL(_____efl_saved)

#endif // __NMDEF_H
