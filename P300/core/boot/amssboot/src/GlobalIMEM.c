#include "comdef.h"
#include "armasm.h"
#include "boothw.h"
#include "target.h"
#include "customer.h"
#include "GlobalData.h"
#include "task.h"

rex_stack_word_type ui_stack [UI_STACK_SIZ];     /* Stack for UI Task.     */
rex_stack_word_type nv_stack [NV_STACK_SIZ];     /* Stack for NV task.     */
rex_stack_word_type wms_stack [WMS_STACK_SIZ];
rex_stack_word_type cm_stack [CM_STACK_SIZ];     /* Stack for CM Task.     */


