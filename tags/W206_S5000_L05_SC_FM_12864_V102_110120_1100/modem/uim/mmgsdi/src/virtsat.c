/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                  V I R T U A L  S A T   A P P L I C A T I O N

GENERAL DESCRIPTION
 .


EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/virtsat.c#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
06/10/05   tml     Lint fixes
05/11/05   sst     Lint fixes
06/02/04   tml     Added support when FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER is 
                   turned on
05/30/03   jar     Linted.  Support for DOG Reporting
04/10/03   jar     Removed setting and handling of timer virtsat_send_cmd_now_
                   timer.
02/27/03   jar     Added VIRTSAT_RESET_F.
11/05/02   jar     Initial Release

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "tmc.h"
#include "msg.h"
#include "memheap.h"
#include "err.h"
#include "sys_cnst.h"
#include "sys_type.h"
#include "sys_stru.h"
#include "timers.h"
#include "gs.h"
#include "ms_timer.h"
#include "virtuim.h"
#include "gsdi_exp.h"
#ifdef FEATURE_DOG
#include "dog.h"
#endif /* FEATURE_DOG */

#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT)
#include "virtsat.h"
#endif /*#if defined (FEATURE_GSTK) && defined (FEATURE_VIRTUAL_SAT) */

#include "mmgsdiutil.h"

#ifdef FEATURE_VIRTUAL_SAT
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* ------------------------------------------------------------------------

                  VIRTUAL SAT APPLICATION GLOBALS

--------------------------------------------------------------------------- */

#define FEATURE_TEST_DISPLAY_TEXT

#ifdef FEATURE_DOG
rex_timer_type virtsat_rpt_timer;
#endif

/* Define VIRTSAT Timer Types */
rex_timer_type virtsat_send_cmd_now_timer;
rex_timer_type virtsat_rpt_timer;

/* Queue to hold commands */
q_type virtsat_cmd_q;

/* Report Built Back */
//LOCAL uim_rpt_type uim_rsp_report;

/* Global App Information */
virtsat_app_info_type app_info;

/* Determine whether to run in SAT or USAT Mode */
static virtuim_app_mode_type app_mode;
static boolean app_mode_set = FALSE;

