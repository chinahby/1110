#ifndef SBI_LIB_API_H
#define SBI_LIB_API_H
/*==========================================================================

         Serial Bus Interface ( SBI ) Library API H E A D E R   F I L E

DESCRIPTION
  This module contains all the constants, register definitions and macro 
  definitions for the "private" interface to the sbi module. This is a collection 
  of non-public items. 

Copyright (c) 1998-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/sbii.h_v   1.2   07 Jun 2002 12:55:24   ehandojo  $
  $Header: //depot/asic/msmshared/drivers/sbi/SSBI/sbi_lib_api.h#11 $ $DateTime: 2007/12/20 11:52:31 $ $Author: gbhat $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/07   gb      Exposing sbi_config_pmic_io_bus api
12/12/07   gb      Removed FEATURE_SLIDER
11/28/06   bmg     C++ compatibility fixes
03/03/06   hkk     Changes for having SSBI on MSM6800
02/08/06   ao      changed RF_HAS_MULTIMODE_SBI to FEATURE_MULTIMODE_SBI
04/27/05   jtn     Initial changes to support multiple RTR chips with single RF driver.
03/29/05   ao      added #define SBI_RFR6250_ID & #define SBI_RFR6220_ID 
03/23/05   ao      Added ADIE_CODEC_ID for Saturn. Saturn bbrx and codec ssbi are 
                   on separate busses.
11/17/03   ih      Changed sbi_results_ready to accept number of results from
                   mdsp_intf instead of keeping track of it internally.
11/17/03   ih      Added RTR6250 alias to RTR0
08/11/03   aks     Add prototype.
06/24/03   jcw     added backwards-compatible hw_req apis
06/02/03   aks     Removed unused function, added sbi_idle_check() and
                   sbi_itm_write_byte() from the RF entity.
05/19/03   jcw     port for MSM6250 from msmshared/drivers/sbi/mm/sbii.h#5
05/08/03   aks     Changed all multibyte to single byte operations.  Wrapped
                   under SBI_HAS_BUSY_DELAY flag.  Part of the fix for SBI
                   hardware block arbitration issue.
04/14/03   dbc     Mainline RFT6120 & RFT6100 SBI IDs so they can be built 
                   in the same library  
03/27/03   jcw     Lint cleanup
03/19/03   aks     Group slave IDs to make more obvious.
12/17/02   aks     Adjust to allow mutual config with previous targets which
                   did not export sbi.h.
11/19/02   aks     Merged with MSM6300.  Renamed symbol to avoid conflict
                   with MSM6100 version of HWIO.
06/06/02    eh     Merged the Multimode SBI changes.
06/05/02    eh     Removed SBI HW Req write from SBI_SLAVE_SEL.
05/09/02   thh     Added support for the MDSP controller.  This change is
                   featurized via the FEATURE_SBI_MDSP_CONTROLLER.
04/05/02    eh     Modified the SBI HW request priority.
01/24/02    eh     Port for MSM6200.
01/09/02   ssw     Fix for ZIF RX DC Correction problems. 
09/14/01   dsb     Cleaned up for MSM6050.
05/18/01   dsb     SBI_CHK_BUSY_OP and SBI_START have now been updated to
                   poll the MSM_INT_STATUS register for SBI completion
                   (as opposed to SBI_INT_STATUS).  Changed MSM_OUT to
                   MSM_OUTH for START_CTL in SBI_START.
03/05/01   dsb     Added inclusion of msm.h.  Changed T_MSM_5100 ifdef to
                   MSMHW_SBI_HAS_HW_REQ and MSMHW_SBI_HAS_FIFO.  Added
                   support for "burst" operations as SBI in MSM5100 now
                   has a FIFO.   Added SBI_CHK_BUSY_OP macro which checks
                   for the end of ANY SBI operation.  Added a new version
                   of SBI_START to support the SBI_CHK_BUSY_OP.
12/18/98   nz      made MACROS for SBI_START and SBI_SLAVE_SEL to improve
                     timing requirements for the adc read operation. 
10/19/98   nz      initial version for sbii.h


===========================================================================*/

/***************************************************************************
* Include Files
****************************************************************************/
#include "err.h"
#include "msm.h"
#ifdef FEATURE_RFM_INTERFACE
#include "rfm.h"
#else
#include "rficap.h"
#endif

#ifndef FEATURE_LIBRARY_ONLY
#error This file can only be included from library code via sbi.h.  It wont ship to customers.
#endif

