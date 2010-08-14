#include "comdef.h"
#include "armasm.h"
#include "boothw.h"
#include "target.h"
#include "customer.h"
#include "GlobalData.h"
#include "task.h"

uint32 g_BootResetFlag;

#ifdef CUST_EDITION
/*---------------------------------------------------------------------------

                     USER INTERFACE TASK DECLARATIONS

---------------------------------------------------------------------------*/

rex_tcb_type        ui_tcb;                      /* TCB for UI task.       */
rex_stack_word_type ui_stack [UI_STACK_SIZ];     /* Stack for UI Task.     */

#endif

