/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   SIM/USIM  Diagnostic Interface


Supports automated testing of SIM/USIM/RUIM cards via DIAG commands.
Implements packet processing.

Copyright (c) 2003-2009 , 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/su/baselines/sqa/mmgsdi/rel/07H1_2/src/gsdidiag_c/rev21/gsdidiag.c#2 $ $DateTime: 2011/03/08 02:14:45 $ $Author: amitp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/16/11   yt      Handle case where EFS filename string does not end in NULL
07/01/09   rn      Fixed memory leaks
05/21/09   js      Fixed memory alignment issue in gsdidiag_sim_get_file_attributes
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
05/04/09   js      Fixed warnings
04/29/09   js      Added prototypes for static functions, fixed warnings
04/28/09   js      Fixed compilation error
04/28/09   js      Support for gsdi diag perso get feature data command
04/15/09   js      Provision for sending delayed response for mmgsdi api failure
03/31/09   js      Workaround for a misalignment when processing
                   gsdidiag_sim_read() and gsdidiag_sim_write()
03/19/09   js      Support for Enhanced NAA Refresh by file path
03/06/09   tml     Limited to 1 internal gsdidiag mmgsdi client id registration
03/04/09   mib     Get perso keys if FEATURE_MMGSDI_PERSO_GET_KEY not defined
03/02/09   mib     Pass original refresh mode to script
02/27/09   mib     Added support for gsdi_sim_get_cphs_information
02/23/09   mib     Fixed compilation warnings
02/13/09   mib     Delayed implementation of gsdi perso commands
01/20/09   nb      Fixed SIM_READ failure in scripts ADN_X_TIMES
01/16/09   nb      Replaced EFS1 API calls with EFS2 APIs
01/12/09   tml     Fixed refresh client index hardcode issue and potential
                   req_table fill up and lint fixes
11/19/08   nb      Klockwork Fixes
10/09/08   tml     Fixed compilation warnings
10/11/08   ssr     Adding memory free code for MMGSDI_SEARCH_REQ
09/11/08   sp      Fixed warning
09/10/08   sp      Fixed lint error
09/05/08   nb      Klockwork Fixes
08/27/08   sp      Added support for get_dck_num_retries
08/26/08   kk      Fix for pkcs15 create table call back
08/11/08   tml     Added GSTK diag support
08/08/08   vs      Fixed compile errors related to featurization
07/31/08   kk      PKCS15 lookup table updated with gsdidiag enum types
07/29/08   SSR     Put search code under FEATURE_MMGSDI_TEST_AUTOMATION
07/20/08   sp      Support for gsdidiag delayed sim increase
07/18/08   sun     Added Client ID information to Search command
07/17/08   kk      updated pkcs15 lookup table rsp handling
07/17/08   kk      Added support for PKCS 15
07/16/08   jk      Fixed compile warning
07/02/08   ssr     Support of MMGSDI Search functionality
06/23/08   tml     Added featurization for BT in WM
06/20/08   kk      Updated Refresh_complete and Naa refresh trigger commands
                   to use correct client_id, one received in the payload.
06/08/08   tml     Fixed sim read search string payload calculation
04/10/08   nk      Fixed Compiler Warning.
03/17/08   nk      Fixed Klokwork issues.
02/15/08   kk      Added support for MMGSDI event handling and other cmds
                   for handling refresh
11/13/07   sun     Added support for NAA Refresh
08/16/07   js      Removed use of strcpy featurized under LTK. Used std_strlcpy in place
06/29/07   sun     Fixed data for sap based on new configuration
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
04/26/07   sp      Featurized for LTK
04/12/07   sp      Fixed buffer overflow
04/13/07   sun     Fixed Buffer Overflow
03/27/07   sp      Initialize reqest to zero before assigning any value
03/14/07   nk      Fixed compiler warning
03/07/07   sun     Added support for Disable with Replacement
03/07/07   sp      Fixed compiler warning
02/28/07   sp      Security check for buffer overflow/underflow
02/28/07   nk      Removed the usage of banned apis
02/22/07   tml     Word alignment fixes
02/08/07   nk      Fixed write diag efs filename bug
02/02/07   sp      Security check for buffer overflow/underflow
01/29/07   sp      Added null pointer check
01/30/07   sun     Added support for Checking Service Available
01/23/07   nk      Moved gsdidiag_sim_malloc_append_efs_dir out of optimization
                   feature.
01/15/07   nk      Added support for efs mount to apps dir if feature defined
12/19/06   tml     Add support for path transparent and record access
10/26/06   sun     Fixed Compile Error
10/25/06   sun     Fixed Compile Error
10/17/06   tml     Fixed compilation warning on 1x
10/16/06   tml     Get All PIN Status diag support
10/11/06   jar     Fix to return error of FEATURE_MMGSDI_ONCHIP_SIM
                   is not defined.  Merged back in missing ONCHIP SIM
                   functionality.
10/11/06   sun     Added support for Multi USIM and Universal PIN
08/23/06   sun     Fixed Lint Errors
08/20/06   tml     Added App Reset and 3G Session Reset supports
08/07/06   tml     Added ACL support
08/09/06   tml     Lint fix
08/08/06   sun     Fixed Lint Error
08/02/06   tml     Add Registration for FCN support
08/02/06   sun     Fixed the length for the perso feature data in
                   set_feature_data
07/28/06   tml     Fixed compilation issue
07/28/06   sun     Added const to mmgsdi_cnf_type in mmgsdi_callback_type
07/13/06   jar     Lint Fix for Error 713 when FEATURE_MMGSDI_ONCHIP_SIM is
                   defined.
06/27/06   tml     Fixed compilation issue
06/16/06   tml     Fixed compilation warning
06/16/06   tml     Fixed compilation issue
06/15/06   tml     Fixed compilation warning
06/15/06   tml     Fixed compilation warning
06/12/06   tml     Lint and merge onchip sim support and compilation fix
06/06/06   sp      Update gfa_cb to return failure case.
06/02/06   tml     BDN support
05/31/06   sp      Updated gsdidiag_refresh_cb.
05/22/06   nk      Fixed Compiler Warning
05/18/06   jk      Added array bounds check to prevent overflow in
                   gsdidiag_sim_select
05/16/06   tml     compilation fix
05/15/06   tml     compilation fix merge
05/11/06   nk      Moved Prototype defs from .h file
05/08/06   nk      Featurized for code reduction
05/03/06   tml     Fixed lint error
04/21/06   tml     RPC compilation fix and lint fix
04/06/06   sun     Added support for RUIM Lock
04/05/06   tml     lint
03/24/06   tml     typo fix
03/23/06   tml     lint
03/10/06   tml     Lint fixes.
01/04/06   sp      Added support for sap_pin_verify
01/04/06   tml     Updated enum name for SAP and Card powerup and down
12/21/05   sp      Added support for Sap_Card_reader_Status and modified Sap_Disconnect
12/20/05   sun     Fixed signature for sap power on, power off and reset functions
12/08/05   sp      Added support for Sap_Init, Sap_reset and Sap_Deregister
11/30/05   tml     Added OTA support for SMS, lint fixes, potential
                   compilation fixes
12/06/05   sk      Fixed lint errors and compilation errors
12/02/05   sp      Proper Indentation
12/01/05   sp      Fixed Typo
11/30/05   sp      Added support for force_sim_read
11/17/05   tml     Added Delayed Response Run GSM Algorithm and Verify PIN
                   supports
11/10/05   tml     lint fix
09/29/05   tml     Client Deregistration support
08/28/05   tml     Added Read/Write/Get File Attributes with uint64 client id
08/29/05   tml     Fixed compilation
08/26/05   pv      Check for correct cnf types when response is received
08/25/05   pv      AT+CFU API support to power down and power up the card.
08/04/05   sun     Fixed one more memory leak
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/26/05   sst     Added MMGSDIBT support.
07/26/05   tml     ISIM Diag support
07/12/05   tml     Added client ID support
07/11/05   sun     Fixed Memory Leaks
06/25/07   pv      Fix usage of delayed response ID & move mmgsdi
                   related stuff to gsdidiag_uim.*
06/20/05   jar     Activate Onchip SIM Support
06/10/05   tml     256 + support and lint cleanup
05/10/05   pv      Addeed Sub command to pipe commands to UIM
05/11/05   sst     Lint fixes
05/05/05   sun     Added new command for GSDI REFRESH
05/02/05   jar     Sim Secure Read and Root Key ID Read Callback Fixes
05/01/05   jar     Additional commands added for Personalization Engine and
                   Non QC Solutions.
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
12/27/04   jk      Cleaned up compliler warnings.
11/23/04   jar     Code cleanup.
11/19/04   jk      Added support for dual slot testing
10/18/04   sk      Added support for gsdidiag_get_ecc and gsdidiag_
                   get_ecc_cb functions.
10/18/04   sk      Added gsdidiag_sim_usim_ruim search request and
                   callback APIs and the necessary support in the
                   gsdidiag_generic_command and gadidiag_get_payload
                   functions.
10/05/04   sk      Added Delayed_sim_read request and callback APIs.
09/07/04   jar     Added support for EF-PL under MF which resolves to EF_ELF
                   because they share the same FID (102.221 & 31.102)
09/05/04   jar     Merged in additional changes for building without FEATURE
                   _MMGSDI_CHANNEL_MANAGEMENT.
09/01/04   sk      Added fixes to allow for building without
                   FEATURE_MMGSDI_CHANNEL_MANAGEMENT
08/30/04   sk      Added Channel Management APIs for GetATR, OpenChannel,
                   CloseChannel,SendChannelData
08/26/04   ss      Added delayed response architecture for generic
                   command and change pin
08/11/04   tml     Added support for ADN, GAS, SNE, EMAIL and GRP
08/11/04   tml     Added support for get unblock key
08/09/04   jar     Added support for HPLMNACT for GSM
06/30/04   tml     Allowed diag to set zero length feature control data for
                   personalization
06/25/04   jar     Added Featurization around SIM Lock Events
06/24/04   jar     Modified code to work with mispelling in event_defs.h as a
                   result of an old email.
06/24/04   tml     Added personalization diag support
03/25/04   jar     Fixed Indexing problems for Cyclic Writes
03/23/04   jar     Merged in Branch Fixes.
03/17/04   jar     Merged in Amar's Fix for PIN Status
01/29/04   tml     Added event notification to diag interface
11/25/03   tml     Added Select API
11/20/03   tml     Fixed compilation issue
11/17/03   rd/tml  Added READ/WRITE Simple Request Handling
                   Added Handling for all Files accessable by UIM.
06/27/03   rdandu  Created diag interface to mm gsdi task.
===========================================================================*/

#include "intconv.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
#include "diagcmd.h"
#include "diagpkt.h"
#include "string.h"
#include "msg.h"
#include "gsdidiag.h"
#include "gsdidiag_uim.h"
#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */
#include "gsdi_exp.h"
#include "gsdi.h"
#include "event.h" /* event_report function */
#include "fs.h"   /* fs_open and fs_close */
#include "fs_public.h"   /* efs_open and efs_close */
#include "tmc.h"  /* tmc(task main controller) heap used for memory allcation*/
#include "gsdi_efs.h"
#include "mmgsdilib.h"
#include "mmgsdiutil.h"
#include "diag.h"
#include "AEEstd.h"
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#endif /* ((FEATURE_BT) || (FEATURE_WM_BT)) && (FEATURE_BT_EXTPF_SAP) */

#include "mmgsdi_gen.h"

#if defined (FEATURE_MMGSDI_GSM ) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*FEATURE_GSM || UMTS */
#include "fs_errno.h"

// #define DIAG_SUBSYS_DELAY_RSP 118


#define GSDIDIAG_MAX_ASYNC_REQ       100 /* max simultaneous transactions */
#define GSDIDIAG_MAX_FILENAME_LENGTH 200 /* max size of filename */
#define GSDIDIAG_MAX_VARS            10
#define GSDIDIAG_NULL_CHAR_LEN       1

/*Function declaration for search */
static void gsdidiag_get_search_payload(
  gsdidiag_req_payload_type        *req_ptr,
  PACKED void                      *req,
  uint32                            offset
);
static void gsdidiag_delayed_search (
  PACKED void                        *req,      /* pointer to request packet  */
  word                                pkt_len,  /* length of request packet */
  uint32                             *status,   /* Status returned by gsdi_sim_open_channel API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,   /* Response Id generated by diag*/
  uint32                              aps_as_transport /* APS transaction ID */
);

static void gsdidiag_delayed_generic_error_rsp(
  mmgsdi_return_enum_type   mmgsdi_status,
  gsdi_returns_T            gsdi_status,
  gsdidiag_req_payload_type request,
  uint16                    transaction_id,
  boolean                   is_gsdi_rsp
);

static void gsdidiag_req_table_row_init(int index);

static int gsdidiag_get_index(void);

static int gsdidiag_get_transaction_id(void);

static void gsdidiag_req_table_init(void);

#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
static uint32 gsdidiag_get_cmd_header(
  gsdidiag_generic_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
);

static void gsdidiag_get_payload(
   gsdidiag_generic_cmd_req_type *req_ptr,
   PACKED void * req,
   uint32 offset
);

static uint32 gsdidiag_get_sub_cmd_header(
  gsdidiag_generic_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
);
#endif /* FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */

static void gsdidiag_event_cb(const mmgsdi_event_data_type *event_ptr);

static void gsdidiag_get_mmgsdidiag_path_type_payload(
  mmgsdidiag_path_type *path_ptr,
  PACKED void         **src_req_ptr,
  uint32               *offset_ptr
);

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
static gsdi_returns_T gsdidiag_populate_according_to_file_type(
  gsdidiag_rsp_get_file_attr_payload_type *rsp_ptr,
  mmgsdi_file_attributes_type file_attr);
#endif /* FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */
/*gsdi_returns_T gsdidiag_cpy_pin_info(
  byte ** diag_pin_info,
  mmgsdi_pin_enum_type *gsdi_pin_info,
  int32 no_of_pin);
*/

static mmgsdi_client_id_type gsdidiag_mmgsdi_client_id;

/*============================================================================
   Data structure to hold information used to access in callback functions.
   Eg: transaction id->filename
============================================================================*/
typedef struct
{
    uint16 transaction_id;   /* transaction id */
    uint8  use_flag;
    uint32 aps_as_transport;
    gsdidiag_commands_enum_type subcommand;
    void*  var[GSDIDIAG_MAX_VARS];
                 /* var[0], var[1], var[2],.. var[GSDIDIAG_MAX_VARS-1]
                 can be used to
                 a) store information for access in callback function.
                 b) pass data to gsdi task
                 Eg: efs filename to store read data.
                 Memory to these vars will be allocated in DIAG context
                 and should be deallocated in the callback function*/

}gsdidiag_req_table_elem_type;

/*=============================================================================
    req_table holds data about requests which are currently
    being processed. Information which callbacks need to access, or
    memory which gsdi task needs to access
=============================================================================*/
static gsdidiag_req_table_elem_type req_table[GSDIDIAG_MAX_ASYNC_REQ];

/* declaring function which is used before it is defined */
#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
static void gsdidiag_simple_gfa_cb (const gsdi_cnf_T* sim_data);
#endif /* FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */

/*=============================================================================
    Enumerated type defining req_table_index_type.
    These are used to set row in table as free or
    currently being used.
=============================================================================*/
#define GSDIDIAG_REQ_TABLE_INDEX_FREE 0
#define GSDIDIAG_REQ_TABLE_INDEX_USED 1

#define GSDIDIAG_SIMPLE_READ  1
#define GSDIDIAG_SIMPLE_WRITE 2


typedef enum
{
    GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET  = 0, /* single packet in chunk */
    GSDIDIAG_SIMPLE_CHUNK_FIRST_PACKET = 1, /* first of multiple chunked pkt */
    GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET  = 2, /* last of multiple chunked pkts */
    GSDIDIAG_SIMPLE_CHUNK_PACKET       = 3  /* one of the internal pkts */
} gsdidiag_simple_chunk_packet_position;

/*lint -esym(715, pkt_len) */ // Suppress unused pkt_len messages.

/*===========================================================================
FUNCTION gsdidiag_req_table_init(void)

DESCRIPTION
    Initializes request table. Sets contents to NULL.

ARGUMENTS:
    None

DEPENDENCIES
    None

RETURN VALUE
    GSDI_SUCCESS

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_req_table_init(void)
{
  /* Initialize table. Set it to NULL */
  (void)memset (req_table, NULL, sizeof(req_table));
}

/*===========================================================================
FUNCTION gsdidiag_req_table_row_init

DESCRIPTION
    Initializes a row of req_table.

ARGUMENTS
    int index: index into the table

DEPENDENCIES
    None

RETURN VALUE
    GSDI_SUCCESS

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_req_table_row_init(int index)
{
    /* Initialize a row. Set it to NULL */
    (void)memset (&req_table[index], 0x00, sizeof(gsdidiag_req_table_elem_type));
}

/*===========================================================================
FUNCTION gsdidiag_get_index

DESCRIPTION
    Gets an available space from the req_table. This row is then marked as
    used. When it is no longer necessary, the row should be freed by
    a) deallocating memory allocated to pointers in the row, and
    b) calling gsdidiag_free_index.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static int gsdidiag_get_index(void)
{
    int i,j;
    for (i=0; i< GSDIDIAG_MAX_ASYNC_REQ ;i++)
    {
        if (req_table[i].use_flag == GSDIDIAG_REQ_TABLE_INDEX_FREE){
            /* found an empty row. assign this */
            req_table[i].use_flag = GSDIDIAG_REQ_TABLE_INDEX_USED;
            /* initialize the vars*/
            for (j=0;j<GSDIDIAG_MAX_VARS;j++)
            {
                if (req_table[i].var[j] != NULL)
                {
                    /* vars should have been initialized previously */
                    MSG_MED("req_table[%d].var[%d] is not initialized to null",i,j,0);
                    req_table[i].var[j]=NULL;
                }
            }
            return i;
        }
    }
    return GSDIDIAG_GET_FN_ERR;
}

/*===========================================================================
FUNCTION gsdidiag_free_index_memory(int index)

DESCRIPTION
     Deallocate any memory assigned to variables in this row of the req_table

ARGUMENTS:
    int index: index into the request table

DEPENDENCIES
    None

RETURN VALUE
    GSDI_SUCCESS

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_free_index_memory(int index)
{
    int i;
    for (i=0;i<GSDIDIAG_MAX_VARS;i++)
    {
        if (req_table[index].var[i] != NULL)
        {
            MMGSDIUTIL_TMC_MEM_FREE( req_table[index].var[i]);
            req_table[index].var[i]=NULL;
        }
    }
}

/*===========================================================================
FUNCTION gsdidiag_free_index

DESCRIPTION
    Frees up space on req_table that is no longer being used. This does not
    deallocate any memory assigned to variables in this row of the req_table.

ARGUMENTS
   int index: index into the request table

DEPENDENCIES
    Memory allocated to variables in this row of should be deallocated
    before calling this function.

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_free_index(int index)
{
    if ((index >= 0) && (index < GSDIDIAG_MAX_ASYNC_REQ) &&
        (index != GSDIDIAG_GET_FN_ERR)) {
      req_table[index].use_flag = GSDIDIAG_REQ_TABLE_INDEX_FREE;

      /* Initialize the row entry */
      gsdidiag_req_table_row_init(index);
    }
}
#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
/*===========================================================================
FUNCTION gsdidiag_is_index_invalid

DESCRIPTION
    Range check for the index

ARGUMENTS
   int index: index into the request table

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static boolean gsdidiag_is_index_invalid(int index)
{
  if ((index < 0) || (index >= GSDIDIAG_MAX_ASYNC_REQ) ||
      (index == GSDIDIAG_GET_FN_ERR)) {
    return TRUE;
  }
  return FALSE;
}
#endif /*#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION*/

/*===========================================================================
FUNCTION gsdidiag_get_transaction_id

DESCRIPTION
    Function assigns an unique transaction id

ARGUMENTS
    None

DEPENDENCIES
    None

RETURN VALUE
    uint16 transaction_id

SIDE EFFECTS
    None
===========================================================================*/
static int gsdidiag_get_transaction_id(void)
{
    /* counter for transaction id. Initializing to 1 */
    static uint16 current_transaction_id=1;

    uint16 transaction_id = current_transaction_id;
    current_transaction_id++;
    if (current_transaction_id == 65535)
    {  /* wrap around */
        current_transaction_id = 1;
    }
    return transaction_id;
}

#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
/*===========================================================================
FUNCTION gsdidiag_map_index_mode

DESCRIPTION
    Maps index modes from User specified ones to gsdi task internal values.
    Returns gsdi task index mode.

ARGUMENTS:
    gsdidiag_index_enum_type gsdidiag_index_mode: user specified index mode

DEPENDENCIES
    None

RETURN VALUE
    gsdi_index_modes_T gsdi_index_mode

SIDE EFFECTS
    None
===========================================================================*/
static gsdi_index_modes_T gsdidiag_map_index_mode(
                    gsdidiag_index_enum_type gsdidiag_index_mode)
{
    gsdi_index_modes_T gsdi_index_mode;
    switch(gsdidiag_index_mode)
    {
        case GSDIDIAG_NO_INDEX:
            gsdi_index_mode = GSDI_NO_INDEX;
            break;
        case GSDIDIAG_INDEX_ABSOLUTE:
            gsdi_index_mode = GSDI_INDEX_ABSOLUTE;
            break;
        case GSDIDIAG_INDEX_CURRENT:
            gsdi_index_mode = GSDI_INDEX_CURRENT;
            break;
        case GSDIDIAG_INDEX_NEXT:
            gsdi_index_mode = GSDI_INDEX_NEXT;
            break;
        case GSDIDIAG_INDEX_PREVIOUS:
            gsdi_index_mode = GSDI_INDEX_PREVIOUS;
            break;
        default:
            gsdi_index_mode = GSDI_NO_INDEX;
            break;
    }
    return gsdi_index_mode;
}


/*===========================================================================
FUNCTION gsdidiag_map_pin_id

DESCRIPTION
    Maps user pin ids to gsdi-internal pin ids

ARGUMENTS:
    gsdi_pin_ids_T gsdidiag_pin_id: User specified pin id

DEPENDENCIES
    None

RETURN VALUE
    GSDI task internal pin id of type gsdi_pin_ids_T

SIDE EFFECTS
    None
===========================================================================*/
static gsdi_pin_ids_T gsdidiag_map_pin_id (
    uint8 gsdidiag_pin_id
)
{
    /* No mapping required. Uing the same mapping as internal pin ids*/

    return (gsdi_pin_ids_T)gsdidiag_pin_id;
}
#endif /*FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */


static void gsdidiag_convert_gsdi_status_to_gsdidiag_status(
  gsdi_returns_T gsdi_status,
  PACKED uint32  *gsdidiag_status_ptr
)
{
  if (gsdidiag_status_ptr == NULL)
  {
    MSG_ERROR("Attempt to use null_ptr *gsdidiag_status_ptr ", 0, 0, 0);
    return;
  }

  MSG_HIGH("convert gsdi to gsdidiag status, gsdi status is 0x%x",
           gsdi_status,0,0);
  switch (gsdi_status)
  {
    case GSDI_SUCCESS:
      *gsdidiag_status_ptr = GSDIDIAG_SUCCESS;
      break;
    case GSDI_ERROR:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_ERROR;
      break;
    case GSDI_ACCESS_DENIED:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_ACCESS_DENIED;
      break;
    case GSDI_NOT_FOUND:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_NOT_FOUND;
      break;
    case GSDI_INCOMPAT_PIN_STATUS:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_INCOMPAT_PIN_STATUS;
      break;
    case GSDI_INCORRECT_CODE:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_INCORRECT_CODE;
      break;
    case GSDI_CODE_BLOCKED:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_CODE_BLOCKED;
      break;
    case GSDI_INCORRECT_PARAMS:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_INCORRECT_PARAMS;
      break;
    case GSDI_NOT_SUPPORTED:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_NOT_SUPPORTED;
      break;
    case GSDI_NOT_INIT:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_NOT_INIT;
      break;
    default:
      *gsdidiag_status_ptr = GSDIDIAG_GSDI_MAX_RETURN_STATUS;
      break;
  }
} /* gsdidiag_convert_gsdi_status_to_gsdidiag_status */


static void gsdidiag_convert_gsdidiag_status_to_gsdi_status(
  uint32         gsdidiag_status,
  gsdi_returns_T *gsdi_status_ptr
)
{
  if (gsdi_status_ptr == NULL)
  {
    MSG_ERROR("Attempt to use null_ptr *gsdi_status_ptr ", 0, 0, 0);
    return;
  }
  MSG_HIGH("convert gsdidiag to gsdi status, gsdidiag status is 0x%x",
           gsdidiag_status,0,0);
  switch (gsdidiag_status)
  {
    case GSDIDIAG_SUCCESS:
      *gsdi_status_ptr = GSDI_SUCCESS;
      break;
    case GSDIDIAG_GSDI_ERROR:
      *gsdi_status_ptr = GSDI_ERROR;
      break;
    case GSDIDIAG_GSDI_ACCESS_DENIED:
      *gsdi_status_ptr = GSDI_ACCESS_DENIED;
      break;
    case GSDIDIAG_GSDI_NOT_FOUND:
      *gsdi_status_ptr = GSDI_NOT_FOUND;
      break;
    case GSDIDIAG_GSDI_INCOMPAT_PIN_STATUS:
      *gsdi_status_ptr = GSDI_INCOMPAT_PIN_STATUS;
      break;
    case GSDIDIAG_GSDI_INCORRECT_CODE:
      *gsdi_status_ptr = GSDI_INCORRECT_CODE;
      break;
    case GSDIDIAG_GSDI_CODE_BLOCKED:
      *gsdi_status_ptr = GSDI_CODE_BLOCKED;
      break;
    case GSDIDIAG_GSDI_INCORRECT_PARAMS:
      *gsdi_status_ptr = GSDI_INCORRECT_PARAMS;
      break;
    case GSDIDIAG_GSDI_NOT_SUPPORTED:
      *gsdi_status_ptr = GSDI_NOT_SUPPORTED;
      break;
    case GSDIDIAG_GSDI_NOT_INIT:
      *gsdi_status_ptr = GSDI_NOT_INIT;
      break;
    default:
      *gsdi_status_ptr = GSDI_MAX_RETURN_STATUS;
      break;
  }
} /* gsdidiag_convert_gsdidiag_status_to_gsdi_status */

/*===========================================================================
FUNCTION gsdidiag_sim_malloc_append_efs_dir

DESCRIPTION
    This file copies the efs_filename into the required pointer checking
    to see if /apps/ dir needs to prepended.

ARGUMENTS:
    char *efs_filename    : efs filename to copy

DEPENDENCIES
    None

RETURN VALUE


SIDE EFFECTS
    None
===========================================================================*/
static char * gsdidiag_sim_malloc_append_efs_dir (const char *efs_filename_ptr)
{
    uint32               filename_len = 0;
    uint32               null_char_len = 1;
    char                 *req_filename_ptr = NULL;
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
    char                 *dir_name_ptr = "/apps/";
    uint32               dir_len = 0;
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */

    if (efs_filename_ptr == NULL)
    {
       MSG_ERROR("NULL efs filename ptr", 0, 0, 0);
       return NULL;
    }
    filename_len = strlen(efs_filename_ptr) + null_char_len;

#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
    dir_len = strlen(dir_name_ptr);
    filename_len += dir_len;
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */

    req_filename_ptr = mmgsdi_malloc(filename_len);
    if (req_filename_ptr != NULL)
    {
      (void)memset(req_filename_ptr, 0, filename_len);

#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
      memcpy(req_filename_ptr, dir_name_ptr, dir_len);
      /* copy the efs filename starting after the "." which should be the first
         character in the filename string */
      memcpy(&req_filename_ptr[dir_len], efs_filename_ptr,
              strlen(efs_filename_ptr));
#else
      memcpy(req_filename_ptr, efs_filename_ptr, strlen(efs_filename_ptr));
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
      return req_filename_ptr;
    }
    else {
      MSG_ERROR("NULL filename request pointer", 0, 0, 0);
      return NULL;
    }
}


#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
/*===========================================================================
FUNCTION gsdidiag_copy_access_to_mmgsdi

DESCRIPTION
  Function to copy the access type from gsdidiag type to mmgsdi type.

INPUT:
  mmgsdi_access_type*             access_ptr : pointer to which data to be
                                               populated
  mmgsdidiag_access_type gsdidiag_access_ptr : pointer to which data to be
                                               copied from

DEPENDENCIES
  None

RETURN VALUE
  None

OUTPUT
  None
===========================================================================*/
static void gsdidiag_copy_access_to_mmgsdi(
  mmgsdi_access_type*           access_ptr,
  const mmgsdidiag_access_type* gsdidiag_access_ptr)
{
  if ((access_ptr == NULL) || (gsdidiag_access_ptr == NULL))
  {
    MSG_ERROR("Null mmgsdi access ptr", 0, 0, 0);
    return;
  }

  switch (gsdidiag_access_ptr->access_method)
  {
    case GSDIDIAG_BY_PATH_ACCESS:
      access_ptr->access_method = MMGSDI_BY_PATH_ACCESS;
      access_ptr->file.path_type.path_len = gsdidiag_access_ptr->path.path_len;
      if( (gsdidiag_access_ptr->path.path_len > 0) &&
          (gsdidiag_access_ptr->path.path_len <= MMGSDI_MAX_PATH_LEN))
      {
        memcpy(access_ptr->file.path_type.path_buf,
              (uint16*)gsdidiag_access_ptr->path.path_buf,
              sizeof(uint16)*(int32touint32(gsdidiag_access_ptr->path.path_len)));
      }
      else
      {
        MSG_ERROR("path_len could be <= 0 or exceeds the buffer size ",0,0,0);
        return;
      }
      break;

    case GSDIDIAG_EF_ENUM_ACCESS:
      access_ptr->access_method = MMGSDI_EF_ENUM_ACCESS;
      access_ptr->file.file_enum = gsdidiag_convert_file_to_mmgsdi_enum(
          gsdidiag_access_ptr->file_enum);
      break;
    case GSDIDIAG_DF_ENUM_ACCESS:
    case GSDIDIAG_MAX_ACCESS_ENUM:
      MSG_ERROR("DF/MAX ENUM Access not supported", 0, 0, 0);
      return;
  }
} /* gsdidiag_copy_access_to_mmgsdi */


static void gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
  mmgsdi_return_enum_type mmgsdi_status,
  PACKED uint32           *gsdidiag_status_ptr)
{
  if (gsdidiag_status_ptr == NULL)
  {
    MSG_ERROR("Attempt to use null_ptr *gsdidiag_status_ptr ", 0, 0, 0);
    return;
  }

  MSG_HIGH("convert mmgsdi to gsdidiag status, mmgsdi status is 0x%x",
           mmgsdi_status,0,0);

  switch (mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      *gsdidiag_status_ptr = GSDIDIAG_SUCCESS;
      break;
    case MMGSDI_INCORRECT_PARAMS:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_INCORRECT_PARAMS;
      break;
    case  MMGSDI_CMD_QUEUE_FULL:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_CMD_QUEUE_FULL;
      break;
    case MMGSDI_ERROR:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_ERROR;
      break;
    case MMGSDI_ACCESS_DENIED:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_ACCESS_DENIED;
      break;
    case MMGSDI_NOT_FOUND:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_NOT_FOUND;
      break;
    case MMGSDI_NOT_SUPPORTED:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_NOT_SUPPORTED;
      break;
    case MMGSDI_NOT_INIT:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_NOT_INIT;
      break;
    default:
      *gsdidiag_status_ptr = GSDIDIAG_MMGSDI_MAX_RETURN_STATUS;
      break;
  }
} /* gsdidiag_convert_mmgsdi_status_to_gsdidiag_status */


static void gsdidiag_convert_gsdidiag_status_to_mmgsdi_status(
  uint32                  gsdidiag_status,
  mmgsdi_return_enum_type *mmgsdi_status_ptr
)
{
  if (mmgsdi_status_ptr == NULL)
  {
    MSG_ERROR("Attempt to use null_ptr *mmgsdi_status ", 0, 0, 0);
    return;
  }

  MSG_HIGH("convert gsdidiag to mmgsdi status, diag status is 0x%x",
           gsdidiag_status,0,0);
  switch (gsdidiag_status)
  {
    case GSDIDIAG_SUCCESS:
      *mmgsdi_status_ptr = MMGSDI_SUCCESS;
      break;
    case GSDIDIAG_MMGSDI_INCORRECT_PARAMS:
      *mmgsdi_status_ptr = MMGSDI_INCORRECT_PARAMS;
      break;
    case  GSDIDIAG_MMGSDI_CMD_QUEUE_FULL:
      *mmgsdi_status_ptr = MMGSDI_CMD_QUEUE_FULL;
      break;
    case GSDIDIAG_MMGSDI_ERROR:
      *mmgsdi_status_ptr = MMGSDI_ERROR;
      break;
    case GSDIDIAG_MMGSDI_ACCESS_DENIED:
      *mmgsdi_status_ptr = MMGSDI_ACCESS_DENIED;
      break;
    case GSDIDIAG_MMGSDI_NOT_FOUND:
      *mmgsdi_status_ptr = MMGSDI_NOT_FOUND;
      break;
    case GSDIDIAG_MMGSDI_NOT_SUPPORTED:
      *mmgsdi_status_ptr = MMGSDI_NOT_SUPPORTED;
      break;
    case GSDIDIAG_MMGSDI_NOT_INIT:
      *mmgsdi_status_ptr = MMGSDI_NOT_INIT;
      break;
    default:
      *mmgsdi_status_ptr = MMGSDI_MAX_RETURN_ENUM;
      break;
  }
} /* gsdidiag_convert_gsdidiag_status_to_mmgsdi_status */


static gsdi_srch_mode gsdidiag_convert_search_mode_to_gsdi(
  gsdidiag_search_mode_enum_type gsdidiag_search_mode)
{
  gsdi_srch_mode mode = GSDI_NO_SUCH_SRCH_MODE;

  switch (gsdidiag_search_mode)
  {
    case GSDIDIAG_NO_SUCH_SRCH_MODE:
      mode = GSDI_NO_SUCH_SRCH_MODE;
      break;
    case GSDIDIAG_SEARCH_SIMPLE:
      mode = GSDI_SIMPLE_SRCH;
      break;
    case GSDIDIAG_SEARCH_ENHANCED:
      mode = GSDI_ENHANCED_SRCH;
      break;
    case GSDIDIAG_SEARCH_MODE_MAX_ENUM:
      mode = GSDI_MAXIMUM_SRCH_MODE;
      break;
  }
  return mode;
}

static gsdi_srch_dir_type gsdidiag_convert_search_dir_to_gsdi(
  gsdidiag_search_dir_type gsdidiag_search_dir)
{
  gsdi_srch_dir_type dir_type = GSDI_SRCH_FORWARD;

  switch (gsdidiag_search_dir)
  {
    case GSDIDIAG_SEARCH_FORWARD:
      dir_type = GSDI_SRCH_FORWARD;
      break;
    case GSDIDIAG_SEARCH_BACKWARD:
      dir_type = GSDI_SRCH_BACKWARD;
      break;
    case GSDIDIAG_SEARCH_DIR_MAX_ENUM:
      MSG_ERROR("Unknown Search Value 0x%x", gsdidiag_search_dir, 0, 0);
      dir_type = GSDI_SRCH_FORWARD;
      break;
  }
  return dir_type;
}

static mmgsdi_slot_id_enum_type gsdidiag_convert_to_mmgsdi_slot (
  uint8 gsdidiag_slot)
{
  if (gsdidiag_slot == 0x01)
    return MMGSDI_SLOT_1;
  else if (gsdidiag_slot == 0x02)
    return MMGSDI_SLOT_2;
  else
    return MMGSDI_MAX_SLOT_ID_ENUM;
}

static mmgsdidiag_access_enum_type gsdidiag_convert_access_method_from_mmgsdi_enum (
  mmgsdi_access_method_enum_type mmgsdi_access)
{
  switch (mmgsdi_access) {
  case MMGSDI_EF_ENUM_ACCESS:
    return GSDIDIAG_EF_ENUM_ACCESS;
  case MMGSDI_DF_ENUM_ACCESS:
    return GSDIDIAG_DF_ENUM_ACCESS;
  case MMGSDI_BY_PATH_ACCESS:
    return GSDIDIAG_BY_PATH_ACCESS;
  case MMGSDI_MAX_ACCESS_METHOD_ENUM:
    return GSDIDIAG_MAX_ACCESS_ENUM;
  case MMGSDI_BY_APP_ID_ACCESS:
    return GSDIDIAG_MAX_ACCESS_ENUM;
  }
  return GSDIDIAG_MAX_ACCESS_ENUM;
}

static mmgsdi_access_method_enum_type  gsdidiag_convert_access_method_to_mmgsdi_enum (
  mmgsdidiag_access_enum_type gsdidiag_access)
{
  switch (gsdidiag_access) {
  case GSDIDIAG_EF_ENUM_ACCESS:
    return MMGSDI_EF_ENUM_ACCESS;
  case GSDIDIAG_DF_ENUM_ACCESS:
    return MMGSDI_DF_ENUM_ACCESS;
  case GSDIDIAG_BY_PATH_ACCESS:
    return MMGSDI_BY_PATH_ACCESS;
  case GSDIDIAG_MAX_ACCESS_ENUM:
    return MMGSDI_MAX_ACCESS_METHOD_ENUM;
  }
  return MMGSDI_MAX_ACCESS_METHOD_ENUM;
}

/*===========================================================================
FUNCTION gsdidiag_convert_from_mmgsdi_pin_id

DESCRIPTION
    Maps user pin ids to mmgsdi-internal pin ids

ARGUMENTS:
    pin_id_ptr   : User specified pin id
    mmgsdi_pin_id: returned MMGSDI pin id

DEPENDENCIES
    None

RETURN VALUE

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_convert_from_mmgsdi_pin_id(
  PACKED uint32       *pin_id_ptr,
  mmgsdi_pin_enum_type mmgsdi_pin_id)
{
  if (pin_id_ptr == NULL)
      return;

  switch (mmgsdi_pin_id)
  {
  case MMGSDI_PIN1:
    *pin_id_ptr = GSDIDIAG_PIN1;
    break;
  case MMGSDI_PIN2:
    *pin_id_ptr = GSDIDIAG_PIN2;
    break;
  case MMGSDI_UNIVERSAL_PIN:
    *pin_id_ptr = GSDIDIAG_PIN_UNIVERSAL;
    break;
  default:
    *pin_id_ptr = GSDIDIAG_PIN_UNKNOWN;
    break;
  }
}

/*===========================================================================
FUNCTION gsdidiag_convert_from_mmgsdi_pin_status

DESCRIPTION
    Maps mmgsdi pin status to GSDIDIAG Pin Status

ARGUMENTS:
    pin_status_ptr    : returned GSDIDIAG Pin Status
    mmgsdi_pin_status : MMGSDI pin status

DEPENDENCIES
    None

RETURN VALUE

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_convert_from_mmgsdi_pin_status(
  PACKED uint8               *pin_status_ptr,
  mmgsdi_pin_status_enum_type mmgsdi_pin_status)
{
  if (pin_status_ptr == NULL)
      return;

  switch (mmgsdi_pin_status)
  {
  case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
    *pin_status_ptr = GSDIDIAG_ENABLED_NOT_VERIFIED;
    break;
  case MMGSDI_PIN_UNBLOCKED:
  case MMGSDI_PIN_CHANGED:
  case MMGSDI_PIN_ENABLED_VERIFIED:
    *pin_status_ptr = GSDIDIAG_ENABLED_VERIFIED;
    break;
  case MMGSDI_PIN_DISABLED:
    *pin_status_ptr = GSDIDIAG_PIN_DISABLED;
    break;
  case MMGSDI_PIN_BLOCKED:
    *pin_status_ptr = GSDIDIAG_PIN_BLOCKED;
    break;
  case MMGSDI_PIN_PERM_BLOCKED:
    *pin_status_ptr = GSDIDIAG_PIN_PERM_BLOCKED;
    break;
  case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
  default:
    *pin_status_ptr = GSDIDIAG_PIN_STATUS_UNKNOWN;
    break;
  }
}

/*===========================================================================
FUNCTION gsdidiag_convert_from_mmgsdi_pin_op

DESCRIPTION
    Maps mmgsdi pin operation to GSDIDIAG Pin Operation

ARGUMENTS:
    pin_op_ptr    : returned GSDIDIAG Pin Operation
    mmgsdi_pin_op : MMGSDI pin operation

DEPENDENCIES
    None

RETURN VALUE

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_convert_from_mmgsdi_pin_op(
  PACKED uint8                  *pin_op_ptr,
  mmgsdi_pin_operation_enum_type mmgsdi_pin_op)
{
  if (pin_op_ptr == NULL)
      return;

  switch (mmgsdi_pin_op)
  {
  case MMGSDI_PIN_OP_VERIFY:
    *pin_op_ptr = GSDIDIAG_PIN_OP_VERIFY;
    break;
  case MMGSDI_PIN_OP_ENABLE:
    *pin_op_ptr = GSDIDIAG_PIN_OP_ENABLE;
    break;
  case MMGSDI_PIN_OP_DISABLE:
    *pin_op_ptr = GSDIDIAG_PIN_OP_DISABLE;
    break;
  case MMGSDI_PIN_OP_CHANGE:
    *pin_op_ptr = GSDIDIAG_PIN_OP_CHANGE;
    break;
  case MMGSDI_PIN_OP_UNBLOCK:
    *pin_op_ptr = GSDIDIAG_PIN_OP_UNBLOCK;
    break;
  case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
    *pin_op_ptr = GSDIDIAG_PIN_OP_DISABLE_REPLACE;
    break;
  default:
    *pin_op_ptr = GSDIDIAG_PIN_OP_UNKNOWN;
    break;
  }
}

/*===========================================================================
FUNCTION gsdidiag_convert_from_mmgsdi_pin_id

DESCRIPTION
    Maps mmgsdi pin operation to GSDIDIAG Pin ID

ARGUMENTS:
    pin_id            : GSDIDIAG Pin ID
    mmgsdi_pin_id_ptr : returned MMGSDI pin ID

DEPENDENCIES
    None

RETURN VALUE

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_convert_to_mmgsdi_pin_id(
  uint32                pin_id,
  mmgsdi_pin_enum_type *mmgsdi_pin_id_ptr)
{
  if (mmgsdi_pin_id_ptr == NULL)
      return;

  switch (pin_id)
  {
  case GSDIDIAG_PIN1:
    *mmgsdi_pin_id_ptr = MMGSDI_PIN1;
    break;
  case GSDIDIAG_PIN2:
    *mmgsdi_pin_id_ptr = MMGSDI_PIN2;
    break;
  case GSDIDIAG_PIN_UNIVERSAL:
    *mmgsdi_pin_id_ptr = MMGSDI_UNIVERSAL_PIN;
    break;
  default:
    *mmgsdi_pin_id_ptr = MMGSDI_MAX_PIN_ENUM;
    break;
  }
}

/*===========================================================================
FUNCTION gsdidiag_convert_to_mmgsdi_replace_pin

DESCRIPTION
    Maps GSDIDIAG Replace to MMGSDI Replace

ARGUMENTS:
    replace_pin        : GSDIDIAG Replace
    mmgsdi_replace_enum: returned MMGSDI Replace Enum

DEPENDENCIES
    None

RETURN VALUE

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_convert_to_mmgsdi_replace_pin(
  uint32                        replace_pin,
  mmgsdi_pin_replace_enum_type *mmgsdi_replace_enum_ptr)
{
  if (mmgsdi_replace_enum_ptr == NULL)
  {
    MSG_ERROR("Replace Pin Ptr is NULL",0,0,0);
    return;
  }

  switch (replace_pin)
  {
   case GSDIDIAG_REPLACED_BY_UNIV:
     *mmgsdi_replace_enum_ptr = MMGSDI_PIN_REPLACED_BY_UNIVERSAL;
    break;
  case GSDIDIAG_NOT_REPLACED_BY_UNIV:
  default:
    *mmgsdi_replace_enum_ptr = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
    break;
  }
}


/*===========================================================================
FUNCTION gsdidiag_convert_directory_to_uim_enum()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_items_enum_type based on the passed in gsdidiag_elementary_file_enum_type

DEPENDENCIES
    None

RETURN VALUE
    uim_items_enum_type

SIDE EFFECTS
    None
===========================================================================*/
static gsdi_returns_T gsdidiag_convert_directory_to_uim_enum(gsdidiag_directory_enum_type sim_directory,
                                                      word *path_p,
                                                      uint8  *path_len,
                                                      uim_file_type  *file_type)
{
  if ((path_p == NULL) || (path_len == NULL) || (file_type == NULL))
    return GSDI_ERROR;

 (void)memset(path_p, 0xFF, sizeof(word) * UIM_MAX_PATH_ELEMENTS);

  switch (sim_directory)
  {
  case GSDIDIAG_MF:
    *path_len = 0;
    *file_type = UIM_MF;
    break;

  case GSDIDIAG_TELECOM_DF:
    *path_len = 1;
    *file_type = UIM_DF;
    path_p[0] = UIM_TELECOM_DF;
    break;

  case GSDIDIAG_GSM_DF:
    *path_len = 1;
    *file_type = UIM_DF;
    path_p[0] = UIM_GSM_DF;
    break;

  case GSDIDIAG_CDMA_DF:
    *path_len = 1;
    *file_type = UIM_DF;
    path_p[0] = UIM_CDMA_DF;
    break;

  case GSDIDIAG_DCS_1800_DF:
    *path_len = 1;
    *file_type = UIM_DF;
    path_p[0] = UIM_DCS_1800_DF;
    break;

  case GSDIDIAG_GRAPHICS_DF:
    *path_len = 3;
    *file_type = UIM_DF;
    path_p[0] = 0x3F00;
    path_p[1] = 0x7F10;
    path_p[2] = 0x5F50;
    break;

  case GSDIDIAG_SOLSA_DF:
    *path_len = 3;
    *file_type = UIM_DF;
    path_p[0] = 0x3F00;
    path_p[1] = 0x7F20;
    path_p[2] = 0x5F70;
    break;

  case GSDIDIAG_MEXE_EF:
    *path_len = 3;
    *file_type = UIM_DF;
    path_p[0] = 0x3F00;
    path_p[1] = 0x7F20;
    path_p[2] = 0x5F3C;
    break;

  case GSDIDIAG_ADF:
    *path_len = 0;
    *file_type = UIM_ADF;
    break;

  default:
    return GSDI_ERROR;
  }
  return GSDI_SUCCESS;
}

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

/*===========================================================================
FUNCTION gsdi_get_file_attributes_cb

DESCRIPTION
    Callback used for an asynchronous gsdi get file attributes.
    The data read will be written to a file on efs.

ARGUMENTS
    gsdi_cnf_T* sim_data: Information, from gsdi task, about request executed

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    NULL

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_get_file_attributes_cb (const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload; /* event information */
    fs_open_xparms_type         open_parms;    /* fileopen params */
    int                         index;         /* index into req_table */
    uint8                       len;           /* length of event */
    int                         file_descriptor = 0;
    struct fs_stat              file_stat = {0};

    (void)memset(&open_parms, 0x00, sizeof(fs_open_xparms_type));
    len   = sizeof( event_payload );
    if (sim_data == NULL) {
        return;
    }
    index = uint32toint32(sim_data->message_header.client_ref);
    event_payload.transaction_id = req_table[index].transaction_id;

    /* Index should be in the used state */
    if (req_table[index].use_flag!=GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use in gsdidiag_read_cb.",
                  index, 0, 0);
        event_payload.status =  GSDIDIAG_MEMORY_ERR;
        event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                             len,
                             (void *) &event_payload);
        return;
    }

    /* test if file already exists */
    if(efs_stat(req_table[index].var[0], &file_stat) == 0)
    {
      /* delete file, it already exists */
      if (efs_unlink (req_table[index].var[0]) == -1)
      {
        MMGSDI_DEBUG_MSG_ERROR ("File remove failed ", 0, 0, 0);
        event_payload.status = GSDIDIAG_EFS_RM_ERR;
        event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                             len,
                             (void *)&event_payload );
        /*free filename memory*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
      }
    }
    else if (efs_errno != ENOENT)
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      MMGSDI_DEBUG_MSG_ERROR ("efs_stat failed.", 0, 0, 0);
      event_payload.status = GSDIDIAG_EFS_NAMETEST_ERR;
      event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                           len,
                           (void *)&event_payload );
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
      return;
    }

    /* Make sure it is a get file attributes resp */
    if ( sim_data->message_header.resp_type != GSDI_GET_FILE_ATTRIBUTES_RSP )
    {
        MMGSDI_DEBUG_MSG_ERROR( "Rsp not GSDI_SIM_GET_FILE_ATTRIBUTES_RSP(%d):%d",
                   GSDI_GET_FILE_ATTRIBUTES_RSP,
                   sim_data->message_header.resp_type,
                   0);
        event_payload.status = GSDIDIAG_INCORRECT_RSP_ERR;
        event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                             len,
                             (void *)&event_payload);
        /*free memory used for filename*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
    }

    /* check if the getfileattributes was successful */
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
        /* successful. write returned data to a file */

        open_parms.create.cleanup_option   = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;

        file_descriptor = efs_open(req_table[index].var[0], O_CREAT | O_WRONLY);

        if(file_descriptor < 0)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);

          /* send event */
          event_payload.status = GSDIDIAG_EFS_OPEN_ERR;
          event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                               len,
                              (void *)&event_payload);

          /* free memory used for filename */
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
        }

        if(efs_write(file_descriptor,
                     (void*)&sim_data->gfa_cnf.gfa_data,
                     (fs_size_t)sizeof(gsdi_file_attributes_T)) <
                     (fs_ssize_t)sizeof(gsdi_file_attributes_T))
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_write failed ", 0, 0, 0);
          event_payload.status = GSDIDIAG_EFS_WRITE_ERR;
          event_report_payload( EVENT_GSDI_GET_FILE_ATTRIBUTES,
                                len,
                                (void *)&event_payload );
        }
        else
        {
          /* success in writing to file */
          event_payload.status = GSDIDIAG_SUCCESS;
          event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES,
                               len,
                               (void *)&event_payload );
        }

        /* close file */
        if(efs_close(file_descriptor) < 0)
        {
          ERR("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
        }
    }else{
        /* getfileattributes failed */
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->message_header.gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_GET_FILE_ATTRIBUTES,
                              len,
                              (void *)&event_payload );
    }

    /* free memory used for filename */
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);

    return;
}


/*===========================================================================
FUNCTION gsdidiag_sim_get_file_attributes

DESCRIPTION
    Gets file attributes. The actual data will be written to a file in
    callback func gsdidiag_sim_get_file_attributes_cb.
    Rsp packet indicates if the req was successfully queued to gsdi task

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_sim_get_file_attributes_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE


SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_sim_get_file_attributes (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)
{
  gsdidiag_sim_get_file_attributes_cmd_req_type *req_ptr        = NULL;
  gsdidiag_sim_get_file_attributes_cmd_rsp_type *rsp_ptr        = NULL;
  gsdi_returns_T                                 gsdi_status    = GSDI_SUCCESS;
  uim_items_enum_type                            sim_file_name  = UIM_NO_SUCH_ITEM;
  int                                            index          = 0;
  uint32                                         filename_len   = 0;
  uint32                                         rsp_len        = 0 ;
  mmgsdi_return_enum_type                        mmgsdi_status  = MMGSDI_ERROR;

  if(req == NULL)
  {
    MSG_ERROR( "gsdidiag_sim_get_file_attributes: Null Ptr *req", 0, 0, 0);
    return NULL;
  }

  if(pkt_len < sizeof(gsdidiag_sim_get_file_attributes_cmd_req_type))
  {
    MSG_ERROR("gsdidiag_sim_get_file_attr req incorrect pkt_len 0x%x",
               pkt_len,0,0);
    return NULL;
  }

  /*Allocate one additional byte in case EFS filename string is not 
    null-terminated*/
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

  if ( mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
  {
    MSG_ERROR("gsdidiag_sim_get_file_attr malloc err req_ptr",0,0,0);
    return NULL;
  }
  else
  {
    memcpy((void *)req_ptr,(void *)req, pkt_len);
  }

  filename_len = strlen((const char*)req_ptr->efs_filename);

  /* check if efs filename is too long*/
  if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
  {
    MMGSDI_DEBUG_MSG_ERROR( "Filename too long(0x%x). Max length=0x%x",
               filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0 );
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return NULL; /*return immediately. No Rsp will be sent out*/
  }

  /* length of rsp */
  rsp_len = sizeof(gsdidiag_sim_get_file_attributes_cmd_rsp_type) + filename_len;

  /* allocate memory for the rsp */
  rsp_ptr = (gsdidiag_sim_get_file_attributes_cmd_rsp_type *)
              diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_FILE_ATTRIBUTES_CMD,
                                   rsp_len );

  if (rsp_ptr == NULL)
  {
    MSG_ERROR( "gsdidiag_sim_get_file_attributes: Null Ptr *rsp_ptr", 0, 0, 0);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return NULL;
  }

  /* copy req_ptr to rsp_ptr */
  rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
  rsp_ptr->sim_app_id_len  = req_ptr->sim_app_id_len;
  (void)memcpy( (char*) rsp_ptr->sim_app_id,
          (const char*) req_ptr->sim_app_id,
          sizeof(req_ptr->sim_app_id));
  rsp_ptr->sim_elementary_file = req_ptr->sim_elementary_file;

  /* Copy efs file name to response structure
     No check here because the source string length is what is
     being used to determine the destination length*/
  (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                     (const char*)req_ptr->efs_filename,
                     (uint32toint32(filename_len) + GSDIDAG_NULL_CHAR_LEN));

  /* initialize transaction_id */
  rsp_ptr->transaction_id = 0;

  /* find a free space in req_table to put the information in*/
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR)
  {
    rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
  }

  /* get a transaction id */
  req_table[index].transaction_id = int32touint16(gsdidiag_get_transaction_id());
  rsp_ptr->transaction_id = req_table[index].transaction_id;

  /* allocate memory for the filename in the req table and copy filename to it */
  req_table[index].var[0] =
    gsdidiag_sim_malloc_append_efs_dir((char *)req_ptr->efs_filename);
  if (req_table[index].var[0] == NULL)
  {
    /* mmgsdi_malloc failed */
    MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);
    rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
  }

  /* elementary file, map from user ids to gsdi internal ids */
  sim_file_name = gsdidiag_convert_file_to_uim_enum(req_ptr->sim_elementary_file);
#ifdef FEATURE_MMGSDI_DUAL_SLOT
  gsdi_status = gsdi2_get_file_attributes(req_ptr->sim_slot_number,
                          sim_file_name,
                          NULL,
                          index,
                          (gsdi_async_cb)gsdidiag_get_file_attributes_cb);
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
  gsdi_status = gsdi_get_file_attributes(
                  sim_file_name,
                  NULL,
                  int32touint32(index),
                  (gsdi_async_cb)gsdidiag_get_file_attributes_cb
                  );
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR(
      "gsdidiag_sim_get_file_attributes queue failed 0x%x",gsdi_status,0,0);
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index( index );
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
  diagpkt_commit( rsp_ptr );
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  return NULL;
} /* gsdidiag_sim_get_file_attributes */


/*===========================================================================
FUNCTION gsdidiag_send_cmd_to_uim_cb

DESCRIPTION
    Callback used for any UIM command.  Will be called from
    the UIM task context.

DEPENDENCIES
    None

INPUT
    uim_rpt_type, the data reported from UIM

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the required response from the uim
===========================================================================*/
static void gsdidiag_send_cmd_to_uim_cb (uim_rpt_type* uim_data)
{

  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */

  /* response pointer to the uim data part of the response */
  gsdidiag_rsp_uim_cmd_type      * uim_rsp_ptr;
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  if (uim_data == NULL) {
      return;
  }

  index = (int) uim_data->user_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (int) DIAG_SUBSYS_GSDI,
                                           (int) GSDIDIAG_GENERIC_CMD,
                                           req_table[index].transaction_id,
                                           len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  /* Zero out the payload data */
  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

   /* subcommand is set to GSDIDIAG_UIM_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand = GSDIDIAG_UIM_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d not in use gsdidiag_send_cmd_to_uim_cb.",
                           index,0,0);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status = GSDI_ERROR;
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    /* The payload's header has the actual pass fail information */
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      (uim_data->rpt_status == (UIM_PASS)?GSDI_SUCCESS:GSDI_ERROR);
  }
  else
  {
    uint32 tmp_gsdidiag_status;

    /* Report a pass or fail in both the rpt header and payload header */
    tmp_gsdidiag_status =
      ( (uim_data->rpt_status ==  UIM_PASS)?GSDIDIAG_SUCCESS:GSDIDIAG_BAD_PARAM );

    diagpkt_subsys_set_status(rsp_ptr, tmp_gsdidiag_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      (uim_data->rpt_status == (UIM_PASS)?GSDI_SUCCESS:GSDI_ERROR);
  }

  /* Get the pointer to the uim portion of the response */
  uim_rsp_ptr = &(rsp_ptr->rsp_payload.response.uim_data);

  /* Fill in the diag response based on the uim response */
  gsdidiag_populate_uim_cmd_rsp(uim_rsp_ptr, uim_data,
                                (uim_cmd_type *)req_table[index].var[0]);

  /* free the memory */
  gsdidiag_free_index_memory(index);
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
  return;
}


/*===========================================================================
FUNCTION gsdidiag_send_cmd_to_uim

DESCRIPTION
    This is designed to pipe any command to the UIM.  This function is expected
    to be called in the diag context itself.

INPUT:
    PACKED void *req,       - Pointer to request packet
    word pkt_len,           - Size of the requets packet
    uint32 * status,        - Status returned by gsdi_sim_send_channel_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id, - Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    None
===========================================================================*/
static void gsdidiag_send_cmd_to_uim (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* Status returned by gsdi_send_cmd_to_uim_server*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,    /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
  gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
  gsdi_returns_T             gsdi_status  = GSDI_SUCCESS;
  int32                      index        = 0;


  uint8                      slot_id      = 0;
  uint8                      app_id_len   = 0;
  const uint8                *app_id      = NULL;
  uim_cmd_type               *uim_cmd_ptr = NULL;
  gsdidiag_uim_cmd_req_type  *uim_cmd_req_ptr = NULL;

  if (status == NULL) {
    MMGSDI_DEBUG_MSG_ERROR("status is Null",0,0,0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is Null",0,0,0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if ( index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* If unable to acquire UIM command buffer, return from here */
  if ((uim_cmd_ptr = gsdi_get_uim_buf()) == NULL)
  {
    *status = GSDIDIAG_MEMORY_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* Clear all the contents of the acquired memory */
  (void)memset(uim_cmd_ptr,0,sizeof(uim_cmd_type));

  /* Store this ID to give back a delayed response */
  req_table[index].transaction_id   = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].var[0] = uim_cmd_ptr;
  req_table[index].subcommand = GSDIDIAG_UIM_CMD;

  /* Get the variables to be passed to gsdidiag_populate_uim_cmd */
  uim_cmd_req_ptr = &(req_ptr->request.uim_cmd_req);
  slot_id = req_ptr->sub_cmd_hdr.slot_id;

  app_id_len = req_ptr->sub_cmd_hdr.sim_app_id_len;
  app_id     = (const uint8 *)req_ptr->sub_cmd_hdr.sim_app_id;

  /* Populate the command specific parameters to uim_cmd_ptr.  Pass var one
  to the populate command so that any memory would be allocated if necessary*/
  gsdi_status = gsdidiag_populate_uim_cmd(uim_cmd_ptr, uim_cmd_req_ptr,
                                          &(req_table[index].var[1]),
                                          slot_id,
                                          app_id_len,
                                          app_id);

  if ( gsdi_status == GSDI_SUCCESS )
  {
    /* Populate the rest of parameters for the UIM command */
    uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
    uim_cmd_ptr->hdr.rpt_function = gsdidiag_send_cmd_to_uim_cb;
    uim_cmd_ptr->hdr.user_data = int32touint32(index);

    /* Send down the UIM command */
    uim_cmd(uim_cmd_ptr);
  }
  else
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",status,0,0);
      /* Free all the memory and the index for the failed command */
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
  }

  *status = (uint32)gsdi_status;
} /* gsdidiag_send_cmd_to_uim */


/*===========================================================================
FUNCTION gsdidiag_read_cb

DESCRIPTION
  Callback used for an asynchronous gsdi read, sim_data will have the data from
  the sim read.

ARGUMENTS
    gsdi_cnf_T* sim_data: Information, from gsdi task, about request executed

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdidiag_read_cb (const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload; /* used to send event*/
    fs_open_xparms_type open_parms;
    uint8                         len             = 0;
    int                           index           = 0;
    int                           file_descriptor = 0;
    struct fs_stat                file_stat       = {0};

    (void) memset(&open_parms, 0x00, sizeof(fs_open_xparms_type));
    len   = sizeof( event_payload);

    if (sim_data == NULL) {
        return;
    }

    index = uint32toint32(sim_data->message_header.client_ref);
    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table index should be in the used state*/
    if (req_table[index].use_flag!=GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
       MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use in gsdidiag_read_cb",
                 index,0,0);
       event_payload.status = GSDIDIAG_MEMORY_ERR;
       event_report_payload( EVENT_GSDI_SIM_READ, len, (void *)&event_payload);
       return;
    }

    /* test if file already exists */
    if(efs_stat(req_table[index].var[0], &file_stat) == 0)
    {
      /* delete file, it already exists */
      if (efs_unlink (req_table[index].var[0]) == -1) {
          MMGSDI_DEBUG_MSG_ERROR ("File remove failed ", 0, 0, 0);
          event_payload.status = GSDIDIAG_EFS_RM_ERR;
          event_report_payload(EVENT_GSDI_SIM_READ,
                               len,
                              (void *)&event_payload );
          /*free filename memory*/
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
      }
    }
    else if (efs_errno != ENOENT)
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      MMGSDI_DEBUG_MSG_ERROR ("efs_stat failed.", 0, 0, 0);
      event_payload.status = GSDIDIAG_EFS_NAMETEST_ERR;
      event_report_payload(EVENT_GSDI_SIM_READ,
                           len,
                           (void *)&event_payload );
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
      return;
    }

    /* Make sure it is a Read Resp */
    if ( sim_data->message_header.resp_type != GSDI_SIM_READ_RSP )
    {
        MMGSDI_DEBUG_MSG_ERROR ("Rsp from gsdi is was not GSDI_SIM_READ_RSP(%d):%d",
                   GSDI_SIM_READ_RSP,
                   sim_data->message_header.resp_type,
                   0);
        event_payload.status = GSDIDIAG_INCORRECT_RSP_ERR;
        event_report_payload(EVENT_GSDI_SIM_READ,
                             len,
                             (void *)&event_payload );

        /*free memory used for filename*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
    }

    /* check if the read was successful */
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
        open_parms.create.cleanup_option = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;

        file_descriptor = efs_open(req_table[index].var[0], O_CREAT | O_WRONLY);

        if(file_descriptor < 0)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);

          /* send event */
          event_payload.status = GSDIDIAG_EFS_OPEN_ERR;
          event_report_payload(EVENT_GSDI_SIM_READ,
                               len,
                              (void *)&event_payload);

          /* free memory used for filename */
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
        }

        /* write to file */
        if(efs_write(file_descriptor,
                     (void *)sim_data->read_cnf.data,
                     (fs_size_t)sim_data->read_cnf.returned_data_len) <
                     (fs_ssize_t)sim_data->read_cnf.returned_data_len)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_write failed ", 0, 0, 0);
          event_payload.status = GSDIDIAG_EFS_WRITE_ERR;
          event_report_payload( EVENT_GSDI_SIM_READ,
                                len,
                                (void *)&event_payload );
        }
        else
        {
          /* success in writing to file */
          event_payload.status = GSDIDIAG_SUCCESS;
          event_report_payload(EVENT_GSDI_SIM_READ,
                               len,
                               (void *)&event_payload );
        }

        /* close file */
        if(efs_close(file_descriptor) < 0)
        {
          ERR("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
        }

    }else{
        /* Error in gsdi_sim_read cmd*/
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->message_header.gsdi_status, &event_payload.status);
        event_report_payload(EVENT_GSDI_SIM_READ,
                             len,
                             (void *)&event_payload );
    }

    /* free memory used for filename and other vars */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);

    return;
}

/*===========================================================================
FUNCTION gsdidiag_delayed_sim_read_x_cb

DESCRIPTION
    Callback used for a sim read call.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_sim_read_x_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    if (sim_data == NULL) {
        return;
    }

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_OPEN_CHANNEL_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_READ_X_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_open_channel_cb.",index,0,0);
        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);
        return;
    }

   if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->read_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->read_cnf.message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->read_cnf.message_header.gsdi_status;
    }

    if (sim_data->read_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.sim_read_data.sim_filename =
        gsdidiag_convert_uim_file_to_gsdidiag_file(
              sim_data->read_cnf.sim_filename);
      rsp_ptr->rsp_payload.response.sim_read_data.rec_num_accessed =
        sim_data->read_cnf.rec_num_accessed;
      rsp_ptr->rsp_payload.response.sim_read_data.payload.len =
        sim_data->read_cnf.returned_data_len;

      /* if (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater than zero,
         copy the data into the buffer else memset the buffer
        */
      if(rsp_ptr->rsp_payload.response.sim_read_data.payload.len > 0)
      {
          if(rsp_ptr->rsp_payload.response.sim_read_data.payload.len <=
             (GSDI_MAX_DATA_BLOCK_LEN * 2))
          {
            (void) memcpy((uint8 *)rsp_ptr->rsp_payload.response.sim_read_data.payload.buffer,
                 (uint8*)sim_data->read_cnf.data,
                 rsp_ptr->rsp_payload.response.sim_read_data.payload.len
                );
          }
          else
          {
            MMGSDI_DEBUG_MSG_ERROR("Payload Len %d is greater than expected",
                       rsp_ptr->rsp_payload.response.sim_read_data.payload.len,0,0);
            diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

            diagpkt_delay_commit(rsp_ptr);
            return;
          }
      }
      else
      {
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.sim_read_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_delayed_sim_read_cb

DESCRIPTION
    Callback used for a sim read call.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_sim_read_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    if (sim_data == NULL) {
        return;
    }

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_READ_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_READ_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_sim_read_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

   if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->read_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->read_cnf.message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->read_cnf.message_header.gsdi_status;
    }

    if (sim_data->read_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.sim_read_data.sim_filename =
        gsdidiag_convert_uim_file_to_gsdidiag_file(
              sim_data->read_cnf.sim_filename);
      rsp_ptr->rsp_payload.response.sim_read_data.rec_num_accessed =
        sim_data->read_cnf.rec_num_accessed;
      rsp_ptr->rsp_payload.response.sim_read_data.payload.len =
        sim_data->read_cnf.returned_data_len;

      /* if (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater than zero,
         copy the data into the buffer else memset the buffer
        */
      if(rsp_ptr->rsp_payload.response.sim_read_data.payload.len > 0)
      {
          if(rsp_ptr->rsp_payload.response.sim_read_data.payload.len <=
             (GSDI_MAX_DATA_BLOCK_LEN * 2))
          {
            (void)memcpy((uint8 *)rsp_ptr->rsp_payload.response.sim_read_data.payload.buffer,
                 (uint8*)sim_data->read_cnf.data,
                 rsp_ptr->rsp_payload.response.sim_read_data.payload.len
                );
          }
          else
          {
            MMGSDI_DEBUG_MSG_ERROR("Payload Len %d is greater than expected",
                       rsp_ptr->rsp_payload.response.sim_read_data.payload.len,0,0);

            diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

            diagpkt_delay_commit(rsp_ptr);
            return;
          }
      }
      else
      {
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.sim_read_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_delayed_sim_read

DESCRIPTION
    Function to read sim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_sim_read (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;
    uim_items_enum_type        uim_file     = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T         index_mode   = GSDI_NO_INDEX;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */

    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_SIM_READ_CMD;

    uim_file = gsdidiag_convert_file_to_uim_enum(
              req_ptr->request.sim_read_req.sim_filename);
    index_mode = gsdidiag_map_index_mode(
              req_ptr->request.sim_read_req.index_mode);
    /* send the request to do a sim_read*/
    gsdi_status = gsdi_sim_read(uim_file,
                                index_mode,
                                uint32toint32(req_ptr->request.sim_read_req.rec_num),
                                req_ptr->request.sim_read_req.search_string,
                                uint32toint32(req_ptr->request.sim_read_req.search_string_len),
                                NULL,
                                req_ptr->request.sim_read_req.required_data_len,
                                req_ptr->request.sim_read_req.data_offset,
                                NULL,
                                int32touint32(index),
                                (gsdi_async_cb)gsdidiag_delayed_sim_read_cb
                               );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_delayed_sim_read */


/*===========================================================================
FUNCTION gsdidiag_delayed_sim_read_x

DESCRIPTION
    Function to read sim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_sim_read_x (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;
    uim_items_enum_type        uim_file     = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T         index_mode   = GSDI_NO_INDEX;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_SIM_READ_X_CMD;

    uim_file = gsdidiag_convert_file_to_uim_enum(
              req_ptr->request.sim_read_req.sim_filename);
    index_mode = gsdidiag_map_index_mode(
              req_ptr->request.sim_read_req.index_mode);

    /* send the request to do a sim_read*/
    gsdi_status = gsdi_sim_read(uim_file,
                              index_mode,
                              uint32toint32(req_ptr->request.sim_read_req.rec_num),
                              req_ptr->request.sim_read_req.search_string,
                              uint32toint32(req_ptr->request.sim_read_req.search_string_len),
                              NULL,
                              req_ptr->request.sim_read_req.required_data_len,
                              req_ptr->request.sim_read_req.data_offset,
                              NULL,
                              int32touint32(index),
                              (gsdi_async_cb)gsdidiag_delayed_sim_read_x_cb
                             );

    if (gsdi_status != GSDI_SUCCESS )
    {
       MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",status,0,0);
       gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_delayed_sim_read_x */


/*===========================================================================
FUNCTION gsdidiag_delayed_force_sim_read

DESCRIPTION
    Function to read sim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_force_sim_read (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;
    uim_items_enum_type        uim_file     = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T         index_mode   = GSDI_NO_INDEX;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        rsp_id = 0;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_FORCE_SIM_READ_CMD;

    uim_file = gsdidiag_convert_file_to_uim_enum(
              req_ptr->request.sim_read_req.sim_filename);
    index_mode = gsdidiag_map_index_mode(
              req_ptr->request.sim_read_req.index_mode);

    /* send the request to do a sim_read*/
    gsdi_status = gsdi_force_sim_read(uim_file,
                                index_mode,
                                uint32toint32(req_ptr->request.sim_read_req.rec_num),
                                req_ptr->request.sim_read_req.required_data_len,
                                req_ptr->request.sim_read_req.data_offset,
                                int32touint32(index),
                                (gsdi_async_cb)gsdidiag_delayed_sim_read_cb
                               );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
    rsp_id = (uint16)req_table[index].transaction_id;
} /* gsdidiag_delayed_force_sim_read */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED WRITE TRANSPARENT CALLBACK

DESCRIPTION
    This function is the callback function used for a write transparent call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the file name to which data has been written.
===========================================================================*/
static void gsdidiag_delayed_write_transparent_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type    * rsp_ptr; /* response pointer */
  uint32                             len;     /* length of event */
  int                                index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  if (cnf_type != MMGSDI_WRITE_CNF)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf type not WRITE 0x%x",cnf_type,0,0);
    return;
  }
  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
              req_table[index].transaction_id,
              len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_WRITE_TRANSPARENT_CMD or MMGSDIDIAG_WRITE_TRANSPARENT_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_write_transparent_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  rsp_ptr->rsp_payload.response.write_transparent_data.access_method =
      gsdidiag_convert_access_method_from_mmgsdi_enum(
        cnf_data->write_cnf.access.access_method);

  if (rsp_ptr->rsp_payload.response.write_transparent_data.access_method ==
      GSDIDIAG_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.write_transparent_data.file_enum =
      gsdidiag_convert_file_from_mmgsdi_enum(
        cnf_data->write_cnf.access.file.file_enum);
  }
  else if (rsp_ptr->rsp_payload.response.write_transparent_data.access_method ==
           GSDIDIAG_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.write_transparent_data.path.path_len =
      cnf_data->write_cnf.access.file.path_type.path_len;
    if( (cnf_data->write_cnf.access.file.path_type.path_len > 0) &&
        (cnf_data->write_cnf.access.file.path_type.path_len <= MMGSDIDIAG_PATH_LEN))
    {
      memcpy((uint16*)rsp_ptr->rsp_payload.response.write_transparent_data.path.path_buf,
          (uint16*)cnf_data->write_cnf.access.file.path_type.path_buf,
          sizeof(uint16)*(int32touint32(cnf_data->write_cnf.access.file.path_type.path_len)));
    }
    else
    {
      MSG_ERROR("path_len is <=0 or greater than expected",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;
    }
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.write_transparent_data.len =
      cnf_data->write_cnf.written_data_len;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}

/*===========================================================================
FUNCTION gsdidiag_delayed_write_transparent

DESCRIPTION
    Function to write to the sim/usim. A task is queued to gsdi task to write sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_write_transparent (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
    int                        index         = 0;
    mmgsdi_write_data_type     write_data;
    mmgsdi_access_type         access_info;
    mmgsdi_slot_id_enum_type   mmgsdi_slot   = MMGSDI_SLOT_1;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status is NULL", 0, 0, 0);
      return;
    }

    if (req_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);
    write_data.data_len = req_ptr->request.write_transparent_req.write_data.data_len;
    write_data.data_ptr = req_ptr->request.write_transparent_req.write_data.data_ptr;

    gsdidiag_copy_access_to_mmgsdi(&access_info, &req_ptr->request.write_transparent_req.access_method);

    if (req_ptr->request.write_transparent_req.access_method.access_method == GSDIDIAG_EF_ENUM_ACCESS)
    {
      /* send the request to do a write transparent*/
      mmgsdi_status = mmgsdi_write_transparent (req_ptr->request.write_transparent_req.client_id,
                                              mmgsdi_slot,
                                              access_info.file.file_enum,
                                              req_ptr->request.write_transparent_req.offset,
                                              write_data,
                                              (mmgsdi_callback_type)gsdidiag_delayed_write_transparent_cb,
                                              int32touint32(index));
    }
    else if (req_ptr->request.write_transparent_req.access_method.access_method == GSDIDIAG_BY_PATH_ACCESS)
    {
      /* send the request to do a write transparent*/
      mmgsdi_status = mmgsdi_write_transparent_ext (
                                      req_ptr->request.write_transparent_req.client_id,
                                      mmgsdi_slot,
                                      access_info,
                                      req_ptr->request.write_transparent_req.offset,
                                      write_data,
                                      (mmgsdi_callback_type)gsdidiag_delayed_write_transparent_cb,
                                      int32touint32(index));
    }

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_write_transparent queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_write_transparent */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED WRITE RECORD CALLBACK

DESCRIPTION
    This function is the callback function used for a write transparent call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_write_record_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                           len;     /* length of event */
  int                              index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }
  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));



  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_WRITE_RECORD_CMD or MMGSDIDIAG_WRITE_RECORD_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_writeb record_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  rsp_ptr->rsp_payload.response.write_record_data.access_method =
      gsdidiag_convert_access_method_from_mmgsdi_enum(
        cnf_data->write_cnf.access.access_method);

  if (rsp_ptr->rsp_payload.response.write_record_data.access_method ==
      GSDIDIAG_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.write_record_data.file_enum =
      gsdidiag_convert_file_from_mmgsdi_enum(
        cnf_data->write_cnf.access.file.file_enum);
  }
  else if (rsp_ptr->rsp_payload.response.write_record_data.access_method ==
           GSDIDIAG_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.write_record_data.path.path_len =
      cnf_data->write_cnf.access.file.path_type.path_len;
    if( (cnf_data->write_cnf.access.file.path_type.path_len > 0) &&
        (cnf_data->write_cnf.access.file.path_type.path_len <= MMGSDIDIAG_PATH_LEN))
    {
      memcpy((uint16*)rsp_ptr->rsp_payload.response.write_record_data.path.path_buf,
          (uint16*)cnf_data->write_cnf.access.file.path_type.path_buf,
          sizeof(uint16)*(int32touint32(cnf_data->write_cnf.access.file.path_type.path_len)));
    }
    else
    {
      MSG_ERROR("path_len is <=0 or greater than expected",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;

    }
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.write_record_data.len =
      cnf_data->write_cnf.response_header.payload_len;
    rsp_ptr->rsp_payload.response.write_record_data.rec_num_accessed=
      int32touint8(cnf_data->write_cnf.accessed_rec_num);
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_write_record

DESCRIPTION
    Function to write to the sim/usim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_write_record (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type      *req_ptr        = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
    int                             index          = 0;
    mmgsdi_write_data_type          write_data;
    mmgsdi_slot_id_enum_type        mmgsdi_slot    = MMGSDI_SLOT_1;
    mmgsdi_access_type              access_info;
    mmgsdi_file_structure_enum_type mmgsdi_file_structure = MMGSDI_MAX_FILE_STRUCTURE_ENUM;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status is NULL", 0, 0, 0);
      return;
    }

    if (req_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);
    write_data.data_len = req_ptr->request.write_record_req.write_data.data_len;
    write_data.data_ptr = req_ptr->request.write_record_req.write_data.data_ptr;

    gsdidiag_copy_access_to_mmgsdi(&access_info, &req_ptr->request.write_record_req.access_method);

    switch (req_ptr->request.write_record_req.file_type)
    {
      case MMGSDIDIAG_LINEAR_FIXED_FILE:
        mmgsdi_file_structure = MMGSDI_LINEAR_FIXED_FILE;
        break;
      case MMGSDIDIAG_CYCLIC_FILE:
        mmgsdi_file_structure = MMGSDI_CYCLIC_FILE;
        break;
      case MMGSDIDIAG_TRANSPARENT_FILE:
        mmgsdi_file_structure = MMGSDI_TRANSPARENT_FILE;
        break;
      case MMGSDIDIAG_MAX_FILE_STRUCTURE_ENUM:
        mmgsdi_file_structure = MMGSDI_MAX_FILE_STRUCTURE_ENUM;
        break;
    }
    if (req_ptr->request.write_record_req.access_method.access_method == GSDIDIAG_EF_ENUM_ACCESS)
    {
      /* send the request to do a sim write*/
      mmgsdi_status = mmgsdi_write_record(req_ptr->request.write_record_req.client_id,
                                        mmgsdi_slot,
                                        access_info.file.file_enum,
                                        req_ptr->request.write_record_req.record_number,
                                        write_data,
                                        (mmgsdi_callback_type)gsdidiag_delayed_write_record_cb,
                                        int32touint32(index));
    }
    else if (req_ptr->request.write_record_req.access_method.access_method == GSDIDIAG_BY_PATH_ACCESS)
    {
      /* send the request to do a sim write*/
      mmgsdi_status = mmgsdi_write_record_ext(req_ptr->request.write_record_req.client_id,
                                            mmgsdi_slot,
                                            access_info,
                                            mmgsdi_file_structure,
                                            req_ptr->request.write_record_req.record_number,
                                            write_data,
                                            (mmgsdi_callback_type)gsdidiag_delayed_write_record_cb,
                                            int32touint32(index));
    }


    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_write_record queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_write_record */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED READ TRANSPARENT CALLBACK

DESCRIPTION
    This function is the callback function used for a read transparent call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_read_transparent_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                             len;     /* length of event */
  int                                index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_READ_TRANSPARENT_CMD or MMGSDIDIAG_READ_TRANSPARENT_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_transparent_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  rsp_ptr->rsp_payload.response.read_transparent_data.access_method =
      gsdidiag_convert_access_method_from_mmgsdi_enum(
        cnf_data->read_cnf.access.access_method);

  if (rsp_ptr->rsp_payload.response.read_transparent_data.access_method == GSDIDIAG_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.read_transparent_data.file_enum =
      gsdidiag_convert_file_from_mmgsdi_enum(
        cnf_data->read_cnf.access.file.file_enum);
  }
  else if (rsp_ptr->rsp_payload.response.read_transparent_data.access_method == GSDIDIAG_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.read_transparent_data.path.path_len =
      cnf_data->read_cnf.access.file.path_type.path_len;
    if( (cnf_data->read_cnf.access.file.path_type.path_len > 0) &&
        (cnf_data->read_cnf.access.file.path_type.path_len <= MMGSDIDIAG_PATH_LEN))
    {
      memcpy((uint16*)rsp_ptr->rsp_payload.response.read_transparent_data.path.path_buf,
          (uint16*)cnf_data->read_cnf.access.file.path_type.path_buf,
          sizeof(uint16)*(int32touint32(cnf_data->read_cnf.access.file.path_type.path_len)));
    }
    else
    {
      MSG_ERROR("path_len is <=0 or greater than expected",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;
    }
  }
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.read_transparent_data.payload.len =
      int32touint32(cnf_data->read_cnf.read_data.data_len);
    if((rsp_ptr->rsp_payload.response.read_transparent_data.payload.len > 0) &&
      (rsp_ptr->rsp_payload.response.read_transparent_data.payload.len < MMGSDIDIAG_MAX_DATA_BLOCK_LEN ))
    {
        (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.read_transparent_data.payload.buffer,
          (uint8*)cnf_data->read_cnf.read_data.data_ptr,
          sizeof(uint8) * rsp_ptr->rsp_payload.response.read_transparent_data.payload.len);
    }
    else
    {
      rsp_ptr->rsp_payload.response.read_transparent_data.payload.len = 0;
      (void)memset((uint8*)rsp_ptr->rsp_payload.response.read_transparent_data.payload.buffer,
              0x00, int32touint32(MMGSDIDIAG_MAX_DATA_BLOCK_LEN));
    }/* if(rsp_ptr->rsp_payload.response.read_transparent_data.payload.len > 0) */
  }/* if (cnf_data->cnf.read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_read_transparent

DESCRIPTION
    Function to read from the sim/usim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_read_transparent (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
    int                        index         = 0;
    mmgsdi_access_type         access_info;
    mmgsdi_slot_id_enum_type   mmgsdi_slot   = MMGSDI_SLOT_1;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status is NULL", 0, 0, 0);
      return;
    }

    if (req_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    gsdidiag_copy_access_to_mmgsdi(&access_info, &req_ptr->request.read_transparent_req.access_method);

    if (req_ptr->request.read_transparent_req.access_method.access_method == GSDIDIAG_EF_ENUM_ACCESS)
    {
      /* send the request to do a read transparent*/
      mmgsdi_status = mmgsdi_read_transparent (req_ptr->request.read_transparent_req.client_id,
                                              mmgsdi_slot,
                                              access_info.file.file_enum,
                                              req_ptr->request.read_transparent_req.offset,
                                              req_ptr->request.read_transparent_req.req_data_len,
                                              (mmgsdi_callback_type)gsdidiag_delayed_read_transparent_cb,
                                              int32touint32(index));
    }
    else if (req_ptr->request.read_transparent_req.access_method.access_method == GSDIDIAG_BY_PATH_ACCESS)
    {
      /* send the request to do a read transparent*/
      mmgsdi_status = mmgsdi_read_transparent_ext (
                                      req_ptr->request.read_transparent_req.client_id,
                                      mmgsdi_slot,
                                      access_info,
                                      req_ptr->request.read_transparent_req.offset,
                                      req_ptr->request.read_transparent_req.req_data_len,
                                      (mmgsdi_callback_type)gsdidiag_delayed_read_transparent_cb,
                                      int32touint32(index));
    }

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_read_transparent queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_read_transparent */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED READ RECORD CALLBACK

DESCRIPTION
    This function is the callback function used for a read record transparent call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_read_record_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                             len;     /* length of event */
  int                                index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));



  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_READ_RECORD_CMD or MMGSDIDIAG_READ_RECORD_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_record_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status =
      mmgsdi_status;
  }

 rsp_ptr->rsp_payload.response.read_record_data.access_method =
      gsdidiag_convert_access_method_from_mmgsdi_enum(
        cnf_data->read_cnf.access.access_method);

  if (rsp_ptr->rsp_payload.response.read_record_data.access_method ==
      GSDIDIAG_EF_ENUM_ACCESS)
  {
    rsp_ptr->rsp_payload.response.read_record_data.file_enum =
      gsdidiag_convert_file_from_mmgsdi_enum(
        cnf_data->read_cnf.access.file.file_enum);
  }
  else if (rsp_ptr->rsp_payload.response.read_record_data.access_method ==
           GSDIDIAG_BY_PATH_ACCESS)
  {
    rsp_ptr->rsp_payload.response.read_record_data.path.path_len =
      cnf_data->read_cnf.access.file.path_type.path_len;
    if( (cnf_data->read_cnf.access.file.path_type.path_len > 0 ) &&
        (cnf_data->read_cnf.access.file.path_type.path_len <= MMGSDIDIAG_PATH_LEN ))
    {
      memcpy((uint16*)rsp_ptr->rsp_payload.response.read_record_data.path.path_buf,
          (uint16*)cnf_data->read_cnf.access.file.path_type.path_buf,
          sizeof(uint16)*(int32touint32(cnf_data->read_cnf.access.file.path_type.path_len)));
    }
    else
    {
      MSG_ERROR("path_len is <=0 or greater than expected",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;

    }
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.read_record_data.rec_num_accessed =
      cnf_data->read_cnf.accessed_rec_num;
    rsp_ptr->rsp_payload.response.read_record_data.payload.len =
      int32touint32(cnf_data->read_cnf.read_data.data_len);
    if((rsp_ptr->rsp_payload.response.read_record_data.payload.len > 0) &&
        (rsp_ptr->rsp_payload.response.read_record_data.payload.len < MMGSDIDIAG_MAX_DATA_BLOCK_LEN ))
    {
      (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.read_record_data.payload.buffer,
          (uint8*)cnf_data->read_cnf.read_data.data_ptr,
          sizeof(uint8) * rsp_ptr->rsp_payload.response.read_record_data.payload.len);
    }
    else
    {
      rsp_ptr->rsp_payload.response.read_record_data.payload.len = 0;
      (void)memset((uint8*)rsp_ptr->rsp_payload.response.read_record_data.payload.buffer,
              0x00,MMGSDIDIAG_MAX_DATA_BLOCK_LEN);
    }/* if(rsp_ptr->rsp_payload.response.read_record_data.payload.len > 0) */
  }/* if (cnf_data->cnf.read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_read_record

DESCRIPTION
    Function to read from the sim/usim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_read_record (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
  gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  int                        index         = 0;
  mmgsdi_access_type         access_info;
  mmgsdi_slot_id_enum_type   mmgsdi_slot   = MMGSDI_SLOT_1;

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (status == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if (req_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id =   rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

  gsdidiag_copy_access_to_mmgsdi(&access_info, &req_ptr->request.read_record_req.access_method);

  if (req_ptr->request.read_record_req.access_method.access_method == GSDIDIAG_EF_ENUM_ACCESS)
  {
    /* send the request to do a sim_read*/
    mmgsdi_status = mmgsdi_read_record(req_ptr->request.read_record_req.client_id,
                                      mmgsdi_slot,
                                      access_info.file.file_enum,
                                      req_ptr->request.read_record_req.record_number,
                                      req_ptr->request.read_record_req.req_data_len,
                                      (mmgsdi_callback_type)gsdidiag_delayed_read_record_cb,
                                      int32touint32(index));
  }
  else if (req_ptr->request.read_record_req.access_method.access_method == GSDIDIAG_BY_PATH_ACCESS)
  {
    /* send the request to do a sim_read*/
    mmgsdi_status = mmgsdi_read_record_ext(req_ptr->request.read_record_req.client_id,
                                          mmgsdi_slot,
                                          access_info,
                                          req_ptr->request.read_record_req.record_number,
                                          req_ptr->request.read_record_req.req_data_len,
                                          (mmgsdi_callback_type)gsdidiag_delayed_read_record_cb,
                                          int32touint32(index));
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_read_record queue failed %d",
                              mmgsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
    mmgsdi_status, status);
} /* gsdidiag_delayed_read_record */


/*===========================================================================
FUNCTION GSDIDIAG DELAYED CLIENT ID REGISTER CALLBACK

DESCRIPTION
    This function is the callback function used for a client id register call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the get file attribute information
===========================================================================*/
static void gsdidiag_delayed_client_id_reg_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type    * rsp_ptr; /* response pointer */
  uint32                             len = 0;     /* length of event */
  int                                index = 0;   /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type    * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_CLIENT_ID_REG_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CLIENT_ID_REG_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_client_id_reg_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.client_id_reg_data.client_id_low =
      (int)((int64)cnf_data->client_id_reg_cnf.response_header.client_id & 0xFFFFFFFF);
    rsp_ptr->rsp_payload.response.client_id_reg_data.client_id_high =
      (int)((int64)(cnf_data->client_id_reg_cnf.response_header.client_id >> 32) & 0xFFFFFFFF);
  }/* if (cnf_data->cnf.read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_client_id_reg

DESCRIPTION
    Function to register client ID of SIM/USIM/RUIM. A task is queued to
    gsdi task to register client ID.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by MMGSDI_CLIENT_ID_REG API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_client_id_reg (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_CLIENT_ID_REG_CMD;

    /* send the request to do a mmgsdi_client_id_reg*/
    mmgsdi_status = mmgsdi_client_id_reg(
                                        (mmgsdi_callback_type)gsdidiag_delayed_client_id_reg_cb,
                                        int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_client_id_reg queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_client_id_reg */ /*lint !e550*/


/*===========================================================================
FUNCTION GSDIDIAG_EVENT_CB

DESCRIPTION
    This function is the event handling function for different events from MMGSDI.

DEPENDENCIES
    None

INPUT
    mmgsdi_event_data_type *event_ptr

RETURN VALUE
    None

OUTPUT
    None
===========================================================================*/
static void gsdidiag_event_cb(const mmgsdi_event_data_type *event_ptr)
{
  mmgsdidiag_event_payload_type event_payload; /* used to send event*/
  uint8 len = 0; /* length of the event_payload*/

  MSG_HIGH("FUNCTION gsdidiag_event_cb",0,0,0);

  (void)memset((void*)&event_payload,0x00,sizeof(mmgsdidiag_event_payload_type));
  len   = sizeof( event_payload);
  if(event_ptr == NULL)
  {
    MSG_ERROR("Refresh:  event_ptr is NULL",0,0,0);
    return;
  }
  event_payload.evt = event_ptr->evt;

  /* Currently handling only refresh events to Diag */
  switch (event_ptr->evt)
  {
    case MMGSDI_REFRESH_EVT:
      event_payload.event_payload[0] = (uint8)event_ptr->data.refresh.slot;
      event_payload.event_payload[1] = (uint8)event_ptr->data.refresh.stage;
      event_payload.event_payload[2] = (uint8)event_ptr->data.refresh.mode;
      event_payload.event_payload[3] = (uint8)event_ptr->data.refresh.orig_mode;
      break;
    default:
      break;
  }/*switch (event_ptr->evt)*/
  event_report_payload( EVENT_MMGSDI_EVENT, len, (void *)&event_payload);
}/* end of gsdidiag_event_cb()*/


/*===========================================================================

FUNCTION GSDIDIAG DELAYED REGISTER FOR EVENT CALLBACK

DESCRIPTION
    This function is the callback function used for a register for event call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_register_for_event_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type  *rsp_ptr = NULL; /* response pointer */
  uint32                         len = 0;     /* length of event */
  int                            index = -1;   /* index into req_table*/
  boolean                        index_is_invalid = TRUE;

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data_ptr == NULL) {
    MMGSDI_DEBUG_MSG_ERROR("Input parameter cnf_data_ptr invalid",0,0,0);
    return;
  }

  index = uint32toint32(cnf_data_ptr->response_header.client_data);

  index_is_invalid = gsdidiag_is_index_invalid(index);
  if (TRUE == index_is_invalid)
  {
    /* index is found to be invalid */
    MMGSDI_DEBUG_MSG_ERROR("Index found to be invalid- failed",0,0,0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_REGISTER_FOR_EVENT_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  MMGSDIDIAG_REGISTER_FOR_EVENT_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_register_for_event_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (!(req_table[index].aps_as_transport))
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*end of gsdidiag_delayed_register_for_event_cb() *//*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_register_for_event

DESCRIPTION
   This function handles the register for mmgsdi-events command from diag

INPUT:
    PACKED void *req_input_ptr : pointer to request packet of type
    word pkt_len               : length of request packet
    uint32 * status_ptr        : Status returned by MMGSDI_REGISTER_FOR_EVENT API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_register_for_event (
    PACKED void *req_input_ptr,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status_ptr,        /* Status returned by MMGSDI_REGISTER_FOR_EVENT API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req_input_ptr;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;

    if (status_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status_ptr null",0,0,0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr null",0,0,0);
      *status_ptr = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = MMGSDIDIAG_REGISTER_FOR_EVENT_CMD;

    /* invoke MMGSDI client event register */
    mmgsdi_status = mmgsdi_client_evt_reg(req_ptr->request.client_evt_reg_req.client_id,
                  gsdidiag_event_cb,
                  (mmgsdi_callback_type)gsdidiag_delayed_register_for_event_cb,
                  int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_register_for_event queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status_ptr);
} /* gsdidiag_delayed_register_for_event() */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED REGISTER FOR REFRESH CALLBACK

DESCRIPTION
    This function is the callback function used for a register for refresh command.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_register_for_refresh_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type  *rsp_ptr = NULL;  /* response pointer */
  uint32                         len      = 0;     /* length of event */
  int                            index    = -1;    /* index into req_table*/
  boolean                        index_is_invalid = TRUE;

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data_ptr == NULL) {
    MMGSDI_DEBUG_MSG_ERROR("Input parameter cnf_data_ptr invalid",0,0,0);
    return;
  }

  index = uint32toint32(cnf_data_ptr->response_header.client_data);

  index_is_invalid = gsdidiag_is_index_invalid(index);
  if (TRUE == index_is_invalid)
  {
    /* index is found to be invalid */
    MMGSDI_DEBUG_MSG_ERROR("Index found to be invalid- failed",0,0,0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_REFRESH_REQ_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_register_for_refresh_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (!(req_table[index].aps_as_transport))
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}/* end of gsdidiag_delayed_register_for_refresh_cb()*//*lint !e715 */


/*===========================================================================
FUNCTION GSDIDIAG DELAYED DEREGISTER FOR REFRESH CALLBACK

DESCRIPTION
    This function is the callback function used for a deregister from refresh
    command.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_deregister_for_refresh_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type  *rsp_ptr = NULL;  /* response pointer */
  uint32                         len      = 0;     /* length of event */
  int                            index    = -1;    /* index into req_table*/
  boolean                        index_is_invalid = TRUE;

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data_ptr == NULL) {
    MMGSDI_DEBUG_MSG_ERROR("Input parameter cnf_data_ptr invalid",0,0,0);
    return;
  }

  index = uint32toint32(cnf_data_ptr->response_header.client_data);

  index_is_invalid = gsdidiag_is_index_invalid(index);
  if (TRUE == index_is_invalid)
  {
    /* index is found to be invalid */
    MMGSDI_DEBUG_MSG_ERROR("Index found to be invalid- failed",0,0,0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                req_table[index].transaction_id,
                len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_REFRESH_REQ_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR(
      "Index %d is not in use gsdidiag_delayed_deregister_for_refresh_cb.",
      index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (!(req_table[index].aps_as_transport))
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}/* end of gsdidiag_delayed_deregister_for_refresh_cb()*//*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_register_for_refresh

DESCRIPTION
   This function handles the register for refresh command from diag

INPUT:
    PACKED void *req_input_ptr : pointer to request packet of type
    word pkt_len               : length of request packet
    uint32 * status_ptr        : Status returned by MMGSDI_REGISTER_FOR_REFRESH API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_register_for_refresh(
    PACKED void  *req_input_ptr,  /* pointer to request packet  */
    word         pkt_len,         /* length of request packet */
    uint32       *status_ptr,     /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32       aps_as_transport
)
{
  gsdidiag_req_payload_type     *req_ptr         = (gsdidiag_req_payload_type  *)req_input_ptr;
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_ERROR;
  int                           index            = 0;
  mmgsdi_refresh_file_list_type mmgsdi_file_list = {0};
  uint32                        i                = 0;
  mmgsdi_option_type            option           = {0};

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status_ptr is NULL", 0,0,0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0,0,0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  if ((req_ptr->request.refresh_reg_req.path_len > 0) ||
      (req_ptr->request.refresh_reg_req.num_file_enums > 0))
  {
    if(req_ptr->request.refresh_reg_req.file_list_ptr != NULL)
    {
      mmgsdi_file_list.file_list_ptr = mmgsdi_malloc(
                            req_ptr->request.refresh_reg_req.num_file_enums *
                            sizeof(mmgsdi_file_enum_type));

      /* check for file_list_ptr*/
      if (mmgsdi_file_list.file_list_ptr == NULL)
      {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("file_list_ptr is NULL",0, 0, 0);
        *status_ptr = GSDIDIAG_MEMORY_ERR;
        gsdidiag_free_index(index);
        return;
      }

      for (i = 0; i< req_ptr->request.refresh_reg_req.num_file_enums; i++)
      {
        mmgsdi_file_list.file_list_ptr[i] = gsdidiag_convert_file_to_mmgsdi_enum(
          req_ptr->request.refresh_reg_req.file_list_ptr[i]);
      }
    } /* if file_list_ptr != NULL */

    /* assign file_path_ptr directly, there is no need to do malloc */
    mmgsdi_file_list.file_path_ptr =
      req_ptr->request.refresh_reg_req.file_path_ptr;
  }
  mmgsdi_file_list.num_files = req_ptr->request.refresh_reg_req.num_file_enums;
  mmgsdi_file_list.file_path_len = req_ptr->request.refresh_reg_req.path_len ;

  /* invoke MMGSDI refresh register */
  mmgsdi_status = mmgsdi_register_for_refresh (req_ptr->request.refresh_reg_req.client_id,
                            MMGSDI_SLOT_1,
                            mmgsdi_file_list,
                            (boolean)req_ptr->request.refresh_reg_req.vote_for_init,
                            option,
                            (mmgsdi_callback_type)gsdidiag_delayed_register_for_refresh_cb,
                            int32touint32(index));

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_client_id_reg queue failed %d",
                              mmgsdi_status,0,0);
      gsdidiag_free_index(index);
  }
  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
    mmgsdi_status, status_ptr);

  /* Free the allocated memory */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_file_list.file_list_ptr);

} /*gsdidiag_delayed_register_for_refresh()*/


/*===========================================================================
FUNCTION gsdidiag_delayed_deregister_for_refresh

DESCRIPTION
   This function handles the deregister for refresh command from diag

INPUT:
    PACKED void *req_input_ptr : pointer to request packet of type
    word pkt_len               : length of request packet
    uint32 * status_ptr        : Status returned by MMGSDI_REGISTER_FOR_REFRESH API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_deregister_for_refresh(
    PACKED void *req_input_ptr,                /* pointer to request packet  */
    word pkt_len,                              /* length of request packet */
    uint32 * status_ptr,                       /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
  gsdidiag_req_payload_type     *req_ptr         = (gsdidiag_req_payload_type  *)req_input_ptr;
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_ERROR;
  int                           index            = 0;
  mmgsdi_refresh_file_list_type mmgsdi_file_list = {0};
  uint32                        i                = 0;
  mmgsdi_option_type            option           = {0};

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status_ptr Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  if ((req_ptr->request.refresh_dereg_req.path_len > 0) ||
      (req_ptr->request.refresh_dereg_req.num_file_enums > 0))
  {
    if(req_ptr->request.refresh_dereg_req.file_list_ptr != NULL)
    {
      mmgsdi_file_list.file_list_ptr = mmgsdi_malloc(
                          req_ptr->request.refresh_dereg_req.num_file_enums *
                          sizeof(mmgsdi_file_enum_type));
      /* check for uim_file_ptr*/
      if (mmgsdi_file_list.file_list_ptr == NULL)
      {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("uim_file_ptr is NULL",0, 0, 0);
        *status_ptr = GSDIDIAG_MEMORY_ERR;
        gsdidiag_free_index(index);
        return;
      }

      for (i = 0; i< req_ptr->request.refresh_dereg_req.num_file_enums; i++)
      {
        mmgsdi_file_list.file_list_ptr[i] = gsdidiag_convert_file_to_mmgsdi_enum(
                              req_ptr->request.refresh_dereg_req.file_list_ptr[i]);
      }
    } /* if file_list_ptr != NULL */

    /* assign file_path_ptr directly, there is no need to do malloc */
    mmgsdi_file_list.file_path_ptr =
      req_ptr->request.refresh_dereg_req.file_path_ptr;
  }
  mmgsdi_file_list.num_files = req_ptr->request.refresh_dereg_req.num_file_enums;
  mmgsdi_file_list.file_path_len = req_ptr->request.refresh_dereg_req.path_len;

  /* invoke MMGSDI refresh deregister */
  mmgsdi_status = mmgsdi_deregister_for_refresh (
                    req_ptr->request.refresh_dereg_req.client_id,
                    MMGSDI_SLOT_1,
                    mmgsdi_file_list,
                    (boolean)req_ptr->request.refresh_dereg_req.vote_for_init,
                    option,
                    (mmgsdi_callback_type)gsdidiag_delayed_deregister_for_refresh_cb,
                    int32touint32(index));

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR(
      "gsdidiag_delayed_deregister_for_refresh queue failed 0x%x",
      mmgsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
    mmgsdi_status, status_ptr);

  /* Free the allocated memory */
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_file_list.file_list_ptr);

} /*gsdidiag_delayed_deregister_for_refresh()*/


/*===========================================================================

FUNCTION GSDIDIAG DELAYED OK TO REFRESH CALLBACK

DESCRIPTION
    This function is the callback function used for ok to refresh call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_ok_to_refresh_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type  *rsp_ptr = NULL; /* response pointer */
  uint32                          len     = 0;     /* length of event */
  int                             index   = -1;   /* index into req_table*/
  boolean                         index_is_invalid = TRUE;

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("Input parameter cnf_data_ptr invalid",0,0,0);
    return;
  }

  index = uint32toint32(cnf_data_ptr->response_header.client_data);

  index_is_invalid = gsdidiag_is_index_invalid(index);
  if (TRUE == index_is_invalid)
  {
    /* index is found to be invalid */
    MMGSDI_DEBUG_MSG_ERROR("Index found to be invalid- failed",0,0,0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_OK_TO_REFRESH_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  MMGSDIDIAG_OK_TO_REFRESH_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_record_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (!(req_table[index].aps_as_transport))
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* end of function gsdidiag_delayed_ok_to_refresh_cb() *//*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_ok_to_refresh

DESCRIPTION
   This function handles the ok to refresh command from diag

INPUT:
    PACKED void *req_input_ptr : pointer to request packet of type
    word pkt_len               : length of request packet
    uint32 * status_ptr        : Status returned by MMGSDI_REGISTER_FOR_EVENT API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_ok_to_refresh (
    PACKED void *req_input_ptr,  /* pointer to request packet  */
    word pkt_len,                /* length of request packet */
    uint32 * status_ptr,         /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req_input_ptr;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;
    mmgsdi_option_type         option         = {0};

    if (status_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status_ptr null",0,0,0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null",0,0,0);
      *status_ptr = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = MMGSDIDIAG_OK_TO_REFRESH_CMD;

    option.tlv_num = 0;

    /* Invoke OK to refresh */
    mmgsdi_status = mmgsdi_ok_to_refresh(req_ptr->request.ok_to_refresh_req.client_id,
                               MMGSDI_SLOT_1,
                               TRUE,
                               option,
                               gsdidiag_delayed_ok_to_refresh_cb,
                               int32touint32(index));
   /* how is the call back for the request can be triggered */
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status_ptr);
}/* gsdidiag_delayed_ok_to_refresh()*/


/*===========================================================================

FUNCTION GSDIDIAG DELAYED CREATE PKCS15 LOOKUP TABLE CALLBACK

DESCRIPTION
    This function is the callback function used for create pkcs15 lookup
    table.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data_ptr

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the data read from the card.
===========================================================================*/
static void gsdidiag_delayed_create_pkcs15_lookup_table_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type *rsp_ptr          = NULL;/* response pointer */
  uint32                         len              = 0;   /* length of event */
  int                            index            = -1;  /* index to req_table*/
  boolean                        index_is_invalid = TRUE;
  int i =0;

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("Input parameter cnf_data_ptr invalid",0,0,0);
    return;
  }

  index = uint32toint32(cnf_data_ptr->response_header.client_data);

  index_is_invalid = gsdidiag_is_index_invalid(index);
  if (TRUE == index_is_invalid)
  {
    /* index is found to be invalid */
    MMGSDI_DEBUG_MSG_ERROR("Index found to be invalid- failed",0,0,0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_create_pkcs15_lookup_table_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (!(req_table[index].aps_as_transport))
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.create_pkcs15_table_data.pkcs_table.no_of_pkcs15_lookup_table_entries =
        cnf_data_ptr->create_pkcs15_table_cnf.pkcs15_lookup_table.no_of_pkcs15_lookup_table_entries;

    for (i=0;i<MMGSDI_MAX_PKCS15_TABLE_ENTRIES;i++)
    {
      /* GSDIDIAG FILE ENUM */
      rsp_ptr->rsp_payload.response.create_pkcs15_table_data.pkcs_table.pkcs15_lookup_table[i].pkcs15_file_enum =
        gsdidiag_convert_file_from_mmgsdi_enum(
               cnf_data_ptr->create_pkcs15_table_cnf.pkcs15_lookup_table.pkcs15_lookup_table[i].pkcs15_file_enum);

      /* FILE PATH LEN */
      rsp_ptr->rsp_payload.response.create_pkcs15_table_data.pkcs_table.pkcs15_lookup_table[i].pkcs15_file_path.path_len =
            cnf_data_ptr->create_pkcs15_table_cnf.pkcs15_lookup_table.pkcs15_lookup_table[i].pkcs15_file_path.path_len;

      /* FILE PATH */
      memcpy((uint16 *)rsp_ptr->rsp_payload.response.create_pkcs15_table_data.pkcs_table.pkcs15_lookup_table[i].pkcs15_file_path.path_buf,
            (uint16*)cnf_data_ptr->create_pkcs15_table_cnf.pkcs15_lookup_table.pkcs15_lookup_table[i].pkcs15_file_path.path_buf,
            int32touint32(cnf_data_ptr->create_pkcs15_table_cnf.pkcs15_lookup_table.pkcs15_lookup_table[i].pkcs15_file_path.path_len
                          * sizeof(uint16)));

    }

  } /* end of if (mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_create_pkcs15_lookup_table_cb() *//*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_create_pkcs15_lookup_table

DESCRIPTION
   This function handles create pkcs15 lookup table from diag

INPUT:
    PACKED void *req_input_ptr : pointer to request packet of type
    word pkt_len               : length of request packet
    uint32 * status_ptr        : Status returned by MMGSDI_REGISTER_FOR_EVENT API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_create_pkcs15_lookup_table (
    PACKED void *req_input_ptr,  /* pointer to request packet  */
    word pkt_len,                /* length of request packet */
    uint32 * status_ptr,         /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req_input_ptr;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;
    mmgsdi_option_type         option         = {0};

    if (status_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status_ptr Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status_ptr = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD;

    option.tlv_num = 0;

    /* Invoke Create PKCS15 lookup table */
    mmgsdi_status = mmgsdi_create_pkcs15_lookup_table(req_ptr->request.create_pkcs15_lookup_table_req.client_id,
                               MMGSDI_SLOT_1,
                               option,
                               gsdidiag_delayed_create_pkcs15_lookup_table_cb,
                               int32touint32(index));
    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_create_pkcs15_lookup_table cmd queue failed %d",
                                mmgsdi_status,0,0);
      gsdidiag_free_index(index);
    }/* end of if (mmgsdi_status != MMGSDI_SUCCESS ) */

   /* how is the call back for the request can be triggered */
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status_ptr);
}/* gsdidiag_delayed_create_pkcs15_lookup_table() */


/*===========================================================================
FUNCTION GSDIDIAG DELAYED CLIENT ID DEREGISTER CALLBACK

DESCRIPTION
    This function is the callback function used for a client id deregister call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the get file attribute information
===========================================================================*/
static void gsdidiag_delayed_client_id_dereg_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type    * rsp_ptr; /* response pointer */
  uint32                             len = 0;     /* length of event */
  int                                index = 0;   /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type *)diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_CLIENT_ID_DEREG_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CLIENT_ID_DEREG_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_client_id_dereg_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.client_id_reg_data.client_id_low =
      (int)((int64)cnf_data->client_id_reg_cnf.response_header.client_id & 0xFFFFFFFF);
    rsp_ptr->rsp_payload.response.client_id_reg_data.client_id_high =
      (int)(((int64)cnf_data->client_id_reg_cnf.response_header.client_id >> 32) & 0xFFFFFFFF);
  }/* if (cnf_data->cnf.read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_client_id_dereg

DESCRIPTION
    Function to deregister client ID of SIM/USIM/RUIM. A task is queued to
    gsdi task to deregister client ID.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by MMGSDI_CLIENT_ID_REG API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_client_id_dereg (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type *)req;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_CLIENT_ID_DEREG_CMD;

    /* send the request to do a mmgsdi_client_id_reg*/
    mmgsdi_status = mmgsdi_client_id_dereg(
                            req_ptr->request.client_id_dereg_req.client_id,
                            (mmgsdi_callback_type)gsdidiag_delayed_client_id_dereg_cb,
                            int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_client_id_dereg queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_client_id_dereg */ /*lint !e550*/


/*===========================================================================
FUNCTION GSDIDIAG DELAYED GET FILE ATTR CALLBACK

DESCRIPTION
    This function is the callback function used for a get file attributes call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the get file attribute information
===========================================================================*/
static void gsdidiag_delayed_get_file_attr_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                             len;     /* length of event */
  int                                index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));



  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_FILE_ATTR_CMD or MMGSDIDIAG_GET_FILE_ATTR_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_get_file_attr_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status =
      mmgsdi_status;
  }
  rsp_ptr->rsp_payload.response.get_file_attr_data.access_method =
      gsdidiag_convert_access_method_from_mmgsdi_enum(
        cnf_data->get_file_attr_cnf.access.access_method);
  rsp_ptr->rsp_payload.response.get_file_attr_data.file_enum =
      gsdidiag_convert_file_from_mmgsdi_enum(
        cnf_data->get_file_attr_cnf.access.file.file_enum);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.get_file_attr_data.file_size =
      cnf_data->get_file_attr_cnf.file_attrib.file_size;
    rsp_ptr->rsp_payload.response.get_file_attr_data.file_id =
      ((int32)cnf_data->get_file_attr_cnf.file_attrib.file_id[0] << 8) |
      (int32)(cnf_data->get_file_attr_cnf.file_attrib.file_id[1]);
    rsp_ptr->rsp_payload.response.get_file_attr_data.file_type =
      (int)cnf_data->get_file_attr_cnf.file_attrib.file_type;

    (void)gsdidiag_populate_according_to_file_type(
      (gsdidiag_rsp_get_file_attr_payload_type *)&rsp_ptr->rsp_payload.response.get_file_attr_data,
      cnf_data->get_file_attr_cnf.file_attrib);
  }/* if (cnf_data->cnf.read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_get_file_attr

DESCRIPTION
    Function to get file attributes from SIM/USIM/RUIM. A task is queued to
    gsdi task to get the file attributes.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_file_attr (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index        = 0;
    mmgsdi_access_type         access = {MMGSDI_EF_ENUM_ACCESS};
    mmgsdi_slot_id_enum_type   mmgsdi_slot = MMGSDI_SLOT_1;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

    access.access_method = gsdidiag_convert_access_method_to_mmgsdi_enum(
      req_ptr->request.get_file_attr_req.access_method.access_method);
    access.file.file_enum = gsdidiag_convert_file_to_mmgsdi_enum(
        req_ptr->request.get_file_attr_req.access_method.file_enum);
    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    /* send the request to do a mmgsdi_get_file_attr*/
    mmgsdi_status = mmgsdi_get_file_attr (req_ptr->request.get_file_attr_req.client_id,
                                        mmgsdi_slot,
                                        access,
                                        (mmgsdi_callback_type)gsdidiag_delayed_get_file_attr_cb,
                                        int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_file_attr queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_get_file_attr */


/*===========================================================================

FUNCTION GSDIDIAG DELAYED SELECT AID CALLBACK

DESCRIPTION
    This function is the callback function used for a select aid call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the app id from the card.
===========================================================================*/
static void gsdidiag_delayed_select_aid_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                             len;     /* length of event */
  int                                index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_READ_TRANSPARENT_CMD or MMGSDIDIAG_READ_TRANSPARENT_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_transparent_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.select_aid_data.data_len =
      cnf_data->select_aid_cnf.app_data.aid.data_len;

    if((rsp_ptr->rsp_payload.response.select_aid_data.data_len > 0) &&
      (rsp_ptr->rsp_payload.response.select_aid_data.data_len <= GSDIDIAG_APP_LABEL_LEN ))
    {
      (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.select_aid_data.data_ptr,
           (uint8*)cnf_data->select_aid_cnf.app_data.aid.data_ptr,
           sizeof(uint8) * int32touint32(rsp_ptr->rsp_payload.response.select_aid_data.data_len));
    }
    else
    {
      rsp_ptr->rsp_payload.response.select_aid_data.data_len = 0;
      (void)memset((uint8*)rsp_ptr->rsp_payload.response.select_aid_data.data_ptr,
                    0x00, GSDIDIAG_APP_LABEL_LEN);
    }/* if(rsp_ptr->rsp_payload.response.select_aid_data.data_le > 0) */
  }/* if (mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_delayed_select_aid

DESCRIPTION
    Function to select AID from an available set of AIDs. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_select_aid (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_generic_cmd_req_type  *req_ptr        = (gsdidiag_generic_cmd_req_type*)req;
    mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
    int                             index          = 0;
    mmgsdi_slot_id_enum_type        mmgsdi_slot    = MMGSDI_SLOT_1;
    mmgsdi_data_type                aid;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->req_payload.sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->req_payload.sub_cmd_hdr.slot_id);
    aid.data_len  = req_ptr->req_payload.sub_cmd_hdr.sim_app_id_len;
    if( (aid.data_len > 0 ) && (aid.data_len <= GSDI_MAX_APP_ID_LEN))
    {
      aid.data_ptr = mmgsdi_malloc(int32touint32(aid.data_len));
      if(aid.data_ptr != NULL )
      {
        memcpy(aid.data_ptr, (void*)req_ptr->req_payload.sub_cmd_hdr.sim_app_id,
        int32touint32(aid.data_len));
      }
      else
      {
        /*  aid.data_len is negative */
        MSG_ERROR(" Aid length is lesser or greater than expected",0,0,0);
        *status = GSDIDIAG_MEMORY_ERR;
        gsdidiag_free_index( index );
        return;
      }
    }
    else
    {
      /*  aid.data_len is negative */
      MSG_ERROR("aid data length could be negative",0,0,0);
      *status = GSDIDIAG_BAD_PARAM;
      gsdidiag_free_index( index );
      return;
    }
    /* send the request to do a read transparent*/
    mmgsdi_status = mmgsdi_select_aid(req_ptr->req_payload.request.select_aid_req.client_id,
                                      mmgsdi_slot,
                                      aid,
                                      (mmgsdi_callback_type)gsdidiag_delayed_select_aid_cb,
                                      int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_select_aid queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);

    if(aid.data_ptr != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( aid.data_ptr);
    }
} /* gsdidiag_delayed_select_aid */


/*===========================================================================

FUNCTION GSDIDIAG DELAYEDP PIN OP CALLBACK

DESCRIPTION
    This function is the callback function used for a pin operation call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the pin information.
===========================================================================*/
static void gsdidiag_delayed_pin_op_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                           len;     /* length of event */
  int                              index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_VERIFY_PIN_CMD, MMGSDIDIAG_ENABLE_PIN_CMD,
    MMGSDIDIAG_DISABLE_PIN_CMD, MMGSDIDIAG_CHANGE_PIN_CMD, MMGSDIDIAG_UNBLOCK_PIN_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_transparent_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  rsp_ptr->rsp_payload.response.pin_op_data.num_retries =
      cnf_data->pin_operation_cnf.pin_info.num_retries;
  rsp_ptr->rsp_payload.response.pin_op_data.num_unblock_retries =
      cnf_data->pin_operation_cnf.pin_info.num_unblock_retries;

  gsdidiag_convert_from_mmgsdi_pin_id(&rsp_ptr->rsp_payload.response.pin_op_data.pin_id,
    cnf_data->pin_operation_cnf.pin_info.pin_id);
  gsdidiag_convert_from_mmgsdi_pin_status(&rsp_ptr->rsp_payload.response.pin_op_data.pin_status,
    cnf_data->pin_operation_cnf.pin_info.status);
  gsdidiag_convert_from_mmgsdi_pin_op(&rsp_ptr->rsp_payload.response.pin_op_data.pin_op,
    cnf_data->pin_operation_cnf.pin_op);

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_pin_op

DESCRIPTION
    Function to pin operation: verify, enable, disable, change, unblock.
    Rsp packet indicates if the req was successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_pin_op (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_generic_cmd_req_type  *req_ptr        = (gsdidiag_generic_cmd_req_type*)req;
    mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
    int                             index          = 0;
    mmgsdi_slot_id_enum_type        mmgsdi_slot    = MMGSDI_SLOT_1;
    mmgsdi_pin_enum_type            pin_id;
    mmgsdi_data_type                pin;
    mmgsdi_data_type                add_pin;
    mmgsdi_pin_replace_enum_type    replace_pin    = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->req_payload.sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->req_payload.sub_cmd_hdr.slot_id);

    gsdidiag_convert_to_mmgsdi_pin_id(req_ptr->req_payload.request.pin_op_req.pin_id, &pin_id);

    pin.data_len = req_ptr->req_payload.request.pin_op_req.pin.data_len;
    pin.data_ptr = req_ptr->req_payload.request.pin_op_req.pin.data_ptr;
    add_pin.data_len = req_ptr->req_payload.request.pin_op_req.additional_pin.data_len;
    add_pin.data_ptr = req_ptr->req_payload.request.pin_op_req.additional_pin.data_ptr;
    switch (req_table[index].subcommand)
    {
    case MMGSDIDIAG_VERIFY_PIN_CMD:
      mmgsdi_status = mmgsdi_verify_pin (
                        req_ptr->req_payload.request.pin_op_req.client_id,
                        mmgsdi_slot,
                        pin_id,
                        pin,
                        (mmgsdi_callback_type)gsdidiag_delayed_pin_op_cb,
                        int32touint32(index));
      break;
    case MMGSDIDIAG_ENABLE_PIN_CMD:
      mmgsdi_status = mmgsdi_enable_pin (
                        req_ptr->req_payload.request.pin_op_req.client_id,
                        mmgsdi_slot,
                        pin_id,
                        pin,
                        (mmgsdi_callback_type)gsdidiag_delayed_pin_op_cb,
                        int32touint32(index));
      break;
    case MMGSDIDIAG_DISABLE_PIN_CMD:
    case MMGSDIDIAG_DISABLE_PIN_EXT_CMD:
      gsdidiag_convert_to_mmgsdi_replace_pin(req_ptr->req_payload.request.pin_op_req.replace_pin,
                                             &replace_pin);
      mmgsdi_status = mmgsdi_disable_pin_ext(
                        req_ptr->req_payload.request.pin_op_req.client_id,
                        mmgsdi_slot,
                        pin_id,
                        replace_pin,
                        pin,
                        (mmgsdi_callback_type)gsdidiag_delayed_pin_op_cb,
                        int32touint32(index));
      break;
    case MMGSDIDIAG_CHANGE_PIN_CMD:
      mmgsdi_status = mmgsdi_change_pin (
                        req_ptr->req_payload.request.pin_op_req.client_id,
                        mmgsdi_slot,
                        pin_id,
                        pin,
                        add_pin,
                        (mmgsdi_callback_type)gsdidiag_delayed_pin_op_cb,
                        int32touint32(index));
      break;
    case MMGSDIDIAG_UNBLOCK_PIN_CMD:
      mmgsdi_status = mmgsdi_unblock_pin (
                        req_ptr->req_payload.request.pin_op_req.client_id,
                        mmgsdi_slot,
                        pin_id,
                        pin,
                        add_pin,
                        (mmgsdi_callback_type)gsdidiag_delayed_pin_op_cb,
                        int32touint32(index));
      break;
    default:
      break;
    }

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_pin_op queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_pin_op */


/*===========================================================================

FUNCTION gsdidiag_delayed_get_all_pin_status_cb

DESCRIPTION
    This function is the callback function used for a get all pin status call.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the pin information.
===========================================================================*/
static void gsdidiag_delayed_get_all_pin_status_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                           len;     /* length of event */
  int                              index;   /* index into req_table*/

  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  if (cnf_data == NULL) {
      return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                          req_table[index].transaction_id,
                                          len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to MMGSDIDIAG_GET_ALL_PIN_STATUS_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_read_transparent_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* pin1 */
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin1_num_retries =
        cnf_data->get_all_pin_status_cnf.pin1.num_retries;
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin1_num_unblock_retries =
       cnf_data->get_all_pin_status_cnf.pin1.num_unblock_retries;
    gsdidiag_convert_from_mmgsdi_pin_status(
      &rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin1_status,
      cnf_data->get_all_pin_status_cnf.pin1.status);

    /* pin2 */
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin2_num_retries =
        cnf_data->get_all_pin_status_cnf.pin2.num_retries;
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin2_num_unblock_retries =
       cnf_data->get_all_pin_status_cnf.pin2.num_unblock_retries;
    gsdidiag_convert_from_mmgsdi_pin_status(
      &rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin2_status,
      cnf_data->get_all_pin_status_cnf.pin2.status);

    /* pin universal*/
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin_univ_num_retries =
        cnf_data->get_all_pin_status_cnf.universal_pin.num_retries;
    rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin_univ_num_unblock_retries =
       cnf_data->get_all_pin_status_cnf.universal_pin.num_unblock_retries;
    gsdidiag_convert_from_mmgsdi_pin_status(
      &rsp_ptr->rsp_payload.response.get_all_pin_status_data.pin_univ_status,
      cnf_data->get_all_pin_status_cnf.universal_pin.status);
  }
  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_all_pin_status

DESCRIPTION
    Function to get all pin status operation.
    Rsp packet indicates if the req was successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_all_pin_status (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_generic_cmd_req_type  *req_ptr        = (gsdidiag_generic_cmd_req_type*)req;
    mmgsdi_return_enum_type         mmgsdi_status  = MMGSDI_ERROR;
    int                             index          = 0;
    mmgsdi_slot_id_enum_type        mmgsdi_slot    = MMGSDI_SLOT_1;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = req_ptr->req_payload.sub_cmd_hdr.subcommand;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->req_payload.sub_cmd_hdr.slot_id);

    mmgsdi_status = mmgsdi_get_all_pin_status (
                        req_ptr->req_payload.request.get_all_pin_status_req.client_id,
                        mmgsdi_slot,
                        (mmgsdi_callback_type)gsdidiag_delayed_get_all_pin_status_cb,
                        int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_all_pin_status queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /* gsdidiag_delayed_get_all_pin_status */


/*===========================================================================
FUNCTION gsdidiag_delayed_service_check
DESCRIPTION
    Function to check if service is available
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

ARGUMENTS
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    Response pointer of type gsdidiag_service_check_cmd_rsp_type
    This will contain the service available.

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_delayed_service_check (
    PACKED void *req,                          /* pointer to request packet  */
    word pkt_len,                             /* length of request packet */
    uint32 * status,                          /* Status */
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport)
{
    gsdi_svr_rsp_type                srv_available;
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32                           len; /* length of event */
    gsdidiag_generic_cmd_req_type  * req_ptr = (gsdidiag_generic_cmd_req_type*)req;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /*Using the Service type from GSDI as this Enum may be replaced by MMGSDI
     ENUM soon. Also, GSDI DIAG may not be used to test this API*/

    srv_available = gsdi_lib_is_service_available(
                       (gsdi_services_type)(req_ptr->req_payload.request.service_check_req.service));

    if (srv_available.gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_service_check failed %d",
                              srv_available.gsdi_status,0,0);
    }
    len = sizeof( gsdidiag_generic_cmd_rsp_type );

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type*)diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             rsp_id,
                                             len );
    if(rsp_ptr == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      *status = GSDIDIAG_MEMORY_ERR;
      return;
    }
    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = aps_as_transport;

     /* subcommand is set to GSDIDIAG_CHECK_SERVICE_AVAILABLE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CHECK_SERVICE_AVAILABLE_CMD;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        srv_available.gsdi_status, status);

    if (aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        srv_available.gsdi_status;
    }
    else
    {
      diagpkt_subsys_set_status(rsp_ptr, *status);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        srv_available.gsdi_status;
    }

    if (srv_available.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.srv_available.svr_bitmap =
        srv_available.svr_bitmap;
    }
    diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_service_check */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION gsdidiag_delayed_secure_device_info_cb

DESCRIPTION
    Callback used for a secure device info command.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_secure_device_info_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    if (sim_data == NULL) {
        return;
    }

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));


    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_ACTIVATE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SECURE_DEVICE_INFO_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_open_channel_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->secure_device_info_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->secure_device_info_cnf.message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->secure_device_info_cnf.message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
/*===========================================================================
FUNCTION gsdidiag_delayed_secure_device_info

DESCRIPTION
    Function to perform secure device info
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_secure_device_info (
    PACKED const void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    #ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

    /* ------------------------------------------------------------------------
       Validate the Input Parameters
       ----------------------------------------------------------------------*/
    if ( req    == NULL ||
         status == NULL )
    {
        return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

    #else

    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED )
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status =  GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_secure_device_info(
                      int32touint32(index),
                      gsdidiag_delayed_secure_device_info_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_secure_device_info queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);

    #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

} /*lint !e715 */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION gsdidiag_delayed_sim_secure_store_cb

DESCRIPTION
    Callback used for a sim secure store command.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_sim_secure_store_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer    */
    uint32                           len;     /* length of event     */
    int                              index;   /* index into req_table*/
    uint32                           temp_status;

    if (sim_data == NULL) {
        return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_SECURE_STORE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_SECURE_STORE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
                         sim_data->message_header.gsdi_status,
                         &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);

    return;
} /* gsdidiag_delayed_sim_secure_store_cb */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */


/*===========================================================================
FUNCTION gsdidiag_delayed_sim_secure_store

DESCRIPTION
    Function to perform sim secure store
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_sim_secure_store (
    PACKED const void                 *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag    */
    uint32                             aps_as_transport /* APS Status set */
)
{

    #ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC

    /* ------------------------------------------------------------------------
       Validate the Input Parameters
       ----------------------------------------------------------------------*/
    if ( req    == NULL ||
         status == NULL )
    {
        return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

    #else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int                        index        = 0;

    /* ------------------------------------------------------------------------
       Validate the Input Parameters
       ----------------------------------------------------------------------*/
    if ( req    == NULL ||
         status == NULL )
    {
        return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status =  GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_sim_secure_store(
                  (gsdi_slot_id_type) req_ptr->sub_cmd_hdr.slot_id,
                  (int32)req_ptr->request.sim_secure_store_req.len,
                  (uint8 *)req_ptr->request.sim_secure_store_req.data_buffer_p,
                  (uint32)index,
                  (gsdi_async_cb)gsdidiag_delayed_sim_secure_store_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_sim_secure_store queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);

    #endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

} /* gsdidiag_delayed_sim_secure_store */ /*lint !e715 */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_ota_deperso_cb

DESCRIPTION
    Callback used for a OTA command.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_ota_deperso_cb(const gsdi_cnf_T* sim_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  int                              i       = 0;
  uint32                           temp_status = 0;


  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (sim_data == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("sim data is null, unable to response back",0,0,0);
      return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = (int)sim_data->message_header.client_ref;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                                           (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                           (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                           req_table[index].transaction_id,
                                           len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

   /* subcommand is set to GSDIDIAG_OTA_DEPERSO_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_OTA_DEPERSO_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
      MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",index,0,0);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;
  }

  if (req_table[index].aps_as_transport)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->message_header.gsdi_status;
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
                                       sim_data->message_header.gsdi_status,
                                       &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->message_header.gsdi_status;
  }
  if (sim_data->message_header.gsdi_status == GSDI_SUCCESS) {
    MSG_HIGH("Number of OTA data bytes returned 0x%x", sim_data->ota_cnf.num_bytes, 0, 0);

    while ( (i < sim_data->ota_cnf.num_bytes ) && (i < GSDI_PERSO_OTA_DEPERSO_RSP_LEN))
    {
      rsp_ptr->rsp_payload.response.ota_data.data[i] =
        sim_data->ota_cnf.data_buffer[i];
      i++;
    }
    rsp_ptr->rsp_payload.response.ota_data.data_len = i;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);


  return;

} /* gsdidiag_delayed_ota_deperso_cb */ /*lint !e715 */
#endif /*#ifdef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION*/

/*===========================================================================
FUNCTION gsdidiag_delayed_ota_deperso

DESCRIPTION
    Function to perform ota depersonalization
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_ota_deperso (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport/* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

  gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
  gsdi_returns_T             gsdi_status  = GSDI_ERROR;
  int32                      index        = 0;
  gsdi_perso_ota_deperso_type ota_deperso;

  if (status == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
              0, 0, 0);
    return;
  }
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* ------------------------------------------------------------------------
      Has the Service Provisioning Code Been Verified.
      ----------------------------------------------------------------------*/
  if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
  {
    MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
    *status = GSDIDIAG_ACCESS_DENIED;
    return;
  }
  else
  {
    MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand = GSDIDIAG_OTA_DEPERSO_CMD;

  ota_deperso.slot = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
  ota_deperso.num_bytes = (int)req_ptr->request.ota_req.len;
  ota_deperso.deperso_user_data_p = req_ptr->request.ota_req.data_buffer_p;

  /* send the request to do a activate */
  gsdi_status = gsdi_perso_ota_deperso(&ota_deperso,
                  int32touint32(index),
                  (gsdi_async_cb)gsdidiag_delayed_ota_deperso_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_ota_deperso queue failed %d",status,0,0);
      gsdidiag_free_index(index);
  }

  /* populate status based off of return value from gsdidiag_delayed_run_gsm_algo */
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      gsdi_status, status);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

} /* gsdidiag_delayed_ota_deperso */ /*lint !e715 */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE_NON_QC
/*===========================================================================
FUNCTION ggsdidiag_delayed_root_key_write_cb

DESCRIPTION
    Callback used for a writing root key command.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_root_key_write_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer     */
    uint32                           len     = 0;    /* length of event      */
    int                              index   = 0;    /* index into req_table */
    int                              i       = 0;    /* temp index           */
    uint32                           temp_status;    /* temp status          */

    /* ------------------------------------------------------------------------
       Validate input parameters
       ----------------------------------------------------------------------*/
    if (sim_data == NULL)
    {
      return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );
    index = sim_data->message_header.client_ref;

    /* ------------------------------------------------------------------------
      Allocate memory for the response pointer.  If allocation fails,
      clean up and return.  Otherwise, initialize it.
      -----------------------------------------------------------------------*/
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                            (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                            req_table[index].transaction_id,
                            len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }
    else
    {
      (void)memset((char*)&rsp_ptr->rsp_payload,
                   0x00,
                   sizeof(gsdidiag_rsp_payload_type));
    }

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

   /* subcommand is set to GSDIDIAG_ROOT_KEY_WRITE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ROOT_KEY_WRITE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR(
          "Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",
          index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.root_key_write.key_len =
        sim_data->root_key_write_cnf.root_key_len;



      while ( i < sim_data->root_key_write_cnf.root_key_len )
      {
        rsp_ptr->rsp_payload.response.root_key_write.key[i] =
          sim_data->root_key_write_cnf.root_key[i];
        i++;
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;


} /* gsdidiag_delayed_root_key_write_cb */ /*lint !e715 */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION gsdidiag_delayed_root_key_write

DESCRIPTION
    Function to perform sim secure store
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_root_key_write (
    PACKED const void                 *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATIO_NON_QC

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdidiag_req_payload_type  *req_ptr     = req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    uint16                     index        = 0;

    if ( status == NULL ||
         req    == NULL )
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",
                0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport =  aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_write_root_key(
                    req_ptr->request.root_key_write_req.len,
                    (uint8 *)req_ptr->request.root_key_write_req.data_buffer_p,
                    index,
                    (gsdi_async_cb)gsdidiag_delayed_root_key_write_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_root_key_write queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
} /* gsdidiag_delayed_root_key_write */ /*lint !e715 */



#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION gsdidiag_delayed_root_key_read_cb

DESCRIPTION
    Callback used for a reading root key command.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_root_key_read_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer     */
    uint32                           len     = 0;    /* length of event      */
    int                              index   = 0;    /* index into req_table */
    int                              i       = 0;    /* temp index           */

    /* ------------------------------------------------------------------------
       Validate input parameters
       ----------------------------------------------------------------------*/
    if (sim_data == NULL)
    {
      return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );
    index = sim_data->message_header.client_ref;

    /* ------------------------------------------------------------------------
      Allocate memory for the response pointer.  If allocation fails,
      clean up and return.  Otherwise, initialize it.
      -----------------------------------------------------------------------*/
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                          (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                          (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                          req_table[index].transaction_id,
                          len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,
                 0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_ROOT_KEY_READ_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ROOT_KEY_READ_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
                "Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",
                index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);

        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.root_key_read.key_len =
        sim_data->root_key_read_cnf.root_key_len;

      while ( i < sim_data->root_key_read_cnf.root_key_len )
      {
        rsp_ptr->rsp_payload.response.root_key_read.key[i] =
          sim_data->root_key_read_cnf.root_key[i];
        i++;
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
} /* gsdidiag_delayed_root_key_read_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION gsdidiag_delayed_root_key_read

DESCRIPTION
    Function to perform sim secure store
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_root_key_read (
    PACKED const void                 *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    uint16                     index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_read_root_key(
                    index,
                    (gsdi_async_cb)gsdidiag_delayed_root_key_read_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_root_key_read queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
} /* gsdidiag_delayed_root_key_read */ /*lint !e715 */



#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION gsdidiag_delayed_sim_secure_read_cb

DESCRIPTION
    Callback used for a reading sim secured data.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_sim_secure_read_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer     */
    uint32                           len     = 0;    /* length of event      */
    int                              index   = 0;    /* index into req_table */
    int                              i       = 0;    /* temp index           */
    uint32                           temp_status;    /* temp status          */

    /* ------------------------------------------------------------------------
       Validate input parameters
       ----------------------------------------------------------------------*/
    if (sim_data == NULL)
    {
      return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type ) +
        (uint32)sim_data->sim_secure_read_cnf.data_len;

    index = (int)sim_data->message_header.client_ref;

    /* ------------------------------------------------------------------------
      Allocate memory for the response pointer.  If allocation fails,
      clean up and return.  Otherwise, initialize it.
      -----------------------------------------------------------------------*/
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                             (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                             req_table[index].transaction_id,
                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,
                 0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_SECURE_READ_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_SECURE_READ_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
          "Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",
          index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS) {
      rsp_ptr->rsp_payload.response.sim_secure_read.data_len =
        sim_data->sim_secure_read_cnf.data_len;

      while ( i < sim_data->sim_secure_read_cnf.data_len )
      {
        /* Add 4 since the SIM Secure Data has a header of
        ** 4 Bytes
        */
        rsp_ptr->rsp_payload.response.sim_secure_read.data[i] =
          sim_data->sim_secure_read_cnf.data[i+4];
        i++;
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_delayed_sim_secure_read_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */

/*===========================================================================
FUNCTION gsdidiag_delayed_sim_secure_read

DESCRIPTION
    Function to perform sim secure store
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_sim_secure_read (
    PACKED const void                 *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* Status Return value    */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
    #ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if ( status == NULL )
    {
        MSG_ERROR("NULL STATUS POINTER",0,0,0);
        return;
    }
    if ( req == NULL )
    {
        MSG_ERROR("NULL REQ POINTER",0,0,0);
        return;
    }

    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

    #else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int                        index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_sim_secure_read(
                    (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                    (uint32)index,
                    (gsdi_async_cb)gsdidiag_delayed_sim_secure_read_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR(
          "gsdidiag_delayed_sim_secure_read queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
} /* gsdidiag_delayed_sim_secure_read */ /*lint !e715 */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_verify_phone_code_cb

DESCRIPTION
    Callback used for a verify phone code

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_verify_phone_code_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len   = 0; /* length of event */
    int    index = 0; /* index into req_table*/
    uint32 temp_status = 0;

    if (sim_data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("sim_data is NULL",0,0,0);
        return;
    }
    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = (int)sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                            (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                            req_table[index].transaction_id,
                            len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_VERIFY_PHONE_CODE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_VERIFY_PHONE_CODE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
            "Index %d is not in use gsdidiag_delayed_sim_secure_store_cb.",
             index,0,0);
        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);

        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_delayed_verify_phone_code_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

/*===========================================================================
FUNCTION gsdidiag_delayed_verify_phone_code

DESCRIPTION
    Function to perform phone code verification
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_verify_phone_code (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_verify_phonecode(
                    req_ptr->request.verify_phone_code_req.phone_code_len,
                    req_ptr->request.verify_phone_code_req.phone_code_ptr,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_delayed_verify_phone_code_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_verify_phone_code queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

} /* gsdidiag_delayed_verify_phone_code */ /*lint !e715 */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_change_phone_code_code_cb

DESCRIPTION
    Callback used for a change phone code

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_change_phone_code_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len   = 0; /* length of event */
    int    index = 0; /* index into req_table*/
    uint32 temp_status = 0;

    if (sim_data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("sim_data is NULL",0,0,0);
        return;
    }
    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = (int)sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                 (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                 (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                 req_table[index].transaction_id,
                 len );

    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,
        0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_CHANGE_PHONE_CODE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CHANGE_PHONE_CODE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
           "Index %d is not in use gsdidiag_delayed_change_phone_code_cb.",
            index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);

        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_delayed_change_phone_code_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

/*===========================================================================
FUNCTION gsdidiag_delayed_change_phone_code

DESCRIPTION
    Function to perform phone code changing
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_change_phone_code (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_change_phonecode(
                    req_ptr->request.change_phone_code_req.old_phone_code_len,
                    req_ptr->request.change_phone_code_req.old_phone_code_ptr,
                    req_ptr->request.change_phone_code_req.new_phone_code_len,
                    req_ptr->request.change_phone_code_req.new_phone_code_ptr,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_delayed_change_phone_code_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_change_phone_code queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_change_phone_code */ /*lint !e715 */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_unblock_phone_code_code_cb

DESCRIPTION
    Callback used for a unblock phone code

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_unblock_phone_code_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len  = 0; /* length of event */
    int index   = 0; /* index into req_table*/
    uint32 temp_status = 0;

    if (sim_data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("sim_data is NULL",0,0,0);
        return;
    }
    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = (int)sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                         (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                         (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                         req_table[index].transaction_id,
                         len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,
                 0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_UNBLOCK_PHONE_CODE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_UNBLOCK_PHONE_CODE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
            "Index %d is not in use gsdidiag_delayed_unblock_phone_code_cb.",
            index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);

        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_delayed_unblock_phone_code_cb */ /*lint !e715 */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
/*===========================================================================
FUNCTION gsdidiag_delayed_unblock_phone_code

DESCRIPTION
    Function to perform phone code unblock
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_unblock_phone_code (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_unblock_phonecode(
                    req_ptr->request.unblock_phone_code_req.master_key_len,
                    req_ptr->request.unblock_phone_code_req.master_key_ptr,
                    0,
                    NULL,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_delayed_unblock_phone_code_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_unblock_phone_code queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

} /* gsdidiag_delayed_unblock_phone_code */ /*lint !e715 */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_set_phone_code_code_cb

DESCRIPTION
    Callback used for a set phone code

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_set_phone_code_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len  = 0; /* length of event */
    int index   = 0; /* index into req_table*/
    uint32                           temp_status = 0x00;

    if (sim_data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("sim_data is NULL",0,0,0);
        return;
    }
    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = (int)sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                        (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                        (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                        req_table[index].transaction_id,
                        len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,
                 0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SET_PHONE_CODE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SET_PHONE_CODE_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR(
            "Index %d is not in use gsdidiag_delayed_set_phone_code_cb.",
            index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
        diagpkt_delay_commit(rsp_ptr);

        return;
    }

    if (req_table[index].aps_as_transport)
    {
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_delayed_set_phone_code_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

/*===========================================================================
FUNCTION gsdidiag_delayed_set_phone_code

DESCRIPTION
    Function to set phone code
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_set_phone_code (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Opn Chnl API Ret */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag     */
    uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

    if (status == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
    }
    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)req;
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int32                      index        = 0;

    /* ------------------------------------------------------------------------
       Validate all input parameters
       ----------------------------------------------------------------------*/
    if (status == NULL  ||
        req    == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("pointer is NULL",0, 0, 0);
      return;
    }

    /* ------------------------------------------------------------------------
       Has the Service Provisioning Code Been Verified.
       ----------------------------------------------------------------------*/
    if ( diag_get_sp_state() != DIAG_SPC_UNLOCKED)
    {
      MMGSDI_DEBUG_MSG_ERROR("SPC CODE:  Not Verified",0,0,0);
      *status = GSDIDIAG_ACCESS_DENIED;
      return;
    }
    else
    {
      MSG_HIGH("SPC CODE:  Verified and valid",0,0,0);
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_perso_set_phonecode(
                    req_ptr->request.set_phone_code_req.phone_code_len,
                    req_ptr->request.set_phone_code_req.phone_code_ptr,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_delayed_set_phone_code_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_set_phone_code queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_set_phone_code */ /*lint !e715 */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_get_dck_num_retries_cb

DESCRIPTION
  Callback used to get_dck_num_retries

DEPENDENCIES
  None

INPUT
  gsdi_cnf_T* sim_data_ptr

RETURN VALUE
  None

OUTPUT
  The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_get_dck_num_retries_cb(const gsdi_cnf_T* sim_data_ptr)
{
  gsdidiag_generic_cmd_rsp_type * rsp_ptr     = NULL; /* response pointer */
  uint32                          len         = 0; /* length of event */
  int                             index       = 0; /* index into req_table*/
  uint32                          temp_status = 0x00;

  /* Check the pointer */
  if (sim_data_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("sim_data_ptr is NULL",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)sim_data_ptr->message_header.client_ref;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
                        (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                        (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                        req_table[index].transaction_id,
                        len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,
                 0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_DCK_NUM_RETRIES_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_DCK_NUM_RETRIES_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR(
          "Index %d is not in use gsdidiag_delayed_get_dck_num_retries_cb.",
            index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data_ptr->message_header.gsdi_status;
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data_ptr->message_header.gsdi_status;
  }

  /* Populate the response info*/
  if(rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status == GSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.dck_num_retries.nw_num_retries =
        sim_data_ptr->dck_num_retries_cnf.nw_num_retries;
    rsp_ptr->rsp_payload.response.dck_num_retries.ns_num_retries =
        sim_data_ptr->dck_num_retries_cnf.ns_num_retries;
    rsp_ptr->rsp_payload.response.dck_num_retries.sp_num_retries =
        sim_data_ptr->dck_num_retries_cnf.sp_num_retries;
    rsp_ptr->rsp_payload.response.dck_num_retries.cp_num_retries =
        sim_data_ptr->dck_num_retries_cnf.cp_num_retries;
    rsp_ptr->rsp_payload.response.dck_num_retries.sim_num_retries =
        sim_data_ptr->dck_num_retries_cnf.sim_num_retries;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

} /* gsdidiag_delayed_get_dck_num_retries_cb */ /*lint !e715 */

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_dck_num_retries

DESCRIPTION
  Function to get dck number of retries
  Rsp packet is sent out. Rsp packet indicates if the req was
  successfully queued to gsdi task.

INPUT:
  PACKED void *orig_req_ptr  : pointer to request packet of type
  word pkt_len               : length of request packet
  uint32 * status_ptr        : Status returned
  diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
  uint32 aps_as_transport

DEPENDENCIES
  None

RETURN VALUE
  None

OUTPUT
  rsp_id: Pointer to response id.
  status: GSDI_SUCCESS
          GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_dck_num_retries (
  PACKED void                        *orig_req_ptr,   /* ptr to req pkt  */
  word                               pkt_len,         /* len of req pkt  */
  uint32                            *status_ptr,      /* GSDI Opn Chnl API Ret */
  diagpkt_subsys_delayed_rsp_id_type rsp_id,          /* Rsp Id gen by diag */
  uint32                             aps_as_transport /* APS Status set  */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                            0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;
  return;

#else

  gsdidiag_req_payload_type  *req_ptr     = (gsdidiag_req_payload_type  *)orig_req_ptr;
  gsdi_returns_T             gsdi_status  = GSDI_ERROR;
  int32                      index        = 0;

  /* ------------------------------------------------------------------------
     Validate all input parameters
     ----------------------------------------------------------------------*/
  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status_ptr is NULL",0, 0, 0);
    return;
  }

  if (orig_req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("orig_req_ptr is NULL",0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* Update member of Request Table*/
  req_table[index].transaction_id   = rsp_id;
  req_table[index].use_flag         = 1;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand       = req_ptr->sub_cmd_hdr.subcommand;

  /* send the request to do a activate */
  gsdi_status = gsdi_perso_get_dck_num_retries(
                    (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_delayed_get_dck_num_retries_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_dck_num_retries queue failed %d",
                           status_ptr,0,0);
    gsdidiag_free_index(index);
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */

} /* gsdidiag_delayed_get_dck_num_retries */ /*lint !e715 */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
/*===========================================================================
FUNCTION gsdidiag_delayed_get_lib_version_cb

DESCRIPTION
    Callback used for get library version

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_get_lib_version_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len = 0; /* length of event */
    int index  = 0; /* index into req_table*/

    if (sim_data == NULL) {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        diagpkt_delay_commit(rsp_ptr);
        return;
    }
    len   = sizeof( gsdidiag_generic_cmd_rsp_type ) +
            sim_data->get_lib_version_cnf.lib_version_len;

    index = sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                                             (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_ACTIVATE_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_LIB_VERSION_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_get_lib_version_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.lib_version.lib_version_len =
        sim_data->get_lib_version_cnf.lib_version_len;

      index = 0;
      while ( index < sim_data->get_lib_version_cnf.lib_version_len )
      {
        rsp_ptr->rsp_payload.response.lib_version.lib_version[index] =
          sim_data->get_lib_version_cnf.lib_version_ptr[index];
        index++;
      }
    }
    else
    {
      rsp_ptr->rsp_payload.response.lib_version.lib_version_len = 0;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
} /* gsdidiag_delayed_get_lib_version_cb */ /*lint !e715 */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_lib_version

DESCRIPTION
    Function to get library version
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_lib_version (
    const PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_NON_QC
    if (status == NULL) {
        return;
    }
    if (req == NULL) {
        return;
    }

    *status = (uint32)GSDI_NOT_SUPPORTED;
    return;

#else

    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    uint16                     index        = 0;

    if (status == NULL) {
        return;
    }
    if (req == NULL) {
        return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;

    /* send the request to do a activate */
    gsdi_status = gsdi_get_lib_version(
                    index,
                    (gsdi_async_cb)gsdidiag_delayed_get_lib_version_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_lib_version queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QC */
} /* gsdidiag_delayed_get_lib_version */ /*lint !e715 */


/*===========================================================================
FUNCTION gsdidiag_activate_onchip_sim_cb

DESCRIPTION
    Callback used for gsdidiag_activate_onchip_sim_cb

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_activate_onchip_sim_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
    uint32 len = 0; /* length of event */
    int index  = 0; /* index into req_table*/

    if ( sim_data == NULL )
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = (int)sim_data->message_header.client_ref;

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_get_lib_version_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->message_header.gsdi_status, &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->message_header.gsdi_status;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;

} /* gsdidiag_activate_onchip_sim_cb */


/*===========================================================================
FUNCTION gsdidiag_activate_onchip_sim

DESCRIPTION
    Function to get library version
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_activate_onchip_sim (
    const PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdi_returns_T             gsdi_status  = GSDI_ERROR;
    int                        index        = 0;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD;

    /* send the request to do a activate */
    gsdi_status = gsdi_activate_onchip_sim(GSDI_SLOT_1,
                    int32touint32(index),
                    (gsdi_async_cb)gsdidiag_activate_onchip_sim_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_activate_onchip_sim_cb queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    *status = (uint32)gsdi_status;

} /* gsdidiag_activate_onchip_sim */


/*===========================================================================
FUNCTION gsdidiag_delayed_isim_auth_cb

DESCRIPTION
    Callback used for gsdidiag_delayed_isim_auth

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_isim_auth_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len     = 0;    /* length of event */
  int                              index   = 0;    /* index into req_table*/
  int32                            cnf_len = 0;

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_ISIM_AUTHENTICATE_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ISIM_AUTHENTICATE_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_isim_auth_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  cnf_len = cnf_data->isim_auth_cnf.res.data_len;
  if (cnf_len > MMGSDIDIAG_MAX_AUTH_DATA)
  {
    MSG_ERROR("Truncate response data to 100, orig len 0x%x",
               cnf_len, 0, 0);
    cnf_len = MMGSDIDIAG_MAX_AUTH_DATA;
  }
  rsp_ptr->rsp_payload.response.isim_auth.res_len = cnf_len;

  if (cnf_len > 0)
  {
    (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.isim_auth.res,
      (uint8*)cnf_data->isim_auth_cnf.res.data_ptr,
      int32touint32(cnf_len));
  }

  cnf_len = cnf_data->isim_auth_cnf.ck.data_len;

  if (cnf_len > MMGSDIDIAG_MAX_AUTH_DATA)
  {
    MSG_ERROR("Truncate response data to 100, orig len 0x%x",
               cnf_len, 0, 0);
    cnf_len = MMGSDIDIAG_MAX_AUTH_DATA;
  }
  rsp_ptr->rsp_payload.response.isim_auth.ck_len = cnf_len;

  if ( cnf_len > 0)
  {
    memcpy((uint8*)rsp_ptr->rsp_payload.response.isim_auth.ck,
      cnf_data->isim_auth_cnf.ck.data_ptr,
      int32touint32(cnf_len));
  }

  cnf_len = cnf_data->isim_auth_cnf.ik.data_len;

  if (cnf_len > MMGSDIDIAG_MAX_AUTH_DATA)
  {
    MSG_ERROR("Truncate response data to 100, orig len 0x%x",
               cnf_len, 0, 0);
    cnf_len = MMGSDIDIAG_MAX_AUTH_DATA;
  }
  rsp_ptr->rsp_payload.response.isim_auth.ik_len = cnf_len;

  if ( cnf_len > 0)
  {
    (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.isim_auth.ik,
      cnf_data->isim_auth_cnf.ik.data_ptr,
      int32touint32(cnf_len));
  }

  cnf_len = cnf_data->isim_auth_cnf.auts.data_len;

  if (cnf_len > MMGSDIDIAG_MAX_AUTH_DATA)
  {
    MSG_ERROR("Truncate response data to 100, orig len 0x%x",
               cnf_len, 0, 0);
    cnf_len = MMGSDIDIAG_MAX_AUTH_DATA;
  }
  rsp_ptr->rsp_payload.response.isim_auth.auts_len = cnf_len;

  if ( cnf_len > 0)
  {
    memcpy((uint8*)rsp_ptr->rsp_payload.response.isim_auth.auts,
      cnf_data->isim_auth_cnf.auts.data_ptr,
      int32touint32(cnf_len));
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_isim_auth_cb */ /*lint !e715*/


/*===========================================================================
FUNCTION gsdidiag_delayed_isim_auth

DESCRIPTION
    Function to perform ISIM authentication
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_isim_authenticate API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_isim_auth (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type   *req_ptr        = (gsdidiag_req_payload_type   *)req;
    mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
    int                          index          = 0;
    mmgsdi_slot_id_enum_type     mmgsdi_slot = MMGSDI_SLOT_1;
    mmgsdi_data_type             auth_data;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_ISIM_AUTHENTICATE_CMD;

    auth_data.data_len = req_ptr->request.isim_auth_req.auth.data_len;
    auth_data.data_ptr = req_ptr->request.isim_auth_req.auth.data_ptr;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);
    /* send the request to do an ISIM authentication */
    mmgsdi_status = mmgsdi_isim_authenticate (
                      req_ptr->request.isim_auth_req.client_id,
                      mmgsdi_slot,
                      auth_data,
                      (mmgsdi_callback_type)gsdidiag_delayed_isim_auth_cb,
                      int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_isim_auth queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);

} /* gsdidiag_delayed_isim_auth */


/*===========================================================================
FUNCTION gsdidiag_delayed_session_open_cb

DESCRIPTION
    Callback used for gsdidiag_delayed_session_open

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_session_open_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len     = 0;    /* length of event */
  int                              index   = 0;    /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SESSION_OPEN_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SESSION_OPEN_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_session_open_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.session_open.session_id_low =
      (int)((int64)cnf_data->session_open_cnf.session_id & 0xFFFFFFFF);
    rsp_ptr->rsp_payload.response.session_open.session_id_high =
      (int)(((int64)cnf_data->session_open_cnf.session_id >> 32) & 0xFFFFFFFF);
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_session_open_cb */ /*lint !e715*/


/*===========================================================================
FUNCTION gsdidiag_delayed_session_open

DESCRIPTION
    Function to perform Open Session
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_isim_authenticate API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_session_open (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type   *req_ptr        = (gsdidiag_req_payload_type   *)req;
    mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
    int                          index          = 0;
    mmgsdi_slot_id_enum_type     mmgsdi_slot    = MMGSDI_SLOT_1;
    mmgsdi_data_type             app_id_data;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_SESSION_OPEN_CMD;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    app_id_data.data_len = req_ptr->sub_cmd_hdr.sim_app_id_len;
    app_id_data.data_ptr = (uint8*)req_ptr->sub_cmd_hdr.sim_app_id;

    /* send the request to do an Open Session */
    mmgsdi_status = mmgsdi_session_management_open (
                      req_ptr->request.session_open_req.client_id,
                      mmgsdi_slot,
                      app_id_data,
                      (mmgsdi_callback_type)gsdidiag_delayed_session_open_cb,
                      int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_session_open queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);

} /* gsdidiag_delayed_session_open */


/*===========================================================================
FUNCTION gsdidiag_delayed_card_status_cb

DESCRIPTION
    Callback used for getting the Card Status from the CARD Status Command

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_card_status_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len = 0; /* length of event */
  int                              index  = 0; /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SESSION_CLOSE_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CARD_STATUS_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_card_status_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

  }
  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_card_status_cb */ /*lint !e715*/


/*===========================================================================
FUNCTION gsdidiag_delayed_session_close

DESCRIPTION
    Function to perform Close Session
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_isim_authenticate API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_card_status (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type   *req_ptr         = (gsdidiag_req_payload_type   *)req;
    mmgsdi_return_enum_type      mmgsdi_status   = MMGSDI_ERROR;
    int                          index           = 0;
    mmgsdi_slot_id_enum_type     mmgsdi_slot     = MMGSDI_SLOT_1;


    if ( status == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_CARD_STATUS_CMD;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    /* send the request to do an Session Close */
    mmgsdi_status = mmgsdi_send_card_status (
                      req_ptr->request.status_req.client_id,
                      mmgsdi_slot,
                      (mmgsdi_status_me_app_enum_type)req_ptr->request.status_req.app_status,
                      (mmgsdi_status_ret_data_enum_type)req_ptr->request.status_req.ret_data_struct,
                      (mmgsdi_callback_type)gsdidiag_delayed_card_status_cb,
                      int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_session_close queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);

} /* gsdidiag_delayed_card_status */


/*===========================================================================
FUNCTION gsdidiag_delayed_session_close_cb

DESCRIPTION
    Callback used for gsdidiag_delayed_session_close

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_session_close_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len = 0; /* length of event */
  int                              index  = 0; /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SESSION_CLOSE_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SESSION_CLOSE_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_session_close_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  rsp_ptr->rsp_payload.response.session_close.session_id_low =
      (int)((int64)cnf_data->session_close_cnf.session_id & 0xFFFFFFFF);
  rsp_ptr->rsp_payload.response.session_close.session_id_high =
      (int)((int64)(cnf_data->session_close_cnf.session_id >> 32) & 0xFFFFFFFF);

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_session_close_cb */ /*lint !e715*/


/*===========================================================================
FUNCTION gsdidiag_delayed_session_close

DESCRIPTION
    Function to perform Close Session
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_isim_authenticate API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_session_close (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type   *req_ptr        = (gsdidiag_req_payload_type   *)req;
    mmgsdi_return_enum_type      mmgsdi_status  = MMGSDI_ERROR;
    int                          index          = 0;
    mmgsdi_slot_id_enum_type     mmgsdi_slot    = MMGSDI_SLOT_1;

    if ( status == NULL )
    {
        return;
    }

    if ( req_ptr == NULL )
    {
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_SESSION_CLOSE_CMD;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    /* send the request to do an Session Close */
    mmgsdi_status = mmgsdi_session_management_close (
                      req_ptr->request.session_close_req.client_id,
                      mmgsdi_slot,
                      (mmgsdi_callback_type)gsdidiag_delayed_session_close_cb,
                      int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_session_close queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);

} /* gsdidiag_delayed_session_close */


/*===========================================================================
FUNCTION gsdidiag_sim_read

DESCRIPTION
    Function to read sim. A task is queued to gsdi task to read sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_sim_read_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_sim_read (
    PACKED void *req,   /* pointer to request packet  */
    word        pkt_len /* length of request packet   */
)
{
    gsdidiag_sim_read_cmd_req_type *req_ptr      = NULL;
    gsdidiag_sim_read_cmd_rsp_type *rsp_ptr      = NULL;
    gsdi_returns_T                 gsdi_status   = GSDI_SUCCESS;
    uim_items_enum_type            sim_file_name = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T             index_mode    = GSDI_MAX_INDEX_MODES;
    int                            index         = 0;
    uint32                         filename_len  = 0;
    uint32                         rsp_len       = 0;
    mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;


    if(req == NULL)
    {
      MSG_ERROR("req ERR:NULL",0,0,0);
      return NULL;
    }

    if(pkt_len < sizeof(gsdidiag_sim_read_cmd_req_type))
    {
      MSG_ERROR("gsdidiag_sim_read req incorrect pkt_len 0x%x",
                 pkt_len,0,0);
      return NULL;
    }

    /* Allocate one additional byte in case EFS filename string is not 
    null-terminated */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

    if ( mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
    {
      MSG_ERROR("gsdidiag_sim_read malloc err req_ptr",0,0,0);
      return NULL;
    }
    else
    {
      memcpy((void *)req_ptr,(void *)req, pkt_len);
    }

    filename_len = strlen((const char*)req_ptr->efs_filename);

    /* check if efs filename is too long*/
    if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("Filename too long(%d). Max length=%d",
                filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0);
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      return NULL;
    }

    /* length of rsp */
    rsp_len = sizeof(gsdidiag_sim_read_cmd_rsp_type) + filename_len;

    /* allocate memory for the rsp*/
    rsp_ptr = (gsdidiag_sim_read_cmd_rsp_type *)diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GSDI,
                                    (uint16)GSDIDIAG_SIM_READ_CMD,
                                    rsp_len );

    if (rsp_ptr==NULL)
    {
      MSG_ERROR("gsdidiag_sim_read Null Ptr rsp_ptr",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      return NULL;
    }

    /* copy req_ptr to rsp_ptr */
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy((void*)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_elementary_file = req_ptr->sim_elementary_file;
    rsp_ptr->sim_index_mode = req_ptr->sim_index_mode;
    rsp_ptr->sim_offset = req_ptr->sim_offset;
    rsp_ptr->sim_rec_num = req_ptr->sim_rec_num;
    rsp_ptr->sim_data_len = req_ptr->sim_data_len;
    rsp_ptr->simple_req  = req_ptr->simple_req;
    /* Copy efs file name to response structure
       No check here because the source string length is what is
       being used to determine the destination length*/
    (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                       (const char*)req_ptr->efs_filename,
                       (uint32toint32(filename_len + GSDIDAG_NULL_CHAR_LEN)));
    /* initialize transaction id */
    rsp_ptr->transaction_id = 0;

    /* find a free space in req_table to put the information in*/
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /* req_table is full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* get a transaction id */
    req_table[index].transaction_id =  int32touint16(gsdidiag_get_transaction_id());
    rsp_ptr->transaction_id         =  req_table[index].transaction_id;

    /* allocate memory for filename  */
    req_table[index].var[0] = gsdidiag_sim_malloc_append_efs_dir((char *)req_ptr->efs_filename);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* elementary file, map from user ids to gsdi internal ids */
    sim_file_name = gsdidiag_convert_file_to_uim_enum(req_ptr->sim_elementary_file);

    /*index mode, map from user id to gsdi internal ids */
    index_mode  = gsdidiag_map_index_mode((gsdidiag_index_enum_type)req_ptr->sim_index_mode);
    if (req_ptr->simple_req == 1) {
        /* for simple request we will store record number */
        req_table[index].var[2] = mmgsdi_malloc(sizeof(uint16));

        /* allocate memory for filename */
        req_table[index].var[3] = mmgsdi_malloc(sizeof(uint32));

        /* allocate memory for represening whether read or write */
        req_table[index].var[4] = mmgsdi_malloc(sizeof(uint8));

        if (req_table[index].var[2]==NULL || req_table[index].var[3]==NULL ||
            req_table[index].var[4]==NULL)
        {
            /* mmgsdi_malloc failed */
            MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
            gsdidiag_free_index_memory(index);
            gsdidiag_free_index(index);
            rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
            MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
            diagpkt_commit(rsp_ptr);
            return rsp_ptr;
        }

        *((uint16*)req_table[index].var[2]) = req_ptr->sim_rec_num;
        *((uint32*)req_table[index].var[3]) = (uint32)sim_file_name;
        *((uint8*) req_table[index].var[4]) = (uint8) GSDIDIAG_SIMPLE_READ;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
        gsdi_status = gsdi2_get_file_attributes(req_ptr->sim_slot_number,
                            sim_file_name,
                            NULL,
                            index,
                            (gsdi_async_cb)gsdidiag_simple_gfa_cb);
#else /* ! FEATURE_MMGSDI_DUAL_SLOT */
        gsdi_status = gsdi_get_file_attributes(sim_file_name,
                            NULL,
                            int32touint32(index),
                            (gsdi_async_cb)gsdidiag_simple_gfa_cb);
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */
    }else{
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        gsdi_status = gsdi2_sim_read(req_ptr->sim_slot_number,
                                   sim_file_name,
                                   index_mode,
                                   req_ptr->sim_rec_num,
                                   req_ptr->sim_data_len,
                                   req_ptr->sim_offset,
                                   NULL,
                                   index,
                                   (gsdi_async_cb)gsdidiag_read_cb
                                   );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
        gsdi_status = gsdi_sim_read( sim_file_name,
                            index_mode,
                            req_ptr->sim_rec_num,
                            NULL,
                            0,
                            NULL,
                            req_ptr->sim_data_len,
                            req_ptr->sim_offset,
                            NULL,
                            int32touint32(index),
                            (gsdi_async_cb) gsdidiag_read_cb
                          );
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR( "gsdiag_sim_read queue failed %d", gsdi_status, 0, 0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return NULL;
}

/*===========================================================================
FUNCTION gsdi_diag_write_cb

DESCRIPTION
  Callback used for an asynchronous gsdi write, sim_data will have the data from
  the read.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void gsdidiag_write_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload;
    uint8 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( event_payload );
    index = uint32toint32(sim_data->message_header.client_ref);

    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_write_cb.",index, 0, 0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_SIM_WRITE, len, (void *)&event_payload);
        return;
    }

    /* check if gsdi_sim_write was successfull */
    if (sim_data->write_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload(EVENT_GSDI_SIM_WRITE, len,(void *) &event_payload );
    }else {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->write_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_report_payload(EVENT_GSDI_SIM_WRITE, len,(void *) &event_payload );
    }

    /* free the memory used to hold write data */
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);

    return;
}

/*===========================================================================
FUNCTION gsdidiag_sim_write

DESCRIPTION
    Function called by diag when a write request is made. EFS file is read
    and data queued to gsdi task to write. Rsp packet indicates if the req
    was successfully queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_sim_write_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_sim_write (
    PACKED void *req,   /* pointer to request packet  */
    word        pkt_len /* length of request packet   */
)
{

    gsdidiag_sim_write_cmd_req_type *req_ptr        = NULL;
    gsdidiag_sim_write_cmd_rsp_type *rsp_ptr        = NULL;
    gsdi_returns_T                  gsdi_status     = GSDI_SUCCESS;
    uim_items_enum_type             sim_file_name   = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T              index_mode      = GSDI_MAX_INDEX_MODES;
    int                             index           = 0;
    uint32                          rsp_len         = 0;
    uint32                          file_size       = 0; /* size of input file */
    int32                           filename_len    = 0;
    struct fs_stat                  file_stat       = {0};
    int                             file_descriptor = 0;
    mmgsdi_return_enum_type         mmgsdi_status   = MMGSDI_ERROR;

    if(req == NULL)
    {
      MSG_ERROR("req ERR:NULL",0,0,0);
      return NULL;
    }

    if(pkt_len < sizeof(gsdidiag_sim_write_cmd_req_type))
    {
      MSG_ERROR("gsdidiag_sim_write req incorect pkt_len 0x%x",
                 pkt_len,0,0);
      return NULL;
    }

    /*Allocate one additional byte in case EFS filename string is not 
    null-terminated*/
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
    {
      MSG_ERROR("gsdidiag_sim_write malloc err req_ptr",0,0,0);
      return NULL;
    }
    else
    {
      memcpy((void *)req_ptr,(void *)req,pkt_len);
    }

    filename_len = uint32toint32(strlen((const char*)req_ptr->efs_filename));

    /* check if efs filename is too long */
    if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
    {
        /* error */
        MMGSDI_DEBUG_MSG_ERROR("Filename too long(%d). Max length=%d",
                  filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return NULL;
    }

    /* rsp length */
    rsp_len = sizeof(gsdidiag_sim_write_cmd_rsp_type) + int32touint32(filename_len);

    /* allocate the rsp */
    rsp_ptr = (gsdidiag_sim_write_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_SIM_WRITE_CMD,
                                   rsp_len );

    if (rsp_ptr==NULL)
    {
      MSG_ERROR("gsdidiag_sim_write Null Ptr *rsp_ptr",0,0,0);
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      return NULL;
    }

    /* copy req_ptr to rsp_ptr */
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy((void *)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_elementary_file = req_ptr->sim_elementary_file;
    rsp_ptr->sim_index_mode = req_ptr->sim_index_mode;
    rsp_ptr->sim_offset = req_ptr->sim_offset;
    rsp_ptr->sim_rec_num = req_ptr->sim_rec_num;
    rsp_ptr->sim_data_len = req_ptr->sim_data_len;
    /* Copy efs file name to response structure
       No check here because the source string length is what is
       being used to determine the destination length*/
    (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                       (const char*)req_ptr->efs_filename,
                       (filename_len + GSDIDAG_NULL_CHAR_LEN));
    /* initialize transaction id */
    rsp_ptr->transaction_id = 0;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
      /* return error */
      rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      diagpkt_commit( rsp_ptr );
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      return rsp_ptr;
    }

    /* get a transaction id */
    req_table[index].transaction_id = int32touint16(gsdidiag_get_transaction_id());
    rsp_ptr->transaction_id         = req_table[index].transaction_id;

    /* allocate and assign new filename with directory if needed */
    req_table[index].var[0] = gsdidiag_sim_malloc_append_efs_dir((char *)req_ptr->efs_filename);

    if (req_table[index].var[0]==NULL) {
      /* mem_malloc failed */
      MMGSDI_DEBUG_MSG_ERROR("mem_malloc failed",0,0,0);
      gsdidiag_free_index(index);
      rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      diagpkt_commit(rsp_ptr);
      return rsp_ptr;
    }

    if(efs_stat((const char*)req_table[index].var[0], &file_stat) != 0)
    {
      MMGSDI_DEBUG_MSG_ERROR("efs_stat failed on file \"%s\". ", req_ptr->efs_filename, 0, 0);
      gsdidiag_free_index(index);
      rsp_ptr->status = (uint32)FS_FAIL_S;
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      diagpkt_commit(rsp_ptr);
      return rsp_ptr;
    }

    file_size =  file_stat.st_size;

    /* allocate memory for data from the file */
    req_table[index].var[1] = mmgsdi_malloc(file_size);

    if (req_table[index].var[1]==NULL) {
      /* mmgsdi_malloc failed */
      MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
      gsdidiag_free_index(index);
      rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      diagpkt_commit(rsp_ptr);
      return rsp_ptr;
    }

    /* read from file and populate data structure */
    file_descriptor = efs_open(req_table[index].var[0], O_RDONLY);

    if(file_descriptor < 0)
    {
      MMGSDI_DEBUG_MSG_ERROR("efs_open Procedure Failed ", 0, 0, 0);
      gsdidiag_free_index_memory(index);
      rsp_ptr->status = (uint32)FS_FAIL_S;
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      diagpkt_commit(rsp_ptr);
      return rsp_ptr;
    }

    /* read file into buffer */
    if(efs_read(file_descriptor, req_table[index].var[1], (fs_size_t)file_size)
                != (fs_ssize_t)file_size)
    {
      (void)efs_close(file_descriptor);
      MMGSDI_DEBUG_MSG_ERROR("efs_read failed. ", 0, 0, 0);
      rsp_ptr->status = (uint32)FS_FAIL_S;
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
      diagpkt_commit(rsp_ptr);

      /* free memory held */
      gsdidiag_free_index_memory(index);
      /* free index */
      gsdidiag_free_index(index);
      return rsp_ptr;
    }

    /* close file */
    if(efs_close(file_descriptor) < 0)
    {
      ERR("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
    }

    /* elementary file, map from user ids to gsdi internal ids */
    sim_file_name = gsdidiag_convert_file_to_uim_enum(req_ptr->sim_elementary_file);

    /* index mode, map from user id to gsdi internal ids */
    index_mode  = gsdidiag_map_index_mode((gsdidiag_index_enum_type)req_ptr->sim_index_mode);

    if (req_ptr->simple_req==1) {

        /* simple request*/
        /* record number to read */
        req_table[index].var[2] = mmgsdi_malloc(sizeof(uint16));

        /* allocate memory for filename */
        req_table[index].var[3] = mmgsdi_malloc(sizeof(uint32));

        /* allocate memory for represening whether read or write*/
        req_table[index].var[4] = mmgsdi_malloc(sizeof(uint8));

        if (req_table[index].var[2]==NULL || req_table[index].var[3]==NULL ||
            req_table[index].var[4]==NULL) {
          /* mmgsdi_malloc failed */
          MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
          MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
          diagpkt_commit(rsp_ptr);
          return rsp_ptr;
        }

        *((uint16*)req_table[index].var[2]) = req_ptr->sim_rec_num;
        *((uint32*)req_table[index].var[3]) = (uint32)sim_file_name;
        *((uint8*) req_table[index].var[4]) = (uint8) GSDIDIAG_SIMPLE_WRITE;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
        gsdi_status = gsdi2_get_file_attributes(req_ptr->sim_slot_number,
                                 sim_file_name,
                                 NULL,
                                 index,
                                 (gsdi_async_cb)gsdidiag_simple_gfa_cb);
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
        gsdi_status = gsdi_get_file_attributes(sim_file_name,
                                          NULL,
                                         int32touint32(index),
                                         (gsdi_async_cb)gsdidiag_simple_gfa_cb);
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */
    }else{
        /* normal request */
        /* queue write req to gsdi task */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
      gsdi_status = gsdi2_sim_write(req_ptr->sim_slot_number,
                                    sim_file_name,
                                    index_mode,
                                    req_ptr->sim_rec_num,
                                    (byte *) req_table[index].var[1],
                                    req_ptr->sim_data_len,
                                    req_ptr->sim_offset,
                                    NULL,
                                    (index),
                                    (gsdi_async_cb) gsdidiag_write_cb );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
       gsdi_status = gsdi_sim_write( sim_file_name,
                                 index_mode,
                                 req_ptr->sim_rec_num,
                                 NULL,
                                 0,
                                 (byte *) req_table[index].var[1],
                                 req_ptr->sim_data_len,
                                 req_ptr->sim_offset,
                                 NULL,
                                 int32touint32(index),
                                 (gsdi_async_cb) gsdidiag_write_cb );
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_write queue failed %d",gsdi_status,0,0);
      /* free memory used for write data */
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
    }

    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, &rsp_ptr->status);

    diagpkt_commit(rsp_ptr);
    return rsp_ptr;

}

/*===========================================================================
FUNCTION gsdidiag_simple_read_cb

DESCRIPTION
  Callback used for an asynchronous gsdi read. This is specific to a simple
  read, and handles read requests which are greater than GSDI_MAX_BLOCK_LEN
  Original read req is split up into multiple gsdi read reqs
  whose return data will be written to a single file.  An event
  EVENT_GSDI_SIM_READ is sent out after the last packet

ARGUMENTS
    gsdi_cnf_T* sim_data: Information, from gsdi task, about request executed

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdidiag_simple_read_cb (const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload; /* used to send event*/
    fs_open_xparms_type open_parms;
    uint8                   len             = 0;
    int                     index           = 0;
    int                     file_descriptor = 0;
    struct fs_stat          file_stat       = {0};
    boolean                 file_exists     = FALSE;

    fs_access_option_type  access_mode; /* open mode for efs file */

    gsdidiag_simple_chunk_packet_position packet_pos;
    /* variable to hold the info whether this packet is start packet,
       end packet or in between */

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    (void) memset(&open_parms, 0x00, sizeof(fs_open_xparms_type));
    len   = sizeof( event_payload);
    index = uint32toint32(sim_data->message_header.client_ref);
    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table index should be in the used state*/
    if (req_table[index].use_flag!=GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
       MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use in gsdidiag_read_cb",
                 index,0,0);
       event_payload.status = GSDIDIAG_MEMORY_ERR;
       event_report_payload( EVENT_GSDI_SIM_READ, len, (void *)&event_payload);
       return;
    }


    /* increment packet within chunk being processed  */
    (*(int*)req_table[index].var[6])++;

  if ((*(int*)req_table[index].var[6]==1)&&
      (*(int*)req_table[index].var[5]==*(int*)req_table[index].var[6])) {
        /* only  packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET;
    }else if (*(int*)req_table[index].var[6]==1) {
        /* first packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_FIRST_PACKET;
    }else if (*(int*)req_table[index].var[5]==*(int*)req_table[index].var[6]){
        /* last packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET;
    }else{
        /* one of the middle packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_PACKET;
    }

    /* test if file already exists */
    if(efs_stat(req_table[index].var[0], &file_stat) == 0)
    {
      file_exists = TRUE;
    }
    else if (efs_errno != ENOENT)
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      MMGSDI_DEBUG_MSG_ERROR ("efs_stat failed.", 0, 0, 0);
      event_payload.status = GSDIDIAG_EFS_NAMETEST_ERR;
      event_report_payload(EVENT_GSDI_SIM_READ,
                           len,
                           (void *)&event_payload );
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
      return;
    }

    /* if first packet then delete existing file */
    if ((packet_pos == GSDIDIAG_SIMPLE_CHUNK_FIRST_PACKET)||
        (packet_pos == GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET)) {

        /* delete file if it already exists */
        if (file_exists == TRUE)
        {
          /* remove file */
          if(efs_unlink(req_table[index].var[0]) < 0)
          {
            MMGSDI_DEBUG_MSG_ERROR ("File remove failed", 0,0,0);
            event_payload.status = GSDIDIAG_EFS_RM_ERR;
            event_report_payload(EVENT_GSDI_SIM_READ,
                                 len,
                                 (void *) &event_payload );
            /* free memory */
            gsdidiag_free_index_memory(index);
            gsdidiag_free_index(index);
            return;
          }
        }
        /* access_mode should be to create, since it is the first packet */
        access_mode = FS_OA_CREATE;
    }else{
        /* not first packet.
           file should exist else some error has occurred */
        if (file_exists == FALSE)
        {
            MMGSDI_DEBUG_MSG_ERROR ("File should have been there", 0,0,0);
            event_payload.status = GSDIDIAG_EFS_ERR;
            event_report_payload( EVENT_GSDI_SIM_READ,
                                  len,
                                  (void *) &event_payload );
            /* free memory */
            gsdidiag_free_index_memory(index);
            gsdidiag_free_index(index);
            return;
        }
        /* file open access_mode should be to append, since it is not the
           first packet */
        access_mode = FS_OA_APPEND;

    }

    /* Make sure it is a Read Resp */
    if ( sim_data->message_header.resp_type != GSDI_SIM_READ_RSP )
    {
         MMGSDI_DEBUG_MSG_ERROR ("Rsp from gsdi is was not GSDI_SIM_READ_RSP(%d):%d",
                    GSDI_SIM_READ_RSP,
                    sim_data->message_header.resp_type,
                    0);
         event_payload.status = GSDIDIAG_INCORRECT_RSP_ERR;
         event_report_payload(EVENT_GSDI_SIM_READ,
                              len,
                              (void *)&event_payload );

        /*free memory used for filename*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
    }

    /* check if the read was successful */
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
        open_parms.create.cleanup_option = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;

        file_descriptor = efs_open(req_table[index].var[0],
                                   access_mode == FS_OA_CREATE ? O_CREAT | O_WRONLY: O_APPEND );

        if(file_descriptor < 0)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_open failed.", 0, 0, 0);

          /* send event */
          event_payload.status = GSDIDIAG_EFS_OPEN_ERR;
          event_report_payload(EVENT_GSDI_SIM_READ,
                               len,
                               (void *)&event_payload );

          /* free memory used for filename */
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
        }

        /* write to file */
        if(efs_write(file_descriptor,
                     (void *)sim_data->read_cnf.data,
                     (fs_size_t)sim_data->read_cnf.returned_data_len) <
                     (fs_ssize_t)sim_data->read_cnf.returned_data_len)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_write failed.", 0, 0, 0);
          event_payload.status = (uint32)GSDIDIAG_EFS_WRITE_ERR;
          event_report_payload( EVENT_GSDI_SIM_READ,
                                len,
                                (void *)&event_payload );
        }else{
            /* success. send only if the last packet */
            if ((packet_pos==GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET)||
                (packet_pos==GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET)) {

                event_payload.status = GSDIDIAG_SUCCESS;
                event_report_payload( EVENT_GSDI_SIM_READ,
                                      len,
                                      (void *)&event_payload );
            }
        }

        /* close file */
        if(efs_close(file_descriptor) < 0)
        {
          ERR("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
        }

    }else{
        /* Error in gsdi_sim_read cmd*/
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->message_header.gsdi_status, &event_payload.status);
        event_report_payload(EVENT_GSDI_SIM_READ,
                             len,
                             (void *)&event_payload );
    }

    /* cleanup if last file write*/
    if (( packet_pos==GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET) ||
        (packet_pos==GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET))  {
         /* free memory used for filename and other vars */
         gsdidiag_free_index_memory(index);
         gsdidiag_free_index(index);
    }


    return;
}
/*===========================================================================
FUNCTION gsdidiag_simple_write_cb

DESCRIPTION
  Callback used for an asynchronous gsdi read. This is specific to a simple
  read, and handles write requests which are greater than GSDI_MAX_BLOCK_LEN
  Original write req is split up into multiple gsdi write reqs. An event
  EVENT_GSDI_SIM_WRITE is sent out after the last packet.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void gsdidiag_simple_write_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload;
    uint8 len; /* length of event */
    int index; /* index into req_table*/

    gsdidiag_simple_chunk_packet_position packet_pos;
    /* variable to hold info whether this packet is start packet,
       end packet or in between */

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( event_payload );
    index = uint32toint32(sim_data->message_header.client_ref);

    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_write_cb.",index, 0, 0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_SIM_WRITE, len, (void *)&event_payload);
        return;
    }
    /* increment packet within chunk being processed  */
    (*(int*)req_table[index].var[6])++;

  if ((*(int*)req_table[index].var[6]==1)&&
      (*(int*)req_table[index].var[5]==*(int*)req_table[index].var[6])) {
        /* only  packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET;
    }else if (*(int*)req_table[index].var[6]==1) {
        /* first packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_FIRST_PACKET;
    }else if (*(int*)req_table[index].var[5]==*(int*)req_table[index].var[6]){
        /* last packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET;
    }else{
        /* one of the middle packet */
        packet_pos = GSDIDIAG_SIMPLE_CHUNK_PACKET;
    }

    /* check if gsdi_sim_write was successfull */
    if (sim_data->write_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        if ((packet_pos == GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET)
            ||(packet_pos==GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET)){

            event_payload.status = GSDIDIAG_SUCCESS;
            event_report_payload(EVENT_GSDI_SIM_WRITE, len,(void *) &event_payload );
        }
    }else {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->write_cnf.message_header.gsdi_status, &event_payload.status);
        event_report_payload(EVENT_GSDI_SIM_WRITE, len,(void *) &event_payload );
    }

    /* free the memory used to hold write data */
    if ((packet_pos == GSDIDIAG_SIMPLE_CHUNK_LAST_PACKET)
    ||(packet_pos==GSDIDIAG_SIMPLE_CHUNK_ONLY_PACKET)) {
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }
    return;
}

/*===========================================================================
FUNCTION gsdidiag_simple_gfa_cb

DESCRIPTION
    Function called by diag when a simple write or read request is made.
    A call to gsdi_get_file_attributes is made to determine the type
    of file, length of file. Depending on the whether read or write,
    a call is made to gsdi_sim_read or gsdi_sim_write.

ARGUMENTS:
    gsdi_cnf_T* sim_data: data from call

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_simple_gfa_cb (const gsdi_cnf_T* sim_data)
{
    /* get file attributes */
    gsdidiag_event_payload_type event_payload; /* used to send event*/
    gsdi_returns_T gsdi_status = GSDI_ERROR;
    uint8 len = 0;
    gsdi_index_modes_T  index_mode=GSDI_NO_INDEX;
    uint16 sim_rec_num=0;
    uint16 sim_data_len=0;
    uint16 sim_data_remaining_len=0;
    uint16 sim_current_offset=0;
    uint8  index =  0;
    uint16 rec_num = *((uint16 *) req_table[index].var[2]);
    uim_items_enum_type sim_file_name =  (uim_items_enum_type)(*((uint32 *)req_table[index].var[3]));
    uint8  read_write    =  *((uint8 *)req_table[index].var[4]);
    int i;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( event_payload);
    index = uint32touint8(sim_data->message_header.client_ref);

    event_payload.transaction_id = req_table[index].transaction_id;

    /* read information */
    switch(sim_data->gfa_cnf.gfa_data.file_structure)
    {
       case GSDI_TRANSPARENT_FILE:
            index_mode   = GSDI_NO_INDEX;
            sim_rec_num  = 0;/*GSDI_NO_OFFSET;*/
            sim_data_len = int32touint8(sim_data->gfa_cnf.gfa_data.file_size);
            break;

       case GSDI_LINEAR_FILE:
            index_mode   = GSDI_INDEX_ABSOLUTE;
            sim_rec_num  = rec_num;
            sim_data_len = sim_data->gfa_cnf.gfa_data.rec_len;
            break;

      case GSDI_CYCLIC_FILE:
            /* Adjust the Index accordingly to Read or write request */
            if ( read_write == GSDIDIAG_SIMPLE_WRITE )
              index_mode   = GSDI_INDEX_PREVIOUS;
            else
              index_mode   = GSDI_INDEX_ABSOLUTE;
            sim_rec_num  = rec_num;
            sim_data_len = sim_data->gfa_cnf.gfa_data.rec_len;
            break;

       default:
            MMGSDI_DEBUG_MSG_ERROR("Error in file structure %d",
                     sim_data->gfa_cnf.gfa_data.file_structure,0,0);
    }


    if ((read_write == GSDIDIAG_SIMPLE_READ)|| (read_write == GSDIDIAG_SIMPLE_WRITE)){
        /* If length of data to read or write is greater than GSDI_MAX_DATA_BLOCK_LEN,
        then it is split up into multiple reads or writes
        req_table[index].var[5] contains the total number of packets.
        req_table[index].var[6] contains the running count of current packet.
        */
        req_table[index].var[5] = mmgsdi_malloc(sizeof(int)); /* initialized number of packets */
        req_table[index].var[6] = mmgsdi_malloc(sizeof(int)); /* ID (1 to NUMPACKETS) */
        if(req_table[index].var[5] == NULL || req_table[index].var[6] == NULL)
        {
            /* mmgsdi_malloc failed */
            MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( req_table[index].var[5]);
            MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( req_table[index].var[6]);
            req_table[index].var[5]=NULL;
            req_table[index].var[6]=NULL;
            return;
        }
        * (int*)req_table[index].var[5] = 1;
        * (int*)req_table[index].var[6] = 0;

        if (sim_data_len > GSDI_MAX_DATA_BLOCK_LEN ) {

            /* number of packets required  */
            /* req_table[index].var[5] = ceil(sim_data_len/GSDI_MAX_DATA_BLOCK_LEN)*/
            /* function 'ceil' is not available, so using division and remainder */
            *(int*)req_table[index].var[5] = (sim_data_len/GSDI_MAX_DATA_BLOCK_LEN);
            if (sim_data_len%GSDI_MAX_DATA_BLOCK_LEN !=0) {
        /* if remainder then increment num of packets */
                (*(int*)req_table[index].var[5])++;
            }
        }
    /* sim_data_remaining_len holds the length of data remaining to be
    read or written */
        sim_data_remaining_len = sim_data_len;
        for (i=1; i<=*(int*)req_table[index].var[5];i++) {

            if (sim_data_remaining_len > GSDI_MAX_DATA_BLOCK_LEN) {
               sim_data_len = GSDI_MAX_DATA_BLOCK_LEN;
               sim_data_remaining_len = sim_data_remaining_len - GSDI_MAX_DATA_BLOCK_LEN;
            }else{
               sim_data_len = sim_data_remaining_len;
               sim_data_remaining_len = 0;
            }
            /* offset is the endpoint of the previous read */
            sim_current_offset = int32touint16((i-1)*GSDI_MAX_DATA_BLOCK_LEN);

            if (read_write == GSDIDIAG_SIMPLE_READ) {
                /* queue read req to gsdi task */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                gsdi_status = gsdi2_sim_read(sim_data->gfa_cnf.message_header.slot,
                                 sim_file_name,
                                 index_mode,
                                 sim_rec_num,
                                 sim_data_len,
                                 sim_current_offset,
                                 NULL,
                                 index,
                                 (gsdi_async_cb)gsdidiag_simple_read_cb
                                 );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
                gsdi_status = gsdi_sim_read( sim_file_name,
                                        index_mode,
                                        sim_rec_num,
                                        NULL,
                                        0,
                                        NULL,
                                        sim_data_len,
                                        sim_current_offset,
                                        NULL, (index),
                                        (gsdi_async_cb) gsdidiag_simple_read_cb
                                      );
                if (gsdi_status != GSDI_SUCCESS)
                {
                   MSG_ERROR("gsdiag_sim_read queue failed %d", gsdi_status,0,0);
                   gsdidiag_convert_gsdi_status_to_gsdidiag_status(
                      gsdi_status, &event_payload.status);
                   event_report_payload( EVENT_GSDI_SIM_READ, len, (void *)&event_payload);
                   return;
                }
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */


            }else if (read_write == GSDIDIAG_SIMPLE_WRITE) {
#ifdef FEATURE_MMGSDI_DUAL_SLOT
                gsdi_status = gsdi2_sim_write(sim_data->gfa_cnf.message_header.slot,
                                    sim_file_name,
                                    index_mode,
                                    sim_rec_num,
                                    (byte *) req_table[index].var[1] + sim_current_offset,
                                    sim_data_len,
                                    sim_current_offset,
                                    NULL,
                                    (index),
                                    (gsdi_async_cb) gsdidiag_simple_write_cb  );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
              /* queue write req to gsdi task */
                gsdi_status = gsdi_sim_write( sim_file_name,
                                         index_mode,
                                         sim_rec_num,
                                         NULL,
                                         0,
                                         (byte *) req_table[index].var[1] + sim_current_offset,
                                         sim_data_len,
                                         sim_current_offset,
                                         NULL,
                                         (index),
                                         (gsdi_async_cb) gsdidiag_simple_write_cb );

                if (gsdi_status != GSDI_SUCCESS)
                {
                   MSG_ERROR("gsdidiag_sim_write queue failed %d", gsdi_status,0,0);
                   gsdidiag_convert_gsdi_status_to_gsdidiag_status(
                      gsdi_status, &event_payload.status);
                   event_report_payload( EVENT_GSDI_SIM_WRITE, len, (void *)&event_payload);
                   return;
                }
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

            }
        }

    }else{
        /* unknown request */
        MMGSDI_DEBUG_MSG_ERROR("read_write flag incorrect value:%d", read_write,0,0);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
                      gsdi_status, &event_payload.status);
        event_report_payload(EVENT_GSDI_GET_FILE_ATTRIBUTES, len, (void *)&event_payload);
    }
    return;
}

/*===========================================================================
FUNCTION gsdidiag_pin_status_cb

DESCRIPTION
    Callback used for an getting pin status

ARGUMENTS
    gsdi_cnf_T* sim_data: Information, from gsdi task, about request executed

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_pin_status_cb( const gsdi_cnf_T* sim_data )
{

    gsdidiag_event_value_payload_type event_payload;
    uint8 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    index = uint32toint32(sim_data->message_header.client_ref);
    len   = sizeof( event_payload );

    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_pen_status_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_GET_PIN_STATUS,
                              len,
                              (void *)&event_payload );
        return;
    }

    /* check if the check status was successful */
    if (sim_data->pin_status_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_payload.value  = (uint8)sim_data->pin_status_cnf.status;
        event_payload.value2  = sim_data->pin_status_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_GET_PIN_STATUS, len,(void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->pin_status_cnf.message_header.gsdi_status, &event_payload.status);
        event_payload.value  = (uint8)sim_data->pin_status_cnf.status;
        event_report_payload(EVENT_GSDI_GET_PIN_STATUS, len,(void *)&event_payload );
    }

    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);
}

/*===========================================================================
FUNCTION gsdidiag_get_pin_status

DESCRIPTION
    Get pin status.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_get_pin_status_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_pin_status (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_get_pin_status_cmd_req_type *req_ptr;
    gsdidiag_get_pin_status_cmd_rsp_type *rsp_ptr;
    gsdi_returns_T gsdi_status;
    int            index;
    gsdi_pin_ids_T pin_id;

    uint16 rsp_len = sizeof( gsdidiag_get_pin_status_cmd_rsp_type);

    if (req==NULL)
    {
        return NULL;
    }

    req_ptr = (gsdidiag_get_pin_status_cmd_req_type*)req;


    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_get_pin_status_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_PIN_STATUS_CMD,
                                   rsp_len );

    if (rsp_ptr==NULL)
    {
        return NULL;
    }

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy( (char*) rsp_ptr->sim_app_id,
            (const char*) req_ptr->sim_app_id,
            sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_pin_id = req_ptr->sim_pin_id;
    /*initialize transaction id*/
    rsp_ptr->transaction_id = 0;

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR){
        /* req_table is full */
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit( rsp_ptr );
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id ;

    /* map the pin id from user ids to internal ids */
    pin_id = gsdidiag_map_pin_id(req_ptr->sim_pin_id);

    /* send the request for pin status */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi2_get_pin_status (req_ptr->sim_slot_number,
                                   pin_id,
                                   NULL,
                                   index,
                                   gsdidiag_pin_status_cb
                                   );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = gsdi_get_pin_status ( pin_id,
                                   NULL,
                                   NULL,
                                   int32touint32(index),
                                   (gsdi_async_cb)gsdidiag_pin_status_cb
                                   );
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, &rsp_ptr->status);

    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}


/*===========================================================================
FUNCTION gsdidiag_verify_pin_cb

DESCRIPTION
    Callback used for an asynchronous verify pin. The number of retries
    remaining will be returned in an Event

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    Event EVENT_GSDI_VERIFY_PIN is sent out
===========================================================================*/
static void gsdidiag_verify_pin_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( event_payload );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* populate the payload */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value = sim_data->verify_pin_cnf.num_of_retries;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_verify_pin_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_VERIFY_PIN, len, (void *)&event_payload );
        return;
    }

    /* check if the verify status was successfull */
    if (sim_data->verify_pin_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_payload.value  = sim_data->verify_pin_cnf.num_of_retries;
        event_report_payload( EVENT_GSDI_VERIFY_PIN, len,(void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->pin_status_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_payload.value  = sim_data->verify_pin_cnf.num_of_retries;
        event_report_payload( EVENT_GSDI_VERIFY_PIN, len,(void *)&event_payload );
    }

    /* free the memory used to hold pin */
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_verify_pin

DESCRIPTION
    Verifies an input pin. A verify pin command is queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_verify_pin_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    Rsp ptr of type gsdidiag_verify_pin_cmd_rsp_type

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_verify_pin (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_verify_pin_cmd_req_type *req_ptr;
    gsdidiag_verify_pin_cmd_rsp_type *rsp_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    uint32                           rsp_len;
    gsdi_pin_ids_T                   pin_id;
    uint32                           pin_len;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_verify_pin_cmd_req_type*)req;

    pin_len = strlen( (char*) req_ptr->sim_pin );

    if (pin_len > GSDI_MAX_PIN_LEN) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                  pin_len, GSDI_MAX_PIN_LENGTH, 0);
        return NULL;
    }

    rsp_len = sizeof(gsdidiag_verify_pin_cmd_rsp_type)
              + pin_len ;

    /* allocate the rsp */
    rsp_ptr = (gsdidiag_verify_pin_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_VERIFY_PIN_CMD,
                                   rsp_len );


    if (rsp_ptr==NULL)
    {
        return NULL;
    }

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len  = req_ptr->sim_app_id_len;
    (void)memcpy((void*)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_pin_id = req_ptr->sim_pin_id;
    (void)memcpy((void*) rsp_ptr->sim_pin,
           (const void*)req_ptr->sim_pin, pin_len + 1); /*lint !e669 !e670 */

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR ){
        /*  req_table is full */
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit( rsp_ptr );
        return rsp_ptr;
    }

    /* get a transaction id */
    req_table[index].transaction_id = int32touint16(gsdidiag_get_transaction_id());
    rsp_ptr->transaction_id = req_table[index].transaction_id;

    /* allocate memory for pin */
    req_table[index].var[0] = mmgsdi_malloc(GSDI_MAX_PIN_LEN);

    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* set the memory to FF, and copy the pin*/
    (void)memset ( (char*)req_table[index].var[0], 0xFF, GSDI_MAX_PIN_LEN );
    (void)memcpy ( (char*)req_table[index].var[0],
             (const char*) req_ptr->sim_pin,
             strlen((char*) req_ptr->sim_pin) );

    /* map the pin id from user ids to internal ids */
    pin_id = gsdidiag_map_pin_id(req_ptr->sim_pin_id);

    /* send the request for verify pin */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi2_verify_pin (req_ptr->sim_slot_number,
                                pin_id,
                               (char*)req_table[index].var[0],
                               NULL,
                               index,
                               gsdidiag_verify_pin_cb
                             );
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = gsdi_verify_pin ( pin_id,
                               (char*)req_table[index].var[0],
                               NULL,
                               int32touint32(index),
                               (gsdi_async_cb)gsdidiag_verify_pin_cb
                             );
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_verify_pin queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}

/*===========================================================================
FUNCTION gsdidiag_change_pin_cb

DESCRIPTION
    Callback used for an asynchronous change pin.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header and the payload
   information. The payload stores the attributes.

===========================================================================*/
static void gsdidiag_change_pin_cb(const gsdi_cnf_T* sim_data)
{

  gsdidiag_generic_cmd_rsp_type *rsp_ptr;
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  if(sim_data == NULL)
  {
    MSG_ERROR("sim_data ERR:NULL",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = uint32toint32(sim_data->message_header.client_ref);

    /* allocate response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type *)diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                           (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                           req_table[index].transaction_id,
                                           len );

  if(rsp_ptr == NULL)
  {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CHANGE_PIN_CMD;
  rsp_ptr->rsp_payload.response.num_retries  =
       sim_data->change_pin_cnf.num_of_retries;

    /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_change_pin_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

    /* populate the status fields according to the value of aps_as transport.
    If asp_as_transport is 1, the data is in the payload, otherwise the
    data is in the header */
  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->change_pin_cnf.message_header.gsdi_status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->change_pin_cnf.message_header.gsdi_status,
        &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->change_pin_cnf.message_header.gsdi_status;
  }

  /* free the memory used to hold pin */
  gsdidiag_free_index_memory(index);
  gsdidiag_free_index(index);

  diagpkt_delay_commit(rsp_ptr);

  return;
}

/*===========================================================================
FUNCTION gsdidiag_change_pin

DESCRIPTION
    Changes the pin. A change pin command is queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_change_pin_cmd_req_type
    word pkt_len    : length of request packets

RETURN VALUE
    None
===========================================================================*/
static void gsdidiag_change_pin (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* gsdi_status */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,     /* delayed response id */
    uint32 aps_as_transport /* determines how status is sent */
)
{
  gsdidiag_req_payload_type        *req_ptr;
  int                              index;
  gsdi_returns_T                   gsdi_status;
  gsdi_pin_ids_T                   pin_id;
  uint32                           old_pin_len;
  uint32                           new_pin_len;

  if (status == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  req_ptr = (gsdidiag_req_payload_type*)req;

  old_pin_len = strlen( (char*) req_ptr->request.change_pin.old_sim_pin );

  if (old_pin_len > GSDI_MAX_PIN_LEN) {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                old_pin_len, GSDI_MAX_PIN_LENGTH, 0);
      return;
  }

  new_pin_len = strlen( (char*) req_ptr->request.change_pin.new_sim_pin );

  if (new_pin_len > GSDI_MAX_PIN_LEN) {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                new_pin_len, GSDI_MAX_PIN_LENGTH, 0);
      return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR ){
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
  }

  /* get a transaction id */
  req_table[index].transaction_id =  rsp_id;

  /* store aps_as_transport into req_table for future use */
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand = GSDIDIAG_CHANGE_PIN_CMD;

  req_table[index].var[0] = mmgsdi_malloc(GSDI_MAX_PIN_LEN );
  if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        return;
  }
  /* set the memory to FF, and copy the old pin*/
  (void)memset ( (char*)req_table[index].var[0], 0x00, GSDI_MAX_PIN_LEN );
  memcpy ( (char*)req_table[index].var[0],
           (const char*) req_ptr->request.change_pin.old_sim_pin,
           old_pin_len);


  req_table[index].var[1] = mmgsdi_malloc(GSDI_MAX_PIN_LEN );
  if (req_table[index].var[1]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
  }
  /* set the memory to FF, and copy the new pin*/
  (void)memset ( (char*)req_table[index].var[1], 0x00, GSDI_MAX_PIN_LEN );
  (void)memcpy ( (char*)req_table[index].var[1],
            (const char*) req_ptr->request.change_pin.new_sim_pin,
            new_pin_len);

  /* map the pin id from user ids to internal ids */
  pin_id = gsdidiag_map_pin_id(req_ptr->request.change_pin.sim_pin_id);

  /* send the request for change pin */
  gsdi_status = gsdi_change_pin ( pin_id,
                                  (char*)req_table[index].var[0],
                                  (char*)req_table[index].var[1],
                                  NULL,
                                  int32touint32(index),
                                  (gsdi_async_cb)gsdidiag_change_pin_cb
                                );

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdiag_change_pin queue failed %d",status,0,0);
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
  }

  /* populate status based off of return value from gsdi_change_pin */
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      gsdi_status, status);
} /* gsdidiag_change_pin */


/*===========================================================================
FUNCTION gsdidiag_delayed_verify_pin_cb

DESCRIPTION
    Callback used for an asynchronous verify pin.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header and the payload
   information. The payload stores the attributes.

===========================================================================*/
static void gsdidiag_delayed_verify_pin_cb(const gsdi_cnf_T* sim_data)
{

  gsdidiag_generic_cmd_rsp_type *rsp_ptr;
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  if (sim_data == NULL)
  {
        MMGSDI_DEBUG_MSG_ERROR("Null Cnf Pointer, unable to free index and commit",0,0,0);
        return;
  }

  index = uint32toint32(sim_data->message_header.client_ref);

  /* allocate response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type *)diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                           (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                           req_table[index].transaction_id,
                                           len );

  if(rsp_ptr == NULL)
  {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed, unable to commit",0,0,0);
        gsdidiag_free_index(index);
        return;
  }

  memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_VERIFY_PIN_CMD;
  rsp_ptr->rsp_payload.response.num_retries  =
       sim_data->verify_pin_cnf.num_of_retries;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_verify_pin_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  /* populate the status fields according to the value of aps_as transport.
    If asp_as_transport is 1, the data is in the payload, otherwise the
    data is in the header */
  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->verify_pin_cnf.message_header.gsdi_status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->verify_pin_cnf.message_header.gsdi_status,
        &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->verify_pin_cnf.message_header.gsdi_status;
  }

  /* free the memory used to hold pin */
  gsdidiag_free_index_memory(index);
  gsdidiag_free_index(index);

  diagpkt_delay_commit(rsp_ptr);

  return;
} /* gsdidiag_delayed_verify_pin_cb */

/*===========================================================================
FUNCTION gsdidiag_delayed_verify_pin

DESCRIPTION
    Verifies the pin. A verify pin command is queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_verify_pin_cmd_req_type
    word pkt_len    : length of request packets

RETURN VALUE
    None
===========================================================================*/
static void gsdidiag_delayed_verify_pin (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* gsdi_status */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,     /* delayed response id */
    uint32 aps_as_transport /* determines how status is sent */
)
{
  gsdidiag_req_payload_type        *req_ptr = (gsdidiag_req_payload_type *)req;
  int                              index;
  gsdi_returns_T                   gsdi_status;
  gsdi_pin_ids_T                   pin_id;
  uint32                           pin_len;

  if (status == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
      return;
  }
  if (req_ptr == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
  }

  pin_len = strlen( (char*) req_ptr->request.verify_pin.sim_pin );

  if (pin_len > GSDI_MAX_PIN_LEN)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                pin_len, GSDI_MAX_PIN_LENGTH, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR )
  {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
  }

  /* get a transaction id */
  req_table[index].transaction_id =  rsp_id;

  /* store aps_as_transport into req_table for future use */
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand = GSDIDIAG_VERIFY_PIN_CMD;

  req_table[index].var[0] = mmgsdi_malloc(GSDI_MAX_PIN_LEN );
  if (req_table[index].var[0]==NULL)
  {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        *status = GSDIDIAG_MEMORY_ERR;
        return;
  }
  /* set the memory to FF, and copy the old pin*/
  memset ( (char*)req_table[index].var[0], 0x00, GSDI_MAX_PIN_LEN );
  memcpy ( (char*)req_table[index].var[0],
            (const char*) req_ptr->request.verify_pin.sim_pin,
            pin_len);

  /* map the pin id from user ids to internal ids */
  pin_id = gsdidiag_map_pin_id(req_ptr->request.verify_pin.sim_pin_id);

  /* send the request for change pin */
  gsdi_status = gsdi_verify_pin ( pin_id,
                                  (char*)req_table[index].var[0],
                                  NULL,
                                  int32touint32(index),
                                  (gsdi_async_cb)gsdidiag_delayed_verify_pin_cb
                                );

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_verify_pin queue failed %d",status,0,0);
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
  }

  /* populate status based off of return value from gsdi_change_pin */
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      gsdi_status, status);
} /* gsdidiag_delayed_verify_pin */

/*===========================================================================
FUNCTION gsdidiag_delayed_sim_increase_cb

DESCRIPTION
  This function handles the sim increase callback.

DEPENDENCIES
  None

INPUT
  gsdi_cnf_T       cnf_ptr:  confirmation data

OUTPUT
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_sim_increase_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  uint32                           temp_status = 0;

  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = uint32toint32(cnf_ptr->message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_REFRESH_FCN_REG_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_INCREASE_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_get_sim_cap_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* Copy the length and the data*/
  rsp_ptr->rsp_payload.response.sim_increase.new_value_len =
    cnf_ptr->increase_cnf.new_value_len;

  if(rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status == GSDI_SUCCESS)
  {
    if ( (rsp_ptr->rsp_payload.response.sim_increase.new_value_len  > 0) &&
         (rsp_ptr->rsp_payload.response.sim_increase.new_value_len < (GSDI_MAX_DATA_BLOCK_LEN -1)))
    {
      memcpy((void*)rsp_ptr->rsp_payload.response.sim_increase.data,
        cnf_ptr->increase_cnf.data,
        cnf_ptr->increase_cnf.new_value_len);
    }
  }
  else
  {
    rsp_ptr->rsp_payload.response.sim_increase.new_value_len = 0;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

}


/*===========================================================================
FUNCTION gsdidiag_delayed_sim_increase

DESCRIPTION
  This function handles the sim increase request.

DEPENDENCIES
  None

INPUT
  req                 Pointer to the request
  pkt_len             Length of the request packet
  status              gsdi_status
  rsp_id              delayed response id
  aps_as_transport    determines how status is sent

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_sim_increase (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* gsdi_status */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,     /* delayed response id */
    uint32 aps_as_transport /* determines how status is sent */
)
{
  gsdidiag_req_payload_type        *req_ptr = (gsdidiag_req_payload_type *)req;
  int                              index;
  gsdi_returns_T                   gsdi_status;
  uim_items_enum_type              sim_file_name;
  dword                            increase_value =0;

  if (status == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
      return;
  }
  if (req_ptr == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL",0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR )
  {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
  }

  /* get a transaction id */
  req_table[index].transaction_id =  rsp_id;

  /* store aps_as_transport into req_table for future use */
  req_table[index].aps_as_transport = aps_as_transport;

  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  sim_file_name = gsdidiag_convert_file_to_uim_enum(
                    req_ptr->request.sim_increase_req.sim_filename);

  increase_value =  req_ptr->request.sim_increase_req.increase_value;

  /* queue the req to gsdi task */
  gsdi_status = gsdi_sim_increase( sim_file_name,
                                   increase_value,
                                   NULL,
                                   NULL,
                                   int32touint32(index),
                                  (gsdi_async_cb) gsdidiag_delayed_sim_increase_cb );

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_sim_increase queue failed %d",
                              gsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status( gsdi_status, status);

} /* gsdidiag_delayed_sim_increase */


/*===========================================================================
FUNCTION gsdidiag_delayed_run_gsm_algo_cb

DESCRIPTION
    Callback used for an asynchronous run GSM algorithm.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header and the payload
   information. The payload stores the attributes.

===========================================================================*/
static void gsdidiag_delayed_run_gsm_algo_cb(const gsdi_cnf_T* sim_data)
{

  gsdidiag_generic_cmd_rsp_type *rsp_ptr;
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  if (sim_data == NULL)
  {
        MMGSDI_DEBUG_MSG_ERROR("Null Cnf Pointer, unable to free index and commit",0,0,0);
        return;
  }

  index = uint32toint32(sim_data->message_header.client_ref);

  /* allocate response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type *)diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                           (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                           req_table[index].transaction_id,
                                           len );

  if(rsp_ptr == NULL)
  {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed, unable to commit",0,0,0);
        gsdidiag_free_index(index);
        return;
  }

  memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_RUN_GSM_ALGO_CMD;
  rsp_ptr->rsp_payload.response.run_gsm_algo.payload.len  =
    sim_data->run_gsm_alg_cnf.returned_data_len;
  if ( (rsp_ptr->rsp_payload.response.run_gsm_algo.payload.len  > 0) &&
       (rsp_ptr->rsp_payload.response.run_gsm_algo.payload.len <= (GSDI_MAX_DATA_BLOCK_LEN *2)))
  {
    memcpy((uint8*)rsp_ptr->rsp_payload.response.run_gsm_algo.payload.buffer,
      (uint8*)sim_data->run_gsm_alg_cnf.data,
      sim_data->run_gsm_alg_cnf.returned_data_len);
  }
  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_change_pin_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
  }

  /* populate the status fields according to the value of aps_as transport.
    If asp_as_transport is 1, the data is in the payload, otherwise the
    data is in the header */
  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->run_gsm_alg_cnf.message_header.gsdi_status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->run_gsm_alg_cnf.message_header.gsdi_status,
        &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      sim_data->run_gsm_alg_cnf.message_header.gsdi_status;
  }

  /* free the memory used to hold pin */
  gsdidiag_free_index_memory(index);
  gsdidiag_free_index(index);

  diagpkt_delay_commit(rsp_ptr);

  return;
}

/*===========================================================================
FUNCTION gsdidiag_delayed_run_gsm_algo

DESCRIPTION
    Changes the pin. A change pin command is queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_change_pin_cmd_req_type
    word pkt_len    : length of request packets

RETURN VALUE
    None
===========================================================================*/
static void gsdidiag_delayed_run_gsm_algo (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* gsdi_status */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,     /* delayed response id */
    uint32 aps_as_transport /* determines how status is sent */
)
{
  gsdidiag_req_payload_type        *req_ptr = (gsdidiag_req_payload_type *)req;
  int                              index;
  gsdi_returns_T                   gsdi_status;
  uint8                            sres[GSDI_SRES_LEN];
  uint8                            kc[GSDI_KC_LEN];

  if (status == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL",
                0, 0, 0);
      return;
  }
  if (req_ptr == NULL)
  {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR )
  {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      return;
  }

  /* get a transaction id */
  req_table[index].transaction_id =  rsp_id;

  /* store aps_as_transport into req_table for future use */
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand = GSDIDIAG_RUN_GSM_ALGO_CMD;

  /* send the request for change pin */
  gsdi_status = gsdi_run_gsm_algorithm(
    req_ptr->request.run_gsm_algo_req.rand_ptr,
    sres,
    kc,
    int32touint32(index),
    (gsdi_async_cb)gsdidiag_delayed_run_gsm_algo_cb );
  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_run_gsm_algo queue failed %d",status,0,0);
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
  }

  /* populate status based off of return value from gsdidiag_delayed_run_gsm_algo */
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      gsdi_status, status);
} /* gsdidiag_delayed_run_gsm_algo */


/*===========================================================================
FUNCTION gsdidiag_delayed_refresh_fcn_reg_cb

DESCRIPTION
  This function handles the Register FCN command callback.

DEPENDENCIES
  None

INPUT
  gsdi_cnf_T       cnf_ptr:  confirmation data

OUTPUT
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_refresh_fcn_reg_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  uint32                           temp_status = 0;

  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = uint32toint32(cnf_ptr->message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_REFRESH_FCN_REG_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_REFRESH_FCN_REG_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_get_sim_cap_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

} /* gsdidiag_delayed_refresh_fcn_reg_cb */

/*===========================================================================
FUNCTION gsdidiag_fcn_cb

DESCRIPTION
    File Registration callback function

INPUT:
    file_list_p: File to be notified
    num_file:    number of files notified

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   None
===========================================================================*/
static void gsdidiag_fcn_cb (
  uim_items_enum_type  * file_list_p,
  uint8                 num_files)
{
  int i = 0;
  MSG_HIGH("Num file 0x%x being requested for refresh", num_files, 0, 0);

  if ((num_files > 0) && (file_list_p == NULL))
    return;

  for (i = 0; i < num_files; i++)
  {
    MSG_HIGH("File 0x%x", file_list_p[i], 0, 0);
  }
} /*lint !e818 */ /* file_list_p can be declared as const */

/*===========================================================================
FUNCTION gsdidiag_delayed_refresh_fcn_reg

DESCRIPTION
    Function to perform FCN Registration process
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_refresh_fcn_reg (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Process BDN Ret  */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag    */
    uint32                             aps_as_transport/* APS Status set  */
)
{
  gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
  gsdi_returns_T             gsdi_status   = GSDI_SUCCESS;
  int32                      index         = 0;
  uim_items_enum_type        *uim_file_ptr = NULL;
  int32                      i             = 0;

  if (status == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  if (req_ptr->request.refresh_fcn_reg_req.num_file > 0)
  {
    uim_file_ptr = mmgsdi_malloc(
                        int32touint32(req_ptr->request.refresh_fcn_reg_req.num_file) *
                        sizeof(uim_items_enum_type));
    /* check for uim_file_ptr*/
    if (uim_file_ptr == NULL)
    {
      /* return error */
      MMGSDI_DEBUG_MSG_ERROR("uim_file_ptr is NULL",0, 0, 0);
      *status = GSDIDIAG_MEMORY_ERR;
      gsdidiag_free_index(index);
      return;
    }

    for (i = 0; i< req_ptr->request.refresh_fcn_reg_req.num_file; i++)
    {
      uim_file_ptr[i] = gsdidiag_convert_file_to_uim_enum(
                        req_ptr->request.refresh_fcn_reg_req.file_list_ptr[i]);
    }
  }
  gsdi_status = gsdi_register_for_file_change_notifications(
                  gsdidiag_fcn_cb,
                  int32touint8(req_ptr->request.refresh_fcn_reg_req.num_file),
                  uim_file_ptr,
                  int32touint32(index),
                  (gsdi_async_cb)gsdidiag_delayed_refresh_fcn_reg_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_refresh_fcn_reg queue failed %d",
                              gsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
    gsdi_status, status);

  MMGSDIUTIL_TMC_MEM_FREE( uim_file_ptr);

} /* gsdidiag_delayed_refresh_fcn_reg */

/*===========================================================================
FUNCTION gsdidiag_delayed_process_bdn_cb

DESCRIPTION
  This function handles the BDN enabling or disabling command callback.

DEPENDENCIES
  None

INPUT
  mmgsdi_return_enum_type       status:  status of command
  mmgsdi_cnf_enum_type          cnf:     confirmation type
  const mmgsdi_cnf_type        *cnf_ptr: confirmation data

OUTPUT
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_process_bdn_cb (
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  uint32                           temp_status = 0;

  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  if ((cnf != MMGSDI_DISABLE_BDN_CNF) && (cnf != MMGSDI_ENABLE_BDN_CNF))
  {
    MMGSDI_DEBUG_MSG_ERROR("bad cnf 0x%x",cnf,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = uint32toint32(cnf_ptr->response_header.client_data);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_DISABLE_BDN_CMD or GSDIDIAG_ENABLE_BDN_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_process_bdn_cb",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

} /* gsdidiag_delayed_process_bdn_cb */

/*===========================================================================
FUNCTION gsdidiag_delayed_process_bdn

DESCRIPTION
    Function to perform BDN enabling or disabling process
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_process_bdn (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Process BDN Ret  */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag    */
    uint32                             aps_as_transport/* APS Status set  */
)
{
  gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  int32                      index         = 0;
  mmgsdi_slot_id_enum_type   mmgsdi_slot   = MMGSDI_SLOT_1;

  if (status == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;
  mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

  if (req_ptr->sub_cmd_hdr.subcommand == GSDIDIAG_DISABLE_BDN_CMD)
  {

    mmgsdi_status = mmgsdi_disable_bdn (req_ptr->request.bdn_req.client_id,
                                        mmgsdi_slot,
                                        (mmgsdi_callback_type)gsdidiag_delayed_process_bdn_cb,
                                        int32touint32(index));
  }
  else
  {
    mmgsdi_status = mmgsdi_enable_bdn (req_ptr->request.bdn_req.client_id,
                                        mmgsdi_slot,
                                        (mmgsdi_callback_type)gsdidiag_delayed_process_bdn_cb,
                                        int32touint32(index));
  }
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_process_bdn queue failed %d",
                              mmgsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
    mmgsdi_status, status);

} /* gsdidiag_delayed_process_bdn */

/*===========================================================================
FUNCTION gsdidiag_delayed_process_acl_cb

DESCRIPTION
  This function handles the ACL enabling or disabling command callback.

DEPENDENCIES
  None

INPUT
  mmgsdi_return_enum_type       status:  status of command
  mmgsdi_cnf_enum_type          cnf:     confirmation type
  const mmgsdi_cnf_type        *cnf_ptr: confirmation data

OUTPUT
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_process_acl_cb (
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  uint32                           temp_status = 0;

  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  if (cnf != MMGSDI_ACL_OPERATION_CNF)
  {
    MMGSDI_DEBUG_MSG_ERROR("bad cnf 0x%x",cnf,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = uint32toint32(cnf_ptr->response_header.client_data);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_DISABLE_ACL_CMD or GSDIDIAG_ENABLE_ACL_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  req_table[index].subcommand;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_process_acl_cb",index,0,0);
    gsdidiag_free_index(index);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

} /* gsdidiag_delayed_process_acl_cb */

/*===========================================================================
FUNCTION gsdidiag_delayed_process_acl

DESCRIPTION
    Function to perform ACL enabling or disabling process
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_process_acl (
    PACKED void                       *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status_ptr,   /* GSDI Process ACL Ret  */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag    */
    uint32                             aps_as_transport/* APS Status set  */
)
{
  gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
  mmgsdi_return_enum_type    mmgsdi_status = MMGSDI_ERROR;
  int32                      index         = 0;
  mmgsdi_slot_id_enum_type   mmgsdi_slot   = MMGSDI_SLOT_1;

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;
  mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

  if (req_ptr->sub_cmd_hdr.subcommand == GSDIDIAG_DISABLE_ACL_CMD)
  {
    mmgsdi_status = mmgsdi_disable_acl (req_ptr->request.bdn_req.client_id,
                                        mmgsdi_slot,
                                        (mmgsdi_callback_type)gsdidiag_delayed_process_acl_cb,
                                        int32touint32(index));
  }
  else
  {
    mmgsdi_status = mmgsdi_enable_acl (req_ptr->request.bdn_req.client_id,
                                        mmgsdi_slot,
                                        (mmgsdi_callback_type)gsdidiag_delayed_process_acl_cb,
                                        int32touint32(index));
  }
  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_process_acl queue failed %d",
                              mmgsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
    mmgsdi_status, status_ptr);

} /* gsdidiag_delayed_process_acl */

/*===========================================================================
FUNCTION gsdidiag_delayed_get_sim_cap_cb

DESCRIPTION
  This function handles the get SIM capabilities command callback.

DEPENDENCIES
  None

INPUT
  gsdi_cnf_T       cnf_ptr:  confirmation data

OUTPUT
  None

RETURN VALUE
  None
===========================================================================*/
static void gsdidiag_delayed_get_sim_cap_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  uint32                           len     = 0;    /* length of event     */
  int                              index   = 0;    /* index into req_table*/
  uint32                           temp_status = 0;

  /* --------------------------------------------------------------------------
     Validate Input Paramaters.
     ------------------------------------------------------------------------*/
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );
  index = uint32toint32(cnf_ptr->message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_SIM_CAPABILITES_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_SIM_CAPABILITIES_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_get_sim_cap_cb.",index,0,0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->get_sim_cap_cnf.message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->get_sim_cap_cnf.message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->get_sim_cap_cnf.message_header.gsdi_status;
  }

  if (cnf_ptr->get_sim_cap_cnf.message_header.gsdi_status == GSDI_SUCCESS)
  {
    /* FDN, BDN, ACL */
    rsp_ptr->rsp_payload.response.sim_cap_data.fdn_enabled =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.fdn_enabled;
    rsp_ptr->rsp_payload.response.sim_cap_data.bdn_enabled =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.bdn_enabled;
    rsp_ptr->rsp_payload.response.sim_cap_data.acl_enabled =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.acl_enabled;

    /* IMSI invalidated or not */
    rsp_ptr->rsp_payload.response.sim_cap_data.imsi_invalidated =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.imsi_invalidated;

    /* SIM Pb */
    rsp_ptr->rsp_payload.response.sim_cap_data.sim_pb_present =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.sim_phone_book.present;
    rsp_ptr->rsp_payload.response.sim_cap_data.sim_pb_mult_entries =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.sim_phone_book.multiple_pbr_entries;
    rsp_ptr->rsp_payload.response.sim_cap_data.sim_pb_status_unknown =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.sim_phone_book.status_unknown;

    /* USIM Global Pb */
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_global_present =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.present;
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_global_mult_entries =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.multiple_pbr_entries;
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_global_status_unknown =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_global_phone_book.status_unknown;

    /* USIM Local Pb */
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_local_present =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.present;
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_local_mult_entries =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.multiple_pbr_entries;
    rsp_ptr->rsp_payload.response.sim_cap_data.usim_pb_local_status_unknown =
      cnf_ptr->get_sim_cap_cnf.sim_capabilities.usim_local_phone_book.status_unknown;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

} /* gsdidiag_delayed_get_sim_cap_cb */

/*===========================================================================
FUNCTION gsdidiag_delayed_get_sim_cap

DESCRIPTION
    Function to perform Get SIM Capabilities process
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_sim_cap (
    PACKED void                        *req,     /* ptr to req pkt        */
    word                               pkt_len,  /* len of req pkt        */
    uint32                            *status,   /* GSDI Process BDN Ret  */
    diagpkt_subsys_delayed_rsp_id_type rsp_id,   /* Rsp Id gen by diag    */
    uint32                             aps_as_transport/* APS Status set  */
)
{
  gsdidiag_req_payload_type  *req_ptr      = (gsdidiag_req_payload_type  *)req;
  gsdi_returns_T             gsdi_status   = GSDI_SUCCESS;
  int32                      index         = 0;
  sim_capabilities_T         dummy_sim_cap;

  if (status == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL", 0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index( index );
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = req_ptr->sub_cmd_hdr.subcommand;

  gsdi_status = gsdi_get_sim_capabilities(&dummy_sim_cap,
                                          int32touint32(index),
                                          (gsdi_async_cb)gsdidiag_delayed_get_sim_cap_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
      MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_sim_cap queue failed %d",
                              gsdi_status,0,0);
      gsdidiag_free_index(index);
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status(
    gsdi_status, status);

} /* gsdidiag_delayed_get_sim_cap */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif/* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if defined(FEATURE_BT) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP) /*xxx_sap*/
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif  /* #if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)*/


#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */

/*===========================================================================
FUNCTION gsdidiag_delayed_card_pup_cb

DESCRIPTION
    Callback used for gsdidiag_delayed_card_pup

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the hdr information and the payload
   information. The payload stores the pup option supplied for the command.
===========================================================================*/
static void gsdidiag_delayed_card_pup_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len     = 0;    /* length of event */
  int                              index   = 0;    /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  *)diagpkt_subsys_alloc_v2_delay(
                                            (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  if (cnf_type != MMGSDI_CARD_PUP_CNF) {
    MMGSDI_DEBUG_MSG_ERROR(
      "gsdidiag_delayed_card_pup_cb failed (type=%d, status=%d)",
      (int)cnf_type, mmgsdi_status, 0);
    gsdidiag_free_index(index);
	diagpkt_delay_commit(rsp_ptr);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_CARD_PUP_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CARD_PUP_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_card_pup_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.card_pup.pup_option =
      (int32)cnf_data->card_pup_cnf.option;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_card_pup_cb */


/*===========================================================================
FUNCTION gsdidiag_delayed_card_pup

DESCRIPTION
    Function to perform card power up
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_card_pdown API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_card_pup (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;
    mmgsdi_slot_id_enum_type   mmgsdi_slot    = MMGSDI_SLOT_1;

    if(status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      *status = GSDIDIAG_BAD_PARAM;
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_CARD_PUP_CMD;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    /* send the request to do a mmgsdi_card_pup*/
    mmgsdi_status = mmgsdi_card_pup(req_ptr->request.card_pup_req.client_id,
                                    mmgsdi_slot,
                                    (mmgsdi_callback_type)
                                      gsdidiag_delayed_card_pup_cb,
                                    (mmgsdi_card_pup_options_enum_type)
                                      req_ptr->request.card_pup_req.pup_option,
                                    int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_card_pup queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /*gsdidiag_delayed_card_pup*/

/*===========================================================================
FUNCTION gsdidiag_delayed_card_pdown_cb

DESCRIPTION
    Callback used for gsdidiag_delayed_card_pdown

DEPENDENCIES
    None

INPUT
    mmgsdi_return_enum_type mmgsdi_status,
    mmgsdi_cnf_enum_type    cnf_type,
    mmgsdi_cnf_type*        cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the
   payload information. The payload stores the power down option supplied
   for the command.
===========================================================================*/
static void gsdidiag_delayed_card_pdown_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32                           len     = 0;    /* length of event */
  int                              index   = 0;    /* index into req_table*/

  if ( cnf_data == NULL )
  {
    return;  /* Not much to do for recovery by allow for timeout */
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = (int)cnf_data->response_header.client_data;

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  if (cnf_type != MMGSDI_CARD_PDOWN_CNF) {
    MMGSDI_DEBUG_MSG_ERROR(
      "gsdidiag_delayed_card_pup_cb failed (type=%d, status=%d)",
      (int)cnf_type, mmgsdi_status, 0);
    gsdidiag_free_index(index);
	diagpkt_delay_commit(rsp_ptr);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_CARD_PDOWN_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_CARD_PDOWN_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_card_pdown_cb.",index,0,0);

    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  else
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status,
      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.card_pdown.pdown_option =
      (int32)cnf_data->card_pdown_cnf.option;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_card_pdown_cb */

/*===========================================================================
FUNCTION gsdidiag_delayed_card_pdown

DESCRIPTION
    Function to perform card power down
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by mmgsdi_card_pdown API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_card_pdown (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by MMGSDI_CLIENT_ID_REG API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr       = (gsdidiag_req_payload_type  *)req;
    mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_ERROR;
    int                        index          = 0;
    mmgsdi_slot_id_enum_type   mmgsdi_slot    = MMGSDI_SLOT_1;

    if ( status == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req_ptr == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].use_flag = 1;
    req_table[index].subcommand = GSDIDIAG_CARD_PDOWN_CMD;

    mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);

    /* send the request to do a mmgsdi_card_pdown*/
    mmgsdi_status = mmgsdi_card_pdown(  req_ptr->request.card_pdown_req.client_id,
                                        mmgsdi_slot,
                                        (mmgsdi_callback_type)gsdidiag_delayed_card_pdown_cb,
                                        (mmgsdi_card_pdown_options_enum_type)
                                          req_ptr->request.card_pdown_req.pdown_option,
                                        int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_card_pdown queue failed %d",
                                mmgsdi_status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      mmgsdi_status, status);
} /*gsdidiag_delayed_card_pdown*/

/*===========================================================================
FUNCTION: gsdidiag_get_generic_cmd_info

DESCRIPTION: Populates the request structure with data from the request packet.

ARGUMENTS:
    gsdidiag_generic_cmd_req_type *req_ptr: generic_command request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type

RETURN VALUE
    None
===========================================================================*/
static void gsdidiag_get_generic_cmd_info (
    gsdidiag_generic_cmd_req_type *req_ptr,
    PACKED void *req
)
{
  uint32 offset = 0;   /* offset for the req pointer */
  if(req_ptr == NULL)
  {
      MSG_ERROR("req_ptr ERR:NULL",0,0,0);
      return;
  }

  /* call utility functions to extract the command header and payload
  fron the request packet and populate the request structure */
  offset = gsdidiag_get_cmd_header(req_ptr, req, offset);
  gsdidiag_get_payload(req_ptr, req, offset);
}

/*===========================================================================
FUNCTION: gsdidiag_get_cmd_header

DESCRIPTION: Populates the cmd_hdr_req_type structure with data from the
             request packet

ARGUMENTS:
    gsdidiag_generic_cmd_req_type *req_ptr: generic_command request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE
    uint8 offset:     offset of the req pointer
===========================================================================*/
static uint32 gsdidiag_get_cmd_header(
  gsdidiag_generic_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{

  if(req_ptr == NULL)
  {
      MSG_ERROR("req_ptr ERR:NULL",0,0,0);
      return offset;
  }

  if(req == NULL)
  {
      MSG_ERROR("req ERR:NULL",0,0,0);
      return offset;
  }
  /* copies the data from req packet to generic_cmd_req_type struct
  and increments the pointer and offset accordingly */

  /* command_code */
  (void)memcpy( ((uint8*)&(req_ptr->cmd_header.command_code)), (uint8*)req, sizeof(uint8) );
  req = (char*)req + sizeof(uint8);
  offset = offset + sizeof(uint8);

  /* subsys_id */
  (void)memcpy( ((uint8*)&(req_ptr->cmd_header.subsys_id)), (uint8*)req, sizeof(uint8) );
  req = (char*)req + sizeof(uint8);
  offset = offset + sizeof(uint8);

  /* subsys_cmd_code */
  (void) memcpy( ((void*)&(req_ptr->cmd_header.subsys_cmd_code)), (void*)req, sizeof(uint16) );
  offset = offset + sizeof(uint16);

  /* returns the offset of the req pointer */
  return offset;
}

/*===========================================================================
FUNCTION: gsdidiag_refresh_payload

DESCRIPTION: Populates the request structures for Refresh
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_sim_refresh_cmd_type *req_ptr: Refresh Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_refresh_payload(
  gsdidiag_sim_refresh_req_cmd_type *req_ptr,
  PACKED void                       *req,
  uint32                             offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Refresh Mode:-  Extract refresh mode and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->refresh_mode)),
                (void*)req,
                sizeof(int32) );
  offset = offset + sizeof(int32);
  req = (char*)req + sizeof(int32);

  /* Notify in progress status or not*/
  (void)memcpy( ((void*)&(req_ptr->notify_in_progress)),
                (void*)req,
                sizeof(uint8) );
  offset = offset + sizeof(uint8);
  req = (char*)req + sizeof(uint8);

  /* Number of File :- Extract # file from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->num_files)),
                (void*)req,
                sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  if (req_ptr->num_files > 0)
  {
    /* Number of bytes of path : */
    (void)memcpy( ((void*)&(req_ptr->data_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    if (req_ptr->data_len > 0)
    {
      req_ptr->file_path_ptr = (uint8 *)mmgsdi_malloc(
                                req_ptr->data_len * sizeof(uint8));
      if (req_ptr->file_path_ptr == NULL)
      {
        MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->num_files, 0, 0);
        return;
      }
      /* FileList :- Extract EF- filename from req packet and store it in the request pointer */
      (void)memcpy( ((void*)(req_ptr->file_path_ptr)),
                      (void*)req,
                      sizeof(uint8)*req_ptr->data_len );

      offset = offset + sizeof(uint8)*req_ptr->data_len;
      req = (char*)req + sizeof(uint8)*req_ptr->data_len;
    }
  }
}

/*===========================================================================
FUNCTION: gsdidiag_naa_refresh_complete_payload

DESCRIPTION: Populates the request structures for Refresh Complete
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_refresh_complete_cmd_type *req_ptr: Refresh Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_naa_refresh_complete_payload(
  gsdidiag_refresh_complete_req_cmd_type *req_ptr,
  PACKED void                            *req,
  uint32                                  offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Refresh Mode:-  Extract refresh mode and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->refresh_mode)),
                (void*)req,
                sizeof(int32) );
  offset = offset + sizeof(int32);
  req = (char*)req + sizeof(int32);

  /* Pass or Fail*/
  (void)memcpy( ((void*)&(req_ptr->pass_fail)),
                (void*)req,
                sizeof(uint8) );
  offset = offset + sizeof(uint8);
  req = (char*)req + sizeof(uint8);

  /* Number of File :- Extract # file from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->num_files)),
                (void*)req,
                sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  if (req_ptr->num_files > 0)
  {
    req_ptr->file_list_ptr = (gsdidiag_elementary_file_enum_type *)mmgsdi_malloc(
                              req_ptr->num_files * sizeof(gsdidiag_elementary_file_enum_type));
    if (req_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->num_files, 0, 0);
      return;
    }
    /* FileList :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)(req_ptr->file_list_ptr)),
                    (void*)req,
                    sizeof(gsdidiag_elementary_file_enum_type)*req_ptr->num_files );

    offset = offset + sizeof(int32)*req_ptr->num_files;
    req = (char*)req + sizeof(int32)*req_ptr->num_files;
  }
}

/*===========================================================================
FUNCTION: gsdidiag_refresh_reg_fcn_payload

DESCRIPTION: Populates the request structures for FCN Registration
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_refresh_fcn_reg_cmd_type *req_ptr: FCN Registration Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_refresh_reg_fcn_payload(
  gsdidiag_refresh_fcn_reg_cmd_type *req_ptr,
  PACKED void                       *req,
  uint32                             offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Number of File :- Extract offset from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->num_file)),
                (void*)req,
                sizeof(int32) );

  offset = offset + sizeof(int32);
  req = (char*)req + sizeof(int32);

  if (req_ptr->num_file > 0)
  {
    req_ptr->file_list_ptr = (gsdidiag_elementary_file_enum_type *)mmgsdi_malloc(
                             int32touint32(req_ptr->num_file) * sizeof(gsdidiag_elementary_file_enum_type));
    if (req_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->num_file, 0, 0);
      return;
    }
    /* FileList :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)(req_ptr->file_list_ptr)),
                    (void*)req,
                    sizeof(int32)*int32touint32(req_ptr->num_file) );

    offset = offset + sizeof(int32)*int32touint32(req_ptr->num_file);
    req = (char*)req + sizeof(int32)*int32touint32(req_ptr->num_file);
  }
}

/*===========================================================================
FUNCTION: gsdidiag_get_sim_cap_payload

DESCRIPTION: Populates the request structures for SIM Capabilities
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_get_sim_cap_cmd_type *req_ptr: SIM Capabilities request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_sim_cap_payload(
  gsdidiag_get_sim_cap_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

}


/*===========================================================================
FUNCTION: gsdidiag_get_bdn_payload

DESCRIPTION: Populates the request structures for BDN
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_bdn_cmd_type *req_ptr: BDN request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_bdn_payload(
  gsdidiag_bdn_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

}

/*===========================================================================
FUNCTION: gsdidiag_get_acl_payload

DESCRIPTION: Populates the request structures for ACL
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_acl_cmd_type *req_ptr: ACL request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_acl_payload(
  gsdidiag_acl_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

}

/*===========================================================================
FUNCTION: gsdidiag_get_ota_deperso_payload

DESCRIPTION: Populates the request structures for OTA Depersonalization
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_ota_deperso_req_cmd_type *req_ptr: OTA Depersonalization request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_ota_deperso_payload(
  gsdidiag_ota_deperso_req_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  (void)memcpy( ((uint32*)&(req_ptr->len)), (uint32*)req, sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Extract Requested Channel_id and store it in the request pointer */
  if (req_ptr->len > 0)
  {
    req_ptr->data_buffer_p = mmgsdi_malloc(req_ptr->len);

    if(req_ptr->data_buffer_p == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
      return;
    }

    (void)memcpy(((byte*)(req_ptr->data_buffer_p)),
            (byte*)req,
            req_ptr->len);

    offset = offset + req_ptr->len;
    req = (char*)req + req_ptr->len;
  }
}

/*===========================================================================
FUNCTION: gsdidiag_get_run_gsm_algo_payload

DESCRIPTION: Populates the request structures for Run GSM Algorithem
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_run_gsm_algo_req_cmd_type *req_ptr: run GSM Algo request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_run_gsm_algo_payload (
  gsdidiag_run_gsm_algo_req_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
    if ((req_ptr == NULL) || (req == NULL))
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
      return;
    }
    req = (char*)req + offset;

    /* Run GSM Algo Rand LEN :- Extract Rand len from req packet and store it in the
       request pointer only if the rand buffer is not NULL */
    memcpy( ((void*)&(req_ptr->rand_len)),
             (void*)req,
             sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    /* Run GSM Algo Rand BUFFER :- Extract Rand buffer from req packet if Rand buffer
       len is greater than zero else assign Rand buffer to NULL) */
    if((req_ptr->rand_len) > 0)
    {
      req_ptr->rand_ptr =
         (uint8 *)mmgsdi_malloc(int32touint32(req_ptr->rand_len));

      if(req_ptr->rand_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      memcpy( ((req_ptr->rand_ptr)),
               (void*)req,
               int32touint32(req_ptr->rand_len));

    }
    else if (( req_ptr->rand_len) == 0)
    {
      req_ptr->rand_ptr = NULL;
    }
    offset = offset + int32touint32(req_ptr->rand_len+1);
    req = (char*)req + req_ptr->rand_len+1;
}

/*===========================================================================
FUNCTION: gsdidiag_get_verify_pin_payload

DESCRIPTION: Populates the request structures for Verify PIN
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_one_pin_cmd_type *req_ptr: Verify PIN request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_verify_pin_payload (
  gsdidiag_one_pin_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
    uint32 pin_length = 0;
    if ((req_ptr == NULL) || (req == NULL))
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
      return;
    }
    /* increment req pointer to obtain position of next data element */
    req = (char*)req + offset;

    /* copy the sim pin id to the payload structure */
    memcpy( ((void*)&(req_ptr->sim_pin_id)),
              (void*)req, sizeof(uint8) );

    /* increment offset and req pointer to go to position of next element */
    offset = offset + sizeof(uint8);
    req = (char*)req + sizeof(uint8);

    /* Obtain pin length to validate proper size of pin; If out of range, the
       pin length is set to 0 */
    pin_length = strlen((char*)(req));

    if(pin_length > GSDI_MAX_PIN_LEN)
    {
       pin_length = 0;
    }
    /* Copy sim pin to request structure */
    if(std_strlcpy( (char *)req_ptr->sim_pin, (char*)req,
                    sizeof(req_ptr->sim_pin)) >=
       (uint32toint32(sizeof(req_ptr->sim_pin))))
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr sim_pin copy error",0,0,0);
    }

    /* Increment offset and req pointer to go to position of next element */
    offset = uint32touint8(offset + pin_length + 1);
    req = (char*)req + pin_length + 1;
}

#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGSDI_ONCHIP_SIM */

/*===========================================================================
FUNCTION: gsdidiag_get_force_sim_read_payload

DESCRIPTION: Populates the request structures for force sim read
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_force_sim_read_cmd_type *req_ptr: force_sim_read request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_force_sim_read_payload (
  gsdidiag_force_sim_read_req_cmd_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
      return;

  }

  req = (char*)req + offset;

  /* Extract EF- filename from req packet and store it in the request pointer */
  memcpy( ((void*)&(req_ptr->sim_filename)),
                    (void*)req,
                    sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Extract index mode from req packet and store it in the request pointer */
  memcpy( ((void*)&(req_ptr->index_mode)),
                    (void*)req,
                    sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Extract record number from req packet and store it in the request pointer */
  memcpy( ((void*)&(req_ptr->rec_num)),
                     (void*)req,
                     sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Extract data_len to be read from req packet and store it in the
  request pointer */
  memcpy( ((void*)&(req_ptr->data_len)),
                    (void*)req,
                    sizeof(int32) );

  offset = offset + sizeof(int32);
  req = (char*)req + sizeof(int32);

  /* Extract offset from where to start the read - from the req packet and
  store it in the request pointer */
  memcpy( ((void*)&(req_ptr->data_offset)),
                    (void*)req,
                     sizeof(int32) );

  offset = offset + sizeof(int32);
  req = (char*)req + sizeof(int32);

}


/*===========================================================================
FUNCTION: gsdidiag_get_sim_increase_payload

DESCRIPTION: Populates the request structures for sim increase
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_sim_increase_cmd_type *req_ptr: sim_increase_request pointer
                        which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                        gsdidiag_generic_cmd_req_type
    uint32 offset:    offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_sim_increase_payload (
  gsdidiag_sim_increase_cmd_req_type *req_ptr,
  PACKED void                        *req,
  uint32                              offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
      return;
  }

  req = (char*)req + offset;

  /* Extract EF- filename from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->sim_filename)),
                  (void*)req,
                  sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Extract increase value from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->increase_value)),
                (void*)req,
                sizeof(uint32));
  offset = offset + sizeof(uint32);
}


/*===========================================================================
FUNCTION: gsdidiag_get_pin_op_payload

DESCRIPTION: Populates the request structures for PIN operation
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_mmgsdi_pin_op_req_cmd_type *req_ptr: pin operation request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_pin_op_payload(
  gsdidiag_commands_enum_type          cmd_type,
  gsdidiag_mmgsdi_pin_op_req_cmd_type *req_ptr,
  PACKED void                         *req,
  uint32                               offset
)
{
  uint32 pin_length = 0;

  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* PIN ID:-  Extract PIN Id from req packet and store it in the request */
  (void)memcpy( ((void*)&(req_ptr->pin_id)),
                (void*)req,
                sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* Obtain pin length to validate proper size of pin; If out of range, the
     pin length is set to 0 */
  pin_length = strlen((char*)(req));

  req_ptr->pin.data_len = uint32toint32(pin_length);

  if (req_ptr->pin.data_len > 0)
  {
    req_ptr->pin.data_ptr = mmgsdi_malloc(pin_length + GSDIDIAG_NULL_CHAR_LEN);
    if (req_ptr->pin.data_ptr == NULL)
    {
      MSG_ERROR("Memory issue, can't alloc pin + null 0x%x", pin_length+1, 0, 0);
      req_ptr->pin.data_len = 0;
    }
    else
    {
      /* Copy pin data to requeset structure, don't verify return value
         because source length is what is used to determine destination
         length*/
      (void)std_strlcpy( (char *)req_ptr->pin.data_ptr,
                         (char*)req,
                         uint32toint32(pin_length + GSDIDAG_NULL_CHAR_LEN));
    }
  }
  /* Increment offset and req pointer to go to position of next element */
  offset = uint32touint8(offset + pin_length + 1);
  req = (uint8*)req + pin_length + 1;

  req_ptr->additional_pin.data_len = 0;
  req_ptr->replace_pin = GSDIDIAG_NOT_REPLACED_BY_UNIV;

  if ((cmd_type == MMGSDIDIAG_CHANGE_PIN_CMD) ||
      (cmd_type == MMGSDIDIAG_UNBLOCK_PIN_CMD))
  {
    /* Obtain pin length to validate proper size of pin; If out of range, the
     pin length is set to 0 */
    pin_length = strlen((char*)(req));

    req_ptr->additional_pin.data_len = uint32toint32(pin_length);

    if (req_ptr->additional_pin.data_len > 0)
    {
      req_ptr->additional_pin.data_ptr = mmgsdi_malloc(pin_length +
                                                       GSDIDIAG_NULL_CHAR_LEN);
    }
    if (req_ptr->additional_pin.data_ptr == NULL)
    {
      MSG_ERROR("Memory issue, can't alloc additional_pin + null 0x%x",
                pin_length + GSDIDIAG_NULL_CHAR_LEN, 0, 0);
      req_ptr->additional_pin.data_len = 0;
    }
    else
    {
      /* Copy additional pin data to request structure don't verify return value
         because source length is what is used to determine destination
         length*/
      (void)std_strlcpy((char *)req_ptr->additional_pin.data_ptr,
                        (char*)req,
                        uint32toint32(pin_length + GSDIDAG_NULL_CHAR_LEN));
    }
    /* Increment offset and req pointer to go to position of next element */
    offset = uint32touint8(offset + pin_length + 1);
    req = (uint8*)req + pin_length + 1;
  }
  else if(cmd_type == MMGSDIDIAG_DISABLE_PIN_EXT_CMD)
  {
    /* Replace Option:-  Extract Replace option from req packet and store it in the request */
    (void)memcpy( ((void*)&(req_ptr->replace_pin)),
                   (void*)req,
                   sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);
  }
}


/*===========================================================================
FUNCTION: gsdidiag_get_get_all_pin_status_payload

DESCRIPTION: Populates the request structures for Get All PIN status
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_mmgsdi_pin_op_req_cmd_type *req_ptr: pin operation request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_get_all_pin_status_payload(
  gsdidiag_mmgsdi_get_all_pin_status_req_cmd_type *req_ptr,
  PACKED void                                     *req,
  uint32                                           offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);
}


/*===========================================================================
FUNCTION: gsdidiag_get_check_service_available_payload

DESCRIPTION: Populates the request structures for Check service Available
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_service_check_req_cmd_type *req_ptr: chk service request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_check_service_available_payload(
  gsdidiag_service_check_req_cmd_type   *req_ptr,
  PACKED void                           *req,
  uint32                                 offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

 /* Skip the Client ID*/
    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);

 /* SERVICE TYPE :-  Extract Srv Type from req packet and store it in the request pointer */
   (void)memcpy( (void*)&(req_ptr->service),
                 (void*)req,
                 sizeof(int32));
}


/*===========================================================================
FUNCTION: gsdidiag_get_mmgsdidiag_path_type_payload

DESCRIPTION: Populates the request's path type structure with data from
             diag

ARGUMENTS:
    mmgsdidiag_path_type *path_ptr: path pointer to which data from the
                                    src_req_ptr packet will be populated.
    PACKED void       *src_req_ptr: pointer to request packet of type
                                    gsdidiag_generic_cmd_req_type
    uint32              offset_ptr: offset pointer of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_mmgsdidiag_path_type_payload(
  mmgsdidiag_path_type *path_ptr,
  PACKED void         **src_req_ptr,
  uint32               *offset_ptr
)
{
  if ((path_ptr == NULL) || (src_req_ptr == NULL) ||
      (offset_ptr == NULL) || (*src_req_ptr == NULL))
  {
    MSG_ERROR("Invalid input ptr 0x%x, 0x%x, 0x%x", path_ptr, src_req_ptr, offset_ptr);
    return;
  }

  /* Extract Path Len from req packet and store it
       in the request pointer */
  (void)memcpy( ((void*)&(path_ptr->path_len)),
                  (void*)*src_req_ptr,
                  sizeof(int32) );

  /* Extract Path value from req packet and store it
      in the request pointer */
  if ( (path_ptr->path_len <= MMGSDIDIAG_PATH_LEN) && (path_ptr->path_len > 0))
  {
    *offset_ptr = *offset_ptr + sizeof(int32);
    *src_req_ptr = (char*)*src_req_ptr + sizeof(int32);

    (void)memcpy( ((void*)(path_ptr->path_buf)),
                  (void*)*src_req_ptr,
                  (sizeof(uint16)*int32touint32(path_ptr->path_len) ));

     *offset_ptr = *offset_ptr +
       (sizeof(uint16)*int32touint32(path_ptr->path_len));
     *src_req_ptr = (char*)*src_req_ptr +
       (sizeof(uint16)*int32touint32(path_ptr->path_len));
  }
  else
  {
    MSG_ERROR("Path Len too big or small for Path Buffer, set path to zero",
      path_ptr->path_len, 0, 0);
    path_ptr->path_len = 0;
  }
} /* gsdidiag_get_mmgsdidiag_path_type_payload */


/*===========================================================================
FUNCTION: gsdidiag_extract_client_id

DESCRIPTION: extracts the clientId from the payload.

ARGUMENTS:
    uint64      *client_id_ptr: Client ID ptr
    PACKED void **req_ptr     : pointer to request packet
    uint32      *offset_ptr   : offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_extract_client_id(
  uint64           *client_id_ptr,
  PACKED void      **req_ptr,
  uint32           *offset_ptr
)
{
  /* Check input parameters */
  if ((client_id_ptr == NULL) || (req_ptr == NULL) ||
      (offset_ptr == NULL))
  {
    MSG_ERROR("Invalid input ptr 0x%x, 0x%x, 0x%x", client_id_ptr, req_ptr, offset_ptr);
    return;
  }

  /* increment req pointer to obtain position of next data element */
  *req_ptr = (char*)(*req_ptr) + *offset_ptr;

  /* CLIENT Id Extract client id from req packet and store it
     in the request pointer */
  (void)memcpy(client_id_ptr,(void*)(*req_ptr),sizeof(uint64));
  *offset_ptr = *offset_ptr + sizeof(uint64);
  *req_ptr = (char*)(*req_ptr) + sizeof(uint64);

}/* end of gsdidiag_extract_client_id()*/


/*===========================================================================
FUNCTION: gsdidiag_extract_uint32

DESCRIPTION: extracts a uint32 argument from the payload.

ARGUMENTS:
    uint32      *uint32_ptr   : pointer to uint32
    PACKED void *req_ptr      : pointer to request packet
    uint32      *offset_ptr   : offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_extract_uint32(
  uint32           *uint32_ptr,
  PACKED void      *req_ptr,
  uint32           *offset_ptr
)
{
  /* Check input parameters */
  if ((uint32_ptr == NULL) || (req_ptr == NULL) ||
      (offset_ptr == NULL))
  {
    MSG_ERROR("Invalid input ptr 0x%x, 0x%x, 0x%x",
              uint32_ptr, req_ptr, offset_ptr);
    return;
  }

  /* Extract uint32 from req packet and store it */
  (void)memcpy((void*)uint32_ptr, (void*)((char*)(req_ptr) + *offset_ptr), sizeof(uint32));

  /* Increment the offset by size of uint32 */
  *offset_ptr = *offset_ptr + sizeof(uint32);

}/* end of gsdidiag_extract_uint32()*/


/*===========================================================================
FUNCTION: gsdidiag_extract_null_term_string

DESCRIPTION: extracts a null terminated string from the payload.

ARGUMENTS:
    char        *string_ptr   : pointer to string
    uint32      buffer_size   : buffer size
    PACKED void *req_ptr      : pointer to request packet
    uint32      *offset_ptr   : offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_extract_null_term_string(
  char             *string_ptr,
  uint32           buffer_size,
  PACKED void      *req_ptr,
  uint32           *offset_ptr
)
{
  char*  src_ptr = NULL;
  uint32 len     = 0;

  /* Check input parameters */
  if ((string_ptr == NULL) || (req_ptr == NULL) ||
      (offset_ptr == NULL))
  {
    MSG_ERROR("Invalid input ptr 0x%x, 0x%x, 0x%x",
              string_ptr, req_ptr, offset_ptr);
    return;
  }

  src_ptr = (char*)(req_ptr) + *offset_ptr;
  len     = strlen(src_ptr);

  /* Extract string */
  if (len < buffer_size)
  {
    (void)std_strlcpy(string_ptr, src_ptr, buffer_size);
  }
  else
  {
    /* this is an error case! */
    MSG_ERROR("String is too long %d", len, 0, 0);
    string_ptr[0] = 0x0;
  }

  /* Increment offset, including NULL termination */
  *offset_ptr = *offset_ptr + len + 1;

}/* end of gsdidiag_extract_null_term_string()*/


/*===========================================================================
FUNCTION: gsdidiag_refresh_req_payload

DESCRIPTION: Populates the request structures refresh registeration
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_register_for_refresh_req_cmd_type *req_ptr:
                      NAA Refresh Registration Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_refresh_req_payload(
  gsdidiag_register_for_refresh_req_cmd_type *req_ptr,
  PACKED void                                *req,
  uint32                                     offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in
     the request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Vote for INIT:-  Extract vote for init flag from req packet
                      and store it in the request pointer */
  (void)memcpy(((void*)&(req_ptr->vote_for_init)),
                (void*)req,
                sizeof(char) );
  offset = offset + sizeof(char);
  req = (char*)req + sizeof(char);

  /* Number of File :- Extract number of files from req packet and store it
                       in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->num_file_enums)),
                (void*)req,
                sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  if (req_ptr->num_file_enums > 0)
  {
    req_ptr->file_path_ptr = NULL;
    req_ptr->file_list_ptr =
      (gsdidiag_elementary_file_enum_type *)mmgsdi_malloc(
         req_ptr->num_file_enums * sizeof(gsdidiag_elementary_file_enum_type));
    if (req_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->num_file_enums, 0, 0);
      return;
    }
    /* FileList :- Extract EF- filename from req packet and store it in the
                   request pointer */
    (void)memcpy( ((void*)(req_ptr->file_list_ptr)),
                    (void*)req,
                    sizeof(int32)*(req_ptr->num_file_enums) );

    offset = offset + sizeof(int32)*(req_ptr->num_file_enums);
    req = (char*)req + sizeof(int32)*(req_ptr->num_file_enums);
  }
}/* gsdidiag_refresh_req_payload */


/*===========================================================================
FUNCTION: GSDIDIAG_REFRESH_REG_BY_PATH_PAYLOAD

DESCRIPTION: Extracts register by path request payload from diag buffer.

ARGUMENTS:
    gsdidiag_register_for_refresh_req_cmd_type *req_ptr:
                      NAA Refresh Registration Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_refresh_reg_by_path_payload(
  gsdidiag_register_for_refresh_req_cmd_type *req_ptr,
  PACKED void                                *req,
  uint32                                     offset
)
{

  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the
                  request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Vote for INIT:-  Extract vote for init flag from req packet
                      and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->vote_for_init)),
                (void*)req,
                sizeof(char) );
  offset = offset + sizeof(char);
  req = (char*)req + sizeof(char);


  /* The file enums are used in this command to test for failure scenarios for
     registration */
  /* Num of file enums */
  (void)memcpy( (void *)&req_ptr->num_file_enums,(void*)req,sizeof(uint32));
  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* set file list ptr to null */
  req_ptr->file_list_ptr = NULL;

  /* Num of Enum files - Copy the number of enum files sent with the command */
  if(req_ptr->num_file_enums > 0)
  {
    req_ptr->file_list_ptr =
      (gsdidiag_elementary_file_enum_type *)mmgsdi_malloc(
         req_ptr->num_file_enums * sizeof(gsdidiag_elementary_file_enum_type));
    if (req_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file enums",
        req_ptr->num_file_enums, 0, 0);
      return;
    }
    /* FileList :- Extract EF- filename from req packet and store it in the
                   request pointer */
    (void)memcpy( ((void*)(req_ptr->file_list_ptr)),
                    (void*)req,
                    sizeof(int32)* req_ptr->num_file_enums );

    offset = offset + sizeof(int32)* req_ptr->num_file_enums;
    req = (char*)req + sizeof(int32)* req_ptr->num_file_enums;
  }

  /* Number of File :- Extract path length from req packet and store
                       it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->path_len)),
                (void*)req,
                sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  if (req_ptr->path_len > 0)
  {
    req_ptr->file_path_ptr = mmgsdi_malloc(
                             req_ptr->path_len * sizeof(uint8));
    if (req_ptr->file_path_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->path_len, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr->file_list_ptr);
      return;
    }

    memset((void *)req_ptr->file_path_ptr,
           0x00,
           req_ptr->path_len * sizeof(uint8));

    memcpy((void*)req_ptr->file_path_ptr,
           (void*)req,
           req_ptr->path_len * sizeof(uint8));

    offset = offset + req_ptr->path_len * sizeof(uint8);
    req = (char*)req + req_ptr->path_len * sizeof(uint8);
  } /* req_ptr->path_len > 0 */

}/* gsdidiag_refresh_reg_by_path_payload */


/*===========================================================================
FUNCTION: GSDIDIAG_REFRESH_DEREG_PAYLOAD

DESCRIPTION: Extracts dergister by path request payload from diag buffer.
             The payload has file list sent in form of file enums

ARGUMENTS:
    gsdidiag_deregister_for_refresh_req_cmd_type *req_ptr:
                      Refresh Registration Req pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_refresh_dereg_payload(
  gsdidiag_deregister_for_refresh_req_cmd_type *req_ptr,
  PACKED void                                *req,
  uint32                                     offset
)
{

  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the
                  request pointer */
  (void)memcpy( ((void*)&(req_ptr->client_id)),
                (void*)req,
                sizeof(uint64) );
  offset = offset + sizeof(uint64);
  req = (char*)req + sizeof(uint64);

  /* Vote for INIT:-  Extract vote for init flag from req packet
                      and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->vote_for_init)),
                (void*)req,
                sizeof(char) );
  offset = offset + sizeof(char);
  req = (char*)req + sizeof(char);

  /* The file enums are used in this command to test for failure scenarios for
     registration */

   /* Num of file enums */
  (void)memcpy( (void *)&req_ptr->num_file_enums,(void*)req,sizeof(uint32));
  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  /* set file list ptr to null */
  req_ptr->file_list_ptr = NULL;

  /* Num of Enum files - Copy the number of enum files sent with the command */
  if(req_ptr->num_file_enums  > 0)
  {
    req_ptr->file_list_ptr =
      (gsdidiag_elementary_file_enum_type *)mmgsdi_malloc(
         req_ptr->num_file_enums  * sizeof(gsdidiag_elementary_file_enum_type));
    if (req_ptr->file_list_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file enums",
        req_ptr->num_file_enums , 0, 0);
      return;
    }
    /* FileList :- Extract EF- filename from req packet and store it in the
                   request pointer */
    (void)memcpy( ((void*)(req_ptr->file_list_ptr)),
                    (void*)req,
                    sizeof(int32)* req_ptr->num_file_enums );

    offset = offset + sizeof(int32)* req_ptr->num_file_enums;
    req = (char*)req + sizeof(int32)* req_ptr->num_file_enums;
  }

  /* Number of File :- Extract path length from req packet and store
                       it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->path_len)),
                (void*)req,
                sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req = (char*)req + sizeof(uint32);

  if (req_ptr->path_len > 0)
  {
    req_ptr->file_list_ptr = NULL;
    req_ptr->file_path_ptr = mmgsdi_malloc(
                             req_ptr->path_len * sizeof(uint8));
    if (req_ptr->file_path_ptr == NULL)
    {
      MSG_ERROR("Unable to mem malloc for 0x%x file", req_ptr->path_len, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(req_ptr->file_list_ptr);
      return;
    }

    memset((void *)req_ptr->file_path_ptr,
           0x00,
           req_ptr->path_len * sizeof(uint8));


    memcpy((void*)req_ptr->file_path_ptr,
           (void*)req,
           req_ptr->path_len * sizeof(uint8));

    offset = offset + req_ptr->path_len * sizeof(uint8);
    req = (char*)req + req_ptr->path_len * sizeof(uint8);
  } /* req_ptr->path_len > 0 */
}/* gsdidiag_refresh_dereg_payload() */


/*===========================================================================
FUNCTION: gsdidiag_get_payload

DESCRIPTION: Populates the request structures (sub_cmd_hdr and req_payload)
    with the data from the request packet.

ARGUMENTS:
    gsdidiag_generic_cmd_req_type *req_ptr: generic_command request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_payload(
  gsdidiag_generic_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  uint32  pin_length = 0;

  if(req == NULL)
  {
    MSG_ERROR("req ERR:NULL",0,0,0);
    return;
  }
  if (req_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }

  /* populates the sub_cmd_hdr structure with data from the req packet */
  offset = gsdidiag_get_sub_cmd_header(req_ptr, req, offset);

  /* Extracts variable information based on the subcommand value */
  switch(req_ptr->req_payload.sub_cmd_hdr.subcommand)
  {
  case GSDIDIAG_SIM_REFRESH_CMD:
    gsdidiag_refresh_payload(&req_ptr->req_payload.request.refresh_req,
                             req, offset);
    break;

  case GSDIDIAG_REFRESH_COMPLETE_CMD:
    gsdidiag_naa_refresh_complete_payload(
      &req_ptr->req_payload.request.refresh_complete_req,
      req, offset);
    break;

  case GSDIDIAG_REFRESH_FCN_REG_CMD:
    gsdidiag_refresh_reg_fcn_payload(&req_ptr->req_payload.request.refresh_fcn_reg_req,
                                     req, offset);
    break;

  case GSDIDIAG_GET_SIM_CAPABILITIES_CMD:
    gsdidiag_get_sim_cap_payload(&req_ptr->req_payload.request.sim_cap_req,
                                 req, offset);
    break;

  case GSDIDIAG_DISABLE_BDN_CMD:
  case GSDIDIAG_ENABLE_BDN_CMD:
    gsdidiag_get_bdn_payload(&req_ptr->req_payload.request.bdn_req,
                             req, offset);
    break;

  case GSDIDIAG_DISABLE_ACL_CMD:
  case GSDIDIAG_ENABLE_ACL_CMD:
    gsdidiag_get_acl_payload(&req_ptr->req_payload.request.acl_req,
                             req, offset);
    break;

  case GSDIDIAG_OTA_DEPERSO_CMD:
    gsdidiag_get_ota_deperso_payload(&req_ptr->req_payload.request.ota_req,
                                     req, offset);
    break;

  case GSDIDIAG_RUN_GSM_ALGO_CMD:
    gsdidiag_get_run_gsm_algo_payload(&req_ptr->req_payload.request.run_gsm_algo_req,
                                       req, offset);
    break;

  case MMGSDIDIAG_WRITE_TRANSPARENT_CMD:
  case GSDIDIAG_WRITE_TRANSPARENT_CMD:
  case MMGSDIDIAG_WRITE_TRANSPARENT_EXT_CMD:

    req = (char*)req + offset;

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand != GSDIDIAG_WRITE_TRANSPARENT_CMD)
    {
      /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
      (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.client_id)),
                    (void*)req,
                    sizeof(uint64) );

      offset = offset + sizeof(uint64);
      req = (char*)req + sizeof(uint64);
    }
    else
    {
      req_ptr->req_payload.request.write_transparent_req.client_id = gsdidiag_mmgsdi_client_id;

      offset = offset + sizeof(int32);
      req = (char*)req + sizeof(int32);
    }

    /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.access_method.access_method)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.access_method.file_enum)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand == MMGSDIDIAG_WRITE_TRANSPARENT_EXT_CMD)
    {
      /* populate the path info */
      gsdidiag_get_mmgsdidiag_path_type_payload(
        &req_ptr->req_payload.request.write_transparent_req.access_method.path,
        &req,
        &offset);
    }

    /* OFFSET :- Extract offset from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.offset)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* WRITE BUFFER LEN :- Extract write buffer len from req packet and store it in the
       request pointer only if the write buffer is not NULL */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.write_data.data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    /* WRITE BUFFER :- Extract write buffer from req packet if write buffer
       len is greater than zero else assign write buffer to NULL) */
    if((req_ptr->req_payload.request.write_transparent_req.write_data.data_len) > 0)
    {
      req_ptr->req_payload.request.write_transparent_req.write_data.data_ptr =
         (uint8 *)mmgsdi_malloc(
                             int32touint32(req_ptr->req_payload.request.write_transparent_req.write_data.data_len));

      if(req_ptr->req_payload.request.write_transparent_req.write_data.data_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.write_transparent_req.write_data.data_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.write_transparent_req.write_data.data_len));

    }
    else if (( req_ptr->req_payload.request.write_transparent_req.write_data.data_len) == 0)
    {
      req_ptr->req_payload.request.write_transparent_req.write_data.data_ptr = NULL;
    }
    offset = offset + int32touint32(req_ptr->req_payload.request.write_transparent_req.write_data.data_len+1);
    req = (char*)req + req_ptr->req_payload.request.write_transparent_req.write_data.data_len+1;

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_transparent_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    break;

    case MMGSDIDIAG_WRITE_RECORD_CMD:
    case GSDIDIAG_WRITE_RECORD_CMD:
    case MMGSDIDIAG_WRITE_RECORD_EXT_CMD:

    req = (char*)req + offset;

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand != GSDIDIAG_WRITE_RECORD_CMD)
    {
      /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
      (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.client_id)),
                    (void*)req,
                    sizeof(uint64) );

      offset = offset + sizeof(uint64);
      req = (char*)req + sizeof(uint64);
    }
    else
    {
      req_ptr->req_payload.request.write_record_req.client_id = gsdidiag_mmgsdi_client_id;

      offset = offset + sizeof(int32);
      req = (char*)req + sizeof(int32);
    }

    /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.access_method.access_method)),
                  (void*)req,
                  sizeof(int32) );
    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    req_ptr->req_payload.request.write_record_req.file_type = MMGSDIDIAG_LINEAR_FIXED_FILE;

    /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.access_method.file_enum)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand == MMGSDIDIAG_WRITE_RECORD_EXT_CMD)
    {
      /* populate the path info */
      gsdidiag_get_mmgsdidiag_path_type_payload(
        &req_ptr->req_payload.request.write_record_req.access_method.path,
        &req,
        &offset);
      /* file type population */
      (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.file_type)),
                    (void*)req,
                    sizeof(int32) );

      offset = offset + sizeof(int32);
      req = (char*)req + sizeof(int32);
    }

    /* RECORD NUMBER  :- Extract record number from req packet and store it in the
       request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.record_number)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* WRITE BUFFER LEN :- Extract write buffer len from req packet and store it in the
       request pointer only if the write buffer is not NULL */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.write_data.data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);


    /* WRITE BUFFER :- Extract write buffer from req packet if write buffer
       len is greater than zero else assign write buffer to NULL) */
    if((req_ptr->req_payload.request.write_record_req.write_data.data_len) > 0)
    {
      req_ptr->req_payload.request.write_record_req.write_data.data_ptr =
         (uint8 *)mmgsdi_malloc(
         int32touint32(req_ptr->req_payload.request.write_record_req.write_data.data_len));

      if(req_ptr->req_payload.request.write_record_req.write_data.data_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.write_record_req.write_data.data_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.write_record_req.write_data.data_len));

    }
    else if (( req_ptr->req_payload.request.write_record_req.write_data.data_len) == 0)
    {
      req_ptr->req_payload.request.write_record_req.write_data.data_ptr = NULL;
    }
    offset = offset + int32touint32(req_ptr->req_payload.request.write_record_req.write_data.data_len+1);
    req = (char*)req + req_ptr->req_payload.request.write_record_req.write_data.data_len+1;

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.write_record_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    break;

    case MMGSDIDIAG_READ_TRANSPARENT_CMD:
    case GSDIDIAG_READ_TRANSPARENT_CMD:
    case MMGSDIDIAG_READ_TRANSPARENT_EXT_CMD:

    req = (char*)req + offset;

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand != GSDIDIAG_READ_TRANSPARENT_CMD)
    {
      /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
      (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.client_id)),
                    (void*)req,
                    sizeof(uint64) );

      offset = offset + sizeof(uint64);
      req = (char*)req + sizeof(uint64);
    }
    else
    {
      req_ptr->req_payload.request.read_transparent_req.client_id = gsdidiag_mmgsdi_client_id;

      offset = offset + sizeof(int32);
      req = (char*)req + sizeof(int32);
    }

    /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.access_method.access_method)),
                  (void*)req,
                  sizeof(int32) );
    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.access_method.file_enum)),
                  (void*)req,
                  sizeof(int32) );
    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand == MMGSDIDIAG_READ_TRANSPARENT_EXT_CMD)
    {
      /* populate the path info */
      gsdidiag_get_mmgsdidiag_path_type_payload(
        &req_ptr->req_payload.request.read_transparent_req.access_method.path,
        &req,
        &offset);
    }

    /* OFFSET :- Extract offset from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.offset)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* REQ LEN  Extract requested len from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.req_data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_transparent_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    break;

    case MMGSDIDIAG_READ_RECORD_CMD:
    case GSDIDIAG_READ_RECORD_CMD:
    case MMGSDIDIAG_READ_RECORD_EXT_CMD:

    req = (char*)req + offset;

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand != GSDIDIAG_READ_RECORD_CMD)
    {
      /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
      (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.client_id)),
                    (void*)req,
                    sizeof(uint64) );

      offset = offset + sizeof(uint64);
      req = (char*)req + sizeof(uint64);
    }
    else
    {
      req_ptr->req_payload.request.read_record_req.client_id = gsdidiag_mmgsdi_client_id;

      offset = offset + sizeof(int32);
      req = (char*)req + sizeof(int32);
    }

     /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.access_method.access_method)),
                  (void*)req,
                  sizeof(int32) );
    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.access_method.file_enum)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    if (req_ptr->req_payload.sub_cmd_hdr.subcommand == MMGSDIDIAG_READ_RECORD_EXT_CMD)
    {
      /* populate the path info */
      gsdidiag_get_mmgsdidiag_path_type_payload(
        &req_ptr->req_payload.request.read_record_req.access_method.path,
        &req,
        &offset);
    }

    /* REC NUM :- Extract record number from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.record_number)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* REQ LEN  Extract requested len from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.req_data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.read_record_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    break;

    case MMGSDIDIAG_GET_FILE_ATTR_CMD:
    case GSDIDIAG_GET_FILE_ATTR_CMD:

    req = (char*)req + offset;

     if (req_ptr->req_payload.sub_cmd_hdr.subcommand == MMGSDIDIAG_GET_FILE_ATTR_CMD)
      {
        /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
        (void)memcpy( ((void*)&(req_ptr->req_payload.request.get_file_attr_req.client_id)),
                      (void*)req,
                      sizeof(uint64) );

        offset = offset + sizeof(uint64);
        req = (char*)req + sizeof(uint64);
      }
      else
      {
        req_ptr->req_payload.request.get_file_attr_req.client_id = gsdidiag_mmgsdi_client_id;

        offset = offset + sizeof(int32);
        req = (char*)req + sizeof(int32);
      }

     /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.get_file_attr_req.access_method.access_method)),
                  (void*)req,
                  sizeof(int32) );
    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

     /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.get_file_attr_req.access_method.file_enum)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.get_file_attr_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;

  case GSDIDIAG_CLIENT_ID_REG_CMD:

    req = (char*)req + offset;

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.client_id_reg_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;

  case GSDIDIAG_CLIENT_ID_DEREG_CMD:
    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void *)&(req_ptr->req_payload.request.client_id_dereg_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.client_id_dereg_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;

  case GSDIDIAG_CHANGE_PIN_CMD:

    /* increment req pointer to obtain position of next data element */
    req = (char*)req + offset;

    /* copy the sim pin id to the payload structure */
     (void)memcpy( ((void*)&(req_ptr->req_payload.request.change_pin.sim_pin_id)),
                   (void*)req, sizeof(uint8) );

    /* increment offset and req pointer to go to position of next element */
    offset = offset + sizeof(uint8);
    req = (char*)req + sizeof(uint8);

    /* Obtain pin length to validate proper size of pin; If out of range, the
       pin length is set to 0 */
    pin_length = strlen((char*)(req));

    if(pin_length > GSDI_MAX_PIN_LEN)
    {
       pin_length = 0;
    }

    /* Copy old sim pin to payload structure */
    if(std_strlcpy(
        (char *)req_ptr->req_payload.request.change_pin.old_sim_pin,
        (char*)req,
        sizeof(req_ptr->req_payload.request.change_pin.old_sim_pin)) >=
       uint32toint32(sizeof
                     (req_ptr->req_payload.request.change_pin.old_sim_pin)))
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr old sim pin copy error to payload struc",
                              0,0,0);
    }

    /* Increment offset and req pointer to go to position of next element */
    offset = uint32touint8(offset + pin_length + GSDIDAG_NULL_CHAR_LEN);
    req = (char*)req + pin_length + GSDIDAG_NULL_CHAR_LEN;

    /* Obtain pin length to validate proper size of pin; If out of range, the
        pin length is set to 0 */
    pin_length = strlen((char*)(req));

    if(pin_length > GSDI_MAX_PIN_LEN)
    {
      pin_length = 0;
    }

     /* Copy new sim pin to payload structure */
    if(std_strlcpy( (char *)req_ptr->req_payload.request.change_pin.new_sim_pin,
                    (char*)req,
                    sizeof(req_ptr->req_payload.request.change_pin.new_sim_pin)) >=
       uint32toint32(sizeof(req_ptr->req_payload.request.change_pin.new_sim_pin)))
    {
      MMGSDI_DEBUG_MSG_ERROR("req_ptr new sim pin copy error to payload struc",
                              0,0,0);
    }
    offset = uint32touint8(offset + pin_length + 1);
    req = (char*)req + pin_length + 1;

    break;

  case GSDIDIAG_VERIFY_PIN_CMD:
    gsdidiag_get_verify_pin_payload(&req_ptr->req_payload.request.verify_pin,
                                       req, offset);

    break;

  #ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  case GSDIDIAG_GET_ATR_CMD:
    break;

  case GSDIDIAG_SIM_OPEN_CHANNEL_CMD:

    /* Extract Channel_id and store it in the request pointer */
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.channel_id)), (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract Requested Channel_id and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.requested_channel_id)), (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    break;

  case GSDIDIAG_SIM_CLOSE_CHANNEL_CMD:

    /* Extract Channel_id from req packetand store it in the request pointer */
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.channel_id)), (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract Requested Channel_id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.requested_channel_id)), (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    break;
  #endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

  case GSDIDIAG_SIM_SECURE_STORE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_secure_store_req.len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.sim_secure_store_req.len) > 0)
    {
      req_ptr->req_payload.request.sim_secure_store_req.data_buffer_p =
                                       mmgsdi_malloc(
                                       req_ptr->req_payload.request.sim_secure_store_req.len);

      if(req_ptr->req_payload.request.sim_secure_store_req.data_buffer_p == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy(((void*)(req_ptr->req_payload.request.sim_secure_store_req.data_buffer_p)),
                   (void*)req,
                   req_ptr->req_payload.request.sim_secure_store_req.len);

      offset = uint32touint8(offset + req_ptr->req_payload.request.sim_secure_store_req.len);
      req = (char*)req + req_ptr->req_payload.request.sim_secure_store_req.len;
    }
    else if ((req_ptr->req_payload.request.sim_secure_store_req.len) == 0)
    {
      req_ptr->req_payload.request.sim_secure_store_req.data_buffer_p = NULL;
    }
    break;

  case MMGSDIDIAG_SELECT_AID_CMD:

    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( (void*)&(req_ptr->req_payload.request.select_aid_req.client_id),
                  (void*)req,
                  sizeof(uint64));

    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);

    break;

  case MMGSDIDIAG_VERIFY_PIN_CMD:
  case MMGSDIDIAG_ENABLE_PIN_CMD:
  case MMGSDIDIAG_DISABLE_PIN_CMD:
  case MMGSDIDIAG_CHANGE_PIN_CMD:
  case MMGSDIDIAG_UNBLOCK_PIN_CMD:
  case MMGSDIDIAG_DISABLE_PIN_EXT_CMD:
    gsdidiag_get_pin_op_payload(req_ptr->req_payload.sub_cmd_hdr.subcommand,
                                &req_ptr->req_payload.request.pin_op_req,
                                req, offset);
    break;

  case MMGSDIDIAG_GET_ALL_PIN_STATUS_CMD:
    gsdidiag_get_get_all_pin_status_payload(
      &req_ptr->req_payload.request.get_all_pin_status_req, req, offset);
    break;

  case GSDIDIAG_CHECK_SERVICE_AVAILABLE_CMD:
    gsdidiag_get_check_service_available_payload(
     &(req_ptr->req_payload.request.service_check_req), req, offset);
    break;


  case GSDIDIAG_SIM_SECURE_READ_CMD:
    break;

  case GSDIDIAG_ROOT_KEY_WRITE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.root_key_write_req.len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.root_key_write_req.len) > 0)
    {
      req_ptr->req_payload.request.root_key_write_req.data_buffer_p =
                                       mmgsdi_malloc(
                                       req_ptr->req_payload.request.root_key_write_req.len);

      if(req_ptr->req_payload.request.root_key_write_req.data_buffer_p == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.root_key_write_req.data_buffer_p)),
                    (void*)req,
                    req_ptr->req_payload.request.root_key_write_req.len);

      offset = uint32touint8(offset + req_ptr->req_payload.request.root_key_write_req.len);
      req = (char*)req + req_ptr->req_payload.request.root_key_write_req.len;
    }
    else if ((req_ptr->req_payload.request.root_key_write_req.len) == 0)
    {
      req_ptr->req_payload.request.root_key_write_req.data_buffer_p = NULL;
    }
    break;

  case GSDIDIAG_ROOT_KEY_READ_CMD:
    break;

  case GSDIDIAG_VERIFY_PHONE_CODE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.verify_phone_code_req.phone_code_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.verify_phone_code_req.phone_code_len) > 0)
    {
      req_ptr->req_payload.request.verify_phone_code_req.phone_code_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.verify_phone_code_req.phone_code_len));

      if(req_ptr->req_payload.request.verify_phone_code_req.phone_code_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.verify_phone_code_req.phone_code_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.verify_phone_code_req.phone_code_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.verify_phone_code_req.phone_code_len);
      req = (char*)req + req_ptr->req_payload.request.verify_phone_code_req.phone_code_len;
    }
    else if ((req_ptr->req_payload.request.verify_phone_code_req.phone_code_len) == 0)
    {
      req_ptr->req_payload.request.verify_phone_code_req.phone_code_ptr = NULL;
    }
    break;

  case GSDIDIAG_CHANGE_PHONE_CODE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len) > 0)
    {
      req_ptr->req_payload.request.change_phone_code_req.old_phone_code_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len));

      if(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len);
      req = (char*)req + req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len;
    }
    else if ((req_ptr->req_payload.request.change_phone_code_req.old_phone_code_len) == 0)
    {
      req_ptr->req_payload.request.change_phone_code_req.old_phone_code_ptr = NULL;
    }
    if((req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len) > 0)
    {
      req_ptr->req_payload.request.change_phone_code_req.new_phone_code_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len));

      if(req_ptr->req_payload.request.change_phone_code_req.new_phone_code_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.change_phone_code_req.new_phone_code_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len);
      req = (char*)req + req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len;
    }
    else if ((req_ptr->req_payload.request.change_phone_code_req.new_phone_code_len) == 0)
    {
      req_ptr->req_payload.request.change_phone_code_req.new_phone_code_ptr = NULL;
    }
    break;


  case GSDIDIAG_UNBLOCK_PHONE_CODE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.unblock_phone_code_req.master_key_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.unblock_phone_code_req.master_key_len) > 0)
    {
      req_ptr->req_payload.request.unblock_phone_code_req.master_key_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.master_key_len));

      if(req_ptr->req_payload.request.unblock_phone_code_req.master_key_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.unblock_phone_code_req.master_key_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.master_key_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.master_key_len);
      req = (char*)req + req_ptr->req_payload.request.unblock_phone_code_req.master_key_len;
    }
    else if ((req_ptr->req_payload.request.unblock_phone_code_req.master_key_len) == 0)
    {
      req_ptr->req_payload.request.unblock_phone_code_req.master_key_ptr = NULL;
    }
    if((req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len) > 0)
    {
      req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len));

      if(req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len);
      req = (char*)req + req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len;
    }
    else if ((req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_len) == 0)
    {
      req_ptr->req_payload.request.unblock_phone_code_req.new_phone_code_ptr = NULL;
    }
    break;

  case GSDIDIAG_SET_PHONE_CODE_CMD:
    req = (char*)req + offset;
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.set_phone_code_req.phone_code_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    if((req_ptr->req_payload.request.set_phone_code_req.phone_code_len) > 0)
    {
      req_ptr->req_payload.request.set_phone_code_req.phone_code_ptr =
                                       mmgsdi_malloc(
                                       int32touint32(req_ptr->req_payload.request.set_phone_code_req.phone_code_len));

      if(req_ptr->req_payload.request.set_phone_code_req.phone_code_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.set_phone_code_req.phone_code_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.set_phone_code_req.phone_code_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.set_phone_code_req.phone_code_len);
      req = (char*)req + req_ptr->req_payload.request.set_phone_code_req.phone_code_len;
    }
    else if ((req_ptr->req_payload.request.set_phone_code_req.phone_code_len) == 0)
    {
      req_ptr->req_payload.request.set_phone_code_req.phone_code_ptr = NULL;
    }
    break;

  case GSDIDIAG_GET_LIB_VERSION_CMD:
    break;

  #ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
  case GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD:

    /* Extract Channel_id from req packet and store it in the request pointer */
    req = (char*)req + offset;

    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.channel_id)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);

    req = (char*)req + sizeof(uint32);

    /* Extract apdu_lenfrom req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.apdu_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);

    req = (char*)req + sizeof(uint32);

    /* Extract Apdu_buffer from req packet if apdu_len is greater than zero else assign aapdu_buffer to NULL) */
    if((req_ptr->req_payload.request.channel_req.apdu_len) > 0)
    {
      req_ptr->req_payload.request.channel_req.apdu_buffer_p =
        (byte *)mmgsdi_malloc(
          int32touint32(req_ptr->req_payload.request.channel_req.apdu_len));

      if(req_ptr->req_payload.request.channel_req.apdu_buffer_p == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.channel_req.apdu_buffer_p)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.channel_req.apdu_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.channel_req.apdu_len);
      req = (char*)req + req_ptr->req_payload.request.channel_req.apdu_len;
    }
    else if ((req_ptr->req_payload.request.channel_req.apdu_len) == 0)
    {
      req_ptr->req_payload.request.channel_req.apdu_buffer_p = NULL;
    }

    break;
    #endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */

    #if (defined(FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM))
    case GSDIDIAG_ATCSIM_RAW_APDU_CMD:

    req = (char*)req + offset;

    /* Extract Channel_id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.channel_id)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);

    req = (char*)req + sizeof(uint32);

    /* Extract apdu_len from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.channel_req.apdu_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);

    req = (char*)req + sizeof(uint32);

    /* Extract Apdu_buffer from req packet if apdu_len is greater than zero else assign aapdu_buffer to NULL) */
    if((req_ptr->req_payload.request.channel_req.apdu_len) > 0)
    {
      req_ptr->req_payload.request.channel_req.apdu_buffer_p =
        (byte *)mmgsdi_malloc(
          int32touint32(req_ptr->req_payload.request.channel_req.apdu_len));

      if(req_ptr->req_payload.request.channel_req.apdu_buffer_p == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.channel_req.apdu_buffer_p)),
                    (void*)req,
                     int32touint32(req_ptr->req_payload.request.channel_req.apdu_len));

      offset = offset + int32touint32(req_ptr->req_payload.request.channel_req.apdu_len);
      req = (char*)req + req_ptr->req_payload.request.channel_req.apdu_len;
    }
    else if ((req_ptr->req_payload.request.channel_req.apdu_len) == 0)
    {
      req_ptr->req_payload.request.channel_req.apdu_buffer_p = NULL;
    }

    break;
    #endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT || FEATURE_MMGSDI_ATCSIM*/
  case GSDIDIAG_SIM_READ_CMD:
  case GSDIDIAG_SIM_READ_X_CMD:

    req = (char*)req + offset;

    /* Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.sim_filename)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract index mode from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.index_mode)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract record number from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.rec_num)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search string length from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.search_string_len)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search_string from req packet and store it in the request pointer only
       if the search string len is greater than 0 */
    if((req_ptr->req_payload.request.sim_read_req.search_string_len) > 0)
    {
      req_ptr->req_payload.request.sim_read_req.search_string =
         (char *)mmgsdi_malloc(
          req_ptr->req_payload.request.sim_read_req.search_string_len);

      if(req_ptr->req_payload.request.sim_read_req.search_string == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.sim_read_req.search_string)),
                    (void*)req,
                    req_ptr->req_payload.request.sim_read_req.search_string_len);

      offset = offset + int32touint32(req_ptr->req_payload.request.sim_read_req.search_string_len);
      req = (char*)req + req_ptr->req_payload.request.sim_read_req.search_string_len;

    }
    else if (( req_ptr->req_payload.request.sim_read_req.search_string_len) == 0)
    {
       req_ptr->req_payload.request.sim_read_req.search_string = NULL;
    }

    /* Extract required_data_len to be read from req packet and store it in the
       request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.required_data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* Extract offset from where to start the read - from the req packet and
       store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_read_req.data_offset)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;
  case GSDIDIAG_FORCE_SIM_READ_CMD:
    gsdidiag_get_force_sim_read_payload(&req_ptr->req_payload.request.force_sim_read_req,
                                       req, offset);
    break;
  case GSDIDIAG_SIM_INCREASE_CMD:
    gsdidiag_get_sim_increase_payload(&req_ptr->req_payload.request.sim_increase_req,
                                      req, offset);
    break;

  case GSDIDIAG_SIM_USIM_RUIM_SEARCH_CMD:

    req = (char*)req + offset;

    /* Extract EF- filename from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.sim_filename)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract index mode from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.index_mode)),
                  (void*)req,
                  sizeof(uint32) );

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract record number from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_record_num)),
                  (void*)req,
                  sizeof(uint32));

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search mode from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_mode)),
                  (void*)req,
                  sizeof(uint32));

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search direction from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_direction)),
                  (void*)req,
                  sizeof(uint32));

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search direction from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len)),
                  (void*)req,
                  sizeof(uint32));

    offset = offset + sizeof(uint32);
    req = (char*)req + sizeof(uint32);

    /* Extract search_string from req packet if search_string_len is greater than zero */
    /* else assign search_string to NULL) */
    if((req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len) > 0)
    {
      req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string =
        (char *)mmgsdi_malloc(
         req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len);

      if(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string)),
                    (void*)req,
                    req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len);

    }
    else if (( req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len) == 0)
    {
       req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string = NULL;
    }
    offset = uint32touint8(offset +  req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len+1);
    req = (char*)req +  req_ptr->req_payload.request.sim_usim_ruim_search_req.search_string_len+1;

    break;
  case GSDIDIAG_GET_ECC_CMD:
  case GSDIDIAG_SECURE_DEVICE_INFO_CMD:
    break;
  case GSDIDIAG_UIM_CMD:
    gsdidiag_get_uim_cmd_payload(&(req_ptr->req_payload.request.uim_cmd_req), req, offset);
    break;
  case GSDIDIAG_ISIM_AUTHENTICATE_CMD:

    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.isim_auth_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);


    /* Authentication LEN :- Extract authentication len from req packet and store it in the
       request pointer only if the authentication buffer is not NULL */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.isim_auth_req.auth.data_len)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    /* Authentication BUFFER :- Extract authentication buffer from req packet if
       Authentication buffer len is greater than zero else assign
       Authentication buffer to NULL) */
    if((req_ptr->req_payload.request.isim_auth_req.auth.data_len) > 0)
    {
      req_ptr->req_payload.request.isim_auth_req.auth.data_ptr =
        (uint8 *)mmgsdi_malloc(
        int32touint32(req_ptr->req_payload.request.isim_auth_req.auth.data_len));

      if(req_ptr->req_payload.request.isim_auth_req.auth.data_ptr == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        return;
      }

      (void)memcpy( ((void*)(req_ptr->req_payload.request.isim_auth_req.auth.data_ptr)),
                    (void*)req,
                    int32touint32(req_ptr->req_payload.request.isim_auth_req.auth.data_len));

    }
    else if (( req_ptr->req_payload.request.isim_auth_req.auth.data_len) == 0)
    {
      req_ptr->req_payload.request.isim_auth_req.auth.data_ptr = NULL;
    }
    offset = offset + int32touint32(req_ptr->req_payload.request.isim_auth_req.auth.data_len+1);
    req = (char*)req + req_ptr->req_payload.request.isim_auth_req.auth.data_len+1;

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.isim_auth_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;

  case GSDIDIAG_SESSION_OPEN_CMD:

    req = (char*)req + offset;

     /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.session_open_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.session_open_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;

  case GSDIDIAG_SESSION_CLOSE_CMD:

    req = (char*)req + offset;

     /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.session_close_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    offset = offset + sizeof(uint64);
    req = (char*)req + sizeof(uint64);

    /* CLIENT REF PTR Extract client ref value from req packet and store it
       in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.session_close_req.client_ref_val)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    break;
  case GSDIDIAG_SAP_CONNECT_CMD:
    /* No payload */
    break;
  case GSDIDIAG_SAP_DISCONNECT_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case GSDIDIAG_SAP_DEREGISTER_CMD:
  case GSDIDIAG_SAP_INIT_CMD:
  case GSDIDIAG_SAP_CARD_READER_STATUS_CMD:
  case GSDIDIAG_SAP_RESET_SIM_CMD:
  case GSDIDIAG_SAP_GET_ATR_CMD:
  case GSDIDIAG_SAP_PIN_VERIFY_CMD:
     break;
  case GSDIDIAG_SAP_SEND_APDU_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
    break;
  case GSDIDIAG_SAP_POWER_OFF_CMD:
  case GSDIDIAG_SAP_POWER_ON_CMD:
    break;
  case GSDIDIAG_CARD_PDOWN_CMD:
    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.card_pdown_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    /* Move the pointer accordingly */
    req = (char*)req + sizeof(uint64);

    /* PDOWN Option:- Extract power down
     * option from req packet and store it in the request pointer
     */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.card_pdown_req.pdown_option)),
                  (void*)req,
                  sizeof(int32) );
    break;
  case GSDIDIAG_CARD_PUP_CMD:
    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.card_pup_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    /* Move the pointer accordingly */
    req = (char*)req + sizeof(uint64);

    /* PUP Option:- Extract power up
     * option from req packet and store it in the request pointer
     */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.card_pup_req.pup_option)),
                  (void*)req,
                  sizeof(int32) );
    break;

  case GSDIDIAG_CARD_STATUS_CMD:
    req = (char*)req + offset;

    /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.status_req.client_id)),
                  (void*)req,
                  sizeof(uint64) );

    /* Move the pointer accordingly */
    req = (char*)req + sizeof(uint64);

     /* APP_STATUS */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.status_req.app_status)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);

    /* RET_DATA_STRUCT */
    (void)memcpy( ((void*)&(req_ptr->req_payload.request.status_req.ret_data_struct)),
                  (void*)req,
                  sizeof(int32) );

    offset = offset + sizeof(int32);
    req = (char*)req + sizeof(int32);
    break;

  #ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
  #endif /* FEATURE_MMGSDI_ONCHIP_SIM */

  case MMGSDIDIAG_REGISTER_FOR_EVENT_CMD:
    gsdidiag_extract_client_id((uint64*)&(req_ptr->req_payload.request.client_evt_reg_req.client_id),
                               (void*)&req,&offset);
    break;
  case MMGSDIDIAG_OK_TO_REFRESH_CMD:
    gsdidiag_extract_client_id((uint64*)&(req_ptr->req_payload.request.ok_to_refresh_req.client_id),
                               (void*)&req,&offset);
    break;
  case MMGSDIDIAG_REFRESH_REQ_CMD:
    gsdidiag_refresh_req_payload(&req_ptr->req_payload.request.refresh_reg_req,
                                     req, offset);
    break;

  case MMGSDIDIAG_UICC_SEARCH_REQ_CMD:
  case MMGSDIDIAG_ICC_SEARCH_REQ_CMD:
    gsdidiag_get_search_payload(&req_ptr->req_payload,req,offset);
    break;
  case MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD:
    gsdidiag_extract_client_id((uint64*)&(req_ptr->req_payload.request.create_pkcs15_lookup_table_req.client_id),
                               (void*)&req,&offset);
    break;
  case MMGSDIDIAG_REFRESH_REG_BY_PATH_CMD:
    gsdidiag_refresh_reg_by_path_payload(
      &req_ptr->req_payload.request.refresh_reg_req,req, offset);
    break;
  case MMGSDIDIAG_REFRESH_DEREG_CMD:
    gsdidiag_refresh_dereg_payload(
      &req_ptr->req_payload.request.refresh_dereg_req, req, offset);
    break;
  case GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.activate_feature_indicator_req.perso_feature),
      req,
      &offset);
    gsdidiag_extract_null_term_string(
      (char*)req_ptr->req_payload.request.activate_feature_indicator_req.perso_feature_key,
      GSDI_PERSO_MAX_CK + 1,
      req,
      &offset);
    break;

  case GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.deactivate_feature_indicator_req.perso_feature),
      req,
      &offset);
    gsdidiag_extract_null_term_string(
      (char*)req_ptr->req_payload.request.deactivate_feature_indicator_req.perso_feature_key,
      GSDI_PERSO_MAX_CK + 1,
      req,
      &offset);
    break;

  case GSDIDIAG_GET_FEATURE_INDICATION_CMD:
    /* No payload */
    break;

  case GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature),
      req,
      &offset);
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_len),
      req,
      &offset);

    req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr = NULL;
    if (req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_len > 0)
    {
      uint32 len = req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_len;

      req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr =
        (uint8*)mmgsdi_malloc(len);
      if (req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr != NULL)
      {
        memcpy((void*)req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr,
               (void*)((char*)req + offset),
               len);
      }
      else
      {
        /* setting length to zero */
        req_ptr->req_payload.request.set_feature_indicator_data_req.perso_feature_data_len = 0;

        MSG_HIGH("memory allocation fail", 0, 0, 0);
      }
      offset = offset + len;
    }
    break;
  case GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.get_feature_indicator_data_req.perso_feature),
      req,
      &offset);
    break;
  case GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.get_feature_indicator_key_req.perso_key),
      req,
      &offset);
    break;

  case GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.unblock_feature_indicator_req.perso_feature),
      req,
      &offset);
    gsdidiag_extract_null_term_string(
      (char*)req_ptr->req_payload.request.unblock_feature_indicator_req.perso_feature_key,
      GSDI_PERSO_MAX_CK + 1,
      req,
      &offset);
    break;

  case GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD:
    /* No payload */
    break;

  case GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD:
    gsdidiag_extract_uint32(
      (uint32*)&(req_ptr->req_payload.request.perm_disable_feature_indicator_req.perso_feature),
      req,
      &offset);
    gsdidiag_extract_null_term_string(
      (char*)req_ptr->req_payload.request.perm_disable_feature_indicator_req.perso_feature_key,
      GSDI_PERSO_MAX_CK + 1,
      req,
      &offset);
    break;

  case GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD:
    /* No payload */
    break;

  default:
    break;
  }
}/*end of gsdidiag_get_payload*/

/*===========================================================================
FUNCTION: gsdidiag_get_sub_cmd_header

DESCRIPTION: Populates the sub_cmd_hdr_req_type structure with the data from
             the request packet.

ARGUMENTS:
    gsdidiag_generic_cmd_req_type *req_ptr: generic_command request pointer
                      which will be populated with data from the req packet.
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    uint8 offset:     offset of the req pointer

RETURN VALUE
    uint8 offset:     offset of the req pointer
===========================================================================*/
static uint32 gsdidiag_get_sub_cmd_header(
  gsdidiag_generic_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
)
{
    /* The req pointer and offset are incremented to reach the position of the
     next element.  This element is then copied into the sub_cmd_hdr
     structure for the request packet */

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return offset;
    }
    if(req_ptr == NULL)
    {
        MSG_ERROR("req_ptr ERR:NULL",0,0,0);
        return offset;
    }

    req = (char*)req + offset;
    /* asp_as_transport */
    memcpy( ((uint32*)&(req_ptr->req_payload.sub_cmd_hdr.aps_as_transport)), (uint32*)req, sizeof(uint32) );
    req = (char*)req + sizeof(uint32);
     offset = offset + sizeof(uint32);

    /* subcommand */
    (void)memcpy( ((uint32*)&(req_ptr->req_payload.sub_cmd_hdr.subcommand)), (uint32*)req, sizeof(uint32) );
    req = (char*)req + sizeof(uint32);
    offset = offset + sizeof(uint32);

    /* slot_id */
    (void)memcpy( ((uint8*)&(req_ptr->req_payload.sub_cmd_hdr.slot_id)), (uint8*)req, sizeof(uint8) );
    req = (char*)req + sizeof(uint8);
    offset = offset + sizeof(uint8);

    /* sim_app_id_len */
    (void)memcpy( ((uint8*)&(req_ptr->req_payload.sub_cmd_hdr.sim_app_id_len)), (uint8*)req, sizeof(uint8) );
    req = (char*)req + sizeof(uint8);
    offset = offset + sizeof(uint8);

    /* sim_app_id */
    (void)memcpy( (void*)req_ptr->req_payload.sub_cmd_hdr.sim_app_id, (uint8*)req, sizeof(uint8)*GSDI_MAX_APP_ID_LEN );
    offset = offset + (sizeof(uint8)*GSDI_MAX_APP_ID_LEN);

    /* returns the offset of the req pointer */
    return offset;
}
#endif /*FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION*/

#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
/*===========================================================================
FUNCTION gsdidiag_get_atr_cb

DESCRIPTION
    Callback used for an asynchronous get ATR.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer (rsp_ptr) is populated with the header information and the payload
   information.

===========================================================================*/
static void gsdidiag_get_atr_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len     = sizeof( gsdidiag_generic_cmd_rsp_type );

    index   = uint32toint32(sim_data->message_header.client_ref);

    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));


    /*   aps_as_transport is set from the value stored in the request table
    */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_GET_ATR_CMD */

    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_ATR_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_get_atr_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->get_atr_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->get_atr_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->get_atr_cnf.message_header.gsdi_status;
    }

    if (sim_data->get_atr_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        rsp_ptr->rsp_payload.response.data.len  = int32touint32(sim_data->get_atr_cnf.data_len);

        /* if (rsp_ptr->rsp_payload.response.data.len) is greater than zero, copy the data
           into the buffer else memset the buffer
        */
        if( (rsp_ptr->rsp_payload.response.data.len > 0) &&
            (rsp_ptr->rsp_payload.response.data.len <= GSDI_MAX_ATR_BYTES))
        {
          (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.data.buffer,
                 (uint8*)sim_data->get_atr_cnf.data_buffer,
                 rsp_ptr->rsp_payload.response.data.len
                );
        }
        else
        {
          (void)memset((uint8*)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
                 0x00,
                 GSDI_MAX_DATA_BLOCK_LEN * 2
                );
        }

    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}


/*===========================================================================
FUNCTION gsdidiag_get_atr

DESCRIPTION
    Gets the ATR. A get ATR command is queued to gsdi task.

INPUT:
    PACKED void *req,        -Pointer to request Packet
    word pkt_len,            -Size of request packet
    uint32 * status,         -Status returned by gsdi_sim_get_atr API
    uint16 * rsp_id,         -Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_get_atr (
    PACKED void *req,       /* Request packet */
    word pkt_len,           /* Size of the request packet */
    uint32 * status,        /* Status returned by gsdi_sim_get_atr API */
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag */
    uint32 aps_as_transport
)
{
    // Create the GSDIDIAG GET ATR REQ TYPE (Void)
    int                              index;
    gsdi_returns_T                   gsdi_status;
    gsdidiag_req_payload_type       *req_ptr;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
        /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id = rsp_id;

    /* store the aps_as_transport information into the request table */
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_GET_ATR_CMD;

    /* send the request to get ATR */
    gsdi_status = gsdi_sim_get_atr ((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                    int32touint32(index),
                                    (gsdi_async_cb)gsdidiag_get_atr_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_get_atr queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);
} /* gsdidiag_get_atr */


/*===========================================================================
FUNCTION gsdidiag_open_channel_cb

DESCRIPTION
    Callback used for an asynchronous open channel.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the channel id.
===========================================================================*/
static void gsdidiag_open_channel_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));


    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SIM_OPEN_CHANNEL_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_OPEN_CHANNEL_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_open_channel_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->open_channel_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->open_channel_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->open_channel_cnf.message_header.gsdi_status;
    }

    if (sim_data->open_channel_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.channel_data.channel_id
        = sim_data->open_channel_cnf.channel_id;
      rsp_ptr->rsp_payload.response.channel_data.payload.len
        = int32touint32(sim_data->open_channel_cnf.data_len);

      /* if (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater than zero,
         copy the data into the buffer else memset the buffer
        */
      if( (rsp_ptr->rsp_payload.response.channel_data.payload.len > 0) &&
          (rsp_ptr->rsp_payload.response.channel_data.payload.len <= (GSDI_MAX_DATA_BLOCK_LEN * 2)))
      {
          (void)memcpy((uint8 *)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
                 (uint8*)sim_data->open_channel_cnf.data_buffer,
                 rsp_ptr->rsp_payload.response.channel_data.payload.len
                );
      }
      else
      {
          MSG_ERROR("Length could be lesser or greater than expected",0,0,0);
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_open_channel

DESCRIPTION
    Opens the requested channel. An open channel command is queued to gsdi task.

INPUT:
    PACKED void *req,        -Pointer to request packet
    word pkt_len,            -Size of the request packet
    uint32 * status,         -Status returned by gsdi_sim_open_channel API
    uint16 * rsp_id,         -Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_open_channel (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    // Create the GSDIDIAG OPEN CHANNEL REQ TYPE
    gsdidiag_req_payload_type        *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    int32                            channel_id;
    int32                            requested_channel_id;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    channel_id           = req_ptr->request.channel_req.channel_id;
    requested_channel_id = req_ptr->request.channel_req.requested_channel_id;


    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_SIM_OPEN_CHANNEL_CMD;

    /* send the request to open the requested channel*/
    gsdi_status = gsdi_sim_open_channel((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                        NULL,
                                        channel_id,
                                        requested_channel_id,
                                        int32touint32(index),
                                        (gsdi_async_cb)gsdidiag_open_channel_cb
                                       );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_open_channel queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, status);
} /* gsdidiag_open_channel */


/*===========================================================================
FUNCTION gsdidiag_close_channel_cb

DESCRIPTION
    Callback used for an asynchronous close channel.

DEPENDENCIES
    None

INPUT
   gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the channel id.
===========================================================================*/
static void gsdidiag_close_channel_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   =  sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is set from the value stored in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SIM_CLOSE_CHANNEL_CMD*/
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_CLOSE_CHANNEL_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_close_channel_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->close_channel_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->close_channel_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
       sim_data->close_channel_cnf.message_header.gsdi_status;
    }

    if (sim_data->close_channel_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.channel_data.channel_id =
        sim_data->close_channel_cnf.channel_id;
      rsp_ptr->rsp_payload.response.channel_data.payload.len =
        int32touint32(sim_data->close_channel_cnf.data_len);

      /* if (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater
         than zero, copy the data into the buffer else memset the buffer
      */
      if( (rsp_ptr->rsp_payload.response.channel_data.payload.len > 0) &&
          (rsp_ptr->rsp_payload.response.channel_data.payload.len <= (GSDI_MAX_DATA_BLOCK_LEN * 2)))
      {
          (void)memcpy((uint8 *)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
                 (uint8*)sim_data->close_channel_cnf.data_buffer,
                 rsp_ptr->rsp_payload.response.channel_data.payload.len
                );
      }
      else
      {
          MSG_ERROR("Length could be lesser or greater than expected",0,0,0);
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }

    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_close_channel

DESCRIPTION
    Closes the requested channel. An close channel command is queued to gsdi task.

INPUT:
    PACKED void *req,        -Pointer to request packet
    word pkt_len,            -Size of the request packet
    uint32 * status,         -Status returned by gsdi_sim_close_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id,  -Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_close_channel (
    PACKED void *req,      /* pointer to request packet*/
    word pkt_len,          /* length of request packet*/
    uint32 * status,       /* Status returned by gsdi_sim_close_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,       /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    // Create the GSDIDIAG CLOSE CHANNEL REQ TYPE (Void)
    gsdidiag_req_payload_type        *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    int32                            channel_id;
    int32                            requested_channel_id;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    channel_id           = req_ptr->request.channel_req.channel_id;
    requested_channel_id = req_ptr->request.channel_req.requested_channel_id;


    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id =   rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_SIM_CLOSE_CHANNEL_CMD;

    /* send the request to close the requested channel*/
    gsdi_status = gsdi_sim_close_channel((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                        channel_id,
                                        requested_channel_id,
                                        int32touint32(index),
                                        (gsdi_async_cb)gsdidiag_close_channel_cb
                                       );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_close_channel queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_close_channel */


 /*===========================================================================
FUNCTION gsdidiag_send_channel_data_cb

DESCRIPTION
    Callback used for an asynchronous send channel data.

DEPENDENCIES
    None

INPUT
   gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the channel id.
===========================================================================*/
static void gsdidiag_send_channel_data_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   =  sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );

    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport in the rsp_ptr is set from the value stored in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_close_channel_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->send_channel_data_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->send_channel_data_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->send_channel_data_cnf.message_header.gsdi_status;
    }

    if (sim_data->send_channel_data_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.channel_data.channel_id
        = sim_data->send_channel_data_cnf.channel_id;
      rsp_ptr->rsp_payload.response.channel_data.payload.len
        = int32touint32(sim_data->send_channel_data_cnf.apdu_len);
      /* if the (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater than zero,
         copy data into the buffer else memset the buffer
      */
      if( (rsp_ptr->rsp_payload.response.channel_data.payload.len > 0) &&
          (rsp_ptr->rsp_payload.response.channel_data.payload.len <= (GSDI_MAX_DATA_BLOCK_LEN * 2)))
      {
        (void)memcpy((uint8 *)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
               (uint8*)sim_data->send_channel_data_cnf.data_buffer,
               rsp_ptr->rsp_payload.response.channel_data.payload.len
              );
      }
      else
      {
         MSG_ERROR("Length could be lesser or greater than expected",0,0,0);
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.channel_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

 /*===========================================================================
FUNCTION gsdidiag_send_channel_data

DESCRIPTION
    Sends the channel data. An send channel data command is queued to gsdi task.

INPUT:
    PACKED void *req,       - Pointer to request packet
    word pkt_len,           - Size of the requets packet
    uint32 * status,        - Status returned by gsdi_sim_send_channel_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id, - Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_send_channel_data (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* Status returned by gsdi_sim_send_channel_data*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,    /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    // Create the GSDIDIAG SEND CHANNEL DATA REQ TYPE
    gsdidiag_req_payload_type        *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    int32                            channel_id;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    channel_id           = req_ptr->request.channel_req.channel_id;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD;

    /* send the request to send the data on the specified channel*/
    gsdi_status = gsdi_sim_send_channel_data((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                        channel_id,
                                        req_ptr->request.channel_req.apdu_len,
                                        req_ptr->request.channel_req.apdu_buffer_p,
                                        int32touint32(index),
                                        (gsdi_async_cb)gsdidiag_send_channel_data_cb
                                       );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_send_channel_data queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_send_channel_data */
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */


#if (defined(FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM))
/*===========================================================================
FUNCTION gsdidiag_atcsim_raw_apdu_cb

DESCRIPTION
    Callback used for an asynchronous send channel data.

DEPENDENCIES
    None

INPUT
   gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the channel id.
===========================================================================*/
static void gsdidiag_atcsim_raw_apdu_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   =  sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );

    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport in the rsp_ptr is set from the value stored in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

    /* subcommand is set to GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ATCSIM_RAW_APDU_CMD;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_close_channel_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

    if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->send_channel_data_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->send_channel_data_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->send_channel_data_cnf.message_header.gsdi_status;
    }

    if (sim_data->send_channel_data_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.atcsim_data.channel_id
        = sim_data->send_channel_data_cnf.channel_id;
      rsp_ptr->rsp_payload.response.atcsim_data.implicit_get_rsp_sw1 =
        sim_data->send_channel_data_cnf.implicit_get_rsp_sw1;
      rsp_ptr->rsp_payload.response.atcsim_data.implicit_get_rsp_sw2 =
        sim_data->send_channel_data_cnf.implicit_get_rsp_sw2;
      rsp_ptr->rsp_payload.response.atcsim_data.payload.len
        = int32touint32(sim_data->send_channel_data_cnf.apdu_len);
      /* if the (rsp_ptr->rsp_payload.response.channel_data.payload.len) is greater than zero,
         copy data into the buffer else memset the buffer
      */
      if( (rsp_ptr->rsp_payload.response.atcsim_data.payload.len > 0) &&
          (rsp_ptr->rsp_payload.response.atcsim_data.payload.len <= ( GSDI_MAX_DATA_BLOCK_LEN * 2)))
      {
        (void)memcpy((uint8 *)rsp_ptr->rsp_payload.response.atcsim_data.payload.buffer,
               (uint8*)sim_data->send_channel_data_cnf.data_buffer,
               rsp_ptr->rsp_payload.response.atcsim_data.payload.len
              );
      }
      else
      {
        MSG_ERROR("Length could be lesser or greater than expected",0,0,0);
        (void)memset((uint8*)rsp_ptr->rsp_payload.response.atcsim_data.payload.buffer,
               0x00,
               GSDI_MAX_DATA_BLOCK_LEN * 2
              );
      }
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_atcsim_raw_apdu

DESCRIPTION
    Sends the channel data. An send channel data command is queued to gsdi task.

INPUT:
    PACKED void *req,       - Pointer to request packet
    word pkt_len,           - Size of the requets packet
    uint32 * status,        - Status returned by gsdi_sim_send_channel_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id, - Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_atcsim_raw_apdu (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* Status returned by gsdi_sim_send_channel_data*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,    /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    // Create the GSDIDIAG SEND CHANNEL DATA REQ TYPE
    gsdidiag_req_payload_type        *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    int32                            channel_id;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;


    channel_id           = req_ptr->request.channel_req.channel_id;


    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_ATCSIM_RAW_APDU_CMD;

    /* send the request to send the data on the specified channel*/
    gsdi_status = gsdi_sim_send_channel_data((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                        channel_id,
                                        req_ptr->request.channel_req.apdu_len,
                                        req_ptr->request.channel_req.apdu_buffer_p,
                                        int32touint32(index),
                                        (gsdi_async_cb)gsdidiag_atcsim_raw_apdu_cb
                                       );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_send_channel_data queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_atcsim_raw_apdu */
#endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT || FEATURE_MMGSDI_ATCSIM */


#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
/*===========================================================================
FUNCTION gsdidiag_sim_usim_ruim_search_cb

DESCRIPTION
    Callback used for a sim read call.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_sim_usim_ruim_search_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_OPEN_CHANNEL_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_USIM_RUIM_SEARCH_CMD ;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_sim_usim_ruim_search_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

   if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->search_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->search_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->search_cnf.message_header.gsdi_status;
    }

    if (sim_data->read_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.rec_num_accessed =
        sim_data->search_cnf.rec_num_accessed;
    }

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_sim_usim_ruim_all_search

DESCRIPTION
    Sends the channel data. An send channel data command is queued to gsdi task.

INPUT:
    PACKED void *req,       - Pointer to request packet
    word pkt_len,           - Size of the request packet
    uint32 * status,        - Status returned by gsdi_sim_send_channel_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id, - Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_sim_usim_ruim_all_search (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* Status returned by gsdi_sim_send_channel_data*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,    /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
  // Create the GSDIDIAG SIM/USIM/RUIM SEARCH REQ TYPE
    gsdidiag_req_payload_type        *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;
    uim_items_enum_type              uim_file     = UIM_NO_SUCH_ITEM;
    gsdi_index_modes_T               index_mode   = GSDI_NO_INDEX;
    gsdi_srch_dir_type               srch_dir;
    gsdi_srch_mode                   srch_mode;
    gsdi_file_enum_type              gsdi_file_item;

    if(status == NULL)
    {
        MSG_ERROR("status ERR:NULL",0,0,0);
        return;
    }

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   =  rsp_id;

    /* store aps_as_transport in the request table */
    req_table[index].aps_as_transport = aps_as_transport;

    index_mode = gsdidiag_map_index_mode(
              req_ptr->request.sim_read_req.index_mode);

    uim_file = gsdidiag_convert_file_to_uim_enum(
      req_ptr->request.sim_usim_ruim_search_req.sim_filename);
    gsdi_status = gsdi_util_map_uim_enum_to_file_enum(
            uim_file ,
            &gsdi_file_item);

    srch_dir = gsdidiag_convert_search_dir_to_gsdi(
      req_ptr->request.sim_usim_ruim_search_req.search_direction);

    srch_mode = gsdidiag_convert_search_mode_to_gsdi(
      req_ptr->request.sim_usim_ruim_search_req.search_mode);

    /* send the request to search for gsm/cdma/wcdma */
    gsdi_status = gsdi_sim_generic_search(gsdi_file_item,
                                          (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                          index_mode,
                                          uint32toint32(req_ptr->request.sim_usim_ruim_search_req.search_record_num),
                                          srch_mode,
                                          srch_dir,
                                          req_ptr->request.sim_usim_ruim_search_req.search_string,
                                          uint32toint32(req_ptr->request.sim_usim_ruim_search_req.search_string_len),
                                          int32touint32(index),
                                          (gsdi_async_cb)gsdidiag_sim_usim_ruim_search_cb
                                         );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdi_sim_usim_ruim_all_search queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
}

/*===========================================================================
FUNCTION gsdidiag_get_ecc_cb

DESCRIPTION
    Callback used for a getting ECC information

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the ECC table
===========================================================================*/
static void gsdidiag_get_ecc_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
    uint32 len; /* length of event */
    int index; /* index into req_table*/
    int32 loop = 0;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len   = sizeof( gsdidiag_generic_cmd_rsp_type );

    index = uint32toint32(sim_data->message_header.client_ref);

    /* Allocate memory for the response pointer */
    rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                             (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
                                             req_table[index].transaction_id,
                                             len );
    if(rsp_ptr == NULL)
    {
        MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
        gsdidiag_free_index(index);
        return;
    }

    (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

    /* aps_as_transport is populated from the value in the request table */
    rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

     /* subcommand is set to GSDIDIAG_SIM_OPEN_CHANNEL_CMD */
    rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_ECC_CMD ;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_sim_usim_ruim_search_cb.",index,0,0);

        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

        diagpkt_delay_commit(rsp_ptr);
        return;
    }

   if (req_table[index].aps_as_transport)
    {
      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->ecc_cnf.message_header.gsdi_status;
    }
    else
    {
      uint32 temp_status;

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->ecc_cnf.message_header.gsdi_status,
        &temp_status);

      diagpkt_subsys_set_status(rsp_ptr, temp_status);

      rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        sim_data->ecc_cnf.message_header.gsdi_status;
    }

    if (sim_data->ecc_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
      rsp_ptr->rsp_payload.response.ecc_data.num_of_rec =
        sim_data->ecc_cnf.emergency_call_code.num_of_rec;
      if(rsp_ptr->rsp_payload.response.ecc_data.num_of_rec > 0)
      {
        while((loop < sim_data->ecc_cnf.emergency_call_code.num_of_rec) &&
              (loop < GSDI_MAX_ECC_RECORDS)
             )
        {
          /* (void)Memset the new record pointer */
          (void)memset((gsdi_ecc_info_T*)&(rsp_ptr->rsp_payload.response.ecc_data.ecc_p[loop]),
                 0xFF,
                 sizeof(gsdi_ecc_info_T)
                );
          (void)memcpy((gsdi_ecc_info_T*)&(rsp_ptr->rsp_payload.response.ecc_data.ecc_p[loop]),
                 sim_data->ecc_cnf.emergency_call_code.ecc_p[loop],
                 sizeof(gsdi_ecc_info_T)
                );
          loop++;
        }/* while((loop < ecc_data_p->num_of_recs) && (loop < GSDI_MAX_ECC_RECORDS)) */
      }/* if(rsp_ptr->rsp_payload.response.ecc_data.num_of_rec > 0) */
    }/*  if (sim_data->ecc_cnf.message_header.gsdi_status == GSDI_SUCCESS) */

    /* free the memory */
    gsdidiag_free_index(index);
    diagpkt_delay_commit(rsp_ptr);
    return;
}


/*===========================================================================
FUNCTION gsdidiag_mmgsdi_naa_refresh_cb

DESCRIPTION
  This function handles the client ID registration command callback.  It will
  assign the global client ID value which will be used in the mmgsdi function
  call

INPUT
  mmgsdi_return_enum_type       status:  status of command
  mmgsdi_cnf_enum_type          cnf:     confirmation type
  const mmgsdi_cnf_type        *cnf_ptr: confirmation data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdidiag_mmgsdi_naa_refresh_cb (
  mmgsdi_return_enum_type       status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr)

{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  if(cnf_ptr == NULL || cnf != MMGSDI_REFRESH_CNF)
  {
    MSG_ERROR("cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = uint32toint32(cnf_ptr->refresh_cnf.response_header.client_data);

  if (req_table[index].subcommand != GSDIDIAG_SIM_REFRESH_CMD)
  {
    /* Since a report of "previous" REFRESH_IN_PROGRESS may be possible, which means
       there won't be any need for reporting back anymore */
    MSG_HIGH("Already report REFRESH", 0, 0, 0);
    return;
  }
  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay( (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
                                            (diagpkt_subsys_cmd_code_type)GSDIDIAG_SIM_REFRESH_CMD,
                                            req_table[index].transaction_id,
                                            len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));


  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_SIM_REFRESH_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_REFRESH_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
      MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_refresh_cb.",index,0,0);

      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

      diagpkt_delay_commit(rsp_ptr);
      return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = status;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}


/*===========================================================================
FUNCTION gsdidiag_delayed_refresh

DESCRIPTION
    Function to refresh the  sim. A task is queued to gsdi task to refresh sim.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_refresh (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type  *req_ptr;
    int32                       index        = 0;
    mmgsdi_aid_type             app_id;
    mmgsdi_data_type            refresh_files;
    mmgsdi_return_enum_type     mmgsdi_status = MMGSDI_ERROR;
    mmgsdi_option_type          option        = {0};

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    (void)memset((void*)&app_id,0x00,sizeof(mmgsdi_aid_type));
    req_ptr     = (gsdidiag_req_payload_type*)req;
    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR)
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand       = GSDIDIAG_SIM_REFRESH_CMD;

    app_id.aid.data_len = uint32touint8(req_ptr->sub_cmd_hdr.sim_app_id_len);
    if (app_id.aid.data_len > GSDI_MAX_APP_ID_LEN)
    {
      MSG_ERROR("Exceed Max App ID Len 0x%x", app_id.aid.data_len, 0, 0);
      app_id.aid.data_len = GSDI_MAX_APP_ID_LEN;
    }
    if (app_id.aid.data_len > 0)
    {
      memcpy(app_id.aid.data_ptr, (uint8*)req_ptr->sub_cmd_hdr.sim_app_id,
             int32touint32(app_id.aid.data_len));
    }
    if (req_ptr->request.refresh_req.notify_in_progress)
    {
      req_table[index].var[0] = mmgsdi_malloc(
                                sizeof(req_ptr->request.refresh_req.notify_in_progress));
    }
    refresh_files.data_ptr = req_ptr->request.refresh_req.file_path_ptr;
    refresh_files.data_len = uint32toint32(req_ptr->request.refresh_req.data_len);

    /* send the request to do a mmgsdi_naa_refresh*/
    mmgsdi_status = mmgsdi_naa_refresh((mmgsdi_client_id_type)req_ptr->request.refresh_req.client_id,
                                     MMGSDI_SLOT_1,
                                     app_id,
                                     (mmgsdi_refresh_mode_enum_type)req_ptr->request.refresh_req.refresh_mode,
                                     refresh_files,
                                     uint32touint8(req_ptr->request.refresh_req.num_files),
                                     option,
                                     (mmgsdi_callback_type)gsdidiag_mmgsdi_naa_refresh_cb,
                                     int32touint32(index));

    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
       MMGSDI_DEBUG_MSG_ERROR("mmgsdi_naa_refresh queue failed %d",status,0,0);
       gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status, status);
} /* gsdidiag_delayed_refresh */

#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_map_perso_feature

DESCRIPTION
    This function maps the gsdidiag perso level to the gsdi enum

DEPENDENCIES
    None

RETURN VALUE
    gsdi_returns_T

SIDE EFFECTS
    None
===========================================================================*/
static gsdi_returns_T gsdidiag_map_perso_feature(
  gsdidiag_perso_feature_type  gsdidiag_perso_feature,
  gsdi_perso_enum_type        *gsdi_perso_feature_ptr
)
{
  gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

  if (gsdi_perso_feature_ptr == NULL)
  {
    MSG_ERROR("gsdi_perso_feature_ptr ERR:NULL",0,0,0);
    return GSDI_ERROR;
  }

  switch (gsdidiag_perso_feature)
  {
#ifdef FEATURE_PERSO_SIM
  case GSDIDIAG_PERSO_NW:
    *gsdi_perso_feature_ptr = GSDI_PERSO_NW;
    break;
  case GSDIDIAG_PERSO_NS:
    *gsdi_perso_feature_ptr = GSDI_PERSO_NS;
    break;
  case GSDIDIAG_PERSO_SP:
    *gsdi_perso_feature_ptr = GSDI_PERSO_SP;
    break;
  case GSDIDIAG_PERSO_CP:
    *gsdi_perso_feature_ptr = GSDI_PERSO_CP;
    break;
  case GSDIDIAG_PERSO_SIM:
    *gsdi_perso_feature_ptr = GSDI_PERSO_SIM;
    break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
  case GSDIDIAG_PERSO_RUIM_NW1:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_NW1;
    break;
  case GSDIDIAG_PERSO_RUIM_NW2:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_NW2;
    break;
  case GSDIDIAG_PERSO_RUIM_SP:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_SP;
    break;
  case GSDIDIAG_PERSO_RUIM_CP:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_CP;
    break;
  case GSDIDIAG_PERSO_RUIM_RUIM:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_RUIM;
    break;
  case GSDIDIAG_PERSO_RUIM_HRPD:
    *gsdi_perso_feature_ptr = GSDI_PERSO_RUIM_HRPD;
    break;
#endif /*FEATURE_PERSO_RUIM*/
  default:
    MSG_ERROR("invalid feature: %d", gsdidiag_perso_feature, 0, 0);
    gsdi_status = GSDI_ERROR;
  }
  return gsdi_status;
} /* gsdidiag_map_perso_feature */

/*===========================================================================
FUNCTION gsdidiag_map_perso_feature_rsp

DESCRIPTION
    This function maps the gsdi enum to the gsdidiag perso level

DEPENDENCIES
    None

RETURN VALUE
    gsdi_returns_T

SIDE EFFECTS
    None
===========================================================================*/
static gsdi_returns_T gsdidiag_map_perso_feature_rsp(
  gsdi_perso_enum_type         gsdi_perso_feature,
  gsdidiag_perso_feature_type *gsdidiag_perso_feature_ptr
)
{
  gsdi_returns_T  gsdi_status = GSDI_SUCCESS;

  if (gsdidiag_perso_feature_ptr == NULL)
  {
    MSG_ERROR("gsdidiag_perso_feature_ptr ERR:NULL",0,0,0);
    return GSDI_ERROR;
  }

  switch (gsdi_perso_feature)
  {
#ifdef FEATURE_PERSO_SIM
  case GSDI_PERSO_NW:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_NW;
    break;
  case GSDI_PERSO_NS:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_NS;
    break;
  case GSDI_PERSO_SP:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_SP;
    break;
  case GSDI_PERSO_CP:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_CP;
    break;
  case GSDI_PERSO_SIM:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_SIM;
    break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
  case GSDI_PERSO_RUIM_NW1:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_NW1;
    break;
  case GSDI_PERSO_RUIM_NW2:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_NW2;
    break;
  case GSDI_PERSO_RUIM_SP:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_SP;
    break;
  case GSDI_PERSO_RUIM_CP:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_CP;
    break;
  case GSDI_PERSO_RUIM_RUIM:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_RUIM;
    break;
  case GSDI_PERSO_RUIM_HRPD:
    *gsdidiag_perso_feature_ptr = GSDIDIAG_PERSO_RUIM_HRPD;
    break;
#endif /*FEATURE_PERSO_RUIM*/
  default:
    MSG_ERROR("invalid feature: %d", gsdi_perso_feature, 0, 0);
    gsdi_status = GSDI_ERROR;
  }
  return gsdi_status;
} /* gsdidiag_map_perso_feature_rsp */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_activate_feature_indicator_cb

DESCRIPTION
    Callback used for activate feature indicator

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_activate_feature_indicator_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->act_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set return values */
  rsp_ptr->rsp_payload.response.activate_feature_indicator.num_retries = cnf_ptr->act_ind_cnf.num_retries;

  gsdi_status = gsdidiag_map_perso_feature_rsp(cnf_ptr->act_ind_cnf.perso_feature,
                   (gsdidiag_perso_feature_type*)&(rsp_ptr->rsp_payload.response.activate_feature_indicator.perso_feature));
  if (gsdi_status != GSDI_SUCCESS)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_BAD_PARAM);

    /* free the memory */
    gsdidiag_free_index(index);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_activate_feature_indicator_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_activate_feature_indicator

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_activate_feature_indicator API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_activate_feature_indicator (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;
  gsdi_perso_control_key_data_type   control_key_info;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* Zero control_key_info structure */
  memset(&control_key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD;

  /* Map the feature indicator */
  gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->request.activate_feature_indicator_req.perso_feature,
                                           &control_key_info.feature);
  if (gsdi_status == GSDI_SUCCESS)
  {
    control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
    control_key_info.num_bytes       = strlen(req_ptr->request.activate_feature_indicator_req.perso_feature_key);
    control_key_info.control_key_p   = (uint8*)req_ptr->request.activate_feature_indicator_req.perso_feature_key;

    gsdi_status = gsdi_perso_activate_feature_indicator(&control_key_info,
                                                        int32touint32(index),
                                                        (gsdi_async_cb)gsdidiag_delayed_activate_feature_indicator_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_activate_feature_indicator queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_activate_feature_indicator */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_deactivate_feature_indicator_cb

DESCRIPTION
    Callback used for deactivating feature indicator

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_deactivate_feature_indicator_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->dact_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set return values */
  rsp_ptr->rsp_payload.response.deactivate_feature_indicator.num_retries = cnf_ptr->dact_ind_cnf.num_retries;

  gsdi_status = gsdidiag_map_perso_feature_rsp(cnf_ptr->dact_ind_cnf.perso_feature,
                   (gsdidiag_perso_feature_type*)&(rsp_ptr->rsp_payload.response.deactivate_feature_indicator.perso_feature));
  if (gsdi_status != GSDI_SUCCESS)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_BAD_PARAM);

    /* free the memory */
    gsdidiag_free_index(index);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_deactivate_feature_indicator_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_deactivate_feature_indicator

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_deactivate_feature_indicator API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_deactivate_feature_indicator (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;
  gsdi_perso_control_key_data_type   control_key_info;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* Zero control_key_info structure */
  memset(&control_key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD;

  /* Map the feature indicator */
  gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->request.deactivate_feature_indicator_req.perso_feature,
                                           &control_key_info.feature);
  if (gsdi_status == GSDI_SUCCESS)
  {
    control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
    control_key_info.num_bytes       = strlen(req_ptr->request.deactivate_feature_indicator_req.perso_feature_key);
    control_key_info.control_key_p   = (uint8*)req_ptr->request.deactivate_feature_indicator_req.perso_feature_key;

    gsdi_status = gsdi_perso_deactivate_feature_indicator(&control_key_info,
                                                          int32touint32(index),
                                                          (gsdi_async_cb)gsdidiag_delayed_deactivate_feature_indicator_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_deactivate_feature_indicator queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_deactivate_feature_indicator */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indication_cb

DESCRIPTION
    Callback used for getting feature indication

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_get_feature_indication_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->get_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_FEATURE_INDICATION_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_GET_FEATURE_INDICATION_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set UMTS return values */
#ifdef FEATURE_PERSO_SIM
  rsp_ptr->rsp_payload.response.get_feature_indication.umts_locks_present = 1;
  rsp_ptr->rsp_payload.response.get_feature_indication.sim_nw_feature     = booltoint(cnf_ptr->get_ind_cnf.nw_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.sim_ns_feature     = booltoint(cnf_ptr->get_ind_cnf.ns_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.sim_sp_feature     = booltoint(cnf_ptr->get_ind_cnf.sp_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.sim_cp_feature     = booltoint(cnf_ptr->get_ind_cnf.cp_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.sim_sim_feature    = booltoint(cnf_ptr->get_ind_cnf.sim_ind_status);
#endif /* FEATURE_PERSO_SIM */

  /* Set CDMA return values */
#ifdef FEATURE_PERSO_RUIM
  rsp_ptr->rsp_payload.response.get_feature_indication.cdma_locks_present = 1;
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_nw1_feature   = booltoint(cnf_ptr->get_ind_cnf.ruim_nw1_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_nw2_feature   = booltoint(cnf_ptr->get_ind_cnf.ruim_nw2_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_sp_feature    = booltoint(cnf_ptr->get_ind_cnf.ruim_sp_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_cp_feature    = booltoint(cnf_ptr->get_ind_cnf.ruim_cp_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_ruim_feature  = booltoint(cnf_ptr->get_ind_cnf.ruim_ruim_ind_status);
  rsp_ptr->rsp_payload.response.get_feature_indication.ruim_hrpd_feature  = booltoint(cnf_ptr->get_ind_cnf.ruim_hrpd_ind_status);
#endif /* FEATURE_PERSO_RUIM */

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_get_feature_indication_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indication

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_get_feature_indicators API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_feature_indication (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_GET_FEATURE_INDICATION_CMD;

  gsdi_status = gsdi_perso_get_feature_indicators((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                                  int32touint32(index),
                                                  (gsdi_async_cb)gsdidiag_delayed_get_feature_indication_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_feature_indication queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_get_feature_indication */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_set_feature_indicator_data_cb

DESCRIPTION
    Callback used for setting feature indicator data

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_set_feature_indicator_data_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->set_data_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* The response to GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD does
     not contain any specific payload */

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_set_feature_indicator_data_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_set_feature_indicator_data

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_set_feature_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_set_feature_indicator_data (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;
  gsdi_perso_set_feature_data_type   feature_data;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* Zero gsdi_perso_set_feature_data_type structure */
  memset(&feature_data, 0, sizeof(gsdi_perso_set_feature_data_type));

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD;

  gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->request.set_feature_indicator_data_req.perso_feature,
                                           &feature_data.feature);
  if (gsdi_status == GSDI_SUCCESS)
  {
    feature_data.slot         = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
    feature_data.num_bytes    = uint32toint32(req_ptr->request.set_feature_indicator_data_req.perso_feature_data_len);
    feature_data.perso_data_p = (uint8*)req_ptr->request.set_feature_indicator_data_req.perso_feature_data_ptr;

    gsdi_status = gsdi_perso_set_feature_data(&feature_data,
                                              int32touint32(index),
                                              (gsdi_async_cb)gsdidiag_delayed_set_feature_indicator_data_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_set_feature_indicator_data queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_set_feature_indicator_data */


#if defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE) && defined(FEATURE_GSDI_PERSO_GET_DATA)
/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indicator_data_cb

DESCRIPTION
    Callback function executed after the get feature indicator data
    command request execution. Cleans up memory used for this req
    in req_table.

DEPENDENCIES
    APS should be used as transport.

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_delayed_get_feature_indicator_data_cb(
  const gsdi_cnf_T* gsdi_cnf_ptr
)
{
  int                                     index         =  0;
  gsdidiag_generic_cmd_rsp_type           *rsp_ptr      = NULL; /* response pointer */

  if(gsdi_cnf_ptr == NULL)
  {
    MSG_ERROR("gsdi_cnf_ptr ERR:NULL",0,0,0);
    return;
  }

  index = uint32toint32(gsdi_cnf_ptr->get_data_cnf.message_header.client_ref);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type*)diagpkt_subsys_alloc_v2_delay(
              (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
              (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
              req_table[index].transaction_id,
              sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((void *)&rsp_ptr->rsp_payload,0x00,
               sizeof(gsdidiag_rsp_payload_type));

  /* req_table[index] should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use get_feature_data_cb.",
                           index,0,0);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

   /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD  */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =
    GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD;

  if(gsdi_cnf_ptr->get_data_cnf.message_header.gsdi_status == GSDI_SUCCESS)
  {
    /* Populate response pointer with feature data */
    rsp_ptr->rsp_payload.response.get_feature_data.feature_data_len =
      int32touint32(gsdi_cnf_ptr->get_data_cnf.num_bytes);

    MSG_HIGH("gsdidiag_delayed_get_feature_data_cb, feature_data_len=0x%x",
         rsp_ptr->rsp_payload.response.get_feature_data.feature_data_len ,0,0);

    /* if feature_data_len > 0 then malloc and copy the data */
    if ((rsp_ptr->rsp_payload.response.get_feature_data.feature_data_len > 0) &&
        (rsp_ptr->rsp_payload.response.get_feature_data.feature_data_len <
          GSDI_PERSO_MAX_DATA_LEN))
    {
      (void)memcpy(
        rsp_ptr->rsp_payload.response.get_feature_data.feature_data,
        gsdi_cnf_ptr->get_data_cnf.data_buffer_ptr,
        int32touint32(gsdi_cnf_ptr->get_data_cnf.num_bytes));
    }
  }
  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      gsdi_cnf_ptr->get_data_cnf.message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status  = 0;
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      gsdi_cnf_ptr->get_data_cnf.message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      gsdi_cnf_ptr->message_header.gsdi_status;
  }

  /* free index in req_table */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_get_feature_indicator_data_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE && FEATURE_GSDI_PERSO_GET_DATA */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indicator_data

DESCRIPTION
    Get the feature indication for personalization

ARGUMENTS
    PACKED void *req        : pointer to request packet
    word         pkt_len    : length of request packet
    uint32      *status_ptr : Status returned by gsdi_perso_set_feature_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport
DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_delayed_get_feature_indicator_data (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#if !defined(FEATURE_MMGSDI_PERSONALIZATION_ENGINE) || !defined(FEATURE_GSDI_PERSO_GET_DATA)
  (void)aps_as_transport;
  (void)rsp_id;
  (void)packed_req_ptr;
  (void)pkt_len;
  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }

  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;
#else
  gsdidiag_req_payload_type *req_ptr    = (gsdidiag_req_payload_type *)packed_req_ptr;
  int32                     index       = 0;
  gsdi_returns_T            gsdi_status = GSDI_SUCCESS;
  gsdi_perso_enum_type      feature     = GSDI_PERSO_NONE;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  req_table[index].transaction_id   = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag         = 1;
  req_table[index].subcommand       = GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD;

  gsdi_status = gsdidiag_map_perso_feature(
        (gsdidiag_perso_feature_type)req_ptr->request.get_feature_indicator_data_req.perso_feature,
        &feature);

  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("delayed_get_feature_data param chk failed %0x%x",
                           gsdi_status,0,0);
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);
    gsdidiag_free_index(index);
    return;
  }

  gsdi_status = gsdi_perso_get_feature_data(
    feature,
    (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
    index,
    (gsdi_async_cb)gsdidiag_delayed_get_feature_indicator_data_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_set_feature_indicator_data queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);
#endif /* !FEATURE_MMGSDI_PERSONALIZATION_ENGINE || !FEATURE_GSDI_PERSO_GET_DATA */

} /* gsdidiag_delayed_get_feature_indicator_data */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indicator_key_cb

DESCRIPTION
    Callback used for getting feature indicator key

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_get_feature_indicator_key_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->get_ind_key.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand = GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  MSG_HIGH("Key length: %d",cnf_ptr->get_ind_key.key_len,0,0);

  /* Set return values */
  if (cnf_ptr->get_ind_key.message_header.gsdi_status == GSDI_SUCCESS &&
      cnf_ptr->get_ind_key.key_len > 0 &&
      cnf_ptr->get_ind_key.key_len <= GSDI_PERSO_MAX_KEY_IMEI_LEN)
  {
    /* Identify where the ':' is */
    int32 colon = 0;
    for (colon = 0; colon < cnf_ptr->get_ind_key.key_len; colon++)
    {
      if (cnf_ptr->get_ind_key.key[colon] == ':')
      {
        int32 imei_len = colon;
        int32 key_len  = cnf_ptr->get_ind_key.key_len - colon - 1;

        if (imei_len <= GSDI_PERSO_MAX_IMEI_LEN &&
            key_len  <= GSDI_PERSO_MAX_CK)
        {
          rsp_ptr->rsp_payload.response.get_feature_indicator_key.imei_len = imei_len + 1; /* Including NULL */
          rsp_ptr->rsp_payload.response.get_feature_indicator_key.key_len = key_len + 1; /* Including NULL */

          if (imei_len > 0)
          {
            memcpy((uint8 *)rsp_ptr->rsp_payload.response.get_feature_indicator_key.imei,
                   (uint8 *)cnf_ptr->get_ind_key.key,
                   int32touint32(imei_len));
          }
          if (key_len > 0)
          {
            memcpy((uint8 *)rsp_ptr->rsp_payload.response.get_feature_indicator_key.key,
                   (uint8 *)cnf_ptr->get_ind_key.key + imei_len + 1,
                   int32touint32(key_len));
          }

          MSG_HIGH("IMEI = %s, Key = %s",
                   rsp_ptr->rsp_payload.response.get_feature_indicator_key.imei,
                   rsp_ptr->rsp_payload.response.get_feature_indicator_key.key,
                   0);
        }
        break;
      }
    }
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_get_feature_indicator_key_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_feature_indicator_key

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_get_feature_indicator_key API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_feature_indicator_key (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type   *req_ptr        = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T               gsdi_status    = GSDI_ERROR;
  int                          index          = 0;
  gsdi_perso_enum_type         feature_ind    = GSDI_PERSO_NONE;
  boolean                      is_unblock_key = FALSE;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD;

  MSG_HIGH("Perso key: %d",req_ptr->request.get_feature_indicator_key_req.perso_key,0,0);

  gsdi_status = GSDI_SUCCESS;
  switch (req_ptr->request.get_feature_indicator_key_req.perso_key)
  {
#ifdef FEATURE_PERSO_SIM
  case 0x00:
    feature_ind    = GSDI_PERSO_NW;
    is_unblock_key = FALSE;
    break;
  case 0x01:
    feature_ind    = GSDI_PERSO_NS;
    is_unblock_key = FALSE;
    break;
  case 0x02:
    feature_ind    = GSDI_PERSO_SP;
    is_unblock_key = FALSE;
    break;
  case 0x03:
    feature_ind    = GSDI_PERSO_CP;
    is_unblock_key = FALSE;
    break;
  case 0x04:
    feature_ind    = GSDI_PERSO_SIM;
    is_unblock_key = FALSE;
    break;
  case 0x05:
    feature_ind    = GSDI_PERSO_NW;
    is_unblock_key = TRUE;
    break;
  case 0x06:
    feature_ind    = GSDI_PERSO_NS;
    is_unblock_key = TRUE;
    break;
  case 0x07:
    feature_ind    = GSDI_PERSO_SP;
    is_unblock_key = TRUE;
    break;
  case 0x08:
    feature_ind    = GSDI_PERSO_CP;
    is_unblock_key = TRUE;
    break;
  case 0x09:
    feature_ind    = GSDI_PERSO_SIM;
    is_unblock_key = TRUE;
    break;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
  case 0x0A:
    feature_ind    = GSDI_PERSO_RUIM_NW1;
    is_unblock_key = FALSE;
    break;
  case 0x0B:
    feature_ind    = GSDI_PERSO_RUIM_NW2;
    is_unblock_key = FALSE;
    break;
  case 0x0C:
    feature_ind    = GSDI_PERSO_RUIM_SP;
    is_unblock_key = FALSE;
    break;
  case 0x0D:
    feature_ind    = GSDI_PERSO_RUIM_CP;
    is_unblock_key = FALSE;
    break;
  case 0x0E:
    feature_ind    = GSDI_PERSO_RUIM_RUIM;
    is_unblock_key = FALSE;
    break;
  case 0x0F:
    feature_ind    = GSDI_PERSO_RUIM_HRPD;
    is_unblock_key = FALSE;
    break;
  case 0x10:
    feature_ind    = GSDI_PERSO_RUIM_NW1;
    is_unblock_key = TRUE;
    break;
  case 0x11:
    feature_ind    = GSDI_PERSO_RUIM_NW2;
    is_unblock_key = TRUE;
    break;
  case 0x12:
    feature_ind    = GSDI_PERSO_RUIM_SP;
    is_unblock_key = TRUE;
    break;
  case 0x13:
    feature_ind    = GSDI_PERSO_RUIM_CP;
    is_unblock_key = TRUE;
    break;
  case 0x14:
    feature_ind    = GSDI_PERSO_RUIM_RUIM;
    is_unblock_key = TRUE;
    break;
  case 0x15:
    feature_ind    = GSDI_PERSO_RUIM_HRPD;
    is_unblock_key = TRUE;
    break;
#endif /* FEATURE_PERSO_RUIM */
  default:
    gsdi_status = GSDI_ERROR;
    break;
  }

  if (gsdi_status == GSDI_SUCCESS)
  {
    gsdi_status = gsdi_perso_get_key((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                     feature_ind,
                                     is_unblock_key,
                                     int32touint32(index),
                                     (gsdi_async_cb)gsdidiag_delayed_get_feature_indicator_key_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_feature_indicator_key queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_get_feature_indicator_key */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_unblock_feature_ind_cb

DESCRIPTION
    Callback used for unblocking feature indicator

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_unblock_feature_ind_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->unblock_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set return values */
  rsp_ptr->rsp_payload.response.activate_feature_indicator.num_retries =
    cnf_ptr->unblock_ind_cnf.num_retries;

  gsdi_status = gsdidiag_map_perso_feature_rsp(cnf_ptr->unblock_ind_cnf.perso_feature,
                   (gsdidiag_perso_feature_type*)&(rsp_ptr->rsp_payload.response.activate_feature_indicator.perso_feature));
  if (gsdi_status != GSDI_SUCCESS)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_BAD_PARAM);

    /* free the memory */
    gsdidiag_free_index(index);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_unblock_feature_ind_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_unblock_feature_ind

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_unblock_feature_indicator API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_unblock_feature_ind (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;
  gsdi_perso_control_key_data_type   control_key_info;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* Zero control_key_info structure */
  memset(&control_key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD;

  /* Map the feature indicator */
  gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->request.unblock_feature_indicator_req.perso_feature,
                                           &control_key_info.feature);
  if (gsdi_status == GSDI_SUCCESS)
  {
    control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
    control_key_info.num_bytes       = strlen(req_ptr->request.unblock_feature_indicator_req.perso_feature_key);
    control_key_info.control_key_p   = (uint8*)req_ptr->request.unblock_feature_indicator_req.perso_feature_key;

    gsdi_status = gsdi_perso_unblock_feature_indicator(&control_key_info,
                                                       int32touint32(index),
                                                       (gsdi_async_cb)gsdidiag_delayed_unblock_feature_ind_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_unblock_feature_ind queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_unblock_feature_ind */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_get_perm_feature_indication_cb

DESCRIPTION
    Callback used for getting permanent feature indication

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_get_perm_feature_indication_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->get_perm_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand = GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set UMTS return values */
#ifdef FEATURE_PERSO_SIM
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.umts_locks_present = 1;
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.sim_nw_feature     = booltoint(cnf_ptr->get_perm_ind_cnf.perm_nw_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.sim_ns_feature     = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ns_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.sim_sp_feature     = booltoint(cnf_ptr->get_perm_ind_cnf.perm_sp_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.sim_cp_feature     = booltoint(cnf_ptr->get_perm_ind_cnf.perm_cp_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.sim_sim_feature    = booltoint(cnf_ptr->get_perm_ind_cnf.perm_sim_ind_status);
#endif /* FEATURE_PERSO_SIM */

  /* Set CDMA return values */
#ifdef FEATURE_PERSO_RUIM
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.cdma_locks_present = 1;
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_nw1_feature   = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_nw1_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_nw2_feature   = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_nw2_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_sp_feature    = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_sp_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_cp_feature    = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_cp_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_ruim_feature  = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_ruim_ind_status);
  rsp_ptr->rsp_payload.response.get_perm_feature_indication.ruim_hrpd_feature  = booltoint(cnf_ptr->get_perm_ind_cnf.perm_ruim_hrpd_ind_status);
#endif /* FEATURE_PERSO_RUIM */

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_get_perm_feature_indication_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_get_perm_feature_indication

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_get_perm_feature_indicators API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_get_perm_feature_indication (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD;

  gsdi_status = gsdi_perso_get_perm_feature_indicators((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                                         int32touint32(index),
                                                         (gsdi_async_cb)gsdidiag_delayed_get_perm_feature_indication_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_get_perm_feature_indication queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_get_perm_feature_indication */


#ifdef FEATURE_MMGSDI_PERSONALIZATION_ENGINE
/*===========================================================================
FUNCTION gsdidiag_delayed_disable_perm_feature_indicator_cb

DESCRIPTION
    Callback used for permanently disabling feature indicator

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_disable_perm_feature_indicator_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdi_returns_T                   gsdi_status = GSDI_ERROR;
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->disable_ind_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set return values */
  rsp_ptr->rsp_payload.response.perm_disable_feature_indicator.num_retries = cnf_ptr->disable_ind_cnf.num_retries;

  gsdi_status = gsdidiag_map_perso_feature_rsp(cnf_ptr->disable_ind_cnf.perso_feature,
                   (gsdidiag_perso_feature_type*)&(rsp_ptr->rsp_payload.response.perm_disable_feature_indicator.perso_feature));
  if (gsdi_status != GSDI_SUCCESS)
  {
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_BAD_PARAM);

    /* free the memory */
    gsdidiag_free_index(index);

    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_disable_perm_feature_indicator_cb */
#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */


/*===========================================================================
FUNCTION gsdidiag_delayed_disable_perm_feature_indicator

DESCRIPTION
    Function to activate perso feature indicator
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_perso_perm_disable_feature_indicator API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_disable_perm_feature_indicator (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
#ifndef FEATURE_MMGSDI_PERSONALIZATION_ENGINE

  if (status_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("status pointer is NULL", 0, 0, 0);
    return;
  }
  *status_ptr = (uint32)GSDI_NOT_SUPPORTED;

#else

  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;
  gsdi_perso_control_key_data_type   control_key_info;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* Zero control_key_info structure */
  memset(&control_key_info, 0, sizeof(gsdi_perso_control_key_data_type));

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD;

  /* Map the feature indicator */
  gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->request.perm_disable_feature_indicator_req.perso_feature,
                                           &control_key_info.feature);
  if (gsdi_status == GSDI_SUCCESS)
  {
    control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id;
    control_key_info.num_bytes       = strlen(req_ptr->request.perm_disable_feature_indicator_req.perso_feature_key);
    control_key_info.control_key_p   = (uint8*)req_ptr->request.perm_disable_feature_indicator_req.perso_feature_key;

    gsdi_status = gsdi_perso_perm_disable_feature_indicator(&control_key_info,
                                                            int32touint32(index),
                                                            (gsdi_async_cb)gsdidiag_delayed_disable_perm_feature_indicator_cb);
  }

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_disable_perm_feature_indicator queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);

#endif /* FEATURE_MMGSDI_PERSONALIZATION_ENGINE */
} /* gsdidiag_delayed_disable_perm_feature_indicator */


/*===========================================================================
FUNCTION gsdidiag_delayed_sim_get_cphs_information_cb

DESCRIPTION
    Callback used for getting cphs information

DEPENDENCIES
    None

INPUT
    None

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information.
===========================================================================*/
static void gsdidiag_delayed_sim_get_cphs_information_cb (
  const gsdi_cnf_T* cnf_ptr)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer    */
  int                              index   = 0;    /* index into req_table*/

  /* Validate Input Paramaters */
  if (cnf_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("cnf ptr is null, unable to response back",0,0,0);
    return;
  }

  index = uint32toint32(cnf_ptr->get_cphs_info_cnf.message_header.client_ref);

  if(gsdidiag_is_index_invalid(index))
  {
    MMGSDI_DEBUG_MSG_ERROR("index returned is out of range 0x%x",index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               sizeof( gsdidiag_generic_cmd_rsp_type ) );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload, 0x00, sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use.", index, 0, 0);
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);
    diagpkt_delay_commit(rsp_ptr);
    return;
  }

  /* Set return values */
  rsp_ptr->rsp_payload.response.get_cphs_info.capable =
    (uint8)cnf_ptr->get_cphs_info_cnf.cphs_capable;
  rsp_ptr->rsp_payload.response.get_cphs_info.phase =
    (uint8)cnf_ptr->get_cphs_info_cnf.services.phase;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp =
   (uint8)cnf_ptr->get_cphs_info_cnf.services.csp;
  rsp_ptr->rsp_payload.response.get_cphs_info.sst =
   (uint8)cnf_ptr->get_cphs_info_cnf.services.sst;
  rsp_ptr->rsp_payload.response.get_cphs_info.mbn =
   (uint8)cnf_ptr->get_cphs_info_cnf.services.mbn;
  rsp_ptr->rsp_payload.response.get_cphs_info.ops =
   (uint8)cnf_ptr->get_cphs_info_cnf.services.ops;
  rsp_ptr->rsp_payload.response.get_cphs_info.ifn =
   (uint8)cnf_ptr->get_cphs_info_cnf.services.ifn;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_offering_cfu =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_offering_CFU;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_offering_cfb =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_offering_CFB;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_offering_cfnry =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_offering_CFNRy;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_offering_cfnrc =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_offering_CFNRc;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_offering_ct =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_offering_CT;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_restrict_boac =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_restrict_BOAC;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_restrict_boic =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_restrict_BOIC;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_restrict_boic_exhc =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_restrict_BOIC_exHC;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_restrict_baic =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_restrict_BAIC;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_restrict_bic_roam =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_restrict_BIC_Roam;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_other_supps_mpty =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.other_supps_MPTY;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_other_supps_cug =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.other_supps_CUG;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_other_supps_aoc =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.other_supps_AoC;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_other_supps_pref_cug =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.other_supps_Pref_CUG;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_other_supps_cug_oa =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.other_supps_CUG_OA;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_compl_hold =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_completion_HOLD;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_compl_cw =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_completion_CW;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_compl_ccbs =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_completion_CCBS;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_call_compl_user_user_sig =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.call_completion_User_User_Signalling;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_sm_mt =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_SM_MT;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_sm_mo =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_SM_MO;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_sm_cb =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_SM_CB;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_reply_path =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_Reply_Path;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_del_conf =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_Del_Conf;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_protocol_id =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_Protocol_ID;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_teleservices_validity_period =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.teleservices_Validity_Period;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_cphs_teleservices_als =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.cphs_teleservices_als;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_cphs_features_sst =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.cphs_features_SST;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_clip =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_CLIP;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_colr =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_CoLR;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_colp =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_CoLP;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_mci =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_MCI;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_cli_send =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_CLI_send;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_number_identify_cli_block =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.number_identify_CLI_block;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_gprs =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_GPRS;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_hscsd =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_HSCSD;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_voice_grp_call =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_Voice_Group_call;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_voice_bcast_svc =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_Voice_Broadcast_Svc;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_muli_sub_prof =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_Muli_Sub_Profile;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_p2plus_srv_multiband =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.p2plus_services_Multiband;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_plmn_mode =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_PLMN_MODE;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_vps =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_VPS;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_sm_mo_paging =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_SM_MO_PAGING;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_sm_mo_email =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_SM_MO_EMAIL;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_fax =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_FAX;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_data =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.value_added_DATA;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_va_language =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.valued_added_Language;
  rsp_ptr->rsp_payload.response.get_cphs_info.csp_information_numbers =
    (uint8)cnf_ptr->get_cphs_info_cnf.csp.information_numbers;

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
        cnf_ptr->message_header.gsdi_status;
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(
      cnf_ptr->message_header.gsdi_status, &temp_status);

    diagpkt_subsys_set_status(rsp_ptr,temp_status);
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status =
      cnf_ptr->message_header.gsdi_status;
  }

  /* free the memory */
  gsdidiag_free_index(index);

  /* commit diag pkt */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_sim_get_cphs_information_cb */


/*===========================================================================
FUNCTION gsdidiag_delayed_sim_get_cphs_information

DESCRIPTION
    Function to get information on cphs
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *packed_req_ptr : pointer to request packet of type
    word pkt_len                : length of request packet
    uint32 * status_ptr         : Status returned by gsdi_sim_get_cphs_information API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status_ptr: GSDI_SUCCESS
               GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_sim_get_cphs_information (
  PACKED void                        *packed_req_ptr,  /* pointer to request packet  */
  word                                pkt_len,         /* length of request packet */
  uint32                             *status_ptr,      /* Status returned by gsdi API*/
  diagpkt_subsys_delayed_rsp_id_type  rsp_id,          /* Response Id generated by diag*/
  uint32                              aps_as_transport /* AS transport */
)
{
  gsdidiag_req_payload_type         *req_ptr     = (gsdidiag_req_payload_type *)packed_req_ptr;
  gsdi_returns_T                     gsdi_status = GSDI_ERROR;
  int                                index       = 0;

  if (status_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
    return;
  }

  if ( req_ptr == NULL )
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr Null", 0, 0, 0);
    *status_ptr = GSDIDIAG_BAD_PARAM;
    return;
  }

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();

  if (index == GSDIDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    *status_ptr = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    return;
  }

  /* get a transaction id */
  req_table[index].transaction_id = rsp_id;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].use_flag = 1;
  req_table[index].subcommand = GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD;

  gsdi_status = gsdi_sim_get_cphs_information(
                     (gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                     int32touint32(index),
                     (gsdi_async_cb)gsdidiag_delayed_sim_get_cphs_information_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_delayed_sim_get_cphs_information queue failed %d",
                           gsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status_ptr);
} /* gsdidiag_delayed_sim_get_cphs_information */


/*===========================================================================
FUNCTION gsdidiag_delayed_naa_refresh_complete_cb

DESCRIPTION
    Callback used for a naa refresh complete call.

DEPENDENCIES
    None

INPUT
    gsdi_cnf_T* sim_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and the payload
   information. The payload stores the
===========================================================================*/
static void gsdidiag_delayed_naa_refresh_complete_cb(
  mmgsdi_return_enum_type       status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr)
{

  gsdidiag_generic_cmd_rsp_type  * rsp_ptr = NULL; /* response pointer */
  uint32 len; /* length of event */
  int index; /* index into req_table*/

  if(cnf_ptr == NULL || cnf!= MMGSDI_REFRESH_CNF)
  {
    MSG_ERROR("cnf_ptr ERR:NULL",0,0,0);
    return;
  }

  len   = sizeof( gsdidiag_generic_cmd_rsp_type );

  index = uint32toint32(cnf_ptr->refresh_cnf.response_header.client_data);

  /* Allocate memory for the response pointer */
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_REFRESH_COMPLETE_CMD,
               req_table[index].transaction_id,
               len );
  if(rsp_ptr == NULL)
  {
      MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
      gsdidiag_free_index(index);
      return;
  }

  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));


  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /* subcommand is set to GSDIDIAG_REFRESH_COMPLETE_CMD */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand =  GSDIDIAG_REFRESH_COMPLETE_CMD;

  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
      MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_delayed_refresh_cb.",index,0,0);

      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_MEMORY_ERR);

      diagpkt_delay_commit(rsp_ptr);
      return;
  }

  if (req_table[index].aps_as_transport)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status =
      status;
  }
  else
  {
    uint32 temp_status;

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(
      status,
      &temp_status);

    diagpkt_subsys_set_status(rsp_ptr, temp_status);

    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status =
      status;
  }

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
  return;
}


/*===========================================================================
FUNCTION gsdidiag_delayed_naa_refresh_complete

DESCRIPTION
    Function to send refresh complete to the  sim. A task is queued to gsdi
    task to notify refresh complete.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req       : pointer to request packet of type
    word pkt_len           : length of request packet
    uint32 * status        : Status returned by gsdi_sim_open_channel API
    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_naa_refresh_complete (
    PACKED void *req,       /* pointer to request packet  */
    word pkt_len,           /* length of request packet */
    uint32 * status,        /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id, /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    /* Need to handle the MMGSDI REFRESH event before this function
       can be called*/
    gsdidiag_req_payload_type  *req_ptr           = NULL;
    int32                       index             = 0;
    gsdi_app_id_type            app_id;
    uim_items_enum_type        *uim_file_list_ptr = NULL;
    uint32                      i                 = 0;
    mmgsdi_option_type          option            = {0};
    mmgsdi_return_enum_type     mmgsdi_status     = MMGSDI_SUCCESS;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr     = (gsdidiag_req_payload_type*)req;
    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR)
    {
      /*  req_table is full */
      *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
      gsdidiag_free_index( index );
      return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand       = GSDIDIAG_REFRESH_COMPLETE_CMD;

    app_id.aid_len = uint32touint8(req_ptr->sub_cmd_hdr.sim_app_id_len);
    if (app_id.aid_len > GSDI_MAX_APP_ID_LEN)
    {
      MSG_ERROR("Exceed Max App ID Len 0x%x", app_id.aid_len, 0, 0);
      app_id.aid_len = GSDI_MAX_APP_ID_LEN;
    }
    if (app_id.aid_len > 0)
    {
      memcpy(app_id.aid_data, (uint8*)req_ptr->sub_cmd_hdr.sim_app_id, app_id.aid_len);
    }
    if (req_ptr->request.refresh_complete_req.num_files > 0)
    {
      uim_file_list_ptr = (uim_items_enum_type*)mmgsdi_malloc(
                           sizeof(uim_items_enum_type)*req_ptr->request.refresh_complete_req.num_files);
      if (uim_file_list_ptr == NULL)
      {
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        gsdidiag_free_index( index );
        return;
      }
      for (i = 0; i < req_ptr->request.refresh_complete_req.num_files; i++)
      {
        *uim_file_list_ptr = gsdidiag_convert_file_to_uim_enum(
                             req_ptr->request.refresh_complete_req.file_list_ptr[i]);
      }
    }
    /* send the request to do a mmgsdi_naa_refresh_complete*/
    mmgsdi_status = mmgsdi_refresh_complete(
                    (mmgsdi_client_id_type)req_ptr->request.refresh_complete_req.client_id,
                    MMGSDI_SLOT_1,
                    (boolean)req_ptr->request.refresh_complete_req.pass_fail,
                    option,
                    (mmgsdi_callback_type)gsdidiag_delayed_naa_refresh_complete_cb,
                    int32touint32(index));


    if (mmgsdi_status != MMGSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("mmgsdi_naa_refresh_complete queue failed %d",status,0,0);
      gsdidiag_free_index(index);
    }

    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status, status);
} /* gsdidiag_delayed_naa_refresh_complete */


/*===========================================================================
FUNCTION gsdidiag_get_ecc

DESCRIPTION
    Sends the channel data. An send channel data command is queued to gsdi task.

INPUT:
    PACKED void *req,       - Pointer to request packet
    word pkt_len,           - Size of the requets packet
    uint32 * status,        - Status returned by gsdi_sim_send_channel_data API
    diagpkt_subsys_delayed_rsp_id_type rsp_id, - Response Id generated by diag
    uint32 aps_as_transport

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
    uint32 * status,
    uint16 * rsp_id
===========================================================================*/
static void gsdidiag_get_ecc (
    PACKED void *req,   /* pointer to request packet  */
    word pkt_len,       /* length of request packet */
    uint32 * status,    /* Status returned by gsdi_sim_send_channel_data*/
    diagpkt_subsys_delayed_rsp_id_type rsp_id,    /* Response Id generated by diag*/
    uint32 aps_as_transport
)
{
    gsdidiag_req_payload_type       *req_ptr;
    int                              index;
    gsdi_returns_T                   gsdi_status;

    if (status == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("status Null", 0, 0, 0);
      return;
    }

    if ( req == NULL )
    {
      MMGSDI_DEBUG_MSG_ERROR("req Null", 0, 0, 0);
      *status = GSDIDIAG_BAD_PARAM;
      return;
    }

    req_ptr = (gsdidiag_req_payload_type*)req;

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();

    if (index == GSDIDIAG_GET_FN_ERR )
    {
         /*  req_table is full */
        *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        return;
    }

    /* get a transaction id */
    req_table[index].transaction_id   = rsp_id;
    req_table[index].aps_as_transport = aps_as_transport;
    req_table[index].subcommand = GSDIDIAG_GET_ECC_CMD;

    /* send the request to get ecc to gsdi */
    gsdi_status = gsdi_get_ecc ((gsdi_slot_id_type)req_ptr->sub_cmd_hdr.slot_id,
                                int32touint32(index),
                                (gsdi_async_cb)gsdidiag_get_ecc_cb
                               );

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdi_get_ecc queue failed %d",status,0,0);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, status);
} /* gsdidiag_get_ecc */


#ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
#endif /* FEATURE_MMGDSI_ONCHIP_SIM */


 /*===========================================================================
FUNCTION gsdidiag_generic_command

DESCRIPTION
    Calls the appropriate GSDIDIAG function based on the subcommand code in
    the header of the request packet.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_generic_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
  Response pointer of type gsdidiag_generic_cmd_rsp_type

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_generic_command (
  PACKED void *req,  /* pointer to request packet  */
  word pkt_len       /* length of request packet   */
)
{
  gsdidiag_generic_cmd_req_type request          = {{0}};
  gsdidiag_generic_cmd_rsp_type *rsp_ptr         = NULL;
  gsdidiag_commands_enum_type   subcommand       = GSDIDIAG_MAX_CMD; /* represents which command to call */
  uint32                        status           = GSDIDIAG_SUCCESS; /* status of gsdidiag cmd */
  uint32                        rsp_len          = 0; /* size of gsdidiag_generic_cmd_rsp_type */
  uint32                        aps_as_transport = 0; /* flag for testing */
  mmgsdi_return_enum_type       mmgsdi_status    = MMGSDI_ERROR;
  gsdi_returns_T                gsdi_status      = GSDI_ERROR;
  diagpkt_subsys_delayed_rsp_id_type rsp_id      = 0;
  boolean                       is_gsdi_rsp      = FALSE;

  /* allocate the response pointer */
  rsp_len = sizeof(gsdidiag_generic_cmd_rsp_type);

  rsp_ptr = (gsdidiag_generic_cmd_rsp_type *)diagpkt_subsys_alloc_v2((int) DIAG_SUBSYS_GSDI,
                                    (int) GSDIDIAG_GENERIC_CMD,
                                    rsp_len );

  (void)memset ( (char*)&request, 0x00, sizeof(gsdidiag_generic_cmd_req_type) );

  if (rsp_ptr==NULL)
  {
    return NULL;
  }

  rsp_id = diagpkt_subsys_get_delayed_rsp_id(rsp_ptr);

  /* The diag_pkt_subsys_alloc_v2 already creates a response id for us
   * extract the information for it to be stored in our request table
   * for use at the time of the delayed response
   */

  (void)memset ( (char*)(&rsp_ptr->rsp_payload), 0x00, sizeof(gsdidiag_rsp_payload_type) );


  /* extract data from req packet and populate fields in the request structure */
  gsdidiag_get_generic_cmd_info(&request, req);

  /* extract subcommand to see which gsdidiag command is to be called */
  subcommand = request.req_payload.sub_cmd_hdr.subcommand;

  aps_as_transport = request.req_payload.sub_cmd_hdr.aps_as_transport;

  /* Call the appropriate gsdidiag function based on subcommand value. The assumption
     is that the individual function used to process the subcommand will generate a
     valid SUCCESS/FAIL. The subcommand will populate these values. */
  switch (subcommand)
  {
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_REFRESH_FCN_REG_CMD:
        gsdidiag_delayed_refresh_fcn_reg(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        /* Free the memory for the file list allocated in the request pointer */
        if(request.req_payload.request.refresh_fcn_reg_req.file_list_ptr != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.refresh_fcn_reg_req.file_list_ptr);
          request.req_payload.request.refresh_fcn_reg_req.file_list_ptr = NULL;
        }
        break;
    case GSDIDIAG_GET_SIM_CAPABILITIES_CMD:
        gsdidiag_delayed_get_sim_cap(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_DISABLE_BDN_CMD:
    case GSDIDIAG_ENABLE_BDN_CMD:
        gsdidiag_delayed_process_bdn(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_DISABLE_ACL_CMD:
    case GSDIDIAG_ENABLE_ACL_CMD:
        gsdidiag_delayed_process_acl(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_OTA_DEPERSO_CMD:
        gsdidiag_delayed_ota_deperso(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        /* Free the memory for the apdu buffer allocated in the request pointer */
        if(request.req_payload.request.ota_req.data_buffer_p != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.ota_req.data_buffer_p);
          request.req_payload.request.ota_req.data_buffer_p = NULL;
        }
        break;
    case GSDIDIAG_RUN_GSM_ALGO_CMD:
        gsdidiag_delayed_run_gsm_algo(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);

        /* Free the memory for the search string allocated in the request pointer */
        if(request.req_payload.request.run_gsm_algo_req.rand_ptr != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.run_gsm_algo_req.rand_ptr);
          request.req_payload.request.run_gsm_algo_req.rand_ptr = NULL;
        }
        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case MMGSDIDIAG_WRITE_TRANSPARENT_CMD:
    case MMGSDIDIAG_WRITE_TRANSPARENT_EXT_CMD:
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_WRITE_TRANSPARENT_CMD:
        gsdidiag_delayed_write_transparent(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);

        /* Free the memory for the search string allocated in the request pointer */
        if(request.req_payload.request.write_transparent_req.write_data.data_ptr != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.write_transparent_req.write_data.data_ptr);
          request.req_payload.request.write_transparent_req.write_data.data_ptr = NULL;
        }

        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case MMGSDIDIAG_WRITE_RECORD_CMD:
      case MMGSDIDIAG_WRITE_RECORD_EXT_CMD:
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case GSDIDIAG_WRITE_RECORD_CMD:
        gsdidiag_delayed_write_record(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        /* Free the memory for the search string allocated in the request pointer */
        if(request.req_payload.request.write_record_req.write_data.data_ptr != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE(request.req_payload.request.write_record_req.write_data.data_ptr);
          request.req_payload.request.write_record_req.write_data.data_ptr = NULL;
        }

        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case MMGSDIDIAG_READ_TRANSPARENT_CMD:
      case MMGSDIDIAG_READ_TRANSPARENT_EXT_CMD:
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case GSDIDIAG_READ_TRANSPARENT_CMD:
        gsdidiag_delayed_read_transparent(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);

        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case MMGSDIDIAG_READ_RECORD_CMD:
      case MMGSDIDIAG_READ_RECORD_EXT_CMD:
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
      case GSDIDIAG_READ_RECORD_CMD:
        gsdidiag_delayed_read_record(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
      case GSDIDIAG_CLIENT_ID_DEREG_CMD:
        gsdidiag_delayed_client_id_dereg(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
     case GSDIDIAG_CLIENT_ID_REG_CMD:
        gsdidiag_delayed_client_id_reg(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
#endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
     case MMGSDIDIAG_GET_FILE_ATTR_CMD:
#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
     case GSDIDIAG_GET_FILE_ATTR_CMD:
        gsdidiag_delayed_get_file_attr(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
    case GSDIDIAG_CHANGE_PIN_CMD:
        gsdidiag_change_pin(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_SIM_INCREASE_CMD:
        gsdidiag_delayed_sim_increase(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_GET_DCK_NUM_RETRIES_CMD:
        gsdidiag_delayed_get_dck_num_retries(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_VERIFY_PIN_CMD:
        gsdidiag_delayed_verify_pin(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
    case GSDIDIAG_FORCE_SIM_READ_CMD:
        gsdidiag_delayed_force_sim_read(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
        break;
#endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
#ifdef FEATURE_MMGSDI_CHANNEL_MANAGEMENT
    case GSDIDIAG_GET_ATR_CMD:
        gsdidiag_get_atr (&request.req_payload,
                         sizeof(gsdidiag_req_payload_type),
                         &status,
                         rsp_id,
                         aps_as_transport);
        break;
    case GSDIDIAG_SIM_OPEN_CHANNEL_CMD:
        gsdidiag_open_channel(&request.req_payload,
                              sizeof(gsdidiag_req_payload_type),
                              &status,
                              rsp_id,
                              aps_as_transport);
        break;
    case GSDIDIAG_SIM_CLOSE_CHANNEL_CMD:
        gsdidiag_close_channel(&request.req_payload,
                              sizeof(gsdidiag_req_payload_type),
                              &status,
                              rsp_id,
                              aps_as_transport);
        break;
    case GSDIDIAG_SIM_SEND_CHANNEL_DATA_CMD:
        gsdidiag_send_channel_data(&request.req_payload,
                              sizeof(gsdidiag_req_payload_type),
                              &status,
                              rsp_id,
                              aps_as_transport);

        /* Free the memory for the apdu buffer allocated in the request pointer */
        if(request.req_payload.request.channel_req.apdu_buffer_p != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.channel_req.apdu_buffer_p);
          request.req_payload.request.channel_req.apdu_buffer_p = NULL;
        }
        break;
     #endif /* FEATURE_MMGSDI_CHANNEL_MANAGEMENT */
     #if (defined(FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM))
     case GSDIDIAG_ATCSIM_RAW_APDU_CMD:
        gsdidiag_atcsim_raw_apdu(&request.req_payload,
                              sizeof(gsdidiag_req_payload_type),
                              &status,
                              rsp_id,
                              aps_as_transport);

        /* Free the memory for the apdu buffer allocated in the request pointer */
        if(request.req_payload.request.channel_req.apdu_buffer_p != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.channel_req.apdu_buffer_p);
          request.req_payload.request.channel_req.apdu_buffer_p = NULL;
        }
        break;
     #endif /* (FEATURE_MMGSDI_CHANNEL_MANAGEMENT) || defined(FEATURE_MMGSDI_ATCSIM) */
     #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
     case GSDIDIAG_SIM_READ_CMD:
        gsdidiag_delayed_sim_read(&request.req_payload,
                          sizeof(gsdidiag_req_payload_type),
                          &status,
                          rsp_id,
                          aps_as_transport);

        /* Free the memory for the search string allocated in the request pointer */
        if(request.req_payload.request.sim_read_req.search_string != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.sim_read_req.search_string);
          request.req_payload.request.sim_read_req.search_string = NULL;
        }

        break;
    case GSDIDIAG_SIM_READ_X_CMD:
      gsdidiag_delayed_sim_read_x(&request.req_payload,
                          sizeof(gsdidiag_req_payload_type),
                          &status,
                          rsp_id,
                          aps_as_transport);


        break;
    case GSDIDIAG_SIM_REFRESH_CMD:
      gsdidiag_delayed_refresh(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
      /* Free the memory for the file path allocated in the request pointer */
      if(request.req_payload.request.refresh_req.file_path_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.refresh_req.file_path_ptr);
        request.req_payload.request.refresh_req.file_path_ptr = NULL;
      }
      break;
    case GSDIDIAG_REFRESH_COMPLETE_CMD:
      gsdidiag_delayed_naa_refresh_complete(&request.req_payload,
                                            sizeof(gsdidiag_req_payload_type),
                                            &status,
                                            rsp_id,
                                            aps_as_transport);
      /* Free the memory for the file path allocated in the request pointer */
      if(request.req_payload.request.refresh_complete_req.file_list_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.refresh_complete_req.file_list_ptr);
        request.req_payload.request.refresh_complete_req.file_list_ptr = NULL;
      }
      break;
    case GSDIDIAG_SIM_USIM_RUIM_SEARCH_CMD:
      gsdidiag_sim_usim_ruim_all_search (&request.req_payload,
                                         sizeof(gsdidiag_req_payload_type),
                                         &status,
                                         rsp_id,
                                         aps_as_transport);
      /* Free the memory for the search string allocated in the request pointer */
      if(request.req_payload.request.sim_usim_ruim_search_req.search_string != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.sim_usim_ruim_search_req.search_string);
        request.req_payload.request.sim_usim_ruim_search_req.search_string = NULL;
      }
      break;

    case GSDIDIAG_GET_ECC_CMD:
      gsdidiag_get_ecc (&request.req_payload,
                        sizeof(gsdidiag_req_payload_type),
                        &status,
                        rsp_id,
                        aps_as_transport);
      break;

    case GSDIDIAG_SECURE_DEVICE_INFO_CMD:
      gsdidiag_delayed_secure_device_info(&request.req_payload,
                        sizeof(gsdidiag_req_payload_type),
                        &status,
                        rsp_id,
                        aps_as_transport);
      break;
    case GSDIDIAG_SIM_SECURE_STORE_CMD:
      gsdidiag_delayed_sim_secure_store(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      if(request.req_payload.request.sim_secure_store_req.data_buffer_p != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.sim_secure_store_req.data_buffer_p);
        request.req_payload.request.sim_secure_store_req.data_buffer_p = NULL;
      }
      break;
    case GSDIDIAG_SIM_SECURE_READ_CMD:
      gsdidiag_delayed_sim_secure_read(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      break;
    case GSDIDIAG_ROOT_KEY_WRITE_CMD:
      gsdidiag_delayed_root_key_write(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
      if(request.req_payload.request.root_key_write_req.data_buffer_p != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.root_key_write_req.data_buffer_p);
        request.req_payload.request.root_key_write_req.data_buffer_p = NULL;
      }
      break;
    case GSDIDIAG_ROOT_KEY_READ_CMD:
      gsdidiag_delayed_root_key_read(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
      break;
    case GSDIDIAG_VERIFY_PHONE_CODE_CMD:
      gsdidiag_delayed_verify_phone_code(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      if(request.req_payload.request.verify_phone_code_req.phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.verify_phone_code_req.phone_code_ptr);
        request.req_payload.request.verify_phone_code_req.phone_code_ptr = NULL;
      }
      break;
    case GSDIDIAG_CHANGE_PHONE_CODE_CMD:
      gsdidiag_delayed_change_phone_code(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      if(request.req_payload.request.change_phone_code_req.old_phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.change_phone_code_req.old_phone_code_ptr);
        request.req_payload.request.change_phone_code_req.old_phone_code_ptr = NULL;
      }
      if(request.req_payload.request.change_phone_code_req.new_phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.change_phone_code_req.new_phone_code_ptr);
        request.req_payload.request.change_phone_code_req.new_phone_code_ptr = NULL;
      }
      break;
    case GSDIDIAG_UNBLOCK_PHONE_CODE_CMD:
      gsdidiag_delayed_unblock_phone_code(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      if(request.req_payload.request.unblock_phone_code_req.master_key_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.unblock_phone_code_req.master_key_ptr);
        request.req_payload.request.unblock_phone_code_req.master_key_ptr = NULL;
      }
      if(request.req_payload.request.unblock_phone_code_req.new_phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.unblock_phone_code_req.new_phone_code_ptr);
        request.req_payload.request.unblock_phone_code_req.new_phone_code_ptr = NULL;
      }
      break;
    case GSDIDIAG_SET_PHONE_CODE_CMD:
      gsdidiag_delayed_set_phone_code(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      if(request.req_payload.request.set_phone_code_req.phone_code_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.set_phone_code_req.phone_code_ptr);
        request.req_payload.request.set_phone_code_req.phone_code_ptr = NULL;
      }
      break;
    case GSDIDIAG_GET_LIB_VERSION_CMD:
      gsdidiag_delayed_get_lib_version(&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);
      break;

    case GSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD:
      gsdidiag_activate_onchip_sim(&request.req_payload,
                                    sizeof(gsdidiag_req_payload_type),
                                    &status,
                                    rsp_id,
                                    aps_as_transport);
       break;

    case MMGSDIDIAG_ACTIVATE_ONCHIP_SIM_CMD:
      #ifdef FEATURE_MMGSDI_ONCHIP_SIM
#error code not present
      #else
      status = GSDIDIAG_NOT_SUPPORTED;
      #endif /* FEATURE_MMGSDI_ONCHIP_SIM */
      break;

    case MMGSDIDIAG_SELECT_AID_CMD:
      gsdidiag_delayed_select_aid(&request,
                                  sizeof(gsdidiag_req_payload_type),
                                  &status,
                                  rsp_id,
                                  aps_as_transport);
      break;

    case MMGSDIDIAG_VERIFY_PIN_CMD:
    case MMGSDIDIAG_ENABLE_PIN_CMD:
    case MMGSDIDIAG_DISABLE_PIN_CMD:
    case MMGSDIDIAG_CHANGE_PIN_CMD:
    case MMGSDIDIAG_UNBLOCK_PIN_CMD:
    case MMGSDIDIAG_DISABLE_PIN_EXT_CMD:
      gsdidiag_delayed_pin_op(&request,
                              sizeof(gsdidiag_req_payload_type),
                              &status,
                              rsp_id,
                              aps_as_transport);
      if((request.req_payload.request.pin_op_req.pin.data_len > 0) &&
         (request.req_payload.request.pin_op_req.pin.data_ptr != NULL))
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.pin_op_req.pin.data_ptr);
        request.req_payload.request.pin_op_req.pin.data_ptr = NULL;
      }
      if((request.req_payload.request.pin_op_req.additional_pin.data_len > 0) &&
         (request.req_payload.request.pin_op_req.additional_pin.data_ptr != NULL))
      {
        MMGSDIUTIL_TMC_MEM_FREE( request.req_payload.request.pin_op_req.additional_pin.data_ptr);
        request.req_payload.request.pin_op_req.additional_pin.data_ptr = NULL;
      }
      break;

    case MMGSDIDIAG_GET_ALL_PIN_STATUS_CMD:
      gsdidiag_delayed_get_all_pin_status(&request,
                                  sizeof(gsdidiag_req_payload_type),
                                  &status,
                                  rsp_id,
                                  aps_as_transport);
      break;

    case GSDIDIAG_CHECK_SERVICE_AVAILABLE_CMD:
      gsdidiag_delayed_service_check(&request,
                                     sizeof(gsdidiag_req_payload_type),
                                     &status,
                                     rsp_id,
                                     aps_as_transport);
      break;

    case GSDIDIAG_UIM_CMD:
      /* Populate uim_cmd and send it down.  Any memory allocated
       * to that pointers would be cleaned in the call back
       */
      gsdidiag_send_cmd_to_uim (&request.req_payload,
                                        sizeof(gsdidiag_req_payload_type),
                                        &status,
                                        rsp_id,
                                        aps_as_transport);

      /* Free any memory that might have been allocated while parsing
       * the request to store into the local command pointer
       */
      gsdidiag_free_uim_req_mem(&(request.req_payload.request.uim_cmd_req));

      break;

    case GSDIDIAG_ISIM_AUTHENTICATE_CMD:
      gsdidiag_delayed_isim_auth (&request.req_payload,
                        sizeof(gsdidiag_req_payload_type),
                        &status,
                        rsp_id,
                        aps_as_transport);
      /* Free the memory for the authentication string allocated in the request pointer */
      if(request.req_payload.request.isim_auth_req.auth.data_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(request.req_payload.request.isim_auth_req.auth.data_ptr);
        request.req_payload.request.isim_auth_req.auth.data_ptr = NULL;
      }
      break;
    case GSDIDIAG_SESSION_OPEN_CMD:
      gsdidiag_delayed_session_open (&request.req_payload,
                        sizeof(gsdidiag_req_payload_type),
                        &status,
                        rsp_id,
                        aps_as_transport);
      break;
    case GSDIDIAG_SESSION_CLOSE_CMD:
      gsdidiag_delayed_session_close (&request.req_payload,
                        sizeof(gsdidiag_req_payload_type),
                        &status,
                        rsp_id,
                        aps_as_transport);
      break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_SAP_CONNECT_CMD: /*xxx_sap*/
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_DISCONNECT_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_SAP_DEREGISTER_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_SAP_GET_ATR_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_SAP_INIT_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_RESET_SIM_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_CARD_READER_STATUS_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_PIN_VERIFY_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_SAP_SEND_APDU_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_POWER_OFF_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    case GSDIDIAG_SAP_POWER_ON_CMD:
#if (defined(FEATURE_BT) || defined(FEATURE_WM_BT)) && defined (FEATURE_BT_EXTPF_SAP)
#error code not present
#endif /* (FEATURE_BT || FEATURE_WM_BT) && FEATURE_BT_EXTPF_SAP */
        break;
    #ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case GSDIDIAG_CARD_PDOWN_CMD:
        gsdidiag_delayed_card_pdown(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
    case GSDIDIAG_CARD_PUP_CMD:
        gsdidiag_delayed_card_pup(&request.req_payload,
                                      sizeof(gsdidiag_req_payload_type),
                                      &status,
                                      rsp_id,
                                      aps_as_transport);
        break;
    case GSDIDIAG_CARD_STATUS_CMD:
        gsdidiag_delayed_card_status(&request.req_payload,
          sizeof(gsdidiag_req_payload_type),
          &status,
          rsp_id,
          aps_as_transport);
        break;
    #endif // FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    case MMGSDIDIAG_REGISTER_FOR_EVENT_CMD:
       gsdidiag_delayed_register_for_event(&request.req_payload,
          sizeof(gsdidiag_req_payload_type),
          &status,
          rsp_id,
          aps_as_transport);
       break;
    case MMGSDIDIAG_OK_TO_REFRESH_CMD:
       gsdidiag_delayed_ok_to_refresh(&request.req_payload,
          sizeof(gsdidiag_req_payload_type),
          &status,
          rsp_id,
          aps_as_transport);
       break;
    case MMGSDIDIAG_REFRESH_REQ_CMD:
    case MMGSDIDIAG_REFRESH_REG_BY_PATH_CMD:
      gsdidiag_delayed_register_for_refresh(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
      /* Free the memory for the file list allocated in the request pointer */
      if(request.req_payload.request.refresh_reg_req.file_list_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(
          request.req_payload.request.refresh_reg_req.file_list_ptr);
      }
      /* Free the memory for the file path allocated in the request pointer */
      if(request.req_payload.request.refresh_reg_req.file_path_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(
          request.req_payload.request.refresh_reg_req.file_path_ptr);
      }
      break;
    case MMGSDIDIAG_REFRESH_DEREG_CMD:
      gsdidiag_delayed_deregister_for_refresh(&request.req_payload,
                            sizeof(gsdidiag_req_payload_type),
                            &status,
                            rsp_id,
                            aps_as_transport);
      /* Free the memory for the file list allocated in the request pointer */
      if(request.req_payload.request.refresh_dereg_req.file_list_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE(
          request.req_payload.request.refresh_dereg_req.file_list_ptr);
      }
      /* Free the memory for the file path allocated in the request pointer */
      if(request.req_payload.request.refresh_dereg_req.file_path_ptr != NULL)
      {
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
          request.req_payload.request.refresh_dereg_req.file_path_ptr);
      }
      break;

      case MMGSDIDIAG_UICC_SEARCH_REQ_CMD:
      case MMGSDIDIAG_ICC_SEARCH_REQ_CMD:
        gsdidiag_delayed_search(&request.req_payload,
                                sizeof(gsdidiag_req_payload_type),
                                &status,
                                rsp_id,
                                aps_as_transport);
        if(request.req_payload.request.search_req.search_pattern.data_ptr != NULL)
        {
          MMGSDIUTIL_TMC_MEM_FREE(request.req_payload.request.search_req.search_pattern.data_ptr);
          request.req_payload.request.search_req.search_pattern.data_ptr =NULL;
        }
        break;

     case MMGSDIDIAG_CREATE_PKCS15_TABLE_CMD:
       gsdidiag_delayed_create_pkcs15_lookup_table(&request.req_payload,
          sizeof(gsdidiag_req_payload_type),
          &status,
          rsp_id,
          aps_as_transport);
       break;

     case GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD:
       gsdidiag_delayed_activate_feature_indicator (&request.req_payload,
                                                    sizeof(gsdidiag_req_payload_type),
                                                    &status,
                                                    rsp_id,
                                                    aps_as_transport);
       break;

     case GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD:
       gsdidiag_delayed_deactivate_feature_indicator (&request.req_payload,
                                                      sizeof(gsdidiag_req_payload_type),
                                                      &status,
                                                      rsp_id,
                                                      aps_as_transport);
       break;

     case GSDIDIAG_GET_FEATURE_INDICATION_CMD:
       gsdidiag_delayed_get_feature_indication (&request.req_payload,
                                                sizeof(gsdidiag_req_payload_type),
                                                &status,
                                                rsp_id,
                                                aps_as_transport);
       break;

     case GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD:
       gsdidiag_delayed_set_feature_indicator_data (&request.req_payload,
                                                    sizeof(gsdidiag_req_payload_type),
                                                    &status,
                                                    rsp_id,
                                                    aps_as_transport);

       if (request.req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr != NULL)
       {
         MMGSDIUTIL_TMC_MEM_FREE(request.req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr);
         request.req_payload.request.set_feature_indicator_data_req.perso_feature_data_ptr = NULL;
       }
       break;

     case GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD:
       gsdidiag_delayed_get_feature_indicator_key (&request.req_payload,
                                                   sizeof(gsdidiag_req_payload_type),
                                                   &status,
                                                   rsp_id,
                                                   aps_as_transport);
       break;

     case GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD:
       gsdidiag_delayed_unblock_feature_ind (&request.req_payload,
                                             sizeof(gsdidiag_req_payload_type),
                                             &status,
                                             rsp_id,
                                             aps_as_transport);
       break;

     case GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD:
       gsdidiag_delayed_get_perm_feature_indication (&request.req_payload,
                                                     sizeof(gsdidiag_req_payload_type),
                                                     &status,
                                                     rsp_id,
                                                     aps_as_transport);
       break;

     case GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD:
       gsdidiag_delayed_disable_perm_feature_indicator (&request.req_payload,
                                                        sizeof(gsdidiag_req_payload_type),
                                                        &status,
                                                        rsp_id,
                                                        aps_as_transport);
       break;
    case GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD:
      gsdidiag_delayed_get_feature_indicator_data (&request.req_payload,
                                                   sizeof(gsdidiag_req_payload_type),
                                                   &status,
                                                   rsp_id,
                                                   aps_as_transport);
      break;

     case GSDIDIAG_SIM_GET_CPHS_INFORMATION_CMD:
       gsdidiag_delayed_sim_get_cphs_information (&request.req_payload,
                                                  sizeof(gsdidiag_req_payload_type),
                                                  &status,
                                                  rsp_id,
                                                  aps_as_transport);
        break;

      default:
        break;
  }

  if( status == GSDIDIAG_SUCCESS)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status = GSDI_SUCCESS;
  }
  /* This covers all failure scenarios from MMGSDI/GSDI */
  else if(status >= GSDIDIAG_MMGSDI_INCORRECT_PARAMS)
  {
    /* We force the status in immediate response to success
       so that asia script expects a delayed response */
    diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_SUCCESS);
  }
  else
  {
    /* Something failed in diag. We will not be able to do a delayed
       response indicate the same */
    diagpkt_subsys_reset_delayed_rsp_id(rsp_ptr);
    diagpkt_subsys_set_status(rsp_ptr, status);
  }

  /* commit the immediate response */
  diagpkt_commit(rsp_ptr);

  /* If the status indicates a value for mmgsdi/gsdi failure then
     convert the status back to gsdi/mmgsdi return type  */

  if((status >= GSDIDIAG_GSDI_ERROR) &&
     (status <= GSDIDIAG_GSDI_MAX_RETURN_STATUS))
  {
    gsdidiag_convert_gsdidiag_status_to_gsdi_status(status,&gsdi_status);
    is_gsdi_rsp = TRUE;
  }
  if((status >= GSDIDIAG_MMGSDI_INCORRECT_PARAMS) &&
     (status <= GSDIDIAG_MMGSDI_MAX_RETURN_STATUS))
  {
    gsdidiag_convert_gsdidiag_status_to_mmgsdi_status(status,&mmgsdi_status);
    is_gsdi_rsp = FALSE;
  }
  /* Send a delayed response. A sleep is introduced to simulate
     mmgsdi processing delay */
  if (status >= GSDIDIAG_MMGSDI_INCORRECT_PARAMS)
  {
    rex_sleep(10);
    gsdidiag_delayed_generic_error_rsp(
      mmgsdi_status,
      gsdi_status,
      request.req_payload,
      rsp_id,
      is_gsdi_rsp
    );
  }

  return rsp_ptr;
} /* gsdidiag_generic_commad */


/*===========================================================================
FUNCTION gsdidiag_unblock_pin_cb

DESCRIPTION
    Callback used for an asynchronous gsdi unblock.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_unblock_pin_cb( const gsdi_cnf_T* sim_data )
{

    gsdidiag_event_value_payload_type event_payload;
    uint8  len;
    uint16 index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    index = uint32touint16(sim_data->unblock_pin_cnf.message_header.client_ref);
    len   = sizeof(event_payload);

    /* intialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value = sim_data->unblock_pin_cnf.num_of_retries;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use gsdidiag_unblock_pin_cb.",
                  index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_UNBLOCK_PIN, len, (void *)&event_payload );
        return;
    }

    /* check if the unblock pin was successfull */
    if (sim_data->unblock_pin_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_payload.value  = sim_data->unblock_pin_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_UNBLOCK_PIN, len,(void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->unblock_pin_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_payload.value  = sim_data->unblock_pin_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_UNBLOCK_PIN, len,(void *)&event_payload );
    }

    /* deallocate memory used for passing the two pins */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index( index );
    return;
}

/*===========================================================================
FUNCTION gsdidiag_unblock_pin

DESCRIPTION
    Unblocks a blocked pin. Queues a cmd to gsdi task to unblock a pin.

ARGUMENTS:
    PACKED void *req: Stream of data in the format of
                      gsdidiag_unblock_pin_cmd_req_type
    word pkt_len    : len of request packet
DEPENDENCIES
    None

RETURN VALUE
    packet of type gsdidiag_unblock_pin_cmd_rsp_type

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_unblock_pin (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_unblock_pin_cmd_req_type *req_ptr;
    gsdidiag_unblock_pin_cmd_rsp_type *rsp_ptr;
    gsdi_returns_T                    gsdi_status;
    int                               index;
    uint32                            rsp_len;
    gsdi_pin_ids_T                    pin_id;
    uint32                            unblock_pin_len;
    char                              unblock_pin[GSDI_MAX_PIN_LENGTH];
    uint32                            new_pin_len;
    char                              new_pin[GSDI_MAX_PIN_LENGTH];

    char*                             pin_ptr;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_unblock_pin_cmd_req_type*)req;

    /* pin_ptr is used  to calculate lengths of each pin*/
    /* both pins are in the date structure sim_pin_unblock_new[1]
       each pin has a null terminator at end.
       these have to be extracted out
    */
    pin_ptr = (char*) req_ptr->sim_pin_unblock_new;
    unblock_pin_len = strlen(pin_ptr);

    if (unblock_pin_len > GSDI_MAX_PIN_LEN) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Unblock pin too long(%d). Max length=%d",
                  unblock_pin_len, GSDI_MAX_PIN_LENGTH, 0);
        return NULL;
    }else{
        (void)memcpy((char*) unblock_pin, (const char*)pin_ptr, unblock_pin_len); /*lint !e670 */
    }

    /* move the pin ptr to the start of the second pin */
    pin_ptr = (char*) ((char*)req_ptr->sim_pin_unblock_new +
                        unblock_pin_len + 1);
    new_pin_len  = strlen(pin_ptr);

    if (new_pin_len > GSDI_MAX_PIN_LEN) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("New pin too long(%d). Max length=%d",
                  new_pin_len, GSDI_MAX_PIN_LENGTH, 0);
        return NULL;
    }else{
        (void)memcpy((char*) new_pin, (const char*)pin_ptr, new_pin_len); /*lint !e670 */
    }

    rsp_len = sizeof(gsdidiag_unblock_pin_cmd_rsp_type)
              + unblock_pin_len + new_pin_len + 1;
                  /* + 1 due to null terminator*/

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_unblock_pin_cmd_rsp_type *)diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GSDI,
                                    (uint16)GSDIDIAG_UNBLOCK_PIN_CMD,
                                    rsp_len );

    if (rsp_ptr==NULL)
    {
        return NULL;
    }


    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy((void*)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));

    rsp_ptr->sim_pin_id = req_ptr->sim_pin_id;

    (void)memcpy( (char*) rsp_ptr->sim_app_id,
            (const char*) req_ptr->sim_app_id,
            GSDI_MAX_APP_ID_LEN);
    (void)memcpy( (char*) rsp_ptr->sim_pin_unblock_new,
            (const char*) req_ptr->sim_pin_unblock_new,
             unblock_pin_len + new_pin_len + 2); /*lint !e669 !e670 */

    /* find a free space in req_table to hold information for callback */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR){
        /* req_table is full */
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* assign a transaction id to this request*/
    req_table[index].transaction_id = int32touint16(gsdidiag_get_transaction_id());
    rsp_ptr->transaction_id         = req_table[index].transaction_id;

    /* allocate memory for the pins
       these will be deallocated in the callback function */
    /* sim unblock pin */
    req_table[index].var[0] = mmgsdi_malloc( GSDI_MAX_PIN_LEN);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }
    /* sim new pin*/
    req_table[index].var[1] = mmgsdi_malloc( GSDI_MAX_PIN_LEN);
    if (req_table[index].var[1]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }
    /* unblock pin */
    (void)memset ( (char*) req_table[index].var[0], 0xFF, GSDI_MAX_PIN_LEN );
    (void)memcpy ( (char*) req_table[index].var[0],
             (const char*) unblock_pin,
             unblock_pin_len );

    /* new pin */
    (void)memset ( (char*) req_table[index].var[1], 0xFF, GSDI_MAX_PIN_LEN );
    (void)memcpy ( (char*) req_table[index].var[1],
             (const char*) new_pin,
              new_pin_len );


    /* map the pin id from user ids to internal ids */
    pin_id = gsdidiag_map_pin_id(req_ptr->sim_pin_id);

    /* Check if pin is blocked
       TODO */

    /* send the request for unblock pin */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi2_unblock_pin(req_ptr->sim_slot_number,
                                 pin_id,
                                 (char*)req_table[index].var[0],
                                 (char *) req_table[index].var[1],
                                 NULL,
                                 index,
                                 gsdidiag_unblock_pin_cb);
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = gsdi_unblock_pin  ( pin_id,
                                 (char *) req_table[index].var[0], /* unblock */
                                 (char *) req_table[index].var[1], /* new */
                                 NULL,
                                 int32touint32(index),
                                 (gsdi_async_cb)gsdidiag_unblock_pin_cb);
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",gsdi_status,0,0);

        /* deallocate memory used for passing the two pins */
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }
    /* rsp status */
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}

/*===========================================================================
FUNCTION gsdidiag_disable_pin_cb

DESCRIPTION
    Callback used for an asynchronous gsdi pin disable.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_disable_pin_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8 len; /* length of event */
    int index; /* index into req_table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32toint32(sim_data->disable_pin_cnf.message_header.client_ref);

    /* populate the payload */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value = sim_data->disable_pin_cnf.num_of_retries;

    /*  req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
        MMGSDI_DEBUG_MSG_ERROR("ndex %d is not in use gsdidiag_disbale_pin_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_DISABLE_PIN,
                              len,
                              (void *)&event_payload );
    }

    /* check if the gsdi_disable_pin status was successfull*/
    if (sim_data->disable_pin_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_payload.value = sim_data->disable_pin_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_DISABLE_PIN,
                             len,
                             (void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->disable_pin_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_payload.value =sim_data->verify_pin_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_DISABLE_PIN,
                             len,
                             (void *)&event_payload );
    }


    /* deallocate memory used for passing the pins */
    gsdidiag_free_index_memory(index);

    /* free the index, so that it can be used later */
    gsdidiag_free_index(index);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_disable_pin

DESCRIPTION:
    Disables pin. Queues a task to gsdi, to disable a pin.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_disable_pin_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES:
    None

RETURN VALUE:
    None

SIDE EFFECTS:
    response packet of type  gsdidiag_disable_pin_cmd_rsp_type
===========================================================================*/
static PACKED void * gsdidiag_disable_pin (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_disable_pin_cmd_req_type *req_ptr;
    gsdidiag_disable_pin_cmd_rsp_type *rsp_ptr;
    gsdi_returns_T                    gsdi_status;
    int                               index;
    gsdi_pin_ids_T                    pin_id;
    uint32                            pin_len;
    uint32                            rsp_len;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_disable_pin_cmd_req_type*)req;

    pin_len = strlen( (char*) req_ptr->sim_pin );
    if (pin_len > GSDI_MAX_PIN_LEN) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                  pin_len, GSDI_MAX_PIN_LENGTH, 0);
        return NULL;
    }

    rsp_len = sizeof(gsdidiag_disable_pin_cmd_rsp_type) + pin_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_disable_pin_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI, (uint16)GSDIDIAG_DISABLE_PIN_CMD,
                                 rsp_len);

    if (rsp_ptr==NULL)
    {
        return NULL;
    }


    /* copy req_ptr to rsp_ptr */
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len  = req_ptr->sim_app_id_len;
    (void)memcpy( (char*) rsp_ptr->sim_app_id,
            (char*) req_ptr->sim_app_id,
            sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_pin_id = req_ptr->sim_pin_id;
    (void)memcpy((void*)rsp_ptr->sim_pin, (const void *)req_ptr->sim_pin, pin_len + 1); /*lint !e669 !e670 */


    /* initialize transaction id */
    rsp_ptr->transaction_id = 0;

    /* map the pin id from user ids to internal ids */
    pin_id = gsdidiag_map_pin_id(req_ptr->sim_pin_id);

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /* req_table is full */
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;

    }
    /* allocate memory for sim_pin */
    req_table[index].var[0] = (char*) mmgsdi_malloc( GSDI_MAX_PIN_LEN);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }
    (void)memset (req_table[index].var[0], 0xFF, GSDI_MAX_PIN_LEN);
    (void)memcpy ((char*)req_table[index].var[0], (const char*) req_ptr->sim_pin, pin_len); /*lint !e670 */

    rsp_ptr->transaction_id= int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    /* send the request to disable pin */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi2_disable_pin(req_ptr->sim_slot_number,
                               pin_id,
                               (char*)req_table[index].var[0],
                               NULL,
                               index,
                               gsdidiag_disable_pin_cb);
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = gsdi_disable_pin ( pin_id,
                                (char*)req_table[index].var[0],
                                NULL,
                                int32touint32(index),
                                (gsdi_async_cb)gsdidiag_disable_pin_cb);
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_sim_read queue failed %d",gsdi_status,0,0);
        /* free memory used for pin */
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);

    }
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status,
      &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}

/*===========================================================================
FUNCTION gsdidiag_enable_pin_cb

DESCRIPTION
    Callback used for an asynchronous gsdi enable_pin. This sends out an event
    EVENT_GSDI_ENABLE_PIN with information about num_of_retries.

ARGUMENTS:

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void gsdidiag_enable_pin_cb(const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_value_payload_type event_payload;
    uint8 len; /* length of event */
    uint16 index; /* indx into req table*/

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->enable_pin_cnf.message_header.client_ref);

    /* intialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value = sim_data->enable_pin_cnf.num_of_retries;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table index should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED){
        MMGSDI_DEBUG_MSG_ERROR("Error gsdidiag. index %d is not in use.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload(EVENT_GSDI_ENABLE_PIN, len, (void *)&event_payload );
    }


    /* check if rsp is enable_pin*/
    if ( sim_data->message_header.resp_type != GSDI_ENABLE_PIN_RSP )
    {
        MMGSDI_DEBUG_MSG_ERROR ("Rspfrom gsdi not GSDI_ENABLE_PIN_RSP(%d):%d",
                   GSDI_ENABLE_PIN_RSP,
                   sim_data->message_header.resp_type,
                   0);
        event_payload.status = GSDIDIAG_INCORRECT_RSP_ERR;
        event_report_payload(EVENT_GSDI_ENABLE_PIN,
                             len,
                             (void *)&event_payload );
        /*free memory used for pin*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
    }

    /* check if enable pin was success*/
    if (sim_data->enable_pin_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_payload.value  = sim_data->enable_pin_cnf.num_of_retries;
        event_report_payload(EVENT_GSDI_ENABLE_PIN, len, (void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->enable_pin_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_report_payload(EVENT_GSDI_ENABLE_PIN, len, (void *)&event_payload );
    }

    /* deallocate memory used for passing pin */
    gsdidiag_free_index_memory(index);

    /* free index for future requests */
    gsdidiag_free_index(index);
    return;
}

/*===========================================================================
FUNCTION gsdidiag_enable_pin

DESCRIPTION
    Enables Pin. This is an asynchronous request. The callback gsdidiag_enable_pin
    is executed when the Enable Pin reuest completes completes.

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_disable_pin_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
  None

RETURN VALUE
  Rsp_ptr of type gsdidiag_enable_pin_cmd_rsp_type

SIDE EFFECTS
  None
===========================================================================*/
static PACKED void * gsdidiag_enable_pin (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_enable_pin_cmd_req_type *req_ptr;
    gsdidiag_enable_pin_cmd_rsp_type *rsp_ptr;
    int                              index;
    gsdi_pin_ids_T                   pin_id;
    gsdi_returns_T                   gsdi_status;
    uint32                           rsp_len;
    uint32                           pin_len;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_enable_pin_cmd_req_type*)req;

    pin_len = strlen( (const char *)req_ptr->sim_pin);
    if (pin_len > GSDI_MAX_PIN_LEN) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Pin too long(%d). Max length=%d",
                  pin_len, GSDI_MAX_PIN_LENGTH, 0);
        return NULL;
    }

    rsp_len = sizeof(gsdidiag_enable_pin_cmd_rsp_type) + pin_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_enable_pin_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI, (uint16)GSDIDIAG_ENABLE_PIN_CMD,
                                 rsp_len );

    if (rsp_ptr==NULL)
    {
        return NULL;
    }

    /* copy req_ptr to rsp_ptr */
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy((void*)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_pin_id = req_ptr->sim_pin_id;
    (void)memcpy((void*)rsp_ptr->sim_pin, (const void *)req_ptr->sim_pin, pin_len + 1); /*lint !e669 !e670 */

    /* initialize transaction id */
    rsp_ptr->transaction_id = 0;

    /* map the pin id from user ids to internal ids */
    pin_id = gsdidiag_map_pin_id(req_ptr->sim_pin_id);

    /* find a free space in req_table to put the information in*/
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /* req_table is full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;

    }

    /* get a transaction id */
    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    /* allocate memory for sim_pin */
    req_table[index].var[0] = (char*) mmgsdi_malloc( GSDI_MAX_PIN_LEN);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    (void)memset (req_table[index].var[0], 0xFF, GSDI_MAX_PIN_LEN);
    (void)memcpy( (char*)req_table[index].var[0],
            (const char*) req_ptr->sim_pin, pin_len); /*lint !e670 */

    /* send request to gsdi task for enabling pin */
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    gsdi_status = gsdi2_enable_pin(req_ptr->sim_slot_number,
                              pin_id,
                              (char*)req_table[index].var[0],
                              NULL,
                              index,
                              gsdidiag_enable_pin_cb);
#else /* !FEATURE_MMGSDI_DUAL_SLOT */
    gsdi_status = gsdi_enable_pin ( pin_id,
                               (char*)req_table[index].var[0],
                               NULL,
                               int32touint32(index),
                               (gsdi_async_cb)gsdidiag_enable_pin_cb);
#endif /* !FEATURE_MMGSDI_DUAL_SLOT */

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_enable_pin queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}

/*===========================================================================
FUNCTION gsdidiag_get_available_apps

DESCRIPTION
    Get apps avaiable of the sim.

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_get_available_apps_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    Response pointer of type gsdidiag_get_available_apps_cmd_rsp_type
    This will contain the apps available.

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_available_apps (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_get_available_apps_cmd_rsp_type *rsp_ptr;
    gsdi_card_apps_T card_apps;
    uint8 num_slots;
    uint16 rsp_len;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    void * ptr;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    /* figure out how many slots are present*/
    /* TODOD the code should be replaced get_num_slots provided by gsdilib.c*/

    num_slots = 1;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    num_slots = 2;
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    rsp_len = sizeof(gsdidiag_get_available_apps_cmd_rsp_type) +
                     (num_slots-1)*sizeof(slot_info);
                     /* one slot has already been assigned memory in rsp_type */

    /* allocate the rsp pointer */
    rsp_ptr = (gsdidiag_get_available_apps_cmd_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_AVAILABLE_APPS_CMD,
                                   rsp_len );

    if (rsp_ptr==NULL)
    {
        return NULL;
    }
    /* send the request for apps */
    gsdi_get_apps_available( &card_apps );

    /* no information about failures. assume success */
    rsp_ptr->status = GSDIDIAG_SUCCESS;
    /* synchornous call. hence set trasaction id=0 */
    rsp_ptr->transaction_id = 0;

    rsp_ptr->operational_slot = (uint8)GSDI_SLOT_1;
    rsp_ptr->num_slots = num_slots;

#ifdef FEATURE_MMGSDI_DUAL_SLOT
    rsp_ptr->slots_info[0].card_inserted = card_apps.slot1_present;
#else
    if ((card_apps.apps_enabled & GSDI_USIM_APP_MASK) == GSDI_USIM_APP_MASK ||
        (card_apps.apps_enabled & GSDI_GSM_SIM_MASK) == GSDI_GSM_SIM_MASK )
    {
        rsp_ptr->slots_info[0].card_inserted = TRUE;
    }
    else
    {
        rsp_ptr->slots_info[0].card_inserted = FALSE;
    }
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    rsp_ptr->slots_info[0].apps_available = card_apps.apps_enabled;
#ifdef FEATURE_MMGSDI_DUAL_SLOT
    ptr = (void*)(rsp_ptr + (rsp_len - sizeof(slot_info)));
    (void)memcpy(ptr, &card_apps.slot2_present,sizeof(uint8));
    ptr = (uint8*)ptr + sizeof(uint8);
    (void)memcpy(ptr, &card_apps.apps2_enabled,sizeof(uint8));
#endif /* FEATURE_MMGSDI_DUAL_SLOT */

    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /*lint !e715 */

/*===========================================================================
FUNCTION gsdidiag_execute_apdu_cb

DESCRIPTION
    Callback function executed after the apdu is executed. Sends out event
    with information about request execution. Cleans up memory used for
    this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_send_apdu_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8  len;
    uint16 index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->send_apdu_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value = (uint8)sim_data->send_apdu_cnf.message_header.gsdi_status;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED ){
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use sendapdu_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_EXECUTE_APDU_REQ,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->send_apdu_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_EXECUTE_APDU_REQ,
                              len,
                              (void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->send_apdu_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_report_payload( EVENT_GSDI_EXECUTE_APDU_REQ,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
}

/*===========================================================================
FUNCTION gsdidiag_execute_apdu

DESCRIPTION
    Sends an APDU to the sim card.

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_sim_send_apdu_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_send_apdu (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_sim_send_apdu_req_type *req_ptr;
    gsdidiag_sim_send_apdu_rsp_type *rsp_ptr;
    int index;
    gsdi_returns_T gsdi_status;
    uint32 rsp_len;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_sim_send_apdu_req_type*)req;

    rsp_len = sizeof( gsdidiag_sim_send_apdu_rsp_type )
                     + req_ptr->apdu_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_sim_send_apdu_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_SEND_APDU_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
    {
        return NULL;
    }
    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy( (char*) rsp_ptr->sim_app_id,
            (const char*) req_ptr->sim_app_id,
            sizeof(req_ptr->sim_app_id));
    rsp_ptr->apdu_len = req_ptr->apdu_len;
    (void)memcpy( (char*)rsp_ptr->apdu_data,
            (const char*) req_ptr->apdu_data,
            req_ptr->apdu_len);

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR){
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    /* allocate memory to pass to the gsdi task */
    req_table[index].var[0] = mmgsdi_malloc( req_ptr->apdu_len);

    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    (void)memcpy( (char*) req_table[index].var[0],
            (const char*) req_ptr->apdu_data,
            req_ptr->apdu_len);


    /* send the request for pin status */
    gsdi_status = gsdi2_send_apdu ( (gsdi_slot_id_type)req_ptr->sim_slot_number,
                               NULL,
                               req_ptr->apdu_len ,
                               req_table[index].var[0],
                               int32touint32(index),
                               (gsdi_async_cb)gsdidiag_send_apdu_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdiag_execute_apdu queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
}

/*===========================================================================
FUNCTION gsdidiag_select_cb

DESCRIPTION
  Callback used for an asynchronous gsdi select, sim_data will have the data from
  the sim select.

ARGUMENTS
    gsdi_cnf_T* sim_data: Information, from gsdi task, about request executed

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdidiag_select_cb (const gsdi_cnf_T* sim_data)
{
    gsdidiag_event_payload_type event_payload; /* used to send event*/
    fs_open_xparms_type open_parms;
    uint8                   len             = 0;
    int                     index           = 0;
    int                     file_descriptor = 0;
    struct fs_stat          file_stat       = {0};

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    (void) memset(&open_parms, 0x00, sizeof(fs_open_xparms_type));
    len   = sizeof( event_payload);

    index = uint32toint32(sim_data->message_header.client_ref);
    event_payload.transaction_id = req_table[index].transaction_id;

    /* req_table index should be in the used state*/
    if (req_table[index].use_flag!=GSDIDIAG_REQ_TABLE_INDEX_USED)
    {
       MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use in gsdidiag_read_cb",
                 index,0,0);
       event_payload.status = GSDIDIAG_MEMORY_ERR;
       event_report_payload( EVENT_GSDI_SELECT, len, (void *)&event_payload);
       return;
    }

    /* test if file already exists */
    if(efs_stat(req_table[index].var[0], &file_stat) == 0)
    {
      /* delete file, it already exists */
      if (efs_unlink (req_table[index].var[0]) == -1) {
          MMGSDI_DEBUG_MSG_ERROR ("File remove failed ", 0, 0, 0);
          event_payload.status = GSDIDIAG_EFS_RM_ERR;
          event_report_payload(EVENT_GSDI_SELECT,
                               len,
                              (void *)&event_payload );
          /*free filename memory*/
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
      }
    }
    else if (efs_errno != ENOENT)
    {
      /* Test failed due to reason other than "File Does not Exists"*/
      MMGSDI_DEBUG_MSG_ERROR ("efs_stat failed.", 0, 0, 0);
      event_payload.status = GSDIDIAG_EFS_NAMETEST_ERR;
      event_report_payload(EVENT_GSDI_SELECT,
                           len,
                           (void *)&event_payload );
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
      return;
    }

    /* Make sure it is a Select Resp */
    if ( sim_data->message_header.resp_type != GSDI_SELECT_RSP )
    {
        MMGSDI_DEBUG_MSG_ERROR ("Rsp from gsdi is was not GSDI_SELECT_RSP(%d):%d",
                   GSDI_SELECT_RSP,
                   sim_data->message_header.resp_type,
                   0);
        event_payload.status = GSDIDIAG_INCORRECT_RSP_ERR;
        event_report_payload(EVENT_GSDI_SELECT,
                             len,
                             (void *)&event_payload );

        /*free memory used for filename*/
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        return;
    }

    /* check if the select was successful */
    if (sim_data->message_header.gsdi_status == GSDI_SUCCESS)
    {
        open_parms.create.cleanup_option = FS_OC_CLOSE;
        open_parms.create.buffering_option = FS_OB_PROHIBIT;

        file_descriptor = efs_open(req_table[index].var[0], O_CREAT | O_WRONLY);

        if(file_descriptor < 0)
        {
          MMGSDI_DEBUG_MSG_ERROR("efs_open failed.", 0, 0, 0);

          /* send event */
          event_payload.status = GSDIDIAG_EFS_OPEN_ERR;
          event_report_payload(EVENT_GSDI_SELECT,
                               len,
                               (void *)&event_payload );

          /* free memory used for filename */
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
        }

        /* write to file */
        if(efs_write(file_descriptor,
                     (void *)sim_data->select_cnf.data,
                     (fs_size_t)sim_data->select_cnf.returned_data_len) <
                     (fs_ssize_t)sim_data->select_cnf.returned_data_len)
        {
            MMGSDI_DEBUG_MSG_ERROR("efs_write failed. ", 0, 0, 0);
            event_payload.status = GSDIDIAG_EFS_WRITE_ERR;
            event_report_payload( EVENT_GSDI_SELECT,
                                  len,
                                  (void *)&event_payload );
        }else{
            /* success */
            event_payload.status = GSDIDIAG_SUCCESS;
            event_report_payload( EVENT_GSDI_SELECT,
                                  len,
                                  (void *)&event_payload );
        }
        /* close file */
        if(efs_close(file_descriptor) < 0)
        {
          ERR("efs_close procedure failed 0x%x", file_descriptor, 0, 0);
        }
    }else{
        /* Error in gsdi_sim_read cmd*/
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->message_header.gsdi_status, &event_payload.status);
        event_report_payload(EVENT_GSDI_SELECT,
                             len,
                             (void *)&event_payload );
    }

    /* free memory used for filename */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);

    return;
}

/*===========================================================================
FUNCTION gsdidiag_sim_select

DESCRIPTION
    Function to select master, dedicate, application dedicate, elementary files
    in the sim. A task is queued to gsdi task to perform the selection.
    Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

ARGUMENTS:
    PACKED void *req: pointer to request packet of type
                      gsdidiag_select_cmd_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_sim_select (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
    gsdidiag_select_cmd_req_type  *req_ptr;
    gsdidiag_select_cmd_rsp_type  *rsp_ptr;
    gsdi_returns_T                 gsdi_status;
    uim_items_enum_type            sim_file_name = UIM_NO_SUCH_ITEM;
    uint32                         filename_len;
    uint32                         rsp_len;
    byte                           dummy_data_p;
    byte                           dummy_data_len_p;
    uim_file_type                  file_type = UIM_NO_SUCH_FILE;
    word                           path_p[UIM_MAX_PATH_ELEMENTS];
    byte                           path_len = 0;
    boolean                        ready = FALSE;
    int                            i = 0;
    int index;
    int                            j = 0;
    mmgsdi_return_enum_type        mmgsdi_status    = MMGSDI_ERROR;

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    if(pkt_len < sizeof(gsdidiag_select_cmd_req_type))
    {
        MSG_ERROR("gsdidiag_sim_select: invalid pkt_len 0x%x",pkt_len,0,0);
        return NULL;
    }

    /* Allocate one additional byte in case EFS filename string is not 
    null-terminated */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
    {
      MSG_ERROR("gsdidiag_sim_select malloc err req_ptr",0,0,0);
      return NULL;
    }
    else
    {
      memcpy((void *)req_ptr,(void *)req,pkt_len);
    }

    (void)memset(path_p, 0xFF, sizeof(word)*UIM_MAX_PATH_ELEMENTS);

    filename_len = strlen((const char*)req_ptr->efs_filename);

    /* check if efs filename is too long*/
    if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
    {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Filename too long(%d). Max length=%d",
                  filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return NULL;
    }

    /* length of rsp */
    rsp_len = sizeof(gsdidiag_select_cmd_rsp_type) + filename_len;

    /* allocate memory for the rsp*/
    rsp_ptr = (gsdidiag_select_cmd_rsp_type  *)diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GSDI,
                                    (uint16)GSDIDIAG_SELECT_CMD,
                                    rsp_len );

    if (rsp_ptr==NULL)
    {
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);   
        return NULL;
    }

    /* copy req_ptr to rsp_ptr */
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->sim_app_id_len = req_ptr->sim_app_id_len;
    (void)memcpy((void*)rsp_ptr->sim_app_id,
           (const void *)req_ptr->sim_app_id,
           sizeof(req_ptr->sim_app_id));
    rsp_ptr->sim_elementary_file = req_ptr->sim_elementary_file;
    rsp_ptr->sim_directory = req_ptr->sim_directory;
    rsp_ptr->sim_elementary_file_path_len = req_ptr->sim_elementary_file_path_len;
    (void)memcpy((void*)rsp_ptr->sim_elementary_file_path,
           (const void *)req_ptr->sim_elementary_file_path,
           sizeof(req_ptr->sim_elementary_file_path));
    rsp_ptr->sim_directory_path_len = req_ptr->sim_directory_path_len;
    (void)memcpy((void*)rsp_ptr->sim_directory_path,
           (const void *)req_ptr->sim_directory_path,
           sizeof(req_ptr->sim_directory_path));

    /* Copy efs file name to response structure
       No check here because the source string length is what is
       being used to determine the destination length*/
    (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                       (const char*)req_ptr->efs_filename,
                       uint32toint32(filename_len + GSDIDAG_NULL_CHAR_LEN));
    /* initialize transaction id */
    rsp_ptr->transaction_id = 0;

    /* find a free space in req_table to put the information in*/
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /* req_table is full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* get a transaction id */
    req_table[index].transaction_id =  int32touint16(gsdidiag_get_transaction_id());
    rsp_ptr->transaction_id         =  req_table[index].transaction_id;

    /* allocate memory for filename  */
    req_table[index].var[0] = gsdidiag_sim_malloc_append_efs_dir((char *)req_ptr->efs_filename);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* populate the correct input parameter based on diag packet inputs */

    /* try elementary file enum type first, map from user ids to gsdi internal ids */
    if (req_ptr->sim_elementary_file != GSDIDIAG_MAX_FILE_ENUM) {
      sim_file_name = gsdidiag_convert_file_to_uim_enum(req_ptr->sim_elementary_file);
      /* found a valid EF enum */
      if (sim_file_name != UIM_NO_SUCH_ITEM) {
        file_type = UIM_EF;
        ready = TRUE;
      }
    }
    if (!ready) {
      /* try the DF enum type next */
      if (req_ptr->sim_directory != 0) {
        gsdi_status = gsdidiag_convert_directory_to_uim_enum((gsdidiag_directory_enum_type)req_ptr->sim_directory, path_p, &path_len, &file_type);
        /* found a valid EF enum */
        if (gsdi_status == GSDI_SUCCESS) {
          sim_file_name = UIM_NO_SUCH_ITEM;
          ready = TRUE;
        }
      }

      if (!ready) {
        /* try file path now */
        if (req_ptr->sim_elementary_file_path_len != 0) {
          /* copy the path over */
          file_type = UIM_EF;
          sim_file_name = UIM_EF_BY_PATH;
          path_len = req_ptr->sim_elementary_file_path_len / 2;
          if(path_len > UIM_MAX_PATH_ELEMENTS)
          {
            MSG_HIGH("Path length is too large, not populating path_p",0,0,0);
          }
          else
          {
            /* populate the MF, DF info */
            j = 0;
            for (i = 0; i< path_len-1; i++) {
              path_p[i] = (req_ptr->sim_elementary_file_path[j] << 0x08) | req_ptr->sim_elementary_file_path[j+1];
              j += 2;
            }
            /* populate the EF */
            path_p[UIM_MAX_PATH_ELEMENTS-1] = (req_ptr->sim_elementary_file_path[req_ptr->sim_elementary_file_path_len-2] << 0x08)
                                              | req_ptr->sim_elementary_file_path[req_ptr->sim_elementary_file_path_len-1];
            ready = TRUE;
            /* reset path len to 4 */
            path_len = 4;
          }
        }

        if(!ready) {
          /* Try directory path now */
          if (req_ptr->sim_directory_path_len != 0) {
            path_len = (req_ptr->sim_directory_path_len / 2);
            if(path_len > UIM_MAX_PATH_ELEMENTS)
            {
              MSG_HIGH("Path length is too large, not populating path_p",0,0,0);
            }
            else
            {
              if ((path_len == 1) && (req_ptr->sim_directory_path[0] == 0x3F)
                  && (req_ptr->sim_directory_path[1] == 0x00)) { /* user passes in MF */
                sim_file_name = UIM_NO_SUCH_ITEM;
                file_type = UIM_MF;
                path_len = 0;
                ready = TRUE;
              }
              else { /* a DF */
                /* copy the path over */
                file_type = UIM_DF;
                sim_file_name = UIM_NO_SUCH_ITEM;
                j = 0;
                for (i = 0; i< path_len; i++) {
                  path_p[i] = (req_ptr->sim_directory_path[j] << 0x08) | req_ptr->sim_directory_path[j+1];
                  j += 2;
                }
                ready = TRUE;
              }
            }
          }
        }
      }
    }

    if (ready) {
      if (path_len == 0) {
        gsdi_status = gsdi_file_select(
                            sim_file_name,
                            file_type,
                            NULL,
                            path_len,
                            &dummy_data_len_p,
                            &dummy_data_p,
                            int32touint32(index),
                            (gsdi_async_cb) gsdidiag_select_cb
                            );
      }
      else {
        gsdi_status = gsdi_file_select(
                            sim_file_name,
                            file_type,
                            path_p,
                            path_len,
                            &dummy_data_len_p,
                            &dummy_data_p,
                            int32touint32(index),
                            (gsdi_async_cb) gsdidiag_select_cb
                            );
      }

      if (gsdi_status != GSDI_SUCCESS )
      {
          MMGSDI_DEBUG_MSG_ERROR( "gsdiag_select queue failed %d", gsdi_status, 0, 0);
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
      }

      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        gsdi_status, &rsp_ptr->status);
    }
    else {
      /* ready flag is not set */
      rsp_ptr->status = GSDIDIAG_BAD_PARAM;
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
    }
    diagpkt_commit(rsp_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return NULL;
} /* gsdidiag_sim_select */
#endif /*FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION*/

#ifdef FEATURE_MMGSDI_PERSONALIZATION

/*===========================================================================
FUNCTION gsdidiag_activate_feature_indicator_cb

DESCRIPTION
    Callback function executed after the activate personalization.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_activate_feature_indicator_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8                             len;
    uint16                            index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->act_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;
    event_payload.value = (uint8)sim_data->act_ind_cnf.perso_feature;
    event_payload.value2 = uint32touint8(sim_data->act_ind_cnf.num_retries);

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use act_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_ACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->act_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_ACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }
    else
    {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->act_ind_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_report_payload( EVENT_GSDI_ACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_activate_feature_indicator_cb */

/*===========================================================================
FUNCTION gsdidiag_activate_feature_indicator

DESCRIPTION
    Activate the feature for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_act_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_activate_feature_indicator (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_act_feature_ind_req_type *req_ptr;
    gsdidiag_act_feature_ind_rsp_type *rsp_ptr;
    int                                index;
    gsdi_returns_T                     gsdi_status = GSDI_SUCCESS;
    gsdi_perso_control_key_data_type   control_key_info;
    uint32                             rsp_len;
    uint32                             perso_feature_key_len;

    MSG_HIGH("GSDI DIAG PERSO ACTIVATE FEATURE INDICATOR",0,0,0);

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_act_feature_ind_req_type*)req;

    perso_feature_key_len = strlen((char*)req_ptr->perso_feature_key);

    if (perso_feature_key_len > GSDI_PERSO_MAX_CK) {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Key too long(%d). Max length=%d",
                  perso_feature_key_len, GSDI_PERSO_MAX_CK, 0);
        return NULL;
    }

    rsp_len = sizeof( gsdidiag_act_feature_ind_rsp_type )
                      + perso_feature_key_len;


    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_act_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->perso_feature = req_ptr->perso_feature;
    (void)memcpy( (char*) rsp_ptr->perso_feature_key,
            (const char*) req_ptr->perso_feature_key,
            perso_feature_key_len + 1 ); /*lint !e669 !e670 */

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;


    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_activate_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    memset(&control_key_info,0,sizeof(gsdi_perso_control_key_data_type));
    /* --------------------------------------------------------------------------
      Map the feature indicator
      ------------------------------------------------------------------------*/

    gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->perso_feature,
                                        &control_key_info.feature);
    if (gsdi_status == GSDI_SUCCESS)
    {
      control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sim_slot_number;
      control_key_info.num_bytes       = uint32toint32(perso_feature_key_len);
      control_key_info.control_key_p   = mmgsdi_malloc( perso_feature_key_len);
      if ( control_key_info.control_key_p != NULL )
      {
        (void)memcpy( (char*)control_key_info.control_key_p,
                (const char*)req_ptr->perso_feature_key,
                perso_feature_key_len); /*lint !e670 */
      }
      else
      {
        MMGSDI_DEBUG_ERR("control_key_p.control_key_p is NULL",0,0,0);
        gsdi_status = GSDI_ERROR;
      }

      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if(gsdi_status == GSDI_SUCCESS)
        gsdi_status = gsdi_perso_activate_feature_indicator(&control_key_info,
                                                       int32touint32(index),
                                                       (gsdi_async_cb)gsdidiag_activate_feature_indicator_cb);
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_activate_feature_indicator queue failed %d",gsdi_status,0,0);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    if (control_key_info.control_key_p != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( control_key_info.control_key_p);
    }
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_activate_feature_indicator */


/*===========================================================================
FUNCTION gsdidiag_deactivate_feature_indicator_cb

DESCRIPTION
    Callback function executed after the deactivate personalization.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_deactivate_feature_indicator_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8                             len;
    uint16                            index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->dact_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;
    event_payload.value = (uint8)sim_data->dact_ind_cnf.perso_feature;
    event_payload.value2 = uint32touint8(sim_data->dact_ind_cnf.num_retries);

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use deact_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_DEACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if deact was successfull */
    if (sim_data->dact_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_DEACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->dact_ind_cnf.message_header.gsdi_status,
          &event_payload.status);
        event_report_payload( EVENT_GSDI_DEACTIVATE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_deactivate_feature_indicator_cb */

/*===========================================================================
FUNCTION gsdidiag_deactivate_feature_indicator

DESCRIPTION
    Deactivate the feature for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_deact_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_deactivate_feature_indicator (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_deact_feature_ind_req_type *req_ptr;
    gsdidiag_deact_feature_ind_rsp_type *rsp_ptr;
    int                                  index;
    gsdi_returns_T                       gsdi_status = GSDI_SUCCESS;
    gsdi_perso_control_key_data_type     control_key_info;
    uint32                               rsp_len;
    uint32                               perso_feature_key_len;

    MSG_HIGH("GSDI DIAG PERSO DEACTIVATE FEATURE INDICATOR",0,0,0);

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_deact_feature_ind_req_type*)req;


    perso_feature_key_len = strlen((char*)req_ptr->perso_feature_key);

    if (perso_feature_key_len > GSDI_PERSO_MAX_CK)
    {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Key too long(%d). Max length=%d",
                  perso_feature_key_len, GSDI_PERSO_MAX_CK, 0);
        return NULL;
    }

    rsp_len = sizeof( gsdidiag_deact_feature_ind_rsp_type )
                      + perso_feature_key_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_deact_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->perso_feature = req_ptr->perso_feature;
    (void)memcpy( (char*) rsp_ptr->perso_feature_key,
            (const char*) req_ptr->perso_feature_key,
            perso_feature_key_len + 1); /*lint !e669 !e670 */

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;


    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_deactivate_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    memset(&control_key_info,0,sizeof(gsdi_perso_control_key_data_type));

    /* --------------------------------------------------------------------------
      Map the feature indicator
      ------------------------------------------------------------------------*/

    gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->perso_feature,
                                        &control_key_info.feature);
    if (gsdi_status == GSDI_SUCCESS)
    {
      control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sim_slot_number;
      control_key_info.num_bytes       = uint32toint32(perso_feature_key_len);
      control_key_info.control_key_p   = mmgsdi_malloc( perso_feature_key_len);
      if ( control_key_info.control_key_p != NULL )
      {
        (void)memcpy( (char*)control_key_info.control_key_p,
                (const char*)req_ptr->perso_feature_key,
                perso_feature_key_len); /*lint !e670 */
      }
      else
      {
        MMGSDI_DEBUG_ERR("control_key_p.control_key_p is NULL",0,0,0);
        gsdi_status = GSDI_ERROR;
      }

      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if(gsdi_status == GSDI_SUCCESS)
        gsdi_status = gsdi_perso_deactivate_feature_indicator(&control_key_info,
                                                         int32touint32(index),
                                                         (gsdi_async_cb)gsdidiag_deactivate_feature_indicator_cb);
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_deactivate_feature_indicator queue failed %d",gsdi_status,0,0);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    if (control_key_info.control_key_p != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( control_key_info.control_key_p);
    }
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_deactivate_feature_indicator */


/*===========================================================================
FUNCTION ggsdidiag_get_feature_indication_cb

DESCRIPTION
    Callback function executed after the get personalization feature indication.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_get_feature_indication_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_perso_value_payload_type event_payload;
    uint8                                   len;
    uint16                                  index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->get_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value1 = 0;
    event_payload.value2 = 0;
    event_payload.value3 = 0;
    event_payload.value4 = 0;
    event_payload.value5 = 0;
    event_payload.value6 = 0;
    event_payload.value7 = 0;
    event_payload.value8 = 0;
    event_payload.value9 = 0;
    event_payload.value10 = 0;
    event_payload.value11 = 0;
    event_payload.umts_valid = FALSE;
    event_payload.ruim_valid = FALSE;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use deact_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_GET_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->get_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
#ifdef FEATURE_PERSO_SIM
        event_payload.value1 = sim_data->get_ind_cnf.nw_ind_status;
        event_payload.value2 = sim_data->get_ind_cnf.ns_ind_status;
        event_payload.value3 = sim_data->get_ind_cnf.sp_ind_status;
        event_payload.value4 = sim_data->get_ind_cnf.cp_ind_status;
        event_payload.value5 = sim_data->get_ind_cnf.sim_ind_status;
        event_payload.umts_valid = TRUE;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        event_payload.value6  = sim_data->get_ind_cnf.ruim_nw1_ind_status;
        event_payload.value7  = sim_data->get_ind_cnf.ruim_nw2_ind_status;
        event_payload.value8  = sim_data->get_ind_cnf.ruim_sp_ind_status;
        event_payload.value9  = sim_data->get_ind_cnf.ruim_cp_ind_status;
        event_payload.value10 = sim_data->get_ind_cnf.ruim_ruim_ind_status;
        event_payload.value11 = sim_data->get_ind_cnf.ruim_hrpd_ind_status;
        event_payload.ruim_valid = TRUE;
#endif /* FEATURE_PERSO_RUIM */
        event_report_payload( EVENT_GSDI_GET_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }
    else
    {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->get_ind_cnf.message_header.gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_GET_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_get_feature_indication_cb */

/*===========================================================================
FUNCTION gsdidiag_get_feature_indication

DESCRIPTION
    Get the feature indication for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_get_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_feature_indication (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_get_feature_ind_req_type *req_ptr;
    gsdidiag_get_feature_ind_rsp_type *rsp_ptr;
    int                                index;
    gsdi_returns_T                     gsdi_status = GSDI_SUCCESS;

    uint32 rsp_len = sizeof( gsdidiag_get_feature_ind_rsp_type );

    MSG_HIGH("GSDI DIAG PERSO GET FEATURE INDICATOR",0,0,0);

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_get_feature_ind_req_type*)req;
    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_get_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_FEATURE_INDICATION_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;


    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    gsdi_status = gsdi_perso_get_feature_indicators(
      (gsdi_slot_id_type)req_ptr->sim_slot_number,
      int32touint32(index),
      (gsdi_async_cb)gsdidiag_get_feature_indication_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_feature_indication queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_get_feature_indication */


/*===========================================================================
FUNCTION gsdidiag_set_feature_indicator_data_cb

DESCRIPTION
    Callback function executed after the set personalization feature data.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_set_feature_indicator_data_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_payload_type event_payload;
    uint8                       len;
    uint16                      index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->set_data_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use set_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_SET_FEATURE_DATA,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->set_data_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_SET_FEATURE_DATA,
                              len,
                              (void *)&event_payload );
    }
    else
    {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->set_data_cnf.message_header.gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_SET_FEATURE_DATA,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /*gsdidiag_set_feature_indicator_data_cb */

/*===========================================================================
FUNCTION gsdidiag_set_feature_indicator_data

DESCRIPTION
    Set the feature data for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_deact_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_set_feature_indicator_data (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_set_feature_ind_data_req_type *req_ptr;
    gsdidiag_set_feature_ind_data_rsp_type *rsp_ptr;
    int                                     index;
    gsdi_returns_T                          gsdi_status = GSDI_SUCCESS;
    gsdi_perso_set_feature_data_type        feature_data;
    uint32                                  perso_feature_data_len;
    uint32                                  rsp_len;


    MSG_HIGH("GSDI DIAG PERSO SET FEATURE INDICATOR DATA",0,0,0);

    memset(&feature_data, 0x00, sizeof(gsdi_perso_set_feature_data_type));

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_set_feature_ind_data_req_type*)req;

  /*Calculate the length of the string - Total length - the header and the other bytes and the
    NULL for the string*/
    perso_feature_data_len = pkt_len - (sizeof(req_ptr->header) + sizeof(req_ptr->sim_slot_number) +
                             sizeof(req_ptr->perso_feature) + GSDIDIAG_NULL_CHAR_LEN);

    rsp_len = sizeof( gsdidiag_set_feature_ind_data_rsp_type )
             + perso_feature_data_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_set_feature_ind_data_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->perso_feature = req_ptr->perso_feature;
    (void)memcpy( (char*) rsp_ptr->perso_feature_data,
            (const char*) req_ptr->perso_feature_data,
            perso_feature_data_len + 1 );

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_set_feature_data_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* --------------------------------------------------------------------------
      Fill In Remainder of Message Contents
      ------------------------------------------------------------------------*/

    gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->perso_feature,
                                        &feature_data.feature);
    if (gsdi_status == GSDI_SUCCESS)
    {
      feature_data.slot            = (gsdi_slot_id_type)req_ptr->sim_slot_number;
      feature_data.num_bytes       = uint32toint32(perso_feature_data_len);
      if (feature_data.num_bytes > 0)
      {
        feature_data.perso_data_p    = mmgsdi_malloc( int32touint32(feature_data.num_bytes));
        if ( feature_data.perso_data_p != NULL )
        {
          (void)memcpy( (char*)feature_data.perso_data_p,
                  (const char*)req_ptr->perso_feature_data,
                  perso_feature_data_len);
        }
        else
        {
          MMGSDI_DEBUG_ERR("feature_data.perso_data_p is NULL",0,0,0);
          gsdi_status = GSDI_ERROR;
        }
      }
      else
      {
        /* user tried to set zero length data */
        feature_data.perso_data_p = NULL;
      }

      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if(gsdi_status == GSDI_SUCCESS)
        gsdi_status = gsdi_perso_set_feature_data(&feature_data,
                                              int32touint32(index),
                                              (gsdi_async_cb)gsdidiag_set_feature_indicator_data_cb);
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_set_feature_indicator_data queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    if (feature_data.perso_data_p != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( feature_data.perso_data_p);
    }
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_set_feature_indicator_data */


/*===========================================================================
FUNCTION gsdidiag_unblock_feature_ind_cb

DESCRIPTION
    Callback function executed after the apdu is executed. Sends out event
    with information about request execution. Cleans up memory used for
    this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_unblock_feature_ind_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8                             len;
    uint16                            index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->unblock_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;
    event_payload.value = (uint8)sim_data->unblock_ind_cnf.perso_feature;
    event_payload.value2 = uint32touint8(sim_data->unblock_ind_cnf.num_retries);

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use sendapdu_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_UNBLOCK_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->unblock_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_UNBLOCK_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }
    else
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(
        sim_data->unblock_ind_cnf.message_header.gsdi_status, &event_payload.status);
      event_report_payload( EVENT_GSDI_UNBLOCK_FEATURE_IND,
                            len,
                            (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_unblock_feature_ind_cb */

/*===========================================================================
FUNCTION gsdidiag_unblock_feature_ind

DESCRIPTION
    Unblock the feature indicator control key after the feature indicator
    has been blocked

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_unblock_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_unblock_feature_ind (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_unblock_feature_ind_req_type *req_ptr;
    gsdidiag_unblock_feature_ind_rsp_type *rsp_ptr;
    int                                    index;
    gsdi_returns_T                         gsdi_status = GSDI_SUCCESS;
    gsdi_perso_unblock_feature_ind_req_T   perso_unblock_req;
    uint32                                 rsp_len;
    uint32                                 perso_feature_key_len;

    MSG_HIGH("GSDI DIAG PERSO UNBLOCK FEATURE INDICATOR",0,0,0);


    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_unblock_feature_ind_req_type*)req;

    perso_feature_key_len = strlen((char*)req_ptr->perso_feature_key);

    if (perso_feature_key_len > GSDI_PERSO_MAX_CK)
    {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Key too long(%d). Max length=%d",
                  perso_feature_key_len, GSDI_PERSO_MAX_CK, 0);
        return NULL;
    }

    rsp_len = sizeof( gsdidiag_unblock_feature_ind_rsp_type )
                      + perso_feature_key_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_unblock_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->perso_feature = req_ptr->perso_feature;
    (void)memcpy( (char*) rsp_ptr->perso_feature_key,
            (const char*) req_ptr->perso_feature_key,
            perso_feature_key_len + 1 ); /*lint !e669 !e670 */

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;


    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_unblock_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    /* --------------------------------------------------------------------------
      Fill In Message Header Contents
      ------------------------------------------------------------------------*/
    perso_unblock_req.message_header.message_set = MS_GSDI;
    perso_unblock_req.message_header.message_id  = GSDI_PERSO_UNBLOCK_FEATURE_IND_REQ;
    perso_unblock_req.message_header.slot        = (gsdi_slot_id_type)req_ptr->sim_slot_number;
    perso_unblock_req.message_header.gsdi_async_callback = (gsdi_async_cb)gsdidiag_unblock_feature_ind_cb;
#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) ||  \
    defined(FEATURE_MMGSDI_UMTS))
    ((IMH_T *)(&perso_unblock_req.message_header))->message_len_msb =
      (unsigned char)((sizeof( gsdi_perso_unblock_feature_ind_req_T ) -
      sizeof(gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)(&perso_unblock_req.message_header))->message_len_lsb =
      (unsigned char)((sizeof( gsdi_perso_unblock_feature_ind_req_T ) -
      sizeof(gsdi_cmdhdr_T)) % 0x100);
#else
    GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_unblock_feature_ind_req_T ) -
      sizeof(gsdi_cmdhdr_T), &perso_unblock_req.message_header );
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) ||
        defined(FEATURE_MMGSDI_UMTS))*/

    /* --------------------------------------------------------------------------
      Fill In Remainder of Message Contents
      ------------------------------------------------------------------------*/

    gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->perso_feature,
                                        &perso_unblock_req.feature);
    if (gsdi_status == GSDI_SUCCESS)
    {
      perso_unblock_req.client_ref      = int32touint32(index);
      perso_unblock_req.num_bytes       = uint32toint32(perso_feature_key_len);
        (void)memcpy( (char*)perso_unblock_req.unblock_ck_buffer,
                (const char*)req_ptr->perso_feature_key,
              perso_feature_key_len); /*lint !e670 */


      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if(gsdi_status == GSDI_SUCCESS) /*lint !e774 */
      {
        gsdi_status = gsdi_cmd_ext(&perso_unblock_req);
      }

    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_unblock_feature_ind queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_unblock_feature_ind */


/*===========================================================================
FUNCTION gsdidiag_get_feature_indicator_key_cb

DESCRIPTION
    Callback function executed after the apdu is executed. Sends out event
    with information about request execution. Cleans up memory used for
    this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_get_feature_indicator_key_cb(gsdi_cnf_T* sim_data)
{

    gsdidiag_event_payload_type event_payload;
    gsdi_returns_T              gsdi_status = GSDI_ERROR;
    uint8                       len;
    uint16                      index;

    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->get_ind_key.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use getfeaturekey_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_GET_CONTROL_KEY,
                              len,
                              (void *)&event_payload );
        return;
    }

    /* check if gsdi_get_feature_indicator_key was successfull */
    if (sim_data->get_ind_key.message_header.gsdi_status == GSDI_SUCCESS)
    {
        /* write the data */
        if (req_table[index].var[0] == NULL)
        {
          MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use getfeaturekey_cb.",index,0,0);
          event_payload.status = GSDIDIAG_MEMORY_ERR;
          event_report_payload( EVENT_GSDI_GET_CONTROL_KEY,
                                len,
                                (void *)&event_payload );
          gsdidiag_free_index_memory(index);
          gsdidiag_free_index(index);
          return;
        }
        gsdi_status = gsdi_efs_write_file((const char*)req_table[index].var[0],
                                      sim_data->get_ind_key.key_len,
                                      sim_data->get_ind_key.key);
        if ( gsdi_status != GSDI_SUCCESS )
        {
          MMGSDI_DEBUG_MSG_ERROR("fs_write failed. 0x%x.",gsdi_status,0,0);
          event_payload.status = GSDIDIAG_EFS_WRITE_ERR;
        }
        else
        {
          event_payload.status = GSDIDIAG_SUCCESS;
        }
        event_report_payload( EVENT_GSDI_GET_CONTROL_KEY,
                              len,
                              (void *)&event_payload );
    }
    else
    {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_GET_CONTROL_KEY,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_get_feature_indicator_key_cb */

/*===========================================================================
FUNCTION gsdidiag_get_feature_indicator_key

DESCRIPTION
    Unblock the feature indicator control key after the feature indicator
    has been blocked

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_unblock_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_feature_indicator_key (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_get_feature_key_req_type *req_ptr;
    gsdidiag_get_feature_key_rsp_type *rsp_ptr;
    int                                index;
    gsdi_returns_T                     gsdi_status = GSDI_SUCCESS;
    gsdi_perso_get_feature_key_req_T   perso_get_key_req;
    uint32                             filename_len;
    uint32                             rsp_len;
    mmgsdi_return_enum_type            mmgsdi_status     = MMGSDI_ERROR;

    MSG_HIGH("GSDI DIAG PERSO GET FEATURE KEY",0,0,0);


    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    if(pkt_len < sizeof(gsdidiag_get_feature_key_req_type))
    {
        MSG_ERROR("Invalid pkt_len: 0x%x",pkt_len,0,0);
        return NULL;
    }

    /* Allocate one additional byte in case EFS filename string is not 
    null-terminated */
    MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

    if (mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
    {
      MSG_ERROR("gsdidiag_sim_write malloc err req_ptr",0,0,0);
      return NULL;
    }
    else
    {
      memcpy((void *)req_ptr,(void *)req,pkt_len);
    }

    filename_len = strlen((const char*)req_ptr->efs_filename);

    /* check if efs filename is too long*/
    if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
    {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Filename too long(%d). Max length=%d",
                  filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return NULL;
    }

    rsp_len = sizeof(gsdidiag_get_feature_key_rsp_type) + filename_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_get_feature_key_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
    {
      MSG_ERROR("gsdidiag_get_feature_indicator_key: Unable to allocate memory",
                 0, 0, 0);
      MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return NULL;
    }

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->feature = req_ptr->feature;

    /* Copy efs file name to response structure
       No check here because the source string length is what is
       being used to determine the destination length*/
    (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                       (const char*)req_ptr->efs_filename,
                       uint32toint32(filename_len + GSDIDAG_NULL_CHAR_LEN));

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    /* allocate memory for filename  */
    req_table[index].var[0] = gsdidiag_sim_malloc_append_efs_dir((char *)req_ptr->efs_filename);
    if (req_table[index].var[0]==NULL) {
        /* mmgsdi_malloc failed */
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        gsdidiag_free_index(index);
        rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
        diagpkt_commit(rsp_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return rsp_ptr;
    }

    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_feature_indicator_key param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
        return rsp_ptr;
    }

    /* --------------------------------------------------------------------------
      Fill In Message Header Contents
      ------------------------------------------------------------------------*/
    perso_get_key_req.message_header.message_set = MS_GSDI;
    perso_get_key_req.message_header.message_id  = GSDI_PERSO_GET_FEATURE_KEY_REQ;
    perso_get_key_req.message_header.slot        = (gsdi_slot_id_type)req_ptr->sim_slot_number;
    perso_get_key_req.message_header.gsdi_async_callback = gsdidiag_get_feature_indicator_key_cb;
#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) ||  \
    defined(FEATURE_MMGSDI_UMTS))
    ((IMH_T *)(&perso_get_key_req.message_header))->message_len_msb =
      (unsigned char)((sizeof( gsdi_perso_get_feature_key_req_T ) -
      sizeof(gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)(&perso_get_key_req.message_header))->message_len_lsb =
      (unsigned char)((sizeof( gsdi_perso_get_feature_key_req_T ) -
      sizeof(gsdi_cmdhdr_T)) % 0x100);
#else
    GSDI_PUT_IMH_LEN( sizeof( gsdi_perso_get_feature_key_req_T ) -
      sizeof(gsdi_cmdhdr_T), &perso_get_key_req.message_header );
#endif /*#if defined(FEATURE_MMGSDI_CDMA) && !(defined(FEATURE_MMGSDI_GSM) ||
        defined(FEATURE_MMGSDI_UMTS))*/

    /* --------------------------------------------------------------------------
      Fill In Remainder of Message Contents
      ------------------------------------------------------------------------*/

    gsdi_status = GSDI_SUCCESS;

    switch (req_ptr->feature) {
#ifdef FEATURE_PERSO_SIM
    case GSDIDIAG_NW_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_NW;
      break;
    case GSDIDIAG_NS_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_NS;
      break;
    case GSDIDIAG_SP_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_SP;
      break;
    case GSDIDIAG_CP_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_CP;
      break;
    case GSDIDIAG_SIM_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_SIM;
      break;
    case GSDIDIAG_NW_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_NW;
      break;
    case GSDIDIAG_NS_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_NS;
      break;
    case GSDIDIAG_SP_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_SP;
      break;
    case GSDIDIAG_CP_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_CP;
      break;
    case GSDIDIAG_SIM_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_SIM;
      break;
#endif /*FEATURE_PERSO_SIM*/
#ifdef FEATURE_PERSO_RUIM
    case GSDIDIAG_RUIM_NW1_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_NW1;
      break;
    case GSDIDIAG_RUIM_NW2_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_NW2;
      break;
    case GSDIDIAG_RUIM_SP_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_SP;
      break;
    case GSDIDIAG_RUIM_CP_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_CP;
      break;
    case GSDIDIAG_RUIM_RUIM_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_RUIM;
      break;
    case GSDIDIAG_RUIM_HRPD_UNBLOCK_KEY:
      perso_get_key_req.is_unblock_key = TRUE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_HRPD;
      break;
    case GSDIDIAG_RUIM_NW1_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_NW1;
      break;
    case GSDIDIAG_RUIM_NW2_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_NW2;
      break;
    case GSDIDIAG_RUIM_SP_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_SP;
      break;
    case GSDIDIAG_RUIM_CP_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_CP;
      break;
    case GSDIDIAG_RUIM_RUIM_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_RUIM;
      break;
    case GSDIDIAG_RUIM_HRPD_KEY:
      perso_get_key_req.is_unblock_key = FALSE;
      perso_get_key_req.feature = GSDI_PERSO_RUIM_HRPD;
      break;
#endif /*FEATURE_PERSO_RUIM*/
    default:
      MMGSDI_DEBUG_MSG_ERROR("Unmatch key request 0x%x", req_ptr->feature, 0, 0);
      gsdi_status = GSDI_ERROR;
    }
    if (gsdi_status == GSDI_SUCCESS)
    {
      perso_get_key_req.client_ref      = int32touint32(index);

      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if( gsdi_status == GSDI_SUCCESS ) /*lint !e774 */
      {
        gsdi_status = gsdi_cmd_ext(&perso_get_key_req);
      }
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_feature_indicator_key queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return rsp_ptr;
} /* gsdidiag_get_feature_indicator_key */



/*===========================================================================
FUNCTION gsdidiag_get_perm_feature_indication_cb

DESCRIPTION
    Callback function executed after the get permanent personalization feature
    indication.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_get_perm_feature_indication_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_perso_value_payload_type event_payload;
    uint8                                   len;
    uint16                                  index;


    if(sim_data == NULL)
    {
        MSG_ERROR("sim_data ERR:NULL",0,0,0);
        return;
    }

    len = sizeof(event_payload);
    index = uint32touint16(sim_data->get_perm_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.value1 = 0;
    event_payload.value2 = 0;
    event_payload.value3 = 0;
    event_payload.value4 = 0;
    event_payload.value5 = 0;
    event_payload.value6 = 0;
    event_payload.value7 = 0;
    event_payload.value8 = 0;
    event_payload.value9 = 0;
    event_payload.value10 = 0;
    event_payload.value11 = 0;
    event_payload.umts_valid = FALSE;
    event_payload.ruim_valid = FALSE;
    event_payload.status = GSDIDIAG_SUCCESS;

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use deact_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_GET_PERM_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_sendapdu was successfull */
    if (sim_data->get_perm_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
#ifdef FEATURE_PERSO_SIM
        event_payload.value1 = sim_data->get_perm_ind_cnf.perm_nw_ind_status;
        event_payload.value2 = sim_data->get_perm_ind_cnf.perm_ns_ind_status;
        event_payload.value3 = sim_data->get_perm_ind_cnf.perm_sp_ind_status;
        event_payload.value4 = sim_data->get_perm_ind_cnf.perm_cp_ind_status;
        event_payload.value5 = sim_data->get_perm_ind_cnf.perm_sim_ind_status;
        event_payload.umts_valid  = TRUE;
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
        event_payload.value6 = sim_data->get_perm_ind_cnf.perm_ruim_nw1_ind_status;
        event_payload.value7 = sim_data->get_perm_ind_cnf.perm_ruim_nw2_ind_status;
        event_payload.value8 = sim_data->get_perm_ind_cnf.perm_ruim_sp_ind_status;
        event_payload.value9 = sim_data->get_perm_ind_cnf.perm_ruim_cp_ind_status;
        event_payload.value10 = sim_data->get_perm_ind_cnf.perm_ruim_ruim_ind_status;
        event_payload.value11 = sim_data->get_perm_ind_cnf.perm_ruim_hrpd_ind_status;
        event_payload.ruim_valid = TRUE;
#endif /* FEATURE_PERSO_RUIM */
        event_report_payload( EVENT_GSDI_GET_PERM_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }
    else
    {
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->get_perm_ind_cnf.message_header.gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_GET_PERM_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_get_perm_feature_indication_cb */

/*===========================================================================
FUNCTION gsdidiag_get_perm_feature_indication

DESCRIPTION
    Get the feature indication for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_get_perm_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_perm_feature_indication (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_get_perm_feature_ind_req_type *req_ptr;
    gsdidiag_get_perm_feature_ind_rsp_type *rsp_ptr;
    int index;
    gsdi_returns_T gsdi_status = GSDI_SUCCESS;

    uint32 rsp_len = sizeof( gsdidiag_get_perm_feature_ind_rsp_type );

    MSG_HIGH("GSDI DIAG PERSO GET PERM FEATURE INDICATOR",0,0,0);

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_get_perm_feature_ind_req_type*)req;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_get_perm_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;

    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_perm_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    gsdi_status = gsdi_perso_get_perm_feature_indicators(
      (gsdi_slot_id_type)req_ptr->sim_slot_number,
      int32touint32(index),
      (gsdi_async_cb)gsdidiag_get_perm_feature_indication_cb);

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_perm_feature_indication queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_get_perm_feature_indication */


/*===========================================================================
FUNCTION gsdidiag_disable_perm_feature_indicator_cb

DESCRIPTION
    Callback function executed after the permanently disable personalization.
    Sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_disable_perm_feature_indicator_cb(const gsdi_cnf_T* sim_data)
{

    gsdidiag_event_value_payload_type event_payload;
    uint8                             len;
    uint16                            index;

    if(sim_data == NULL)
    {
      MSG_ERROR("sim_data ERR:NULL",0,0,0);
      return;
    }

    len = sizeof(event_payload);

    index = uint32touint16(sim_data->disable_ind_cnf.message_header.client_ref);

    /* initilialize */
    event_payload.transaction_id = req_table[index].transaction_id;
    event_payload.status = GSDIDIAG_SUCCESS;
    event_payload.value = (uint8)sim_data->disable_ind_cnf.perso_feature;
    event_payload.value2 = uint32touint8(sim_data->disable_ind_cnf.num_retries);

    /* req_table[index] should be in the used state */
    if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
    {
        MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use perm_disable_cb.",index,0,0);
        event_payload.status = GSDIDIAG_MEMORY_ERR;
        event_report_payload( EVENT_GSDI_PERM_DISBALE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
        return;
    }
    /* check if gsdi_perm_disable was successfull */
    if (sim_data->disable_ind_cnf.message_header.gsdi_status == GSDI_SUCCESS)
    {
        event_payload.status = GSDIDIAG_SUCCESS;
        event_report_payload( EVENT_GSDI_PERM_DISBALE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }else{
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(
          sim_data->disable_ind_cnf.message_header.gsdi_status, &event_payload.status);
        event_report_payload( EVENT_GSDI_PERM_DISBALE_FEATURE_IND,
                              len,
                              (void *)&event_payload );
    }

    /* deallocate memory used for passing the apdu */
    gsdidiag_free_index_memory(index);

    gsdidiag_free_index(index);
} /* gsdidiag_disable_perm_feature_indicator_cb */

/*===========================================================================
FUNCTION gsdidiag_disable_perm_feature_indicator

DESCRIPTION
    Permanently disable the feature for personalization

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_disable_perm_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_disable_perm_feature_indicator (
    PACKED void *req,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{

    gsdidiag_disable_perm_feature_ind_req_type *req_ptr;
    gsdidiag_disable_perm_feature_ind_rsp_type *rsp_ptr;
    int                                         index;
    gsdi_returns_T                              gsdi_status = GSDI_SUCCESS;
    gsdi_perso_control_key_data_type            control_key_info;
    uint32                                      rsp_len;
    uint32                                      perso_feature_key_len;

    MSG_HIGH("GSDI DIAG PERSO PERM DISABLE FEATURE INDICATOR",0,0,0);

    if(req == NULL)
    {
        MSG_ERROR("req ERR:NULL",0,0,0);
        return NULL;
    }

    req_ptr = (gsdidiag_disable_perm_feature_ind_req_type*)req;

    perso_feature_key_len = strlen((char*)req_ptr->perso_feature_key);

    if (perso_feature_key_len > GSDI_PERSO_MAX_CK)
    {
        /* return error */
        MMGSDI_DEBUG_MSG_ERROR("Key too long(%d). Max length=%d",
                  perso_feature_key_len, GSDI_PERSO_MAX_CK, 0);
        return NULL;
    }

    rsp_len = sizeof( gsdidiag_disable_perm_feature_ind_rsp_type )
                      + perso_feature_key_len;

    /* allocate the rsp pointer*/
    rsp_ptr = (gsdidiag_disable_perm_feature_ind_rsp_type *)diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_GSDI,
                                   (uint16)GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD,
                                   rsp_len );
    if (rsp_ptr==NULL)
        return NULL;

    /*copy req_ptr to rsp_ptr*/
    rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
    rsp_ptr->perso_feature = req_ptr->perso_feature;
    (void)memcpy( (char*) rsp_ptr->perso_feature_key,
            (const char*) req_ptr->perso_feature_key,
            perso_feature_key_len + 1); /*lint !e669 !e670 */

    /* store this request information in req_table */
    index = gsdidiag_get_index();
    if (index == GSDIDIAG_GET_FN_ERR)
    {
        /*req_table if full*/
        rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
    req_table[index].transaction_id = rsp_ptr->transaction_id;


    if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
       && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
    )
    {
      MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
      gsdi_status = GSDI_INCORRECT_PARAMS;
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_disable_perm_feature_ind param chk failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
        gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
        diagpkt_commit(rsp_ptr);
        return rsp_ptr;
    }

    memset(&control_key_info,0,sizeof(gsdi_perso_control_key_data_type));
    /* --------------------------------------------------------------------------
      Map the feature indicator
      ------------------------------------------------------------------------*/

    gsdi_status = gsdidiag_map_perso_feature((gsdidiag_perso_feature_type)req_ptr->perso_feature,
                                        &control_key_info.feature);
    if (gsdi_status == GSDI_SUCCESS)
    {
      control_key_info.slot            = (gsdi_slot_id_type)req_ptr->sim_slot_number;
      control_key_info.num_bytes       = uint32toint32(perso_feature_key_len);
      control_key_info.control_key_p   = mmgsdi_malloc( perso_feature_key_len);
      if ( control_key_info.control_key_p != NULL )
      {
        (void)memcpy( (char*)control_key_info.control_key_p,
                (const char*)req_ptr->perso_feature_key,
                perso_feature_key_len); /*lint !e670 */
      }
      else
      {
        MMGSDI_DEBUG_ERR("control_key_p.control_key_p is NULL",0,0,0);
        gsdi_status = GSDI_ERROR;
      }

      /* --------------------------------------------------------------------------
        Queue Command with GSDI Task.
        Reminder:  Memory for Message Queue is allocated in the callers task
                    context but then freed in gsdi when the message is removed
                    from the Queue for further processing.
        ------------------------------------------------------------------------*/
      if(gsdi_status == GSDI_SUCCESS)
        gsdi_status = gsdi_perso_perm_disable_feature_indicator(&control_key_info,
                                                           int32touint32(index),
                                                           (gsdi_async_cb)gsdidiag_disable_perm_feature_indicator_cb);
    }

    if (gsdi_status != GSDI_SUCCESS )
    {
        MMGSDI_DEBUG_MSG_ERROR("gsdidiag_disable_perm_feature_indicator queue failed %d",gsdi_status,0,0);
        gsdidiag_free_index_memory(index);
        gsdidiag_free_index(index);
    }

    if (control_key_info.control_key_p != NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE( control_key_info.control_key_p);
    }
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    return rsp_ptr;
} /* gsdidiag_disable_perm_feature_indicator */


/*===========================================================================
FUNCTION gsdidiag_get_feature_indicator_data_cb

DESCRIPTION
    Callback function executed after the get personalization feature data
    sends out event with information about request execution. Cleans up
    memory used for this req in req_table.

DEPENDENCIES
    req_table[sim_data->message_header.client_ref] has been populated

RETURN VALUE
    None

SIDE EFFECTS
    Event
===========================================================================*/
static void gsdidiag_get_feature_indicator_data_cb(
  const gsdi_cnf_T *sim_data_ptr
)
{
  gsdidiag_event_payload_type event_payload = {0};
  gsdi_returns_T              gsdi_status   = GSDI_ERROR;
  uint8                       len           = 0;
  uint16                      index         = 0;

  if(sim_data_ptr == NULL)
  {
    MSG_ERROR("sim_data ERR:NULL",0,0,0);
    return;
  }

  len = sizeof(event_payload);
  index = uint32touint16(sim_data_ptr->get_data_cnf.message_header.client_ref);

  /* initialize */
  event_payload.transaction_id = req_table[index].transaction_id;
  event_payload.status = GSDIDIAG_SUCCESS;

  /* req_table[index] should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use getdata_cb.",index,0,0);
    event_payload.status = GSDIDIAG_MEMORY_ERR;
    event_report_payload( EVENT_GSDI_GET_FEATURE_INDICATOR_DATA,
                          len,
                          (void *)&event_payload );
    return;
  }

  /* check if gsdidiag_get_feature_indicator_data was successfull */
  if (sim_data_ptr->get_data_cnf.message_header.gsdi_status == GSDI_SUCCESS)
  {
    /* write the data */
    if (req_table[index].var[0] == NULL)
    {
      MMGSDI_DEBUG_MSG_ERROR("Index %d is not in use getdata_cb",index,0,0);
      event_payload.status = GSDIDIAG_MEMORY_ERR;
      event_report_payload( EVENT_GSDI_GET_FEATURE_INDICATOR_DATA,
                            len,
                            (void *)&event_payload );
      gsdidiag_free_index_memory(index);
      gsdidiag_free_index(index);
      return;
    }
    gsdi_status = gsdi_efs_write_file( (const char*)req_table[index].var[0],
                                       sim_data_ptr->get_data_cnf.num_bytes,
                                       sim_data_ptr->get_data_cnf.data_buffer_ptr );
    if ( gsdi_status != GSDI_SUCCESS )
    {
      MMGSDI_DEBUG_MSG_ERROR("fs_write failed. 0x%x.",gsdi_status,0,0);
      event_payload.status = GSDIDIAG_EFS_WRITE_ERR;
    }
    else
    {
      event_payload.status = GSDIDIAG_SUCCESS;
    }
    event_report_payload( EVENT_GSDI_GET_FEATURE_INDICATOR_DATA,
                          len,
                          (void *)&event_payload );
  }
  else
  {
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &event_payload.status);
    event_report_payload( EVENT_GSDI_GET_FEATURE_INDICATOR_DATA,
                          len,
                          (void *)&event_payload );
  }

  /* deallocate the memory used for EFS data */
  gsdidiag_free_index_memory((int)index);

  gsdidiag_free_index((int)index);
} /* gsdidiag_get_feature_indicator_data_cb */


/*===========================================================================
FUNCTION gsdidiag_get_feature_indicator_data

DESCRIPTION
    Get the personalization feature data.

ARGUMENTS
    PACKED void *req: pointer to request packet of type
                      gsdidiag_unblock_feature_ind_req_type
    word pkt_len    : length of request packet

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static PACKED void * gsdidiag_get_feature_indicator_data (
    PACKED void *packed_req_ptr,  /* pointer to request packet  */
    word pkt_len       /* length of request packet   */
)
{
  gsdidiag_get_feature_ind_data_req_type *req_ptr     = NULL;
  gsdidiag_get_feature_ind_data_rsp_type *rsp_ptr     = NULL;
  int                                    index        = 0;
  gsdi_returns_T                         gsdi_status  = GSDI_SUCCESS;
  gsdi_perso_enum_type                   feature      = GSDI_PERSO_NONE;/* Perso Feature affected    */
  uint32                                 filename_len = 0;
  uint32                                 rsp_len      = 0;
  mmgsdi_return_enum_type                mmgsdi_status  = MMGSDI_ERROR;

  MSG_HIGH("GSDI DIAG PERSO GET FEATURE INDICATOR DATA",0,0,0);


  if(packed_req_ptr == NULL)
  {
    MSG_ERROR("packed_req_ptr ERR:NULL",0,0,0);
    return NULL;
  }

  if(pkt_len < sizeof(gsdidiag_get_feature_ind_data_req_type))
  {
    MSG_ERROR("Invalid pkt_len: 0x%x",pkt_len,0,0);
    return NULL;
  }

  /* Allocate one additional byte in case EFS filename string is not 
    null-terminated */
  MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE(req_ptr,pkt_len+1,mmgsdi_status);

  if (mmgsdi_status != MMGSDI_SUCCESS || req_ptr == NULL)
  {
    MSG_ERROR("gsdidiag_sim_write malloc err req_ptr",0,0,0);
    return NULL;
  }
  else
  {
    memcpy((void *)req_ptr,(void *)packed_req_ptr,pkt_len);
  }

  filename_len = strlen((const char*)req_ptr->efs_filename);

  /* check if efs filename is too long*/
  if (filename_len > GSDIDIAG_MAX_FILENAME_LENGTH)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("Filename too long(0x%x). Max length=0x%x",
              filename_len, GSDIDIAG_MAX_FILENAME_LENGTH, 0);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return NULL;
  }

  rsp_len = sizeof( gsdidiag_get_feature_ind_data_rsp_type ) + filename_len;

  /* allocate the rsp pointer*/
  rsp_ptr = (gsdidiag_get_feature_ind_data_rsp_type *)diagpkt_subsys_alloc(
                                 (uint8)DIAG_SUBSYS_GSDI,
                                 (uint16)GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD,
                                 rsp_len );
  if (rsp_ptr==NULL)
  {
    MSG_ERROR("rsp_ptr ERR:NULL",0,0,0);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return NULL;
  }

  /*copy req_ptr to rsp_ptr*/
  rsp_ptr->sim_slot_number = req_ptr->sim_slot_number;
  rsp_ptr->feature = req_ptr->feature;

  /* Copy efs file name to response structure
     No check here because the source string length is what is
     being used to determine the destination length*/
  (void)std_strlcpy( (char*)rsp_ptr->efs_filename,
                     (const char*)req_ptr->efs_filename,
                     uint32toint32(filename_len + GSDIDAG_NULL_CHAR_LEN));

  /* store this request information in req_table */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR)
  {
    /*req_table if full*/
    rsp_ptr->status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    diagpkt_commit(rsp_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return rsp_ptr;
  }

  rsp_ptr->transaction_id = int32touint16(gsdidiag_get_transaction_id());
  req_table[index].transaction_id = rsp_ptr->transaction_id;

  /* allocate memory for filename  */
  req_table[index].var[0] = gsdidiag_sim_malloc_append_efs_dir(
                              (char *)req_ptr->efs_filename);
  if (req_table[index].var[0] == NULL)
  {
    /* mmgsdi_malloc failed */
    MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
    gsdidiag_free_index(index);
    rsp_ptr->status = GSDIDIAG_MEMORY_ERR;
    diagpkt_commit(rsp_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return rsp_ptr;
  }

  if ( req_ptr->sim_slot_number != (uint8)GSDI_SLOT_1
#ifdef FEATURE_MMGSDI_DUAL_SLOT
     && req_ptr->sim_slot_number != (uint8)GSDI_SLOT_2
#endif /* FEATURE_MMGSDI_DUAL_SLOT */
  )
  {
    MMGSDI_DEBUG_MSG_ERROR("BAD SLOT ID: %x",req_ptr->sim_slot_number,0,0);
    gsdi_status = GSDI_INCORRECT_PARAMS;
  }

  if ( gsdi_status == GSDI_SUCCESS )
  {
    gsdi_status = gsdidiag_map_perso_feature(
                    (gsdidiag_perso_feature_type)req_ptr->feature,
                    &feature);
  }


  if ( gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR(
      "gsdidiag_get_feature_indicator_data param chk failed 0x%x",
      gsdi_status,0,0);
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);
    gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
    diagpkt_commit(rsp_ptr);
    MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
    return rsp_ptr;
  }

  /* --------------------------------------------------------------------------
  Queue Command with GSDI Task.
  Reminder:  Memory for Message Queue is allocated in the callers task
              context but then freed in gsdi when the message is removed
              from the Queue for further processing.
  ------------------------------------------------------------------------*/

  gsdi_status = gsdi_perso_get_feature_data( feature,
                                 (gsdi_slot_id_type)req_ptr->sim_slot_number,
                                 int32touint32(index),
                                 (gsdi_async_cb)gsdidiag_get_feature_indicator_data_cb);

  if (gsdi_status != GSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("gsdidiag_get_feature_indicator_data queue failed 0x%x",
                           gsdi_status,0,0);
    gsdidiag_free_index_memory(index);
    gsdidiag_free_index(index);
  }

  gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status, &rsp_ptr->status);
  diagpkt_commit(rsp_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(req_ptr);
  return rsp_ptr;
} /* gsdidiag_get_feature_indicator_data */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */


/*===========================================================================
FUNCTION gsdidiag_mmgsdi_client_id_reg_cb

DESCRIPTION
  This function handles the client ID registration command callback.  It will
  assign the global client ID value which will be used in the mmgsdi function
  call

INPUT
  mmgsdi_return_enum_type       status:  status of command
  mmgsdi_cnf_enum_type          cnf:     confirmation type
  const mmgsdi_cnf_type        *cnf_ptr: confirmation data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gsdidiag_mmgsdi_client_id_reg_cb (
  mmgsdi_return_enum_type       status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr)

{
  if(cnf_ptr == NULL)
  {
    MSG_ERROR("cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  if (status == MMGSDI_SUCCESS)
  {
    if (cnf == MMGSDI_CLIENT_ID_REG_CNF)
    {
      gsdidiag_mmgsdi_client_id =
        cnf_ptr->client_id_reg_cnf.response_header.client_id;
      MSG_MED("GSDI DIAG Client ID 0x%x",gsdidiag_mmgsdi_client_id,0,0);
    }
  }
}

/*===========================================================================
FUNCTION gsdidiag_util_convert_sim_events_to_gsdidiag_event

DESCRIPTION
  This function converts the gsdi_sim_events_T to the corresponding
  gsdidiag sim event.

ARGUMENTS
  gsdi_sim_events_T sim_event: the event to be mapped
  gsdidiag_card_evt_enum_type *event_notified_p: the gsdidiag event pointer,
                                                 the event to be mapped to.

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
static gsdi_returns_T gsdidiag_util_convert_sim_events_to_gsdidiag_event (
    gsdi_sim_events_T sim_event,
    gsdidiag_card_evt_enum_type *event_notified_p )
{
  if (event_notified_p == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR ("Not able to assign event", 0, 0, 0);
    return GSDI_ERROR;
  }

  *event_notified_p = GSDIDIAG_NO_EVENT_TO_DIAG;

  switch (sim_event)
  {
    case GSDI_SIM_INSERTED:
      *event_notified_p = GSDIDIAG_SIM_INSERTED;

      /* Register for client ID */
      if (gsdidiag_mmgsdi_client_id == 0)
      {
        (void)mmgsdi_client_id_reg (
         (mmgsdi_callback_type)gsdidiag_mmgsdi_client_id_reg_cb,
         10);
      }
      break;
    case GSDI_SIM_REMOVED:
      *event_notified_p = GSDIDIAG_SIM_REMOVED;
      break;
    case GSDI_DRIVER_ERROR:
      *event_notified_p = GSDIDIAG_UIM_DRIVER_ERROR;
      break;
    case GSDI_CARD_ERROR:
      *event_notified_p = GSDIDIAG_CARD_ERROR;
      break;
    case GSDI_MEMORY_WARNING:
      *event_notified_p = GSDIDIAG_MEMORY_WARNING;
      break;
    case GSDI_NO_SIM_EVENT:
      *event_notified_p = GSDIDIAG_NO_SIM_EVENT;
      break;
    case GSDI_NO_SIM:
      *event_notified_p = GSDIDIAG_NO_SIM;
      break;
    case GSDI_SIM_INIT_COMPLETED:
      *event_notified_p = GSDIDIAG_SIM_INIT_COMPLETED;
      break;
    case GSDI_SIM_INIT_COMPLETED_NO_PROV:
      *event_notified_p = GSDIDIAG_SIM_INIT_COMPLETED_NO_PROV;
      break;
    case GSDI_PIN1_VERIFIED:
      *event_notified_p = GSDIDIAG_PIN1_VERIFIED;
      break;
    case GSDI_PIN1_BLOCKED:
      *event_notified_p = GSDIDIAG_PIN1_BLOCKED;
      break;
    case GSDI_PIN1_UNBLOCKED:
      *event_notified_p = GSDIDIAG_PIN1_UNBLOCKED;
      break;
    case GSDI_PIN1_ENABLED:
      *event_notified_p = GSDIDIAG_PIN1_ENABLED;
      break;
    case GSDI_PIN1_DISABLED:
      *event_notified_p = GSDIDIAG_PIN1_DISABLED;
      break;
    case GSDI_PIN1_CHANGED:
      *event_notified_p = GSDIDIAG_PIN1_CHANGED;
      break;
    case GSDI_PIN1_PERM_BLOCKED:
      *event_notified_p = GSDIDIAG_PIN1_PERM_BLOCKED;
      break;
    case GSDI_PIN2_VERIFIED:
      *event_notified_p = GSDIDIAG_PIN2_VERIFIED;
      break;
    case GSDI_PIN2_BLOCKED:
      *event_notified_p = GSDIDIAG_PIN2_BLOCKED;
      break;
    case GSDI_PIN2_UNBLOCKED:
      *event_notified_p = GSDIDIAG_PIN2_UNBLOCKED;
      break;
    case GSDI_PIN2_ENABLED:
      *event_notified_p = GSDIDIAG_PIN2_ENABLED;
      break;
    case GSDI_PIN2_DISABLED:
      *event_notified_p = GSDIDIAG_PIN2_DISABLED;
      break;
    case GSDI_PIN2_CHANGED:
      *event_notified_p = GSDIDIAG_PIN2_CHANGED;
      break;
    case GSDI_PIN2_PERM_BLOCKED:
      *event_notified_p = GSDIDIAG_PIN2_PERM_BLOCKED;
      break;
    case GSDI_NO_EVENT:
      /* No event to be generated */
      break;
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL:
      *event_notified_p = GSDIDIAG_OK_FOR_TP_DL;
      break;
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL:
      *event_notified_p = GSDIDIAG_NOT_OK_FOR_TP_DL;
      break;
    case GSDI_REFRESH_SIM_RESET:
      *event_notified_p = GSDIDIAG_REFRESH_SIM_RESET;
      break;
    case GSDI_REFRESH_SIM_INIT:
      *event_notified_p = GSDIDIAG_REFRESH_SIM_INIT;
      break;
    case GSDI_FDN_ENABLE:
      *event_notified_p = GSDIDIAG_FDN_ENABLE;
      break;
    case GSDI_FDN_DISABLE:
      *event_notified_p = GSDIDIAG_FDN_DISABLE;
      break;
    case GSDI_SIM_INSERTED_2:
      *event_notified_p = GSDIDIAG_SIM_INSERTED_2;
      break;
    case GSDI_SIM_REMOVED_2:
      *event_notified_p = GSDIDIAG_SIM_REMOVED_2;
      break;
    case GSDI_CARD_ERROR_2:
      *event_notified_p = GSDIDIAG_CARD_ERROR_2;
      break;
    case GSDI_MEMORY_WARNING_2:
      *event_notified_p = GSDIDIAG_MEMORY_WARNING_2;
      break;
    case GSDI_NO_SIM_EVENT_2:
      *event_notified_p = GSDIDIAG_NO_SIM_EVENT_2;
      break;
    case GSDI_SIM_INIT_COMPLETED_2:
      *event_notified_p = GSDIDIAG_SIM_INIT_COMPLETED_2;
      break;
    case GSDI_SIM_INIT_COMPLETED_NO_PROV_2:
      *event_notified_p = GSDIDIAG_SIM_INIT_COMPLETED_NO_PROV_2;
      break;
    case GSDI_PIN1_VERIFIED_2:
      *event_notified_p = GSDIDIAG_PIN1_VERIFIED_2;
      break;
    case GSDI_PIN1_BLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN1_BLOCKED_2;
      break;
    case GSDI_PIN1_UNBLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN1_UNBLOCKED_2;
      break;
    case GSDI_PIN1_ENABLED_2:
      *event_notified_p = GSDIDIAG_PIN1_ENABLED_2;
      break;
    case GSDI_PIN1_DISABLED_2:
      *event_notified_p = GSDIDIAG_PIN1_DISABLED_2;
      break;
    case GSDI_PIN1_CHANGED_2:
      *event_notified_p = GSDIDIAG_PIN1_CHANGED_2;
      break;
    case GSDI_PIN1_PERM_BLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN1_PERM_BLOCKED_2;
      break;
    case GSDI_PIN2_VERIFIED_2:
      *event_notified_p = GSDIDIAG_PIN2_VERIFIED_2;
      break;
    case GSDI_PIN2_BLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN2_BLOCKED_2;
      break;
    case GSDI_PIN2_UNBLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN2_UNBLOCKED_2;
      break;
    case GSDI_PIN2_ENABLED_2:
      *event_notified_p = GSDIDIAG_PIN2_ENABLED_2;
      break;
    case GSDI_PIN2_DISABLED_2:
      *event_notified_p = GSDIDIAG_PIN2_DISABLED_2;
      break;
    case GSDI_PIN2_CHANGED_2:
      *event_notified_p = GSDIDIAG_PIN2_CHANGED_2;
      break;
    case GSDI_PIN2_PERM_BLOCKED_2:
      *event_notified_p = GSDIDIAG_PIN2_PERM_BLOCKED_2;
      break;
    case GSDI_REFRESH_SIM_RESET_2:
      *event_notified_p = GSDIDIAG_REFRESH_SIM_RESET_2;
      break;
    case GSDI_REFRESH_SIM_INIT_2:
      *event_notified_p = GSDIDIAG_REFRESH_SIM_INIT_2;
      break;
    case GSDI_FDN_ENABLE_2:
      *event_notified_p = GSDIDIAG_FDN_ENABLE_2;
      break;
    case GSDI_FDN_DISABLE_2:
      *event_notified_p = GSDIDIAG_FDN_DISABLE_2;
      break;
    case GSDI_OK_FOR_TERMINAL_PROFILE_DL_2:
      *event_notified_p = GSDIDIAG_OK_FOR_TP_DL_2;
      break;
    case GSDI_NOT_OK_FOR_TERMINAL_PROFILE_DL_2:
      *event_notified_p = GSDIDIAG_NOT_OK_FOR_TP_DL_2;
      break;
    default:
      break;
  }
  return GSDI_SUCCESS;
}/* gsdidiag_util_convert_sim_events_to_gsdidiag_event */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*===========================================================================
FUNCTION gsdidiag_event_notification

DESCRIPTION
  This function is called when GSDI is going to notify Diag of any event
  related to the card.

ARGUMENTS
  gsdi_sim_events_T, the event to be notified

DEPENDENCIES
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gsdidiag_event_notification (gsdi_sim_events_T event)
{
    gsdidiag_card_evt_enum_type event_to_be_notified; /* used to send event*/
    gsdi_returns_T gsdi_status = GSDI_ERROR;
    uint8 len;
    len   = sizeof( event_to_be_notified );

    gsdi_status = gsdidiag_util_convert_sim_events_to_gsdidiag_event(event,
                                      &event_to_be_notified);
    if ((gsdi_status == GSDI_ERROR) ||
        (event_to_be_notified == GSDIDIAG_NO_EVENT_TO_DIAG)) {
      return;
    }
    event_report_payload( EVENT_GSDI_CARD_EVENT_NOTIFICATION,
                          len,
                          (void *)&event_to_be_notified );

    return;
}/* gsdidiag_event_notification */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */


/*=============================================================================
   Map of gsdi diag request ids to the generic command function. When a request
   is received by diag subsystem, the corresponding function is called. This
   table is used for the functions which implement delayed responses.
=============================================================================*/
#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
static const diagpkt_user_table_entry_type gsdidiag_subsys_delayed_tbl[] = {
  { (word)GSDIDIAG_GENERIC_CMD,
    (word)GSDIDIAG_GENERIC_CMD,
    gsdidiag_generic_command }
#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */
};
#endif /*FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION*/

/*=============================================================================
   Map of gsdi diag request ids to functions. When a request is received by
   diag subsystem, the corresponding function is called.
=============================================================================*/
static const diagpkt_user_table_entry_type gsdidiag_subsys_tbl[] = {
#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    { (word)GSDIDIAG_SIM_READ_CMD,
      (word)GSDIDIAG_SIM_READ_CMD,
      gsdidiag_sim_read},
    { (word)GSDIDIAG_SIM_WRITE_CMD,
      (word)GSDIDIAG_SIM_WRITE_CMD,
      gsdidiag_sim_write },
   /* { (word)GSDIDIAG_SIM_INCREASE_CMD,
      (word)GSDIDIAG_SIM_INCREASE_CMD,
      gsdidiag_sim_increase},
   */
    /* reserved */
    { (word)GSDIDIAG_GET_FILE_ATTRIBUTES_CMD ,
      (word)GSDIDIAG_GET_FILE_ATTRIBUTES_CMD,
      gsdidiag_sim_get_file_attributes },
    { (word)GSDIDIAG_GET_PIN_STATUS_CMD,
      (word)GSDIDIAG_GET_PIN_STATUS_CMD,
      gsdidiag_get_pin_status },
    { (word)GSDIDIAG_VERIFY_PIN_CMD,
      (word)GSDIDIAG_VERIFY_PIN_CMD,
      gsdidiag_verify_pin },
    { (word)GSDIDIAG_UNBLOCK_PIN_CMD,
      (word)GSDIDIAG_UNBLOCK_PIN_CMD,
      gsdidiag_unblock_pin },
    { (word)GSDIDIAG_DISABLE_PIN_CMD,
      (word)GSDIDIAG_DISABLE_PIN_CMD,
      gsdidiag_disable_pin },
    { (word)GSDIDIAG_ENABLE_PIN_CMD,
      (word)GSDIDIAG_ENABLE_PIN_CMD,
      gsdidiag_enable_pin },
    /* reserved */
    /* {(word)GSDIDIAG_GET_SIM_CAPABILITES_CMD, (word)GSDIDIAG_GET_SIM_CAPABILITES_CMD,
                                          gsdidiag_sim_read},*/
    /* reserved */
    /* reserved */
    /* {(word)GSDIDIAG_ILLEGAL_SIM_CMD, (word)GSDIDIAG_ILLEGAL_SIM_CMD, gsdidiag_sim_read}, */
    /* {(word)GSDIDIAG_SIM_SEARCH_CMD,  (word)GSDIDIAG_SIM_SEARCH_CMD, gsdidiag_sim_read}, */
    /* reserved */
    { (word)GSDIDIAG_SELECT_CMD,
      (word)GSDIDIAG_SELECT_CMD,
      gsdidiag_sim_select},
    /* reserved */
    /* {(word)GSDIDIAG_GET_IMAGE_CMD,  (word)GSDIDIAG_GET_IMAGE_CMD, gsdidiag_sim_read}, */
    /* {(word)GSDIDIAG_SIM_REFRESH_CMD,  (word)GSDIDIAG_SIM_REFRESH_CMD, gsdidiag_sim_read},*/
    /* reserved */
    { (word)GSDIDIAG_GET_AVAILABLE_APPS_CMD,
      (word)GSDIDIAG_GET_AVAILABLE_APPS_CMD,
      gsdidiag_get_available_apps },
    /* {(word)GSDIDIAG_GET_SLOTS_INFO_CMD, (word)GSDIDIAG_GET_SLOTS_INFO_CMD,
                                       gsdidiag_get_slots_info },
    */
    { (word)GSDIDIAG_SEND_APDU_CMD,
      (word)GSDIDIAG_SEND_APDU_CMD,
      gsdidiag_send_apdu },
#endif /* FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
    { (word)GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD,
      (word)GSDIDIAG_ACTIVATE_FEATURE_INDICATOR_CMD,
      gsdidiag_activate_feature_indicator },
    { (word)GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD,
      (word)GSDIDIAG_DEACTIVATE_FEATURE_INDICATOR_CMD,
      gsdidiag_deactivate_feature_indicator },
    { (word)GSDIDIAG_GET_FEATURE_INDICATION_CMD,
      (word)GSDIDIAG_GET_FEATURE_INDICATION_CMD,
      gsdidiag_get_feature_indication },
    { (word)GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD,
      (word)GSDIDIAG_SET_FEATURE_INDICATOR_DATA_CMD,
      gsdidiag_set_feature_indicator_data },
    { (word)GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD,
      (word)GSDIDIAG_GET_FEATURE_INDICATOR_DATA_CMD,
      gsdidiag_get_feature_indicator_data },
    { (word)GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD,
      (word)GSDIDIAG_GET_FEATURE_INDICATOR_KEY_CMD,
      gsdidiag_get_feature_indicator_key },
    { (word)GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD,
      (word)GSDIDIAG_UNBLOCK_FEATURE_INDICATOR_CMD,
      gsdidiag_unblock_feature_ind },
    { (word)GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD,
      (word)GSDIDIAG_GET_PERM_FEATURE_INDICATION_CMD,
      gsdidiag_get_perm_feature_indication },
    { (word)GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD,
      (word)GSDIDIAG_PERM_DISABLE_FEATURE_INDICATOR_CMD,
      gsdidiag_disable_perm_feature_indicator },
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
    /* {(word)GSDIDIAG_EXECUTE_APDU_CMD, (word)GSDIDIAG_MAX_CMD, gsdidiag_handler} */
};

/*===========================================================================
FUNCTION gsdidiag_init()

DESCRIPTION
    Initializes the diag interface to mmgsdi task.
    Registers table of diag callback commands with diag service.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
void gsdidiag_init (void)
{

    DIAGPKT_DISPATCH_TABLE_REGISTER ((uint8)DIAG_SUBSYS_GSDI, gsdidiag_subsys_tbl);

#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
    DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(
      DIAG_SUBSYS_CMD_VER_2_F,
      (uint8)DIAG_SUBSYS_GSDI,
      gsdidiag_subsys_delayed_tbl);
#endif /* FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */

    /*Initialize request table*/
    gsdidiag_req_table_init();
#ifdef FEATURE_GSTK_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_GSTK_TEST_AUTOMATION */
} /* gsdidiag_init */


#ifndef FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION
/*===========================================================================
FUNCTION gsdidiag_populate_according_to_file_type

DESCRIPTION
    Function to populate an internal gsdidiag structure with the get file
    attribute information.

INPUT:
  file_attr - pointer containing the get file attributed information
  rsp_ptr   - response pointer into which the get file attribute information
  is copied.

DEPENDENCIES
    None

RETURN VALUE
   gsdi_returns_T: GSDI_SUCCESS
                   GSDI_ERROR
OUTPUT
  rsp_ptr is populated with the relevant information depending on the
  file type.
===========================================================================*/
static gsdi_returns_T gsdidiag_populate_according_to_file_type(
  gsdidiag_rsp_get_file_attr_payload_type *rsp_ptr,
  mmgsdi_file_attributes_type file_attr)
{
  if(rsp_ptr == NULL)
  {
    MSG_ERROR("rsp_ptr ERR:NULL",0,0,0);
    return GSDI_ERROR;
  }

  switch(rsp_ptr->file_type)
  {
  case MMGSDI_TRANSPARENT_FILE:
    rsp_ptr->cf_increase_allowed = 0;
    rsp_ptr->no_of_rec = 0;
    rsp_ptr->rec_len = 0;
    rsp_ptr->file_invalid_flg =
      file_attr.file_info.transparent_file.file_status.file_invalidated;
    rsp_ptr->rw_allowed_when_invalid_flg =
      file_attr.file_info.transparent_file.file_status.read_write_when_invalidated;

    rsp_ptr->increase_cond =
      (int)file_attr.file_info.transparent_file.file_security.increase.protection_method;
    rsp_ptr->increase_no_of_pin_req =
      file_attr.file_info.transparent_file.file_security.increase.num_protection_pin;
    if((file_attr.file_info.transparent_file.file_security.increase.num_protection_pin > 0) &&
      (file_attr.file_info.transparent_file.file_security.increase.num_protection_pin <
       MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->increase_pin_info,
             file_attr.file_info.transparent_file.file_security.increase.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.transparent_file.file_security.increase.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info((PACKED)&rsp_ptr->increase_pin_info,
      (mmgsdi_pin_enum_type*)file_attr.file_info.transparent_file.file_security.increase.protection_pin_ptr,
      file_attr.file_info.transparent_file.file_security.increase.num_protection_pin);

    */

    rsp_ptr->read_cond =
      (int)file_attr.file_info.transparent_file.file_security.read.protection_method;
    rsp_ptr->read_no_of_pin_req =
      file_attr.file_info.transparent_file.file_security.read.num_protection_pin;
    if((file_attr.file_info.transparent_file.file_security.read.num_protection_pin > 0) &&
       (file_attr.file_info.transparent_file.file_security.read.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->read_pin_info,
             file_attr.file_info.transparent_file.file_security.read.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.transparent_file.file_security.read.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info((PACKED)&rsp_ptr->read_pin_info,
      file_attr.file_info.transparent_file.file_security.read.protection_pin_ptr,
      file_attr.file_info.transparent_file.file_security.read.num_protection_pin);
    */

    rsp_ptr->invalidate_cond =
      (int)file_attr.file_info.transparent_file.file_security.invalidate_deactivate.protection_method;
    rsp_ptr->invalidate_no_on_pin_req =
      file_attr.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin;
    if((file_attr.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin > 0) &&
      (file_attr.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin <
       MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->invalidate_pin_info,
             file_attr.file_info.transparent_file.file_security.invalidate_deactivate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->invalidate_pin_info,
      file_attr.file_info.transparent_file.file_security.invalidate_deactivate.protection_pin_ptr,
      file_attr.file_info.transparent_file.file_security.invalidate_deactivate.num_protection_pin);
    */

    rsp_ptr->validate_cond =
        (int)file_attr.file_info.transparent_file.file_security.rehabilitate_activate.protection_method;
    rsp_ptr->validate_no_of_pin_req =
        file_attr.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin;
    if((file_attr.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin > 0) &&
       (file_attr.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin <
        MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->validate_pin_info,
             file_attr.file_info.transparent_file.file_security.rehabilitate_activate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->validate_pin_info,
      file_attr.file_info.transparent_file.file_security.rehabilitate_activate.protection_pin_ptr,
      file_attr.file_info.transparent_file.file_security.rehabilitate_activate.num_protection_pin);
    */
    rsp_ptr->write_cond =
      (int)file_attr.file_info.transparent_file.file_security.write.protection_method;
    rsp_ptr->write_no_of_pin_req =
      file_attr.file_info.transparent_file.file_security.write.num_protection_pin;
    if((file_attr.file_info.transparent_file.file_security.write.num_protection_pin > 0) &&
      (file_attr.file_info.transparent_file.file_security.write.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->write_pin_info,
             file_attr.file_info.transparent_file.file_security.write.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.transparent_file.file_security.write.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->write_pin_info,
      file_attr.file_info.transparent_file.file_security.write.protection_pin_ptr,
      file_attr.file_info.transparent_file.file_security.write.num_protection_pin);
    */
    break;
  case MMGSDI_LINEAR_FIXED_FILE:
    rsp_ptr->cf_increase_allowed = 0;
    rsp_ptr->no_of_rec = file_attr.file_info.linear_fixed_file.num_of_rec;
    rsp_ptr->rec_len = file_attr.file_info.linear_fixed_file.rec_len;
    rsp_ptr->file_invalid_flg =
      file_attr.file_info.linear_fixed_file.file_status.file_invalidated;
    rsp_ptr->rw_allowed_when_invalid_flg =
      file_attr.file_info.linear_fixed_file.file_status.read_write_when_invalidated;

    rsp_ptr->increase_cond =
      (int)file_attr.file_info.linear_fixed_file.file_security.increase.protection_method;
    rsp_ptr->increase_no_of_pin_req =
      file_attr.file_info.linear_fixed_file.file_security.increase.num_protection_pin;
    if((file_attr.file_info.linear_fixed_file.file_security.increase.num_protection_pin > 0) &&
      (file_attr.file_info.linear_fixed_file.file_security.increase.num_protection_pin <
       MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->increase_pin_info,
             file_attr.file_info.linear_fixed_file.file_security.increase.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.linear_fixed_file.file_security.increase.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->increase_pin_info,
      file_attr.file_info.linear_fixed_file.file_security.increase.protection_pin_ptr,
      file_attr.file_info.linear_fixed_file.file_security.increase.num_protection_pin);
    */
    rsp_ptr->read_cond =
      (int)file_attr.file_info.linear_fixed_file.file_security.read.protection_method;
    rsp_ptr->read_no_of_pin_req =
      file_attr.file_info.linear_fixed_file.file_security.read.num_protection_pin;
    if((file_attr.file_info.linear_fixed_file.file_security.read.num_protection_pin > 0) &&
      (file_attr.file_info.linear_fixed_file.file_security.read.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->read_pin_info,
             file_attr.file_info.linear_fixed_file.file_security.read.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.linear_fixed_file.file_security.read.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->read_pin_info,
      file_attr.file_info.linear_fixed_file.file_security.read.protection_pin_ptr,
      file_attr.file_info.linear_fixed_file.file_security.read.num_protection_pin);
    */

    rsp_ptr->invalidate_cond =
      (int)file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.protection_method;
    rsp_ptr->invalidate_no_on_pin_req =
      file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin;
    if((file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin > 0) &&
       (file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin <
        MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->invalidate_pin_info,
             file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->invalidate_pin_info,
      file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.protection_pin_ptr,
      file_attr.file_info.linear_fixed_file.file_security.invalidate_deactivate.num_protection_pin);
    */

    rsp_ptr->validate_cond =
      (int)file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.protection_method;
    rsp_ptr->validate_no_of_pin_req =
      file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin;
    if((file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin > 0) &&
      (file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin <
       MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->validate_pin_info,
             file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->validate_pin_info,
      file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.protection_pin_ptr,
      file_attr.file_info.linear_fixed_file.file_security.rehabilitate_activate.num_protection_pin);
    */
    rsp_ptr->write_cond =
      (int)file_attr.file_info.linear_fixed_file.file_security.write.protection_method;
    rsp_ptr->write_no_of_pin_req =
      file_attr.file_info.linear_fixed_file.file_security.write.num_protection_pin;
    if((file_attr.file_info.linear_fixed_file.file_security.write.num_protection_pin > 0) &&
      (file_attr.file_info.linear_fixed_file.file_security.write.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->write_pin_info,
             file_attr.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.linear_fixed_file.file_security.write.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->write_pin_info,
      file_attr.file_info.linear_fixed_file.file_security.write.protection_pin_ptr,
      file_attr.file_info.linear_fixed_file.file_security.write.num_protection_pin);
      */
    break;
  case MMGSDI_CYCLIC_FILE:
    rsp_ptr->cf_increase_allowed = file_attr.file_info.cyclic_file.increase_allowed;
    rsp_ptr->no_of_rec = file_attr.file_info.cyclic_file.num_of_rec;
    rsp_ptr->rec_len = file_attr.file_info.cyclic_file.rec_len;
    rsp_ptr->file_invalid_flg =
      file_attr.file_info.cyclic_file.file_status.file_invalidated;
    rsp_ptr->rw_allowed_when_invalid_flg =
      file_attr.file_info.cyclic_file.file_status.read_write_when_invalidated;

    rsp_ptr->increase_cond =
      (int)file_attr.file_info.cyclic_file.file_security.increase.protection_method;
    rsp_ptr->increase_no_of_pin_req =
      file_attr.file_info.cyclic_file.file_security.increase.num_protection_pin;
    if((file_attr.file_info.cyclic_file.file_security.increase.num_protection_pin > 0) &&
      (file_attr.file_info.cyclic_file.file_security.increase.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->increase_pin_info,
             file_attr.file_info.cyclic_file.file_security.increase.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.cyclic_file.file_security.increase.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->increase_pin_info,
      file_attr.file_info.cyclic_file.file_security.increase.protection_pin_ptr,
      file_attr.file_info.cyclic_file.file_security.increase.num_protection_pin);
      */


    rsp_ptr->read_cond =
      (int)file_attr.file_info.cyclic_file.file_security.read.protection_method;
    rsp_ptr->read_no_of_pin_req =
      file_attr.file_info.cyclic_file.file_security.read.num_protection_pin;
    if((file_attr.file_info.cyclic_file.file_security.read.num_protection_pin > 0) &&
      (file_attr.file_info.cyclic_file.file_security.read.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->read_pin_info,
             file_attr.file_info.cyclic_file.file_security.read.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.cyclic_file.file_security.read.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->read_pin_info,
      file_attr.file_info.cyclic_file.file_security.read.protection_pin_ptr,
      file_attr.file_info.cyclic_file.file_security.read.num_protection_pin);
      */

    rsp_ptr->invalidate_cond =
      (int)file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.protection_method;
    rsp_ptr->invalidate_no_on_pin_req =
      file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin;
    if((file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin > 0) &&
       (file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin <
        MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->invalidate_pin_info,
             file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->invalidate_pin_info,
      file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.protection_pin_ptr,
      file_attr.file_info.cyclic_file.file_security.invalidate_deactivate.num_protection_pin);
    */


    rsp_ptr->validate_cond =
      (int)file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.protection_method;
    rsp_ptr->validate_no_of_pin_req =
      file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin;
    if((file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin > 0) &&
       (file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin <
        MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->validate_pin_info,
             file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->validate_pin_info,
      file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.protection_pin_ptr,
      file_attr.file_info.cyclic_file.file_security.rehabilitate_activate.num_protection_pin);
    */

    rsp_ptr->write_cond =
      (int)file_attr.file_info.cyclic_file.file_security.write.protection_method;
    rsp_ptr->write_no_of_pin_req =
      file_attr.file_info.cyclic_file.file_security.write.num_protection_pin;
    if((file_attr.file_info.cyclic_file.file_security.write.num_protection_pin > 0) &&
       (file_attr.file_info.cyclic_file.file_security.write.num_protection_pin < MMGSDIDIAG_MAX_PIN))
    {
      (void)memcpy((int32*)rsp_ptr->write_pin_info,
             file_attr.file_info.cyclic_file.file_security.write.protection_pin_ptr,
             sizeof(int32) * int32touint32(file_attr.file_info.cyclic_file.file_security.write.num_protection_pin));
    }
    /*gsdidiag_cpy_pin_info(&rsp_ptr->write_pin_info,
      file_attr.file_info.cyclic_file.file_security.write.protection_pin_ptr,
      file_attr.file_info.cyclic_file.file_security.write.num_protection_pin);
      */
    break;
  default:
    return GSDI_ERROR;
  }
  return GSDI_SUCCESS;
}
#endif /*FEATURE_MMGSDI_DELAYED_RESPONSE_OPTIMIZATION */


/*===========================================================================
FUNCTION: gsdidiag_get_search_payload

DESCRIPTION: Populates the request structures for Search
             with the data from the request packet.

ARGUMENTS:
    gsdidiag_req_payload_type *req_ptr : Pointer to request command
    PACKED void               *req     : pointer to request packet of type
                                         gsdidiag_generic_cmd_req_type
    uint8                     offset   : offset of the req pointer

RETURN VALUE:
    NONE
===========================================================================*/
static void gsdidiag_get_search_payload(
  gsdidiag_req_payload_type        *req_ptr,
  PACKED void                      *req,
  uint32                            offset
)
{
  if ((req_ptr == NULL) || (req == NULL))
  {
    MMGSDI_DEBUG_MSG_ERROR("req_ptr NULL",0,0,0);
    return;
  }
  /* increment req pointer to obtain position of next data element */
  req    = (char*)req + offset;

  /* CLIENT ID:-  Extract Client Id from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->request.search_req.client_id)),
                (void*)req,
                sizeof(uint64) );

  offset = offset + sizeof(uint64);
  req    = (char*)req + sizeof(uint64);

  /* Reading mmgsdidiag_access_type access.access_method*/
  /* ACCESS METHOD :- Extract EF- filename from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->request.search_req.access.access_method)),
                (void*)req, sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req    = (char*)req + sizeof(uint32);

  /* FILENAME :- Extract EF- filename from req packet and store it in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->request.search_req.access.file_enum)),
                (void*)req, sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req    = (char*)req + sizeof(uint32);

  /*PATH LEN and PATH */
  gsdidiag_get_mmgsdidiag_path_type_payload(
        &req_ptr->request.search_req.access.path,
        &req,
        &offset);

  if(req_ptr->sub_cmd_hdr.subcommand == MMGSDIDIAG_ICC_SEARCH_REQ_CMD)
  {
    /* ICC Search information*/
    /* Reading ICC Search Direction*/
    (void)memcpy( ((void*)&(req_ptr->request.search_req.search_direction.icc_search_direction)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);
  }
  else
  {
    /* UICC Search information
       ['UICC_SEARCH_TYPE',             BYTE,    4],
       ['RECORD_NUMBER',                BYTE,    4],
       ['UICC_SEARCH_DIRECTION',        BYTE,    4],
       ['UICC_ENHANCED_OFFSET_TYPE',    BYTE,    4],
       ['UICC_ENHANCED_OFFSET_VAL',     BYTE,    4],
    */
    /* Reading UICC Search Type*/
    (void)memcpy( ((void*)&(req_ptr->request.search_req.search_type)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);

    /* Reading Record Number */
    (void)memcpy( ((void*)&(req_ptr->request.search_req.rec_num)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);

    /* Reading UICC Search Direction*/
    (void)memcpy( ((void*)&(req_ptr->request.search_req.search_direction.uicc_search_direction)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);

    /* Reading enhanced search offset*/
    /* Reading offset type */
    (void)memcpy( ((void*)&(req_ptr->request.search_req.enhanced_search_offset.offset_type)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);

    /* Reading offset data */
    (void)memcpy( ((void*)&(req_ptr->request.search_req.enhanced_search_offset.offset_data)),
                  (void*)req, sizeof(uint32) );
    offset = offset + sizeof(uint32);
    req    = (char*)req + sizeof(uint32);
  }
  /* Reading of the search pattern */
  req_ptr->request.search_req.search_pattern.data_len = 0x00;
  req_ptr->request.search_req.search_pattern.data_ptr = NULL;

  /* Reading Data Length */
  (void)memcpy( ((void*)&(req_ptr->request.search_req.search_pattern.data_len)),
                (void*)req, sizeof(uint32) );

  offset = offset + sizeof(uint32);
  req    = (char*)req + sizeof(uint32);
  if (req_ptr->request.search_req.search_pattern.data_len > 0x00)
  {
    /* Reading the search pattern */
    req_ptr->request.search_req.search_pattern.data_ptr =
    (uint8 *)mmgsdi_malloc(int32touint32(req_ptr->request.search_req.search_pattern.data_len));
    if (NULL == req_ptr->request.search_req.search_pattern.data_ptr)
    {
      MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
      return;
    }
    (void)memcpy( ((void*)(req_ptr->request.search_req.search_pattern.data_ptr)),
                  (void*)req,
                  int32touint32(req_ptr->request.search_req.search_pattern.data_len));
    offset = offset + int32touint32(req_ptr->request.search_req.search_pattern.data_len);
    req = (char*)req + req_ptr->request.search_req.search_pattern.data_len;
  }/*end of if */

  /* CLIENT REF PTR Extract client ref value from req packet and store it
     in the request pointer */
  (void)memcpy( ((void*)&(req_ptr->request.search_req.client_ref_val)),
                (void*)req,
                sizeof(uint32) );
  offset = offset + sizeof(uint32);
  req    = (char*)req + sizeof(uint32);
}/*end of function gsdidiag_get_search_payload*/


/*===========================================================================

FUNCTION GSDIDIAG DELAYED SEARCH CALLBACK

DESCRIPTION
    This function is the callback function used for a search api.

DEPENDENCIES
    None

INPUT
    mmgsdi_cnf_type* cnf_data

RETURN VALUE
    None

OUTPUT
   The response pointer is populated with the header information and
   the Search payload information.
===========================================================================*/
static void gsdidiag_delayed_search_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf_type,
  const mmgsdi_cnf_type * cnf_data)
{
  gsdidiag_generic_cmd_rsp_type  * rsp_ptr; /* response pointer */
  uint32                           len;     /* length of event */
  int                              index;   /* index into req_table*/
  if (cnf_data == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR(
     "mmgsdi cnf type is null in gsdidiag_delayed_search_cb.",
      0,0,0);
    return;
  }

  index = uint32toint32(cnf_data->response_header.client_data);
  /* req_table index should be in the used state */
  if (req_table[index].use_flag != GSDIDIAG_REQ_TABLE_INDEX_USED )
  {
    MMGSDI_DEBUG_MSG_ERROR(
     "Index 0x%x is not in use gsdidiag_delayed_uicc_search_cb.",
      index,0,0);
    return;
  }

  /* Allocate memory for the response pointer */
  len   = sizeof(gsdidiag_generic_cmd_rsp_type);
  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               req_table[index].transaction_id,
               len );
  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    gsdidiag_free_index(index);
    return;
  }
  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* Populate Response Header */
  /* aps_as_transport is populated from the value in the request table */
  rsp_ptr->rsp_header.aps_as_transport = req_table[index].aps_as_transport;

  /*Populate sub command header for Search payload */
  /* Updating sub command */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand = req_table[index].subcommand;
  /* Updating mmgsdi_status */
  if (0x00 == req_table[index].aps_as_transport)
  {
    uint32 temp_status;
    gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status,
                                                      &temp_status);
    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;

  /* Populate UICC Search Response Data from payload */
  /* Initialize Access Method */
  rsp_ptr->rsp_payload.response.search_data.access_method  =
    GSDIDIAG_MAX_ACCESS_ENUM;

  /* Initialize file path with 0x00 */
  rsp_ptr->rsp_payload.response.search_data.path.path_len =0;
  (void)memset((uint16*)rsp_ptr->rsp_payload.response.search_data.path.path_buf,
                0x0000, sizeof(uint16)*MMGSDIDIAG_PATH_LEN);

  /* Initialize response data with 0x00*/
  rsp_ptr->rsp_payload.response.search_data.payload.len = 0;
  (void)memset((uint8*)rsp_ptr->rsp_payload.response.search_data.payload.buffer,
               0x00, sizeof(uint8)*MMGSDIDIAG_MAX_DATA_BLOCK_LEN );

  /* Updating Access method from cnf response */
  rsp_ptr->rsp_payload.response.search_data.access_method =
    gsdidiag_convert_access_method_from_mmgsdi_enum(
      cnf_data->search_cnf.access.access_method );

  switch(rsp_ptr->rsp_payload.response.search_data.access_method)
  {
    case GSDIDIAG_EF_ENUM_ACCESS:
      /* File Enum */
      rsp_ptr->rsp_payload.response.search_data.file_enum =
        gsdidiag_convert_file_from_mmgsdi_enum(
          cnf_data->search_cnf.access.file.file_enum);
      break;

    case GSDIDIAG_BY_PATH_ACCESS:
      if (cnf_data->search_cnf.access.file.path_type.path_len > MMGSDIDIAG_PATH_LEN)
      {
        MSG_ERROR("Invalid path length ",0,0,0);
        gsdidiag_free_index(index);
        diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
        diagpkt_delay_commit(rsp_ptr);
        return;
      }
      /* Updating File path */
      rsp_ptr->rsp_payload.response.search_data.path.path_len =
        cnf_data->search_cnf.access.file.path_type.path_len;

      memcpy((uint16*)rsp_ptr->rsp_payload.response.search_data.path.path_buf,
             (uint16*)cnf_data->read_cnf.access.file.path_type.path_buf,
             int32touint32(sizeof(uint16)*(cnf_data->search_cnf.access.file.path_type.path_len)));
      break;

    default:
      MSG_ERROR(" Invalid Access Method  ",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;
  }/*end of switch */

  /* Updating Response Data */
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    rsp_ptr->rsp_payload.response.search_data.payload.len =
       int32touint32(cnf_data->search_cnf.searched_record_nums.data_len);
    if (rsp_ptr->rsp_payload.response.search_data.payload.len >
        MMGSDIDIAG_MAX_DATA_BLOCK_LEN)
    {
      MSG_ERROR("Invalid path length ",0,0,0);
      gsdidiag_free_index(index);
      diagpkt_subsys_set_status(rsp_ptr, GSDIDIAG_INCORRECT_RSP_ERR);
      diagpkt_delay_commit(rsp_ptr);
      return;
    }
    (void)memcpy((uint8*)rsp_ptr->rsp_payload.response.search_data.payload.buffer,
                 (uint8*)cnf_data->search_cnf.searched_record_nums.data_ptr,
                 sizeof(uint8) * rsp_ptr->rsp_payload.response.search_data.payload.len);
  }/*end of if (mmgsdi_status == MMGSDI_SUCCESS) */

  /* free the memory */
  gsdidiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);

  /* fix lint complain on unused input parameter */
  (void)cnf_type;
}/*gsdidiag_delayed_search_cb*/


/*===========================================================================
FUNCTION gsdidiag_delayed_search

DESCRIPTION
    Function to search record from uicc and icc card. A task is queued to gsdi
    task to search. Rsp packet is sent out. Rsp packet indicates if the req was
    successfully queued to gsdi task.

INPUT:
    PACKED void *req  : pointer to request packet type
    word pkt_len      : length of request packet
    uint32 * status   : Status returned by mmgsdi_search API

    diagpkt_subsys_delayed_rsp_id_type rsp_id : Response Id generated by diag
    uint32 aps_as_transport                   : APS transaction ID

DEPENDENCIES
    None

RETURN VALUE
    None

OUTPUT
   rsp_id: Pointer to response id.
   status: GSDI_SUCCESS
           GSDI_ERROR
===========================================================================*/
static void gsdidiag_delayed_search (
    PACKED void                         *req,      /* pointer to request packet  */
    word                                pkt_len,   /* length of request packet */
    uint32                              *status,   /* Status returned by gsdi_sim_open_channel API*/
    diagpkt_subsys_delayed_rsp_id_type  rsp_id,    /* Response Id generated by diag*/
    uint32                              aps_as_transport /* APS transaction ID */
)
{
  gsdidiag_req_payload_type      *req_ptr       = (gsdidiag_req_payload_type *)req;
  mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_ERROR;
  int                             index         = 0;
  mmgsdi_access_type              access_info;
  mmgsdi_slot_id_enum_type        mmgsdi_slot   = MMGSDI_SLOT_1;
  mmgsdi_option_type              option        = {0};
  mmgsdi_search_data_type         search_pattern;
  mmgsdi_seek_data_type           seek_pattern;
  mmgsdi_search_offset_data_type  enhanced_search_offset;

  if (status == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("Status pointer is NULL in gsdidiag_delayed_search ",
                           0, 0, 0);
    return;
  }

  /* Checking for Input parameter */
  if (req_ptr == NULL)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR("req_ptr is NULL in gsdidiag_delayed_search ",0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }
  if(0x00 == pkt_len)
  {
    /* return error */
    MMGSDI_DEBUG_MSG_ERROR(
     "Request packet length is NULL in gsdidiag_delayed_search ",0, 0, 0);
    *status = GSDIDIAG_BAD_PARAM;
    return;
  }
  /* rep_id and aps_as_transport does not require checking*/

  /* find a free space in req_table to hold information for callback */
  index = gsdidiag_get_index();
  if (index == GSDIDIAG_GET_FN_ERR)
  {
    MMGSDI_DEBUG_MSG_ERROR("Request table is full in gsdidiag_delayed_search ",
                           0, 0, 0);
    /*  req_table is full */
    *status = GSDIDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    gsdidiag_free_index(index);
    return;
  }/* end of if (index == GSDIDIAG_GET_FN_ERR )*/

  /* Update member of Request Table*/
  req_table[index].transaction_id   = rsp_id;
  req_table[index].use_flag         = 1;
  req_table[index].aps_as_transport = aps_as_transport;
  req_table[index].subcommand       = req_ptr->sub_cmd_hdr.subcommand;

  mmgsdi_slot = gsdidiag_convert_to_mmgsdi_slot(req_ptr->sub_cmd_hdr.slot_id);
  gsdidiag_copy_access_to_mmgsdi(&access_info,
                                 &req_ptr->request.search_req.access);

  if (req_ptr->sub_cmd_hdr.subcommand == MMGSDIDIAG_ICC_SEARCH_REQ_CMD)
  {
    /*For ICC Search Request*/
    seek_pattern.data_len =  req_ptr->request.search_req.search_pattern.data_len;
    seek_pattern.data_ptr = req_ptr->request.search_req.search_pattern.data_ptr;
    mmgsdi_status         = mmgsdi_seek(req_ptr->request.search_req.client_id,
                                        mmgsdi_slot,
                                        access_info,
                                        (mmgsdi_seek_direction_enum_type)req_ptr->request.search_req.search_direction.icc_search_direction,
                                        seek_pattern,
                                        option,
                                        (mmgsdi_callback_type)gsdidiag_delayed_search_cb,
                                        int32touint32(index));
  }
  else
  {
    /* For UICC Search Request */
    search_pattern.data_len = req_ptr->request.search_req.search_pattern.data_len;
    search_pattern.data_ptr = req_ptr->request.search_req.search_pattern.data_ptr;

    switch(req_ptr->request.search_req.enhanced_search_offset.offset_type)
    {
      case GSDIDIAG_UICC_SEARCH_OPTION_NONE:
        enhanced_search_offset.offset_type = MMGSDI_SEARCH_OPTION_NONE;
        break;
      case GSDIDIAG_UICC_SEARCH_BY_OFFSET:
        enhanced_search_offset.offset_type = MMGSDI_SEARCH_BY_OFFSET;
        break;
      case GSDIDIAG_UICC_SEARCH_BY_CHAR:
        enhanced_search_offset.offset_type = MMGSDI_SEARCH_BY_CHAR;
        break;
      case GSDIDIAG_MAX_UICC_SEARCH_OPTION_ENUM:
        enhanced_search_offset.offset_type = MMGSDI_MAX_SEARCH_OPTION_ENUM;
        break;
      default:
        enhanced_search_offset.offset_type = MMGSDI_MAX_SEARCH_OPTION_ENUM;
    } /* end switch */

    enhanced_search_offset.offset_data =
      req_ptr->request.search_req.enhanced_search_offset.offset_data;

    mmgsdi_status = mmgsdi_search( req_ptr->request.search_req.client_id,
                                   mmgsdi_slot,
                                   access_info,
                                   (mmgsdi_search_enum_type)req_ptr->request.search_req.search_type,
                                   req_ptr->request.search_req.rec_num,
                                   (mmgsdi_search_direction_enum_type)req_ptr->request.search_req.search_direction.uicc_search_direction,
                                   enhanced_search_offset,
                                   search_pattern,
                                   option,
                                   (mmgsdi_callback_type)gsdidiag_delayed_search_cb,
                                   int32touint32(index));
  }

  if (mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR("mmgsdidiag_delayed_search queue failed 0x%x",
                           mmgsdi_status,0,0);
    gsdidiag_free_index(index);
  }
  gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status, status);
}/*gsdidiag_delayed_search*/


/*===========================================================================

FUNCTION GSDIDIAG DELAYED GENERIC ERROR RSP

DESCRIPTION
    This is a generic response function used for sending error information
    to the script in case the command fails in MMGSDI/GSDI command request
    processing before it gets sent to the card

DEPENDENCIES
    None

INPUT


RETURN VALUE
    None

OUTPUT
    None
===========================================================================*/
static void gsdidiag_delayed_generic_error_rsp(
  mmgsdi_return_enum_type   mmgsdi_status,
  gsdi_returns_T            gsdi_status,
  gsdidiag_req_payload_type request,
  uint16                    transaction_id,
  boolean                   is_gsdi_rsp
)
{
  gsdidiag_generic_cmd_rsp_type    *rsp_ptr     = NULL; /* response pointer */
  uint32                           len          = 0;    /* length of event  */
  uint32                           temp_status  = 0;


  /* Allocate memory for the response pointer */
  len   = sizeof(gsdidiag_generic_cmd_rsp_type);

  rsp_ptr = (gsdidiag_generic_cmd_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
               (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
               (diagpkt_subsys_cmd_code_type)GSDIDIAG_GENERIC_CMD,
               transaction_id,
               len );

  if(rsp_ptr == NULL)
  {
    MMGSDI_DEBUG_MSG_ERROR("diagpkt_subsys_alloc_v2_delay failed",0,0,0);
    return;
  }
  (void)memset((char*)&rsp_ptr->rsp_payload,0x00,sizeof(gsdidiag_rsp_payload_type));

  /* aps_as_transport is populated from the value in the request data */
  rsp_ptr->rsp_header.aps_as_transport = request.sub_cmd_hdr.aps_as_transport;

  /* Updating sub command */
  rsp_ptr->rsp_payload.sub_cmd_hdr.subcommand = request.sub_cmd_hdr.subcommand;

  /* Updating mmgsdi_status */
  if (0x00 == request.sub_cmd_hdr.aps_as_transport)
  {
    if(is_gsdi_rsp)
    {
      gsdidiag_convert_gsdi_status_to_gsdidiag_status(gsdi_status,
                                                      &temp_status);
    }
    else
    {
      gsdidiag_convert_mmgsdi_status_to_gsdidiag_status(mmgsdi_status,
                                                        &temp_status);
    }
    diagpkt_subsys_set_status(rsp_ptr, temp_status);
  }
  if(is_gsdi_rsp)
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.gsdi_status = gsdi_status;
  }
  else
  {
    rsp_ptr->rsp_payload.sub_cmd_hdr.st.mmgsdi_status = mmgsdi_status;
  }
  rsp_ptr->rsp_payload.response.data.len = 0;

  /* Send Delayed Response */
  diagpkt_delay_commit(rsp_ptr);
} /* gsdidiag_delayed_generic_error_rsp */
#endif /* FEATURE_MMGSDI_TEST_AUTOMATION */
