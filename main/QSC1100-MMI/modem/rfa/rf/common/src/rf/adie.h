#ifndef ADIE_H
#define ADIE_H
/*===========================================================================


               A N A L O G   D I E    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the analog die of a stacked digital/analog die MSM.

Copyright (c) 1998 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/adie.h_v   1.1   22 Mar 2002 16:01:22   ehandojo  $
$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/adie.h#2 $ $DateTime: 2008/07/20 12:54:16 $ $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/08   jc/vm   Fixed HKADC Driver for QSC1100
08/28/07   ymc     Added mask and delay to adie_codec_reg_type.
08/09/07   dyc     Extern adie_codec_enable().
                   Added API adie_codec_read()
06/16/07   abh     Changes to support XO Thermsitor ADC.
12/14/06   ans     Changes for SC2x
12/01/06   dyc     Updated for SC2x ADIE reg limits.
05/22/06   hkk     Common Adie driver for 1x and UMTS 
05/10/04    et     added rf_mode parameter to adie_init
                   added adie_system_mode to this file from adie_sbi.h
03/15/04    rv     Added type defn for rf_path_enum_type for 6250.
01/28/04    et     changed parameter to adie_bbrx_wakeup to rf_path_enum_type
                   rf_path from device type.
                   added function adie_clk_regime_config() to be called
                   by adie_bbrx_wakeup to configure the adie clock regime
                   enable at wakeup, disable at sleep, based on diff/se feature
02/17/03   rmd     Brought in the ADIE HKADC header definitions.
03/22/02    eh     include adie_regdefs.h
01/22/02    eh     Port for MSM6200.
12/04/01   rmd     adie re-org
11/19/01    aw     Added type, and prototypes for adie codec config.
11/15/01   aks     Include rficap.h.  Wrap some functions in ADIE_HAS_ADC.
10/25/01    et     added ADIE_OUT_BYTES macro
10/19/01   aks     Add type, add quantity entry in enums, removed
                   adie_set_system_mode().
10/09/01   aks     Added function prototype, merged phone_t changes.
03/05/01   dsb     Updated ADIE_IN macro to support new SBI API for reading
                   multiple bytes from possibly non-sequential registers.
                   Conditionally compile adie_write_codec_func_ctrl
                   under ADIE_DEBUG.
01/17/01   vlc     Updated comments, no code change.
12/19/00   vlc     Added support for function adie_wakeup().
10/10/00   dsb     Created.

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include "adc.h"

#include "adie_adcdefs.h"
#include "adie_cdcdefs.h"  
#include "adie_regdefs.h"

#include "rflib.h"
#include "rfcom.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifndef T_QSC60X5
#error code not present
#else
#define ADIE_MAX_REGS 0xFF

/* Max/Min Register range for CODEC ADDR map */
#define ADIE_MAX_CODEC_REGS 0xB2
#define ADIE_MIN_CODEC_REGS 0x48
#endif /* T_QSC60X5 */

typedef enum
{
  ADIE_AMPS_MODE,
  ADIE_CELLULAR_MODE,
  ADIE_PCS_MODE,
  ADIE_GPS_MODE,
  ADIE_CDMA_MODE,
  ADIE_GSM_MODE,
  ADIE_WCDMA_MODE

} adie_system_mode;

/* Type of operation on the analog die. */
typedef enum 
{
  ADIE_DISABLE,
  ADIE_ENABLE,
  ADIE_NUM_OPS

} adie_op_type;

/* Types of hardware blocks on the analog die which need to be accessed. */
typedef enum 
{
  ADIE_BBRX_BLK,
  ADIE_HKADC_BLK,
  ADIE_CODEC_BLK,
  #ifdef FEATURE_XO
  ADIE_XOTHERM_BLK,
  #endif /* FEATURE_XO */
  ADIE_NUM_BLKS

} adie_hwblock_type;


#define BBRX_EN1_SW_SLEEP_M        0xFF
#define BBRX_EN1_ACTIVE_MODE_V     0x0D
#define BBRX_EN1_POWERDOWN_V       0x00



