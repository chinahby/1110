/*===========================================================================

FILE: DRMTestApp.c

SERVICES: DRM test app

GENERAL DESCRIPTION:
   N/A

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
   N/A

(c) COPYRIGHT 2002-2006 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
===========================================================================*/


/*===========================================================================

$Header: //depot/asic/msmshared/apps/FldDbg/DRMTestApp.c#6 $
$DateTime: 2008/08/31 21:54:10 $ $Author: ravisoni $


                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------
08/11/06     sw/jas  Lint fixes
04/15/05     jay     Added unit tests for check right
08/05/04     aag     Added additional tests, fixed consume tests
06/03/04     aag     Added all existing DRM tests and converted to BREW API
06/02/04     jay     Created file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"         //Handset Feature Definitions
#include "comdef.h"

#include "AEE.h"
#include "AEE_OEM.h"

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "OEMClassIDs.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEFile.h"

#include "msg.h"

#ifdef FEATURE_SEC_DRM
#include "AEEDRM.h"
#include "AEEDCF.h"

/*
 * This should be enabled if you want to turn the asynchronous test suite on
 */
//#define ASYNC_TESTS

/*
 * This should be enabled if you want to turn the internal test suite on
 */
//#define INTERNAL_TESTS

#ifdef FEATURE_SEC_TESTAPPS

extern OEMDCF_EncodeUintvar( uint8*  pUintvarBuffer,
                             uint32* pUintvarBufferLength,
                             uint32  ValueToEncode );

extern OEMDCF_DecodeUintvar( uint8*  pUintvarBuffer,
                             uint32* pUintvarLen,
                             uint32* pDecodedValue);

extern OEMDCF_ParseEncryptionHeader
( uint8*                      pContentBuf,
  uint32                      BufLen,
  uint32*                     pBytesParsed,
  AEEDRM_EncryptionInfoType*  pEncryptionInfo
);


/* ----------------------------------------------------------------------- */
/*                                                                         */
/*               OMA 1.0 Rights objects for basic testing                  */
/*                                                                         */
/*  Note: This file is included by DRMTestApptest.c                            */
/* ----------------------------------------------------------------------- */

char DRMTestApp_oma10_uri_1_play_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_1@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint><o-dd:count>2</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*              Raw rights objects for testing IPC retrieve                */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/* Raw right that includes the o-ex:rights header */
char DRMTestApp_raw_uri_2_no_right_1_sep_del[] =
"<o-ex:rights xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\"><head><title> Strict DTD XHTML Example </title></head><body><p>Please Choose a Day:<select name=\"day\"><option selected=\"selected\">Monday</option><option>Tuesday</option><option>Wednesday</option></select></p></body></o-ex:rights>";

char DRMTestApp_raw_uri_3_execute_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmlddsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_raw_1@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>2</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*              OMA 1.0 Rights objects for Extended Testing                */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/* Includes spaces between elements and inside elements */
char DRMTestApp_oma10_uri_4_play_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"> <o-ex:context> <o-dd:version>1.0</o-dd:version> </o-ex:context> <o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission> <o-dd:play> <o-ex:constraint><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:play> </o-ex:permission></o-ex:agreement></o-ex:rights>";

/* Includes tabs between elements and inside elements */
char DRMTestApp_oma10_uri_4_play_right_2_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\">\t<o-ex:context>\t<o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo>\t<ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission>\t<o-dd:play><o-ex:constraint><o-dd:count>4</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* Includes newlines between elements and inside elements */
char DRMTestApp_oma10_uri_4_play_right_3_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo>\n<ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset>\n<o-ex:permission><o-dd:play><o-ex:constraint><o-dd:count>5</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement>\n</o-ex:rights>";

char DRMTestApp_oma10_uri_4_display_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:display><o-ex:constraint><o-dd:count>6</o-dd:count></o-ex:constraint></o-dd:display></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_4_display_right_2_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:display><o-ex:constraint><o-dd:count>7</o-dd:count></o-ex:constraint></o-dd:display></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_4_display_right_3_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:display><o-ex:constraint><o-dd:count>8</o-dd:count></o-ex:constraint></o-dd:display></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_4_print_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>9</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_4_print_right_2_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_4@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>10</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_5_print_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context> <o-dd:uid>cid:uri_5@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>11</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_5_execute_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_5@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>12</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_5_execute_right_2_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_5@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>13</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_5_execute_right_3_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_5@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>14</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";


/* ----------------------------------------------------------------------- */
/*                                                                         */
/*              Additional Rights Objects for Cache Testing                */
/*                                                                         */
/* ----------------------------------------------------------------------- */

char DRMTestApp_oma10_uri_6_execute_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_6@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>15</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_6_display_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_6@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:display><o-ex:constraint><o-dd:count>16</o-dd:count></o-ex:constraint></o-dd:display></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_7_print_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_7@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>17</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_7_play_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_7@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint><o-dd:count>18</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_8_execute_right_1_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_8@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>19</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_8_execute_right_2_sep_del[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_8@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>20</o-dd:count></o-ex:constraint></o-dd:execute></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                              License Key                                */
/*                                                                         */
/* ----------------------------------------------------------------------- */

char DRMTestApp_oma10_uri_1_play_right_1_sep_del_license[] =
"vUEwR8LzEJoeiC+dgT1mgg==";
char DRMTestApp_oma10_all_license[] = "vUEwR8LzEJoeiC+dgT1mgg==";

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                         Content Protection Rights                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */

char DRMTestApp_oma10_uri_9_print_right_1_fwd_lock[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_9@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>21</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_9_print_right_2_fwd_lock[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_9@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwXXXkJJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>21</o-dd:count></o-ex:constraint></o-dd:print></o-ex:permission></o-ex:agreement></o-ex:rights>";


/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                             Consume Rights                              */
/*                                                                         */
/* ----------------------------------------------------------------------- */

char DRMTestApp_oma10_uri_10_consume_right_1_1_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_10@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint><o-dd:count>2</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_11_consume_right_1_2_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_11@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:execute><o-ex:constraint><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:execute><o-dd:play><o-ex:constraint><o-dd:count>2</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_11_consume_right_2_2_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_11@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:print><o-dd:play><o-ex:constraint><o-dd:count>2</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_11_consume_right_3_2_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_11@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>1</o-dd:count></o-ex:constraint></o-dd:print><o-dd:play><o-ex:constraint><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

char DRMTestApp_oma10_uri_11_consume_right_4_2_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_11@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:print><o-ex:constraint><o-dd:count>1</o-dd:count></o-ex:constraint></o-dd:print><o-dd:play><o-ex:constraint><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* This right has no constraints for the play permission */
char DRMTestApp_oma10_uri_12_consume_right_1_1_perm_no_constraints[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_12@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* This right also has no constraints for the play permission but uses another method to indicate this */
char DRMTestApp_oma10_uri_12_consume_right_2_1_perm_no_constraints[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_12@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play /></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* This right has no permissions at all */
char DRMTestApp_oma10_uri_13_consume_right_1_0_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_13@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* This right has start/end time constraints for Play permission */
char DRMTestApp_oma10_uri_14_time_right_1_1_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_14@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint><o-dd:datetime><o-dd:start>2005-04-10T12:30:00</o-dd:start><o-dd:end>2005-04-12T18:00:00</o-dd:end></o-dd:datetime></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";

/* This right has interval time constraints
 *
 * It can only be exercised unlimited times within 10 seconds
 */
char DRMTestApp_oma10_uri_15_interval_right_1_1_perm[] =
"<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\"><o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context><o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:uri_15@internal.qualcomm.com</o-dd:uid></o-ex:context><ds:KeyInfo><ds:KeyValue>vUEwR8LzEJoeiC+dgT1mgg==</ds:KeyValue></ds:KeyInfo></o-ex:asset><o-ex:permission><o-dd:play><o-ex:constraint><o-dd:interval>10S</o-dd:interval><o-dd:count>3</o-dd:count></o-ex:constraint></o-dd:play></o-ex:permission></o-ex:agreement></o-ex:rights>";



/* Lookup/Retrieve tests structure                                         */
typedef struct
{
  uint8 num_lookup_tests;                /* Number of lookup tests         */
  uint8 num_retrieve_tests;              /* Number of retrieve tests       */
} DRMTestApp_total_lr_tests_type;

/*
 * Asynchronous tests structure
 */
typedef struct
{
  IDRM*                                     pIDRM;
  uint8*                                    pOMARight;
  uint32                                    OMARightLength;
  int                                       Errno;
  uint32                                    TotalAsyncTests;
  uint32                                    AsyncTestsPassed;
  AEECallback                               CB;
  boolean                                   DoneAllTests;
  uint32                                    Counter;
  char                                      URI [ AEEDRM_MAX_URI_LEN ];
  AEEDRM_RightsHandleType                   RightsHandle;
  AEEDRM_AddRightRspType                    AddRsp;
  AEEDRM_LookupRightRspType                 LookupRsp;
  AEEDRM_DeleteRightRspType                 DeleteRsp;
  AEEDRM_DeleteAssociatedRightsRspType      DeleteAssocRsp;
} DRMTestApp_AsyncRsp;

/*
 * Instance of that asynchronous tests structure
 */
DRMTestApp_AsyncRsp AsyncRsp;

#define NUM_INVALID_ENCR_HDRS 15
#define NUM_VALID_ENCR_HDRS 14
#define ENCR_HDR_LENGTH 256

/*
 * Test cases for the encryption header parser
 */
char InvalidEncrHdrArray [ NUM_INVALID_ENCR_HDRS ] [ ENCR_HDR_LENGTH ] =
{
  "Encryption-Method:AES128CBC;padding=RFC2130",
  "Encryption-Method:AES128CBC;paddingRFC2630",
  "Encryption-Method:AES128CBC;pidding=RFC2630",
  "Encryption-Method:AES128CAC;padding=RFC2630",
  "Encryption-Method=AES128CBC;padding=RFC2630",
  "Encryption-MethodAES128CBC;padding=RFC2630",
  "Encryption-method:AES128CBC;padding=RFC2630",
  "Encryption-Method:AES128CBC;padding=",
  "Encryption-Method:AES128CBC;",
  "Encryption-Method:AES128CAC",
  "Encryption-Method:",
  "Encryption-Method",
  "",
  "GARBAGEGARBAGEEncryption-Method    :SOMESTUFF",
  "Encryption-Method:       ",
};

char ValidEncrHdrArray [ NUM_VALID_ENCR_HDRS ] [ ENCR_HDR_LENGTH ] =
{
  "Encryption-Method:AES128CBC;padding=RFC2630",
  "Encryption-Method: AES128CBC;padding=RFC2630",
  "Encryption-Method : AES128CBC;padding=RFC2630",
  "Encryption-Method:AES128CBC; padding=RFC2630",
  "Encryption-Method:AES128CBC ; padding=RFC2630",
  "Encryption-Method: AES128CBC; padding=RFC2630",
  "Encryption-Method :   AES128CBC   ; padding=RFC2630",
  "Encryption-Method:AES128CBC;padding= RFC2630",
  "Encryption-Method:AES128CBC;padding = RFC2630",
  "Encryption-Method: AES128CBC; padding= RFC2630",
  "Encryption-Method    : AES128CBC;   padding       =         RFC2630",
  "Encryption-Method:AES128CBC",
  "SOMESTUFFSOMESTUFFEncryption-Method:AES128CBC;padding=RFC2630",
  "               Encryption-Method:AES128CBC;padding=RFC2630"
};



/* Constants the define how many tests are available                       */

const uint8 DRMTestApp_total_add_tests     = 7;   /* # of add tests        */
const uint8 DRMTestApp_total_consume_tests = 10;  /* # of consume tests    */
const uint8 DRMTestApp_total_check_tests   = 4;  /* # of check tests    */
const uint8 DRMTestApp_total_delete_tests  = 4;   /* # of delete tests     */
const uint8 DRMTestApp_total_protect_tests = 6;   /* # of protect tests    */
const uint8 DRMTestApp_total_update_tests  = 2;   /* # of update tests     */
const uint8 DRMTestApp_total_flow_tests    = 4;   /* # of flow tests       */
const uint8 DRMTestApp_total_create_tests  = 4;   /* # of create tests     */
const DRMTestApp_total_lr_tests_type DRMTestApp_total_lr_tests = { 4, 7 };



/*===========================================================================
FUNCTION DRMTestApp_Xmlproc_base64_decode

DESCRIPTION
  This function is called with a string containing a base64 encoded value
  and a buffer to return the decoded value in.  It convertes the base64
  encoding to binary and fills the passed buffer with the decoded value.
  The size of the string is passed in the encodelen argument while the size
  of the buffer is passed in the decodelen argument.  The size of the
  decoded string is filled and returned in the decodelen argument.
  
DEPENDENCIES
  None

RETURN VALUE
  int
  The decoded value is returned in the passed buffer which should be large
  enough for the purpose.

SIDE EFFECTS
  None
===========================================================================*/
int DRMTestApp_xmlproc_base64_decode
(
  uint8                *string,        /* String containing encoded value  */
  uint32                encodelen,     /* Length of string                 */
  uint8                *buffer,        /* Buffer for the decoded value     */
  uint32               *decodelen      /* Length of buffer                 */
)
{
  int                 ret = SUCCESS;
  uint32              decodebuf;
  uint8              *pdecoded;
  uint8              *pcoded;
  uint32              bufferlen;
  uint32              i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != string );
  ASSERT( NULL != buffer );
  ASSERT( NULL != decodelen );
  ASSERT(    0 != *decodelen );

  /*------------------------------------------------------------------------
    The encoded string length must be a multiple of 4 bytes.
  ------------------------------------------------------------------------*/
  if( 0 != (encodelen % sizeof(int)) )
  {
    return EBADPARM;
  }

  bufferlen  = *decodelen;
  *decodelen = 0;

  /*------------------------------------------------------------------------
    Four bytes of base64 encoded value converts to 3 bytes of ascii value
  ------------------------------------------------------------------------*/
  while( 0 < encodelen && SUCCESS == ret )
  {
    pcoded   = string;
    pdecoded = (uint8 *)&decodebuf;

    /*----------------------------------------------------------------------
      Convert base64 value to 6-bit ascii equivalent.
    ----------------------------------------------------------------------*/
    for( i=0; i < sizeof(int); i++)
    {
      if( 'A' <= *pcoded && 'Z' >= *pcoded )
      {
        *pdecoded = *pcoded - 'A';
      }
      else
      {
        if( 'a' <= *pcoded && 'z' >= *pcoded )
        {
          *pdecoded = *pcoded - 'a'; *pdecoded += 26;
        }
        else
        {
          if( '0' <= *pcoded && '9' >= *pcoded )
          {
            *pdecoded = *pcoded - '0'; *pdecoded += 52;
          }
          else
          {
            if( '+' == *pcoded )
            {
              *pdecoded = 62;
            }
            else
            {
              if( '/' == *pcoded )
              {
                *pdecoded = 63;
              }
              else
              {
                /*-----------------------------------------------------------
                  Special value of 64 for '='
                -----------------------------------------------------------*/
                if( '=' == *pcoded )
                {
                  *pdecoded = 64;
                }
                else
                {
                  MSG_MED("invalid base64 value %d", *pcoded, 0, 0 );
                  ret = EFAILED;
                  break;
                }
              }
            }
          }
        }
      }

      pcoded++;
      pdecoded++;
    } /* for */

    /*----------------------------------------------------------------------
      Convert the 4 bytes containing 6-bit of ASCII values to 3 bytes of
      8-bit each.  Check to see that there is enough room for the decoded
      data and that '=' (assigned 64) marks the end of the encoded stream.
    ----------------------------------------------------------------------*/
    pdecoded = (uint8 *)&decodebuf;

    if( (64 != pdecoded[3] && 64 != pdecoded[2] && 3 > bufferlen) ||
        (64 != pdecoded[2] && 2 > bufferlen)                    ||
         1 > bufferlen )
    {
      ret = ENOMEMORY;
    }
    else
    {
      if( 64 == pdecoded[0]                        ||
          64 == pdecoded[1]                        ||
          (64 == pdecoded[2] && 64 != pdecoded[3]) ||
          (64 == pdecoded[3] && 4 < encodelen) )
      {
        ret = EFAILED;
      }
      else
      {
        *buffer++ = (((pdecoded[0]<<2) & 0xfC) | ((pdecoded[1]>>4) & 0x03));
        (*decodelen)++; bufferlen--;

        if( 64 != pdecoded[2] )
        {
          *buffer++ = (((pdecoded[1]<<4) & 0xf0)|((pdecoded[2]>>2) & 0x0f));
          (*decodelen)++; bufferlen--;

          if( 64 != pdecoded[3] )
          {
            *buffer++ = (((pdecoded[2]<<6) & 0xC0) | (pdecoded[3] & 0x3f));
            (*decodelen)++; bufferlen--;
          }
        }

        string    += 4;
        encodelen -= 4;
      }
    }

  }

  return ret;
} /* DRMTestApp_xmlproc_base64_decode() */


uint8 gDRMTest[ AEEDRM_XML_BUFFER_SIZE ];

/*===========================================================================

FUNCTION DRMTestApp_PlayRight

DESCRIPTION
  This function is used to test the DRM agent APIs

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_PlayRight
              ( IShell*                       pIShell )
{
  IDRM*                                       piDRM = NULL;
  char                                        uri[AEEDRM_MAX_URI_LEN];
  AEEDRM_AddRightRspType                      add_rsp;
  AEEDRM_LookupRightRspType                   lookup_rsp;
  AEEDRM_ConsumeRightRspType                  consume_rsp;
  AEEDRM_DeleteRightRspType                   delete_rsp;
  AEEDRM_RightsHandleType                     rights_handle;
  int                                         _errno = EFAILED;
  AEEDRM_RightsObjectType*                    pRightsList = NULL;
  uint32                                      num_rights = 0;
  AEEDRM_XactionIdType                        xaction_id = 0;
  uint8*                                      rights_ptr = NULL;
  AEEDRM_PermissionTemplateType               rights_template;
  uint32                                      rights_size = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_FATAL ( "***** DRMTestApp_PlayRight: BEGIN TEST. *****", 0, 0, 0 );

  /* Empty the URI string */
  uri[0] = '\0';

  do
  {
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    /* Create Rights from a template */
    rights_ptr = &gDRMTest[0];
    MEMSET( &rights_template,
            0,
            sizeof(AEEDRM_PermissionTemplateType) ); 
    rights_template.Play.IsValid = TRUE;
    rights_template.Play.TotalCount = 2;
    rights_size =  AEEDRM_XML_BUFFER_SIZE;
    _errno = IDRM_CreateRight( piDRM,
                               "uri_1@internal.qualcomm.com",
                               rights_template,
                               rights_ptr,
                               &rights_size );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR("Create Rights Failed (%d)", _errno, 0, 0 );
      break;
    }
    else
    {
      MSG_ERROR(" Create Success!!", 0,0,0);
    }

    /* Add the XML rights object to the agent */
    IDRM_AddRight( piDRM,
                   uri,
                   AEEDRM_UNKNOWN_VERSION,
                   rights_ptr,
                   rights_size,
                   &add_rsp,
                   NULL );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Add failed (%d)", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ("Add success!!", 0, 0, 0 );
    }

    rights_handle = add_rsp.RightsHandle;
    MEMCPY( uri,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );

    uri[add_rsp.ContentURILen] = '\0';

    /* Lookup the just installed right */
    IDRM_LookupRight( piDRM,
                      uri,
                      0x00,
                      &lookup_rsp,
                      NULL );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Lookup failed (%d)", lookup_rsp.Errno, 0, 0 );
      break;
    }

    num_rights = lookup_rsp.NumRights;
    xaction_id = lookup_rsp.XactionID;
    MSG_HIGH ( "Number of rights: %d xaction_id: %d", num_rights,
        xaction_id, 0 );

    pRightsList= (AEEDRM_RightsObjectType*)MALLOC
      (20 * sizeof(AEEDRM_RightsObjectType));

    if ( pRightsList == NULL )
    {
      MSG_ERROR ( "MALLOC failed.", 0, 0, 0 );
      break;
    }

    /* Retrieve the rights data */
    _errno = IDRM_RetrieveRight( piDRM,
                                 xaction_id,
                                 pRightsList,
                                 &num_rights );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Retrieve failed (%d) using xaction_id: %d", _errno,
         xaction_id, 0 );
      break;
    }

    // Valid only for this RO !
    MSG_HIGH ( "Playcount = %d / %d",
              pRightsList[0].Constraints.Play.RemainingCount,
              pRightsList[0].Constraints.Play.TotalCount, 0 );

    /* Consume from the play count */
    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rights_handle,
                       AEEDRM_ACTION_PLAY,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Consume failed (%d)!!!", consume_rsp.Errno, 0, 0 );
      break;
    }

    /* Consume again from the play count (by URI) */
    IDRM_ConsumeRight( piDRM,
                       uri,
                       0x00,
                       AEEDRM_ACTION_PLAY,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Consume failed (%d)", consume_rsp.Errno, 0, 0 );
      break;
    }

    if ( consume_rsp.RightsHandle != rights_handle )
    {
      MSG_ERROR ( "Bad handle 0x%x / 0x%x",
                 consume_rsp.RightsHandle, rights_handle, 0 );
      break;
    }

    /* Consume again from the play count (should fail) */
    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rights_handle,
                       AEEDRM_ACTION_PLAY,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != AEEDRM_ERIGHTSEXPIRED )
    {
      MSG_ERROR ( "Consume failed (%d)", consume_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "No right available", 0, 0, 0 );
    }

    IDRM_DeleteRight( piDRM,
                      rights_handle,
                      &delete_rsp,
                      NULL );

    ASSERT( delete_rsp.RspType == AEEDRM_RSP_DELETE_RIGHT  );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "delete failed (%d)", delete_rsp.Errno, 0, 0 );
      break;
    }

    MSG_FATAL ( "***** DRMTestApp_PlayRight: TEST PASSED. *****", 0, 0, 0 );

  } while( 0 );

  FREE( pRightsList);
  IDRM_Release( piDRM );

  if ( _errno != SUCCESS )
  {
    MSG_FATAL ( "***** DRMTestApp_PlayRight: TEST FAILED. *****", 0, 0, 0 );
  }

} /* DRMTestApp_play_right */

/* Buffers to hold URIs from rights objects                                */
char DRMTestApp_uri_1[256];
char DRMTestApp_uri_2[256];
char DRMTestApp_uri_3[256];



/*===========================================================================

FUNCTION DRMTestApp_CLEAR_RO_LIST

DESCRIPTION
  This function is used to clear the rights object lists

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_clear_ro_list
  (
    AEEDRM_RightsObjectType* list_to_clear,
    int                      list_size
  )
{
  /* Sanity check */
  if ( list_size <= 0 || list_to_clear == NULL )
    return;

  MEMSET (
    list_to_clear,
    0,
    sizeof ( AEEDRM_RightsObjectType ) * list_size
    );
} /* DRMTestApp_clear_ro_list */



/*===========================================================================

FUNCTION DRMTestApp_AddUnitTests

DESCRIPTION
  This function is used to test the behavior of the add API.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of add tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_AddUnitTests ( IShell* pIShell )
{
  uint8                                add_tests_passed;
  uint8                                bin_buf[16];
  uint32                               bin_buf_len;
  uint32                               rhandle_1;
  uint32                               actions_mask;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_ConsumeRightRspType           consume_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear out variables */

  add_tests_passed = 0;
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  _errno = EFAILED;
  piDRM = NULL;
  bin_buf_len = 0;
  rhandle_1 = 0;
  actions_mask = 0;
  _errno = 0;

  MSG_FATAL ( "***** AddUnitTests: BEGIN TEST. *****", 0, 0, 0 );

  do
  {

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("AddUnitTests: Can't create a DRM Object", 0,0,0 );
      break;
    }

    /* Tests the following parameters:
     *
     * Empty URI field
     * Rights version specified
     * First rights object for given URI
     */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_1,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    MEMCPY (
        DRMTestApp_uri_1,
        add_rsp.szContentURI,
        add_rsp.ContentURILen
        );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #1 success.", 0, 0, 0 );
    /* End test */

    /* Tests the following parameters:
     *
     * Empty URI field
     * Rights version left unspecified
     * Second rights object for given URI
     * Same rights type as first rights object
     */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_2_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_2_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #2 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    if ( DRMTestApp_uri_2[0] != '\0' )
    {
      MSG_ERROR ( "AddUnitTests: Add #2 modified the blank URI",
                  0,
                  0,
                  0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #2 success.", 0, 0, 0 );
    /* End test */

    /* Tests the following parameters:
     *
     * URI field given (matches URI in the rights object)
     * Rights version specified
     * Third rights object for given URI
     * New rights type
     */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_1,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_display_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_display_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #3 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    if ( DRMTestApp_uri_1 != NULL  && add_rsp.szContentURI != NULL )
    {
      if ( STRCMP ( DRMTestApp_uri_1, (char*) add_rsp.szContentURI ) != 0 )
      {
        MSG_ERROR ( "AddUnitTests: URI field mismatched.",
                    0,
                    0,
                    0 );
        break;
      }
    }
    else
    {
      MSG_ERROR ( "AddUnitTests: URI field(s) NULL.", 0, 0, 0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #3 success.", 0, 0, 0 );
    /* End test */

    /* Tests the following parameters:
     *
     * URI field given (matches URI in the rights object)
     * Rights version unspecified
     * Fourth rights object for given URI
     * New rights type
     */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_print_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_print_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #4 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    if ( DRMTestApp_uri_1 != NULL && add_rsp.szContentURI != NULL )
    {
      if ( STRCMP ( DRMTestApp_uri_1, (char*) add_rsp.szContentURI ) != 0 )
      {
        MSG_ERROR ( "AddUnitTests: URI field mismatched.",
                    0,
                    0,
                    0 );
        break;
      }
    }
    else
    {
      MSG_ERROR ( "AddUnitTests: URI fields NULL.", 0, 0, 0 );
      break;
    }

    rhandle_1 = add_rsp.RightsHandle;

    actions_mask = AEEDRM_ACTION_PRINT;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_1,
        actions_mask,
        &consume_rsp,
        NULL
        );

    /* Check if the licenses are correct */

    bin_buf_len = sizeof( bin_buf );

    /* Decode the license value and compare */
    _errno = DRMTestApp_xmlproc_base64_decode(
        (uint8*) DRMTestApp_oma10_all_license,
        sizeof( DRMTestApp_oma10_all_license ) -1,
        (uint8*) bin_buf,
        &bin_buf_len);
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ("Base 64 decode failed %d, len %d",
          _errno, 
          bin_buf_len,
          0 );
      break;
    }

    if ( MEMCMP( bin_buf,
          consume_rsp.LicenseKey.KeyInfo.Value,
          bin_buf_len )
        ||
        (consume_rsp.LicenseKey.KeyInfo.Size
         !=
         bin_buf_len) )
    {
      MSG_ERROR ( "License Modified", 0, 0, 0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #4 success.", 0, 0, 0 );
    /* End test */

    /* Tests the following parameters:
     *
     * Empty URI field
     * Rights version incorrectly specified
     * First rights object for given URI
     */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_2,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_raw_uri_3_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_3_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno == SUCCESS )
    {
      MSG_ERROR (
        "AddUnitTests: Add #5 improperly added right.  Error #: %d",
        add_rsp.Errno,
        0,
        0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #5 success.", 0, 0, 0 );
    /* End test */

    STRLCPY ( DRMTestApp_uri_2, "uri_raw_1@internal.qualcomm.com", sizeof(DRMTestApp_uri_2) );

    /* Tests the following parameters:
     *
     * URI field specified
     * Rights version specified
     * First raw right object for the given URI
     */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_2,
        AEEDRM_UNKNOWN_VERSION,
        (uint8*) DRMTestApp_raw_uri_3_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_3_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #6 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    if ( DRMTestApp_uri_2 != NULL && add_rsp.szContentURI != NULL )
    {
      if ( STRCMP ( DRMTestApp_uri_2, (char*) add_rsp.szContentURI ) != 0 )
      {
        MSG_ERROR ( "AddUnitTests: Add #6 modified the given URI", 0, 0, 0 );
        break;
      }
    }
    else
    {
      MSG_ERROR ( "AddUnitTests: URI fields NULL.", 0, 0, 0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #6 success.", 0, 0, 0 );
    /* End test */

    /* Tests the following parameters:
     *
     * URI field specified
     * Rights version specified
     * Second raw right object for the given URI
     */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_2,
        AEEDRM_UNKNOWN_VERSION,
        (uint8*) DRMTestApp_raw_uri_3_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_3_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "AddUnitTests: Add #7 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }

    if ( DRMTestApp_uri_2 != NULL && add_rsp.szContentURI != NULL )
    {
      if ( STRCMP ( DRMTestApp_uri_2, (char*) add_rsp.szContentURI ) != 0 )
      {
        MSG_ERROR ( "AddUnitTests: Add #7 modified the given URI", 0, 0, 0 );
        break;
      }
    }
    else
    {
      MSG_ERROR ( "AddUnitTests: URI fields NULL.", 0, 0, 0 );
      break;
    }

    add_tests_passed++;
    MSG_HIGH ( "AddUnitTests: Add test #7 success.", 0, 0, 0 );
    /* End test */

    MSG_FATAL ( "***** AddUnitTests: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** AddUnitTests: TEST FAILED *****", 0, 0, 0 );
  }

  /* Clear out the db */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "AddUnitTests: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "AddUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }
  } /* endif */

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "AddUnitTests: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "AddUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }
  } /* endif */

  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';

  IDRM_Release( piDRM );

  return add_tests_passed;
} /* DRMTestApp_AddUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RunAddTests

DESCRIPTION
  This function runs all the add unit tests and displays their results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunAddTests ( IShell* pIShell )
{
  uint8 add_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  add_tests_passed += DRMTestApp_AddUnitTests ( pIShell );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "*   ADD TESTS: %d/%d TESTS PASSED   *",
              add_tests_passed,
              DRMTestApp_total_add_tests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
}/* DRMTestApp_RunAddTests */



/*===========================================================================

FUNCTION DRMTestApp_CreateUnitTests

DESCRIPTION
  This function is used to test the behavior of the create API

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of add tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_CreateUnitTests ( IShell* pIShell )
{
  IDRM*                          piDRM;
  char                           uri[256];
  AEEDRM_AddRightRspType         add_rsp;
  AEEDRM_LookupRightRspType      lookup_rsp;
  AEEDRM_DeleteRightRspType      delete_rh_rsp;
  AEEDRM_RightsHandleType        rights_handle;
  int                            _errno;
  uint8                          create_tests_passed;
  AEEDRM_RightsObjectType*       pRightsList;
  uint32                         num_rights;
  AEEDRM_XactionIdType           xaction_id;
  uint8*                         rights_ptr;
  AEEDRM_PermissionTemplateType  rights_template;
  uint32                         rights_size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear out variables */

  create_tests_passed = 0;
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  _errno = EFAILED;
  piDRM = NULL;
  pRightsList = NULL;
  num_rights = NULL;
  xaction_id = 0;
  rights_ptr = NULL;
  rights_size = 0;
  
  MSG_FATAL ( "***** CreateUnitTests: BEGIN TEST. *****", 0, 0, 0 );

  do
  {

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR( "CreateUnitTests: Can't create a DRM Object", 0,0,0 );
      break;
    }

    pRightsList= (AEEDRM_RightsObjectType*) MALLOC ( 20 * 
                            sizeof( AEEDRM_RightsObjectType ) );

    if ( pRightsList == NULL )
    {
      MSG_ERROR ( "CreateUnitTests: MALLOC failed.", 0, 0, 0 );
      break;
    }

    /* No Permission No Count */
    rights_ptr = &gDRMTest[0];
    MEMSET( &rights_template,
            0,
            sizeof( AEEDRM_PermissionTemplateType ) ); 
    rights_size =  AEEDRM_XML_BUFFER_SIZE;
    _errno = IDRM_CreateRight( piDRM,
                               "uri_1@internal.qualcomm.com",
                               rights_template,
                               rights_ptr,
                               &rights_size );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR( "CreateUnitTests: Create Right #1 Failed (%d)", _errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH( "CreateUnitTests: Create #1 Success!!", 0,0,0);
    }

    /* Add the XML rights object to the agent */
    IDRM_AddRight( piDRM,
                   uri,
                   AEEDRM_UNKNOWN_VERSION,
                   rights_ptr,
                   rights_size,
                   &add_rsp,
                   NULL );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Add failed (%d)", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Add success!!", 0, 0, 0 );
    }

    rights_handle = add_rsp.RightsHandle;
    MEMCPY( uri,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );
    uri[add_rsp.ContentURILen] = '\0';

    /* Lookup the just installed right */
    IDRM_LookupRight( piDRM,
                      uri,
                      0x00,
                      &lookup_rsp,
                      NULL );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Lookup failed (%d)",
         lookup_rsp.Errno, 0, 0 );
      break;
    }

    num_rights = lookup_rsp.NumRights;
    xaction_id = lookup_rsp.XactionID;
    MSG_HIGH ( "CreateUnitTests: Number of rights: %d xaction_id: %d",
       num_rights, xaction_id, 0 );


    /* Retrieve the rights data */
    _errno = IDRM_RetrieveRight( piDRM,
                                 xaction_id,
                                 pRightsList,
                                 &num_rights );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Retrieve failed (%d) using xaction_id: %d",
                  _errno,
                  xaction_id,
                  0 );
      break;
    }
    
    if ( pRightsList[0].PermissionsMask != ( AEEDRM_PERM_PLAY |
                                             AEEDRM_PERM_PRINT |
                                             AEEDRM_PERM_EXECUTE |
                                             AEEDRM_PERM_DISPLAY ) )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect permissions specified: 0x%x",
                  pRightsList[0].PermissionsMask,
                  0,
                  0
                );
      break;
    }
    
    if ( pRightsList[0].Constraints.Play.ConstraintsMask 
         != 
         AEEDRM_CONSTR_PERMANENT
       )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect play constraints specified: 0x%x",
                  pRightsList[0].Constraints.Play.ConstraintsMask,
                  0,
                  0
                );
      break;
    }

    if ( pRightsList[0].Constraints.Display.ConstraintsMask
         !=
         AEEDRM_CONSTR_PERMANENT
       )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect display constraints specified:\
                  0x%x",
                  pRightsList[0].Constraints.Display.ConstraintsMask,
                  0,
                  0
                );
      break;
    }
    
    if ( pRightsList[0].Constraints.Execute.ConstraintsMask
         != 
         AEEDRM_CONSTR_PERMANENT )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect execute constraints specified: 0x%x",
                  pRightsList[0].Constraints.Execute.ConstraintsMask,
                  0,
                  0
                );
                break;
    }
    
    if ( pRightsList[0].Constraints.Print.ConstraintsMask
         !=
         AEEDRM_CONSTR_PERMANENT
       )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect print constraints specified: 0x%x",
                  pRightsList[0].Constraints.Print.ConstraintsMask,
                  0,
                  0
                );
      break;
    }
    
    /* Delete the right */
    IDRM_DeleteRight (
        piDRM,
                                rights_handle,
        &delete_rh_rsp,
        NULL
        );

    if ( delete_rh_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Error #: %d.  Delete failed.",
         delete_rh_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }

    MSG_HIGH ( "CreateUnitTests: Test #1 successful", 0, 0, 0 );
    create_tests_passed++;
                
    /* 1 Permission No Count */
    rights_ptr = &gDRMTest[0];
    MEMSET( &rights_template,
            0,
            sizeof(AEEDRM_PermissionTemplateType) ); 
    rights_template.Play.IsValid = TRUE;
    rights_size =  AEEDRM_XML_BUFFER_SIZE;
    _errno = IDRM_CreateRight( piDRM,
                               "uri_1@internal.qualcomm.com",
                               rights_template,
                               rights_ptr,
                               &rights_size );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR( "Create Rights Failed (%d)", _errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( " Create Success!!", 0, 0, 0 );
    }

    /* Add the XML rights object to the agent */
    IDRM_AddRight( piDRM,
                   uri,
                   AEEDRM_UNKNOWN_VERSION,
                   rights_ptr,
                   rights_size,
                   &add_rsp,
                   NULL );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Add failed (%d)", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Add success!!", 0, 0, 0 );
    }

    rights_handle = add_rsp.RightsHandle;
    MEMCPY( uri,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );
    uri[add_rsp.ContentURILen] = '\0';

    /* Lookup the just installed right */
    IDRM_LookupRight( piDRM,
                      uri,
                      0x00,
                      &lookup_rsp,
                      NULL );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Lookup failed (%d)",
         lookup_rsp.Errno, 0, 0 );
      break;
    }

    num_rights = lookup_rsp.NumRights;
    xaction_id = lookup_rsp.XactionID;
    MSG_HIGH ( "CreateUnitTests: Number of rights: %d xaction_id: %d",
                                                         num_rights,
                                                         xaction_id,
                                                         0 );

    /* Retrieve the rights data */
    _errno = IDRM_RetrieveRight( piDRM,
                                 xaction_id,
                                 pRightsList,
                                 &num_rights );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Retrieve failed (%d) using xaction_id: %d",
                   _errno,
                   xaction_id,
                   0 );
      break;
    }

    if ( pRightsList[0].PermissionsMask != AEEDRM_PERM_PLAY )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect permissions specified: 0x%x",
                  pRightsList[0].PermissionsMask,
                  0,
                  0
                 );
      break;
    }

    if ( pRightsList[0].Constraints.Play.ConstraintsMask !=
         AEEDRM_CONSTR_PERMANENT )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect play constraints specified: 0x%x",
                  pRightsList[0].Constraints.Play.ConstraintsMask,
                  0,
                  0
                );
      break;
    }

    /* Delete the right */
    IDRM_DeleteRight (
        piDRM,
                                rights_handle,
        &delete_rh_rsp,
        NULL
        );

    if ( delete_rh_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Error #: %d.  Delete failed.",
                  delete_rh_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }

    MSG_HIGH ( "CreateUnitTests: Test #2 successful", 0, 0, 0 );
    create_tests_passed++;

    /* 1 Permission 1 Count */
    rights_ptr = &gDRMTest[0];
    MEMSET( &rights_template,
            0,
            sizeof( AEEDRM_PermissionTemplateType ) ); 
    rights_template.Play.IsValid = TRUE;
    rights_template.Play.TotalCount = 2;
    rights_size =  AEEDRM_XML_BUFFER_SIZE;
    _errno = IDRM_CreateRight( piDRM,
                               "uri_1@internal.qualcomm.com",
                               rights_template,
                               rights_ptr,
                               &rights_size );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR( "CreateUnitTests: Create Rights #3 Failed (%d)",
         _errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH( "CreateUnitTests: Create #3 Success!!", 0,0,0);
    }
    /* Add the XML rights object to the agent */
    IDRM_AddRight( piDRM,
                   uri,
                   AEEDRM_UNKNOWN_VERSION,
                   rights_ptr,
                   rights_size,
                   &add_rsp,
                   NULL );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Add failed (%d)", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ("CreateUnitTests: Add success!!", 0, 0, 0 );
    }

    rights_handle = add_rsp.RightsHandle;
    MEMCPY( uri,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );
    uri[add_rsp.ContentURILen] = '\0';

    /* Lookup the just installed right */
    IDRM_LookupRight( piDRM,
                      uri,
                      0x00,
                      &lookup_rsp,
                      NULL );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Lookup failed (%d)",
         lookup_rsp.Errno, 0, 0 );
      break;
    }

    num_rights = lookup_rsp.NumRights;
    xaction_id = lookup_rsp.XactionID;
    MSG_HIGH ( "Number of rights: %d xaction_id: %d",
       num_rights, xaction_id, 0 );

    /* Retrieve the rights data */
    _errno = IDRM_RetrieveRight( piDRM,
                                 xaction_id,
                                 pRightsList,
                                 &num_rights );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Retrieve failed (%d) using xaction_id: %d",
         _errno, xaction_id, 0 );
      break;
    }

    if ( pRightsList[0].PermissionsMask != AEEDRM_PERM_PLAY )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect permissions specified: 0x%x",
                  pRightsList[0].PermissionsMask,
                  0,
                  0
                );
      break;
    }
   
    if ( pRightsList[0].Constraints.Play.TotalCount != 2 )
    {
      MSG_ERROR ( "CreateUnitTests: Incorrect number of play counters: %d",
                  pRightsList[0].Constraints.Play.TotalCount,
                  0,
                  0
                );
      break;
    }
    
    /* Delete the right */
    IDRM_DeleteRight (
        piDRM,
                                rights_handle,
        &delete_rh_rsp,
        NULL
        );

    if ( delete_rh_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Error #: %d.  Delete failed.",
         delete_rh_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }

    create_tests_passed++;
    MSG_HIGH ( "CreateUnitTests: Create test #3 passed.", 0, 0, 0 );

    /* 2 Permission 1 Count */
    rights_ptr = &gDRMTest[0];
    MEMSET( &rights_template,
            0,
            sizeof( AEEDRM_PermissionTemplateType ) ); 
    rights_template.Play.IsValid = TRUE;
    rights_template.Play.TotalCount = 2;
                rights_template.Display.IsValid = TRUE;
                rights_template.Display.TotalCount = 3;

    rights_size =  AEEDRM_XML_BUFFER_SIZE;
    _errno = IDRM_CreateRight( piDRM,
                               "uri_1@internal.qualcomm.com",
                               rights_template,
                               rights_ptr,
                               &rights_size );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR( "CreateUnitTests: Create Rights #4 Failed (%d)",
         _errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH( "CreateUnitTests: Create #4 Success!!", 0,0,0);
    }

    /* Add the XML rights object to the agent */
    IDRM_AddRight( piDRM,
                   uri,
                   AEEDRM_UNKNOWN_VERSION,
                   rights_ptr,
                   rights_size,
                   &add_rsp,
                   NULL );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Add failed (%d)", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ("CreateUnitTests: Add success!!", 0, 0, 0 );
    }

    rights_handle = add_rsp.RightsHandle;
    MEMCPY( uri,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );
    uri[add_rsp.ContentURILen] = '\0';

    /* Lookup the just installed right */
    IDRM_LookupRight( piDRM,
                      uri,
                      0x00,
                      &lookup_rsp,
                      NULL );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Lookup failed (%d)",
         lookup_rsp.Errno, 0, 0 );
      break;
    }

    num_rights = lookup_rsp.NumRights;
    xaction_id = lookup_rsp.XactionID;
    MSG_HIGH ( "Number of rights: %d xaction_id: %d",
       num_rights, xaction_id, 0 );

    /* Retrieve the rights data */
    _errno = IDRM_RetrieveRight( piDRM,
                                 xaction_id,
                                 pRightsList,
                                 &num_rights );
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Retrieve failed (%d) using xaction_id: %d",
         _errno, xaction_id, 0 );
      break;
    }

   if ( pRightsList[0].PermissionsMask != 
       ( AEEDRM_PERM_PLAY | AEEDRM_PERM_DISPLAY ) )
   {
    MSG_ERROR ( "CreateUnitTests: Incorrect permissions specified: 0x%x",
                pRightsList[0].PermissionsMask,
                0,
                0
              );
    break;
   }
   
   if ( pRightsList[0].Constraints.Play.TotalCount != 2 ||
        pRightsList[0].Constraints.Display.TotalCount != 3 )
   {
     MSG_ERROR ( "CreateUnitTests: Incorrect number of play counters: %d",
                 pRightsList[0].Constraints.Play.TotalCount,
                 0,
                 0
                );
     break;
   }
  
   /* Delete the right */
   IDRM_DeleteRight (
       piDRM,
       rights_handle,
       &delete_rh_rsp,
       NULL
       );

    if ( delete_rh_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "CreateUnitTests: Error #: %d.  Delete failed.",
         delete_rh_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "CreateUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }

                create_tests_passed++;
                MSG_HIGH ( "CreateUnitTests: Create test #4 passed.", 0, 0, 0 );

    MSG_FATAL ( "***** CreateUnitTests: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** CreateUnitTests: TEST FAILED *****", 0, 0, 0 );
  }

  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';

  IDRM_Release( piDRM );
        FREE ( pRightsList );

  return create_tests_passed;
} /* DRMTestApp_CreateUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RunCreateTests

DESCRIPTION
  This function runs all the create unit tests and displays their results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunCreateTests ( IShell* pIShell )
{
  uint8 create_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  create_tests_passed += DRMTestApp_CreateUnitTests ( pIShell );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* CREATE TESTS: %d/%d TESTS PASSED  *",
              create_tests_passed,
              DRMTestApp_total_create_tests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
}/* DRMTestApp_RunCreateTests */



