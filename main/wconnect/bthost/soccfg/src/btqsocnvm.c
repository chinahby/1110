
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   B T S   N V M   M O D U L E

GENERAL DESCRIPTION
  This file implements Bluetooth Host Controller Common interface (NVM Data)

EXTERNALIZED FUNCTIONS

  bt_qsoc_nvm_vs_update_nvm_entries()
    Initializes fixed and runtime NVM tables based on SoC version.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2011 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/soccfg/rel/00.00.26/src/btqsocnvm.c#25 $
  $DateTime: 2011/01/18 00:25:12 $
  $Author: roystonr $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-10-25   tw Added support for 4025 B3
  2010-12-03   av  4025 B2 NVM update
  2010-08-17   ss  Add changes, featurization for XO Support.
  2010-07-26   rr  Enabling patch tag for previous 4025 B2 NVM update.
  2010-07-26   rr  4025 B2 NVM update.
  2010-07-05   rr  4025 B2 NVM update.
  2010-04-27   rr  4025 B2 NVM update.
  2010-03-10   rr  4020 BD B1 NVM update. Corrected tag 41 variable reference in macro and variable definition.
  2010-03-10   rr  4020 BD B1 NVM update. Corrected tag 41 to be in sync with released dynamic NVM.
  2010-03-10   rr  Updated 4025 B2. ROM patch for CR#189653 - pabBT Scatternet: AG(master)+FTP(slave)+DUNP (master/slave) concurrent connections fails.
  2010-03-10   rr  4020 BD B1 NVM update.
  2009-12-09  dgh  Updated NVM version for 4020 BD B1.
  2009-11-10  dgh  Updated NVM version for 4025 B2.
  2009-09-29  dgh  Updated the 32 MHz 2.1 NVM version tag.  No other change was 
                   necessary to bring the NVMs up to date.
  2009-09-15   jn  Fix SoC version table in comments section.
  2009-08-27   jn  Updated NVM version for 4025 B1.
  2009-08-07  dgh  Added support for 4025 B2.
  2009-07-07   jn  Updated NVM version for 4025 B1.
  2009-07-01   jn  Updated 4025 B1 patches.
  2009-06-02   sa  Support for Class 2 device.
  2009-05-19   sa  Updated patch for 4025 B1.
  2009-04-24   sa  Updated patch for 4025 B1.
  2009-04-07   rb  Updated for 4025 B1 19.2MHz and 4021 B1 
  2009-03-14   sa  Added Support for CLASS 2 device.
  2009-03-14   sa  Updated patch for 4025 B1 (32 MHz only).
  2009-02-13   rb  Added support for sw in-band
  2009-02-02   sa  Updated patch for 4025 B1 
  2009-01-12   sa  Updated patch for 4021 B1 (19.2 MHz).
  2009-01-05   sa  Fixed Compiler Warning on 76XX.
  2008-12-19   sa  Updated patch for 4025 B0.
  2008-11-25   sa  Updated patch for 4020 BD B0 (19.2 MHz only).
  2008-11-19   sa  Fixed sending TAG 114 & 115 twice.
  2008-11-17   sa  Update patch for 4025 B1.
  2008-11-17   sa  Resolved Crash for unsupported SOC.
  2008-11-14   sa  Support for NVM Automation.
  2008-11-06   sa  Updated NVM for 4025 B1
  2008-11-03   sa  Removed Compilation Warning by removing data structures 
                   that are not required for R3 BD.
  2008-10-30   sa  Resolved Dependency between TAG 6 and TAG 78 for BT 2.1 
                   support.
  2008-10-30   jn  Fix tag 37 and remove tag 76 for all 4025 revs. Fix typo in
                   4025 B1 RAM index.
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-10-13   sa  Removing support for 4020 R3 BD and 4020 R4
  2008-10-10   sa  Updated NVM tag 55d for 4025 B1 and upgraded to ver 07.02 
                   (19.2 MHz) and ver 0D.02 (32MHz) 
  2008-09-10   jn  Update NVM for 4020 BD B0.
  2008-08-30   rb  Update NVM for 4020 BD B0. Added preliminary support for 
                   4020 BD B1.
  2008-08-20   rb  Added support for 4025 B1
  2008-08-20   jn  Fix date in the previous checkin.
  2008-08-19   jn  Update patch for 4020 R3. Update NVM for 4020 BD B0.
  2008-08-12   rb  Added new patches and updates for 4025 B0
  2008-07-28   sa  Added support to configure soc logging enabled at run-time.
  2008-07-28   rb  Fix an error in patch 108 for 4025 A0
  2008-07-24   sp  Correct previous mistake for FFA SoC logging enable
  2008-07-16   sp  Disable SoC logging for QSC6240/QSC6270
  2008-07-06   rb  Patches for 4021 B1, 4020BD B0 (19.2MHz and 32MHz)
  2008-06-12   rb  Patches for BT connection loss in sniff mode and updates for 
                   4021 B1, 4020BD B0 and 4020 B0 (19.2MHz and 32MHz)
  2008-05-21   rb  Patches for esco issue on 4020 BD B0 
  2008-05-15   rb  Support for 4025 B0 Beta and Patches for esco issue. 
  2008-05-06   rb  Patches for 4025 A0
  2008-03-31   rb  Patches for 4020 BD B0 and 4021 B1
  2008-03-21   rb  Update 4020 BD B0 with correct f/w version nos.
  2008-03-13   rb  Add support for 4020 BD B0 & 4021 B1 and updated patches
                   for 4020 A0 and 4020 B0

*
*   #2          29 Feb 2008           JN
*   Fixed features mask when BT2.1 features is enabled but
*   SoC does not support BT2.1.
*
*   #1          31 Jan 2008           BH
*   Branched and renamed file from WM.
*
*   #5          31 Jan 2008           RH
*   Merged in bretth's latest update.
*
*   #4          24 Jan 2008           BH
*   Updated NVMs to the following values 
*       ----------- R3 ------------
*       - 19.2 Mhz Clock: 07.0F (01/21/2008)
*       - 32 Mhz Clock: 0D.10 (01/21/2008)
*       ----------- R4 ------------
*       LDO Front End Regulator
*       - 19.2 Mhz Clock: 07.08 (01/23/2008)
*       - 32 Mhz Clock: 0D.04 (01/23/2008)
*
*   #3          23 Jan 2008           RH
*   Merged in sunnyk's R3/R4 latest ROM patches.
*
*   #2          18 Jan 2008           RH
*   Merged in latest R4 NVM changes.
*
*   #1          21 Nov 2007           DR
*   Initial version.

===============================================================================*/

/******************************************************************************
                  -------------- R3 ----------------
                  - 19.2 Mhz Clock: 07.12  (08/13/08)
                  - 32 Mhz Clock: 0D.13    (08/13/08)
                  ----------- 4020BD A0 ------------
                  - 19.2 Mhz Clock: 07.0B  (01/23/08)
                  - 32 Mhz Clock: 0D.0A    (01/23/08)
                  ----------- 4020BD B0 ------------
                  - 19.2 Mhz Clock: 07.0D  (11/20/08)
                  - 32 Mhz Clock: 0D.08    (07/01/08)
                  ------------- 4021 B1 -------------
                  - 19.2 Mhz Clock: 07.09  (03/26/09)
                  - 32 Mhz Clock: 0D.08    (03/26/09)
                  ------------- 4021 C0  -------------
                  - 19.2 Mhz Clock: 07.08 
                  - 32 Mhz Clock: 0D.04 
                  ------------- 4025 A0  -------------
                  - 19.2 Mhz Clock: 07.04 
                  - 32 Mhz Clock: 0D.01 
                  ------------- 4025 B0  -------------
                  - 19.2 Mhz Clock: 07.08  (08/01/08)
                  - 32 Mhz Clock: 0D.00
                  ------------- 4025 B1  -------------
                  - 19.2 Mhz Clock: 07.0C (BT 2.1), 17.0B (CLASS 2)
		                            47.0D (BT 2.0)  (06/24/09)
                  - 32 Mhz Clock:   0D.0D (BT 2.1), 1D.0B (CLASS 2)
		                            4D.0E (BT 2.0)  (06/24/09)
                  ----------- 4020BD B1 ------------
                  - 19.2 Mhz Clock: 07.0A  (08/05/08)
                  - 32 Mhz Clock: 0D.08    (07/01/08)
                  ----------- 4020BD B1 ------------
                  - 19.2 Mhz Clock: 07.03 (BT 2.1)
                                    47.03 (BT 2.0)  (03/10/2010)
                  - 32 Mhz Clock:   0D.02 (BT 2.1)
                                    4D.02 (BT 2.0)  (03/10/2010)
                  ------------- 4025 B2  -------------
                  - 19.2 Mhz Clock: 07.02 (BT 2.1), 17.02 (CLASS 2)
                                    47.02 (BT 2.0)  (03/10/2010)
                  - 32 Mhz Clock:   0D.03 (BT 2.1), 1D.02 (CLASS 2)
                                    4D.02 (BT 2.0)  (03/10/2010)
                  ------------- 4025 B2  -------------
                  - 19.2 Mhz Clock: 07.03 (BT 2.1), 17.03 (CLASS 2)
                                    47.03 (BT 2.0)  (04/27/2010)
                  - 32 Mhz Clock:   0D.04 (BT 2.1), 1D.03 (CLASS 2)
                                    4D.03 (BT 2.0)  (04/27/2010)									
                  ------------- 4025 B2  -------------
                  - 19.2 Mhz Clock: 07.05 (BT 2.1), 17.05 (CLASS 2)
                                    47.05 (BT 2.0)  (07/05/2010)
                  - 32 Mhz Clock:   0D.06 (BT 2.1), 1D.05 (CLASS 2)
                                    4D.05 (BT 2.0)  (07/05/2010)
                  ------------- 4025 B2  -------------
                  - 19.2 Mhz Clock: 07.06 (BT 2.1), 17.06 (CLASS 2)
                                    47.06 (BT 2.0)  (07/26/2010)
                  - 32 Mhz Clock:   0D.07 (BT 2.1), 1D.06 (CLASS 2)
                                    4D.06 (BT 2.0)  (07/26/2010)                                    
********************************************************************************/
#include "btqsocnvmplatform.h"
#include "btqsocnvm.h"
#include "btqsocnvmBTS.h"
#include "btqsocnvmclass2.h"

#include <ctype.h>
#include <stdlib.h>
/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/


/*===========================================================================
                                 Globals
===========================================================================*/
/* The Clock Stabilization time for XO crystals is longer than TCXO. So BT 
   modules using XO needs a larger SoC Power on Count to hold off SoC Powerup 
   for this delay. SoC NVM Tag 37 configures the SoC Sleep/Wakeup parameters.
*/
#ifdef FEATURE_BT_SYSTEM_CLOCK_XO_SUPPORT
  #define TAG_37_SLEEP_PARAMS_MIN_BCST_VAL   (0x80)
  #define TAG_37_SLEEP_PARAMS_PWR_ON_CNT_VAL (0x7E)  
#else
  #define TAG_37_SLEEP_PARAMS_MIN_BCST_VAL   (0x34)
  #define TAG_37_SLEEP_PARAMS_PWR_ON_CNT_VAL (0x32)  
#endif /* FEATURE_BT_SYSTEM_CLOCK_XO_SUPPORT */

#define BUF_SIZE 500           /* Number of bytes to read in each call to readfile */
#define BT_QSOC_TAG_WRITE 0x01 /* Indicates the tag is to be written */
#define SOC_TAG_LENGTH_MAX 300 /* Max tag length + 4 bytes for header */

#define REL_NVM_MAX_TAG_NO 256

/* static OI_UINT8 buf[BUF_SIZE];*/ /* Buffer which would store the bytes returned by readfile */
static const OI_UINT8 *buf;
OI_UINT16 buf_len;
static OI_UINT8 dest[16];          /* Temp buffer */
static OI_UINT16 bt_soc_r_ind = 0; /* read index for 'buf' */

OI_UINT8 soc_tag[SOC_TAG_LENGTH_MAX];  /* This array will contain the tag */

/* Global struct for parser */
typedef struct
{
  OI_UINT16 current_tag_num;     /* tracks current tag number for parser */
  OI_UINT16 total_num_nvm_tags;  /* tracks number of tags in nvm file */
} bt_qsoc_nvm_parser_type;
static bt_qsoc_nvm_parser_type bt_qsoc_parser;

/* Keeps track of error */
static btqsocnvm_parser_err_type btqsocnvm_parser_error;

/* variable to keep track of nvm parser state */
static boolean nv_parser_state = FALSE;

/* array which indicates dynamic tags */
static int rel_nvm[REL_NVM_MAX_TAG_NO];

/* Forward declaration and function macro */
static boolean bt_qsoc_nvm_getnext_n_char(unsigned char *dest_buf, OI_UINT16 len);
#define bt_qsoc_nvm_getnextchar(n) bt_qsoc_nvm_getnext_n_char(n,1)

static OI_UINT16 bt_qsoc_nvm_strlen(const OI_UINT8 *buf_ptr);
/*===========================================================================
                        File Maintenance Info
===========================================================================*/
/*****************************************************************************/
/*********************         ADDING NEW TAGS              ******************/
/*****************************************************************************/
/*
Steps to adding a new NVM tag.  Tags should only be added once they come from 
QShare or are validated by the BT integration team.

The tags can be divided into two kinds:
1. Dynamic
2. Static

Dynamic Parse: This is applicable only to those SoC which support dynamic
parsing of nvm file. If there is an entry for the SoC in released_nvm_tags_<clock> 
it means that dynamic parsing is supported for the SoC.

Furthermore, the tag can only be classified as dynamic if its value can
be fetched from nvm file and can be sent to SoC without any modification.
If the value of the tag depends on some compile flags or run-time NVM value,
it cannot be dynamic.

Static: All tags (other than dynamic) would fall under this category.

*******************************************************************
*                           DYNAMIC TAGS                          *
*******************************************************************
*******************************************************************
*                           Step 1.                               *
*******************************************************************

Add the tag number into corresponding case under 
bt_qsoc_populate_rel_nvm

For eg, in order to add a new tag for 4021_B1, 32 MHz; add the new 
if(qsoc_type == BT_QSOC_4021_B1)
{ rel_nvm[<new_tag>] = 1; }

Refer to bt_qsoc_enum_type for all the supported soc

*******************************************************************
*                           STATIC TAGS                           *
*******************************************************************
*******************************************************************
*                           Step 1.                               *
*******************************************************************

Add the specific NVM tag to an array like the one here:
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_XX[] = \
{
  Entire size (tag size + 3), upload request(always 0x01), tag number (XX), tag size, 
  NVM tag payload (0x01, 0x02, 0x03, etc...)
};

For each unique tag needed, there will be at least one array like the one above. 

*******************************************************************
*                           Step 2.                               *
*******************************************************************

Once this array has been defined, we must also define a macro which will be used
to place a pointer to this array in our SoC specific table much further below. 

#define CFG_TAG_XX_SOC_VERSION       (&bt_qsoc_cfg_tag_XX[0]) 

The format of this define can change if there are multiple versions of a tag
for one SoC version, or if the tag is SoC version independent.

*******************************************************************
*                           Step 3.                               *
*******************************************************************
Finally the macro created above should be entered into one of the NVM tables 
for a specific SoC version.  Currently we have two tables for each SoC version.

****************************
*       Fixed Table        *
****************************

The first table is the fixed table, which contains NVMs which do not change at 
runtime.  Simply add your macro to this table. If this tag applies to multiple
SoC versions, add the tag to all tables that apply.

#define SIZEOF_BT_QSOC_CFG_TBL_SOC_VERSION \
        ((sizeof(bt_qsoc_cfg_tbl_soc_version))/(sizeof(bt_qsoc_cfg_cst_element_type*)))

static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_soc_version[] = \
{
  CFG_TAG_XX_SOC_VERSION,
  CFG_TAG_YY_SOC_VERSION,
  CFG_TAG_ZZ_SOC_VERSION,
}

****************************
*       Runtime Table      *
****************************

The next table contains NVMs that are picked out at runtime based on the function
bt_qsoc_nvm_vs_update_nvm_entries().  The table is sized to the most runtime NVMs
we could ever need at one time and initially filled with NULLs.  These NULLs are 
basically placeholders and are used by the client of this file to know that no
NVM resides in that spot.

#define SIZEOF_BT_QSOC_CFG_TBL_SOC_VERSION_RAM \
      ((sizeof(bt_qsoc_cfg_tbl_soc_version_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))

static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_soc_version_ram[] = \
{
  NULL, 
  NULL, 
  NULL
};
#define CFG_TAG_XX_SOC_VERSION_RAM_INDEX  (0)
#define CFG_TAG_YY_SOC_VERSION_RAM_INDEX  (1)
#define CFG_TAG_ZZ_SOC_VERSION_RAM_INDEX  (2)

Simply modify bt_qsoc_nvm_vs_update_nvm_entries() to overwrite the default NULL
with the macro to your new tag.  Ensure your overwriting is correct so you don't 
overwrite the wrong tag unintentionally.

bt_qsoc_cfg_tbl_soc_version_ram[CFG_TAG_XX_SOC_VERSION_RAM_INDEX] = CFG_TAG_XX_SOC_VERSION;

*/

/*****************************************************************************/
/*********************     Common  NVM  Entries   ****************************/
/*****************************************************************************/

