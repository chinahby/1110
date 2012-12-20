#ifndef DSATI_H
#define DSATI_H

/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the 
  data services AT command processor.

   Copyright (c) 2001-2011 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsati.h_v   1.12   11 Nov 2002 14:02:32   sbandaru  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsati.h#5 $ $DateTime: 2011/04/20 04:28:48 $ $Author: c_avijay $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/11   dvk     Fixed compilation issues.
01/21/09   nc      Added support for SMS Commands.
01/02/09   bs      Added support for 1X AT Phonebook commands.
04/11/08   bs      Fixed Lint High's.
02/07/08   sn      Removed FEATURE_SPECIAL_MDR.
10/29/07   sn      Merged dsatutil_fmt_mixed_param_val() prototype changes.
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
12/25/03   rsl     Changes relating to Unrec. cmds not working.
05/01/03   sb      new funtion declaration (dsat_finished_ath_cmd())
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
03/17/03   ar      Add VALID_TOKEN macro.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
01/17/03   wx      Added dsatutil_fill_modifier_info()
01/07/03   rsl     Removed values relating to SPECIAL_QCPMA.
11/11/02   sb      Declaration of dsatutil_srch_list() function
10/22/02   sb      Merge of latest ETSI related changes
10/15/02    ak     Added #define SPECIAL_QCCHS
09/24/02   atp     Added support for QOS. (1x Release-A feature).
09/18/02   jay     Modified #define MAX_VAL_NUM_ITEM
09/10/02   ak      Compilation fix
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
06/27/02   ar      Add support for more parameters & appending DSM items 
                   based on buffer length
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around DNS 
                   defines.
08/28/01   sjd     Modifications to support new ATCOP VU structure.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dsat.h"
#include "dsm.h"
#include "nv.h"
#include "target.h"

#ifndef FEATURE_DATA_STRIP_ATCOP

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/


/*===========================================================================
                      AT Command Processor constants
===========================================================================*/

/*--------------------------------------------------------------------------
                   Common Limitation Constants
--------------------------------------------------------------------------*/
#define MAX_ARG 13               /*  Maximum number of args for compound   */
                                /*  AT command                            */

#define MAX_LINE_SIZE 260       /*  Maximum length of AT command line     */
                                /*  (including terminating null char      */

#define MIN_ROOM  270           /*  A data item must have at least this   */
                                /*  much room if a new item is to be put  */
                                /*  in it. Based on longest name, longest */
                                /*  response, formatting and some padding */

#define DSM_BUFFER_THRESHOLD 510 /*  Response buffer size at which a new   */
                                 /*  DSM item is requested and appended    */
                                 /*  to buffer chain                       */ 

#define MAX_BROKEN_RANGE 0xFFFF  /*  Denotes end of broken numeric range   */

/*--------------------------------------------------------------------------
                Syntax Flags For the AT Command Parser 
--------------------------------------------------------------------------*/
#define NA    1                 /*  Name field found      */
#define EQ    2                 /*  <=> found             */
#define QU    4                 /*  <?> found             */
#define AR    8                 /*  Argument field found  */


/*===========================================================================
                      Special Processing Codes 
                        
    The following definitions represent the special processing codes that 
    must be assigned to each AT command that requires some special processing 
    in addition to, or instead of, the standard processing performed by the 
    AT command processing engine. 
    SPECIAL_NONE if no special processing is required. 
===========================================================================*/
#define SPECIAL_NONE              0x00
#define SPECIAL_FCLASS            0x01
#define SPECIAL_FCC               0x02
#define SPECIAL_CAD               0x03
#define SPECIAL_CBC               0x04
#define SPECIAL_CBIP              0x05
#define SPECIAL_CHV               0x06
#define SPECIAL_CMIP              0x07
#define SPECIAL_CMUX              0x08
#define SPECIAL_CSQ               0x09
#define SPECIAL_CSS               0x0A
#define SPECIAL_FBS               0x0B
#define SPECIAL_FDR               0x0C
#define SPECIAL_FDT               0x0D
#define SPECIAL_FHS               0x0E
#define SPECIAL_FKS               0x0F
#define SPECIAL_FMI               0x10
#define SPECIAL_FMM               0x11
#define SPECIAL_FMR               0x12
#define SPECIAL_FNS               0x13
#define SPECIAL_GCAP              0x14
#define SPECIAL_GMI               0x15
#define SPECIAL_GMM               0x16
#define SPECIAL_GMR               0x17
#define SPECIAL_GOI               0x18
#define SPECIAL_GSN               0x19
#define SPECIAL_IPR               0x1A
#define SPECIAL_QCDMG             0x1B
#define SPECIAL_QCDMR             0x1C
#define SPECIAL_AMP_F             0x1E
#define SPECIAL_AMP_V             0x1F
#define SPECIAL_Z                 0x20
#define SPECIAL_QCOTC             0x21
#define SPECIAL_QCRLPD            0x22  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCRLPR            0x23  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCPPPD            0x24  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCPPPR            0x25  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCIPD             0x26  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCIPR             0x27  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCCLR             0x28  /*  Used only if DS_PSTATS defined */

#if defined(FEATURE_DS_BROWSER_INTERFACE) || defined(FEATURE_DS_SOCKETS)
#define SPECIAL_QCUDPD            0x29  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCUDPR            0x2A  /*  Used only if DS_PSTATS defined */
#endif /* FEATURE_DS_BROWSER_INTERFACE || FEATURE_DS_SOCKETS*/

#define SPECIAL_QCTCPD            0x2B  /*  Used only if DS_PSTATS defined */
#define SPECIAL_QCTCPR            0x2C  /*  Used only if DS_PSTATS defined */

#ifdef  FEATURE_DS_MTOM
#define SPECIAL_QCMTOM            0x2D  /*  Used only if FEATURE_DS_MTOM   */ 
#endif  /*  FEATURE_DS_MTOM */

#ifdef FEATURE_DS_QNC
#define SPECIAL_QCQNC             0x2E  /*  Used only if FEATURE_DS_QNC    */ 
#endif

#ifdef FEATURE_DS_CHOOSE_SO
#define SPECIAL_QCSO              0x2F  /*  Only if FEATURE_DS_CHOOSE_SO   */
#endif

#define SPECIAL_QCVAD             0x30

#ifdef FEATURE_QMIP 
#error code not present
#endif  /*  FEATURE_QMIP  */

#if defined(FEATURE_DS_DOTG_DATA)
#error code not present
#endif /* FEATURE_DS_DOTG_DATA */

#define SPECIAL_QCCAV             0x33  /*  Answer Voice call              */
#define SPECIAL_QCPKND            0x34  /*  Orig Packet without dialling   */

#if defined(FEATURE_IS95B_MDR) || defined(FEATURE_DS_IS2000)
#define SPECIAL_QCMDR             0x35
#endif /* FEATURE_IS95B_MDR || FEATURE_DS_IS2000 */

#define SPECIAL_QCRLPD33          0x36  /*used with DS_PSTATS && DS_IS2000 */
#define SPECIAL_QCRLPR33          0x37  /*used with DS_PSTATS && DS_IS2000 */

