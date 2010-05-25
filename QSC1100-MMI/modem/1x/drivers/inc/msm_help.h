#ifndef MSM_HELP_H
#define MSM_HELP_H

/*===========================================================================

              M S M   R E G I S T E R   A C C E S S   H E L P E R S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains macros to support read and writes of MSM registers,
  as well as to access bit-fields within MSM registers.
  
Copyright (c) 1999 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/msm_help.h#3 $



when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/09   jtm     Additional checks for redefined macros
12/07/06   trc     Don't redefine MSM_SET() and MSM_CLEAR() if already defined
03/31/05   bt      Removed unnecessary ## in some macros.
09/30/04   sfm     Mainlined FEATURE_FINGER_DRIVER
09/28/04   ejv     Removed old/unused macros.
09/03/04   sfm     Finger Driver changes
06/24/04   sfm     Removed MSM_FINGER_SELECT() and MSM_FINGER_UNSELECT() macros
                   if FEATURE_FINGER_DRIVER is defined
04/21/04   ejv     In MSM_LOCK( ), changed word to uint32.  Removed unused
                   shadow macros.
02/25/04   kwo     Lint cleanup for MSM_NOT_USED()
02/24/04   ddh     Removed MSM_W2B, MSM_W2H, MSM_W3B, MSM_W3H, MSM_W6B since
                   they are no longer used
02/24/04   ddh     Cleaned up lint disable/enable
02/12/04   sfm     Suppressed a lint warning in MSM_W_MASKED()
01/28/04   ejv     Mainline T_MSM6500.
10/28/03   ddh     Added macro to use macro defined variables which might
                   not be used in subsequent macro calls to clean up compiler
                   warnings
08/11/03   fu/aaj  Added macro to enable RX diversity
07/07/03   ejv     Mainline FEATURE_SIX_FINGERS.
06/17/03   ejv     Added additional 6500+ macros.
04/16/03   aar     Added macro MSM_R2W which reads 32-bit val from 2 16-bit
                   registers.
01/16/03   ejv     Changed mn indices to ij.
12/26/02   ejv     Added first cut of new macros for MSM6500.
10/02/02   rm      Added support for 6 fingers.
09/06/02   gs      Fix MSM_WW macro dword cast...missed in 7/30 merge.
08/06/02   gs      Merge in support for new shadow scheme.
07/30/02   gs      Initial merge into perforce for cougar.
07/05/02   bt      Change type cast of MSM_WW macros to dword
07/01/02   ejv     Back out last change to MSM_LOCK.
06/28/02   ejv     Save 32 bits in MSM_LOCK, rather than 18.
02/28/02   ejv     Added more 32 bit macros.  Cleanup.
02/14/02   rm      Added support for 32 bit shadow writes.
01/18/01   ejv     Added some more macros to support new macros in msm_drv.h.
08/31/00   rmd     Changed the shadow register code so that it supports 
                   8 bit and 16 bit registers in one array.
01/13/00   ajn     Added the MSM_UNLOCK_AND_LOCK macro.
11/10/99   ejv     Added MSM_RH_MASKED macro.
08/12/99   ejv     Lint cleanup.
08/06/99   ejv     Merged jcw's changes for initial Tiger2 support.
07/23/99   ajn     First cut of file.

===========================================================================*/

#include "msm.h"

/*===========================================================================

                            LOCK/UNLOCK MACROS

===========================================================================*/


