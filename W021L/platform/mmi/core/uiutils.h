#ifndef UIUTILS_H
#define UIUTILS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


    U S E R   I N T E R F A C E   U T I L S   S E R V I C E

GENERAL DESCRIPTION
  UI utility functions

  Copyright (c) 2002-2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/uiutils.h#18 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/09   Gopi    Moving back ClearBlockUITask, UnblockUITask, IsCallActive, BlockUITask
                     to AppComFunc.c
03/19/09   Gopi    Gaurding ClearBlockUITask, UnblockUITask, IsCallActive, BlockUITask 
                     with if !defined(FEATURE_UI_CORE_REMOVED) to fix Build issue in ThinUI Build.
03/11/09   Gopi    Change for Mango UI Integration
03/08/07   ypmw    Remove uiutils remote api from misc group
11/14/06   cvs     change check_provisioning remoting strategy to prevent 
                   deadlock on 7200
11/06/06   jas     Reverting prior change (doesn't work)
10/02/06   cvs     ui_init_provisioning now runs from ui_check_provisioning
07/24/06   cvs     remove unnecessary featurization
1/10/05    AT      Added new API for network name info.
11/10/04   cvs     Initialize provisioning earlier in powerup
02/25/04   ram     Integration with IBatt Interface.
11/03/03   tml     Fixed compilation error
10/06/03   tml     Added thermo sensors function
09/30/03   tml     Added function prototype
09/26/03   kar     Initial revision
===========================================================================*/
/* ^L<EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "sys.h"
#include "nv.h"
#include "cm.h"

/* ^L<EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef enum {
  UI_PROVISIONING_SUCCESS,
  UI_PROVISIONING_FAILURE,
  UI_PROVISIONING_PENDING
} ui_provisioning_status_type;


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/* The following codes may be returned by a PF_CHECK_TOKEN function.  */
#define REGEX_ILLEGAL_ENTRY -2
#define REGEX_NO_MATCH      -1

/* The following is the type of function to be called when we have a match
 * to the input string with the associated expression.  This function must
 * return either ILLEGAL_ENTRY or NO_MATCH or an integer >= 0 to indicate
 * that we found a match. 
 */
typedef int (*PF_CHECK_TOKEN)(int ntokens, void *tokens[]);


/* This structure associates an expression string with a function that
 * figures out the command.  The check_fn will get the tokens to determine
 * how many items of the array have been set.
 */
typedef struct token_input
{
  char *reg_exp;                /* Quasi-regular expression. */
  PF_CHECK_TOKEN check_fn;      /* Function to be called. */
} TOKEN_INPUT;

/*===========================================================================
               N E T W O R K    U T I L I T I E S    T Y P E S
===========================================================================*/
#define UI_MAX_NETWORK_NAME_LENGTH    UI_WIDE

#define UI_NETWORK_NAMETYPE_SHORT 1
#define UI_NETWORK_NAMETYPE_FULL  2

/*
** Define a type for the type of network.
*/
typedef enum uinetwk_network_type_e
{
   UI_NETWK_TYPE_UNKNOWN_TYPE,
   UI_NETWK_TYPE_GSM_900,
   UI_NETWK_TYPE_DCS_1800,
   UI_NETWK_TYPE_PCS_1900,
   UI_NETWK_TYPE_GSM_SAT,
   UI_NETWK_TYPE_UMTS
} uinetwk_network_type_e_type;

/*
** Define a type that contains the networks, country code, network code
** and name.
*/
typedef struct uinetwk_network_info_s
{

   sys_mcc_type                  mcc;
     /* Mobile Network Code                                */

   sys_mnc_type                  mnc;
    /* Mobile Country Code                                 */

   uinetwk_network_type_e_type   network_type;

   char                         *short_name_ptr;
     /* Pointer to a null terminated string containing the */
     /* network's short name.                              */

   char                         *full_name_ptr;
     /* Pointer to a null terminated string containing the */
     /* network's full name.                               */

} uinetwk_network_info_s_type;



#endif /* FEATURE_WCDMA, FEATURE_GSM */
/* ^L<EJECT> */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
                    N E T W O R K    U T I L I T I E S
===========================================================================*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/* ^L<EJECT> */
/*===========================================================================
FUNCTION uinetwk_init

DESCRIPTION

  This function must be called before calling uinetwk_get_network_info.
  This function initializes the data structures for the ui_netwk_get_-
  network_info function.

RETURNS
   Nothing.

DEPENDENCIES

===========================================================================*/
extern void uinetwk_init(void);


/* ^L<EJECT> */
/*===========================================================================
FUNCTION ui_netwk_find_network_info_by_index

DESCRIPTION
  This function returns the Mobile Country Code (mcc), Mobile Network Code
  (mnc) and network name for the requested network, references by index.
  The purpose of this API is to allow other entities to search based on
  other criteria than those directly supported, or get a full list of
  entries (start with index 0 and keep going until this routine returns 
  FALSE).

RETURNS
  TRUE  - The index was found in the lookup table.  The returned info
          parameter contains all relevant information regarding the network.
  FALSE - The index was not found.

DEPENDENCIES
  Function uinetwk_init must be called once before calling this function.

===========================================================================*/
boolean ui_netwk_find_network_info_by_index
(
  uint32                         index,
    /* The index to look up. */

  uinetwk_network_info_s_type  *info_ptr
    /* The returned network info   */
);

/*===========================================================================
FUNCTION ui_netwk_find_network_info_by_name

DESCRIPTION
  This function returns the Mobile Country Code (mcc), Mobile Network Code
  (mnc) and network name for the requested network, matching by name.
  The name_type is a bitmask indicating which name to search.
  Allowed values may contain either or both of:
    UI_NETWORK_NAMETYPE_SHORT 
    UI_NETWORK_NAMETYPE_FULL  

RETURNS
  TRUE  - The PLMN Id was found in the lookup table.  The returned info
          parameter contains all relevant information regarding the network.
  FALSE - The PLMN Id was not found in the lookup table.  

DEPENDENCIES
  Function uinetwk_init must be called once before calling this function.

===========================================================================*/
extern boolean ui_netwk_find_network_info_by_name
(
  char                         *network_name,
    /* The PLMN id to be processed */

  uint32                         name_type,
    /* UI_NETWORK_NAMETYPE_SHORT for Short, UI_NETWORK_NAMETYPE_FULL for long, 
       UI_NETWORK_NAMETYPE_SHORT | UI_NETWORK_NAMETYPE_FULL for either. */

  uinetwk_network_info_s_type  *info_ptr
    /* The returned network info   */
);

/* ^L<EJECT> */
/*===========================================================================
FUNCTION ui_netwk_get_network_info

DESCRIPTION
  This function returns the Mobile Country Code (mcc), Mobile Network Code
  (mnc) and network name for the requested PLMN.

RETURNS
  TRUE  - The PLMN was found in the lookup table.  The returned info
          parameter contains all relevant information regarding the network.
  FALSE - The PLMN was not found in the lookup table.  The returned info
          parameter contains only a valid mnc and mcc.

DEPENDENCIES
  Function uinetwk_init must be called once before calling this function.

===========================================================================*/
extern boolean ui_netwk_get_network_info
(
  sys_plmn_id_s_type           plmn,         /* The PLMN to be processed  */
  uinetwk_network_info_s_type  *info         /* The returned network info */
);


/* ^L<EJECT> */
/*===========================================================================
FUNCTION ui_netwk_format_network_name

DESCRIPTION
DESCRIPTION
  This function formats the network's name for display purposes.  The
  returned buffer (buffer_ptr) is not null terminated.  The number of
  characters in the buffer is returned to the calling routine.

  If the full name is provided and its length is <= UI_WIDE, then
  the the full name is copied to the buffer_ptr parameter.

  Otherwise, if the short name is provided, then the short name
  is copied to the buffer_ptr parameter.

  Otherwise, "<mcc>  <mnc>" is copied to the buffer_ptr parameter, where
  <mcc> is the country code and <mnc> is the network code.

RETURNS
  The number of characters in the buffer (buffer_ptr).

DEPENDENCIES
  The info parameter must have been initialized via a call to
  ui_netwk_get_network_info.  The length of the display_text_ptr
  parameter should be UI_WIDE+1;

===========================================================================*/
extern uint32 ui_netwk_format_network_name
(
  uinetwk_network_info_s_type   *info_ptr,
    /* The network info                                     */

  char                          *buffer_ptr
    /* The formatted network name to be displayed on the UE */
);
#endif /* FEATURE_WCDMA, FEATURE_GSM */

/* <EJECT> */
/*===========================================================================
FUNCTION CONVERT_NUMBER_TO_ASCII

DESCRIPTION
  This function converts input number to ascii and returns the number of ascii
  that have been converted.

  ex) input_number = 365
      converted_ascii = "       365"
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
byte convert_number_to_ascii
(
  unsigned int input_number,        /* input */
  char*        converted_ascii,     /* where retured ascii is going to be*/
  byte         size_of_array        /* size of converted_ascii array */
);

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)

