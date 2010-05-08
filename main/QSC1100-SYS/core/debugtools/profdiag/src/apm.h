
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           A H B  P e r f o r m a n c e  M o ni t o r   D r i v e r
          
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc6055/services/profdiag/apm.h#1 $  $DateTime: 2007/01/10 13:51:34 $  $Author: fserrao $ 


 when      who     what, where, why
 --------  -----   ----------------------------------------------------------
 09/06/06  cc      Creat the file.

 
============================================================================*/

/*============================================================================

                       INCLUDE FILES FOR MODULE
                       
============================================================================*/
#include "comdef.h"
#include "log.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/



/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

PACKED void * apm_get_cfg_tbl_addr(
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
);


PACKED void * apm_start_log(
  PACKED void *in_req_ptr,     /* pointer to request packet  */
         word pkt_len          /* length of request packet   */
);

/*===========================================================================

FUNCTION  APM_INIT

DESCRIPTION
   Initialize data structure for AHB BUS performance Moniter driver

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void apm_init(void);