/*===========================================================================

MOTIVATION:

  The MSM_LOCK and MSM_UNLOCK macros are used to provide uninterrupted
  access to sets of MSM registers and the MSM shadow registers.
  
  In the past, DMOD_(F)_IN/OUT(H)(M) macros would provide the appropriate
  interrupt locking around the specific msm register, to assure proper
  operation.  For example, the DMOD_F_OUT macro would lock interrupts,
  select the appropriate finger page, output the quantity to the desired
  address, and finally restore interrupt processing.  However, this leads
  to inefficient code.  Consider:

        DMOD_F_OUT( f, DMOD_SLEW_0, B_PTR(slew)[0] );
        DMOD_F_OUT( f, DMOD_SLEW_1, B_PTR(slew)[1] );
        DMOD_F_OUT( f, DMOD_SLEW_2, B_PTR(slew)[2] );
        
  This code translates into:
        INTLOCK( );
          DMOD_OUT( DMOD_F_PAGE, 1 << f );
          DMOD_OUT( DMOD_SLEW_0, B_PTR(slew)[0] );
        INTFREE( );
        INTLOCK( );
          DMOD_OUT( DMOD_F_PAGE, 1 << f );
          DMOD_OUT( DMOD_SLEW_1, B_PTR(slew)[1] );
        INTFREE( );
        INTLOCK( );
          DMOD_OUT( DMOD_F_PAGE, 1 << f );
          DMOD_OUT( DMOD_SLEW_2, B_PTR(slew)[2] );
        INTFREE( );
        
  Interrupts are being locked and unlocked 3 times, and the finger page
  register is being written to 3 times with the same value.  Because
  INTLOCK( ) / INTFREE( ) translates to an extra stack level with a local
  "sav" variable, and function call to REX, the temporary "1 << f" may have
  to be computed 3 times.
  
  Clearly, it would be more efficient to write the following:
        INTLOCK( );
          DMOD_OUT( DMOD_F_PAGE, 1 << f );
          DMOD_OUT( DMOD_SLEW_0, B_PTR(slew)[0] );
          DMOD_OUT( DMOD_SLEW_1, B_PTR(slew)[1] );
          DMOD_OUT( DMOD_SLEW_2, B_PTR(slew)[2] );
        INTFREE( );
   
  More over, this even avoids a subtle bug in that the DMOD_SLEW_0 and
  DMOD_SLEW_1 registers are not actually finger paged registers, and if an
  interrupt between an INTFREE( )/INTLOCK( ) slews a finger, when the
  interrupt returns the original slew command is corrupted.
  
  To this end, MSM register access macros have been designed to NOT
  internally perform INTLOCK/INTFREE operations, but rather rely on the
  calling procedure to provide MSM_LOCK( ) and MSM_FREE( ) calls surrounding
  the MSM register access macros, to create smaller, faster, more robust
  code.

===========================================================================*/


/*===========================================================================

MACRO MSM_LOCK_REQUIRED_DEFN
MACRO MSM_LOCK_REQUIRED

  These macros are used to help prevent against accidental use of MSM
  register access macros which require use of a surrounding MSM_LOCK and
  MSM_UNLOCK context, without the appropriate surrounding context.
  
  The MSM_LOCK_REQUIRED_DEFN macro, used in MSM_LOCK( ), declares a local
  scope variable, "MSM_LOCK_required".  The variable will go out of scope at
  the corresponding MSM_UNLOCK( ) macro.
  
  The MSM_LOCK_REQUIRED macro assigns "MSM_LOCK_required" to itself.  This
  requires the compiler to know of the existance of the "MSM_LOCK_required"
  variable, but otherwise does not generate any code.  The MSM_LOCK_REQUIRED
  macro is then included in any MSM register access macro that requires
  interrupts to be disabled for proper operation.  If that macro is called
  without the surrounding MSM_LOCK/MSM_UNLOCK pair, the compiler will
  generate an error message about an undefined variable "MSM_LOCK_required"
  at that point.  The developer would then have to modify the procedure to
  call MSM_LOCK and MSM_UNLOCK around the register access.
  

  If it is determined that the MSM_LOCK_REQUIRED_DEFN / MSM_LOCK_REQUIRED
  macros actually increase the code size for a particular version of the
  compiler or level of optimization, once the code has been deemed correct,
  the macro DISABLE_MSM_LOCK_REQUIRED_CHECKING may be defined, to compile
  out the MSM_LOCK_REQUIRED code.

===========================================================================*/


#undef DISABLE_MSM_LOCK_REQUIRED_CHECKING
  /* Define this macro if it is determined that executable code size has
     increased due to the checking */


#ifdef DISABLE_MSM_LOCK_REQUIRED_CHECKING

#ifndef MSM_LOCK_REQUIRED_DEFN
#define MSM_LOCK_REQUIRED_DEFN
  /* No code here */
#endif
  