/* Same Entry For R3, Unknown */
/* #4 Min. encryption key size - set to 0x01 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_4[] = \
{
  0x04, 0x01, 0x04, 0x01, 
  0x01
};
#define CFG_TAG_4_R3        (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4020BD_B0 (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4020BD_B1 (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4021_B1   (&bt_qsoc_cfg_tag_4[0]) 
#define CFG_TAG_4_4025_A0   (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4025_B0   (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4025_B1   (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4025_B2   (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_4025_B3   (&bt_qsoc_cfg_tag_4[0])
#define CFG_TAG_4_UNKNOWN   (&bt_qsoc_cfg_tag_4[0])

/* Same Entry For R3, Unknown */
/* #5 Max. encryption key size - set to 0x10 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_5[] = \
{
  0x04, 0x01, 0x05, 0x01, 
  0x10
};
#define CFG_TAG_5_R3        (&bt_qsoc_cfg_tag_5[0]) 
#define CFG_TAG_5_4020BD_B0 (&bt_qsoc_cfg_tag_5[0]) 
#define CFG_TAG_5_4020BD_B1 (&bt_qsoc_cfg_tag_5[0]) 
#define CFG_TAG_5_4021_B1   (&bt_qsoc_cfg_tag_5[0]) 
#define CFG_TAG_5_4025_A0   (&bt_qsoc_cfg_tag_5[0]) 
#define CFG_TAG_5_4025_B0   (&bt_qsoc_cfg_tag_5[0])
#define CFG_TAG_5_4025_B1   (&bt_qsoc_cfg_tag_5[0])
#define CFG_TAG_5_4025_B2   (&bt_qsoc_cfg_tag_5[0])
#define CFG_TAG_5_4025_B3   (&bt_qsoc_cfg_tag_5[0])
#define CFG_TAG_5_UNKNOWN   (&bt_qsoc_cfg_tag_5[0]) 

/* Same Entry For R3, Unknown */
/* #6 BT supported features mask - TBD */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_6_bt_2_1_supported[] = \
{
  0x0B, 0x01, 0x06, 0x08, 
  0xFF, 0xFE, 0x8F, 0xFE, 0x9B, 0xFF, 0x59, 0x83
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_6_bt_2_1_not_supported[] = \
{
  0x0B, 0x01, 0x06, 0x08, 
  0xFF, 0xFE, 0x8F, 0xFE, 0x9B, 0xF9, 0x00, 0x80
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_6_4020bd_b0_bt_2_1_supported[] = \
{
  0x0B, 0x01, 0x06, 0x08, 
  0xFF, 0xFE, 0x8F, 0x7E, 0x98, 0x1F, 0x59, 0x83 
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_6_4020bd_b0_bt_2_1_not_supported[] = \
{
  0x0B, 0x01, 0x06, 0x08, 
  0xFF, 0xFE, 0x8F, 0x7E, 0x98, 0x19, 0x00, 0x80
};

/* R3 does not support BT2.1 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_6_r3[] = \
{
  0x0B, 0x01, 0x06, 0x08, 
  0xFF, 0xFE, 0x8F, 0xFE, 0x9B, 0xF9, 0x00, 0x80
};


#define CFG_TAG_6_R3        (&bt_qsoc_cfg_tag_6_r3[0]) 
#define CFG_TAG_6_4025_A0   (&bt_qsoc_cfg_tag_6_bt_2_1_supported[0]) 
#define CFG_TAG_6_UNKNOWN   (&bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0]) 

/* Same Entry For R3, Unknown */
/* #17 HCI Transport : User Defined Baud 
* The values below specifies transport - H4 with used defined
* baud rate (0xFF third byte from the last). Actual baud rate is
* specified through tag #47
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_17[] = \
{
/* set running baud rate to value defined by BT_QSOC_BAUD_RATE */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  0x08, 0x01, 0x11, 0x05, 
  0x4A, 0x01, BT_QSOC_BAUD_RATE, 0x00, 0x00 
#else
  0x08, 0x01, 0x11, 0x05, 
  0x0A, 0x01, BT_QSOC_BAUD_RATE, 0x00, 0x00
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_17_4025[] = \
{
/* set running baud rate to value defined by BT_QSOC_BAUD_RATE */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  0x0A, 0x01, 0x11, 0x07, 
  0x4A, 0x01, BT_QSOC_BAUD_RATE, 0x00, 0x00, 0x00, 0x00 
#else
  0x0A, 0x01, 0x11, 0x07, 
  0x0A, 0x01, BT_QSOC_BAUD_RATE, 0x00, 0x00, 0x00, 0x00
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */  	
};

#define CFG_TAG_17_R3        (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4020BD_B0 (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4020BD_B1 (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4021_B1   (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4025_A0   (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4025_B0   (&bt_qsoc_cfg_tag_17[0]) 
#define CFG_TAG_17_4025_B1   (&bt_qsoc_cfg_tag_17_4025[0]) 
#define CFG_TAG_17_4025_B2   (&bt_qsoc_cfg_tag_17_4025[0])
#define CFG_TAG_17_4025_B3   (&bt_qsoc_cfg_tag_17_4025[0])
#define CFG_TAG_17_UNKNOWN   (&bt_qsoc_cfg_tag_17[0]) 

/* Same Entry For R3, Unknown */
/* #27 Sleep Enable Mask - enable sleep */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_27[] = \
{
#ifdef FEATURE_BT_QSOC_SLEEP
  0x04, 0x01, 0x1B, 0x01, 
  0x01
#else
  0x04, 0x01, 0x1B, 0x01, 
  0x00
#endif /* FEATURE_BT_QSOC_SLEEP */
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_27_unknown[] = \
{
  0x04, 0x01, 0x1B, 0x01, 
  0x00
};

#define CFG_TAG_27_R3        (&bt_qsoc_cfg_tag_27[0]) 
#define CFG_TAG_27_4020BD_B0 (&bt_qsoc_cfg_tag_27[0]) 
#define CFG_TAG_27_4020BD_B1 (&bt_qsoc_cfg_tag_27[0]) 
#define CFG_TAG_27_4021_B1   (&bt_qsoc_cfg_tag_27[0]) 
#define CFG_TAG_27_4025_A0   (&bt_qsoc_cfg_tag_27[0])
#define CFG_TAG_27_4025_B0   (&bt_qsoc_cfg_tag_27[0])
#define CFG_TAG_27_4025_B1   (&bt_qsoc_cfg_tag_27[0])
#define CFG_TAG_27_4025_B2   (&bt_qsoc_cfg_tag_27[0])
#define CFG_TAG_27_4025_B3   (&bt_qsoc_cfg_tag_27[0])
#define CFG_TAG_27_UNKNOWN   (&bt_qsoc_cfg_tag_27_unknown[0]) 

/* #28 Low Power Clock Control parameters - LPO type - Digital 
* Minimun Clk Cal. period - 0x00001000 (4096 * 312.5 = 1.28sec)
* Crystal stabilization time - not applicable
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_28[] = \
{
  0x0A, 0x01, 0x1C, 0x07, 
  0x00, 0x10, 0x00, 0x00, 0x2C, 0x01, 0x01
};
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_28_4020bd_b0[] = \
{
  0x12, 0x01, 0x1C, 0x0F, 
  0x00, 0x10, 0x00, 0x00, 0x2C, 0x01, 0x01, 0x00, 
  0x00, 0xF0, 0x00, 0x00, 0xFF, 0xFF, 0x00
};

#define CFG_TAG_28_4025_A0   (&bt_qsoc_cfg_tag_28_4020bd_b0[0])
#define CFG_TAG_28_UNKNOWN   (&bt_qsoc_cfg_tag_28[0]) 

/* #32 USB product ID tag - used for versioning NVM releases 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_a0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x04, 0x07    // version 07.04
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_a0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x01, 0x0D    // version 0D.01
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b1_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0D, 0x47    // version 47.0D
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b1_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0C, 0x07    // version 07.0C
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b1_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0E, 0x4D    // version 4D.0E
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b1_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0D, 0x0D    // version 0D.0D
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b2_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x07, 0x47    // version 47.07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b2_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x07, 0x07    // version 07.07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b2_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x07, 0x4D    // version 4D.07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b2_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x08, 0x0D    // version 0D.08
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b0_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0D, 0x07    // version 07.0D
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b0_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x0C, 0x67    // version 67.0C
};

#define CFG_TAG_19P2_4020BD_B0_BT2_1 (&bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b0_bt_2_1[0])
#define CFG_TAG_19P2_4020BD_B0_BT2_0 (&bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b0_bt_2_0[0])

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b1_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x03, 0x47    // version 47.03
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b1_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x03, 0x07    // version 07.03
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4020bd_b1_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x02, 0x4D    // version 4D.02
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4020bd_b1_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x02, 0x0D    // version 0D.02
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b3_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x00    // version 00.00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_19p2MHz_4025_b3_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x10    // version 10.00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b3_bt_2_0[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x01    // version 01.00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_32_32MHz_4025_b3_bt_2_1[] = \
{
  0x05, 0x01, 0x20, 0x02, 
  0x00, 0x11    // version 11.00
};

/* #36 Remove lowest power mode (01-07) 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_36_4025_a0[] = \
{
  0x26, 0x01, 0x24, 0x23, 
  0x09, 0x09, 0x07, 0x04, 0x09, 0x00, 0x01, 0x02, 
  0x03, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
  0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
  0x07, 0x08, 0x09
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_36_4025_CLASS1[] = \
{
  0x26, 0x01, 0x24, 0x23,
  0x09, 0x09, 0x07, 0x04, 0x09, 0x00, 0x01, 0x02, 
  0x03, 0x04, 0x05, 0x06, 0x07, 0x07, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 
  0x02, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
  0x07, 0x08, 0x09
};

#define CFG_TAG_36_4025_A0   (&bt_qsoc_cfg_tag_36_4025_a0[0])  

/* #37 Sleep parameters - refer to documentation for values used */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_37_clock_sharing[] = \
{
  0x19, 0x01, 0x25, 0x16, 
  0x00, 0x00, 0x02, 0x01, 0x01, 0x12, 0x01, 0x00,
  0x00, 0x12, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  TAG_37_SLEEP_PARAMS_MIN_BCST_VAL, TAG_37_SLEEP_PARAMS_PWR_ON_CNT_VAL,
  0x02, 0x00, 0x00, 0x00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_37_clock_sharing_4025[] =
{
  0x19, 0x01, 0x25, 0x16, 
  0x00, 0x00, 0x02, 0x01, 0x01, 0x12, 0x1D, 0x00, 
  0x00, 0x12, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 
  TAG_37_SLEEP_PARAMS_MIN_BCST_VAL, TAG_37_SLEEP_PARAMS_PWR_ON_CNT_VAL,
  0x02, 0x00, 0x00, 0x00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_37_clock_sharing_4025_b3[] =
{
  0x19, 0x01, 0x25, 0x16, 
  0x00, 0x00, 0x02, 0x01, 0x01, 0x12, 0x1D, 0x00, 
  0x00, 0x12, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 
  TAG_37_SLEEP_PARAMS_MIN_BCST_VAL, TAG_37_SLEEP_PARAMS_PWR_ON_CNT_VAL,
  0x30, 0x00, 0x00, 0x00
};

#define CFG_TAG_37_CLK_SHARING_R3        (&bt_qsoc_cfg_tag_37_clock_sharing[0]) 
#define CFG_TAG_37_CLK_SHARING_4020BD_B0 (&bt_qsoc_cfg_tag_37_clock_sharing[0]) 
#define CFG_TAG_37_CLK_SHARING_4020BD_B1 (&bt_qsoc_cfg_tag_37_clock_sharing[0]) 
#define CFG_TAG_37_CLK_SHARING_4021_B1   (&bt_qsoc_cfg_tag_37_clock_sharing[0]) 
#define CFG_TAG_37_CLK_SHARING_4025_A0 \
                                    (&bt_qsoc_cfg_tag_37_clock_sharing_4025[0]) 
#define CFG_TAG_37_CLK_SHARING_4025_B0 \
                                    (&bt_qsoc_cfg_tag_37_clock_sharing_4025[0]) 
#define CFG_TAG_37_CLK_SHARING_4025_B1 \
                                    (&bt_qsoc_cfg_tag_37_clock_sharing_4025[0]) 
#define CFG_TAG_37_CLK_SHARING_4025_B2 \
                                    (&bt_qsoc_cfg_tag_37_clock_sharing_4025[0])
#define CFG_TAG_37_CLK_SHARING_4025_B3    \
                                (&bt_qsoc_cfg_tag_37_clock_sharing_4025_b3[0])
#define CFG_TAG_37_CLK_SHARING_UNKNOWN   (&bt_qsoc_cfg_tag_37_clock_sharing[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_37_no_clock_sharing[] = \
{
  0x19, 0x01, 0x25, 0x16, 
  0x00, 0x00, 0x02, 0x01, 0x01, 0x12, 0x01, 0x00,
  0x00, 0x12, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x34, 0x32, 0x30, 0x00, 0x00, 0x00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_37_no_clock_sharing_4025[] = \
{
  0x19, 0x01, 0x25, 0x16, 
  0x00, 0x00, 0x02, 0x01, 0x01, 0x12, 0x1D, 0x00,
  0x00, 0x12, 0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x34, 0x32, 0x30, 0x00, 0x00, 0x00
};

#define CFG_TAG_37_NO_CLK_SHARING_R3        (&bt_qsoc_cfg_tag_37_no_clock_sharing[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4020BD_B0 (&bt_qsoc_cfg_tag_37_no_clock_sharing[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4020BD_B1 (&bt_qsoc_cfg_tag_37_no_clock_sharing[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4021_B1   (&bt_qsoc_cfg_tag_37_no_clock_sharing[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4025_A0 \
                                       (&bt_qsoc_cfg_tag_37_no_clock_sharing_4025[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4025_B0 \
                                       (&bt_qsoc_cfg_tag_37_no_clock_sharing_4025[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4025_B1 \
                                       (&bt_qsoc_cfg_tag_37_no_clock_sharing_4025[0]) 
#define CFG_TAG_37_NO_CLK_SHARING_4025_B2 \
                                       (&bt_qsoc_cfg_tag_37_no_clock_sharing_4025[0])
#define CFG_TAG_37_NO_CLK_SHARING_4025_B3 \
                                       (&bt_qsoc_cfg_tag_37_no_clock_sharing_4025[0])
#define CFG_TAG_37_NO_CLK_SHARING_UNKNOWN   (&bt_qsoc_cfg_tag_37_no_clock_sharing[0]) 

/* #38 Debug control - Disable logger output 
* logger buffer - 300 bytes used only when logger is enabled 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_38_soc_logging[] = \
{
/* Turn on only for BT2.1 and Surf.
    Targets that have common FFA and SURF builds shouldn't turn it on */
  0x07, 0x01, 0x26, 0x04, 
  0x02, 0x00, 0x2C, 0x01
};
#define CFG_TAG_38_SOC_LOGGING_R3        (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4020BD_B0 (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4020BD_B1 (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4021_B1   (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4025_A0   (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4025_B0   (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4025_B1   (&bt_qsoc_cfg_tag_38_soc_logging[0]) 
#define CFG_TAG_38_SOC_LOGGING_4025_B2   (&bt_qsoc_cfg_tag_38_soc_logging[0])
#define CFG_TAG_38_SOC_LOGGING_4025_B3   (&bt_qsoc_cfg_tag_38_soc_logging[0])
#define CFG_TAG_38_SOC_LOGGING_UNKNOWN   (&bt_qsoc_cfg_tag_38_soc_logging[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_38_soc_no_logging[] = \
{
  0x07, 0x01, 0x26, 0x04, 
  0x00, 0x00, 0x2C, 0x01
};
#define CFG_TAG_38_SOC_NO_LOGGING_R3        (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4020BD_B0 (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4020BD_B1 (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4021_B1   (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4025_A0   (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4025_B0   (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4025_B1   (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 
#define CFG_TAG_38_SOC_NO_LOGGING_4025_B2   (&bt_qsoc_cfg_tag_38_soc_no_logging[0])
#define CFG_TAG_38_SOC_NO_LOGGING_4025_B3   (&bt_qsoc_cfg_tag_38_soc_no_logging[0])
#define CFG_TAG_38_SOC_NO_LOGGING_UNKNOWN   (&bt_qsoc_cfg_tag_38_soc_no_logging[0]) 

/* #39 RM and LC Override Enable - Optimize RM register settings 
 * Enables RM register override 
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_39_4025_a0[] = \
{
  0x07, 0x01, 0x27, 0x04, 
  0x13, 0x01, 0x00, 0x00
};

#define CFG_TAG_39_4025_A0           (&bt_qsoc_cfg_tag_39_4025_a0[0])

/* #40 LC Burst Timer Override - Overrides the burst timers for each
 * burst.
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_40[] = \
{
  0x15, 0x01, 0x28, 0x12, 
  0x32, 0x05, 0x67, 0x0D, 0x42, 0x12, 0x43, 0x13,
  0x1C, 0x0C, 0x17, 0x10, 0x59, 0x13, 0x8D, 0x04, 
  0x3D, 0x08
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_40_4025_b0[] = \
{
  0x15, 0x01, 0x28, 0x12, 
  0x32, 0x05, 0x07, 0x0D, 0xE2, 0x11, 0x43, 0x13,
  0x1C, 0x0C, 0x17, 0x10, 0x59, 0x13, 0x8D, 0x04,
  0x3D, 0x08 
};
#define CFG_TAG_40_4025_A0   (&bt_qsoc_cfg_tag_40[0])
#define CFG_TAG_40_4025_B0   (&bt_qsoc_cfg_tag_40_4025_b0[0])

 
/* #41 Radio Register Override - Configures values for RM registers in
* Addr/Value pairs. 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_19p2MHz_4025_a0[] = \
{
  0x29, 0x01, 0x29, 0x26, 
  0x11, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x20, 0x62,
  0x41, 0x1A, 0x5A, 0x07, 0xA4, 0x65, 0xA5, 0x32,
  0xB2, 0x02, 0xB3, 0x02, 0xB4, 0x02, 0xB5, 0x01, 
  0xB6, 0x01, 0xB7, 0x01, 0xB8, 0x00, 0xB9, 0x00, 
  0x76, 0x03, 0xAD, 0x04, 0xC6, 0x3D 
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_32MHz_4025_a0[] = \
{
  0x29, 0x01, 0x29, 0x26, 
  0x11, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x20, 0x62,
  0x41, 0x12, 0x5A, 0x07, 0xA4, 0xC8, 0xA5, 0x42,
  0xB2, 0x02, 0xB3, 0x02, 0xB4, 0x02, 0xB5, 0x01, 
  0xB6, 0x01, 0xB7, 0x01, 0xB8, 0x00, 0xB9, 0x00, 
  0x76, 0x03, 0xAD, 0x04, 0xC6, 0x3D 
};

#define CFG_TAG_41_19P2MHZ_4025_A0   (&bt_qsoc_cfg_tag_41_19p2MHz_4025_a0[0]) 

#define CFG_TAG_41_32MHZ_4025_A0     (&bt_qsoc_cfg_tag_41_32MHz_4025_a0[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_19p2MHz_4020_bd_b1[] = \
{
  0x3D, 0x01, 0x29, 0x3A, 
  0x1B, 0x00, 0x00, 0x00, 0x06, 0x1A, 0x3D, 0xFB,
  0x3E, 0x0F, 0x41, 0x1D, 0x42, 0x00, 0x43, 0xCC, 
  0x49, 0xF3, 0x4A, 0xF5, 0x5D, 0x06, 0x5E, 0x06,
  0x5F, 0x03, 0x64, 0x01, 0x72, 0x20, 0x75, 0x01,
  0x92, 0x00, 0x93, 0x00, 0x9C, 0x03, 0x1C, 0xF6,
  0x1D, 0x06, 0x20, 0x62, 0x1E, 0x0F, 0xA6, 0xAA,
  0xA7, 0x55, 0xA8, 0x15, 0xA4, 0x65, 0xA5, 0x32,
  0x30, 0x07 
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_32MHz_4020_bd_b1[] = \
{
  0x3B, 0x01, 0x29, 0x38, 
  0x1A, 0x00, 0x00, 0x00, 0x06, 0x1A, 0x3D, 0xFB,
  0x3E, 0x0F, 0x41, 0x15, 0x42, 0x00, 0x43, 0xCC,
  0x49, 0xF4, 0x4A, 0xF5, 0x5D, 0x06, 0x5E, 0x06,
  0x5F, 0x03, 0x64, 0x01, 0x72, 0x20, 0x75, 0x01,
  0x92, 0x02, 0x9C, 0x00, 0x1C, 0xF6, 0x1D, 0x06,
  0x20, 0x62, 0x1E, 0x0F, 0xA6, 0xAA, 0xA7, 0x55,
  0xA8, 0x15, 0xA4, 0xC8, 0xA5, 0x42, 0x30, 0x07
};

#define CFG_TAG_41_19P2MHZ_4020_BD_B1   (&bt_qsoc_cfg_tag_41_19p2MHz_4020_bd_b1[0]) 
#define CFG_TAG_41_32MHZ_4020_BD_B1     (&bt_qsoc_cfg_tag_41_32MHz_4020_bd_b1[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS1[] = \
{
  0x29, 0x01, 0x29, 0x26,
  0x11, 0x00, 0x00, 0x00, 0x42, 0x33, 0xA5, 0x31, 
  0xB1, 0x07, 0xC1, 0x70, 0x1C, 0xFA, 0x1D, 0x04, 
  0x1E, 0x1F, 0x20, 0x6C, 0x3D, 0xEF, 0x3E, 0xC9, 
  0xA7, 0xF5, 0xAA, 0x42, 0xAB, 0x94, 0xAC, 0x8F, 
  0xA6, 0xBF, 0xA8, 0x36, 0xCF, 0x58
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_41_32MHz_4025_CLASS1[] = \
{
  0x2B, 0x01, 0x29, 0x28,
  0x12, 0x00, 0x00, 0x00, 0x42, 0x33, 0xA5, 0x31, 
  0xB1, 0x07, 0xC1, 0x70, 0x1C, 0xFA, 0x1D, 0x04, 
  0x1E, 0x1F, 0x20, 0x6C, 0x3D, 0xEF, 0x3E, 0xC9, 
  0xA7, 0xF5, 0xAA, 0x42, 0xAB, 0x94, 0xAC, 0x8F, 
  0xA6, 0xBF, 0xA8, 0x36, 0xCF, 0x58, 0x5B, 0x06
};

/* #44 Codec configuration - Codec interface configuration */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_44[] = \
{
  0x0D, 0x01, 0x2C, 0x0A, 
  0x10, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
  0x0F, 0x0F
};
#define CFG_TAG_44_R3        (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4020BD_B0 (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4020BD_B1 (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4021_B1   (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4025_A0   (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4025_B0   (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4025_B1   (&bt_qsoc_cfg_tag_44[0]) 
#define CFG_TAG_44_4025_B2   (&bt_qsoc_cfg_tag_44[0])
#define CFG_TAG_44_4025_B3   (&bt_qsoc_cfg_tag_44[0])
#define CFG_TAG_44_UNKNOWN   (&bt_qsoc_cfg_tag_44[0]) 


/* #45 CVSD Gain Volume Setting -  */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_45[] = \
{
  0x09, 0x01, 0x2D, 0x06, 
  0x02, 0x00, 0x00, 0x00, 0x0F, 0x0F
};
#define CFG_TAG_45_R3        (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4020BD_B0 (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4020BD_B1 (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4021_B1   (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4025_A0   (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4025_B0   (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4025_B1   (&bt_qsoc_cfg_tag_45[0]) 
#define CFG_TAG_45_4025_B2   (&bt_qsoc_cfg_tag_45[0])
#define CFG_TAG_45_4025_B3   (&bt_qsoc_cfg_tag_45[0])
#define CFG_TAG_45_UNKNOWN   (&bt_qsoc_cfg_tag_45[0]) 

/* #46 Voice Settings -  */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_46[] = \
{
  0x05, 0x01, 0x2E, 0x02, 
  0x40, 0x00
};
#define CFG_TAG_46_R3        (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4020BD_B0 (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4020BD_B1 (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4021_B1   (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4025_A0   (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4025_B0   (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4025_B1   (&bt_qsoc_cfg_tag_46[0]) 
#define CFG_TAG_46_4025_B2   (&bt_qsoc_cfg_tag_46[0])
#define CFG_TAG_46_4025_B3   (&bt_qsoc_cfg_tag_46[0])
#define CFG_TAG_46_UNKNOWN   (&bt_qsoc_cfg_tag_46[0]) 


/* Tag 53 for WLAN Coex */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_53[] = \
{
#ifdef FEATURE_BT_WLAN_COEXISTENCE
  0x0B, 0x01, 0x35, 0x08, 
  0x02, 0x08, 0x70, 0x80, 0x04, 0x00, 0x08, 0x00
#else
  0x0B, 0x01, 0x35, 0x08, 
  0x00, 0x00, 0x70, 0x80, 0x04, 0x00, 0x08, 0x00 
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
};

#define CFG_TAG_53_R3        (&bt_qsoc_cfg_tag_53[0]) 
#define CFG_TAG_53_4020BD_B0 (&bt_qsoc_cfg_tag_53[0]) 
#define CFG_TAG_53_4020BD_B1 (&bt_qsoc_cfg_tag_53[0]) 
#define CFG_TAG_53_4021_B1   (&bt_qsoc_cfg_tag_53[0])  
#define CFG_TAG_53_4025_A0   (&bt_qsoc_cfg_tag_53[0])  
#define CFG_TAG_53_4025_B0   (&bt_qsoc_cfg_tag_53[0])  
#define CFG_TAG_53_4025_B1   (&bt_qsoc_cfg_tag_53[0])  
#define CFG_TAG_53_4025_B2   (&bt_qsoc_cfg_tag_53[0])
#define CFG_TAG_53_4025_B3   (&bt_qsoc_cfg_tag_53[0])
#define CFG_TAG_53_UNKNOWN   (&bt_qsoc_cfg_tag_53[0]) 

/* Tag 55 - GPIOs for Sleep, Atheros Coex */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_55[] = \
{
  0x27, 0x01, 0x37, 0x24, 

#if defined( FEATURE_BT_QSOC_SLEEP ) && !defined( FEATURE_BT_QSOC_INBAND_SLEEP )
  0x04, 0x15, /* HOST_WAKE (4) => GPIO #21 */
#else
  0xFF, 0x00, /* No Function */
#endif /* FEATURE_BT_QSOC_SLEEP && !FEATURE_BT_QSOC_INBAND_SLEEP */

#ifdef FEATURE_BT_WLAN_COEXISTENCE
  0x09, 0x02, /* PTA_RF_Active(9) => GPIO #2 */
  0x0A, 0x01, /* PTA_Status(10) => GPIO #1 */
  0x0C, 0x00, /* PTA_TxConfx(12) => GPIO #0 */
#else
  0xFF, 0x00, /* No Function */
  0xFF, 0x00, /* No Function */
  0xFF, 0x00, /* No Function */
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
  /* No Function */
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00
};

/* Tag 55 - GPIOs for Sleep, Atheros Coex for Fiji 
 * These mappings are valid for UART and in-band sleep UART.  
 * If support for H5 (3-wire UART) is ever added to the host, a different mapping will be required. 
 * */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_55_4025[] = \
{
  0x27, 0x01, 0x37, 0x24, 

#if defined( FEATURE_BT_QSOC_SLEEP ) && !defined( FEATURE_BT_QSOC_INBAND_SLEEP )
  0x04, 0x15, /* HOST_WAKE (4) => GPIO #21 */
#else
  0xFF, 0x00, /* No Function */
#endif /* FEATURE_BT_QSOC_SLEEP && !FEATURE_BT_QSOC_INBAND_SLEEP */

#ifdef FEATURE_BT_WLAN_COEXISTENCE
  0x09, 0x04, 
  0x0A, 0x03,
  0x0C, 0x01A,
#else
  0xFF, 0x00, /* No Function */
  0xFF, 0x00, /* No Function */
  0xFF, 0x00, /* No Function */
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
  /* No Function */
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 
  0xFF, 0x00, 0xFF, 0x00
};

#define CFG_TAG_55_R3        (&bt_qsoc_cfg_tag_55[0]) 
#define CFG_TAG_55_4020BD_B0 (&bt_qsoc_cfg_tag_55[0]) 
#define CFG_TAG_55_4020BD_B1 (&bt_qsoc_cfg_tag_55[0]) 
#define CFG_TAG_55_4021_B1   (&bt_qsoc_cfg_tag_55[0]) 
#define CFG_TAG_55_4025_A0   (&bt_qsoc_cfg_tag_55_4025[0]) 
#define CFG_TAG_55_4025_B0   (&bt_qsoc_cfg_tag_55_4025[0]) 
#define CFG_TAG_55_4025_B1   (&bt_qsoc_cfg_tag_55_4025[0]) 
#define CFG_TAG_55_4025_B2   (&bt_qsoc_cfg_tag_55_4025[0])
#define CFG_TAG_55_4025_B3   (&bt_qsoc_cfg_tag_55_4025[0])
#define CFG_TAG_55_UNKNOWN   (&bt_qsoc_cfg_tag_55[0]) 


/* #56 - Max Transmit Power level */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_56_4025_CLASS1[] = \
{
  0x04, 0x01, 0x38, 0x01,
  0x0A
};

/* #57 - AFH parameters */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_57[] = \
{
  0x20, 0x01, 0x39, 0x1D, 
  0x02, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40,
  0x00, 0x00, 0x0A, 0x05, 0x13, 0x4F, 0x02, 0x08,
  0x01, 0x0C, 0x01, 0x05, 0x01
};
#define CFG_TAG_57_R3        (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4020BD_B0 (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4020BD_B1 (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4021_B1   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4025_A0   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4025_B0   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4025_B1   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4025_B2   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_4025_B3   (&bt_qsoc_cfg_tag_57[0]) 
#define CFG_TAG_57_UNKNOWN   (&bt_qsoc_cfg_tag_57[0]) 

/* #70 Internal IO Resistor Settings - Configures the internal 
 * pullup/pulldown resistor settings for external I/O pins on the BTS4020 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_70[] = \
{
  0x0F, 0x01, 0x46, 0x0C, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00
};
#define CFG_TAG_70_R3        (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4020BD_B0 (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4020BD_B1 (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4021_B1   (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4025_A0   (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4025_B0   (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4025_B1   (&bt_qsoc_cfg_tag_70[0]) 
#define CFG_TAG_70_4025_B2   (&bt_qsoc_cfg_tag_70[0])
#define CFG_TAG_70_4025_B3   (&bt_qsoc_cfg_tag_70[0])
#define CFG_TAG_70_UNKNOWN   (&bt_qsoc_cfg_tag_70[0]) 

/* #71 Buffer Sizes - Tx buffer size - BT_QSOC_MAX_HC_TX_LEN, 
 *                    # of Tx buffers - BT_QSOC_MAX_HC_NUM_RX_BUF
 *                    Rx buffer size - BT_QSOC_MAX_HC_RX_LEN
 *                    # of Rx buffers - BT_QSOC_MAX_HC_NUM_TX_BUF
 *                    sco buffer size - BT_QSOC_SCO_BUF_SIZE
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_71[] = \
{
  0x0A, 0x01, 0x47, 0x07, 
  BT_QSOC_MAX_HC_RX_LEN_LSB, 
  BT_QSOC_MAX_HC_RX_LEN_MSB, 
  BT_QSOC_MAX_HC_TX_LEN_LSB, 
  BT_QSOC_MAX_HC_TX_LEN_MSB, 
  BT_QSOC_MAX_HC_NUM_RX_BUF, 
  BT_QSOC_MAX_HC_NUM_TX_BUF,
  BT_QSOC_SCO_BUF_SIZE
};
#define CFG_TAG_71_R3        (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4020BD_B0 (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4020BD_B1 (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4021_B1   (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4025_A0   (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4025_B0   (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4025_B1   (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4025_B2   (&bt_qsoc_cfg_tag_71[0]) 
#define CFG_TAG_71_4025_B3   (&bt_qsoc_cfg_tag_71[0])
#define CFG_TAG_71_UNKNOWN   (&bt_qsoc_cfg_tag_71[0]) 

/* #72 - HCI supported override */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_72[] = \
{
  0x44, 0x01, 0x48, 0x41,
  0X40, /* Length is a part of payload */ 
  0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_72_4025_bt_2_0[] = \
{
  0x19, 0x01, 0x48, 0x16,	
  0x15, 0xFF, 0xFF, 0xFF, 0x03, 0xCE, 0xFF, 0xEF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xF2, 0x0F, 0xE8, 
  0xFE, 0x3F, 0x00, 0x00, 0x00, 0x00
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_72_4020_bd_bt_2_0[] = \
{
  0x19, 0x01, 0x48, 0x16,
  0x15, 0xFF, 0xFF, 0xFF, 0x03, 0xCE, 0xFF, 0xEF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xF2, 0x0F, 0xE8, 
  0xFE, 0x3F, 0x00, 0x00, 0x00, 0x00	  
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_72_4020_bd_b1_bt_2_1[] = \
{
  0x19, 0x01, 0x48, 0x16,
  0x15, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};

#define CFG_TAG_72_4025_A0   (&bt_qsoc_cfg_tag_72[0]) 
#define CFG_TAG_72_4025_B0   (&bt_qsoc_cfg_tag_72[0]) 
#define CFG_TAG_72_4025_B1   (&bt_qsoc_cfg_tag_72_4025_bt_2_0[0]) 
#define CFG_TAG_72_4025_B2   (&bt_qsoc_cfg_tag_72_4025_bt_2_0[0]) 
#define CFG_TAG_72_4025_B3   (&bt_qsoc_cfg_tag_72_4025_bt_2_0[0])
#define CFG_TAG_72_4020_BD_B0_BT2_1 (&bt_qsoc_cfg_tag_72[0])
#define CFG_TAG_72_4020_BD_B0_BT2_0 (&bt_qsoc_cfg_tag_72_4020_bd_bt_2_0[0])
#define CFG_TAG_72_4020_BD_B1_BT2_1 (&bt_qsoc_cfg_tag_72_4020_bd_b1_bt_2_1[0])
#define CFG_TAG_72_4020_BD_B1_BT2_0 (&bt_qsoc_cfg_tag_72_4020_bd_bt_2_0[0])

/* #76 Switcher LDO Settings 
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_76[] = \
{
#ifdef FEATURE_BT_AUTOMATIC_SWITCHER
  0x0B, 0x01, 0x4C, 0x08, 
  0xBC, 0x80, 0x20, 0x03, 0x5E, 0x00, 0x29, 0x00
#else
  0x0B, 0x01, 0x4C, 0x08,
  0xBC, 0x82, 0x00, 0x02, 0x54, 0x1C, 0x20, 0x00
#endif /* FEATURE_BT_AUTOMATIC_SWITCHER */
};
/* BTS 4025 does not have freg, so this is not required */
#define CFG_TAG_76_R3        (&bt_qsoc_cfg_tag_76[0]) 
#define CFG_TAG_76_4020BD_B0 (&bt_qsoc_cfg_tag_76[0]) 
#define CFG_TAG_76_4020BD_B1 (&bt_qsoc_cfg_tag_76[0]) 
#define CFG_TAG_76_4021_B1   (&bt_qsoc_cfg_tag_76[0]) 
#define CFG_TAG_76_UNKNOWN   (&bt_qsoc_cfg_tag_76[0]) 

/* #78 Bluetooth version information - Bluetooth HCI and 
   Link Manager version parameters 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_78_bt_2_1_supported[] = \
{
  0x05, 0x01, 0x4E, 0x02, 
  0x04, 0x04
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_78_bt_2_1_not_supported[] = \
{
  0x05, 0x01, 0x4E, 0x02, 
  0x03, 0x03
};
#define CFG_TAG_78_4025_A0   (&bt_qsoc_cfg_tag_78_bt_2_1_supported[0]) 
#define CFG_TAG_78_UNKNOWN   (&bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0]) 

/* #95 BT Local Features - eSCO CQDDR evaluation - disabled 
 *                         L2CAP Flow Control - enabled
 *                         ACL Automatic Flush - enabled
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_95[] = \
{
  0x05, 0x01, 0x5F, 0x02, 
  0x00, 0x03
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_95_4025_b0[] = \
{
  0x0E, 0x01, 0x5F, 0x0B, 
  0x00, 0x03, 0x01, 0x0D, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x02, 0x02

};
#define CFG_TAG_95_R3        (&bt_qsoc_cfg_tag_95[0]) 
#define CFG_TAG_95_4020BD_B0 (&bt_qsoc_cfg_tag_95[0]) 
#define CFG_TAG_95_4020BD_B1 (&bt_qsoc_cfg_tag_95[0]) 
#define CFG_TAG_95_4021_B1   (&bt_qsoc_cfg_tag_95[0]) 
#define CFG_TAG_95_4025_A0   (&bt_qsoc_cfg_tag_95[0]) 
#define CFG_TAG_95_4025_B0   (&bt_qsoc_cfg_tag_95_4025_b0[0]) 
#define CFG_TAG_95_UNKNOWN   (&bt_qsoc_cfg_tag_95[0]) 

/* #97 System Clock Request */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_97_clock_sharing[] = \
{
  /* NVM: Active High */
  0x05, 0x01, 0x61, 0x02, 
  0x00, 0x01
};

#define CFG_TAG_97_CLK_SHARING_R3        (&bt_qsoc_cfg_tag_97_clock_sharing[0]) 
#define CFG_TAG_97_CLK_SHARING_4020BD_B0 (&bt_qsoc_cfg_tag_97_clock_sharing[0]) 
#define CFG_TAG_97_CLK_SHARING_4020BD_B1 (&bt_qsoc_cfg_tag_97_clock_sharing[0]) 
#define CFG_TAG_97_CLK_SHARING_4021_B1   (&bt_qsoc_cfg_tag_97_clock_sharing[0]) 
#define CFG_TAG_97_CLK_SHARING_4025_A0   (&bt_qsoc_cfg_tag_97_clock_sharing[0])  
#define CFG_TAG_97_CLK_SHARING_4025_B0   (&bt_qsoc_cfg_tag_97_clock_sharing[0])  
#define CFG_TAG_97_CLK_SHARING_4025_B1   (&bt_qsoc_cfg_tag_97_clock_sharing[0])  
#define CFG_TAG_97_CLK_SHARING_4025_B2   (&bt_qsoc_cfg_tag_97_clock_sharing[0])
#define CFG_TAG_97_CLK_SHARING_4025_B3   (&bt_qsoc_cfg_tag_97_clock_sharing[0])
#define CFG_TAG_97_CLK_SHARING_UNKNOWN   (&bt_qsoc_cfg_tag_97_clock_sharing[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_97_no_clock_sharing[] = \
{
  /* NVM: Active Undefined */
  0x05, 0x01, 0x61, 0x02, 
  0x00, 0xFF
};
#define CFG_TAG_97_NO_CLK_SHARING_R3        (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4020BD_B0 (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4020BD_B1 (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4021_B1   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4025_A0   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4025_B0   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4025_B1   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4025_B2   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 
#define CFG_TAG_97_NO_CLK_SHARING_4025_B3   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0])
#define CFG_TAG_97_NO_CLK_SHARING_UNKNOWN   (&bt_qsoc_cfg_tag_97_no_clock_sharing[0]) 

/* #99 System Reference Clock Control */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_99_19p2MHz[] = \
{
  /* 19.2 Mhz */
  0x04, 0x01, 0x63, 0x01, 
  0x07
};

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_99_32MHz[] = \
{
  /* 32 Mhz */
  0x04, 0x01, 0x63, 0x01, 
  0x0D
};

#define CFG_TAG_99_19P2MHZ_R3         (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 
#define CFG_TAG_99_19P2MHZ_4020_BD_B1 (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 
#define CFG_TAG_99_19P2MHZ_4025_A0    (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 
#define CFG_TAG_99_19P2MHZ_4025_B0    (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 
#define CFG_TAG_99_19P2MHZ_4025_B1    (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 
#define CFG_TAG_99_19P2MHZ_UNKNOWN    (&bt_qsoc_cfg_tag_99_19p2MHz[0]) 

#define CFG_TAG_99_32MHZ_R3           (&bt_qsoc_cfg_tag_99_32MHz[0]) 
#define CFG_TAG_99_32MHZ_4020_BD_B1   (&bt_qsoc_cfg_tag_99_32MHz[0]) 
#define CFG_TAG_99_32MHZ_4025_A0      (&bt_qsoc_cfg_tag_99_32MHz[0]) 
#define CFG_TAG_99_32MHZ_4025_B0      (&bt_qsoc_cfg_tag_99_32MHz[0]) 
#define CFG_TAG_99_32MHZ_4025_B1      (&bt_qsoc_cfg_tag_99_32MHz[0]) 
#define CFG_TAG_99_32MHZ_UNKNOWN      (&bt_qsoc_cfg_tag_99_32MHz[0]) 


/* #116 HW Encryption Enable Code 
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_116[] = \
{
  0x13, 0x01, 0x74, 0x10, 
  0x02, 0x4A, 0x01, 0x03, 0x11, 0x43, 0x08, 0x60, 
  0x70, 0x47, 0x00, 0x00, 0x48, 0x00, 0x00, 0xB0
};

#define CFG_TAG_116_4025_A0   (&bt_qsoc_cfg_tag_116[0]) 
#define CFG_TAG_116_UNKNOWN   (&bt_qsoc_cfg_tag_116[0]) 

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_119_4025_a0[] = \
{
      0xFB, 0x01, 0x77, 0xF8, 
      0x3D, 0x3E, 0xA7, 0xAA, 0xAB, 0xAC, 0xA6, 0xA8,
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16,
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16, 
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16, 
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16,
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16,
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16, 
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16, 
      0x1B, 0x09, 0xB7, 0x62, 0x80, 0x27, 0xBB, 0x16, 
      0x1B, 0x09, 0xB7, 0x62, 0x84, 0x07, 0xBB, 0x16, 
      0x1B, 0x09, 0xFB, 0x62, 0x84, 0x07, 0xFF, 0x17, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x16,
      0xBB, 0x08, 0xB7, 0x41, 0xC4, 0x00, 0xBB, 0x16,
      0xDB, 0x09, 0xB7, 0x42, 0x84, 0x07, 0xBB, 0x16, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12,
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12, 
      0xFB, 0x08, 0xB7, 0x40, 0x84, 0x00, 0xBB, 0x12, 
      0xBB, 0x08, 0xB7, 0x41, 0xC4, 0x00, 0xBB, 0x12, 
      0xDB, 0x09, 0xB7, 0x42, 0x84, 0x07, 0xBB, 0x12, 
};

#define CFG_TAG_119_4025_A0 (&bt_qsoc_cfg_tag_119_4025_a0[0]) 

/* #123 Temperature Calibration Configuration */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_123_4025_a0[] = \
{
  0x14, 0x01, 0x7b, 0x11, 
  0x16, 0x13, 0x10, 0x0D, 0x0A, 0x08, 0x06, 0x01, 
  0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x01, 
  0x00
};

#define CFG_TAG_123_4025_A0   (&bt_qsoc_cfg_tag_123_4025_a0[0]) 


/******************************************************************
 *            R3 BD (4020BD) B0 AND B1 R O M  P A T C H E S              *
 ******************************************************************/
/* #106 Rom Insertion Patch 
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4020bd_b0_tag_106[] = \
{
  0x37, 0x01, 0x6A, 0x34,
  0x50, 0x81, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x84, 0x00, 0x28, 0x00, 0x49, 0x08, 0x47, 0x09,
  0x00, 0x00, 0x00, 0x08, 0x40, 0x06, 0x49, 0x08,
  0x40, 0x01, 0x99, 0x01, 0x29, 0x01, 0xD0, 0x01,
  0x49, 0x08, 0x47, 0x01, 0x49, 0x08, 0x47, 0x61,
  0x81, 0x00, 0x70, 0x59, 0x81, 0x00, 0x70, 0xF9,
  0xCC, 0xFF, 0xFF, 0x00

};
#define CFG_TAG_106_4020BD_B0       (&bt_qsoc_cfg_4020bd_b0_tag_106[0])

/* #107 Rom Insertion Patch 
 */

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4020bd_b0_tag_107[] = \
{
  0x3B, 0x01, 0x6B, 0x38,
  0x50, 0x82, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x84, 0x00, 0x2D, 0x00, 0x49, 0x08, 0x47, 0x09,
  0x00, 0x00, 0x00, 0x18, 0x71, 0xDE, 0x80, 0x20,
  0x69, 0x06, 0x4E, 0xE1, 0x68, 0xE0, 0x31, 0x49,
  0x78, 0x00, 0x29, 0x00, 0xD0, 0x37, 0x40, 0x02,
  0x4E, 0xF1, 0x43, 0x00, 0x4E, 0x30, 0x47, 0x59,
  0x82, 0x00, 0x70, 0xF9, 0xCC, 0xFF, 0xFF, 0x00
};

#define CFG_TAG_107_4020BD_B0       (&bt_qsoc_cfg_4020bd_b0_tag_107[0])

/* #108 Rom Insertion Patch 
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4020bd_b0_tag_108[] = \
{
  0x5D, 0x01, 0x6C, 0x5A, 
  0xC8, 0x6D, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x84, 0x08, 0x4F, 0x00, 0x49, 0x08, 0x47, 0x09,
  0x00, 0x00, 0x00, 0x8E, 0x21, 0x18, 0x73, 0x20,
  0x6F, 0x0A, 0x5A, 0x00, 0x21, 0x0B, 0x4B, 0x00,
  0xF0, 0x1A, 0xF8, 0x00, 0x28, 0x0C, 0xD1, 0x20,
  0x6F, 0x00, 0x21, 0x80, 0x30, 0xC2, 0x89, 0x20,
  0x1C, 0x07, 0x4B, 0x00, 0xF0, 0x10, 0xF8, 0x21,
  0x1C, 0x01, 0x20, 0x05, 0x4B, 0x00, 0xF0, 0x0B,
  0xF8, 0x01, 0x48, 0x00, 0x47, 0x00, 0x00, 0xE1,
  0x6D, 0x00, 0x70, 0x29, 0x82, 0x00, 0x70, 0xD9,
  0x21, 0x01, 0x70, 0x81, 0x5B, 0x00, 0x70, 0x18,
  0x47, 0x00
};
#define CFG_TAG_108_4020BD_B0       (&bt_qsoc_cfg_4020bd_b0_tag_108[0])

/******************************************************************
 *             4 0 2 5  A 0  R O M  P A T C H E S                 *
 ******************************************************************/

/* #107 Rom Insertion Patch #7
 * TT-4801: 4020 BD B0: In-Band Sleep Does not Respond to Un-Break
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_tag_107[] = \
{
  0x22, 0x01, 0x6B, 0x1F, 
  0x80, 0xE4, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x80, 0x04, 0x14, 0x05, 0x00, 0x00, 0x00, 0x01,
  0x49, 0x88, 0x70, 0x01, 0x49, 0x08, 0x47, 0xDC,
  0x6C, 0x00, 0x00, 0xED, 0x83, 0x03, 0x70 
};
#define CFG_TAG_107_4025_A0       (&bt_qsoc_cfg_4025_a0_tag_107[0]) 

/* #108 Rom Insertion Patch #8
 * TT-4765: 4025_A0: Inquiry with SCO and Sniff causes the device to abort.
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_tag_108[] = \
{
  0x12, 0x01, 0x6C, 0x0F, 
  0xA0, 0x6A, 0x02, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x04, 0x04, 0xC0, 0x46, 0x61, 0x7E 
};
#define CFG_TAG_108_4025_A0       (&bt_qsoc_cfg_4025_a0_tag_108[0]) 

/* #111 Rom Insertion Patch #11
 * TT-4724: 4025 A0V0: Legacy pairing failure using WinMobile stack
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_tag_111[] = \
{
  0x44, 0x01, 0x6F, 0x41, 
  0x80, 0x6F, 0x01, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x84, 0x08, 0x36, 0x00, 0x48, 0x00, 0x47, 0x09, 
  0x00, 0x00, 0x00, 0x16, 0x1C, 0x08, 0x69, 0x00, 
  0x28, 0x08, 0xD0, 0x0D, 0x1C, 0x20, 0x1C, 0x18, 
  0x30, 0x06, 0x49, 0x00, 0xF0, 0x0C, 0xF8, 0x29, 
  0x1C, 0x00, 0x20, 0x28, 0x61, 0xB7, 0x78, 0x01, 
  0x20, 0x07, 0x40, 0x00, 0x48, 0x00, 0x47, 0x89, 
  0x6F, 0x01, 0x70, 0x67, 0xAD, 0x02, 0x70, 0x08, 
  0x47
};
#define CFG_TAG_111_4025_A0       (&bt_qsoc_cfg_4025_a0_tag_111[0]) 

/* #112 Rom Insertion Patch #12
 * TT-4724: 4025 A0V0: Legacy pairing failure using WinMobile stack 
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_tag_112[] = \
{
  0x5A, 0x01, 0x70, 0x57, 
  0x0C, 0xD9, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x84, 0x08, 0x4C, 0x00, 0x48, 0x00, 0x47, 0x09, 
  0x00, 0x00, 0x00, 0xA0, 0x6B, 0x40, 0x30, 0x40, 
  0x79, 0x00, 0x28, 0x01, 0xD0, 0x04, 0x28, 0x0B, 
  0xD1, 0x04, 0x21, 0x08, 0x9A, 0x20, 0x1C, 0x08, 
  0xB4, 0x0A, 0x4B, 0x00, 0xF0, 0x04, 0xF8, 0x08, 
  0xBC, 0x00, 0x28, 0x04, 0xD0, 0x05, 0xE0, 0x18, 
  0x47, 0x04, 0x21, 0x02, 0x48, 0x00, 0x47, 0x02, 
  0x48, 0x00, 0x47, 0x02, 0x48, 0x00, 0x47, 0x41, 
  0xD9, 0x00, 0x70, 0x1F, 0xD9, 0x00, 0x70, 0x43, 
  0xE5, 0x00, 0x70, 0x3D, 0x71, 0x01, 0x70
};
#define CFG_TAG_112_4025_A0       (&bt_qsoc_cfg_4025_a0_tag_112[0]) 
  
/* #113 Rom Insertion Patch #13
 * TT-4722: 4025 A0V0: Refresh_Encryption_Key not reported as a 
 * supported HCI Command
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_tag_113[] = \
{
  0x12, 0x01, 0x71, 0x0F, 
  0x38, 0xB1, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x04, 0x04, 0xE8, 0xFE, 0x3F, 0xF7
};
#define CFG_TAG_113_4025_A0       (&bt_qsoc_cfg_4025_a0_tag_113[0]) 

/* #114 Rom Insertion Patch #14
 * 4025 A0 Fiji: Patch the Desense feature - Add 2 register and 
 * update Rx Register settings
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_19p2MHz_tag_114[] = \
{
  0x4A, 0x01, 0x72, 0x47, 
  0x2C, 0x3B, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x84, 0x08, 0x3C, 0x00, 0x48, 0x00, 0x47, 0x09, 
  0x00, 0x00, 0x00, 0x0A, 0x48, 0x11, 0x21, 0x01, 
  0x60, 0x1A, 0x21, 0x41, 0x61, 0x15, 0x21, 0x81, 
  0x61, 0x03, 0x21, 0x81, 0x62, 0x07, 0x48, 0x06, 
  0x21, 0x01, 0x60, 0x00, 0x21, 0x81, 0x62, 0x41, 
  0x66, 0xC1, 0x66, 0x01, 0x67, 0x10, 0x21, 0x81, 
  0x66, 0x96, 0x21, 0x41, 0x67, 0x70, 0x47, 0x04, 
  0x01, 0x00, 0xA8, 0x9C, 0x01, 0x00, 0xA8
};
#define CFG_TAG_114_19P2MHZ_4025_A0       (&bt_qsoc_cfg_4025_a0_19p2MHz_tag_114[0]) 

/* #115 Rom Insertion Patch #15
 * 4025 A0 Fiji: Patch the Desense feature - Add 2 register and 
 * update Rx Register settings
 */
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_a0_19p2MHz_tag_115[] = \
{
  0x2E, 0x01, 0x73, 0x2B, 
  0x58, 0x3B, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x84, 0x08, 0x20, 0x00, 0x48, 0x00, 0x47, 0x09, 
  0x00, 0x00, 0x00, 0x19, 0x20, 0x48, 0x60, 0x18, 
  0x20, 0x88, 0x61, 0x23, 0x20, 0xC8, 0x61, 0x04, 
  0x22, 0xCA, 0x62, 0x00, 0x49, 0x08, 0x47, 0x61, 
  0x3B, 0x03, 0x70
};
#define CFG_TAG_115_19P2MHZ_4025_A0       (&bt_qsoc_cfg_4025_a0_19p2MHz_tag_115[0]) 

/******************************************************************
 *             4 0 2 5  B 0  R O M  P A T C H E S                 *
 ******************************************************************/

/* TT 5047: Fiji B0 - Watchdog reset occurs when LPPS is enabled */

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_b0_tag_112[] = \
{
  0x36, 0x01, 0x70, 0x33, 
  0x68, 0x21, 0x03, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x84, 0x08, 0x28, 0x00, 0x48, 0x00, 0x47, 0x09,
  0x00, 0x00, 0x00, 0x01, 0x25, 0xA5, 0x60, 0x03,
  0x4E, 0x75, 0x60, 0x03, 0x49, 0x00, 0x20, 0x08,
  0x63, 0x03, 0x48, 0x00, 0x47, 0x00, 0x00, 0x00,
  0x10, 0x00, 0xB0, 0xC0, 0xF0, 0x0F, 0xB0, 0x71,
  0x21, 0x03, 0x70
};
#define CFG_TAG_112_4025_B0       (&bt_qsoc_cfg_4025_b0_tag_112[0]) 

/* TT-5021: 4025 B0 TX mode fails while in DUT */

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_b0_tag_113[] = \
{
  0x12, 0x01, 0x71, 0x0F, 
  0x04, 0x5A, 0x02, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x04, 0x04, 0xC9, 0x69, 0x40, 0x1A 
};
#define CFG_TAG_113_4025_B0       (&bt_qsoc_cfg_4025_b0_tag_113[0]) 

/* TT-5026: 4025 B0 Max length RNR followed by Write EIR Data results
 * in no events
 */

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_b0_tag_114[] = \
{
  0x12, 0x01, 0x72, 0x0F, 
  0x14, 0xD0, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x04, 0x04, 0xC0, 0x46, 0xC0, 0x46 
};
#define CFG_TAG_114_4025_B0       (&bt_qsoc_cfg_4025_b0_tag_114[0]) 

/* TT-5025: 4025 B0 HCI_RESET in the middle of Remote Name causes
 * memory allocation issue
 */

static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_4025_b0_tag_115[] = \
{
  0x3E, 0x01, 0x73, 0x3B, 
  0xD0, 0x54, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
  0x84, 0x08, 0x30, 0x00, 0x4E, 0x30, 0x47, 0x09,
  0x00, 0x00, 0x00, 0xA0, 0x6B, 0x00, 0x28, 0x04,
  0xD0, 0x00, 0xF0, 0x05, 0xF8, 0xA0, 0x6B, 0x00,
  0xF0, 0x04, 0xF8, 0x02, 0x4E, 0x30, 0x47, 0x02,
  0x4E, 0x30, 0x47, 0x02, 0x4E, 0x30, 0x47, 0xDB,
  0x54, 0x00, 0x70, 0x35, 0x7B, 0x00, 0x70, 0x8B,
  0xB7, 0x02, 0x70
};
#define CFG_TAG_115_4025_B0       (&bt_qsoc_cfg_4025_b0_tag_115[0]) 

/*********** R U N  T I M E ****************/
#define CFG_TAG_2_RAM (&bt_qsoc_cfg_tag_2_ram[0])
static bt_qsoc_cfg_element_type bt_qsoc_cfg_tag_2_ram[] = \
{
  0x09, 0x01, 0x02, 0x06, 
  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

/*****************************************************************************/
/*********************** R 3   C O N F I G   T A B L E ***********************/
/*****************************************************************************/

#define SIZEOF_BT_QSOC_CFG_TBL_R3 ((sizeof(bt_qsoc_cfg_tbl_r3))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_r3[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_R3,
  CFG_TAG_5_R3,
  CFG_TAG_6_R3,
  CFG_TAG_17_R3,
  CFG_TAG_27_R3,
  CFG_TAG_44_R3,
  CFG_TAG_45_R3,
  CFG_TAG_46_R3,
  CFG_TAG_53_R3,
  CFG_TAG_55_R3,
  CFG_TAG_57_R3,
  CFG_TAG_70_R3,
  CFG_TAG_71_R3,
  CFG_TAG_76_R3,
  CFG_TAG_95_R3,
};

#define SIZEOF_BT_QSOC_CFG_TBL_R3_RAM ((sizeof(bt_qsoc_cfg_tbl_r3_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_r3_ram[] = \
{
  NULL, /* CFG_TAG_37_R3_RAM_INDEX */
  NULL, /* CFG_TAG_38_R3_RAM_INDEX */
  NULL, /* CFG_TAG_97_R3_RAM_INDEX */
};
#define CFG_TAG_37_R3_RAM_INDEX  (0) /* #37: sleep parameters */
#define CFG_TAG_38_R3_RAM_INDEX  (1) /* #38: soc logging   CFG_TAG_38_R3,*/
#define CFG_TAG_97_R3_RAM_INDEX  (2) /* #97: clock sharing */                  

/*****************************************************************************/
/***************** R 3 B D (4020BD) B0 C O N F I G   T A B L E ***************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_4020BD_B0 ((sizeof(bt_qsoc_cfg_tbl_4020bd_b0))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4020bd_b0[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4020BD_B0,
  CFG_TAG_5_4020BD_B0,
  CFG_TAG_17_4020BD_B0,
  CFG_TAG_27_4020BD_B0,
  CFG_TAG_44_4020BD_B0,
  CFG_TAG_45_4020BD_B0,
  CFG_TAG_46_4020BD_B0,
  CFG_TAG_53_4020BD_B0,
  CFG_TAG_55_4020BD_B0,
  CFG_TAG_57_4020BD_B0,
  CFG_TAG_70_4020BD_B0,
  CFG_TAG_71_4020BD_B0,
  CFG_TAG_76_4020BD_B0,
  CFG_TAG_95_4020BD_B0,
/* 4020BD B0 ROM Patches 100-114 */
  CFG_TAG_106_4020BD_B0,
  CFG_TAG_107_4020BD_B0,
  CFG_TAG_108_4020BD_B0,
};

#define SIZEOF_BT_QSOC_CFG_TBL_4020BD_B0_RAM ((sizeof(bt_qsoc_cfg_tbl_4020bd_b0_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4020bd_b0_ram[] = \
{
  NULL, /* CFG_TAG_6_4020BD_B0_RAM_INDEX */	
  NULL, /* CFG_TAG_32_4020BD_B0_RAM_INDEX */	  
  NULL, /* CFG_TAG_37_4020BD_B0_RAM_INDEX */
  NULL, /* CFG_TAG_38_4020BD_B0_RAM_INDEX */
  NULL, /* CFG_TAG_72_4020BD_B0_RAM_INDEX */
  NULL, /* CFG_TAG_78_4020BD_B0_RAM_INDEX */
  NULL, /* CFG_TAG_97_4020BD_B0_RAM_INDEX */
};
#define CFG_TAG_6_4020BD_B0_RAM_INDEX    (0) /* #6 : BT 2.1 support*/
#define CFG_TAG_32_4020BD_B0_RAM_INDEX   (1) /* #32: USB version number */
#define CFG_TAG_37_4020BD_B0_RAM_INDEX   (2) /* #37: sleep parameters */
#define CFG_TAG_38_4020BD_B0_RAM_INDEX   (3) /* #38: soc logging */
#define CFG_TAG_72_4020BD_B0_RAM_INDEX   (4) /* #72: HCI supported override */
#define CFG_TAG_78_4020BD_B0_RAM_INDEX   (5) /* #78: BT LMP version */
#define CFG_TAG_97_4020BD_B0_RAM_INDEX   (6) /* #97: clock sharing */                  
// tag 117/118 not required for 4020BD B0, tag 119 replaces tag 117/118

/*****************************************************************************/
/***************** R 3 B D (4020BD) B1 C O N F I G   T A B L E ***************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_4020BD_B1 ((sizeof(bt_qsoc_cfg_tbl_4020bd_b1))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4020bd_b1[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4020BD_B1,
  CFG_TAG_5_4020BD_B1,
  CFG_TAG_17_4020BD_B1,
  CFG_TAG_27_4020BD_B1,
  CFG_TAG_44_4020BD_B1,
  CFG_TAG_45_4020BD_B1,
  CFG_TAG_46_4020BD_B1,
  CFG_TAG_53_4020BD_B1,
  CFG_TAG_55_4020BD_B1,
  CFG_TAG_57_4020BD_B1,
  CFG_TAG_70_4020BD_B1,
  CFG_TAG_71_4020BD_B1,
  CFG_TAG_76_4020BD_B1,
  CFG_TAG_95_4020BD_B1,
};

#define SIZEOF_BT_QSOC_CFG_TBL_4020BD_B1_RAM ((sizeof(bt_qsoc_cfg_tbl_4020bd_b1_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4020bd_b1_ram[] = \
{
  NULL, /* CFG_TAG_6_4020BD_B1_RAM_INDEX */	 
  NULL, /* CFG_TAG_32_4020BD_B1_RAM_INDEX */	 
  NULL, /* CFG_TAG_37_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_38_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_41_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_72_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_78_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_97_4020BD_B1_RAM_INDEX */
  NULL, /* CFG_TAG_99_4020BD_B1_RAM_INDEX */
};
#define CFG_TAG_6_4020BD_B1_RAM_INDEX    (0) /* #6 : BT 2.1 support */
#define CFG_TAG_32_4020BD_B1_RAM_INDEX   (1) /* #32: USB version number */
#define CFG_TAG_37_4020BD_B1_RAM_INDEX   (2) /* #37: sleep parameters */
#define CFG_TAG_38_4020BD_B1_RAM_INDEX   (3) /* #38: soc logging */
#define CFG_TAG_41_4020BD_B1_RAM_INDEX   (4) /* #41 Radio Register Override */
#define CFG_TAG_72_4020BD_B1_RAM_INDEX   (5) /* #72: HCI supported override */
#define CFG_TAG_78_4020BD_B1_RAM_INDEX   (6) /* #78: BT LMP version */
#define CFG_TAG_97_4020BD_B1_RAM_INDEX   (7) /* #97: clock sharing */                  
#define CFG_TAG_99_4020BD_B1_RAM_INDEX   (8) /* #99: System Reference Clock Control*/

/*****************************************************************************/
/**************** 4 0 2 1  B 1  C O N F I G   T A B L E **********************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_4021_B1 ((sizeof(bt_qsoc_cfg_tbl_4021_b1))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4021_b1[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4021_B1,
  CFG_TAG_5_4021_B1,
  CFG_TAG_17_4021_B1,
  CFG_TAG_27_4021_B1,
  CFG_TAG_44_4021_B1,
  CFG_TAG_45_4021_B1,
  CFG_TAG_46_4021_B1,
  CFG_TAG_53_4021_B1,
  CFG_TAG_55_4021_B1,
  CFG_TAG_57_4021_B1,
  CFG_TAG_70_4021_B1,
  CFG_TAG_71_4021_B1,
  CFG_TAG_76_4021_B1,
  CFG_TAG_95_4021_B1,
/* 4021 B1 ROM Patches 107-115 */
}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4021_B1_RAM ((sizeof(bt_qsoc_cfg_tbl_4021_b1_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4021_b1_ram[] = \
{
  NULL, /* CFG_TAG_6_4021_B1_RAM_INDEX */	
  NULL, /* CFG_TAG_37_4021_B1_RAM_INDEX */
  NULL, /* CFG_TAG_38_4021_B1_RAM_INDEX */ 
  NULL, /* CFG_TAG_78_4021_B1_RAM_INDEX */
  NULL, /* CFG_TAG_97_4021_B1_RAM_INDEX */
};
#define CFG_TAG_6_4021_B1_RAM_INDEX   (0)  /* #6 : BT 2.1 support */
#define CFG_TAG_37_4021_B1_RAM_INDEX  (1)  /* #37: sleep parameters */
#define CFG_TAG_38_4021_B1_RAM_INDEX  (2)  /* #38: soc logging */
#define CFG_TAG_78_4021_B1_RAM_INDEX  (3)  /* #78: BT LMP version */
#define CFG_TAG_97_4021_B1_RAM_INDEX  (4)  /* #97: clock sharing */

/*****************************************************************************/
/*********************** 4025 A0  C O N F I G   T A B L E ********************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_4025_A0 ((sizeof(bt_qsoc_cfg_tbl_4025_a0))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_a0[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4025_A0,
  CFG_TAG_5_4025_A0,
  CFG_TAG_6_4025_A0,
  CFG_TAG_17_4025_A0,
  CFG_TAG_27_4025_A0,
  CFG_TAG_28_4025_A0,
  CFG_TAG_36_4025_A0,
  CFG_TAG_39_4025_A0,
  CFG_TAG_40_4025_A0,
  CFG_TAG_44_4025_A0,
  CFG_TAG_45_4025_A0,
  CFG_TAG_46_4025_A0,
  CFG_TAG_53_4025_A0,
  CFG_TAG_55_4025_A0,
  CFG_TAG_57_4025_A0,
  CFG_TAG_70_4025_A0,
  CFG_TAG_71_4025_A0,
  CFG_TAG_72_4025_A0,
  CFG_TAG_78_4025_A0,
  CFG_TAG_95_4025_A0,
/* 4025_A0 ROM Patches */
  CFG_TAG_107_4025_A0,
  CFG_TAG_108_4025_A0,
  CFG_TAG_111_4025_A0,
  CFG_TAG_112_4025_A0,
  CFG_TAG_113_4025_A0,
/* 4025 A0 NVM continued */
  CFG_TAG_116_4025_A0,
  CFG_TAG_119_4025_A0,
  CFG_TAG_123_4025_A0

}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4025_A0_RAM ((sizeof(bt_qsoc_cfg_tbl_4025_a0_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_a0_ram[] = \
{
  NULL, /* CFG_TAG_32_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_37_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_38_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_41_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_97_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_99_4025_A0_RAM_INDEX */
  NULL, /* CFG_TAG_114_4025_A0_RAM_INDEX */
  NULL  /* CFG_TAG_115_4025_A0_RAM_INDEX */
};


#define CFG_TAG_32_4025_A0_RAM_INDEX  (0)  /* #32: USB version number */
#define CFG_TAG_37_4025_A0_RAM_INDEX  (1)  /* #37: sleep parameters */
#define CFG_TAG_38_4025_A0_RAM_INDEX  (2)  /* #38: soc logging*/
#define CFG_TAG_41_4025_A0_RAM_INDEX  (3)  /* #41: radio registers override */
#define CFG_TAG_97_4025_A0_RAM_INDEX  (4)  /* #97: clock sharing */
#define CFG_TAG_99_4025_A0_RAM_INDEX  (5)  /* #99: 19.2/32Mhz clock */
#define CFG_TAG_114_4025_A0_RAM_INDEX (6)  /* #114: ROM patches for 19.2MHz only */
#define CFG_TAG_115_4025_A0_RAM_INDEX (7)  /* #115: ROM patches for 19.2MHz only */

/*****************************************************************************/
/*********************** 4025 B0  C O N F I G   T A B L E ********************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_4025_B0 ((sizeof(bt_qsoc_cfg_tbl_4025_b0))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b0[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4025_B0,
  CFG_TAG_5_4025_B0,
  CFG_TAG_17_4025_B0,
  CFG_TAG_27_4025_B0,
  CFG_TAG_40_4025_B0,
  CFG_TAG_44_4025_B0,
  CFG_TAG_45_4025_B0,
  CFG_TAG_46_4025_B0,
  CFG_TAG_53_4025_B0,
  CFG_TAG_55_4025_B0,
  CFG_TAG_57_4025_B0,
  CFG_TAG_70_4025_B0,
  CFG_TAG_71_4025_B0,
  CFG_TAG_72_4025_B0,
  CFG_TAG_95_4025_B0,
/* 4025 B0 ROM Patches 112-115 */
  CFG_TAG_112_4025_B0,
  CFG_TAG_113_4025_B0,
  CFG_TAG_114_4025_B0,
  CFG_TAG_115_4025_B0
}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B0_RAM ((sizeof(bt_qsoc_cfg_tbl_4025_b0_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b0_ram[] = \
{
  NULL, /* CFG_TAG_6_4025_B0_RAM_INDEX */	
  NULL, /* CFG_TAG_37_4025_B0_RAM_INDEX */
  NULL, /* CFG_TAG_38_4025_B0_RAM_INDEX */
  NULL, /* CFG_TAG_78_4025_B0_RAM_INDEX */
  NULL, /* CFG_TAG_97_4025_B0_RAM_INDEX */
};
#define CFG_TAG_6_4025_B0_RAM_INDEX   (0)  /* #6 : BT 2.1 support*/
#define CFG_TAG_37_4025_B0_RAM_INDEX  (1)  /* #37: sleep parameters */
#define CFG_TAG_38_4025_B0_RAM_INDEX  (2)  /* #38: soc logging */
#define CFG_TAG_78_4025_B0_RAM_INDEX  (3)  /* #78: BT LMP version */
#define CFG_TAG_97_4025_B0_RAM_INDEX  (4)  /* #97: clock sharing */

/*****************************************************************************/
/*********************** 4025 B1  C O N F I G   T A B L E ********************/
/*****************************************************************************/

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B1 ((sizeof(bt_qsoc_cfg_tbl_4025_b1))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b1[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4025_B1,
  CFG_TAG_5_4025_B1,
  CFG_TAG_17_4025_B1,
  CFG_TAG_27_4025_B1,
  CFG_TAG_44_4025_B1,
  CFG_TAG_45_4025_B1,
  CFG_TAG_46_4025_B1,
  CFG_TAG_53_4025_B1,
  CFG_TAG_55_4025_B1,
  CFG_TAG_57_4025_B1,
  CFG_TAG_70_4025_B1,
  CFG_TAG_71_4025_B1
}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B1_RAM ((sizeof(bt_qsoc_cfg_tbl_4025_b1_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b1_ram[] = \
{
  NULL, /* CFG_TAG_6_4025_B1_RAM_INDEX */	
  NULL, /* CFG_TAG_32_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_36_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_37_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_38_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_41_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_56_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_72_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_78_4025_B1_RAM_INDEX */
  NULL, /* CFG_TAG_97_4025_B1_RAM_INDEX */
};
#define CFG_TAG_6_4025_B1_RAM_INDEX   (0)  /* #6 : BT 2.1 Support*/
#define CFG_TAG_32_4025_B1_RAM_INDEX  (1)  /* #32: USB version number */
#define CFG_TAG_36_4025_B1_RAM_INDEX  (2)  /* #36: Power Configuration */
#define CFG_TAG_37_4025_B1_RAM_INDEX  (3)  /* #37: sleep parameters */
#define CFG_TAG_38_4025_B1_RAM_INDEX  (4)  /* #38: soc logging */
#define CFG_TAG_41_4025_B1_RAM_INDEX  (5)  /* #41:  */
#define CFG_TAG_56_4025_B1_RAM_INDEX  (6)  /* #56:  */
#define CFG_TAG_72_4025_B1_RAM_INDEX  (7)  /* #72: HCI supported override */ 
#define CFG_TAG_78_4025_B1_RAM_INDEX  (8)  /* #78: BT LMP version */ 
#define CFG_TAG_97_4025_B1_RAM_INDEX  (9)  /* #97: clock sharing */

/*****************************************************************************/
/*********************** 4025 B2  C O N F I G   T A B L E ********************/
/*****************************************************************************/

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B2 ((sizeof(bt_qsoc_cfg_tbl_4025_b2))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b2[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4025_B2,
  CFG_TAG_5_4025_B2,
  CFG_TAG_17_4025_B2,
  CFG_TAG_27_4025_B2,
  CFG_TAG_44_4025_B2,
  CFG_TAG_45_4025_B2,
  CFG_TAG_46_4025_B2,
  CFG_TAG_53_4025_B2,
  CFG_TAG_55_4025_B2,
  CFG_TAG_57_4025_B2,
  CFG_TAG_70_4025_B2,
  CFG_TAG_71_4025_B2
}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B2_RAM ((sizeof(bt_qsoc_cfg_tbl_4025_b2_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b2_ram[] = \
{
  NULL, /* CFG_TAG_6_4025_B2_RAM_INDEX */	
  NULL, /* CFG_TAG_32_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_36_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_37_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_38_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_41_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_56_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_72_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_78_4025_B2_RAM_INDEX */
  NULL, /* CFG_TAG_97_4025_B2_RAM_INDEX */
};
#define CFG_TAG_6_4025_B2_RAM_INDEX   (0)  /* #6 : BT 2.1 Support*/
#define CFG_TAG_32_4025_B2_RAM_INDEX  (1)  /* #32: USB version number */
#define CFG_TAG_36_4025_B2_RAM_INDEX  (2)  /* #36: Power Configuration */
#define CFG_TAG_37_4025_B2_RAM_INDEX  (3)  /* #37: sleep parameters */
#define CFG_TAG_38_4025_B2_RAM_INDEX  (4)  /* #38: soc logging */
#define CFG_TAG_41_4025_B2_RAM_INDEX  (5)  /* #41:  */
#define CFG_TAG_56_4025_B2_RAM_INDEX  (6)  /* #56:  */
#define CFG_TAG_72_4025_B2_RAM_INDEX  (7)  /* #72: HCI supported override */ 
#define CFG_TAG_78_4025_B2_RAM_INDEX  (8)  /* #78: BT LMP version */ 
#define CFG_TAG_97_4025_B2_RAM_INDEX  (9)  /* #97: clock sharing */

/*****************************************************************************/
/*********************** 4025 B3  C O N F I G   T A B L E ********************/
/*****************************************************************************/

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B3 ((sizeof(bt_qsoc_cfg_tbl_4025_b3))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b3[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_4025_B3,
  CFG_TAG_5_4025_B3,
  CFG_TAG_17_4025_B3,
  CFG_TAG_27_4025_B3,
  CFG_TAG_44_4025_B3,
  CFG_TAG_45_4025_B3,
  CFG_TAG_46_4025_B3,
  CFG_TAG_53_4025_B3,
  CFG_TAG_55_4025_B3,
  CFG_TAG_57_4025_B3,
  CFG_TAG_70_4025_B3,
  CFG_TAG_71_4025_B3
}; 

#define SIZEOF_BT_QSOC_CFG_TBL_4025_B3_RAM ((sizeof(bt_qsoc_cfg_tbl_4025_b3_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_4025_b3_ram[] = \
{
  NULL, /* CFG_TAG_6_4025_B3_RAM_INDEX */	
  NULL, /* CFG_TAG_32_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_36_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_37_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_38_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_41_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_56_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_72_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_78_4025_B3_RAM_INDEX */
  NULL, /* CFG_TAG_97_4025_B3_RAM_INDEX */
};
#define CFG_TAG_6_4025_B3_RAM_INDEX   (0)  /* #6 : BT 2.1 Support*/
#define CFG_TAG_32_4025_B3_RAM_INDEX  (1)  /* #32: USB version number */
#define CFG_TAG_36_4025_B3_RAM_INDEX  (2)  /* #36: Power Configuration */
#define CFG_TAG_37_4025_B3_RAM_INDEX  (3)  /* #37: sleep parameters */
#define CFG_TAG_38_4025_B3_RAM_INDEX  (4)  /* #38: soc logging */
#define CFG_TAG_41_4025_B3_RAM_INDEX  (5)  /* #41:  */
#define CFG_TAG_56_4025_B3_RAM_INDEX  (6)  /* #56:  */
#define CFG_TAG_72_4025_B3_RAM_INDEX  (7)  /* #72: HCI supported override */ 
#define CFG_TAG_78_4025_B3_RAM_INDEX  (8)  /* #78: BT LMP version */ 
#define CFG_TAG_97_4025_B3_RAM_INDEX  (9)  /* #97: clock sharing */

/*****************************************************************************/
/******************* UNKNOWN BTS   C O N F I G   T A B L E *******************/
/*****************************************************************************/
#define SIZEOF_BT_QSOC_CFG_TBL_UNKNOWN ((sizeof(bt_qsoc_cfg_tbl_unknown))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_unknown[] = \
{
  (bt_qsoc_cfg_cst_element_type*)CFG_TAG_2_RAM,
  CFG_TAG_4_UNKNOWN,
  CFG_TAG_5_UNKNOWN,
  CFG_TAG_6_UNKNOWN,
  CFG_TAG_17_UNKNOWN,
  CFG_TAG_27_UNKNOWN,
  CFG_TAG_28_UNKNOWN,
  CFG_TAG_44_UNKNOWN,
  CFG_TAG_45_UNKNOWN,
  CFG_TAG_46_UNKNOWN,
  CFG_TAG_53_UNKNOWN,
  CFG_TAG_55_UNKNOWN,
  CFG_TAG_57_UNKNOWN,
  CFG_TAG_70_UNKNOWN,
  CFG_TAG_71_UNKNOWN,
  CFG_TAG_76_UNKNOWN,
  CFG_TAG_78_UNKNOWN,
  CFG_TAG_95_UNKNOWN,
  CFG_TAG_116_UNKNOWN
};

#define SIZEOF_BT_QSOC_CFG_TBL_UNKNOWN_RAM ((sizeof(bt_qsoc_cfg_tbl_unknown_ram))/(sizeof(bt_qsoc_cfg_cst_element_type*)))
static bt_qsoc_cfg_cst_element_type* bt_qsoc_cfg_tbl_unknown_ram[] = \
{
  NULL, /* CFG_TAG_37_UNKNOWN_RAM_INDEX */
  NULL, /* CFG_TAG_38_UNKNOWN_RAM_INDEX */ 
  NULL, /* CFG_TAG_97_UNKNOWN_RAM_INDEX */
  NULL  /* CFG_TAG_99_UNKNOWN_RAM_INDEX */
};


#define CFG_TAG_37_UNKNOWN_RAM_INDEX  (0)
#define CFG_TAG_38_UNKNOWN_RAM_INDEX  (1)
#define CFG_TAG_97_UNKNOWN_RAM_INDEX  (2)
#define CFG_TAG_99_UNKNOWN_RAM_INDEX  (3)

/* Customer specific configuration for BTS. Any cusomter specific values 
 * must be added here.
 */
const bt_qsoc_poke8_tbl_struct_type bt_qsoc_vs_poke8_tbl_r3[] =
{
  /* Patch R3: Rx Abort Disable for Test Modes (#1) 
    * In-band sleep poke #1 for SoC un-break refresh
    */
    {
      0x42, {BT_QSOC_VS_EDL_DNLOAD_REQ, 0x40, 0xD4, 0x7A, 0x00, 0x00, 
                           0x1F, 0x70, 0x03, 0x70, 0x81, 0x71, 0x03, 0x70, 
                           0x00, 0x00, 0x00, 0x00, 0xED, 0x6B, 0x03, 0x70, 
                           0x00, 0x00, 0x00, 0x00, 0x0B, 0x72, 0x03, 0x70, 
                           0x8F, 0x72, 0x03, 0x70, 0x01, 0x7B, 0x00, 0x00, 
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                           0x1D, 0x73, 0x03, 0x70, 0x01, 0x49, 0x88, 0x70, 
                           0x01, 0x49, 0x08, 0x47, 0xDC, 0x6C, 0x00, 0x00, 
                           0xD1, 0x72, 0x03, 0x70}
    },

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
   /* In-band sleep poke #2 for SoC un-break refresh 
   */
   {
      0x0A, {BT_QSOC_VS_EDL_DNLOAD_REQ, 0x08, 0x10, 0x03, 0x00, 0x00, 
                           0xD4, 0x7A, 0x00, 0x00}
   },
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

};
/*********************************************************************************************************/

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4020_R3_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4020 R3.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4020_R3_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_37_R3_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_R3;
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_97_R3_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_R3;
  }
  else
  {
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_37_R3_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_R3;
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_97_R3_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_R3;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_38_R3_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_R3;
  }
  else
  {
    bt_qsoc_cfg_tbl_r3_ram[CFG_TAG_38_R3_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_R3;
  } 

    runtime_params_ptr->nvm_fixed_table_ptr = &bt_qsoc_cfg_tbl_r3[0];
    runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_r3_ram[0];
    runtime_params_ptr->nvm_fixed_table_size = SIZEOF_BT_QSOC_CFG_TBL_R3;
    runtime_params_ptr->nvm_runtime_table_size = SIZEOF_BT_QSOC_CFG_TBL_R3_RAM;
}


/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4020BD_B0_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4020 BD B0.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4020BD_B0_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_37_4020BD_B0_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4020BD_B0;
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_97_4020BD_B0_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4020BD_B0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_37_4020BD_B0_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4020BD_B0;
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_97_4020BD_B0_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4020BD_B0;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_38_4020BD_B0_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4020BD_B0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_38_4020BD_B0_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4020BD_B0;
  } 

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_6_4020BD_B0_RAM_INDEX] = 
		                                  &bt_qsoc_cfg_tag_6_4020bd_b0_bt_2_1_not_supported[0];

	if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
	{
      bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_32_4020BD_B0_RAM_INDEX] = CFG_TAG_19P2_4020BD_B0_BT2_0;
	}

    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_72_4020BD_B0_RAM_INDEX] = CFG_TAG_72_4020_BD_B0_BT2_0;
	bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_78_4020BD_B0_RAM_INDEX] =
		                                  &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
  }
  else
  {
    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_6_4020BD_B0_RAM_INDEX] = 
		                                  &bt_qsoc_cfg_tag_6_4020bd_b0_bt_2_1_supported[0];

	if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
	{
      bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_32_4020BD_B0_RAM_INDEX] = CFG_TAG_19P2_4020BD_B0_BT2_1;
	}

    bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_72_4020BD_B0_RAM_INDEX] = CFG_TAG_72_4020_BD_B0_BT2_1;
	bt_qsoc_cfg_tbl_4020bd_b0_ram[CFG_TAG_78_4020BD_B0_RAM_INDEX]=
		                                  &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];
  }

  runtime_params_ptr->nvm_fixed_table_ptr =    &bt_qsoc_cfg_tbl_4020bd_b0[0];
  runtime_params_ptr->nvm_runtime_table_ptr =  &bt_qsoc_cfg_tbl_4020bd_b0_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =   SIZEOF_BT_QSOC_CFG_TBL_4020BD_B0;
  runtime_params_ptr->nvm_runtime_table_size = SIZEOF_BT_QSOC_CFG_TBL_4020BD_B0_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4020BD_B1_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4020 BD B1.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4020BD_B1_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_37_4020BD_B1_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4020BD_B1;
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_97_4020BD_B1_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4020BD_B1;
  }
  else
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_37_4020BD_B1_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4020BD_B1;
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_97_4020BD_B1_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4020BD_B1;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
	bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_38_4020BD_B1_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4020BD_B1;
  }
  else
  {
	bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_38_4020BD_B1_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4020BD_B1;
  } 

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_6_4020BD_B1_RAM_INDEX] = 
		                                  &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_72_4020BD_B1_RAM_INDEX] = CFG_TAG_72_4020_BD_B1_BT2_0;
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_78_4020BD_B1_RAM_INDEX] =
		                                  &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
  }
  else
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_6_4020BD_B1_RAM_INDEX] =
		                                  &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_72_4020BD_B1_RAM_INDEX] = CFG_TAG_72_4020_BD_B1_BT2_1;
	  bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_78_4020BD_B1_RAM_INDEX] =
		                                  &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];
  }
  
  if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_41_4020BD_B1_RAM_INDEX] = 
										CFG_TAG_41_19P2MHZ_4020_BD_B1;
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_99_4020BD_B1_RAM_INDEX] = 
										CFG_TAG_99_19P2MHZ_4020_BD_B1;
  }
  else 
  {
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_41_4020BD_B1_RAM_INDEX] = 
										CFG_TAG_41_32MHZ_4020_BD_B1;
    bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_99_4020BD_B1_RAM_INDEX] = 
										CFG_TAG_99_32MHZ_4020_BD_B1;
  }    
  
  /* update version tags */
  if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
  {
    if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
    {
      bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_32_4020BD_B1_RAM_INDEX] = 
        &bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b1_bt_2_0[0];;
    }
    else 
    {
      bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_32_4020BD_B1_RAM_INDEX] = 
        &bt_qsoc_cfg_tag_32_19p2MHz_4020bd_b1_bt_2_1[0];;
    }
  }
  else 
  {
    if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
    {
      bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_32_4020BD_B1_RAM_INDEX] = 
        &bt_qsoc_cfg_tag_32_32MHz_4020bd_b1_bt_2_0[0];;
    }
    else 
    {
      bt_qsoc_cfg_tbl_4020bd_b1_ram[CFG_TAG_32_4020BD_B1_RAM_INDEX] = 
        &bt_qsoc_cfg_tag_32_32MHz_4020bd_b1_bt_2_1[0];;
    }
  }
  
  runtime_params_ptr->nvm_fixed_table_ptr =    &bt_qsoc_cfg_tbl_4020bd_b1[0];
  runtime_params_ptr->nvm_runtime_table_ptr =  &bt_qsoc_cfg_tbl_4020bd_b1_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =   SIZEOF_BT_QSOC_CFG_TBL_4020BD_B1;
  runtime_params_ptr->nvm_runtime_table_size = SIZEOF_BT_QSOC_CFG_TBL_4020BD_B1_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4021_B1_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4021 B1.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4021_B1_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_37_4021_B1_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4021_B1;
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_97_4021_B1_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4021_B1;
  }
  else
  {
     bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_37_4021_B1_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4021_B1;
     bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_97_4021_B1_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4021_B1;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_38_4021_B1_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4021_B1;
  }
  else
  {
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_38_4021_B1_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4021_B1;
  }

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_6_4021_B1_RAM_INDEX] = 
		                                &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_78_4021_B1_RAM_INDEX] = 
		                                &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
  }
  else
  {
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_6_4021_B1_RAM_INDEX] = 
		                                &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];
    bt_qsoc_cfg_tbl_4021_b1_ram[CFG_TAG_78_4021_B1_RAM_INDEX] =
		                                &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];
  }

  runtime_params_ptr->nvm_fixed_table_ptr =    &bt_qsoc_cfg_tbl_4021_b1[0];
  runtime_params_ptr->nvm_runtime_table_ptr =  &bt_qsoc_cfg_tbl_4021_b1_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =   SIZEOF_BT_QSOC_CFG_TBL_4021_B1;
  runtime_params_ptr->nvm_runtime_table_size = SIZEOF_BT_QSOC_CFG_TBL_4021_B1_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4025_A0_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B0.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4025_A0_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
  {
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_32_4025_A0_RAM_INDEX] = 
										&bt_qsoc_cfg_tag_32_19p2MHz_4025_a0[0];
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_41_4025_A0_RAM_INDEX] = 
										CFG_TAG_41_19P2MHZ_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_99_4025_A0_RAM_INDEX] = 
										CFG_TAG_99_19P2MHZ_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_114_4025_A0_RAM_INDEX] = 
										CFG_TAG_114_19P2MHZ_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_115_4025_A0_RAM_INDEX] = 
										CFG_TAG_115_19P2MHZ_4025_A0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_32_4025_A0_RAM_INDEX] = 
										&bt_qsoc_cfg_tag_32_32MHz_4025_a0[0];
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_41_4025_A0_RAM_INDEX] = 
										CFG_TAG_41_32MHZ_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_99_4025_A0_RAM_INDEX] = 
										CFG_TAG_99_32MHZ_4025_A0;        
  }

  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_37_4025_A0_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_97_4025_A0_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4025_A0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_37_4025_A0_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4025_A0;
    bt_qsoc_cfg_tbl_4025_a0_ram[CFG_TAG_97_4025_A0_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4025_A0;
  }
	  
  runtime_params_ptr->nvm_fixed_table_ptr =   &bt_qsoc_cfg_tbl_4025_a0[0];
  runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_4025_a0_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =  SIZEOF_BT_QSOC_CFG_TBL_4025_A0;
  runtime_params_ptr->nvm_runtime_table_size =SIZEOF_BT_QSOC_CFG_TBL_4025_A0_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4025_B0_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B0.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4025_B0_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_37_4025_B0_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4025_B0;
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_97_4025_B0_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4025_B0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_37_4025_B0_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4025_B0;
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_97_4025_B0_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4025_B0;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_38_4025_B0_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4025_B0;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_38_4025_B0_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4025_B0;
  }

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_6_4025_B0_RAM_INDEX] = 
		                                &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
	bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_78_4025_B0_RAM_INDEX] =
		                                &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0]; 
  }
  else
  {
     bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_6_4025_B0_RAM_INDEX] = 
		                                 &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];
	 bt_qsoc_cfg_tbl_4025_b0_ram[CFG_TAG_78_4025_B0_RAM_INDEX] =
		                                 &bt_qsoc_cfg_tag_78_bt_2_1_supported[0]; 
  }

  runtime_params_ptr->nvm_fixed_table_ptr =    &bt_qsoc_cfg_tbl_4025_b0[0];
  runtime_params_ptr->nvm_runtime_table_ptr =  &bt_qsoc_cfg_tbl_4025_b0_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =   SIZEOF_BT_QSOC_CFG_TBL_4025_B0;
  runtime_params_ptr->nvm_runtime_table_size = SIZEOF_BT_QSOC_CFG_TBL_4025_B0_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4025_B1_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B1.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4025_B1_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_37_4025_B1_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4025_B1;
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_97_4025_B1_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4025_B1;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_37_4025_B1_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4025_B1;
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_97_4025_B1_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4025_B1;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_38_4025_B1_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4025_B1;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_38_4025_B1_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4025_B1;
  }

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_6_4025_B1_RAM_INDEX] =
		                        &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_78_4025_B1_RAM_INDEX] =
                                        &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_72_4025_B1_RAM_INDEX] = CFG_TAG_72_4025_B1;

    if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
    {
      bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b1_bt_2_0[0];
    }
    else 
    {
      bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b1_bt_2_0[0];
    }    
   }
   else
   {
     bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_6_4025_B1_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];

     bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_78_4025_B1_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];

     if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
     {
       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b1_bt_2_1[0];
     }
     else
     {
       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b1_bt_2_1[0];
     }	    
		                                        
   }