#define SPECIAL_QCDNSPRI          0x38
#define SPECIAL_QCDNSSEC          0x39

#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */

#if defined(FEATURE_DS_IS2000) && defined(FEATURE_IS2000_R_SCH)
#define SPECIAL_QCSCRM            0x3B
#endif /* FEATURE_DS_IS2000  && FEATURE_IS2000_R_SCH */

#if defined(FEATURE_DS_IS2000) && defined(FEATURE_IS2000_R_SCH)
#define SPECIAL_QCTRTL            0x3C
#endif /* FEATURE_DS_IS2000  && FEATURE_IS2000_R_SCH */

#ifdef FEATURE_DS_MOBILE_IP
#define SPECIAL_QCMIP             0x3D
#define SPECIAL_CRM               0x3E
#define SPECIAL_QCMIPP            0x3F
#endif  /*  FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MOBILE_IP
#define SPECIAL_QCMIPT            0x43
#endif  /*  FEATURE_DS_MOBILE_IP */

#define SPECIAL_QCPREV            0x44

#ifdef FEATURE_DS_MOBILE_IP
#define SPECIAL_QCMIPNAI          0x45
#define SPECIAL_QCMIPRT           0x46
#define SPECIAL_QCMIPMASS         0x47
#define SPECIAL_QCMIPMHSS         0x48
#define SPECIAL_QCMIPPHA          0x49
#define SPECIAL_QCMIPSHA          0x4A
#define SPECIAL_QCMIPMASPI        0x4B
#define SPECIAL_QCMIPMHSPI        0x4C
#define SPECIAL_QCMIPHA           0x4D
#define SPECIAL_QCMIPEP           0x4E
#define SPECIAL_QCMIPGETP         0x4F
#define SPECIAL_QCMIPMASSX        0x50
#define SPECIAL_QCMIPMHSSX        0x51
#endif  /*  FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_IS2000_REL_A
#define SPECIAL_QCQOSPRI          0x52
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_CHS
#define SPECIAL_QCCHS             0x53
#endif /* FEATURE_IS2000_CHS */

#define SPECIAL_QCCHV             0x54

/*===========================================================================
               Bit-Mapped AT Command Attributes Constants 
===========================================================================*/
#define ATTRIB_NONE   0x00000000    /*  No attributes                      */
#define NO_RESET      0x00100000    /*  ATZ or AT&F leave 
                                        the command value alone            */
#define BROKEN_RANGE  0x00080000    /*  Discontinuous numeric range        */
#define MULTI_INDEX   0x00040000    /*  Multi-dimensioned parameter set    */
#define NO_DISPLAY    0x00020000    /*  Value not displayed (eg  password) */
#define RESTRICTED    0x00010000    /*  Allowed in PIN restricted mode     */
#define CONFIG        0x00008000    /*  Variable: takes argument           */
#define FCLASS2P0     0x00004000    /*  Class 2.0 Fax item                 */
#define LOCAL_TEST    0x00002000    /*  Execute =? command locally         */
#define ALWAYS        0x00001000    /*  always transmit in config          */
#define SIMPLE_DEC    0x00000800    /*  3 digit, leading 0 decimal         */
#define STRING        0x00000400    /*  Value is string                    */
#define HEX           0x00000200    /*  Numeric value is Hex               */
#define NO_QUOTE      0x00000200    /*  String value has no '"'s           */
#define EXTENDED      0x00000100    /*  Extended syntax                    */
#define READ_ONLY     0x00000080    /*  Read only; may have '?' in syntax  */
#define DOWN_LOAD     0x00000040    /*  Transmit in config if not default  */
#define LIST          0x00000020    /*  def_lim_ptr points to a list       */

#define MIXED_PARAM   0x00000002    /*  Mixed parameter types              */
#define YES_QUOTE     0x00000001    /*  The string argument must be 
                                    quoted                            */


#define MAX_VAL_NUM_ITEM 0xFFFFFFFF /*  Max value of a numeric AT parm     */
/*---------------------------------------------------------------------------
    The following defines MUST match the values in                     
    "dsat_result_enum_type" in dsat.h".        
    MIN_CELL_RES must be LESS than MIN_CELL_ERR       
---------------------------------------------------------------------------*/
#define MIN_CELL_RES  10  /*  First cellular result code. */
#define MIN_CELL_ERR  20  /*  First cellular result code with +CERROR: prefix */


/*===========================================================================
                        Number of Asynchronous Events    
===========================================================================*/
#define DSATI_NUM_ASYNC_EVENT_TABLE_ENTRIES (DS_AT_ASYNC_EVENT_END -  \
                                             DS_AT_ASYNC_EVENT_START)
#define DSATI_NUM_ASYNC_EVENTS (DSATI_NUM_ASYNC_EVENT_TABLE_ENTRIES - 1)

/*===========================================================================
  bit masks for dsati_dial_str_type's attrib field
===========================================================================*/
/* Dial string ends with ;                                      */
#define         DIAL_ATTRIB_SEMICOLON           0x0001

/* Dial string starts with > in etsi mode                       */
#define         DIAL_ATTRIB_GREAT               (0x0001 << 1)

/* Dial string ends with G at the end or 2nd end before ;       */
#define         DIAL_ATTRIB_UP_G                (0x0001 << 2)

/* Dial string ends with g at the end or 2nd end before ;       */
#define         DIAL_ATTRIB_DN_G                (0x0001 << 3)

/* Dial string ends with I at the end or 2nd end before ; 
   or 3rd end before ;[G|g]                                     */
#define         DIAL_ATTRIB_UP_I                (0x0001 << 4)

/* Dial string ends with i at the end or 2nd end before ; 
   or 3rd end before ;[G|g]                                     */
#define         DIAL_ATTRIB_DN_I                (0x0001 << 5)

/* Dial string has the pattern: *[98|99]...# in etsi mode or
   #777\0 in cdma mode.                                         */
#define         DIAL_ATTRIB_PKT                 (0x0001 << 6)

/* Dial string contains char other than digit * # +, 
   Specificly:  ABCD in etsi mode
                ABCDTP,W@!$; in cdma mode                       */
#define         DIAL_ATTRIB_ASCII               (0x0001 << 7)

/* Dial string'first char is '+'                                */
#define         DIAL_ATTRIB_START_PLUS          (0x0001 << 8)


/*===========================================================================
  Utility macros
===========================================================================*/

/* Test for null string token in parameter list */
#define VALID_TOKEN(tok) \
          ((tok < tok_ptr->args_found) && \
           (NULL != tok_ptr->arg[tok]) && \
           ('\0' != *tok_ptr->arg[tok]))


/*===========================================================================
                     Debugging Message Definitions
===========================================================================*/
#define DSATI_ERROR_MSG()  MSG_MED("DSAT_ERROR result", 0, 0, 0)

#ifdef DEBUG_DSATCOP
/* #define DSATI_DEBUG_CR */
/* #define DSATI_DEBUG_CRC */
/* #define DSATI_DEBUG_CMEE */
#define DSATI_DEBUG_CPB 
#endif /* DEBUG_DSATCOP */