#ifndef MSM_LOCK_REQUIRED
#define MSM_LOCK_REQUIRED
  /* No code here */
#endif


#else /* MSM_LOCK_REQUIRED checking is enabled */

#ifndef MSM_LOCK_REQUIRED_DEFN
#define MSM_LOCK_REQUIRED_DEFN  \
    boolean MSM_LOCK_required = TRUE;
#endif

#ifndef MSM_LOCK_REQUIRED
#define MSM_LOCK_REQUIRED                   \
    /*lint -save -e548 else expected*/      \
    MSM_LOCK_required = MSM_LOCK_required;  \
    /*lint -restore */
#endif

#endif



/*===========================================================================

MACRO MSM_LOCK

DESCRIPTION     This macro int-locks the microprocessor for MSM register
                input/output operations.

DEPENDENCIES    It must be followed by a MSM_UNLOCK in the same function,
                at the same block-nesting level.

RETURN VALUE    None.
      
SIDE EFFECTS    Interrupts are blocked until MSM_UNLOCK( ).

===========================================================================*/
#ifndef MSM_LOCK
#define MSM_LOCK( )    \
    { \
      MSM_LOCK_REQUIRED_DEFN \
      uint32 msm_sav; \
      INTLOCK_SAV( msm_sav )
#endif

/*===========================================================================

MACRO MSM_UNLOCK

DESCRIPTION     This macro undoes the locking done by the MSM_LOCK( ) macro.

DEPENDENCIES    It must be followed by a MSM_UNLOCK in the same function,
                at the same block-nesting level.

RETURN VALUE    None.
      
SIDE EFFECTS    Interrupts processing is restored to previous state.

===========================================================================*/
#ifndef MSM_UNLOCK
#define MSM_UNLOCK( )    \
    MSM_LOCK_REQUIRED \
    INTFREE_SAV( msm_sav ); }
#endif

/*===========================================================================

MACRO MSM_UNLOCK_AND_LOCK

DESCRIPTION     This macro undoes the locking done by the MSM_LOCK( ) macro
      and then quickly redoes the lock.

DEPENDENCIES    It must be preceded by an MSM_LOCK and then followed by an
      MSM_UNLOCK in the same function.

RETURN VALUE    None.
      
SIDE EFFECTS    Interrupts processing is restored to previous state.

===========================================================================*/
#ifndef MSM_UNLOCK_AND_LOCK
#define MSM_UNLOCK_AND_LOCK( )    \
    MSM_LOCK_REQUIRED \
    INTFREE_SAV( msm_sav ), \
    INTLOCK_SAV( msm_sav )
#endif


/*===========================================================================

                            FINGER SHADOW MEMORY

===========================================================================*/
#ifndef MSM_NUM_FINGERS
#define MSM_NUM_FINGERS   6                     /* Number of fingers */
#endif

/*===========================================================================

                            BIT MASK HELPER MACROS

===========================================================================*/


/* Masking operation helper macros */
#ifndef MSM_MASK_VALUE
#define MSM_MASK_VALUE( size, reg, mask, val ) \
    MSM_LOCK_REQUIRED \
    (reg = (reg & (size)(~(mask))) | ((size)((val) & (mask))))
#endif

#ifndef MSM_MASK_SET
#define MSM_MASK_SET( size, reg, mask ) \
    MSM_LOCK_REQUIRED \
    (reg |= (size)(mask))
#endif

#ifndef MSM_MASK_CLEAR
#define MSM_MASK_CLEAR( size, reg, mask ) \
    MSM_LOCK_REQUIRED \
    (reg &= (size)(~(mask)))
#endif

#ifndef MSM_MASK_STROBE_HIGH
#define MSM_MASK_STROBE_HIGH( size, reg, mask ) \
    (reg | (size)(mask))
#endif

#ifndef MSM_MASK_STROBE_LOW
#define MSM_MASK_STROBE_LOW( size, reg, mask ) \
    (reg & (size)(~(mask)))
#endif


/*===========================================================================

                       BIT MASK MACROS (16-bit registers)

   In the following macros, the field parameter is appended to the base
   address parameter in order to generate the field mask literal name.
   
   For example, for a base of XXX and a field of YYY, the address parameter
   would expand to "XXX_WH" (16-bit write register) and the mask parameter
   would expand to "XXX_WH__YYY_MASK" (field of the 16-bit write register.)

===========================================================================*/