/*===========================================================================

FUNCTION DRMTestApp_RetrieveUnitTests

DESCRIPTION
  This function tests the behavior of the DRM retrieve_right API.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  The number of lookup/retrieve tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
DRMTestApp_total_lr_tests_type DRMTestApp_RetrieveUnitTests ( IShell* pIShell )
{
  uint8                                   _err_flag;
  uint32                                  ro_list_size;
  uint32                                  num_rights_1;
  uint32                                  num_rights_2;
  uint32                                  num_rights_copied;
  int                                     _errno;
  AEEDRM_AddRightRspType                  add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType    delete_rsp;
  AEEDRM_LookupRightRspType               lookup_rsp;
  AEEDRM_RightsObjectType*                ro_list;
  AEEDRM_XactionIdType                    xaction_id_1;
  AEEDRM_XactionIdType                    xaction_id_2;
  DRMTestApp_total_lr_tests_type          num_lr_tests_passed;
  IDRM*                                   piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear out variables */
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  num_lr_tests_passed.num_lookup_tests = 0;
  num_lr_tests_passed.num_retrieve_tests = 0;
  _err_flag = 0;
  num_rights_1 = 0;
  num_rights_2 = 0;
  num_rights_copied = 0;
  ro_list_size = 0;
  _errno = EFAILED;
  ro_list = NULL;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  piDRM = NULL;

  do 
  {
    ro_list = (AEEDRM_RightsObjectType*)MALLOC
      (20 * sizeof(AEEDRM_RightsObjectType));

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "RetrieveUnitTests: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MSG_FATAL ( "***** RetrieveUnitTests: BEGIN TEST. *****", 0, 0, 0 );

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    /* Do Add #1 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RetrieveUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY (
        DRMTestApp_uri_1,
        add_rsp.szContentURI,
        add_rsp.ContentURILen
        );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';

    /* Do Add #2 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_2_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_2_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Add #2 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RetrieveUnitTests: Add #2 success.", 0, 0, 0 );
    }

    /* Do Add #3 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_display_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_display_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Add #3 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RetrieveUnitTests: Add #3 success.", 0, 0, 0 );
    }

    /* Do Add #4 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_print_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_print_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Add #4 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RetrieveUnitTests: Add #4 success.",
                 0,
                 0,
                 0 );
    }

    /* Tests the following parameters:
     *
     * URI specified
     * Rights handle unspecified
     */
    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
      "RetrieveUnitTests: Lookup #1 success.  # rights: %d xaction_id: %d",
      num_rights_1,
      xaction_id_1,
      0 );
    }

    if ( num_rights_1 != 4 )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }

    num_lr_tests_passed.num_lookup_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Lookup test #1 success.",
               0,
               0,
               0 );
    /* End test */

    /* Parameters:
     * Retrieve will operate _immediately_ after the lookup takes place
     * List size matches the number of rights for this URI
     */

    MSG_HIGH (
      "RetrieveUnitTests: # rights: %d list size: %d xaction_id %d",
      num_rights_1,
      num_rights_1,
      xaction_id_1 );

    ro_list_size = num_rights_1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "RetrieveUnitTests: Retrieve #1 could not use xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Retrieve #1 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
      "RetrieveUnitTests: Retrieve #1 successful and retrieved %d rights",
      ro_list_size,
      0,
      0 );
    }

    if ( ro_list_size != 4 )
    {
      MSG_ERROR ( "RetrieveUnitTests: List size incorrect.   # rights: %d",
                  ro_list_size,
                  0,
                  0 );
      break;
    }

    if ( ro_list[0].LicenseKey.KeyInfo.Value != NULL )
    {
      if ( STRCMP ( (char*) ro_list[0].LicenseKey.KeyInfo.Value, "" ) != 0 )
      {
        MSG_ERROR ( "RetrieveUnitTests: License field is _not_ empty.", 0, 0, 0 );
        break;
      }
    }
    else
    {
      MSG_ERROR ( "RetrieveUnitTests: License field is NULL!", 0, 0, 0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_1 );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Retrieve test #1 success.",
               0,
               0,
               0 );
    /* End test */


    /* Parameters
     *
     * Perform two lookups in a row for the same URI
     * URI specified
     * Rights handle left unspecified
     */

    /* Perform 1st of 2 lookups */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "RetrieveUnitTests: Lookup #2 success.   # rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( num_rights_1 != 4 )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #2 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }

    /* Perform 2nd of 2 lookups */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #3 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH ( "RetrieveUnitTests: Lookup #3 success.  num_rights: %d xaction_id: %d", num_rights_2, xaction_id_2, 0 );
    }

    if ( num_rights_2 != 4 )
    {
      MSG_ERROR ( "RetrieveUnitTests: Lookup #3 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_2, xaction_id_2, 0 );
      break;
    }

    num_lr_tests_passed.num_lookup_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Lookup test #2 success.",
               0,
               0,
               0 );
    /* End test */

    /* Parameters
     *
     * Perform two retrieves in a row using different xaction_ids
     * 1st retrieve: list size > # of rights for the URI
     * 2nd retrieve: list size < # of rights for the URI
     */

    /* Perform 1st retrieve */

    ro_list_size = num_rights_1 * 2;

    MSG_HIGH ( "RetrieveUnitTests: # rights %d list size %d xaction_id %d",
               num_rights_1,
               num_rights_1 * 2,
               xaction_id_1 );

    _errno = IDRM_RetrieveRight (
              piDRM,
              xaction_id_1,
              ro_list,
              &ro_list_size
              );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "RetrieveUnitTests: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Retrieve #2 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }

    if ( ro_list_size != num_rights_1 )
    {
      MSG_ERROR ( "RetrieveUnitTests: List size not modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_1 );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Retrieve test #2 success.",
               0,
               0,
               0 );
    /* End test */

    /* Perform 2nd retrieve */

    num_rights_copied = 0;

    MSG_HIGH ( "RetrieveUnitTests: # rights %d list size %d xaction_id %d",
               num_rights_2,
               1,
               xaction_id_2 );

    do
    {
      ro_list_size = 1;

      _errno = IDRM_RetrieveRight (
                  piDRM,
                  xaction_id_2,
                  ro_list,
                  &ro_list_size
                  );

      if ( _errno == AEEDRM_ENOTFOUND )
      {
        MSG_ERROR ( "RetrieveUnitTests: Retrieve #3 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
        _err_flag = 1;
      }

      /* TODO -- Check if the returned objects are the correct ones */

      if ( _err_flag != 1 )
      {
        num_rights_copied ++;
      }

      if ( ro_list_size != 1 )
      {
        _err_flag = 1;
        MSG_ERROR ( "RetrieveUnitTests: Incorrect # of rights received.  rights_copied: %d", ro_list_size, 0, 0 );
      }
      else
      {
        MSG_HIGH ( "RetrieveUnitTests: Correct # of rights received.", 0, 0, 0 );
        MSG_HIGH ( "RetrieveUnitTests: # of rights rcvd so far: %d", num_rights_copied, 0, 0 );
      }

      /* TODO -- Check if only the first list element modified -- */
    } while ( _errno != SUCCESS && _err_flag != 1 );

    if ( _err_flag == 1 )
    {
      MSG_ERROR (
        "RetrieveUnitTests: An error occurred while retrieving rights.",
        0,
        0,
        0 );
      break;
    }

    if ( num_rights_copied != num_rights_2 )
    {
      MSG_ERROR ( "RetrieveUnitTests: Incorrect number of rights received.  num_rights: %d", num_rights_copied, 0, 0 );
      break;
    }

    DRMTestApp_clear_ro_list ( ro_list, num_rights_2 );

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Retrieve test #3 success.",
               0,
               0,
               0 );
    /* End test */

    /* Parameters
     * Attempt a retrieve _after_ the xaction_id is used up
     */

    MSG_HIGH (
      "RetrieveUnitTests: Attempting retrieve using xaction_id: %d",
      xaction_id_2,
      0,
      0 );

    ro_list_size = num_rights_2 * 2;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    if ( (_errno != SUCCESS)
         &&
         ( ro_list_size != 0 )
       )
    {
      MSG_ERROR ( "RetrieveUnitTests: Retrieve #4 error for xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    DRMTestApp_clear_ro_list ( ro_list, num_rights_2 );

    /* Clear variables */
    num_rights_2 = xaction_id_2 = 0;

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RetrieveUnitTests: Retrieve test #4 success.",
               0,
               0,
               0 );
    /* End test */

    MSG_FATAL ( "***** RetrieveUnitTests: TEST PASSED. *****",
                0,
                0,
                0 );
    _errno = SUCCESS;
  } while ( 0 );

  /* Print out a failure message */

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** RetrieveUnitTests: TEST FAILED. *****",
                0,
                0,
                0 );
  }

  /* Parameters
   * Attempt to delete all the rights for an associated URI
   * URI is specified
   */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RetrieveUnitTests: Error #: %d.  Delete failed.",
                  delete_rsp.Errno,
                  0,
                  0 );
    }
    else
    {
      MSG_HIGH ( "RetrieveUnitTests: Delete succeded.",
      0,
      0,
      0 );
    }
  }

  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';

        FREE ( ro_list );
  IDRM_Release( piDRM );

  return num_lr_tests_passed;
} /* DRMTestApp_RetrieveUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RawRetrieveUnitTests

DESCRIPTION
  This function is used to test the DRM retrieve_raw_right API

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of lookup/retrieve tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
DRMTestApp_total_lr_tests_type DRMTestApp_RawRetrieveUnitTests ( IShell* pIShell )
{
  uint8                                _err_flag;
  uint8*                               byte_stream;
  uint32                               num_rights_1;
  uint32                               bytes_reqd;
  uint32                               bytes_copied;
  uint32                               stream_size;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_XactionIdType                 xaction_id_1;
  DRMTestApp_total_lr_tests_type       num_lr_tests_passed;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear out variables */
  DRMTestApp_uri_1[0] = '\0';
  num_lr_tests_passed.num_lookup_tests = 0;
  num_lr_tests_passed.num_retrieve_tests = 0;
  _err_flag = 0;
  num_rights_1 = 0;
  bytes_copied = 0;
  bytes_reqd = 0;
  stream_size = 0;
  _errno = EFAILED;
  byte_stream = NULL;
  piDRM = NULL;

  do
  {
    byte_stream = (uint8*) MALLOC ( 8096 );
    if ( byte_stream == NULL )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MSG_FATAL ( "***** RawRetrieveUnitTests: BEGIN TEST. *****",
                0,
                0,
                0 );

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    STRLCPY ( DRMTestApp_uri_1, "uri_raw_r_test_int@qualcomm.com", sizeof(DRMTestApp_uri_1));

    /* Do a preemptive delete */

    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    /* Do Add #1 */

   IDRM_AddRight (
      piDRM,
      DRMTestApp_uri_1,
      AEEDRM_UNKNOWN_VERSION,
      (uint8*) DRMTestApp_raw_uri_2_no_right_1_sep_del,
      STRLEN ( DRMTestApp_raw_uri_2_no_right_1_sep_del ),
      &add_rsp,
      NULL
      );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RawRetrieveUnitTests: Add #1 success.",
                 0,
                 0,
                 0 );
    }

   /* Do Add #2 */

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_1,
        AEEDRM_UNKNOWN_VERSION,
        (uint8*) DRMTestApp_raw_uri_2_no_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_2_no_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Add #2 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "RawRetrieveUnitTests: Add #2 success.",
                 0,
                 0,
                 0 );
    }

    /* Do Lookup #1 */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "RawRetrieveUnitTests: Lookup #1 failure.  Error #: %d",
        lookup_rsp.Errno,
        0,
        0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "RawRetrieveUnitTests: Lookup #1 success.  num_rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( num_rights_1 != 2 )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }

    /* Parameters:
     * Retrieve will operate _immediately_ after the lookup takes place
     * List size matches the number of rights for this URI
     */

    stream_size =
        STRLEN ( DRMTestApp_raw_uri_2_no_right_1_sep_del ) +
        STRLEN ( DRMTestApp_raw_uri_2_no_right_1_sep_del );
    bytes_reqd = stream_size;

    MSG_HIGH ( "RawRetrieveUnitTests: # rights %d buffer %d xaction_id %d",
               num_rights_1,
               bytes_reqd,
               xaction_id_1 );

    /* Returns the number of characters BEFORE the terminating null */
    /* Add 2 for the terminating nulls */

    _errno = IDRM_RetrieveRawRight (
                piDRM,
                xaction_id_1,
                                                                byte_stream,
                                                                &stream_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Error #: %d xaction_id %d unusable", _errno, xaction_id_1, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Retrieve #1 was unable to retrieve %d bytes with xaction_id: %d.  Error #: %d",
        bytes_reqd, xaction_id_1, _errno );
      break;
    }

    if ( stream_size != bytes_reqd )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: List size not modified or an incorrect number of bytes received.  bytes_recvd: %d", stream_size, 0, 0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    MEMSET ( byte_stream, 0, bytes_reqd );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RawRetrieveUnitTests: Retrieve test #5 success.",
               0,
               0,
               0 );
    /* End test */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "RawRetrieveUnitTests: Lookup #2 failure.  Error #: %d",
        lookup_rsp.Errno,
        0,
        0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "RawRetrieveUnitTests: Lookup #2 success.  num_rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( stream_size != bytes_reqd )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: List size not modified or an incorrect number of bytes received.  bytes_recvd: %d", stream_size, 0, 0 );
      break;
    }

    /* Parameters
     *
     * Perform another retrieve_raw_right
     * Uses a list size that is larger than the # of rights for the URI
     */

    /* Perform 1st retrieve */

    /* Returns the number of characters BEFORE the terminating null */
    /* Add 2 for the terminating nulls */
    stream_size = bytes_reqd * 2;

    MSG_HIGH ( "RawRetrieveUnitTests: # ro: %d buffer %d xaction_id %d",
               num_rights_1,
               stream_size,
               xaction_id_1 );

    _errno = IDRM_RetrieveRawRight (
                piDRM,
                xaction_id_1,
                                                                byte_stream,
                                                                &stream_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Retrieve #2 was unable to retrieve %d bytes with xaction_id: %d.  Error #: %d",
        bytes_reqd, xaction_id_1, _errno );
      break;
    }

    if ( stream_size != bytes_reqd )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: List size not modified or an incorrect number of bytes received.  bytes_recvd: %d", stream_size, 0, 0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    MEMSET ( byte_stream, 0, stream_size );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RawRetrieveUnitTests: Retrieve test #6 success.",
               0,
               0,
               0 );
    /* End test */

    /* Perform the third and final lookup */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "RawRetrieveUnitTests: Lookup #3 failure.  Error #: %d",
        lookup_rsp.Errno,
        0,
        0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "RawRetrieveUnitTests: Lookup #3 success.  num_rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( num_rights_1 != 2 )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Lookup #3 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }

    /* Perform 3rd retrieve */

    bytes_copied = 0;

    MSG_HIGH ( "RawRetrieveUnitTests: bytes %d buffer %d xaction_id %d",
      bytes_reqd, 8, xaction_id_1 );

    do
    {
      stream_size = 8;

      _errno = IDRM_RetrieveRawRight (
                  piDRM,
                  xaction_id_1,
                                                                        byte_stream,
                                                                        &stream_size
                  );

      if ( _errno == AEEDRM_ENOTFOUND )
      {
        MSG_ERROR ( "RawRetrieveUnitTests: Retrieve #3 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
        _err_flag = 1;
      }

      /* TODO -- Check if the returned objects are the correct ones */

      if ( _err_flag != 1 )
      {
        bytes_copied +=stream_size;
        MSG_HIGH ( "RawRetrieveUnitTests: Copied %d bytes so far.",
                   bytes_copied,
                   0,
                   0 );
      }

      if ( stream_size > 8 )
      {
        _err_flag = 1;
        MSG_ERROR (
          "RawRetrieveUnitTests: Wrong # bytes rcvd.  bytes_copied: %d",
          stream_size,
          0,
          0 );
      }

      /* TODO -- Check if only the first 8 bytes modified -- */
    } while ( _errno != SUCCESS && _err_flag != 1 );

    if ( _err_flag == 1 )
    {
      MSG_ERROR (
      "RawRetrieveUnitTests: An error occurred while retrieving rights.",
      0,
      0,
      0 );
      break;
    }

    if ( bytes_copied != bytes_reqd )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Incorrect number of bytes received.  bytes_copied: %d", bytes_copied, 0, 0 );
      break;
    }

    MEMSET ( byte_stream, 0, bytes_reqd );

    num_lr_tests_passed.num_retrieve_tests++;
    MSG_HIGH ( "RawRetrieveUnitTests: Retrieve test #7 success.",
               0,
               0,
               0 );
    /* End test */

    MSG_FATAL (
      "***** RawRetrieveUnitTests: TEST PASSED. *****",
      0,
      0,
      0 );
    _errno = SUCCESS;
  } while ( 0 );

  /* Print out a failure message */

  if ( _errno != SUCCESS )
  {
    MSG_ERROR (
      "***** RawRetrieveUnitTests: TEST FAILED. *****",
      0,
      0,
      0 );
  }

  /* Parameters
   * Attempt to delete all the rights for an associated URI
   * URI is specified
   */

  MSG_HIGH (
    "RawRetrieveUnitTests: Deleting all rights objects with URI # 1.",
    0,
    0,
    0 );

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "RawRetrieveUnitTests: Error #: %d.  Delete failed.",
                  delete_rsp.Errno,
                  0,
                  0 );
    }
    else
    {
      MSG_HIGH ( "RawRetrieveUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }
  }

  DRMTestApp_uri_1[0] = '\0';

        FREE ( byte_stream );
  IDRM_Release( piDRM );

  return num_lr_tests_passed;
} /* DRMTestApp_RawRetrieveUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_LookupUnitTests

DESCRIPTION
  This function is used to test the DRM lookup_right API.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of lookup tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
DRMTestApp_total_lr_tests_type DRMTestApp_LookupUnitTests ( IShell* pIShell )
{
  uint32                               num_rights_1;
  uint32                               num_rights_2;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsHandleType              rhandle;
  AEEDRM_XactionIdType                 xaction_id_1;
  AEEDRM_XactionIdType                 xaction_id_2;
  DRMTestApp_total_lr_tests_type       lookup_tests_passed;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  num_rights_1 = 0;
  num_rights_2 = 0;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  piDRM = NULL;
  rhandle = 0;
  lookup_tests_passed.num_lookup_tests = 0;
  lookup_tests_passed.num_retrieve_tests = 0;
  STRLCPY ( DRMTestApp_uri_1, "1234@foo.com", sizeof(DRMTestApp_uri_1));
  _errno = EFAILED;

  MSG_FATAL ( "***** LookupUnitTests: BEGIN TEST. *****",
              0,
              0,
              0 );

  do
  {
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_1,
        AEEDRM_UNKNOWN_VERSION,
        (uint8*) DRMTestApp_raw_uri_3_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_3_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "LookupUnitTests: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "LookupUnitTests: Add #1 success.", 0, 0, 0 );
    }

    rhandle = add_rsp.RightsHandle;

    /* Perform 1st lookup */

    /* Parameters:
     * Perform lookup based on rights handle only
     *
     * URI is unspecified
     * Rights handle specified
     */

    MSG_HIGH (
      "LookupUnitTests: Performing first lookup using handle %d only.",
      rhandle,
      0,
      0 );

    IDRM_LookupRight (
        piDRM,
        NULL,
        rhandle,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "LookupUnitTests: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "LookupUnitTests: Lookup #1 success.  num_rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( num_rights_1 != 1 )
    {
      MSG_ERROR ( "LookupUnitTests: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }

    lookup_tests_passed.num_lookup_tests++;
    MSG_HIGH ( "LookupUnitTests: Lookup test #3 success.",
               0,
               0,
               0 );
    /* End test */

    /* Perform second lookup */

    /* Parameters:
     * Perform a lookup based on rights handle and mismatched URI
     *
     * URI is wrongly specified
     * Rights handle is specified
     */

    STRLCPY ( DRMTestApp_uri_2, "xax@xax.com", sizeof(DRMTestApp_uri_2));

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        rhandle,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "LookupUnitTests: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH ( "LookupUnitTests: Lookup #2 success.  num_rights: %d xaction_id: %d", num_rights_2, xaction_id_2, 0 );
    }

    if ( num_rights_2 != 1 )
    {
      MSG_ERROR ( "LookupUnitTests: Lookup #2 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_2, xaction_id_2, 0 );
      break;
    }

    lookup_tests_passed.num_lookup_tests++;
    MSG_HIGH ( "LookupUnitTests: Lookup test #4 success.",
               0,
               0,
               0 );
    /* End test */

   MSG_FATAL ( "***** LookupUnitTests: TEST PASSED *****",
               0,
               0,
               0 );
    _errno = SUCCESS;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** LookupUnitTests: TEST FAILED *****",
                0,
                0,
                0 );
  }

  /* Clear out the db */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "LookupUnitTests: Error #: %d.  Delete failed.",
                  delete_rsp.Errno,
                  0,
                  0 );
    }
    else
    {
      MSG_HIGH ( "LookupUnitTests: Delete succeded.", 0, 0, 0 );
    }
  }

  IDRM_Release( piDRM );

  return lookup_tests_passed;
} /* DRMTestApp_RawRetrieveUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RunLookupRetrieveTests

DESCRIPTION
  This function is used to run all the lookup/retrieve tests and print
  results.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunLookupRetrieveTests ( IShell* pIShell )
{
  DRMTestApp_total_lr_tests_type num_lr_tests_passed_1;
  DRMTestApp_total_lr_tests_type num_lr_tests_passed_2;
  DRMTestApp_total_lr_tests_type num_lr_tests_passed_3;
  DRMTestApp_total_lr_tests_type num_lr_tests_passed;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  num_lr_tests_passed_1.num_lookup_tests = 0;
  num_lr_tests_passed_2.num_lookup_tests = 0;
  num_lr_tests_passed_3.num_lookup_tests = 0;
  num_lr_tests_passed_1.num_retrieve_tests = 0;
  num_lr_tests_passed_2.num_retrieve_tests = 0;
  num_lr_tests_passed_3.num_retrieve_tests = 0;

  num_lr_tests_passed_1 = DRMTestApp_RetrieveUnitTests ( pIShell );
  num_lr_tests_passed_2 = DRMTestApp_RawRetrieveUnitTests ( pIShell );
  num_lr_tests_passed_3 = DRMTestApp_LookupUnitTests ( pIShell );

  num_lr_tests_passed.num_lookup_tests =
  num_lr_tests_passed_1.num_lookup_tests +
  num_lr_tests_passed_2.num_lookup_tests +
  num_lr_tests_passed_3.num_lookup_tests;

  num_lr_tests_passed.num_retrieve_tests =
  num_lr_tests_passed_1.num_retrieve_tests +
  num_lr_tests_passed_2.num_retrieve_tests +
  num_lr_tests_passed_3.num_retrieve_tests;

  MSG_FATAL (
      "**************************************",
      0,
      0,
      0 );
  MSG_FATAL (
      "*   LOOKUP TESTS: %d/%d TESTS PASSED   *",
      num_lr_tests_passed.num_lookup_tests,
      DRMTestApp_total_lr_tests.num_lookup_tests,
      0 );

  MSG_FATAL (
      "*  RETRIEVE TESTS: %d/%d TESTS PASSED  *",
      num_lr_tests_passed.num_retrieve_tests,
      DRMTestApp_total_lr_tests.num_retrieve_tests, 0 );

  MSG_FATAL (
      "**************************************",
      0,
      0,
      0 );
} /* DRMTestApp_RunLookupRetrieveUnitTests */