/*--------------------------------------------------------------------------
   Macro's for GSM @ character support 
-------------------------------------------------------------------------- */
/* The replacement character for GSM @(0x00) is chosen to be 0xE6 as GSM
valid characters are only from 0x00 - 0X7F and 0xE6 doesnt fall into that. */
#define GSM_AT_CHAR_REPLACEMENT  0xE6
#define GSM_AT_CHAR  0x00

/*---------------------------------------------------------------------------
     Length of a single character in each of these character sets:
     e.g. 4 hexadecimal characters to represent a single UCS2 character, etc.
---------------------------------------------------------------------------*/
#define UCS2_CHAR_LEN      4
#define HEX_CHAR_LEN       2
#define GSM_CHAR_LEN       1
#define IRA_CHAR_LEN       1
#define EIGHT_BIT_CHAR_LEN 1
#define PBM_8BIT_CHAR_LEN  1
#define PBM_UCS2_CHAR_LEN  2

/*===========================================================================
                 SIO Data Preprocessor Enumeration Types
===========================================================================*/

/*---------------------------------------------------------------------------
  SIO data preprocessor mode enumeration type.
---------------------------------------------------------------------------*/
typedef enum
{
  DSATI_AT_CMD_MODE,               /* AT command line entry mode      */
  DSATI_ABORT_CMD_MODE,            /* Abort AT command mode           */
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  DSATI_SMS_MSG_ENTRY_MODE,        /* ETSI SMS message entry mode     */
  DSATI_SMS_MSG_END_MODE,          /* ETSI SMS message end mode       */
#endif /* (FEATURE_ETSI_SMS) || (FEATURE_CDMA_SMS) */
  DSATI_INVALID_MODE
} dsati_preproc_mode_enum_type;

/*---------------------------------------------------------------------------
     Character sets between which translation is supported. Not neccesarily 
     all supported by +CSCS command, additional alphabets are for use in 
     conversion function. Values indices in array dsat_cscs_valstr should 
     match enum dsati_chset_type.
---------------------------------------------------------------------------*/
typedef enum {
  ALPHA_IRA = 0,        /* International Reference Alphabet T.50 */
  ALPHA_GSM,            /* GSM 7 bit alphabet, not packed to 7 bits
                          (will not contain @ (0x00); might have got mapped to 
                           0xe6 at the ATCOP parser ) */
  ALPHA_UCS2,           /* UCS2 Unicode, rep'd by "4 hex character"-tuplets */
  ALPHA_HEX,            /* HEX, rep'd by "2 hex character"-tuplets */
  ALPHA_8BIT,           /* Octets, of 0-255 value */
  ALPHA_PBM_8BIT,       /* PBM's 8 bit alphabet */
  ALPHA_GSM_WITH_AT,    /* GSM 7 bit alphabet, which contains @ (0x00)
                           This character set should be used when 
                           dsatutil_convert_chset has to be called for sending the
                           converted string to modules other than ATCOP. 
                           the output might contain GSM7 bit @ (0x00) so, str_len on 
                           output string might return a wrong value. Output_len has to be 
                           calculated from the input string only.
                           This is for internal use with in ATCOP. This character
                           set is exactly the same as ALPHA_GSM above */
  ALPHA_PBM_UCS2,       /* The 2 byte representation of UCS2 for PBM */
  ALPHA_MAX
} dsati_chset_type;


/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
  The following structure is the data type for tokens in the parsing of   
  the AT commands.

  working_at_line - Pointer to command line to be processed. 
  name            - The name of the AT command.
  op              - Syntax flags. Can be one of four valid values (NA, 
                    EQ, QU, AR) or an invalid value. 
  arg             - AT command arguments.
  args_found      - Keeps track of the number of AT command arguments 
                    found.
  end_of_line     - Pointer to end of data (last NULL character) placed in
                    working_at_line buffer.
  cmd_category    - Category of command (Basic, Extended, S-register,
                    Vendor Specific)
---------------------------------------------------------------------------*/
typedef enum
{
  BASIC_AT_CMD = 0,
  SREG_AT_CMD,
  EXT_AT_CMD,
  VENDOR_AT_CMD,
  NUM_AT_CMD_CATEGORIES
} cmd_category_enum_type;

typedef enum
{                       
  ATOI_OK,              /*  conversion complete             */
  ATOI_NO_ARG,          /*  nothing to convert              */
  ATOI_OUT_OF_RANGE     /*  character or value out of range */
} atoi_enum_type;

typedef enum
{                       
  PARAM_MISS,           /*  Not found in values list        */
  PARAM_HIT,            /*  Found in values list            */
  PARAM_NULL            /*  No parameter specified          */
} param_srch_enum_type;

typedef enum
{
  ACTION_QUOTE_BOTH_OK,    /* don't care about the quote 
                              If the argument is quoted, trip 
                              out the quotes.
                              If the argument is not quoted,
                              pass it through */
  ACTION_QUOTE_YES_QUOTE,  /* the argument must be quoted */
  ACTION_QUOTE_NO_QUOTE    /* the argument must not be quoted */
} action_quote_e_type;

typedef struct
{
  byte  *working_at_line; 
  byte *name;                        
  unsigned int op;                    
  byte *arg[MAX_ARG];                  
  unsigned int args_found;
  byte *end_of_line;
  cmd_category_enum_type cmd_category;
} tokens_struct_type;

typedef struct {
  byte          *num;           /* points to dial cmd token's arg[0]    */
  uint16        len;            /* dial string length                   */
  uint16        attrib;         /* attributes                           */
} dsati_dial_str_type;

/*-------------------------------------------------------------------------
   Operating mode for selection of different AT command sets from
   command table.
-------------------------------------------------------------------------*/

typedef enum
{
  ETSI_CMD_MODE,
  CDMA_CMD_MODE,
  NUM_OPER_CMD_MODES
} dsati_operating_cmd_mode_type;

/* Internal Current Operating Mode Variable */
typedef enum
{
  DSAT_MODE_GSM,  /* GSM mode   */
  DSAT_MODE_WCDMA,/* WCDMA mode */
  DSAT_MODE_CDMA, /* CDMA mode  */
  DSAT_MODE_MAX   /* Mode None  */
} dsati_mode_e_type;

/*---------------------------------------------------------------------------
  The following type definition defines a pointer to an unrecognized
  command handler function called when the AT commmand processor does
  not recognize the command.

  cmd_line_ptr - Pointer to a NULL terminated command line with the "AT"
                 prefix removed.
---------------------------------------------------------------------------*/
typedef dsat_result_enum_type (*dsati_unrec_cmd_handler_ptr_type)
(
  byte* cmd_line_ptr
);


/*---------------------------------------------------------------------------
   The data type below is used to define the default value and the lower and 
   upper bounds of the arguments for AT commands with numeric arguments.         
---------------------------------------------------------------------------*/
typedef struct
{
  dsat_num_item_type default_v;   /* The default value for the argument*/
  const dsat_num_item_type lower; /* Lower limit of the allowed range. */
  const dsat_num_item_type upper; /* Upper limit of the allowed range. */
} dflm_type;
  