#ifdef FEATURE_BT_QSOC_CLASS2   
     if(runtime_params_ptr->bt_qsoc_device_class == BT_QSOC_DEV_CLASS2)
     {	     
       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_36_4025_B1_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS2[0];

       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_56_4025_B1_RAM_INDEX] =
          &bt_qsoc_cfg_tag_56_4025_CLASS2[0];

       /* Reassigning tag 32 for class 2 device */
       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_41_4025_B1_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS2[0]; 

         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_19P2MHz_4025_b1_CLASS2[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_41_4025_B1_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS2[0];		   

         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_32_4025_B1_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_32_32MHz_4025_b1_CLASS2[0];		   
       }
     }
     else
#endif /* FEATURE_BT_QSOC_CLASS2 */	     
     {	     
       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_36_4025_B1_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS1[0];   
       bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_56_4025_B1_RAM_INDEX] =
          &bt_qsoc_cfg_tag_56_4025_CLASS1[0];    

       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_41_4025_B1_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS1[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b1_ram[CFG_TAG_41_4025_B1_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS1[0];		   
       }
     }
     
   runtime_params_ptr->nvm_fixed_table_ptr =   &bt_qsoc_cfg_tbl_4025_b1[0];
   runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_4025_b1_ram[0];
   runtime_params_ptr->nvm_fixed_table_size =  SIZEOF_BT_QSOC_CFG_TBL_4025_B1;
   runtime_params_ptr->nvm_runtime_table_size =SIZEOF_BT_QSOC_CFG_TBL_4025_B1_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4025_B2_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B2.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4025_B2_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_37_4025_B2_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4025_B2;
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_97_4025_B2_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4025_B2;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_37_4025_B2_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4025_B2;
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_97_4025_B2_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4025_B2;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_38_4025_B2_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4025_B2;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_38_4025_B2_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4025_B2;
  }

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_6_4025_B2_RAM_INDEX] =
		                        &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_78_4025_B2_RAM_INDEX] =
                                        &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_72_4025_B2_RAM_INDEX] = CFG_TAG_72_4025_B2;

    if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
    {
      bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b2_bt_2_0[0];
    }
    else 
    {
      bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b2_bt_2_0[0];
    }    
   }
   else
   {
     bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_6_4025_B2_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];

     bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_78_4025_B2_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];

     if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
     {
       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b2_bt_2_1[0];
     }
     else
     {
       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b2_bt_2_1[0];
     }	    
		                                        
   }

