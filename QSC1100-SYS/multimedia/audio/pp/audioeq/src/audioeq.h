#ifndef AUDIOEQ_H
#define AUDIOEQ_H
/*===========================================================================

                      A U D I O   E Q U A L I Z E R   
                         H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions of the equalizer coefficient
  mathimatic function.

REFERENCES

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/audio/pp/audioeq/main/latest/src/audioeq.h#1 $ $DateTime: 2008/05/20 07:02:55 $ $Author: vparasar $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/13/04    aw     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"   /* Customer specific definitions                   */
#include "target.h"     /* Target specific definitions                     */

#ifdef FEATURE_AUDFMT_EQUALIZER
/* <EJECT> */
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

        A U D I O   E Q U A L I Z E R   F U N C T I O N S
  
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION audioeq_calccoefs

DESCRIPTION
  This function will compute the bass Num, Den and Shift value for equalizer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void audioeq_calccoefs
(
  int32  V0,
  int32  fc, 
  int32  fs, 
  uint16 type, 
  int32  Q,
  int32  *bassNum,
  int32  *bassDen, 
  uint16 *shiftQ
);
#endif /* FEATURE_AUDFMT_EQUALIZER */

#endif /* AUDIOEQ_H */