/* This enumeration is the logical slave ID.  An enum is provided for all
   chips supported by SBI.  Aliases are defined below.  New code should use
   the generic IDs, since this is logical, and won't have to be updated 
   every time a new device is introduced (since physical addresses are 
   specified in the sbi_xxxx.c tables, which are updated for each chip
*/
typedef enum
{
  SBI_INVALID_ID = 0,
  SBI_PMIC_ID,       /* Generic PMICs */
  SBI_ADIE_ID,       /* Generic Analog die slave ID */
  SBI_RTR0_ID,       /* Generic ZIFTRIC - chain 0 */
  SBI_RTR1_ID,       /* Generic GZIFTRIC 1 */
  SBI_RTR2_ID,       /* Generic GZIFTRIC 2 */
  SBI_RFL0_ID,       /* Generic LNA - chain 0 */
  SBI_RFL1_ID,       /* Generic LNA - chain 1 */
  SBI_RFR0_ID,       /* Generic CDMA Rx - chain 0 */
  SBI_RFR1_ID,       /* Generic CDMA Rx - chain 1 */
  SBI_RFR2_ID,       /* Generic CDMA Rx - chain 2 */
  SBI_RGR0_ID,       /* Generic GPS Rx - chain 0 */
  SBI_RFT0_ID,       /* Generic CDMA Tx - chain 0 */
  SBI_ADIE_CODEC_ID, /* Generic Analog die slave ID codec + adc */
  SBI_MBP_RF_ID,     /* RF receiver on MBP1xxx */
  SBI_MBP_ADC_ID,    /* ADC on MBP1xxx */
  SBI_SLDR_PMIC_ID,  /* PMIC on Slider UBM daughter card */
  SBI_NUM_SLAVE_IDS  /* This must be last */
} sbi_slave_id_type;

/* Aliases to support existing (non-generic) code. New code should use generics
 * Rema promised to clean these up for Raven 1.0
 */

#define SBI_RFR6125_ID  SBI_RFR0_ID
#define SBI_RFR6000_ID  SBI_RFR0_ID
#define SBI_RFR6200_ID  SBI_RFR0_ID
#define SBI_RFR6250_ID  SBI_RFR1_ID
#define SBI_RFR6220_ID  SBI_RFR2_ID
#define SBI_RFR6275_ID  SBI_RFR0_ID

#define SBI_RFT6120_ID  SBI_RFT0_ID
#define SBI_RFT6100_ID  SBI_RFT0_ID

#define SBI_RTR6300_ID  SBI_RTR0_ID
#define SBI_RTR6250_ID  SBI_RTR0_ID
#define SBI_RTR6275_ID  SBI_RTR0_ID

#ifdef RF_HAS_RTR6200_FOR_CAL
#define SBI_RTR6200_ID  SBI_RTR1_ID
#else
#define SBI_RTR6200_ID  SBI_RTR0_ID
#endif

#define SBI_RFL6000_ID  SBI_RFL0_ID
#define SBI_RFL6200_ID  SBI_RFL0_ID

/****************************************************************
* Supported Bus frequencies
* Note that these are divider values.  As such, if you change the SBI
* clock source, these logical values (in MHZ or KHZ) change.
* SBI clock is sourced by PERIPH regime.
* SBI clock ratio for SBI_CLK_CTL::CLKCTL register.
* This clock ratio is based on
* master SBI core clock rate / desired I3Q clock rate
***********************************************************************/

typedef enum
{
   /* 19.2 MHz */
   SBI_9p6MHZ  = 2,
   SBI_6p4MHZ  = 3,
   SBI_4p8MHZ  = 4,
   SBI_3p84MHZ = 5,
   SBI_3p2MHZ  = 6,
   SBI_2p74MHZ = 7,
   SBI_2p4MHZ  = 8,
   SBI_2p13MHZ = 9,
   SBI_1p92MHZ = 10,
   SBI_1MHZ    = 19,
   /* If master SBI core clock rate is TCXO and desired I3Q clock rate is */
   /* 500Khz then the SBI clock ratio = 19.2 MHz / 0.5 MHz = 38.4 ~= 38.  */
   SBI_500KHZ  = 38,

   SBI_16KHZ   = 2     /* for use with sleep clock as the clock source */
} sbi_freq;