/*--------------------------------------------------------------------------
   The data type below is used to define the default value and the allowed 
   range of values of the arguments, for AT commands of type LIST.               

    default_v   - Default value of the argument.
    list_v      - A pointer to and array of 8-byte strings that represent 
                  all of the valid values that the argument can assume. The 
                  ATCoP performs a string match with the elements in this 
                  array order to determine whether or not an argument's 
                  value is within valid range.
    list_t      - Pointer to a string that is returned word for word in 
                  response to a test command. 
--------------------------------------------------------------------------*/ 
typedef struct
{
  dsat_num_item_type default_v;            
  const dsat_string_item_type (*list_v)[8];
  const dsat_string_item_type *list_t;     
} def_list_type;

/*--------------------------------------------------------------------------
   The data type below is used to define the default value and the allowed 
   range of values of the mixed arguments, for AT commands with attrib
   MIXED_PARAM .
--------------------------------------------------------------------------*/ 

/* mixed_def_u_type is used in mixed_def_s_type 
   The corresponding fields in the union takes effect
   depending on attrib in mixed_def_s_type */
typedef union
{
  const def_list_type def_list;   /* when attrib contains CONFIG and 
                                     LIST */
  const dflm_type     dflm;       /* when attrib contains CONFIG but no
                                     LIST */
  const word          string_len; /* when attrib is STRING, this is the 
                                     max length of the string val, 
                                     excluding trailing null terminator
                                  */
} mixed_def_u_type;

typedef struct
{
  const uint32 attrib;  /* QUOTE:  the argument must be quoted 
                           STRING: def is string_len, write command 
                                   writes the arument to the val */
  const mixed_def_u_type * def;
} mixed_def_s_type;

/*---------------------------------------------------------------------------
   The following data type is the data structure for a command table entry.      
   
   name        -  The name of the AT cmd, including the leading "+", "$" 
                  or "$" and the terminating NULL.
   attrib      -  A 32-bit mask that specifies one or more command 
                  attributes.       
   special     -  The "special processing code," if any, or SPECIAL_NONE. 
                  The special processing codes are defined in this file. 
                  A unique special processing code must be assigned to 
                  each AT cmd that requires some special processing. 
   compound    -  The number of arguments, for numeric commands, or the 
                  maximum string length (not including the terminating NULL), 
                  for string commands.
   val_ptr     -  A pointer to the variable that stores the argument value. 
                  If the variable is an array, this is a pointer to the 
                  first element of the array. For commands with the LIST 
                  attribute the value stored in *val_ptr are not the 
                  actual parameters. Instead, they are indices into array 
                  pointed to by list_v.
   def_lim_ptr -  Defines the default value and the allowed range for the 
                  arguments.
   proc_func   -  Pointer to function to be called to process the command.
   abort_func  -  Pointer to function to be called to abort the command.
                  If the pointer is NULL, command is not abortable.
---------------------------------------------------------------------------*/
typedef struct dsati_cmd_struct
{
  byte name[20];         
  uint32 attrib;    
  byte special;           
  byte compound;          
  const void *val_ptr;     
  const void *def_lim_ptr;
  dsat_result_enum_type (*proc_func)( dsat_mode_enum_type,
                                      const struct dsati_cmd_struct*,
                                      const tokens_struct_type*,
                                      dsm_item_type* );
  boolean (*abort_func)( const struct dsati_cmd_struct* );
} dsati_cmd_type;


/*--------------------------------------------------------------------------
   The data types below is used to define command table structure.

    table_ptr   - Pointer to an array of command table entries.
    table_size  - Pointer to number of command table entries in array.

   When multiple operating modes are supported, the AT command table
   will be two dimensional, with rows representing AT command categories
   and columns representing operating command modes.
--------------------------------------------------------------------------*/
typedef struct
{
  const dsati_cmd_type *table_ptr;
  const unsigned int *table_size;
} dsati_at_cmd_table_entry_type;

typedef dsati_at_cmd_table_entry_type*
                                dsati_at_cmd_table_type[NUM_AT_CMD_CATEGORIES]
                                                       [NUM_OPER_CMD_MODES];


/*--------------------------------------------------------------------------
   The data types below are used to define the asynchronous event handler
   table entry structure.

    event            - 3G Dsmgr sub-task command generating asynchronous
                       event
    event_handler[ ] - table of function pointers to handle event for
                       each mode

   When multiple operating modes are supported, the asynchronous event
   handler tables will contain more than one row, with each row
   representing one of the operating command modes.
--------------------------------------------------------------------------*/
typedef dsat_result_enum_type (*dsati_async_event_handler_type)
                                                   ( dsat_mode_enum_type,
                                                     ds_cmd_type* );

typedef struct
{
  const ds_cmd_enum_type event;
  const dsati_async_event_handler_type event_handler[NUM_OPER_CMD_MODES];
} dsati_async_event_table_entry_type;


/*---------------------------------------------------------------------------
     Data type for a list strings. Used in function dsat_srch_list.    
---------------------------------------------------------------------------*/  
typedef byte list_type[][8];    


/*===========================================================================
                 ATCOP Utility Function Enumeration Types
===========================================================================*/

/*---------------------------------------------------------------------------
     Specifies the format type for displaying AT parameters.
---------------------------------------------------------------------------*/  
typedef enum        
{                   
  RESPONSE,             /*  Generates response to command - */
                        /*    No name for +Fxxx             */

  RESPONSE_ALL,         /*  Generates response to command - */
                        /*    Names all commands            */

  SET                   /*  Generates a SET command - used  */
                        /*    to transmit configuaration    */
                        /*    to the base station           */
} fmt_enum_type;


/*---------------------------------------------------------------------------
     Specifies mode of string argument storage.
---------------------------------------------------------------------------*/  
typedef enum
{
  STORE,                /*  stores argument                 */
  CAT                   /*  concatenates arg to existing val*/
} put_string_enum_type;


/*---------------------------------------------------------------------------
     Specifies mode of IP address conversion.
---------------------------------------------------------------------------*/  
typedef enum
{
  STRING_TO_UINT = 0,
  UINT_TO_STRING
} ip_convert_enum_type;


/*---------------------------------------------------------------------------
     Formatting options for mult-index read commands 
---------------------------------------------------------------------------*/  
typedef enum
{
  MULTI_INDEX_TEST,
  MULTI_INDEX_READ,
  MULTI_INDEX_AMPV
} multi_index_fmt_e_type;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*-------------------------------------------------------------------------
  Enum for +CMSS ERRORS (3G TS 27.007 section 3.2.5)
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CMS_NONE               = 0,
  DSAT_CMS_ME_FAILURE         = 300,
  DSAT_CMS_SERVICE_RESERVED   = 301,
  DSAT_CMS_OP_NOT_ALLOWED     = 302,
  DSAT_CMS_OP_NOT_SUPPORTED   = 303,
  DSAT_CMS_INVALID_PDU_PARAM  = 304,
  DSAT_CMS_INVALID_TXT_PARAM  = 305,
  DSAT_CMS_SIM_NOT_INSERTED   = 310,
  DSAT_CMS_SIM_PIN_REQ        = 311,
  DSAT_CMS_PHSIM_PIN_REQ      = 312,
  DSAT_CMS_SIM_FAILURE        = 313,
  DSAT_CMS_SIM_BUSY           = 314,
  DSAT_CMS_SIM_WRONG          = 315,
  DSAT_CMS_SIM_PUK_REQ        = 316,
  DSAT_CMS_SIM_PIN2_REQ       = 317,
  DSAT_CMS_SIM_PUK2_REQ       = 318,
  DSAT_CMS_MEM_FAILURE        = 320,
  DSAT_CMS_INVALID_INDEX      = 321,
  DSAT_CMS_MEM_FULL           = 322,
  DSAT_CMS_SCA_ADDR_UNKNOWN   = 330, 
  DSAT_CMS_NO_SERVICE         = 331,
  DSAT_CMS_NETWORK_TIMEOUT    = 332,
  DSAT_CMS_CNMA_NOT_EXP       = 340,
  DSAT_CMS_UNKNOWN_ERR        = 500,
  DSAT_CMS_MAX                = 512
} dsati_cms_err_e_type;
#endif /* (FEATURE_ETSI_SMS) || (FEATURE_CDMA_SMS) */

