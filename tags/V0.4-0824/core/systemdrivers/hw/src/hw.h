#ifndef HW_H
#define HW_H
/*===========================================================================

                H A R D W A R E   U T I L I T I E S
                      H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the utilities contained
  in hw.c.

Copyright (c) 1992, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1996, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004 -2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/hw/hw.h#2 $ $DateTime: 2008/10/23 13:20:45 $ $Author: vphan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/03   ejv     Added hw_partnum_version(...) and hw_rev_type.
10/23/00   rmd     Removed HW_IS_Q186 (Obsolete). Removed some defines that
                   are not longer used in the code and removed "#ifdef T_MSM5000"
                   which is not longer necessary.                   
08/28/00   rmd     Added definition for MSM5000 and MSM5105.
05/10/00   aks     Added definition for MSM3300 Rev A0.
11/04/99   mk      Added definitions for MSM3100 REV4,5,6,7.
09/11/99   mk      Added definition for MSM3100 REV3.
07/15/99   mk      Added defs for MSM_31_A0,A1,A2.
04/12/99   rm      Added definitions for Lynx (MSM2310)
08/04/09   ychan   Added new revision value MSM_30_A2.
06/26/98   kss     Added definitions for Lynx (MSM2310)
03/20/98   aks     Added definitions for the alternate LCD and the enhanced
                   Audio circuit capability/configuration.
12/10/97   aks     Modified and added definitions for MSM, up to and including
                   2.3 rev A4.
11/02/96   jah     Configured for TGP (T_T), by adding comments for a block
                   of definitions which are obsolete.
03/08/96   jah     Added HW_IS_Q186() to determine native CPU type, and
                   added definitions for MSM 2.2 A0, A1, A2, and B0
11/28/95   jah     Added prototypes for hw_init() and hw_capability().
05/10/95   jah     Added ISS-1 as phone with old-style EEPROM
05/10/95   jah     Added hw_version() and Gemini support
10/30/92   jah     Added hw_force_offline().
08/06/92   jah     Corrected EEPROM length
07/06/92   jah     Added hw_reset()
05/29/92   jah     Created/Ported from the brassboard

===========================================================================*/

#include "target.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Optional Hardware capabilities available on this phone
**
** The masks are to be used with the return value from hw_capability()
** to determine whether or not an option is available/enabled.
*/
#define HW_CAP_BASE_V           0x00000000      /* Base configuration */
#define HW_MSM6550_HAS_R1_V1    0
#define HW_REV_TYPE_V2

typedef unsigned long hw_capability_type;

/* Data type for software to declard and pass a pointer of to function
   hw_partnum_version, which will enter the MSM rev info for MSM6500
   MSMs and newer (ie. those using new MSM revision format) */
typedef struct
{
  uint16 partnum;
  uint8  version;
  uint32 hw_register;
}
hw_rev_type;

/* Values for each rev of the MSM ASIC.
*/
#define MSM_2_REV_3     0x0101
#define MSM_2_REV_4     0x0201        /* MSM 2P  Rev 4  */
#define MSM_2_REV_5     0x0301        /* MSM 2P1 Rev 5  */

#define MSM_22_A0       0x1002        /* MSM 2.2 Rev A0 */
#define MSM_22_A1       0x1102        /* MSM 2.2 Rev A1 */
#define MSM_22_A2       0x1202        /* MSM 2.2 Rev A2 */
#define MSM_22_B0       0x1303        /* MSM 2.2 Rev B0 */
#define MSM_22_B1       0x1403        /* MSM 2.2 Rev B1 */
#define MSM_22_B2       0x1404        /* MSM 2.2 Rev B2 */
#define MSM_22_B3       0x1404        /* MSM 2.2 Rev B3 */

#define MSM_23_A0       0x2005        /* MSM 2.3 Rev A0 */
#define MSM_23_A1       0x2105        /* MSM 2.3 Rev A1 */
#define MSM_23_A2       0x2205        /* MSM 2.3 Rev A2 */
#define MSM_23_A3       0x2305        /* MSM 2.3 Rev A3 */
#define MSM_23_A4       0x2405        /* MSM 2.3 Rev A4 */

#define MSM_2310_A0     0x3000        /* MSM 2310 */

