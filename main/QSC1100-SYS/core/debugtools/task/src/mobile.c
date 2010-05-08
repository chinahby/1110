/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Main procedure for the DMSS Mobile software.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2005             by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc1100/services/task/mobile.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/09   c_kumarg Version Q1100ASBNAALZ3.3.50 Q1100_SURF_BurstNOR_NonPBoot_Lowtier_StripAndPacked
06/21/07   gparekh Version Q6055ASBAXP3.0.44T SBAXP, QSC6055, w/Enc, NOR + PSRAM mem, L4 Microkernel, 6055 Pre-paid
06/07/07   ts      Assigned 0 to re_curr_task to avoid compiler warning.
05/30/07   ts      Fixed code for core dumps.  Also, included rex_curr_task
                   so scripts work when restoring core dump from script.
03/31/07   sbin    Version Q6055ASBAXL2.0.10T SBAXL, QSC6055, w/Enc, NOR + PSRAM mem, L4 Microkernel, 6055 Standard
06/08/06   cc      change for FEATURE_ERR_EXTENDED_STORE
05/05/06   archanag Version M6800AFMBRLA4.2.10T FFA6800-RF6500,  Unstrip, Encrypt, 32-bit SDRAM+16-bit NAND, EBPI, Enh Band Class, L4 MicroKernel, Baseline
12/01/05    wd     Added a better decoding mechinism for exceptions.
10/04/05    wr     Added mobile exception handler for AMSS on L4.
10/18/02   jct     Moved diag_guid to here and renamed mob_guid.  GUID is now
                   optionally passed in externally
09/09/02   jct     Changed way build info is passed in from make file
08/29/02   jct     Added conditional compilation for BUILD_VERSION so that
                   a release version can be embedded in this file for reference
07/12/02   jct     Created from all previous versions removing lots of unused
                   information and correcting others
===========================================================================*/

#include <ctype.h>      
#include <locale.h>    
#include "armasm.h"    
#include "target.h"     
#include "customer.h"   
#include "comdef.h"     
#include "rex.h" 
#include "rexlog.h"
#include "task.h"       
#include "mc.h"         
#include "mobile.h"     
#include "msg.h"
#include "boot_stacks.h"
#include "AEEstd.h"

#if defined(FEATURE_ERR_EXTENDED_STORE) && defined(FEATURE_L4)
#error code not present
#endif

#ifdef FEATURE_ERR_EXTENDED_STORE
static rex_tcb_type *rex_curr_task;
#endif

extern int sprintf(char *destination, const char * format, ...);

/* ==========================================================================
** Version Information 
**   Version Date and Time - when compiled
**   Version Directory     - QCT CRM four letter build id
**   Version Number        - QCT CRM four number build version
**   Release Date          - Same as version
**   Release Time          - Same as version
**   Copyright             - Copyright info placed in ROM
**   Authors               - Tribute to the development teams
**   Version String        - QCT CRM fully qualified version 
** ========================================================================== */

NOTICE_TYPE ver_date[] = __DATE__;
NOTICE_TYPE ver_time[] = __TIME__;
NOTICE_TYPE ver_dir[]  = BUILD_TARGET BUILD_VER;

NOTICE_TYPE rel_date[] = "Jun 25 2009";
NOTICE_TYPE rel_time[] = "11:00:00";

NOTICE_TYPE copyright[] =
 "Copyright  (c) 1995,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005 "
 "by QUALCOMM, Incorporated. "
 "All Rights Reserved.";

NOTICE_TYPE authors[]=
  "Authors: QCT";

/* Mobile Firmware Marketing Software Revision
*/
#ifdef T_QSC60X5REV1_0
#ifdef T_QSC6085
   NOTICE_TYPE mob_sw_rev[] = "Q6085A" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6075)
   NOTICE_TYPE mob_sw_rev[] = "Q6075A" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6065)
   NOTICE_TYPE mob_sw_rev[] = "Q6065A" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6055)
   NOTICE_TYPE mob_sw_rev[] = "Q6055A" BUILD_TARGET BUILD_VER;
#else
   NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;
#endif
#elif defined (T_QSC60X5REV1_1)
#ifdef T_QSC6085
   NOTICE_TYPE mob_sw_rev[] = "Q6085B" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6075)
   NOTICE_TYPE mob_sw_rev[] = "Q6075B" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6065)
   NOTICE_TYPE mob_sw_rev[] = "Q6065B" BUILD_TARGET BUILD_VER;
#elif defined (T_QSC6055)
   NOTICE_TYPE mob_sw_rev[] = "Q6055B" BUILD_TARGET BUILD_VER;
#else
   NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;
#endif
#elif defined (T_QSC1110)
   NOTICE_TYPE mob_sw_rev[] = "Q1110-" BUILD_TARGET "-" BUILD_VER;
#else
   NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;