/* Adie codec config */
typedef struct {
  byte                     reg;               /* Register address             */
  byte                     mask;              /* mask                         */
  byte                     val;               /* Register value               */
  uint32                   delay_us;          /* busy wait after reg. write   */
} adie_codec_reg_type;

/********************* MULTI-MODE ADIE HKADC DEFINITIONS *******************/
/* This enum type is used to inform the user that the ADIE AD conversion
   was successful or not */

typedef enum
{
  ADIE_FAILURE,
  ADIE_SUCCESS

} adie_result_type;



/*************** END OF MULTI-MODE ADIE HKADC DEFINITIONS ******************/
/*===========================================================================

                           MACROS

===========================================================================*/
/*===========================================================================

MACRO ADIE_OUTM

DESCRIPTION
  This macro outputs a specified value, qualified by a specified mask,
  to a specified analog die register via the SBI interface.  Only the bits
  corresponding to the mask are actually affected; other bits retain
  their previous values.  To do this, an image is maintained of the
  previous value written to the register which is then combined with the
  new value.

PARAMETERS
  reg     the address of the analog die register to write to.
  mask    the mask to use, 1s enable bit update
  value   the byte value to write to the register

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  SBI_SUCCESS for a successful operation,
  SBI_FAILURE if a failure occurs.

SIDE EFFECTS
  None

===========================================================================*/
#define ADIE_OUTM( reg, mask, value ) adie_write_byte( reg, mask, value )
/*===========================================================================

MACRO ADIE_IN

DESCRIPTION
  This macro reads and returns the value from an analog die register.  No
  mask is applied to the read value.  The value read is undefined in the
  case of a analog die failure.

PARAMETERS
  reg_array  the address of the start of an array of analog die registers to
             be read from.
  var        the *address* of the variable to store the value(s) read.

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  SBI_SUCCESS for a successful operation,
  SBI_FAILURE if a failure occurs.

SIDE EFFECTS
  None

===========================================================================*/

#define ADIE_IN( reg_array, var)  ADIE_IN_I(reg_array, var)

/*===========================================================================

MACRO ADIE_OUTM_VERIFY

DESCRIPTION
  This macro writes to an SBI slave or a memory location, and then proceeds
  to read back the value that was written, and compares it to the original value
  to make sure it is the same.

PARAMETERS
  reg     the address of the analog die register to write to.
  mask    the mask to use, 1s enable bit update
  value   the byte value to write to the register

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,
  ADIE_FAILURE if a failure occurs.

SIDE EFFECTS
  None

===========================================================================*/
#define ADIE_OUTM_VERIFY( reg, mask, value)  \
        ADIE_OUTM_VERIFY_I(reg, mask, value)

/*===========================================================================

MACRO ADIE_OUT_BYTES

DESCRIPTION
  This macro takes as an input the address to an array or registers,
  the address to an array of values, and a length, and writes the values to the
  corrisponding registers.  This is done through SBI w/ a slave ID when SBI is
  used, and through memory mapping when memory mapping is used.
  in the case of SBI, the macro also updates the adie_image array.

PARAMETERS
  *addr - address to the array of addressers
  *data - address to the array of values
  length - length of both those arrays (should be the same)

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,
  ADIE_FAILURE if a failure occurs.

SIDE EFFECTS
  in the case of SBI,
  If the MACRO SBI_CHK_BUSY_WRITE times-out you will return from the function
  with a SBI_FAILURE

===========================================================================*/

#define ADIE_OUT_BYTES(addr, data, length )\
    adie_bbrx_write_mult((adie_addr_type*)addr, (adie_addr_type*)data, length )