/***************************************************************
* Result Type
****************************************************************/
typedef enum
{
   SBI_FAILURE, /* SBI request failed */
   SBI_SUCCESS, /* SBI request successfully processed */
   SBI_PENDING  /* SBI request pending.  Only use in GSM Rx & Tx modes only */
} sbi_result_type;


/****************************************************************
* List of Hardware Request channels
*****************************************************************/
typedef enum
{
  SBI_HW_REQ1,
  SBI_HW_REQ2,
  SBI_HW_REQ3,
  SBI_HW_REQ4
} sbi_hw_req_type;


/***************************************************************
* Pending Result Type
****************************************************************/
typedef enum {
  SBI_PENDING_RESULT_FAILURE,   /* Pending result ready.  SBI request failed */
  SBI_PENDING_RESULT_SUCCESS    /* Pending result ready.  SBI request successful */
} sbi_pending_result_type;

/****************************************************************
* List of SBI bus types
*****************************************************************/
typedef enum
{
  SBI_BUS_SBI,
  SBI_BUS_SSBI,
  SBI_BUS_GPIO
} sbi_bus_type;


#ifdef __cplusplus
extern "C"
{
#endif
/***************************************************************
* SBI Callback pointer type
****************************************************************/
typedef void (*sbi_cb_ptr_type)(sbi_pending_result_type, void *, byte *);

/*===========================================================================

FUNCTION sbi_init

DESCRIPTION
  This function initializes the Master Serial Bus Controller. It takes as an
  input the desired bus frequency and initializes the serial bus.  
  
DEPENDENCIES
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sbi_init(sbi_freq freq);

/*===========================================================================

FUNCTION sbi_set_device_id

DESCRIPTION
  This function sets the physical SBI address for a given logical 
  sbi slave id.  This can be used for run time configuration.  
  
DEPENDENCIES
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sbi_set_device_id(sbi_slave_id_type slave_id, uint8 device_id);

/*===========================================================================

FUNCTION sbi_get_device_id

DESCRIPTION
  This function gets the physical SBI address for a given logical 
  sbi slave id.  This can be used for run time checking of the device
  id that can be read from the device
  
DEPENDENCIES
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
uint8 sbi_get_device_id(sbi_slave_id_type slave_id);

/*===========================================================================

FUNCTION sbi_read_bytes

DESCRIPTION
  This function reads data from the slave. It takes as input the slave id, 
  array of register addresses and the no of bytes that need to be read from 
  the slave.  Input is returned in the data ptr. Can be used to read data
  from multiple register addresses.  
    
DEPENDENCIES
 
RETURN VALUE
  SBI_SUCCESS on a successful read
  SBI_FAILURE on a failure
  
SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#else
sbi_result_type sbi_read_bytes( sbi_slave_id_type slave_id,
                                uint8 *addr,
                                uint8 *data,
                                uint8 length );

#endif /* FEATURE_MULTIMODE_SBI */
/*===========================================================================

FUNCTION sbi_read_byte

DESCRIPTION
  This function reads a byte of data from the slave. 
  It takes as input the slave id, register address and the data that
  need to be read from the slave.  
  Input is returned in the data ptr. Can be used to read data
  from one register address.  
    
DEPENDENCIES
 
RETURN VALUE
  SBI_SUCCESS on a successful read
  SBI_FAILURE on a failure
  
SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#else
sbi_result_type sbi_read_byte( sbi_slave_id_type slave_id,
                               uint8 addr,
                               uint8 *data );

#endif /* FEATURE_MULTIMODE_SBI */
/*===========================================================================

FUNCTION sbi_write_bytes

DESCRIPTION
  This function writes data to the slaves. Takes as input the slave id, 
  array of register addresses, the data to be transmitted, and the
  no_of_bytes. Can write data to multiple registers in the slave. 
       
DEPENDENCIES
 
RETURN VALUE
  SBI_SUCCESS on a successful write
  SBI_FAILURE on a failure
  
SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#else
sbi_result_type sbi_write_bytes( sbi_slave_id_type slave_id,
                                 uint8 *addr,
                                 uint8 *data,
                                 uint8 length );
#endif /* FEATURE_MULTIMODE_SBI */
/*===========================================================================

FUNCTION sbi_write_byte

DESCRIPTION
  This function writes a single byte of data to the slaves. 
  Takes as input the slave id, register Address and the data to be sent. 
       
DEPENDENCIES
 
RETURN VALUE
  SBI_SUCCESS on a successful write
  SBI_FAILURE on a failure
  
SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#else
sbi_result_type sbi_write_byte( sbi_slave_id_type slave_id,
                                uint8 addr,
                                uint8 data );

#endif /* FEATURE_MULTIMODE_SBI */
/*===========================================================================

FUNCTION sbi_write_verify_byte

DESCRIPTION
  This function writes a single byte of data to a slave.  It then verifies
  that the write tooks place successfully by reading it back from the same
  register.
       
DEPENDENCIES
 
RETURN VALUE
  SBI_SUCCESS on a successful write with read verify 
  SBI_FAILURE on a failure
  SBI_PENDING if the transaction can't be done immediately
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_MULTIMODE_SBI
#error code not present
#else
sbi_result_type sbi_write_verify_byte( sbi_slave_id_type slave_id,
                                       uint8 addr,
                                       uint8 data );

#endif /* FEATURE_MULTIMODE_SBI */



/*===========================================================================

FUNCTION sbi_hw_request_enable                          EXTERNALIZED FUNCTION
         sbi_hw_req_enable - backwards compatible version

DESCRIPTION
  This function enables the hardware request on the SBI controller used to
  control the specified slave
    
DEPENDENCIES
  The associated priority of the Hardware Request channel should have been
  set prior to calling this function.
  
  Backwards compatible version assumes SBI0
 
RETURN VALUE
  None
  
SIDE EFFECTS
  
===========================================================================*/
void sbi_hw_request_enable( sbi_slave_id_type slave_id, sbi_hw_req_type chan );
void sbi_hw_req_enable( sbi_hw_req_type chan );


/*===========================================================================

FUNCTION sbi_hw_request_disable                         EXTERNALIZED FUNCTION
         sbi_hw_req_disable - backwards compatible version

DESCRIPTION
  This function disables the hardware request on the SBI controller used
  to control the specified slave.
    
DEPENDENCIES
  The specified hardware block should disable the associated SBI interface
  prior to calling this function.
  
  Backwards compatible version assumes SBI0

RETURN VALUE
  None
  
SIDE EFFECTS
  
===========================================================================*/
void sbi_hw_request_disable( sbi_slave_id_type slave_id, sbi_hw_req_type chan );
void sbi_hw_req_disable( sbi_hw_req_type chan );

/*===========================================================================

FUNCTION sbi_send_itm_packet                             EXTERNALIZED FUNCTION

DESCRIPTION
  Sends an ITM packet
  
DEPENDENCIES
  Only use when SBI driver is in ARM mode.

RETURN VALUE
  SBI_SUCCESS on a successful read
  SBI_FAILURE on a failure

SIDE EFFECTS
  None
===========================================================================*/
sbi_result_type sbi_send_itm_packet(sbi_slave_id_type slave_id, uint8 data);

/*=========================================================================

FUNCTION    SBI_BUS_CONFIG

DESCRIPTION
  This functions configures the bus to SBI/SSBI/GPIO and FTM mode.
  
DEPENDENCIES

RETURN VALUE
  SBI_SUCCESS on a successful 
  SBI_FAILURE on a failure

SIDE EFFECTS
  None.

===========================================================================*/
sbi_result_type sbi_bus_config(sbi_slave_id_type slave_id, sbi_bus_type type, int bus_num, boolean ftm);

/*=========================================================================

FUNCTION    SBI_MSM_CONFIG

DESCRIPTION
  This functions configures the MSM to pick the right SBI function pointers.
  
DEPENDENCIES

RETURN VALUE
  SBI_SUCCESS on a successful 
  SBI_FAILURE on a failure

SIDE EFFECTS
  None.

===========================================================================*/

void sbi_msm_config(sbi_slave_id_type slave_id, sbi_bus_type type, int bus_num, boolean ftm);
#ifdef __cplusplus
}
#endif

#ifdef FEATURE_SBI_PMIC_AUTO_DETECT
/*===========================================================================

FUNCTION sbi_config_pmic_io_bus

DESCRIPTION
  This function tries the PMIC SBI bus configurations (traditional 3-wire SBI, 
  SSBI, or FTM SSBI) in sequence to identify and set up the bus configuration
  at run time. Needs to be called from PMIC driver if QC PMIC is being used.
  
DEPENDENCIES
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
boolean sbi_config_pmic_io_bus(void);
#endif /* FEATURE_SBI_PMIC_AUTO_DETECT */

#endif /* SBI_LIB_API_H */

