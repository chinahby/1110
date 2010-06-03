/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  V I R T U A L   U I M   S E R V E R

GENERAL DESCRIPTION
  This is an emulator that replaces the real UIM Server.   This emulatorf
  accesses RAM as opposed to the SIM to emulate SIM or USIM access.

  This Virtual UIM Server is activated by the feature FEATURE_GSDI_UIM_SERVER.
  By default, the Virtual UIM Server emulates a GSM SIM.  To make the server
  emulate a USIM, add FEATURE_GSDI_UIM_SERVER_USIM to custsurf.h and rebuild.
  Also, by default, user PIN1 is disabled.  To enable PIN1, add
  FEATURE_GSDI_UIM_SERVER_PIN_ENABLED to custsurf.h and rebuild.

  The data supports both GSM SIM and 3G USIM data. Two pools of data exists.

  One is for EF data of a select number of EFs.
  uim_sim[UIM_NUM_FILES]

  The other is for EF header information of a select number of EFs.
  gsdi_sim_file_attr[UIM_NUM_FILE_ATTR]
   Note: GSM SIM EFs and 3G EFs have different header formats.


EXTERNALIZED FUNCTIONS
  test_sim_server_task - Main control task for the Virtual UIM Server.

  virtuim_cmd - Utility to place a command onto the Virtual UIM Server
                queue.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM5200/gsdi/vcs/virtuim.c_v   1.13   15 May 2002 08:16:08   jruvalca  $
$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/virtuim.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/2/07    js      Fixed revision update header
08/01/07   sun     Changed the CSKN value in KCGPRS to 00
07/26/07   sun     Check for Data Ptr only for Relevant commands
05/07/07   sp      Replaced mem_malloc/fre with mmgsdi malloc/free
08/20/06   tml     Added Multi UICC App support
07/07/06   sun     Fixed Record Number
06/24/06   jar     Added support for BDN Files for Single Slot.
06/02/06   jar     Added support for UIM_CACHED_SEL to keep the MMGSDI Task
                   from hanging.
03/21/06   tml     check for aid
03/01/06   sp      Fixed compiler error
01/12/06   tml     Fixed alignment issue
01/05/06   tml     Added support for dual slot and more ADN
12/14/05   tml     MMGSDI Cleanup
07/12/05   tml     Read/Write/Inc new command supports
06/10/05   tml     Lint fixes
05/11/05   sst     Lint fixes
03/24/05   tml     Added JSR test supports and USIM support
02/18/05   bw      Initialize timer needed for LTK
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
11/29/04   tml     corrected ECC data
10/27/04   sk      Changed data types from byte to uint32/int32 in
                   uim_server_read_sim and uim_server_read_attr.
08/11/04   sk      Added files for FEATURE_MMGSDI_CPHS, fixed compile warnings
                   fixed incorrect parameters passed to UIM_MC_CLOSE
08/02/04   sk      Added files, fixed Link Establish Message to work with
                   GSDI, fixed warnings due to uninitialized data, fixed
                   errors due to missing files.
06/02/04   tml     Added virtual sat additional support
05/17/04 jar/tml   Added support for channel management
05/04/04   tml     Compilation fix
04/15/04   jar     Virtual SIM Fix for Select DF.
04/12/04   tml     Read, Search additional support
03/31/04   tml     code clean up
03/08/04   tml     Added Support for Unit Testing
02/21/04   jar     Added Support for API Testers.
01/21/04   jar     Featurized UIM_SLOT_CHANGE_F and UIM_SLOT_CHANGE_R under
                   FEATURE_MMGSDI_DUAL_SLOT for Single Slot Builds.
11/05/03   jar     Added handling for UIM_PREF_SLOT_CHANGE_F
08/25/03   jar     Added FEATURE_MMGSDI_DUAL_SLOT
07/11/03   jar     Updated FEATURE Defines.  Added support for Dual Slot Support
                   FEATURE_MMGSDI_DUAL_SLOT
05/19/03   jar     Lint Error Fixes
05/13/03   tml     Added CDMA SMS, SMS Parameters and SMS Status supports
05/11/03   jar     Added Better Handling for Verify PIN to allow it to block.
                   Also added Unblock, Enable, Disable,and Change PIN
                   Modified CDMA Service Table to allow for proper inti.
05/09/03   jar     Fixed compile error
05/06/03   jar     Updated Extension Record in for UIM_TELECOM_EXT1 Record 3
03/18/03   jar     Modified Select Header data for UIM_USIM_ECC, changed from
                   5 Records to 3 Records.
02/24/03   jar     Added Virtual SIM fix to acknowledge Virtual SAT Signals
02/18/03   jar     Added Support for Reset
02/04/03   jar     Added Support to Stub Out UIM with a Virtual SAT Task
01/29/03   jar     Fix Double Definition Errors for Virtual SIM and LTK Fixes.
01/24/03   jar     Added CDMA RUIM Emergency Call Codes and Lang Pref
                   Added the ability to "somewhat" do a Status Command.  Added
                   UIM Functions needed by CM & Virtual SIM.
01/21/03   jar     Added UIM_GSM_LP (Language Preference Support)
01/02/03   jar     LTK RUIM Support Merge.  Some Cleanup.
12/03/02   jar     Modified UIM_GSM_PLMN for QC Scripts.
11/27/02   jar     Fixed Merge error for MSM_OUTSH - MISC_CTL_0.
11/18/02   jar     Added MSM_OUTSH for MISC_CTL_0 Register to enable USIM PAD.
11/08/02   aaa     Added select bytes for PIN2 information
10/18/02   jar     Removed MSG_LOW for MSG_LOW(" GOT UIM_RPT_TIMER_SIG",0,0,0);
10/14/02   tml     changed FEATURE_GSDI_PHONEBOOK_SUPPORT to
                   FEATURE_GSDI_PHONEBOOK_SUPPORT && FEATURE_MMGSDI_UMTS
08/22/02   jar     Renamed uim_rpt_timer to uim_rpt_timer_alternate to avoid
                   double definition issues between Virtual SIM and UIM.
                   Modified TMSI (UIM_GSM_LOCI) and PTMSI (UIM_GSM_LOCIGPRS)
                   values to 0xFF to force location updates to use IMSI.
05/15/02   jar     Merged Fixes from Branch for GSM Test Stack
05/01/02   jar     Merged fixes from Branch for FEATURE_DOG and for MSG_HIGH
03/17/02   jar     Changed Byte 0 of the SST Entry from 0xF0 to 0xFF.
03/16/02   jar     Added EFs Entries for ADN and additional entries for EXT1.
03/14/02   jar     Added EFs UIM_GSM_ECC, UIM_USIM_ECC, UIM_TELECOM_LND and
                   their headers.  Added call to virtuim_get_ef_from_path
                   for phonebook EFs when in USIM Mode.ch
02/13/02   jar     Modified last by of UIM_GSM_KC to be 0x00 if FEATURE_GSM
                   is defined else the last byte is 0x07.
02/12/02   jar     Added Watchdog Reporting Mechanism.  It will report using
                   DOG_UIM_RPT.
02/03/02   jar     Added ability to find EFs and Headers by Path.  Virtuim will
                   decode the path, and use an EF Filename Instead.  Added
                   Basic Search Capabilities.
01/25/02   jar     Modified values in the UIM_TELECOM_SMS and UIM_TELECOM_SMSP
                   headers within the gsdi_sim_file_attr[] array.
01/16/02   jar     Added More IMSIs to the IMSI List
01/01/02   jar     Added IMSI List For Changing IMSIs with UI Menus.  Modified
                   uim_server_read_sim code to use the IMSI List for SIM/USIM
                   IMSI Reads.  Added Function Code to perform get/put to NV.
11/30/01   jar     Moved featurized code from UIM_USIM_IMSI to UIM_GSM_IMSI
11/30/01   jar     Featurized UIM_GSM_IMSI to use a seperate IMSI for GSM
                   Builds.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "task.h"
#ifdef FEATURE_DOG
#include "dog.h"
#endif /* FEATURE_DOG */
#include "tmc.h"

#include "msg.h"
#include "memheap.h"
#include "err.h"

#include <string.h>

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif

#ifndef T_MSM6100
#error code not present
#endif /* T_MSM6100 */
#include "gsdi_exp.h"
#include "gsdi.h"

#ifdef FEATURE_GSDI_UIM_SERVER
#include "virtuim.h"
#endif
#include "uim.h"

#ifdef FEATURE_GSTK
#include "gstki.h"
#endif /* FEATURE_GSTK */

#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
#include "gsdi_event_tester.h"
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
#include "virtsat.h"
#include "gstk_exp.h"
#endif /*#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT) */

#ifdef FEATURE_GSDI_UIM_SERVER

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define VIRTUIM_WORK_WAITING_TIME 3000

#define VIRTUIM_TIMER_1_SECOND  1000  /* In Miliseconds */
#define VIRTUIM_TIMER_EXPIRE_SIG 0x0008

#define VIRTUIM_DF_GSM_LEN                            24
#define VIRTUIM_CHV_LEN                               GSDI_MAX_PIN_LEN
#define VIRTUIM_DF_GSM_CHV_INIT_MASK                  0x80
#define VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK              0x0F
#define VIRTUIM_DF_GSM_CHV_ENABLED_MASK               0x00
#define VIRTUIM_DF_GSM_CHV_DISABLED_MASK              0x80
#define VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK     0x83
#define VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK    0x03
#define VIRTUIM_DF_GSM_NUMBER_OF_PUK_RETRIES_LEFT_MASK  0x0A
#define VIRTUIM_DF_GSM_PUK_INIT_AND_VERIFIED_MASK     0x8A
#define VIRTUIM_BYTE_FILE_CHARACTERISTICS             (14 - 1)
#define VIRTUIM_BYTE_CHV1_STATUS                      (19 - 1)
#define VIRTUIM_BYTE_PUK1_STATUS                      (20 - 1)
#define VIRTUIM_BYTE_CHV2_STATUS                      (21 - 1)
#define VIRTUIM_BYTE_PUK2_STATUS                      (22 - 1)


/* TODO 03/16/03 -> 03/19/03  */
/* for setting the initial power up data structure */
/* Modify #define to Globals with use of search    */
/* functions to replace dependency on hardcoded    */
/* #defines.                                       */
#define VIRTUIM_GSM_SST_INDEX_1  23
#define VIRTUIM_CDMA_SST_INDEX_1 109
#define VIRTUIM_GSM_SST_INDEX_2  17
#define VIRTUIM_CDMA_SST_INDEX_2 93
byte Toolkit_Disabled[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0xFC,0xFF,0xFC};
byte Toolkit_Enabled[10] =  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

byte PIN_Enabled[VIRTUIM_DF_GSM_LEN] = {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
      0x00,0x00,0x08,0x00,0x00,
      0x00,0x00,0x00,0x83,0x8A,
      0x83,0x8A,0x00,0x00};

byte PIN_Disabled[VIRTUIM_DF_GSM_LEN] = {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
      0x00,0x00,0x08,0x80,0x00,
      0x00,0x00,0x00,0x83,0x8A,
      0x83,0x8A,0x00,0x00};

//* Need to find the value in Spec */
byte PIN_Blocked[VIRTUIM_DF_GSM_LEN] = {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
      0x00,0x00,0x08,0x00,0x00,
      0x00,0x00,0x00,0x80,0x8A,
      0x83,0x8A,0x00,0x00};

byte PIN_Perm_Blocked[VIRTUIM_DF_GSM_LEN] = {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
      0x00,0x00,0x08,0x00,0x00,
      0x00,0x00,0x00,0x80,0x80,
      0x83,0x8A,0x00,0x00};

rex_timer_type   test_driver_delay_timer;
rex_timer_type   virtuim_reset_delay_timer;
rex_timer_type   uim_rpt_timer_alternate;

// Control behavior of Dual Slot Virtual SIM
virtuim_card_status_type virtuim_card_status;

#if defined (T_MSM6100) && defined (FEATURE_VIRTUAL_SIM)
/* Status of UIM initialization */
uim_status_type uim_status = UIM_INITIALIZED_S;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
uim_status_type uim_status_2 = UIM_INITIALIZED_S;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /* T_MSM6100 && FEATURE_VIRTUAL_SIM */

/* Queue to hold commands */
q_type                uim_server_task_cmd_q;
boolean               just_powered_up=TRUE;

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
q_type uim_virtsat_proactive_cmd_q;
LOCAL virtsat_report_type virtsat_report;
#endif /* FEATURE_GSTK  && FEATURE_VIRTUAL_SAT */

#ifdef FEATURE_MMGSDI_JSR_177
/* JSR 177 JTK Test */
uint8 myClassByte = 0x00;
uint8 myInstrC1Byte = 0x30; // command APDU case 1
uint8 myInstrC2Byte = 0x32; // command APDU case 2
uint8 myInstrC3Byte = 0x34; // command APDU case 3
uint8 myInstrC4Byte = 0x36; // command APDU case 4
uint8 swOKByte1   = 0x90;
uint8 swOKByte2   = 0x00;
uint8 selectInstrByte = 0xa4;
uint8 selectByDfP1    = 0x04;
uint8 selectByExactDfP2 = 0x00;
uint8 manChInstrByte = 0x70;
uint8 manChOpenP1 = 0x00;
uint8 manChCloseP1 = 0x80;
uint8 manCh1P2 = 0x01;
uint8 rightClaByte[4] = {0x00,0x80,0x90,0xA0 };
uint8 wrongClaByte  = 0x70;
uint8 MY_INS_61XX_C2_REPEAT = 0x38;

uint8 cmdApduSelect[14] = {0x00,0xa4,0x04,0x00,
 0x09, 0xa0,0x00,0x00,0x00,0x62,0xb0,0xba,0xa1,0x81};
uint8 rspApduSelect[3] = {0x12,0x90,0x00};

uint8 cmdApduCase1[4] = {0x00, 0x30, 0x00, 0x00 };
uint8 cmdApduCase2[5] = {0x00, 0x32, 0x00, 0x00, 0x05 };
uint8 cmdApduCase3[10] = {0x00, 0x34, 0x00, 0x00, 0x05, 0x01, 0x03, 0x00, 0x01, 0x02 };
uint8 cmdApduCase4[11] = {0x00, 0x36, 0x00, 0x00, 0x05, 0x01, 0x03, 0x00, 0x01, 0x02, 0x05 };

uint8 rspApduCase13[2] = {0x90, 0x00 };
uint8 rspApduCase24[7] = {0x01, 0x03, 0x00, 0x01, 0x02, 0x90, 0x00 };

// Test 02
uint8 cmdApduManCh1Open[4] = {0x00, 0x70, 0x00, 0x01};
uint8 cmdApduManCh1Close[4] = {0x00, 0x70, 0x80, 0x01};
uint8 malformedApdu1[3] = {0x00, 0x30, 0x00 };
uint8 malformedApdu2[10] = {0x00, 0x34,0x00,0x00,0x0B,0x01,0x03,0x00,0x01,0x02 };
uint8 rspApduManCh1Open[2] = {0x67,0x00};
uint8 rspApduManCh1Close[2] = {0x67,0x00};
uint8 rspmalformedApdu1[2] = {0x67,0x00};
uint8 rspmalformedApdu2[2] = {0x67,0x00};
//uint8 cmdApduTest02Case3 = {0x00, 0x34, 0x00, 0x00,0x05, 0x01, 0x03, 0x00, 0x01,0x02 };

// Test Special case
uint8 cmdApduSelect3[14] = {0x00, 0xa4, 0x04, 0x00,
 0x09, 0xa0,0x00,0x00,0x00,0x62,0xb0,0xba,0xa3,0x81};
uint8 rspApduSelect3[3] = {0x12,0x90,0x00};
uint8 SpecialrspApduCase24[7] = {0x01,0x03,0x00,0x01,0x02,0x90,0x00 };

// Test Special case 02
uint8 SpecialcmdApduCase2[5] = {0x00, 0x38,0x00,0x00,0x02 };
uint8 SpecialcmdApduCase2_0[5] = {0x00, 0x34,0x00,0x00,0x02 };
uint8 SpecialcmdApduCase2_1[5] = {0x00, 0x30,0x00,0x00,0x02 };
uint8 SpecialcmdApduCase2_2[0x0B] = {0x00, 0x36,0x00,0x00,0x05, 0x01,0x03,0x00,0x01,0x02,0x02 };

uint8 SpecialcmdApduCase2_3[0x0B] = {0x00, 0x32,0x00,0x00,0x05, 0x01,0x03,0x00,0x01,0x02,0x02 };

uint8 cmdApduSelect2[14] = {0x00,0xa4,0x04,0x00,
 0x09, 0xa0,0x00,0x00,0x00,0x62,0xb0,0xba,0xa2,0x81};
uint8 rspApduSelect2[3] = {0x12,0x90,0x00};

uint8 cmdApduSelect4[21] = {0x00,0xa4,0x04,0x00,
 0x10, 0xa0,0x00,0x00,0x00,0x62,0xb0,0xba,0xff,0xf1,0xca,0xfe,0x00,0xbf,0xff,0xaa,0x81};
uint8 rspApduSelect4[3] = {0x12,0x90,0x00};

#define UIM_JSR_MAX_TEST_NUM 25

typedef struct
{
  int       app_id;
  int       cmd_len;
  uint8*    cmd_p;
  int       rsp_len;
  uint8*    rsp_p;
} uim_jsr_test_type;


static uim_jsr_test_type jsr_177_test[UIM_JSR_MAX_TEST_NUM] =
{
  {1, 14, cmdApduSelect, 3, rspApduSelect},
  {1, 4, cmdApduCase1, 2, rspApduCase13},
  {1, 5, cmdApduCase2, 7, rspApduCase24},
  {1, 10, cmdApduCase3, 2, rspApduCase13},
  {1, 11, cmdApduCase4, 7, rspApduCase24},
  {1, 4, cmdApduManCh1Open, 2, rspApduManCh1Open},
  {1, 4, cmdApduManCh1Close, 2, rspApduManCh1Close},
  {1, 3, malformedApdu1, 2, rspmalformedApdu1},
  {1, 10, malformedApdu2, 2, rspmalformedApdu2},
  {3, 14, cmdApduSelect3, 3, rspApduSelect3},
  {3, 4, cmdApduCase1, 7, SpecialrspApduCase24},
  {3, 5, cmdApduCase2, 7, SpecialrspApduCase24},
  {3, 10, cmdApduCase3, 7, SpecialrspApduCase24},
  {3, 11, cmdApduCase4, 7, SpecialrspApduCase24},
  {3, 5,  SpecialcmdApduCase2, 7, SpecialrspApduCase24},
  {3, 5,  SpecialcmdApduCase2_0, 7, SpecialrspApduCase24},
  {3, 5,  SpecialcmdApduCase2_1, 7, SpecialrspApduCase24},
  {3, 11,  SpecialcmdApduCase2_2, 7, SpecialrspApduCase24},
  {3, 11,  SpecialcmdApduCase2_3, 7, SpecialrspApduCase24},
  {2, 14, cmdApduSelect2, 3, rspApduSelect2},
  {2, 4, cmdApduCase1, 2, rspApduCase13},
  {2, 5, cmdApduCase2, 7, rspApduCase24},
  {2, 10, cmdApduCase3, 2, rspApduCase13},
  {2, 11, cmdApduCase4, 7, rspApduCase24},
  {4, 21, cmdApduSelect4, 3, rspApduSelect4},

};
#endif /*FEATURE_MMGSDI_JSR_177 */
#if !defined(FEATURE_GSM) && defined(FEATURE_UI)
static uim_sim_element_T imsi_list[UIM_MAX_IMSIS_IN_CACHE] =
{
/*00*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },

/*01*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x08,0x09,0x10,0x10,0x10,0x32,0x54,0x76,0x98} },

/*02*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x22,0xFF} },

/*03*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x23,0xFF} },

/*04*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x24,0xFF} },

/*05*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x25,0xFF} },

/*06*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x26,0xFF} },

/*07*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x27,0xFF} },

/*08*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x28,0xFF} },

/*09*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x29,0xFF} },

/*10*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x30,0xFF} },

/*11*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x31,0xFF} },

/*12*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x32,0xFF} },

/*13*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x33,0xFF} },

/*14*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x34,0xFF} },

/*15*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x35,0xFF} },

/*16*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x36,0xFF} }
};

#endif /* !FEATURE_GSM && 0FEATURE_UI */


/* SIM EF data */
/*lint -e785 */
uim_sim_element_T uim_sim[UIM_NUM_FILES] =
{
/*01*/   {UIM_GSM_IMSI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
    /*{0x08,0x29,0x43,0x10,0x95,0x99,0x99,0x99,0x99 } },*/
    /* MCC=234, MNC=015 MSIN=999999999 */
#ifdef FEATURE_GSM
#error code not present
#else
    {0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },
    /* MCC=440, MNC=793 MSIN 1787802 */
#endif /* FEATURE_GSM */

/*02*/   {UIM_USIM_IMSI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
    /*{0x08,0x29,0x44,0x11,0x88,0x88,0x88,0x88,0x88 } },*/
    /* MCC=244, MNC=118 MSIN=888888888 */
    {0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },
    /* MCC=440, MNC=793 MSIN 1787802 */

/*03*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},
 
/*04*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},

/*05*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},

/*06*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},
    
/*07*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},

/*08*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 6, 6, 5,
    {0x80,0x01,0x01,0x90,0x00 }},


/*09*/   {UIM_TELECOM_MSISDN,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 26,
    {0x51,0x75,0x61,0x6C,0x63,0x6F,0x6D,0x6D,
     0xFF,0xFF,0xFF,0xFF,     /* Alpha Identifier - 'Qualcomm'*/
     0x05,                    /* Length of BCD number/SSC contents */
     0x0A,                    /* TON and NPI */
     0x85,0x85,0x55,0x12,0x12,
     0xFF,0xFF,0xFF,0xFF,0xFF,/* Dialling Number */
     0xFF,                    /* CCP Identifier */
     0xFF                     /* Extension1 Record Number */
    } },

/*10*/   {UIM_TELECOM_CCP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 14,
    {0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,/* 10 bytes bearer capability info */
     0xFF,0xFF,0xFF,0xFF      /* 4 bytes reserved */
    } },

/*11*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 3, 13,
    {0xFF,                          /* 1 byte Record type      */
     0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 11 bytes Extension data */
     0xFF                           /* 1 byte Identifier       */
    } },

/*12*/   {UIM_GSM_KC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
#ifdef FEATURE_GSM
#error code not present
#else
    {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07 } },
#endif /* FEATURE_GSM */

/*13*/   {UIM_GSM_HPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_HPLMN_SP_LEN,  0x02  },

/*14*/   {UIM_GSM_AD, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,  0x81  },

/*15*/   {UIM_USIM_AD, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,  0x89  },

/*16*/   {UIM_GSM_ACC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACC_LEN, { 0x00,0x01 } },

/*17*/   {UIM_GSM_BCCH, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
     {
       0x11,0x22,0x33,
       0x44,0x55,0x66,
       0x00,0x00,0x00,
       0x00,0x00,0x00,
       0x00,0x00,0x00,
       0x00 } },

/*18*/   {UIM_DIR, TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, 20,
    {
     0x61, /* Application tag '61'  */
     0x0E, /* Length of remainder '03'-'7F' */
     0x4F, /* AID tag '4F'  */
     0x10, /* AID length '01'-'10' */
     0xA0,0x00,0x00,0x00,0x87, /* RID */
     0x10,0x02, /* 3G App Code */
     0xFF,0xFF, /* Country Code */
     0xFF,0xFF,0x89,  /* Appl. Provider Code */
     0x03,0x02,0x00,0x00  /* Appl. Provider Field */
    } },

/*19*/   {UIM_DIR, TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, 20,
    {
     0x61, /* Application tag '61'  */
     0x0E, /* Length of remainder '03'-'7F' */
     0x4F, /* AID tag '4F'  */
     0x10, /* AID length '01'-'10' */
     0xA0,0x00,0x00,0x00,0x87, /* RID */
     0x10,0x04, /* ISIM App Code */
     0xFF,0xFF, /* Country Code */
     0xFF,0xFF,0x89,  /* Appl. Provider Code */
     0x03,0x02,0x00,0x00  /* Appl. Provider Field */
    } },
    
 /*20*/   {UIM_GSM_PLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x44,0x30,0x97,  /* qualcomm */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF } },

/*21*/   {UIM_GSM_FPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 12,
    {
     0x42,0xF0,0x10,  /* S TELIA */
     0x42,0xF0,0x70,  /* S COMVIQ */
     0x42,0xF0,0x80,  /* S EURO */
     0xFF,0xFF,0xFF } },

     /* Modified to for use of IMSI in Location Update */
/*22*/   {UIM_GSM_LOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_LOCI_LEN,
    {0xFF,0xFF,0xFF,0xFF,       /* TMSI */
     0xFF,0xFF,0xFF,0xFF,0xFE,   /* LAI */
     0x00,                       /* TMSI TIME */
     0x03 } },                   /* Loc upd status = not_updated */

/*23*/   {UIM_GSM_ACM_MAX,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACM_MAX_LEN,   0x00  },

/*24*/   {UIM_GSM_SST,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SST_LEN,
     /* TOOLKIT DISABLED */
     //{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFF,0xFF
     /* TOOLKIT ENABLED */
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
     }},

/*25*/   {UIM_GSM_ACM,      TRUE, VIRTUIM_CYCLIC, 1, 1, UIM_ACM_LEN,       0x00, 0x00, 0x00  },

/*26*/   {UIM_GSM_SPN,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SPN_LEN,       0x00  },

/*27*/   {UIM_GSM_PUCT,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_PUCT_LEN,      0x00  },

/*28*/   {UIM_GSM_PHASE,    TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_PHASE_LEN,     0x02  },

/*29*/   {UIM_GSM_KCGPRS,   TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
    {0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x00 } },

/*30*/   {UIM_USIM_UST,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0x3F,0xBF,0x6D,0xFF,0x01,0xFF,0xFF,0xFF,0xFF } },

/*31*/   {UIM_USIM_EST,    TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,    0x07 },

/*32*/   {UIM_USIM_ACC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACC_LEN, { 0x00,0x01 } },

/*33*/   {UIM_USIM_HPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1, 0x02 },