/*---------------------------------------------------------------------------
     Abortable command state
---------------------------------------------------------------------------*/
typedef enum {
  NOT_ABORTABLE,        /* current command is not an abortable command  */
  ABORTABLE,            /* current command is abortable but the aborting
                           hasn't happen                                */
  ABORTING              /* a char has been received to abort the 
                           current command                              */
} dsati_abort_type;

/*---------------------------------------------------------------------------
     Operating system timers
---------------------------------------------------------------------------*/
/* Delay x mS waiting for DSM large items to be freed when listing large 
   quantities of PB entries or SMSes */
#define DSAT_FREE_ITEM_DLY 5

typedef enum dsat_timer_id
{
  DSAT_TIMER_VOICE_RING,   /* Voice call RING response         */
  DSAT_TIMER_FREE_ITEM,
  DSAT_TIMER_MAX
}dsat_timer_id_e_type;

typedef dsat_result_enum_type (*dsat_timer_exp_hdlr_type)
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);


typedef struct dsat_timer
{
  dsat_timer_id_e_type      timer_id;    /* Timer identifier             */
  rex_timer_type *          timer_ptr;   /* Operating system timer       */
  rex_timer_cb_type         cb_func;     /* REX callback on timer expiry */
  dsat_timer_exp_hdlr_type  hdlr_func;   /* DS event handler             */
  dword                     duration;    /* Length of timer              */
  boolean                   is_defined;  /* Timer registered with OS     */
  boolean                   is_running;  /* Timer active flag            */
} dsat_timer_s_type;

/*---------------------------------------------------------------------------
    AT Command Table
---------------------------------------------------------------------------*/
extern const dsati_at_cmd_table_type at_cmd_table;


/*---------------------------------------------------------------------------
    AT Command State
---------------------------------------------------------------------------*/
extern dsat_mode_enum_type dsatcmdp_at_state;  


/*---------------------------------------------------------------------------
    Asynchronous Event Handler Table
---------------------------------------------------------------------------*/
extern const dsati_async_event_table_entry_type
                                   async_event_table[DSATI_NUM_ASYNC_EVENT_TABLE_ENTRIES];


/*---------------------------------------------------------------------------
    Flag that indicates whether or not an abortable command is in progress.
---------------------------------------------------------------------------*/
extern dsati_abort_type dsatcmdp_abortable_state;


/*---------------------------------------------------------------------------
    Flag that indicates whether or not calling the function to send a
    result code to the TE terminates the abort handling for that command.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_end_abort_in_send_rsp;


/*---------------------------------------------------------------------------
  Flag that indicates whether or not indications that are sent to TE
  should be blocked.  Indications are blocked from the point at which an
  AT command line starts being entered to the end of processing the
  command line and also while a data call is active.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_block_indications_flag;

/*---------------------------------------------------------------------------
    Flag that indicates whether or not ME is operating in restricted
    command mode due to authorization requirement.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_restricted_commands;

/*---------------------------------------------------------------------------
    Flag that indicates a async cmd is under processing,
    Atcop preprocessor does not process any incoming char when it is TRUE.
---------------------------------------------------------------------------*/
extern boolean dsatcmdp_processing_async_cmd;

/*---------------------------------------------------------------------------
    Flag that indicates ATH cmd is under processing,
    continues to process the AT command line if TRUE.
---------------------------------------------------------------------------*/
extern boolean dsati_processing_ath_cmd;

#ifdef FEATURE_DSAT_FLOW_CONTROL
/* Flag to indicate if tx (dsat_to_sio) watermark hit high.
   If this flag is high, ATCOP should halt current commands that
   requesting data from the subsystem
*/
extern boolean dsati_tx_wm_high;

/* Function to reenter current command processing which has halted
   when tx (dsat_to_sio) watermark hit high.
*/
extern dsati_async_event_handler_type dsati_to_sio_low_handler;

#endif /* FEATURE_DSAT_FLOW_CONTROL */

/* Trigger the second layer of flow control by the free item timer when
   DSM large item count goes below this quantity */
#define DSAT_FREE_ITEM_TRIGGER 3

/* Handler function and timer to delay when listing large quantities of PB 
   entries or SMSes and DSM large items are close to being exhausted: this 
   needed as USB dequeues from DS3GSIOLIB WM as quickly as queued so normal 
   flow control doesn't kick in, USB can acquire chain of all available DSM 
   large items and never have a chance to free them as its task doesn't get 
   a chance to run while DS and WMS or PBM are busy with this. */
extern rex_timer_type dsati_free_item_timer;

/* Handler function to set pointer in dsat_timer_s_type to.
   It calls function pointer below if not NULL. */
extern dsat_result_enum_type dsati_free_item_timer_expired_handler
(
  dsat_mode_enum_type   mode, 
  ds_cmd_type         * cmd_ptr 
);

/* Handlers for PB entry/SMS reads can set this function to reenter 
   processing following delay to allow DSM items to be freed by USB task. */

extern dsati_async_event_handler_type dsati_handle_free_item_timer_expiry;

/*-------------------------------------
  Table to convert IRA characters to GSM 7 bit default alphabets
  Refer to 3G TS 27.005 Annex A for more details                 
---------------------------------------*/
extern const uint8 ira_to_gsm7[];
/*-------------------------------------
  Table to convert GSM 7 bit default alphabets to IRA characters 
---------------------------------------*/
extern const uint8 gsm7_to_ira[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
     

/*===========================================================================

FUNCTION dsatprep_set_preprocessor_mode

DESCRIPTION
  Changes the SIO data preprocessor mode of operation.  Operating modes
  can be: AT command mode, abort command mode, or SMS message entry mode.
  AT command mode is the default mode of SIO data preprocessor operation.
  It handles normal AT command line entry.  Abort command mode provides
  processing to abort an abortable command when any character is received.
  SMS message entry mode handles input of SMS messages.

  A state machine in this function controls transitions between SMS
  message entry mode and abort command mode when abortable commands are
  being processed.  When an abortable command is being processed and
  current mode is SMS message entry mode, entry into abort command mode
  will be delayed until the next call to this function.  Once abort
  command mode is exited, AT command mode will always be entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Mode changes may be delayed when abortable SMS commands are being
  processed (see description above).  SIO AUTODETECT mode is rearmed
  on entry into AT command line mode.

===========================================================================*/
extern void dsatprep_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode    /* mode to set data preprocessor to */
);