/*===========================================================================

FUNCTION DRMTestApp_CheckUnitTests

DESCRIPTION
  This function is used to test the DRM check_right API.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of consume tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_CheckUnitTests ( IShell* pIShell )
{
  uint8                                bin_buf[16];
  uint8                                check_tests_passed;
  uint8                                counter;
  uint8                                _err_flag;
  uint32                               ro_list_size;
  uint32                               rhandle_1;
  uint32                               rhandle_2;
  uint32                               rhandle_3;
  uint32                               rhandle_4;
  uint32                               num_rights_1;
  AEEDRM_ActionEnumType                action;
  uint32                               bin_buf_len;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_ConsumeRightRspType           consume_rsp;
  AEEDRM_CheckRightRspType              check_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_XactionIdType                 xaction_id_1;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  check_tests_passed = 0;
  num_rights_1 = 0;
  xaction_id_1 = 0;
  bin_buf_len = 0;
  rhandle_1 = 0;
  rhandle_2 = 0;
  rhandle_3 = 0;
  rhandle_4 = 0;
  counter = 0;
  _err_flag = 0;
  _errno = EFAILED;
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  ro_list = NULL;
  piDRM = NULL;
  bin_buf_len = 0;
  action = 0;
  ro_list_size = 0;

  do 
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      DBGPRINTF ( "CheckUnitTests: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MEMSET ( bin_buf, 0, sizeof ( bin_buf ) );

    DBGPRINTF ( "***** CheckUnitTests: BEGIN TEST *****", 0, 0, 0 );

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      DBGPRINTF("Can't create a DRM Object", 0,0,0 );
      break;
    }


    /*
     * Check Unit Test #1
     *
     * Check this right using an action which it has
     */

    /*
     * Add a right with one permission ( PLAY )
     *
     * This permission has 2 counts and will be used for tests 1 through 4
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_10_consume_right_1_1_perm,
        STRLEN ( DRMTestApp_oma10_uri_10_consume_right_1_1_perm ),
        &add_rsp,
        NULL
        );

    if ( add_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 failed.  Error #: %d",
          add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY( DRMTestApp_uri_1,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );

    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
    rhandle_1 = add_rsp.RightsHandle;



    action = AEEDRM_ACTION_PLAY;

    IDRM_CheckRight( piDRM,
                     NULL,     // URI
                     rhandle_1,
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: The right with handle %d could not be checked.  Error #: %d", rhandle_1, check_rsp.Errno, 0 );
      break;
    }
    

    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle_1,
                       action,
                       &consume_rsp,
                       NULL);

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_1, consume_rsp.Errno, 0 );
      break;
    }

    /*
     * Ensure that the license key returned by the consume is the correct one
     */
    bin_buf_len = sizeof( bin_buf );

    _errno = DRMTestApp_xmlproc_base64_decode(
                       (uint8*)DRMTestApp_oma10_all_license,
                       sizeof( DRMTestApp_oma10_all_license ) -1,
                       (uint8*) bin_buf,
                       &bin_buf_len);

    if ( _errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Base 64 decode failed %d, len %d.",
                  _errno, bin_buf_len, 0 );
      break;
    }

    if ( MEMCMP( bin_buf,
                 consume_rsp.LicenseKey.KeyInfo.Value,
                 bin_buf_len )
         ||
         ( consume_rsp.LicenseKey.KeyInfo.Size
          !=
          bin_buf_len ) )
    {
      DBGPRINTF ( "CheckUnitTests: Bad license.", 0, 0, 0 );
      break;
    }

    /*
     * Now, do a lookup and retrieve the right.
     *
     * We do this so that we can check the constraints and confirm that the
     * count was deleted
     */
    IDRM_LookupRight 
      ( piDRM,
        NULL,
        rhandle_1,
        &lookup_rsp,
        NULL
        );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      DBGPRINTF (
        "CheckUnitTests: Lookup for rhandle %d failed with error #%d.",
        rhandle_1, lookup_rsp.Errno, 0 );
      break;
    }
      
      xaction_id_1 = lookup_rsp.XactionID;
      num_rights_1 = lookup_rsp.NumRights;

    /*
     * Retrieve the right
     */
    ro_list_size = 1;

    _errno = IDRM_RetrieveRight 
              ( piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    if ( _errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Failed to receive all rights using xaction_id %d.", xaction_id_1, 0, 0 );
      break;
    }

    if ( ro_list[0].Constraints.Play.RemainingCount != 1 )
    {
      DBGPRINTF ( "CheckUnitTests: Consume operated incorrectly.  %d/%d play counts left.",
        ro_list[0].Constraints.Play.RemainingCount,
        ro_list[0].Constraints.Play.TotalCount,
        0
        );
      break;
    }
    
    /* Now consume again */
    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle_1,
                       action,
                       &consume_rsp,
                       NULL);

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_1, consume_rsp.Errno, 0 );
      break;
    }

    /* Check again */
    IDRM_CheckRight( piDRM,
                     DRMTestApp_uri_1,     // URI
                     0,                    // Rights Handle
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != AEEDRM_ERIGHTSEXPIRED )
    {
      DBGPRINTF ( "CheckUnitTests: The right with handle %d could not be checked.  Error #: %d", rhandle_1, check_rsp.Errno, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Check test #1 succeeded.",0,0,0 );
      check_tests_passed++;
    }


    /*
     * Check Unit Test #2
     *
     * Try to check the right using an action for which it doesn't have a
     * permission
     */
    action = AEEDRM_ACTION_EXECUTE;

    IDRM_CheckRight 
      ( piDRM,
        NULL,
        rhandle_1,
        action,
        &check_rsp,
        NULL
        );

    if ( check_rsp.Errno == EFAILED )
    {
      DBGPRINTF (
      "CheckUnitTests: The right with handle %d could not be checked.",
      rhandle_1, check_rsp.Errno, 0 );
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: The right with handle %d was checked.", 
                   rhandle_1, 0, 0 );
      break;
    }

    DBGPRINTF ( "CheckUnitTests: Check test #2 passed.",0,0,0 );
    check_tests_passed++;

    /*
     * Check Unit Test #3
     *
     * Test out whether no permissions means _all_ actions allowed and whether all
     * constraints permanent
     */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_13_consume_right_1_0_perm,
        STRLEN ( DRMTestApp_oma10_uri_13_consume_right_1_0_perm ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Add #7 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Add #7 success. rhandle: %d",
                 add_rsp.RightsHandle,
                 0,
                 0 );
    }

    rhandle_4 = add_rsp.RightsHandle;

    action = AEEDRM_ACTION_PLAY;

    IDRM_CheckRight 
      ( piDRM,
        NULL,
        rhandle_4,
        action,
        &check_rsp,
        NULL
        );

    if ( check_rsp.Errno == EFAILED )
    {
      DBGPRINTF ("CheckUnitTests: Invalid Right %d, %d", rhandle_4, check_rsp.Errno, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Valid Right", 0, 0, 0 );
    }


    /* Loop 3 times to confirm that the right can be consumed */
    for ( counter = 0; counter < 3; counter++ )
    {
      IDRM_ConsumeRight (
          piDRM,
          NULL,
          rhandle_4,
          action,
          &consume_rsp,
          NULL
          );

      ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

      if ( consume_rsp.Errno != SUCCESS )
      {
        DBGPRINTF ( "CheckUnitTests: Error #: %d.  RO %d cannot be consumed.",
            consume_rsp.Errno,
            rhandle_4,
            0
            );
        _err_flag = 1;
        break;
      }
      else
      {
        DBGPRINTF (
            "CheckUnitTests: The right with handle %d was consumed.",
            rhandle_4,
            0,
            0 );
      }
    } /* end for */

    if ( _err_flag == 1 )
    {
      DBGPRINTF ( "CheckUnitTests: Consume Error", 0, 0, 0 );
      /*                      break; */
    }

    /* Now check the Rights again */
    IDRM_CheckRight 
      ( piDRM,
        NULL,
        rhandle_4,
        action,
        &check_rsp,
        NULL
        );

    if ( check_rsp.Errno == EFAILED )
    {
      DBGPRINTF ("CheckUnitTests: Invalid Right %d, %d", rhandle_4, check_rsp.Errno, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Valid Right", 0, 0, 0 );
    }

    DBGPRINTF ( "CheckUnitTests: Check test #3 passed.",0,0,0 );
    check_tests_passed++;

#if 0
    /*
     * Check Unit Test #4
     *
     * Check this right for a time based constraint
     *
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_14_time_right_1_1_perm,
        STRLEN ( DRMTestApp_oma10_uri_14_time_right_1_1_perm ),
        &add_rsp,
        NULL
        );

    if ( add_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 failed.  Error #: %d",
          add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY( DRMTestApp_uri_2,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );

    DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
    rhandle_2 = add_rsp.RightsHandle;



    action = AEEDRM_ACTION_PLAY;

    IDRM_CheckRight( piDRM,
                     NULL,     // URI
                     rhandle_2,
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: The right with handle %d could not be checked.  Error #: %d", rhandle_1, check_rsp.Errno, 0 );
      break;
    }


    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle_2,
                       action,
                       &consume_rsp,
                       NULL);

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_1, consume_rsp.Errno, 0 );
      break;
    }

    DBGPRINTF ( "CheckUnitTests: Check test #4 passed.",0,0,0 );
    check_tests_passed++;  

#endif

    /*
     * Check Unit Test #5
     *
     * Check this right for a interval based constraint
     *
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_15_interval_right_1_1_perm,
        STRLEN ( DRMTestApp_oma10_uri_15_interval_right_1_1_perm ),
        &add_rsp,
        NULL
        );

    if ( add_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 failed.  Error #: %d",
          add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY( DRMTestApp_uri_2,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );

    DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
    rhandle_2 = add_rsp.RightsHandle;



    action = AEEDRM_ACTION_PLAY;

    IDRM_CheckRight( piDRM,
                     NULL,     // URI
                     rhandle_2,
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Invalid Right Error #: %x",check_rsp.Errno,0, 0 );
      break;
    }


    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle_2,
                       action,
                       &consume_rsp,
                       NULL);

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_1, consume_rsp.Errno, 0 );
      break;
    }   

    /* Now check Rights again */
    IDRM_CheckRight( piDRM,
                     NULL,     // URI
                     rhandle_2,
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Invalid Right Error #: %x",check_rsp.Errno,0, 0 );
      break;
    }

    /* Sleep for 12 seconds */
    MSLEEP(12000);

    IDRM_CheckRight( piDRM,
                     NULL,     // URI
                     rhandle_2,
                     action,
                     &check_rsp,
                     NULL);

    ASSERT ( check_rsp.RspType == AEEDRM_RSP_CHECK_RIGHT );

    if ( check_rsp.Errno != AEEDRM_ERIGHTSEXPIRED )
    {
      DBGPRINTF ( "CheckUnitTests: Invalid Right Error #: %x",check_rsp.Errno,0, 0 );
      break;
    }
    
    DBGPRINTF ( "CheckUnitTests: Check test #5 passed.",0,0,0 );
    check_tests_passed++;

    
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    DBGPRINTF ( "***** CheckUnitTests: TEST FAILED. *****",
                0,
                0,
                0 );
  }

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      DBGPRINTF ( "CheckUnitTests: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      DBGPRINTF ( "CheckUnitTests: Delete succeded.", 0, 0, 0 );
    }
  }

  FREE ( ro_list );
  IDRM_Release( piDRM );

  return check_tests_passed;

}/* DRMTestApp_CheckUnitTests */

/*===========================================================================

FUNCTION DRMTestApp_RunCheckTests

DESCRIPTION
  This function is used to run all the consume tests and display their results

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of delete tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunCheckTests ( IShell* pIShell )
{
  uint8 check_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  check_tests_passed += DRMTestApp_CheckUnitTests ( pIShell );

  DBGPRINTF ( "**************************************", 0, 0, 0 );
  DBGPRINTF ( "*   Check TESTS: %d/%d TESTS PASSED  *",
              check_tests_passed,
              DRMTestApp_total_check_tests,
              0 );
  DBGPRINTF ( "**************************************", 0, 0, 0 );
} /* DRMTestApp_RunCheckTests */


/*===========================================================================

FUNCTION DRMTestApp_ConsumeUnitTests

DESCRIPTION
  This function is used to test the DRM consume_right API.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of consume tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_ConsumeUnitTests ( IShell* pIShell )
{
  uint8                                bin_buf[16];
  uint8                                consume_tests_passed;
  uint8                                counter;
  uint8                                _err_flag;
  uint32                               ro_list_size;
  uint32                               rhandle_1;
  uint32                               rhandle_2;
  uint32                               rhandle_3;
  uint32                               rhandle_4;
  uint32                               num_rights_1;
  AEEDRM_ActionEnumType                action;
  uint32                               bin_buf_len;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_ConsumeRightRspType           consume_rsp;
  AEEDRM_DeleteRightRspType            delete_rh_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_XactionIdType                 xaction_id_1;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  consume_tests_passed = 0;
  num_rights_1 = 0;
  xaction_id_1 = 0;
  bin_buf_len = 0;
  rhandle_1 = 0;
  rhandle_2 = 0;
  rhandle_3 = 0;
  rhandle_4 = 0;
  counter = 0;
  _err_flag = 0;
  _errno = EFAILED;
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  ro_list = NULL;
  piDRM = NULL;
  bin_buf_len = 0;
  action = 0;
  ro_list_size = 0;

  do 
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "ConsumeUnitTests: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MEMSET ( bin_buf, 0, sizeof ( bin_buf ) );

    MSG_FATAL ( "***** ConsumeUnitTests: BEGIN TEST *****", 0, 0, 0 );

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }



    /*
     * Add a right with one permission ( PLAY )
     *
     * This permission has 2 counts and will be used for tests 1 through 4
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_10_consume_right_1_1_perm,
        STRLEN ( DRMTestApp_oma10_uri_10_consume_right_1_1_perm ),
        &add_rsp,
        NULL
        );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #1 failed.  Error #: %d",
          add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY( DRMTestApp_uri_1,
            add_rsp.szContentURI,
            add_rsp.ContentURILen );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
    rhandle_1 = add_rsp.RightsHandle;



    /*
     * Consume Unit Test #1
     *
     * Consume this right using an action which it has
     */
    action = AEEDRM_ACTION_PLAY;
    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle_1,
                       action,
                       &consume_rsp,
                       NULL);

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_1, consume_rsp.Errno, 0 );
      break;
    }

    /*
     * Ensure that the license key returned by the consume is the correct one
     */
    bin_buf_len = sizeof( bin_buf );

    _errno = DRMTestApp_xmlproc_base64_decode(
                       (uint8*)DRMTestApp_oma10_all_license,
                       sizeof( DRMTestApp_oma10_all_license ) -1,
                       (uint8*) bin_buf,
                       &bin_buf_len);

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Base 64 decode failed %d, len %d.",
                  _errno, bin_buf_len, 0 );
      break;
    }

    if ( MEMCMP( bin_buf,
                 consume_rsp.LicenseKey.KeyInfo.Value,
                 bin_buf_len )
         ||
         ( consume_rsp.LicenseKey.KeyInfo.Size
          !=
          bin_buf_len ) )
    {
      MSG_ERROR ( "ConsumeUnitTests: Bad license.", 0, 0, 0 );
      break;
    }

    /*
     * Now, do a lookup and retrieve the right.
     *
     * We do this so that we can check the constraints and confirm that the
     * count was deleted
     */
    IDRM_LookupRight 
      ( piDRM,
        NULL,
        rhandle_1,
        &lookup_rsp,
        NULL
        );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "ConsumeUnitTests: Lookup for rhandle %d failed with error #%d.",
        rhandle_1, lookup_rsp.Errno, 0 );
      break;
    }
      
      xaction_id_1 = lookup_rsp.XactionID;
      num_rights_1 = lookup_rsp.NumRights;

    /*
     * Retrieve the right
     */
    ro_list_size = 1;

    _errno = IDRM_RetrieveRight 
              ( piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Failed to receive all rights using xaction_id %d.", xaction_id_1, 0, 0 );
      break;
    }

    if ( ro_list[0].Constraints.Play.RemainingCount != 1 )
    {
      MSG_ERROR ( "ConsumeUnitTests: Consume operated incorrectly.  %d/%d play counts left.",
        ro_list[0].Constraints.Play.RemainingCount,
        ro_list[0].Constraints.Play.TotalCount,
        0
        );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Consume test #1 succeeded.",0,0,0 );
      consume_tests_passed++;
    }



    /*
     * Consume Unit Test #2
     *
     * Try to consume the right using an action for which it doesn't have a
     * permission
     */
    action = AEEDRM_ACTION_EXECUTE;

    IDRM_ConsumeRight 
      ( piDRM,
        NULL,
        rhandle_1,
        action,
        &consume_rsp,
        NULL
        );

    if ( consume_rsp.Errno == EFAILED )
    {
      MSG_HIGH (
      "ConsumeUnitTests: The right with handle %d could not be consumed.",
      rhandle_1, consume_rsp.Errno, 0 );
    }
    else
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d was consumed.", 
                   rhandle_1, 0, 0 );
      break;
    }

    MSG_HIGH ( "ConsumeUnitTests: Consume test #2 passed.",0,0,0 );
    consume_tests_passed++;



    /*
     * Consume Unit Test #3
     *
     * The consume API has two possible behaviors.  In one scenario, you pass
     * it a URI.  Consume will pick a right ( specific right not guaranteed )
     * associated with that URI that matches the required action and consume
     * it
     *
     * Alternatively, you can give it a rights handle
     *
     * In this case, we're going to look up all the rights associated with a
     * URI, grab the first one, get its rights handle and pass that to
     * consume.
     */
    IDRM_LookupRight 
      ( piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
      );


    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Lookup returned Error #: %d.",
         lookup_rsp.Errno, 0, 0 );
      _errno = lookup_rsp.Errno;
      break;
    }

    xaction_id_1 = lookup_rsp.XactionID;
    num_rights_1 = lookup_rsp.NumRights;

    /*
     * Now let's retrieve all the rights associated with this xaction id
     */

    ro_list_size = 1;

    _errno = IDRM_RetrieveRight 
              ( piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
              );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Retrieve unsuccessful.  Error #: %d.", _errno, 0, 0 );
      MSG_ERROR ( "FAIL: Transaction ID: %d.", xaction_id_1, 0, 0 );
      break;
    }

    /*
     * Now that we've retrieved the rights, lets use the rights handle to
     * pass it to the consume
     */

    /*
     * We set this to an action we know the right has
     */

    action = AEEDRM_ACTION_PLAY;

    IDRM_ConsumeRight
      ( piDRM,
        NULL,
        ro_list[0].RightsHandle,
        action,
        &consume_rsp,
        NULL
      );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: The consume failed with Error #%d.",
          consume_rsp.Errno, 0, 0 );
      _errno = consume_rsp.Errno;
      break;
    }

    if ( consume_rsp.Errno == SUCCESS )
    {
      consume_tests_passed++;
      MSG_HIGH ( "Consume Test #3 passed!", 0, 0, 0 );
    }



    /*
     * Consume Unit Test #4
     *
     * In the previous test, we consumed the right again.  However, the
     * internal state of the right shows that its play count is now
     * decremented to 0.
     *
     * This test confirms that if consume is called a _third_ time on the
     * right, that the right is actually deleted
     */

    /*
     * Now I call consume a third and final time.  Since the internal state of
     * count is already 0, this will trigger the delete
     */
    action = AEEDRM_ACTION_PLAY;

    IDRM_ConsumeRight
      ( piDRM,
        NULL,
        ro_list[0].RightsHandle,
        action,
        &consume_rsp,
        NULL
      );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_HIGH ( "Unable to consume.  Error #%d.", consume_rsp.Errno, 0, 0 );
    }

    /*
     * We do a lookup to confirm that consume has (indeed) deleted the right
     */

    IDRM_LookupRight
      ( piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
      );

    if ( lookup_rsp.NumRights != 0 )
    {
      MSG_ERROR ( "FAIL: Consume did not delete rights.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Number of rights: %d.", lookup_rsp.NumRights, 0, 0 );
      _errno = EFAILED;
      break;
    }

    consume_tests_passed++;
    MSG_HIGH ( "Consume Test #4 passed!", 0, 0, 0 );



    /*
     * Add two rights with two permissions each
     *
     * These rights are used in test cases X through X
     */

    /*
     * First right
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_11_consume_right_1_2_perm,
        STRLEN ( DRMTestApp_oma10_uri_11_consume_right_1_2_perm ),
        &add_rsp,
        NULL
        );


    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #2 failed.  Error #: %d",
                  add_rsp.Errno,0,0 );
      break;
    }

    MEMCPY ( DRMTestApp_uri_2,
             add_rsp.szContentURI,
             add_rsp.ContentURILen
           );
    DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
    rhandle_1 = add_rsp.RightsHandle;

    /*
     * Second right
     */
    IDRM_AddRight 
      ( piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_11_consume_right_2_2_perm,
        STRLEN ( DRMTestApp_oma10_uri_11_consume_right_2_2_perm ),
        &add_rsp,
        NULL
        );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #3 failed.  Error #: %d",
                  add_rsp.Errno,0,0 );
      break;
    }

    rhandle_2 = add_rsp.RightsHandle;

    /*
     * Consume Unit Test #5
     *
     * In this test, I am checking to ensure that the consume API is able to
     * use a URI and choose a right that's valid
     *
     * Both the rights have a common permission type, so one should be
     * returned
     */

    action = AEEDRM_ACTION_PLAY;

    IDRM_ConsumeRight 
      ( piDRM,
      DRMTestApp_uri_2,
      0x00,
      action,
      &consume_rsp,
      NULL
      );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Consume failed.",0,0,0 );
      break;
    }

    /*
     * Check if a correct rights handle was returned
     */
    if ( ( consume_rsp.RightsHandle != rhandle_1 )
         && 
         ( consume_rsp.RightsHandle != rhandle_2 ) 
       )
    {
      MSG_ERROR 
        ( "ConsumeUnitTests: Consume returned an invalid rights handle: %d",
                   consume_rsp.RightsHandle,0,0 );
      break;
    }
      
      consume_tests_passed++;
      MSG_HIGH ( "ConsumeUnitTests: Consume test #5 passed.",0,0,0 );



    /* Add another right for URI 2 */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_11_consume_right_3_2_perm,
        STRLEN ( DRMTestApp_oma10_uri_11_consume_right_3_2_perm ),
        &add_rsp,
        NULL
        );


    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #4 failed.  Error #: %d",
          add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #4 success. rhandle: %d",
          add_rsp.RightsHandle, 0, 0 );
    }

    rhandle_3 = add_rsp.RightsHandle;

    /* Consume its print / play */
    action = AEEDRM_ACTION_PLAY;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: The right with handle %d was consumed.", rhandle_3, 0, 0 );
    }

      MSG_HIGH ( "ConsumeUnitTests: The right was printed properly.",
                 0,
                 0,
                 0 );

    /* Print / Play again */
    action = AEEDRM_ACTION_PLAY;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
      "ConsumeUnitTests: The right with handle %d was consumed. Error %d",
      rhandle_3,
      consume_rsp.Errno,
      0 );
    }

      MSG_HIGH ( "ConsumeUnitTests: The right can be played.",
                 0,
                 0,
                 0 );

    /* Attempt to consume its play */
    action = AEEDRM_ACTION_PLAY;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "ConsumeUnitTests: The right with handle %d was consumed.",
        rhandle_3,
        0,
        0 );
    }

      MSG_HIGH ( "ConsumeUnitTests: Consume test #6 passed.", 0, 0, 0 );
      consume_tests_passed++;
    
    /* --------------------------------------------------------------------*/
    /* Do no constraints == permanent license for that permission?                           */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_12_consume_right_1_1_perm_no_constraints,
        STRLEN ( DRMTestApp_oma10_uri_12_consume_right_1_1_perm_no_constraints ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #5 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #5 success. rhandle: %d",
                 add_rsp.RightsHandle,
                 0,
                 0 );
    }

    MEMCPY ( DRMTestApp_uri_3,
             add_rsp.szContentURI,
             add_rsp.ContentURILen
           );
    DRMTestApp_uri_3[add_rsp.ContentURILen] = '\0';
    rhandle_4 = add_rsp.RightsHandle;
                
    /* Loop 3 times to confirm that the right can be consumed */
    for ( counter = 0; counter < 3; counter++ )
    {
      action = AEEDRM_ACTION_PLAY;

      IDRM_ConsumeRight (
          piDRM,
          NULL,
          rhandle_4,
          action,
          &consume_rsp,
          NULL
          );

      ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

      if ( consume_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  RO %d Cannot be consumed.",
            consume_rsp.Errno,
            rhandle_4,
            0
            );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH (
            "ConsumeUnitTests: RO %d was consumed.",
            rhandle_4,
            0,
            0 );
      }

      action= action << 1;

    } /* end for */

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "ConsumeUnitTests: Consume error.", 0, 0, 0 );
      /*      break; */
    }

    /* Now Do a retrieve to check the constraints manually */
    IDRM_LookupRight ( piDRM,
        NULL,
        rhandle_4,
        &lookup_rsp,
        NULL
        );

    ASSERT ( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    xaction_id_1 = lookup_rsp.XactionID;
    ro_list_size = 1;

    _errno = IDRM_RetrieveRight ( piDRM,
        xaction_id_1,
        ro_list,
        &ro_list_size
        );
    
    if ( _errno != SUCCESS )
    {
      MSG_ERROR ("Could not retrieve rights using xaction_id %d.", 
          xaction_id_1,
          0,
          0 );
      break;
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ("Incorrect list size of %d returned.", ro_list_size, 0, 0 );
      break;
    }

    /* We should have a permanent mask on the play constraint */
    if ( ( ro_list[0].Constraints.Play.ConstraintsMask != 
          AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Play.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Play.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d", 
          ro_list[0].Constraints.Play.TotalCount,
          0,
          0 );
      /*                      break;*/
    }

    consume_tests_passed++;
    MSG_HIGH ( "ConsumeUnitTests: Consume test #7 succeeded.", 0, 0, 0 );

    rhandle_4 = 0;
    _err_flag = 0;
    counter = 0;

    /* Right with the same layout as previous (but different XML)             */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_12_consume_right_2_1_perm_no_constraints,
        STRLEN ( DRMTestApp_oma10_uri_12_consume_right_2_1_perm_no_constraints ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #6 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #6 success. rhandle: %d",
                 add_rsp.RightsHandle,
                 0,
                 0 );
    }

    rhandle_4 = add_rsp.RightsHandle;

    /* Loop 3 times to confirm that the right can be consumed */
    for ( counter = 0; counter < 3; counter++ )
    {
      action = AEEDRM_ACTION_PLAY;
      IDRM_ConsumeRight (
          piDRM,
          NULL,
          rhandle_4,
          action,
          &consume_rsp,
          NULL
          );

      ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

      if ( consume_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  RO %d cannot be consumed.",
            consume_rsp.Errno,
            rhandle_4,
            0
            );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH (
            "ConsumeUnitTests: The right with handle %d was consumed.",
            rhandle_4,
            0,
            0 );
      }

    } /* end for */

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "ConsumeUnitTests: Consume Error.", 0, 0, 0 );
      /*                      break; */
    }

    /* Now Do a retrieve to check the constraints manually */
    IDRM_LookupRight ( piDRM,
        NULL,
        rhandle_4,
        &lookup_rsp,
        NULL
        );

    ASSERT ( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    xaction_id_1 = lookup_rsp.XactionID;
    ro_list_size = 1;

    _errno = IDRM_RetrieveRight ( piDRM,
        xaction_id_1,
        ro_list,
        &ro_list_size
        );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Could not retrieve rights using xaction_id %d.",
          xaction_id_1,
          0,
          0 );
      break;
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ("Incorrect list size of %d returned.", ro_list_size, 0, 0 );
      break;
    }

    /* We should have a permanent mask on the play constraint */
    if ( ( ro_list[0].Constraints.Play.ConstraintsMask != AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Play.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Play.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d",
          ro_list[0].Constraints.Play.TotalCount,
          0,
          0 );
      /* break; */
    }

    consume_tests_passed++;
    MSG_HIGH ( "ConsumeUnitTests: Consume test #8 succeeded.", 0, 0, 0 );

    rhandle_4 = 0;
    _err_flag = 0;
    counter = 0;

    /* Test out whether no permissions means _all_ actions allowed and whether all
     * constraints permanent */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_13_consume_right_1_0_perm,
        STRLEN ( DRMTestApp_oma10_uri_13_consume_right_1_0_perm ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #7 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #7 success. rhandle: %d",
                 add_rsp.RightsHandle,
                 0,
                 0 );
    }

    rhandle_4 = add_rsp.RightsHandle;

    /* Loop 3 times to confirm that the right can be consumed */
    for ( counter = 0; counter < 3; counter++ )
    {
      action = AEEDRM_ACTION_PLAY;
      IDRM_ConsumeRight (
          piDRM,
          NULL,
          rhandle_4,
          action,
          &consume_rsp,
          NULL
          );

      ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

      if ( consume_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  RO %d cannot be consumed.",
            consume_rsp.Errno,
            rhandle_4,
            0
            );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH (
            "ConsumeUnitTests: The right with handle %d was consumed.",
            rhandle_4,
            0,
            0 );
      }
    } /* end for */

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "ConsumeUnitTests: Consume Error", 0, 0, 0 );
      /*                      break; */
    }

    /* Now Do a retrieve to check the constraints manually */
    IDRM_LookupRight ( piDRM,
        NULL,
        rhandle_4,
        &lookup_rsp,
        NULL
        );

    ASSERT ( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    xaction_id_1 = lookup_rsp.XactionID;
    ro_list_size = 1;

    _errno = IDRM_RetrieveRight ( piDRM,
        xaction_id_1,
        ro_list,
        &ro_list_size
        );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Could not retrieve rights using xaction_id %d.",
          xaction_id_1,
          0,
          0 );
      break;
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ( "Incorrect list size of %d returned.", ro_list_size, 0, 0 );
      break;
    }

    if ( ro_list[0].PermissionsMask != ( AEEDRM_PERM_PLAY |
          AEEDRM_PERM_EXECUTE | AEEDRM_PERM_DISPLAY |
          AEEDRM_PERM_PRINT ) )
    {
      MSG_ERROR ( "Incorrect permissions returned: 0x%x",
          ro_list[0].PermissionsMask, 0, 0 );
      /*                      break;*/
    }

    /* We should have a permanent mask on the play constraint */
    if ( ( ro_list[0].Constraints.Play.ConstraintsMask != 
          AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Play.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications for play.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Play.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d", ro_list[0].Constraints.Play.TotalCount,
          0,
          0 );
      /* break; */
    }

    /* We should have a permanent mask on the display constraint */
    if ( ( ro_list[0].Constraints.Display.ConstraintsMask != 
          AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Display.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications for display.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Display.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d",
          ro_list[0].Constraints.Display.TotalCount,
          0,
          0 );
      /*                      break; */
    }

    /* We should have a permanent mask on the print constraint */
    if ( ( ro_list[0].Constraints.Print.ConstraintsMask != 
          AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Print.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications for print.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Print.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d",
          ro_list[0].Constraints.Print.TotalCount,
          0,
          0 );
      /*break;*/
    }

    /* We should have a permanent mask on the execute constraint */
    if ( ( ro_list[0].Constraints.Play.ConstraintsMask != 
          AEEDRM_CONSTR_PERMANENT ) ||
        ( ro_list[0].Constraints.Play.TotalCount != 0 ) )
    {
      MSG_ERROR ( "Incorrect specifications for execute.", 0, 0, 0 );
      MSG_ERROR ( "Constraints: 0x%x",
          ro_list[0].Constraints.Execute.ConstraintsMask,
          0,
          0 );
      MSG_ERROR ( "Total Count: %d",
          ro_list[0].Constraints.Execute.TotalCount,
          0,
          0 );
      /*break;*/
    }

    consume_tests_passed++;
    MSG_HIGH ( "ConsumeUnitTests: Consume test #9 succeeded.", 0, 0, 0 );

    /* Delete the right */

    IDRM_DeleteRight (
        piDRM,
        rhandle_4,
        &delete_rh_rsp,
        NULL
        );

    if ( delete_rh_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  Delete failed.",
                  delete_rh_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Delete succeded.",
                 0,
                 0,
                 0 );
    }

    rhandle_4 = 0;
    _err_flag = 0;
    counter = 0;    
    /* --------------------------------------------------------------------- */ 
   /* Add last right for URI 2 */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_11_consume_right_4_2_perm,
        STRLEN ( DRMTestApp_oma10_uri_11_consume_right_4_2_perm ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Add #9 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Add #9 success. rhandle: %d",
                 add_rsp.RightsHandle,
                 0,
                 0 );
    }

    rhandle_3 = add_rsp.RightsHandle;

    /* Consume its print */
    action = AEEDRM_ACTION_PRINT;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "ConsumeUnitTests: The right with handle %d was consumed.",
        rhandle_3,0,0 );
    }

    /* Print again */
    action = AEEDRM_ACTION_PRINT;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_HIGH ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
    }
    else
    {
      MSG_ERROR (
        "ConsumeUnitTests: The right with handle %d was consumed.",
        rhandle_3, 0, 0 );
      break;
    }

    /* Attempt to consume its play */
    action = AEEDRM_ACTION_PLAY;
    IDRM_ConsumeRight (
        piDRM,
        NULL,
        rhandle_3,
        action,
        &consume_rsp,
        NULL
        );

    ASSERT ( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: The right with handle %d could not be consumed.  Error #: %d", rhandle_3, consume_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "ConsumeUnitTests: The right with handle %d was consumed.",
        rhandle_3,
        0,
        0 );
      
      MSG_HIGH ( "ConsumeUnitTests: Consume test #10 passed.", 0, 0, 0 );
      consume_tests_passed++;
      _errno = SUCCESS;
    }
    
    MSG_FATAL ( "***** ConsumeUnitTests: TEST PASSED. *****",
                0,
                0,
                0 );
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** ConsumeUnitTests: TEST FAILED. *****",
                0,
                0,
                0 );
  }

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_3[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_3,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "ConsumeUnitTests: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "ConsumeUnitTests: Delete succeded.", 0, 0, 0 );
    }
  }
 
        FREE ( ro_list );
        IDRM_Release( piDRM );
  
        return consume_tests_passed;
} /* DRMTestApp_ConsumeUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RunConsumeTests

DESCRIPTION
  This function is used to run all the consume tests and display their results

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of delete tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunConsumeTests ( IShell* pIShell )
{
  uint8 consume_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  consume_tests_passed += DRMTestApp_ConsumeUnitTests ( pIShell );

  MSG_FATAL ( "**************************************", 0, 0, 0 );
  MSG_FATAL ( "*   CONSUME TESTS: %d/%d TESTS PASSED  *",
              consume_tests_passed,
              DRMTestApp_total_consume_tests,
              0 );
  MSG_FATAL ( "**************************************", 0, 0, 0 );
} /* DRMTestApp_RunConsumeTests */



