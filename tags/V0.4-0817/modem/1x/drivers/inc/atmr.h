#ifndef ATMR_H
#define ATMR_H
/*===========================================================================

                           MSM5000 Action Time Register               

DESCRIPTION
   Definitions for the MSM5000 Action Timer Register

Copyright (c) 1999 through 2008 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5105/vcs/atmr.h_v   1.1   29 Aug 2000 11:10:14   efytal  $
$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/atmr.h#1 $ $DateTime: 2009/02/13 14:29:02 $ $Author: joshuam $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_ATMR_TEST
11/15/99   jct     Created

===========================================================================*/

/* -------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------- */

/* Action Timer Function Type
**   Signature of function passed to atmr_set.  User must provide a function
**   with this signature.  Called when the specified action time interval
**   elapses.
*/
typedef void (*atmr_func_type)( void );

/* Action Timer Identification
**   Specifies which Action Timer resource.  QPCH is included since the
**   QPCH is shared with Action Timer #2
*/
typedef enum {
   ATMR_TIME_1,
   ATMR_TIME_2,
   ATMR_QPCH
} atmr_timer_enum_type;

/* Number of Resources
**   There are two action time resources available
*/
#define ATMR_NUM_RESOURCES 2

/* Action Timer Resource Handle
**    A 32 bit opaque handle to an action timer resource.  Returned in
**    calls to atmr_get, used in subsequent calls to atmr_set
*/
typedef void* atmr_handle_type;

/* Invalude Action Timer Handle
**    Returned from a call to atmr_get when the resource can not
**    be allocated
*/
#define ATMR_INVALID_HANDLE ((atmr_handle_type)0xFFFFFFFF)

/* 5ms Frame Mask
**    Mask used in the ATMR_5MS_FRAME macro to specify which bits are sent
**    to the action timer registers for a given 5ms frame.  Users need not
**    use these values directly.  Instead users should use the ATMR_5MS_FRAME
**    macro.
*/
typedef enum {
#ifdef T_MSM6700
     ATMR_5MS_FRAME_NONE = 0x0000,
     ATMR_5MS_FRAME_1    = 0x0001, 
     ATMR_5MS_FRAME_2    = 0x0002,  
     ATMR_5MS_FRAME_3    = 0x0003,  
     ATMR_5MS_FRAME_4    = 0x0004 
#else
   ATMR_5MS_FRAME_NONE = 0x0000,
//fas
//   ATMR_5MS_FRAME_1    = 0x0040,
//   ATMR_5MS_FRAME_2    = 0x0080,
//   ATMR_5MS_FRAME_3    = 0x0100,
//   ATMR_5MS_FRAME_4    = 0x0200
     ATMR_5MS_FRAME_1    = (1<<7), 
     ATMR_5MS_FRAME_2    = (1<<8),  
     ATMR_5MS_FRAME_3    = (1<<9),  
     ATMR_5MS_FRAME_4    = (1<<10)  
#endif /* T_MSM6700 */

} atmr_5ms_mask_enum_type;

/* PCG Offset Mask
**   Mask used in the ATMR_PCG_OFFSET macro to specify which bits are sent
**   to the action timer registers for a given PCG.  Users need not use
**   these values directly.  Instead users should use the ATMR_PCG_OFFSET
**   macro.
*/
typedef enum {
#ifdef T_MSM6700
   ATMR_PCG_OFFSET_1 = 0x0000,
   ATMR_PCG_OFFSET_2 = 0x0001,
   ATMR_PCG_OFFSET_3 = 0x0002,
   ATMR_PCG_OFFSET_4 = 0x0003
#else
   ATMR_PCG_OFFSET_1 = 0x0000,
//fas
//   ATMR_PCG_OFFSET_2 = 0x0010,
//   ATMR_PCG_OFFSET_3 = 0x0020,
//   ATMR_PCG_OFFSET_4 = 0x0030
   ATMR_PCG_OFFSET_2 = (1<<5),
   ATMR_PCG_OFFSET_3 = (2<<5),
   ATMR_PCG_OFFSET_4 = (3<<5)
#endif /* T_MSM6700 */
} atmr_pcg_offset_enum_type;

/* -------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------- */
#ifdef T_MSM6700

#define ATMR_GET_INT1_STATUS  HWIO_IN(DEM1X_TIME_INT1_STATUS)
#define ATMR_GET_INT2_STATUS  HWIO_IN(DEM1X_TIME_INT2_STATUS)

#define ATMR_CLEAR_INT1_STATUS(bits)  HWIO_OUT(DEM1X_TIME_INT1_CLEAR , bits)
#define ATMR_CLEAR_INT2_STATUS(bits)  HWIO_OUT(DEM1X_TIME_INT2_CLEAR, bits)

#endif /* T_MSM6700 */

/*==========================================================================

MACRO ATMR_5MS_FRAME

DESCRIPTION
  Macro to convert 5ms frame specification to an action timer frame mask.
  This is based on a 5ms frame specification within a 20ms frame, therefore
  the valid values are 0..4 with 0 meaning disable all the interrupt sources.
  1 means enable the first 5 ms frame, 2 means enable the second 5ms frame
  and so on.
  
  Since each use of the macro specifies one frame, they can be orred together
  to specify multiple 5ms frames within the 20ms frame like this:
    ATMR_5MS_FRAME( 1 ) | ATMR_5MS_FRAME( 2 )
  would enable the interrupt for the first and second 5ms frames of a 20ms
  frame.
    ATMR_5MS_FRAME( 0 )
  would disable all the interrupts for all 5ms frames.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define ATMR_5MS_FRAME( xx_frame ) \
   ATMR_5MS_FRAME_##xx_frame

/*==========================================================================

MACRO ATMR_PCG_OFFSET

DESCRIPTION
  Macro to convert a PCG offset from a 5ms frame boundary to an action timer
  PCG offset mask.  Note there are four 1.25 PCGs per 5ms frame.  The valid range
  of values is therefore 1..4 inclusive.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define ATMR_PCG_OFFSET( xx_pcg ) \
   ATMR_PCG_OFFSET_##xx_pcg
   
/* -------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------- */
#ifdef __cplusplus
   extern "C" {
#endif

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
    //fas
    atmr_timer_enum_type tmr
    //atmr_tmr_enum_type tmr 
    //fas
);
   

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
);

#ifdef __cplusplus
   }
#endif

#endif /* ATMR_H */