/*===========================================================================

MACRO MSM_WH_SET

DESCRIPTION     This macro sets a specific bit in a "_WH" style 16-bit msm
                register.
                
                "_WH" is appended to the address to ensure that the macro
                cannot be used with 8-bit registers or input registers.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image_H[ ]) is updated.

===========================================================================*/

#ifndef MSM_WH_SET
#define MSM_WH_SET( base, field ) \
    MSM_MASK_SET( word, HWIO_SHDW(base), HWIO_FMSK(base,field) ), \
    MSM_W(base, HWIO_SHDW(base) )
#endif

/*===========================================================================

MACRO MSM_WH_CLEAR

DESCRIPTION     This macro clears a specific bit in a "_WH" style 16-bit msm
                register.
                
                "_WH" is appended to the address to ensure that the macro
                cannot be used with 8-bit registers or input registers.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image_H[ ]) is updated.

===========================================================================*/

#ifndef MSM_WH_CLEAR
#define MSM_WH_CLEAR( base, field ) \
    MSM_MASK_CLEAR( word, HWIO_SHDW(base), HWIO_FMSK(base,field) ), \
    MSM_W(base, HWIO_SHDW(base) )
#endif


/*===========================================================================

                     SIZE-INDEPENDENT MACROS FOR MSM6500+

===========================================================================*/

/*===========================================================================

MACRO MSM_INIT

DESCRIPTION     This macro zeros all bits in a direct access msm
                register and the corresponding shadow register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/

#ifndef MSM_INIT
#define MSM_INIT( base ) \
    MSM_LOCK_REQUIRED \
    HWIO_OUT( base, 0x0 )
#endif

/*===========================================================================

MACRO MSM_INIT_i

DESCRIPTION     This macro zeros all bits in a single array access msm
                register and the corresponding shadow register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_INIT_i
#define MSM_INIT_i( base, n ) \
    MSM_LOCK_REQUIRED \
    HWIO_OUTI( base, n, 0x0 )
#endif

/*===========================================================================

MACRO MSM_W

DESCRIPTION     This macro writes to a direct access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only writes to one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_W
#define MSM_W( base, val ) \
    HWIO_OUT( base, val )
#endif

/*===========================================================================

MACRO MSM_W_i

DESCRIPTION     This macro writes to a single array access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only writes to one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_W_i
#define MSM_W_i( base, val, n ) \
    HWIO_OUTI( base, n, val )
#endif

/*===========================================================================

MACRO MSM_W_ij

DESCRIPTION     This macro writes to a double array access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only writes to one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_W_ij
#define MSM_W_ij( base, val, m, n ) \
    HWIO_OUTI2( base, m, n, val )
#endif

/*===========================================================================

MACRO MSM_R

DESCRIPTION     This macro reads from a direct access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R
#define MSM_R( base ) \
    HWIO_IN( base )
#endif

/*===========================================================================

MACRO MSM_R_i

DESCRIPTION     This macro reads from a single array access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R_i
#define MSM_R_i( base, n ) \
    HWIO_INI( base, (n) )
#endif

/*===========================================================================

MACRO MSM_R_ij

DESCRIPTION     This macro reads from a single array access msm register.
                
                The address must be passed in as a literal.  Variables are
                not allowed.

DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R_ij
#define MSM_R_ij( base, m, n ) \
    HWIO_INI2( base, (m), (n) )
#endif

/*===========================================================================

                       BIT MASK MACROS

   In the following macros, the field parameter is appended to the base
   address parameter in order to generate the field mask literal name.

===========================================================================*/

/*===========================================================================

MACRO MSM_W_MASKED

DESCRIPTION     This macro alters a subset of bits in a direct
                access msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_W_MASKED
#define MSM_W_MASKED( base, field, val )                                    \
    { /*lint -save -e703 -e701 Shift left of signed quantity intentional */ \
      HWIO_OUTM( base, HWIO_FMSK(base,field),                         \
                 val << HWIO_SHFT(base,field));                       \
    } /*lint -restore */
