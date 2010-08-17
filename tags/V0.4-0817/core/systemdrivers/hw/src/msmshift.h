#ifndef MSMSHIFT_H
#define MSMSHIFT_H

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//                MACROS DEFINITIONS
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/*---------------------------------------------------------------------------
                           SHIFT_TEST

This macro returns a 1 (TRUE) if the specified bit in the mask is equal to 1,
else it returns a 0.

INPUT:
    val         = MASK
    shift       = Specified bit

NOTE: See SHIFT_FROM_MASK
---------------------------------------------------------------------------*/
#define SHIFT_TEST( val , shift ) ( (val) & (1U << (shift)) )

/*---------------------------------------------------------------------------
                           SHIFT_GET

This macro is used by SHIFT_FROM_MASK to test if the specified bit in the mask
is equal 1 or 0. If the specified bit equal to 1 then it returns # of shifts else
check the next bit.

INPUT:
    val         = MASK
    shift       = Current bit
    next_shift  = Next bit

NOTE: See SHIFT_FROM_MASK
----------------------------------------------------------------------------*/
#define SHIFT_GET( val, shift, next_shift) (SHIFT_TEST((val),(shift)) ?   \
(shift) : (next_shift))

/*----------------------------------------------------------------------------
                           SHIFT_FROM_MASK

This macro uses the register mask definitions in msmXXXXreg.h to calculate
how many times a bit value needs to be shifted so that it modifies the correct
bit within a register.

MACRO FORMAT:
        if ((MASK && 0x01) != 0)
            return 0;                       //if 1st bit = 1 then do not shift
        else
            if ((MASK && 0x02) != 0)
                return 1;                   //if 2nd bit = 1 then shift once
            else
                if ((MASK && 0x04) != 0)
                    return 2;               //if 3rd bit = 1 then shift twice
                else
                    if ((MASK && 0x08) != 0)
                        return 3;           //if 4th bit = 1 then shift three
                    else
                            ......          //we check until bit 31
                            ......
                            ......


INPUT:
    x   = MASK

NOTE:   ONLY A MASK DEFINITION CAN BE USE AS AN INPUT FOR THIS MACRO!.
        THIS MACRO PRODUCES NO CODE, IT RETURNS A CONSTANT!.
---------------------------------------------------------------------------*/
#define SHIFT_FROM_MASK(x) (SHIFT_TEST(x##_MASK,0) ? 0 : \
                             (SHIFT_GET(x##_MASK,1, \
                              (SHIFT_GET(x##_MASK,2, \
                               (SHIFT_GET(x##_MASK,3, \
                                (SHIFT_GET(x##_MASK,4, \
                                 (SHIFT_GET(x##_MASK,5, \
                                  (SHIFT_GET(x##_MASK,6, \
                                   (SHIFT_GET(x##_MASK,7, \
                                    (SHIFT_GET(x##_MASK,8, \
                                     (SHIFT_GET(x##_MASK,9, \
                                      (SHIFT_GET(x##_MASK,10, \
                                       (SHIFT_GET(x##_MASK,11, \
                                        (SHIFT_GET(x##_MASK,12, \
                                         (SHIFT_GET(x##_MASK,13, \
                                          (SHIFT_GET(x##_MASK,14, \
                                           (SHIFT_GET(x##_MASK,15, \
                                            (SHIFT_GET(x##_MASK,16, \
                                             (SHIFT_GET(x##_MASK,17, \
                                              (SHIFT_GET(x##_MASK,18, \
                                               (SHIFT_GET(x##_MASK,19, \
                                                (SHIFT_GET(x##_MASK,20, \
                                                 (SHIFT_GET(x##_MASK,21, \
                                                  (SHIFT_GET(x##_MASK,22, \
                                                   (SHIFT_GET(x##_MASK,23, \
                                                    (SHIFT_GET(x##_MASK,24, \
                                                     (SHIFT_GET(x##_MASK,25, \
                                                      (SHIFT_GET(x##_MASK,26, \
                                                       (SHIFT_GET(x##_MASK,27, \
                                                        (SHIFT_GET(x##_MASK,28, \
                                                         (SHIFT_GET(x##_MASK,29, \
                                                          (SHIFT_GET(x##_MASK,30, \
                                                           (SHIFT_GET(x##_MASK,31,0) \
                           ) )))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))

/*----------------------------------------------------------------------------
                           SHIFT_DN_FROM_MASK

This macro uses the register mask definitions in msmXXXXreg.h to calculate
how many times a bit value needs to be shifted so that it modifies the correct
bit within a register.

The design is identical to SHIFT_FROM_MASK except that the full name of the
mask is used.  This is needed for use in existing that already take the full
mask name as a parameter.

INPUT:
    x   = MASK

NOTE:   ONLY A MASK DEFINITION CAN BE USE AS AN INPUT FOR THIS MACRO!.
        THIS MACRO PRODUCES NO CODE, IT RETURNS A CONSTANT!.
---------------------------------------------------------------------------*/
#define SHIFT_DN_FROM_MASK(x)     \
        ((x & 0x00000001) ? 0 :    \
         ((x & 0x00000002) ? 1 :    \
          ((x & 0x00000004) ? 2 :    \
           ((x & 0x00000008) ? 3 :    \
            ((x & 0x00000010) ? 4 :    \
             ((x & 0x00000020) ? 5 :    \
              ((x & 0x00000040) ? 6 :    \
               ((x & 0x00000080) ? 7 :    \
                ((x & 0x00000100) ? 8 :    \
                 ((x & 0x00000200) ? 9 :    \
                  ((x & 0x00000400) ? 10 :   \
                   ((x & 0x00000800) ? 11 :   \
                    ((x & 0x00001000) ? 12 :   \
                     ((x & 0x00002000) ? 13 :   \
                      ((x & 0x00004000) ? 14 :   \
                       ((x & 0x00008000) ? 15 :   \
                        ((x & 0x00010000) ? 16 :   \
                         ((x & 0x00020000) ? 17 :   \
                          ((x & 0x00040000) ? 18 :   \
                           ((x & 0x00080000) ? 19 :   \
                            ((x & 0x00100000) ? 20 :   \
                             ((x & 0x00200000) ? 21 :   \
                              ((x & 0x00400000) ? 22 :   \
                               ((x & 0x00800000) ? 23 :   \
                                ((x & 0x01000000) ? 24 :   \
                                 ((x & 0x02000000) ? 25 :   \
                                  ((x & 0x04000000) ? 26 :   \
                                   ((x & 0x08000000) ? 27 :   \
                                    ((x & 0x10000000) ? 28 :   \
                                     ((x & 0x20000000) ? 29 :   \
                                      ((x & 0x40000000) ? 30 :   \
                                       ((x & 0x80000000) ? 31 : 0 \
                                       ))))))))))))))))))))))))))))))))
/*----------------------------------------------------------------------------
                           SHIFT_UP_FROM_MASK

These series of macros return a count of the number of shifts required to shift
a value to the MSBs of an 8-bit, 16-bit or 32-bit value.  The anticipated use
of these macros are to shift a value up and back in order to sign extend the
value.  The macros work similarly to the SHIFT_FROM_MASK macro except that the
mask bits are tested starting with the MSB.

Unlike the SHIFT_FROM_MASK macro, this macro requires the "_MASK" suffix for
mask constants.  This is so that it can be used with mask constants other
than those defined in msm6050reg.h.

E.g.:
        if ((MASK && 0x8000) != 0)
            return 0;                       //if MSB = 1 then do not shift
        else
            if ((MASK && 0x4000) != 0)
                return 1;                   //if MSB-1 = 1 then shift once
            else
                if ((MASK && 0x2000) != 0)
                    return 2;               //if MSB-2 = 1 then shift twice
                else
                    if ((MASK && 0x1000) != 0)
                        return 3;           //if MSB-3 = 1 then shift three
                    else
                            ......          //we check until bit 31
                            ......
                            ......


INPUT:
    x   = MASK

NOTE:   ONLY MASK DEFINITIONS CAN BE USE AS AN INPUTS FOR THESE MACROS!.
        THESE MACROS PRODUCE NO CODE, THEY RETURN CONSTANTS!.
---------------------------------------------------------------------------*/
/* Byte version of SHIFT_UP_FROM_MASK macro: */
#define SHIFT_UP_FROM_MASK_B(x) \
        ((x & 0x80) ? 0 :        \
         ((x & 0x40) ? 1 :        \
          ((x & 0x20) ? 2 :        \
           ((x & 0x10) ? 3 :        \
            ((x & 0x08) ? 4 :        \
             ((x & 0x04) ? 5 :        \
              ((x & 0x02) ? 6 :        \
               ((x & 0x01) ? 7 : 0      \
               ))))))))

/* Half word (16-bit) version of SHIFT_UP_FROM_MASK macro: */
#define SHIFT_UP_FROM_MASK_H(x) \
        ((x & 0x8000) ? 0 :      \
         ((x & 0x4000) ? 1 :      \
          ((x & 0x2000) ? 2 :      \
           ((x & 0x1000) ? 3 :      \
            ((x & 0x0800) ? 4 :      \
             ((x & 0x0400) ? 5 :      \
              ((x & 0x0200) ? 6 :      \
               ((x & 0x0100) ? 7 :      \
                ((x & 0x0080) ? 8 :      \
                 ((x & 0x0040) ? 9 :      \
                  ((x & 0x0020) ? 10 :     \
                   ((x & 0x0010) ? 11 :     \
                    ((x & 0x0008) ? 12 :     \
                     ((x & 0x0004) ? 13 :     \
                      ((x & 0x0002) ? 14 :     \
                       ((x & 0x0001) ? 15 : 0   \
                       ))))))))))))))))

/* Word (32-bit) version of SHIFT_UP_FROM_MASK macro: */
#define SHIFT_UP_FROM_MASK_W(x)   \
        ((x & 0x80000000) ? 0 :    \
         ((x & 0x40000000) ? 1 :    \
          ((x & 0x20000000) ? 2 :    \
           ((x & 0x10000000) ? 3 :    \
            ((x & 0x08000000) ? 4 :    \
             ((x & 0x04000000) ? 5 :    \
              ((x & 0x02000000) ? 6 :    \
               ((x & 0x01000000) ? 7 :    \
                ((x & 0x00800000) ? 8 :    \
                 ((x & 0x00400000) ? 9 :    \
                  ((x & 0x00200000) ? 10 :   \
                   ((x & 0x00100000) ? 11 :   \
                    ((x & 0x00080000) ? 12 :   \
                     ((x & 0x00040000) ? 13 :   \
                      ((x & 0x00020000) ? 14 :   \
                       ((x & 0x00010000) ? 15 :   \
                        ((x & 0x00008000) ? 16 :   \
                         ((x & 0x00004000) ? 17 :   \
                          ((x & 0x00002000) ? 18 :   \
                           ((x & 0x00001000) ? 19 :   \
                            ((x & 0x00000800) ? 20 :   \
                             ((x & 0x00000400) ? 21 :   \
                              ((x & 0x00000200) ? 22 :   \
                               ((x & 0x00000100) ? 23 :   \
                                ((x & 0x00000080) ? 24 :   \
                                 ((x & 0x00000040) ? 25 :   \
                                  ((x & 0x00000020) ? 26 :   \
                                   ((x & 0x00000010) ? 27 :   \
                                    ((x & 0x00000008) ? 28 :   \
                                     ((x & 0x00000004) ? 29 :   \
                                      ((x & 0x00000002) ? 30 :   \
                                       ((x & 0x00000001) ? 31 : 0 \
                                       ))))))))))))))))))))))))))))))))

#endif /* MSMSHIFT_H */