#ifdef FEATURE_BT_QSOC_CLASS2   
     if(runtime_params_ptr->bt_qsoc_device_class == BT_QSOC_DEV_CLASS2)
     {	     
       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_36_4025_B2_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS2[0];

       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_56_4025_B2_RAM_INDEX] =
          &bt_qsoc_cfg_tag_56_4025_CLASS2[0];

       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_41_4025_B2_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS2[0]; 

         /* Reassigning tag 32 for class 2 device */
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_19P2MHz_4025_b2_CLASS2[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_41_4025_B2_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS2[0];		   

         /* Reassigning tag 32 for class 2 device */
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_32_4025_B2_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_32_32MHz_4025_b2_CLASS2[0];		   
       }
     }
     else
#endif /* FEATURE_BT_QSOC_CLASS2 */	     
     {	     
       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_36_4025_B2_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS1[0];       
       bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_56_4025_B2_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_56_4025_CLASS1[0];

       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_41_4025_B2_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS1[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b2_ram[CFG_TAG_41_4025_B2_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS1[0];		   
       }
     }

   runtime_params_ptr->nvm_fixed_table_ptr =   &bt_qsoc_cfg_tbl_4025_b2[0];
   runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_4025_b2_ram[0];
   runtime_params_ptr->nvm_fixed_table_size =  SIZEOF_BT_QSOC_CFG_TBL_4025_B2;
   runtime_params_ptr->nvm_runtime_table_size =SIZEOF_BT_QSOC_CFG_TBL_4025_B2_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_4025_B3_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B3.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_4025_B3_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_37_4025_B3_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_4025_B3;
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_97_4025_B3_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_4025_B3;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_37_4025_B3_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_4025_B3;
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_97_4025_B3_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_4025_B3;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_38_4025_B3_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_4025_B3;
  }
  else
  {
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_38_4025_B3_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_4025_B3;
  }

  if(runtime_params_ptr->bt_2_1_lisbon_disabled == 1)
  {
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_6_4025_B3_RAM_INDEX] =
		                        &bt_qsoc_cfg_tag_6_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_78_4025_B3_RAM_INDEX] =
                                        &bt_qsoc_cfg_tag_78_bt_2_1_not_supported[0];
    bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_72_4025_B3_RAM_INDEX] = CFG_TAG_72_4025_B3;

    if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
    {
      bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b3_bt_2_0[0];
    }
    else 
    {
      bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b3_bt_2_0[0];
    }    
   }
   else
   {
     bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_6_4025_B3_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_6_bt_2_1_supported[0];

     bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_78_4025_B3_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_78_bt_2_1_supported[0];

     if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
     {
       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_32_19p2MHz_4025_b3_bt_2_1[0];
     }
     else
     {
       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_32MHz_4025_b3_bt_2_1[0];
     }	    
		                                        
   }

