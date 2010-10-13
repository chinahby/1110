/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Action Timer Services

GENERAL DESCRIPTION

  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999 through 2008 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/src/atmr.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_ATMR_TEST
08/30/04   bn      Upon new registering, clear the corresponding pending ATMR bit
08/25/04   jyw     Removed the debuging print
07/27/04   jyw     added the interrupt cleaning for atmr1_isr
08/28/00    et     featurized overloading of QPCH and time 2 interrupt for 5105
02/04/99   jct     Fix to set function so setting TIME_2 works
11/22/99   jct     Created
===========================================================================*/

/* =========================================================================
** Includes
** ========================================================================= */
#include "atmr.h"
#include "tramp.h"
#include "msm.h"
  
/* =========================================================================
** Local Definitions and Declarations
** ========================================================================= */
#define ATMR_FREE_HANDLE    ((atmr_handle_type)0x00000000) 
#define ATMR_SEIZED_HANDLE  ((atmr_handle_type)0xF0000001)

#ifdef T_MSM6700
#define ATMR_GET_TIME_INT1_STATUS() \
    HWIO_IN(DEM1X_TIME_INT1_STATUS)
#define ATMR_GET_TIME_INT2_STATUS() \
    HWIO_IN( DEM1X_TIME_INT2_STATUS)

#define ATMR_SET_TIME_INT1_CLEAR() \
    HWIO_OUT(DEM1X_TIME_INT1_CLEAR, 0xffff)
#define ATMR_SET_TIME_INT2_CLEAR() \
    HWIO_OUT( DEM1X_TIME_INT2_CLEAR, 0xffff )

#define ATMR_SET_TIME_INT1_ENABLE(set_bits) \
    HWIO_OUT(DEM1X_TIME_INT1_ENABLE , (set_bits) )
#define ATMR_SET_TIME_INT2_ENABLE(set_bits) \
    HWIO_OUT( DEM1X_TIME_INT2_ENABLE, (set_bits) )

#define ATMR_GET_TIME_INT1_ENABLE() \
    HWIO_IN(DEM1X_TIME_INT1_ENABLE)
#define ATMR_GET_TIME_INT2_ENABLE() \
    HWIO_IN( DEM1X_TIME_INT2_ENABLE)
#else
#ifdef T_MSM6500
#define ATMR_SET_TIME_INT1_MASK(set_bits) \
    HWIO_OUTM( DEM1X_TIME_INT1_MASK, HWIO_RMSK(DEM1X_TIME_INT1_MASK), (set_bits) )
#define ATMR_SET_TIME_INT2_MASK(set_bits) \
    HWIO_OUTM( DEM1X_TIME_INT2_MASK, \
               HWIO_RMSK(DEM1X_TIME_INT2_MASK), (set_bits) )
#else
#error code not present
#endif /* T_MSM6500 */
#endif /* T_MSM6700 */

static atmr_func_type atmr_funcs[ ATMR_NUM_RESOURCES ] = { 0, 0 };

/* =========================================================================
** Functions
** ========================================================================= */

/*==========================================================================

FUNCTION ATMR_GET

DESCRIPTION
  Given which timer to obtain, asks the action timer driver for a handle to that
  timer.  Possible selections are:
        ATMR_TIME_1
        ATMR_TIME_2
        ATMR_QPCH
  Note that TIME_2 and QPCH share an interrupt resource and therefore
  must be used orthogonally.  
  
  Usage instructions are provided with the comments for atmr_set
  
DEPENDENCIES
  None.

RETURN VALUE
  NULL if the requested resource can not be granted
  A 32 bit handle to the requested resource otherwise

SIDE EFFECTS
  None.

===========================================================================*/
atmr_handle_type atmr_get(
   atmr_timer_enum_type tmr
)
{
   /* Fix up the id, if the user asked for QPCH, then set it to TIME2 since
   ** these are overloaded, otherwise just retain the value which is 0 or 1
   ** or invalid 
   */
//BAMA
#ifndef T_MSM5105
#error code not present
#endif

   /* Validate the tmr is in the valid range, if not return an invalid handle
   */
   if( tmr < ATMR_NUM_RESOURCES )
   {
      /* Verify the resource is free, if not return an invalid handle
      */
      if( atmr_funcs[ tmr ] == ATMR_FREE_HANDLE )
      {
         atmr_funcs[ tmr ] = (atmr_func_type)ATMR_SEIZED_HANDLE;

         /* Return the 32 bit handle to the user, the user will pass
         ** this back in subsequent calls to atmr_set at which time
         ** the address will be mapped back to the atmr_funcs array
         */
         return (atmr_handle_type*)&atmr_funcs[ tmr ];
      }
   }

   /* Reaching here means either the tmr input was invalid or the
   ** requested resource is not available
   */
   return ATMR_INVALID_HANDLE;
   
} /* END atmr_get */


/*==========================================================================

FUNCTION ATMR_TIME_INT1_ISR

DESCRIPTION
  Interrupt service routine for SYS_TIME_INT1
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void atmr_time_int1_isr( void )
{
   uint32 atmr_int_status;
   atmr_int_status = ATMR_GET_TIME_INT1_STATUS();
   ATMR_SET_TIME_INT1_CLEAR();
   atmr_funcs[ 0 ]();
} /* END atmr_time_int1_isr */

