#ifndef __SDCC_API_H
#define __SDCC_API_H
/**********************************************************************
 * sdcc_api.h
 *
 * This file provides SDCC external definitions.
 *
 * Copyright (c) 2004-2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_api.h#3 $
  $DateTime: 2008/11/12 10:46:30 $ $Author: vink $

when         who     what, where, why
--------     ---     -----------------------------------------------
2008-10-31   vin     Support for SDCC power collapse
2008-08-25   vin     Moved sdcc_isr to sdcc_util.h
01/18/08     sc      Moved SDCC_STATUS to sdcc_errno.h
10/15/07     umr     Integrate pre and post init from 4.0 branch
08/08/07     dng     Added FEATURE_SDCC_WLAN_CONFIG_API feature
10/04/06     dng     Added the customer.h header file
09/06/06     dng     Added header files to fix compiler errors.
07/17/06     dng     First pass of code review clean up.
08/28/05     hwu     General cleanup. Move testing related to sdcc_priv.h
07/24/05     hwu     Moved the common includes from private API to
                     the public API.
05/26/05     hwu     Merge in changes from L4Linux.
02/15/05     hwu     Added more SDIO related defines.
06/15/04     hwu     Initial version.
==================================================================*/
#include "sd_IF_ait_api.h"
#include "customer.h"
#include "comdef.h"

#include "msm.h"
#include "clk.h"        /* clk_busy_wait()  */
#include "hw.h"         /* MSM version info */

#ifdef FEATURE_PDA
#include "biostew.h"
#endif /* #ifdef FEATURE_PDA */

#ifdef FEATURE_L4LINUX
#error code not present
#endif

#ifdef FEATURE_SDCC_WLAN_CONFIG_API
#include "pm.h"
#endif

#include "sdcc_errno.h"

//advertise existence of sdcc_activity_timeout_event
#define HAVE_SDCC_POWER_COLLAPSE


/**************************************************************************
 *
 *                SDCC/SDIO API Structures
 *
 **************************************************************************/

/* Card Types  */
typedef enum
{
   SDCC_CARD_UNKNOWN = 0,
   SDCC_CARD_SD      = 1,
   SDCC_CARD_MMC     = 2,
   SDCC_CARD_COMB    = 3,
   SDCC_CARD_SDIO    = 4,
   SDCC_CARD_SDHC    = 5,
   SDCC_CARD_MMCHC   = 6,  /* MMC card with higher than a density of 2GB */
} SDCC_CARD_TYPE;

enum sdcc_card_info
{
    SDCC_CARD_STATE = 0,
    SDCC_CARD_SIZE,
    SDCC_GET_TYPE,
    SDCC_GET_ERROR,
    SDCC_BLOCK_LEN,
    SDCC_DRIVE_GEOMETRY,
    SDCC_SERIAL_NUM
};

typedef enum sdcc_sdio_dev_t
{
    SDCC_SDIO_UNKNOWN = 0,
    SDCC_SDIO_UART = 1,
    SDCC_SDIO_BT_A,
    SDCC_SDIO_BT_B,
    SDCC_SDIO_GPS,
    SDCC_SDIO_CAMERA,
    SDCC_SDIO_PHS,
    SDCC_SDIO_WLAN,
    SDCC_SDIO_WLAN_EXT,
    SDCC_SDIO_WLAN_INT
}sdcc_sdio_dev_type;

typedef struct drv_geometry_desc
{
    uint32  totalLBA;           /* Total drive logical blocks */
    uint16  dfltCyl;            /* Number of cylinders        */
    uint16  dfltHd;             /* Number of Heads            */
    uint16  dfltSct;            /* Sectors per track          */
    uint16  dfltBytesPerSect;   /* Bytes per sector           */
    uint8   serialNum[16];      /* Drive serial number        */
    uint8   modelNum[32];       /* Drive model number         */
    uint8   MID;                /* Manufacturer ID (binary) */
    uint8   OEMAppID[3];        /* OEM/Application ID 2 ASCII string */
    uint8   revisionNum;        /* Product revision number (binary) */
    uint8   MDateM;             /* Manufacturing date : month (binary) */
    uint8   MDateY;             /* Manufacturing date : year (binary) */   
}DRV_GEOMETRY_DESC;