#define MSM_30_A0       0x4000        /* MSM 3.0 Rev A0, only upper byte is valid. */
#define MSM_30_A2       0x4100        /* MSM 3.0 Rev A1/A2. Same ID for both. */
#define MSM_30_A3       0x4200        /* MSM 3.0 Rev 3 */
#define MSM_30B_A0      0x4300        
#define MSM_30C_A0      0x4400

#define MSM_31_A0       0x5000        /* MSM 3100 Rev A0 */
#define MSM_31_A1       0x5100        /* MSM 3100 Rev A1 */
#define MSM_31_A2       0x5200        /* MSM 3100 Rev A2 */
#define MSM_31_A3       0x5300        /* MSM 3100 Rev A3 */
#define MSM_31_A4       0x5400        /* MSM 3100 Rev A4 */
#define MSM_31_A5       0x5500        /* MSM 3100 Rev A5 */
#define MSM_31_A6       0x5600        /* MSM 3100 Rev A6 */
#define MSM_31_A7       0x5700        /* MSM 3100 Rev A7 */

#define MSM_33_A0       0x9000        /* MSM 3300 Rev A0 */

#define MSM_50_A0       0x6000        
#define MSM_50B_A0      0x6400

#define MSM_5105_A0     0xc000        /* MSM 5105 Rev A0 */   

#define MSM_6050_A0     0xF000        /* MSM6050 rev A0 (actual reg val = 0x50F0) */

#define MSM_6500        0xFF00        /* MSM6500 or newer -
                                         call hw_partnum_version(...)
                                         for exact details */ 
/* Part numbers for MSM6500                            */
#define HW_MSM6500_R1  0x3c
#define HW_MSM6500_R2  0x3d
#define HW_MSM6500_R3  0x3e
#define HW_MSM6500_R4  0x3f

/* Part numbers for MSM6800                            */
#define HW_MSM6800_R1 0x0072
#define HW_MSM6800_R2 0x0072
#define HW_MSM6800_R3 0x2070
#define HW_MSM6800_R4 0x2070

/* version id for MSM6800 */
#define HW_MSM6800_V1P0 0x00
#define HW_MSM6800_V1P1 0x01
#define HW_MSM6800_V2P0 0x02
#define HW_MSM6800_V3P0 0x00

/* Mask for masking off the firmware id from id values */
#define HW_MSM_VERSION_M 0xFF00

/* QSC60x5 partnum */
#define HW_QSC6055_P0R0  0x00FF
#define HW_QSC6065_P0R0  0x00FE
#define HW_QSC6075_P0R0  0x00FA
#define HW_QSC6085_P0R0  0x00F8

/* QSC11x0 MSM/PMIC version */
typedef struct {
  int msm_hw_version;
  int pmic_hw_version;
  int pg;
} hw_qsc11x0_rev_type;

extern hw_qsc11x0_rev_type qsc11x0_hw_rev;

/*===========================================================================

                            MACRO DECLARATIONS

===========================================================================*/