/*===========================================================================

FUNCTION DRMTestApp_DeleteUnitTests

DESCRIPTION
  This function is used to test the DRM delete_right/delete_associated_right

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of delete tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_DeleteUnitTests ( IShell* pIShell )
{
  uint8                                delete_tests_passed;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delassoc_rsp;
  AEEDRM_DeleteRightRspType            delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsHandleType              rhandle;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  delete_tests_passed = 0;
  _errno = EFAILED;
  piDRM = NULL;

  DRMTestApp_uri_1[0] = '\0';

  MSG_FATAL ( "***** DeleteUnitTests: BEGIN TEST. *****",
              0,
              0,
              0 );

  do 
  {
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }
    /* Do Add #1 for URI #1 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "DeleteUnitTests: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "DeleteUnitTests: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY (
      DRMTestApp_uri_1,
      add_rsp.szContentURI,
      add_rsp.ContentURILen
      );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';


    /* Do Add #2 for URI #1 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_2_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_2_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "DeleteUnitTests: Add #2 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "DeleteUnitTests: Add #2 success.", 0, 0, 0 );
    }

    /* Do del_assoc on these two (uri)*/

    /* Parameters
     * Attempt to delete all the rights for an associated URI
     * URI is specified
     */

    MSG_HIGH ( "DeleteUnitTests: Deleting all rights with URI # 1.",
               0,
               0,
               0 );

    if ( DRMTestApp_uri_1[0] != '\0' )
    {
      IDRM_DeleteAssociatedRights (
          piDRM,
          DRMTestApp_uri_1,
          &delassoc_rsp,
          NULL
          );

      ASSERT ( delassoc_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

      if ( delassoc_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "DeleteUnitTests: Error #: %d.  Delete failed.", delassoc_rsp.Errno, 0, 0 );
        break;
      }
      else
      {
        MSG_HIGH ( "DeleteUnitTests: Delete succeded.",
                   0,
                   0,
                   0 );
      }

      /* Confirm the deletion */
      MSG_HIGH ( "DeleteUnitTests: Confirming that the delete worked.",
                 0,
                 0,
                 0 );

      IDRM_LookupRight (
          piDRM,
          DRMTestApp_uri_1,
          0x00,
          &lookup_rsp,
          NULL
          );

      ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

      if ( lookup_rsp.Errno != SUCCESS )
      {
        MSG_HIGH ( "DeleteUnitTests: Lookup failure.  Error #: %d", lookup_rsp.Errno, 0, 0 );
        delete_tests_passed++;
        MSG_HIGH ( "DeleteUnitTests: Delete test #1 success.", 0, 0, 0 );
      }
      else
      {
        MSG_ERROR ( "DeleteUnitTests: Lookup success.  ERROR!!", 0, 0, 0 );
        MSG_ERROR ( "DeleteUnitTests: Delete test #1 failure.", 0, 0, 0 );
        break;
      }
    }
    DRMTestApp_uri_1[0] = '\0';

    /* Do Add #3 for URI #2 */

    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_5_print_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_5_print_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "DeleteUnitTests: Add #3 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "DeleteUnitTests: Add #3 success.", 0, 0, 0 );
    }

    MEMCPY (
      DRMTestApp_uri_1,
      add_rsp.szContentURI,
      add_rsp.ContentURILen
      );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
    rhandle = add_rsp.RightsHandle;

    /* Do del_ on this one (rhandle) */

    if ( DRMTestApp_uri_1[0] != '\0' )
    {
      /* Parameters
       * Attempt to delete all the rights for URI #1
       * URI is unspecified
       * Rights handle is specified
       */
      IDRM_DeleteRight (
          piDRM,
          rhandle,
          &delete_rsp,
          NULL
          );
                        
                        /* FIXME */
      ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_RIGHT );

      if ( delete_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "DeleteUnitTests: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
        break;
      }
      else
      {
        MSG_HIGH ( "DeleteUnitTests: Delete succeded.",
                   0,
                   0,
                   0 );
      }

      /* Confirm the deletion */
      MSG_HIGH ( "DeleteUnitTests: Confirming that the delete worked.",
                 0,
                 0,
                 0 );

      IDRM_LookupRight (
          piDRM,
          DRMTestApp_uri_1,
          0x00,
          &lookup_rsp,
          NULL
          );

      ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

      if ( lookup_rsp.Errno != SUCCESS )
      {
        MSG_HIGH ( "DeleteUnitTests: Lookup failure.  Error #: %d",
                   lookup_rsp.Errno,
                   0,
                   0 );
        delete_tests_passed++;
        MSG_HIGH ( "DeleteUnitTests: Delete test #2 success.", 0, 0, 0 );
      }
      else
      {
        MSG_ERROR ( "DeleteUnitTests: Lookup success.  ERROR!!", 0, 0, 0 );
        MSG_ERROR ( "DeleteUnitTests: Delete test #2 failure.", 0, 0, 0 );
        break;
      }
    }
    DRMTestApp_uri_1[0] = '\0';

    /* Do Add #4 for URI #3 */
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_6_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_6_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "DeleteUnitTests: Add #4 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "DeleteUnitTests: Add #4 success.", 0, 0, 0 );
    }

    MEMCPY (
        DRMTestApp_uri_1,
        add_rsp.szContentURI,
        add_rsp.ContentURILen
        );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';

    /* Do del_assoc on this one (uri)*/

    if ( DRMTestApp_uri_1[0] != '\0' )
    {
      IDRM_DeleteAssociatedRights (
          piDRM,
          DRMTestApp_uri_1,
          &delassoc_rsp,
          NULL
          );

      ASSERT (
        delassoc_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

      if ( delassoc_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "DeleteUnitTests: Error #: %d.  Delete failed.", delassoc_rsp.Errno, 0, 0 );
        break;
      }
      else
      {
        MSG_HIGH ( "DeleteUnitTests: Delete succeded.",
                   0,
                   0,
                   0 );
      }

      /* Confirm the deletion */
      MSG_HIGH ( "DeleteUnitTests: Confirming that the delete worked.",
                 0,
                 0,
                 0 );

      IDRM_LookupRight (
          piDRM,
          DRMTestApp_uri_1,
          0x00,
          &lookup_rsp,
          NULL
          );

      ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

      if ( lookup_rsp.Errno != SUCCESS )
      {
        MSG_HIGH ( "DeleteUnitTests: Lookup failure.  Error #: %d",
                   lookup_rsp.Errno,
                   0,
                   0 );
        delete_tests_passed++;
        MSG_HIGH ( "DeleteUnitTests: Delete test #3 success.", 0, 0, 0 );
      }
      else
      {
        MSG_ERROR ( "DeleteUnitTests: Lookup success.  ERROR!!", 0, 0, 0 );
        MSG_ERROR ( "DeleteUnitTests: Delete test #3 failure.", 0, 0, 0 );
        break;
      }

      /*
       * Test #4
       */
      IDRM_DeleteAssociatedRights
        ( piDRM,
          "someuri@uri.com",
          &delassoc_rsp,
          NULL
        );

       ASSERT (
        delassoc_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

      if ( delassoc_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "DeleteUnitTests: Error #: %d.\
            Failed in deleting empty URI.", delassoc_rsp.Errno, 0, 0 );
        break;
      }
      else
      {
        delete_tests_passed++;
        MSG_HIGH ( "DeleteUnitTests: Delete test #4 succeded.",
                   0,
                   0,
                   0 );
      }

    }
    DRMTestApp_uri_1[0] = '\0';

    MSG_FATAL ( "***** DeleteUnitTests: TEST PASSED. *****",
                0,
                0,
                0 );

    _errno = SUCCESS;
  } while ( 0 );

  if ( _errno != SUCCESS ) {
    MSG_ERROR ( "***** DeleteUnitTests: TEST FAILED. *****",
                0,
                0,
                0 );
  }
  /* There's not much we can do if the delete fails now is there? */
  
        IDRM_Release( piDRM );

  return delete_tests_passed;
} /* DRMTestApp_DeleteUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_RUN_DELETE_TESTS

DESCRIPTION
  This function runs all the delete tests and prints out the results.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunDeleteTests ( IShell* pIShell )
{
  uint8 delete_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  delete_tests_passed += DRMTestApp_DeleteUnitTests ( pIShell );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* DELETE TESTS: %d/%d TESTS PASSED  *",
              delete_tests_passed,
              DRMTestApp_total_delete_tests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
} /* DRMTestApp_RunDeleteTests */



/*===========================================================================

FUNCTION DRMTestApp_ProtectUnitTests

DESCRIPTION
  This function is used to test the DRM protect_content API

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_ProtectUnitTests ( IShell* pIShell )
{
  /* This function tests the IDRM_ProtectContent api */

  boolean                                is_content_complete;
  uint8                                  counter;
  uint8                                  content_buffer[160];
  uint8                                  content_buffer_ue[160];
  uint8                                  protect_tests_passed;
  uint8                                  _err_flag;
  uint32                                 content_len;
  uint32                                 content_buf_len;
  uint32                                 content_buf_len_ue;
  uint32                                 offset;
  uint32                                 encrypted_content_len;
  int                                    _errno;
  AEEDRM_LicenseKeyType                  license_key;
  AEEDRM_AddRightRspType                 add_rsp;
  AEEDRM_ConsumeRightRspType             consume_rsp;
  AEEDRM_DeleteAssociatedRightsRspType   delete_rsp;
  AEEDRM_EncryptionInfoType              enc_info;
  AEEDRM_RightsHandleType                rhandle;
  IDRM*                                  piDRM;
  IDCF*                                  piDCF;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  protect_tests_passed = 0;
  is_content_complete = FALSE;
  DRMTestApp_uri_1[0] = '\0';
  counter = 0;
  rhandle = 0;
  _err_flag = 0;
  _errno = EFAILED;
  content_len = 0;
  content_buf_len = 0;
  content_buf_len_ue = 0;
  encrypted_content_len = 0;
  piDRM = NULL;
  piDCF = NULL;
  offset = 0;

  MSG_FATAL ( "***** ProtectUnitTests: BEGIN TEST. *****",
      0,
      0,
      0 );

  do
  {
    if ( ISHELL_CreateInstance( pIShell,
          AEECLSID_IDRM,
          (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    if ( ISHELL_CreateInstance( pIShell,
          AEECLSID_IDCF,
          (void**)(&piDCF) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DCF Object", 0,0,0 );
      break;
    }

    MSG_HIGH (
        "protect_content: Testing the DRM API IDRM_ProtectContent( ... ).",
        0,
        0,
        0 );

    /* Add a new right */
    MSG_HIGH ( "protect_content: Adding a new right.", 0, 0, 0 );
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_9_print_right_1_fwd_lock,
        STRLEN ( DRMTestApp_oma10_uri_9_print_right_1_fwd_lock ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "protect_content: Add #1 failed.  Error #: %d",
          add_rsp.Errno,
          0,
          0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY (
        DRMTestApp_uri_1,
        add_rsp.szContentURI,
        add_rsp.ContentURILen
        );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
    rhandle = add_rsp.RightsHandle;

    /* Setup the content information */
    MSG_HIGH (
        "protect_content: Setting up the content & encryption information.",
        0,
        0,
        0 );
    STRLCPY ( ( char* )content_buffer_ue, "Hello World", sizeof(content_buffer_ue));
    content_len = STRLEN ( (char*) content_buffer_ue ) + 1;
    content_buf_len = 160;
    is_content_complete = TRUE;

    /* Setup the encryption information */
    enc_info.Method = AEEDRM_ENC_METHOD_AES_128_CBC;
    enc_info.Padding = AEEDRM_ENC_PADDING_RFC2630;

    for ( counter = 0 ; counter < AEEDRM_ENCRYPTION_BLOCK_SIZE; counter++ )
    {
      /* TODO -- Find a random number generator for use here -- */
      enc_info.IV[counter] = counter;
    }

    /* The above should only have to be set once */

    MSG_HIGH (
        "protect_content: Content & encryption information setup complete.",
        0,
        0,
        0 );

    /* Protect the content passed in
     * Parameters:
     *
     * No important optional parameters
     */

    MSG_HIGH ( "protect_content: Protecting the content.", 0, 0, 0 );

    /* Encrypt out of place */

    _errno = IDRM_ProtectContent (
        piDRM,
        rhandle,
        content_buffer_ue,
        content_len,
        content_buffer,
        &content_buf_len,
        is_content_complete,
        &enc_info
        );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "protect_content: Unable to protect the content with rights handle: %d.  Error #: %d",
          rhandle,
          _errno,
          0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Content protected.", 0, 0, 0 );
    }

    if ( content_buf_len != 16 )
    {
      MSG_ERROR ( "protect_content: The content length is incorrect.", 0, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Content padded correctly.", 0, 0, 0 );
    }

    MSG_HIGH ( "protect_content: Protect test #1 passed.", 0, 0, 0 );
    protect_tests_passed++;
    /* End test */

    /* Now try to _decrypt_ the content */
    /* We have to try and consume it first... and get the license key */

    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle,
                       AEEDRM_ACTION_PRINT,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Consume failed (%d)!!!", consume_rsp.Errno, 0, 0 );
      break;
    }

    MEMCPY ( license_key.KeyInfo.Value, consume_rsp.LicenseKey.KeyInfo.Value,
             consume_rsp.LicenseKey.KeyInfo.Size );
    license_key.KeyInfo.Size = consume_rsp.LicenseKey.KeyInfo.Size;
    content_buf_len_ue = 160;
    is_content_complete = TRUE;

    /* Setup the encryption information */
    enc_info.Method = AEEDRM_ENC_METHOD_AES_128_CBC;
    enc_info.Padding = AEEDRM_ENC_PADDING_RFC2630;

    for ( counter = 0 ; counter < AEEDRM_ENCRYPTION_BLOCK_SIZE; counter++ )
    {
      /* TODO -- Find a random number generator for use here -- */
      enc_info.IV[counter] = counter;
    }

    /* Call the decrypt API */
    /* Decrypt out of place */
    _errno = IDCF_DecryptContent ( piDCF,
                                  content_buffer,
                                  content_buf_len,
                                  content_buffer_ue,
                                  &content_buf_len_ue,
                                  is_content_complete,
                                  &license_key,
                                  &enc_info
                                );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "ProtectContent: Couldn't decrypt", 0, 0, 0 );
      break;
    }

    /* FIXME -- Wait for implementation */
    if ( content_buf_len_ue != content_len )
    {
      MSG_ERROR ( "ProtectContent: Content length (%d) does not match decrypted length (%d)",
                  content_len,
                  content_buf_len_ue,
                  0 
                );
      break;
    }

    if ( STRCMP ( "Hello World", (char*) content_buffer_ue ) != 0 )
    {
      MSG_ERROR ( "ProtectContent: Couldn't decrypt properly!", 0, 0, 0 );
      break;
    }

    protect_tests_passed++;
    MSG_FATAL ( "ProtectContentTests: Encrypt/Decrypt test #1 passed!", 0, 0, 0 );

    /* Add a new right */
    MSG_HIGH ( "protect_content: Adding a new right.", 0, 0, 0 );
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_9_print_right_2_fwd_lock,
        STRLEN ( DRMTestApp_oma10_uri_9_print_right_2_fwd_lock ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "protect_content: Add #2 failed.  Error #: %d",
          add_rsp.Errno,
          0,
          0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Add #2 success.", 0, 0, 0 );
    }
    rhandle = add_rsp.RightsHandle;

    MEMSET ( content_buffer, 0, 160 );
    MEMSET ( content_buffer_ue, 0, 160 );
    
    /* Check what happens if the content length is a multiple of 16 */
    STRLCPY ( ( char* )content_buffer, "111111111111111",sizeof(content_buffer) );
    content_len = STRLEN ( (char*) content_buffer ) + 1;
    content_buf_len = 160;
    is_content_complete = TRUE;

    /* Protect the content passed in
     * Parameters:
     *
     * No important optional parameters
     */

    MSG_HIGH ( "protect_content: Protecting the content.", 0, 0, 0 );

    _errno = IDRM_ProtectContent (
        piDRM,
        rhandle,
        content_buffer,
        content_len,
        content_buffer,
        &content_buf_len,
        is_content_complete,
        &enc_info
        );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "protect_content: Unable to protect the content with rights handle: %d.  Error #: %d",
          rhandle,
          _errno,
          0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Content protected.", 0, 0, 0 );
    }

    if ( content_buf_len != 32 )
    {
      MSG_ERROR ( "protect_content: The content length is incorrect.", 0, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "protect_content: Content padded correctly.", 0, 0, 0 );
    }

    protect_tests_passed++;
    MSG_HIGH ( "protect_content: Protect test #2 passed.", 0, 0, 0 );

    /* Now try to _decrypt_ the content */
    /* We have to try and consume it first... and get the license key */

    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle,
                       AEEDRM_ACTION_PRINT,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Consume failed (%d)!!!", consume_rsp.Errno, 0, 0 );
      break;
    }

    MEMSET ( &license_key, 0, sizeof ( AEEDRM_LicenseKeyType ) );

    MEMCPY ( license_key.KeyInfo.Value, consume_rsp.LicenseKey.KeyInfo.Value,
             consume_rsp.LicenseKey.KeyInfo.Size );
    license_key.KeyInfo.Size = consume_rsp.LicenseKey.KeyInfo.Size;
    content_buf_len_ue = 160;
    is_content_complete = TRUE;

    /* Setup the encryption information */
    enc_info.Method = AEEDRM_ENC_METHOD_AES_128_CBC;
    enc_info.Padding = AEEDRM_ENC_PADDING_RFC2630;

    for ( counter = 0 ; counter < AEEDRM_ENCRYPTION_BLOCK_SIZE; counter++ )
    {
      /* TODO -- Find a random number generator for use here -- */
      enc_info.IV[counter] = counter;
    }

    /* Call the decrypt API */
    _errno = IDCF_DecryptContent ( piDCF,
                                  content_buffer,
                                  content_buf_len,
                                  content_buffer_ue,
                                  &content_buf_len_ue,
                                  is_content_complete,
                                  &license_key,
                                  &enc_info
                                );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "ProtectContent: Couldn't decrypt", 0, 0, 0 );
      break;
    }

    if ( content_buf_len_ue != content_len )
    {
      MSG_ERROR ( "ProtectContent: Content length (%d) does not match decrypted length (%d)",
                  content_len,
                  content_buf_len_ue,
                  0 
                );
      break;
    }

    if ( STRCMP ( "111111111111111", (char*) content_buffer_ue ) != 0 )
    {
      MSG_ERROR ( "ProtectContent: Couldn't decrypt properly!", 0, 0, 0 );
      break;
    }

    protect_tests_passed++;
    MSG_FATAL ( "ProtectContentTests: Encrypt/Decrypt test #2 passed!", 0, 0, 0 );

    /* Check what happens with multi-part content */
    /* 49 characters */
    MEMSET ( (void*) content_buffer, 1, 48 );
    ( (char*) content_buffer )[48] = '\0';

    MSG_HIGH ( "protect_content: Protecting the content.", 0, 0, 0 );

    counter = 0;
    is_content_complete = FALSE;
    content_buf_len = 160;
    content_len = 16;

    while ( counter <= 48 )
    {
      /* Protect the content passed in
       * Parameters:
       *
       * No important optional parameters
       */

      _errno = IDRM_ProtectContent (
          piDRM,
          rhandle,
          content_buffer + counter,
          content_len,
          content_buffer + counter,
          &content_buf_len,
          is_content_complete,
          &enc_info
          );

      if ( _errno != SUCCESS )
      {
        MSG_ERROR ( "protect_content: Unable to protect the content with rights handle: %d.  Error #: %d",
            rhandle,
            _errno,
            0 );
        _err_flag = 1;
        break;
      }

      counter+=16;
      if ( counter == 48 )
      {
        /* This should only run once */
        content_len = 1;
        is_content_complete = TRUE;
      }
      else
      {
        is_content_complete = FALSE;
      }

      if ( ( is_content_complete == FALSE ) && ( content_buf_len > 16 ) )
      {
        MSG_ERROR ( "protect_content: %d content incorrectly padded: %d.",
            counter,
            content_buf_len,
            0 );
        _err_flag = 1;
        break;
      }

      if ( ( is_content_complete == TRUE ) && ( content_buf_len != 16 ) )
      {
        MSG_ERROR ( "protect_content: Final content incorrectly padded.", 0, 0, 0 );
        _err_flag = 1;
        break;
      }
      content_buf_len = 160 - ( counter );
    }

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "An error occurred while protecting multipart content", 0, 0, 0 );
      break;
    }

    protect_tests_passed++;
    MSG_HIGH ( "protect_content: Protect test #3 passed.", 0, 0, 0 );

    /* Decrypt the content */
    /* We have to try and consume it first... and get the license key */

    IDRM_ConsumeRight( piDRM,
                       NULL,
                       rhandle,
                       AEEDRM_ACTION_PRINT,
                       &consume_rsp,
                       NULL );

    ASSERT( consume_rsp.RspType == AEEDRM_RSP_CONSUME_RIGHT );

    if ( consume_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Consume failed (%d)!!!", consume_rsp.Errno, 0, 0 );
      break;
    }

    MEMSET ( &license_key, 0, sizeof ( AEEDRM_LicenseKeyType ) );

    MEMCPY ( license_key.KeyInfo.Value, consume_rsp.LicenseKey.KeyInfo.Value,
             consume_rsp.LicenseKey.KeyInfo.Size );
    license_key.KeyInfo.Size = consume_rsp.LicenseKey.KeyInfo.Size;
    content_buf_len_ue = 160;
    content_buf_len = 64;
    is_content_complete = TRUE;

    /* Setup the encryption information */
    enc_info.Method = AEEDRM_ENC_METHOD_AES_128_CBC;
    enc_info.Padding = AEEDRM_ENC_PADDING_RFC2630;

    for ( counter = 0 ; counter < AEEDRM_ENCRYPTION_BLOCK_SIZE; counter++ )
    {
      /* TODO -- Find a random number generator for use here -- */
      enc_info.IV[counter] = counter;
    }

    encrypted_content_len = content_buf_len;
    content_buf_len = AEEDRM_ENCRYPTION_BLOCK_SIZE;
    content_buf_len_ue = 0;
    is_content_complete = FALSE;
    offset = 0;

    do
    {
      /* Decrypt in place */
      _errno = IDCF_DecryptContent ( piDCF,
                                     content_buffer + offset,
                                     content_buf_len,
                                     content_buffer + offset,
                                     &content_buf_len,
                                     is_content_complete,
                                     &license_key,
                                     &enc_info
                                   );

      offset += AEEDRM_ENCRYPTION_BLOCK_SIZE;
      content_buf_len_ue += content_buf_len;
      content_buf_len = AEEDRM_ENCRYPTION_BLOCK_SIZE;

      if ( offset == ( encrypted_content_len - AEEDRM_ENCRYPTION_BLOCK_SIZE ) )
      {
        is_content_complete = TRUE;
      }
      else
      {
        is_content_complete = FALSE;
      }

    } while ( ( _errno == SUCCESS ) && ( offset < encrypted_content_len ) );

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "ProtectContent: Couldn't decrypt", 0, 0, 0 );
      break;
    }

    if ( content_buf_len_ue != 49 )
    {
      MSG_ERROR ( "ProtectContent: Content length (%d) does not match decrypted length (%d)",
                  49,
                  content_buf_len_ue,
                  0 
                );
      break;
    }

    protect_tests_passed++;
    MSG_FATAL ( "ProtectContentTests: Encrypt/Decrypt test #3 passed!", 0, 0, 0 );

    MSG_FATAL ( "***** protect_content: TEST PASSED. *****",
        0,
        0,
        0 );
    _errno = SUCCESS;
  } while ( 0 );

  /* Print out a failure message */
  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** protect_content: TEST FAILED. *****",
        0,
        0,
        0 );
  }

  MSG_HIGH ( "protect_content: Deleting all rights objects with URI #1.",
      0,
      0,
      0 );

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
          "protect_content: Error #: %d.  Delete failed.",
          delete_rsp.Errno,
          0,
          0 );
    }
    else
    {
      MSG_HIGH ( "protect_content: Delete succeded.", 0, 0, 0 );
    }
  }
  DRMTestApp_uri_1[0] = '\0';

  IDRM_Release( piDRM );
  IDCF_Release( piDCF );

  return protect_tests_passed;
} /* DRMTestApp_ProtectUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_PROTECT_CONTENT_TESTS

DESCRIPTION
  This function is used to run the protect_content tests and print results.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunProtectContentTests ( IShell* pIShell )
{
  uint8 protect_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  protect_tests_passed += DRMTestApp_ProtectUnitTests ( pIShell );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* PROTECT TESTS: %d/%d TESTS PASSED *",
              protect_tests_passed,
              DRMTestApp_total_protect_tests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
} /* DRMTestApp_RunProtectContentTests */



/*===========================================================================

FUNCTION DRMTestApp_UpdateKnownRight

DESCRIPTION
  This function tries to use the update_right API to update a known right.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of update tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_UpdateKnownRight ( IShell* pIShell )
{
  uint8                          update_tests_passed;
  int                            _errno;
  AEEDRM_AddRightRspType         add_rsp;
  AEEDRM_DeleteRightRspType      delete_rsp;
  AEEDRM_LookupRightRspType      lookup_rsp;
  AEEDRM_RightsHandleType        rhandle;
  AEEDRM_RightsObjectType        robj;
  AEEDRM_UpdateRightsInfoRspType update_rsp;
  IDRM*                          piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  update_tests_passed = 0;
  _errno = EFAILED;
  rhandle = 0;
  DRMTestApp_uri_1[0] = '\0';
  MEMSET ( &robj, 0, sizeof ( robj ) );
  piDRM = NULL;

  MSG_FATAL (
    "***** UpdateKnownRight: BEGIN TEST. *****", 0, 0, 0 );

  do
  {
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    /* Add a new right */
    MSG_HIGH ( "UpdateUnknownRight: Adding a new right.", 0, 0, 0 );
    IDRM_AddRight (
        piDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_oma10_uri_4_play_right_1_sep_del,
        STRLEN ( DRMTestApp_oma10_uri_4_play_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "update_right: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "update_right: Add #1 success.", 0, 0, 0 );
    }

    MEMCPY (
        DRMTestApp_uri_1,
        add_rsp.szContentURI,
        add_rsp.ContentURILen
        );
    DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
    rhandle = add_rsp.RightsHandle;

    /* Parameters:
     *
     * Trying to update the rights object for this rights handle
     * No important optional parameters
     */

    MSG_HIGH (
      "update_right: Updating rights information for object with handle %d",
      rhandle,
      0,
      0 );

    IDRM_UpdateRightsInfo (
        piDRM,
        rhandle,
        &robj,
        &update_rsp,
        NULL
        );

    ASSERT ( update_rsp.RspType == AEEDRM_RSP_UPDATE_RIGHTS_INFO );

    if ( update_rsp.Errno == SUCCESS )
    {
      MSG_ERROR ( "update_right: Incorrectly updated a known right with handle %d.", rhandle, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "update_right: Correctly unable to update a known right.",
                 0,
                 0,
                 0 );
    }

    update_tests_passed++;
    MSG_FATAL ( "***** UpdateKnownRight: TEST PASSED. *****",
                0,
                0,
                0 );
    /* End test */
    _errno = SUCCESS;
  } while ( 0 );

  /* -- TODO Print out a failure message -- */
  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** UpdateKnownRight: TEST FAILED. *****",
                0,
                0,
                0 );
  }

  MSG_HIGH ( "UpdateKnownRight: Deleting all rights objects with URI #1.",
             0,
             0,
             0 );

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteRight (
        piDRM,
        rhandle,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateKnownRight: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "UpdateKnownRight: Delete succeded.",
                 0,
                 0,
                 0 );
    }

    /* Confirm the deletion */
    MSG_HIGH ( "UpdateKnownRight: Confirming that the delete worked.",
               0,
               0,
               0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_HIGH ( "UpdateKnownRight: Lookup failure.  Error #: %d",
                 lookup_rsp.Errno,
                 0,
                 0 );
    }
    else
    {
      MSG_ERROR ( "UpdateKnownRight: Lookup success.  ERROR!!", 0, 0, 0 );
    }
  }
  DRMTestApp_uri_1[0] = '\0';
  
        IDRM_Release( piDRM );
        
        return update_tests_passed;
} /* DRMTestApp_UpdateKnownRight */



/*===========================================================================

FUNCTION DRMTestApp_UpdateUnknownRight

DESCRIPTION
  This function uses the update_right API to update an unknown right.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Number of update tests passed.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_UpdateUnknownRight ( IShell* pIShell )
{
  uint8                                update_tests_passed;
  uint32                               ro_list_size;
  uint32                               num_rights_1;
  uint32                               num_rights_2;
  uint32                               old_info_play;
  uint32                               old_info_display;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_RightsHandleType              rhandle;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_RightsObjectType              robj;
  AEEDRM_UpdateRightsInfoRspType       update_rsp;
  AEEDRM_XactionIdType                 xaction_id_1;
  AEEDRM_XactionIdType                 xaction_id_2;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  STRLCPY ( DRMTestApp_uri_1, "1234@foo.com", sizeof(DRMTestApp_uri_1));
  update_tests_passed = 0;
  rhandle = 0;
  ro_list_size = 0;
  num_rights_1 = 0;
  num_rights_2 = 0;
  old_info_play = 0;
  old_info_display = 0;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  _errno = EFAILED;
  DRMTestApp_uri_2[0] = '\0';
  ro_list = NULL;
  piDRM = NULL;

  do
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "UpdateUnknownRight: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MSG_FATAL ( "***** UpdateUnknownRight: BEGIN_TEST *****",
                0,
                0,
                0 );

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    /* Add a new right */
    MSG_HIGH ( "UpdateUnknownRight: Adding a new right.", 0, 0, 0 );
    IDRM_AddRight (
        piDRM,
        DRMTestApp_uri_1,
        AEEDRM_OMA_1_0_VERSION,
        (uint8*) DRMTestApp_raw_uri_3_execute_right_1_sep_del,
        STRLEN ( DRMTestApp_raw_uri_3_execute_right_1_sep_del ),
        &add_rsp,
        NULL
        );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Add #1 failed.  Error #: %d",
                  add_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: Add #1 success.", 0, 0, 0 );
    }

    rhandle = add_rsp.RightsHandle;

    /* Perform 1st lookup */

    MSG_HIGH ( "UpdateUnknownRight: Performing two lookups.", 0, 0, 0 );
    MSG_HIGH (
      "UpdateUnknownRight: Performing first lookup using handle %d only.",
      rhandle,
      0,
      0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        rhandle,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH ( "UpdateUnknownRight: Lookup #1 success.  num_rights: %d xaction_id: %d", num_rights_1, xaction_id_1, 0 );
    }

    if ( num_rights_1 != 1 )
    {
      MSG_ERROR ( "UpdateUnknownRight: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "UpdateUnknownRight: Lookup #1 returned correct # of rights.",
        0,
        0,
        0 );
    }

    /* Perform second lookup */

    STRLCPY ( DRMTestApp_uri_2, "non_existent_uri@internal.qualcomm.com", sizeof(DRMTestApp_uri_2));

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        rhandle,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH ( "UpdateUnknownRight: Lookup #2 success.  num_rights: %d xaction_id: %d", num_rights_2, xaction_id_2, 0 );
    }

    if ( num_rights_2 != 1 )
    {
      MSG_ERROR ( "UpdateUnknownRight: Lookup #2 returned wrong # of rights.  num_rights: %d xaction_id: %d",
        num_rights_2, xaction_id_2, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "UpdateUnknownRight: Lookup #2 returned correct # of rights.",
        0,
        0,
        0 );
    }

    /* Retrieve info and save for later comparison */
    ro_list_size = 20;

    MSG_HIGH ( "UpdateUnknownRight: Retrieving %d rights into a %d element list using %d xaction_id", num_rights_2, ro_list_size, xaction_id_2 );

    /* Parameters:
     *
     * Retrieve the rights objects based on the results of the _2nd_ lookup
     */

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    MSG_HIGH ( "UpdateUnknownRight: Checking if IDRM_RetrieveRight succeeded and returned the correct # of rights", 0, 0, 0 );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "UpdateUnknownRight: Retrieve #1 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Retrieve #2 was unable to retrieve %d rights with xaction_id %d.  Error #: %d", num_rights_2, xaction_id_2, _errno );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: Retrieve #2 was successful and retrieved %d rights", num_rights_2, 0, 0 );
    }

    if ( ro_list_size != num_rights_2 )
    {
      MSG_ERROR ( "UpdateUnknownRight: List size not modified or an incorrect number of rights received.  num_rights: %d", num_rights_2, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: List size correctly modified.",
                 0,
                 0,
                 0 );
    }

    old_info_play = robj.Constraints.Play.TotalCount;
    old_info_display = robj.Constraints.Display.TotalCount;

    MSG_HIGH (
      "UpdateUnknownRight: play constraint: %d, display constraint: %d",
      old_info_play,
      old_info_display,
      0 );
    MSG_HIGH ( "UpdateUnknownRight: Retrieved old rights information",
               0,
               0,
               0 );

    /* The following assumes that as an application,
     * we've parsed our own proprietary XML and are
     * transferring data to the DRM application via a rights object type
     */

    MSG_HIGH (
      "UpdateUnknownRight: Setting the rights object parameters.",
      0,
      0,
      0 );

    robj.RightsHandle = rhandle;
    robj.DrmVersion = AEEDRM_UNKNOWN_VERSION;
    robj.Constraints.Play.TotalCount = 1;
    robj.Constraints.Display.TotalCount = 2;

    /* -- TODO Can we not update the raw XML string itself?
     * or is it meant to be pretty much inaccessible? -- */

    /* Parameters:
     *
     * No important optional arguments to specify
     */

    /* Update this raw right */

    MSG_HIGH (
      "UpdateUnknownRight: Updating unknown right object with handle %d",
      rhandle,
      0,
      0 );
    IDRM_UpdateRightsInfo (
        piDRM,
        rhandle,
        &robj,
        &update_rsp,
        NULL
        );

    ASSERT ( update_rsp.RspType == AEEDRM_RSP_UPDATE_RIGHTS_INFO );

    if ( update_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Unable to update rights object with handle %d.  Error #: %d", rhandle, update_rsp.Errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "UpdateUnknownRight: Updated rights object with handle %d",
        rhandle,
        0,
        0 );
    }

    /* Retrieve the raw right and confirm that it's been updated */

    ro_list_size = 20;

    MSG_HIGH ( "UpdateUnknownRight: Preparing to retrieve right to confirm it has been updated properly", 0, 0, 0 );

    MSG_HIGH ( "UpdateUnknownRight: Retrieving %d rights into a %d element list using previous lookup results", num_rights_1, ro_list_size, 0 );
    MSG_HIGH ( "UpdateUnknownRight: Preparing to receive %d rights using %d xaction_id", num_rights_1, xaction_id_1, 0 );

    /* Parameters:
     *
     * Retrieve the rights objects based on the results of the _1st_ lookup
     */

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    MSG_HIGH ( "UpdateUnknownRight: Checking if IDRM_RetrieveRight succeeded and returned the correct # of rights", 0, 0, 0 );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "UpdateUnknownRight: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Retrieve #2 was unable to retrieve %d rights with xaction_id %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: Retrieve #2 was successful and retrieved %d rights", num_rights_1, 0, 0 );
    }

    if ( ro_list_size != num_rights_1 )
    {
      MSG_ERROR ( "UpdateUnknownRight: List size not modified or an incorrect number of rights received.  num_rights: %d", num_rights_1, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: List size correctly modified.",
                 0,
                 0,
                 0 );
    }

    if ( ro_list[0].Constraints.Play.TotalCount != 1 || ro_list[0].Constraints.Display.TotalCount != 2 )
    {
      MSG_ERROR ( "UpdateUnknownRight: Right with handle %d and with xaction_id %d not updated.", rhandle, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: Right updated successfully.",
                 0,
                 0,
                 0 );
    }

    /* Clear out variables */
    DRMTestApp_clear_ro_list ( ro_list, ( sizeof ( ro_list ) / sizeof ( AEEDRM_RightsObjectType ) ) );

    update_tests_passed++;

    /* FIXME */
    /* num_tests_retrieve++; */
    /* FIXME */
    MSG_FATAL ( "***** UpdateUnknownRight: TEST PASSED. *****",
                0,
                0,
                0 );
    /* End test */

    _errno = SUCCESS;
  } while ( 0 );

  /* Print out a failure message */
  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** UpdateUnknownRight: TEST FAILED. *****",
                0,
                0,
                0 );
  }

  MSG_HIGH ( "UpdateUnknownRight: Deleting all rights with URI #1.",
             0,
             0,
             0 );

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "UpdateUnknownRight: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "UpdateUnknownRight: Delete succeded.",
                 0,
                 0,
                 0 );
    }
  }

  DRMTestApp_uri_1[0] = '\0';
 
        FREE ( ro_list );
        IDRM_Release( piDRM );

  return update_tests_passed;
} /* DRMTestApp_UpdateUnknownRight */



