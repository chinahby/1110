
/*===========================================================================

        P M   A P P L I C A T I O N   T I M E R   D E F I N I T I O N 
                   I M P L E M E N T A T I O N   F I L E  

DESCRIPTION  
  This file contains the implementation of the generic timer type definitions
  for use by application code provided with the software library for the
  Qualcomm Power Management Chip Set.

Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_timer.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/08   aab     Fixed Lint warnings
08/01/06   cng     Fixed Lint warnings
05/31/06   Vish    Fixed LINT warnings.
12/12/05   Vish    Replaced single master timeout callback function with unique
                   ones for each individual timer.
09/03/05   Vish    Incorporated a wrapper around basic CLK/TIME services to
                   provide a mechanism for reliable detection/cancellation
                   of timeouts.
01/25/05   Vish    Created.
===========================================================================*/


/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "pmapp_timer.h"
#include "err.h"
#include "assert.h"


/*===========================================================================
             LOCAL TYPE DEFINITIONS / FUNCTION PROTOTYPES
===========================================================================*/
static void pm_app_timer_set_token(
   pm_app_timer_token_type   *token_ptr,
   uint16                     index,
   pm_app_timer_sess_id_type  tmo_sess_id);

static void pm_app_timer_get_from_token(
   const pm_app_timer_token_type   *token_ptr,
   uint16                          *index_ptr,
   pm_app_timer_sess_id_type       *tmo_sess_id_ptr);

static void pm_app_timer_master_tmo_cb(
#ifdef FEATURE_USE_TIME_VU
   int32 delay,
   timer_cb_data_type data
#else
   int32 delay,
   void *data
#endif //FEATURE_USE_TIME_VU
   );

typedef pm_app_timer_type *pm_app_timer_ptr_type;

typedef void (*pm_app_timer_system_callback_fn_ptr_type) (
#ifdef FEATURE_USE_TIME_VU
   int32 delay,
   timer_cb_data_type data
#else
   int32 delay,
   void *data
#endif //FEATURE_USE_TIME_VU
);


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/
#define PM_APP_TIMER_COUNT_MAX_LIMIT  (20)      // set/increase as appropriate

static pm_app_timer_ptr_type pm_app_timer_ptr_list[PM_APP_TIMER_COUNT_MAX_LIMIT];
static uint16 pm_app_timer_count = 0;

//lint -esym(551, pm_app_timer_overall_session_count) suppress "symbol not accessed"
// Reason: added that just for debugging
static uint32 pm_app_timer_overall_session_count = 0;

#ifdef FEATURE_USE_TIME_VU
static timer_group_type  pm_app_timer_group;
#endif //FEATURE_USE_TIME_VU

#ifdef FEATURE_USE_TIME_VU
#define PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(i)  \
   static void pm_app_timer_uniq_callback_fn_##i(int32 delay, timer_cb_data_type data) { pm_app_timer_master_tmo_cb(delay, data); }
#else
#define PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(i)  \
   static void pm_app_timer_uniq_callback_fn_##i(int32 delay, void *data) { pm_app_timer_master_tmo_cb(delay, data); }
#endif //FEATURE_USE_TIME_VU

PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(0)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(1)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(2)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(3)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(4)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(5)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(6)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(7)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(8)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(9)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(10)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(11)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(12)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(13)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(14)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(15)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(16)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(17)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(18)
PM_APP_TIMER_GENERATE_UNIQ_SYSTEM_CALLBACK_FN_CODE(19)