#endif

/*===========================================================================

MACRO MSM_W_MASKED_i

DESCRIPTION     This macro alters a subset of bits in a single array
                access msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_W_MASKED_i
#define MSM_W_MASKED_i( base, field, val, n )                          \
    { /*lint -save  -e701 Shift left of signed quantity intentional */ \
      HWIO_OUTMI( base, n, HWIO_FMSK(base,field),                \
                  val << HWIO_SHFT(base,field));                 \
    } /*lint -restore */
#endif

/*===========================================================================

MACRO MSM_W_MASKED_ij

DESCRIPTION     This macro alters a subset of bits in a double array
                access msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_W_MASKED_ij
#define MSM_W_MASKED_ij( base, field, val, m, n ) \
    { \
      HWIO_OUTMI2( base, m, n, HWIO_FMSK(base,field), val << HWIO_SHFT(base,field)); \
    }
#endif

/*===========================================================================

MACRO MSM_R_MASKED

DESCRIPTION     This macro reads from a direct access msm register.
                The "field" mask is then used to mask off the
                undesired bits
                
                The address must be passed in as a literal.  Variables are
                not allowed.
                
DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value & MASK.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R_MASKED
#define MSM_R_MASKED( base, field ) \
    ( HWIO_INM ( base, HWIO_FMSK(base,field)) >> \
              HWIO_SHFT(base,field) )
#endif

/*===========================================================================

MACRO MSM_R_MASKED_i

DESCRIPTION     This macro reads from a single array access msm register.
                The "field" mask is then used to mask off the
                undesired bits
                
                The address must be passed in as a literal.  Variables are
                not allowed.
                
DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value & MASK.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R_MASKED_i
#define MSM_R_MASKED_i( base, field, n ) \
    ( HWIO_INMI( base, n, HWIO_FMSK(base,field)) >> \
                HWIO_SHFT(base,field) )
#endif

/*===========================================================================

MACRO MSM_R_MASKED_ij

DESCRIPTION     This macro reads from a double array access msm register.
                The "field" mask is then used to mask off the
                undesired bits
                
                The address must be passed in as a literal.  Variables are
                not allowed.
                
DEPENDENCIES    Since this macro only reads from one memory location, and does
                not update any shadow registers, it may be used outside the
                context of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    Register value & MASK.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_R_MASKED_ij
#define MSM_R_MASKED_ij( base, field, m, n ) \
    ( HWIO_INMI2( base, m, n, HWIO_FMSK(base,field)) >> \
                 HWIO_SHFT(base,field) )
#endif

/*===========================================================================

                   ADDITIONAL BIT MASK MACROS

===========================================================================*/

/*===========================================================================

MACRO MSM_SET

DESCRIPTION     This macro sets a specific bit in a direct access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/

#ifndef MSM_SET
#define MSM_SET( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), HWIO_FMSK(base,field))
#endif

/*===========================================================================

MACRO MSM_SET_i

DESCRIPTION     This macro sets a specific bit in a single array access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_SET_i
#define MSM_SET_i( base, field, n ) \
    HWIO_OUTMI( base, n, HWIO_FMSK(base,field), HWIO_FMSK(base,field))
#endif

/*===========================================================================

MACRO MSM_SET_ij

DESCRIPTION     This macro sets a specific bit in a double array access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_SET_ij
#define MSM_SET_ij( base, field, m, n ) \
    HWIO_OUTMI2( base, m, n, HWIO_FMSK(base,field), HWIO_FMSK(base,field))
#endif

/*===========================================================================

MACRO MSM_SET_BIT

DESCRIPTION     This macro sets a specific bit in a direct access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_SET_BIT
#define MSM_SET_BIT( base, bit ) \
    HWIO_OUTM( base, (1<<bit), (1<<bit) )
#endif

/*===========================================================================

MACRO MSM_CLEAR

DESCRIPTION     This macro clears a specific bit in a direct access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/

#ifndef MSM_CLEAR
#define MSM_CLEAR( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), 0 )
#endif

/*===========================================================================

MACRO MSM_CLEAR_i

DESCRIPTION     This macro clears a specific bit in a single array access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_CLEAR_i
#define MSM_CLEAR_i( base, field, n ) \
    HWIO_OUTMI( base, n, HWIO_FMSK(base,field), 0 )
#endif

/*===========================================================================

MACRO MSM_CLEAR_ij

DESCRIPTION     This macro clears a specific bit in a double array access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_CLEAR_ij
#define MSM_CLEAR_ij( base, field, m, n ) \
    HWIO_OUTMI2( base, m, n, HWIO_FMSK(base,field), 0 )
#endif

/*===========================================================================

MACRO MSM_CLEAR

DESCRIPTION     This macro clears a specific bit in a direct access
                msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_CLEAR_BIT
#define MSM_CLEAR_BIT( base, bit ) \
    HWIO_OUTM( base, (1<<bit), 0 )
#endif


/*===========================================================================

MACRO MSM_PULSE

DESCRIPTION     This macro pulses (sets high and then low) a specific bit in
                a direct access msm register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_PULSE
/* Keep in mind that we may need some delay between set & clear */
#define MSM_PULSE( base, field ) \
    MSM_SET( base, field ); \
    MSM_CLEAR( base, field )