/*==========================================================================

FUNCTION ATMR_TIME_INT2_ISR

DESCRIPTION
  Interrupt service routine for SYS_TIME_INT2
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void atmr_time_int2_isr( void )
{
   /* JYW
      The interrupt cleaning actually happens at the service level
      we may move the interrupt cleaning to this level later 
   */
   uint32 atmr_int_status;
   atmr_int_status=ATMR_GET_TIME_INT2_STATUS();
   atmr_funcs[ 1 ]();
} /* END atmr_time_int2_isr */


/*==========================================================================

FUNCTION ATMR_SET

DESCRIPTION
  Given a timer interrupt routine, the time line is set up to call the
  routine at the given PCG and 64 chip symbol offset from the specified 5ms
  frame.
  
  Helper macros are provided for selecting the 5ms frame and which PCG
  from that frame.  The symbol offset is in 64chips.  
  
  Users must first obtain a handle to a timer via a call to atmr_get.
  
  Required usage is:
  
    atmr_handle_type handle;
    handle = atmr_get( ATMR_TIME_1 );
    if( handle != ATMR_INVALID_HANDLE )
    {
        atmr_set( 
           handle,
           my_time_func,
           ATMR_5MS_FRAME( 1 ),
           ATMR_PCG_OFFSET( 1 ),
           5
    }
    
    This would set up my_time_func to execute on the first 5ms frame
    of a 20ms frame at a PCG offset of 1 and a 64 chip symbol offset
    of 5.  Note that 5 is 5 sixty four chip symbols or 320 chips.
    
  Function does nothing if the handle is invalid.    
  
  An action timer resource is freed when a the user specifies that none
  of the 5MS frames should be enabled.  This can be done as follows:
    atmr_set(
       handle,
       NULL,
       ATMR_5MS_FRAME( NONE ),
       0,
       0
    );
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void atmr_set(
   atmr_handle_type          handle,
   void                    (*int_func)( void ),
   atmr_5ms_mask_enum_type   which_5ms_frames,
   atmr_pcg_offset_enum_type pcg_offset_from_5ms_frame,
   unsigned long             chip64_symb_offset_from_pcg_boundary
)
{
   int            atmr_id  = 0;
   atmr_func_type func_ptr = NULL;
   unsigned long  set_bits = 0;

   atmr_id = (int)( (int)((int)handle - (int)atmr_funcs)/(int)sizeof(atmr_func_type) );
   func_ptr = atmr_funcs[ atmr_id ];

   if( ( func_ptr == (atmr_func_type)ATMR_SEIZED_HANDLE  ) ||
       ( handle   != ATMR_INVALID_HANDLE )    )
   {
       if( ( which_5ms_frames == 0 ) || 
           ( int_func == NULL      )    ) 
       {   
           if( atmr_id == ATMR_TIME_1 )
           {
#ifdef T_MSM6700
              ATMR_SET_TIME_INT1_ENABLE(0);
#else
              ATMR_SET_TIME_INT1_MASK( 0 );
#endif /* T_MSM6700 */
              tramp_set_isr( TRAMP_SYS_TIME_INT1, NULL );
           }
           else
           { 
#ifdef T_MSM6700
              ATMR_SET_TIME_INT2_ENABLE(0);
#else
              ATMR_SET_TIME_INT2_MASK( 0 );
#endif /* T_MSM6700 */
              tramp_set_isr( TRAMP_SYS_TIME_INT2, NULL );
           }
           atmr_funcs[ atmr_id ] = ATMR_FREE_HANDLE;
       }
       else
       {
          atmr_funcs[ atmr_id ] = int_func;
#ifdef T_MSM6700
          set_bits = ((0x1) << ((4*(which_5ms_frames - 1)) + pcg_offset_from_5ms_frame));
#else
          set_bits = 
              which_5ms_frames | 
              pcg_offset_from_5ms_frame | 
              chip64_symb_offset_from_pcg_boundary;
#endif
       
          if( atmr_id == 0 )
          {
             tramp_set_isr( TRAMP_SYS_TIME_INT1, atmr_time_int1_isr );
#ifdef T_MSM6700
             /* clear only the corresponding pending ATMR1 bits */
             HWIO_OUT(DEM1X_TIME_INT1_CLEAR, set_bits);
			 /* read back the register */
			 set_bits = (ATMR_GET_TIME_INT1_ENABLE()) | set_bits;
             ATMR_SET_TIME_INT1_ENABLE (set_bits );
#else
             ATMR_SET_TIME_INT1_MASK( set_bits );
#endif /* T_MSM6700 */
          }
          else
          {
             tramp_set_isr( TRAMP_SYS_TIME_INT2, atmr_time_int2_isr );
#ifdef T_MSM6700
             /* clear only the corresponding pending ATMR2 bits */
             HWIO_OUT(DEM1X_TIME_INT2_CLEAR, set_bits);
			 /* read back the register */
			 set_bits = (ATMR_GET_TIME_INT2_ENABLE()) | set_bits;
             ATMR_SET_TIME_INT2_ENABLE (set_bits );
#else
             ATMR_SET_TIME_INT2_MASK( set_bits );
#endif /* T_MSM6700 */
          }
       }
   }
} /* END atmr_set */