/*===========================================================================

FUNCTION DRMTestApp_RUN_UPDATE_TESTS

DESCRIPTION
  This function runs all the update_right tests and prints results.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunUpdateTests ( IShell* pIShell )
{
  uint8 update_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  update_tests_passed += DRMTestApp_UpdateKnownRight ( pIShell );
  update_tests_passed += DRMTestApp_UpdateUnknownRight ( pIShell );

  MSG_FATAL ( "************************************", 0, 0, 0 );
  MSG_FATAL ( "*  UPDATE TESTS: %d/%d TESTS PASSED  *",
              update_tests_passed,
              DRMTestApp_total_update_tests,
              0 );
  MSG_FATAL ( "************************************", 0, 0, 0 );
} /* DRMTestApp_run_udpate_tests */



/*===========================================================================

FUNCTION DRMTestApp_runUnitTests

DESCRIPTION
  This function runs all the unit tests.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunUnitTests ( IShell* pIShell )
{
  DRMTestApp_RunAddTests( pIShell );
  DRMTestApp_RunCreateTests ( pIShell );
  DRMTestApp_RunLookupRetrieveTests ( pIShell );
  DRMTestApp_RunDeleteTests( pIShell );
  DRMTestApp_RunUpdateTests( pIShell );
  DRMTestApp_RunConsumeTests(pIShell );
  DRMTestApp_RunCheckTests( pIShell );
  DRMTestApp_RunProtectContentTests(pIShell ); 
} /* DRMTestApp_RunUnitTests */



/*===========================================================================

FUNCTION DRMTestApp_Flow1

DESCRIPTION
  This function tests flow #1.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Integer indicating if the flow succeded.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_Flow1 ( IShell* pIShell )
{
  char*                                r_ptr_locs[10];
  uint8                                flow_tests_passed;
  uint8                                counter;
  uint8                                _err_flag;
  uint32                               ro_list_size;
  uint32                               uri_rights[3];
  uint32                               num_rights_1;
  uint32                               num_rights_2;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_XactionIdType                 xaction_id_1;
  AEEDRM_XactionIdType                 xaction_id_2;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_FATAL ( "***** Flow1: BEGIN TEST. *****", 0, 0, 0 );

  /* Clear variables */
  MSG_HIGH ( "Flow1: Clearing variables.", 0, 0, 0 );

  MEMSET ( r_ptr_locs, 0, sizeof ( r_ptr_locs ) );
  flow_tests_passed = 0;
  counter = 0;
  num_rights_1 = 0;
  num_rights_2 = 0;
  _err_flag = 0;
  ro_list_size = 0;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  DRMTestApp_uri_3[0] = '\0';
  _errno = EFAILED;
  ro_list = NULL;
  piDRM = NULL;

  do
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "Flow1: MALLOC failed.", 0, 0, 0 );
      break;
    }

    MSG_HIGH ( "Flow1: Setting the locations of rights objects.", 0, 0, 0 );

    /* Load pointers for the 10 URIs into the array */
    r_ptr_locs[0]  = DRMTestApp_oma10_uri_4_play_right_1_sep_del;
    r_ptr_locs[1]  = DRMTestApp_oma10_uri_4_play_right_2_sep_del;
    r_ptr_locs[2]  = DRMTestApp_oma10_uri_4_display_right_1_sep_del;
    r_ptr_locs[3]  = DRMTestApp_oma10_uri_4_display_right_2_sep_del;
    r_ptr_locs[4]  = DRMTestApp_oma10_uri_4_display_right_3_sep_del;
    r_ptr_locs[5]  = DRMTestApp_oma10_uri_5_print_right_1_sep_del;
    r_ptr_locs[6]  = DRMTestApp_oma10_uri_5_execute_right_1_sep_del;
    r_ptr_locs[7]  = DRMTestApp_oma10_uri_5_execute_right_2_sep_del;
    r_ptr_locs[8]  = DRMTestApp_oma10_uri_6_execute_right_1_sep_del;
    r_ptr_locs[9]  = DRMTestApp_oma10_uri_6_display_right_1_sep_del;

    /* Set the number of rights/URI */
    MSG_HIGH ( "Flow1: Setting the # of rights/URI.", 0, 0, 0 );

    uri_rights[0]  = 5;
    uri_rights[1]  = 3;
    uri_rights[2]  = 2;

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    MSG_HIGH (
      "Flow1: Adding 10 rights objects.  Please observe state of cache.",
      0,
      0,
      0 );

    /* Now add 10 rights */

    for ( counter = 0; counter < 10; counter++ )
    {
      IDRM_AddRight (
          piDRM,
          NULL,
          AEEDRM_OMA_1_0_VERSION,
          (uint8*) r_ptr_locs[counter],
          STRLEN ( r_ptr_locs[counter] ),
          &add_rsp,
          NULL
          );

      ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

      if ( add_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "Flow1: Add #%d failed.  Error #: %d",
                    counter + 1,
                    add_rsp.Errno,
                    0 );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH ( "Flow1: Add #%d success.", counter + 1, 0, 0 );
      }

      if ( counter == 0 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_1,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 5 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_2,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 8  && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_3,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_3[add_rsp.ContentURILen] = '\0';
      }
    }

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "Flow1: Error occured on Add #%d", counter + 1, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: All adds performed successfully.", 0, 0, 0 );
    }

    /* Do lookup #1 */

    MSG_HIGH ( "Flow1: Looking up URI #1", 0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow1: Lookup #1 success.  num_rights: %d xaction_id: %d",
        num_rights_1,
        xaction_id_1,
        0 );
    }

    if ( num_rights_1 != uri_rights[0] )
    {
      MSG_ERROR ( "Flow1: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: Lookup #1 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do lookup #2 */

    MSG_HIGH ( "Flow1: Looking up URI #2", 0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow1: Lookup #2 success.  num_rights: %d xaction_id: %d",
        num_rights_2,
        xaction_id_2,
        0 );
    }

    if ( num_rights_2 != uri_rights[1] )
    {
      MSG_ERROR ( "Flow1: Lookup #2 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_2, xaction_id_2, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: Lookup #2 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #1 */

    MSG_HIGH ( "Flow1: Retrieving %d rights using previous lookup results",
               num_rights_1,
               0,
               0 );
    MSG_HIGH ( "Flow1: Preparing to receive %d rights using %d xaction_id",
               num_rights_1,
               xaction_id_1,
               0 );

    ro_list_size = num_rights_1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    MSG_HIGH ( "Flow1: Checking if IDRM_RetrieveRight succeeded and returned the correct # of rights", 0, 0, 0 );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR (
      "Flow1: Retrieve #1 was unable to use xaction_id: %d.  Error #: %d",
      xaction_id_1,
      _errno,
      0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Retrieve #1 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow1: Retrieve #1 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[0] )
    {
      MSG_ERROR ( "Flow1: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_1 );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    MSG_HIGH ( "Flow1: Looking up URI #3", 0, 0, 0 );

    /* Do lookup #3 */

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_3,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Lookup #3 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow1: Lookup #3 success.  num_rights: %d xaction_id: %d",
        num_rights_1,
        xaction_id_1,
        0 );
    }

    if ( num_rights_1 != uri_rights[2] )
    {
      MSG_ERROR ( "Flow1: Lookup #3 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: Lookup #3 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #2 */

    MSG_HIGH ( "Flow1: Retrieving %d rights using previous lookup results",
               num_rights_2,
               0,
               0 );
    MSG_HIGH ( "Flow1: Preparing to receive %d rights using %d xaction_id",
               num_rights_2,
               xaction_id_2,
               0 );

    ro_list_size = num_rights_2;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow1: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Retrieve #2 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_2, xaction_id_2, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow1: Retrieve #2 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[1] )
    {
      MSG_ERROR ( "Flow1: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_2 );

    /* Clear variables */
    num_rights_2 = xaction_id_2 = 0;

    /* Do retrieve #3 */

    MSG_HIGH (
      "Flow1: Retrieving %d rights using previous lookup results",
      num_rights_1,
      0,
      0 );
    MSG_HIGH ( "Flow1: Preparing to receive %d rights using %d xaction_id",
               num_rights_1,
               xaction_id_1,
               0 );

    ro_list_size = num_rights_1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow1: Retrieve #3 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Retrieve #3 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow1: Retrieve #3 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[2] )
    {
      MSG_ERROR ( "Flow1: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow1: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_1 );

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    MSG_FATAL ( "***** Flow1: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
    flow_tests_passed++;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** Flow1: TEST FAILED *****", 0, 0, 0 );
  }

  /* Clear out the db */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "Flow1: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow1: Error #: %d.  Delete failed.", delete_rsp.Errno, 0, 0 );
    }
    else
    {
      MSG_HIGH ( "Flow1: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_3[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_3,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow1: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow1: Delete succeded.", 0, 0, 0 );
    }
  }
 
  FREE ( ro_list );
  IDRM_Release( piDRM );

  return flow_tests_passed;
} /* DRMTestApp_Flow1 */



/*===========================================================================

FUNCTION DRMTestApp_Flow2

DESCRIPTION
  This function tests flow #2.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Integer indicating if the flow succeded.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_Flow2 ( IShell* pIShell )
{
  char*                                r_ptr_locs[10];
  uint8                                flow_tests_passed;
  uint8                                counter;
  uint32                               ro_list_size;
  uint32                               uri_rights[3];
  uint32                               num_rights_1;
  uint32                               num_rights_2;
  uint32                               num_rights_3;
  uint32                               _err_flag;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_XactionIdType                 xaction_id_1;
  AEEDRM_XactionIdType                 xaction_id_2;
  AEEDRM_XactionIdType                 xaction_id_3;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_FATAL ( "***** Flow2: BEGIN TEST. *****", 0, 0, 0 );

  MSG_HIGH ( "Flow2: Clearing variables.", 0, 0, 0 );

  MEMSET ( r_ptr_locs, 0, sizeof ( r_ptr_locs ) );
  flow_tests_passed = 0;
  counter = 0;
  num_rights_1 = 0;
  num_rights_2 = 0;
  num_rights_3 = 0;
  _err_flag = 0;
  ro_list_size = 0;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  xaction_id_3 = 0;
  _errno = EFAILED;
  ro_list = NULL;
  piDRM = NULL;
  
  do
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "Flow2: MALLOC failed.", 0, 0, 0 );
      break;
    }

    DRMTestApp_uri_1[0] = '\0';
    DRMTestApp_uri_2[0] = '\0';
    DRMTestApp_uri_3[0] = '\0';

    MSG_HIGH ( "Flow2: Setting the locations of rights objects.", 0, 0, 0 );

    /* Load pointers for the 10 URIs into the array */
    r_ptr_locs[0]  = DRMTestApp_oma10_uri_4_play_right_1_sep_del;
    r_ptr_locs[1]  = DRMTestApp_oma10_uri_4_play_right_2_sep_del;
    r_ptr_locs[2]  = DRMTestApp_oma10_uri_4_display_right_1_sep_del;
    r_ptr_locs[3]  = DRMTestApp_oma10_uri_4_display_right_2_sep_del;
    r_ptr_locs[4]  = DRMTestApp_oma10_uri_4_display_right_3_sep_del;
    r_ptr_locs[5]  = DRMTestApp_oma10_uri_5_print_right_1_sep_del;
    r_ptr_locs[6]  = DRMTestApp_oma10_uri_5_execute_right_1_sep_del;
    r_ptr_locs[7]  = DRMTestApp_oma10_uri_5_execute_right_2_sep_del;
    r_ptr_locs[8]  = DRMTestApp_oma10_uri_6_execute_right_1_sep_del;
    r_ptr_locs[9]  = DRMTestApp_oma10_uri_6_display_right_1_sep_del;

    /* Set the number of rights/URI */
    MSG_HIGH ( "Flow2: Setting the # of rights/URI.", 0, 0, 0 );

    uri_rights[0]  = 5;
    uri_rights[1]  = 3;
    uri_rights[2]  = 2;

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    MSG_HIGH (
      "Flow2: Adding 10 rights objects.  Please observe state of cache.",
      0,
      0,
      0 );

    /* Now add 10 rights */

    for ( counter = 0; counter < 10; counter++ )
    {
      IDRM_AddRight (
          piDRM,
          NULL,
          AEEDRM_OMA_1_0_VERSION,
          (uint8*) r_ptr_locs[counter],
          STRLEN ( r_ptr_locs[counter] ),
          &add_rsp,
          NULL
          );

      ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

      if ( add_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "Flow2: Add #%d failed.  Error #: %d",
                    counter + 1,
                    add_rsp.Errno,
                    0 );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH ( "Flow2: Add #%d success.", counter + 1, 0, 0 );
      }

      if ( counter == 0 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_1,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 5 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_2,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 8  && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_3,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_3[add_rsp.ContentURILen] = '\0';
      }
    }

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "Flow2: Error occured on Add #%d", counter + 1, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: All adds performed successfully.", 0, 0, 0 );
    }

    /* Do lookup #1 */

    MSG_HIGH ( "Flow2: Looking up URI #1", 0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow2: Lookup #1 success.  num_rights: %d xaction_id: %d",
        num_rights_1,
        xaction_id_1,
        0 );
    }

    if ( num_rights_1 != uri_rights[0] )
    {
      MSG_ERROR ( "Flow2: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: Lookup #1 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do lookup #2 */

    MSG_HIGH ( "Flow2: Looking up URI #2", 0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow2: Lookup #2 success.  num_rights: %d xaction_id: %d",
        num_rights_2,
        xaction_id_2,
        0 );
    }

    if ( num_rights_2 != uri_rights[1] )
    {
      MSG_ERROR ( "Flow2: Lookup #2 returned wrong # of rights.  num_rights: %d, xaction_id: %d",
          num_rights_2, xaction_id_2, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: Lookup #2 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #1 */

    MSG_HIGH ( "Flow2: Retrieving %d right using previous lookup results",
               1,
               0,
               0 );
    MSG_HIGH ( "Flow2: Preparing to receive %d right using %d xaction_id",
               1,
               xaction_id_1,
               0 );

    ro_list_size = 1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );


    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow2: Retrieve #1 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno == SUCCESS )
    {
      MSG_ERROR ( "Flow2: Retrieve #1 retrieved too many rights (%d) with xaction_id: %d.  Error #: %d", num_rights_1, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: Retrieve #1 was successful and retrieved %d right.", ro_list_size, 0, 0 );
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ( "Flow2: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, 1 );

    /* Do lookup #3 */

    MSG_HIGH ( "Flow2: Looking up URI #3", 0, 0, 0 );

    IDRM_LookupRight (
          piDRM,
          DRMTestApp_uri_3,
          0x00,
          &lookup_rsp,
          NULL
          );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Lookup #3 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_3 = lookup_rsp.NumRights;
      xaction_id_3 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow2: Lookup #3 success.  num_rights: %d xaction_id: %d",
        num_rights_3,
        xaction_id_3,
        0 );
    }

    if ( num_rights_3 != uri_rights[2] )
    {
      MSG_ERROR ( "Flow2: Lookup #3 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_3, xaction_id_3, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: Lookup #3 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #2 */

    MSG_HIGH ( "Flow2: Retrieving %d right using xaction_id %d",
               1,
               xaction_id_2,
               0 );

    ro_list_size = 1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow2: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    if ( _errno == SUCCESS )
    {
      MSG_ERROR ( "Flow2: Retrieve #2 retrieved too many rights (%d) with xaction_id: %d.  Error #: %d", num_rights_2, xaction_id_2, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow2: Retrieve #2 was successful and retrieved %d right.",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ( "Flow2: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, 1 );

    /* Do retrieve #3 */

    MSG_HIGH ( "Flow2: Retrieving %d rights using xaction_id %d",
            num_rights_3,
            xaction_id_3,
            0 );

    ro_list_size = num_rights_3;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_3,
                ro_list,
                &ro_list_size
                );

if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow2: Retrieve #3 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_3, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Retrieve #3 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_3, xaction_id_3, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow2: Retrieve #3 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[2] )
    {
      MSG_ERROR ( "Flow2: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_3 );

    /* Clear variables */
    num_rights_3 = xaction_id_3 = 0;

    /* Complete retrieve #1 */

    MSG_HIGH ( "Flow2: Retrieving %d rights using xaction_id %d", num_rights_1 - 1, xaction_id_1, 0 );

    ro_list_size = num_rights_1 - 1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );


    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow2: Retrieve #1 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Retrieve #1 retrieved %d rights with xaction_id: %d.  Error #: %d", ro_list_size, xaction_id_1, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow2: Retrieve #1 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[0] - 1 )
    {
      MSG_ERROR ( "Flow2: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, uri_rights[0] - 1 );

    /* Clear variables */
    xaction_id_1 = num_rights_1 = 0;

    /* Complete retrieve #2 */

    MSG_HIGH ( "Flow2: Retrieving %d rights using xaction_id %d",
               num_rights_2 - 1,
               xaction_id_2,
               0 );

    ro_list_size = num_rights_2 - 1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow2: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow2: Retrieve #2 retrieved %d rights with xaction_id: %d.  Error #: %d", ro_list_size, xaction_id_2, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow2: Retrieve #2 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[1] - 1 )
    {
      MSG_ERROR ( "Flow2: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow2: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_2  - 1 );

    /* Clear variables */
    xaction_id_2 = num_rights_2 = 0;

    MSG_FATAL ( "***** Flow2: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
    flow_tests_passed++;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR ( "***** Flow2: TEST FAILED *****", 0, 0, 0 );
  }

  /* Clear out the db */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow2: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow2: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow2: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow2: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_3[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_3,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow2: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow2: Delete succeded.", 0, 0, 0 );
    }
  }
 
        FREE ( ro_list );
        IDRM_Release( piDRM );

  return flow_tests_passed;
} /* DRMTestApp_Flow2 */



/*===========================================================================

FUNCTION DRMTestApp_Flow3

DESCRIPTION
  This function tests flow #3.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  Integer indicating if the flow succeded.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_Flow3 ( IShell* pIShell )
{
  char*                                r_ptr_locs[14];
  uint8                                flow_tests_passed;
  uint8                                _err_flag;
  uint8                                counter;
  uint32                               ro_list_size;
  uint32                               uri_rights[3];
  uint32                               num_rights_1;
  uint32                               num_rights_2;
  uint32                               num_rights_3;
  uint32                               num_rights_4;
  int                                  _errno;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsObjectType*             ro_list;
  AEEDRM_XactionIdType                 xaction_id_1;
  AEEDRM_XactionIdType                 xaction_id_2;
  AEEDRM_XactionIdType                 xaction_id_3;
  AEEDRM_XactionIdType                 xaction_id_4;
  IDRM*                                piDRM;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_FATAL ( "***** Flow3: BEGIN TEST. *****", 0, 0, 0 );

  MSG_HIGH ( "Flow3: Clearing variables.", 0, 0, 0 );

  MEMSET ( r_ptr_locs, 0, sizeof ( r_ptr_locs ) );
  counter = 0;
  flow_tests_passed = 0;
  num_rights_1 = 0;
  num_rights_2 = 0;
  num_rights_3 = 0;
  num_rights_4 = 0;
  _err_flag = 0;
  ro_list_size = 0;
  xaction_id_1 = 0;
  xaction_id_2 = 0;
  xaction_id_3 = 0;
  xaction_id_4 = 0;
  _errno = EFAILED;
  ro_list = NULL;
  piDRM = NULL;

  do
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "Flow3: MALLOC failed.", 0, 0, 0 );
      break;
    }

    DRMTestApp_uri_1[0] = '\0';
    DRMTestApp_uri_2[0] = '\0';
    DRMTestApp_uri_3[0] = '\0';

    MSG_HIGH ( "Flow3: Setting the locations of rights objects.", 0, 0, 0 );

    /* Load pointers for the 10 URIs into the array */
    r_ptr_locs[0]  = DRMTestApp_oma10_uri_4_play_right_1_sep_del;
    r_ptr_locs[1]  = DRMTestApp_oma10_uri_4_play_right_2_sep_del;
    r_ptr_locs[2]  = DRMTestApp_oma10_uri_4_play_right_3_sep_del;
    r_ptr_locs[3]  = DRMTestApp_oma10_uri_4_display_right_1_sep_del;
    r_ptr_locs[4]  = DRMTestApp_oma10_uri_4_display_right_2_sep_del;
    r_ptr_locs[5]  = DRMTestApp_oma10_uri_4_display_right_3_sep_del;
    r_ptr_locs[6]  = DRMTestApp_oma10_uri_4_print_right_1_sep_del;
    r_ptr_locs[7]  = DRMTestApp_oma10_uri_4_print_right_2_sep_del;
    r_ptr_locs[8]  = DRMTestApp_oma10_uri_5_print_right_1_sep_del;
    r_ptr_locs[9]  = DRMTestApp_oma10_uri_5_execute_right_1_sep_del;
    r_ptr_locs[10]  = DRMTestApp_oma10_uri_5_execute_right_2_sep_del;
    r_ptr_locs[11]  = DRMTestApp_oma10_uri_5_execute_right_3_sep_del;
    r_ptr_locs[12]  = DRMTestApp_oma10_uri_6_execute_right_1_sep_del;
    r_ptr_locs[13]  = DRMTestApp_oma10_uri_6_display_right_1_sep_del;

    /* Set the number of rights/URI */
    MSG_HIGH ( "Flow3: Setting the # of rights/URI.", 0, 0, 0 );

    uri_rights[0] = 8;
    uri_rights[1] = 4;
    uri_rights[2] = 2;

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    MSG_HIGH ( "Flow3: Adding 14 rights objects.", 0, 0, 0 );

    /* Now add 14 rights */

    for ( counter = 0; counter < 14; counter++ )
    {
      IDRM_AddRight (
          piDRM,
          NULL,
          AEEDRM_OMA_1_0_VERSION,
          (uint8*) r_ptr_locs[counter],
          STRLEN ( r_ptr_locs[counter] ),
          &add_rsp,
          NULL
          );

      ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

      if ( add_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "Flow3: Add #%d failed.  Error #: %d",
                    counter + 1,
                    add_rsp.Errno,
                    0 );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH ( "Flow3: Add #%d success.", counter + 1, 0, 0 );
      }

      if ( counter == 0 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_1,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 8 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_2,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 12 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_3,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_3[add_rsp.ContentURILen] = '\0';
      }
    }

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "Flow3: Error occured on Add #%d", counter + 1, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: All adds performed successfully.", 0, 0, 0 );
    }

    /* Do lookup #1 */

    MSG_HIGH ( "Flow3: Looking up URI #1",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow3: Lookup #1 success.  num_rights: %d xaction_id: %d",
        num_rights_1,
        xaction_id_1,
        0 );
    }

    if ( num_rights_1 != uri_rights[0] )
    {
      MSG_ERROR ( "Flow3: Lookup #1 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: Lookup #1 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do lookup #2 */

    MSG_HIGH ( "Flow3: Looking up URI #2",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow3: Lookup #2 success.  num_rights: %d xaction_id: %d",
        num_rights_2,
        xaction_id_2,
        0 );
    }

    if ( num_rights_2 != uri_rights[1] )
    {
      MSG_ERROR ( "Flow3: Lookup #2 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_2, xaction_id_2, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: Lookup #2 returned correct # of rights.",
      0,
      0,
      0 );
    }

    /* Do retrieve #1 - Should return AEEDRM_ENOTFOUND */

    MSG_HIGH ( "Flow3: Retrieving %d rights using xaction_id %d",
               num_rights_1,
               xaction_id_1,
               0 );

    ro_list_size = num_rights_1;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_1,
                ro_list,
                &ro_list_size
                );

    if ( _errno != AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow3: Retrieve #1 was able to use this xaction_id: %d. Cache error.  Error #: %d", xaction_id_1, _errno, 0 );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow3: Retrieve #1 was unable to use this xaction_id: %d.",
        xaction_id_1,
        0,
        0 );
    }

    /* Clear variables */
    num_rights_1 = xaction_id_1 = 0;

    /* Do lookup #3 */
    MSG_HIGH ( "Flow3: Looking up URI #3",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_3,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Lookup #3 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_3 = lookup_rsp.NumRights;
      xaction_id_3 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow3: Lookup #3 success.  num_rights: %d xaction_id: %d",
        num_rights_3,
        xaction_id_3,
        0 );
    }

    if ( num_rights_3 != uri_rights[2] )
    {
      MSG_ERROR ( "Flow3: Lookup #3 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_3, xaction_id_3, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: Lookup #3 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #2 */
    MSG_HIGH ( "Flow3: Retrieving %d rights using xaction_id %d",
               num_rights_2,
               xaction_id_2,
               0 );

    ro_list_size = num_rights_2;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_2,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow3: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_2, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Retrieve #2 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_2, xaction_id_2, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow3: Retrieve #2 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[1] )
    {
      MSG_ERROR ( "Flow3: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_2 );

    /* Clear variables */
    num_rights_2 = xaction_id_2 = 0;

    /* Do retrieve #3 */
    MSG_HIGH ( "Flow3: Retrieving %d rights using xaction_id %d",
               num_rights_3,
               xaction_id_3,
               0 );

    ro_list_size = num_rights_3;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_3,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow3: Retrieve #3 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_3, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Retrieve #3 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_3, xaction_id_3, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow3: Retrieve #3 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[2] )
    {
      MSG_ERROR ( "Flow3: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: List size correctly modified.", 0, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_3 );

    /* Clear variables */
    num_rights_3 = xaction_id_3 = 0;


    /* Do lookup #4 */
    MSG_HIGH ( "Flow3: Looking up URI #1",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Lookup #4 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_4 = lookup_rsp.NumRights;
      xaction_id_4 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow3: Lookup #4 success.  num_rights: %d xaction_id: %d",
        num_rights_4,
        xaction_id_4, 0 );
    }

    if ( num_rights_4 != uri_rights[0] )
    {
      MSG_ERROR ( "Flow3: Lookup #4 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_4, xaction_id_4, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: Lookup #4 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do retrieve #4 */
    MSG_HIGH ( "Flow3: Retrieving %d rights using xaction_id %d",
               num_rights_4,
               xaction_id_4,
               0 );

    ro_list_size = num_rights_4;

    _errno = IDRM_RetrieveRight (
                  piDRM,
                  xaction_id_4,
                  ro_list,
                  &ro_list_size
                  );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow3: Retrieve #4 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_4, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow3: Retrieve #4 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_4, xaction_id_4, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow3: Retrieve #4 was successful and retrieved %d rights",
        ro_list_size,
        0,
        0 );
    }

    if ( ro_list_size != uri_rights[0] )
    {
      MSG_ERROR ( "Flow3: List size modified or an incorrect number of rights received.  num_rights: %d", ro_list_size, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow3: List size correctly modified.", 0, 0, 0 );
    }

    DRMTestApp_clear_ro_list ( ro_list, ro_list_size );

    /* Clear variables */
    xaction_id_4 = num_rights_4 = 0;

     MSG_FATAL ( "***** Flow3: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
    flow_tests_passed++;

    /* Test completed */
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_HIGH ( "***** Flow3: TEST FAILED *****", 0, 0, 0 );
  }

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow3: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow3: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow3: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow3: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_3[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_3,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow3: Error #: %d.  Delete failed.",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow3: Delete succeded.", 0, 0, 0 );
    }
  }
 
        FREE ( ro_list );
        IDRM_Release( piDRM );

  return flow_tests_passed;
} /* DRMTestApp_Flow3 */



/*===========================================================================

FUNCTION DRMTestApp_Flow4

DESCRIPTION
  This function tests flow #4.  This flow is described below:

  Add RO #1 for URI #1
  Add RO #2 for URI #1
  Add RO #3 for URI #1
  Add RO #4 for URI #1
  Add RO #5 for URI #1
  Add RO #6 for URI #2
  Add RO #7 for URI #2
  Add RO #8 for URI #2
  Lookup URI #1 returns XID #1 ( num rights 5 )
  Lookup URI #2 returns XID #2
  Add RO #9 for URI #1 returns RH #xx1
  Lookup RH #xx1 returns XID #3
  Retrieve RO using XID #3
  Lookup URI #1 returns XID #4 ( num rights 6 )
  Retrieve using XID #4
  Retrieve using XID #3

DEPENDENCIES
  Does not Return.

RETURN VALUE
  1               Flow test was successful
  0               Flow test was unsuccessful

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_Flow4 ( IShell*       pIShell )
{
  char*                                r_ptr_locs[8];
  uint8                                flow_tests_passed = 0;
  uint8                                counter = 0;
  uint8                                _err_flag = 0;
  uint32                               ro_list_size = 0;
  uint32                               uri_rights[2];
  uint32                               num_rights_1 = 0;
  uint32                               num_rights_2 = 0;
  uint32                               num_rights_3 = 0;
  uint32                               num_rights_4 = 0;
  int                                  _errno = EFAILED;
  AEEDRM_AddRightRspType               add_rsp;
  AEEDRM_DeleteAssociatedRightsRspType delete_rsp;
  AEEDRM_LookupRightRspType            lookup_rsp;
  AEEDRM_RightsHandleType              rhandle_1 = 0;
  AEEDRM_RightsObjectType*             ro_list = NULL;
  AEEDRM_XactionIdType                 xaction_id_1 = 0;
  AEEDRM_XactionIdType                 xaction_id_2 = 0;
  AEEDRM_XactionIdType                 xaction_id_3 = 0;
  AEEDRM_XactionIdType                 xaction_id_4 = 0;
  IDRM*                                piDRM = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_FATAL ( "***** Flow4: BEGIN TEST. *****", 0, 0, 0 );

  MSG_HIGH ( "Flow4: Clear variables.", 0, 0, 0 );

  MEMSET ( r_ptr_locs, 0, sizeof ( r_ptr_locs ) );

  DRMTestApp_uri_1[0] = '\0';
  DRMTestApp_uri_2[0] = '\0';
  DRMTestApp_uri_3[0] = '\0';

  MSG_HIGH ( "Flow4: Setting the locations of rights objects.", 0, 0, 0 );

  /* Load pointers for the 10 URIs into the array */
  r_ptr_locs[0]  = DRMTestApp_oma10_uri_4_play_right_1_sep_del;
  r_ptr_locs[1]  = DRMTestApp_oma10_uri_4_play_right_2_sep_del;
  r_ptr_locs[2]  = DRMTestApp_oma10_uri_4_display_right_1_sep_del;
  r_ptr_locs[3]  = DRMTestApp_oma10_uri_4_display_right_2_sep_del;
  r_ptr_locs[4]  = DRMTestApp_oma10_uri_4_display_right_3_sep_del;
  r_ptr_locs[5]  = DRMTestApp_oma10_uri_5_print_right_1_sep_del;
  r_ptr_locs[6]  = DRMTestApp_oma10_uri_5_execute_right_1_sep_del;
  r_ptr_locs[7]  = DRMTestApp_oma10_uri_5_execute_right_2_sep_del;

  /* Set the number of rights/URI */
  MSG_HIGH ( "Flow4: Setting the # of rights/URI.", 0, 0, 0 );

  uri_rights[0]  = 5;
  uri_rights[1]  = 3;

  do
  {
    ro_list = (AEEDRM_RightsObjectType*) MALLOC 
                  ( 20 * sizeof ( AEEDRM_RightsObjectType ) );

    if ( ro_list == NULL )
    {
      MSG_ERROR ( "Flow4: MALLOC failed.", 0, 0, 0 );
      break;
    }

    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      MSG_ERROR("Can't create a DRM Object", 0,0,0 );
      break;
    }

    MSG_HIGH (
      "Flow4: Adding 8 rights objects.  Please observe state of cache.",
      0,
      0,
      0 );

    /* Now add 8 rights */

    for ( counter = 0; counter < 8; counter++ )
    {
      IDRM_AddRight (
          piDRM,
          NULL,
          AEEDRM_OMA_1_0_VERSION,
          (uint8*) r_ptr_locs[counter],
          STRLEN ( r_ptr_locs[counter] ),
          &add_rsp,
          NULL
          );

      ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

      if ( add_rsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "Flow4: Add #%d failed.  Error #: %d",
                    counter + 1,
                    add_rsp.Errno,
                    0 );
        _err_flag = 1;
        break;
      }
      else
      {
        MSG_HIGH ( "Flow4: Add #%d success.", counter + 1, 0, 0 );
      }

      if ( counter == 0 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_1,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_1[add_rsp.ContentURILen] = '\0';
      }
      else if ( counter == 5 && _err_flag != 1 )
      {
        MEMCPY (
          DRMTestApp_uri_2,
          add_rsp.szContentURI,
          add_rsp.ContentURILen
          );
        DRMTestApp_uri_2[add_rsp.ContentURILen] = '\0';
      }
    }

    if ( _err_flag == 1 )
    {
      MSG_ERROR ( "Flow4: Error occured on Add #%d", counter + 1, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow4: All adds performed successfully.", 0, 0, 0 );
    }

    /* Do lookup #1 */

    MSG_HIGH ( "Flow4: Looking up URI #1",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Lookup #1 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_1 = lookup_rsp.NumRights;
      xaction_id_1 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow4: Lookup #1 success.  num_rights: %d xaction_id: %d",
        num_rights_1,
        xaction_id_1,
        0 );
    }

    if ( num_rights_1 != uri_rights[0] )
    {
      MSG_ERROR ( "Flow4: Lookup #1 returned wrong # of rights.", 0, 0, 0 );
      MSG_ERROR ( "Flow4: num_rights: %d xaction_id: %d",
          num_rights_1, xaction_id_1, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow4: Lookup #1 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Do lookup #2 */

    MSG_HIGH ( "Flow4: Looking up URI #2", 0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_2,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Lookup #2 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_2 = lookup_rsp.NumRights;
      xaction_id_2 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow4: Lookup #2 success.  num_rights: %d xaction_id: %d",
        num_rights_2,
        xaction_id_2,
        0 );
    }

    if ( num_rights_2 != uri_rights[1] )
    {
      MSG_ERROR ( "Flow4: Lookup #2 returned wrong # of rights.", 0, 0, 0 );
      MSG_ERROR ( "Flow4: num_rights: %d xaction_id: %d",
          num_rights_2, xaction_id_2, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow4: Lookup #2 returned correct # of rights.",
                 0,
                 0,
                 0 );
    }

    /* Add an additional right for URI #1 */

    IDRM_AddRight (
          piDRM,
          NULL,
          AEEDRM_OMA_1_0_VERSION,
          (uint8*) DRMTestApp_oma10_uri_4_display_right_3_sep_del,
          STRLEN ( DRMTestApp_oma10_uri_4_display_right_3_sep_del ),
          &add_rsp,
          NULL
          );

    ASSERT( add_rsp.RspType == AEEDRM_RSP_ADD_RIGHT );

    if ( add_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Add failed.  Error #: %d", add_rsp.Errno, 0, 0 );
      break;
    }
    else
    {
      MSG_HIGH ( "Flow4: Add success.", 0, 0, 0 );
    }

    rhandle_1 = add_rsp.RightsHandle;
    uri_rights[0]++;

    /* Do lookup #3 */

    IDRM_LookupRight (
        piDRM,
        NULL,
        rhandle_1,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Lookup #3 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_3 = lookup_rsp.NumRights;
      xaction_id_3 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow4: Lookup #3 success.  num_rights: %d xaction_id: %d",
        num_rights_3,
        xaction_id_3,
        0 );
    }

    if ( num_rights_3 != 1 )
    {
      MSG_ERROR ( "Flow4: Lookup #3 returned wrong # of rights.", 0, 0, 0 );
      MSG_ERROR ( "Flow4: num_rights: %d xaction_id: %d",
          num_rights_3, xaction_id_3, 0 );
      break;
    }

    /* Do retrieve #3 */

    MSG_HIGH ( "Flow4: Retrieving %d right using xaction_id %d",
               num_rights_3,
               xaction_id_3,
               0 );

    ro_list_size = num_rights_3;

    _errno = IDRM_RetrieveRight (
                  piDRM,
                  xaction_id_3,
                  ro_list,
                  &ro_list_size
                  );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow4: Retrieve #1 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_3, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Retrieve #1 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_3, xaction_id_3, _errno );
      break;
    }

    if ( ro_list_size != 1 )
    {
      MSG_ERROR ( "Flow4: List size incorrect.  num_rights: %d",
                  ro_list_size,
                  0,
                  0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_3 );

    /* Do lookup #4 */

    MSG_HIGH ( "Flow4: Looking up URI #1",0, 0, 0 );

    IDRM_LookupRight (
        piDRM,
        DRMTestApp_uri_1,
        0x00,
        &lookup_rsp,
        NULL
        );

    ASSERT( lookup_rsp.RspType == AEEDRM_RSP_LOOKUP_RIGHT );

    if ( lookup_rsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "Flow4: Lookup #4 failure.  Error #: %d",
                  lookup_rsp.Errno,
                  0,
                  0 );
      break;
    }
    else
    {
      num_rights_4 = lookup_rsp.NumRights;
      xaction_id_4 = lookup_rsp.XactionID;

      MSG_HIGH (
        "Flow4: Lookup #4 success.  num_rights: %d xaction_id: %d",
        num_rights_4,
        xaction_id_4,
        0 );
    }

    if ( num_rights_4 != 6 )
    {
      MSG_ERROR ( "Flow4: Lookup #4 returned wrong # of rights.  num_rights: %d xaction_id: %d",
          num_rights_4, xaction_id_4, 0 );
      break;
    }

    /* Do retrieve #2 */

    MSG_HIGH ( "Flow4: Retrieving %d rights using previous lookup results",
               num_rights_4,
               xaction_id_4,
               0 );

    ro_list_size = num_rights_4;

    _errno = IDRM_RetrieveRight (
                piDRM,
                xaction_id_4,
                ro_list,
                &ro_list_size
                );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR ( "Flow4: Retrieve #2 was unable to use this xaction_id: %d.  Error #: %d", xaction_id_4, _errno, 0 );
      break;
    }

    if ( _errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow4: Retrieve #2 was unable to retrieve %d rights with xaction_id: %d.  Error #: %d", num_rights_4,
        xaction_id_4, _errno );
      break;
    }

    if ( ro_list_size != uri_rights[0] )
    {
      MSG_ERROR ( "Flow4: # ro incorrect: %d", ro_list_size, 0, 0 );
      break;
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_4 );

    /* Clear variables */
    num_rights_4 = xaction_id_4 = 0;

    /* Do retrieve again on XID #3 */
    MSG_HIGH ( "Flow4: Retrieving %d rights using xaction_id %d",
               num_rights_3,
               xaction_id_3,
               0 );

    ro_list_size = num_rights_3;

    _errno = IDRM_RetrieveRight (
              piDRM,
              xaction_id_3,
              ro_list,
              &ro_list_size
              );

    if ( _errno == AEEDRM_ENOTFOUND )
    {
      MSG_ERROR (
        "Flow4: Retrieve #3 unable to use xaction_id: %d.  Error #: %d",
        xaction_id_3,
        _errno,
        0 );
      break;
    }

    if ( (_errno != SUCCESS)
         &&
         ( ro_list_size != 0 )
       )
    {
      MSG_ERROR (
        "Flow4: Retrieve #3 got %d ro with xaction_id: %d.  Error #: %d",
        num_rights_3,
        xaction_id_3, _errno );
      break;
    }
    else
    {
      MSG_HIGH (
        "Flow4: Retrieve #3 unable to retrieve %d rights",
        ro_list_size, 0, 0 );
    }

    /* TODO -- Check if the returned objects are the correct ones */

    DRMTestApp_clear_ro_list ( ro_list, num_rights_3 );

    /* Clear variables */
    num_rights_3 = xaction_id_3 = 0;

    /* Test passed */
    MSG_FATAL (
      "***** Flow4: TEST PASSED *****", 0, 0, 0 );
    _errno = SUCCESS;
    flow_tests_passed++;
  } while ( 0 );

  if ( _errno != SUCCESS )
  {
    MSG_ERROR (
      "***** Flow4: TEST FAILED *****", 0, 0, 0 );
  }

  /* Clear out the db */

  if ( DRMTestApp_uri_1[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_1,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow4: Failed to delete all rights objects. Error #: %d",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow4: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_2[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_2,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow4: Failed to delete all rights objects. Error #: %d",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH (
        "Flow4: Delete succeded.", 0, 0, 0 );
    }
  }

  if ( DRMTestApp_uri_3[0] != '\0' )
  {
    IDRM_DeleteAssociatedRights (
        piDRM,
        DRMTestApp_uri_3,
        &delete_rsp,
        NULL
        );

    ASSERT ( delete_rsp.RspType == AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS );

    if ( delete_rsp.Errno != SUCCESS )
    {
      MSG_ERROR (
        "Flow4: Failed to delete all rights objects. Error #: %d",
        delete_rsp.Errno,
        0,
        0 );
    }
    else
    {
      MSG_HIGH ( "Flow4: Delete succeded.", 0, 0, 0 );
    }
  }

  FREE ( ro_list );
  IDRM_Release( piDRM );

  return flow_tests_passed;
} /* DRMTestApp_Flow4 */



/*===========================================================================

FUNCTION DRMTestApp_RunFlowTests

DESCRIPTION
  This function runs all the flow tests and displays their results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunFlowTests ( IShell* pIShell )
{
  uint8 flow_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  flow_tests_passed += DRMTestApp_Flow1 ( pIShell );
  flow_tests_passed += DRMTestApp_Flow2 ( pIShell );
  flow_tests_passed += DRMTestApp_Flow3 ( pIShell );
  flow_tests_passed += DRMTestApp_Flow4 ( pIShell );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "*  FLOW TESTS: %d/%d TESTS PASSED   *",
              flow_tests_passed,
              DRMTestApp_total_flow_tests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
} /* DRMTestApp_RunFlowTests */



/*===========================================================================

FUNCTION DRMTestApp_ContentHandlingFlow

DESCRIPTION
  This function runs unit tests of the create/parse APIs and the
  protect/decrypt APIs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 DRMTestApp_ContentHandlingFlow ( IShell* pIShell )
{
  char                                pContent[50];
  char                                content_mime_type[20];
  char                                content_uri[50];
  char                                ContentReturned[50];
  int                                 Errno = SUCCESS;
  int                                 pad_len = 0;
  uint8*                              dcf_buffer = NULL;
  uint8*                              temp_ptr =  NULL;
  uint8                               chf_tests_passed = 0;
  uint32                              content_data_len = 0;
  uint32                              dcf_buffer_len = 0;
  IDCF*                               piDCF = NULL;
  IDRM*                               piDRM = NULL;
  AEEDRM_EncryptionInfoType           content_enc_info;
  AEEDCF_HeaderType                   dcf_header;

  
  MSG_FATAL ( "***** ContentHandlingFlow: BEGIN TEST *****", 0, 0, 0 );
  do
  {
    // Get the DCF and DRM instance
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDCF,
                                (void**)(&piDCF) ) != SUCCESS )
    {
      Errno = EFAILED;
      MSG_ERROR ( "Can't create a DCF Object", 0, 0, 0 );
      break;
    }
    
    if ( ISHELL_CreateInstance( pIShell,
                                AEECLSID_IDRM,
                                (void**)(&piDRM) ) != SUCCESS )
    {
      Errno = EFAILED;
      MSG_ERROR ( "Can't create a DRM Object", 0, 0, 0 );
      break;
    }

    dcf_buffer = (uint8*) MALLOC ( 2048 );
    
    if ( !dcf_buffer )
    {
      Errno = EFAILED;
      MSG_ERROR ( "ContentHandlingFlow: MALLOC failed.", 0, 0, 0 );
      break;
    }
    dcf_buffer_len = 2048;

    // Create some content
    STRLCPY ( pContent, "Hi!  I'm creating some EXTRA special content", sizeof(pContent));
    content_data_len = STRLEN ( pContent ) + 1;
    STRLCPY ( content_mime_type, "text/html", sizeof(content_mime_type));
    STRLCPY ( content_uri, "chf@internal.qualcomm.com", sizeof(content_uri));
    
    // Now, attempt to create a header for this content
    Errno = IDCF_CreateContentHeader ( piDCF,
                                       content_uri,
                                       content_mime_type,
                                       content_data_len,
                                       &content_enc_info,
                                       dcf_buffer,
                                       &dcf_buffer_len
                                      );
    if ( Errno == EBADPARM )
    {
      MSG_ERROR ( "ContentHandlingFlow: Bad parameters specified.", 0, 0, 0 );
      break;
    }

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "ContentHandlingFlow: Error %d encountered.", Errno, 0, 0 );
      break;
    }

    /* Assume the content is encrypted and append it to the Header */
    temp_ptr = dcf_buffer;
    temp_ptr += dcf_buffer_len;
    MEMCPY( temp_ptr,
            pContent,
            content_data_len );
    
    pad_len = AEEDRM_ENCRYPTION_BLOCK_SIZE - ( content_data_len %
        AEEDRM_ENCRYPTION_BLOCK_SIZE );
                                  
    /* Update dcf_buffer_len */
    dcf_buffer_len +=  content_data_len + pad_len;
                                  
    // Now attempt to parse the newly returned header;
    Errno = IDCF_ParseContentHeader ( piDCF,
                                      dcf_buffer,
                                      dcf_buffer_len,
                                      &dcf_header
                                     );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "ContentHandlingFlow: Unable to parse header", 0, 0, 0 );
      break;
    }

    if ( dcf_header.Version != AEEDRM_OMA_1_0_VERSION )
    {
      Errno = EFAILED;
      MSG_ERROR ( "ContentHandlingFlow: Version incorrect.", 0, 0, 0 );
      break;
    }

    if ( dcf_header.szContentURI != NULL && content_uri != NULL )
    {
      if ( STRCMP ( dcf_header.szContentURI, content_uri ) != 0 )
      {
        MSG_ERROR ( "ContentHandlingFlow: The content URI was copied incorrectly.",
                    0,
                    0,
                    0
                  );
        break;
      }

      if ( STRCMP ( dcf_header.szContentMIMEType, content_mime_type ) != 0 )
      {
        MSG_ERROR ( "ContentHandlingFlow: The mime type was copied incorrectly.",
                    0,
                    0,
                    0
                  );
        break;
      }
    }
    else
    {
      Errno = EFAILED;
      MSG_ERROR ( "ContentHandlingFlow: The URIs are NULL.", 0, 0, 0 );
      break;
    }
    
    if ( dcf_header.ContentDataLen != content_data_len + pad_len )
    {
      Errno = EFAILED;
      MSG_ERROR ( "ContentHandlingFlow: The content data length is wrong", 0, 0, 0 );
      break;
    }

    /* Check if we have pointer to the content */
    MEMCPY( ContentReturned,
            dcf_header.pContentData,
            dcf_header.ContentDataLen );
    if ( MEMCMP( ContentReturned,
                 pContent,
                 content_data_len )
         != 0 )
    {
      Errno = EFAILED;
      MSG_ERROR("Content corrupted", 0, 0, 0 );
      break;
    }


    chf_tests_passed = 1;

    MSG_FATAL ( "***** ContentHandlingFlow: TEST PASSED *****", 0, 0, 0 );
  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** ContentHandlingFlow: TEST FAILED *****", 0, 0, 0 );
  }

  // Delete any created stuff...

  FREE( dcf_buffer );
  IDCF_Release ( piDCF );
  IDRM_Release ( piDRM );
  return chf_tests_passed;
}