/*lint -e785 */
static virtsat_setup_menu_type setup_menu=
{
#ifdef FEATURE_TEST_DISPLAY_TEXT
  0x3C,
  {
    /* Setup Menu
    ** 1) Display Text 1
    ** 2) Display Text 2
    */
#if 0
    0xD0,0x09,0x01,0x03,0x10,0x01,0x04,0x02,0x02,0x81,
    0x82,0x90,0x00
#endif

    0xD0,0x3A,0x81,0x03,0x01,0x25,0x00,0x82,0x02,0x81,
    0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
    0x6E,0x75,0x8F,0x0F,0x01,0x44,0x69,0x73,0x70,0x6C,
    0x61,0x79,0x20,0x54,0x65,0x78,0x74,0x20,0x32,0x8F,
    0x0F,0x02,0x44,0x69,0x73,0x70,0x6C,0x61,0x79,0x20,
    0x54,0x65,0x78,0x74,0x20,0x31,0x18,0x02,0x02,0x02,
    0x90,0x00

#endif /* FEATURE_TEST_DISPLAY_TEXT */

#if defined FEATURE_TEST_SETUP_CALL1 || defined FEATURE_TEST_SETUP_CALL2
  0x22,
    {
      /* 1) Setup Call */
    0xD0,0x24,0x81,0x03,0x01,0x25,0x00,0x82,0x02,0x81,
    0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
    0x6E,0x75,0x8F,0x0B,0x01,0x73,0x65,0x74,0x75,0x70,
    0x63,0x61,0x6C,0x6C,0x33,0x18,0x01,0x02,0x90,0x00
#endif /*  FEATURE_TEST_SETUP_CALL1 || FEATURE_TEST_SETUP_CALL2 */

#if defined FEATURE_TEST_GET_INPUT_CHAR1 || defined FEATURE_TEST_GET_INPUT_CHAR2
    /* Setup menu
       1) Char GI - hidden (1-8)
       2) Char GI - show (0-13)
       */
    0x43,
    {
      0xD0,0x45,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x1D,0x01,0x47,0x65,0x74,0x20,0x49,
      0x6E,0x70,0x75,0x74,0x20,0x31,0x2D,0x20,0x43,0x68,
      0x61,0x72,0x2C,0x20,0x73,0x68,0x6F,0x77,0x20,0x75,
      0x73,0x65,0x72,0x8F,0x0C,0x02,0x47,0x65,0x74,0x20,
      0x49,0x6E,0x70,0x75,0x74,0x20,0x31,0x18,0x02,0x02,
      0x02,0x90,0x00

#endif /* FEATURE_TEST_GET_INPUT_CHAR1 || FEATURE_TEST_GET_INPUT_CHAR2 */

#if defined FEATURE_TEST_GET_INPUT_DIGIT1 || defined FEATURE_TEST_GET_INPUT_DIGIT2
    /* Setup menu
       1) Digit GI - show (<=16)
       2) digit GI - hidden (4)
       */
    0x49,
    {
      0xD0,0x4B,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x13,0x01,0x47,0x65,0x74,0x20,0x49,
      0x6E,0x70,0x75,0x74,0x20,0x32,0x2D,0x20,0x64,0x69,
      0x67,0x69,0x74,0x8F,0x1C,0x02,0x47,0x65,0x74,0x20,
      0x49,0x6E,0x70,0x75,0x74,0x20,0x32,0x20,0x2D,0x20,
      0x64,0x69,0x67,0x69,0x74,0x20,0x6E,0x6F,0x20,0x73,
      0x68,0x6F,0x77,0x18,0x02,0x02,0x02,0x90,0x00
#endif /* FEATURE_TEST_GET_INPUT_DIGIT1 || FEATURE_TEST_GET_INPUT_DIGIT2 */

#if defined FEATURE_TEST_GET_INPUT_DEFAULT1 || defined FEATURE_TEST_GET_INPUT_DEFAULT2
      /* Setup menu
      1) digit default (hidden, <10, 123456789 default)
      2) char default (show, no limit, ABCDEFG)
      */
    0x56,
    {
      0xD0,0x52,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x1A,0x01,0x47,0x65,0x74,0x20,0x49,
      0x6E,0x70,0x75,0x74,0x20,0x33,0x2D,0x20,0x64,0x65,
      0x66,0x61,0x6C,0x74,0x20,0x64,0x69,0x67,0x69,0x74,
      0x8F,0x1C,0x02,0x47,0x65,0x74,0x20,0x49,0x6E,0x70,
      0x75,0x74,0x20,0x33,0x20,0x2D,0x20,0x64,0x65,0x66,
      0x61,0x75,0x6C,0x74,0x20,0x61,0x6C,0x70,0x68,0x61,
      0x18,0x02,0x02,0x02,0x90,0x00
#endif /* FEATURE_TEST_GET_INPUT_DEFAULT1 || FEATURE_TEST_GET_INPUT_DEFAULT2 */

#if defined FEATURE_TEST_PLAY_TONE1 || defined FEATURE_TEST_PLAY_TONE2 || defined FEATURE_TEST_PLAY_TONE3 \
      || defined FEATURE_TEST_PLAY_TONE4 || defined FEATURE_TEST_PLAY_TONE5 || defined FEATURE_TEST_PLAY_TONE6

    0x47,
    {
      0xD0,0x45,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x12,0x01,0x70,0x6C,0x61,0x79,0x20,
      0x74,0x6F,0x6E,0x65,0x20,0x6E,0x6F,0x74,0x68,0x69,
      0x6E,0x67,0x8F,0x17,0x02,0x70,0x6C,0x61,0x79,0x20,
      0x74,0x6F,0x6E,0x65,0x20,0x77,0x20,0x65,0x76,0x65,
      0x72,0x79,0x74,0x68,0x69,0x6E,0x67,0x18,0x02,0x02,
      0x02,0x90,0x00
#endif /* FEATURE_TEST_PLAY_TONE1 || FEATURE_TEST_PLAY_TONE2 || FEATURE_TEST_PLAY_TONE3 \
       || FEATURE_TEST_PLAY_TONE4 || FEATURE_TEST_PLAY_TONE5 || FEATURE_TEST_PLAY_TONE6 */

#if defined FEATURE_TEST_SMS_TPDU_ONLY || defined FEATURE_TEST_SMS_ALL || defined FEATURE_TEST_SMS_ALPHA_TPDU || \
 defined FEATURE_TEST_SMS_ADDRESS_ALPHA_ERROR

    0x4C,
    {
      0xD0,0x4A,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x15,0x01,0x73,0x65,0x6E,0x64,0x20,
      0x73,0x6D,0x73,0x20,0x77,0x20,0x74,0x70,0x64,0x75,
      0x20,0x6F,0x6E,0x6C,0x79,0x8F,0x19,0x02,0x73,0x65,
      0x6E,0x64,0x20,0x73,0x6D,0x73,0x20,0x77,0x69,0x74,
      0x68,0x20,0x65,0x76,0x65,0x72,0x79,0x74,0x68,0x69,
      0x6E,0x67,0x18,0x02,0x02,0x02,0x90,0x00

#endif /*  FEATURE_TEST_SMS_TPDU_ONLY || FEATURE_TEST_SMS_ALL || FEATURE_TEST_SMS_ALPHA_TPDU || \
           FEATURE_TEST_SMS_ADDRESS_ALPHA_ERROR */

#if defined FEATURE_TEST_GET_INKEY_CHAR || defined FEATURE_TEST_GET_INKEY_YES_NO || defined FEATURE_TEST_GET_INKEY_DIGIT
      0x4C,
    {
      0xD0,0x4A,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x15,0x01,0x73,0x65,0x6E,0x64,0x20,
      0x73,0x6D,0x73,0x20,0x77,0x20,0x74,0x70,0x64,0x75,
      0x20,0x6F,0x6E,0x6C,0x79,0x8F,0x19,0x02,0x73,0x65,
      0x6E,0x64,0x20,0x73,0x6D,0x73,0x20,0x77,0x69,0x74,
      0x68,0x20,0x65,0x76,0x65,0x72,0x79,0x74,0x68,0x69,
      0x6E,0x67,0x18,0x02,0x02,0x02,0x90,0x00
#endif /* FEATURE_TEST_GET_INKEY_CHAR || FEATURE_TEST_GET_INKEY_YES_NO || FEATURE_TEST_GET_INKEY_DIGIT */

#if defined FEATURE_TEST_MORE_TIME || defined FEATURE_TEST_SETUP_IDLE_MODE_TEXT
      0x4C,
    {
      0xD0,0x4A,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x15,0x01,0x73,0x65,0x6E,0x64,0x20,
      0x73,0x6D,0x73,0x20,0x77,0x20,0x74,0x70,0x64,0x75,
      0x20,0x6F,0x6E,0x6C,0x79,0x8F,0x19,0x02,0x73,0x65,
      0x6E,0x64,0x20,0x73,0x6D,0x73,0x20,0x77,0x69,0x74,
      0x68,0x20,0x65,0x76,0x65,0x72,0x79,0x74,0x68,0x69,
      0x6E,0x67,0x18,0x02,0x02,0x02,0x90,0x00
#endif /* FEATURE_TEST_MORE_TIME || FEATURE_TEST_SETUP_IDLE_MODE_TEXT */

#if defined FEATURE_TEST_SS_SS_STRING_ONLY || defined FEATURE_TEST_SS_ALL || defined FEATURE_TEST_SS_NO_STRING_ERROR
      0x39,
      {
      0xD0,0x37,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x0D,0x01,0x53,0x53,0x20,0x2D,0x20,
      0x77,0x20,0x61,0x6C,0x70,0x68,0x61,0x8F,0x0E,0x02,
      0x53,0x53,0x20,0x2D,0x20,0x77,0x6F,0x20,0x61,0x6C,
      0x70,0x68,0x61,0x18,0x02,0x02,0x02,0x90,0x00

#endif /* FEATURE_TEST_SS_SS_STRING_ONLY || FEATURE_TEST_SS_ALL || FEATURE_TEST_SS_NO_STRING_ERROR */

#if defined FEATURE_TEST_USSD_STRING_ONLY || defined FEATURE_TEST_USSD_ALL || defined FEATURE_TEST_USSD_ERROR
      0x39,
      {
      0xD0,0x37,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x0D,0x01,0x53,0x53,0x20,0x2D,0x20,
      0x77,0x20,0x61,0x6C,0x70,0x68,0x61,0x8F,0x0E,0x02,
      0x53,0x53,0x20,0x2D,0x20,0x77,0x6F,0x20,0x61,0x6C,
      0x70,0x68,0x61,0x18,0x02,0x02,0x02,0x90,0x00

#endif /* FEATURE_TEST_USSD_STRING_ONLY || FEATURE_TEST_USSD_ALL || FEATURE_TEST_USSD_ERROR */

#if defined FEATURE_TEST_PROVIDE_IMEI || defined FEATURE_TEST_PROVIDE_LOCATION
      0x31,
      {
      0xD0,0x2F,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x05,0x01,0x69,0x6D,0x65,0x69,0x8F,
      0x0E,0x02,0x4C,0x6F,0x63,0x61,0x74,0x69,0x6F,0x6E,
      0x20,0x69,0x6E,0x66,0x6F,0x18,0x02,0x02,0x02,0x90,
      0x00

#endif /* FEATURE_TEST_PROVIDE_IMEI || FEATURE_TEST_PROVIDE_LOCATION */

#if defined FEATURE_TEST_PROVIDE_NMR || defined FEATURE_TEST_PROVIDE_DATE || defined FEATURE_TEST_BCCH || \
      defined FEATURE_TEST_LANG || defined FEATURE_TEST_TIMING_ADV
      0x31,
      {
      0xD0,0x2F,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x05,0x01,0x69,0x6D,0x65,0x69,0x8F,
      0x0E,0x02,0x4C,0x6F,0x63,0x61,0x74,0x69,0x6F,0x6E,
      0x20,0x69,0x6E,0x66,0x6F,0x18,0x02,0x02,0x02,0x90,
      0x00

#endif /* FEATURE_TEST_PROVIDE_NMR || FEATURE_TEST_PROVIDE_DATE || FEATURE_TEST_BCCH || \
          FEATURE_TEST_LANG || FEATURE_TEST_TIMING_ADV */

#if defined FEATURE_TEST_SETUP_EVT || defined FEATURE_TEST_POLLING
      0x31,
      {
      0xD0,0x2F,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x05,0x01,0x69,0x6D,0x65,0x69,0x8F,
      0x0E,0x02,0x4C,0x6F,0x63,0x61,0x74,0x69,0x6F,0x6E,
      0x20,0x69,0x6E,0x66,0x6F,0x18,0x02,0x02,0x02,0x90,
      0x00

#endif /* FEATURE_TEST_SETUP_EVT || FEATURE_TEST_POLLING */

#if defined  FEATURE_TEST_LAUNCH_BROWSER_ALL || defined FEATURE_TEST_LAUNCH_BROWSER_URL_ALPHA
#if 0
    0x31,
      {
      0xD0,0x2F,0x81,0x03,0x01,0x25,0x80,0x82,0x02,0x81,
      0x82,0x85,0x09,0x4D,0x61,0x69,0x6E,0x20,0x4D,0x65,
      0x6E,0x75,0x8F,0x05,0x01,0x69,0x6D,0x65,0x69,0x8F,
      0x0E,0x02,0x4C,0x6F,0x63,0x61,0x74,0x69,0x6F,0x6E,
      0x20,0x69,0x6E,0x66,0x6F,0x18,0x02,0x02,0x02,0x90,
      0x00
#endif
#endif /* FEATURE_TEST_LAUNCH_BROWSER_ALL || FEATURE_TEST_LAUNCH_BROWSER_URL_ALPHA */
      }
};
static virtsat_app_type sat_app_1 =
{
  0x01, /* App1 Proactive Commands */
  {
#ifdef FEATURE_TEST_DISPLAY_TEXT
    0x38, /* Proactive Command Length */
    { 0xD0,0x36,0x81,0x03,0x02,0x21,0x80,0x82,0x02,0x81,
      0x02,0x8D,0x2B,0x04,0x54,0x65,0x78,0x74,0x20,0x31,
      0x20,0x4E,0x6F,0x72,0x6D,0x61,0x6C,0x20,0x50,0x72,
      0x69,0x6F,0x72,0x69,0x74,0x79,0x20,0x55,0x73,0x65,
      0x72,0x20,0x43,0x6C,0x65,0x61,0x72,0x73,0x20,0x4D,
      0x65,0x73,0x73,0x61,0x67,0x65,0x90,0x00
    },
    0x0D, /* Expected Terminal Response Length */
    {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x01,0x25,0x00,
      0x83,0x01,0x00
    },
    0x00, /* Help Text Length */
    {
      0xFF
    }
#endif
#ifdef FEATURE_TEST_SETUP_CALL1
    0x1F,
    {
      0xD0,0x1D,0x81,0x03,0x03,0x10,0x00,0x82,0x02,0x81,
      0x83,0x05,0x0B,0x43,0x61,0x6C,0x6C,0x69,0x6E,0x67,
      0x20,0x2E,0x2E,0x2E,0x86,0x05,0x81,0x48,0x35,0x14,
      0xF8,0x90,0x00
    },
    0x0D,
    {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x10,0x00,
      0x83,0x01,0x00
    }
#endif
#ifdef FEATURE_TEST_SETUP_CALL2
      /* user confirm: Calling...
         call setup text: Calling...
         address: 8453418
         cap: 0x65, 0x66, 0x67, 0x21
         subaddress: 1234567890
         duration: second: 10 */
    0x44,
    {
      0xD0,0x40,0x81,0x03,0x03,0x10,0x00,0x82,0x02,0x81,
      0x83,0x05,0x0B,0x43,0x61,0x6C,0x6C,0x69,0x6E,0x67,
      0x20,0x2E,0x2E,0x2E,0x86,0x05,0x81,0x48,0x35,0x14,
      0xF8,0x87,0x04,0x65,0x66,0x67,0x21,0x88,0x0A,0x31,
      0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x84,
      0x02,0x01,0x10,0x85,0x0B,0x43,0x61,0x6C,0x6C,0x69,
      0x6E,0x67,0x20,0x2E,0x2E,0x2E,0x90,0x00
    },
    0x0D,
    {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x10,0x00,
      0x83,0x01,0x00
    }
#endif
#ifdef FEATURE_TEST_GET_INPUT_CHAR1
    0x2B,
    {
      /* char, 0-13, show user */
      0xD0,0x28,0x81,0x03,0x02,0x23,0x81,0x82,0x02,0x81,
      0x82,0x8D,0x19,0x04,0x50,0x6C,0x65,0x61,0x73,0x65,
      0x20,0x65,0x6E,0x74,0x65,0x72,0x20,0x43,0x68,0x61,
      0x72,0x20,0x28,0x30,0x2D,0x31,0x33,0x29,0x11,0x02,
      0x00,0x0D,0x90,0x00
    },
    0x13,
    {
      0xA0,0x14,0x00,0x00,0x0E,0x81,0x03,0x02,0x23,0x81,
      0x83,0x01,0x00,0x8D,0x04,0x04,0x6B,0x65,0x79
    }
#endif
#ifdef FEATURE_TEST_GET_INPUT_DIGIT1
    0x2B,
    {
      /* max=min = 4, digit, hidden */
      0xD0,0x28,0x81,0x03,0x02,0x23,0x80,0x82,0x02,0x81,
      0x82,0x8D,0x19,0x04,0x50,0x6C,0x65,0x61,0x73,0x65,
      0x20,0x65,0x6E,0x74,0x65,0x72,0x20,0x6E,0x75,0x6D,
      0x62,0x65,0x72,0x28,0x3C,0x31,0x36,0x29,0x11,0x02,
      0x00,0x10,0x90,0x00
    },
    0x1B,
    {
      0xA0,0x14,0x00,0x00,0x17,0x81,0x03,0x02,0x23,0x80,
      0x83,0x01,0x00,0x8D,0x0D,0x04,0x32,0x32,0x33,0x33,
      0x36,0x35,0x32,0x33,0x35,0x32,0x34,0x36
    }
#endif
#ifdef FEATURE_TEST_GET_INPUT_DEFAULT1
     0x38,
     {
       /* digit: 123456789
       0-10 hidden */
      0xD0,0x34,0x81,0x03,0x02,0x23,0x84,0x82,0x02,0x81,
      0x82,0x8D,0x19,0x04,0x50,0x6C,0x65,0x61,0x73,0x65,
      0x20,0x65,0x6E,0x74,0x65,0x72,0x20,0x6E,0x75,0x6D,
      0x62,0x65,0x72,0x28,0x3C,0x31,0x36,0x29,0x11,0x02,
      0x00,0x0A,0x17,0x0A,0x04,0x31,0x32,0x33,0x34,0x35,
      0x36,0x37,0x38,0x39,0x90,0x00
     },
  0x19,
      {
      0xA0,0x14,0x00,0x00,0x14,0x81,0x03,0x02,0x23,0x84,
      0x83,0x01,0x00,0x8D,0x0A,0x04,0x31,0x32,0x33,0x34,
      0x35,0x36,0x37,0x38,0x39
  }
#endif
#ifdef FEATURE_TEST_GET_INPUT_CHAR2
    0x36,
    {
      /*1-8, no user */
      /* SMS default */
      0xD0,0x27,0x81,0x03,0x03,0x23,0x85,0x82,0x02,0x81,
      0x82,0x8D,0x18,0x04,0x45,0x6E,0x74,0x65,0x72,0x20,
      0x43,0x68,0x61,0x72,0x20,0x53,0x74,0x72,0x69,0x6E,
      0x67,0x20,0x28,0x31,0x2D,0x38,0x29,0x11,0x02,0x01,
      0x08,0x90,0x00
    },
    0x14,
    {
      0xA0,0x14,0x00,0x00,0x0F,0x81,0x03,0x03,0x23,0x84,
      0x83,0x01,0x00,0x8D,0x05,0x04,0x38,0x36,0x35,0x34
    }
#endif
#ifdef FEATURE_TEST_GET_INPUT_DIGIT2
    0x39,
    {
      /* max: 16, digit, show user
      */
      0xD0,0x20,0x81,0x03,0x03,0x23,0x84,0x82,0x02,0x81,
      0x82,0x8D,0x11,0x04,0x45,0x6E,0x74,0x65,0x72,0x20,
      0x6E,0x75,0x6D,0x62,0x65,0x72,0x20,0x28,0x34,0x29,
      0x11,0x02,0x04,0x04,0x90,0x00
    },
    0x14,
      {
      0xA0,0x14,0x00,0x00,0x0F,0x81,0x03,0x03,0x23,0x84,
      0x83,0x01,0x00,0x8D,0x05,0x04,0x32,0x33,0x35,0x32
    }
#endif
#ifdef FEATURE_TEST_GET_INPUT_DEFAULT2
      0x28,
      {
        /* default text: "ABCDEF"
         show user
        supposedly unlimited input -> 61?
        SMS default */
      0xD0,0x24,0x81,0x03,0x03,0x23,0x81,0x82,0x02,0x81,
      0x82,0x8D,0x0B,0x04,0x45,0x6E,0x74,0x65,0x72,0x20,
      0x54,0x65,0x78,0x74,0x11,0x02,0x00,0x3D,0x17,0x08,
      0x04,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x90,0x00
  },
   0x18,
      {
      0xA0,0x14,0x00,0x00,0x13,0x81,0x03,0x03,0x23,0x80,
      0x83,0x01,0x00,0x8D,0x09,0x04,0x41,0x42,0x43,0x44,
      0x45,0x46,0x47,0x35
  }
#endif
#if defined FEATURE_TEST_GET_INKEY_CHAR
   0x16,
      {
        /* enter char,
           text: effgg hh*/
      0xD0,0x14,0x81,0x03,0x03,0x22,0x81,0x82,0x02,0x81,
      0x82,0x8D,0x09,0x04,0x65,0x66,0x66,0x67,0x67,0x20,
      0x68,0x68,0x90,0x00
   },
   0x20,
      {
      0xA0,0x14,0x00,0x00,0x13,0x81,0x03,0x03,0x23,0x80,
      0x83,0x01,0x00,0x8D,0x02,0x04,0x65
  }
#endif
#if defined FEATURE_TEST_GET_INKEY_YES_NO
  0x12,
      {
        /* yes
           text: efgh*/
      0xD0,0x10,0x81,0x03,0x03,0x22,0x85,0x82,0x02,0x81,
      0x82,0x8D,0x05,0x04,0x65,0x66,0x67,0x68,0x90,0x00
   },
   0x20,
      {
      0xA0,0x14,0x00,0x00,0x13,0x81,0x03,0x03,0x23,0x80,
      0x83,0x01,0x00,0x8D,0x02,0x04,0x65
  }
#endif
#if defined FEATURE_TEST_GET_INKEY_DIGIT
  0x18,
      {
        /* digit
           text: 1929394959*/
      0xD0,0x16,0x81,0x03,0x03,0x22,0x80,0x82,0x02,0x81,
      0x82,0x8D,0x0B,0x04,0x31,0x39,0x32,0x39,0x33,0x39,
      0x34,0x39,0x35,0x39,0x90,0x00
   },
   0x20,
      {
      0xA0,0x14,0x00,0x00,0x13,0x81,0x03,0x03,0x23,0x80,
      0x83,0x01,0x00,0x8D,0x02,0x04,0x30
  }
#endif


#ifdef FEATURE_TEST_PLAY_TONE1
   0x14,
   {
     /* duration : 0.1 seconds
     tone: dial tone */
      0xD0,0x10,0x81,0x03,0x02,0x20,0x00,0x82,0x02,0x81,
      0x03,0x0E,0x01,0x01,0x04,0x02,0x02,0x01,0x90,0x00
   },
   0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x02,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_PLAY_TONE2
  0x2E,
  {
    /* duration: 10, unit: sec
    tone: 0x02 (congestion? or called subscriber busy?)
    text: "play tonessss quiquiqui"
    */
      0xD0,0x2A,0x81,0x03,0x03,0x20,0x00,0x82,0x02,0x81,
      0x03,0x05,0x18,0x70,0x6C,0x61,0x79,0x20,0x74,0x6F,
      0x6E,0x65,0x73,0x73,0x73,0x73,0x20,0x20,0x71,0x75,
      0x69,0x71,0x75,0x69,0x71,0x75,0x69,0x0E,0x01,0x02,
      0x04,0x02,0x01,0x0A,0x90,0x00
  },
  0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_PLAY_TONE3
      0x25,
  {
    /* alpha only
    text: "play tonessss quiquiqui"
    */
      0xD0,0x23,0x81,0x03,0x03,0x20,0x00,0x82,0x02,0x81,
      0x03,0x05,0x18,0x70,0x6C,0x61,0x79,0x20,0x74,0x6F,
      0x6E,0x65,0x73,0x73,0x73,0x73,0x20,0x20,0x71,0x75,
      0x69,0x71,0x75,0x69,0x71,0x75,0x69,0x90,0x00
  },
  0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_PLAY_TONE4
      0x11,
  {
    /* duration only
      48, unit: min
    */
      0xD0,0x0D,0x81,0x03,0x03,0x20,0x00,0x82,0x02,0x81,
      0x03,0x04,0x02,0x00,0x30,0x90,0x00
  },
  0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_PLAY_TONE5
      0x10,
  {
    /* tone only
      0x13 - RFU
    */
      0xD0,0x0C,0x81,0x03,0x03,0x20,0x00,0x82,0x02,0x81,
      0x03,0x0E,0x01,0x13,0x90,0x00
  },
  0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_PLAY_TONE6
  0x38,
  {
    /*
    tone: 0x08 ringing
    text: "play tonessss quiquiqui"
    */
      0xD0,0x26,0x81,0x03,0x03,0x20,0x00,0x82,0x02,0x81,
      0x03,0x05,0x18,0x70,0x6C,0x61,0x79,0x20,0x74,0x6F,
      0x6E,0x65,0x73,0x73,0x73,0x73,0x20,0x20,0x71,0x75,
      0x69,0x71,0x75,0x69,0x71,0x75,0x69,0x0E,0x01,0x08,
      0x90,0x00
  },
  0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x20,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_SMS_TPDU_ONLY
      /* tpdu only */
   0x41,
   {
     /*
        tpdu: dest addr: 9630963
              text: I am trying to send sms with tpdu only */
      0xD0,0x3E,0x81,0x03,0x03,0x13,0x01,0x82,0x02,0x81,
      0x83,0x8B,0x33,0x11,0xFF,0x07,0x81,0x69,0x03,0x69,
      0xF3,0x00,0x04,0x8F,0x27,0x69,0x20,0x61,0x6D,0x20,
      0x74,0x72,0x79,0x69,0x6E,0x67,0x20,0x74,0x6F,0x20,
      0x73,0x65,0x6E,0x64,0x20,0x73,0x6D,0x73,0x20,0x77,
      0x69,0x74,0x68,0x20,0x74,0x70,0x64,0x75,0x20,0x6F,
      0x6E,0x6C,0x79,0x21,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_SMS_ALL
 /* sms with all possible tlv except icon! */
   0x53,
   {
     /* alpha = Sending sms
        address: 90807060504030201
        tpdu: dest addr: 0123456789
              text: Hello, this is SMS with all TLVs */
      0xD0,0x51,0x81,0x03,0x06,0x13,0x01,0x82,0x02,0x81,
      0x83,0x05,0x0B,0x73,0x65,0x6E,0x64,0x69,0x6E,0x67,
      0x20,0x73,0x6D,0x73,0x86,0x0A,0x81,0x09,0x08,0x07,
      0x06,0x05,0x04,0x03,0x02,0xF1,0x8B,0x2D,0x11,0xFF,
      0x0A,0x81,0x10,0x32,0x54,0x76,0x98,0x00,0x04,0x8F,
      0x20,0x48,0x65,0x6C,0x6C,0x6F,0x2C,0x20,0x74,0x68,
      0x69,0x73,0x20,0x69,0x73,0x20,0x53,0x4D,0x53,0x20,
      0x77,0x69,0x74,0x68,0x20,0x61,0x6C,0x6C,0x20,0x54,
      0x4C,0x56,0x73,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x06,0x13,0x01,
      0x83,0x01,0x00
      }

#endif
#if defined FEATURE_TEST_SMS_ALPHA_TPDU
   0x52,
   {
     /* alpha = Sending!!!! qui!
        tpdu: dest addr: 9630963
              text: I am trying to send sms with tpdu only */
      0xD0,0x50,0x81,0x03,0x03,0x13,0x01,0x82,0x02,0x81,
      0x83,0x05,0x10,0x73,0x65,0x6E,0x64,0x69,0x6E,0x67,
      0x21,0x21,0x21,0x21,0x20,0x71,0x75,0x69,0x21,0x8B,
      0x33,0x11,0xFF,0x07,0x81,0x69,0x03,0x69,0xF3,0x00,
      0x04,0x8F,0x27,0x69,0x20,0x61,0x6D,0x20,0x74,0x72,
      0x79,0x69,0x6E,0x67,0x20,0x74,0x6F,0x20,0x73,0x65,
      0x6E,0x64,0x20,0x73,0x6D,0x73,0x20,0x77,0x69,0x74,
      0x68,0x20,0x74,0x70,0x64,0x75,0x20,0x6F,0x6E,0x6C,
      0x79,0x21,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_SMS_ADDRESS_ALPHA_ERROR
   0x24,
   {
     /* alpha = Sending!!!! qui!
        address: 13132424 */
      0xD0,0x22,0x81,0x03,0x03,0x13,0x01,0x82,0x02,0x81,
      0x83,0x05,0x10,0x73,0x65,0x6E,0x64,0x69,0x6E,0x67,
      0x21,0x21,0x21,0x21,0x20,0x71,0x75,0x69,0x21,0x86,
      0x05,0x81,0x31,0x31,0x42,0x42,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_MORE_TIME
  0x0B,
   {

      0xD0,0x09,0x81,0x03,0x03,0x02,0x01,0x82,0x02,0x81,
      0x83,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif
#ifdef FEATURE_TEST_SETUP_IDLE_MODE_TEXT
  0x15, /* Proactive Command Length */
    { 0xD0,0x13,0x81,0x03,0x02,0x28,0x80,0x82,0x02,0x81,
      0x02,0x8D,0x08,0x04,0x67,0x72,0x65,0x61,0x74,0x21,
      0x21,0x90,0x00
    },
    0x0D, /* Expected Terminal Response Length */
    {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x01,0x25,0x00,
      0x83,0x01,0x00
    }
#endif
#if defined FEATURE_TEST_SS_ALL
   0x2E,
   {
     /* alpha: making SS call with alpha"
        TON = 0x81, string: 123456789 */
      0xD0,0x2C,0x81,0x03,0x03,0x11,0x00,0x82,0x02,0x81,
      0x83,0x05,0x19,0x6D,0x61,0x6B,0x69,0x6E,0x67,0x20,
      0x73,0x73,0x20,0x63,0x61,0x6C,0x6C,0x20,0x77,0x69,
      0x74,0x68,0x20,0x61,0x6C,0x70,0x68,0x61,0x89,0x06,
      0x81,0x21,0x43,0x65,0x87,0xF9,0x90,0x00
      },
   0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_SS_SS_STRING_ONLY
   0x13,
      {
     /* ss: 9876543210 */
      0xD0,0x11,0x81,0x03,0x06,0x11,0x00,0x82,0x02,0x81,
      0x83,0x89,0x06,0x81,0x89,0x67,0x45,0x23,0x01,0x90,
      0x00
      },
    0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x06,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_SS_NO_STRING_ERROR
      0x26,
   {
     /* alpha: making SS call with alpha"
         */
      0xD0,0x24,0x81,0x03,0x03,0x11,0x00,0x82,0x02,0x81,
      0x83,0x05,0x19,0x6D,0x61,0x6B,0x69,0x6E,0x67,0x20,
      0x73,0x73,0x20,0x63,0x61,0x6C,0x6C,0x20,0x77,0x69,
      0x74,0x68,0x20,0x61,0x6C,0x70,0x68,0x61,0x90,0x00
      },
   0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_USSD_ALL
   0x2D,
   {
     /* alpha: making USSD call with alpha"
        string: Ceee  dcs = 1*/
      0xD0,0x2B,0x81,0x03,0x03,0x12,0x00,0x82,0x02,0x81,
      0x83,0x05,0x19,0x6D,0x61,0x6B,0x69,0x6E,0x67,0x20,
      0x73,0x73,0x20,0x63,0x61,0x6C,0x6C,0x20,0x77,0x69,
      0x74,0x68,0x20,0x61,0x6C,0x70,0x68,0x61,0x0A,0x05,
      0x21,0x43,0x65,0x65,0x65,0x90,0x00
      },
   0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_USSD_STRING_ONLY
   0x12,
      {
     /* ussd: 70!1  dcs = 30 */
      0xD0,0x10,0x81,0x03,0x06,0x12,0x00,0x82,0x02,0x81,
      0x83,0x0A,0x05,0x30,0x37,0x30,0x21,0x31,0x90,0x00
      },
    0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x06,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_USSD_ERROR
      0x26,
   {
     /* alpha: making SS call with alpha"
         */
      0xD0,0x24,0x81,0x03,0x03,0x12,0x00,0x82,0x02,0x81,
      0x83,0x05,0x19,0x6D,0x61,0x6B,0x69,0x6E,0x67,0x20,
      0x73,0x73,0x20,0x63,0x61,0x6C,0x6C,0x20,0x77,0x69,
      0x74,0x68,0x20,0x61,0x6C,0x70,0x68,0x61,0x90,0x00
      },
   0x0D,
      {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x11,0x00,
      0x83,0x01,0x00
      }
#endif
#if defined FEATURE_TEST_PROVIDE_IMEI
      0x0B,
      {
      0xD0,0x09,0x81,0x03,0x02,0x26,0x01,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      0x17,
      {
      0xA0,0x14,0x00,0x00,0x12,0x81,0x03,0x02,0x26,0x01,
      0x83,0x01,0x00,0x94,0x08,0x11,0x22,0x33,0x44,0x55,
      0x66,0x77,0x88
      }
#endif
#if defined FEATURE_TEST_PROVIDE_LOCATION
      0x0B,
      {
      0xD0,0x09,0x81,0x03,0x04,0x26,0x00,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      0x17,
      {
      0xA0,0x14,0x00,0x00,0x11,0x81,0x03,0x04,0x26,0x00,
      0x83,0x01,0x00,0x93,0x07,0x11,0x22,0x33,0x44,0x55,
      0x66,0x77
      }
#endif
#if defined FEATURE_TEST_PROVIDE_NMR
      0x0B,
      {
      0xD0,0x09,0x81,0x03,0x02,0x26,0x02,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      0x32, //51,
      {
      0xA0,0x14,0x00,0x00,0x4C,0x81,0x03,0x03,0x26,0x02,
      0x83,0x01,0x00,0x96,0x10,0x00,0x11,0x22,0x33,0x44,
      0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,
      0xFF,0x9D,0x30,0x00,0x11,0x22,0x33,0x44,0x55,0x66,
      0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00
   /*   0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
      0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,
      0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,
      0xFF
     */
      }
#endif
#if defined FEATURE_TEST_PROVIDE_DATE
      0x0B,
      {
      0xD0,0x09,0x81,0x03,0x03,0x26,0x03,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      0x17,
      {
      0xA0,0x14,0x00,0x00,0x11,0x81,0x03,0x03,0x26,0x03,
      0x83,0x01,0x00,0xA6,0x07,0x20,0x21,0x70,0x81,0x20,
      0x34,0x46
      }
#endif

#if defined FEATURE_TEST_BCCH
      0x0B,
      {
      0xD0,0x09,0x81,0x03,0x03,0x26,0x02,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      0x32, //51,
      {
      0xA0,0x14,0x00,0x00,0x4C,0x81,0x03,0x03,0x26,0x02,
      0x83,0x01,0x00,0x96,0x10,0x00,0x11,0x22,0x33,0x44,
      0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,
      0xFF,0x9D,0x30,0x00,0x11,0x22,0x33,0x44,0x55,0x66,
      0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00
   /*   0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,
      0xBB,0xCC,0xDD,0xEE,0xFF,0x00,0x11,0x22,0x33,0x44,
      0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,
      0xFF
     */
      }
#endif /* FEATURE_TEST_BCCH */
#ifdef FEATURE_TEST_LANG
       0x0B,
      {
      0xD0,0x09,0x81,0x03,0x03,0x26,0x04,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      /* not really correct output! */
      0x17,
      {
      0xA0,0x14,0x00,0x00,0x11,0x81,0x03,0x03,0x26,0x03,
      0x83,0x01,0x00,0xA6,0x07,0x20,0x21,0x70,0x81,0x20,
      0x34,0x46
      }
#endif /* FEATURE_TEST_LANG */
#ifdef FEATURE_TEST_TIMING_ADV
       0x0B,
      {
      0xD0,0x09,0x81,0x03,0x05,0x26,0x05,0x82,0x02,0x81,
      0x82,0x90,0x00
      },
      /* not really correct output! */
      0x17,
      {
      0xA0,0x14,0x00,0x00,0x11,0x81,0x03,0x03,0x26,0x03,
      0x83,0x01,0x00,0xA6,0x07,0x20,0x21,0x70,0x81,0x20,
      0x34,0x46
      }
#endif /* FEATURE_TEST_TIMING_ADV */
#if defined FEATURE_TEST_SETUP_EVT
      0x12,
   {
        /* Mt, call conncected/disconnected, location status
        idle screen */

      0xD0,0x10,0x81,0x03,0x03,0x05,0x01,0x82,0x02,0x81,
      0x83,0x99,0x05,0x00,0x03,0x01,0x02,0x05,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif /* FEATURE_TEST_SETUP_EVT */
#if defined FEATURE_TEST_POLLING
    0x0F,
   {
        /* polling, duration: 10 unit, 10 seconds */

      0xD0,0x0D,0x81,0x03,0x03,0x03,0x00,0x82,0x02,0x81,
      0x83,0x84,0x02,0x01,0x0A,0x90,0x00
   },
   0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif /* FEATURE_TEST_POLLING */
#if defined  FEATURE_TEST_LAUNCH_BROWSER_URL_ALPHA
      /*
      alpha = Sending!!!! qui!
      url: http

         */
#if 0
      0x3D,
      {
      0xD0,0x3B,0x81,0x03,0x03,0x15,0x00,0x82,0x02,0x81,
      0x83,0x31,0x04,0x68,0x74,0x74,0x71,0x33,0x06,0x3F,
      0x00,0x7F,0x10,0x6F,0x3A,0x33,0x06,0x3F,0x00,0x7F,
      0x10,0x6F,0x3B,0x33,0x08,0x3F,0x00,0x7F,0x20,0x4F,
      0x10,0x6F,0x3B,0x05,0x10,0x73,0x65,0x6E,0x64,0x69,
      0x6E,0x67,0x21,0x21,0x21,0x21,0x20,0x71,0x75,0x69,
      0x21,0x90,0x00
      },
      /* not correct */
      0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }
#endif
#endif /* FEATURE_TEST_LAUNCH_BROWSER_URL_ALPHA */
#if defined  FEATURE_TEST_LAUNCH_BROWSER_ALL
      /* text: making SS call with alpha"
      alpha = Sending!!!! qui!
      url: http
      bearer priorty: CSD, SMS, USSd, GPRS
      browser id = 0
         */
      0x59,
      {
      0xD0,0x57,0x81,0x03,0x03,0x15,0x00,0x82,0x02,0x81,
      0x83,0x30,0x01,0x00,0x31,0x04,0x68,0x74,0x74,0x71,
      0x32,0x05,0x01,0x00,0x02,0x03,0x02,0x33,0x06,0x3F,0x00,
      0x7F,0x10,0x6F,0x3A,0x33,0x06,0x3F,0x00,0x7F,0x10,
      0x6F,0x3B,0x8D,0x1A,0x04,0x6D,0x61,0x6B,0x69,0x6E,
      0x67,0x20,0x73,0x73,0x20,0x63,0x61,0x6C,0x6C,0x20,
      0x77,0x69,0x74,0x68,0x20,0x61,0x6C,0x70,0x68,0x61,
      0x05,0x10,0x73,0x65,0x6E,0x64,0x69,0x6E,0x67,0x21,
      0x21,0x21,0x21,0x20,0x71,0x75,0x69,0x21,0x90,0x00
      },
      /* not correct */
      0x0D,
   {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x13,0x01,
      0x83,0x01,0x00
      }

#endif /* FEATURE_TEST_LAUNCH_BROWSER_ALL */
  }
};
/*lint +e785 */

#ifdef FEATURE_VIRTSAT_MULTIPLE_APPS
/*lint -e528*/
static virtsat_app_type sat_app_2 =
{
  0x01, /* App1 Proactive Commands */
  {
    0x36, /* Proactive Command Length */
    {
      0xD0,0x32,0x81,0x03,0x03,0x21,0x81,0x82,0x02,0x81,
      0x02,0x8D,0x27,0x04,0x44,0x69,0x73,0x70,0x6C,0x61,
      0x79,0x20,0x54,0x65,0x78,0x74,0x20,0x48,0x69,0x67,
      0x68,0x20,0x50,0x72,0x69,0x6F,0x72,0x69,0x74,0x79,
      0x20,0x55,0x73,0x65,0x72,0x20,0x43,0x6C,0x65,0x61,
      0x72,0x73,0x90,0x00
    },
    0x0D,
    {
      0xA0,0x14,0x00,0x00,0x08,0x81,0x03,0x03,0x21,0x81,
      0x83,0x01,0x00
    },
    0x00, /* Help Text Length */
    {
      0xFF
    }
  }
};
/*lint +e528*/
#endif


/*===========================================================================
FUNCTION VIRSAT_VIRTUIM_CB

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECT
  gsdi_returns_T

===========================================================================*/
/*lint -e715*/
LOCAL void virtsat_virtuim_cb(
  virtuim_result_type virtuim_result
)
{
  MSG_HIGH("VIRTSAT: VIRTUIM CB %x ",virtuim_result,0,0);

  (void)rex_set_sigs(&virtsat_tcb,VIRTSAT_CMD_RSP_SIG);
}
/*lint +e715*/
/*===========================================================================
FUNCTION VIRTUIM_SET_APP_MODE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECT
  gsdi_returns_T

===========================================================================*/
LOCAL gsdi_returns_T virtuim_set_app_mode(
  virtsat_protocol_type  apdu_protocol
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);
  MSG_LOW("VIRTSAT:  Enter function ",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_set_app_mode with",0,0,0);
  MSG_LOW("VIRTSAT:  Protocol = %x",apdu_protocol,0,0);

  /* Set Static Global app_mode based */
  /* on protocol for APDU             */
  /* UIM_GSM  --> app_mode = SAT      */
  /* UIM_WCDMA -> app_mode = USAT     */
  switch ( apdu_protocol )
  {
  case VIRTSAT_GSM:
    app_mode =  VIRTUIM_APP_MODE_SAT;
    app_mode_set = TRUE;
    break;
  case VIRTSAT_WCDMA:
    app_mode =  VIRTUIM_APP_MODE_USAT;
    app_mode_set = TRUE;
    break;
  default:
    ERR("VIRTUIM APP:  Unknown Protocol: %x", apdu_protocol,
                          0,
                          0);
    gsdi_status = GSDI_ERROR;
    break;
  }
  MSG_LOW("VIRTSAT:  Exit Function",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_set_app_mode with ",0,0,0);
  MSG_LOW("VIRTSAT:  Status = %x",gsdi_status,0,0);
  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);

  return gsdi_status;
}


/*===========================================================================
FUNCTION VIRTUIM_CHECK_APP_MODE_AND_PROTOCOL

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECT
  gsdi_returns_T

===========================================================================*/
LOCAL gsdi_returns_T virtuim_check_app_mode_and_protocol (
  uim_protocol_type  apdu_protocol
)
{
  gsdi_returns_T gsdi_status = GSDI_SUCCESS;

  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);
  MSG_LOW("VIRTSAT:  Enter Function ",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_check_app_mode_and_protocol",0,0,0);
  MSG_LOW("VIRTSAT:  with Protocol = %x",apdu_protocol,0,0);

  /* Check Mode App is running in */
  switch ( app_mode )
  {
  case VIRTUIM_APP_MODE_SAT:

    /* Check APDU Protocol */
    switch ( apdu_protocol )
    {
    case VIRTSAT_GSM:
      /* App Mode and Protocol OK */
      gsdi_status = GSDI_SUCCESS;
      MSG_LOW("VIRTSAT:  MODE SAT APDU GSM OK",0,0,0);
      break;

    case VIRTSAT_WCDMA:
      /* App Mode and Protocol Not OK */
      gsdi_status = GSDI_ERROR;
      ERR("VIRTSAT:  MODE SAT APDU WCDMA NOT OK",0,0,0);
      break;

    default:
      ERR("VIRTSAT:  Unknown apdu_protocol: %x",apdu_protocol,
                             0,
                             0);
      break;
    }
    break;

  case VIRTUIM_APP_MODE_USAT:

    /* Check APDU Protocol */
    switch ( apdu_protocol )
    {
    case VIRTSAT_GSM:
      /* App Mode and Protocol OK */
      gsdi_status = GSDI_ERROR;
      ERR("VIRTSAT:  MODE USAT APDU GSM NOT OK",0,0,0);
      break;

    case VIRTSAT_WCDMA:
      /* App Mode and Protocol Not OK */
      gsdi_status = GSDI_SUCCESS;
      MSG_LOW("VIRTSAT:  MODE USAT APDU WCDMA OK",0,0,0);
      break;

    default:
      ERR("VIRTSAT:  Unknown apdu_protocol: %x",apdu_protocol,
                             0,
                             0);
      break;
    }
    break;

  default:
    ERR("VIRTSAT:  Unknown App Mode:%x", app_mode,
                        0,
                        0);
    break;
  }

  MSG_LOW("VIRTSAT:  Exit Function",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_check_app_mode_and_protocol",0,0,0);
  MSG_LOW("VIRTSAT:  with Status = %x",gsdi_status,0,0);
  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);

  return gsdi_status;


}

/*===========================================================================
FUNCTION VIRTUIM_DECODE_TERMINAL_PROFILE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECT
  gsdi_returns_T

===========================================================================*/
/*lint -e715 terminal_profile_data not referenced */
LOCAL void virtuim_decode_terminal_profile(
  byte num_bytes,
  byte * terminal_profile_data
)
{
  byte i = 0;

  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);
  MSG_LOW("VIRTSAT:  Enter Function ",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_decode_terminal_profile",0,0,0);
  MSG_LOW("VIRTSAT:  teriminal_profile_data: %x",terminal_profile_data,0,0);
  MSG_LOW("VIRTSAT:  num_bytes = %x",num_bytes,0,0);

  MSG_LOW("VIRTSAT:  DUMP TERMINAL PROFILE RECEIVED",0,0,0);
  while ( i < num_bytes)
  {
    MSG_LOW("VIRTSAT:  BYTE %x = %x",i,terminal_profile_data[i],0);
    i++;
  }

  MSG_LOW("VIRTSAT:  Exit Function",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_decode_terminal_profile",0,0,0);
  MSG_MED("VIRTSAT:  ------------------------------------",0,0,0);
}
/*lint +e715*/
/*===========================================================================
FUNCTION VIRTUIM_PROCESS_TERMINAL_PROFILE

DESCRIPTION
  Will be used to parse and dump the Terminal Profile Download to the
  Diagnostics Monitor.

DEPENDENCIES
  The calling task sending the Terminal Profile download must adhere to
  3GPP TS 31.111

RETURN VALUE
  None

SIDE EFFECT
  gsdi_returns_T

===========================================================================*/
virtsat_result_type virtuim_process_terminal_profile(
  virtsat_transaction_type *virtsat_cmd_ptr
)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;

  MSG_HIGH("VIRTSAT:  ------------------------------------",0,0,0);
  MSG_LOW("VIRTSAT:  Enter function ",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_process_terminal_profile",0,0,0);

  /* Check App Mode              */
  /* If set...this is the second Terminal    */
  /* Profile Download from ME - Spec violation */
  if ( app_mode_set )
  {
    ERR("VIRTSAT:  2nd Terminal Profile",0,0,0);
    ERR("VIRTSAT:  download, you need to",0,0,0);
    ERR("VIRTSAT:  close session and reopen",0,0,0);
    virtsat_result = VIRTSAT_ERROR;
  }
  else
  {
    /* Set the APP Mode         */
    /* First Terminal Profile Download  */
    /* from ME.                         */
    virtsat_result = virtuim_set_app_mode(virtsat_cmd_ptr->hdr.protocol);

    if ( virtsat_result == GSDI_SUCCESS )
    {
      MSG_LOW("VIRTSAT:  Set Mode Success",0,0,0);
      MSG_LOW("VIRTSAT:  Mode Set USAT",0,0,0);
    }
    else
    {
      ERR("VIRTSAT:  Could not set Mode",0,0,0);
    }

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
    if (gsdi_gstk_cmd_unit_verify_terminal_profile(virtsat_cmd_ptr)) 
    {
      virtsat_result = VIRTSAT_SUCCESS;
    }
    else 
    {
      virtsat_result = VIRTSAT_ERROR;
    }
#else
    /* Second Check num_bytes        */
    /* should not be zero or exceed 20 Bytes */
    if ( ( virtsat_cmd_ptr->terminal_profile.num_bytes == 0  ) ||
      ( virtsat_cmd_ptr->terminal_profile.num_bytes >  20 ) )
    {
      virtsat_result = VIRTSAT_ERROR;

      ERR("VIRTSAT:  Number of bytes in terimal",0,0,0);
      ERR("VIRTSAT:  does not pass check       ",0,0,0);
      ERR("VIRTSAT:  num_bytes = %x",
              virtsat_cmd_ptr->terminal_profile.num_bytes,
              0,
              0);

    }
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
    /* Now Decode the Terminal Profile Download */
    /* to the Diagnostics Monitor to verify     */
    /* ME truly support these commands          */
    if (virtsat_result == VIRTSAT_SUCCESS )
    {
      virtuim_decode_terminal_profile(virtsat_cmd_ptr->terminal_profile.num_bytes,
                      virtsat_cmd_ptr->terminal_profile.data);
    }
  }


  MSG_LOW("VIRTSAT:  Exit Function",0,0,0);
  MSG_LOW("VIRTSAT:  virtuim_check_app_mode_and_protocol",0,0,0);
  MSG_LOW("VIRTSAT:  with Status = %x",virtsat_result,0,0);
  MSG_HIGH("VIRTSAT:  ------------------------------------",0,0,0);

  return virtsat_result;

}

/*===========================================================================
FUNCTION VIRTUIM_PROCESS_TERIMAL_RESPONSE

DESCRIPTION
  Will be used to handle and check the contents of the Termainal Response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
virtsat_result_type virtuim_process_terminal_response( void )
{
  MSG_LOW("Processing a Terminal Response",0,0,0);

  return 1;
}

/*===========================================================================
FUNCTION VIRTUIM_PROCESS_ENVELOPE

DESCRIPTION
  Will be used to handle and check the contents of then Envelope Command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

===========================================================================*/
virtsat_result_type virtuim_process_envelope( void )
{
  MSG_LOW("Processing an Envelope Command",0,0,0);

  return 1;
}

/*===========================================================================
FUNCTION virtsat_wait

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL  rex_sigs_type virtsat_wait (rex_sigs_type  virtsat_requested_mask)
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
  if (virtsat_requested_mask & VIRTSAT_CMD_Q_SIG)
  {
    (void) rex_clr_sigs( &virtsat_tcb, VIRTSAT_CMD_Q_SIG );

    if (q_cnt( &virtsat_cmd_q ))
    {
      queue_mask |= VIRTSAT_CMD_Q_SIG;
    }
  }

  /* Get current state of rex signals */
  rex_signals_mask = rex_get_sigs( &virtsat_tcb );

  /* --------------------------------------------------------------------
  ** Loop until the current signals from REX (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  do
  {
    /* Perform the 'real' wait (if needed) */
    if (((rex_signals_mask | queue_mask) & virtsat_requested_mask) == 0)
    {
       rex_signals_mask = rex_wait( virtsat_requested_mask | VIRTSAT_CMD_Q_SIG );
    }
    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue indicated
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & VIRTSAT_CMD_Q_SIG)
    {
      (void) rex_clr_sigs(&virtsat_tcb, VIRTSAT_CMD_Q_SIG);

      if (q_cnt(&virtsat_cmd_q) == 0)   /* No elements in queue ? */
      {
        rex_signals_mask &= ~VIRTSAT_CMD_Q_SIG;  /* Turn off bit */
      }
    }

  /* do while mask requested does not match current mask */
  } while (((rex_signals_mask | queue_mask) & virtsat_requested_mask) == 0);

  return ((rex_signals_mask | queue_mask));

} /* test_sim_server_wait */

/* <EJECT> */

/*===========================================================================
FUNCTION virtsat_get_cmd_buf

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtsat_transaction_type *virtsat_get_cmd_buf(void)
{
   /* Allocate command buffer */
   return (virtsat_transaction_type*)mmgsdi_malloc(sizeof(virtsat_transaction_type));

} /* virtsat_get_cmd_buf */


/*===========================================================================
FUNCTION virtsat_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtsat_cmd(virtsat_transaction_type *virtsat_cmd_ptr)
{

  /* Check for Null pointer */
  if(virtsat_cmd_ptr != NULL)
  {
    (void) q_link(virtsat_cmd_ptr, &virtsat_cmd_ptr->hdr.link);

    /* Put the message on the queue */
    q_put(&virtsat_cmd_q, &virtsat_cmd_ptr->hdr.link);

    /* Set the command queue signal for */
    /* receive task                     */
    (void) rex_set_sigs(&virtsat_tcb, VIRTSAT_CMD_Q_SIG);
    MSG_LOW("VIRTSAT:  VIRTSAT_CMD_Q_SIG set from virtsat_cmd",0,0,0);
  }
  else
  {
    ERR("uim_put_cmd_buf cmd_ptr is NULL",0,0,0);
  }
} /* virtsat_cmd */

/*===========================================================================
FUNCTION virtsat_build_virtuim_response

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtsat_build_virtuim_response(
  virtsat_transaction_type  *virtsat_cmd_ptr,
  virtsat_report_type       *virtsat_report_ptr,
  virtsat_result_type        virtsat_result   
)
{
#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
  int    fetch_byte = 0;
#endif /*FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
  virtsat_report_ptr->num_bytes = 0;
  if ( virtsat_result == VIRTSAT_SUCCESS )
  {
#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
    if(virtsat_cmd_ptr->hdr.transaction == VIRTSAT_ENVELOPE_F) 
    {
      virtsat_report_ptr->status_report.sw1 = 0x90;
      virtsat_report_ptr->status_report.sw2 = 0x00;
      gsdi_gstk_cmd_unit_tester_populate_env_rsp(&virtsat_report_ptr->num_bytes,
        virtsat_report_ptr->data);
    }
    else if (virtsat_cmd_ptr->hdr.transaction == VIRTSAT_RESET_F)
    {
      virtsat_report_ptr->status_report.sw1 = 0x90;
      virtsat_report_ptr->status_report.sw2 = 0x00;
    }
    else
    {
      /* Terminal Profile, Proactive Command */
      if (gsdi_gstk_cmd_unit_tester_fetch_pending(&fetch_byte))
      {
        virtsat_report_ptr->status_report.sw1 = 0x91;
        virtsat_report_ptr->status_report.sw2 = fetch_byte;
      }
      else 
      {
        virtsat_report_ptr->status_report.sw1 = 0x90;
        virtsat_report_ptr->status_report.sw2 = 0x00;
      }
    }
#else /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
    virtsat_report_ptr->status_report.sw1 = 0x90;
    virtsat_report_ptr->status_report.sw2 = 0x00;
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
    virtsat_report_ptr->transaction = VIRTSAT_RESPONSE_R;
    virtsat_report_ptr->status_report.virtsat_result = virtsat_result;
    virtsat_report_ptr->status_report.uim_rpt_function =
      virtsat_cmd_ptr->hdr.uim_rpt_function;
    MSG_LOW("VIRTSAT:  SW1: %x SW2: %x",virtsat_report_ptr->status_report.sw1,
                      virtsat_report_ptr->status_report.sw2,
                      0);

  }
  else
  {
    virtsat_report_ptr->status_report.sw1 = 0x00;
    virtsat_report_ptr->status_report.sw2 = 0x00;
    virtsat_report_ptr->status_report.virtsat_result = virtsat_result;
    virtsat_report_ptr->status_report.uim_rpt_function =
      virtsat_cmd_ptr->hdr.uim_rpt_function;
    MSG_LOW("VIRTSAT:  SW1: %x SW2: %x",virtsat_report_ptr->status_report.sw1,
                      virtsat_report_ptr->status_report.sw2,
                      0);
  }
}


/*===========================================================================
FUNCTION virtsat_send_virtuim_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtsat_send_virtuim_cmd(
  virtuim_cmd_type * virtuim_cmd_ptr)
{

  virtuim_cmd_ptr->hdr.virtuim_result_cb = (virtsat_proactive_rpt_function)virtsat_virtuim_cb;
  (void)q_link(virtuim_cmd_ptr,&virtuim_cmd_ptr->hdr.link);

  (void)rex_clr_sigs(&virtsat_tcb, VIRTSAT_CMD_RSP_SIG);

  /* PUT ON VIRTUIM CMD QUEUE FOR */
  /* PROACTIVE COMMANDS           */
  virtuim_app_cmd(virtuim_cmd_ptr);

  MSG_HIGH("VIRT SAT:  Wait for cmd response",0,0,0);
  (void) virtsat_wait(VIRTSAT_CMD_RSP_SIG|VIRTSAT_CMD_Q_SIG);


}
/*===========================================================================
FUNCTION virtsat_build_internal_setup_menu

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtsat_build_setup_main_menu(
  byte * data_buffer,
  uint32 * data_len)
{
  /* Initialize App Information */
  app_info.app_id = 0;
  app_info.proactive_cmd_index = 0;
  app_info.terminal_res_index = 0;
  app_info.envelope_cmd_index = 0;

  /* Need to MEMCOPY SETUP MENU PROACTIVE COMMAND */
  memcpy(data_buffer,
       setup_menu.setup_menu_data,
       setup_menu.setup_menu_len);

  *data_len = setup_menu.setup_menu_len;

}

/*===========================================================================
FUNCTION virtsat_send_setup_main_menu

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtsat_send_setup_main_menu (
  virtuim_cmd_type *virtuim_req_ptr,
  byte *data_buffer,
  byte *data_len)

{
  /* Get Virtuim Buffer Space fro Proactive Command */
  if ((virtuim_req_ptr = virtuim_get_cmd_buf()) != NULL)
  {
    MSG_LOW("GOT VIRTUIM COMMAND BUFFER",0,0,0);

    /* Build the Command */
    virtuim_req_ptr->data_len = *data_len;
    memcpy(virtuim_req_ptr->data_buffer,
           data_buffer,
           *data_len);

    MSG_MED("VIRT SAT: Data Len: %x",*data_len,0,0);
  }

  virtsat_send_virtuim_cmd( virtuim_req_ptr);

  MMGSDIUTIL_TMC_MEM_FREE(virtuim_req_ptr);
}
/*===========================================================================
FUNCTION virtsat_test_tag

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtsat_result_type virtsat_test_tag(
  byte env_cmd_tag)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  switch (env_cmd_tag)
  {
  case  VIRTSAT_SMS_PP_DL_TAG_99:
    MSG_HIGH("VIRTSAT: ENV TAG SMS-PP: PASS %x",env_cmd_tag,
                           0,
                           0);
    break;

  case  VIRTSAT_CELL_BC_DOWNLOAD_TAG_99:
    MSG_HIGH("VIRTSAT: ENV TAG CELL BC: PASS %x",env_cmd_tag,
                           0,
                           0);
    break;
  case VIRTSAT_MEUN_SEL_TAG_99:
    MSG_HIGH("VIRTSAT: ENV TAG MENU SEL: PASS %x",env_cmd_tag,
                             0,
                             0);
    break;

  case VIRTSAT_CALL_CNTL_TAG_99:
    MSG_HIGH("VIRTSAT: ENV TAG MENU SEL: PASS %x",env_cmd_tag,
                             0,
                             0);
    break;
  default:
    ERR("VIRTSAT: ENV TAG UNKNOWN: FAIL %x",env_cmd_tag,
                          0,
                          0);
    virtsat_result = VIRTSAT_ERROR;
    break;
  }
  return virtsat_result;


}
/*===========================================================================
FUNCTION virtsat_test_tag

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL virtsat_result_type  virtsat_test_len(
  byte env_cmd_total_len,
  byte env_cmd_data_len)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;

  /* Check made is Total Length vs. Length of Data In */
  /* Envelope Data                    */
  if ( env_cmd_data_len == ( env_cmd_total_len - 2 ) )
  {
    MSG_HIGH("VIRTSAT: ENV LEN: PASS %x",
      env_cmd_data_len,
      0,
      0);
  }
  else
  {
    ERR("VIRSAT: ENV LEN: FAIL %x != %x",env_cmd_data_len,
                       env_cmd_total_len - 2,
                       0);
    virtsat_result = VIRTSAT_ERROR;
  }

  return virtsat_result;

}
/*===========================================================================
FUNCTION virtsat_validate_src_des_id

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL virtsat_result_type virtsat_validate_src_des_id(
  byte device,
  boolean src
)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  switch ( device )
  {
  case VIRTSAT_DEVICE_KEYPAD:
    if (src)
    {
      MSG_HIGH("VIRTSAT: DEV ID SRC KEYPAD PASS: %x",device,0,0);
    }
    else
    {
      ERR("VIRTSAT: DEV ID DES KEYPAD FAIL: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    break;
  case VIRTSAT_DEVICE_DISPLAY:
    if (src)
    {
      ERR("VIRTSAT: DEV ID SRC DISPLAY FAIL: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      MSG_HIGH("VIRTSAT: DEV ID DES DISPLAY PASS: %x",device,0,0);
    }
    break;
  case VIRTSAT_DEVICE_EARPIECE:
    if (src)
    {
      ERR("VIRTSAT: DEV ID SRC EARPIECE FAIL: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      MSG_HIGH("VIRTSAT: DEV ID DES EARPIECE PASS: %x",device,0,0);
    }
    break;
  case VIRTSAT_DEVICE_CARD_READER:
  case VIRTSAT_DEVICE_CARD_READER1:
  case VIRTSAT_DEVICE_CARD_READER2:
  case VIRTSAT_DEVICE_CARD_READER3:
  case VIRTSAT_DEVICE_CARD_READER4:
  case VIRTSAT_DEVICE_CARD_READER5:
  case VIRTSAT_DEVICE_CARD_READER6:
  case VIRTSAT_DEVICE_CARD_READER7:
    ERR("VIRTSAT:  DEV ID CARD READER NOT SUPPORTED: %x", device,0,0);
    break;
  case VIRTSAT_DEVICE_CHANNEL:
  case VIRTSAT_DEVICE_CHANNEL1:
  case VIRTSAT_DEVICE_CHANNEL2:
  case VIRTSAT_DEVICE_CHANNEL3:
  case VIRTSAT_DEVICE_CHANNEL4:
  case VIRTSAT_DEVICE_CHANNEL5:
  case VIRTSAT_DEVICE_CHANNEL6:
  case VIRTSAT_DEVICE_CHANNEL7:
    ERR("VIRTSAT: DEV ID CHANNEL NOT SUPPORTED: %x",device,0,0);
    break;
  case VIRTSAT_DEVICE_SIM:
    if (src)
    {
      MSG_HIGH("VIRTSAT: DEV ID SRC SIM PASS: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      MSG_HIGH("VIRTSAT: DEV ID DES SIM PASS: %x",device,0,0);
    }
    break;
  case VIRTSAT_DEVICE_ME:
    if (src)
    {
      MSG_HIGH("VIRTSAT: DEV ID SRC MOBILE PASS: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      MSG_HIGH("VIRTSAT: DEV ID DES MOBILE PASS: %x",device,0,0);
    }
    break;
  case VIRTSAT_DEVICE_NETWORK:
    if (src)
    {
      MSG_HIGH("VIRTSAT: DEV ID SRC NETWORK PASS: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      MSG_HIGH("VIRTSAT: DEV ID DES NETWORK PASS: %x",device,0,0);
    }
    break;
  default:
    if (src)
    {
      ERR("VIRTSAT: DEV ID SRC UNKNONW FAIL: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    else
    {
      ERR("VIRTSAT: DEV ID DES UNKNOWN FAIL: %x",device,0,0);
      virtsat_result = VIRTSAT_ERROR;
    }
    break;
  }
  return virtsat_result;



}
/*===========================================================================
FUNCTION virtsat_validate_dev_ids

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtsat_result_type virtsat_validate_dev_ids(
  byte env_cmd_tag,
  byte env_cmd_dev_id_tag,
  byte env_cmd_dev_id_len,
  byte env_cmd_dev_id_src,
  byte env_cmd_dev_id_des
)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  /* Check DEVICE IDENTITY TAG */
  if ( env_cmd_dev_id_tag == VIRTSAT_DEVICE_IDENTITY_TAG)
  {
    MSG_HIGH("VIRTSAT: DEV ID TAG PASS : %x",env_cmd_dev_id_tag,0,0);
  }
  else
  {
    ERR("VIRTSAT: DEV ID TAG FAIL: %x",env_cmd_dev_id_tag,0,0);
    virtsat_result = VIRTSAT_ERROR;
  }

  /* Check DEVICE IDENTITY LEN */
  /* MUST EQUAL TO 2           */
  if ( env_cmd_dev_id_len == VIRTSAT_DEVICE_IDENTITY_LEN)
  {
    MSG_HIGH("VIRTSAT: DEV ID LEN PASS : %x",env_cmd_dev_id_len,0,0);
  }
  else
  {
    ERR("VIRTSAT: DEV ID LENFAIL: %x",env_cmd_dev_id_len,0,0);
    virtsat_result = VIRTSAT_ERROR;
  }

  if ( virtsat_result == VIRTSAT_SUCCESS )
  {
    /* Check the Source Device ID */
    virtsat_result = virtsat_validate_src_des_id(env_cmd_dev_id_src,1);
  }
  if ( virtsat_result == VIRTSAT_SUCCESS )
  {
    /* Check the Destination Device ID */
    virtsat_result = virtsat_validate_src_des_id(env_cmd_dev_id_des,0);
  }

  return virtsat_result;

}
/*===========================================================================
FUNCTION virtsat_valide_env_menu_sel

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL virtsat_result_type virtsat_valide_env_menu_sel(
 virtsat_transaction_type * virtsat_cmd_ptr,
 byte           env_cmd_tag
)
{
  virtsat_result_type virtsat_result;
  byte        env_cmd_dev_id_tag;
  byte        env_cmd_dev_id_len;
  byte        env_cmd_dev_id_src;
  byte        env_cmd_dev_id_des;

  env_cmd_dev_id_tag  = virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_DEV_ID_OFFSET];
  env_cmd_dev_id_len  = virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_DEV_ID_LEN_OFFSET];
  env_cmd_dev_id_src  = virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_DEV_ID_SRC_OFFSET];
  env_cmd_dev_id_des  = virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_DEV_ID_DES_OFFSET];

  /* Checking Device Identities */
  virtsat_result = virtsat_validate_dev_ids(env_cmd_tag,
                        env_cmd_dev_id_tag,
                        env_cmd_dev_id_len,
                        env_cmd_dev_id_src,
                        env_cmd_dev_id_des);
  if ( virtsat_result == VIRTSAT_SUCCESS)
  {
    MSG_HIGH("DONE VALIDATING ENVELOPE CMD ",0,0,0);
    MSG_HIGH("MENU SELECTION PASS",0,0,0);

  }
  return virtsat_result;


}


/*===========================================================================
FUNCTION virtsat_validate_envelope

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL virtsat_result_type virtsat_validate_envelope(
   virtsat_transaction_type * virtsat_cmd_ptr
)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
  if ( virtsat_cmd_ptr != NULL )
  {
    if (!gsdi_gstk_cmd_unit_verify_envelope_cmd(virtsat_cmd_ptr))
    {
      virtsat_result = VIRTSAT_ERROR;
    }
  }
  else
  {
    virtsat_result = VIRTSAT_ERROR;
  }
#else /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */

  byte        env_cmd_tag;
  byte        env_cmd_total_len;
  byte        env_cmd_data_len;


  MSG_HIGH("VIRSAT: TEST ENVELOPE",0,0,0);
  env_cmd_tag     =   virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_TAG_OFFSET];
  env_cmd_data_len  =   virtsat_cmd_ptr->
              envelope_cmd.data[VIRTSAT_ENV_TOT_LEN_OFFSET];
  env_cmd_total_len = virtsat_cmd_ptr->
              envelope_cmd.num_bytes;

  /* Check the Tag */
  virtsat_result = virtsat_test_tag(env_cmd_tag);

  if (virtsat_result == VIRTSAT_SUCCESS)
  {
    /* Check the Length */
    virtsat_result = virtsat_test_len(env_cmd_total_len,
                      env_cmd_data_len);
  }

  /* Tag and length are ok */
  /* Validate the rest of the ENV CMD */
  switch (env_cmd_tag)
  {
  case VIRTSAT_MEUN_SEL_TAG_99:
    virtsat_result = virtsat_valide_env_menu_sel(virtsat_cmd_ptr,
                           env_cmd_tag);
    break;
  default:
    break;
  }
#endif /*FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
  return virtsat_result;
}

LOCAL virtsat_result_type virtsat_set_app_info(
  virtsat_transaction_type* virtsat_cmd_ptr)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  MSG_LOW("VIRTSAT:  CURR APP ID:      %x",app_info.app_id,0,0);
  MSG_LOW("VIRTSAT:  CURR P CMD INDEX: %x",app_info.proactive_cmd_index,0,0);

  app_info.app_id = virtsat_cmd_ptr->envelope_cmd.data[VIRTSAT_ENV_ITEM_ID_SEL_OFFSET];
  app_info.proactive_cmd_index = 0;

  return virtsat_result;

}
/*===========================================================================
FUNCTION virtsat_build_and_queue_proactive_cmd

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL void virtsat_build_and_queue_proactive_cmd(
    virtsat_transaction_type    *virtsat_int_cmd_ptr)
{
  /* Get Command Buffer */
  virtsat_int_cmd_ptr = virtsat_get_cmd_buf();

  /* Based on Current App Info Build Proactive Command */
  virtsat_int_cmd_ptr->hdr.transaction = VIRTSAT_PROACTIVE_CMD_F;

  /* Now Get the Number of bytes of the Proactive Command */
  virtsat_int_cmd_ptr->proactive_cmd.num_bytes =
    sat_app_1.cmd_rsp[app_info.proactive_cmd_index].proactive_cmd_len;

  /* Now Copy the Data */
  memcpy( virtsat_int_cmd_ptr->proactive_cmd.data,
      sat_app_1.cmd_rsp[app_info.proactive_cmd_index].proactive_cmd_buffer,
      sat_app_1.cmd_rsp[app_info.proactive_cmd_index].proactive_cmd_len);

  MSG_LOW("VIRT SAT: LINKING TO SAT QUEUE",0,0,0);
    (void)q_link(virtsat_int_cmd_ptr, &virtsat_int_cmd_ptr->hdr.link);

    virtsat_int_cmd_ptr->hdr.virtsat_rpt_function = NULL;

  /* Place on Command Queue */
  virtsat_cmd(virtsat_int_cmd_ptr);


}
/*===========================================================================
FUNCTION virtsat_process_proactive_command

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
virtsat_result_type virtsat_process_proactive_command(
  virtsat_transaction_type      *virtsat_int_cmd_ptr,
  virtuim_cmd_type        *virtuim_req_ptr
)
{

  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  byte i = 0;

   if ((virtuim_req_ptr = virtuim_get_cmd_buf()) != NULL)
   {
    MSG_LOW("GOT VIRTUIM COMMAN BUFFER",0,0,0);

    /* Set Virtuim Data Len*/
    virtuim_req_ptr->data_len = virtsat_int_cmd_ptr->proactive_cmd.num_bytes;

    /* Memcopy Proactive Command Bytes */
    memcpy(virtuim_req_ptr->data_buffer,
         virtsat_int_cmd_ptr->proactive_cmd.data,
         virtsat_int_cmd_ptr->proactive_cmd.num_bytes);

    while ( i < virtsat_int_cmd_ptr->proactive_cmd.num_bytes )
    {
      MSG_MED("VIRT SAT: Data %d = %x",i,
             virtuim_req_ptr->data_buffer[i],
           0);
      i++;
    }

    virtsat_send_virtuim_cmd( virtuim_req_ptr);

   }

  return virtsat_result;

}

/*===========================================================================
FUNCTION virtsat_process_proactive_command

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
LOCAL virtsat_result_type virtsat_process_terminal_response(
  virtsat_transaction_type      *virtsat_cmd_ptr
)
{
  virtsat_result_type virtsat_result = VIRTSAT_SUCCESS;
  if ( virtsat_cmd_ptr == NULL )
  {
    return VIRTSAT_ERROR;
  }
#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
  if (!gsdi_gstk_cmd_unit_verify_terminal_response(virtsat_cmd_ptr))
  {
      virtsat_result = VIRTSAT_ERROR;
  }
#endif /*FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
  return virtsat_result;
}
/*===========================================================================
FUNCTION virtsat_main

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
void virtsat_main
(
  dword dummy
    /* Required for REX, ignore */
    /*lint -esym(715,dummy)
    ** Have lint not complain about the ignored parameter 'dummy' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  static virtuim_cmd_type   *virtuim_req_ptr = NULL;
  rex_sigs_type             sigs;
  rex_sigs_type       wait_for_sigs;
  virtsat_transaction_type  *virtsat_cmd_ptr;
  virtsat_report_type       virtsat_report_cmd;
  virtsat_transaction_type  *virtsat_int_cmd_ptr = 0x00;
  virtsat_result_type       virtsat_result  =  VIRTSAT_SUCCESS;
  byte                      data_buffer[VIRTSAT_MAX_DATA_BLOCK_LEN];
  byte                      data_len;
  byte                      cmd_envelope_tag;

  memset( data_buffer, 0xFF, VIRTSAT_MAX_DATA_BLOCK_LEN );

  /* Initialize Virtual SAT Application Queue */
  (void) q_init(&virtsat_cmd_q);

#ifdef FEATURE_DOG
    /* Define timer      */
    rex_def_timer( &virtsat_rpt_timer, &virtsat_tcb, VIRTSAT_RPT_TIMER_SIG );
#endif

  /* Wait for start signal */
  tmc_task_start();

  for(;;)
  {
#ifdef FEATURE_DOG 
    /* Report to Dog */
    dog_report( DOG_VIRTSAT_RPT);
    rex_set_timer(&virtsat_rpt_timer, DOG_VIRTSAT_RPT_TIME);
#endif /* FEATURE_DOG */
//    MSG_HIGH("VIRTSAT MAIN",0,0,0);
    wait_for_sigs = VIRTSAT_CMD_Q_SIG | 
                    TASK_OFFLINE_SIG  | 
                    TASK_STOP_SIG | 
                    VIRTSAT_RPT_TIMER_SIG;

    sigs = virtsat_wait( wait_for_sigs );

    if(sigs & TASK_OFFLINE_SIG)
    {
      (void) rex_clr_sigs(rex_self(),TASK_OFFLINE_SIG);
      task_offline();
    }
    else if (sigs & TASK_STOP_SIG)
    {
      (void) rex_clr_sigs(rex_self(),TASK_STOP_SIG);
      task_stop();
    }
#ifdef FEATURE_DOG
    else if ( sigs & VIRTSAT_RPT_TIMER_SIG ) 
    {
      (void) rex_clr_sigs(rex_self(),VIRTSAT_RPT_TIMER_SIG);
      
      /* Report to Dog */
      dog_report( DOG_VIRTSAT_RPT);
    }
#endif /* FEATURE_DOG */
    else if((sigs & VIRTSAT_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs(rex_self(),VIRTSAT_CMD_Q_SIG);

      if ((virtsat_cmd_ptr = (virtsat_transaction_type*)q_get(&virtsat_cmd_q)) != NULL)
      {
        MSG_LOW("GOT COMMAND",0,0,0);

        /*
        ** Now Determine what type of Command to Process
        ** 1) Envelope
        ** 2) Terminal Profile Download
        ** 3) Terminal Response
        */
        switch(virtsat_cmd_ptr->hdr.transaction)
        {
        case VIRTSAT_TERMINAL_PROFILE_F:
          MSG_LOW("VIRTSAT:  CASE TERM PROF DL",0,0,0);
          virtsat_result = virtuim_process_terminal_profile(virtsat_cmd_ptr);

          /* Build and Send RSP Type */
          virtsat_build_virtuim_response(
                    virtsat_cmd_ptr,
                    &virtsat_report_cmd,
                    virtsat_result);

          /* Signal Virtual SIM Task */
          (*(virtsat_cmd_ptr->hdr.virtsat_rpt_function))(&virtsat_report_cmd);

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
          gsdi_gstk_cmd_unit_tester_next_action(0);
#else
          /* Build Internal Proactive Command :  Setup Menu */
          MSG_LOW("VIRTSAT:  Building Internal Set Up Menu",0,0,0);
          virtsat_build_setup_main_menu(data_buffer,
                                       (uint32*)&data_len);

          MSG_LOW("VIRTSAT:  Build and Send Set Up Menu",0,0,0);
          virtsat_send_setup_main_menu(virtuim_req_ptr,
                                       data_buffer,
                                       &data_len);
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
          break;
        case VIRTSAT_ENVELOPE_F:
          MSG_LOW("VIRTSAT:  CASE ENVELOPE CMD",0,0,0);

          virtsat_result = virtsat_validate_envelope(virtsat_cmd_ptr);

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER

          /* Build and Send Rsp Type */
          virtsat_build_virtuim_response(virtsat_cmd_ptr,
                    &virtsat_report_cmd,
                    virtsat_result);

          /* Signal Virtual SIM Task */
          (*(virtsat_cmd_ptr->hdr.virtsat_rpt_function))(&virtsat_report_cmd);

          gsdi_gstk_cmd_unit_tester_next_action(0);

#else /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */
          if (virtsat_result == VIRTSAT_SUCCESS)
          {
            cmd_envelope_tag = virtsat_cmd_ptr->envelope_cmd.data[VIRTSAT_ENV_TAG_OFFSET];

            /* Set Global APP Data */
            if ( cmd_envelope_tag == VIRTSAT_MEUN_SEL_TAG_99)
            {
              virtsat_result = virtsat_set_app_info(virtsat_cmd_ptr);
            }
          }

          /* Build and Send Rsp Type */
          virtsat_build_virtuim_response(virtsat_cmd_ptr,
                    &virtsat_report_cmd,
                    virtsat_result);

          /* Signal Virtual SIM Task */
          (*(virtsat_cmd_ptr->hdr.virtsat_rpt_function))(&virtsat_report_cmd);

          /* Now Build Proactive Command to Send */
          virtsat_build_and_queue_proactive_cmd(virtsat_int_cmd_ptr);
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */

          break;
        case VIRTSAT_PROACTIVE_CMD_F:
          MSG_HIGH("VIRTSAT:  PROACTIVE COMMAND",0,0,0);

          virtsat_result = virtsat_process_proactive_command(virtsat_cmd_ptr,
                                     virtuim_req_ptr);

          break;

        case VIRTSAT_TERMINAL_RESPONSE_F:
          MSG_LOW("VIRTSAT:  TERM RESP ",0,0,0);

          virtsat_result = virtsat_process_terminal_response(virtsat_cmd_ptr);

          /* Build and Send Rsp Type */
          virtsat_build_virtuim_response(virtsat_cmd_ptr,
                    &virtsat_report_cmd,
                    virtsat_result);

          /* Signal Virtual SIM Task */
          (*(virtsat_cmd_ptr->hdr.virtsat_rpt_function))(&virtsat_report_cmd);

#ifdef FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER
          gsdi_gstk_cmd_unit_tester_next_action(0);
#endif /* FEATURE_GSDI_GSTK_ENABLE_UNIT_TESTER */

          break;
        case VIRTSAT_RESET_F:
          MSG_HIGH("VIRTSAT: RESET COMMAND",0,0,0);

          /* Simply Clear Out Globals */
          app_mode = VIRTUIM_APP_MODE_NONE;
          app_mode_set = FALSE;

          /* Build and Send Rsp Type */
          virtsat_build_virtuim_response(virtsat_cmd_ptr,
                    &virtsat_report_cmd,
                    virtsat_result);

          /* Signal Virtual SIM Task */
          (*(virtsat_cmd_ptr->hdr.virtsat_rpt_function))(&virtsat_report_cmd);
          break;

        default:
          ERR("VIRTSAT:  UNKNOWN CMD",0,0,0);
          break;
        }
      }
    }
  }
}

#endif /* FEATURE_VIRTUAL_SAT */

/*lint -e750 Supress Lint Error:
  Error (Info) 750: local macro 'VIRTSAT_C' (line 2, file virtsat.c) not
  referenced
*/
