#ifndef ADIE_SBI_H
#define ADIE_SBI_H
/*===========================================================================


               A N A L O G   D I E  S B I  H E A D E R    F I L E

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the analog die of a stacked digital/analog die MSM for targets 
  who have SBI communcation with the analog die.

Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/adie_sbi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/06   hkk     Added adie_bbrx_read_byte and adie_bbrx_write_byte
06/06/06   hkk     Added adie_bbrx_write_mult prototype. 
05/22/06   hkk     Common Adie driver for 1x and UMTS 
11/30/05    xw     added adie_bbrx_write_byte() prototype.
05/10/04    et     moved adie_system_mode type to adie.h
12/05/03    et     removed #include of sbi.h
11/11/03    et     including adie_adc.h
11/11/03    et     removed adie_read_bytes() prototype and added 
                   adie_adc_sbi_callback() prototype
11/01/02   aks     Added support for RF_HAS_MULTIMODE_BBRX.
03/21/02   aks     Changed macros to call Library API.
03/21/02   aks     Remove sbi header inclusion.
12/04/01   rmd     adie re-org
10/25/01    et     added ADIE_WRITE_BYTES macro to work with SBI
10/19/01   aks     Changed ADIE_WRITE_VERIFY_I and ADIE_IN_I to return ADIE
                   type.  Removed MSMHW_ADIE_INTF_IS_SBI wrap.
10/09/01   aks     Removed duplicated definitions, add adie_regdefs.h.  Change
                   flag to MSMHW_ADIE_INTF_IS_SBI.
09/05/01    et     created

===========================================================================*/

#include "comdef.h"
#include "clk.h"
#include "adie.h"
#include "adie_adc.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* Array used by software to buffer the write  */
extern byte adie_image[ ];



/* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  */

/* Function prototypes   
*/
extern void adie_init( adie_system_mode rf_mode );

#ifdef ADIE_DEBUG
#error code not present
#endif

/* Use this type to define the reg addr pass into ADIE_IN() */
#define adie_addr_type      uint8

/* Use this type to define the varible which will store the value return from
   ADIE_IN() */

#define adie_data_type      uint8


#ifdef FEATURE_HDET_USES_DM
#ifdef FEATURE_MEMORY_VIRTUAL
#define ADIE_MDSP_HDET_CMD_DST  HWIO_SSBI4_SSBI_CMD_PHYS
#else
#define ADIE_MDSP_HDET_CMD_DST  HWIO_SSBI4_SSBI_CMD_ADDR
#endif
#endif


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO ADIE_OUT_BUF_I

DESCRIPTION
  This macro changes the shadow buffer for a particular analog die register
  without actually performing a write.  Thus, any changes specified by this 
  macro will not take effect until the next analog die SBI write operation to
  the same register.

PARAMETERS
  reg     the address of the analog die register buffer to alter.
  mask    the mask to use, 1s enable bit update
  value   the value to write to the register buffer

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define ADIE_OUT_BUF_I(reg, mask, value )\
  ( adie_image[reg] = ( (adie_image[reg] & ~(mask) )\
                     | ( (value) & (mask) ) ) )


/*===========================================================================

MACRO ADIE_IN_I

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

#define ADIE_IN_I( reg_array, var )         \
  (                                         \
    adie_read_byte( reg_array, var )              \
  )
  

/*===========================================================================

MACRO ADIE_OUTM_VERIFY_I

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

#define ADIE_OUTM_VERIFY_I( reg, mask, value)                               \
  (                                                                         \
    adie_write_verify( reg, mask, value )                                   \
  )


/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION adie_write_verify

DESCRIPTION
  This function outputs a specified value, qualified by a specified mask,
  to a specified analog die.  Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values.  To do this,
  an image is maintained of the previous value written to the register
  which is then combined with the new value. This is done through the SBI.

PARAMETERS
  reg     the address of the analog die register to write to.
  mask    the mask to use, 1s enable bit update
  value   the byte value to write to the register

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,

SIDE EFFECTS
  None

===========================================================================*/
adie_result_type adie_write_verify(byte reg, byte mask, byte value );


/*===========================================================================

FUNCTION adie_write_mult

DESCRIPTION
  This function takes as an input the address to an array or registers,
  the address to an array of values, and a length, and writes the values to the
  corrisponding registers.  This is done through the SBI.

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
  None
===========================================================================*/
extern adie_result_type adie_write_mult(byte *addr, byte *data, byte length);

