/*===========================================================================

                       E F U S E  H e a d e r  F i l e

DESCRIPTION
 Contains common defintion and apis to be used to read CONFIG Chain EFUSE
 values.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/efuse/src/efuse.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09   ddk     Efuse drivers support for ULC.
01/25/07   amw     Update efuses for 60x5.
07/05/06   anb     Added support to program/blow fuses via FTM.
06/02/06   anb     Changes ef_confg_data from static to static const
05/22/06   anb     Initial revision.

============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "efuse.h"
#include "msmhwioreg.h"
#ifdef BUILD_BOOT_CHAIN
#include "boot_msmhwio.h"
#else
#include "msmhwio.h"
#if defined(FEATURE_QFUSE_PROGRAMMING)
#include "pm.h"
#include "ftm_task.h"
#include "clk.h"
#include "efuse_target.h"
#endif
#endif


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* It is 32 bit magic number to determine whether the EFUSE needs special
 * treatment
 */
#define EFUSE_SPECIAL_ITEM                       0xDEADBEEF
/* SIZE of OEM HW IS in bits */
#define EFUSE_OEM_HW_ID_SIZE                     32         /* 32 bits */

#if defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN)
#define BLOW_TIMER_VALUE     0xC0     /* 10 usec at 19.2 MHz E-Fuse clock */
#define CONFIG_CHAIN_LENGTH  128      /* 128 fuses (64 bits) */

#endif /* defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN) */

/*---------------------------------------------------------------------------
  Structure that defines data that will be used by EFUSE API to get/read
  items from the EFUSE config register
---------------------------------------------------------------------------*/
typedef struct
{
  /* EFUSE ITEM                            */
  efuse_item_type   ef_item;
  /* Whether ITEM needs special treatment  */
  uint32            special_item;
  /* Address of register to read           */
  uint32            reg_addr;
  /* AND mask of valid bits that should be read for this ITEM  */
  uint32            reg_mask;
  /* Shift mask to get data right shifted  */
  uint32            reg_shift;
} efuse_config_chain_data_type;

/*---------------------------------------------------------------------------
  Array containing EFUSE item that can be read and associated registers,
  mask and shift values for the same.
---------------------------------------------------------------------------*/
static const efuse_config_chain_data_type ef_confg_data [] =
{
  {
    INCLUDE_OEM_HW_ID,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_EF_CONFIG_MSB_OEM_HW_ID_AUTH_ENABLE_BMSK,
    HWIO_EF_CONFIG_MSB_OEM_HW_ID_AUTH_ENABLE_SHFT
   },

  {
    SOFTWARE_CRASH_DEBUG,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_FMSK(EF_CONFIG_MSB, SOFTWARE_CRASH_DEBUG),
    HWIO_SHFT(EF_CONFIG_MSB, SOFTWARE_CRASH_DEBUG)
  },

  {
    JTAG_EN_N,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_FMSK(EF_CONFIG_MSB, JTAG_EN_N),
    HWIO_SHFT(EF_CONFIG_MSB, JTAG_EN_N)
  },

  {
    SECURE_BOOT,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_FMSK(EF_CONFIG_MSB, SECURE_BOOT),
    HWIO_SHFT(EF_CONFIG_MSB, SECURE_BOOT)
  },

  {
    TRUSTED_BOOT,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_FMSK(EF_CONFIG_MSB, BOOT_FROM_ROM),
    HWIO_SHFT(EF_CONFIG_MSB, BOOT_FROM_ROM)
  },

  {
    ROOT_KEY_SELECT,
    0,
    HWIO_ADDR(EF_CONFIG_MSB),
    HWIO_FMSK(EF_CONFIG_MSB, ROOT_KEY_SELECT),
    HWIO_SHFT(EF_CONFIG_MSB, ROOT_KEY_SELECT)
  },

  {
    OEM_HW_ID,
    EFUSE_SPECIAL_ITEM,
    0,
    0,
    0
  },

  {
    SW_FUSE_PGM_DSBL,
    0,
    HWIO_ADDR(EF_CONFIG_LSB),
    HWIO_FMSK(EF_CONFIG_LSB, SW_FUSE_PGM_DSBL),
    HWIO_SHFT(EF_CONFIG_LSB, SW_FUSE_PGM_DSBL)
  },

  {
    HW_KEY_PGM_DSBL,
    0,
    HWIO_ADDR(EF_CONFIG_LSB),
    HWIO_FMSK(EF_CONFIG_LSB, HW_KEY_PGM_DSBL),
    HWIO_SHFT(EF_CONFIG_LSB, HW_KEY_PGM_DSBL)
  },

  {
    MEM_REDUN_AND_CHIP_ID_DSBL,
    0,
    HWIO_ADDR(EF_CONFIG_LSB),
    HWIO_FMSK(EF_CONFIG_LSB, MEM_REDUN_AND_CHIP_ID_DSBL),
    HWIO_SHFT(EF_CONFIG_LSB, MEM_REDUN_AND_CHIP_ID_DSBL)
  },

  {
    CONFIG_PGM_DSBL,
    0,
    HWIO_ADDR(EF_CONFIG_LSB),
    HWIO_FMSK(EF_CONFIG_LSB, CONFIG_PGM_DSBL),
    HWIO_SHFT(EF_CONFIG_LSB, CONFIG_PGM_DSBL)
  },

};

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/
#if defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN)
/*===========================================================================

FUNCTION efuse_clear_chain

DESCRIPTION
  Function to clear the fuse chain. This function does NOT blow any fuses.

PARAMETERS:
 shift_value: This input specifies the bit position of the fuse to be blown.
              Shifts are performed from the MSB of the chain
 timer: value to program into the EF_BLOW_TIMER register.

DEPENDENCIES
  None

RETURN VALUE
  0 if successful

SIDE EFFECTS
  Permanent blowing of efuse. Possible disabling of MSM Hardware.

===========================================================================*/
/*lint -save -e715 code not reference */
void efuse_clear_chain
(
  uint32 shift_value,
  uint32 timer
)
{
  int status=0;

  HWIO_OUT(EF_CHAIN_SEL, HWIO_FMSK(EF_CHAIN_SEL,CONFIG_CHAIN_SEL));
  HWIO_OUT(EF_BLOW_VALUE, 0);
  HWIO_OUT(EF_SHIFT_VALUE, shift_value);

  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x0)));
  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x1)));

  /* Wait for the state machine to blow fuse */
  do
  {
    status = HWIO_IN(EF_STATUS);
  } while ( (status & HWIO_FMSK(EF_STATUS,STATUS_DONE)) !=
            HWIO_FMSK(EF_STATUS,STATUS_DONE)
          );

  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x0)));

}
/*lint -restore */