/*===========================================================================

MACRO HW_MSM_VERSION

DESCRIPTION
  Strips vocoder firmware information from the MSM version id value;
  useful if only the MSM hardware version is wanted.

PARAMETERS
  The 16-bit id value to have firmware info masked off

DEPENDENCIES
  None

RETURN VALUE
  version id with firmware info masked off.

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM_VERSION( version ) ( ( version ) & HW_MSM_VERSION_M )


/*===========================================================================

MACRO HW_MSM6500_HAS_R1_V1

DESCRIPTION
  Checks part number and version for Version 1 (aka 1.1)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6500_HAS_R1_V1 \
  hw_check_partnum_version(HW_MSM6500_R1,1)


/*===========================================================================

MACRO HW_MSM6500_HAS_R1_V2

DESCRIPTION
  Checks part number and version for Version 2 (aka 1.3)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6500_HAS_R1_V2 \
  hw_check_partnum_version(HW_MSM6500_R1,2)


/*===========================================================================

MACRO HW_MSM6500_HAS_R1_V5

DESCRIPTION
  Checks part number and version for Version 5 (aka 2.3)

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6500_HAS_R1_V5 \
  hw_check_partnum_version(HW_MSM6500_R1,5)
  

/*===========================================================================

MACRO HW_MSM6800_HAS_R1_V0

DESCRIPTION
  Checks part number and version for Version 1.0

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6800_HAS_R1_V0 \
  hw_check_partnum_version(HW_MSM6800_R1,0)

/*===========================================================================

MACRO HW_MSM6800_HAS_R1_V1

DESCRIPTION
  Checks part number and version for Version 1.1

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6800_HAS_R1_V1 \
  hw_check_partnum_version(HW_MSM6800_R1,1)

/*===========================================================================

MACRO HW_MSM6800_HAS_R2_V0

DESCRIPTION
  Checks part number and version for Version 2.0

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6800_HAS_R2_V0 \
  hw_check_partnum_version(HW_MSM6800_R2,2)

/*===========================================================================

MACRO HW_MSM6800_HAS_R3_V0

DESCRIPTION
  Checks part number and version for Version 3.0

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6800_HAS_R3_V0 \
  hw_check_partnum_version(HW_MSM6800_R3,0)

/*===========================================================================

MACRO HW_MSM6800_HAS_R4_V0

DESCRIPTION
  Checks part number and version for Version 4.0

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

#define HW_MSM6800_HAS_R4_V0 \
  hw_check_partnum_version(HW_MSM6800_R4,0)

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION HW_VALID_ADDR

DESCRIPTION
  This procedure verifies that an address is within legal range.

DEPENDENCIES
  None

RETURN VALUE
  True if the address is legal, False if not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hw_valid_addr
(
  void *ptr,            /* Address to validate                       */
  word len              /* Length of memory block pointed to by *ptr */
);


/*===========================================================================

FUNCTION HW_POWER_OFF

DESCRIPTION
  Turn off the power to the phone.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
extern void hw_power_off( void );


/*===========================================================================

FUNCTION HW_RESET

DESCRIPTION
  Reset the phone.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
extern void hw_reset( void );


/*===========================================================================

FUNCTION HW_FORCE_OFFLINE

DESCRIPTION
  See if the phone is to be forced into offline.

DEPENDENCIES
  None

RETURN VALUE
  True if the phone is to be forced offline, False if not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hw_force_offline( void );


/*===========================================================================

FUNCTION HW_VERSION

DESCRIPTION
  Return the hardware version

DEPENDENCIES
  None

RETURN VALUE
  16-bit hardware version

SIDE EFFECTS
  None

===========================================================================*/
extern word hw_version( void );


/*===========================================================================

FUNCTION HW_PARTNUM_VERSION

DESCRIPTION
  Fill in the hardware partnumber and version ID from the MSM.

DEPENDENCIES
  None

RETURN VALUE
  None - data is passed by filling in pointer data structure

SIDE EFFECTS
  None

===========================================================================*/
extern void hw_partnum_version( hw_rev_type * rev_ptr );


/*===========================================================================

FUNCTION HW_CHECK_PARTNUM_VERSION

DESCRIPTION

  Compare hardware partnumber and version ID passed as parameter with the MSM.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE  - Part number and version passed as parameter match MSM

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hw_check_partnum_version( uint16 partnum, uint8  version);


/*===========================================================================

FUNCTION HW_INIT

DESCRIPTION
  This function determines the options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  None

RETURN VALUE
  Mask of options the hardware is supports.

SIDE EFFECTS
  None

===========================================================================*/
extern void hw_init
(
  dword config_value /* Configuration value read from NVM */
);


/*===========================================================================

FUNCTION HW_CAPABILITY

DESCRIPTION
  This function determines the options this hardware supports,
  and returns that information to the caller.

DEPENDENCIES
  hw_init() must be called before hw_capability() is called.

RETURN VALUE
  Mask of options the hardware is supports.

SIDE EFFECTS
  None

===========================================================================*/
extern hw_capability_type hw_capability( void );


/*===========================================================================

FUNCTION HW_INIT_QSC11X0_HW_REV

DESCRIPTION
  This function initialized MSM/PMIC version in the 

DEPENDENCIES
  pm_init() must be called before hw_init_qsc110x_hw_rev() is called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hw_init_qsc11x0_hw_rev(void);

#endif  /* HW_H */

