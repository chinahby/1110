#ifndef SRCH_CONV_H
#define SRCH_CONV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S R C H  _ C O N V  ---  H E A D E R   F I L E 

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_conv. 

EXTERNALIZED FUNCTIONS
  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2004-2005
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_conv.h#1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
08/21/06   aps/awj removed references to trunc parameter
12/09/05   gs/ds   Macro for converting 20 bit Ec/Io to 16 bit value.
06/23/05   ejv     Move DB_DOWN from srchi.h and add DB_UP.
06/14/05   ejv     Added macros to convert db to linear.  Added prototype
                   for new function srch_conv_get_lecio_table_size.
12/06/04   kwo     Added linear to db macros
11/04/04   kwo     Added srch_conv_db_adj() 
09/10/04   kwo     Converted fractional remainders to decimals
07/22/04   kwo     implementation , first cut
===========================================================================*/ 

#include "customer.h"

#include "comdef.h"

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

#define CONV_EIGHTH_DB_DECIMAL(x) (((x) & 7) * 125)

#define CONV_HALF_DB_UNITS(x)     ((x)/4)
#define CONV_ONE_DB_UNITS(x)      ((x)/8)
#define CONV_TWO_DB_UNITS(x)      ((x)/16)

#define CONV_LIN_EIGHTH_DB_DEC(x) (CONV_EIGHTH_DB_DECIMAL(srch_conv_linear_to_db(x)))
#define CONV_LIN_HALF_DB_UNITS(x) (CONV_HALF_DB_UNITS(srch_conv_linear_to_db(x)))
#define CONV_LIN_ONE_DB_UNITS(x)  (CONV_ONE_DB_UNITS(srch_conv_linear_to_db(x)))
#define CONV_LIN_TWO_DB_UNITS(x)  (CONV_TWO_DB_UNITS(srch_conv_linear_to_db(x)))

#define CONV_HALF_DB_2_EIGHTHS(x) ((x)*4)
#define CONV_ONE_DB_2_EIGHTHS(x)  ((x)*8)
#define CONV_TWO_DB_2_EIGHTHS(x)  ((x)*16)

#define CONV_HALF_DB_LIN_UNITS(x) (srch_conv_db_to_linear(CONV_HALF_DB_2_EIGHTHS(x)))
#define CONV_ONE_DB_LIN_UNITS(x)  (srch_conv_db_to_linear(CONV_ONE_DB_2_EIGHTHS(x)))
#define CONV_TWO_DB_LIN_UNITS(x)  (srch_conv_db_to_linear(CONV_TWO_DB_2_EIGHTHS(x)))

/* Linear Ec/Io is a 20 bit value.  Saturated Linear Ecio is a 16 bit
   value, saturated at about -0.5 dB.  Saturated Linear Ec/Io's are converted
   to dB by 10*log10(x/9/2^13).  */
#define CONV_LECIO_2_SAT_LECIO(lecio) \
    (((lecio) & ~0x7FFFF) ? ((uint16) ~0) : ((lecio) >> 3))

/* Reduces/increases a given value by a specified number of 1/2 db steps. */
#define DB_DOWN(val, db)          (srch_conv_db_adj((val), (-1*CONV_HALF_DB_2_EIGHTHS(db))))
#define DB_UP(val, db)            (srch_conv_db_adj((val), (   CONV_HALF_DB_2_EIGHTHS(db))))

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
  
/*-------------------------------------------------------------------------
      Prototypes 
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SRCH_CONV_MNRD_TO_TRUNC

DESCRIPTION    This function calculates trucation value from coherent
               length, non-coherent length.
               Truncation is calculated here assuming receive diversity
               is not set.

               T = ceil{ 0.5 * [ log2(2 * EcIo * Nnc *(sigma_in*G*Nc)^2 ) - 15] 

DEPENDENCIES   None

RETURN VALUE   Truncation value.

SIDE EFFECTS   None
===========================================================================*/
uint8 srch_conv_mnrd_to_trunc
(
  uint16   coh_int,
  uint8    non_coh_int
);


/*===========================================================================

FUNCTION SRCH_CONV_GET_LECIO_TABLE_SIZE

DESCRIPTION
      This function returns the size of the linear energy table. 

DEPENDENCIES
      
RETURN VALUE
      The size of the linear energy table conv_linear_to_db_tab.

SIDE EFFECTS
      None
===========================================================================*/
extern uint32 srch_conv_get_lecio_table_size( void );