#ifdef FEATURE_BT_QSOC_CLASS2   
     if(runtime_params_ptr->bt_qsoc_device_class == BT_QSOC_DEV_CLASS2)
     {	     
       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_36_4025_B3_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS2[0];

       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_56_4025_B3_RAM_INDEX] =
          &bt_qsoc_cfg_tag_56_4025_CLASS2[0];

       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_41_4025_B3_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS2[0]; 

         /* Reassigning tag 32 for class 2 device */
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
		                          &bt_qsoc_cfg_tag_32_19P2MHz_4025_b3_CLASS2[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_41_4025_B3_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS2[0];		   

         /* Reassigning tag 32 for class 2 device */
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_32_4025_B3_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_32_32MHz_4025_b3_CLASS2[0];		   
       }
     }
     else
#endif /* FEATURE_BT_QSOC_CLASS2 */	     
     {	     
       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_36_4025_B3_RAM_INDEX] =
	                                  &bt_qsoc_cfg_tag_36_4025_CLASS1[0];       
       bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_56_4025_B3_RAM_INDEX] =
                                          &bt_qsoc_cfg_tag_56_4025_CLASS1[0];

       if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_41_4025_B3_RAM_INDEX] =
            &bt_qsoc_cfg_tag_41_19P2MHz_4025_CLASS1[0]; 
       }
       else
       {
         bt_qsoc_cfg_tbl_4025_b3_ram[CFG_TAG_41_4025_B3_RAM_INDEX] =
           &bt_qsoc_cfg_tag_41_32MHz_4025_CLASS1[0];		   
       }
     }

   runtime_params_ptr->nvm_fixed_table_ptr =   &bt_qsoc_cfg_tbl_4025_b3[0];
   runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_4025_b3_ram[0];
   runtime_params_ptr->nvm_fixed_table_size =  SIZEOF_BT_QSOC_CFG_TBL_4025_B3;
   runtime_params_ptr->nvm_runtime_table_size =SIZEOF_BT_QSOC_CFG_TBL_4025_B3_RAM;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_unknown_nvm_entries 

  DESCRIPTION    This routine updates the run time nvm parameters for 4025 B1.

  DEPENDENCIES   None.

  PARAMETERS     runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   NONE.

  SIDE EFFECTS   None.