/* SDIO definitions */
typedef enum 
{
    SD_IO_FUNCTION_ENABLE = 0,
    SD_IO_FUNCTION_DISABLE, 
    SD_IO_FUNCTION_SET_BLOCK_SIZE,
    SD_SET_DATA_TRANSFER_CLOCKS, 
    SD_SET_CARD_INTERFACE,
    SD_SET_OP_CODE,
    SD_SET_TYPE_COUNT,
    SD_SET_BLOCK_MODE,
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
    SDCC_SDIO_WLAN_CONFIG
#endif
} SDIO_SET_FEATURE_TYPE;

/* SDIO Information Type */
typedef enum 
{
    SD_GET_RCA,                  // RCA data type for MMC/SDMemory/SD IO Cards
    SD_GET_CARD_INTERFACE,       // card interface for all card types
    SD_GET_SDIO,                 // SDIO information (SD IO only)
    SD_GET_HOST_BLOCK_CAPABILITY,// SD Host Block Length Capabilities
    SD_GET_TYPE_COUNT            // number of SD_INFO_TYPEs 
}SDIO_GET_INFO_TYPE;

typedef enum {
    SDCC_INTERFACE_SDIO_1BIT = 0, 
    SDCC_INTERFACE_SDIO_4BIT = 1, 
}SDCC_SDIO_INTERFACE_MODE;

typedef enum {
    SDCC_DRV_2MA,
    SDCC_DRV_4MA,
    SDCC_DRV_6MA,
    SDCC_DRV_8MA,
    SDCC_DRV_10MA,
    SDCC_DRV_12MA,
    SDCC_DRV_14MA,
    SDCC_DRV_16MA
}sdcc_drive_strength_type;

typedef enum
{
  SDCC_CLK_400KHZ,
  SDCC_CLK_4P8MHZ,
  SDCC_CLK_9P6MHZ,
  SDCC_CLK_19P2MHZ
} sdcc_clk_type;

typedef struct _stMMC_SDIO_CARD_INTERFACE
{
    SDCC_SDIO_INTERFACE_MODE  InterfaceMode;  // interface mode
    unsigned long             ClockRate;      // clock rate
}SDIO_CARD_INTERFACE;

typedef struct _stMMC_SDIO_CARD_INFO 
{
    unsigned char FunctionNumber;   // SDIO function number
    unsigned char DeviceCode;       // device interface code for this function
    dword         CISPointer;       // CIS pointer for this function
    dword         CSAPointer;       // CSA pointer for this function
    unsigned char CardCapability;   // common card capabilities
}SDIO_CARD_INFO;

#ifdef FEATURE_SDCC_WLAN_CONFIG_API
typedef struct SDCC_WLAN_PARAMS
{
    uint32                    init_delay;     /* delay before sdcc_init, in milliseconds. 
                                              default 0*/
    sdcc_drive_strength_type  drive_strength; /* desired drive strength on sdc lines
                                              default: SDCC_DRV_2MA*/
    sdcc_clk_type             clk;            /* desired clock frequency
                                              default: SDCC_CLK_19P2MHZ*/
    sdcc_sdio_dev_type        device;         /* ffa or surf, kind of ffa, built-in or removable wlan.
                                              default: SDCC_SDIO_UNKNOWN*/
    pm_vreg_id_type           vreg;           /* voltage regulator to be used
                                              default: PM_VREG_MMC_ID*/
}sdcc_wlan_param_t;
#endif

/**************************************************************************
 *
 *                SDCC API functions
 *
 **************************************************************************/

boolean sdcc_init (void);
boolean sdcc_close(int16 driveno);
boolean sdcc_open (int16 driveno);

boolean sdcc_read (int16   driveno, 
                   uint32  s_sector,
                   byte   *buff, 
                   uint16  n_sectors);

boolean sdcc_write(int16   driveno, 
                   uint32  s_sector,
                   byte   *buff, 
                   uint16  n_sectors);
                   
boolean sdcc_erase (int16  driveno, 
                    uint32 s_sector, 
                    uint16 n_sectors);
                    
uint32  sdcc_ioctl(int16 driveno, 
                   uint8 what);

boolean sdcc_read_serial (uint16             driveno, 
                          DRV_GEOMETRY_DESC *iDrvPtr);