/*===========================================================================

FUNCTION efuse_blow_config_fuse

DESCRIPTION
  Function to blow a single fuse.

PARAMETERS:
 shift_value: This input specifies the bit position of the fuse to be blown.
              Shifts are performed from the MSB of the chain
 timer: value to program into the EF_BLOW_TIMER register.

DEPENDENCIES
  None

RETURN VALUE
  0 if successful

SIDE EFFECTS
  Permanent blowing of efuse. Possible disabling of MSM Hardware.

===========================================================================*/
void efuse_blow_config_fuse
(
  uint32 shift_value,
  uint32 timer
)
{
  int status=0;

  /* Clear out any Values in the Chain_select register */
  HWIO_OUT(EF_CHAIN_SEL, HWIO_FMSK(EF_CHAIN_SEL,CONFIG_CHAIN_SEL));
  HWIO_OUT(EF_BLOW_TIMER, timer);
  HWIO_OUT(EF_BLOW_VALUE, 1);
  HWIO_OUT(EF_SHIFT_VALUE, shift_value);

  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x0)));
  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x1)));

  /* Wait for the state machine to blow fuse */
  do
  {
    status = HWIO_IN(EF_STATUS);
  } while ( (status & HWIO_FMSK(EF_STATUS,STATUS_DONE)) !=
            HWIO_FMSK(EF_STATUS,STATUS_DONE)
          );

  (*((volatile uint32 *) (HWIO_ADDR(EF_STATUS))) = ((dword) (0x0)));

  /* Clear the chain selection */
  HWIO_OUT(EF_CHAIN_SEL, 0);

  /* For safety's sake, set the value to be blown back to 0. */
  HWIO_OUT(EF_BLOW_VALUE, 0);

}