/* <EJECT> */
/*===========================================================================
FUNCTION REGEX_TOKENIZE

DESCRIPTION
  This function is called to compare a series of regular expressions
  to a single input_string.  If the strings match, we call the function
  associated with the regular expression.  This function should process
  the string and return the command to be executed.  The caller can then
  call the appropriate function to handle the request.

  The regular expression formating understood by this function is:
    () to indicate save this part in one of the tokens.
    NOTE: the tokens are not NULL terminated by this function.
    The input string is preserved unless the called function modifies them.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int regex_tokenize(char *in_str, TOKEN_INPUT *inputs, void *tokens[]);

/* <EJECT> */
/*===========================================================================
FUNCTION UIUTILS_GET_IMEI

DESCRIPTION
  This function fills in the imei into the given byte array.  The
  input string must be at least 2*NV_UE_IMEI_SIZE.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int uiutils_get_imei(byte imei_ascii[]);
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */



/* <EJECT> */
/*===========================================================================
FUNCTION UI_GET_NV

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  None
===========================================================================*/
extern nv_stat_enum_type ui_get_nv(            /* in UI.C */
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);



/* <EJECT> */
/*===========================================================================
FUNCTION UI_PUT_NV

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  NV_TASK should be running
===========================================================================*/
extern nv_stat_enum_type ui_put_nv(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);