/*===========================================================================

FUNCTION DRMTestApp_RunContentHandleTests

DESCRIPTION
  This function runs all the flow tests and displays their results.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunContentHandleTests ( IShell* pIShell )
{
  uint8 content_tests_passed = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  content_tests_passed += DRMTestApp_ContentHandlingFlow ( pIShell );
  
  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* CONTENT TESTS: %d/%d TESTS PASSED *",
              content_tests_passed,
              1,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
} /* DRMTestApp_RunContentHandleTests */



/*===========================================================================

FUNCTION DRMTestApp_LoadDB

DESCRIPTION
  This function loads the DB with 100+ rights and performs a lookup

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_LoadDB
        ( IShell*                         pIShell,
          uint32*                         pLoadDBTestsPassed,
          uint32*                         pTotalLoadDBTests
        )
{
  /* Variable declarations */
  IDRM*                                   pIDRM = NULL;
  AEEDRM_PermissionTemplateType           PermissionTemplate;
  AEEDRM_AddRightRspType                  AddRsp;
  AEEDRM_LookupRightRspType               LookupRsp;
  AEEDRM_DeleteAssociatedRightsRspType    DelAssocRsp;
  uint8*                                  pOMARight = NULL;
  uint32                                  OMARightLength = 0;
  uint32                                  Counter = 0;
  int                                     Errno = SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /*
   * URI used for this test
   */
#define DB_STRESS_TEST_URI "db_stress_test@foo.com"

  /*
   * Number of rights we want to load the DB with
   */
#define DB_STRESS_TEST_NUM_RIGHTS 100


  /*
   * Loop as long as the functionality isn't complete and there are no errors
   */
  do
  {
    /*
     * We want to load the DB with 100 rights for the same URI and perform a
     * lookup for this URI
     */



    /*
     * The first step in dealing with playing a media object is
     * get a handle to the DRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Memset the PermissionTemplate to 0 (to clear it) and set the
     * display/print counts
     */
    MEMSET ( &PermissionTemplate, 0, sizeof ( AEEDRM_PermissionTemplateType ) );

    PermissionTemplate.Display.TotalCount = 5;
    PermissionTemplate.Display.IsValid = TRUE;
    PermissionTemplate.Print.TotalCount = 7;
    PermissionTemplate.Display.IsValid = TRUE;



    /*
     * Create the right
     */

    /*
     * Allocate memory for the right and set the size
     */
    pOMARight = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARight == NULL )
    {
      MSG_ERROR ( "FAIL: Can't allocate %d bytes.", AEEDRM_XML_BUFFER_SIZE,
          0, 0 );
      Errno = EFAILED;
      break;
    }
    OMARightLength = AEEDRM_XML_BUFFER_SIZE;

    Errno = IDRM_CreateRight
              ( pIDRM,
                DB_STRESS_TEST_URI,
                PermissionTemplate,
                pOMARight,
                &OMARightLength
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create right.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now add the right multiple times to the database
     */
    do
    {



      IDRM_AddRight
          ( pIDRM,
            DB_STRESS_TEST_URI,
            AEEDRM_OMA_1_0_VERSION,
            pOMARight,
            OMARightLength,
            &AddRsp,
            NULL
          );

      if ( AddRsp.Errno != SUCCESS )
      {
        Errno = AddRsp.Errno;
        MSG_ERROR ( "FAIL: Add failed.  Error #: %d.", Errno, 0, 0 );
      }


      Counter++;
    } while ( Counter < DB_STRESS_TEST_NUM_RIGHTS
              &&
              Errno == SUCCESS
            );

    /*
     * Check error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Add # %d failed.  Error #: %d", Counter + 1, Errno,
          0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Perform a lookup
     */
    MSG_FATAL ( "Lookup started.", 0, 0, 0 );

    IDRM_LookupRight
      ( pIDRM,
        DB_STRESS_TEST_URI,
        0x00,
        &LookupRsp,
        NULL
      );

    MSG_FATAL ( "Lookup ended.", 0, 0, 0 );

    if ( LookupRsp.Errno == SUCCESS )
    {
      MSG_ERROR ( "FAIL: Copied 100 records.  Error #: %d.",
         LookupRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }




  } while ( 0 );



  *pTotalLoadDBTests = 1;

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** DRMTestApp_LoadDB: TEST FAILED *****", 0, 0, 0 );
    *pLoadDBTestsPassed = 0; 
  }
  else
  {
    MSG_FATAL ( "***** DRMTestApp_LoadDB: TEST PASSED *****", 0, 0, 0 );
    *pLoadDBTestsPassed = 1;
  }

  /*
   * Delete all 100 rights
   */
  IDRM_DeleteAssociatedRights
    ( pIDRM,
      DB_STRESS_TEST_URI,
      &DelAssocRsp,
      NULL
    );

  if ( DelAssocRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete rights.", 0, 0, 0 );
    Errno = EFAILED;
  }

  FREE ( pOMARight );
  IDRM_Release ( pIDRM );


  
}



/*===========================================================================

FUNCTION DRMTestApp_SimulateFieldDBLoad

DESCRIPTION
  This function loads the DB with 100+ rights and performs a lookup
  There are 20 URIS with 5 rights/URI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_SimulateFieldDBLoad
        ( IShell*                         pIShell,
          uint32*                         pSimFieldDBTestsPassed,
          uint32*                         pTotalSimFieldDBTests
        )
{
#define SIM_NUM_DB_URIS 20
#define SIM_NUM_DB_RIGHTS 100
  /* Variable declarations */
  IDRM*                                   pIDRM = NULL;
  AEEDRM_PermissionTemplateType           PermissionTemplate;
  AEEDRM_AddRightRspType                  AddRsp;
  AEEDRM_LookupRightRspType               LookupRsp;
  AEEDRM_DeleteAssociatedRightsRspType    DelAssocRsp;
  uint8*                                  pOMARight = NULL;
  uint32                                  OMARightLength = 0;
  uint32                                  Counter = 0;
  uint32                                  DBURICounter = 0;
  int                                     Errno = SUCCESS;
  int                                     StrLCatErr = 0;
  char*                                   URIArray [ SIM_NUM_DB_URIS ];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /*
   * Loop as long as the functionality isn't complete and there are no errors
   */
  do
  {
    /*
     * We want to load the DB with 5 rights/URI for 20 URIs and perform
     * lookups for these URIs
     */



    /*
     * The first step in dealing with playing a media object is
     * get a handle to the DRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MEMSET ( URIArray, 0, sizeof ( char* ) * SIM_NUM_DB_URIS );



    /*
     * Allocate memory for each of the 20 URIs as long as there are no errors
     *
     * After we allocate the memory, we create all the 20 URIs
     */
    do
    {



      URIArray [ Counter ] = (char*) MALLOC ( AEEDRM_MAX_URI_LEN );
      if ( URIArray [ Counter ] == NULL )
      {
        MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
            AEEDRM_MAX_URI_LEN, 0, 0 );
        Errno = ENOMEMORY;
      }

      /*
       * If we have memory in which to create the URI
       */
      if ( Errno != ENOMEMORY )
      {

        /*
         * STRLCAT uses the null terminator to start appending
         */
        URIArray [ Counter ] [0] = '\0';
        
        /*
         * Append the first part of the URI
         */
        StrLCatErr = STRLCAT ( URIArray [ Counter ], "db_test_",
            AEEDRM_MAX_URI_LEN );

        if ( StrLCatErr != -1 )
        {
          /*
           * Append a two digit # (01, 02 .... 20) to the db_test_ part
           */
          SNPRINTF ( &( URIArray [ Counter ] [ STRLEN ( "db_test_" ) ] ),
                     AEEDRM_MAX_URI_LEN - STRLEN ( URIArray [ Counter ] ),
                     "%02d",
                     Counter + 1
                   );
          
          /*
           * Concatenate the last part
           */
          StrLCatErr = STRLCAT ( URIArray [ Counter ], "@uri.com",
              AEEDRM_MAX_URI_LEN );
        }

        if ( StrLCatErr == -1 )
        {
          Errno = EFAILED;
        }
        else
        {
          Counter++;
        }

      }/* endif */



    } while ( ( Counter < SIM_NUM_DB_URIS )
              &&
              ( Errno == SUCCESS )
            );

    /*
     * If there was an error concatenating the string or allocating memory
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't create URI #%d.",
          Counter + 1, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Memset the PermissionTemplate to 0 (to clear it) and set the
     * display/print counts
     */
    MEMSET ( &PermissionTemplate, 0, sizeof ( AEEDRM_PermissionTemplateType ) );

    PermissionTemplate.Display.TotalCount = 5;
    PermissionTemplate.Display.IsValid = TRUE;
    PermissionTemplate.Print.TotalCount = 7;
    PermissionTemplate.Display.IsValid = TRUE;



    /*
     * Create and add 100 rights
     */

    /*
     * Allocate memory for the right and set the size
     */
    pOMARight = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARight == NULL )
    {
      MSG_ERROR ( "FAIL: Can't allocate %d bytes.", AEEDRM_XML_BUFFER_SIZE,
          0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset the counter
     */
    Counter = 0;
    DBURICounter = 0;

    /*
     * First time
     */
    ( (char*) pOMARight )[0] = '\0';
    OMARightLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Create the right using the next URI in the series
     */
    Errno = IDRM_CreateRight
      ( pIDRM,
        URIArray [ DBURICounter ],
        PermissionTemplate,
        pOMARight,
        &OMARightLength
      );

    /*
     * Check error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create right.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    do
    {


      /*
       * Every 5 rights, I have to create a new rights object using the next
       * URI in the series
       */
      if ( ( ( Counter + 1 ) % 5 ) == 0 )
      {
        /*
         * Reset the OMA rights object parameters
         */
        ( (char*) pOMARight )[0] = '\0';
        OMARightLength = AEEDRM_XML_BUFFER_SIZE;
        DBURICounter++;

        /*
         * Create the right using the next URI in the series
         */
        Errno = IDRM_CreateRight
          ( pIDRM,
            URIArray [ DBURICounter ],
            PermissionTemplate,
            pOMARight,
            &OMARightLength
          );

        /*
         * Check error conditions
         */
        if ( Errno != SUCCESS )
        {
          MSG_ERROR ( "FAIL: Can't create right.  Error #: %d",
              Errno, 0, 0 );
          Errno = EFAILED;
          break;
        }


      }

      /*
       * Add the right
       *
       * Each URI has 5 rights added for it
       */
      IDRM_AddRight
        ( pIDRM,
          URIArray [ DBURICounter ],
          AEEDRM_OMA_1_0_VERSION,
          pOMARight,
          OMARightLength,
          &AddRsp,
          NULL
        );

      /*
       * Check error conditions
       */
      if ( AddRsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Can't add right #%d.", Counter, 0, 0 );
        Errno = EFAILED;
        break;
      }

      Counter ++;



    } while ( Counter < SIM_NUM_DB_RIGHTS );

    /*
     * Check error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't add right #%d.",
          Counter + 1, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Reset the counters
     */
    Counter = 0;
    DBURICounter = 0;
    Errno = SUCCESS;


    /*
     * Do 20 lookups ( each lookup should return 5 rights )
     *
     * Continue looping as long as there are no errors
     */

    do
    {


      ( *pTotalSimFieldDBTests ) ++;
      /*
       * Perform lookup
       */
      MSG_FATAL ( "Lookup URI #%d.", DBURICounter + 1, 0, 0 );
      IDRM_LookupRight
        ( pIDRM,
          URIArray [ DBURICounter ],
          0x00,
          &LookupRsp,
          NULL
        );
      MSG_FATAL ( "Finished lookup URI #%d.", DBURICounter + 1, 0, 0 );

      /*
       * Check error conditions
       */
      if ( LookupRsp.Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Couldn't lookup URI #%d.",
            DBURICounter + 1, 0, 0 );
        Errno = EFAILED;
      }
      else
      {
        MSG_FATAL ( "Number of rights: %d.  Xaction ID: %d.",
            LookupRsp.NumRights, LookupRsp.XactionID, 0 );

        ( *pSimFieldDBTestsPassed ) ++;
        DBURICounter ++;
      }



    } while ( ( Errno == SUCCESS )
              &&
              ( DBURICounter < SIM_NUM_DB_URIS )
            );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: URI Lookup failed.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** SIMULATE DB LOAD TESTS FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** SIMULATE DB LOAD TESTS PASSED *****", 0, 0, 0 );
  }
  


  /*
   * Free rights & memory associated with each URI
   */
  for ( Counter = 0; Counter < SIM_NUM_DB_URIS; Counter++ )
  {


    /*
     * Delete the five rights for each URI
     */
    IDRM_DeleteAssociatedRights
      ( pIDRM,
        URIArray [ Counter ],
        &DelAssocRsp,
        NULL
      );

    /*
     * Check error conditions
     */
    if ( DelAssocRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't delete rights for URI #%d.",
          Counter + 1, 0, 0 );
      Errno = EFAILED;
    }

    /*
     * Now, free the memory associated with the URI
     */
    FREE ( URIArray [ Counter ] );



  }

  FREE ( pOMARight );

  IDRM_Release ( pIDRM );



}



/*===========================================================================

FUNCTION DRMTestApp_RunLoadDBTests

DESCRIPTION
  This function runs all the LoadDB tests

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunLoadDBTests ( IShell* pIShell )
{
  /* Variable declarations */
  uint32                                      TestsPassed = 0;
  uint32                                      TotalTests = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*DRMTestApp_LoadDB ( pIShell, &TestsPassed, &TotalTests );*/
  DRMTestApp_SimulateFieldDBLoad ( pIShell, &TestsPassed, &TotalTests );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "*  LDDB TESTS: %d/%d TESTS PASSED   *",
              TestsPassed,
              TotalTests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_TestUintvar

