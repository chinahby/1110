#ifndef MSMIO_H
#define MSMIO_H

/*===========================================================================

           MSM5100 REGISTER INPUT/OUTPUT HEADER FILE

DESCRIPTION
  This file contains the input and output macro definitions to support reading 
  and writing from/to MSM registers in the MSM5100.

Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/hw/msmio.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/01   dsb     Created from msm5100io.h
===========================================================================*/


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/
#ifndef FEATURE_6200_COMPATIBILITY
/*---------------------------------------------------------------------------
                           MACRO MSM_OUT

Writes a value out to an 8-bit wide MSM register.
Does NOT use/update the shadow buffer
---------------------------------------------------------------------------*/
#define MSM_OUT( io, val )  (void) outp( io, (int) val)

/*---------------------------------------------------------------------------
                           MACRO MSM_OUTH

Writes a value out to a 16-bit wide (halfword) MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_OUTH( io, val )  (void) outpw( io, (word) val)

/*---------------------------------------------------------------------------
                           MACRO MSM_OUTW
                           
Writes a value out to a 32-bit wide (word) MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_OUTW( io, val )  (void) outpdw( io, (dword) val)



/*---------------------------------------------------------------------------
                           MACRO MSM_OUTM

Writes a masked value out to an 8-bit wide MSM register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_OUTM( io, mask, val) \
  INTLOCK();\
  MSM_image[MSM_INX (io)] = (MSM_image[ MSM_INX (io) ] & (word)(~(mask))) | \
                            ((word)((val) & (mask)));\
  (void) outp( io, (byte)MSM_image[ MSM_INX (io) ]);\
  INTFREE()
#endif /* FEATURE_6200_COMPATIBILITY */
/*---------------------------------------------------------------------------
                           MACRO MSM_OUTM_IL

Synonym for MSM_OUTM.  Included for legacy support.
---------------------------------------------------------------------------*/
#define MSM_OUTM_IL( io, mask, val) \
  MSM_OUTM(io, mask, val)

#define MSM_OUTHM_IL( io, mask, val) \
  MSM_OUTHM(io, mask, val)


/*---------------------------------------------------------------------------
                           MACRO MSM_STROBEM

Write a masked value out to an 8-bit register.
Does NOT update the shadows, but does read from them to preserve the
previous values which are not masked.
     
Must only be used for writing to ports which ARE NOT related to the fingers.
---------------------------------------------------------------------------*/
#define  MSM_STROBEM( io, mask, val ) \
   INTLOCK( ); \
   HWIO_OUT(io, (HWIO_SHDW(io) & (word)(~(mask))) \
                    | (word)((val) & (mask))); \
   INTFREE( )


/*---------------------------------------------------------------------------

                           MACRO MSM_F_OUTM

Writes a masked value out to an 8-bit wide MSM paged finger register.  
To do this, a page is first selected , and then the value is 
written/shadowed.
Uses/updates paged finger shadow buffer.

Must only be used for writing to ports which ARE related to the fingers.
---------------------------------------------------------------------------*/
#define MSM_F_OUTM( finger, io, mask, val ) \
  INTLOCK( );\
  (void) outp( FFE_FINGER_PAGE_REG_WB, 1<<(finger) );\
  MSM_fimage[ finger ][ MSM_FINX(io) ] = \
               (MSM_fimage[ finger ][ MSM_FINX(io) ] & ((byte)(~(mask)))) \
               | (byte)( (val) & (mask) ); \
  (void) outp( io, MSM_fimage[ finger ][ MSM_FINX(io) ] ); \
  INTFREE( )

#ifndef FEATURE_6200_COMPATIBILITY
/*---------------------------------------------------------------------------
                           MACRO MSM_OUTHM

Writes a masked value out to an 16-bit wide MSM register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/

#define MSM_OUTHM( io, mask, val) \
INTLOCK( );\
MSM_image[MSM_INX(io)] = (MSM_image[ MSM_INX( io ) ] & (word)(~(mask))) | \
                         ((word)((val) & (mask)));\
(void) outpw( io, ((word)MSM_image[ MSM_INX( io ) ]));\
INTFREE( )

#endif /* FEATURE_6200_COMPATIBILITY */