static pm_app_timer_system_callback_fn_ptr_type pm_app_timer_system_callback_fn_list[PM_APP_TIMER_COUNT_MAX_LIMIT]
 = {
      pm_app_timer_uniq_callback_fn_0,
      pm_app_timer_uniq_callback_fn_1,
      pm_app_timer_uniq_callback_fn_2,
      pm_app_timer_uniq_callback_fn_3,
      pm_app_timer_uniq_callback_fn_4,
      pm_app_timer_uniq_callback_fn_5,
      pm_app_timer_uniq_callback_fn_6,
      pm_app_timer_uniq_callback_fn_7,
      pm_app_timer_uniq_callback_fn_8,
      pm_app_timer_uniq_callback_fn_9,
      pm_app_timer_uniq_callback_fn_10,
      pm_app_timer_uniq_callback_fn_11,
      pm_app_timer_uniq_callback_fn_12,
      pm_app_timer_uniq_callback_fn_13,
      pm_app_timer_uniq_callback_fn_14,
      pm_app_timer_uniq_callback_fn_15,
      pm_app_timer_uniq_callback_fn_16,
      pm_app_timer_uniq_callback_fn_17,
      pm_app_timer_uniq_callback_fn_18,
      pm_app_timer_uniq_callback_fn_19
   };


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/
void pm_app_timer_def(
   pm_app_timer_type *new_timer_ptr)
{
   dword                      int_sav_token;
   uint16                     index;
   pm_app_timer_sess_id_type  tmo_sess_id;

   // This timer should not already be on my list
   // If it is on my list already, then it should be stopped so as to start
   //   on a clean slate.
   ASSERT( new_timer_ptr );
   pm_app_timer_get_from_token(
      &(new_timer_ptr->__token),
      &index,
      &tmo_sess_id);

   if ( (index < PM_APP_TIMER_COUNT_MAX_LIMIT) && (new_timer_ptr == pm_app_timer_ptr_list[index]) )
   {
      // already defined (initialized) timer - just stop it
      pm_app_timer_stop(
         new_timer_ptr);
   }
   else
   {
      INTLOCK_SAV(int_sav_token);
      if (pm_app_timer_count >= PM_APP_TIMER_COUNT_MAX_LIMIT)
      {
         INTFREE_SAV(int_sav_token);
         ERR_FATAL("Max timer capacity exceeded; bump up PM_APP_TIMER_COUNT_MAX_LIMIT", 0, 0, 0);
         return; //lint !e527 suppress "Unreachable code at token 'return'"
                 // Reason: We don't really care about the internals of ERR_FATAL();
                 //         the main idea is to make the logical flow of this fn clear.
      }
      else
      {
         pm_app_timer_ptr_list[pm_app_timer_count] = new_timer_ptr;
         pm_app_timer_set_token(
            &(new_timer_ptr->__token),
            pm_app_timer_count, // timer index into our local array
            0);                 // session ID = 0 when def'ing for the first/only time
         pm_app_timer_count ++;
         INTFREE_SAV(int_sav_token);
      }

      new_timer_ptr->__client_cb = NULL;

#ifdef FEATURE_USE_TIME_VU
      timer_def2(
         &(new_timer_ptr->__core_timer),
         &pm_app_timer_group );
#else
      clk_def(
         &(new_timer_ptr->__core_timer) );
#endif //FEATURE_USE_TIME_VU

   }
}


pm_app_timer_sess_id_type pm_app_timer_start(
   pm_app_timer_type           *my_timer_ptr,
   pm_app_timer_cb_func_type    tmo_cb,
   int32                        tmo_duration)
{
   uint16                     index;
   pm_app_timer_sess_id_type  tmo_sess_id;

   INTLOCK();

   // This timer must already be on my list
   ASSERT( my_timer_ptr );
   pm_app_timer_get_from_token(
      &(my_timer_ptr->__token),
      &index,
      &tmo_sess_id);
   ASSERT(index < PM_APP_TIMER_COUNT_MAX_LIMIT);
   ASSERT(my_timer_ptr == pm_app_timer_ptr_list[index]);

   tmo_sess_id ++;
   pm_app_timer_set_token(
      &(my_timer_ptr->__token),
      index,
      tmo_sess_id);

   my_timer_ptr->__client_cb = tmo_cb;
   ASSERT(index < PM_APP_TIMER_COUNT_MAX_LIMIT);

   // starting an already running timer is equivalent to restarting it
#ifdef FEATURE_USE_TIME_VU
   (void)timer_clr(
      &(my_timer_ptr->__core_timer),
      T_NONE );
   timer_reg(
      &(my_timer_ptr->__core_timer),
      pm_app_timer_system_callback_fn_list[index], //lint !e661 ASSERT macro above has checked out-of-bound access
      my_timer_ptr->__token,
      tmo_duration,   //lint !e732 suppress "Loss of sign (long to unsigned long"
                      // Reason: Unfortunately timer_reg() and clk_reg2() use
                      //         2 different data types for representing the timeout
                      //         duration. We have chosen the most restrictive type
                      //         to suit either one.
      0 );
#else
   clk_dereg(
      &(my_timer_ptr->__core_timer) );
   clk_reg2(
      &(my_timer_ptr->__core_timer),
      pm_app_timer_system_callback_fn_list[index], //lint !e661 ASSERT macro above has checked out-of-bound access
      tmo_duration,
      0,
      FALSE,
      (void*)my_timer_ptr->__token );
#endif //FEATURE_USE_TIME_VU

   INTFREE();

   return tmo_sess_id;
}