DESCRIPTION
  This function tests if the uintvar encoders and decoders are working
  correctly

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_TestUintvar
        ( uint32*                       pUintvarTestsPassed,
          uint32*                       pTotalUintvarTests
        )
{
  /* Variable declarations */
  int                                   Errno = SUCCESS;
  uint8*                                pUintvarBuffer = NULL;
  uint32                                UintvarBufferLength = 0;
  uint32                                DecodedUintvarLength = 0;
  uint32                                DecodedValue = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define UINTVAR_BUFFER_LENGTH 6

#define UINTVAR_1 43
#define UINTVAR_1_LENGTH 1

#define UINTVAR_2 10673
#define UINTVAR_2_LENGTH 2

#define UINTVAR_3 2097151
#define UINTVAR_3_LENGTH 3

#define UINTVAR_4 268000034
#define UINTVAR_4_LENGTH 4

#define UINTVAR_5 4294960295LL
#define UINTVAR_5_LENGTH 5

#define UINTVAR_6 0
#define UINTVAR_6_LENGTH 1



  /*
   * Do while the functionality isn't complete and there are no errors
   */
  do
  {



    /*
     * Yeah, yeah...I know this can be done in a loop
     *
     * I realized this the moment I started to cut/paste.  I'm opting for this
     * approach because then I guess you see exactly each test.
     *
     * But it can be made into a loop (two arrays, elements set to the
     * #defines...)
     */



    /*
     * Allocate memory for the uintvar buffer
     */
    pUintvarBuffer = (uint8*) MALLOC ( UINTVAR_BUFFER_LENGTH );
    if ( pUintvarBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          UINTVAR_BUFFER_LENGTH, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;


    /*
     * Test # 1
     *
     * Encode and decode UINTVAR_1
     */

    *pTotalUintvarTests = 1;

    /*
     * Encode
     */
    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_1
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_1, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_1_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_1_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %u.  Error #: %d.",
          UINTVAR_1, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_1 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_1,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_1_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_1_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #1.", 0, 0, 0 );
    *pUintvarTestsPassed = 1;



    /*
     * Test #2
     *
     * Encode and decode UINTVAR_2
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_2
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_2, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_2_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_2_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %u.  Error #: %d.",
          UINTVAR_2, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_2 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_2,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_2_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_2_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #2.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Test #3
     *
     * Encode and decode UINTVAR_3
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_3
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_3, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_3_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_3_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %u.  Error #: %d.",
          UINTVAR_3, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_3 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_3,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_3_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_3_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #3.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Test #4
     *
     * Encode and decode UINTVAR_4
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_4
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_4, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_4_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_4_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %u.  Error #: %d.",
          UINTVAR_4, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_4 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_4,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_4_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_4_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #4.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Test #5
     *
     * Encode and decode UINTVAR_5
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_5
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_5, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_5_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_5_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %u.  Error #: %d.",
          UINTVAR_5, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_5 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_5,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_5_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_5_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #5.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Test #6
     *
     * Encode and decode UINTVAR_6
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                UINTVAR_6
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't encode %u into a uintvar.  Error #: %d.",
          UINTVAR_6, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( UintvarBufferLength != UINTVAR_6_LENGTH )
    {
      MSG_ERROR ( "FAIL: Incorrect buffer length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_6_LENGTH,
          UintvarBufferLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Decode
     */
    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                &DecodedUintvarLength,
                &DecodedValue
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decode %d.  Error #: %d.",
          UINTVAR_6, Errno, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedValue != UINTVAR_6 )
    {
      MSG_ERROR ( "FAIL: Incorrectly decoded uintvar.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %u.  Received: %u.", UINTVAR_6,
          DecodedValue, 0 );
      Errno = EFAILED;
      break;
    }

    if ( DecodedUintvarLength != UINTVAR_6_LENGTH )
    {
      MSG_ERROR ( "FAIL: Decoded uintvar has wrong length.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Expected: %d.  Received: %d.", UINTVAR_6_LENGTH,
          DecodedUintvarLength, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Reset everything
     */
    MEMSET ( pUintvarBuffer, 0, UINTVAR_BUFFER_LENGTH );
    UintvarBufferLength = UINTVAR_BUFFER_LENGTH;
    DecodedValue = 0;

    /*
     * Passed the test
     */
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #6.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Now start testing failure cases
     */

    /*
     * Error case #1:
     *
     * NULL buffers
     */

    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( NULL,
                &UintvarBufferLength,
                43
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Encode function didn't detect NULL buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #7.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;

    /*
     * Next test
     */

    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                NULL,
                43
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Encode function didn't detect NULL buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #8.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;

    /*
     * Next test
     */

    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_DecodeUintvar
              ( NULL,
                0,
                &DecodedValue
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Decode function didn't detect NULL buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #9.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;

    /*
     * Next test
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                NULL,
                0
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Decode function didn't detect NULL buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #10.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;

    /*
     * Next test
     */
    ( *pTotalUintvarTests )++;

    Errno = OEMDCF_DecodeUintvar
              ( pUintvarBuffer,
                0,
                NULL
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Decode function didn't detect NULL buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #11.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Error case #2:
     * 
     * Bad buffer length
     */

    ( *pTotalUintvarTests )++;

    UintvarBufferLength = 0;

    Errno = OEMDCF_EncodeUintvar
              ( pUintvarBuffer,
                &UintvarBufferLength,
                43
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: Encode function didn't detect bad buffer length.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #12.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



    /*
     * Error case #3:
     *
     * Uintvar is too long
     */

    ( *pTotalUintvarTests )++;
    /*
     * Set it so that the continue bit is always on
     */
    MEMSET ( pUintvarBuffer, 0x80, UINTVAR_BUFFER_LENGTH );

    Errno = OEMDCF_DecodeUintvar
                ( pUintvarBuffer,
                  &DecodedUintvarLength,
                  &DecodedValue
                );

    if ( Errno != EFAILED )
    {
      MSG_ERROR ( "FAIL: Decode function decoded too large a buffer.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Returned code: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * We wanted it to return EFAILED, but now that it did, reset it so that
     * it doesn't trip the wrong closing message
     */
    Errno = SUCCESS;
    MSG_FATAL ( "PASS: Passed uintvar encode/decode test #13.", 0, 0, 0 );
    ( *pUintvarTestsPassed )++;



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** UINTVAR ENCODE/DECODE TESTS FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** UINTVAR ENCODE/DECODE TESTS PASSED *****", 0, 0, 0 );
  }

  FREE ( pUintvarBuffer );

}



/*===========================================================================

FUNCTION DRMTestApp_RunUintvarTests

DESCRIPTION
  This function runs all the uintvar tests

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunUintvarTests ( void )
{
  /* Variable declarations */
  uint32                                      TestsPassed = 0;
  uint32                                      TotalTests = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  DRMTestApp_TestUintvar ( &TestsPassed, &TotalTests );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "*  UINT TESTS: %d/%d TESTS PASSED   *",
              TestsPassed,
              TotalTests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_TestEncryptionHeader

DESCRIPTION
  This function tests out the OEMDCF_ParseEncryptionHeader

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_TestEncryptionHeader
        ( uint32*                       pEncrHdrTestsPassed,
          uint32*                       pTotalEncrHdrTests
        )
{
  /* Variable declarations */
  int                                   Errno = SUCCESS;
  boolean                               Continue = TRUE;
  uint32                                Counter = 0;
  uint32                                HeaderParsed = 0;
  AEEDRM_EncryptionInfoType             EncryptionInfo;
  /*
   * "Encryption-Method:AES128CBCpadding=RFC2630",
   */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /*
   * Do as long as the functionality is not complete and there are no errors
   */
  do
  {



    /*
     * Basic sanity checks
     */
    if ( pTotalEncrHdrTests == NULL
         ||
         pEncrHdrTestsPassed == NULL
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }

    *pTotalEncrHdrTests = 0;
    *pEncrHdrTestsPassed = 0;



    /*
     * This series of tests will loop through all the invalid encryption
     * headers and ensure that the parser correctly returns EFAILED
     */
    do
    {



      /*
       * Start each test off
       *
       * Each test in this series should fail
       */
      ( *pTotalEncrHdrTests )++;

      Errno = OEMDCF_ParseEncryptionHeader
                ( (uint8*) &( InvalidEncrHdrArray [ Counter ][0] ),
                  STRLEN ( InvalidEncrHdrArray [ Counter ] ),
                  &HeaderParsed,
                  &EncryptionInfo
                );

      if ( Errno != SUCCESS )
      {
        ( *pEncrHdrTestsPassed )++;
        Continue = TRUE;
        Counter++;
      }
      else
      {
        MSG_ERROR ( "FAIL: OEMDCF_ParseEncryptionHeader parsed header #%d.",
            Counter + 1, 0, 0 );
        Continue = FALSE;
      }



    } while ( ( Counter < NUM_INVALID_ENCR_HDRS )
              &&
              ( Continue == TRUE )
            );

    if ( Continue == FALSE )
    {
      MSG_ERROR ( "FAIL: Couldn't complete invalid header test suite.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Reset everything
     */
    Counter = 0;
    Continue = TRUE;
    Errno = SUCCESS;



    /*
     * This series of tests will loop through all the valid encryption headers
     * and ensure that the parser correctly returns SUCCESS
     */
    do
    {



      /*
       * Start each test off
       *
       * Each test in this series should pass
       */
       ( *pTotalEncrHdrTests )++;

      Errno = OEMDCF_ParseEncryptionHeader
                ( (uint8*) &( ValidEncrHdrArray [ Counter ][0] ),
                  STRLEN ( ValidEncrHdrArray [ Counter ] ),
                  &HeaderParsed,
                  &EncryptionInfo
                );

      if ( Errno == SUCCESS )
      {
        ( *pEncrHdrTestsPassed )++;
        MSG_FATAL ( "Header Length: %d.  Length parsed: %d.",
            STRLEN ( ValidEncrHdrArray [ Counter ] ),
            HeaderParsed,
            0 );
        Continue = TRUE;
        Counter++;
      }
      else
      {
        MSG_ERROR ( "FAIL: Can't parse header # %d.", Counter + 1, 0, 0 );
        Continue = FALSE;
      }



    } while ( ( Counter < NUM_VALID_ENCR_HDRS )
              &&
              ( Continue == TRUE )
            );

    if ( Continue == FALSE )
    {
      MSG_ERROR ( "FAIL: Couldn't complete valid header test suite.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * FIXME
     * Hmmm...do a test where the encryption header is longer than
     * buffer length
     */
    ( *pTotalEncrHdrTests )++;
    Errno = OEMDCF_ParseEncryptionHeader
      ( (uint8*) &( ValidEncrHdrArray [0][0] ),
        ( STRLEN ( ValidEncrHdrArray [0] ) / 2 ),
        &HeaderParsed,
        &EncryptionInfo
      );

    if ( Errno != SUCCESS )
    {
      ( *pEncrHdrTestsPassed )++;
    }
    else
    {
      MSG_ERROR ( "FAIL: OEMDCF_ParseEncryptionHeader parsed\
          a header that was too small.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now its time for me to test out the arguments
     */

    /*
     * First of this test series
     */
    ( *pTotalEncrHdrTests )++;

    Errno = OEMDCF_ParseEncryptionHeader
              ( NULL,
                1,
                &HeaderParsed,
                &EncryptionInfo
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: OEMDCF_ParseEncryptionHeader accepted NULL buffer.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    ( *pEncrHdrTestsPassed )++;

    /*
     * Second of this test series
     */
    ( *pTotalEncrHdrTests )++;

    Errno = OEMDCF_ParseEncryptionHeader
              ( (uint8*) &( ValidEncrHdrArray[0][0] ),
                1,
                NULL,
                &EncryptionInfo
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: OEMDCF_ParseEncryptionHeader accepted NULL buffer.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    ( *pEncrHdrTestsPassed )++;

    /*
     * Third of this test series
     */
    ( *pTotalEncrHdrTests )++;

    Errno = OEMDCF_ParseEncryptionHeader
              ( (uint8*) &( ValidEncrHdrArray[0][0] ),
                1,
                &HeaderParsed,
                NULL
              );

    if ( Errno != EBADPARM )
    {
      MSG_ERROR ( "FAIL: OEMDCF_ParseEncryptionHeader accepted NULL buffer.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    ( *pEncrHdrTestsPassed )++;
    
    /*
     * Since the last test should have ended in an error, we reset the Errno
     * variable since we want the correct result to be printed
     */
    Errno = SUCCESS;



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** ENCRYPTION HEADER TESTS FAILED *****", 0, 0, 0 );
    MSG_ERROR ( "***** LAST ERROR REPORTED: ERROR #%d *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** ENCRYPTION HEADER TESTS PASSED *****", 0, 0, 0 );
  }



}



/*===========================================================================

FUNCTION DRMTestApp_RunEncryptionHeaderTests

DESCRIPTION
  This function runs all the encryption header tests

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunEncryptionHeaderTests ( void )
{
  /* Variable declarations */
  uint32                                      TestsPassed = 0;
  uint32                                      TotalTests = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  DRMTestApp_TestEncryptionHeader ( &TestsPassed, &TotalTests );

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* EHEAD TESTS: %d/%d TESTS PASSED *",
              TestsPassed,
              TotalTests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_UseContentToolkitFile

DESCRIPTION
  This function uses .dcf and .dm files created by the Content toolkit
  to check if our stuff works

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_UseContentToolkitFile ( IShell* pIShell )
{
  /* Variable declarations */
  IFileMgr*                                   pIFileMgr = NULL;
  IFile*                                      pIFile = NULL;
  IDRM*                                       pIDRM = NULL;
  IDCF*                                       pIDCF = NULL;
  int                                         Errno = SUCCESS;
  uint8*                                      pOMARight = NULL;
  uint32                                      OMARightLength = 0;
  uint8*                                      pBuffer = NULL;
  uint32                                      BufferLength = 0;
  uint32                                      ContentDataLen = 0;
  FileInfo                                    OpenedFileInfo;
  AEEDRM_AddRightRspType                      AddRsp;
  AEEDRM_RightsHandleType                     RightsHandle = 0;
  AEEDRM_DeleteRightRspType                   DeleteRsp;
  AEEDRM_ConsumeRightRspType                  ConsumeRsp;
  AEEDRM_ActionEnumType                       Action = 0x00;
  boolean                                     IsContentComplete = TRUE;
  int                                         BytesRead = 0;
  int                                         BytesWritten = 0;
  AEEDCF_HeaderType                           ParsedDCFHeader;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define CONTENT_FILE_DIR "DRMFiles"
#define CONTENT_DCF_FILENAME "DRMFiles/cpy-encry.dcf"
#define CONTENT_SEPARATE_DELIVERY_FILENAME "DRMFiles/cpy-right.dr"
#define CONTENT_DECRYPTED_FILENAME "Media/cpy.gif"



  /*
   * Do while functionality isn't complete and there are no errors
   */
  do
  {



    /*
     * Create a handle for the IFileMgr interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Create a handle for the IDRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Create a handle for the IDCF interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDCF, ( void** )(&pIDCF) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DCF interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Check if the directory in which all the .dm/.dcf files are supposed to
     * be exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                CONTENT_FILE_DIR
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Directory with DRM rights doesn't exist.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Now check if the combined rights file exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                CONTENT_DCF_FILENAME
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Combined delivery file doesn't exist.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Now check if the separate delivery file exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                CONTENT_SEPARATE_DELIVERY_FILENAME
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Separate delivery file doesn't exist.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * This is a little simplistic in that it wants both files to be available
     * before it proceeds with the test.  I could have designed this
     * differently in that if the combined rights file doesn't exist, it only
     * runs through the separate delivery test and vice versa.
     *
     * I have consciously decided not to do this.
     */



    /*
     * Now that we know that both files do exist, we will first try to add the
     * separate delivery rights to the database
     */

    /*
     * First open the separate delivery Content file for reading
     */
    pIFile = IFILEMGR_OpenFile
              ( pIFileMgr,
                CONTENT_SEPARATE_DELIVERY_FILENAME,
                _OFM_READ
              );

    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't open separate delivery file for read.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get information on the file so we can figure out how many bytes to read
     */
    Errno = IFILE_GetInfo
              ( pIFile,
                &OpenedFileInfo
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't get file information.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * The next step to do that is to allocate space for the buffer into
     * which the OMA rights will be read
     */
    pOMARight = ( uint8* ) MALLOC ( OpenedFileInfo.dwSize );
    if ( pOMARight == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          OpenedFileInfo.dwSize, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightLength = OpenedFileInfo.dwSize;

    /*
     * Read information from the separate delivery file
     */
    BytesRead = IFILE_Read
                  ( pIFile,
                    pOMARight,
                    OMARightLength
                  );

    if ( BytesRead == 0 )
    {
      MSG_ERROR ( "FAIL: Can't read data from file.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Close this file - we don't need it anymore
     */
    IFILE_Release ( pIFile );

    /*
     * Now that we have the right...lets add it to the DB
     */
    IDRM_AddRight
      ( pIDRM,
        NULL,
        AEEDRM_OMA_1_0_VERSION,
        pOMARight,
        OMARightLength,
        &AddRsp,
        NULL
      );

    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't add Content separate delivery right.", 0, 0,
          0 );
      MSG_ERROR ( "FAIL: Error #: %d.", AddRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    RightsHandle = AddRsp.RightsHandle;

    MSG_FATAL ( "PASS: Correctly added Content separate delivery right.",
        0, 0, 0 );



    /*
     * Now...let's attempt to deal with the .dcf file
     */
    pIFile = IFILEMGR_OpenFile
              ( pIFileMgr,
                CONTENT_DCF_FILENAME,
                _OFM_READ
              );

    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't open separate delivery file for read.",
          0, 0, 0 );
      MSG_ERROR ( "FAIL: Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get information on the file so we can figure out how many bytes to read
     */
    Errno = IFILE_GetInfo
              ( pIFile,
                &OpenedFileInfo
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't get file information.", 0, 0, 0 );
      MSG_ERROR ( "FAIL: Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * The next step to do that is to allocate space for the buffer into
     * which the .dcf file contents will be read
     */
    pBuffer = ( uint8* ) MALLOC ( OpenedFileInfo.dwSize );
    if ( pBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          OpenedFileInfo.dwSize, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    BufferLength = OpenedFileInfo.dwSize;

    /*
     * Read information from the separate delivery file
     */
    BytesRead = IFILE_Read
                  ( pIFile,
                    pBuffer,
                    BufferLength
                  );

    if ( BytesRead == 0 )
    {
      MSG_ERROR ( "FAIL: Can't read data from file.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Close this file - we don't need it anymore
     */
    IFILE_Release ( pIFile );

    /*
     * Try to parse the DCF header for information
     */
    Errno = IDCF_ParseContentHeader
              ( pIDCF,
                pBuffer,
                BufferLength,
                &ParsedDCFHeader
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't parse the .dcf file.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Try to decrypt the content
     */
    Action = AEEDRM_ACTION_DISPLAY;

    /*
     * Consume the right so that you can get the license key
     */
    IDRM_ConsumeRight
      ( pIDRM,
        NULL,
        RightsHandle,
        Action,
        &ConsumeRsp,
        NULL
      );

    if ( ConsumeRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't consume right.  Error #: %d.",
          ConsumeRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    IsContentComplete = TRUE;
    ContentDataLen = ParsedDCFHeader.ContentDataLen;

    /*
     * We have the IV, encryption information and license key, so we can
     * decrypt the content
     */
    MSG_FATAL ( "NOTE: Decryption started.", 0, 0, 0 );
    Errno = IDCF_DecryptContent
              ( pIDCF,
                ParsedDCFHeader.pContentData,
                ParsedDCFHeader.ContentDataLen,
                ParsedDCFHeader.pContentData,
                &ContentDataLen,
                IsContentComplete,
                &( ConsumeRsp.LicenseKey ),
                &( ParsedDCFHeader.EncryptionInfo )
              );
    MSG_FATAL ( "NOTE: Decryption ended.", 0, 0, 0 );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't decrypt content.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_FATAL ( "PASS: Correctly decrypted Content encrypted content.",
        0, 0, 0 );



    /*
     * Let's write the decrypted data to the filesystem so that we can view
     * it with the media player
     */

    /*
     * First test if the file exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                CONTENT_DECRYPTED_FILENAME
              );

    /*
     * The file exists, so we have to remove this
     */
    if ( Errno == SUCCESS )
    {
      Errno = IFILEMGR_Remove
                ( pIFileMgr,
                  CONTENT_DECRYPTED_FILENAME
                );

      if ( Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Unable to remove decrypted file.", 0, 0, 0 );
        Errno = EFAILED;
        break;
      }
    }

    /*
     * We have to reset Errno because if it was false...then Errno = EFAILED
     */
    Errno = SUCCESS;

    /*
     * Create the file
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  CONTENT_DECRYPTED_FILENAME,
                  _OFM_CREATE
                );

    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Unable to create file.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    IFILE_Release ( pIFile );
    
    /*
     * Now actually open the file for writing
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  CONTENT_DECRYPTED_FILENAME,
                  _OFM_READWRITE
                );

    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Unable to open file for read/write.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Write the GIF to the file
     */
    BytesWritten = IFILE_Write
                      ( pIFile,
                        ParsedDCFHeader.pContentData,
                        ContentDataLen
                      );

    if ( BytesWritten == 0 )
    {
      MSG_ERROR ( "FAIL: Unable to write decrypted data to .GIF file.",
          0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Close the file - we're done
     */
    IFILE_Release ( pIFile );

    /*
     * Done
     */
    MSG_FATAL ( "PASS: Correctly wrote .GIF data to file.  Please view.",
        0, 0, 0 );



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** Content TOOLKIT TEST FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_ERROR ( "***** Content TOOLKIT TEST PASSED *****", 0, 0, 0 );
  }




  /*
   * Clean up by deleting the right
   */
  IDRM_DeleteRight
    ( pIDRM,
      RightsHandle,
      &DeleteRsp,
      NULL
    );

  if ( DeleteRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete right.  Error #: %d.",
        DeleteRsp.Errno, 0, 0 );
  }



  /*
   * Clean up buffers and interfaces
   */
  FREE ( pOMARight );
  FREE ( pBuffer );
  IFILEMGR_Release ( pIFileMgr );
  IDRM_Release ( pIDRM );
  IDCF_Release ( pIDCF );
}



/*===========================================================================

FUNCTION DRMTestApp_PrintFinalAsyncMessage

DESCRIPTION
  This is the callback function that prints out how many tests ran/passed

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_PrintFinalAsyncMessage ( uint32 TestsPassed,
                                         uint32 TotalTests
                                       )
{

  MSG_FATAL ( "***********************************", 0, 0, 0 );
  MSG_FATAL ( "* ASYNC TESTS: %d/%d TESTS PASSED *",
              TestsPassed,
              TotalTests,
              0 );
  MSG_FATAL ( "***********************************", 0, 0, 0 );

}



/*===========================================================================

FUNCTION DRMTestApp_DoneDeleteAssocAsyncTest

DESCRIPTION
  This is the callback function for the delete associated rights

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_DoneDeleteAssocAsyncTest ( DRMTestApp_AsyncRsp *pAsyncRsp )
{
  MSG_FATAL ( "NOTE: DeleteAssociated callback started execution.",
     0, 0, 0 );



  if ( pAsyncRsp->DeleteAssocRsp.Errno != SUCCESS )
  {
    /*
     * If there's a problem
     */
    MSG_ERROR ( "FAIL: DeleteAssociated didn't complete.  Error #: %d.",
        pAsyncRsp->DeleteAssocRsp.Errno, 0, 0 );

    /*
     * Setup the error stuff
     */
    pAsyncRsp->DoneAllTests = FALSE;
    pAsyncRsp->Errno = pAsyncRsp->DeleteAssocRsp.Errno;
  }
  else
  {
    /*
     * There's no error, so we are completed!
     */
    pAsyncRsp->AsyncTestsPassed ++;
    pAsyncRsp->DoneAllTests = TRUE;

    MSG_FATAL ( "NOTE: All tests passed.", 0, 0, 0 );
    MSG_FATAL ( "NOTE: DeleteAssociated executed asynchronously.",
       0, 0, 0 );
  }

  /*
   * Clean up and print outs
   */
  IDRM_Release ( pAsyncRsp->pIDRM );
  FREE ( pAsyncRsp->pOMARight );
  DRMTestApp_PrintFinalAsyncMessage
    ( pAsyncRsp->AsyncTestsPassed,
      pAsyncRsp->TotalAsyncTests
    );

  MSG_FATAL ( "NOTE: DeleteAssociated callback completed execution.",
     0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_DoneDeleteAsyncTest

DESCRIPTION
  This is the callback function for the delete

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_DoneDeleteAsyncTest ( DRMTestApp_AsyncRsp *pAsyncRsp )
{
  MSG_FATAL ( "NOTE: Delete callback started execution.", 0, 0, 0 );



  if ( pAsyncRsp->DeleteRsp.Errno != SUCCESS )
  {
    /*
     * If there's a problem
     */
    MSG_ERROR ( "FAIL: Delete didn't complete.  Error #: %d.",
        pAsyncRsp->DeleteRsp.Errno, 0, 0 );

    /*
     * Setup the error stuff
     */
    pAsyncRsp->DoneAllTests = FALSE;
    pAsyncRsp->Errno = pAsyncRsp->DeleteRsp.Errno;


    /*
     * Clean up and print outs
     */
    IDRM_Release ( pAsyncRsp->pIDRM );
    FREE ( pAsyncRsp->pOMARight );
    DRMTestApp_PrintFinalAsyncMessage 
      ( pAsyncRsp->AsyncTestsPassed,
        pAsyncRsp->TotalAsyncTests
      );
  }
  else
  {
    /*
     * There's no error, so run the next test
     */
    pAsyncRsp->AsyncTestsPassed ++;

    MSG_FATAL ( "NOTE: Delete executed asynchronously.", 0, 0, 0 );

    /*
     * Set it up for the next test
     */
    pAsyncRsp->TotalAsyncTests ++;
    CALLBACK_Init ( &( pAsyncRsp->CB ),
                    DRMTestApp_DoneDeleteAssocAsyncTest,
                    pAsyncRsp
                  );
    MEMSET ( &( pAsyncRsp->DeleteAssocRsp ), 0,
       sizeof ( AEEDRM_DeleteAssociatedRightsRspType ) );



    /*
     * Do the second delete
     */
    IDRM_DeleteAssociatedRights
      ( pAsyncRsp->pIDRM,
        pAsyncRsp->URI,
        &( pAsyncRsp->DeleteAssocRsp ),
        &( pAsyncRsp->CB )
      );
  }



  MSG_FATAL ( "NOTE: Delete callback completed execution.", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_DoneLookupAsyncTest

DESCRIPTION
  This is the callback function for the lookup

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_DoneLookupAsyncTest ( DRMTestApp_AsyncRsp *pAsyncRsp )
{
  MSG_FATAL ( "NOTE: Lookup callback started execution.", 0, 0, 0 );



  if ( pAsyncRsp->LookupRsp.Errno != SUCCESS )
  {
    /*
     * If there's a problem
     */
    MSG_ERROR ( "FAIL: Lookup didn't complete.  Error #: %d.",
        pAsyncRsp->LookupRsp.Errno, 0, 0 );

    /*
     * Setup the error stuff
     */
    pAsyncRsp->DoneAllTests = FALSE;
    pAsyncRsp->Errno = pAsyncRsp->LookupRsp.Errno;

    /*
     * Clean up and print outs
     */
    IDRM_Release ( pAsyncRsp->pIDRM );
    FREE ( pAsyncRsp->pOMARight );
    DRMTestApp_PrintFinalAsyncMessage 
      ( pAsyncRsp->AsyncTestsPassed,
        pAsyncRsp->TotalAsyncTests
      );
  }
  else
  {
    /*
     * There's no error, so run the next test
     */
    pAsyncRsp->AsyncTestsPassed ++;

    MSG_FATAL ( "NOTE: Number of rights: %d.  XactionID: %d.",
        pAsyncRsp->LookupRsp.NumRights, pAsyncRsp->LookupRsp.XactionID, 0 );
    MSG_FATAL ( "NOTE: Lookup executed asynchronously.", 0, 0, 0 );

    /*
     * Set it up for the next test
     */
    pAsyncRsp->TotalAsyncTests ++;
    CALLBACK_Init ( &( pAsyncRsp->CB ),
                    DRMTestApp_DoneDeleteAsyncTest,
                    pAsyncRsp
                  );
    MEMSET ( &( pAsyncRsp->DeleteRsp ), 0,
       sizeof ( AEEDRM_DeleteRightRspType ) );



    /*
     * Do the delete
     */
    IDRM_DeleteRight
      ( pAsyncRsp->pIDRM,
        pAsyncRsp->RightsHandle,
        &( pAsyncRsp->DeleteRsp ),
        &( pAsyncRsp->CB )
      );
  }



  MSG_FATAL ( "NOTE: Lookup callback completed execution.", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_DoneAdd2AsyncTest

DESCRIPTION
  This is the callback function for the 2nd add

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_DoneAdd2AsyncTest ( DRMTestApp_AsyncRsp *pAsyncRsp )
{
  MSG_FATAL ( "NOTE: Add #2 callback started execution.", 0, 0, 0 );



  if ( pAsyncRsp->AddRsp.Errno != SUCCESS )
  {
    /*
     * If there's a problem
     */
    MSG_ERROR ( "FAIL: Add #2 didn't complete.  Error #: %d.",
        pAsyncRsp->AddRsp.Errno, 0, 0 );

    /*
     * Setup the error stuff
     */
    pAsyncRsp->DoneAllTests = FALSE;
    pAsyncRsp->Errno = pAsyncRsp->AddRsp.Errno;

    /*
     * Clean up and print outs
     */
    IDRM_Release ( pAsyncRsp->pIDRM );
    FREE ( pAsyncRsp->pOMARight );
    DRMTestApp_PrintFinalAsyncMessage
      ( pAsyncRsp->AsyncTestsPassed,
        pAsyncRsp->TotalAsyncTests
      );
  }
  else
  {
    /*
     * There's no error, so run the next test
     */
    pAsyncRsp->AsyncTestsPassed ++;

    MSG_FATAL ( "NOTE: Add #2 executed asynchronously.", 0, 0, 0 );

    /*
     * Set it up for the next test
     */
    pAsyncRsp->TotalAsyncTests ++;
    CALLBACK_Init ( &( pAsyncRsp->CB ),
                    DRMTestApp_DoneLookupAsyncTest,
                    pAsyncRsp
                  );
    MEMSET ( &( pAsyncRsp->LookupRsp ), 0,
       sizeof ( AEEDRM_LookupRightRspType ) );



    /*
     * Do the lookup
     */
    IDRM_LookupRight
      ( pAsyncRsp->pIDRM,
        pAsyncRsp->URI,
        0x00,
        &( pAsyncRsp->LookupRsp ),
        &( pAsyncRsp->CB )
      );
  }



  MSG_FATAL ( "NOTE: Add #2 callback completed execution.", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_DoneAdd1AsyncTest

DESCRIPTION
  This is the callback function for the 1st add

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_DoneAdd1AsyncTest ( DRMTestApp_AsyncRsp *pAsyncRsp )
{
  MSG_FATAL ( "NOTE: Add #1 callback started execution.", 0, 0, 0 );



  if ( pAsyncRsp->AddRsp.Errno != SUCCESS )
  {
    /*
     * If there's a problem
     */
    MSG_ERROR ( "FAIL: Add #1 didn't complete.  Error #: %d.",
        pAsyncRsp->AddRsp.Errno, 0, 0 );

    /*
     * Setup the error stuff
     */
    pAsyncRsp->DoneAllTests = FALSE;
    pAsyncRsp->Errno = pAsyncRsp->AddRsp.Errno;

    /*
     * Clean up and print outs
     */
    IDRM_Release ( pAsyncRsp->pIDRM );
    FREE ( pAsyncRsp->pOMARight );
    DRMTestApp_PrintFinalAsyncMessage 
      ( pAsyncRsp->AsyncTestsPassed,
        pAsyncRsp->TotalAsyncTests
      );

  }
  else
  {
    /*
     * There's no error, so run the next test
     */
    pAsyncRsp->AsyncTestsPassed ++;
    pAsyncRsp->RightsHandle = pAsyncRsp->AddRsp.RightsHandle;

    MSG_FATAL ( "NOTE: Add #1 executed asynchronously.", 0, 0, 0 );

    /*
     * Set it up for the next test
     */
    pAsyncRsp->TotalAsyncTests ++;
    CALLBACK_Init ( &( pAsyncRsp->CB ),
                    DRMTestApp_DoneAdd2AsyncTest,
                    pAsyncRsp
                  );
    MEMSET ( &( pAsyncRsp->AddRsp ), 0, sizeof ( AEEDRM_AddRightRspType ) );



    /*
     * Do the second add
     */
    IDRM_AddRight
      ( pAsyncRsp->pIDRM,
        pAsyncRsp->URI,
        AEEDRM_OMA_1_0_VERSION,
        pAsyncRsp->pOMARight,
        pAsyncRsp->OMARightLength,
        &( pAsyncRsp->AddRsp ),
        &( pAsyncRsp->CB )
      );
  }



  MSG_FATAL ( "NOTE: Add #1 callback completed execution.", 0, 0, 0 );
}



/*===========================================================================

FUNCTION DRMTestApp_TestAsyncAPI

DESCRIPTION
  This function uses the DRM API functions in asynchronous mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_TestAsyncAPI
        ( IShell*                               pIShell )
{
  /* Variable declarations */
  IDRM*                                         pIDRM = NULL;
  int                                           Errno = SUCCESS;
  uint8*                                        pOMARight = NULL;
  uint32                                        OMARightLength = 0;
  AEEDRM_PermissionTemplateType                 PermissionTemplate;
  int                                           StrLCpyError = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define ASYNC_URI "async_uri_1@qc-int.com"



  /*
   * Do while the functionality isn't complete and there are no errors
   */
  do
  {



    /*
     * Functions to test out:
     *
     * IDRM_AddRight
     * IDRM_LookupRight
     * IDRM_ConsumeRight
     * IDRM_DeleteRight
     * IDRM_DeleteAssociatedRights
     */

    /*
     * Get a handle to the DRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Create a forward lock right
     */
    MEMSET ( &PermissionTemplate, 0, 
        sizeof ( AEEDRM_PermissionTemplateType ) );

    /*
     * Allocate memory for this right
     */
    pOMARight = ( uint8* ) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARight == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          AEEDRM_XML_BUFFER_SIZE, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Create the right
     */
    Errno = IDRM_CreateRight
              ( pIDRM,
                ASYNC_URI,
                PermissionTemplate,
                pOMARight,
                &OMARightLength
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create right.  Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Set up some of the information for the global structure
     */
    AsyncRsp.pIDRM = pIDRM;
    AsyncRsp.pOMARight = pOMARight;
    AsyncRsp.OMARightLength = OMARightLength;
    AsyncRsp.Errno = SUCCESS;
    AsyncRsp.TotalAsyncTests = 0;
    AsyncRsp.AsyncTestsPassed = 0;
    AsyncRsp.RightsHandle = 0;
    MEMSET ( &( AsyncRsp.CB ), 0, sizeof ( AEECallback ) );
    MEMSET ( &( AsyncRsp.AddRsp ), 0, sizeof ( AEEDRM_AddRightRspType ) );
    
    StrLCpyError = STRLCPY ( (char*) &( AsyncRsp.URI ), ASYNC_URI,
        AEEDRM_MAX_URI_LEN );

    if ( StrLCpyError == -1 )
    {
      MSG_ERROR ( "FAIL: Can't copy the URI.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    AsyncRsp.DoneAllTests = FALSE;
    AsyncRsp.Counter = 0;



    /*
     * Set up the add callback
     */
    CALLBACK_Init
      ( &( AsyncRsp.CB ),
        DRMTestApp_DoneAdd1AsyncTest,
        &( AsyncRsp )
      );

    /*
     * Do the add
     */
    AsyncRsp.TotalAsyncTests = 1;
    IDRM_AddRight
      ( pIDRM,
        ASYNC_URI,
        AEEDRM_OMA_1_0_VERSION,
        pOMARight,
        OMARightLength,
        &( AsyncRsp.AddRsp ),
        &( AsyncRsp.CB )
      );

  } while ( 0 );

  MSG_FATAL ( "***** DONE FXN *****", 0, 0, 0 );

}



/*===========================================================================

FUNCTION DRMTestApp_RunAsyncTests

DESCRIPTION
  This function runs all the APIs that have the ability to be called
  asynchronously

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunAsyncTests ( IShell* pIShell )
{
  DRMTestApp_TestAsyncAPI ( pIShell );
}



/*===========================================================================

FUNCTION DRMTestApp_RunAllTests

DESCRIPTION
  This function runs all the tests.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMTestApp_RunAllTests ( IShell* pIShell )
{
  DRMTestApp_RunAddTests ( pIShell );
  DRMTestApp_RunCreateTests ( pIShell );
  DRMTestApp_RunLookupRetrieveTests ( pIShell );
  DRMTestApp_RunConsumeTests( pIShell );
  DRMTestApp_RunCheckTests( pIShell );
  DRMTestApp_RunDeleteTests ( pIShell );
  DRMTestApp_RunProtectContentTests (pIShell );
  DRMTestApp_RunUpdateTests ( pIShell );
  DRMTestApp_RunFlowTests( pIShell );
  DRMTestApp_RunContentHandleTests ( pIShell );
  DRMTestApp_UseContentToolkitFile ( pIShell );
#ifdef INTERNAL_TESTS
  DRMTestApp_RunLoadDBTests ( pIShell );
  DRMTestApp_RunEncryptionHeaderTests ();
  DRMTestApp_RunUintvarTests ();
#endif /* INTERNAL_TESTS */

#ifdef ASYNC_TESTS
  DRMTestApp_RunAsyncTests ( pIShell );
#endif /* ASYNC_TESTS */
} /* DRMTestApp_RunAllTests */

#endif /* FEATURE_SEC_TESTAPPS */



/*===========================================================================

FUNCTION DRMUnitTestApp

DESCRIPTION
  Entry point to all BREW based DRM Unit tests

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMUnitTestApp( IShell* pIShell )
{
#ifdef  FEATURE_SEC_TESTAPPS
  DRMTestApp_RunAllTests ( pIShell );
#endif /* FEATURE_SEC_TESTAPPS */
}/* DRMUnitTestApp */



/*===========================================================================

FUNCTION DRMTestApp

DESCRIPTION
  Entry point to a refernce App using DRM and DCF APIs

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


/* General defines for downloader & player */
#define CIPHER_BUFFER_LENGTH 2048
#define OUTPUT_BUFFER_LENGTH 2048
#define RIGHTS_OBJECT_LIST_LENGTH 2
#define DCF_BASE_DIRECTORY "DCFStore"
#define PATH_SEPARATOR "/"

/*
 * FORWARD LOCK CONTENT
 *
 * Application (.exe)
 */
#define CONTENT_1_LENGTH 512
#define CONTENT_1_URI "content1@foo.com"
#define CONTENT_1_MIME_TYPE "application/octet-stream"
#define CONTENT_1_DCF_FILE_NAME "DCFStore/content1.dcf"

/*
 * COMBINED DELIVERY CONTENT
 *
 * Image (.jpg)
 */
#define CONTENT_2_LENGTH 277
#define CONTENT_2_URI "content2@foo.com"
#define CONTENT_2_MIME_TYPE "image/jpeg"
#define CONTENT_2_DCF_FILE_NAME "DCFStore/content2.dcf"

/*
 * SEPARATE DELIVERY CONTENT
 *
 * Mp3 File (.mp3)
 */
#define CONTENT_3_LENGTH 1029
#define CONTENT_3_URI "content3@foo.com"
#define CONTENT_3_MIME_TYPE "audio/mpeg3"
#define CONTENT_3_DCF_FILE_NAME "DCFStore/content3.dcf"


/*===========================================================================
FUNCTION
  CreateDCFFile

DESCRIPTION
  This function creates a buffer that holds the contents of a .DCF file.
  Given unencrypted content and all required arguments, the function will
  automatically create a header, encrypt the content and append them
  appropriately into a .DCF  buffer.

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - .DCF buffer created
    ENOMEMORY - Couldn't allocate memory
    EFAILED - General failure case
    EBADPARM - Bad parameters

SIDE EFFECTS
  None
===========================================================================*/
int CreateDCFFile
      ( IShell*                       pIShell,
        IDCF*                         pIDCF,
        IDRM*                         pIDRM,
        const char*                   FileName,
        const char*                   URI,
        const char*                   MimeType,
        uint8*                        pContent,
        uint32                        ContentLength,
        const AEEDRM_RightsHandleType RightsHandle
      )
{
  /* Variable declarations */
  IFileMgr*                           pIFileMgr = NULL;
  IFile*                              pIFile = NULL;
  AEEDRM_EncryptionInfoType           EncryptionInfo;
  uint8*                              pDCFHeader = NULL;
  uint32                              DCFHeaderLength = 0;
  uint8*                              pCipherBuffer = NULL;
  uint32                              CipherBufferLength = 0;
  uint32                              RefCount = 0;
  uint32                              FileError = 0;
  int                                 Errno = SUCCESS;
  boolean                             IsContentComplete = FALSE;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Do sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
         ||
         RightsHandle == NULL
        )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * Allocate memory for the DCF header and set its size
     */
    pDCFHeader = (uint8*) MALLOC ( AEEDCF_DCFHDR_BUFFER_SIZE );
    if ( pDCFHeader == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_DCFHDR_BUFFER_SIZE\
          bytes.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    DCFHeaderLength = AEEDCF_DCFHDR_BUFFER_SIZE;



    /*
     * To create a DCF file, the content must first be given a DCF header
     */
    Errno = IDCF_CreateContentHeader
              ( pIDCF,
                URI,
                MimeType,
                ContentLength,
                &EncryptionInfo,
                pDCFHeader,
                &DCFHeaderLength
              );
    /*
     * At the end of this function call, DCFHeaderLength now holds the size of
     * the created DCF header
     */

    /*
     * Check for error conditions to ensure that the header was created
     * successfully
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Could not create content header.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * The next step is to encrypt the content
     *
     * In this case, because the content itself can't be modified, we are
     * creating a separate buffer and dumping the encrypted content in there.
     * This will be done in a SINGLE STEP
     *
     * In other words, encryption is done OUT OF PLACE, SINGLE STEP
     */

    /*
     * Allocate memory for the cipher buffer and set its size
     */
    pCipherBuffer = (uint8*) MALLOC ( CIPHER_BUFFER_LENGTH );
    if ( pCipherBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CIPHER_BUFFER_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    CipherBufferLength = CIPHER_BUFFER_LENGTH;
    IsContentComplete = TRUE;

    /*
     * Protect the content OUT OF PLACE, SINGLE STEP
     */
    Errno = IDRM_ProtectContent
        ( pIDRM,
          RightsHandle,
          pContent,
          ContentLength,
          pCipherBuffer,
          &CipherBufferLength,
          IsContentComplete,
          &EncryptionInfo
        );

    /*
     * Check for error conditions to ensure that the content was encrypted
     * successfully
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Could not protect content. Error #: %d.", Errno, 0,
          0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We now have:
     *    a) DCF Header
     *    b) Protected Content
     *
     * Using these two pieces we can create a DCF file (.dcf)
     */

    /*
     * First, get a handle to the IFileMgr interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * First test and see if the file exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                FileName
              );

    if ( Errno == SUCCESS )
    {
      /*
       * The file exists...so delete it!
       */
      Errno = IFILEMGR_Remove
                ( pIFileMgr,
                  FileName
                );
      if ( Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Couldn't delete file.  Error #: %d.", Errno, 0,
            0 );
        Errno = EFAILED;
        break;
      }
    }

    /*
     * Well...it doesn't exist or has been deleted, so reset Errno
     */
    Errno = SUCCESS;

    /*
     * Well, we can create the file now
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_CREATE
                );

    /*
     * Check error conditionss
     */
    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't create file.  Error #: %d.", Errno, 0,
          0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Release the file
     */
    RefCount = IFILE_Release ( pIFile );
    /*
     * The file should be closed now
     */

    /*
     * Now open in R/W mode...do I have to do this in two separate steps?!
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_READWRITE
                );

    /*
     * Check error conditionss
     */
    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't read/write file.  Error #: %d.",
        Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Finally, we are ready to write to the file
     */

    /*
     * I assume that I'm at the start of the file (since it was just
     * created )
     *
     * Write the DCF header
     */
    FileError = IFILE_Write
                  ( pIFile,
                    pDCFHeader,
                    DCFHeaderLength
                  );

    if ( FileError == 0 )
    {
      MSG_ERROR ( "FAIL: Can't write DCF header to file.", 0, 0, 0 );
      Errno = EFAILED;

      /*
       * Before we break out, close the file - this should only loop once
       */
      do
      {
        RefCount = IFILE_Release ( pIFile );
      } while ( RefCount != 0 );

      break;
    }

    /*
     * Write the encrypted content
     */
    FileError = IFILE_Write
                  ( pIFile,
                    pCipherBuffer,
                    CipherBufferLength
                  );

    if ( FileError == 0 )
    {
      MSG_ERROR ( "FAIL: Can't write encrypted content to file.", 0, 0, 0 );
      Errno = EFAILED;


      /*
       * Before we break out, close the file - this should only loop once
       */
      do
      {
        RefCount = IFILE_Release ( pIFile );
      } while ( RefCount != 0 );

      break;
    }

    /*
     * Close up the file
     */
    RefCount = IFILE_Release ( pIFile );
    /*
     * Now the file is closed and we have a brand new .DCF file!
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** CreateDCFFile FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** CreateDCFFile PASSED *****", 0, 0, 0 );
  }



  FREE ( pDCFHeader );
  FREE ( pCipherBuffer );

  (void) IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  HandleForwardLock

DESCRIPTION
  This function demonstrates how the downloader would deal with a forward
  lock media object.

  To recap, a forward lock media object is one which the user has permanent
  rights on.  _However_ they will not be allowed to transfer these objects to
  other users.  Forward lock media objects do not come with associated OMA
  rights.

  To insulate the player application from the downloading scheme, the
  dowloader takes the following steps:
      a) It creates a right with all permissions enabled and permanent usage
      b) It adds this permission to the secure database
      c) It creates a .DCF file with the content

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Forward lock right added to database
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleForwardLock
      ( IShell*                         pIShell,
        IDCF*                           pIDCF,
        IDRM*                           pIDRM,
        const char*                     FileName,
        const char*                     URI,
        const char*                     MimeType,
        uint8*                          pContent,
        uint32                          ContentLength
      )
{
  /* Variable declarations */
  AEEDRM_PermissionTemplateType         PermissionsTemplate;
  AEEDRM_AddRightRspType                AddRsp;
  AEEDRM_RightsHandleType               RightsHandle = 0;
  uint8*                                pOMARightsObject = NULL;
  uint32                                OMARightsObjectLength = 0;
  int                                   Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/


  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step is to create a forward lock right for this media object.
     */

    /*
     * The simplest way to create a forward lock right is to memset the entire
     * template to 0.
     */
    (void) MEMSET ( &PermissionsTemplate, 0,
        sizeof ( AEEDRM_PermissionTemplateType ) );

    /*
     * Allocate memory for an OMA 1.0 rights object and set its size
     */
    pOMARightsObject = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARightsObject == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_XML_BUFFER_SIZE bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightsObjectLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Use this template to create a right
     */
    Errno = IDRM_CreateRight
              ( pIDRM,
                URI,
                PermissionsTemplate,
                pOMARightsObject,
                &OMARightsObjectLength
              );
    /*
     * At the end of this call, OMARightsObjectLength is set to the actual
     * length of the created OMA 1.0 rights object
     */

    /*
     * Check for error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Fwd lock rights can't be created.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now we add this newly created right to the secure database, so that we
     * can get a rights handle for it
     */
    IDRM_AddRight
      ( pIDRM,
        URI,
        AEEDRM_OMA_1_0_VERSION,
        pOMARightsObject,
        OMARightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't add fwd lock right.  Error #: %d.",
          AddRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the rights handle so that it can be used to create a .DCF file
     */
    RightsHandle = AddRsp.RightsHandle;



    /*
     * Now that the right has been added to the database, it is a simple
     * matter to create the .DCF file
     */
    Errno = CreateDCFFile
              ( pIShell,
                pIDCF,
                pIDRM,
                FileName,
                URI,
                MimeType,
                pContent,
                ContentLength,
                RightsHandle
              );

    /*
     * Check the error conditions.  Errors can be one of:
     *    a) EFAILED
     *    b) ENOMEMORY
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create .DCF file.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleForwardLock FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleForwardLock PASSED *****", 0, 0, 0 );
  }

  FREE ( pOMARightsObject );

  return Errno;
}



/*===========================================================================
FUNCTION
  HandleCombinedDelivery

DESCRIPTION
  This function demonstrates the role the downloader application should play
  if it has to deal with a combined delivery media object.

  To recap, in the case of combined delivery, the unencrypted media object is
  shipped together with its rights in a DRM message.  Since the goal is to
  insulate the player application from the downloading method used, the
  downloader has the following responsibilities:
      a) Add the supplied right to the secure database
      b) Create a .DCF file using the content

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Combined delivery right added to database
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleCombinedDelivery
      ( IShell*                           pIShell,
        IDCF*                             pIDCF,
        IDRM*                             pIDRM,
        const char*                       FileName,
        const char*                       URI,
        const char*                       MimeType,
        uint8*                            pContent,
        uint32                            ContentLength,
        uint8*                            pRightsObject,
        uint32                            RightsObjectLength,
        const AEEDRM_VersionEnumType      RELVersion
      )
{
  /* Variable declarations */
  AEEDRM_AddRightRspType                    AddRsp;
  AEEDRM_RightsHandleType                   RightsHandle = 0;
  int                                       Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDRM == NULL
         ||
         pIDCF == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
         ||
         pRightsObject == NULL
         ||
         ( (char*)pRightsObject )[0] == '\0' /* Assuming right is in text! */
         ||
         RightsObjectLength == 0
         ||
         RightsObjectLength > AEEDRM_XML_BUFFER_SIZE
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step is to add the right to the secure database in order to
     * get a rights handle
     */

    /*
     * Add the right to the database using the user supplied right
     */
    IDRM_AddRight
      ( pIDRM,
        URI,
        RELVersion,
        pRightsObject,
        RightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Check error conditions to ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot add right.  Error #: %d.", AddRsp.Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    };

    /*
     * Set the rights handle so that it can be used to create a .DCF file
     */
    RightsHandle = AddRsp.RightsHandle;



    /*
     * Now that the right has been added to the database, it is a simple
     * matter to create the .DCF file
     */
    Errno = CreateDCFFile
              ( pIShell,
                pIDCF,
                pIDRM,
                FileName,
                URI,
                MimeType,
                pContent,
                ContentLength,
                RightsHandle
              );

    /*
     * Check the error conditions.  Errors can be one of:
     *    a) EFAILED
     *    b) ENOMEMORY
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create .DCF file.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleCombinedDelivery FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleCombinedDelivery PASSED *****", 0, 0, 0 );
  }


  return Errno;
}



/*===========================================================================
FUNCTION
  HandleSeparateDelivery

DESCRIPTION
  This function demonstrated the role the downloader should take when dealing
  with a separate delivery DRM protected media object.

  To recap, in the case of separate delivery, the rights object and the
  encrypted content are shipped separately.  This is (by far) the simplest
  case for the downloader to deal with.  It simply has to:
      a) Add the supplied right to the secure database

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Added a separate delivery right to the secure database
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleSeparateDelivery
      ( IDRM*                                   pIDRM,
        const char*                             URI,
        uint8*                                  pRightsObject,
        uint32                                  RightsObjectLength,
        const AEEDRM_VersionEnumType            RELVersion
      )
{
  /* Variable declarations */
  AEEDRM_AddRightRspType                        AddRsp;
  int                                           Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIDRM == NULL
         ||
         pRightsObject == NULL
         ||
         ( (char*) pRightsObject )[0] == '\0' /* Assuming it is text! */
         ||
         RightsObjectLength == 0
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The only step is to add the right to the secure database in order to
     * get a rights handle
     */

    IDRM_AddRight
      ( pIDRM,
        URI,
        RELVersion,
        pRightsObject,
        RightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Check the error code to ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot add right.  Error #: %d.", AddRsp.Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    };



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleSeparateDelivery FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleSeparateDelivery PASSED *****", 0, 0, 0 );
  }


  return Errno;
}



/*===========================================================================
FUNCTION
  PrintRightsInformation

DESCRIPTION
  This function does a lookup on a supplied URI and prints out constraints
  information on all the rights associated with that URI

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Lookup successful, rights retrieved successfully and printed
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int PrintRightsInformation
      ( IDRM*                           pIDRM,
        const char*                     URI
      )
{
  /* Variable declarations */
  AEEDRM_LookupRightRspType             LookupRsp;
  AEEDRM_XactionIdType                  XactionId = 0;
  AEEDRM_RightsObjectType*              pRightsObjectList = NULL;
  uint32                                RightsObjectListLength = 0;
  uint32                                NumRights = 0;
  uint32                                RightsRetrieved = 0;
  uint32                                Counter = 0;
  int                                   Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIDRM == NULL
         ||
         URI == NULL
         ||
         URI[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * Lookup the URI
     *
     * We _must_ do a lookup before we do a retrieve because the transaction
     * ID returned by the lookup is associated with the list of rights that
     * the retrieve has to pick up
     */
    IDRM_LookupRight
      ( pIDRM,
        URI,
        0x00,
        &LookupRsp,
        NULL
      );

    /*
     * Do an error check
     */
    if ( LookupRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't look up the URI.  Error #: %d",
          LookupRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get the number of rights and transaction id
     */
    NumRights = LookupRsp.NumRights;
    XactionId = LookupRsp.XactionID;




    /*
     * Retrieve the rights using the transaction id generated above
     */

    /*
     * Allocate the memory for the rights object list
     */
    pRightsObjectList = (AEEDRM_RightsObjectType*) MALLOC
                          ( sizeof ( AEEDRM_RightsObjectType ) *
                            RIGHTS_OBJECT_LIST_LENGTH
                          );
    if ( pRightsObjectList == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC couldn't allocate memory", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    RightsObjectListLength = RIGHTS_OBJECT_LIST_LENGTH;

    /*
     * Now do the actual retrieve in a loop
     */
    do
    {
      Errno = IDRM_RetrieveRight
                ( pIDRM,
                  XactionId,
                  pRightsObjectList,
                  &RightsObjectListLength
                );

      /*
       * These are the two cases where the retrieve was successful
       */
      if ( Errno == SUCCESS
           ||
           Errno == AEEDRM_EMOREDATA )
      {
        /*
         * Let's keep a running tally of the number of rights retrieved by the
         * API
         */
        RightsRetrieved += RightsObjectListLength;



        /*
         * Print out information about the rights objects retrieved
         */
        for ( Counter = 0; Counter < RightsObjectListLength; Counter++ )
        {
          MSG_HIGH ( "DRM Version: %d.",
              pRightsObjectList[Counter].DrmVersion, 0, 0 );
          MSG_HIGH ( "Permissions Mask: 0x%x.",
              pRightsObjectList[Counter].PermissionsMask, 0, 0 );

          /*
           * Print out information on the Play constraints
           */
          MSG_HIGH ( "Play Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Play.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Play.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Play.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Play.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Play.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Display constraints
           */
          MSG_HIGH ( "Display Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Display.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Display.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Display.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Display.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Display.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Execute constraints
           */
          MSG_HIGH ( "Execute Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Execute.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Execute.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Execute.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Execute.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Execute.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Print constraints
           */
          MSG_HIGH ( "Print Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Print.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Print.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Print.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Print.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Print.RemainingValue,
              0, 0 );
        }

        /*
         * Done printing all the constraints for the rights that were
         * retrieved
         */



      }

      /*
       * In this case, there are more rights to be retrieved, so we reset the
       * size of the rights list so that another RIGHTS_OBJECT_LIST_LENGTH
       * rights can be copied.
       */
      if ( Errno == AEEDRM_EMOREDATA )
      {
        /*
         * Reset the rights object list length again
         */
        RightsObjectListLength = RIGHTS_OBJECT_LIST_LENGTH;
      }



    } while ( Errno == AEEDRM_EMOREDATA );

    if ( Errno != SUCCESS )
    {
      /*
       * It appears we've broken out early because of some sort of error.
       */
      MSG_ERROR ( "FAIL: Unable to retrieve rights.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_HIGH ( "Number of rights retrieved: %d.", RightsRetrieved, 0, 0 );



  } while ( 0 );

  FREE ( pRightsObjectList );

  return Errno;
}



/*===========================================================================
FUNCTION
  DRMPlayer

DESCRIPTION
  This function demonstrates the role of a DRM enabled media player.

  NOTE: This function does NO actual playing of media.  It simply
        illustrates the steps that have to be taken by the player to
        actually play the content

  To play the content, the player has to go through the following steps:
      a) Consume the right
      b) Check if the actions requested actually match the permissions
         returned
      c) Parse the .DCF file for information about the encrypted content
         attached to it
      d) Decrypt the content using the license key returned by the
         IDRM_ConsumeRight() and the encryption information returned by
         IDCF_ParseContentHeader()
      e) Play the right

  NOTE:  You can supply _ONE_ of URI/RightsHandle.  You don't have to supply
         both.  However, it is an error to supply neither.

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Content played
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int DRMPlayer
      ( IShell*                           pIShell,
        IDCF*                             pIDCF,
        IDRM*                             pIDRM,
        const char*                       FileName,
        const AEEDRM_ActionEnumType       Action
      )
{
  /* Variable declarations */
  AEEDRM_ConsumeRightRspType              ConsumeRsp;
  AEEDCF_HeaderType                       ParsedDCFHeader;
  IFileMgr*                               pIFileMgr = NULL;
  IFile*                                  pIFile = NULL;
  FileInfo                                DCFFileInfo;
  uint8*                                  pOutput = NULL;
  uint8*                                  pDCFFileBuffer = NULL;
  uint32                                  DCFFileBufferLength = 0;
  uint32                                  RefCount = 0;
  boolean                                 IsContentComplete = FALSE;
  int                                     BytesRead = 0;
  int                                     Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * We need to access the DCF file for a number of reasons:
     *
     * 1) Find the size of the file so that we can allocate memory for a
     *    buffer into which we can place the contents
     * 2) Read the DCF header and content into this buffer
     */

    /*
     * Get a handle to the IFileMgr interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Open up this file for read access
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_READ
                );

    if ( pIFile == NULL )
    {
      /*
       * Something broke - so let's check what the error is
       */
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't read from file.  Error #: %d.",
        Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get the file size of the file
     */
    Errno = IFILE_GetInfo
              ( pIFile,
                &DCFFileInfo
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't get file information.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;

      /*
       * In case we failed with an error, we still want to make sure the file is
       * closed properly
       */
      while ( RefCount != 0 )
      {
        RefCount = IFILE_Release ( pIFile );
      }

      break;
    }

    /*
     * We need to allocate space for the buffer into which the contents of
     * the .DCF file is read.
     */
    pDCFFileBuffer = (uint8*) MALLOC ( DCFFileInfo.dwSize );
    if ( pDCFFileBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          DCFFileInfo.dwSize, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    DCFFileBufferLength = DCFFileInfo.dwSize;

    /*
     * Read all the bytes from the file
     */
    BytesRead = IFILE_Read
                  ( pIFile,
                    pDCFFileBuffer,
                    DCFFileBufferLength
                  );
    if ( BytesRead == 0 )
    {
      MSG_ERROR ( "FAIL: Can't read from file.", 0, 0, 0 );
      Errno = EFAILED;

      /*
       * In case we failed with an error, we still want to make sure the file is
       * closed properly
       */
      while ( RefCount != 0 )
      {
        RefCount = IFILE_Release ( pIFile );
      }

      break;
    }

    /*
     * Close up the file...we're done - we only need to do this once,
     * but you should check RefCount...
     */
    RefCount = IFILE_Release ( pIFile );



    /*
     * To play the file/decrypt the content we need two pieces of
     * information:
     *    a) License Key
     *    b) Encryption Information
     *
     * The license key is returned as part of a valid consume response.
     * The encryption information has to be retrieved from the .DCF file
     */

    /*
     * Get the encryption information
     */

    /*
     * The first step is to get the information from the .DCF file into the DCF
     * header structure
     */
    Errno = IDCF_ParseContentHeader
              ( pIDCF,
                pDCFFileBuffer,
                DCFFileBufferLength,
                &ParsedDCFHeader
              );

    /*
     * Check the error code to ensure that the DCF file was parsed properly
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "Can't parse the .DCF file.  Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Print out rights information for the rights associated with the
     * content
     */
    Errno = PrintRightsInformation
              ( pIDRM,
                ParsedDCFHeader.szContentURI
              );
    /*
     * Check error condition
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't print out rights information.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now, we use the information in the .DCF file to see if the object can
     * be consumed.  Specifically, we use the URI.
     */

    /*
     * To do this, simply call IDRM_ConsumeRight() with the URI returned in
     * the ParsedDCFHeader
     */
    IDRM_ConsumeRight
      ( pIDRM,
        ParsedDCFHeader.szContentURI,
        0x00,
        Action,
        &ConsumeRsp,
        NULL
      );

    /*
     * Check the error codes...these are extremely important to the player.
     * Right now we don't do anything specific with them.  However, based on
     * the return value, the player can tell exactly what the issue was with
     * the consume
     */
    if ( ConsumeRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't consume right.  Error #: %d.",
          ConsumeRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }
   
    /*
     * Finally, we have the pieces required to decrypt the content
     */


    /*
     * Since we are going to decrypt in a single step
     */
    IsContentComplete = TRUE;

    /*
     * For the interests of the example we will demonstrate how to decrypt the
     * content IN PLACE in a SINGLE STEP
     *
     * This API supports IN PLACE decryption and decryption in MULTIPLE STEPS
     */
    Errno = IDCF_DecryptContent
              ( pIDCF,
                ParsedDCFHeader.pContentData,
                ParsedDCFHeader.ContentDataLen,
                ParsedDCFHeader.pContentData,
                &( ParsedDCFHeader.ContentDataLen ),
                IsContentComplete,
                &( ConsumeRsp.LicenseKey ),
                &( ParsedDCFHeader.EncryptionInfo )
              );

    /*
     * Check the error codes to ensure that we decrypted properly
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't decrypt content.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now that we have decrypted the content (and it is in pOutput) we (as
     * the player) can do whatever we want with it.  i.e. play it, display it
     * etc.
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** DRMPlayer FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** DRMPlayer PASSED *****", 0, 0, 0 );
  }

  FREE ( pOutput );
  FREE ( pDCFFileBuffer );

  (void) IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  CreateDCFDirectory

DESCRIPTION
  This function creates the DCF directory that all the .DCF files are stored
  in

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Directory created successfully
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int CreateDCFDirectory ( IShell*            pIShell,
                         const char*        DCFDirName
                       )
{
  /* Variable declarations */
  IFileMgr*                                 pIFileMgr = NULL;
  int                                       Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Keep looping as long as there are no errors and the functionality is not
   * complete
   */
  do
  {
    /*
     * Sanity checks
     */
    if ( pIShell == NULL
         ||
         DCFDirName == NULL
         ||
         DCFDirName[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * To create the directory we need to create a handle to the IFILEMGR
     * interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We are going to store all the .DCF files in a directory named
     * DCFStore
     *
     * Since I am assuming BREW 3.0, this name is _CASE SENSITIVE_
     * The exact path for this directory is fs:/DCFStore
     */

    /*
     * First, we do a quick test to see if the directory exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                DCFDirName
              );

    /*
     * The directory does not exist, so we must create it
     */
    if ( Errno != SUCCESS )
    {
      /*
       * Create the directory then
       */
      Errno = IFILEMGR_MkDir
                ( pIFileMgr,
                  DCFDirName
                );

      if ( Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Couldn't create DCF_BASE_DIRECTORY.  Error #: %d",
            Errno, 0, 0 );
        Errno = EFAILED;
        break;
      }
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** CreateDCFDirectory FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** CreateDCFDirectory PASSED *****", 0, 0, 0 );
  }

  (void) IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  DRMDownloadPlayerApp

DESCRIPTION
  This function is the entry point to demonstrate usage of the
  downloader/player.  It creates content, decides which use case it will
  execute (forward lock, combined delivery, separate delivery), executes
  that use case and finally plays the content

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void DRMTestApp ( IShell* pIShell )
{
  /* Variable declarations */
  IFileMgr*                               pIFileMgr = NULL;
  IDRM*                                   pIDRM = NULL;
  IDCF*                                   pIDCF = NULL;
  AEEDRM_ActionEnumType                   Action = 0x00;
  AEEDRM_VersionEnumType                  RELVersion = AEEDRM_OMA_1_0_VERSION;
  AEEDRM_DeleteAssociatedRightsRspType    DeleteAssocRsp;
  AEEDRM_PermissionTemplateType           PermissionsTemplate;
  uint8*                                  pContent1 = NULL;
  uint32                                  Content1Length = 0;
  uint8*                                  pOMARightsObject = NULL;
  uint32                                  OMARightsObjectLength = 0;
  char*                                   pContent1URI = NULL;
  char*                                   pContent1MimeType = NULL;
  char*                                   pContent1FileName = NULL;
  uint8*                                  pContent2 = NULL;
  uint32                                  Content2Length = 0;
  char*                                   pContent2URI = NULL;
  char*                                   pContent2MimeType = NULL;
  char*                                   pContent2FileName = NULL;
  int                                     StrErr = 0;
  int                                     Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL )
    {
      /* It should _NEVER_ get here */
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step in dealing with playing a media object is
     * get a handle to the DRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We follow this by creating a handle to the DCF interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDCF, ( void** )(&pIDCF) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DCF interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Create the directory in which the DCF files are stored
     */
    Errno = CreateDCFDirectory ( pIShell, DCF_BASE_DIRECTORY );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create DCF directory.  Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * FORWARD LOCK MEDIA OBJECT
     *
     * A forward lock media object is one that is transmitted unprotected to
     * the user.  The user is allowed permanent and unconstrainted rights to
     * do whatever desired with that object _on the device_.  The user cannot
     * transmit this media object to another device.
     *
     * A typical example of a forward lock media object being transmitted
     * using HTTP download is given below
     *
     *
     * HTTP/1.1 200 OK 
     * Content-type: application/vnd.oma.drm.message;
     * boundary=boundary-1 
     * Content-Length: 574 
     * --boundary-1
     * Content-type: image/jpeg
     * Content-Transfer-Encoding: binary
     * ...jpeg image in binary format...
     * --boundary-1--
     *
     * In this case, the downloader would parse the HTTP stream and extract
     * all the headers.  It would then pass the headers we are interested in
     * to the DRM stack.  The information we are interested in is:
     *
     * Content-Length
     * Content-Type (Mime Type)
     * Content (in the above case it would be the jpeg image)
     * Content-URI
     *
     * In the example, I have below we are not receiving our content from a
     * DRM aware downloader.  So, I have to:
     *
     * a) Create the content
     * b) Create a mime-type and URI for this content
     *
     * In a real system, all this information is parsed from the HTTP stream.
     */

    /*
     * The first step is to create a piece of content.  For our purposes, the
     * content will be an opaque memory blob.  We must also set its size
     */
    pContent1 = (uint8*) MALLOC ( CONTENT_1_LENGTH );
    if ( pContent1 == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CONTENT_1_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    Content1Length = CONTENT_1_LENGTH;

    /*
     * Let's initialize the content to some known value
     */
    (void) MEMSET ( pContent1, 1, Content1Length );


    /*
     * The next step is to allocate space for the URI and mime type
     */
    pContent1URI = (char*) MALLOC ( AEEDRM_MAX_URI_LEN );
    if ( pContent1URI == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_MAX_URI_LEN bytes.", 0,
          0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1URI[0] = '\0';

    pContent1MimeType = (char*) MALLOC ( AEEDCF_MIMETYPE_MAXLEN );
    if ( pContent1MimeType == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_MIMETYPE_MAXLEN bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1MimeType[0] = '\0';

    /*
     * Set the URI and mime types of content 1
     */
    (void) STRLCPY ( pContent1URI, CONTENT_1_URI, AEEDRM_MAX_URI_LEN);
    (void) STRLCPY ( pContent1MimeType, CONTENT_1_MIME_TYPE, AEEDCF_MIMETYPE_MAXLEN);

    /*
     * Finally, allocate space for the filename and set it
     */
    pContent1FileName = (char*) MALLOC
                                  ( STRLEN ( CONTENT_1_DCF_FILE_NAME )
                                    +
                                    1 /* Terminating null */
                                  );
    if ( pContent1FileName == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          STRLEN ( CONTENT_1_DCF_FILE_NAME ) + 1, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1FileName[0] = '\0';

    StrErr = STRLCPY ( pContent1FileName, CONTENT_1_DCF_FILE_NAME,
        STRLEN ( CONTENT_1_DCF_FILE_NAME ) + 1 );

    if ( StrErr == -1 )
    {
      MSG_ERROR ( "FAIL: Can't copy filename.  Buffer too small.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }

    /*
     * Now, simply call the downloader function for forward lock that we
     * created before!
     */
    Errno = HandleForwardLock
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent1FileName,
                pContent1URI,
                pContent1MimeType,
                pContent1,
                Content1Length
              );

    /*
     * Check the returned error code to see if we successfully 'downloaded' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't download fwd lock media.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the action mask we want with this media object in preparation for a
     * 'player'
     */
    Action = AEEDRM_ACTION_EXECUTE;

    /*
     * Now, play the media!
     */
    Errno = DRMPlayer
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent1FileName,
                Action
              );

    /*
     * Check the returned error code to see if we successfully 'played' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't play fwd lock media.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * We have now successfully 'downloaded' and 'played' a fwd lock media.
     * Congratulations! :-)
     */




    /*
     * COMBINED DELIVERY MEDIA OBJECT
     *
     * A combined delivery media object is one in which the rights are
     * transmitted along with the unprotected content.  Unlike the forward
     * lock media object, the combined delivery media object does not grant
     * the user with unconstrained usage rights.  In fact, the user is limited
     * according to the rights shipped with the media object.
     *
     * HTTP/1.1 200 OK
     * Content-type: application/vnd.oma.drm.message;
     * boundary=boundary-1
     * Content-Length: 893
     *
     * --boundary-1
     * Content-type: application/vnd.oma.drm.rights+xml
     * Content-Transfer-Encoding: binary
     *
     * <o-ex:rights 
     *      xmlns:o-ex="http://odrl.net/1.1/ODRL-EX" 
     *      xmlns:o-dd="http://odrl.net/1.1/ODRL-DD" >
     *    <o-ex:context>
     *        <o-dd:version>1.0</o-dd:version>
     *    </o-ex:context>
     *    <o-ex:agreement>
     *        <o-ex:asset>
     *            <o-ex:context>
     *                <o-dd:uid>cid:4567829547@foo.bar</o-dd:uid>
     *            </o-ex:context>
     *        </o-ex:asset>
     *        <o-ex:permission>
     *            <o-dd:display/>
     *        </o-ex:permission>
     *    </o-ex:agreement>
     *  </o-ex:rights>
     *
     *  --boundary-1
     *  Content-type: image/jpeg
     *  Content-ID: <45678929547@foo.bar>
     *  Content-Transfer-Encoding: binary
     *
     *  ...jpeg image in binary format...
     * --boundary-1--
     *
     * Notice that the HTTP stream has two sections:
     * a) Rights section
     * b) Content section
     *
     * In a complete system, the downloader would parse the HTTP stream and
     * separate the two different components.  The media rights
     * (described in XML) would be passed, as is, to the IDRM_AddRight()
     * function call.  The following information is required for the content:
     *
     * Content-ID
     * Content-Type (Mime Type)
     * Content-Length
     * Content itself
     *
     * These pieces of information are extracted by the downloader from the
     * second half of the HTTP stream and are passed to the DRM stack.
     *
     * In our case, we are creating a standalone system, so we:
     *
     * a) Create a rights object (instead of downloading it)
     * b) Create a piece of content, associated URI, Mime-Type and Length
     *    (instead of getting the information from the downloader)
     */

    /*
     * The next step is to create a piece of content.  For our purposes, the
     * content will be an opaque memory blob.  We must also set its size
     */
    pContent2 = (uint8*) MALLOC ( CONTENT_2_LENGTH );
    if ( pContent2 == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CONTENT_2_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    Content2Length = CONTENT_2_LENGTH;

    /*
     * Let's initialize the content to some known value
     */
    (void) MEMSET ( pContent2, 1, Content2Length );

    /*
     * The next step is to allocate space for the URI and mime type
     */
    pContent2URI = (char*) MALLOC ( AEEDRM_MAX_URI_LEN );
    if ( pContent2URI == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_MAX_URI_LEN bytes.", 0,
          0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2URI[0] = '\0';

    pContent2MimeType = (char*) MALLOC ( AEEDCF_MIMETYPE_MAXLEN );
    if ( pContent2MimeType == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_MIMETYPE_MAXLEN\
          bytes.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2MimeType[0] = '\0';

    /*
     * Set the URI and mime types of content 2
     */
    (void) STRLCPY ( pContent2URI, CONTENT_2_URI, AEEDRM_MAX_URI_LEN);
    (void) STRLCPY ( pContent2MimeType, CONTENT_2_MIME_TYPE, AEEDCF_MIMETYPE_MAXLEN);

    /*
     * Finally, allocate space for the filename and set it
     */
    pContent2FileName = (char*) MALLOC
                                  ( STRLEN ( CONTENT_2_DCF_FILE_NAME )
                                    +
                                    1 /* Terminating null */
                                  );
    if ( pContent2FileName == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          STRLEN ( CONTENT_2_DCF_FILE_NAME ) + 1, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2FileName[0] = '\0';

    StrErr = STRLCPY ( pContent2FileName, CONTENT_2_DCF_FILE_NAME,
        STRLEN ( CONTENT_2_DCF_FILE_NAME ) + 1 );

    if ( StrErr == -1 )
    {
      MSG_ERROR ( "FAIL: Can't copy filename.  Buffer too small.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }

    /*
     * Now we have to create a right to use with this content
     *
     * In your application you don't _have_ to do this at all.  Rights are
     * created by the content supplier.  In the case of combined delivery,
     * the rights are delivered at the same time with the unencrypted
     * content
     */

    /*
     * Let's create a right with:
     *    5 display counts
     *    3 print counts
     */

    /*
     * First, lets clear the template out to make sure there aren't any issues
     */
    (void) MEMSET ( &PermissionsTemplate, 0,
        sizeof ( AEEDRM_PermissionTemplateType ) );

    /*
     * Now, set the constraints
     */
    PermissionsTemplate.Display.IsValid = TRUE;
    PermissionsTemplate.Display.TotalCount = 5;
    PermissionsTemplate.Print.IsValid = TRUE;
    PermissionsTemplate.Print.TotalCount = 3;

    /*
     * Allocate memory for an OMA 1.0 rights object and set its size
     */
    pOMARightsObject = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARightsObject == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_XML_BUFFER_SIZE bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightsObjectLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Use this template to create a right
     */
    Errno = IDRM_CreateRight
              ( pIDRM,
                pContent2URI,
                PermissionsTemplate,
                pOMARightsObject,
                &OMARightsObjectLength
              );
    /*
     * At the end of this call, OMARightsObjectLength is set to the actual
     * length of the created OMA 1.0 rights object
     */

    /*
     * Check for error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create combined delivery right.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Now, simply call the downloader function for combined delivery that we
     * created before!
     */
    Errno = HandleCombinedDelivery
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent2FileName,
                pContent2URI,
                pContent2MimeType,
                pContent2,
                Content2Length,
                pOMARightsObject,
                OMARightsObjectLength,
                RELVersion
              );

    /*
     * Check the returned error code to see if we successfully 'downloaded' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't download cmbined delivery media.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the action mask we want with this media object in preparation for a
     * 'player'
     */
    Action = AEEDRM_ACTION_DISPLAY;

    /*
     * Now, play the media!
     */
    Errno = DRMPlayer
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent2FileName,
                Action
              );

    /*
     * Check the returned error code to see if we successfully 'played' a
     * combined delivery media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't play fwd lock media.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * We have now successfully 'downloaded' and 'played' a combined delivery
     * media
     * Congratulations! :-)
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** DRMDownloadPlayerApp FAILED *****", 0, 0, 0 );
    MSG_ERROR ( "***** Error Code Returned: %d     *****", Errno, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** DRMDownloadPlayerApp PASSED *****", 0, 0, 0 );
  }



  /*
   * Either way, today, we're going to have to clean up.
   *
   * To clean up we'll be:
   *  1) Freeing memory (as we always should!)
   *  2) Deleting the rights (NOTE)
   *  3) Deleting the content (NOTE)
   *
   * NOTE: In a normal application, we wouldn't want to do this (after all,
   *       the user paid for those rights and content!)
   */

  /*
   * Create the file manager interface
   */
  if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
        ( void** )(&pIFileMgr) )
      !=
      SUCCESS )
  {
    MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
    Errno = EFAILED;
    /* HMMMMM */
    /* FIXME */
  }

  /*
   * Content #1
   */

  /*
   * Delete the rights
   */
  IDRM_DeleteAssociatedRights
    ( pIDRM,
      CONTENT_1_URI,
      &DeleteAssocRsp,
      NULL
    );

  if ( DeleteAssocRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete CONTENT_1_URI rights", 0, 0, 0 );
  }

  /*
   * Delete the .dcf file
   */
  Errno = IFILEMGR_Remove
            ( pIFileMgr,
              pContent1FileName
            );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't remove Content # 1 .dcf file.  Error #: %d.",
        Errno, 0, 0 );
  }

  /*
   * No need to keep the memory we've allocated around
   */
  FREE ( pContent1 );
  FREE ( pContent1URI );
  FREE ( pContent1MimeType );
  FREE ( pContent1FileName );

  /*
   * Content # 2
   */

  /*
   * Delete the rights
   */
  IDRM_DeleteAssociatedRights
    ( pIDRM,
      CONTENT_2_URI,
      &DeleteAssocRsp,
      NULL
    );

  if ( DeleteAssocRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete CONTENT_2_URI rights", 0, 0, 0 );
  }

  /*
   * Delete the .dcf file
   */
  Errno = IFILEMGR_Remove
            ( pIFileMgr,
              pContent2FileName
            );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't remove Content # 2 .dcf file.  Error #: %d.",
        Errno, 0, 0 );
  }

  /*
   * No need to keep the memory we've allocated around
   */
  FREE ( pContent2 );
  FREE ( pContent2URI );
  FREE ( pContent2MimeType );
  FREE ( pContent2FileName );
  FREE ( pOMARightsObject );



  /*
   * Finally, release all the interfaces
   */
  (void) IDRM_Release ( pIDRM );
  (void) IDCF_Release ( pIDCF );
  (void) IFILEMGR_Release ( pIFileMgr );
}
#endif  /* FEATURE_SEC_DRM */