/*===========================================================================
FUNCTION UI_GET_NV_FOR_CM

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

  This function is defined for the purpose of supporting two UI callback
  functions: get_call_type, call_is_allowed that are used by the CM to
  determine the call type and whether the call is allowed. This function
  must be completed in a specified time as defined by the CM API.

  This is an example for using the call back functions.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
extern nv_stat_enum_type ui_get_nv_for_cm(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);

/*===========================================================================
FUNCTION ui_kick_dog

DESCRIPTION
  This function reports to dog task that UI is alive and kicking!

RETURN VALUE
  None

DEPENDENCIES
  Dog Task is running.
===========================================================================*/
extern void ui_kick_dog( void );

/* ^L<EJECT> */
#ifdef FEATURE_TEMPERATURE_MONITOR
/*===========================================================================
FUNCTION uitherm_read

DESCRIPTION
  Read the 5 different temperatures.

RETURNS
  None

DEPENDENCIES
  None

===========================================================================*/
void uitherm_read(void);
#endif

/*===========================================================================
FUNCTION ui_init_provisioning

DESCRIPTION
  initialize provisioning

DEPENDENCIES
  the corresponding initialization functions

SIDE EFFECTS
  None
===========================================================================*/
void ui_init_provisioning(cm_client_id_type cm_client_id);

/*===========================================================================
FUNCTION ui_return_provisioning_status

DESCRIPTION
  check provisioning status (success, fail, pending)

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
ui_provisioning_status_type ui_return_provisioning_status(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION ui_return_provisioning_status */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================
FUNCTION ui_check_provisioning

DESCRIPTION
  check provisioning

DEPENDENCIES
  ui_init_provisioning() must have been run

SIDE EFFECTS
  None
===========================================================================*/
boolean ui_check_provisioning(void);

#endif /* UIUTILS_H */