void pm_app_timer_invalidate_sess_id(
   pm_app_timer_sess_id_type   *timer_sess_id_ptr)
{
   if (timer_sess_id_ptr)
   {
      (*timer_sess_id_ptr) --;
   }
}


void pm_app_timer_stop(
   pm_app_timer_type           *my_timer_ptr)
{
   uint16                     index;
   pm_app_timer_sess_id_type  tmo_sess_id;

   INTLOCK();

   // This timer must already be on my list
   ASSERT( my_timer_ptr );
   pm_app_timer_get_from_token(
      &(my_timer_ptr->__token),
      &index,
      &tmo_sess_id);
   ASSERT( (index < PM_APP_TIMER_COUNT_MAX_LIMIT) && (my_timer_ptr == pm_app_timer_ptr_list[index]) );

   tmo_sess_id ++;
   pm_app_timer_set_token(
      &(my_timer_ptr->__token),
      index,
      tmo_sess_id);

   my_timer_ptr->__client_cb = NULL;

#ifdef FEATURE_USE_TIME_VU
   (void)timer_clr(
      &(my_timer_ptr->__core_timer),
      T_NONE );
#else
   clk_dereg(
      &(my_timer_ptr->__core_timer) );
#endif //FEATURE_USE_TIME_VU

   INTFREE();
}


static void pm_app_timer_set_token(
   pm_app_timer_token_type   *token_ptr,
   uint16                     index,
   pm_app_timer_sess_id_type  tmo_sess_id)
{
   *token_ptr = ( (uint32) ((uint16)tmo_sess_id << 16) ) + index;
}


static void pm_app_timer_get_from_token(
   const pm_app_timer_token_type   *token_ptr,
   uint16                          *index_ptr,
   pm_app_timer_sess_id_type       *tmo_sess_id_ptr)
{
   *index_ptr = (uint16) ((*token_ptr) & 0xffff);
   *tmo_sess_id_ptr = (uint16) ( ((*token_ptr) >> 16) & 0xffff );
}


static void pm_app_timer_master_tmo_cb(
#ifdef FEATURE_USE_TIME_VU
   int32 delay,
   timer_cb_data_type data
#else
   int32 delay,
   void *data
#endif //FEATURE_USE_TIME_VU
   )
{
   pm_app_timer_token_type    timer_token = (pm_app_timer_token_type) data;
   uint16                     index;
   pm_app_timer_sess_id_type  tmo_sess_id;
   pm_app_timer_cb_func_type  client_cb_copy = NULL;

   pm_app_timer_overall_session_count ++;

   pm_app_timer_get_from_token(
      &timer_token,
      &index,
      &tmo_sess_id);
   ASSERT( (index < PM_APP_TIMER_COUNT_MAX_LIMIT) );

   if ( timer_token == pm_app_timer_ptr_list[index]->__token )  //lint !e661 ASSERT macro above has checked out-of-bound access
   {
      INTLOCK();
      client_cb_copy = pm_app_timer_ptr_list[index]->__client_cb;  //lint !e661 ASSERT macro above has checked out-of-bound access
      INTFREE();
      if ( client_cb_copy )
      {
         client_cb_copy(
            pm_app_timer_ptr_list[index],  //lint !e661 ASSERT macro above has checked out-of-bound access
            tmo_sess_id,
            delay );
      }
   }
}