/*===========================================================================

FUNCTION DSATPAR_PARSE_CMD_LINE

DESCRIPTION
  This function parses a NULL terminated command line with the "AT"
  removed.  It parses each command and its associated arguments within
  the command line.  A token structure is produced for each command
  parsed.  Each token structure produced is placed on the AT command
  processor token queue for later processing.  

DEPENDENCIES
  None

RETURN VALUE
  Result code indicating whether an error occurred during parsing or
  parsing was successful (DSAT_OK or DSAT_ERROR).

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_result_enum_type dsatpar_parse_cmd_line
(
  byte *cmd_line                /*  NULL terminated command line with
                                    "AT" prefix and <CR> removed. */
);


/*===========================================================================

FUNCTION DSATCMDP_INIT_CONFIG

DESCRIPTION
  This function initializes the complete set of AT command paramters
  to their default values.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
extern void dsatcmdp_init_config( void );


/*===========================================================================

FUNCTION DSATCMDP_REG_UNREC_CMD_HANDLER

DESCRIPTION
  This function registers a handler that is called when an unrecognized
  AT command is received.  The handler is registered for the AT command
  operating mode specified.  If no handler is registered for a command
  operating mode, an error result code is returned by default when an
  unrecognized command is encountered.

  This functionality is intended to provide support for IS-707 unrecognized
  command handling.
  
  The handler may be deregistered for the AT command mode specified by
  passing in a NULL pointer.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dsatcmdp_reg_unrec_cmd_handler
(
  dsati_unrec_cmd_handler_ptr_type func_ptr,    /* Pointer to mode specific
                                                   unrecognized command
                                                   handler */
  dsati_operating_cmd_mode_type oper_mode       /* AT command set operating
                                                   mode */
);


/*===========================================================================

FUNCTION DSATCMDP_QUEUE_TOKEN

DESCRIPTION
  Queues token structure generated by parser on token queue.  A token
  structure is generated by each command parsed by the AT command parser.

DEPENDENCIES
  Token queues must have been initialized.

RETURN VALUE
  DSAT_OK if token structure was successfully queued.  DSAT_ERROR
  if no buffers are available for token structure and it could not be
  placed on queue.

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_result_enum_type dsatcmdp_queue_token
(
  tokens_struct_type *token_ptr       /* Pointer to token structure */
);


/*===========================================================================

FUNCTION DSATCMDP_FLUSH_TOKEN_QUEUE

DESCRIPTION
  Flushes token queue.  Any token structures in queue are removed and
  placed back on the token free queue.
  
DEPENDENCIES
  Token queues must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_flush_token_queue( void );


/*===========================================================================

FUNCTION DSATCMDP_QUEUE_CMD_RESULTS

DESCRIPTION
  This function places the results of AT commands in a queue,
  so they can be output (or discarded) at the end of the command
  line. It will output the contents of the queue if more than a
  defined number of DSM items are on the queue.  This is done
  to control DSM usage, as some responses can be large.

  Note: The queue count threshold may require adjustment for IS-707
  mode of operation, to insure the entire command line response
  can be buffered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsatcmdp_queue_cmd_results
(
  dsm_item_type *item_ptr       /* Pointer to command results to queue */
);


/*===========================================================================

FUNCTION DSATCMDP_SEND_QUEUED_CMD_RESULTS

DESCRIPTION
  This functions flushes the results queue to the serial port or PS
  protocol stack based on the registered response routing function.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_send_queued_cmd_results( void );


/*===========================================================================

FUNCTION DSATCMDP_DISCARD_QUEUED_CMD_RESULTS

DESCRIPTION
  This functions discards any queued up AT command results.  Required
  for IS-707 unrecognized command processing.
  
DEPENDENCIES
  AT command processor must have been initialized prior to calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatcmdp_discard_queued_cmd_results( void );


/*===========================================================================

FUNCTION DSATCMDP_HANDLE_ASYNC_CMD_RSP

DESCRIPTION
  Handles the raw unformatted AT command response generated by an
  asynchronous command handler function.  Formats the raw response
  contained in possibly chained DSM items and queues it up for
  transmission or sends it immediately.  The DSM item pointed to by
  the raw response buffer is freed.

  The response is formatted based on the input parameter "type of
  response".  This specifies whether the response contained in the
  DSM item is the start, end, complete, or intermediate response and
  in turn if a header, trailer, header and trailer, or no header and
  trailer should be added to the response, respectively.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Uses DSM item pointed to by formatted_rsp_ptr.

===========================================================================*/
void dsatcmdp_handle_async_cmd_rsp
(
  dsm_item_type *raw_rsp_ptr,   /* Ptr to raw unformatted AT cmd response */
  dsat_rsp_enum_type type_of_rsp   /* Type of response data in DSM item   */
);


/*===========================================================================

FUNCTION DSATCMDP_ABORT_COMMAND

DESCRIPTION
  Calls abort command handler located in command table to abort the
  abortable AT command currently being processed.  This function
  may be called multiple times during the command abort process,
  although only the first call to this function during a particular
  command's abort sequence will invoke the command abort handler.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  May set abortable command table entry pointer to NULL.

===========================================================================*/
extern boolean dsatcmdp_abort_command( void );


/*===========================================================================

FUNCTION DSATCMDP_COMMAND_ABORT_COMPLETE

DESCRIPTION
  Clears flag used during abort command processing and places SIO
  preprocessor in AT command mode.  Must be called when command abort
  processing has been completed, even if the command was not sucessfully
  aborted.  

DEPENDENCIES
  Should only be called when the process of aborting a command has
  completed, and must be called even if command was not aborted
  successfully.

RETURN VALUE
  None

SIDE EFFECTS
  Clears abort command flag and sets SIO preprocessor to AT command
  mode.

===========================================================================*/
extern void dsatcmdp_command_abort_complete( void );


/*===========================================================================

FUNCTION DSATCMDP_DISABLE_CMD_ABORT_COMPLETE

DESCRIPTION
  Disables termination of abort command handling of current command
  being processed when call to function to send result code is made.
  By default, calls to function to send result code will end the abort
  handling of an abortable command in progress.

  Should be called by ATCoP command execution function requiring this
  behavior for abortable commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Clears end abort in send response flag.

===========================================================================*/
extern void dsatcmdp_disable_cmd_abort_complete( void );


/*===========================================================================

FUNCTION DSATCMDP_GET_OPERATING_CMD_MODE

DESCRIPTION
  Gets the current AT command processor operating service mode used for
  selection of different AT command sets from command table.

DEPENDENCIES
  None
  
RETURN VALUE
  Current operating service mode.

SIDE EFFECTS
  None
  
===========================================================================*/
extern dsati_operating_cmd_mode_type dsatcmdp_get_operating_cmd_mode( void );