/* just so we can compile with SFAT */
void   system_controller_close (int16 driveno);

uint32 platform_get_ticks (void);

void   oem_getsysdate (uint16 *tdate, 
                       uint16 *ttime);

int16  get_extended_error (int16 driveno);

int16  get_interface_error (int16 driveno);

void   drive_format_information (int16   driveno, 
                                 uint16 *n_heads, 
                                 uint16 *sec_ptrack, 
                                 uint16 *n_cyls);

boolean sdcc_chk_for_sdio(void);

/**************************************************************************
 *
 *                SDCC_SDIO API functions
 *
 ****************************************************************************/

/******************************************************************************
* Name: sdio_pre_init
*
* Description:
*    This function checks to see if we can exit early if the drive is use.
*    If it  is not in use, initialize the SDCC controller.
*
* Arguments:
*    dev_manfid       [IN] : SDIO manufacturer identification
*
* Returns:
*    Returns card type in use (SDCC_CARD_UNKNOWN, in case it's not in use)
*
******************************************************************************/
SDCC_CARD_TYPE sdio_pre_init( uint16   dev_manfid );

/******************************************************************************
* Name: sdio_post_init
*
* Description:
*    This function looks for the specified device function implemented
*    on the device.  If the device is found as mentioned in "card type", it 
*    then will configure the device and enable it. 
*
* Arguments:
*    dev_manfid       [IN] : SDIO manufacturer identification
*    cardtype         [IN] : card type from SDCC_CARD_TYPE
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdio_post_init( uint16            dev_manfid,
                            SDCC_CARD_TYPE    card_type );

/******************************************************************************
* Name: sdcc_sdio_init
*
* Description:
*    This function looks for the specified device function implemented
*    on the device.  If it finds the device, it then will configure the
*    device and enable it. 
*
* Arguments:
*    dev_manfid       [IN] : SDIO manufacturer identification
*
* Returns:
*    Returns error code
*
******************************************************************************/

SDCC_STATUS  sdcc_sdio_init(uint16 dev_manfid);

/******************************************************************************
* Name: sdcc_find_card
*
* Description:
*    This function is to find out the type of the device.
*
* Arguments:
*    None
*
* Returns:
*    Returns card type
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_find_card( void );

SDCC_STATUS  sdcc_sdio_cmd52(uint32  devfn,
                             uint32  dir,
                             uint32  reg_addr,
                             uint8  *pdata);

SDCC_STATUS  sdcc_sdio_write(uint8   devfn,
                             uint32  reg_addr,
                             uint16  count,
                             uint8  *buff);

SDCC_STATUS  sdcc_sdio_read (uint8   devfn,
                             uint32  reg_addr,
                             uint16  count,
                             uint8  *buff);

SDCC_STATUS  sdcc_sdio_set(uint8                   devfn,
                           SDIO_SET_FEATURE_TYPE   type,
                           void                   *pdata);

SDCC_STATUS  sdcc_sdio_get(SDIO_GET_INFO_TYPE      type,
                           void                   *pdata,
                           uint32                  size);

void sdcc_sdio_connect_intr(uint8 devfn,
                            void *isr,
                            void *isr_param);

void sdcc_sdio_disconnect_intr(uint8 devfn);
void sdcc_sdio_abort(uint8 devfn);
void sdcc_sdio_reset_device(void);
void sdcc_enable_int(uint32 int_mask);
void sdcc_disable_int(uint32 int_mask);

/*=============================================================================
 * FUNCTION      sdcc_activity_timeout_event
 *
 * DESCRIPTION   This function tells SDCC that the SD slot has not been accessed 
 *               for a certain period of time. If we have the ability to 
 *               detect card removal / insertion, we shutdown the slot, otherwise
 *               do nothing.
 *
 * DEPENDENCIES  None
 *
 * PARAMETERS    driveno [IN]: The driveno parameter is not used, and is present 
 *               for consistency.
 *
 * RETURN VALUE  SDCC_NO_ERROR if successful, error code otherwise
 *
 * SIDE EFFECTS  None
 *
 *===========================================================================*/
SDCC_STATUS sdcc_activity_timeout_event( int16 driveno );


#endif /* __SDCC_API_H */