==========================================================================*/

static void bt_qsoc_nvm_vs_update_unknown_nvm_entries
(
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if(runtime_params_ptr->refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_99_UNKNOWN_RAM_INDEX] = CFG_TAG_99_19P2MHZ_UNKNOWN;
  }
  else
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_99_UNKNOWN_RAM_INDEX] = CFG_TAG_99_32MHZ_UNKNOWN;
  }

  if(runtime_params_ptr->clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED)
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_37_UNKNOWN_RAM_INDEX] = CFG_TAG_37_CLK_SHARING_UNKNOWN;
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_97_UNKNOWN_RAM_INDEX] = CFG_TAG_97_CLK_SHARING_UNKNOWN;
  }
  else
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_37_UNKNOWN_RAM_INDEX] = CFG_TAG_37_NO_CLK_SHARING_UNKNOWN;
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_97_UNKNOWN_RAM_INDEX] = CFG_TAG_97_NO_CLK_SHARING_UNKNOWN;
  }

  if(runtime_params_ptr->soc_logging == 1)
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_38_UNKNOWN_RAM_INDEX] = CFG_TAG_38_SOC_LOGGING_UNKNOWN;
  }
  else
  {
    bt_qsoc_cfg_tbl_unknown_ram[CFG_TAG_38_UNKNOWN_RAM_INDEX] = CFG_TAG_38_SOC_NO_LOGGING_UNKNOWN;
  }

  runtime_params_ptr->nvm_fixed_table_ptr =   &bt_qsoc_cfg_tbl_unknown[0];
  runtime_params_ptr->nvm_runtime_table_ptr = &bt_qsoc_cfg_tbl_unknown_ram[0];
  runtime_params_ptr->nvm_fixed_table_size =  SIZEOF_BT_QSOC_CFG_TBL_UNKNOWN;
  runtime_params_ptr->nvm_runtime_table_size =SIZEOF_BT_QSOC_CFG_TBL_UNKNOWN_RAM;
}
/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_update_nvm_entries

  DESCRIPTION    This routine updates the run time nvm parameters.

  DEPENDENCIES   None.

  PARAMETERS     soc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   TRUE (NVM init success) or FALSE (NVM init failure),
                 Pointer to the nvm table.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_update_nvm_entries
