/*
* msm6100ahb.h
*
* This file provides the AHB interface to msm6100io.h and msmhwio.h.
*
* The interface was cut/paste from msm6100io.h.
*
*/
#ifndef AHB_OUT_H
#define AHB_OUT_H

/*---------------------------------------------------------------------------
                           MACRO AHB_OUTM

Writes a masked value out to an 8-bit wide AHB register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/
#define AHB_OUTM( io, mask, val) \
  INTLOCK( );\
  ASSERT((io >= AHB_BASE) && ( io < AHB_END ));\
  AHB_image[AHB_INX(io)] = (AHB_image[ AHB_INX( io ) ] & (word)(~(mask))) | \
         ((word)((val) & (mask)));\
  (void) outp( io, ((word)AHB_image[ AHB_INX( io ) ]));\
  INTFREE( )


/*---------------------------------------------------------------------------
                           MACRO AHB_OUTHM

Writes a masked value out to an 16-bit wide AHB register.
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/
#define AHB_OUTHM( io, mask, val) \
  INTLOCK( );\
  ASSERT((io >= AHB_BASE) && ( io < AHB_END ));\
  AHB_image[AHB_INX(io)] = (AHB_image[ AHB_INX( io ) ] & (word)(~(mask))) | \
         ((word)((val) & (mask)));\
  (void) outpw( io, ((word)AHB_image[ AHB_INX( io ) ]));\
  INTFREE( )


/*---------------------------------------------------------------------------
                           MACRO AHB_OUTWM

Writes a masked value out to a 32-bit wide (word) AHB register.  
Uses/updates shadow buffer.
---------------------------------------------------------------------------*/

#define AHB_OUTWM( io, mask, val) \
   INTLOCK( );\
   ASSERT((io >= AHB_BASE) && ( io < AHB_END ));\
   AHB_image[AHB_INX(io)] = (AHB_image[ AHB_INX( io ) ] & (dword)(~(mask))) | \
          ((dword)((val) & (mask)));\
   (void) outpdw( io, ((dword)AHB_image[ AHB_INX( io ) ]));\
   INTFREE( )

/*---------------------------------------------------------------------------
                           MACRO AHB_OUTH

Writes a value out to a 16-bit wide (halfword) AHB block register.
Does NOT use/update the shadow buffer.
---------------------------------------------------------------------------*/
#define AHB_OUTH( io, val )  (void) outpw( io, (word) val)

#endif /* AHB_OUT_H */