/*===========================================================================

FUNCTION DSATCMDP_GET_CURRENT_MODE

DESCRIPTION
  Gets the current AT command mode, GSM or WCDMA or CDMA.

DEPENDENCIES
  None

RETURN VALUE
  Current service mode.

SIDE EFFECTS
  None

===========================================================================*/
dsati_mode_e_type dsatcmdp_get_current_mode( void );


/*===========================================================================

FUNCTION DSATCMDP_BLOCK_INDICATIONS

DESCRIPTION
  Returns value to indicate whether or not indications sent to the TE
  should be blocked.  No indications should be sent to TE from the
  time entering an AT command line begins to the time command line
  processing completes.  Command line processing is considered completed
  after the final result code or response is sent to the TE.  Also,
  indications should not be sent to the TE while in a data call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if sending of indications to TE should be blocked or FALSE
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsatcmdp_block_indications( void );


/*===========================================================================

FUNCTION  DSATRSP_SEND_RESPONSE 

DESCRIPTION
  Sends AT response based on routing function registered or to TE
  if no function is registerd.  Routing function routes response to TE
  or PS protocol stack based on operating service mode and AT command
  state.

DEPENDENCIES
  Routing is only performed if routing function is registered, otherwise
  response is sent to TE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatrsp_send_response
(
  dsm_item_type *rsp_ptr,                 /* Ptr to response buffer to send */
  const boolean rearm_autodetect          /* Flag to rearm SIO autodetect */
);


/*===========================================================================

FUNCTION DSATRSP_FMT_RESULT_CODE

DESCRIPTION
  Places a formatted result code in the output buffer.  The type of
  result code is identified in the input parameter.  Formatting is
  based on whether or not the mode is verbose.  The result codes
  generated are based on mode specific result code generating
  functions.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatrsp_fmt_result_code
(
  dsat_result_enum_type result_code,   /* Result code                      */
  dsm_item_type **out_ptr_ptr          /* Location to store formatted
                                          response, output will NOT be NULL
                                          terminated                       */
);


/*===========================================================================

FUNCTION DSATRSP_SET_BASIC_RESULT_CODE

DESCRIPTION
  Produces and unformatted ITU-T V.25 ter basic result code and returns
  in DSM item.  Result code returned will be either numeric or verbose,
  based on input parameter.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted basic result code.

SIDE EFFECTS
  None
  
===========================================================================*/
dsm_item_type *dsatrsp_set_basic_result_code
(
  dsat_result_enum_type result_code,    /* Basic result code               */
  boolean               verbose         /* verbose result code or numeric  */
);


/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_cell_result
(
  dsat_result_enum_type result_code         /* Command result code */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL

DESCRIPTION
  This function formats the result of a value inquiry "?" for a normal
  numeric variable.  The return string is a null terminated 3 - digit
  decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_val
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr                     /*  point to buffer for result  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL_EX

DESCRIPTION
  This function formats the value of an extended format numeric
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <value>                            for a simple variable, or
      +NAME: <value1>, <value2>, ... , <valueN> for a compound variable.

  For the SET command, it is in the form

      +NAME=<value>;                            for a simple variable, or
      +NAME=<value1>, <value2>, ... , <valueN>; for a compound variable.


  The value will be in decimal, unless the parameter is specified with
  a HEX attribute in the parse table entry.  Leading zeros are suppressed.

  ".0" is added to the result for +FCLASS=2.0

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_val_ex
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr,                    /*  point to buffer for result  */
  fmt_enum_type fmt_sw             /*  selects response or set fmt */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" command
  for a numeric variable.

  The formatted value is returned as a null terminated string.
DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.


SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_num_range
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr                     /*  point to buffer for result  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" for a string
  variable.  The function always returns
      +NAME: (20,21,23-7E)
  which is the range of printable ASCII characters.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void dsatutil_fmt_string_range
(
  const dsati_cmd_type *tab_entry,       /*  entry in parse table  */
  byte *rb_ptr,                          /*  pointer to buffer for result  */
  const dsat_string_item_type *range_ptr /*  pointer to range  */
);


/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_VAL

DESCRIPTION
  This function formats the value of an extended format string
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <string>

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_string_val
(
  const dsati_cmd_type *tab_entry, /*  entry in parse table  */
  byte *rb_ptr,                    /*  point to buffer for result  */
  fmt_enum_type fmt_sw             /*  selects response or set fmt */
);

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_CHSET

DESCRIPTION
  This function performs conversion of a given null terminated string in one 
  supported character set to another supported character set, writing it into
  an output buffer provided and null terminating it. It will handle 
  inconvertible characters as indicated by drop_inconvertible, simply not 
  copying them if TRUE or stopping conversion and returning NULL if FALSE. 
  The character with value 0x00 is not supported in the GSM 7 bit or IRA 
  alphabet as NULL is used for terminating strings, neither will the function 
  output this character value except as a termination. When converting from 
  HEX/UCS2 for instance this counts as an inconvertible character.
  It will stop the conversion if indicated maximum output buffer length is 
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  possible values:
    NULL : if drop_inconvertible FALSE and conversion fails
    pointer to terminating NULL in output buffer : if conversion successful

SIDE EFFECTS
  None
  
===========================================================================*/
extern char *dsatutil_convert_chset
(
  const char *in_str, 
  dsati_chset_type in_chset,
  const char *out_str,
  dsati_chset_type out_chset,
  uint16 out_max,
  boolean drop_inconvertible
);

/*===========================================================================

FUNCTION dsatutil_ucs2_strlen

DESCRIPTION
  This function counts the number of UCS2 characters in a string.
  The  string must be terminated by 0x0000.

DEPENDENCIES
  None

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 dsatutil_ucs2_strlen
(
  const char* str                 /*  Unicode character string  */
);

/*===========================================================================

FUNCTION DSATUTIL_FMT_MIXED_PARAM_VAL

DESCRIPTION
  This function formats the values of a mixed parameter.
  The values may be formatted as the result of a READ "?" command,
  or in the form of a SET command for use in uploading the configuration.
  For the READ command, the returned string is of the form:

      +NAME: <p1>,<p2>,...

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
extern unsigned int dsatutil_fmt_mixed_param_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  dsm_item_type *res_buff_ptr,            /*  Place to put response       */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
);


/*===========================================================================

FUNCTION DSATUTIL_PUT_NUM_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for
  a command, and, if they are correct, stores the value(s) in the
  parameter.  If the range or number of values is out of bounds, the
  global "dsat_result" will be set to ERROR.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS
  Describe here any side effects that may occur as a result of calling this
  function. This includes such things as global vars that are modified, etc.

===========================================================================*/
extern boolean dsatutil_put_num_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr     /*  Command tokens from parser  */
);

/*===========================================================================

FUNCTION DSATUTIL_MIXED_PARAM_SET_NUM_ARG

DESCRIPTION
  This function checks the value of a string argument for
  a command in the def list.  It determines the string argument's index 
  into the string list.  Use this index to set the val_ptr.

DEPENDENCIES


RETURN VALUE
  returns a param_srch_enum_type:
    PARAM_MISS - Not found in values list 
    PARAM_HIT  - Found in values list            
    PARAM_NULL - No parameter specified

SIDE EFFECTS
  None

===========================================================================*/
extern param_srch_enum_type dsatutil_mixed_param_set_num_arg
(
  dsat_num_item_type * val_num_ptr, /* the index number to be returned */
  const def_list_type * def_list_ptr, /* the def list */
  const byte * arg,                   /* the token argument */
  uint32 attrib                       /* the attrib of the MIXED_PARAM */
);