(
bt_qsoc_enum_type soc_type,
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  boolean returnValue = FALSE;

  if(runtime_params_ptr == NULL)
  {
    return returnValue;
  }

  /* Copy the BD Address into the RAM pointed by the table pointer */
  (void)memcpy( &(bt_qsoc_cfg_tag_2_ram[4]), 
                (const OI_UINT8 *)(&(runtime_params_ptr->bd_address[0])),
                BT_QSOC_MAX_BD_ADDRESS_SIZE );

  switch(soc_type)
  {
  case BT_QSOC_R3:
    {
      bt_qsoc_nvm_vs_update_4020_R3_nvm_entries(runtime_params_ptr);
      returnValue = TRUE;
      break;
    }

  case BT_QSOC_4020BD_B0:
    {
      bt_qsoc_nvm_vs_update_4020BD_B0_nvm_entries(runtime_params_ptr);
      returnValue = TRUE;
      break;
    }

  case BT_QSOC_4020BD_B1:
    {
      bt_qsoc_nvm_vs_update_4020BD_B1_nvm_entries(runtime_params_ptr);
      returnValue = TRUE;
      break;
    }

  case BT_QSOC_4021_B1:
    {
       bt_qsoc_nvm_vs_update_4021_B1_nvm_entries(runtime_params_ptr);
       returnValue = TRUE;
       break;
    }

  case BT_QSOC_4025_A0:
	{
	  bt_qsoc_nvm_vs_update_4025_A0_nvm_entries(runtime_params_ptr);
	  returnValue = TRUE;
	  break;
	}

  case BT_QSOC_4025_B0:
	{
      bt_qsoc_nvm_vs_update_4025_B0_nvm_entries(runtime_params_ptr);
	  returnValue = TRUE;
	  break;
	}

  case BT_QSOC_4025_B1:
	{
	  bt_qsoc_nvm_vs_update_4025_B1_nvm_entries(runtime_params_ptr);
	  returnValue = TRUE;
	  break;
	}

  case BT_QSOC_4025_B2:
	{
	  bt_qsoc_nvm_vs_update_4025_B2_nvm_entries(runtime_params_ptr);
	  returnValue = TRUE;
	  break;
	}

  case BT_QSOC_4025_B3:
	{
	  bt_qsoc_nvm_vs_update_4025_B3_nvm_entries(runtime_params_ptr);
	  returnValue = TRUE;
	  break;
	}

  case BT_QSOC_R3BD:	
  case BT_QSOC_R4:	
  case BT_QSOC_R2B:
  case BT_QSOC_R2C:
    {
      /* Un supported BTS versions */
      runtime_params_ptr->nvm_fixed_table_ptr = NULL;
      returnValue = FALSE;
      break;
    }
  default:
    /* Unknown BTS versions */
    {
      bt_qsoc_nvm_vs_update_unknown_nvm_entries(runtime_params_ptr);
      returnValue = TRUE;
      break;
    }
  }
  return returnValue;
} /* bt_qsoc_nvm_vs_update_nvm_entries */



/*==========================================================================

  FUNCTION       bt_qsoc_nvm_getnext_n_char

  DESCRIPTION    Reads n char into 'dest_buf'

                 The bytes read from the file system are placed in 'buf'

                 Whenever this function is called, it copies n bytes
                 from 'buf' to 'dest_buf'. If there are not enough bytes
                 in 'buf', it reads from the file system and populates 'buf'

                 'bt_soc_r_ind' keeps track of the current read position in
                 'buf'

                 It assumes that there are enough bytes in the file
                 to service this call.
==========================================================================*/