/*===========================================================================

FUNCTION adie_read_byte

DESCRIPTION
  this function reads a value from a specified address, and stores it in
  a data field passed in.

PARAMETERS
  addr    the address of the analog die register to read from
  * data  the location where the value read is to be stored

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,

SIDE EFFECTS
  None

===========================================================================*/
adie_result_type adie_read_byte( uint8 addr, uint8 *data );

/*===========================================================================

FUNCTION adie_read_bytes                            EXTERNALIZED FUNCTION

DESCRIPTION
  This function reads data from the ADIE. It takes as input the slave id,
  register Address and the no of bytes that need to be read from the
  slave. Input is returned in the data ptr. Can be used to read data from
  sequential register addresses.

DEPENDENCIES

RETURN VALUE
  SBI_SUCCESS on a successful read
  SBI_FAILURE on a failure

SIDE EFFECTS
  If the MACRO SBI_CHK_BUSY_WRITE/SBI_CHK_BUSY_READ time out then you will
  return from the function with a SBI_FAILURE.

===========================================================================*/
adie_result_type adie_read_bytes
(
  uint8 *addr,
  uint8 *data,
  uint8 length

);

/*===========================================================================

FUNCTION adie_write_byte

DESCRIPTION
  This function outputs a specified value, qualified by a specified mask,
  to a specified analog die.  Only the bits corresponding to the mask are 
  actually affected; other bits retain their previous values.  To do this, 
  an image is maintained of the previous value written to the register 
  which is then combined with the new value. This is done through the SBI.
  
PARAMETERS
  reg     the address of the analog die register to write to.
  mask    the mask to use, 1s enable bit update
  value   the byte value to write to the register

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,

SIDE EFFECTS
  None

===========================================================================*/
extern adie_result_type adie_write_byte(byte reg, byte mask, byte value);

/*===========================================================================
FUNCTION adie_adc_sbi_callback                        INTERNAL FUNCTION

DESCRIPTION
    This function is a SBI call back function set up in
  "adie_read_adc_conv()" which gets called when we are done reading
  the ADIE HKADC status and data registers.

INPUT PARAMETERS
  1) Parameter name: sbi_st.
     - This parameter is passed by the SBI driver. It indicates
       if the SBI operation was successful or not.

     Parameter type: sbi_pending_result_type (enum).
        SBI_PENDING_RESULT_FAILURE = SBI request failed.
        SBI_PENDING_RESULT_SUCCESS = SBI request successful.

  2) Parameter name: token.
     - This parameter is passed by the SBI driver. THIS
       PARAMETER IS NOT USED IN THIS FUNCTION.

     Parameter type: void*.

  3) Parameter name: data.
     - This parameter is passed by the SBI driver. It contains
       the pointer to the status and data registers values read
       from the ADIE.

     Parameter type: byte* (pointer).

RETURN VALUE
  None.

DEPENDENCIES
  Call the following functions before calling this function:
  1) rflib_init()
  2) adie_init()
  3) adie_read_adc_conv()

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void adie_adc_sbi_callback(boolean sbi_st,
                                 void *token, byte *data);


/*===========================================================================
FUNCTION adie_bbrx_write_mult

DESCRIPTION
  This function takes as an input the address to an array or registers,
  the address to an array of values, and a length, and writes the values to the
  corrisponding registers.  This is done through the SBI.

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
  None
===========================================================================*/
adie_result_type adie_bbrx_write_mult(byte *addr, byte *data, byte length );

/*===========================================================================

FUNCTION adie_bbrx_write_byte

DESCRIPTION
  This function outputs a specified value, qualified by a specified mask,
  to a specified analog die.  Only the bits corresponding to the mask are
  actually affected; other bits retain their previous values.  To do this,
  an image is maintained of the previous value written to the register
  which is then combined with the new value. This is done through the SBI.

PARAMETERS
  reg     the address of the analog die register to write to.
  mask    the mask to use, 1s enable bit update
  value   the byte value to write to the register

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,

SIDE EFFECTS
  None

===========================================================================*/
adie_result_type adie_bbrx_write_byte(byte reg, byte mask, byte value );

/*===========================================================================

FUNCTION adie_bbrx_read_byte

DESCRIPTION
  this function reads a value from a specified address, and stores it in
  a data field passed in.

PARAMETERS
  addr    the address of the analog die register to read from
  * data  the location where the value read is to be stored

DEPENDENCIES
  Interrupts should be disabled.

RETURN VALUE
  ADIE_SUCCESS for a successful operation,

SIDE EFFECTS
  None

===========================================================================*/
adie_result_type adie_bbrx_read_byte( uint8 addr, uint8 *data );

#endif /* ADIE_SBI_H */