/*===========================================================================

FUNCTION SRCH_CONV_ENG_TO_LECIO

DESCRIPTION
      This function process converts a searcher energy to linear 
      Ec/Io * 9 * 2^16. 
      
      For derivation of the formulas please see the systems memo
      "Searcher Energy and Finger RSSI to Ec/Io Conversion."
     

DEPENDENCIES
      The conversion to dB depends on the 9 * 2^16 scaling
      
RETURN VALUE
      Linear Ec/Io * 9 * 2^16

SIDE EFFECTS None

===========================================================================*/
extern uint32 srch_conv_eng_to_lecio
(
  uint16   srch_eng,      /* hardware srch energy */
  uint16   c_int,         /* coherent integration in chips */
  uint8    nc_int,        /* noncoherent integration passes minimum is 1 */
  uint8    trunc          /* coherent truncation in bits */
);


/*===========================================================================

FUNCTION SRCH_CONV_LECIO_TO_ENG

DESCRIPTION
      This function converts linear Ec/Io * 9 * 2^16 to
      searcher energy. 
      
      For derivation of the formulas please see the systems memo
      "Searcher Energy and Finger RSSI to Ec/Io Conversion."
     

DEPENDENCIES
      
      
RETURN VALUE Hardware searcher energy

SIDE EFFECTS None
===========================================================================*/
extern uint32 srch_conv_lecio_to_eng
(
  uint32   linear_ecio,   /* linear Ec/Io */
  uint16   c_int,         /* coherent integration in chips */
  uint8    nc_int,        /* noncoherent integration passes minimum is 1 */
  uint8    trunc          /* coherent truncation in bits */
);


/*===========================================================================

FUNCTION SRCH_CONV_RSSI_TO_LECIO

DESCRIPTION
      This function process converts the 16 bit finger RSSI to linear 
      Ec/Io * 9 * 2^16. 
      
      For derivation of the formulas please see the systems memo
      "Searcher Energy and Finger RSSI to Ec/Io Conversion."
     

DEPENDENCIES
      The conversion to dB depends on the 9 * 2^16 scaling
      
RETURN VALUE Linear Ec/Io * 9 * 2^16

SIDE EFFECTS None

===========================================================================*/
extern uint32 srch_conv_rssi_to_lecio
(
  uint16   rssi,          /* hardware rssi */
  uint16   pfg            /* the pilot filter gain that the finger was set to */ 
                        
);

/*===========================================================================

FUNCTION SRCH_CONV_LECIO_TO_RSSI

DESCRIPTION
      This function process converts linear (Ec/Io * 9 * 2^16) to the 16 bit 
      finger RSSI 
            
       For derivation of the formulas please see the systems memo
      "Searcher Energy and Finger RSSI to Ec/Io Conversion."
     

DEPENDENCIES
      The conversion to dB depends on the 9 * 2^16 scaling
      
RETURN VALUE finger RSSI

SIDE EFFECTS None

===========================================================================*/
extern uint32 srch_conv_lecio_to_rssi
(
  uint32   linear_ecio,   /* hardware rssi */
  uint8    pfg            /* the pilot filter gain that the finger was set to */ 
                        
);


/*===========================================================================

FUNCTION SRCH_CONV_LINEAR_TO_DB

DESCRIPTION    This function converts from linear units to db units in 1/8db 

DEPENDENCIES   None.

RETURN VALUE   The ecio in 1/8 db units  

SIDE EFFECTS   None.

===========================================================================*/
extern uint32  srch_conv_linear_to_db
(
  uint32 lecio /* The lineary ecio to be converted */
);

/*===========================================================================

FUNCTION srch_conv_db_to_linear

DESCRIPTION    This function converts a ecio (in 1/8 db units) to 
      linear ecio. 

DEPENDENCIES   None.

RETURN VALUE   The lineary ecio 

SIDE EFFECTS   None.

===========================================================================*/
extern uint32  srch_conv_db_to_linear
(
  uint32  ecio /* The Ec/Io value to convert in 1/8 db units*/
);
  

/*===========================================================================

FUNCTION SRCH_CONV_DB_ADJ

DESCRIPTION    This function adjusts a value up or down in 1/8 db units 

DEPENDENCIES   None.

RETURN VALUE   The adjusted value 

SIDE EFFECTS   None.

===========================================================================*/
extern uint32  srch_conv_db_adj
(
  uint32  value, /* The value to adjust (20 bits range) */ 
  int32   adj    /* the amount to adjust in 1/8 dB (pos up, neg down) */
);

#endif /* SRCH_CONV_H */