/*34*/   {UIM_USIM_HPLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*35*/   {UIM_USIM_PLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*36*/   {UIM_USIM_UPLMNSEL, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*37*/   {UIM_USIM_OPLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*38*/   {UIM_USIM_OPLMNSEL,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*39*/   {UIM_USIM_CPBCCH, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
    {
     0x11,0x22,0x33,
     0x44,0x55,0x66,
     0x00,0x00,0x00,
     0x00,0x00,0x00,
     0x00,0x00,0x00,
     0x00
    } },

/*40*/   {UIM_USIM_KEYS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 33,
    {0xF0,                                          /* KSI */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,            /* CK */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77             /* IK */
    } },

/*41*/   {UIM_USIM_KEYSPS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 33,
    {0xF0,                                          /* KSI */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,            /* CK */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77             /* IK */
    } },

/*42*/   {UIM_USIM_LOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 11,
    {0xFF,0xFF,0xFF,0xFF,         /* TMSI */
     0x02,0xF8,0x99,0x99,0x99,    /* LAI */
     0xFF,                        /* TMSI TIME */
     0x01                         /* Loc upd status = not_updated */
    } },

/*43*/   {UIM_USIM_PSLOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 14,
    {0xEE,0xEE,0xEE,0xEE,           /* P-TMSI */
     0x02,0xF8,0x10,                /* P-TMSI signature value */
     0x00,0x00,0xFF,0x02,0xF8,0x10, /* RAI */
     0x01                           /* Routing Area update status */
    } },

/*44*/   {UIM_USIM_FPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 12,
    {0x42,0xF0,0x10,  /* S TELIA */
     0x42,0xF0,0x70,  /* S COMVIQ */
     0x42,0xF0,0x80,  /* S EURO */
     0xFF,0xFF,0xFF
    } },

/*45*/   {UIM_USIM_START_HFN,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 6,
    {0x66,0x66,0x66,  /* START-CS */
     0x77,0x77,0x77   /* START-PS */
    } },

/*46*/   {UIM_USIM_THRESHOLD,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, 3,
    {0x88,0x88,0x88 } }, /* MAX START-CS START-PS */

/*47*/   {UIM_USIM_CBMID, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 6,
    {
     0x33,0x33,     /* CB Msg Id 1 */
     0x44,0x44,     /* CB Msg Id 2 */
     0x55,0x55      /* CB Msg Id 3 */
    } },

/*48*/   {UIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 5,
    {0x80,0x01,0x01,0x90,0x00 } },

   /* Modified to Force IMSI Usage in Location Update */
/*49*/   {UIM_GSM_LOCIGPRS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_LOCIGPRS_LEN,
    {0xFF,0xFF,0xFF,0xFF,           /* P-TMSI */
     0xFF,0xFF,0xFF,                /* P-TMSI signature value */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* RAI */
     0x00                           /* Routing Area update status */
    } },

/*50*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*51*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*52*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*53*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*54*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*55*/ {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 6, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*56*/ {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 7, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*57*/ {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 8, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*58*/ {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 9, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*59*/ {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 10, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*60*/   {UIM_TELECOM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*61*/   {UIM_TELECOM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*62*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*63*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*64*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*65*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*66*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*67*/   {UIM_TELECOM_SMSS,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
    {0x00, /* Last Used TP-MR */
     0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
    } },

/*68*/   {UIM_USIM_SMSS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
   {0x00, /* Last Used TP-MR */
    0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
   } },

/*69*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*70*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*71*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*72*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*73*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*74*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 6, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*75*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 7, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*76*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 8, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*77*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 9, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*78*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 10, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*79*/   {UIM_USIM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*80*/   {UIM_USIM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*81*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*82*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*83*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*84*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*85*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },


/*86*/   {UIM_USIM_NETPAR,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_CACHE_NETPAR_LEN,
    {0x00,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  /*10*/
    } },

/*87*/   {UIM_USIM_KC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
    {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07
    } },

/*88*/   {UIM_GSM_ECC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_GSM_ECC_LEN,
    {0x19,0xF1,0xFF
    } },
/*89*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  1, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*90*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  2, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*91*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  3, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*92*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  1, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*93*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  2, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*94*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  3, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*95*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  4, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*96*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  5, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*97*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  6, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*98*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  7, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*99*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  8, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*100*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  9, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*101*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  10, 10, UIM_TELECOM_LND_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*102*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 1, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*103*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 2, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*104*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 3, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*105*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 4, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*106*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 5, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*107*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 6, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*108*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 7, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*109*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 8, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*110*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 9, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*111*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 10, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },

/*112*/   {UIM_TELECOM_FDN, TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*113*/   {UIM_TELECOM_FDN, TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*114*/   {UIM_TELECOM_FDN, TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*115*/   {UIM_TELECOM_FDN, TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*116*/   {UIM_TELECOM_FDN, TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*117*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 3, 13,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF
    } },
/*118*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 3, 13,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF
        } },
/*119*/   {UIM_CDMA_UIM_ID_USAGE_IND, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,
    {0x01
    } },
/*120*/   {UIM_CDMA_CDMA_SVC_TABLE, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
     /* TOOLKIT DISABLED */
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFF,0xFF
     /* TOOLKIT ENABLED */
     //{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*121*/   {UIM_CDMA_IMSI_M, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*122*/   {UIM_CDMA_TMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*123*/   {UIM_CDMA_IMSI_T, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*124*/   {UIM_CDMA_RUIM_ID, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*125*/   {UIM_GSM_LP, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 2,
    {
     0x01,0x02
    } },
/*126*/   {UIM_CDMA_PREF_LANG, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 2,
  { 0x01,
    0x02
  } },
/*127*/  {UIM_CDMA_ECC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 4,
  { 0x19, 0xF1, 0x55, 0x43
  } },
/*128*/  {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 1, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF    /* RFU Byte 2 */
    } },
/*129*/ {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 2, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF,   /* RFU Byte 2 */
    } },
/*130*/ {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 3, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF,   /* RFU Byte 2 */
    } },
/*131*/  {UIM_GSM_IMG1INST1,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*132*/  {UIM_GSM_IMG1INST2,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*133*/  {UIM_GSM_IMG1INST3,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*134*/  {UIM_GSM_IMG2INST1,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*135*/ {UIM_GSM_IMG2INST2,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*136*/ {UIM_GSM_IMG2INST3,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*137*/ {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*138*/  {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*139*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*140*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*141*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*142*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 6, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*143*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 7, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*144*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 8, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*145*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 9, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },
/*146*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 10, 10, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*147*/   {UIM_CDMA_SMS_PARAMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*148*/   {UIM_CDMA_SMS_PARAMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*149*/   {UIM_CDMA_SMS_STATUS,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
    {0x00, /* Last Used TP-MR */
     0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
    } },
/*150*/   {UIM_ICCID, TRUE, VIRTUIM_TRANSPARENT, 0,0, 10,
    {0xA5, 0x5A, 0xA5, 0x5A, 0xA5,
     0x5A, 0xA5, 0x5A, 0xA5, 0x5A
    }},
/*151*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  1, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*152*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  2, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*153*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  3, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*154*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  4, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*155*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  5, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*156*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  6, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*157*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  7, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*158*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  8, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*159*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  9, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*160*/   {UIM_USIM_ICI, TRUE, VIRTUIM_CYCLIC,  10, 10, UIM_USIM_ICI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*161*/   {UIM_GSM_DCK, TRUE, VIRTUIM_TRANSPARENT,  0, 0, UIM_DCK_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*162*/   {UIM_USIM_DCK, TRUE, VIRTUIM_TRANSPARENT,  0, 0, UIM_DCK_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*163*/   {UIM_USIM_OCI, TRUE, VIRTUIM_CYCLIC,  1, 5, UIM_USIM_OCI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*164*/   {UIM_USIM_OCI, TRUE, VIRTUIM_CYCLIC,  2, 5, UIM_USIM_OCI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*165*/   {UIM_USIM_OCI, TRUE, VIRTUIM_CYCLIC,  3, 5, UIM_USIM_OCI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*166*/   {UIM_USIM_OCI, TRUE, VIRTUIM_CYCLIC,  4, 5, UIM_USIM_OCI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*167*/   {UIM_USIM_OCI, TRUE, VIRTUIM_CYCLIC,  5, 5, UIM_USIM_OCI_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*168*/{UIM_GSM_GID2, TRUE, FALSE, 0, 0, 7,
    {0x06,0x07,0x08,0x09,0x10,0x11,0xFF} },

/*169*/{UIM_USIM_GID1, TRUE, FALSE, 0, 0, 7,
    {0x01,0x02,0x03,0x04,0x05,0xFF,0xFF} },

/*170*/{UIM_USIM_GID2, TRUE, FALSE, 0, 0, 7,
    {0x06,0x07,0x08,0x09,0x10,0x11,0xFF} },

/*171*/{UIM_GSM_CNL, TRUE, FALSE, 0, 0, 36,
    {0x44,0xF0,0x97,0x13,0x01,0x06,
     0x44,0x30,0x97,0x17,0x01,0x06,
     0x44,0xF0,0x97,0x13,0x02,0x07,
     0x44,0x30,0x97,0x17,0x02,0x07,
     0x44,0xF0,0x97,0x13,0x03,0x08,
     0x44,0x30,0x97,0x17,0x04,0x04,
    } },

/*172*/{UIM_USIM_CNL, TRUE, FALSE, 0, 0, 36,
    {0x44,0xF0,0x97,0x13,0x01,0x06,
     0x44,0x30,0x97,0x17,0x01,0x06,
     0x44,0xF0,0x97,0x13,0x02,0x07,
     0x44,0x30,0x97,0x17,0x02,0x07,
     0x44,0xF0,0x97,0x13,0x03,0x08,
     0x44,0x30,0x97,0x17,0x04,0x04,
     } },
/*173*/   {UIM_TELECOM_EXT2,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 3, 13,
    {0xFF,                          /* 1 byte Record type      */
     0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 11 bytes Extension data */
     0xFF                           /* 1 byte Identifier       */
    } },
/*174*/   {UIM_TELECOM_EXT2,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 3, 13,
    {0xFF,                          /* 1 byte Record type      */
     0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 11 bytes Extension data */
     0xFF                           /* 1 byte Identifier       */
    } },
/*175*/   {UIM_TELECOM_EXT2,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 3, 13,
    {0xFF,                          /* 1 byte Record type      */
     0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 11 bytes Extension data */
     0xFF                           /* 1 byte Identifier       */
    } },
/*176*/   {UIM_GSM_CBMI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*177*/   {UIM_GSM_CBMIR,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*178*/{UIM_GSM_GID1, TRUE, FALSE, 0, 0, 7,
    {0x06,0x07,0x08,0x09,0x10,0x11,0xFF} }

#ifdef FEATURE_MMGSDI_CPHS
,
/*179*/   {UIM_GSM_VMWI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_VMWI_LEN,
     {0xFF,0xFF
     }},
/*180*/   {UIM_GSM_CFF, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_CFF_LEN,
     {0xFF,0xFF
     }},
/*181*/   {UIM_GSM_ONS, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ONS_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
     }},
/*182*/   {UIM_GSM_CSP, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_CSP_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
     }},
/*183*/   {UIM_GSM_CPHSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_CPHSI_LEN,
     {0xFF,0xFF,0xFF
     }},
/*184*/   {UIM_GSM_MN, TRUE, VIRTUIM_LINEAR_FIXED, 1, 4, UIM_MN_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*185*/   {UIM_GSM_MN, TRUE, VIRTUIM_LINEAR_FIXED, 2, 4, UIM_MN_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*186*/   {UIM_GSM_MN, TRUE, VIRTUIM_LINEAR_FIXED, 3, 4, UIM_MN_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*187*/   {UIM_GSM_MN, TRUE, VIRTUIM_LINEAR_FIXED, 4, 4, UIM_MN_LEN,
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } }
#endif /* FEATURE_MMGSDI_CPHS */
/*188*/
   ,{UIM_TELECOM_PBR, TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 6,
    {0xA8,0x04,0xC0,0x02,0x4F,0x3A
    } },

/*189*/   {UIM_TELECOM_BDN, TRUE, VIRTUIM_LINEAR_FIXED, 1, 5,UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*190*/  {UIM_TELECOM_BDN, TRUE, VIRTUIM_LINEAR_FIXED, 2, 5,UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*191*/   {UIM_TELECOM_BDN, TRUE, VIRTUIM_LINEAR_FIXED, 3, 5,UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*192*/  {UIM_TELECOM_BDN, TRUE, VIRTUIM_LINEAR_FIXED, 4,5,UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*193*/   {UIM_TELECOM_BDN, TRUE, VIRTUIM_LINEAR_FIXED, 5,5,UIM_TELECOM_FDN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
};


static uim_sim_file_attr_element_T
       gsdi_sim_file_attr[UIM_NUM_FILE_ATTR] =
{
/* 1 */   { UIM_GSM_FPLMN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x7B,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },


/* 2 */   { UIM_TELECOM_ADN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X01,  // byte 02:  file size
        0X18,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3A,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },

/* 3 */   { UIM_TELECOM_MSISDN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x4E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x40,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0x1A,  // byte 14:  rec len
   } },

/* 4 */   { UIM_TELECOM_CCP, 15,
   {
       0x00,  // byte 00:  RFU
       0x00,  // byte 01:  RFU
       0x00,  // byte 02:  file size
       0x2A,  // byte 03:  file size (lsb)
       0x6F,  // byte 04:  file id
       0x3D,  // byte 05:  file id
       0x04,  // byte 06:  file type
       0x00,  // byte 07:  cyclic increase allowed
       0x11,  // byte 08:  file protection (READ/WRITE)
       0x00,  // byte 09:  file protection (INCREASE)
       0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
       0x01,  // byte 11:  file status
       0x02,  // byte 12:  Length of following data
       0x01,  // byte 13:  file structure
       0x0E,  // byte 14:  rec len
    } },

/* 5 */   { UIM_TELECOM_EXT1, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x27,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x4A,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  Length of following data
     0x01,  // byte 13:  file structure
     0x0D,  // byte 14:  rec len
   } },

/* 6 */   { UIM_GSM_BCCH, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x74,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE))
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 7 */   { UIM_GSM_KC, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x20,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 8 */   { UIM_GSM_KCGPRS, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x20,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
    } },

/* 9 */   { UIM_GSM_LOCIGPRS, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x20,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 10 */  { UIM_GSM_ACC, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x78,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 11 */   { UIM_GSM_AD, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0xAD,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 12 */   { UIM_GSM_HPLMN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x31,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 13 */   { UIM_GSM_PLMN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x30,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 14 */   { UIM_GSM_LOCI, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x7E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 15 */   { UIM_GSM_PHASE, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x7E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 16 */   { UIM_GSM_SST, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x38,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

// { UIM_DIR, 30, // w/o total_file_size and w/o SFI
/* 17 */   { UIM_DIR, 33, // w/o total_file_size and w/  SFI
// { UIM_DIR, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_DIR, 34  // w/ total_file_size and w/o SFI
// { UIM_DIR, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x10,  // Value  rec lgth
     0x02,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x00,  // Value   upper
     0x2F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x10,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

/* 18 */   { UIM_GSM_IMSI, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 19 */   { UIM_TELECOM_SMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x03,  // byte 02:  file size
     0x70,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3C,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0xB0   // byte 14:  rec len
   } },

/* 20 */   { UIM_TELECOM_SMSP, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x50,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x42,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0x28   // byte 14:  rec len
   } },

/* 21 */   { UIM_TELECOM_SMSS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x43,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 22 */   { UIM_TELECOM_SMSR, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x96,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x47,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 14:  file structure
     0x1E   // byte 15:  rec len
   } },

// { UIM_USIM_SMS, 30, // w/o total_file_size and w/o SFI
/* 23 */   { UIM_USIM_SMS, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMS, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMS, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMS, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0xB0,  // Value  rec lgth
     0x05,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x3C,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x03,  // Value
     0x70,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x03,  // Value
//     0x70,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSS, 30, // w/o total_file_size and w/o SFI
/* 24 */   { UIM_USIM_SMSS, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSS, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSS, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSS, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x01,  // Value  transparent
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x00,  // Value  rec lgth
     0x00,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x43,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x02,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSP, 30, // w/o total_file_size and w/o SFI
/* 25 */   { UIM_USIM_SMSP, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSP, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSP, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSP, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x28,  // Value  rec lgth
     0x02,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x42,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x50,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSR, 30, // w/o total_file_size and w/o SFI
/* 26 */   { UIM_USIM_SMSR, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSR, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSR, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSR, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x1E,  // Value  rec lgth
     0x05,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x47,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x96,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_PLMNWACT, 30, // w/o total_file_size and w/o SFI
/* 27 */   { UIM_USIM_PLMNWACT, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_PLMNWACT, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_PLMNWACT, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_PLMNWACT, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x01,  // Value  transparent
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x00,  // Value  rec lgth
     0x00,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x60,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x02,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

/* 28 */   { UIM_USIM_FPLMN, 34,
   {
     0x62,   /* FCP Template Tage*/
     0x20,   /* Len of FCP Template */
     0x82,   /* File Discriptor Tag */
     0x02,
     0x01,
     0x21,
     0x83,   /* File Id Tag */
     0x02,
     0x6F,
     0x7B,
     0xA5,   /* Proprietary Info Tag */
     0x03,
     0x80,
     0x01,
     0x31,
     0x8A,
     0x01,
     0x05,
     0x8B,   /* Security Attributes Tag */
     0x03,
     0x6F,
     0x06,
     0x03,
     0x80,   /* File Size */
     0x02,
     0x00,
     0x3C,
     0x81,   /* Total File Size */
     0x02,
     0x00,
     0x4E,
     0x88,   /* Short File ID Tag */
     0x01,
     0x68
  } },

/*29*/ {UIM_GSM_ECC, 15,
  {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0X03,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0XB7,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0X0B,  // byte 08:  file protection (READ/WRITE)
     0xFF,  // byte 09:  file protection (INCREASE)
     0XBB,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
  } },
/*30*/ {UIM_USIM_ECC, 37,
  {
    0X62,
    0X23,
    0X82,
    0X05,
    0X02,
    0X21,
    0X00,
    0X1C,
    0X03,
    0X83,
    0X02,
    0X6F,
    0XB7,
    0XA5,
    0X03,
    0X80,
    0X01,
    0X31,
    0X8A,
    0X01,
    0X05,
    0X8B,
    0X03,
    0X6F,
    0X06,
    0X06,
    0X80,
    0X02,
    0X00,
    0X8C,
    0X81,
    0X02,
    0X00,
    0XA0,
    0X88,
    0X01,
    0X08
  } },
/*32*/ {UIM_TELECOM_LND, 15,
 {
     0X00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0X01,  // byte 02:  file size
     0X18,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0X44,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0X11,  // byte 08:  file protection (READ/WRITE)
     0XFB,  // byte 09:  file protection (INCREASE)
     0XBB,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0X03,  // byte 13:  file structure
     0X1C,  // byte 14:  rec len
 } },
 /* 33 */   { UIM_CDMA_IMSI_M, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
 /* 34 */   { UIM_CDMA_TMSI, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 35 */   { UIM_CDMA_SMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x03,  // byte 02:  file size
     0x70,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3C,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0xB0   // byte 14:  rec len
   } },

/* 36 */   { UIM_CDMA_SMS_PARAMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x50,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3D,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0x28   // byte 14:  rec len
   } },

/* 37 */   { UIM_CDMA_SMS_STATUS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 39 */   { UIM_GSM_GID1, 15, {
     0x00,  // byte 00:  file size (msb)
     0x00,  // byte 01:  file size
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ)
     0x00,  // byte 09:  file protection (WRITE)
     0x00,  // byte 10:  file protection (INCREASE)
     0x01,  // byte 11:  file protection (INVALIDATE)
     0x02,  // byte 12:  file protection (REHABILITATE)
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 40 */   { UIM_GSM_GID2, 15, {
     0x00,  // byte 00:  file size (msb)
     0x00,  // byte 01:  file size
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3F,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ)
     0x00,  // byte 09:  file protection (WRITE)
     0x00,  // byte 10:  file protection (INCREASE)
     0x01,  // byte 11:  file protection (INVALIDATE)
     0x02,  // byte 12:  file protection (REHABILITATE)
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/*41*/ {UIM_USIM_GID1, 37,
  {
    0X62,
    0X20,
    0X82,
    0X02,
    0X01,  /* Indicates Linear Fixed, Transparent, etc. */
    0X21,
    0X83,
    0X02,
    0X6F,
    0X3E,
    0XA5,
    0X03,
    0X80,
    0X01,
    0X31,
    0X8A,
    0X01,
    0X05,
    0X8B,
    0X03,
    0X6F,
    0X06,
    0X06,
    0X80,
    0X02,
    0X00,
    0X8C,
    0X81,
    0X02,
    0X00,
    0XA0,
    0X88,
    0X01,
    0X08
  } },

/*42*/ {UIM_USIM_GID2, 37,
  {
    0X62,
    0X20,
    0X82,
    0X02,
    0X01,  /* Indicates Linear Fixed, Transparent, etc. */
    0X21,
    0X83,
    0X02,
    0X6F,
    0X3F,
    0XA5,
    0X03,
    0X80,
    0X01,
    0X31,
    0X8A,
    0X01,
    0X05,
    0X8B,
    0X03,
    0X6F,
    0X06,
    0X06,
    0X80,
    0X02,
    0X00,
    0X8C,
    0X81,
    0X02,
    0X00,
    0XA0,
    0X88,
    0X01,
    0X08
  } },

/* 43 */   { UIM_GSM_CNL, 15, {
     0x00,  // byte 00:  file size (msb)
     0x00,  // byte 01:  file size
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x32,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ)
     0x00,  // byte 09:  file protection (WRITE)
     0x00,  // byte 10:  file protection (INCREASE)
     0x01,  // byte 11:  file protection (INVALIDATE)
     0x02,  // byte 12:  file protection (REHABILITATE)
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/*44*/ {UIM_USIM_CNL, 37,
  {
    0X62,
    0X20,
    0X82,
    0X02,
    0X01,  /* Indicates Linear Fixed, Transparent, etc. */
    0X21,
    0X83,
    0X02,
    0X6F,
    0X32,
    0XA5,
    0X03,
    0X80,
    0X01,
    0X31,
    0X8A,
    0X01,
    0X05,
    0X8B,
    0X03,
    0X6F,
    0X06,
    0X06,
    0X80,
    0X02,
    0X00,
    0X8C,
    0X81,
    0X02,
    0X00,
    0XA0,
    0X88,
    0X01,
    0X08
  } },
 /* 45 */   { UIM_TELECOM_EXT2, 15,
   {
     0x00,  // byte 00:  file size (msb)
     0x00,  // byte 01:  file size
     0x00,  // byte 02:  file size
     0x27,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x4B,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ)
     0x00,  // byte 09:  file protection (WRITE)
     0x44,  // byte 10:  file protection (INCREASE)
     0x01,  // byte 11:  file protection (INVALIDATE)
     0x02,  // byte 12:  Length of following data
     0x01,  // byte 13:  file structure
     0x0D,  // byte 14:  rec len
   } },
/* 46 */   { UIM_TELECOM_FDN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3B,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },
/* 47 */   { UIM_GSM_CBMI, 15,
   {
        0x00,
        0x00,
        0X00,
        0X14,
        0x6F,
        0x45,
        0x04,
        0x00,
        0X11,
        0XFF,
        0XBB,
        0X01,
        0x02,
        0x00,
        0x00,
   } },
/* 48 */   { UIM_GSM_CBMIR, 15,
   {
        0x00,
        0x00,
        0X00,
        0X28,
        0x6F,
        0x50,
        0x04,
        0x00,
        0X11,
        0XFF,
        0XBB,
        0X01,
        0x02,
        0x00,
        0x00,
   } },
/* 49 */   { UIM_USIM_ADN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3A,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },
   /* 50 */   { UIM_USIM_FDN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3B,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },


/* 51 */   { UIM_TELECOM_SDN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3A,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } }
#ifdef FEATURE_MMGSDI_CPHS
,

/* 52 */   { UIM_GSM_VMWI, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x11,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 53 */   { UIM_GSM_CFF, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x13,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 54 */   { UIM_GSM_ONS, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x18,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x14,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 55 */   { UIM_GSM_CSP, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x18,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x15,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 56 */   { UIM_GSM_CPHSI, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x03,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x16,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 57 */   { UIM_GSM_MN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x60,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x17,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ/WRITE))
     0x00,  // byte 09:  file protection (INCREASE)
     0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  status
     0x02,  // byte 12:  Length of following data
     0x01,  // byte 13:  file structure
     0x0D,  // byte 14:  rec len
   } },
#endif /* FEATURE_MMGSDI_CPHS */
/* 48 */   { UIM_TELECOM_BDN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x4D,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } }
};

#ifdef FEATURE_MMGSDI_DUAL_SLOT
#if !defined(FEATURE_GSM) && defined(FEATURE_UI)
static uim_sim_element_T imsi_list2[UIM_MAX_IMSIS_IN_CACHE] =
{
/*00*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },

/*01*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x08,0x09,0x10,0x10,0x10,0x32,0x54,0x76,0x98} },

/*02*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x22,0xFF} },

/*03*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x23,0xFF} },

/*04*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x24,0xFF} },

/*05*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x25,0xFF} },

/*06*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x26,0xFF} },

/*07*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x27,0xFF} },

/*08*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x28,0xFF} },

/*09*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x29,0xFF} },

/*10*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x30,0xFF} },

/*11*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x31,0xFF} },

/*12*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x32,0xFF} },

/*13*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x33,0xFF} },

/*14*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x34,0xFF} },

/*15*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x35,0xFF} },

/*16*/
  { UIM_GSM_IMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
      { 0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x36,0xFF} }
};

#endif /* !FEATURE_GSM && 0FEATURE_UI */


/* SIM EF data */
/*lint -e785 */
uim_sim_element_T uim_sim2[UIM_NUM_FILES_2] =
{
/*01*/   {UIM_GSM_IMSI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
    /*{0x08,0x29,0x43,0x10,0x95,0x99,0x99,0x99,0x99 } },*/
    /* MCC=234, MNC=015 MSIN=999999999 */
#ifdef FEATURE_GSM
#error code not present
#else
    {0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },
    /* MCC=440, MNC=793 MSIN 1787802 */
#endif /* FEATURE_GSM */

/*02*/   {UIM_USIM_IMSI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_IMSI_LEN,
    /*{0x08,0x29,0x44,0x11,0x88,0x88,0x88,0x88,0x88 } },*/
    /* MCC=244, MNC=118 MSIN=888888888 */
    {0x07,0x49,0x04,0x97,0x13,0x87,0x97,0x20,0xFF} },
    /* MCC=440, MNC=793 MSIN 1787802 */

/*03*/   {UIM_USIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 5,
    {0x80,0x01,0x01,0x90,0x00 } },

/*04*/   {UIM_TELECOM_MSISDN,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 26,
    {0x51,0x75,0x61,0x6C,0x63,0x6F,0x6D,0x6D,
     0xFF,0xFF,0xFF,0xFF,     /* Alpha Identifier - 'Qualcomm'*/
     0x05,                    /* Length of BCD number/SSC contents */
     0x0A,                    /* TON and NPI */
     0x85,0x85,0x55,0x12,0x12,
     0xFF,0xFF,0xFF,0xFF,0xFF,/* Dialling Number */
     0xFF,                    /* CCP Identifier */
     0xFF                     /* Extension1 Record Number */
    } },

/*05*/   {UIM_TELECOM_CCP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 14,
    {0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,/* 10 bytes bearer capability info */
     0xFF,0xFF,0xFF,0xFF      /* 4 bytes reserved */
    } },

/*06*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 3, 13,
    {0xFF,                          /* 1 byte Record type      */
     0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* 11 bytes Extension data */
     0xFF                           /* 1 byte Identifier       */
    } },

/*07*/   {UIM_GSM_KC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
#ifdef FEATURE_GSM
#error code not present
#else
    {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07 } },
#endif /* FEATURE_GSM */

/*08*/   {UIM_GSM_HPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_HPLMN_SP_LEN,  0x02  },

/*09*/   {UIM_GSM_AD, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,  0x81  },

/*10*/   {UIM_USIM_AD, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,  0x89  },

/*11*/   {UIM_GSM_ACC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACC_LEN, { 0x00,0x01 } },

/*12*/   {UIM_GSM_BCCH, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
     {
       0x11,0x22,0x33,
       0x44,0x55,0x66,
       0x00,0x00,0x00,
       0x00,0x00,0x00,
       0x00,0x00,0x00,
       0x00 } },

/*13*/   {UIM_DIR, TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 20,
    {
     0x61, /* Application tag '61'  */
     0x0E, /* Length of remainder '03'-'7F' */
     0x4F, /* AID tag '4F'  */
     0x10, /* AID length '01'-'10' */
     0xA0,0x00,0x00,0x00,0x87, /* RID */
     0x10,0x02, /* 3G App Code */
     0xFF,0xFF, /* Country Code */
     0xFF,0xFF,0x89,  /* Appl. Provider Code */
     0x03,0x02,0x00,0x00  /* Appl. Provider Field */
    } },

/*14*/   {UIM_GSM_PLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x44,0x30,0x97,  /* qualcomm */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF } },

/*15*/   {UIM_GSM_FPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 12,
    {
     0x42,0xF0,0x10,  /* S TELIA */
     0x42,0xF0,0x70,  /* S COMVIQ */
     0x42,0xF0,0x80,  /* S EURO */
     0xFF,0xFF,0xFF } },

     /* Modified to for use of IMSI in Location Update */
/*16*/   {UIM_GSM_LOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_LOCI_LEN,
    {0xFF,0xFF,0xFF,0xFF,       /* TMSI */
     0xFF,0xFF,0xFF,0xFF,0xFE,   /* LAI */
     0x00,                       /* TMSI TIME */
     0x03 } },                   /* Loc upd status = not_updated */

/*17*/   {UIM_GSM_ACM_MAX,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACM_MAX_LEN,   0x00  },

/*18*/   {UIM_GSM_SST,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SST_LEN,
     /* TOOLKIT DISABLED */
     //{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFF,0xFF
     /* TOOLKIT ENABLED */
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
     }},

/*19*/   {UIM_GSM_ACM,      TRUE, VIRTUIM_CYCLIC, 1, 3, UIM_ACM_LEN,       0x00, 0x00, 0x03  },

/*20*/   {UIM_GSM_ACM,      TRUE, VIRTUIM_CYCLIC, 2, 3, UIM_ACM_LEN,       0x00, 0x00, 0x02  },

/*21*/   {UIM_GSM_ACM,      TRUE, VIRTUIM_CYCLIC, 3, 3, UIM_ACM_LEN,       0x00, 0x00, 0x01  },

/*22*/   {UIM_GSM_SPN,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SPN_LEN,       0x00  },

/*23*/   {UIM_GSM_PUCT,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_PUCT_LEN,      0x00  },

/*24*/   {UIM_GSM_PHASE,    TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_PHASE_LEN,     0x02  },

/*25*/   {UIM_GSM_KCGPRS,   TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
    {0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x00 } },

/*26*/   {UIM_USIM_UST,      TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0x3F,0xBF,0x6D,0xFF,0x01,0xFF,0xFF,0xFF,0xFF } },

/*27*/   {UIM_USIM_EST,    TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,    0x07 },

/*28*/   {UIM_USIM_ACC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_ACC_LEN, { 0x00,0x01 } },

/*29*/   {UIM_USIM_HPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1, 0x02 },

/*30*/   {UIM_USIM_HPLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*31*/   {UIM_USIM_PLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*32*/   {UIM_USIM_UPLMNSEL, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*33*/   {UIM_USIM_OPLMNWACT, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*34*/   {UIM_USIM_OPLMNSEL,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, 30,
    {
     0x82,0xF6,0x10,  /* turkcell */
     0x24,0xF7,0x10,  /* qatarnet */
     0x22,0xF2,0x01,  /* omnitel */
     0x32,0xF4,0x01,  /* cellnet */
     0x54,0xF4,0x60,  /* smartone */
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF
    } },

/*35*/   {UIM_USIM_CPBCCH, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
    {
     0x11,0x22,0x33,
     0x44,0x55,0x66,
     0x00,0x00,0x00,
     0x00,0x00,0x00,
     0x00,0x00,0x00,
     0x00
    } },

/*36*/   {UIM_USIM_KEYS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 33,
    {0xF0,                                          /* KSI */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,            /* CK */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77             /* IK */
    } },

/*37*/   {UIM_USIM_KEYSPS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 33,
    {0xF0,                                          /* KSI */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,            /* CK */
     0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07,
     0x11,0x22,0x33,0x44,0x55,0x66,0x77             /* IK */
    } },

/*38*/   {UIM_USIM_LOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 11,
    {0xFF,0xFF,0xFF,0xFF,         /* TMSI */
     0x02,0xF8,0x99,0x99,0x99,    /* LAI */
     0xFF,                        /* TMSI TIME */
     0x01                         /* Loc upd status = not_updated */
    } },

/*39*/   {UIM_USIM_PSLOCI,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 14,
    {0xEE,0xEE,0xEE,0xEE,           /* P-TMSI */
     0x02,0xF8,0x10,                /* P-TMSI signature value */
     0x00,0x00,0xFF,0x02,0xF8,0x10, /* RAI */
     0x01                           /* Routing Area update status */
    } },

/*40*/   {UIM_USIM_FPLMN, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 12,
    {0x42,0xF0,0x10,  /* S TELIA */
     0x42,0xF0,0x70,  /* S COMVIQ */
     0x42,0xF0,0x80,  /* S EURO */
     0xFF,0xFF,0xFF
    } },

/*41*/   {UIM_USIM_START_HFN,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, 6,
    {0x66,0x66,0x66,  /* START-CS */
     0x77,0x77,0x77   /* START-PS */
    } },

/*42*/   {UIM_USIM_THRESHOLD,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, 3,
    {0x88,0x88,0x88 } }, /* MAX START-CS START-PS */

/*43*/   {UIM_USIM_CBMID, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 6,
    {
     0x33,0x33,     /* CB Msg Id 1 */
     0x44,0x44,     /* CB Msg Id 2 */
     0x55,0x55      /* CB Msg Id 3 */
    } },

/*44*/   {UIM_ARR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 1, 5,
    {0x80,0x01,0x01,0x90,0x00 } },

   /* Modified to Force IMSI Usage in Location Update */
/*45*/   {UIM_GSM_LOCIGPRS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_LOCIGPRS_LEN,
    {0xFF,0xFF,0xFF,0xFF,           /* P-TMSI */
     0xFF,0xFF,0xFF,                /* P-TMSI signature value */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, /* RAI */
     0x00                           /* Routing Area update status */
    } },

/*46*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*47*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*48*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*49*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*50*/   {UIM_TELECOM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*51*/   {UIM_TELECOM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*52*/   {UIM_TELECOM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*53*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*54*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*55*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*56*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*57*/   {UIM_TELECOM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*58*/   {UIM_TELECOM_SMSS,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
    {0x00, /* Last Used TP-MR */
     0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
    } },

/*59*/   {UIM_USIM_SMSS,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
   {0x00, /* Last Used TP-MR */
    0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
   } },

/*60*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*61*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*62*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*63*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*64*/   {UIM_USIM_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*65*/   {UIM_USIM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*66*/   {UIM_USIM_SMSP,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*67*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*68*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*69*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*70*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },

/*71*/   {UIM_USIM_SMSR,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMSR_LEN,
    {0x00,  /* Record Identifier */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF                                    /*29*/
    } },


/*72*/   {UIM_USIM_NETPAR,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_CACHE_NETPAR_LEN,
    {0x00,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  /*10*/
    } },

/*73*/   {UIM_USIM_KC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_KC_LEN,
    {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x07
    } },

/*74*/   {UIM_GSM_ECC,     TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_GSM_ECC_LEN,
    {0x19,0xF1,0xFF
    } },
/*75*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  1, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*76*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  2, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*77*/   {UIM_USIM_ECC,    TRUE, VIRTUIM_LINEAR_FIXED,  3, 3, UIM_USIM_ECC_LEN,
    {0X62,0X38,0X82,0X02,0X38,0X21,0X84,0X10,0XA0,0X00,0X00,0X00,
     0X87,0X10,0X02,0XFF,0XFF,0XFF,0XFF,0X89,0X03,0X05,0X00,0X01,
     0XA5,0X0A,0X80,0X01,0X31,0X82,0X01,0X1E,0X83,0X02,0X00,0X98,
     0X8A,0X01,0X05,0X8B,0X03,0X2F,0X06,0X03,0XC6,0X0C,0X90,0X01,
     0X60,0X83,0X01,0X01,0X83,0X01,0X02,0X83,0X01,0X81
    } },
/*78*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  1, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*79*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  2, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*80*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  3, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*81*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  4, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*82*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  5, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*83*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  6, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*84*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  7, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*85*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  8, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*86*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  9, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*87*/   {UIM_TELECOM_LND, TRUE, VIRTUIM_CYCLIC,  10, 10, UIM_TELECOM_LND_LEN,
    {0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
     0XFF,0XFF,0XFF,0XFF
    } },
/*88*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 1, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x31,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x80,0x11,0x11,0x11,0x11,0x11,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*89*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 2, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x32,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0x91,0x22,0x22,0x22,0x22,0x22,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*90*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 3, 10, UIM_TELECOM_ADN_LEN,
    {0x4E,0x6F,0x20,0x4E,0x75,0x6D,0x62,0x65,0x72,0x73,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*91*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 4, 10, UIM_TELECOM_ADN_LEN,
    {0x54,0x65,0x73,0x74,0x20,0x4E,0x75,0x6D,0x34,0xFF,0xFF,0xFF,
     0xFF,0xFF,0x06,0xA4,0x44,0x44,0x44,0x44,0x44,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF
    } },
/*92*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 5, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*93*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 6, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*94*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 7, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*95*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 8, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*96*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 9, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*97*/   {UIM_TELECOM_ADN, TRUE, VIRTUIM_LINEAR_FIXED, 10, 10, UIM_TELECOM_ADN_LEN,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0xFF,0xFF,0xFF,
    } },
/*98*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 3, 13,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF
    } },
/*99*/   {UIM_TELECOM_EXT1,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 3, 13,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF
        } },
/*100*/   {UIM_CDMA_UIM_ID_USAGE_IND, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 1,
    {0x01
    } },
/*101*/   {UIM_CDMA_CDMA_SVC_TABLE, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
     /* TOOLKIT DISABLED */
     //{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0xFF,0xFF
     /* TOOLKIT ENABLED */
     {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*102*/   {UIM_CDMA_IMSI_M, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*103*/   {UIM_CDMA_TMSI, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 16,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*104*/   {UIM_CDMA_IMSI_T, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    } },
/*105*/   {UIM_CDMA_RUIM_ID, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 10,
    {0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x2,0x22
    } },
/*106*/   {UIM_GSM_LP, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 2,
    {
     0x02,0x02
    } },
/*107*/
       {UIM_CDMA_PREF_LANG, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 2,
  { 0x02,
    0x02
  } },
/*108*/
       {UIM_CDMA_ECC, TRUE, VIRTUIM_TRANSPARENT, 0, 0, 4,
  { 0x22, 0x22, 0x12, 0x12
  } },
/*109*/  {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 1, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF    /* RFU Byte 2 */
    } },
/*110*/ {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 2, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF,   /* RFU Byte 2 */
    } },
/*111*/ {UIM_GSM_IMG,                  TRUE, VIRTUIM_LINEAR_FIXED, 3, 3, 29,
    {
      0x02,   /* Number of Actual Image Instances */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x00,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x01,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0x50,   /* INST1 Width,  Expressed in Raster Image Points */
      0x50,   /* INST1 Height, Expressed in Raster Image Points */
      0x11,   /* INST1 IMG CODING SCHEME, 0x11 Basic, 0x21 Colour Image */
      0x4F,   /* INST1 FID High Byte */
      0x02,   /* INST1 FID Low  Byte */
      0x00,   /* INST1 OFFSET High Byte Into Image Instace File */
      0x00,   /* INST1 OFFSET Low  Byte Into Image Instance File */
      0x00,   /* INST1 LENGTH High Byte of Image Instance File  */
      0x10,   /* INST1 LNEGHT Low Byte of Image Instance File */
      0xFF,   /* RFU Byte 1 */
      0xFF,   /* RFU Byte 2 */
    } },
/*112*/  {UIM_GSM_IMG1INST1,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*113*/  {UIM_GSM_IMG1INST2,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*114*/  {UIM_GSM_IMG1INST3,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*115*/  {UIM_GSM_IMG2INST1,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*116*/ {UIM_GSM_IMG2INST2,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*117*/ {UIM_GSM_IMG2INST3,     TRUE, VIRTUIM_TRANSPARENT, 0,0,16,
    {
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF,
      0xFF,0xFF,0xFF,0xFF
    } },
/*118*/ {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*119*/  {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*120*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 3, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*121*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 4, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*122*/   {UIM_CDMA_SMS,     TRUE, VIRTUIM_LINEAR_FIXED, 5, 5, UIM_SMS_LEN,
    {0x00, /* Status */
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*48*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*60*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*72*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*84*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*96*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*108*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*120*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*132*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*144*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*156*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*168*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF                          /*175*/
    } },

/*123*/   {UIM_CDMA_SMS_PARAMS,     TRUE, VIRTUIM_LINEAR_FIXED, 1, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*124*/   {UIM_CDMA_SMS_PARAMS,     TRUE, VIRTUIM_LINEAR_FIXED, 2, 2, UIM_SMSP_LEN,
    {
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*12*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*24*/
     0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*36*/
     0xFF,0xFF,0xFF,0xFF                                         /*40*/
    } },

/*125*/   {UIM_CDMA_SMS_STATUS,  TRUE, VIRTUIM_TRANSPARENT, 0, 0, UIM_SMSS_LEN,
    {0x00, /* Last Used TP-MR */
     0xFF  /* SMS "Memory Cap. Exceeded" Not. Flag */
    } },
/*126*/   {UIM_ICCID, TRUE, VIRTUIM_TRANSPARENT, 0,0, 10,
    {0x5A, 0xA5, 0x5A, 0xA5, 0x5A,
     0xA5, 0x5A, 0xA5, 0x5A, 0xA5
    }}
};


static uim_sim_file_attr_element_T
       gsdi_sim_file_attr2[UIM_NUM_FILE_ATTR] =
{
/* 1 */   { UIM_GSM_FPLMN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x7B,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 2 */   { UIM_TELECOM_ADN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X01,  // byte 02:  file size
        0X18,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3A,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },

/* 3 */   { UIM_TELECOM_MSISDN, 15,
   {
       0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x4E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x40,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
       0x1A,  // byte 14:  rec len
   } },

/* 4 */   { UIM_TELECOM_CCP, 15,
   {
       0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
       0x00,  // byte 02:  file size
       0x2A,  // byte 03:  file size (lsb)
       0x6F,  // byte 04:  file id
       0x3D,  // byte 05:  file id
       0x04,  // byte 06:  file type
       0x00,  // byte 07:  cyclic increase allowed
       0x11,  // byte 08:  file protection (READ/WRITE))
       0x00,  // byte 09:  file protection (INCREASE)
       0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
       0x01,  // byte 11:  status
       0x02,  // byte 12:  Length of following data
       0x01,  // byte 13:  file structure
       0x0E,  // byte 14:  rec len
    } },

/* 5 */   { UIM_TELECOM_EXT1, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x27,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x4A,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x11,  // byte 08:  file protection (READ/WRITE))
     0x00,  // byte 09:  file protection (INCREASE)
     0x44,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  status
     0x02,  // byte 12:  Length of following data
     0x01,  // byte 13:  file structure
     0x0D,  // byte 14:  rec len
   } },

/* 6 */   { UIM_GSM_BCCH, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x74,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE))
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
       0x00,  // byte 14:  rec len
   } },

/* 7 */   { UIM_GSM_KC, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0x20,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
       0x00,  // byte 14:  rec len
   } },

/* 8 */   { UIM_GSM_KCGPRS, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x20,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
    } },

/* 9 */   { UIM_GSM_LOCIGPRS, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x20,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 10 */  { UIM_GSM_ACC, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x78,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 11 */   { UIM_GSM_AD, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0xAD,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 12 */   { UIM_GSM_HPLMN, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x31,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 13 */   { UIM_GSM_PLMN, 15,
   {
     0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x1E,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x30,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 14 */   { UIM_GSM_LOCI, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x7E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 15 */   { UIM_GSM_PHASE, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x7E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 16 */   { UIM_GSM_SST, 15,
   {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x0B,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x38,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

// { UIM_DIR, 30, // w/o total_file_size and w/o SFI
/* 17 */   { UIM_DIR, 33, // w/o total_file_size and w/  SFI
// { UIM_DIR, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_DIR, 34  // w/ total_file_size and w/o SFI
// { UIM_DIR, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x10,  // Value  rec lgth
     0x01,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x00,  // Value   upper
     0x2F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x10,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

/* 18 */   { UIM_GSM_IMSI, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 19 */   { UIM_TELECOM_SMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x03,  // byte 02:  file size
     0x70,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3C,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0xB0   // byte 14:  rec len
   } },

/* 20 */   { UIM_TELECOM_SMSP, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x50,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x42,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0x28   // byte 14:  rec len
   } },

/* 21 */   { UIM_TELECOM_SMSS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x43,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },

/* 22 */   { UIM_TELECOM_SMSR, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x96,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x47,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0X00,  // byte 09:  file protection (INCREASE)
     0X00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0X01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 14:  file structure
     0x1E   // byte 15:  rec len
   } },

// { UIM_USIM_SMS, 30, // w/o total_file_size and w/o SFI
/* 23 */   { UIM_USIM_SMS, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMS, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMS, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMS, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0xB0,  // Value  rec lgth
     0x05,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x3C,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x03,  // Value
     0x70,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x03,  // Value
//     0x70,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSS, 30, // w/o total_file_size and w/o SFI
/* 24 */   { UIM_USIM_SMSS, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSS, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSS, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSS, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x01,  // Value  transparent
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x00,  // Value  rec lgth
     0x00,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x43,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x02,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSP, 30, // w/o total_file_size and w/o SFI
/* 25 */   { UIM_USIM_SMSP, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSP, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSP, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSP, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x28,  // Value  rec lgth
     0x02,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x42,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x50,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_SMSR, 30, // w/o total_file_size and w/o SFI
/* 26 */   { UIM_USIM_SMSR, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_SMSR, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_SMSR, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_SMSR, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x02,  // Value  linear fixed
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x1E,  // Value  rec lgth
     0x05,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x47,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x96,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

// { UIM_USIM_PLMNWACT, 30, // w/o total_file_size and w/o SFI
/* 27 */   { UIM_USIM_PLMNWACT, 33, // w/o total_file_size and w/  SFI
// { UIM_USIM_PLMNWACT, 56, // w/o total_file_size and w/o SFI & w/ Sec Attr AB
// { UIM_USIM_PLMNWACT, 34  // w/ total_file_size and w/o SFI
// { UIM_USIM_PLMNWACT, 37,  // 37 w/ total_file_size and w/ SFI
   {
     0x62,  // TAG File Descriptor '62'
//     0x1D,  // Lgth w/o total_file_size and w/o SFI
       0x20,  // Lgth w/o total_file_size and w/ SFI
//     0x36,  // Lgth w/o total_file_size and w/ SFI & w/ Sec Attr AB
//     0x21,  // Lgth w/ total_file_size and w/o SFI
//     0x24,  // Lgth w/ total_file_size and w/ SFI
     0x82,  // TAG File Descriptor '82'
     0x05,  // Lgth
     0x01,  // Value  transparent
     0x21,  // Value  data coding
     0x00,  // Value  rec lgth
     0x00,  // Value  rec lgth
     0x00,  // Value  num recs
     0x83,  // TAG File Identifier '83'
     0x02,  // Lgth
     0x60,  // Value   upper
     0x6F,  // Value   lower
     0xA5,  // TAG Proprietary information 'A5'
     0x03,  // Lgth
     0x80,  //   TAG UICC Characteristics '80'
     0x01,  //   Lgth
     0x21,  //   Value Clock stop allowed & Voltage Class B
     0x8A,  // TAG Life Cycle Integer '8A'
     0x01,  // Lgth
     0x00,  // Value
       0x8B,  // TAG Security Attributes '8B'
       0x03,  // Lgth
       0x2F,  // Value - EF ARR File ID
       0x06,  // Value - EF ARR File ID
           0x01,  // Value - EF ARR Rec Num
//       0x8C,  // TAG Security Attributes '8C'
//       0x03,  // Lgth
//       0x0A,  // Value - AM byte
//       0x10,  // Value - SC byte
//         0x10,  // Value - SC byte
//       0xAB,  // TAG Security Attributes 'AB'
//       0x1A,  // Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x01,  //   Value
//       0x90,  //    SC_DO TAG '90'
//       0x00,  //    Lgth
//       0x80,  //   TAG AM DO Tag '80'
//       0x01,  //   Lgth
//       0x02,  //   Value
//       0xA0,  //   OR TAG 'A0'
//       0x10,  //   Lgth
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x01,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
//       0xA4,  //    SC_DO TAG 'A4'
//       0x06,  //    Lgth
//         0x83,  //     Key Ref Tag '83'
//       0x01,  //     Lgth
//       0x02,  //     Value
//         0x95,  //     Usage Qual Tag '95'
//       0x01,  //     Lgth
//       0x08,  //     Value
           0x80,  // TAG File Size '80'
     0x02,  // Lgth
     0x00,  // Value
     0x02,  // Value
//     0x81,  // TAG Total File Size '81'
//     0x02,  // Lgth
//     0x00,  // Value
//     0x55,  // Value
     0x88,  // TAG SFI tag '88'
     0x01,  // Lgth
     0x3F   // Value
   } },

/* 28 */   { UIM_USIM_FPLMN, 34,
   {
     0x62,   /* FCP Template Tage*/
     0x20,   /* Len of FCP Template */
     0x82,   /* File Discriptor Tag */
     0x02,
     0x01,
     0x21,
     0x83,   /* File Id Tag */
     0x02,
     0x6F,
     0x7B,
     0xA5,   /* Proprietary Info Tag */
     0x03,
     0x80,
     0x01,
     0x31,
     0x8A,
     0x01,
     0x05,
     0x8B,   /* Security Attributes Tag */
     0x03,
     0x6F,
     0x06,
     0x03,
     0x80,   /* File Size */
     0x02,
     0x00,
     0x3C,
     0x81,   /* Total File Size */
     0x02,
     0x00,
     0x4E,
     0x88,   /* Short File ID Tag */
     0x01,
     0x68
  } },

/*29*/ {UIM_GSM_ECC, 15,
  {
    0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0X03,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0XB7,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0X0B,  // byte 08:  file protection (READ/WRITE)
     0xFF,  // byte 09:  file protection (INCREASE)
     0XBB,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
  } },
/*30*/ {UIM_USIM_ECC, 37,
  {
    0X62,
    0X23,
    0X82,
    0X05,
    0X02,
    0X21,
    0X00,
    0X1C,
    0X03,
    0X83,
    0X02,
    0X6F,
    0XB7,
    0XA5,
    0X03,
    0X80,
    0X01,
    0X31,
    0X8A,
    0X01,
    0X05,
    0X8B,
    0X03,
    0X6F,
    0X06,
    0X06,
    0X80,
    0X02,
    0X00,
    0X8C,
    0X81,
    0X02,
    0X00,
    0XA0,
    0X88,
    0X01,
    0X08
  } },
/*32*/ {UIM_TELECOM_LND, 15,
 {
    0X00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0X01,  // byte 02:  file size
     0X18,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id (msb)
     0X44,  // byte 05:  file id (lsb)
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0X11,  // byte 08:  file protection (READ/WRITE)
     0XFB,  // byte 09:  file protection (INCREASE)
     0XBB,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0X03,  // byte 13:  file structure
     0X1C,  // byte 14:  rec len
 } },
 /* 33 */   { UIM_CDMA_IMSI_M, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
 /* 34 */   { UIM_CDMA_TMSI, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x09,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x07,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
/* 35 */   { UIM_CDMA_SMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x03,  // byte 02:  file size
     0x70,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3C,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0xB0   // byte 14:  rec len
   } },

/* 36 */   { UIM_CDMA_SMS_PARAMS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x50,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3D,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x01,  // byte 13:  file structure
     0x28   // byte 14:  rec len
   } },

/* 37 */   { UIM_CDMA_SMS_STATUS, 15, {
     0x00,  // byte 00:  RFU
     0x00,  // byte 01:  RFU
     0x00,  // byte 02:  file size
     0x02,  // byte 03:  file size (lsb)
     0x6F,  // byte 04:  file id
     0x3E,  // byte 05:  file id
     0x04,  // byte 06:  file type
     0x00,  // byte 07:  cyclic increase allowed
     0x00,  // byte 08:  file protection (READ/WRITE)
     0x00,  // byte 09:  file protection (INCREASE)
     0x00,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
     0x01,  // byte 11:  file status
     0x02,  // byte 12:  length of the following data
     0x00,  // byte 13:  file structure
     0x00,  // byte 14:  rec len
   } },
 /* 38 */   { UIM_USIM_ADN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3A,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } },
   /* 39 */   { UIM_USIM_FDN, 15,
   {
        0x00,  // byte 00:  RFU
        0x00,  // byte 01:  RFU
        0X00,  // byte 02:  file size
        0X8C,  // byte 03:  file size (lsb)
        0x6F,  // byte 04:  file id (msb)
        0x3B,  // byte 05:  file id (lsb)
        0x04,  // byte 06:  file type
        0x00,  // byte 07:  cyclic increase allowed
        0X11,  // byte 08:  file protection (READ/WRITE)
        0XFB,  // byte 09:  file protection (INCREASE)
        0X22,  // byte 10:  file protection (INVALIDATE/REHABILIATE)
        0X05,  // byte 11:  file status
        0x02,  // byte 12:  length of the following data
        0x01,  // byte 13:  file structure
        0x1C,  // byte 14:  rec len
   } }

};
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
/*lint +e785 */

static nv_cmd_type nvi;           /* nv command buffer */
/* 1234 */
static byte chv1[VIRTUIM_CHV_LEN] = {0x31,0x32,0x33,0x34,0x00,0x00,0x00,0x00};
/* 5678 */
static byte chv2[VIRTUIM_CHV_LEN] = {0x35,0x36,0x37,0x38,0x00,0x00,0x00,0x00};
/* 11111111 */
static byte chv1_unblock[VIRTUIM_CHV_LEN] = {0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31};
/* 22222222 */
static byte chv2_unblock[VIRTUIM_CHV_LEN] = {0x32,0x32,0x32,0x32,0x32,0x32,0x32,0x32};

/* 4321 */
static byte chv1_2[VIRTUIM_CHV_LEN] = {0x34,0x33,0x32,0x31,0x00,0x00,0x00,0x00};
/* 8765 */
static byte chv2_2[VIRTUIM_CHV_LEN] = {0x38,0x37,0x36,0x35,0x00,0x00,0x00,0x00};
/* 33333333 */
static byte chv1_unblock_2[VIRTUIM_CHV_LEN] = {0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33};
/* 44444444 */
static byte chv2_unblock_2[VIRTUIM_CHV_LEN] = {0x34,0x34,0x34,0x34,0x34,0x34,0x34,0x34};
/*
DF Information
BYTE 1-2:  RFU
BYTE 3-4:  Total Memory used by DF
BYTE 5-6:  FILE ID
BYTE  7:   TYPE of File
BYTE 8-12: RFU
BYTE 13:   Number of Bytes to Follow
BYTE 14:   FILE CHARACTERISTICTS
BYTE 18:   RFU
BYTE 19:   CHV1 STATUS
       B7:    1 -> Enabled or 0-> Disabled
           B6-B4: RFU
           B3-B0: Number of attempts left 0 indicates BLOCKED
BYTE 20:   PUK1 Status
           B7:    1 -> Init or 0-> Not Init
           B6-B4: RFU
           B3-B0: Number of attempts left 0 indicates BLOCKED
BYTE 21:   CHV2 STATUS
       B7:    1 -> Init or 0-> Not Init
BYTE 22:   PUK2 STATUS
*/
/* GSM DF SLOT 1 */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
/* CDMA DF SLOT 1 */
#ifdef FEATURE_MMGSDI_CDMA
static byte cdma_df[VIRTUIM_DF_GSM_LEN] =
  {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
#ifdef FEATURE_GSDI_UIM_SERVER_PIN_ENABLED
      /* PIN1 ENABLED */
      0x00,0x00,0x08,0x00,0x00,
#else
      /* PIN1 DISABLED */
      0x00,0x00,0x08,0x80,0x00,
#endif /* FEATURE_GSDI_UIM_SERVER_PIN_ENABLED */
      0x00,0x00,0x00,0x83,0x8A,
      0x83,0x8A,0x00,0x00
  };
#ifdef FEATURE_MMGSDI_DUAL_SLOT
/* CDMA DF SLOT 2 */
static byte cdma_df2[VIRTUIM_DF_GSM_LEN] =
  {
      0x00,0x00,0x00,0xFF,0x7F,
      0x25,0x00,0x00,0x00,0x00,
#ifdef FEATURE_GSDI_UIM_SERVER_PIN_ENABLED_2
      /* PIN1 ENABLED */
      0x00,0x00,0x08,0x00,0x00,
#else
      /* PIN1 DISABLED */
      0x00,0x00,0x08,0x80,0x00,
#endif /* FEATURE_GSDI_UIM_SERVER_PIN_ENABLED */
      0x00,0x00,0x00,0x83,0x8A,
      0x83,0x8A,0x00,0x00
  };

#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /*FEATURE_MMGSDI_CDMA */

#define VIRTUAL_SIM_CHANNEL_ID_EXTRACTION_MASK  0x03
#define VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED      0x04

typedef struct {
  boolean opened;
  int     app_id;
}virtual_sim_channel_table_type;
extern virtual_sim_channel_table_type virtual_sim_channel_table[VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED] =
{
  {TRUE,   0},/* False - Channel Availalbe TRUE - Channel in Use */
  {FALSE,  0},/* False - Channel Availalbe TRUE - Channel in Use */
  {FALSE,  0},/* False - Channel Availalbe TRUE - Channel in Use */
  {FALSE,  0}/* False - Channel Availalbe TRUE - Channel in Use */
};

LOCAL void virtuim_verify_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
);

LOCAL void virtuim_enable_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
);

LOCAL void virtuim_disable_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
);

LOCAL  void virtuim_unblock_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * unblock_chv,
    byte              * new_chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
);


LOCAL void  virtuim_change_chv(
  uim_slot_type       slot,
  uim_chv_type        chv1_or_chv2,
  byte              * old_chv,
  byte              * new_chv,
  uim_rpt_status    * rpt_status_p,
  uim_sw1_type      * sw1_p,
  uim_sw2_type      * sw2_p
);

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_is_channel_open

DESCRIPTION
  Function is used to see if a requested channel is availalbe in the
  virtual sim channel table.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_is_channel_open(
  byte      channel_id,
  boolean * channel_status
)
{
  if ( channel_id > VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED )
    return GSDI_INCORRECT_PARAMS;

  *channel_status = virtual_sim_channel_table[channel_id].opened;
  return GSDI_SUCCESS;
} /* virtual_sim_is_channel_open */




/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_is_channel_open

DESCRIPTION
  Function is used to validate the slot from GSDI to UIM.

  Parameters:
  uim_slot_type slot

DEPENDENCIES
  None

RETURN VALUE
  boolean  TRUE:  Slot is valid
           FALSE: Slot is not valid
===========================================================================*/
boolean virtual_sim_validate_slot(
  uim_slot_type slot
)
{
  switch (slot)
  {
  case UIM_SLOT_1:
    return TRUE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case UIM_SLOT_2:
    return TRUE;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  case UIM_SLOT_NONE:
  case UIM_SLOT_AUTOMATIC:
  default:
    return FALSE;
  }
} /* virtual_sim_validate_slot */


/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_is_mc_action_valid

DESCRIPTION
  Function is used to validate the Manage Channel sent from MMGSDI to UIM

  typedef enum {
  UIM_MC_OPEN   = 0,    Manage Channel Open
  UIM_MC_CLOSE  = 1     Manage Channel Close
  }uim_mc_action_type;

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE
  boolean  TRUE:  MC Action is valid
           FALSE: MC Action is not valid
===========================================================================*/
boolean virtual_sim_is_mc_action_valid(
  uim_mc_action_type mc_action
  )
{
  switch (mc_action)
  {
  case UIM_MC_OPEN:
  case UIM_MC_CLOSE:
    return TRUE;

  default:
    return FALSE;
  }
} /* virtual_sim_is_mc_action_valid */


/*===========================================================================
FUNCTION virtual_sim_get_requested_channel

DESCRIPTION
  Function is used to set the requested channel ID to TRUE to indicate
  it is now in use.

  Parameters:
  byte channel_id

DEPENDENCIES
  Parameter checking is expected to occur before this function is called.

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_get_requested_channel (
  byte channel_id
)
{
  if ( virtual_sim_channel_table[channel_id].opened == TRUE )
    return GSDI_ERROR;
  else if ( channel_id >= VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED )
    return GSDI_INCORRECT_PARAMS;
  else
    virtual_sim_channel_table[channel_id].opened = TRUE;

  return GSDI_SUCCESS;
}
/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_get_next_channel

DESCRIPTION
  Function is used to retrieve the next channel available.  If no more
  channels are available, a GSDI_ERROR is generated.

  Parameters:
  byte * channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_get_next_channel(
  byte * channel_id
)
{
  gsdi_returns_T gsdi_status = GSDI_ERROR;
  byte           i           = 0;

  while ( i < VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED )
  {
    if (virtual_sim_channel_table[i].opened == FALSE)
    {
      gsdi_status = GSDI_SUCCESS;
      *channel_id = i;
      virtual_sim_channel_table[i].opened = TRUE;
      break;
    }
    i++;
  }
  return gsdi_status;
}
/* <EJECT> */
/*===========================================================================
FUNCTION virtual_get_uim_chv_number

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtuim_get_uim_chv_number(byte p2, uim_chv_type *chv, byte *data_p)
{
  p2 &= 0x1F;

  switch (p2) {
  case 0x01:
    *chv = UIM_CHV1;
    return GSDI_SUCCESS;
  case 0x02:
    *chv = UIM_CHV2;
    return GSDI_SUCCESS;
  default:
    data_p[0] = 0x6E;
    data_p[1] = 0x00;
    return GSDI_ERROR;
  }
}


/* <EJECT> */
/*===========================================================================
FUNCTION virtual_chk_lc

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtuim_chk_lc(byte inst, byte lc, byte * data_p)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  switch (inst)
  {
    case SELECT: /* 0xA4 */
      break;
    case STATUS: /* 0xF2 */
    case READ_BINARY: /* 0xB0 */
    case READ_RECORD: /* 0xB2 */
      /* lc not present */
      if (lc != 0) {
        data_p[0] = 0x6A;
        data_p[1] = 0x87;
        gsdi_status = GSDI_ERROR;
      }
      break;
    case UPDATE_BINARY: /* 0xD6 */
    case UPDATE_RECORD:  /* 0xDC */
    case SEEK: /* 0xA2 */
    case INCREASE: /* 0x32 */
    case INVALIDATE: /* 0x04 */
    case REHABILITATE: /* 0x44 */
      break;
    case VERIFY_CHV: /* 0x20 */
    case DISABLE_CHV: /* 0x26 */
    case ENABLE_CHV: /* 0x28 */
      if (lc != 8) {
        data_p[0] = 0x6A;
        data_p[1] = 0x87;
        gsdi_status = GSDI_ERROR;
      }
      break;
    case CHANGE_CHV: /* 0x24 */
    case UNBLOCK_CHV: /* 0x2C */
      if (lc != 16) {
        data_p[0] = 0x6A;
        data_p[1] = 0x87;
        gsdi_status = GSDI_ERROR;
      }
      break;
  }
  return gsdi_status;
}

/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_process_data_buffer

DESCRIPTION
  Function is used to parse the APDU Buffer and provide a corresponding
  sent of data.

DEPENDENCIES
  None

RETURN VALUE
  gsdi_returns_T
===========================================================================*/
gsdi_returns_T virtual_sim_process_data_buffer(
  uim_slot_type slot,
  byte  num_data_bytes,
  byte * apdu_data_p,
  byte * returned_data_len_p,
  byte * data_p
)
{
  uim_chv_type     chv;
  uim_rpt_status   rsp = UIM_PASS;
  int              lc=0;
  int              le=0;
  byte             p2= apdu_data_p[3];
  byte             inst = apdu_data_p[1];
  gsdi_returns_T   gsdi_status = GSDI_SUCCESS;
  uint8            channel_id;

  if (num_data_bytes == 5) {
    if (apdu_data_p[4] > 0) {
      le = apdu_data_p[4];
    }
    else {
      *returned_data_len_p = 2;
      data_p[0] = 0x6A;
      data_p[1] = 0x80; /* Incorrect parameter in data field */
      return GSDI_ERROR;
    }
  }
  else if (num_data_bytes > 5) {
    lc = apdu_data_p[4];
    if (num_data_bytes == (lc + 5 + 1)) {
      le = apdu_data_p[lc+5];
    }
    else if (num_data_bytes == (lc + 5)) {
      le = 0;
    }
    else {
      data_p[0] = 0x6A;
      data_p[1] = 0x80;
      return GSDI_ERROR;
    }
  }

  /* simple parsing for instruction class */
  switch (inst)
  {
  case SELECT: /* 0xA4 */
  case STATUS: /* 0xF2 */
  case READ_BINARY: /* 0xB0 */
  case UPDATE_BINARY: /* 0xD6 */
  case READ_RECORD: /* 0xB2 */
  case UPDATE_RECORD:  /* 0xDC */
  case SEEK: /* 0xA2 */
  case INCREASE: /* 0x32 */
  case INVALIDATE: /* 0x04 */
  case REHABILITATE: /* 0x44 */
    channel_id = apdu_data_p[0] & 0x0F;
    switch ( channel_id )
    {
    case 0:
      /* Always return the same data */
      *returned_data_len_p = 7;
      data_p[0] = 0x00;
      data_p[1] = 0x01;
      data_p[2] = 0x02;
      data_p[3] = 0x03;
      data_p[4] = 0x04;
      data_p[5] = 0x90;
      data_p[6] = 0x00;
      break;
    case 1:
      *returned_data_len_p = 7;
      data_p[0] = 0x01;
      data_p[1] = 0x02;
      data_p[2] = 0x03;
      data_p[3] = 0x04;
      data_p[4] = 0x05;
      data_p[5] = 0x90;
      data_p[6] = 0x00;
      break;
    case 2:
      *returned_data_len_p = 7;
      data_p[0] = 0x02;
      data_p[1] = 0x03;
      data_p[2] = 0x04;
      data_p[3] = 0x05;
      data_p[4] = 0x06;
      data_p[5] = 0x90;
      data_p[6] = 0x00;
      break;
    case 3:
      *returned_data_len_p = 7;
      data_p[0] = 0x03;
      data_p[1] = 0x04;
      data_p[2] = 0x05;
      data_p[3] = 0x06;
      data_p[4] = 0x07;
      data_p[5] = 0x90;
      data_p[6] = 0x00;
      break;
    default:
      *returned_data_len_p = 2;
      data_p[0] = 0x90;
      data_p[1] = 0x00;
    }
    return GSDI_SUCCESS;

  case VERIFY_CHV: /* 0x20 */
    *returned_data_len_p = 2;
    gsdi_status = virtuim_get_uim_chv_number(p2, &chv, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    gsdi_status = virtuim_chk_lc(inst, lc, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    virtuim_verify_chv(slot, chv, &apdu_data_p[5], &rsp, (uim_sw1_type*)&data_p[0], (uim_sw2_type*)&data_p[1]);
    if (rsp == UIM_FAIL) {
      return GSDI_ERROR;
    }
    return GSDI_SUCCESS;

  case CHANGE_CHV: /* 0x24 */
    *returned_data_len_p = 2;
    gsdi_status = virtuim_get_uim_chv_number(p2, &chv, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    gsdi_status = virtuim_chk_lc(inst, lc, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    virtuim_change_chv(slot, chv, &apdu_data_p[5], &apdu_data_p[13], &rsp, (uim_sw1_type*)&data_p[0], (uim_sw2_type*)&data_p[1]);
    if (rsp == UIM_FAIL) {
      return GSDI_ERROR;
    }
    return GSDI_SUCCESS;
  case DISABLE_CHV: /* 0x26 */
    *returned_data_len_p = 2;
    gsdi_status = virtuim_get_uim_chv_number(p2, &chv, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    gsdi_status = virtuim_chk_lc(inst, lc, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    virtuim_disable_chv(slot, chv, &apdu_data_p[5], &rsp, (uim_sw1_type*)&data_p[0], (uim_sw2_type*)&data_p[1]);
    if (rsp == UIM_FAIL) {
      return GSDI_ERROR;
    }
    return GSDI_SUCCESS;
  case ENABLE_CHV: /* 0x28 */
    *returned_data_len_p = 2;
    gsdi_status = virtuim_get_uim_chv_number(p2, &chv, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    gsdi_status = virtuim_chk_lc(inst, lc, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    virtuim_enable_chv(slot, chv, &apdu_data_p[5], &rsp, (uim_sw1_type*)&data_p[0], (uim_sw2_type*)&data_p[1]);
    if (rsp == UIM_FAIL) {
      return GSDI_ERROR;
    }
    return GSDI_SUCCESS;
  case UNBLOCK_CHV: /* 0x2C */
    *returned_data_len_p = 2;
    gsdi_status = virtuim_get_uim_chv_number(p2, &chv, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    gsdi_status = virtuim_chk_lc(inst, lc, data_p);
    if (gsdi_status != GSDI_SUCCESS) {
      return gsdi_status;
    }
    virtuim_unblock_chv(slot, chv, &apdu_data_p[5], &apdu_data_p[13], &rsp, (uim_sw1_type*)&data_p[0], (uim_sw2_type*)&data_p[1]);
    if (rsp == UIM_FAIL) {
      return GSDI_ERROR;
    }
    return GSDI_SUCCESS;

  default:
    *returned_data_len_p = 2;
    data_p[0] = 0x6F;
    data_p[1] = 0x00;
    return GSDI_ERROR;
  }
}

#ifdef FEATURE_MMGSDI_JSR_177
/*===========================================================================
FUNCTION virtual_sim_jsr_process

DESCRIPTION
  Function is compared the JST 177 test table in determining if the
  APDU is correct or not.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_jsr_process( byte  num_data_bytes,
  byte * apdu_data_p,
  byte * returned_data_len_p,
  byte * data_p
  )
{
  int i = 0;
  int app = 0;
  uint8 ch_id = apdu_data_p[0] & 0x02;
  /* is it a select command */
  if (apdu_data_p[1] == 0xA4) {
    if (num_data_bytes == 14) {
      if (!memcmp(&cmdApduSelect[1], &apdu_data_p[1], 13)) {
        virtual_sim_channel_table[ch_id].app_id = 1;
      }
    }
    if (num_data_bytes == 14) {
      if (!memcmp(&cmdApduSelect3[1], &apdu_data_p[1], 13)) {
        virtual_sim_channel_table[ch_id].app_id = 3;
      }
    }
    if (num_data_bytes == 14) {
      if (!memcmp(&cmdApduSelect2[1], &apdu_data_p[1], 13)) {
        virtual_sim_channel_table[ch_id].app_id = 2;
      }
    }
    if (num_data_bytes == 21) {
      if (!memcmp(&cmdApduSelect4[1], &apdu_data_p[1], 20)) {
        virtual_sim_channel_table[ch_id].app_id = 1;
      }
    }
  }
  app = virtual_sim_channel_table[ch_id].app_id;
  for (i = 0; i< UIM_JSR_MAX_TEST_NUM; i++)
  {
    if (num_data_bytes == jsr_177_test[i].cmd_len)
    {
      if (app == jsr_177_test[i].app_id) {
        if(!memcmp(&apdu_data_p[1], &jsr_177_test[i].cmd_p[1], (jsr_177_test[i].cmd_len-1)))
        {
          /* match */
          *returned_data_len_p = jsr_177_test[i].rsp_len;
          memcpy(data_p, jsr_177_test[i].rsp_p, *returned_data_len_p);
          return GSDI_SUCCESS;
        }
      }
    }
  }
  MSG_ERROR("Can't find a map", 0, 0, 0);
  *returned_data_len_p = 2;
  data_p[0] = 0x67;
  data_p[1] = 0x00;
  return GSDI_ERROR;
} /* virtual_sim_jsr_process */
#endif /* FEATURE_MMGSDI_JSR_177 */

/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_process_iso7816_apdu

DESCRIPTION
  Function is used to see if a requested channel is availalbe in the
  virtual sim channel table.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_process_iso7816_apdu(
  uim_stream_iso7816_apdu_type *     cmd_ptr,
  byte *                             returned_data_len_p,
  byte *                             data_p
)
{
  gsdi_returns_T gsdi_status       = GSDI_ERROR;
  byte           channel_id        = 0x00;
  boolean        channel_status    = FALSE;
  boolean        is_slot_valid     = FALSE;

  /* Validate tHe Slot ID and MC Action */
  is_slot_valid = virtual_sim_validate_slot(cmd_ptr->hdr.slot);
  if (!is_slot_valid)
    ERR_FATAL("SLOT IS NOT VALID: %x",cmd_ptr->hdr.slot,0,0);

  /* First See if Channel Requested is Opened */
  channel_id        = cmd_ptr->cmd_data[0] & VIRTUAL_SIM_CHANNEL_ID_EXTRACTION_MASK;

  gsdi_status = virtual_sim_is_channel_open(channel_id,&channel_status);

  if ( gsdi_status != GSDI_SUCCESS )
    return GSDI_ERROR;

  if ( !channel_status )
    return GSDI_ERROR;

  if ( cmd_ptr->num_cmd_bytes >= GSDI_MAX_DATA_BLOCK_LEN )
    return GSDI_ERROR;

#ifndef FEATURE_MMGSDI_JSR_177
  gsdi_status = virtual_sim_process_data_buffer(cmd_ptr->hdr.slot,
                                                cmd_ptr->num_cmd_bytes,
                                                cmd_ptr->cmd_data,
                                                returned_data_len_p,
                                                data_p);
#else
  gsdi_status = virtual_sim_jsr_process(cmd_ptr->num_cmd_bytes,
                                        cmd_ptr->cmd_data,
                                        returned_data_len_p,
                                        data_p);
#endif /* FEATURE_MMGSDI_JSR_177 */

  return gsdi_status;

} /* virtual_sim_process_iso_7816_apdu */


/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_process_open_channel

DESCRIPTION
  Function is used to perform the open channel procedures as defined
  in the 7816-4 specifications and 102.221 and WIM Specs.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_process_open_channel(
  uim_slot_type slot,
  byte          channel,
  byte          op_close_channel,
  byte *        returned_data_len_p,
  byte *        data_p
)
{
  gsdi_returns_T gsdi_status         = GSDI_SUCCESS;
  boolean        channel_open        = FALSE;
  byte           assigned_channel_id = 0;


  gsdi_status = virtual_sim_is_channel_open(channel,&channel_open);
  if ( gsdi_status != GSDI_SUCCESS )
  {
    MSG_ERROR("BAD CHANNEL ID RCVD: %x", channel,0,0);
    return gsdi_status;
  }

  if ( !channel_open )
  {
    MSG_ERROR("CHANNEL TO PERFORM ACTION ON NOT OPEN: %x", channel,0,0);
    return GSDI_ERROR;
  }

  if ( op_close_channel != 0x00 )
  {
    gsdi_status = virtual_sim_is_channel_open(op_close_channel,&channel_open);
    if ( gsdi_status != GSDI_SUCCESS)
    {
      MSG_ERROR("BAD CHANNEL ID RCVD: %x",channel,0,0);
      return gsdi_status;
    }

    gsdi_status = virtual_sim_get_requested_channel(op_close_channel);
    *returned_data_len_p = 0;
    return gsdi_status;
  }
  else
  {
    gsdi_status = virtual_sim_get_next_channel(&assigned_channel_id);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      *returned_data_len_p = 0;
      MSG_ERROR("OUT OF CHANNEL IDS",0,0,0);
      return GSDI_ERROR;
    }
    *returned_data_len_p = 1;
    *data_p = assigned_channel_id;
  }
  return gsdi_status;
}


/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_close_channel

DESCRIPTION
  Function is used to perform the open channel procedures as defined
  in the 7816-4 specifications and 102.221 and WIM Specs.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_close_channel(
  byte      channel_id,
  boolean * channel_open_p
)
{
  if ( channel_id >= VIRTUAL_SIM_MAX_CHANNELS_SUPPORTED )
  {
    MSG_ERROR("BAD CHANNEL ID: %x", channel_id,0,0);
    return GSDI_INCORRECT_PARAMS;
  }

  (void)virtual_sim_is_channel_open(channel_id,channel_open_p);
  if ( *channel_open_p )
  {
    virtual_sim_channel_table[channel_id].opened = FALSE;
    virtual_sim_channel_table[channel_id].app_id = 0;
    return GSDI_SUCCESS;
  }
  else
  {
    MSG_ERROR("CHANNEL TO CLOSE IS NOT OPEN",0,0,0);
    return GSDI_ERROR;
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_process_open_channel

DESCRIPTION
  Function is used to perform the open channel procedures as defined
  in the 7816-4 specifications and 102.221 and WIM Specs.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_process_close_channel(
  uim_slot_type slot,
  byte          channel,
  byte *        returned_data_len_p,
  byte *        data_p
)
{
  gsdi_returns_T gsdi_status         = GSDI_SUCCESS;
  boolean        channel_open        = FALSE;

  gsdi_status = virtual_sim_close_channel(channel,&channel_open);

  if ( gsdi_status != GSDI_SUCCESS )
    MSG_ERROR("COULD NOT CLOSE CHANNEL ID: %x",channel,0,0);

  return gsdi_status;
}
/* <EJECT> */
/*===========================================================================
FUNCTION virtual_sim_is_channel_open

DESCRIPTION
  Function is used to see if a requested channel is availalbe in the
  virtual sim channel table.

  Parameters:
  byte channel_id

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T virtual_sim_process_manage_channel_req(
  uim_manage_channel_cmd_type * cmd_ptr,
  byte *                        returned_data_len_p,
  byte *                        data_p
)
{
  gsdi_returns_T gsdi_status       = GSDI_SUCCESS;
  boolean       is_slot_valid      = FALSE;
  boolean       is_mc_action_valid = FALSE;

  /* Validate tHe Slot ID and MC Action */
  is_slot_valid = virtual_sim_validate_slot(cmd_ptr->hdr.slot);
  if (!is_slot_valid)
    ERR_FATAL("SLOT IS NOT VALID: %x",cmd_ptr->hdr.slot,0,0);

  is_mc_action_valid = virtual_sim_is_mc_action_valid(cmd_ptr->mc_action);
  if (!is_mc_action_valid)
    ERR_FATAL("MC ACTION IS NOT VALID: %x",cmd_ptr->mc_action,0,0);

  switch ( cmd_ptr->mc_action )
  {
  case UIM_MC_OPEN:
    MSG_HIGH("VIRTUAL SIM : OPEN CHANNEL REQ",0,0,0);
    gsdi_status = virtual_sim_process_open_channel(cmd_ptr->hdr.slot,
                                                   cmd_ptr->channel,
                                                   cmd_ptr->op_close_channel,
                                                   returned_data_len_p,
                                                   data_p);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MSG_ERROR("OPEN CHANNEL REQ FAILURE: %x",
        gsdi_status,cmd_ptr->channel,cmd_ptr->op_close_channel);
    }

    break;

  case UIM_MC_CLOSE:
    MSG_HIGH("VIRTUAL SIM : OPEN CHANNEL REQ",0,0,0);
    gsdi_status = virtual_sim_process_close_channel(cmd_ptr->hdr.slot,
                                                    cmd_ptr->op_close_channel,
                                                    returned_data_len_p,
                                                    data_p);
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MSG_ERROR("OPEN CHANNEL REQ FAILURE: %x",
        gsdi_status,cmd_ptr->channel,cmd_ptr->op_close_channel);
    }
    break;

  default:
    ERR_FATAL("BAD MC Action: %x",cmd_ptr->mc_action,0,0);
    break;
  }


  return gsdi_status;
}

#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
/* <EJECT> */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION uim_get_slot_for_app

DESCRIPTION
  This function fills in the slot information for the application requested
  and returns TRUE if the slot information is valid.

  Parmeters:
  #include “uim.h”
  boolean uim_get_slot_for_app( uim_app_type, *uim_slot_type )


DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
uim_slot_type uim_get_slot_for_app (
  uim_app_type app
)
{

    if ( app == UIM_GSM_APP )
    {
        MSG_HIGH("GSM UIM Op Slot %d ",virtuim_card_status.gsm_op_slot,0,0);
        return virtuim_card_status.gsm_op_slot;
    }
    else
    {
        MSG_HIGH("CDMA UIM Op Slot %d ",virtuim_card_status.cdma_op_slot,0,0);
        return virtuim_card_status.cdma_op_slot;
    }
} /* uim_get_slot_for_app */

#endif /* FEATURE_MMGSDI_DUAL_SLOT */
/*===========================================================================
FUNCTION uim_server_read_response

DESCRIPTION
  Will Set the Response for the Read Access

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void uim_server_build_response(
  gsdi_returns_T    gsdi_status,
  uim_cmd_type    * cmd_ptr,
  uim_rpt_type    * uim_rpt_buf_ptr,
  byte              returned_record_num,
  byte              returned_data_len,
  byte            * data_ptr
)
{
  if ( cmd_ptr         == NULL ||
       uim_rpt_buf_ptr == NULL  )
  {
    MSG_ERROR("NULL POINTER",0,0,0);
    return;
  }

  switch ( cmd_ptr->hdr.command )
  {
  case UIM_READ_BIN_CMD_F:
  case UIM_READ_REC_CMD_F:
    if (cmd_ptr->hdr.command == UIM_READ_BIN_CMD_F)
      uim_rpt_buf_ptr->rpt_type = UIM_READ_BIN_CMD_R;
    else
      uim_rpt_buf_ptr->rpt_type = UIM_READ_REC_CMD_R;

    if(data_ptr == NULL)
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
      gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_SUCCESS )
    {
      uim_rpt_buf_ptr->rpt_status = UIM_PASS;
      uim_rpt_buf_ptr->sw1 = 0x90;
      uim_rpt_buf_ptr->sw2 = 0x00;
      uim_rpt_buf_ptr->rpt.read.num_bytes_read   = returned_data_len;
      uim_rpt_buf_ptr->rpt.read.data = mmgsdi_malloc(returned_data_len);
      (void) memcpy(uim_rpt_buf_ptr->rpt.read.data,
                  (byte*)data_ptr,
                   returned_data_len);
    }
    else
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
      uim_rpt_buf_ptr->sw1 = 0x6A;
      uim_rpt_buf_ptr->sw2 = 0x82;
#else
      uim_rpt_buf_ptr->sw1 = 0x94;
      uim_rpt_buf_ptr->sw2 = 0x04;
#endif
    }
    break;

  case UIM_WRITE_BIN_CMD_F:
  case UIM_WRITE_REC_CMD_F:
    if ( gsdi_status == GSDI_SUCCESS )
    {
      uim_rpt_buf_ptr->rpt_status = UIM_PASS;
      uim_rpt_buf_ptr->sw1 = 0x90;
      uim_rpt_buf_ptr->sw2 = 0x00;
      uim_rpt_buf_ptr->rpt.write.num_bytes_written   = returned_data_len;
    }
    else
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
      uim_rpt_buf_ptr->sw1 = 0x6A;
      uim_rpt_buf_ptr->sw2 = 0x82;
#else
      uim_rpt_buf_ptr->sw1 = 0x94;
      uim_rpt_buf_ptr->sw2 = 0x04;
#endif
    }

    if (cmd_ptr->hdr.command == UIM_WRITE_BIN_CMD_F)
      uim_rpt_buf_ptr->rpt_type = UIM_WRITE_BIN_CMD_R;
    else
      uim_rpt_buf_ptr->rpt_type = UIM_WRITE_REC_CMD_R;
    break;


  case UIM_ACCESS_F:
    uim_rpt_buf_ptr->rpt_type = UIM_ACCESS_R;
    switch ( cmd_ptr->access_uim.access )
    {
    case UIM_READ_F:
      if ( gsdi_status == GSDI_SUCCESS )
      {
        if(data_ptr == NULL)
        {
          gsdi_status = GSDI_ERROR;
          break;
        }
        uim_rpt_buf_ptr->rpt_status = UIM_PASS;
        uim_rpt_buf_ptr->sw1 = 0x90;
        uim_rpt_buf_ptr->sw2 = 0x00;
        cmd_ptr->access_uim.num_bytes_rsp   = returned_data_len;
        cmd_ptr->access_uim.num_records_rsp = returned_record_num;
        (void) memcpy(cmd_ptr->access_uim.data_ptr,
                    (byte*)data_ptr,
                  cmd_ptr->access_uim.num_bytes_rsp);
      }
      break;

    case UIM_WRITE_F:
      if ( gsdi_status == GSDI_SUCCESS )
      {
        uim_rpt_buf_ptr->rpt_status = UIM_PASS;
        uim_rpt_buf_ptr->sw1 = 0x90;
        uim_rpt_buf_ptr->sw2 = 0x00;
      }
      break;

    case UIM_INC_F:
      if ( gsdi_status == GSDI_SUCCESS )
      {
        if(data_ptr == NULL)
        {
          gsdi_status = GSDI_ERROR;
          break;
        }
        uim_rpt_buf_ptr->rpt_status = UIM_PASS;
        uim_rpt_buf_ptr->sw1 = 0x90;
        uim_rpt_buf_ptr->sw2 = 0x00;
        cmd_ptr->access_uim.num_bytes_rsp   = returned_data_len;
        (void) memcpy(cmd_ptr->access_uim.data_ptr,
                    (byte*)data_ptr,
                  cmd_ptr->access_uim.num_bytes_rsp);
      }
      break;

    default:
      /* TREAT FAILURE AS FILE NOT FOUND */
      gsdi_status = GSDI_ERROR;
      break;
    }

    if ( gsdi_status != GSDI_SUCCESS )
    {
      /* All Accesses will simulate the Same */
      /* error of not found                  */
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
      uim_rpt_buf_ptr->sw1 = 0x6A;
      uim_rpt_buf_ptr->sw2 = 0x82;
#else
      uim_rpt_buf_ptr->sw1 = 0x94;
      uim_rpt_buf_ptr->sw2 = 0x04;
#endif
    }
    break;

  case UIM_CLEAR_CACHE_F:
    uim_rpt_buf_ptr->rpt_type       = UIM_CLEAR_CACHE_R;
    uim_rpt_buf_ptr->cmd_transacted = TRUE;
    uim_rpt_buf_ptr->rpt_status = UIM_PASS;
    uim_rpt_buf_ptr->sw1 = 0x90;
    uim_rpt_buf_ptr->sw2 = 0x00;
    break;

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  case UIM_MANAGE_CHANNEL_F:
    uim_rpt_buf_ptr->rpt_type       = UIM_MANAGE_CHANNEL_R;
    uim_rpt_buf_ptr->cmd_transacted = TRUE;
    if ( gsdi_status == GSDI_SUCCESS )
    {
      uim_rpt_buf_ptr->rpt_status = UIM_PASS;
      uim_rpt_buf_ptr->sw1 = 0x90;
      uim_rpt_buf_ptr->sw2 = 0x00;
      switch(cmd_ptr->manage_channel.mc_action) {
      case UIM_MC_OPEN:
        if(data_ptr == NULL)
        {
          uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
          uim_rpt_buf_ptr->sw1 = 0x69;
          uim_rpt_buf_ptr->sw2 = 0x86;
          return;
        }
        uim_rpt_buf_ptr->rpt.channel.channel_num = data_ptr[0];
        break;
      default:
        break;
      }
    }
    else
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
      uim_rpt_buf_ptr->sw1 = 0x69;
      uim_rpt_buf_ptr->sw2 = 0x86;
    }
    break;

  case UIM_STREAM_ISO7816_APDU_F:
    uim_rpt_buf_ptr->rpt_type       = UIM_STREAM_ISO7816_APDU_R;
    uim_rpt_buf_ptr->cmd_transacted = TRUE;
    if ( gsdi_status == GSDI_SUCCESS )
    {
      uim_rpt_buf_ptr->rpt_status = UIM_PASS;

    }
    else
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
    }
    if(data_ptr == NULL)
    {
      uim_rpt_buf_ptr->rpt_status = UIM_FAIL;
      return;
    }
        
    uim_rpt_buf_ptr->sw1 = data_ptr[returned_data_len-2];
    uim_rpt_buf_ptr->sw2 = data_ptr[returned_data_len-1];
    uim_rpt_buf_ptr->rpt.stream_iso7816_apdu.num_data_bytes = returned_data_len - 2;
    (void) memcpy(uim_rpt_buf_ptr->rpt.stream_iso7816_apdu.data,
                    (byte*)data_ptr,
                    returned_data_len - 2);
    break;
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */


  case UIM_CACHED_SEL_F:
  case UIM_SELECT_F:
    if ( cmd_ptr->hdr.command == UIM_SELECT_F )
    {
      uim_rpt_buf_ptr->rpt_type = UIM_SELECT_R;
    }
    else
    {
      uim_rpt_buf_ptr->rpt_type = UIM_CACHED_SEL_R;
    }
    if(data_ptr == NULL)
    {
      gsdi_status = GSDI_ERROR;
    }

    if ( gsdi_status == GSDI_SUCCESS )
    {
      uim_rpt_buf_ptr->rpt_status = UIM_PASS;
      uim_rpt_buf_ptr->sw1 = 0x90;
      uim_rpt_buf_ptr->sw2 = 0x00;
      uim_rpt_buf_ptr->rpt.select.num_data_bytes = returned_data_len;
      memcpy(uim_rpt_buf_ptr->rpt.select.data,
                   data_ptr,
                   returned_data_len);
    }
    else
    {
      uim_rpt_buf_ptr->rpt.select.num_data_bytes = 0;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
      uim_rpt_buf_ptr->sw1 = 0x6A;
      uim_rpt_buf_ptr->sw2 = 0x82;
#else
      uim_rpt_buf_ptr->sw1 = 0x94;
      uim_rpt_buf_ptr->sw2 = 0x04;
#endif
    }
    break;

  default:
    MSG_ERROR("UNKNOWN MSG TYPE ",0,0,0);
    break;
  }
}

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_read_attr

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_read_attr (
  uim_slot_type         slot,
  uim_items_enum_type   sim_filename,
  int32 *               file_header_len,
  byte *                file_header
)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    uim_sim_file_attr_element_T * read_attr_file_list_p = NULL;

    switch ( slot )
    {
    case UIM_SLOT_1:
      read_attr_file_list_p = gsdi_sim_file_attr;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case UIM_SLOT_2:
      read_attr_file_list_p = gsdi_sim_file_attr2;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
      MSG_ERROR("BAD SLOT OR SLOT NOT SET %x",slot,0,0);
      ERR_FATAL("BAD SLOT OR SLOT NOT SET %x",slot,0,0);
      return GSDI_ERROR;
      break;
    }

    /*
    ** FIND REQUESTED FILE IN SIM
    */
    for( file_num = 0; file_num < UIM_NUM_FILE_ATTR; file_num++ )
    {
        if( read_attr_file_list_p[file_num].filename == sim_filename )
        {
            file_found = TRUE;
            break;
        }
    }
    if (file_found)
    {
        file_header_len[0] = read_attr_file_list_p[file_num].rec_len;
        memcpy(file_header,
               read_attr_file_list_p[file_num].body,
               file_header_len[0]);
    }
    else
    {
        gsdi_status = GSDI_NOT_FOUND;
    }
   return gsdi_status;
} /* uim_server_read_attr */


/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_write_sim

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_write_sim (
  uim_slot_type         slot,
  uim_items_enum_type   sim_filename,
  byte                  data_len,
  byte                  data_offset,
  byte *                data_p
)
{
    boolean         file_found = FALSE;
    boolean         record_found = FALSE;
    byte            requested_record_number;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    int             num_element = 0;

    uim_sim_element_T * uim_sim_p = 0x00;

    switch ( slot )
    {
    case UIM_SLOT_1:
        MSG_LOW("USING SLOT 1 FOR WRITE",0,0,0);
        uim_sim_p   = uim_sim;
        num_element = UIM_NUM_FILES;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case UIM_SLOT_2:
        MSG_LOW("USING SLOT 2 FOR WRITE",0,0,0);
        uim_sim_p   = uim_sim2;
        num_element = UIM_NUM_FILES_2;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
        MSG_ERROR("BAD SLOT ID %x",slot,0,0);
        return GSDI_ERROR;
    }

    /*
    ** FIND REQUESTED FILE IN SIM
    */
    for( file_num = 0; file_num < num_element; file_num++ )
    {
        if( uim_sim_p[file_num].filename == sim_filename )
        {
            file_found = TRUE;
            break;
        }
    }
    /*
    ** WRITE FILE
    */
    if( file_found == FALSE )
    {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }
    else
    {
        requested_record_number = data_offset;
        switch (uim_sim_p[file_num].file_type)
        {
        case VIRTUIM_LINEAR_FIXED:
        case VIRTUIM_CYCLIC:
            if (requested_record_number == 0)
            {
                gsdi_status = GSDI_INCORRECT_PARAMS;
                break;
            }
            /* do not start from 0 since we already found the starting point of the file */
            while( file_num < num_element )
            {
                if( ( uim_sim_p[file_num].filename == sim_filename ) &&
                    ( uim_sim_p[file_num].rec_number == requested_record_number ) )
                {
                    record_found = TRUE;
                    data_offset = 0;
                    break;
                }
                file_num++;
            }
            if (record_found)
            {
                data_len = uim_sim_p[file_num].rec_len;
                memcpy( (uim_sim_p[file_num].body) + data_offset,
                        data_p, data_len );
                gsdi_status = GSDI_SUCCESS;
            }
            else
            {
                gsdi_status = GSDI_NOT_FOUND;
            }
            break;
        case VIRTUIM_TRANSPARENT:
            /* Binary file write */
            if (data_offset > uim_sim_p[file_num].rec_len) {
                gsdi_status = GSDI_ERROR;
            }
            else {
              if (data_offset + data_len > uim_sim_p[file_num].rec_len) {
                  gsdi_status = GSDI_ERROR;
              }
              else {
                memcpy( (uim_sim_p[file_num].body) + data_offset, data_p, data_len );
                gsdi_status = GSDI_SUCCESS;
              }
            }
            break;
        }

        if( data_len == uim_sim_p[file_num].rec_len )
        {
            uim_sim_p[file_num].init = TRUE;
        }

        else
        {
            /*
            ** DON'T SET THE FLAG, AS ONLY PART OF THE RECORD HAS BEEN STORED
            */
            uim_sim_p[file_num].init = FALSE;
        }
        if(sim_filename == UIM_GSM_SST)
        {
            uim_sim_p[file_num].init = TRUE;
            uim_sim_p[file_num].rec_len = data_len;
        }
    }
    return gsdi_status;
} /* uim_server_write_sim */

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_read_sim

DESCRIPTION
  Reads data from the SIM/USIM.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_read_sim (
  uim_slot_type       slot,
  uim_items_enum_type sim_filename,
  int32               requested_data_len,
  uint32              data_offset,
  int32              *number_of_records_p,
  int32              *actual_data_len_p,
  byte *              data_p
)
{
  boolean         file_found = FALSE;
  boolean         record_found = FALSE;
  byte            requested_record_number;
  word            file_num = 0;
  gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
  uim_sim_element_T * imsi_list_p = 0x00;
  uim_sim_element_T * uim_sim_p   = 0x00;
  int             num_element = 0;

  switch ( slot )
  {
  case UIM_SLOT_1:
    MSG_LOW("USING SLOT 1 FOR READ",0,0,0);
//    imsi_list_p = &imsi_list[0];
    uim_sim_p   = uim_sim;
    num_element = UIM_NUM_FILES;
    break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case UIM_SLOT_2:
    MSG_LOW("USING SLOT 2 FOR READ",0,0,0);
  //  imsi_list_p = &imsi_list2[0];
    uim_sim_p   = uim_sim2;
    num_element = UIM_NUM_FILES_2;
    break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  default:
    MSG_ERROR("BAD SLOT ID %x",slot,0,0);
    ERR_FATAL("BAD SLOT ID %x",slot,0,0);
    break;
  }

#if !defined (FEATURE_GSM) && defined (FEATURE_UI)
    byte            nv_imsi_index = 0;
#endif /* !FEATURE_GSM && FEATURE_UI */

    /*
    ** FIND REQUESTED FILE IN SIM
    */
#if !defined(FEATURE_GSM) && defined(FEATURE_UI)
    switch(sim_filename)
    {
    case UIM_GSM_IMSI:
    case UIM_USIM_IMSI:
        MSG_HIGH(" ** REQUEST IMSI",0,0,0);
        /*
        ** Get NV_IMSI_INDEX
        */
        (void) virtuim_get_nv( NV_IMSI_INDEX_I, ( nv_item_type * ) &nv_imsi_index );
        MSG_MED(" -- NV_IMSI_INDEX = %x", nv_imsi_index,0,0);

        /* Verify IMIS_INDEX_I Was not corrupted */
        if (nv_imsi_index > UIM_MAX_IMSIS_IN_CACHE)
        {
            /* Reset to Lab Test IMSIs */
#ifndef FEATURE_GSM
            nv_imsi_index = 0;
#else
#error code not present
#endif /* FEATURE_GSM */
            MSG_MED(" -- RESET NV_IMSI_INDEX_I To = %x", nv_imsi_index,0,0);
            (void) virtuim_put_nv( NV_IMSI_INDEX_I, (nv_item_type *) &nv_imsi_index );
        }
        else
        {
            MSG_MED(" -- NV_IMSI_INDEX_I OK : %x", nv_imsi_index,0,0);
        }
        /* Verify EF-IMSI is INIT */
        if (imsi_list_p[nv_imsi_index].init == FALSE)
        {
            gsdi_status = GSDI_NOT_INIT;
            MSG_MED(" -- EF NOT INIT, status: %x", gsdi_status,0,0);
        }
        else {
            MSG_MED(" -- IMIS IS INIT",0,0,0);
        }
        /* Set the data length of the pointer passed in*/
        if( requested_data_len == 0 )
        {
            actual_data_len_p[0] = imsi_list_p[nv_imsi_index].rec_len;
        }
        else
        {
            actual_data_len_p[0] = requested_data_len;
        }
        /* Is request for Record > 0 */
        if (imsi_list_p[nv_imsi_index].rec_number > 0)
        {
            gsdi_status = GSDI_NOT_SUPPORTED;
            MSG_MED(" -- Don't support more than 1 record, status %x",gsdi_status,0,0);
            return gsdi_status;
        }
        else
        {
            memcpy( data_p, (imsi_list_p[nv_imsi_index].body) + data_offset,
                    actual_data_len_p[0] );
            gsdi_status = GSDI_SUCCESS;
        }
        return gsdi_status;
        break;

    default:
      for( file_num = 0; file_num < num_element; file_num++ )
      {
          if( uim_sim_p[file_num].filename == sim_filename )
          {
              MSG_MED(" -- Found file %x",uim_sim_p[file_num].filename,0,0);
              file_found = TRUE;
              break;
          }
      }
      break;
    }
#else
    for( file_num = 0; file_num < num_element; file_num++ )
    {
        if( uim_sim_p[file_num].filename == sim_filename )
        {
            MSG_MED(" -- Found file %x",uim_sim_p[file_num].filename,0,0);
            file_found = TRUE;
            break;
        }
    }
#endif /* !FEATURE_GSM && FEATURE_UI */

    /*
    ** READ FILE
    */

    if( file_found == FALSE )
    {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }
    else if( uim_sim_p[file_num].init == FALSE )
    {
        gsdi_status = GSDI_NOT_INIT;
    }
    else
    {
        if( requested_data_len == 0 )
        {
            actual_data_len_p[0] = uim_sim_p[file_num].rec_len;
        }
        else
        {
            actual_data_len_p[0] = requested_data_len;
        }
        switch (uim_sim_p[file_num].file_type)
        {
        case VIRTUIM_LINEAR_FIXED:
        case VIRTUIM_CYCLIC:
            requested_record_number = data_offset;
            if (requested_record_number == 0)
            {
                gsdi_status = GSDI_INCORRECT_PARAMS;
                break;
            }
            while( file_num < num_element )
            {
                if( ( uim_sim_p[file_num].filename == sim_filename ) &&
                    ( uim_sim_p[file_num].rec_number == requested_record_number ))
                {
                    record_found = TRUE;
                    data_offset = 0;
                    number_of_records_p[0] = uim_sim_p[file_num].num_records;
                    break;
                }
                file_num++;
            }
            if (record_found)
            {
                actual_data_len_p[0] = uim_sim_p[file_num].rec_len;
                memcpy( data_p,
                        uim_sim_p[file_num].body,
                        actual_data_len_p[0] );
                gsdi_status = GSDI_SUCCESS;
            }
            else
            {
                gsdi_status = GSDI_NOT_FOUND;
            }
            break;
        case VIRTUIM_TRANSPARENT:
            if ( (data_offset <  uim_sim_p[file_num].rec_len ) &&
                 (data_offset + actual_data_len_p[0] > uim_sim_p[file_num].rec_len))
            {
                memcpy(data_p,
                       uim_sim_p[file_num].body + data_offset,
                       uim_sim_p[file_num].rec_len - data_offset);

                actual_data_len_p[0] =  uim_sim_p[file_num].rec_len - data_offset;
                gsdi_status = GSDI_SUCCESS;
            }
            else if ( ( data_offset < uim_sim_p[file_num].rec_len) &&
                      ( data_offset + actual_data_len_p[0] <= uim_sim_p[file_num].rec_len))

            {
                memcpy( data_p,
                       (uim_sim_p[file_num].body) + data_offset,
                       actual_data_len_p[0] );
                gsdi_status = GSDI_SUCCESS;
            }
            else
            {
                /* data_offset is greater than record lenght */
                gsdi_status = GSDI_INCORRECT_PARAMS;
            }
            break;
        }
    }
    return gsdi_status;
} /* uim_server_read_sim */


/* <EJECT> */

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_get_header

DESCRIPTION
  Generic Function to read Data from a SIM / USIM / or RUIM.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_get_header(
  uim_cmd_type *cmd_ptr,
  int32        *returned_data_len_p,
  byte         *data_p
)
{
  uim_slot_type slot = 0x00;
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ( ( cmd_ptr->hdr.slot != UIM_SLOT_1 ) &&
     ( cmd_ptr->hdr.slot != UIM_SLOT_2 ) )
  {
    /* SET Slot Information to Slot 1 */
    ERR_FATAL("SLOT INFO NOT SET TO 1 OR 2",cmd_ptr->hdr.slot,0,0);
  }
  else
  {
    slot = cmd_ptr->hdr.slot;
    MSG_LOW("USING SLOT %x FOR SELECT",slot,0,0);
  }
#else
  slot = UIM_SLOT_1;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  gsdi_status = uim_server_read_attr(slot,
                    cmd_ptr->select.item,
                    returned_data_len_p,
                    data_p);
  return gsdi_status;
}

/*===========================================================================
FUNCTION uim_server_read

DESCRIPTION
  Generic Function to read Data from a SIM / USIM / or RUIM.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_read(
  uim_cmd_name_type cmd_type,
  uim_cmd_type *cmd_ptr,
  int32        *number_of_records_p,
  int32        *actual_data_len_p,
  byte *        data_p
)
{
  gsdi_returns_T gsdi_status;
  uim_slot_type  slot;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ( ( cmd_ptr->hdr.slot != UIM_SLOT_1 ) &&
       ( cmd_ptr->hdr.slot != UIM_SLOT_2 ) )
  {
    /* SET Slot Information to Slot 1 */
    ERR_FATAL("SLOT INFO NOT SET TO 1 OR 2",cmd_ptr->hdr.slot,0,0);
  }
  else
  {
    slot = cmd_ptr->hdr.slot;
  }
#else
  slot = UIM_SLOT_1;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  switch (cmd_type)
  {
  case UIM_ACCESS_F:
    gsdi_status = uim_server_read_sim( slot,
                    cmd_ptr->access_uim.item,
                    cmd_ptr->access_uim.num_bytes,
                    cmd_ptr->access_uim.offset,
                    number_of_records_p,
                    actual_data_len_p,
                    data_p);
    break;
  case UIM_READ_BIN_CMD_F:
    gsdi_status = uim_server_read_sim( slot,
                    cmd_ptr->read_bin.item,
                    cmd_ptr->read_bin.len,
                    cmd_ptr->read_bin.offset,
                    number_of_records_p,
                    actual_data_len_p,
                    data_p);
    break;
  case UIM_READ_REC_CMD_F:
    gsdi_status = uim_server_read_sim( slot,
                    cmd_ptr->read_rec.item,
                    256, // TODO
                    cmd_ptr->read_rec.rec_num,
                    number_of_records_p,
                    actual_data_len_p,
                    data_p);
    break;
  }
  return gsdi_status;
}

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_increase_sim

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_increase_sim (
  uim_slot_type         slot,
  uim_items_enum_type   sim_filename,
  byte                  data_len,
  byte *                data_p,
  byte *                return_data_ptr
)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    byte            curr_value[UIM_ACM_LEN];
    int             num_element = 0;
    int i=0;

    uim_sim_element_T * uim_sim_p = 0x00;

    switch ( slot )
    {
    case UIM_SLOT_1:
        MSG_LOW("USING SLOT 1 FOR INCREASE",0,0,0);
        uim_sim_p   = uim_sim;
        num_element = UIM_NUM_FILES;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case UIM_SLOT_2:
        MSG_LOW("USING SLOT 2 FOR INCREASE",0,0,0);
        uim_sim_p   = uim_sim2;
        num_element = UIM_NUM_FILES_2;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
        MSG_ERROR("BAD SLOT ID %x",slot,0,0);
        return GSDI_ERROR;
    }

    if (sim_filename != UIM_GSM_ACM) {
        return GSDI_ERROR;
    }
    /*
    ** FIND REQUESTED FILE IN SIM
    */
    for( file_num = 0; file_num < num_element; file_num++ )
    {
        if( uim_sim_p[file_num].filename == sim_filename )
        {
            file_found = TRUE;
            break;
        }
    }
    /*
    ** INCREASE FILE
    */
    if( file_found == FALSE )
    {
        gsdi_status = GSDI_NOT_SUPPORTED;
    }
    else
    {
        memcpy(curr_value, uim_sim_p[file_num].body, uim_sim_p[file_num].rec_len);
        /* shift all values */
        for (i = (uim_sim_p[file_num].num_records - 1); i>0; i--)
        {
            memcpy(uim_sim_p[file_num + i].body,
                   uim_sim_p[file_num + i - 1].body,
                   uim_sim_p[file_num + i].rec_len);
        }
        uim_sim_p[file_num].body[0] = curr_value[0] + data_p[0];
        uim_sim_p[file_num].body[1] = curr_value[1] + data_p[1];
        uim_sim_p[file_num].body[2] = curr_value[2] + data_p[2];

        memcpy( return_data_ptr,
                        uim_sim_p[file_num].body,
                        uim_sim_p[file_num].rec_len );
        gsdi_status = GSDI_SUCCESS;
        uim_sim_p[file_num].init = TRUE;
    }
    return gsdi_status;
} /* uim_server_increase_sim */




/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_seek_sim_cache( )

DESCRIPTION
   This function is used for the Virtual UIM to make a best effort attempt
   to simulate a seek on a USIM.

   Current limitations are as follows:
    1.  Can only support SIMPLE SEARCHES
    2.  Records only contain a single record, thus the search pattern
        must be in the body of uim_sim_cache[index to file] if you
        want it to succeed or not be in the body if you want the search to
        Fail.
    3.  Record Offsets are not currently supported.
    4.  Will seek starting at Position 0 of the record.
    5.  Only returns 1 record number.  Record number is 1.


DEPENDENCIES
  virtuim.c :  Information in the uim_sim_cache_element_T
               uim_sim_cache[UIM_NUM_FILES_IN_CACHE] structure.

              This structure will only hold one record per EF.
RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)
===========================================================================*/
/* Add uim_server_seek_cache */
gsdi_returns_T uim_server_seek_sim_cache(
    uim_slot_type        slot,
    uim_protocol_type    protocol,
    uim_items_enum_type  sim_filename,
    uim_seek_type        seek_type,
    uim_srch_mode        srch_mode,
    byte *               srch_pattern_ptr,
    byte                 srch_pattern_len,
    byte                 rec_num,
    byte *               rec_num_accessed_ptr)
{
    boolean         file_found = FALSE;
    word            file_num = 0;
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    int             compare = 1;
    uim_sim_element_T * uim_sim_p   = 0x00;
    int             num_element = 0;
    int i = 0 ;

    *rec_num_accessed_ptr = 0;
    switch ( slot )
    {
    case UIM_SLOT_1:
        MSG_LOW("USING SLOT 1 FOR SEEK",0,0,0);
        uim_sim_p   = uim_sim;
        num_element = UIM_NUM_FILES;
        break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case UIM_SLOT_2:
        MSG_LOW("USING SLOT 2 FOR SEEK",0,0,0);
        uim_sim_p   = uim_sim2;
        num_element = UIM_NUM_FILES_2;
        break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
    default:
        MSG_ERROR("BAD SLOT ID %x",slot,0,0);
        ERR_FATAL("BAD SLOT ID %x",slot,0,0);
        break;
    }

    if (srch_pattern_ptr == NULL)
    {
        ERR(" NULL ptr for srch_pattern_ptr ",0,0,0);
        return GSDI_ERROR;
    }
    /*
    ** FIND REQUESTED FILE IN SIM CACHE
    */
    MSG_MED(" IN UIM_SERVER_SEEK_CACHE FUNCTION",0,0,0);
    for ( file_num = 0; file_num < num_element; file_num++ )
    {
        if ( uim_sim_p[file_num].filename == sim_filename )
        {
            if (uim_sim_p[file_num].file_type == VIRTUIM_TRANSPARENT)
            {
                /* seek cannot be performed on transparent file */
                return GSDI_ERROR;
            }

            if (rec_num == 0) /* 0 => current record, treat as 1 */
            {
              rec_num = 1;
            }

            for ( i = 0; i < uim_sim_p[file_num].num_records; i++)
            {
              if (uim_sim_p[file_num+i].rec_number == rec_num)
              {
                  file_found = TRUE;
                  file_num += i;
                  break;
              }
            }
        }
        if (file_found)
          break;
    }
    if ( file_found == TRUE )
    {
        switch (protocol)
        {
        case UIM_GSM:
        case UIM_CDMA:
            if ((seek_type != UIM_SEEK_TYPE_1) && (seek_type != UIM_SEEK_TYPE_2))
            {
                ERR(" CAN'T DETERMINE SEEK TYPE ",0,0,0);
                return GSDI_ERROR;
            }
            break;

        default: /* WCDMA or UICC */
            if ((srch_mode != UIM_SIMPLE_SRCH ) && (srch_mode != UIM_ENHANCED_SRCH))
            {
                ERR(" CAN'T DETERMINE SEARCH MODE ",0,0,0);
                return GSDI_ERROR;
            }
            break;
        }

        /* Display first character to search */
        /* For Debug Purposes only           */
        MSG_MED(" Searching for %c ",*srch_pattern_ptr,0,0);
        MSG_MED(" Search Len %x ", srch_pattern_len,0,0);
        /*
        ** NOW DETERMINE IF YOU CAN MATCH
        ** WITH THE RECORDS IN YOUR FILE
        */

        for (i = 0; i < uim_sim_p[file_num].rec_len; i++)
        {
            if (srch_pattern_len < (uim_sim_p[file_num].rec_len - i))
            {
              compare = memcmp(srch_pattern_ptr, &(uim_sim_p[file_num].body[i]), srch_pattern_len);
              if (!compare)
              {
                  break;
              }
            }
            else
            {
              break;
            }
        }

        if (!compare)
        {
            *rec_num_accessed_ptr = uim_sim_p[file_num].rec_number;
            gsdi_status = GSDI_SUCCESS;
        }
        else
        {
            gsdi_status = GSDI_NOT_FOUND;
        }
    }
    return gsdi_status;
} /* uim_server_seek_sim_cache */


/*===========================================================================
FUNCTION uim_server_write

DESCRIPTION
  Generic Function to write Data from a SIM / USIM / or RUIM.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_write(
  uim_cmd_type *cmd_ptr
)
{
  gsdi_returns_T gsdi_status;
  uim_slot_type  slot;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ( ( cmd_ptr->hdr.slot != UIM_SLOT_1 ) &&
       ( cmd_ptr->hdr.slot != UIM_SLOT_2 ) )
  {
    /* SET Slot Information to Slot 1 */
    ERR_FATAL("SLOT INFO NOT SET TO 1 OR 2",cmd_ptr->hdr.slot,0,0);
  }
  else
  {
    slot = cmd_ptr->hdr.slot;
  }
#else
  slot = UIM_SLOT_1;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  switch (cmd_ptr->hdr.command)
  {
  case UIM_ACCESS_F:
    gsdi_status = uim_server_write_sim( slot,
                                        cmd_ptr->access_uim.item,
                                        (byte)cmd_ptr->access_uim.num_bytes,
                                        (byte)cmd_ptr->access_uim.offset,
                                        (byte*)cmd_ptr->access_uim.data_ptr);
    break;
  case UIM_WRITE_BIN_CMD_F:
    gsdi_status = uim_server_write_sim( slot,
                                        cmd_ptr->write_bin.item,
                                        (byte)cmd_ptr->write_bin.len,
                                        (byte)cmd_ptr->write_bin.offset,
                                        (byte*)cmd_ptr->write_bin.data);
    break;
  case UIM_WRITE_REC_CMD_F:
    gsdi_status = uim_server_write_sim( slot,
                                        cmd_ptr->write_rec.item,
                                        (byte)cmd_ptr->write_rec.len,
                                        0,
                                        (byte*)cmd_ptr->write_rec.data);
    break;
  }

  return gsdi_status;
} /* uim_server_write */


/*===========================================================================
FUNCTION uim_server_increase

DESCRIPTION
  Generic Function to Increase Data for a SIM / USIM / or RUIM.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
gsdi_returns_T uim_server_increase(
  uim_cmd_type *cmd_ptr,
  byte *        return_data_ptr
)
{
  gsdi_returns_T gsdi_status;
  uim_slot_type  slot;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  if ( ( cmd_ptr->hdr.slot != UIM_SLOT_1 ) &&
       ( cmd_ptr->hdr.slot != UIM_SLOT_2 ) )
  {
    /* SET Slot Information to Slot 1 */
    ERR_FATAL("SLOT INFO NOT SET TO 1 OR 2",cmd_ptr->hdr.slot,0,0);
  }
  else
  {
    slot = cmd_ptr->hdr.slot;
  }
#else
  slot = UIM_SLOT_1;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  gsdi_status = uim_server_increase_sim(  slot,
                                          cmd_ptr->access_uim.item,
                                         (byte)cmd_ptr->access_uim.num_bytes,
                                         (byte*)cmd_ptr->access_uim.data_ptr,
                                         return_data_ptr);

  return gsdi_status;
} /* uim_server_increase */

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_seek( )

DESCRIPTION
   This function is used for the Virtual UIM to make a best effort attempt
   to simulate a seek

DEPENDENCIES
  virtuim.c :  Information in the uim_sim_cache_element_T
               uim_sim_cache[UIM_NUM_FILES_IN_CACHE] structure.

              This structure will only hold one record per EF.
RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)
===========================================================================*/
gsdi_returns_T uim_server_seek(uim_cmd_type *cmd_ptr,
                               byte *rec_num_accessed_ptr)
{
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    uim_slot_type  slot;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    if ( ( cmd_ptr->hdr.slot != UIM_SLOT_1 ) &&
         ( cmd_ptr->hdr.slot != UIM_SLOT_2 ) )
    {
      /* SET Slot Information to Slot 1 */
      ERR_FATAL("SLOT INFO NOT SET TO 1 OR 2",cmd_ptr->hdr.slot,0,0);
    }
    else
    {
      slot = cmd_ptr->hdr.slot;
    }
#else
    slot = UIM_SLOT_1;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = uim_server_seek_sim_cache(slot,
                                            cmd_ptr->hdr.protocol,
                                            cmd_ptr->seek.item,
                                            cmd_ptr->seek.seek_type,
                                            cmd_ptr->seek.srch_mode,
                                            cmd_ptr->seek.srch_pattern,
                                            cmd_ptr->seek.num_bytes,
                                            cmd_ptr->seek.rec_number,
                                            rec_num_accessed_ptr);
    return gsdi_status;
} /* uim_server_seek */

/* <EJECT> */
/*===========================================================================
FUNCTION uim_build_link_established_message

DESCRIPTION
  Used to build the link established message for GSDI to process.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void uim_build_link_established_message(
  gsdi_uim_link_established_T * link_est_cmd_p
)
{
  link_est_cmd_p->message_header.message_set = MS_UIM_SERVER;
  link_est_cmd_p->message_header.message_id  = GSDI_LINK_ESTABLISHED_MSG;
#if defined (FEATURE_MMGSDI_CDMA) && !(defined (FEATURE_MMGSDI_UMTS) || defined(FEATURE_MMGSDI_GSM))
  ((IMH_T *)(&link_est_cmd->message_header))->message_len_msb =
    (unsigned char)((sizeof( gsdi_uim_link_established_T ) - sizeof(IMH_T)) / 0x100);
  ((IMH_T *)(&link_est_cmd_p->message_header))->message_len_lsb =
    (unsigned char)((sizeof( gsdi_uim_link_established_T ) - sizeof(IMH_T)) % 0x100);
#else
  PUT_IMH_LEN( sizeof( gsdi_uim_link_established_T ) - sizeof( IMH_T ),
    &link_est_cmd_p->message_header );
#endif  /* FEATURE_MMGSDI && !(FEATURE_MMGSDI_UMTS || FEATURE_MMGSDI_GSM ) */

#ifdef FEATURE_GSDI_UIM_SERVER_USIM
  link_est_cmd_p->protocol_slot1 = GSDI_PROTOCOL_UICC;
#else
  link_est_cmd_p->protocol_slot1 = GSDI_PROTOCOL_GSM;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  link_est_cmd_p->protocol_slot2 = GSDI_PROTOCOL_GSM;
#endif

#ifdef FEATURE_GSDI_UIM_SERVER_USIM
  /** Enables USIM capability */
  link_est_cmd_p->length_historical_bytes = 7;
  link_est_cmd_p->historical_bytes[0] = 0x80;
  link_est_cmd_p->historical_bytes[1] = 0x31;
  link_est_cmd_p->historical_bytes[2] = 0xA0;
  link_est_cmd_p->historical_bytes[3] = 0x73;
  link_est_cmd_p->historical_bytes[4] = 0xbe;
  link_est_cmd_p->historical_bytes[5] = 0x21;
  link_est_cmd_p->historical_bytes[6] = 0x00;
#else
  /** Disables USIM capability */
  link_est_cmd_p->length_historical_bytes = 0;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
  link_est_cmd_p->length_atr_bytes = 10;
  link_est_cmd_p->atr_bytes[0]     = 0x3F;
  link_est_cmd_p->atr_bytes[1]     = 0x20;
  link_est_cmd_p->atr_bytes[2]     = 0x31;
  link_est_cmd_p->atr_bytes[3]     = 0x32;
  link_est_cmd_p->atr_bytes[4]     = 0x33;
  link_est_cmd_p->atr_bytes[5]     = 0x34;
  link_est_cmd_p->atr_bytes[6]     = 0x35;
  link_est_cmd_p->atr_bytes[7]     = 0x36;
  link_est_cmd_p->atr_bytes[8]     = 0x37;
  link_est_cmd_p->atr_bytes[9]     = 0x38;

  /* slot 1 */

  link_est_cmd_p->gsdi_dfs_present_slot1.gsm_present  = virtuim_card_status.slot1.gsm_present;
  link_est_cmd_p->gsdi_dfs_present_slot1.cdma_present = virtuim_card_status.slot1.cdma_present;
  if ( virtuim_card_status.slot1.slot_present )
  {
      link_est_cmd_p->gsdi_status_slot1 = GSDI_SLOT_PRESENT;
      MSG_HIGH("SLOT 1 PRESENT, GSM %d, CDMA %d",
        virtuim_card_status.slot1.gsm_present,
        virtuim_card_status.slot1.cdma_present,
        0);
  }
  else {
      /* slot 1 not present */
      MSG_HIGH("SLOT 1 NOT PRESENT",0,0,0);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      link_est_cmd_p->gsdi_status_slot1 = GSDI_SLOT_ERROR;
      /* Notify GSDI of this Error */
      gsdi_uim_slot_error_condition ( UIM_ERR_S,
                                    UIM_SLOT_1);
#else
      gsdi_uim_error_condition( UIM_ERR_S );
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* slot 2 */
  link_est_cmd_p->gsdi_dfs_present_slot2.gsm_present  = virtuim_card_status.slot2.gsm_present;
  link_est_cmd_p->gsdi_dfs_present_slot2.cdma_present = virtuim_card_status.slot2.cdma_present;
  if ( virtuim_card_status.slot2.slot_present )
  {
      link_est_cmd_p->gsdi_status_slot2 = GSDI_SLOT_PRESENT;
      MSG_HIGH("SLOT 2 PRESENT, GSM %d, CDMA %d",
        virtuim_card_status.slot2.gsm_present,
        virtuim_card_status.slot2.cdma_present,
        0);
  }
  else {
      /* slot 2 not present */
      MSG_HIGH("SLOT 2 NOT PRESENT",0,0,0);
      link_est_cmd_p->gsdi_status_slot2 = GSDI_SLOT_ERROR;
      gsdi_uim_slot_error_condition ( UIM_ERR_S,
                                    UIM_SLOT_2);
  }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* Set the work waiting time to 3 seconds */
  /* We need to do this here bacause we don't have all the UIM files
  included in our tools, so this timer does not get set */
  link_est_cmd_p->work_waiting_time = VIRTUIM_WORK_WAITING_TIME;


} /* uim_build_link_established_message */

/* <EJECT> */
/*===========================================================================
FUNCTION uim_server_get_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
uim_cmd_type *uim_server_get_cmd_buf(void)
{
    /* Allocate command buffer */
    return (uim_cmd_type*)mmgsdi_malloc(sizeof(uim_cmd_type));
} /* uim_server_get_cmd_buf */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
/* <EJECT> */
/*===========================================================================
FUNCTION virtuim_get_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtuim_cmd_type *virtuim_get_cmd_buf(void)
{
   /* Allocate command buffer */
   return (virtuim_cmd_type*)mmgsdi_malloc(sizeof(virtuim_cmd_type));

} /* virtuim_get_cmd_buf */
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

/* <EJECT> */
/*===========================================================================
FUNCTION virtuim_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_cmd(uim_cmd_type *cmd_ptr)
{
    /* Check for Null pointer */
    if(cmd_ptr != NULL)
    {
        (void) q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);
        /* Put the message on the queue */
        q_put(&uim_server_task_cmd_q, &cmd_ptr->hdr.cmd_hdr.link);
        /* Set the command queue signal for */
        /* receive task                     */
        (void) rex_set_sigs(&gsdi_uim_server_task_tcb, UIM_CMD_Q_SIG);
        MSG_LOW("UIM_CMD_Q_SIG set from put_cmd_buf",0,0,0);
    }
    else
    {
        ERR("uim_put_cmd_buf cmd_ptr is NULL",0,0,0);
    }
} /* uim_cmd */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
/* <EJECT> */
/*===========================================================================
FUNCTION virtuim_app_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_app_cmd(virtuim_cmd_type *cmd_ptr)
{
    /* Check for Null pointer */
    if(cmd_ptr != NULL)
    {
        (void) q_link(cmd_ptr, &cmd_ptr->hdr.link);
        /* Put the message on the queue */
        MSG_LOW("VIRTUIM: CMD ON SAT CMD QUEUE",0,0,0);
        q_put(&uim_virtsat_proactive_cmd_q, &cmd_ptr->hdr.link);
        /* Set the command queue signal for */
        /* receive task                     */
        (void) rex_set_sigs(&gsdi_uim_server_task_tcb,VIRTUIM_SEND_SAT_CMD_SIG );
        MSG_LOW("UIM_CMD_Q_SIG set from put_cmd_buf",0,0,0);
    }
    else
    {
        ERR("uim_put_cmd_buf cmd_ptr is NULL",0,0,0);
    }
} /* uim_app_cmd */
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

/* <EJECT> */
/*===========================================================================
FUNCTION test_sim_server_wait

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
rex_sigs_type test_sim_server_wait (rex_sigs_type  requested_mask)
{
    rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
    rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initialize temporary mask holder for queue information */
    queue_mask = 0;

    /* --------------------------------------------------------------
    ** Check message queue:  If the request mask contains a check for
    ** GSDI_TASK_CMD_Q_SIG then we should check the queue, and set in
    ** the queue_mask an indication that something is waiting to be
    ** serviced on that queue.  The rex signal is also cleared
    ** regardless of the queue count.  This is to cover the case
    ** where there is a signal set but nothing on the queue.
    ** ------------------------------------------------------------- */
    if (requested_mask & UIM_CMD_Q_SIG)
    {
        (void) rex_clr_sigs( &gsdi_uim_server_task_tcb, UIM_CMD_Q_SIG );
        if (q_cnt( &uim_server_task_cmd_q ))
        {
            queue_mask |= UIM_CMD_Q_SIG;
        }
    }
    /* Get current state of rex signals */
    rex_signals_mask = rex_get_sigs( &gsdi_uim_server_task_tcb );

    /* --------------------------------------------------------------------
    ** Loop until the current signals from REX (including the queue signals
    ** faked due to an item on a queue, if any) match with one or more of
    ** the requested signals.
    ** -------------------------------------------------------------------- */
    do
    {
#ifdef FEATURE_DOG
        /* Kick the watchdog */
        dog_report( DOG_UIM_RPT );
        (void) rex_set_timer( &uim_rpt_timer_alternate, DOG_UIM_RPT_TIME );
#endif
        /* Perform the 'real' wait (if needed) */
        if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
        {
            rex_signals_mask = rex_wait( requested_mask | UIM_RPT_TIMER_SIG );
        }
        /* ---------------------------------------------------------------
        ** Make sure that if we got a queue signal, that there is actually
        ** an item on the queue indicated
        ** --------------------------------------------------------------- */
        if (rex_signals_mask & UIM_CMD_Q_SIG)
        {
            (void) rex_clr_sigs(&gsdi_uim_server_task_tcb, UIM_CMD_Q_SIG);
            if (q_cnt(&uim_server_task_cmd_q) == 0)   /* No elements in queue ? */
            {
                rex_signals_mask &= ~UIM_CMD_Q_SIG;  /* Turn off bit */
            }
        }

        /* do while mask requested does not match current mask */
    } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

    return ((rex_signals_mask | queue_mask));
} /* test_sim_server_wait */

/* <EJECT> */
/*===========================================================================
FUNCTION VIRTUIM_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type virtuim_get_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
    MSG_MED(" -- In VIRTUIM_GET_NV",0,0,0);
    /*
    ** notify this task when done
    */
    nvi.tcb_ptr = rex_self();
    nvi.sigs = VIRTUIM_NV_SIG;
    /*
    ** command goes on no queue when done
    */
    nvi.done_q_ptr = NULL;
    /*
    ** item to read
    */
    nvi.item = item;
    nvi.cmd = NV_READ_F;
    /*
    ** Set the Data Ptr for NV ITEM TO read
    */
    nvi.data_ptr = data_ptr;
    /*
    ** Clear REX Signals
    */
    (void) rex_clr_sigs( rex_self(), VIRTUIM_NV_SIG );
    /*
    ** Send Command to NV
    */
    nv_cmd( &nvi );
    /*
    ** Wait for Signal from NV that it's complete
    */
    virtuim_wait_nv();

    MSG_MED(" -- NV STATUS = %x",nvi.status,0,0);
    if( nvi.status != NV_DONE_S )
    {
        ERR( "NV Write Failed Item %d Code %d", nvi.item, nvi.status, 0 );
    }
    return ( nvi.status );
}

/* <EJECT> */
/*===========================================================================
FUNCTION VIRTUIM_PUT_NV

DESCRIPTION
  Put an item on to the nonvolatile memory.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
nv_stat_enum_type virtuim_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
)
{
    if ( data_ptr == NULL )
    {
        return NV_FAIL_S;
    }
    MSG_MED(" -- In VIRTUIM_PUT_NV",0,0,0);
    /*
    ** notify this task when done
    */
    nvi.tcb_ptr = rex_self();
    nvi.sigs = VIRTUIM_NV_SIG;
    /*
    ** command goes on no queue when done
    */
    nvi.done_q_ptr = NULL;
    /*
    ** item to read
    */
    nvi.item = item;
    nvi.cmd = NV_WRITE_F;
    /*
    ** Clear REX Signals
    */
    (void) rex_clr_sigs( rex_self(), VIRTUIM_NV_SIG );
    /*
    ** Send Command to NV
    */
    nv_cmd( &nvi );
    /*
    ** Wait for Signal from NV that it's complete
    */
    virtuim_wait_nv();
    MSG_MED(" -- NV STATUS = %x",nvi.status,0,0);
    if( nvi.status != NV_DONE_S )
    {
        ERR( "NV Write Failed Item %d Code %d", nvi.item, nvi.status, 0 );
    }
    return ( nvi.status );
}

/* <EJECT> */
/*===========================================================================
FUNCTION UI_WAIT_NV

DESCRIPTION
  Wait on NV actions.  Handle signals as they come in just in case NV
  takes a while.  Only time critical watchdog.

DEPENDENCIES
  There is a command sent to NV, and this task will be signalled.  There
  are no other outstanding NV requests from this task.

RETURN VALUE
  None

SIDE EFFECT
  Some signals are ignored till we're done with NV

===========================================================================*/
static void virtuim_wait_nv( void )
{
    rex_sigs_type sigs;             /* hold signals from rex_wait() */

    for ( ;; )
    {
        /*
        ** Wait for NV Command to Complete
        */
        sigs = rex_wait( VIRTUIM_NV_SIG );
        if( sigs & VIRTUIM_NV_SIG )
        {
            /*
            ** NV ACTION (GET or PUT) Completed
            */
            MSG_HIGH(" ** GOT SIGS %x : %x",sigs, VIRTUIM_NV_SIG,0);
            return;
        }
    }
}


/* <EJECT> */
/*===========================================================================
FUNCTION virtuim_get_ef_from_path

DESCRIPTION
   This function is used for the Virtual UIM Only.  This function will used
   the cmd_ptr passed to it to determine the type of command so that the
   correct path is resolved.  It will resolve the PATH/FID assumming that
   the information in the EF-PBR File in Virtuim.c never changes.

DEPENDENCIES
  virtuim.c :  UIM_USIM_PBR   Information is assummed to not change
         UIM_TELECOM_PBR  Information is assummed to not change

         WARNING:  Changing the FIDs in the EF-PBR Will create undersired
         results in the operation of this function.

RETURN VALUE
  gsdi_returns_T:  GSDI_SUCCESS or ERROR(s)
===========================================================================*/
gsdi_returns_T virtuim_get_ef_from_path( uim_cmd_type *cmd_ptr )
{
    gsdi_returns_T  gsdi_status = GSDI_SUCCESS;
    word path_0_DF      = 0;
    word path_2_FID     = 0;
    boolean global      = FALSE;
    uim_items_enum_type file_name = 0x00;

    /* Check for Null Pointer */
    if (cmd_ptr == NULL)
    {
      return GSDI_ERROR;
    }
    /* Get Path[0] and Path[2] from the appropriate command */
    path_0_DF  = cmd_ptr->access_uim.path[0];
    path_2_FID = cmd_ptr->access_uim.path[2];

    /* Check to see if we are using global EFs or */
    /* local EFs                  */
    switch (path_0_DF )
    {
    case 0x7F10:
        /* Telecom DF */
        global = TRUE;
        break;
    case 0xFFFF:
        /* USIM ADF */
        global = FALSE;
        break;
    default:
        ERR(" -- Can't determine DF %x",path_0_DF,0,0);
        gsdi_status = GSDI_ERROR;
        break;
    }

    if ( gsdi_status == GSDI_SUCCESS )
    {
        /* Now determine the EF from the PATH/FID in path_2_FID */
        switch (path_2_FID)
        {
        case 0x4F3A:
            if (global)
            {
                MSG_MED(" -- Set file_name UIM_TELECOM_ADN",0,0,0);
                file_name = UIM_TELECOM_ADN;
            }
            else
            {
                MSG_MED(" -- Set file_name UIM_USIM_ADN",0,0,0);
                file_name = UIM_USIM_ADN;
            }
            break;

        case 0x4F25:
            if (global)
            {
                MSG_MED(" -- Set file_name UIM_TELECOM_IAP",0,0,0);
                file_name = UIM_TELECOM_IAP;
            }
            else
            {
                MSG_MED(" -- Set file_name UIM_USIM_IAP",0,0,0);
                file_name = UIM_USIM_IAP;
            }
            break;

        default:
            ERR(" -- Can't resolve FID %x, ",path_2_FID,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        }
    }
    /* Now overwrite UIM_EF_BY_PATH with file_name in command */
    if (cmd_ptr->hdr.command == UIM_ACCESS_F)
    {
        cmd_ptr->access_uim.item = file_name;
    }
    else if (cmd_ptr->hdr.command == UIM_SELECT_F ||
             cmd_ptr->hdr.command == UIM_CACHED_SEL_F)
    {
        cmd_ptr->select.item = file_name;
    }
    else if (cmd_ptr->hdr.command == UIM_SEEK_F)
    {
        cmd_ptr->seek.item = file_name;
    }
    else
    {
        /* A command which does not need a path resolved */
        return GSDI_ERROR;
    }
    return gsdi_status;
} /* virtuim_get_ef_from_path */


#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
/* <EJECT> */
/*===========================================================================
FUNCTION VIRTUIM_VIRTSAT_REPORT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
void virtuim_virtsat_report (
  virtsat_report_type *report
)
{
    MSG_HIGH("VIRTUIM: In virtuim_virtsat_report function",0,0,0);
    virtsat_report.status_report.sw1 = report->status_report.sw1;
    virtsat_report.status_report.sw2 = report->status_report.sw2;
    virtsat_report.status_report.virtsat_result = report->status_report.virtsat_result;
    virtsat_report.status_report.uim_rpt_function = report->status_report.uim_rpt_function;
    virtsat_report.num_bytes = report->num_bytes;
    if (report->num_bytes > 0)
    {
      memcpy(virtsat_report.data,
        report->data,
        report->num_bytes);
    }
    (void) rex_set_sigs(&gsdi_uim_server_task_tcb, VIRTUIM_WAIT_FOR_VIRTSAT_SIG);
} /* virtuim_virtsat_report */


/*===========================================================================
FUNCTION VIRTUIM_SEND_CMD_TO_VIRTSAT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
LOCAL virtsat_report_type virtuim_send_cmd_to_virtsat (
  virtsat_transaction_type *virtsat_cmd_ptr
)
{
    MSG_LOW("VIRT SIM: LINKING TO SAT QUEUE",0,0,0);
    (void)q_link(virtsat_cmd_ptr, &virtsat_cmd_ptr->hdr.link);

    virtsat_cmd_ptr->hdr.virtsat_rpt_function = virtuim_virtsat_report;

    (void)rex_clr_sigs(&gsdi_uim_server_task_tcb, VIRTUIM_WAIT_FOR_VIRTSAT_SIG);

    virtsat_cmd(virtsat_cmd_ptr);

    MSG_LOW("VIRTUIM:  Waiting for VIRTUIM_WAIT_FOR_VIRTSAT_SIG",0,0,0);

    (void) test_sim_server_wait(VIRTUIM_WAIT_FOR_VIRTSAT_SIG);

    (void)rex_clr_sigs(&gsdi_uim_server_task_tcb, VIRTUIM_WAIT_FOR_VIRTSAT_SIG);

    MSG_LOW("VIRTUIM:  GOT VIRTUIM_WAIT_FOR_VIRTSAT SIG",0,0,0);

    return(virtsat_report);

} /* gsdi_send_cmd_to_uim_server */

/*===========================================================================
FUNCTION virtuim_send_to_virtsat

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
LOCAL gsdi_returns_T virtuim_send_to_virtsat(
  uim_cmd_type *cmd_ptr,
  uim_rpt_type *uim_rpt_buf_ptr
)
{
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;
    static virtsat_transaction_type     *virtsat_cmd_ptr;

    MSG_HIGH("VIRTUIM:  ------------------------------------",0,0,0);
    MSG_LOW("VIRTUIM:  Enter function ",0,0,0);
    MSG_LOW("VIRTUIM:  virtuim_send_to_virtsat",0,0,0);

    uim_rpt_buf_ptr->user_data = cmd_ptr->hdr.user_data;
    /* Lets get a buffer for the request */
    if ((virtsat_cmd_ptr = virtsat_get_cmd_buf()) != NULL)
    {
        MSG_LOW("GOT VIRTSAT COMMAND BUFFER",0,0,0);

        /* Copy necessary Data and Send to Virtual Sat */
        switch ( cmd_ptr->hdr.command )
        {
        case UIM_RESET_F:
            MSG_HIGH("VIRT SIM: GOT RESET ",0,0,0);
            /* Set the Transaction Type */
            virtsat_cmd_ptr->hdr.transaction = VIRTSAT_RESET_F;

            /* Copy Protoco Information */
            virtsat_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;

            /* Virtal SAT to call Virtual SIM Call Back */
            virtsat_cmd_ptr->hdr.virtsat_rpt_function = virtuim_virtsat_report;

            /* Copy Call Back for Virtual SIM to Call */
            virtsat_cmd_ptr->hdr.uim_rpt_function = cmd_ptr->hdr.rpt_function;

            /* Now send it to the Virtual Sat App */
            (void)virtuim_send_cmd_to_virtsat(virtsat_cmd_ptr);

            /* Globals are Set for Virtual Sat Response */
            /* Copy to UIM Report Type and use callback */
            uim_rpt_buf_ptr->rpt_type = UIM_TERMINAL_PROFILE_R;
            if ( virtsat_report.status_report.virtsat_result == VIRTSAT_SUCCESS)
            {
                uim_rpt_buf_ptr->rpt_status = UIM_PASS;
            }
            uim_rpt_buf_ptr->sw1 = 0;
            uim_rpt_buf_ptr->sw2 = 0;

            break;

        case UIM_TERMINAL_PROFILE_F:
            MSG_HIGH("VIRT SIM: GOT TERM PROF DL",0,0,0);
            /* Set the Transaction Type */
            virtsat_cmd_ptr->hdr.transaction = VIRTSAT_TERMINAL_PROFILE_F;

            /* Copy Protocol Information */
            virtsat_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;
            /* Virtual SAT to Call Virtual SIM Call Back */
            virtsat_cmd_ptr->hdr.virtsat_rpt_function = virtuim_virtsat_report;

            /* Copy Call Back for Virtual SIM to Call */
            virtsat_cmd_ptr->hdr.uim_rpt_function =
                cmd_ptr->hdr.rpt_function;

            /* Copy Number of bytes */
            virtsat_cmd_ptr->terminal_profile.num_bytes =
                cmd_ptr->terminal_profile.num_bytes;

            /* Copy the Terminal Profile Data */
            memcpy(virtsat_cmd_ptr->terminal_profile.data,
                   cmd_ptr->terminal_profile.data,
                   cmd_ptr->terminal_profile.num_bytes);

            /* Now send it to the Virtual Sat App */
            (void)virtuim_send_cmd_to_virtsat(virtsat_cmd_ptr);

            /* Globals are Set for Virtual Sat Response */
            /* Copy to UIM Report Type and use callback */
            uim_rpt_buf_ptr->rpt_type = UIM_TERMINAL_PROFILE_R;
            if ( virtsat_report.status_report.virtsat_result == VIRTSAT_SUCCESS)
            {
                uim_rpt_buf_ptr->rpt_status = UIM_PASS;
            }
            uim_rpt_buf_ptr->sw1 = virtsat_report.status_report.sw1;
            uim_rpt_buf_ptr->sw2 = virtsat_report.status_report.sw2;


            break;

        case UIM_ENVELOPE_F:
            MSG_HIGH("VIRT SIM: GOT ENVELOPE CMD",0,0,0);
            /* Set the Transaction Type */
            virtsat_cmd_ptr->hdr.transaction = VIRTSAT_ENVELOPE_F;

            /* Copy Protocol Information */
            virtsat_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;
            /* Virtual SAT to Call Virtual SIM Call Back */
            virtsat_cmd_ptr->hdr.virtsat_rpt_function = virtuim_virtsat_report;

            /* Copy Call Back for Virtual SIM to Call */
            virtsat_cmd_ptr->hdr.uim_rpt_function =
                cmd_ptr->hdr.rpt_function;

            /* Copy Number of bytes */
            virtsat_cmd_ptr->envelope_cmd.num_bytes =
                cmd_ptr->envelope.num_bytes;

            /* Copy the Envelope Command Data */
            memcpy(virtsat_cmd_ptr->envelope_cmd.data,
                   &cmd_ptr->envelope.data[cmd_ptr->envelope.offset],
                   cmd_ptr->envelope.num_bytes);

            /* Now send it to the Virtual Sat App */
            (void)virtuim_send_cmd_to_virtsat(virtsat_cmd_ptr);

            /* Globals are Set for Virtual Sat Response */
            /* Copy to UIM Report Type and use callback */
            uim_rpt_buf_ptr->rpt_type = UIM_ENVELOPE_R;
            if ( virtsat_report.status_report.virtsat_result == VIRTSAT_SUCCESS)
            {
                uim_rpt_buf_ptr->rpt_status = UIM_PASS;
            }
            uim_rpt_buf_ptr->sw1 = virtsat_report.status_report.sw1;
            uim_rpt_buf_ptr->sw2 = virtsat_report.status_report.sw2;
            uim_rpt_buf_ptr->rpt.envelope.data_length = virtsat_report.num_bytes;
            if (uim_rpt_buf_ptr->rpt.envelope.data_length > 0)
            {
              memcpy(uim_rpt_buf_ptr->rpt.envelope.data,
                virtsat_report.data,
                virtsat_report.num_bytes);
            }
            break;

        case UIM_TERMINAL_RESPONSE_F:
            MSG_HIGH("VIRT SIM: GOT ENVELOPE CMD",0,0,0);
            /* Set the Transaction Type */
            virtsat_cmd_ptr->hdr.transaction = VIRTSAT_TERMINAL_RESPONSE_F;

            /* Copy Protocol Information */
            virtsat_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;
            /* Virtual SAT to Call Virtual SIM Call Back */
            virtsat_cmd_ptr->hdr.virtsat_rpt_function = virtuim_virtsat_report;

            /* Copy Call Back for Virtual SIM to Call */
            virtsat_cmd_ptr->hdr.uim_rpt_function =
                cmd_ptr->hdr.rpt_function;

            /* Copy Number of bytes */
            virtsat_cmd_ptr->terminal_response.num_bytes =
                cmd_ptr->terminal_response.num_bytes;

            /* Copy the Envelope Command Data */
            memcpy(virtsat_cmd_ptr->terminal_response.data,
                   cmd_ptr->terminal_response.data,
                   cmd_ptr->terminal_response.num_bytes);

            /* Now send it to the Virtual Sat App */
            (void)virtuim_send_cmd_to_virtsat(virtsat_cmd_ptr);

            /* Globals are Set for Virtual Sat Response */
            /* Copy to UIM Report Type and use callback */
            uim_rpt_buf_ptr->rpt_type = UIM_TERMINAL_RESPONSE_R;
            if ( virtsat_report.status_report.virtsat_result == VIRTSAT_SUCCESS)
            {
                uim_rpt_buf_ptr->rpt_status = UIM_PASS;
            }
            uim_rpt_buf_ptr->sw1 = virtsat_report.status_report.sw1;
            uim_rpt_buf_ptr->sw2 = virtsat_report.status_report.sw2;
            break;

        default:
            ERR("DON'T KNOW COMMAND: %x",cmd_ptr->hdr.command,0,0);
            gsdi_status = GSDI_ERROR;
            break;
        }
    }

    MGSDIUTIL_TMC_MEM_FREE(virtsat_cmd_ptr);
   
    return gsdi_status;
}

/*===========================================================================
FUNCTION virtuim_send_cmd_to_gstk

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtuim_result_type virtuim_send_cmd_to_gstk(
  virtuim_cmd_type *virtuim_cmd_ptr)
{
    gstk_status_enum_type gstk_result;
    virtuim_result_type virtuim_result = VIRTUIM_SUCCESS;

    /* Now send the Command to GSTK */
    gstk_result = gstk_send_proactive_cmd(virtuim_cmd_ptr->data_len,
                                          virtuim_cmd_ptr->data_buffer);


    if (gstk_result == GSTK_SUCCESS)
    {
        virtuim_result = VIRTUIM_SUCCESS;
    }
    else
    {
        virtuim_result = VIRTUIM_ERROR;
    }
    return virtuim_result;
}

#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */
/*===========================================================================
FUNCTION VIRTUIM_VERIFY_CHV

DESCRIPTION
  Function is used to verify the currently selected PIN.  Please note that the PIN
  will always be the same PIN in the global static chv1 when reset.

  Otherwise, during the course of the Powerup, the CHV Values can be modified.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtuim_verify_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
)
{
  byte * df_p = 0x00;
  byte * chv_p = 0x00;
  switch( slot )
  {
  case UIM_SLOT_1:
    MSG_LOW("SLOT 1 USES DF",0,0,0);
    df_p = df;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case UIM_SLOT_2:
    MSG_LOW("SLOT 2 USES DF2",0,0,0);
    df_p = df2;
    break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MSG_ERROR("BAD SLOT ID %x",slot,0,0);
    ERR_FATAL("BAD SLOT ID %x",slot,0,0);
    break;
  }

    switch (chv1_or_chv2)
    {
    case UIM_CHV1:
      /* Are we trying to verify a Disabled PIN ? */
      if ( ( df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS] |
              VIRTUIM_DF_GSM_CHV_ENABLED_MASK ) !=
              VIRTUIM_DF_GSM_CHV_ENABLED_MASK )
      {
          ERR("CHV1 NOT ENABLED: %x",df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS],0,0);

          *rpt_status_p = UIM_FAIL;
          *sw1_p      = 0x98;
          *sw2_p      = 0x08;
          return;
      }
      /* Are we trying to verify a PIN Not init */
      if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
              VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
              VIRTUIM_DF_GSM_CHV_INIT_MASK )
      {
          ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
          *rpt_status_p = UIM_FAIL;
          *sw1_p      = 0x98;
          *sw2_p      = 0x08;
          return;
      }
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          chv_p = chv1;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          chv_p = chv1_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }

        if ( !(memcmp(chv,chv_p,VIRTUIM_CHV_LEN)))
        {
            /* Update DF Information accordingly */
            df_p[VIRTUIM_BYTE_CHV1_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

            /* Set Report Information */
            *rpt_status_p = UIM_PASS;
            *sw1_p      = 0x90;
            *sw2_p      = 0x00;

            MSG_HIGH("SUCCESSFUL CHV1 VERIFICATION",0,0,0);
        }
        else
        {
            /* Decrement Number of Retries by 1 */
            --df_p[VIRTUIM_BYTE_CHV1_STATUS];

            /* Set Report Information Accordingly */
            *rpt_status_p = UIM_FAIL;
            *sw1_p      = 0x98;

            MSG_HIGH("FAILED CHV1 VERIFICATION: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);

            /* Set SW2_P Accordingly */
            if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
                   VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
            {
                /* Have at least one more try */
                *sw2_p    = 0x04;
            }
            else
            {
                /* Use Exhausted Number of retries */
                /* No More attempts Left           */
                MSG_HIGH("CHV1 BLOCKED",0,0,0);
                *sw2_p    = 0x40;

                /* Need to make CHV1 Not Init */
                df_p[VIRTUIM_BYTE_CHV1_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
            }
        }
        break;

    case UIM_CHV2:
        /* Are we trying to verify a PIN Not init */
        if ( ( df_p[VIRTUIM_BYTE_CHV2_STATUS] &
               VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
               VIRTUIM_DF_GSM_CHV_INIT_MASK )
        {
            ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
            *rpt_status_p = UIM_FAIL;
            *sw1_p      = 0x98;
            *sw2_p      = 0x08;
            return;
        }
    switch( slot )
    {
    case UIM_SLOT_1:
      MSG_LOW("USES SLO1 1 CHV 2",0,0,0);
      chv_p = chv2;
      break;

    case UIM_SLOT_2:
      MSG_LOW("SLOT 2 CHV 2",0,0,0);
      chv_p = chv2_2;
      break;

    default:
      MSG_ERROR("BAD SLOT ID %x",slot,0,0);
      ERR_FATAL("BAD SLOT ID %x",slot,0,0);
      break;
    }
        if ( !(memcmp(chv,chv_p,VIRTUIM_CHV_LEN)))
        {
            /* Reset Number of retries for CHV2 */
            df_p[VIRTUIM_BYTE_CHV2_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

            /* Set Report Information */
            *rpt_status_p = UIM_PASS;
            *sw1_p      = 0x90;
            *sw2_p      = 0x00;

            MSG_HIGH("SUCCESSFUL CHV2 VERIFICATION",0,0,0);
        }
        else
        {
            /* Decrement Number of Retries by 1 */
            --df_p[VIRTUIM_BYTE_CHV2_STATUS];

            /* Set Report Information Accordingly */
            *rpt_status_p = UIM_FAIL;
            *sw1_p      = 0x98;

            MSG_HIGH("FAILED CHV2 VERIFICATION: %x",df_p[VIRTUIM_BYTE_CHV2_STATUS],0,0);

            /* Set SW2_P Accordingly */
            if ( ( df_p[VIRTUIM_BYTE_CHV2_STATUS] &
                   VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
            {
                /* Have at least one more try */
                *sw2_p    = 0x04;
            }
            else
            {
                /* Use Exhausted Number of retries */
                /* No More attempts Left           */
                MSG_HIGH("CHV2 BLOCKED",0,0,0);
                *sw2_p    = 0x40;

                /* Need to make CHV2 Not Init */
                df_p[VIRTUIM_BYTE_CHV2_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
            }
        }
        break;

    default:
        ERR("UNKNOWN CHV: %x",chv1_or_chv2,0,0);
        break;
    }
}
/*===========================================================================
FUNCTION VIRTUIM_ENABLE_CHV

DESCRIPTION
  Function is used to enable CHV1 or CHV2.  Please note, that they are always
  enabled upon Power up when FEATURE_GSDI_UIM_SERVER_PIN_ENABLED is defined.

  Otherwise, CHV1 will be initialized to a disabled state.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtuim_enable_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
)
{
  byte * df_p = 0x00;
  byte * chv_p = 0x00;
  switch( slot )
  {
  case UIM_SLOT_1:
    MSG_LOW("SLOT 1 USES DF",0,0,0);
    df_p = df;
    break;


#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case UIM_SLOT_2:
    MSG_LOW("SLOT 2 USES DF2",0,0,0);
    df_p = df2;
    break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MSG_ERROR("BAD SLOT ID %x",slot,0,0);
    ERR_FATAL("BAD SLOT ID %x",slot,0,0);
    break;
  }

  switch (chv1_or_chv2)
    {
    case UIM_CHV1:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          chv_p = chv1;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          chv_p = chv1_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
      /* Are we trying enable an enabled PIN ? */
      if ( ( df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS] |
              VIRTUIM_DF_GSM_CHV_ENABLED_MASK ) ==
              VIRTUIM_DF_GSM_CHV_ENABLED_MASK )
      {
        ERR("CHV1 ENABLED ALREADY: %x",df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS],0,0);
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }

      /* Are we trying to enable a PIN Not init */
      if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
              VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
              VIRTUIM_DF_GSM_CHV_INIT_MASK )
      {
        ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }

      if ( !(memcmp(chv,chv_p,VIRTUIM_CHV_LEN)))
      {
        /* Verification succeeded, update DF to Enable CHV1 */
        df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS] = VIRTUIM_DF_GSM_CHV_ENABLED_MASK;

        /* Update DF Information accordingly */
        df_p[VIRTUIM_BYTE_CHV1_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

        /* Set Report Information */
        *rpt_status_p = UIM_PASS;
        *sw1_p      = 0x90;
        *sw2_p      = 0x00;

        MSG_HIGH("SUCCESSFUL CHV1 ENABLE",0,0,0);
      }
      else
      {
        /* Decrement Number of Retries by 1 */
        --df_p[VIRTUIM_BYTE_CHV1_STATUS];

        /* Set Report Information Accordingly */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;

        MSG_HIGH("FAILED CHV1 ENABLE: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);

        /* Set SW2_P Accordingly */
        if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] & VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
        {
          /* Have at least one more try */
          *sw2_p    = 0x04;
        }
        else
        {
          /* Use Exhausted Number of retries */
          /* No More attempts Left           */
          MSG_HIGH("CHV1 BLOCKED",0,0,0);
          *sw2_p    = 0x40;

          /* Need to make CHV1 Not Init */
          df_p[VIRTUIM_BYTE_CHV1_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
        }
      }
      break;

    case UIM_CHV2:
      /* Can't Enable CHV2 */
      MSG_ERROR("CAN'T ENABLE A PIN THAT CAN NOT BE DISABLED",0,0,0);
      *rpt_status_p = UIM_FAIL;
      *sw1_p      = 0x98;
      *sw2_p      = 0x08;
      break;

    default:
      ERR("UNKNOWN CHV: %x",chv1_or_chv2,0,0);
      break;
    }
}

/*===========================================================================
FUNCTION VIRTUIM_DISABLE_CHV

DESCRIPTION
  Function is used to disable CHV1 and CHV2.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtuim_disable_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
)
{
  byte * df_p = 0x00;
  byte * chv_p = 0x00;
  switch( slot )
  {
  case UIM_SLOT_1:
    MSG_LOW("SLOT 1 USES DF",0,0,0);
    df_p = df;
    break;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  case UIM_SLOT_2:
    MSG_LOW("SLOT 2 USES DF2",0,0,0);
    df_p = df2;
    break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

  default:
    MSG_ERROR("BAD SLOT ID %x",slot,0,0);
    ERR_FATAL("BAD SLOT ID %x",slot,0,0);
    break;
  }
  switch (chv1_or_chv2)
  {
    case UIM_CHV1:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          chv_p = chv1;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          chv_p = chv1_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
      /* Are we trying disable a disabled PIN ? */
      if ( ( df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS] &
             VIRTUIM_DF_GSM_CHV_DISABLED_MASK ) ==
             VIRTUIM_DF_GSM_CHV_DISABLED_MASK )
      {
        ERR("CHV1 NOT ENABLED: %x",df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS],0,0);

        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }

      /* Are we trying to disable a PIN Not init */
      if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
              VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
              VIRTUIM_DF_GSM_CHV_INIT_MASK )
      {
        ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }
      if ( !(memcmp(chv,chv_p,VIRTUIM_CHV_LEN)))
      {
        /* Verification succeeded, update DF to Disable CHV1 */
        df_p[VIRTUIM_BYTE_FILE_CHARACTERISTICS] = VIRTUIM_DF_GSM_CHV_DISABLED_MASK;

        /* Update DF Information accordingly */
        df_p[VIRTUIM_BYTE_CHV1_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

        /* Set Report Information */
        *rpt_status_p = UIM_PASS;
        *sw1_p      = 0x90;
        *sw2_p      = 0x00;

        MSG_HIGH("SUCCESSFUL CHV1 DISABLE",0,0,0);
      }
      else
      {
        /* Decrement Number of Retries by 1 */
        --df_p[VIRTUIM_BYTE_CHV1_STATUS];

        /* Set Report Information Accordingly */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;

        MSG_HIGH("FAILED CHV1 DISABLE: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);

        /* Set SW2_P Accordingly */
        if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
                VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
        {
          /* Have at least one more try */
          *sw2_p    = 0x04;
        }
        else
        {
          /* Use Exhausted Number of retries */
          /* No More attempts Left           */
          MSG_HIGH("CHV1 BLOCKED",0,0,0);
          *sw2_p    = 0x40;

          /* Need to make CHV1 Not Init */
          df_p[VIRTUIM_BYTE_CHV1_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
        }
      }
      break;

    case UIM_CHV2:
        /* Can't Disable CHV2 */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        break;

    default:
        ERR("UNKNOWN CHV: %x",chv1_or_chv2,0,0);
        break;
    }
}

/*===========================================================================
FUNCTION VIRTUIM_UNBLOCK_CHV

DESCRIPTION
  Function is used to unblock CHV1 or CHV2 when blocked by incorrectly
  supplying CHV1 or CHV2 Codes.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL  void virtuim_unblock_chv(
    uim_slot_type       slot,
    uim_chv_type        chv1_or_chv2,
    byte              * unblock_chv,
    byte              * new_chv,
    uim_rpt_status    * rpt_status_p,
    uim_sw1_type      * sw1_p,
    uim_sw2_type      * sw2_p
)
{
  byte * df_p = 0x00;
  byte * unblock_chv_p = 0x00;
  byte * chv_p = 0x00;
  switch( slot )
  {
    case UIM_SLOT_1:
      MSG_LOW("SLOT 1 USES DF",0,0,0);
      df_p = df;
      break;

#ifdef FEATURE_MMGSI_DUAL_SLOT
    case UIM_SLOT_2:
      MSG_LOW("SLOT 2 USES DF2",0,0,0);
      df_p = df2;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MSG_ERROR("BAD SLOT ID %x",slot,0,0);
      ERR_FATAL("BAD SLOT ID %x",slot,0,0);
      break;
  }
  switch (chv1_or_chv2)
  {
    case UIM_CHV1:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          unblock_chv_p = chv1_unblock;
          chv_p         = chv1;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          unblock_chv_p = chv1_unblock_2;
          chv_p         = chv1_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
      /* Are we trying to Unbluck a PUK Not init */
      if ( ( df_p[VIRTUIM_BYTE_PUK1_STATUS] &
              VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
              VIRTUIM_DF_GSM_CHV_INIT_MASK )
      {
        ERR("PUK1 NOT INIT: %x",df_p[VIRTUIM_BYTE_PUK1_STATUS],0,0);
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }

      if ( !(memcmp(unblock_chv,unblock_chv_p,VIRTUIM_CHV_LEN)))
      {
        /* Update DF Information accordingly */
        df_p[VIRTUIM_BYTE_PUK1_STATUS] = VIRTUIM_DF_GSM_PUK_INIT_AND_VERIFIED_MASK;

        /* Now Change PIN1 */
        memcpy(chv_p,new_chv,VIRTUIM_CHV_LEN);

        /* Now Update CHV1 Status */
        df_p[VIRTUIM_BYTE_CHV1_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

        /* Set Report Information */
        *rpt_status_p = UIM_PASS;
        *sw1_p      = 0x90;
        *sw2_p      = 0x00;

        MSG_HIGH("SUCCESSFUL PUK1 VERIFICATION",0,0,0);
      }
      else
      {
        /* Decrement Number of Retries by 1 */
        --df_p[VIRTUIM_BYTE_PUK1_STATUS];

        /* Set Report Information Accordingly */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;

        MSG_HIGH("FAILED PUK1 VERIFICATION: %x",df_p[VIRTUIM_BYTE_PUK1_STATUS],0,0);

        /* Set SW2_P Accordingly */
        if ( ( df_p[VIRTUIM_BYTE_PUK1_STATUS] &
                VIRTUIM_DF_GSM_NUMBER_OF_PUK_RETRIES_LEFT_MASK )  > 0 )
        {
          /* Have at least one more try */
          *sw2_p    = 0x04;
        }
        else
        {
          /* Use Exhausted Number of retries */
          /* No More attempts Left           */
          MSG_HIGH("PUK1 BLOCKED",0,0,0);
          *sw2_p    = 0x40;

          /* Need to make PUK1 Not Init */
          df_p[VIRTUIM_BYTE_PUK1_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
        }
      }
      break;

    case UIM_CHV2:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          unblock_chv_p = chv2_unblock;
          chv_p         = chv2;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          unblock_chv_p = chv2_unblock_2;
          chv_p         = chv2_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
      if ( !(memcmp(unblock_chv,unblock_chv_p,VIRTUIM_CHV_LEN)))
      {
        /* Update DF Information accordingly */
        df_p[VIRTUIM_BYTE_PUK2_STATUS] = VIRTUIM_DF_GSM_PUK_INIT_AND_VERIFIED_MASK;

        /* Now Change CHV2 */
        memcpy(chv_p,new_chv,VIRTUIM_CHV_LEN);

        /* Now Update CHV2 Status */
        df_p[VIRTUIM_BYTE_CHV2_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

        /* Set Report Information */
        *rpt_status_p = UIM_PASS;
        *sw1_p      = 0x90;
        *sw2_p      = 0x00;

        MSG_HIGH("SUCCESSFUL PUK2 VERIFICATION",0,0,0);
      }
      else
      {
        /* Decrement Number of Retries by 1 */
        --df_p[VIRTUIM_BYTE_PUK2_STATUS];

        /* Set Report Information Accordingly */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;

        MSG_HIGH("FAILED PUK2 VERIFICATION: %x",df_p[VIRTUIM_BYTE_PUK2_STATUS],0,0);

        /* Set SW2_P Accordingly */
        if ( ( df_p[VIRTUIM_BYTE_PUK2_STATUS] &
                VIRTUIM_DF_GSM_NUMBER_OF_PUK_RETRIES_LEFT_MASK )  > 0 )
        {
          /* Have at least one more try */
          *sw2_p    = 0x04;
        }
        else
        {
          /* Use Exhausted Number of retries */
          /* No More attempts Left           */
          MSG_HIGH("PUK2 BLOCKED",0,0,0);
          *sw2_p    = 0x40;

          /* Need to make PUK1 Not Init */
          df_p[VIRTUIM_BYTE_PUK2_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
        }
      }
      break;

    default:
        ERR("UNKNOWN CHV: %x",chv1_or_chv2,0,0);
        break;
    }
}

/*===========================================================================
FUNCTION VIRTUIM_CHANGE_CHV

DESCRIPTION
  Function is used to change the CHV Values for CHV1 & CHV2

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void  virtuim_change_chv(
  uim_slot_type       slot,
  uim_chv_type        chv1_or_chv2,
  byte              * old_chv,
  byte              * new_chv,
  uim_rpt_status    * rpt_status_p,
  uim_sw1_type      * sw1_p,
  uim_sw2_type      * sw2_p
)
{
  byte * df_p = 0x00;
  byte * chv_p = 0x00;
  switch( slot )
  {
    case UIM_SLOT_1:
      MSG_LOW("SLOT 1 USES DF",0,0,0);
      df_p = df;
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case UIM_SLOT_2:
      MSG_LOW("SLOT 2 USES DF2",0,0,0);
      df_p = df2;
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

    default:
      MSG_ERROR("BAD SLOT ID %x",slot,0,0);
      ERR_FATAL("BAD SLOT ID %x",slot,0,0);
      break;
  }
  switch (chv1_or_chv2)
  {
    case UIM_CHV1:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          chv_p         = chv1;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          chv_p         = chv1_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
      /* Are we trying to change a PIN Not init */
      if ( ( df_p[VIRTUIM_BYTE_CHV1_STATUS] &
              VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
              VIRTUIM_DF_GSM_CHV_INIT_MASK )
      {
        ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;
        *sw2_p      = 0x08;
        return;
      }
      if ( !(memcmp(old_chv,chv_p,VIRTUIM_CHV_LEN)))
      {
        /* Update DF Information accordingly */
        df_p[VIRTUIM_BYTE_CHV1_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

        /* Need to copy new chv over old chv */
        memcpy(chv_p,new_chv,VIRTUIM_CHV_LEN);

        /* Set Report Information */
        *rpt_status_p = UIM_PASS;
        *sw1_p      = 0x90;
        *sw2_p      = 0x00;

        MSG_HIGH("SUCCESSFUL CHV1 VERIFICATION",0,0,0);
      }
      else
      {
        /* Decrement Number of Retries by 1 */
        --df[VIRTUIM_BYTE_CHV1_STATUS];

        /* Set Report Information Accordingly */
        *rpt_status_p = UIM_FAIL;
        *sw1_p      = 0x98;

        MSG_HIGH("FAILED CHV1 VERIFICATION: %x",df[VIRTUIM_BYTE_CHV1_STATUS],0,0);

        /* Set SW2_P Accordingly */
        if ( ( df[VIRTUIM_BYTE_CHV1_STATUS] &
                VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
        {
          /* Have at least one more try */
          *sw2_p    = 0x04;
        }
        else
        {
          /* Use Exhausted Number of retries */
          /* No More attempts Left           */
          MSG_HIGH("CHV1 BLOCKED",0,0,0);
          *sw2_p    = 0x40;

          /* Need to make CHV1 Not Init */
          df[VIRTUIM_BYTE_CHV1_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
        }
      }
      break;

    case UIM_CHV2:
      switch( slot )
      {
        case UIM_SLOT_1:
          MSG_LOW("USES SLO1 1 CHV 1",0,0,0);
          chv_p         = chv2;
          break;

        case UIM_SLOT_2:
          MSG_LOW("SLOT 2 CHV 1",0,0,0);
          chv_p         = chv2_2;
          break;

        default:
          MSG_ERROR("BAD SLOT ID %x",slot,0,0);
          ERR_FATAL("BAD SLOT ID %x",slot,0,0);
          break;
      }
        /* Are we trying to verify a PIN Not init */
        if ( ( df_p[VIRTUIM_BYTE_CHV2_STATUS] &
               VIRTUIM_DF_GSM_CHV_INIT_MASK ) !=
               VIRTUIM_DF_GSM_CHV_INIT_MASK )
        {
            ERR("CHV1 NOT INIT: %x",df_p[VIRTUIM_BYTE_CHV1_STATUS],0,0);
            *rpt_status_p = UIM_FAIL;
            *sw1_p      = 0x98;
            *sw2_p      = 0x08;
            return;
        }
        if ( !(memcmp(old_chv,chv_p,VIRTUIM_CHV_LEN)))
        {
            /* Reset Number of retries for CHV2 */
            df_p[VIRTUIM_BYTE_CHV2_STATUS] = VIRTUIM_DF_GSM_CHV_INIT_AND_VERIFIED_MASK;

            /* Need to copy new chv over old chv */
            memcpy(chv_p,new_chv,VIRTUIM_CHV_LEN);

            /* Set Report Information */
            *rpt_status_p = UIM_PASS;
            *sw1_p      = 0x90;
            *sw2_p      = 0x00;

            MSG_HIGH("SUCCESSFUL CHV2 VERIFICATION",0,0,0);
        }
        else
        {
            /* Decrement Number of Retries by 1 */
            --df_p[VIRTUIM_BYTE_CHV2_STATUS];

            /* Set Report Information Accordingly */
            *rpt_status_p = UIM_FAIL;
            *sw1_p      = 0x98;

            MSG_HIGH("FAILED CHV2 VERIFICATION: %x",df_p[VIRTUIM_BYTE_CHV2_STATUS],0,0);

            /* Set SW2_P Accordingly */
            if ( ( df_p[VIRTUIM_BYTE_CHV2_STATUS] &
                   VIRTUIM_DF_GSM_NUMBER_OF_RETRIES_LEFT_MASK )  > 0 )
            {
                /* Have at least one more try */
                *sw2_p    = 0x04;

                /* Need to make CHV1 Not Init */
                df_p[VIRTUIM_BYTE_CHV2_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
            }
            else
            {
                /* Use Exhausted Number of retries */
                /* No More attempts Left           */
                MSG_HIGH("CHV2 BLOCKED",0,0,0);
                *sw2_p    = 0x40;

                /* Need to make CHV1 Not Init */
                df_p[VIRTUIM_BYTE_CHV2_STATUS] &= VIRTUIM_DF_GSM_CHV_NOT_INIT_MASK;
            }
        }
        break;

    default:
        ERR("UNKNOWN CHV: %x",chv1_or_chv2,0,0);
        break;
    }
}

/* <EJECT> */
/*===========================================================================
FUNCTION uim_convert_path_to_ef

DESCRIPTION
  Only when FEATURE_GSDI_UIM_SERVER_USIM is defined, it will convert a
  request of UIM_EF_BY_PATH to a UIM File name.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
void uim_convert_path_to_ef (
  uim_cmd_type * cmd_ptr
)
{
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;
  /* A request by Path needs to be converted back
  ** to a uim_items_enum_type
  */
  gsdi_status = virtuim_get_ef_from_path( cmd_ptr );
    if ( gsdi_status != GSDI_SUCCESS)
    {
    MSG_HIGH("Could not convert PATH to FID",0,0,0);
        MSG_HIGH("Path:  %x, may still be ok",cmd_ptr->access_uim.path[2],0,0);
    }
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
} /* uim_convert_path_to_ef */

/*===========================================================================
FUNCTION uim_set_signals_to_wait_for

DESCRIPTION
  Encapsulates the setting of the Signals the Test SIM Server should
  wait for.

DEPENDENCIES
  None

RETURN VALUE
  Signals that Test SIM Server Should Wait for
===========================================================================*/
rex_sigs_type uim_set_signals_to_wait_for(
  void
)
{
  rex_sigs_type sigs_to_wait_for;

  sigs_to_wait_for = UIM_CMD_Q_SIG
#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
                    | VIRTUIM_SEND_SAT_CMD_SIG
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
                    | VIRTUIM_EVENT_TESTER_RESTART_POWER_UP_SIG
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */
                      ;

  return sigs_to_wait_for;
}

/*===========================================================================
FUNCTION test_sim_server_init

DESCRIPTION
  This function is used to Initialize all Timers, Queues, and Registers
  as needed.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void test_sim_server_init (
  void
)
{
  virtuim_slot_info_type slot_info;

#if defined(T_UIM_USIM_PAD_EN)
  MSM_OUTSH( MISC_CTL_0,
               MISC_CTL_0_USIM_PAD_EN_BMSK,
               MISC_CTL_0_USIM_PAD_EN_BMSK);
#endif /* T_UIM_USIM_PAD_EN */

  rex_def_timer(&virtuim_reset_delay_timer,&gsdi_uim_server_task_tcb,VIRTUIM_TIMER_EXPIRE_SIG);

#ifdef FEATURE_DOG
  /* Initiailize timers */
  rex_def_timer( &uim_rpt_timer_alternate,
                   &gsdi_uim_server_task_tcb,
                   UIM_RPT_TIMER_SIG);

#endif /* FEATURE_DOG */

  (void)q_init(&uim_server_task_cmd_q);

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
  (void)q_init(&uim_virtsat_proactive_cmd_q);
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

  /* populate the initialize power up info regarding cards */
  virtuim_card_status.cdma_op_slot = UIM_SLOT_2;
  virtuim_card_status.gsm_op_slot = UIM_SLOT_1;
  virtuim_card_status.umts_op_slot = UIM_SLOT_NONE;

  slot_info.slot_present = TRUE;
  slot_info.cdma_present = TRUE;
  slot_info.gsm_present = TRUE;
#ifdef FEATURE_GSDI_UIM_SERVER_PIN_ENABLED
  slot_info.pin1_status = GSDI_PIN_ENABLED;
#else
  slot_info.pin1_status = GSDI_PIN_DISABLED;
#endif /* FEATURE_GSDI_UIM_SERVER_PIN_ENABLED */
  slot_info.tk_from_cdma = FALSE;
  slot_info.tk_from_gsm = TRUE;
  slot_info.umts_present = FALSE;
  virtuim_set_card_status(GSDI_SLOT_1, &slot_info);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  slot_info.pin1_status = GSDI_PIN_DISABLED;
  slot_info.cdma_present = TRUE;
  slot_info.gsm_present  = FALSE;

  virtuim_set_card_status(GSDI_SLOT_2, &slot_info);
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

} /* test_sim_server_init */

/*===========================================================================
FUNCTION test_sim_server_get_df_header

DESCRIPTION
  This function is used to retrieve the correct header information;

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void test_sim_server_get_df_header(
  uim_cmd_type *cmd_ptr,
  int32        *returned_data_len_ptr,
  byte         *data_ptr
)
{
  boolean use_gsm;
  byte * df_p = 0x00;

  /* Pointer to Byte Array that contains
  ** DF Information based on slot */
  switch (cmd_ptr->select.dir )
  {
    case UIM_GSM_DF:
      use_gsm = TRUE;
      break;

    case UIM_CDMA_DF:
      use_gsm = FALSE;
      break;

    default:
      MSG_ERROR("BAD DIR %x",cmd_ptr->select.dir,0,0);
      MSG_ERROR("USIM GSM DF INSTEAD",0,0,0);
      use_gsm = TRUE;
      break;
  }

#ifdef FEATURE_MMGSDI_DUAL_SLOT
  /* Determine Slot to Use */
  switch ( cmd_ptr->hdr.slot )
  {
    case UIM_SLOT_1:
      if ( use_gsm )
        df_p = df;
      else
        df_p = cdma_df;
      break;

    case UIM_SLOT_2:
      if ( use_gsm )
        df_p = df2;
      else
        df_p = cdma_df2;
      break;

    default:
      MSG_ERROR("BAD SLOT INFO %x",cmd_ptr->hdr.slot,0,0);
      MSG_ERROR("DEFAULT TO SLOT 1",0,0,0);
      if ( use_gsm )
        df_p = df;
      else
        df_p = cdma_df;
      break;
  }
#else
  /* Default to Slot 1 */
  if ( use_gsm )
    df_p = df;
  else
#ifdef FEATURE_MMGSDI_CDMA
    df_p = cdma_df;
#else
    df_p = df;
#endif /*FEATURE_MMGSDI_CDMA */
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

  /* Now Copy DF Information for Requested
  ** Slot (if applicable) and Requested
  ** technology.  Use GSM LEN as LEN For CDMA As
  ** well.
  */
  memcpy(data_ptr,df_p,VIRTUIM_DF_GSM_LEN);
    *returned_data_len_ptr = VIRTUIM_DF_GSM_LEN;
}

/*===========================================================================
FUNCTION test_sim_server_task

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void test_sim_server_task
(
  dword dummy
    /* Required for REX, ignore */
    /*lint -esym(715,dummy)
    ** Have lint not complain about the ignored parameter 'dummy' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
    rex_sigs_type   sigs;
    rex_sigs_type   sigs_to_wait_for;
    uim_cmd_type        *cmd_ptr;
    gsdi_returns_T      gsdi_status=GSDI_SUCCESS;
    byte                data_ptr[GSDI_MAX_DATA_BLOCK_LEN];
    int32                returned_data_len = 0;
    int32                returned_record_num = 0;
    uim_rpt_type        uim_rpt_buf;
    byte                i;
    byte                r;
    byte                accessed_record_number = 0;
    word                seek_number_of_records = 0;
    gsdi_uim_link_established_T  link_est_cmd;

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
    virtuim_cmd_type    *virtuim_cmd_ptr;
    virtuim_result_type virtuim_result=VIRTUIM_SUCCESS;
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

  /* Initialize Structures */
    memset( data_ptr, 0xFF, GSDI_MAX_DATA_BLOCK_LEN );
    memset (&link_est_cmd,0x00,sizeof(gsdi_uim_link_established_T));
    memset (&uim_rpt_buf, 0x00, sizeof(uim_rpt_type));

    /* Initialize Queues, Timers, and Registers as needed */
    test_sim_server_init();

    /* Wait for Start Signal */
    tmc_task_start();

#ifdef FEATURE_DOG
    dog_report(DOG_UIM_RPT);

    (void) rex_set_timer(&uim_rpt_timer_alternate, DOG_UIM_RPT_TIME);
#endif /*FEATURE_DOG*/
    /* Set Signals Test SIM Server Should Wait For */
    sigs_to_wait_for = uim_set_signals_to_wait_for();

    /* Test SIM Server Main Loop */
    for(;;)
    {
        /* Did we just Power Up or go
        ** throught a RESET
        */
        if (just_powered_up == TRUE)
        {
            /* Build and Send Link Established Message for GSDI */
            uim_build_link_established_message(&link_est_cmd);
            MSG_MED("gsdi_cmd executed",0,0,0);

            /* Send to GSDI iff a slot is availalbe */
            if ( virtuim_card_status.slot1.slot_present
      #ifdef FEATURE_MMGSDI_DUAL_SLOT
              || virtuim_card_status.slot2.slot_present
      #endif /*FEATURE_MMGSDI_DUAL_SLOT */
              )
            {
                (void)gsdi_cmd_ext(&link_est_cmd);
                just_powered_up = FALSE;
            }
            else
            {
                /* Slot Error ... Do nothing */
                MSG_ERROR ( "SLOT ERROR...DO NOTHING",0,0,0);
            }
        }

        sigs = test_sim_server_wait(sigs_to_wait_for);

        if((sigs & UIM_CMD_Q_SIG) != 0)
        {
            MSG_HIGH("UIM_CMD_Q_SIG SET: %x",sigs,0,0);
            (void)rex_clr_sigs(rex_self(),UIM_CMD_Q_SIG);

            if ((cmd_ptr = (uim_cmd_type*)q_get(&uim_server_task_cmd_q)) != NULL)
            {
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                cmd_ptr->hdr.slot = UIM_SLOT_1;
#endif
                uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;
                switch(cmd_ptr->hdr.command)
                {
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
                case UIM_MANAGE_CHANNEL_F:
                    gsdi_status = virtual_sim_process_manage_channel_req(
                                    (uim_manage_channel_cmd_type *)cmd_ptr,
                                    (byte *)&returned_data_len,
                                    (byte *)&data_ptr );

                    uim_server_build_response(
                                    gsdi_status,
                                    cmd_ptr,
                                    &uim_rpt_buf,
                                    returned_record_num,
                                    returned_data_len,
                                    data_ptr);
                    /* Send Back to GSDI */
                    MSG_LOW("MANAGE_CHANNEL REPORTING TO GSDI",0,0,0);
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                    break;

                case UIM_STREAM_ISO7816_APDU_F:
                    gsdi_status = virtual_sim_process_iso7816_apdu(
                                  (uim_stream_iso7816_apdu_type *)cmd_ptr,
                                  (byte *)&returned_data_len,
                                  (byte *)&data_ptr );

                    uim_server_build_response(
                                    gsdi_status,
                                    cmd_ptr,
                                    &uim_rpt_buf,
                                    returned_record_num,
                                    returned_data_len,
                                    data_ptr);

                    /* Send Back to GSDI */
                    MSG_LOW("MANAGE_CHANNEL REPORTING TO GSDI",0,0,0);
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);

                    break;


#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

                case UIM_READ_BIN_CMD_F:
                  MSG_MED("UIM Server UIM_READ_BIN_F received ",0,0,0);
                  /* If needed, convert EF_BY_PATH to a UIM File Name */

                  // TODO
                  /*if (cmd_ptr->read_bin.item == UIM_EF_BY_PATH) {
                    uim_convert_path_to_ef(cmd_ptr);
                  }
                  */

                  /* Handle Read Command */
                  gsdi_status = uim_server_read(cmd_ptr->hdr.command,
                                                cmd_ptr,
                                                &returned_record_num,
                                                &returned_data_len,
                                                (byte *)&data_ptr );
                  /* Build Response */
                  uim_server_build_response(gsdi_status,
                                            cmd_ptr,
                                            &uim_rpt_buf,
                                            returned_record_num,
                                            returned_data_len,
                                            data_ptr);

                  /* Send Back to GSDI */
                  MSG_LOW("UIM_READ REPORTING TO GSDI",0,0,0);
                  (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);

                  if (gsdi_status == GSDI_SUCCESS)
                    mmgsdi_free(uim_rpt_buf.rpt.read.data);
                  break;

                case UIM_READ_REC_CMD_F:
                  MSG_MED("UIM Server UIM_READ_REC_CMD_F received ",0,0,0);
                  /* If needed, convert EF_BY_PATH to a UIM File Name */

                  // TODO
                  /*if (cmd_ptr->read_bin.item == UIM_EF_BY_PATH) {
                    uim_convert_path_to_ef(cmd_ptr);
                  }
                  */

                  /* Handle Read Command */
                  gsdi_status = uim_server_read(cmd_ptr->hdr.command,
                                                cmd_ptr,
                                                &returned_record_num,
                                                &returned_data_len,
                                                (byte *)&data_ptr );
                  /* Build Response */
                  uim_server_build_response(gsdi_status,
                                            cmd_ptr,
                                            &uim_rpt_buf,
                                            returned_record_num,
                                            returned_data_len,
                                            data_ptr);

                  /* Send Back to GSDI */
                  MSG_LOW("UIM_READ REPORTING TO GSDI",0,0,0);
                  (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);

                  if (gsdi_status == GSDI_SUCCESS)
                    mmgsdi_free(uim_rpt_buf.rpt.read.data);
                  break;

                case UIM_WRITE_BIN_CMD_F:
                  MSG_MED("UIM_WRITE_BIN_CMD_F SLOT %x EF %x",
                  cmd_ptr->hdr.slot,cmd_ptr->write_bin.item,0);

                  /* Handle Write Command */
                  gsdi_status = uim_server_write( cmd_ptr );

                  /* Build Response to Write Command */
                  uim_server_build_response(gsdi_status,
                                            cmd_ptr,
                                            &uim_rpt_buf,
                                            cmd_ptr->write_bin.offset,
                                            0x00,
                                            0x00);
                  /* Report Back to GSDI */
                  MSG_LOW("UIM_WRITE REPORTING TO GSDI",0,0,0);
                  (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                  break;

                case UIM_WRITE_REC_CMD_F:
                  MSG_MED("UIM_WRITE_REC_CMD_F SLOT %x EF %x",
                  cmd_ptr->hdr.slot,cmd_ptr->write_rec.item,0);

                  /* Handle Write Command */
                  gsdi_status = uim_server_write( cmd_ptr );

                  /* Build Response to Write Command */
                  uim_server_build_response(gsdi_status,
                                            cmd_ptr,
                                            &uim_rpt_buf,
                                            0,
                                            0x00,
                                            0x00);
                  /* Report Back to GSDI */
                  MSG_LOW("UIM_WRITE REPORTING TO GSDI",0,0,0);
                  (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                  break;


                case  UIM_ACCESS_F:
                    MSG_MED("UIM Server UIM_ACCESS_F received",0,0,0);
                    /* If needed, convert EF_BY_PATH to a UIM File Name */
                    if (cmd_ptr->access_uim.item == UIM_EF_BY_PATH) {
                      uim_convert_path_to_ef(cmd_ptr);
                    }

                    switch(cmd_ptr->access_uim.access)
                    {
                    case UIM_READ_F:
                        MSG_MED("UIM_READ_F SLOT %x EF %x",
                        cmd_ptr->hdr.slot,cmd_ptr->access_uim.item,0);

                        /* Handle Read Command */
                        gsdi_status = uim_server_read( cmd_ptr->hdr.command,
                                                       cmd_ptr,
                                                      &returned_record_num,
                                                      &returned_data_len,
                                                      (byte *)&data_ptr );
                        /* Build Response */
                        uim_server_build_response(gsdi_status,
                                                  cmd_ptr,
                                                  &uim_rpt_buf,
                                                  returned_record_num,
                                                  returned_data_len,
                                                  data_ptr);

                        /* Send Back to GSDI */
                        MSG_LOW("UIM_READ REPORTING TO GSDI",0,0,0);
                        (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                        break;

                    case UIM_WRITE_F:
                        MSG_MED("UIM_READ_F SLOT %x EF %x",
                        cmd_ptr->hdr.slot,cmd_ptr->access_uim.item,0);

                        /* Handle Write Command */
                        gsdi_status = uim_server_write( cmd_ptr );

                        /* Build Response to Write Command */
                        uim_server_build_response(gsdi_status,
                                                  cmd_ptr,
                                                  &uim_rpt_buf,
                                                  cmd_ptr->access_uim.offset,
                                                  0x00,
                                                  0x00);
                        /* Report Back to GSDI */
                        MSG_LOW("UIM_WRITE REPORTING TO GSDI",0,0,0);
                        (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                        break;

                    case UIM_INC_F:
                        MSG_MED("UIM Server - UIM_INC_F receved",0,0,0);

                        /* Handle Increase Command */
                        gsdi_status = uim_server_increase( cmd_ptr,
                                                          (byte*)&data_ptr);

                        /* Build Command Response:  Not handled */
                        uim_server_build_response(gsdi_status,
                                                  cmd_ptr,
                                                  &uim_rpt_buf,
                                                  0x00,
                                                  cmd_ptr->access_uim.num_bytes,
                                                  data_ptr);

                        /* Report Back to GSDI */
                        MSG_LOW("UIM_INC REPORTING TO GSDI",0,0,0);
                        (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                        break;

                    default:
                        ERR("UIM Server - Invalid access type",0,0,0);
                        break;
                    } /* switch */

                break; /* UIM_ACCESS_F */

            case UIM_CLEAR_CACHE_F:
                MSG_MED("UIM Server UIM_CLEAR_CACHE_F", 0, 0, 0);
                gsdi_status = GSDI_SUCCESS;
                uim_server_build_response(gsdi_status,
                                          cmd_ptr,
                                          &uim_rpt_buf,
                                          0,
                                          0x00,
                                          0x00);
                /* Report Back to GSDI */
                MSG_LOW("UIM_CLEAR_CACHE REPORTING TO GSDI",0,0,0);
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;

            case UIM_SELECT_F:
            case UIM_CACHED_SEL_F:
                MSG_MED("UIM Server UIM_SELECT_F/UIM_CACHED_SEL_F received EF %x",
                cmd_ptr->select.item,0,0);
                switch (cmd_ptr->select.file_type)
                {
                case UIM_EF:
                    gsdi_status = uim_server_get_header(cmd_ptr,
                                                        &returned_data_len,
                                                        data_ptr);
                    break;

                case UIM_DF:
                    test_sim_server_get_df_header(cmd_ptr,
                                                  &returned_data_len,
                                                  data_ptr);

                    /* Build Response */
                    uim_server_build_response(GSDI_SUCCESS,
                                              cmd_ptr,
                                              &uim_rpt_buf,
                                              0,
                                              returned_data_len,
                                              data_ptr);

                    gsdi_status = GSDI_SUCCESS;

                    break;

                case UIM_MF:
                    ERR("NO SUPPORT FOR SELECT ON MF",0,0,0);
                      gsdi_status = GSDI_NOT_SUPPORTED;
                    break;

#if defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_UMTS */
                default:
                    ERR("UNKNOWN FILE TYPE:",0,0,0);
                    gsdi_status = GSDI_ERROR;
                    break;
                } /* switch (cmd_ptr->select.file_type) */

                if (cmd_ptr->hdr.command == UIM_SELECT_F)
                {
                  uim_rpt_buf.rpt_type = UIM_SELECT_R;
                }
                else
                {
                  uim_rpt_buf.rpt_type = UIM_CACHED_SEL_R;
                }

                if ( gsdi_status == GSDI_SUCCESS )
                {
                  if ( cmd_ptr->select.file_type != UIM_DF )
                  {
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
                    uim_rpt_buf.rpt.select.num_data_bytes = returned_data_len,
                    memcpy(uim_rpt_buf.rpt.select.data,data_ptr,returned_data_len);
                  }
                }
                else
                {
                  MSG_ERROR("UIM_SELECT_F NOT GSDI_SUCCESS",0,0,0);
                            uim_rpt_buf.rpt_status = UIM_FAIL;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                  uim_rpt_buf.sw1 = 0x6A;
                  uim_rpt_buf.sw2 = 0x82;
#else
                  uim_rpt_buf.sw1 = 0x94;
                  uim_rpt_buf.sw2 = 0x04;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                  uim_rpt_buf.rpt.select.num_data_bytes = returned_data_len,
                  memcpy(uim_rpt_buf.rpt.select.data,data_ptr,returned_data_len);
                }
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break; /* UIM_SELECT */

            /* Adding CASE UIM_SEEK_F: */
            case UIM_SEEK_F:
                MSG_MED("UIM Server - UIM_SEEK_F receved EF %x", cmd_ptr->access_uim.item,0,0);
                gsdi_status = uim_server_seek(cmd_ptr,
                                              &accessed_record_number);
                if (gsdi_status == GSDI_SUCCESS)
                {
                    MSG_MED(" Returned OK from uim_server_seek",0,0,0);
                    /* Only one record can be supported */
                    seek_number_of_records = 1;
                    if (cmd_ptr->hdr.rpt_function != NULL)
                    {
                        uim_rpt_buf.rpt_type = UIM_SEEK_R;
                        uim_rpt_buf.rpt_status = UIM_PASS;
                        uim_rpt_buf.sw1 = 0x90;
                        uim_rpt_buf.sw2 = 0x00;
                        uim_rpt_buf.rpt.seek.num_records = seek_number_of_records;
                        uim_rpt_buf.rpt.seek.data[0] = accessed_record_number;
                        (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                    }
                }
                else
                {
                    ERR(" UIM_SEEK_F not success %x ", gsdi_status,0,0);
                    if (cmd_ptr->hdr.rpt_function != NULL)
                    {
                        uim_rpt_buf.rpt_type = UIM_SEEK_R;
                        uim_rpt_buf.rpt_status = UIM_FAIL;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                        uim_rpt_buf.sw1 = 0x6A;
                        uim_rpt_buf.sw2 = 0x82;
#else
                        uim_rpt_buf.sw1 = 0x94;
                        uim_rpt_buf.sw2 = 0x04;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                        uim_rpt_buf.rpt.select.num_data_bytes = returned_data_len,
                        memcpy(uim_rpt_buf.rpt.select.data,
                                data_ptr,
                                returned_data_len);
                        (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                    }
                }
                break;


            case UIM_AUTHENTICATE_F:
                MSG_MED("UIM Server UIM_AUTHENTICATE_F received",0,0,0);

                /* This is a temporary stub out of running algorithm until
                    the drivers to the actual SIM is ready */

                /* case 1, 3G security context command successful with GSM security
                    Service n27 is unavailable */
                data_ptr[0] = 0xDB; /* Successful 3G authentication */

                data_ptr[1] = 4;    /* Length of RES */
                for( i = 2; i <= 5; i++ )
                {
                    data_ptr[i] = 0x11;  /* fill in the res */
                }
                data_ptr[6] = 16;   /* Length of CK */
                for( i = 7; i <= 22; i++ )
                {
                    data_ptr[i] = 0x22;  /* fill in the CK */
                }

                data_ptr[23] = 16;  /* Length of IK */
                for( i = 24; i <= 39; i++ )
                {
                    data_ptr[i] = 0x33;  /* fill in the IK */
                }

                returned_data_len = i;

                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    uim_rpt_buf.rpt_type = UIM_AUTHENTICATE_R;
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
                    uim_rpt_buf.rpt.select.num_data_bytes = returned_data_len,
                    memcpy(uim_rpt_buf.rpt.select.data,
                            data_ptr,
                            returned_data_len);
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }

                break;
            case UIM_RUN_GSM_ALGO_F:

                MSG_MED("UIM Server UIM_RUN_GSM_ALGO_F received",0,0,0);

                /* This is a temporary stub out of running algorithm until
                    the drivers to the actual SIM is ready */
                /* This is a temporary stub out of running algorithm until
                    the drivers to the actual SIM is ready */
                for( i = 0; i < 4; i++ )
                {
                    data_ptr[i] = 0x11;  /* fill in the sres */
                }
                for( i = 4; i < 12; i++ )
                {
                    data_ptr[i] = 0x77;  /* fill in the kc */
                }
                returned_data_len = i;

                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    uim_rpt_buf.rpt_type = UIM_RUN_GSM_ALGO_R;
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
                    uim_rpt_buf.rpt.select.num_data_bytes = returned_data_len,
                    memcpy(uim_rpt_buf.rpt.select.data,
                            data_ptr,
                            returned_data_len);
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;

            case UIM_VERIFY_CHV_F:
                MSG_MED("UIM Server UIM_VERIFY_CHV_F received",0,0,0);
                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    virtuim_verify_chv( cmd_ptr->hdr.slot,
                                        cmd_ptr->verify_chv.chv,
                                        cmd_ptr->verify_chv.chv_digits,
                                        &uim_rpt_buf.rpt_status,
                                        &uim_rpt_buf.sw1,
                                        &uim_rpt_buf.sw2
                                      );
                    uim_rpt_buf.rpt_type = UIM_VERIFY_CHV_R;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                    if (cmd_ptr->verify_chv.chv_digits[0] == '\0')
                    {
                      uim_rpt_buf.rpt_status = UIM_FAIL;
                      uim_rpt_buf.sw1 = 0x63;
                      uim_rpt_buf.sw2 = 0xC3;
                    }
                    else
                    {
#ifdef FEATURE_MMGSDI_JSR_177
                      uim_rpt_buf.rpt_status = UIM_PASS;
                      uim_rpt_buf.sw1 = 0x90;
                      uim_rpt_buf.sw2 = 0x00;
#endif /* #ifdef FEATURE_MMGSDI_JSR_177 */
                    }
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;

            case UIM_UNBLOCK_CHV_F:
                MSG_MED("UIM Server UIM_UNBLOCK_CHV_F received",0,0,0);

                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    virtuim_unblock_chv(cmd_ptr->hdr.slot,
                                        cmd_ptr->unblk_chv.chv,
                                        cmd_ptr->unblk_chv.chv_digits,
                                        cmd_ptr->unblk_chv.new_chv_digits,
                                        &uim_rpt_buf.rpt_status,
                                        &uim_rpt_buf.sw1,
                                        &uim_rpt_buf.sw2
                                        );
                    uim_rpt_buf.rpt_type = UIM_UNBLOCK_CHV_R;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                    if (cmd_ptr->unblk_chv.chv_digits[0] == '\0')
                    {
                      uim_rpt_buf.rpt_status = UIM_FAIL;
                      uim_rpt_buf.sw1 = 0x63;
                      uim_rpt_buf.sw2 = 0xCA;
                    }
                    else
                    {
#ifdef FEATURE_MMGSDI_JSR_177
                      uim_rpt_buf.rpt_status = UIM_PASS;
                      uim_rpt_buf.sw1 = 0x90;
                      uim_rpt_buf.sw2 = 0x00;
#endif /* #ifdef FEATURE_MMGSDI_JSR_177 */
                    }
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;

            case UIM_DISABLE_CHV_F:
                MSG_MED("UIM Server UIM_DISABLE_CHV_F received",0,0,0);

                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    virtuim_disable_chv(cmd_ptr->hdr.slot,
                                        cmd_ptr->disable_chv.chv,
                                        cmd_ptr->disable_chv.chv_digits,
                                        &uim_rpt_buf.rpt_status,
                                        &uim_rpt_buf.sw1,
                                        &uim_rpt_buf.sw2
                                        );
                    uim_rpt_buf.rpt_type = UIM_DISABLE_CHV_R;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;

            case UIM_ENABLE_CHV_F:
                MSG_MED("UIM_ENABLE_CHV_F",0,0,0);

                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    virtuim_enable_chv(cmd_ptr->hdr.slot,
                                        cmd_ptr->enable_chv.chv,
                                        cmd_ptr->enable_chv.chv_digits,
                                        &uim_rpt_buf.rpt_status,
                                        &uim_rpt_buf.sw1,
                                        &uim_rpt_buf.sw2
                                      );
                    uim_rpt_buf.rpt_type = UIM_ENABLE_CHV_R;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;
            case UIM_CHANGE_CHV_F:
                MSG_MED("UIM_CHANGE_CHV_F",0,0,0);
                if (cmd_ptr->hdr.rpt_function != NULL)
                {
                    virtuim_change_chv(cmd_ptr->hdr.slot,
                                        cmd_ptr->change_chv.chv,
                                        cmd_ptr->change_chv.chv_digits,
                                        cmd_ptr->change_chv.new_chv_digits,
                                        &uim_rpt_buf.rpt_status,
                                        &uim_rpt_buf.sw1,
                                        &uim_rpt_buf.sw2
                                      );
                    uim_rpt_buf.rpt_type = UIM_CHANGE_CHV_R;
#ifdef FEATURE_GSDI_UIM_SERVER_USIM
                    uim_rpt_buf.rpt_status = UIM_PASS;
                    uim_rpt_buf.sw1 = 0x90;
                    uim_rpt_buf.sw2 = 0x00;
#endif /* FEATURE_GSDI_UIM_SERVER_USIM */
                    (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                }
                break;

            case UIM_REHABILITATE_F:
                MSG_MED("UIM Server UIM_REHABILITATE_F received",0,0,0);
                MSG_LOW("USING SLOT %x FOR REHABILITATE",cmd_ptr->hdr.slot,0,0);
                uim_rpt_buf.rpt_type = UIM_REHABILITATE_R;
                uim_rpt_buf.rpt_status = UIM_PASS;
                uim_rpt_buf.sw1 = 0x90;
                uim_rpt_buf.sw2 = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);


                break;

            case UIM_INVALIDATE_F:
                MSG_MED("UIM Server UIM_INVALIDATE_F received",0,0,0);
                MSG_LOW("USING SLOT %x FOR INVALIDATE",cmd_ptr->hdr.slot,0,0);
                uim_rpt_buf.rpt_type = UIM_INVALIDATE_R;
                uim_rpt_buf.rpt_status = UIM_PASS;
                uim_rpt_buf.sw1 = 0x90;
                uim_rpt_buf.sw2 = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);

                break;
            case UIM_STORE_ESN_ME_F:
                uim_rpt_buf.rpt_type =   UIM_STORE_ESN_ME_R;
                uim_rpt_buf.rpt_status = UIM_PASS;
                uim_rpt_buf.sw1 = 0x90;
                uim_rpt_buf.sw2 = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;

            case UIM_STATUS_F:
                uim_rpt_buf.rpt_type  = UIM_STATUS_R;
                uim_rpt_buf.rpt_status  = UIM_PASS;
                uim_rpt_buf.sw1     = 0x90;
                uim_rpt_buf.sw2     = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
            case UIM_PREF_SLOT_CHANGE_F:
                uim_rpt_buf.rpt_type  = UIM_PREF_SLOT_CHANGE_R;
                uim_rpt_buf.rpt_status = UIM_PASS;
                if ( cmd_ptr->slot_chg.app == UIM_GSM_APP )
                {
                  virtuim_card_status.gsm_op_slot = cmd_ptr->slot_chg.slot;
                }
                else if ( cmd_ptr->slot_chg.app = UIM_CDMA_APP )
                {
                  virtuim_card_status.cdma_op_slot = cmd_ptr->slot_chg.slot;
                }
                else
                {
                  ERR_FATAL("BAD SLOT ID: %x",0,0,0);
                }
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
            case UIM_RESET_F:
                MSG_HIGH(" RESETTING SIM/USIM/RUIM ",0,0,0);

                /* Set the Just Powered Up Flag to TRUE */
                (void)rex_clr_sigs(&gsdi_uim_server_task_tcb,VIRTUIM_TIMER_EXPIRE_SIG);
                MSG_HIGH("SETTING RESET TIMER...",0,0,0);
                (void) rex_set_timer(&virtuim_reset_delay_timer, VIRTUIM_TIMER_1_SECOND);
                (void)test_sim_server_wait(VIRTUIM_TIMER_EXPIRE_SIG);
                MSG_HIGH("RESET TIMER EXPIRE...DOING RESET",0,0,0);
                (void)rex_clr_sigs(&gsdi_uim_server_task_tcb,VIRTUIM_TIMER_EXPIRE_SIG);
#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
                gsdi_status = virtuim_send_to_virtsat(cmd_ptr,
                                                      &uim_rpt_buf);
#endif
                just_powered_up = TRUE;
                uim_rpt_buf.rpt_type   = UIM_RESET_R;
                uim_rpt_buf.rpt_status = UIM_PASS;
                uim_rpt_buf.sw1 = 0x90;
                uim_rpt_buf.sw2 = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;
#if defined(FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
            case UIM_TERMINAL_PROFILE_F:
            case UIM_ENVELOPE_F:
            case UIM_TERMINAL_RESPONSE_F:
                MSG_HIGH("SENDING COMMAND TO VIRTSAT",0,0,0);
                gsdi_status = virtuim_send_to_virtsat(cmd_ptr,
                                                      &uim_rpt_buf);
                /* Simply Send this command to the Virtual Sat App */
                /* and wait for a response                         */
                MSG_HIGH("CALLING GSTK CALLBACK",0,0,0);
                MSG_HIGH("Calling Callback: %x TPDL",cmd_ptr->hdr.rpt_function,0,0);
                (*(&virtsat_report.status_report.uim_rpt_function))(&uim_rpt_buf);
                break;
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */

#ifdef FEATURE_UIM_ISIM
            case UIM_ISIM_AUTHENTICATE_F:
                MSG_HIGH("SENDING COMMAND TO VIRTSAT",0,0,0);

                uim_rpt_buf.rpt.isim_autn.data_length = 32;
                memset(uim_rpt_buf.rpt.isim_autn.data, 0x00, UIM_MAX_CHARS);
                memset(uim_rpt_buf.rpt.isim_autn.data, 0x21, 29);
                uim_rpt_buf.rpt.isim_autn.data[29] = 0x10;
                uim_rpt_buf.rpt_type  = UIM_ISIM_AUTHENTICATE_R;
                uim_rpt_buf.rpt_status  = UIM_PASS;
                uim_rpt_buf.sw1     = 0x90;
                uim_rpt_buf.sw2     = 0x00;

                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;
#endif /* FEATURE_UIM_ISIM */

            case UIM_POWER_UP_UIM_PASSIVE_F:
                uim_rpt_buf.rpt_type  = UIM_POWER_UP_UIM_PASSIVE_R;
                uim_rpt_buf.rpt_status  = UIM_PASS;
                uim_rpt_buf.sw1     = 0x90;
                uim_rpt_buf.sw2     = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;
            case UIM_POWER_DOWN_F:
                uim_rpt_buf.rpt_type  = UIM_POWER_DOWN_R;
                uim_rpt_buf.rpt_status  = UIM_PASS;
                uim_rpt_buf.sw1     = 0x90;
                uim_rpt_buf.sw2     = 0x00;
                (*(cmd_ptr->hdr.rpt_function))(&uim_rpt_buf);
                break;

            default:
                ERR("UIM Server received unknown command",0,0,0);
                break;
            } /* end switch cmd_ptr->hdr.command */
        }
        else
        {
            ERR("UIM Server NULL cmd_ptr",0,0,0);
            gsdi_status = GSDI_ERROR;
        }
    }
#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
    else if((sigs & VIRTUIM_SEND_SAT_CMD_SIG) != 0)
    {
        (void)rex_clr_sigs(rex_self(),VIRTUIM_SEND_SAT_CMD_SIG);
        MSG_LOW("VIRT SIM:  GOT CMD FROM VIRT APP",0,0,0);
        if ((virtuim_cmd_ptr = (virtuim_cmd_type*)q_get(&uim_virtsat_proactive_cmd_q)) != NULL)
        {
            MSG_LOW("VIRT SIM:  GOT COMMAND FROM QUEUE",0,0,0);
            virtuim_result = virtuim_send_cmd_to_gstk((virtuim_cmd_type*)virtuim_cmd_ptr);
            /* Now send Response back to Virtual SAT APP*/
            (*(virtuim_cmd_ptr->hdr.virtuim_result_cb))(virtuim_result);
        }

    }
#endif /* FEATURE_GSTK && FEATURE_VIRTUAL_SAT */
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
    else if((sigs & VIRTUIM_EVENT_TESTER_RESTART_POWER_UP_SIG) != 0)
    {
        (void)rex_clr_sigs(rex_self(),VIRTUIM_EVENT_TESTER_RESTART_POWER_UP_SIG);
        MSG_LOW("VIRT SIM:  GOT Next EVENT Tester Sig",0,0,0);
    }
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */
    else
    {
        ERR("UIM Server received invalid signal",0,0,0);
        (void)rex_clr_sigs(rex_self(), (rex_sigs_type)~0);
    }
  } /* for loop */
} /* test_sim_server_task */

#if defined(T_MSM6100) && defined (FEATURE_VIRTUAL_SIM)
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS

DESCRIPTION
  This procedure retrieves the current state of UIM.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim status.

SIDE EFFECTS
  None

===========================================================================*/
uim_status_type uim_get_uim_status( void )
{

    return(uim_status);
}

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS_2

DESCRIPTION
  This procedure retrieves the current state of UIM.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim status.

SIDE EFFECTS
  None

===========================================================================*/
uim_status_type uim_get_uim_status_2( void )
{

    return(uim_status_2);
}
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
#endif /* T_MSM6100 && FEATURE_VIRTUAL_SIM */

/*===========================================================================
FUNCTION virtuim_set_card_status

DESCRIPTION
  Set the following global data in virtuim for card based on slot provided
  card presence, gsm df, cdma df, umts adf, pin1 disabled, gsm tk, cdma tk

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_set_card_status(gsdi_slot_id_type slot, virtuim_slot_info_type* slot_info)
{
  switch (slot) {
    case GSDI_SLOT_1:
      memcpy(&virtuim_card_status.slot1, slot_info, sizeof(virtuim_slot_info_type));
      /* toolkit info */
      if (virtuim_card_status.slot1.tk_from_gsm) {
        memcpy(uim_sim[VIRTUIM_GSM_SST_INDEX_1].body,Toolkit_Enabled, sizeof(Toolkit_Enabled));
      }
      else {
        memcpy(uim_sim[VIRTUIM_GSM_SST_INDEX_1].body,Toolkit_Disabled, sizeof(Toolkit_Disabled));
      }
      if (virtuim_card_status.slot1.tk_from_cdma) {
        memcpy(uim_sim[VIRTUIM_CDMA_SST_INDEX_1].body,Toolkit_Enabled, sizeof(Toolkit_Enabled));
      }
      else {
        memcpy(uim_sim[VIRTUIM_CDMA_SST_INDEX_1].body,Toolkit_Disabled, sizeof(Toolkit_Disabled));
      }
      /* pin info */
      switch (virtuim_card_status.slot1.pin1_status)
      {
      case GSDI_PIN_ENABLED:
        /* pin is enabled */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df, PIN_Enabled, sizeof(PIN_Enabled));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_DISABLED:
        /* pin is disabled */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df, PIN_Disabled, sizeof(PIN_Disabled));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_BLOCKED:
        /* pin is blocked */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df, PIN_Blocked, sizeof(PIN_Blocked));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_PERM_BLOCKED:
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df, PIN_Perm_Blocked, sizeof(PIN_Perm_Blocked));
#endif /*FEATURE_MMGSDI_CDMA */
        break;
      }
      break;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    case GSDI_SLOT_2:
      memcpy(&virtuim_card_status.slot2, slot_info, sizeof(virtuim_slot_info_type));
      memcpy(&virtuim_card_status.slot2, slot_info, sizeof(virtuim_slot_info_type));
      /* toolkit info */
      if (virtuim_card_status.slot2.tk_from_gsm) {
        memcpy(uim_sim2[VIRTUIM_GSM_SST_INDEX_2].body,Toolkit_Enabled, sizeof(Toolkit_Enabled));
      }
      else {
        memcpy(uim_sim2[VIRTUIM_GSM_SST_INDEX_2].body,Toolkit_Disabled, sizeof(Toolkit_Disabled));
      }
      if (virtuim_card_status.slot2.tk_from_cdma) {
        memcpy(uim_sim2[VIRTUIM_CDMA_SST_INDEX_2].body,Toolkit_Enabled, sizeof(Toolkit_Enabled));
      }
      else {
        memcpy(uim_sim2[VIRTUIM_CDMA_SST_INDEX_2].body,Toolkit_Disabled, sizeof(Toolkit_Disabled));
      }
      /* pin info */
      switch (virtuim_card_status.slot2.pin1_status)
      {
      case GSDI_PIN_ENABLED:
        /* pin is enabled */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df2, PIN_Enabled, sizeof(PIN_Enabled));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_DISABLED:
        /* pin is disabled */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df2, PIN_Disabled, sizeof(PIN_Disabled));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_BLOCKED:
        /* pin is blocked */
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df2, PIN_Blocked, sizeof(PIN_Blocked));
#endif /*FEATURE_MMGSDI_CDMA */
        break;

      case GSDI_PIN_PERM_BLOCKED:
#ifdef FEATURE_MMGSDI_GSM
#error code not present
#endif /*FEATURE_MMGSDI_GSM */
#ifdef FEATURE_MMGSDI_CDMA
        memcpy(cdma_df2, PIN_Perm_Blocked, sizeof(PIN_Perm_Blocked));
#endif /*FEATURE_MMGSDI_CDMA */
        break;
      }
      break;
#endif /*FEATURE_MMGSDI_DUAL_SLOT */
  }

} /* virtuim_set_card_status */


#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION virtuim_set_operational_slot

DESCRIPTION
  Set the operational slot for the specific application in the passed in
  parameter

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_set_operational_slot(uim_app_type app, uim_slot_type slot)
{
  if ( app == UIM_GSM_APP )
  {
    virtuim_card_status.gsm_op_slot = slot;
  }
  else if (app == UIM_CDMA_APP )
  {
    virtuim_card_status.cdma_op_slot = slot;
  }
  else
  {
    virtuim_card_status.umts_op_slot = slot;
  }

} /* virtuim_set_operational_slot */
#endif /*FEATURE_MMGSDI_DUAL_SLOT */

/*===========================================================================
FUNCTION virtuim_reset_to_power_up

DESCRIPTION
  Reset the just_powered_up parameter to TRUE so that virtuim can restart
  the power up procedure

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_reset_to_power_up (void)
{
   just_powered_up = TRUE;
} /* virtuim_reset_to_power_up */

/*===========================================================================
FUNCTION virtuim_restart

DESCRIPTION
  Reset the just_powered_up parameter, and populate the rest of the virtuim
  card status type data

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtuim_restart( virtuim_card_status_type *card_status_info)
{
  virtuim_set_card_status(GSDI_SLOT_1, &(card_status_info->slot1));
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  virtuim_set_card_status(GSDI_SLOT_2, &(card_status_info->slot2));
  virtuim_set_operational_slot(UIM_GSM_APP, card_status_info->gsm_op_slot);
  virtuim_set_operational_slot(UIM_CDMA_APP, card_status_info->cdma_op_slot);
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  /* UMTS operational slot to be added later on */
  virtuim_reset_to_power_up();
#ifdef FEATURE_MMGSDI_ENABLE_EVENT_TESTER
 (void) rex_set_sigs(&gsdi_uim_server_task_tcb, VIRTUIM_EVENT_TESTER_RESTART_POWER_UP_SIG);
#endif /*FEATURE_MMGSDI_ENABLE_EVENT_TESTER */

}
#endif /* FEATURE_GSDI_UIM_SERVER */
/*lint -e750 Surpress Lint Error:
  : Error (Info) 750: local macro 'VIRTUIM_C' (line 3, file virtuim.c) not
    referenced
*/