#endif

/* GUID
*/
#ifdef BUILD_GUID
   mob_guid_type mob_guid = { BUILD_GUID };
#else
   mob_guid_type mob_guid = { 0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10 };
#endif

/* ==========================================================================
** Air Standard Identifiers
** ========================================================================== */

/* Station Class Mark
*/
const byte NOTICE_SEG_TYPE station_class_mark =
  SCM_RSVD_80_V |                       /* Reserved                    */
#ifdef FEATURE_ACP
#error code not present
#else
  SCM_CDMA_ONLY_V |                     /* CDMA Only phone             */
#endif
#ifdef T_SLEEP
  SCM_SLOTTED_V |                       /* Slotted                     */
#else
  SCM_NON_SLOTTED_V |                   /* Non-Slotted                 */
#endif
  SCM_IS54_POWER_CLASS_V |              /* IS-54 Power class           */
  SCM_25_MHZ_BANDWIDTH_V |              /* 25 MHz bandwidth            */
  SCM_CONTINOUS_V |                     /* Continuous transmission     */
  SCM_POWER_CLASS_III_V                 /* Power Class III (portable)  */
  ;

/* Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/
const byte NOTICE_SEG_TYPE slot_cycle_index = 0x01;

/* Mobile CAI Revision (CAI Section 6.7.4.7)
*/
const byte NOTICE_SEG_TYPE mob_cai_rev = MOB_P_REV;

/* Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
const word NOTICE_SEG_TYPE mob_firm_rev = 4318;

#ifdef FEATURE_GPSONE
#error code not present
#endif /* GPSONE */

/* Mobile Model (CAI Section 6.7.4.7)
**    Value provided by cust*.h file
*/
#ifndef CUST_MOB_MODEL
   #error Must specify a CUST_MOB_MODEL number
#endif

const byte NOTICE_SEG_TYPE mob_model = CUST_MOB_MODEL;

#ifdef BOOT_TARGET
/*===========================================================================
FUNCTION C_ENTRY

DESCRIPTION
  This is the application entry point for bootable.

DEPENDENCIES
  None

RETURN VALUE
  None, this routine does not return

SIDE EFFECTS
  None
===========================================================================*/
int C_Entry( void )
{
   uint32 count;
   uint32 leds = 0;
   for(;;)
   {
     MSG_ERROR("Entered Application main",0,0,0);
     outp(LEDS,leds++);
     for ( count = 0; count < 1000000; count++);
     if ( leds == 1)
     {
        MSG_FATAL("Once only",0,0,0);
     }
   }
   return 0;
} /* END main */

void lcd_message (char *message)
{
}

#else

#if defined(FEATURE_L4)
#error code not present
#endif /* FEATURE_L4 */

/*===========================================================================
FUNCTION MAIN

DESCRIPTION
  This procedure uses the rex_init procedure to call the mc_task task.

DEPENDENCIES
  None

RETURN VALUE
  None, this routine does not return

SIDE EFFECTS
  None
===========================================================================*/
int main( void )
{
#if defined(FEATURE_MP4_DECODER)
  /* This is needed for C-library functions such as stroul */
  setlocale(LC_ALL, "C");
#endif

  #ifdef FEATURE_MULTIMODE_ARCH
  rex_init( (void *)irq_stack,  /* Interrupt stack                         */
            IRQ_Stack_Size,     /* Interrupt stack size                    */
            &tmc_tcb,            /* Task TCB of the TMC task               */
            (void *)tmc_stack,   /* Stack for TMC task                     */
            TMC_STACK_SIZ,       /* TMC stack size                         */
            TMC_PRI,             /* TMC priority                           */
            tmc_task,            /* Entry point for TMC task               */
            0L );               /* Parameter to pass the TMC task          */
  
  #else /* !FEATURE_MULTIMODE_ARCH */
  rex_init( (void *)irq_stack,  /* Interrupt stack                         */
            IRQ_Stack_Size,     /* Interrupt stack size                    */
            &mc_tcb,            /* Task TCB of the main control task       */
            (void *)mc_stack,   /* Stack for the main control task         */
            MC_STACK_SIZ,       /* Main control task stack size            */
            MC_PRI,             /* Main control task priority              */
            mc_task,            /* Entry point for the main control task   */
            0L );               /* Parameter to pass the main control task */
  #endif /* FEATURE_MULTIMODE_ARCH */

#if defined(FEATURE_L4)
#error code not present
#endif

#ifdef FEATURE_ERR_EXTENDED_STORE
/* Added this variable to allow the core dump scripts to work properly
   The problem is the existing software (Diag) creates the script command
   that will cause the script invocation to fail when restoring the core 
   dump. This assignment is to get rid of compiler warning                 */
rex_curr_task = 0;
#endif
   return 0;
} /* END main */

#endif /* BOOT_TARGET */