/*===========================================================================

FUNCTION DSATUTIL_PUT_STRING_ARG

DESCRIPTION
  This checks an extended format string WRITE command for correct
  syntax and string length.  If everthing is OK, the string is copied
  into the specified parameter.  Otherwise, the global dsat_result" is
  set to ERROR. Non space characters outside of the '"' are considered
  an error.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS


===========================================================================*/
extern boolean dsatutil_put_string_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  put_string_enum_type action           /*  Store or concatenate  */
);


/*===========================================================================

FUNCTION DSATUTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFF and a radix of 2,
  which returns 17 bytes (16 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
extern byte * dsatutil_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
);


/*===========================================================================

FUNCTION DSATUTIL_XITOA

DESCRIPTION
  Formats an unsigned long to hexadecimal.
  Supplies leading zeros.

DEPENDENCIES

RETURN VALUE
  A pointer to the null terminator is returned.


SIDE EFFECTS

===========================================================================*/
unsigned char * dsatutil_xitoa
(
  unsigned long val,  /*  value to convert  */
  byte * a_ptr        /*  pointer to null terminator of result  */
);


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case
  and spaces.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case and spaces,
            otherwise, it returns the difference between the first
            characters to disagree. The difference is first arg minus
            second arg.  Lower case letters are converted to upper
            case before the difference is taken.

SIDE EFFECTS
  None.

===========================================================================*/
extern int dsatutil_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
);

/*===========================================================================
FUNCTION DSATUTIL_STRIP_QUOTES_OUT

DESCRIPTION
  This function strips the quotes from the quoted_str, the results
  are in out_str.  str_len is the length of the out_str string.

  The quoted_str is a null terminated string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the action succeeds.
  FALSE if the action fails.
        The reason could be 
        1. the str_len is less than the length of
           quoted_str -2, ie the out_str is too small to hold the result.
        2. the quoted_str is not quoted

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dsatutil_strip_quotes_out
( 
  const byte * quoted_str, byte * out_str, word str_len
);

/*===========================================================================

FUNCTION DSATUTIL_FORMAT_IPADDR

DESCRIPTION
  This function checks for the validity of the IP address and converts it
  to and from internal storage format.
  It verifies the length of the IP address and also verifies if all
  the 4 tuples, when converted to int, are below and upper limit.
  The limit is 254 or 255, depending on passed flag.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_result_enum_type dsatutil_format_ipaddr 
(
   ip_convert_enum_type mode,                /* Mode of conversion      */
   char* ipstr,                              /* String representation   */
   uint32* ipnum,                            /* Interger representation */
   boolean allow_255                         /* Validation flag         */
);


/*===========================================================================

FUNCTION DSATUTIL_RESET_PASSWORD

DESCRIPTION
  Clear the password variable or specified length.
  The password string is set to 0xFF, null terminated.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void dsatutil_reset_password
(
  byte * password,            /* String containing password */
  word length                 /* Maximum length of string */
);


/*===========================================================================

FUNCTION DSATUTIL_APPEND_DSM_ITEM

DESCRIPTION
  This function checks the buffer length against the passed threshold
  value.  If the threshold is exceeded, a new DSM item is requested and
  appended to the buffer chain.  An error is generated if no dsm_items
  are available. A pointer to the new DSM item is returned as a parameter;
  it equals the input DSM item if no action taken.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to current (appended) DSM item

SIDE EFFECTS
  A new dsm_item may be allocated.
  
===========================================================================*/
extern dsm_item_type * dsatutil_append_dsm_item
(
  dsm_item_type * dsm_item_ptr,          /* Pointer to head DSM item */
  word threshold                         /* Threshold length of buffer */
);


#ifdef FEATURE_DSAT_FLOW_CONTROL
/*===========================================================================

FUNCTION  DSATI_TO_SIO_LOW_CMD_HANDLER

DESCRIPTION
  This is the handler function for DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: if the dsat_to_sio low hanlder succeeds.
  DSAT_ERROR    : if the dsat_to_sio low hanlder fails.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsati_to_sio_low_cmd_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);
#endif /* FEATURE_DSAT_FLOW_CONTROL */


/*===========================================================================

FUNCTION DSATUTIL_VALIDATE_DIAL_CHAR

DESCRIPTION
  This function examines the passed dial string charater and returns a
  validation result.  It is up to the caller to act upon the result as
  approppriate within its context.

  Refer to 3GPP TS 27.007 section 6.2 for details on the valid dialing 
  digits.  The supported set are summarized as follows:
    0 - 9 
    A - C
    + * ; #           
    D - E             (ETSI SMS PDU mode)
    W , $ T P ! @ D   (accepted but flagged for strip)
  
DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating validation result.

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatutil_validate_dial_char
(
  byte * c_ptr                 /*  Input character   */
);


/*===========================================================================
FUNCTION DSATUTIL_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  Index set to NULL if 
  parameter is null, as with empty commas entered, 

  If ignore_quote is true, then srch_list will find quoted args in 
  unquoted lists.  For instance, \"ME\" will find itself as the first 
  item in list ME, MT, SM

DEPENDENCIES
  None

RETURN VALUE
  PARAM_HIT if found.
  PARAM_MISS if no match.
  PARAM_NULL if no parameter specified.

SIDE EFFECTS
  None
===========================================================================*/
param_srch_enum_type dsatutil_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const byte l[][8],
  const byte * arg,
  action_quote_e_type action_quote
);

/*===========================================================================

FUNCTION DSATUTIL_STRCMP_PKT_DIAL_STR

DESCRIPTION
  This function scans the input null terminated string for packet data
  control string prefix and suffix.  
  For ETSI mode Prefix is "*98" and suffix is "#".

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the input string has correct prefix and suffix,  FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsatutil_strcmp_pkt_dial_str
(
  byte               *orig_dial_str,   /*  Pointer to the dial string */
  dsati_mode_e_type  mode              /*  Mode of Operation          */
);

#endif /* FEATURE_DATA_STRIP_ATCOP */
#endif /* FEATURE_DATA */

#ifndef FEATURE_DATA_STRIP_ATCOP
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

/*===========================================================================

FUNCTION DSAT_FINISHED_ATH_CMD

DESCRIPTION
  Resets the state variable. Continues processing the AT command line.
  Called by dsat_send_result_code(). Used when we are processing ATH cmd.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Does not resutn to CS data handler tillwe finifh processing the AT command
  line.

===========================================================================*/
void dsat_finished_ath_cmd 
(
 dsat_result_enum_type result_code         /* Command result code */
);

#endif /* FEATURE_DATA_STRIP_ATCOP */

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

/*===========================================================================

FUNCTION DSATUTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns 0 for a correct conversion,
          1 if no argument was found,
          2 if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/
extern atoi_enum_type dsatutil_atoi
(
  dsat_num_item_type *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
);

#endif /* DSATI_H */