/*===========================================================================

FUNCTION efuse_blow_config_chain_bit

DESCRIPTION

  Function to blow an even/odd pair of configuration fuses. This function
  expects a bit number and calculates the corresponding even and odd
  locations in the Qfuse chain.

PARAMETERS:
   bit: This input specifies the logical bit to be blown within the
        config chain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Permanent blowing of efuse. Possible disabling of MSM Hardware.

===========================================================================*/
void efuse_blow_config_chain_bit(uint32 bit)
{
  uint32 fuse_position=0;
  uint32 shift_value=0;

  /*------------------------------------------------------------------------
    The configuration chain is implemented as a 128-bit chain; the bits are
    duplicated to provide redundancy such that each read-back bit contains
    an even-numbered and the next odd-numbered fuse thus forming a 64 fuse
    bit residing in 128-bit configuration chain (bit i in 64 bit will occupy
    fuse 2*i and (2*i) + 1 in the 128-bit chain)

    These 64 bit are further divided into two 32 bits register, EF_CONFIG_LSB
    and EF_CONFIG_MSB, which contains all the configuration fuses which 6K
    series support. Each configuration fuse is made up of one; two or
    multiple bits (please refer to the software interface document
    for more details)

    When writing/blowing a configuration fuse:
    1.First determine the fuse position in the 64 bit register.
      For example
      CONFIG_PGM_DSBL fuse occupies bit 0 in LSB register and it occupies
      bit 0 in 64 bit.
      TRUSTED_BOOT fuse occupies bit 9 in MSB register and it occupies
      bit 41 in 64 bit.

      So for a configuration fuse located in MSB register one has to add 32
      to it in order to get the correct bit position in 64 bit register.

    2.Using the bit position calculated above, say m, program the bit
      position (2*m) and (2*m)+1 in the 128-bit configuration chain.
  ---------------------------------------------------------------------------*/

  /*--------------------------------------------------------------
    First blow the even fuse.
  ---------------------------------------------------------------*/
  fuse_position =  bit * 2;
  shift_value = CONFIG_CHAIN_LENGTH - fuse_position;
  FTM_MSG_FATAL("BitToBlow:%d SHIFT_VALUE:%d", bit, shift_value, 0);

  /*--------------------------------------------------------------
    Now clear the chain and blow the fuse for the even position.
  ---------------------------------------------------------------*/
  efuse_clear_chain(CONFIG_CHAIN_LENGTH, BLOW_TIMER_VALUE);
  efuse_blow_config_fuse(shift_value, BLOW_TIMER_VALUE);

  /*--------------------------------------------------------------
    Now blow the odd fuse.
  ---------------------------------------------------------------*/
  fuse_position++;
  shift_value = CONFIG_CHAIN_LENGTH - fuse_position;
  FTM_MSG_FATAL("BitToBlow:%d SHIFT_VALUE:%d", bit, shift_value, 0);

  /*--------------------------------------------------------------
    Now clear the chain and blow the fuse for the even position.
  ---------------------------------------------------------------*/
  efuse_clear_chain(CONFIG_CHAIN_LENGTH, BLOW_TIMER_VALUE);
  efuse_blow_config_fuse(shift_value, BLOW_TIMER_VALUE);

}

/*===========================================================================

FUNCTION  EFUSE_WRITE_ITEM_INIT

DESCRIPTION
 Here the assumption is that the QFUSE circuit is connected to Vreg_UIM. This
 function is going to use PMIC library API to control the voltage level for
 this LDO.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Any Errors while setting up the system before blowing the fuses.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_item_init( void )
{
  return efuse_target_write_item_init();
}

/*===========================================================================

FUNCTION  EFUSE_WRITE_ITEM_CLEANUP

DESCRIPTION
 This function is going to use PMIC library API to trun OFF the LDO.

DEPENDENCIES
 None.

RETURN VALUE
 uint32   Any Errors while doing the cleanup.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_item_cleanup( void )
{
  return efuse_target_write_item_cleanup();
}

/*===========================================================================

FUNCTION  EFUSE_READ_CONFIGURATION_CHAIN

DESCRIPTION
 Config Chain EFUSE registers stores information which needs to read and
 retrieved for the caller.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Data read from the EFUSE register.
         Any Errors while reading the specified item.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_read_configuration_chain
(
  uint32   *config_chain_data
)
{
  uint32 *data_ptr;

  /*------------------------------------------------------------------------
    Make sure the pointer to store the data is not NULL.
   ----------------------------------------------------------------------*/
  if (config_chain_data == NULL)
  {
    return EFUSE_DATA_PTR_NULL_ERR;
  }

  data_ptr = config_chain_data;
  *data_ptr = HWIO_IN(EF_CONFIG_LSB);
  data_ptr++;
  *data_ptr = HWIO_IN(EF_CONFIG_MSB);

  return EFUSE_NO_ERR;
}