/*---------------------------------------------------------------------------
                           MACRO MSM_OUTHM

Writes a masked value out to an 16-bit wide MSM register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/

#define MSM_OUTHM( io, mask, val) \
INTLOCK( );\
MSM_image[MSM_INX(io)] = (MSM_image[ MSM_INX( io ) ] & (word)(~(mask))) | \
                         ((word)((val) & (mask)));\
(void) outpw( io, ((word)MSM_image[ MSM_INX( io ) ]));\
INTFREE( )


/*---------------------------------------------------------------------------
                          MACRO MSM_STROBEM_H

Write a masked value out to a 16-bit (halfword) register.
Does NOT update the shadows, but does read from them to preserve the
previous values which are not masked.
     
Must only be used for writing to ports which ARE NOT related to the fingers.
---------------------------------------------------------------------------*/

#define  MSM_STROBEM_H( io, mask, val ) \
   INTLOCK( ); \
   HWIO_OUT(io, (HWIO_SHDW(io) & (word)(~(mask))) \
     | (word)((val) & (mask))); \
   INTFREE( )
   
#ifndef FEATURE_6200_COMPATIBILITY
/*---------------------------------------------------------------------------
                           MACRO MSM_OUTWM

Writes a masked value out to a 32-bit wide (word) MSM register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/

#define MSM_OUTWM( io, mask, val) \
   INTLOCK( );\
   MSM_image[MSM_INX(io)] = (MSM_image[ MSM_INX( io ) ] & (dword)(~(mask))) | \
                            ((dword)((val) & (mask)));\
   (void) outpdw( io, ((dword)MSM_image[ MSM_INX( io ) ]));\
   INTFREE( )

/*---------------------------------------------------------------------------
                           MACRO MSM_IN

Reads an 8-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_IN( io )  (byte) inp( io )

/*---------------------------------------------------------------------------
                           MACRO MSM_INM

Reads a masked 8-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_INM( io, mask ) ( inp( io ) & (mask) )


/*---------------------------------------------------------------------------
                           MACRO MSM_INH

Reads a 16-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_INH( io )  (word) inpw( io )


/*---------------------------------------------------------------------------
                           MACRO MSM_INHM

Reads a masked 16-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_INHM( io, mask ) ( inpw( io ) & (mask) )


/*---------------------------------------------------------------------------
                           MACRO MSM_INW

Reads a 32-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_INW( io )  (dword) inpdw( io )


/*---------------------------------------------------------------------------
                           MACRO MSM_INWM

Reads a masked 32-bit value from an MSM register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define MSM_INWM( io, mask ) ( inpdw( io ) & (mask) )

#endif /* FEATURE_6200_COMPATIBILITY */ 

/*---------------------------------------------------------------------------
                           MACRO GPIO2_OUT

Writes a value out to an 8-bit wide GPIO2 block register.
Does NOT use/update the shadow buffer
---------------------------------------------------------------------------*/
#define GPIO2_OUT( io, val )  (void) outp( io, (int) val)


/*---------------------------------------------------------------------------
                           MACRO GPIO2_OUTH

Writes a value out to a 16-bit wide (halfword) GPIO2 block register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define GPIO2_OUTH( io, val )  (void) outpw( io, (word) val)



/*---------------------------------------------------------------------------
                           MACRO GPIO2_INM

Reads a masked 8-bit value from a GPIO2 block register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define GPIO2_INM( io, mask ) ( inp( io ) & (mask) )


/*---------------------------------------------------------------------------
                           MACRO GPIO2_INWM

Reads a masked 32-bit value from a GPIO2 block register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define GPIO2_INHM( io, mask ) ( inpw( io ) & (mask) )

#endif /*MSMIO_H*/
