#ifndef SBI_H
#define SBI_H

/*===========================================================================

              Serial Bus Interface   H E A D E R   F I L E

DESCRIPTION
  This module contains all the constants, function declarations, macro 
  definitions and register declarations for the SBI modules.

Copyright (c) 1998-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/sbi.h_v   1.4   07 Jun 2002 12:54:54   ehandojo  $
  $Header: //depot/asic/msmshared/drivers/sbi/SSBI/sbi.h#8 $ $DateTime: 2007/12/12 04:00:23 $ $Author: gbhat $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/06   gb      Changes for PMIC SBI auto detection
03/03/06   hkk     Changes for having SSBI on MSM6800 65nm
02/08/06   ao      changes for 6280 sbi_set_ssbi_data_delay
07/16/04    et     added #ifndef T_MSM6500 around 3 extern definitions that 
                   were not used by 6500
09/12/03   jcw     implement library only include correctly
08/14/03   aks     Make SBI MDSP definitions available for non-Library use.
04/09/03   aks     Took out symbol which is not used.
03/18/03   jcw     Added sbi_handle_runtime_error to externalize error handling.
03/19/03   aks     Changed featurization to use flag supplied during
                   Library creation.
03/17/03   pgh     Cleaned some warnings
02/12/03   aks     Wrapped functions used just for FEATURE_INHIBIT_PSBI.
01/16/03   aks     Feature-wrap for MSM6200 target.
12/17/02   aks     Adjust to allow sbi.h to be exported, with previous
                   targets which did not export it.
11/19/02   aks     Fixed syntax and flag error.
11/19/02   aks     Merged with MSM6300.  Featurized the API of
                   sbi_results_ready().
10/11/02   gw      Added function sbi_queues_empty() to support mode switching
                   for GSM sleep.
10/08/02   ih      Added local result buffer pointer to the callback interface
09/27/02   jc      Added sbi_get_mode() as a temp func for compressed mode.
09/25/02   ih      Added support for transaction status check in MDSP mode
06/06/02    eh     Merged the Multimode SBI changes.
06/05/02    eh     Removed all ZIF chips slave ID.
05/08/02   thh     Added type definition for the different SBI clock speeds
                   for the MDSP controller.
                   Defined the SBI modes.
05/08/02   thh     Added GSM support.  Function prototypes have changed as
04/18/02    sd     Added SBI ID from RTR6200.
04/05/02    eh     Added SBI ID for RFR6200, RFL6200, PM6000, and PM6050.
03/25/02    eh     Added FEATURE_TCXO2600.
01/24/02    eh     Port for MSM6200
11/12/01   dsb     Analog die SBI ID for MSM6050 is now 0x22.
09/14/01   dsb     Added sbi_write_verify_byte, cleaned up for MSM6050.
05/11/01   dsb     Removed sbi_chk_mchn_busy.
03/06/01   tn      restored SBI id names for IFR and RFT to previous names.
03/05/01   dsb     Added inclusion of msm.h.  Changed SBI ID constants to 
                   safer names.  eg.  IFR is now SBI_IFR_ID.  SBI_ADIE_ID
                   is no longer conditionally compiled by T_MSM_5100.
                   Changed interface to sbi_read_bytes: it now takes an
                   array of registers to read from, as opposed to a 
                   base address.   Similarly, with sbi_write_bytes, 
                   support for the "old" API was removed.
01/27/00   jc      Added sbi_pre_write_byte(), sbi_post_write_byte().
01/20/00   jc      Added SBI_16KHZ.
09/09/99   jc      Added Slave Id definition of PM1000.
06/18/99   ymc     Added Slave Id definition of RFT3100.
02/17/99   nzaveri removed the sbi_twice_write_r function and replaced it with
         sbi_write_twice function
12/18/98   nzaveri added a new function sbi_write_twice_r that writes twice to
                   register x and reads from register y. This function is 
                   specific needed for the adc_read operation.
10/19/98   nzaveri initial version for sbi.h.

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "bio.h"

#if defined(FEATURE_LIBRARY_ONLY)
#include "sbi_lib_api.h"
#endif

/* type for configuring the GPIO (bit-banged) SBI lines */
typedef enum{
  SBI_CLOCK_SIG,
  SBI_DATA_SIG,
  SBI_LATCH_SIG
} sbi_gpio_sig_type;

typedef enum{
  SBI_SSBI,
  SBI_FTM_SSBI,
  SBI_3WIRE,
  SBI_3WIRE_GPIO,
  SBI_UNKNOWN
} sbi_wire_type;

/* Values used to specify which bus */
/* Note that only the following devices are supported on GPIO buses, and
   not all devices are on GPIO buses for all targets.  Consult your MSM
   training material for your project to determine which devices are on
   GPIO buses
 */
extern const int SBI_RGR_GPIO_BUS_ID;
extern const int SBI_PMIC_GPIO_BUS_ID;
extern const int SBI_RFT_GPIO_BUS_ID;
extern const int SBI_SLIDER_PMIC_GPIO_BUS_ID;

/*=========================================================================

FUNCTION    SBI_GPIO_CONFIG

DESCRIPTION
  This functions configures the selected GPIOs to be used as for bit-banged
  sbi buses.  The  user passes in the bus identifier, signal (clock, latch or 
  data), the register, and the mask they want to use, and the function 
  assigns these values to runtime variables that will be used throughout.
  
DEPENDENCIES
  This must be called for all GPIOs that are different from the default
  configuration before rflib_init_gsm or rflib_init!!!!

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sbi_gpio_config(
  int bus_id,    /* aux bus id e.g. SBI_PMIC_GPIO_BUS_ID */
  sbi_gpio_sig_type signal, /* signal specifier */
  bio_gpio_type reg,        /* BIO specifier e.g. BIO_GPIO_81_REG */
  dword mask                /* BIO specifier e.g. BIO_GPIO_81_M */
);

#ifdef FEATURE_SSBI
/*=========================================================================

FUNCTION    SBI_SET_SSBI_DATA_DELAY

DESCRIPTION
  This functions configures the delay on the SSBI bus lines to be used.
  
DEPENDENCIES

RETURN VALUE
  TRUE on success. FALSE if called on a non-SSBI slave.

SIDE EFFECTS
  None.

===========================================================================*/
boolean sbi_set_ssbi_data_delay(int slave_id, uint8 data_del, uint8 sel_rd_data);
#endif

/*===========================================================================

FUNCTION sbi_configure_pmic

DESCRIPTION
  This function configures the PMIC to use traditional 3-wire SBI, SSBI, or
  FTM SSBI.
  
DEPENDENCIES
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sbi_configure_pmic( sbi_wire_type config );

#endif