/*===========================================================================

FUNCTION  EFUSE_WRITE_CONFIGURATION_CHAIN

DESCRIPTION
 Write to Config Chain EFUSE registers,

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Any Errors while writing/blowing configuration chain.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_write_configuration_chain
(
  uint32   *config_chain_data
)
{
  efuse_err_enum_type efuse_err_status = EFUSE_NO_ERR;
  uint32 bit_position,i;
  uint32 add_value = 0;
  uint32 config_data;
  uint32 bit_to_blow;
  uint32 *data_ptr;
  boolean efuse_is_write_allowed;

  /*------------------------------------------------------------------------
    Make sure we are allowed to do a write operation....
   ----------------------------------------------------------------------*/
  efuse_is_write_allowed = efuse_is_write_operation_allowed();
  if (efuse_is_write_allowed == FALSE)
  {
    efuse_err_status = EFUSE_OPERATION_NOT_ALLOWED;
    return  efuse_err_status;
  }
  /*------------------------------------------------------------------------
    Make sure the pointer to read data from is not NULL.
   ----------------------------------------------------------------------*/
  if (config_chain_data == NULL)
  {
    efuse_err_status = EFUSE_DATA_PTR_NULL_ERR;
    return  efuse_err_status;
  }

  data_ptr = config_chain_data;
  config_data = *data_ptr;
  for (i=0; i<=(uint32)EFUSE_CONFIG_CHAIN_MSB_SEL; i++)
  {
    add_value = 32*i;

    for (bit_position=0; bit_position<32; bit_position++)
    {
      if (config_data & 0x1)
      {
        bit_to_blow = ( bit_position + add_value );
        //FTM_MSG_FATAL("BitToBlow:%d", bit_to_blow, 0, 0);
        efuse_blow_config_chain_bit(bit_to_blow);
      }
      config_data >>=1;
    }
    data_ptr++;
    config_data = *data_ptr;
  }

  return efuse_err_status;
}
#endif /* defined(FEATURE_QFUSE_PROGRAMMING) && !defined(BUILD_BOOT_CHAIN) */

/*===========================================================================

FUNCTION  EFUSE_GET_ITEM

DESCRIPTION
 Config Chain EFUSE registers stores information which needs to read and
 retrieved for the caller.

DEPENDENCIES
 None.

RETURN VALUE
 uint32  Data read from the EFUSE register.
         Any Errors while reading the specified item.

SIDE EFFECTS
 None.
===========================================================================*/
efuse_err_enum_type efuse_get_item
(
  efuse_item_type  config_item,
  uint32           *config_chain_data
)
{
  efuse_err_enum_type efuse_err_status = EFUSE_NO_ERR;
  uint32 index,max_efuse_item;

  /*------------------------------------------------------------------------
    Make sure the pointer to store the data is not NULL.
   ----------------------------------------------------------------------*/
  if (config_chain_data == NULL)
  {
    efuse_err_status = EFUSE_DATA_PTR_NULL_ERR;
    return  efuse_err_status;
  }

  /*------------------------------------------------------------------------
    Make sure the item to read is within valid range.
   ----------------------------------------------------------------------*/
  if (config_item >= MAX_CONFIG_CHAIN_ITEM)
  {
    efuse_err_status = EFUSE_NO_CONFIG_CHAIN_ITEM_FOUND_ERR;
    return  efuse_err_status;
  }

  /*------------------------------------------------------------------------
    Go through ef_confg_data array to find the matching ITEM
   ----------------------------------------------------------------------*/
  max_efuse_item = (sizeof(ef_confg_data)/sizeof(efuse_config_chain_data_type));
  for (index = 0; index < max_efuse_item; index++)
  {
    if (ef_confg_data[index].ef_item == config_item)
    {
      /*---------------------------------------------------------------
        Found the item we are looking for
       ---------------------------------------------------------------*/
      if (ef_confg_data[index].special_item)
      {
        /*--------------------------------------------------------------
          Does this item need any special treatment.
         ---------------------------------------------------------------*/
        if (ef_confg_data[index].ef_item == OEM_HW_ID)
        {
          uint32 lsb_read, msb_read;

          lsb_read = HWIO_IN(EF_CONFIG_LSB);
          lsb_read = lsb_read & HWIO_FMSK(EF_CONFIG_LSB,OEM_HW_ID);
          lsb_read = lsb_read >> HWIO_SHFT(EF_CONFIG_LSB,OEM_HW_ID);

          msb_read = HWIO_IN(EF_CONFIG_MSB);
          msb_read = msb_read & HWIO_FMSK(EF_CONFIG_MSB,OEM_HW_ID);
          msb_read = msb_read >> HWIO_SHFT(EF_CONFIG_MSB,OEM_HW_ID);

          msb_read = msb_read <<
                    (EFUSE_OEM_HW_ID_SIZE - HWIO_SHFT(EF_CONFIG_LSB,OEM_HW_ID));
          *config_chain_data = (msb_read  | lsb_read);
          break;
        }
        else
        {
          /*--------------------------------------------------------------
            Currently this special item is not supported.
           ---------------------------------------------------------------*/
          efuse_err_status = EFUSE_NO_SPECIAL_ITEM_FOUND_ERR;
          return  efuse_err_status;
        }
      }
      else
      {
        /*--------------------------------------------------------------
          ITEM to read is not a special item read from the register
         ---------------------------------------------------------------*/
        *config_chain_data = ( in_dword(ef_confg_data[index].reg_addr)
                               & ef_confg_data[index].reg_mask
                             ) >>  ef_confg_data[index].reg_shift;
        break;
      }
    }
  }

  return efuse_err_status;
}