/* Assume the buffer is filled the first time */
static boolean bt_qsoc_nvm_getnext_n_char(unsigned char *dest_buf, OI_UINT16 len)
{   
  OI_UINT16 i = 0;

  if(btqsocnvm_parser_error)
  { 
    return FALSE;
  }

  if(len + bt_soc_r_ind > buf_len)
  {
	  return FALSE;
  }

  for ( ; (i<len) && (bt_soc_r_ind<=buf_len);i++,bt_soc_r_ind++ )
  {
     dest_buf[i] = buf[bt_soc_r_ind];
  }

  return TRUE;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_skip_line

  DESCRIPTION    Reads/Moves till the end of line
  
==========================================================================*/
static void bt_qsoc_nvm_skip_line()
{
  unsigned char c;
  bt_qsoc_nvm_getnextchar(&c);
  while ( (c != ';') && (!btqsocnvm_parser_error) )
  {
    bt_qsoc_nvm_getnextchar(&c);
  }
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_getnext_number

  DESCRIPTION    Extracts the next 5 (at the most) digit number and the 
                 following space
  
==========================================================================*/
static OI_UINT16 bt_qsoc_nvm_getnext_number()
{
  OI_UINT16 num = 0;
  OI_UINT16 len = 0;
  
  bt_qsoc_nvm_getnextchar(&dest[len]);

  while ( isdigit(dest[len]) && (!btqsocnvm_parser_error) && (len < 5) )
  {
    bt_qsoc_nvm_getnextchar(&dest[++len]);
  }

  if(len >= 5)
  {
    btqsocnvm_parser_error = BTQSOCNVM_NUMBER_FORMAT_ERR;
    btqsocnvmplatform_log_error(btqsocnvm_parser_error);
  }

  if(!btqsocnvm_parser_error)
  {
    dest[len]= '\0';
    num = atoi((const char *)dest);
    return(num);
  }
  else
  {
    return 0;
  }
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_xtoi

  DESCRIPTION    Converts hex to decimal
  
==========================================================================*/
static unsigned char bt_qsoc_nvm_xtoi(unsigned char c)
{
  return(((c > '9')? (tolower(c) - 87):(c-48)));
}

/*==========================================================================

  FUNCTION       getnext_hexnumber

  DESCRIPTION    Extracts the next two digit hex value and the following space
  
==========================================================================*/
static OI_UINT8 bt_qsoc_nvm_getnext_hexnumber()
{
  OI_UINT8 num = 0;
  
  bt_qsoc_nvm_getnext_n_char(&dest[0], 3);

  if( (isxdigit(dest[0])) && (isxdigit(dest[1])) )
  {
    num = bt_qsoc_nvm_xtoi(dest[0]);
    num <<= 4;
    num = num | bt_qsoc_nvm_xtoi(dest[1]);
    return(num);
  }

  if(!btqsocnvm_parser_error)
  {
    /* Not a file read error? */
    btqsocnvm_parser_error = BTQSOCNVM_NUMBER_FORMAT_ERR;
    btqsocnvmplatform_log_error(btqsocnvm_parser_error);
  }
  return 0;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_tag_to_be_sent_to_SoC

  DESCRIPTION    This function the released_nvm_tag for the given tag
  
==========================================================================*/
static OI_UINT8 bt_qsoc_nvm_tag_to_be_sent_to_SoC(OI_UINT8 tag_num_to_find)
{
  if (rel_nvm[tag_num_to_find] == 1 ) /* look is much faster*/
  { 
    return TRUE; 
  }   
  return FALSE;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_strlen

  DESCRIPTION    Converts hex to decimal
  
==========================================================================*/
static OI_UINT16 bt_qsoc_nvm_strlen(const OI_UINT8 *buf_ptr)
{
  OI_UINT16 count = 0;
  while(*buf_ptr)
  { 
     count++;
     buf_ptr++;
  }
  return count;
}


/*==========================================================================

  FUNCTION       bt_qsoc_read_nvm_tags_from_header

  DESCRIPTION    This function checks if NVM buffer is available for qsoc type
                 assign to a buf pointer or returns an error.
  
==========================================================================*/
boolean bt_qsoc_read_nvm_tags_from_header 
(
bt_qsoc_enum_type qsoc_type,
bt_soc_refclock_type bt_refclock_type
)
{
   boolean return_status = TRUE;

   switch(qsoc_type)
   {
       case BT_QSOC_R3:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_R3_19P2Mhz; 
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_R3_32Mhz;
       }       
       break;

       case BT_QSOC_4020BD_B0:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ) 
       {  
	  buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_BDB0_19P2Mhz;
       }
       else
       {
	  buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_BDB0_32Mhz; 
       }
       break;

       case BT_QSOC_4020BD_B1:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ) 
       {  
	  buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_BDB1_19P2Mhz;
       }
       else
       {
	  buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4020_BDB1_32Mhz;
       }
       break;

       case BT_QSOC_4021_B1:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4021_B1_19P2Mhz;
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4021_B1_32Mhz;
       }
       break;

       case BT_QSOC_4025_B0:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B0_19P2Mhz;
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B0_32Mhz;
       }
       break;

       case BT_QSOC_4025_B1:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B1_19P2Mhz;
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B1_32Mhz;
       }
       break;

       case BT_QSOC_4025_B2:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B2_19P2Mhz;
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B2_32Mhz;
       }
       break;

       case BT_QSOC_4025_B3:
       if(bt_refclock_type == BT_SOC_REFCLOCK_19P2MHZ)
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B3_19P2Mhz;
       }
       else
       {
          buf = (OI_UINT8 *) bt_qsoc_nvm_BTS4025_B3_32Mhz;
       }
       break;

	   default:
       return_status = FALSE;
       return return_status;
   }

   buf_len = bt_qsoc_nvm_strlen(buf);

   if(buf_len == 1)
   {
       return_status = FALSE;
   }
   return return_status;
}

/*==========================================================================

  FUNCTION       bt_qsoc_populate_rel_nvm

  DESCRIPTION    This function finds the pointer to released_nvm_tags
                 and its length for the given SoC
  
==========================================================================*/
static boolean bt_qsoc_populate_rel_nvm
(
bt_qsoc_enum_type qsoc_type, 
bt_soc_refclock_type ref_clock
)
{
  int i = 0;
  boolean return_status = FALSE;

  /* reset the rel_nvm array */
  for(i = 0 ; i < REL_NVM_MAX_TAG_NO; i++)
  {
    rel_nvm[i] = 0;
  }

  if( qsoc_type == BT_QSOC_R3 )
  {
    rel_nvm[28]= rel_nvm[32]= rel_nvm[36]= rel_nvm[39]= rel_nvm[40]= 
    rel_nvm[41]= rel_nvm[42]= rel_nvm[62]= rel_nvm[63]= rel_nvm[64]= 
    rel_nvm[65]= rel_nvm[83]= rel_nvm[84]= rel_nvm[85]= rel_nvm[86]= 
    rel_nvm[87]= rel_nvm[88]= rel_nvm[89]= rel_nvm[90]= rel_nvm[91]= 
    rel_nvm[92]= rel_nvm[93]= rel_nvm[94]= rel_nvm[99]= 1; 
    return_status = TRUE; 
  }

  if( qsoc_type == BT_QSOC_4021_B1 )
  {
    rel_nvm[28]= rel_nvm[36]= rel_nvm[40]= rel_nvm[42]= rel_nvm[72]= 
    rel_nvm[100]= rel_nvm[103]= rel_nvm[104]= 
    rel_nvm[107]= rel_nvm[108]= rel_nvm[109]= rel_nvm[110]= rel_nvm[111]=
    rel_nvm[112]= rel_nvm[113]= rel_nvm[114]= rel_nvm[115]= rel_nvm[116]= 
    rel_nvm[119]= rel_nvm[32]= rel_nvm[39]= rel_nvm[99]= rel_nvm[41] = 1;

    if(ref_clock == BT_SOC_REFCLOCK_19P2MHZ)
    {
      rel_nvm[105]= rel_nvm[106]= 1;
    }

    
    return_status = TRUE;
  }

  if( qsoc_type == BT_QSOC_4020BD_B0 )
  { 
    rel_nvm[36]= rel_nvm[39]= rel_nvm[40]= rel_nvm[42]= 
    rel_nvm[100]= rel_nvm[101]= rel_nvm[102]= rel_nvm[103]= rel_nvm[104]= 
    rel_nvm[105]= rel_nvm[109]= rel_nvm[110]= rel_nvm[111]= rel_nvm[112]= 
    rel_nvm[113]= rel_nvm[114]= rel_nvm[116]= rel_nvm[119]=
    rel_nvm[41]= rel_nvm[99] = rel_nvm[28] = 1; 

    if(ref_clock == BT_SOC_REFCLOCK_19P2MHZ)
    {
      rel_nvm[115] = 1;
    }
    return_status = TRUE;
  }

  if(  qsoc_type == BT_QSOC_4020BD_B1 )
  {
    rel_nvm[28]= rel_nvm[36]= rel_nvm[39]= rel_nvm[40]= rel_nvm[42]=
    rel_nvm[100]= rel_nvm[101]= rel_nvm[102]= rel_nvm[103]= rel_nvm[104]= 
    rel_nvm[105]= rel_nvm[106]= rel_nvm[107]= rel_nvm[108]= rel_nvm[109]= 
    rel_nvm[110]= rel_nvm[111]= rel_nvm[112]= rel_nvm[113]= rel_nvm[114]= 
    rel_nvm[115]= rel_nvm[116]= rel_nvm[119]= 1;
  
    return_status = TRUE;    
  }

  if( qsoc_type == BT_QSOC_4025_B0)
  {
    rel_nvm[28]= rel_nvm[32]= rel_nvm[36]= rel_nvm[39]= rel_nvm[41]= rel_nvm[62]=
    rel_nvm[99]= rel_nvm[116]= rel_nvm[119]= rel_nvm[120]= rel_nvm[121]= 
    rel_nvm[123]= 1;

    return_status = TRUE; 
  }

  if( qsoc_type == BT_QSOC_4025_B1)
  {
    rel_nvm[28]= rel_nvm[39]= rel_nvm[40]= rel_nvm[42]= 
    rel_nvm[95]= rel_nvm[99]= rel_nvm[100]= rel_nvm[101]= rel_nvm[102]=  
    rel_nvm[103] = rel_nvm[104] = rel_nvm[105]= rel_nvm[106]= rel_nvm[107]= 
    rel_nvm[108]= rel_nvm[109]= rel_nvm[110]= rel_nvm[111]= rel_nvm[112]= 
    rel_nvm[113]= rel_nvm[114]= rel_nvm[115]= rel_nvm[116]= rel_nvm[119]= 
    rel_nvm[120]= rel_nvm[121]= rel_nvm[123]= rel_nvm[126]= 1;
 
    return_status = TRUE; 
  }  

  if( qsoc_type == BT_QSOC_4025_B2)
  {
    rel_nvm[28]= rel_nvm[39]= rel_nvm[40]= rel_nvm[42]= rel_nvm[95]= 
    rel_nvm[99]= rel_nvm[100]= rel_nvm[101]= rel_nvm[102]= rel_nvm[103]= 
    rel_nvm[104]= rel_nvm[105]= rel_nvm[106]= rel_nvm[107]= rel_nvm[108]= 
    rel_nvm[109]= rel_nvm[110]= rel_nvm[111]= rel_nvm[112]= rel_nvm[113]= 
    rel_nvm[114]= rel_nvm[115]= rel_nvm[116]= rel_nvm[119]= rel_nvm[120]= 
    rel_nvm[121]= rel_nvm[123]= rel_nvm[126]= 1;

    return_status = TRUE; 
  }  

  if( qsoc_type == BT_QSOC_4025_B3)
  {
    rel_nvm[28]= rel_nvm[39]= rel_nvm[40]= rel_nvm[42]= rel_nvm[95]= 
    rel_nvm[116]= rel_nvm[119]= rel_nvm[120]= rel_nvm[121]= rel_nvm[123]=
	rel_nvm[126]= 1;

    return_status = TRUE; 
  }  

  if(return_status == FALSE)
  {
    btqsocnvm_parser_error = BTQSOCNVM_QSOC_NOT_SUPPORTED;
    btqsocnvmplatform_log_error(btqsocnvm_parser_error);
  }
  return return_status;

}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_parse_supported

  DESCRIPTION    This queries whether dynamic parsing is supported for a 
                 particular SoC at the given clock speed.

  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If initialized successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_parse_supported
(
bt_qsoc_enum_type qsoc_type, 
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  if ( runtime_params_ptr == NULL ) return FALSE;
  return(bt_qsoc_populate_rel_nvm (qsoc_type, 
                                   runtime_params_ptr->refclock_type));
                         
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_extract_nvm

  DESCRIPTION    This function extract the next tag.
  
==========================================================================*/
static boolean bt_qsoc_nvm_vs_extract_nvm
(
  OI_UINT8** soc_nvm_ptr,
  OI_UINT8* parser_error
)
{
  OI_UINT8 tag_length = 0;
  OI_UINT8 tag_number = 0;
  OI_UINT16 j;

  bt_qsoc_parser.current_tag_num++;
  *parser_error = btqsocnvm_parser_error;
  if ( (bt_qsoc_parser.current_tag_num > bt_qsoc_parser.total_num_nvm_tags) 
       || (!nv_parser_state) 
       || btqsocnvm_parser_error )
  {
    return FALSE;
  }

  bt_qsoc_nvm_skip_line(); 
  bt_qsoc_nvm_skip_line(); /* Skip [Tag<no>] line */
  bt_qsoc_nvm_getnext_n_char(dest,9); /* Get/Skip the "TagNumber = " string */
  tag_number = (OI_UINT8)bt_qsoc_nvm_getnext_number();
  bt_qsoc_nvm_skip_line();

  if ( bt_qsoc_nvm_tag_to_be_sent_to_SoC(tag_number) == FALSE )
  {
    /* Set the tag pointer to NULL */
    (*soc_nvm_ptr) = NULL;
    
    /* Skip the lines */
    bt_qsoc_nvm_skip_line();
    bt_qsoc_nvm_skip_line();
    *parser_error = btqsocnvm_parser_error;
    if(btqsocnvm_parser_error)
    { 
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  
  bt_qsoc_nvm_getnext_n_char(dest,12); /* Get/Skip the "TagLength = " string */
  tag_length = (OI_UINT8)bt_qsoc_nvm_getnext_number();
  bt_qsoc_nvm_skip_line();

  bt_qsoc_nvm_getnext_n_char(dest,11); /* Get/Skip the "TagValue = " */

  /* Build the tag */
  soc_tag[3] = tag_length;
  soc_tag[2] = tag_number;
  soc_tag[1] = BT_QSOC_TAG_WRITE;   
  soc_tag[0] = tag_length + 3;

  for ( j = 0; j < tag_length; j++ ) 
  {
    soc_tag[j+4] = bt_qsoc_nvm_getnext_hexnumber();
  }

  bt_qsoc_nvm_skip_line(); /* get/skip to next line */

  *parser_error = btqsocnvm_parser_error;
  if(btqsocnvm_parser_error)
  { 
    return FALSE;
  }
  else
  {
    (*soc_nvm_ptr) = (OI_UINT8 *) &soc_tag[0];
    return TRUE;
  }
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_get_next_released_nvm

  DESCRIPTION    This gives the next tag which needs to be sent to SoC.
  
  DEPENDENCIES   bt_qsoc_nvm_vs_init_released_nvm must be successful.

  PARAMETERS     soc_nvm_ptr (out): pointer to tag, if return value is TRUE.

  RETURN VALUE   boolean: True: If tag extracted. 
                          False: If no more tags in nvm file

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_get_next_released_nvm
(
  OI_UINT8** soc_nvm_ptr,
  OI_UINT8* parser_error
)
{
  *soc_nvm_ptr = NULL;

  if( (nv_parser_state == FALSE) || (btqsocnvm_parser_error) )
  {  return FALSE; }
  while ( bt_qsoc_nvm_vs_extract_nvm(soc_nvm_ptr, parser_error) == TRUE )
  {
    if ( *soc_nvm_ptr != NULL )
     {
       return TRUE;
     }
  }
  return FALSE;
}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_close_released_nvm

  DESCRIPTION    This resets the state of parser
  
  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If closed successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_close_released_nvm()
{
  /* Close file handle, deallocate any dynamic memory */
  nv_parser_state = FALSE;
  bt_soc_r_ind = 0;
  return TRUE;

}

/*==========================================================================

  FUNCTION       bt_qsoc_nvm_vs_init_released_nvm

  DESCRIPTION    This routine initializes the parser for nvm file.

  DEPENDENCIES   None.

  PARAMETERS     qsoc_type: SOC version
                 runtime_params_ptr: Parameters that determine the run time 
                 configuration

  RETURN VALUE   boolean: True: If initialized successfully else false.

  SIDE EFFECTS   None.

==========================================================================*/
boolean bt_qsoc_nvm_vs_init_released_nvm
(
bt_qsoc_enum_type qsoc_type, 
bt_qsoc_runtime_params_struct_type *runtime_params_ptr
)
{
  boolean done = FALSE;

  /* Reset the globals */
  bt_qsoc_parser.total_num_nvm_tags = 0;
  bt_qsoc_parser.current_tag_num = 0;
  bt_soc_r_ind = 0;

  /* NVMFile should not be open */
  if ( (nv_parser_state == TRUE) || (btqsocnvm_parser_error))
  { 
    return FALSE; 
  }

  if ( runtime_params_ptr == NULL ) 
  { 
    return FALSE; 
  }

  if ( !bt_qsoc_populate_rel_nvm(qsoc_type, 
                                      runtime_params_ptr->refclock_type))
  {
    return FALSE;
  }
  while ( (!done) && (!btqsocnvm_parser_error) )
  {
    bt_qsoc_nvm_getnextchar(dest);
    if ( dest[0] == '#' )
    {
      /* Skip the comment line */
      bt_qsoc_nvm_skip_line();
    }
    if ( dest[0]=='[' )
    {
      /* Read [Tag]<EOL character> */
      //bt_qsoc_nvm_getnext_n_char(&dest[1],6); /* Read CR and LF also */

      bt_qsoc_nvm_getnext_n_char(&dest[1],4); /* DONT Read CR and LF also */
      bt_qsoc_nvm_skip_line();
      dest[5] = '\0';
      if ( strcmp((const char *)dest, "[Tag]") == 0 )
      {
        bt_qsoc_nvm_getnext_n_char(dest,6);

        /* get no of tags */
        bt_qsoc_parser.total_num_nvm_tags = bt_qsoc_nvm_getnext_number();
        bt_qsoc_nvm_skip_line();
        done = TRUE;
      }
      else
      {
        // Not a [Tag], maybe [General]
      }
    }
  }
  if(btqsocnvm_parser_error)
  { 
    btqsocnvmplatform_log_error(btqsocnvm_parser_error);
    nv_parser_state = FALSE;
    return FALSE;
  }

  nv_parser_state = TRUE;
  return TRUE;
}