/*===========================================================================

MACRO ADIE_BBRX_OUT_BYTES

DESCRIPTION
  This macro takes as an input the address to an array or registers the address 
  to an array of values, and a length, and writes the values to the
  corrisponding registers in BBRx. This is done through SBI w/ a slave ID when SBI is
  used, and through memory mapping when memory mapping is used.In the case of 
  SBI, the macro also updates the adie_image array.

PARAMETERS
  *addr - address to the array of addressers
  *data - address to the array of values
  length - length of both those arrays (should be the same)

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,
  ADIE_FAILURE if a failure occurs.

SIDE EFFECTS
  in the case of SBI,
  If the MACRO SBI_CHK_BUSY_WRITE times-out you will return from the function
  with a SBI_FAILURE

===========================================================================*/

#define ADIE_BBRX_OUT_BYTES(addr, data, length )\
    adie_bbrx_write_mult((adie_addr_type*)addr, (adie_addr_type*)data, length )

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION adie_init

DESCRIPTION
  This function initializes the analog die for operation.
      
DEPENDENCIES
  sbi_init() must have been called.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Interrupts are disabled during this routine.
===========================================================================*/
void adie_init(adie_system_mode rf_mode);

/*===========================================================================

FUNCTION adie_wakeup

DESCRIPTION
  This function wakes up the ADIE or allows it to sleep.  The ADIE only goes to
  sleep if none of its hardware blocks are being used.
  
DEPENDENCIES
  sbi_init() must have been called.
  adie_init() must have been called.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

void adie_wakeup (adie_op_type wakeup, adie_hwblock_type req_blk);

/*===========================================================================

FUNCTION      adie_bbrx_wakeup

DESCRIPTION

  This function wakes up or puts to sleep the BBRX core of the ADIE

PARAMETERS

  wakeup- whether the core is to be put to sleep or be woken up

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void adie_bbrx_wakeup (rf_path_enum_type rf_path, adie_op_type wakeup);

/*===========================================================================

FUNCTION      adie_codec_wakeup

DESCRIPTION

  This function wakes up or puts to sleep the CODEC core of the ADIE

PARAMETERS

  wakeup- whether the core is to be put to sleep or be woken up

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void adie_codec_wakeup (adie_op_type wakeup);

/*===========================================================================

FUNCTION      adie_hkadc_wakeup

DESCRIPTION

  This function wakes up or puts to sleep the HKADC core of the ADIE

PARAMETERS

  wakeup- whether the core is to be put to sleep or be woken up

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void adie_hkadc_wakeup (adie_op_type wakeup);

/*===========================================================================

FUNCTION adie_codec_read

DESCRIPTION
  This function read codec register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_read
(
  byte reg,
  byte mask,
  byte *value
);

/*===========================================================================

FUNCTION adie_codec_write

DESCRIPTION
  This function write codec register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_write
(
  byte reg,
  byte mask,
  byte value
);

/*===========================================================================

FUNCTION adie_codec_config

DESCRIPTION
  This function for writing the codec config array to Adie

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_config
(
  uint8               size,
  adie_codec_reg_type *codec_func_ctrl
);

/*===========================================================================

FUNCTION adie_codec_enable                                     

DESCRIPTION
  This function enable CODEC_OUT bit in GPR register [4].

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void adie_codec_enable (adie_op_type wakeup);

#ifdef ADIE_DEBUG
#error code not present
#endif

#ifdef T_MSM6500
/*===========================================================================

FUNCTION adie_clk_regime_config

DESCRIPTION
  This function configures the ADIE clock regime to either DIFFERENTIAL or
  SINGLE ENDED based on FEATURE_DIFFERENTIAL_RX_MOD_CLK.

  It will only configure the appropriate clock regime (RX0 or RX1) based
  on the device passed in, and it will enable the regime or disable
  the regime based on the wakeup parameter passed in.

DEPENDENCIES
  This function should be called to enable the regime before the registers
  on the ADIE are being accessed, and it should be called to disable
  the regime after the registers have been accessed.

RETURN VALUE
  None

SIDE EFFECTS
  turns on and off the clock regime.

===========================================================================*/
void adie_clk_regime_config(rf_path_enum_type rf_path, adie_op_type wakeup);
#endif

/*************** END OF MULTI-MODE ADIE HKADC DEFINITIONS ******************/

#endif /* ADIE_H */