#endif

/*===========================================================================

MACRO MSM_SET_MASK

DESCRIPTION     This macro sets a specified mask of bits.
                
                The base address and mask parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_SET_MASK
#define MSM_SET_MASK( base, mask ) \
    HWIO_OUTM( base, mask, mask )
#endif
    
/*===========================================================================

MACRO MSM_CLEAR_MASK

DESCRIPTION     This macro clears a specified mask of bits.
                
                The base address and mask parameters must be passed in as
                literals.  Variables are not allowed.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    The corresponding shadow register (MSM_image[ ]) is updated.

===========================================================================*/
#ifndef MSM_CLEAR_MASK
#define MSM_CLEAR_MASK( base, mask ) \
    HWIO_OUTM( base, mask, 0 )
#endif

/*===========================================================================

MACRO MSM_GET_BIT

DESCRIPTION     This macro returns the value of a specified bit in a
                specified register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_GET_BIT
#define MSM_GET_BIT( base, bit ) \
   ( HWIO_INM( base, (1<<bit) ) >> bit )
#endif    

/*===========================================================================

MACRO MSM_GET

DESCRIPTION     This macro returns the value of a specified field in a
                specified register.
                
                The base address and field parameters must be passed in as
                literals.  Variables are not allowed.
                
DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_GET
#define MSM_GET( base, field ) \
   ( HWIO_INM ( base, HWIO_FMSK(base,field)) \
                         >> HWIO_SHFT(base,field) )
#endif

/*===========================================================================

MACRO MSM_NOT_USED

DESCRIPTION     This macro uses the variables defined in a macro, to 
                clearup the compiler warnings for unused variables.
                
DEPENDENCIES    None

RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/
#ifndef MSM_NOT_USED
#define MSM_NOT_USED( i ) if(i) {}
#endif

#ifdef FEATURE_CDMA_RX_DIVERSITY
/*===========================================================================

MACRO MSM_FINGER_SET_RX_DIVERSITY

DESCRIPTION     This macro sets the _rx_div flag defined in the 
                MSM_FINGER_SELECT macro. This macro should be called
                whenever MSM_FINGER_SELECT() macro is called if the function
                calling MSM_FINGER_SELECT() macro may operate in rx diversity 
                enviroment.

DEPENDENCIES    This function must only be called from inside the context
                of a MSM_LOCK / MSM_UNLOCK pair.
                
                It must be followed by a MSM_FINGER_UNSELECT in the same
                function at the same block-nesting level.
                
RETURN VALUE    None.
      
SIDE EFFECTS    May result in writing Diversity settings in subsequent 
                finger programming operations

===========================================================================*/
#ifndef MSM_FINGER_SET_RX_DIVERSITY
#define MSM_FINGER_SET_RX_DIVERSITY( div_flag ) \
    _rx_div = div_flag
#endif

#endif /* FEATURE_CDMA_RX_DIVERSITY */

/*===========================================================================

                               End of Interface

===========================================================================*/

#endif /* MSM_HELP_H */
