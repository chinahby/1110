/*****************************************************************************

  C A M E R A   T U N E

  (runtime tuning, that is)

  (1)  reads and writes configurations from and to EFS,

  (2)  reads and writes configurations in binary for
       embedding in EXIF headers (and possibly elsewhere),

  (3)  holds the keypress shortcut interpreter,

  (4)  holds the keypresses -> numbers interpreter, 

and provides a place for whatever else doesn't belong in the BREW apps.

Copyright (c) Qualcomm 2006, all rights reserved.

$Header: //source/qcom/qct/multimedia/camera/qmobicatcommon/main/latest/src/camera_tune.c#8 $
$DateTime: 2009/02/20 11:11:13 $ 
$Author: jgarber $

REVISION HISTORY:
  
  when        who       what, where, why
  ----------  --------  ------------------------------------------------------
2009-02-20  jgarber     For Winmobile, ensure preferential use of sockets.
2009-02-05  jgarber     For Winmobile, use sockets for communication with PC.    
2008-11-06  jgarber     For Winmobile, don't open and close control file when
                        not communicating with PC. It triggers a MS bug related
                        to closed file handles. Instead create a flag file
                        that the PC tool will look for.
  2008-10-09  jgarber   Fixed line endings.
  2008-03-12  jgarber   Reduced memory requirement for JPEG header data by not 
                        counting excluded items.
  2008-01-21  jgarber   Expanded storage for new, longer names.
  2007-10-18  jgarber   Added code for WinMobile.
  2007-06-25  jgarber   Added I/O exclusions.
  2007-06-14  jgarber   Made corrections to previous checkin.
  2007-06-12  jgarber   Fixed compiler error if FEATURE_CAMERA_MOBICAT_CONFIG_H
                        not defined.
  2007-05-29  jgarber   Added functions for new data exchange protocol (for PC)
  2007-05-03  c_rmutti  Changes needed for FTM_Camera task
  2007-02-14  jgarber   Used AEEstd functions if available, removed some 
                        suppressed lint warnings, added ability to use
                        shortcuts with numeric names (e.g. 3x3...).
  2007-01-24  jgarber   Added some checks to strlcat to prevent buffer 
                        overflows.
  2006-12-06  jgarber   Change JPEG header payload elements from static
                        array of 256 items to dynamically allocated list. 
  2006-11-01  jgarber   Fixed default float increment to be 0.01
                        instead of 0.1
  2006-10-30  jgarber   Made shortcut entry case insensitive
  2006-10-25  jgarber   Shortened potential strings in registry
  2006-10-18  jgarber   Added CAMERA_TUNE_KEY_UP as a way to enter negative
                        values, and fixed CAMERA_TUNE_KEY_CLR deletion
                        behavior when reading digits
  2006-10-06  jgarber   Initial rewrite of QMobiCaT engine to use
                        camera services tunable parameters API.


DESCRIPTION:
  
  See camera_tune.h for a clear explanation, and to have only
  one copy of the explanation between the two associated files.

*****************************************************************************/



//////////////////////////////////////////////////////////////////////////////
//    INCLUSIONS and MACROS
//////////////////////////////////////////////////////////////////////////////

#include "customer.h"

#ifdef FEATURE_WINCE
#error code not present
#else //FEATURE_WINCE
  #include "custcamera.h"
#endif //FEATURE_WINCE

#ifdef     FEATURE_CAMERA_MOBICAT
  #ifndef FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
    #define  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
  #endif
#endif

// For now, we discontinue support of FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H by itself
#ifdef     FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
  #ifndef FEATURE_CAMERA_MOBICAT
    #define  FEATURE_CAMERA_MOBICAT
  #endif
#endif

#include "camera_parms_api.h"
#include "camera_init_config.h"

//////////////////////////////////////////////////////////////////////////////
//    Featurization Normalization from the 6550r4400
//    feature names.
//
//    The custcamera.h abstraction
//    doesn't represent the structure of the code,
//    so I rewrite the definition symbols to those
//    local to this compilation unit (.c file) which do
//    represent the structure of the code.
//
//    If FEATURE_CAMERA_MOBICAT is defined,
//    generate code for 
//      JPEG headers,
//      FFA UI,
//      Diag UI,
//      EFS IO,
//      compilation of camera_config.h,
//      initialization with default/cache variables.
//
//    If FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H is defined,
//    generate code for only the last two above.
//
//////////////////////////////////////////////////////////////////////////////


/////////////////////
//  Declarations.. //
/////////////////////


#ifndef FEATURE_WINCE
#include "fs_public.h"
#include "camera_tune_ui_for_brew.h"
#endif //!FEATURE_WINCE

#include "camera_3a_shared.h"
#include "camsensor.h"
#include "jpege.h"
#include "camera_tune.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef CAMERA_USES_LIBSTD
  #include "AEEstd.h"
#endif


////////////////////
//  CONSTANTS...  //
////////////////////


#define  CAMERA_TUNE_EOF                   ('\0')  /*  Works for EFS. */
#define  CAMERA_TUNE_LONGEST_VALUE_STRING  (CAMERA_TUNE_LONGEST_STRING)  // parameter value
#define  CAMERA_TUNE_LONGEST_EXPRESSION_STRING          ((CAMERA_TUNE_LONGEST_STRING) + (CAMERA_TUNE_LONGEST_VALUE_STRING) + 3) //parm=value\r\n
#define  CAMERA_TUNE_MOST_CONFIGS                          (100)
//#define  CAMERA_TUNE_REGISTRY_CONFIG_STRING_BUFFER_SIZE  (16384)
#define  CAMERA_TUNE_LONGEST_STRING                        (60) // nickname or file name

#define CAMERA_TUNE_PARM_PAST_READ_INI                   (0) //read ini entry is parm at parm count
#define CAMERA_TUNE_PARM_PAST_WRITE_INI                  (1) //write ini entry is 1 past parm count
#define CAMERA_TUNE_PARM_PAST_APPLY                      (2) //apply entry is 2 past parm count
#define CAMERA_TUNE_PARM_PAST_LAST                       CAMERA_TUNE_PARM_PAST_APPLY  //Change this if more things added to end

#define NUM_CAMERA_CONFIG_INIT_VALUES                    ((sizeof(camera_config_init_values)) / (sizeof(camera_config_init_values_type)))

#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
#define NUM_SUMMARY_PARMS                                ((sizeof(summary_parms)) / (sizeof(camera_tune_registry_type)))
#endif //FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

#define DEFAULT_INCREMENT_INT                            (1)
#define DEFAULT_INCREMENT_FLOAT                          (0.01)

#ifdef  FEATURE_CAMERA_MOBICAT

#ifdef  FEATURE_WINCE
#error code not present
#else // FEATURE_WINCE
  #define  CAMERA_TUNE_WHERE_ARE_CONFIGS  "/"  
#endif  // FEATURE_WINCE

#ifdef FEATURE_WINCE
#error code not present
#endif //FEATURE_WINCE

#endif//FEATURE_CAMERA_MOBICAT

#define CAMERA_TUNE_DIAG_VERSION_ID "2.1.0"

typedef  enum
{
  APP_STATE_INIT ,
  APP_STATE_RUNNING ,
  APP_STATE_NOT_RUNNING
}APP_STATE ;

typedef  enum
{
  VALUE_CHANGE_NONE,
  VALUE_CHANGE_INCREMENT,
  VALUE_CHANGE_DECREMENT
} value_change_type;


/////////////////////////////////////////////
//  Camera Tune Registry Entry Structure   //
//                                         //
//  Holds a list of parameters (and some   //
//  associated information) whose          //
//  implementations and properties are     //
//  encapsulated by the camera servcies    //
//  tunable parameter API.                 //
//                                         //
/////////////////////////////////////////////

#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
typedef struct
{
  camera_api_parms_type parm;
  char name[CAMERA_TUNE_LONGEST_STRING];
  int table_elements;  // Number of table elements
  uint32 flags;
} camera_tune_registry_type;
#endif //FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H


#include "camera_tune_msm.c"  //platform-specific stuff

//////////////////////////////////////////////////////////////////////////////
//  LOCAL VARIABLES...(local to this C compilation unit)
//  NOTE:  WRAP THESE INSTANCE VARIABLES IN A STATE STRUCT FOR
//         MULTI-CAMERA-SENSOR PHONES...eventually, post 6550 4.4 release.
//////////////////////////////////////////////////////////////////////////////

#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
// Concentrate these lint warnings here rather than on each use.
//lint -e{641} converting enum to int
static const int camera_tune_error_INVALID_STATE = (int)CAMERA_TUNE_ERROR_INVALID_STATE;
//lint -e{641} converting enum to int
static const int camera_tune_error_INVALID_PARM  = (int)CAMERA_TUNE_ERROR_INVALID_PARM;
//lint -e{641} converting enum to int
static const int camera_tune_error_NOT_SUPPORTED = (int)CAMERA_TUNE_ERROR_NOT_SUPPORTED;
//lint -e{641} converting enum to int
static const int camera_tune_error_FAILED        = (int)CAMERA_TUNE_ERROR_FAILED;
static const int camera_parms_api_READ_ONLY      = (int)CAMERA_PARMS_API_READ_ONLY;
static const int camera_parms_api_REQUIRES_APPLY = (int)CAMERA_PARMS_API_REQUIRES_APPLY;

static camera_tune_registry_type camera_tune_registry[(int)CAMERA_API_LAST + CAMERA_TUNE_PARM_PAST_LAST + 1];  // Room for 3A summary + things at end (read/write ini, etc)
static int num_parms = 0;  //number of parameters in the registry; a table counts as 1 parm.
static int num_items = 0;  //number of items, including non-parameters (e.g., read/write ini files)
static int num_JPEGvalues;     //This is total name/value pairs to go in the JPEG header, counting each table item.
static APP_STATE                    camera_tune_application_state = APP_STATE_INIT;
#endif//FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

#ifdef  FEATURE_CAMERA_MOBICAT

#ifdef FEATURE_WINCE
#error code not present
#else //FEATURE_WINCE
static int               camera_tune_sourcefile  = (-1) ; //  open()'s "file descriptor"
       char              camera_tune_is_visible           = 0;
static int               camera_tune_is_reading_digits    = 0;
static char              camera_tune_ascii                [ CAMERA_TUNE_LONGEST_EXPRESSION_STRING ];
static char              camera_tune_entry_string         [ CAMERA_TUNE_LONGEST_STRING ];
static int               camera_tune_entry_delimiter      = (int)(CAMERA_TUNE_KEY_STAR);
static int               camera_tune_entry_head_of_string = 0;
static unsigned int      camera_tune_config_file_index    = 0;
#endif //FEATURE_WINCE

static int               camera_tune_object               = 0;
static uint8             camera_tune_annotate_jpegs_enable = 1;
static char              camera_tune_symbol       [ CAMERA_TUNE_LONGEST_STRING       ];
static char              camera_tune_value_string [ CAMERA_TUNE_LONGEST_VALUE_STRING ];
static int               line_number = 1;
static int               camera_tune_table_index = 0;
static boolean           is_read_ini_user_initiated = FALSE;  //TRUE if user initiates read from ini, FALSE if from diag
static boolean           unapplied_items = FALSE;  //TRUE if items have been written that need to be applied
static char              camera_tune_ini_file_path [ CAMERA_TUNE_LONGEST_STRING ];
static char            * camera_tune_config_choices[CAMERA_TUNE_MOST_CONFIGS];
static unsigned int      camera_tune_num_of_configs       = 0;


//////////////////////////////////////////////////////////////////////////////
//  Function prototypes
//////////////////////////////////////////////////////////////////////////////


#ifndef FEATURE_WINCE
void  camera_tune_read_configuration ( void ) ;
static int   camera_tune_propose_object_by_nickname ( void );
static void  camera_tune_seek_object_by_nickname    ( void );
static void  camera_tune_write_to_ui                ( void );
#endif //!FEATURE_WINCE

static int camera_tune_get_to_string(camera_api_parms_type parm, 
                                     int table_index, 
                                     char * buffer, 
                                     unsigned int buffer_length,
                                     boolean is_for_lcd,
                                     int summary_item,
                                     boolean value_only);

static int camera_tune_get_summary_to_string(char *buffer, unsigned int buffer_length, boolean is_for_lcd);

static int camera_tune_get_to_jpege(camera_api_parms_type parm, 
                                    int table_index, 
                                    char * buffer, 
                                    unsigned int buffer_length,
                                    JPEGENC_mobiCat_type * elem);

static int camera_tune_set_value(camera_api_parms_type parm, 
                                 int table_index, 
                                 const char * buffer,
                                 value_change_type value_change,
                                 camera_tune_phase_type phase); 

static int camera_tune_apply_changes(void);
static int camera_tune_assign_registered_variable_value( void );


//////////////////////////////////////////////////////////////////////////////
//    CODE
//////////////////////////////////////////////////////////////////////////////


#ifndef CAMERA_USES_LIBSTD
static void  strlcat ( char       * dest , 
                const char * suffix ,
                unsigned int length_of_dest
              )
{
  if ((length_of_dest > 0) && (strlen(suffix) < length_of_dest))
  {
    unsigned int size = strlen( dest );
    (void)strncat( dest , suffix , (length_of_dest - (1 + size)) );
  }
}
#endif  //CAMERA_USES_LIBSTD

// strncpy that guarantees null termination
static char* safe_strncpy (char *d, const char *s, size_t n)
{
  size_t count = n;
  char *dest = d;
  const char *src = s;
  while (((*dest++ = *src++) != 0) && --count)
  {
  }
  if (count == 0)
  {
    *dest = 0;
  }

  return d;
}

#ifndef FEATURE_WINCE
///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_show_digits_or_propose_name 
//
//  Description:
//    Shows proposal or current entry based on delimeter.
//
//  Parameters:
//    None.
//
//  Return value:
//    String for proposed entry, or current entry
//
///////////////////////////////////////////////////////////////////////
char *  camera_tune_show_digits_or_propose_name ()
{
  if (camera_tune_entry_delimiter == (int)CAMERA_TUNE_KEY_STAR)
  {
    int proposal = camera_tune_propose_object_by_nickname();

#ifndef CAMERA_USES_LIBSTD
    (void) sprintf( camera_tune_ascii , "?: %s" ,
                    camera_tune_registry[proposal].name
                  );
#else
    (void) std_strlprintf( camera_tune_ascii , CAMERA_TUNE_LONGEST_EXPRESSION_STRING,
                           "?: %s" , camera_tune_registry[proposal].name
                         );
#endif //CAMERA_USES_LIBSTD

    return &(camera_tune_ascii[0]);
  }
  else
  {

#ifndef CAMERA_USES_LIBSTD
    (void)sprintf( camera_tune_ascii , "?: %s" , camera_tune_entry_string);
#else
    (void)std_strlprintf( camera_tune_ascii , CAMERA_TUNE_LONGEST_EXPRESSION_STRING, "?: %s" , camera_tune_entry_string);
#endif //CAMERA_USES_LIBSTD

    return &(camera_tune_entry_string[0]);
  }
}


//////////////////////////////////////////////////////////////////////////////
//  NOTE  :  use lookup tables rather than long if-else sequences?
//////////////////////////////////////////////////////////////////////////////


void  camera_tune_read_a_digit( int key )
{
  if (camera_tune_entry_head_of_string < CAMERA_TUNE_LONGEST_STRING - 1)
  {
    unsigned char digit = 0;
    if      (key == (int)CAMERA_TUNE_KEY_0)   digit = '0';
    else if (key == (int)CAMERA_TUNE_KEY_1)   digit = '1';
    else if (key == (int)CAMERA_TUNE_KEY_2)   digit = '2';
    else if (key == (int)CAMERA_TUNE_KEY_3)   digit = '3';
    else if (key == (int)CAMERA_TUNE_KEY_4)   digit = '4';
    else if (key == (int)CAMERA_TUNE_KEY_5)   digit = '5';
    else if (key == (int)CAMERA_TUNE_KEY_6)   digit = '6';
    else if (key == (int)CAMERA_TUNE_KEY_7)   digit = '7';
    else if (key == (int)CAMERA_TUNE_KEY_8)   digit = '8';
    else if (key == (int)CAMERA_TUNE_KEY_9)   digit = '9';
    else if (key == (int)CAMERA_TUNE_KEY_DOWN) digit = '.';
    else if (key == (int)CAMERA_TUNE_KEY_PTT ) digit = '.';
    else if (key == (int)CAMERA_TUNE_KEY_UP) digit = '-';

    if (digit)
    {
      camera_tune_entry_string[ camera_tune_entry_head_of_string++ ] = digit ;
      camera_tune_entry_string[ camera_tune_entry_head_of_string   ] = '\0'  ;
    }
  }

  if (key == (int)CAMERA_TUNE_KEY_CLR) 
  {
    if (camera_tune_entry_head_of_string > 0)
    {
      camera_tune_entry_string[ --camera_tune_entry_head_of_string ] = '\0';
    }
  }
}


// String Entry is reached by pressing either "*" or "#".
// Thereafter, subsequent numbers are accumulated as a digit string into
// camera_tune_entry_string[ CAMERA_TUNE_LONGEST_STRING ] buffer,
// and the sequence is ended by a (second) press of either "*" or "#".
//
// If the string entry was started by "#", then the digit string
// is converted to an integer, cf. Unix "atoi", 
// and that integer is passed to the setter function for the
// current screen, if such function exists.
//
// (Such functions may exist for screens tracking
// writable variables, for example.)
//
// If the string entry was started by "*", then the digit string
// is compared with digit sequences arising from the nicknames of
// the registered screens.  The index of the first of any such match
// is used as the new value of "camera_tune_object".
//
// (I.e., this is the SMS-style keyboard shortcut among screens.)
//
// If no observable variable's nickname corresponds to the digit
// sequence entered, then camera_tune_object is unchanged.


//////////////////////////////////////////////////////////////////////////////


static  char  numeral_from_letter (  int letter  )
{
  switch( letter )
  {
     case '1' :  return '1';

     case 'A' :
     case 'a' :
     case 'B' :
     case 'b' :
     case 'C' :
     case 'c' :
     case '2' :  return '2' ;

     case 'D' :
     case 'd' :
     case 'E' :
     case 'e' :
     case 'F' :
     case 'f' :
     case '3' :  return '3' ;

     case 'G' :
     case 'g' :
     case 'H' :
     case 'h' :
     case 'I' :
     case 'i' :
     case '4' :  return '4' ;

     case 'J' :
     case 'j' :
     case 'K' :
     case 'k' :
     case 'L' :
     case 'l' :
     case '5' :  return '5' ;

     case 'M' :
     case 'm' :
     case 'N' :
     case 'n' :
     case 'O' :
     case 'o' :
     case '6' :  return '6' ;

     case 'P' :
     case 'p' :
     case 'Q' :
     case 'q' :
     case 'R' :
     case 'r' :
     case 'S' :
     case 's' :
     case '7' :  return '7' ;

     case 'T' :
     case 't' :
     case 'U' :
     case 'u' :
     case 'V' :
     case 'v' :
     case '8' :  return '8' ;

     case 'W' :
     case 'w' :
     case 'X' :
     case 'x' :
     case 'Y' :
     case 'y' :
     case 'Z' :
     case 'z' :
     case '9' :  return '9' ;

     case '0' :  return '0' ;

     default:  return '?';
  }
}


/*****************************************************************************
* 
*   NOT lexicographic  :  1 means initial sequences correspond,
*                         0 means they don't.
* 
*****************************************************************************/


static  unsigned char  compare_string_and_numerals (  
                                                     const char * string
                                                   , const char * numerals
                                                   )
{
  unsigned int Lc         = strlen( string   ),
               Ln         = strlen( numerals ),
               Ic         = 0,
               In         = 0,
               correspond = 1;
  while (
             correspond
          && (Ic < Lc)
          && (In < Ln)
        )
  {
    if (    ( string[ Ic ] == ' ' ) 
         || ( string[ Ic ] == '_' )
       )
    {
      ++Ic ;
      continue;
    }
    correspond = correspond &&
                 ( numeral_from_letter( string[ Ic ] ) == numerals[ In ] ) ;
    ++Ic ;
    ++In ;
  }
  return  ((unsigned char)correspond);
}
#endif //!FEATURE_WINCE

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_index_of_object_with_nickname
//
//  Description:
//    Searches the name list for an item matching the name passed in.
//
//  Parameters:
//    n: name to search for
//
//  Return value:
//    Index of item found, or -1 if no item found.
//
///////////////////////////////////////////////////////////////////////
int  camera_tune_index_of_object_with_nickname ( const char * n )
{
  int i ;

  for ( i = 0 ; i < num_items ; ++i)
  {
    if ( ! strcmp ( camera_tune_registry[i].name , n ) )
    {
      return i;
    }
  }
  return (-1);
}


#ifndef FEATURE_WINCE
///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_seek_object_by_nickname 
//
//  Description:
//    Sets current object based on current entry string.
//
//  Parameters:
//    None.
//
//  Return value:
//    None, but sets global camera_tune_object.
//
///////////////////////////////////////////////////////////////////////
static void  camera_tune_seek_object_by_nickname ( void )
{
  camera_tune_object = camera_tune_propose_object_by_nickname();
}


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_propose_object_by_nickname
//
//  Description:
//    Searches for match of current user entry string.
//
//  Parameters:
//    None, but uses global camera_tune_entry_string.
//
//  Return value:
//    Index of match, or current object if no match.
//
///////////////////////////////////////////////////////////////////////
static int  camera_tune_propose_object_by_nickname ( void )
{
  int  i ;
  char found = 0;
  int  proposal = camera_tune_object ;

  for ( i = 0 ;
        (!found) && (i < num_items) ; 
        ++i
      )
  {
    unsigned char  comparison_result =
                   compare_string_and_numerals
                     (
                       camera_tune_registry[i].name
                     , camera_tune_entry_string
                     );

    if ( comparison_result && !(camera_tune_registry[ i ].flags & (int)CAMERA_PARMS_API_EXCLUDE_TARGET_UI)) 
    {
      found    = 1;
      proposal = i;
    }
  }
  return proposal;
}
#endif //!FEATURE_WINCE


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_apply_changes
//
//  Description:
//    Some changes to parameters are applied immediately, but some are delayed until the
//    user indicates it is a good time to apply a group of changes, or changes that may
//    require time-consuming steps.  This function applies those delayed changes.
//
//  Parameters:
//    None, but resets global unapplied_items.
//
//  Return value:
//    0 if successful.
//
///////////////////////////////////////////////////////////////////////
static int camera_tune_apply_changes()
{
  int ret;

  ret = (int)camera_parms_apply();
  if (ret == 0)
  {
    unapplied_items = FALSE;
  }

  return ret;
}

//////////////////////////////////////////////////////////////////////////////

#ifdef FEATURE_WINCE
#error code not present
#else //FEATURE_WINCE
int  camera_tune_read_dot_ini_file ( const char * filename )
{
  //  Does filename exist?  Can it be opened?

  MSG_HIGH( "JOHN : camera_svcs_is_running = %d" , camera_svcs_is_running(camera_tune_application_state) , 0 , 0 );

  if (! camera_svcs_is_running(camera_tune_application_state))
  {
    return camera_tune_error_INVALID_STATE;  //  ignoring the request.
  }

  camera_tune_sourcefile = efs_open( filename , O_RDONLY );
  if (camera_tune_sourcefile == (-1))
  {  
    //  The file isn't readable...no error, just nothing read.
    //  That means reading was degenerate, and is done.

    return camera_tune_error_FAILED;
  }
  else
  {
    //printf( "\"%s\" opened for read...\n" , filename );
  }

  camera_tune_read_configuration ();

  (void)efs_close(camera_tune_sourcefile);

  if (is_read_ini_user_initiated)
  {
    (void)camera_tune_apply_changes();
    is_read_ini_user_initiated = FALSE;
  }

  return 0;
}
#endif //FEATURE_WINCE

//////////////////////////////////////////////////////////////////////////////


//  void  camera_tune_read_config_from_string ( char * string )  //  assumes INI
//  {
//    Reserved for future Diag use.
//  }


void freeconfigs(void)
{
  int i;
  
  camera_tune_num_of_configs = 0;
  for(i=0 ; i < CAMERA_TUNE_MOST_CONFIGS ; ++i)
  {
    if (camera_tune_config_choices[i] != NULL)
    {
         free( camera_tune_config_choices[i] );  // A string was held there.
         camera_tune_config_choices[i] = NULL;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
//
// camera_tune_list_desired_files_in_directory
// Caller must free the config list by calling freeconfigs()
//
/////////////////////////////////////////////////////////////////////////////
#ifdef FEATURE_WINCE
#error code not present
#else //FEATURE_WINCE
int  camera_tune_list_desired_files_in_directory (
                                                   const char * directory
                                                 , const char * substring
                                                 )
{
  EFSDIR           *  dirp;
  struct fs_dirent *  dp;
//char             *  reason = NULL;

  camera_tune_num_of_configs = 0;
  dirp = efs_opendir( directory );
  if (dirp == NULL)
  {
    //  reason = "opendir failed";
    return 1;
  }

  while ( (dp = efs_readdir(dirp)) != NULL )
  {
    //  Does "substring" occur in the filename?

    if (strstr( dp->d_name , substring ) != NULL)
    {
      if (camera_tune_num_of_configs < CAMERA_TUNE_MOST_CONFIGS)
      {
        //
        //  Is there room to list more config files ?
        //

        if (camera_tune_config_choices[camera_tune_num_of_configs] != NULL)
        {
          //
          // Is there a previous remnant?
          //

          free( camera_tune_config_choices[camera_tune_num_of_configs] );
        }

        camera_tune_config_choices[ camera_tune_num_of_configs ] =
          (char *)(malloc( CAMERA_TUNE_LONGEST_STRING*sizeof(char) ));

        if (camera_tune_config_choices[ camera_tune_num_of_configs] == NULL)
        {
          //  reason = "malloc ran out of room";
          return 1;
        }

        //
        //  Record this file's name from the directory structure.
        //

        (void)
        safe_strncpy(
                 camera_tune_config_choices[ camera_tune_num_of_configs ] ,
                 dp->d_name ,
                 CAMERA_TUNE_LONGEST_STRING
               );
        ++camera_tune_num_of_configs;
      }
      //printf( "added %s\n" , dp->d_name );
    }
  }
  (void)efs_closedir(dirp);

//printf( "Done scanning:  there were %d choices noted.\n" , camera_tune_num_of_configs );
//{
//  unsigned int i = 0;
//  while ( camera_tune_config_choices[i] != NULL )
//  {
//    //printf( "choice[ %2d ] = \"%s\"\n" , i , camera_tune_config_choices[i] );
//    ++i;
//  }
//}

  return 0;
}


extern void  camera_tune_config_file_read ( void );  // forward declaration.


void  config_read_dialog_handle_buttons ( int k )
{
  if (k == (int)CAMERA_TUNE_KEY_SELECT)
  {
    if (camera_tune_num_of_configs == 0)
    {
      //  Initialize the config file reader
      camera_tune_config_file_index = 0;
      (void) camera_tune_list_desired_files_in_directory
             (
               CAMERA_TUNE_WHERE_ARE_CONFIGS ,
               ".ini"
             );
      if (camera_tune_num_of_configs > 0)
      {
        // Display first found ini file

#ifndef CAMERA_USES_LIBSTD
        (void)sprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                      "select to read %s", 
                      camera_tune_config_choices[0]);
#else
        (void)std_strlprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                            CAMERA_TUNE_LONGEST_STRING,
                            "select to read %s", 
                            camera_tune_config_choices[0]);
#endif //CAMERA_USES_LIBSTD

      }
    }
    else
    {
      //  We've selected a file, so read it in.
      is_read_ini_user_initiated = TRUE;
      camera_tune_config_file_read();
      // Cleanup config list
      (void)safe_strncpy(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, "select to read ini file(7)", CAMERA_TUNE_LONGEST_STRING);
      freeconfigs();
    }
  }
  else if (camera_tune_num_of_configs > 0)
  {
    if (k == (int)CAMERA_TUNE_KEY_UP)
    {
      if (camera_tune_config_file_index == 0)
      {
        camera_tune_config_file_index = camera_tune_num_of_configs;
      }
      --camera_tune_config_file_index ;

      // Display previous ini file

#ifndef CAMERA_USES_LIBSTD
      (void)sprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                    "select to read %s", 
                    camera_tune_config_choices[camera_tune_config_file_index]);
#else
      (void)std_strlprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                           CAMERA_TUNE_LONGEST_STRING,
                           "select to read %s", 
                           camera_tune_config_choices[camera_tune_config_file_index]);
#endif //CAMERA_USES_LIBSTD

    }
    else if (k == (int)CAMERA_TUNE_KEY_DOWN)
    {
      ++camera_tune_config_file_index ;
      if (camera_tune_config_file_index == camera_tune_num_of_configs)
      {
        camera_tune_config_file_index = 0;
      }

      // Display next ini file

#ifndef CAMERA_USES_LIBSTD
      (void)sprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                    "select to read %s", 
                    camera_tune_config_choices[camera_tune_config_file_index]);
#else
      (void)std_strlprintf(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, 
                           CAMERA_TUNE_LONGEST_STRING,
                          "select to read %s", 
                          camera_tune_config_choices[camera_tune_config_file_index]);
#endif //CAMERA_USES_LIBSTD

    }
  }
}
#endif //FEATURE_WINCE


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_get_to_string
//
//  Description:
//    Gets info about 1 parameter to a string.
//
//  Parameters:
//    parm: Parameter whose info to read.
//    table_index: Element index if parameter is part of a table, 
//                      -1 if parameter is not part of a table.
//    buffer: Buffer to write string to.
//    buffer_length: Length of buffer.
//    is_for_lcd: TRUE if for UI lcd, false if for .ini file or other use.
//    summary_item: If >= 0 uses info from the summary table.
//                  Negative if not a summary item.
//    value_only: If TRUE only the value is copied into the buffer
//                (not the name and value)
//
//  Return value:
//    0 if no error.
//
///////////////////////////////////////////////////////////////////////
static int camera_tune_get_to_string(camera_api_parms_type parm, 
                                     int table_index, 
                                     char * buffer, 
                                     unsigned int buffer_length,
                                     boolean is_for_lcd,
                                     int summary_item,
                                     boolean value_only)
{
  int ret;
  unsigned int len;
  void* valueptr = NULL;
  camera_tune_parm_format_type format;
  int32 i32val=0, i32min=0, i32max=0;
  uint32 ui32val=0, ui32min=0, ui32max=0;
  double dval = 0;
  boolean is_summary = (summary_item >= 0) && (summary_item < (int)NUM_SUMMARY_PARMS);

  if (buffer == NULL)
  {
    return camera_tune_error_INVALID_PARM;
  }

  ret = (int)camera_parms_read
               (CAMERA_TUNE_PHASE_RUN, 
                parm,
                table_index,
                &valueptr,
                &format);

  if (ret == (int)CAMERA_SUCCESS)  // Parameter was read successfully
  {
    // begin with emtpy string to contatenate to
    if (buffer_length > 0)
    {
      *buffer = 0;
    }

    if (!value_only)
    {
      // read-only?  put '-' before name on screen
      if (((int)format.flags & camera_parms_api_READ_ONLY) && is_for_lcd && !is_summary)
      {
#ifndef CAMERA_USES_LIBSTD
        strlcat(buffer, "-", buffer_length);
#else
        (void)std_strlcat(buffer, "-", (int)buffer_length);
#endif //CAMERA_USES_LIBSTD
      }

      // name
//lint -e{676} suppress "Possibly negative subscript (-1) in operator '['"; is_summary explicitly checks this
#ifndef CAMERA_USES_LIBSTD
      strlcat(buffer, 
             is_summary ? summary_parms[summary_item].name : format.name,
             buffer_length);
#else
      (void)std_strlcat(buffer, 
                        is_summary ? summary_parms[summary_item].name : format.name,
                        (int)buffer_length);
#endif //CAMERA_USES_LIBSTD

      // table index (if any)
      if (table_index != -1)
      {
        len = strlen(buffer);
        (void)snprintf(buffer+len, buffer_length-len, "[%d]", table_index);
      }

      if (is_summary)
      {
        //:
#ifndef CAMERA_USES_LIBSTD
        strlcat(buffer, ":", buffer_length);
#else
        (void)std_strlcat(buffer, ":", (int)buffer_length);
#endif //CAMERA_USES_LIBSTD

      }
      else
      {
        // =
#ifndef CAMERA_USES_LIBSTD
        strlcat(buffer, " = ", buffer_length);
#else
        (void)std_strlcat(buffer, " = ", (int)buffer_length);
#endif //CAMERA_USES_LIBSTD
      }
    }  //!value_only

    // value as number or descriptive string
    if (valueptr == NULL)
    {
#ifndef CAMERA_USES_LIBSTD
      strlcat(buffer, "?", buffer_length);
#else
      (void)std_strlcat(buffer, "?", (int)buffer_length);
#endif //CAMERA_USES_LIBSTD

      ret = camera_tune_error_FAILED;
    }
    else
    {
      //lint -e{123} suppress "Macro 'min'/'max' defined with arguments..."; these are structure fields, not calls to macros
      switch (format.parm_type)
      {
        case JPEGENC_MC_FLOAT:
          dval = *(float*)valueptr;
          break;
        case JPEGENC_MC_DOUBLE:
          dval = *(double*)valueptr;
          break;
        case JPEGENC_MC_INT8:
          i32val = *(int8*)valueptr;
          if (format.min != NULL)
          {
            i32min = *(int8*)format.min;
          }
          if (format.max != NULL)
          {
            i32max = *(int8*)format.max;
          }
          break;
        case JPEGENC_MC_UINT8:
          ui32val = *(uint8*)valueptr;
          if (format.min != NULL)
          {
            ui32min = *(uint8*)format.min;
          }
          if (format.max != NULL)
          {
            ui32max = *(uint8*)format.max;
          }
          break;
        case JPEGENC_MC_INT16:
          i32val = *(int16*)valueptr;
          if (format.min != NULL)
          {
            i32min = *(int16*)format.min;
          }
          if (format.max != NULL)
          {
            i32max = *(int16*)format.max;
          }
          break;
        case JPEGENC_MC_UINT16:
          ui32val = *(uint16*)valueptr;
          if (format.min != NULL)
          {
            ui32min = *(uint16*)format.min;
          }
          if (format.max != NULL)
          {
            ui32max = *(uint16*)format.max;
          }
          break;
        case JPEGENC_MC_INT32:
          i32val = *(int32*)valueptr;
          if (format.min != NULL)
          {
            i32min = *(int32*)format.min;
          }
          if (format.max != NULL)
          {
            i32max = *(int32*)format.max;
          }
          break;
        case JPEGENC_MC_UINT32:
          ui32val = *(uint32*)valueptr;
          if (format.min != NULL)
          {
            ui32min = *(uint32*)format.min;
          }
          if (format.max != NULL)
          {
            ui32max = *(uint32*)format.max;
          }
          break;
      }

      if (
          (format.parm_type == JPEGENC_MC_INT8 ||
           format.parm_type == JPEGENC_MC_INT16 ||
           format.parm_type == JPEGENC_MC_INT32) &&
          is_for_lcd &&
          i32val >= i32min &&
          i32val <= i32max &&
          format.value_names != NULL &&
          !is_summary &&
          !value_only
         )
      {
        // signed value as descriptive string
#ifndef CAMERA_USES_LIBSTD
        strlcat(buffer, format.value_names[i32val], buffer_length);
#else
        (void)std_strlcat(buffer, format.value_names[i32val], (int)buffer_length);
#endif //CAMERA_USES_LIBSTD
      }
      else if (
               (format.parm_type == JPEGENC_MC_UINT8 ||
                format.parm_type == JPEGENC_MC_UINT16 ||
                format.parm_type == JPEGENC_MC_UINT32) &&
               is_for_lcd &&
               ui32val >= ui32min &&
               ui32val <= ui32max &&
               format.value_names != NULL &&
               !is_summary &&
               !value_only
              )
      {
        // unsigned value as descriptive string
#ifndef CAMERA_USES_LIBSTD
        strlcat(buffer, format.value_names[ui32val], buffer_length);
#else
        (void)std_strlcat(buffer, format.value_names[ui32val], (int)buffer_length);
#endif //CAMERA_USES_LIBSTD
      }
      else
      {
        // value as number
        len = strlen(buffer);
        switch (format.parm_type)
        {
          case JPEGENC_MC_FLOAT:
          case JPEGENC_MC_DOUBLE:
            (void)snprintf(buffer+len, buffer_length-len, "%f", dval);
            break;
          case JPEGENC_MC_INT8:
          case JPEGENC_MC_INT16:
          case JPEGENC_MC_INT32:
            (void)snprintf(buffer+len, buffer_length-len, "%ld", i32val);
            break;
          case JPEGENC_MC_UINT8:
          case JPEGENC_MC_UINT16:
          case JPEGENC_MC_UINT32:
            (void)snprintf(buffer+len, buffer_length-len, "%lu", ui32val);
            break;
        }
      }
    }
  }

  return ret;
}

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_get_summary_to_string
//
//  Description:
//    Writes the 3A live summary info to the string passed in
//
//  Parameters:
//    buffer: Buffer to write string to.
//    buffer_length: Length of buffer.
//    is_for_lcd: TRUE if for UI lcd, false if for .ini file or other use.
//
//  Return value:
//    0 if no error.
//
///////////////////////////////////////////////////////////////////////
static int camera_tune_get_summary_to_string(char *buffer, unsigned int buffer_length, boolean is_for_lcd)
{
  int ret = 0, result, summary_item;
  unsigned int size, current_string_length;
  char *current_string;

  current_string = buffer;
  current_string_length = 0;
  size = buffer_length;

  for (summary_item = 0; summary_item < (int)NUM_SUMMARY_PARMS; ++summary_item)
  {
    result = camera_tune_get_to_string(summary_parms[summary_item].parm, 
                                       -1, 
                                       current_string, 
                                       size, 
                                       is_for_lcd,
                                       summary_item,
                                       FALSE);
    if (result != (int)CAMERA_SUCCESS)
    {
      ret = result;
    }

    if (summary_item < (int)NUM_SUMMARY_PARMS - 1)
    {
#ifndef CAMERA_USES_LIBSTD
      strlcat (current_string, ", ", size);
#else
      (void)std_strlcat (current_string, ", ", (int)size);
#endif //CAMERA_USES_LIBSTD
    }
    current_string_length = strlen(current_string);
    current_string += current_string_length;
    size -= current_string_length;
  }
  return ret;
}

#ifndef FEATURE_WINCE
/*
void  config_read_dialog_write_to_string ( char         * buffer ,
                                           unsigned int   buffer_length ,
                                           boolean        is_for_lcd
                                         )
{
  if (is_for_lcd)
  {
    if (camera_tune_num_of_configs == 0)
    {
      (void)snprintf( buffer , buffer_length , "read a .ini ?  (select = yes)" );
    }
    else
    {
      (void)snprintf( buffer , buffer_length, "read  \"%s\" ?  (select = yes)" , 
                      camera_tune_config_choices[ camera_tune_config_file_index ]
                    );
    }
  }
  else
  {
    buffer[0] = '\0';
  }
}
*/

void  camera_tune_config_file_read ( void )
{

#ifndef CAMERA_USES_LIBSTD
  (void)sprintf( camera_tune_ini_file_path ,
                 "%s%s" ,
                 CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                 camera_tune_config_choices[ camera_tune_config_file_index ]
               );
#else
  (void)std_strlprintf( camera_tune_ini_file_path ,
                        CAMERA_TUNE_LONGEST_STRING,
                        "%s%s" ,
                        CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                        camera_tune_config_choices[ camera_tune_config_file_index ]
                      );
#endif //CAMERA_USES_LIBSTD

  MSG_HIGH( "CAMERA EFS CONFIG \"%s\"", camera_tune_ini_file_path , 0 , 0 );

  (void)camera_tune_read_dot_ini_file ( camera_tune_ini_file_path );
}
#endif //!FEATURE_WINCE


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_write_dot_ini_to_filename
//
//  Description:
//    Writes a .ini file named with string passed in.
//
//  Parameters:
//    filename: Name of file to write
//
//  Return value:
//    0 if successful.
//
///////////////////////////////////////////////////////////////////////
#ifdef FEATURE_WINCE
#error code not present
#else //FEATURE_WINCE
int  camera_tune_write_dot_ini_to_filename ( const char  * filename )
{
  long          r;  //  efs_write's return value = # of bytes written
  unsigned int  v;
           int  f;  //  file descriptor (or handle)
  int table_index;

  MSG_HIGH( "JOHN : camera_svcs_is_running = %d" , camera_svcs_is_running(camera_tune_application_state) , 0 , 0 );

  if (! camera_svcs_is_running(camera_tune_application_state))
  {
    return 2;  //  ignoring the request.
  }

  //
  //  Can a file named filename be opened for write?
  //

  f = efs_open( filename , O_CREAT | O_TRUNC | O_WRONLY );
  if (f == (-1))
  {
    return 2;  //  open failed
  }

  ///////////////////

  for ( v = 0; v < (unsigned)num_parms; ++v)
  {
    //
    //  Walk the registry (camera_tune_registry)
    //

    if (!(camera_tune_registry[ v ].flags & (int)CAMERA_PARMS_API_EXCLUDE_INI))
    {

      for (table_index = 0; table_index < camera_tune_registry[v].table_elements; ++table_index)
      {
        char        * current_string;
        long          size;
        static  char  buffer [ CAMERA_TUNE_LONGEST_EXPRESSION_STRING ];

        // 3A live summary parameters at front of registry
        if (v == 0)
        {
          // Make this a comment in the .ini file by prepending ';'
          buffer[0] = ';';
          (void)camera_tune_get_summary_to_string(buffer+1, CAMERA_TUNE_LONGEST_EXPRESSION_STRING-1, FALSE);
        }
        else
        {
          (void)camera_tune_get_to_string
                  (camera_tune_registry[v].parm, 
                    (camera_tune_registry[v].table_elements == 1) ? -1 : table_index,
                    buffer, 
                    CAMERA_TUNE_LONGEST_EXPRESSION_STRING,
                    FALSE,
                    -1,
                    FALSE);
        }

#ifndef CAMERA_USES_LIBSTD
        strlcat( buffer , "\r\n" , CAMERA_TUNE_LONGEST_EXPRESSION_STRING );
#else
        (void)std_strlcat( buffer , "\r\n" , CAMERA_TUNE_LONGEST_EXPRESSION_STRING );
#endif //CAMERA_USES_LIBSTD

        current_string = &(buffer[0]);

        size = (long)strlen( current_string );

        r = efs_write( f , (void *)current_string , (unsigned long)size );

        while (r < size)
        {
          current_string += r;
          size           -= r;
          r = efs_write( f , (void *)current_string , (unsigned long)size );
        }

        if (r != size)  //  write didn't write everything...thus failed.
        {
          (void)efs_close (f);
          return 3;
        }
      }
    }
  }

  ///////////////////

  //
  //  efs_open and efs_write worked , so this is done.
  //  efs_close the file and return "success".
  //

  (void)efs_close(f);
  return 0;
}
#endif //FEATURE_WINCE


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_write_config_to_string
//
//  Description:
//    Writes config info (what goes in .ini file) to a buffer.
//
//  Parameters:
//    None, but writes to global camera_tune_config_string_buffer
//
//  Return value:
//    None.
//
///////////////////////////////////////////////////////////////////////
/*
void  camera_tune_write_config_to_string  ( void )
{
  int  v = 0;
  unsigned int  characters_space_left = CAMERA_TUNE_REGISTRY_CONFIG_STRING_BUFFER_SIZE - 1;
  char * finale_string = "; End of Qualcomm Qcamera Config\r\n";
  static char  camera_tune_config_string_buffer
                 [ CAMERA_TUNE_REGISTRY_CONFIG_STRING_BUFFER_SIZE ];
  int table_index;

  camera_tune_config_string_buffer[ 0 ] = '\0';

  ///////////////////

  for ( v = 0; v < num_parms; ++v)
  {
    //
    //  Walk the registry (camera_tune_registry)
    //

    for (table_index = 0; table_index < camera_tune_registry[v].table_elements; ++table_index)
    {
      static char   buffer[ CAMERA_TUNE_LONGEST_EXPRESSION_STRING ];
      char        * current_string;
      unsigned int  current_string_length;

      //  3A live summary parameters at front of registry
      if (v == 0)
      {
        (void)camera_tune_get_summary_to_string(buffer, CAMERA_TUNE_LONGEST_EXPRESSION_STRING, FALSE);
      }
      else
      {
        (void)camera_tune_get_to_string
                (camera_tune_registry[v].parm, 
                 camera_tune_registry[v].table_elements == 1 ? -1 : table_index,
                 buffer, 
                 CAMERA_TUNE_LONGEST_EXPRESSION_STRING,
                 FALSE,
                 -1,
                 FALSE);
      }

#ifndef CAMERA_USES_LIBSTD
      strlcat( buffer , "\r\n" , CAMERA_TUNE_LONGEST_EXPRESSION_STRING );
#else
      (void)std_strlcat( buffer , "\r\n" , CAMERA_TUNE_LONGEST_EXPRESSION_STRING );
#endif //CAMERA_USES_LIBSTD

      current_string = &(buffer[0]);

      current_string_length = (unsigned int) strlen(current_string);

      if (characters_space_left > current_string_length)
      {
#ifndef CAMERA_USES_LIBSTD
        (void)strcat ( camera_tune_config_string_buffer , current_string );
#else
        (void)std_strlcat ( camera_tune_config_string_buffer , current_string, CAMERA_TUNE_REGISTRY_CONFIG_STRING_BUFFER_SIZE );
#endif //CAMERA_USES_LIBSTD

        characters_space_left -=  current_string_length;
      }
      //  If not, don't write a partial syntactic statement that
      //  won't be readable later.
    }
  }
  if (characters_space_left > (unsigned int)strlen(finale_string))
  {
#ifndef CAMERA_USES_LIBSTD
    (void)strcat ( camera_tune_config_string_buffer , finale_string );
#else
    (void)std_strlcat ( camera_tune_config_string_buffer , finale_string, CAMERA_TUNE_REGISTRY_CONFIG_STRING_BUFFER_SIZE );
#endif //CAMERA_USES_LIBSTD

  }
}
*/

//////////////////////////////////////////////////////////////////////////////


//  WARNING:  No registered system can contribute
//            more than 4 independent variables to the MobiCaT
//            element stream of the JPEG Encoder.
//            Also, this approach is chosen rather than mallocing
//            so as not to worry about memory leaking since this
//            feature is only intended for tuning use.

/*
void  mobicat_deliver_config_to_jpege_as_a_single_string (const char * string)
{
  camera_tune_write_config_to_string();
  mobicat_jpege_payload [0] . elemName = "Config";
  mobicat_jpege_payload [0] . type     = JPEGENC_MC_UINT8;
  mobicat_jpege_payload [0] . height   = 1;
  mobicat_jpege_payload [0] . width    = (uint16)strlen( string );
  mobicat_jpege_payload [0] . data     = (void *)string ;

  (void)
  jpege_mobicat_insert_info(
                             & (mobicat_jpege_payload [ 0 ] ),
                             1 
                           );
}
*/

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_get_to_jpege
//
//  Description:
//    Transfers info about 1 parameter to JPEG encoding element
//
//  Parameters:
//    parm: Parameter whose info to read.
//    table_index: Element index if parameter is part of a table, 
//                      -1 if parameter is not part of a table.
//    buffer: Place to store the name of the parameter until JPEG encoder is called.
//                      If part of a table, element index will be appended.
//    buffer_length: Length of buffer.
//    elem: JPEG encoding element.
//
//  Return value:
//    0 if no error.
//
///////////////////////////////////////////////////////////////////////
static int camera_tune_get_to_jpege(camera_api_parms_type parm, 
                                      int table_index, 
                                      char * buffer, 
                                      unsigned int buffer_length,
                                      JPEGENC_mobiCat_type * elem)
{
  int ret;
  unsigned int len;
  void* valueptr = NULL;
  camera_tune_parm_format_type format;

  ret = (int)camera_parms_read
              (CAMERA_TUNE_PHASE_RUN, 
               parm,
               table_index,
               &valueptr,
               &format);

   if (ret == (int)CAMERA_SUCCESS)  // Parameter was read successfully
   {
     if (valueptr == NULL)
     {
       ret = camera_tune_error_FAILED;
     }
     else
     {
       //Copy base name to buffer
       (void)safe_strncpy(buffer, format.name, buffer_length);

       //If table, copy index
       if (table_index != -1)
       {
         len = strlen(buffer);
         (void)snprintf(buffer+len, buffer_length-len, "[%d]", table_index);
       }
  
       elem -> elemName = buffer;
  
       elem -> type     = format.parm_type;
       elem -> height   = 1;
       elem -> width    = 1;
       elem -> data     = valueptr;
     }
   }
   return ret;
}


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    mobicat_deliver_config_to_jpege_as_distinct_assignments
//
//  Description:
//    Traverses the name list to create a JPEG encoding element for each parameter
//
//  Parameters:
//    None.
//
//  Return value:
//    None.
//
///////////////////////////////////////////////////////////////////////
void  mobicat_deliver_config_to_jpege_as_distinct_assignments  ( void )
{
  int  i ,
       number_of_elements = 0,
       summary_item,
       ret,
       table_index;
  unsigned int name_size = CAMERA_TUNE_LONGEST_STRING + 12; // Enough room to add "[nnnnnnnnnn]"
  char** names; // place to stash the names
  JPEGENC_mobiCat_type*  mobicat_jpege_payload = NULL;

  mobicat_jpege_payload = (JPEGENC_mobiCat_type*)malloc((unsigned)num_JPEGvalues * sizeof(JPEGENC_mobiCat_type));
  if (mobicat_jpege_payload == NULL)
    return;

  names = (char**)malloc((unsigned)num_JPEGvalues * sizeof(char*));
  if (names == NULL)
  {
    free(mobicat_jpege_payload);
    return;
  }

  memset(names, 0, sizeof(char*) * (unsigned)num_JPEGvalues);

  for (   i = 0; 
        ( i < num_parms ) && ( number_of_elements < num_JPEGvalues );
        ++i
      )
  {

    if (!(camera_tune_registry[ i ].flags & (int)CAMERA_PARMS_API_EXCLUDE_JPEG))
    {

      for (table_index = 0; 
           (table_index < camera_tune_registry[i].table_elements) && (number_of_elements < num_JPEGvalues);
          ++table_index)
      {

        // 3A live summary parameters at front of registry
        if (i == 0)
        {
          for (summary_item = 0; summary_item < (int)NUM_SUMMARY_PARMS; ++summary_item)
          {
            names[number_of_elements] = (char*)malloc(name_size);
            if (names[number_of_elements])
            {
              ret = camera_tune_get_to_jpege(summary_parms[summary_item].parm,
                                            -1, 
                                            names[number_of_elements],
                                            name_size,
                                            &( mobicat_jpege_payload [ number_of_elements ]));
              if (ret == (int)CAMERA_SUCCESS)
              {
                ++number_of_elements;
              }
              else
              {
                // We will re-use this slot.  Deallocate name memory before next round.
                free(names[number_of_elements]);
              }
            }
          }
        }
        else
        {
          names[number_of_elements] = (char*)malloc(name_size);
          if (names[number_of_elements])
          {
            ret = camera_tune_get_to_jpege(camera_tune_registry[i].parm, 
                                          (camera_tune_registry[i].table_elements == 1) ? -1 : table_index,
                                          names[number_of_elements],
                                          name_size,
                                          &( mobicat_jpege_payload [ number_of_elements ]));
            if (ret == (int)CAMERA_SUCCESS)
            {
              ++number_of_elements;
            }
            else
            {
              // We will re-use this slot.  Deallocate name memory before next round.
              free(names[number_of_elements]);
            }
          }
        }
      }
    }
  }

 (void) jpege_mobicat_insert_info(
                                     & (mobicat_jpege_payload [ 0 ] ),
                                     (uint16)number_of_elements
                                  );


  for (i = 0; i < number_of_elements; ++i)
  {
    free(names[i]);
  }
  free(names);
  free(mobicat_jpege_payload);
}


//////////////////////////////////////////////////////////////////////////////

//lint -esym(714,mobicat_deliver_config_to_jpege) this function referenced from outside this module
void  mobicat_deliver_config_to_jpege  (  void  )
{
  if (camera_tune_annotate_jpegs_enable)
  {
    //  mobicat_deliver_config_to_jpege_as_a_single_string( camera_tune_config-string_buffer );
    mobicat_deliver_config_to_jpege_as_distinct_assignments();
  }
}


//////////////////////////////////////////////////////////////////////////////

#ifndef FEATURE_WINCE
/*
void  config_write_dialog_write_to_string ( char         * buffer ,
                                            unsigned int   buffer_length ,
                                            boolean        is_for_lcd
                                          )
{
  if (is_for_lcd)
  {
    (void)snprintf( buffer , buffer_length , "press 'select' to write ini files" );
  }
  else
  {
    buffer[0] = '\0';
  }
}
*/
#endif //!FEATURE_WINCE

//////////////////////////////////////////////////////////////////////////////


int  camera_tune_write_dot_ini_files ( void )
{
  int  result;


  (void)snprintf( camera_tune_ini_file_path ,
                  CAMERA_TUNE_LONGEST_STRING ,
                  "%scamera%s.ini" ,
                  CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                  camera_tune_current_sensor_file_name()
                );
  MSG_HIGH( "CAMERA EFS .INI \"%s\"", camera_tune_ini_file_path,0,0 );
  result =
    camera_tune_write_dot_ini_to_filename ( camera_tune_ini_file_path );
  MSG_HIGH( "camera_tune_write_dot_ini... returned  \"%d\"", result,0,0 );
  return result;
}


//////////////////////////////////////////////////////////////////////////////


#ifndef FEATURE_WINCE
void  config_write_dialog_handle_buttons ( int k )
{
  if ( k == (int)CAMERA_TUNE_KEY_SELECT )
  {
    (void)camera_tune_write_dot_ini_files();
  }
}


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_write_to_ui 
//
//  Description:
//    Writes the current object's expression to string used for UI.
//
//  Parameters:
//    None.
//
//  Return value:
//    None.
//
///////////////////////////////////////////////////////////////////////
static void  camera_tune_write_to_ui ( void )
{
  register int i;
  char *p;
  unsigned int length;

  for (i=0;i< CAMERA_TUNE_LONGEST_EXPRESSION_STRING ; ++i)
  {
    camera_tune_ascii [i] = '\0';
  }

  p = camera_tune_ascii;
  length = CAMERA_TUNE_LONGEST_EXPRESSION_STRING;

  if (unapplied_items)
  {
    // Place a * at begnning of display string to let user know there are unapplied items
    *(p++) = '*';
    --length;
  }

  if (num_parms > 0)
  {
    if (camera_tune_object == 0)
    {
      // 3A live summary parameters at front of registry
      (void)camera_tune_get_summary_to_string(p, length, TRUE);
    }
    else if (camera_tune_object < num_parms)
    {
      // A regular parameter
      (void)camera_tune_get_to_string
              (camera_tune_registry[camera_tune_object].parm, 
               (camera_tune_registry[camera_tune_object].table_elements == 1) ? -1 : camera_tune_table_index,
               p, 
               length,
               TRUE,
               -1,
               FALSE);
    }
    else
    {
      // Special stuff past end of parms (read .ini, etc.)
      (void)safe_strncpy(p, camera_tune_registry[camera_tune_object].name, length);
    }
  }
  else
  {
    // initialization failed
    (void)safe_strncpy(p, "No parameters available", length);
  }
}
#endif //!FEATURE_WINCE


//////////////////////////////////////////////////////////////////////////////


static int  camera_tune_is_digit( int k )
{
  return (  /* safe to assume the digits are in order? */
            (k == '0')
         || (k == '1')
         || (k == '2')
         || (k == '3')
         || (k == '4')
         || (k == '5')
         || (k == '6')
         || (k == '7')
         || (k == '8')
         || (k == '9')
         );
}


static int   camera_tune_is_alpha( int k )
{
  return (
            ( ('a' <= k) && (k <= 'z') )
         || ( ('A' <= k) && (k <= 'Z') )
         );
}


static int  camera_tune_is_alnum( int k )
{
  return (
            camera_tune_is_alpha(k)
         || camera_tune_is_digit(k)
         );
}


#ifndef FEATURE_WINCE
static  void  unread_char( int k )
{
  //  Then our source is a file,
  //  whose "file descriptor" is a global variable.
  //  This is the "tricky" case,
  //  because EFS doesn't (May 2005) support
  //  the notion of file streams, so I have
  //  to implement it here myself.

  if (camera_tune_sourcefile != (-1))
  {
    (void)efs_lseek( camera_tune_sourcefile , -1 , SEEK_CUR );
  }

  (void)k; // This is just to eliminate Lint info 715: not referenced
}


static  int  read_char()
{
  //  Then our source is a file.
  //  This is the "tricky" case,
  //  because EFS doesn't (May 2005) support
  //  the notion of file streams, so I have
  //  to implement it here myself.

  if (camera_tune_sourcefile != (-1))
  {
    int k = '\0';
    int L =   0 ;
    L = efs_read( camera_tune_sourcefile , &k , 1 ); 
    if (L == 0)
       return CAMERA_TUNE_EOF ;  //  ...because that's the EFS behavior.
    else
       return k;
  }
  else
  {
    return CAMERA_TUNE_EOF;      //  ...because the file wasn't opened.
  }
}


/*****************************************************************************
* 
*   space()  reads characters matched by the regular expression
*            [ \t\n\f\r]* , or semicolon-through-newline,
*            which is a comment.
* 
*****************************************************************************/


static  void  space()
{
  int  k = read_char();

  while  (       (k != CAMERA_TUNE_EOF)
          && (   (k == ' ')
              || (k == '\n')
              || (k == '\t')
              || (k == '\f')
              || (k == '\r')
              || (k == ';')
             )
        )
        {
          if (k == ';' )  //  A comment started;  read through '\n'.
             {
               while (   ((k=read_char()) != CAMERA_TUNE_EOF)
                      && ( k              !='\n')
                     )
               {
               }
             };
          if (k=='\n')
             {
               line_number++  ;
             };
          k = read_char();
        }
  if ( k!=CAMERA_TUNE_EOF ) 
     unread_char(k);
}


/*****************************************************************************
* 
*   camera_tune_parse_error( char * , char )
* 
*   prints the string message given to it
*   and the char on which the error occurred,
*   then returns failure up the call chain.
* 
*****************************************************************************/


static  char  camera_tune_parse_error ( char * wahwahwah , char why )
{
  int k;
//sprintf(message, "Parse Error @ line %d @ '%c' :  %s\n" , line_number , why , wahwahwah );
//printf(message);
  MSG_HIGH(  "Parse Error @ line %d @ '%c' : %s" , line_number , why , wahwahwah );

  k = read_char();

  while ( (k != '\n'     ) &&
          (k != '\r'     ) &&
          (k != CAMERA_TUNE_EOF)
        )
  {
    k = read_char();
  }

  space();

  return  0;
}


/*****************************************************************************
* 
*   camera_tune_read_variablename_helper ( char * buffer , int lengthofbuffer )
* 
*                            consumes characters from the input stream
*                            as long as, collectively, they match the
*                            regular expression  [a-zA-Z_0-9\.->]+ .
*                            It returns success or failure up the call chain.
* 
*                            It would also allow "pointer>-_nonsense.>>".
* 
*****************************************************************************/

//lint -e{801} use of goto
static  char  camera_tune_read_variablename_helper ( char * buffer 
                                       , int    lengthofbuffer
                                       )
{
  int position = 0 ,    //  ...in buffer [...] .
      k            ;

  camera_tune_read_variablename_helper_start_of_loop:

  buffer [ position ] = '\0';     //  Delimit the string.
  k                   = read_char();

  if    (
             ( k == CAMERA_TUNE_EOF )
          || !(   
                  ( camera_tune_is_alnum( k ) )
               || ( k == '_'     )
               || ( k == '-'     )
               || ( k == '>'     )
               || ( k == '.'     )
               || ( k == '['     )
               || ( k == ']'     )
              )
          || ( position >= lengthofbuffer )
        )
        goto  camera_tune_read_variablename_helper_end_of_loop ;

  buffer [ position ] = (char)k;  //  Append the chars as a string.
  position++;

  goto  camera_tune_read_variablename_helper_start_of_loop ;

  camera_tune_read_variablename_helper_end_of_loop:

  if ( k!=CAMERA_TUNE_EOF ) 
     unread_char(k);
//printf( "\"%s\"\n" , (char *) buffer );
  return 1;
}


/*****************************************************************************
* 
*   camera_tune_read_variablename (...) 
*                            consumes characters from the input stream
*                            as long as, collectively, they match the
*                            regular expression  [a-zA-Z_][a-zA-Z_0-9\.->]* .
*                            It returns success or failure up the call chain.
* 
*                            Valid variablenames are constrained to start
*                            with a letter, that is, and may use '-', '>', '.'
*                            and underscore, letters, and digits thereafter.
* 
*****************************************************************************/


static  char  camera_tune_read_variablename ()
{
  return camera_tune_read_variablename_helper( camera_tune_symbol , CAMERA_TUNE_LONGEST_STRING);
}


/*****************************************************************************
* 
*   camera_tune_read_ini_value_string ()
*       consumes characters from the input stream
*       as long as, collectively, they match the
*       regular expression  [a-zA-Z_][a-zA-Z_0-9\.->]* .
*       It returns success or failure up the call chain.
* 
*       Valid variablenames are constrained to start
*       with a letter, that is, and may use '-', '>', '.'
*       and underscore, letters, and digits thereafter.
* 
*****************************************************************************/


static  char  camera_tune_read_ini_value_string ()
{
  int   position   = 0 ,    //  ...in char camera_tune_value_string [...] .
        k              ;

  camera_tune_value_string [ position ] = '\0';     //  Delimit the string.
  k = read_char();

  while (     ( k != CAMERA_TUNE_EOF )
          &&  ( ! camera_tune_is_alpha( k ) )
          &&  ( k != ';'       )
          &&  ( position <  CAMERA_TUNE_LONGEST_VALUE_STRING - 1   )
        )
  {
    camera_tune_value_string [ position ] = (char)k;  //  Grow a string.
    position++;
    camera_tune_value_string [ position ] = '\0';
    k = read_char();
  }

  unread_char(k);

  return 1;
}
#endif //!FEATURE_WINCE


/****************************************************************************/
/**                                                                        **/
/**  camera_tune_assign_registered_variable_value ( ... )                  **/
/**                                                                        **/
/**  the parser reads an assignment, the variable name plays the role      **/
/**  of a camera_tune entry's nickname, and the value read is assigned.    **/
/**                                                                        **/
/**  The assumption is that every such nicknamed screen represents         **/
/**  a tunable variable.                                                   **/
/**                                                                        **/
/****************************************************************************/


extern int camera_tune_index_of_object_with_nickname ( const char * );

//lint -e{123} suppress "Macro 'min'/'max' defined with arguments..."; these are structure fields, not calls to macros

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_set_value
//
//  Description:
//    Sets a parameter value possibly from a supplied string
//    or by incrementing/decrementing current value.
//
//  Parameters:
//    parm: Parameter whose info to read.
//    table_index: Element index if parameter is part of a table, 
//                      -1 if parameter is not part of a table.
//    buffer: String representation of value to set.  Used for value if not NULL.
//    value_change: How to change the value: none, increment, or decrement.
//    phase: init or runtime
//
//  Return value:
//    0 if no error.
//
///////////////////////////////////////////////////////////////////////
static int camera_tune_set_value(camera_api_parms_type parm, 
                                 int table_index, 
                                 const char * buffer,
                                 value_change_type value_change,
                                 camera_tune_phase_type phase) 
{
  int ret;
  void* valueptr = NULL;
  camera_tune_parm_format_type format;
  float fVal;
  double dVal, dinc = DEFAULT_INCREMENT_FLOAT;
  int8 i8Val;
  uint8 ui8Val;
  int16 i16Val;
  uint16 ui16Val;
  int32 i32Val, i32inc = DEFAULT_INCREMENT_INT;
  uint32 ui32Val, ui32inc = DEFAULT_INCREMENT_INT;
  boolean has_min, has_max;  // if there is a min or max valid value

  ret = (int)camera_parms_read(phase, parm, table_index, &valueptr, &format);
  if (ret == (int)CAMERA_SUCCESS)  // Parameter was read successfully
  {
    if (valueptr == NULL)
    {
      ret = camera_tune_error_FAILED;
    }
    else
    {
      has_min = format.min != NULL;
      has_max = format.max != NULL;

      switch (format.parm_type)
      {
        case JPEGENC_MC_FLOAT:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            fVal = (float)atof(buffer);
          }
          else
          {
            // Otherwise use the current value just read
            fVal = *(float*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              dinc = *(float*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              fVal += (float)dinc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              fVal -= (float)dinc;
            }
          }

          if ( !(has_min && (fVal < *(float*)format.min)) && 
               !(has_max && (fVal > *(float*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &fVal);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_DOUBLE:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            dVal = atof(buffer);
          }
          else
          {
            // Otherwise use the current value just read
            dVal = *(double*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              dinc = *(double*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              dVal += dinc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              dVal -= dinc;
            }
          }

          if ( !(has_min && (dVal < *(double*)format.min)) && 
               !(has_max && (dVal > *(double*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &dVal);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_INT8:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            i8Val = (int8)strtol(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            i8Val = *(int8*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              i32inc = *(int8*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              i8Val += (int8)i32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              i8Val -= (int8)i32inc;
            }
          }

          if ( !(has_min && (i8Val < *(int8*)format.min)) && 
               !(has_max && (i8Val > *(int8*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &i8Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_UINT8:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            ui8Val = (uint8)strtoul(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            ui8Val = *(uint8*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              ui32inc = *(uint8*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              ui8Val += (uint8)ui32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              ui8Val -= (uint8)ui32inc;
            }
          }

          if ( !(has_min && (ui8Val < *(uint8*)format.min)) && 
               !(has_max && (ui8Val > *(uint8*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &ui8Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_INT16:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            i16Val = (int16)strtol(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            i16Val = *(int16*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              i32inc = *(int16*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              i16Val += (int16)i32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              i16Val -= (int16)i32inc;
            }
          }

          if ( !(has_min && (i16Val < *(int16*)format.min)) && 
               !(has_max && (i16Val > *(int16*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &i16Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_UINT16:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            ui16Val = (uint16)strtoul(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            ui16Val = *(uint16*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              ui32inc = *(uint16*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              ui16Val += (uint16)ui32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              ui16Val -= (uint16)ui32inc;
            }
          }

          if ( !(has_min && (ui16Val < *(uint16*)format.min)) && 
               !(has_max && (ui16Val > *(uint16*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &ui16Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_INT32:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            i32Val = strtol(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            i32Val = *(int32*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              i32inc = *(int32*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              i32Val += i32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              i32Val -= i32inc;
            }
          }

          if ( !(has_min && (i32Val < *(int32*)format.min)) && 
               !(has_max && (i32Val > *(int32*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &i32Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        case JPEGENC_MC_UINT32:
          if (buffer)
          {
            // If a buffer was passed in, use it for the value
            ui32Val = strtoul(buffer, (char**)NULL, 10);
          }
          else
          {
            // Otherwise use the current value just read
            ui32Val = *(uint32*)valueptr;
          }

          if (value_change != VALUE_CHANGE_NONE)
          {
            // Increment or decrement the value
            if (format.increment != NULL)
            {
              // If increment amount exists in the format info, use it
              // otherwise increment is default for parameter type (previously initialized)
              ui32inc = *(uint32*)format.increment;
            }
            if (value_change == VALUE_CHANGE_INCREMENT)
            {
              ui32Val += ui32inc;
            }
            else if (value_change == VALUE_CHANGE_DECREMENT)
            {
              ui32Val -= ui32inc;
            }
          }

          if ( !(has_min && (ui32Val < *(uint32*)format.min)) && 
               !(has_max && (ui32Val > *(uint32*)format.max)) )
          {
            // If resulting value is within bounds, write it
            ret = (int)camera_parms_write(phase, parm, table_index, &ui32Val);
          }
          else
          {
            ret = camera_tune_error_INVALID_PARM;
          }
          break;

        default:
          ret = camera_tune_error_NOT_SUPPORTED;
          break;
      }
      if ((ret == (int)CAMERA_SUCCESS) && ((int)format.flags & camera_parms_api_REQUIRES_APPLY))
      {
        unapplied_items = TRUE;
      }
    }
  }

  return ret;
}


///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_assign_registered_variable_value
//
//  Description:
//    Attempts to locate object with name matching camera_tune_symbol and assign it
//    the value in camera_tune_value_string.
//
//  Parameters:
//    None.
//
//  Return value:
//    0 if success.
//
///////////////////////////////////////////////////////////////////////
static int  camera_tune_assign_registered_variable_value( void )
{
  int i, table_index;
  char buffer[CAMERA_TUNE_LONGEST_VALUE_STRING];
  char *delim;
  unsigned len;
  int ret = 0;

  // See if the string passed in indicates a table element i.e., name[index]
  delim = strchr(camera_tune_symbol, '[');
  if (delim != NULL)
  {
    // Create copy of nickname without index, and set index value
    table_index = atoi(delim + 1);
    len = (unsigned)(delim - camera_tune_symbol);
    if (CAMERA_TUNE_LONGEST_VALUE_STRING < len)
    {
      len = CAMERA_TUNE_LONGEST_VALUE_STRING;
    }
    (void)safe_strncpy(buffer, camera_tune_symbol, len+1);
    i = camera_tune_index_of_object_with_nickname( buffer );
  }
  else
  {
    i = camera_tune_index_of_object_with_nickname( camera_tune_symbol );
    table_index = 0;
  }

  MSG_HIGH( "JOHN : nickname index =s %d" , i , 0 , 0 );

  if ((i > 0) && (i < num_parms))
  {
     /*
      *  A registered object with the given nickname was found.
      *
      */

    if (!(camera_tune_registry[i].flags & (int)CAMERA_PARMS_API_READ_ONLY))
    {
      // It's not read-only
      ret = camera_tune_set_value(camera_tune_registry[i].parm, 
                                  table_index, 
                                  camera_tune_value_string,
                                  VALUE_CHANGE_NONE,
                                  CAMERA_TUNE_PHASE_RUN);
    }
  }
  else
  {
    ret = camera_tune_error_NOT_SUPPORTED;
  }

  return ret;
}


/*****************************************************************************
* 
*   camera_tune_read_assignment (...)
* 
*       is intended to realize a superset of
*       (or possibly exactly) the language
*       specified at
*           http://cloanto.com/specs/ini.html
*       which itself is intended to be 
*       Microsoft's .INI file format.
* 
*       Success or failure is returned up the call chain.
* 
*****************************************************************************/


int  camera_tune_assign_registered_variable_value( void );


#ifndef FEATURE_WINCE
static  char  camera_tune_read_assignment ( void )
{
  char  r ;
  int   k ;

  space();

  k = read_char();
  if ( ! camera_tune_is_alpha(k) )
  {
    return  camera_tune_parse_error( "Assignments (.INI) start with a variablename.\n" , (char)k );
  }
  unread_char( k );  //  Here comes an assignment's left-hand-side.
  r = camera_tune_read_variablename(); 
  if (r == 0)  return r;

  space();

  k = read_char();
  if ( k != '=' )
  {
    return  camera_tune_parse_error( "Assignments (.INI) need an '='.\n" , (char)k );
  }
  space();

  //
  //  Now the variablename and the '=' have been read.
  //  A list of numbers must now appear, of length 1 or more,
  //  where if more than 1 number, then ',' and whitespace separates them.
  //

  //
  //  WARNING : THIS BELOW IS TECHNICALLY RIGHT, THOUGH
  //            TRICKY IN C, BECAUSE IT REQUIRES A DYNAMICALLY
  //            SIZING ARRAY OF NUMBERS, AND I'M NOT IN THE BUSINESS
  //            IN THIS ITERATION OF PARSING FULL-ON LISP IN AN
  //            ARM BUILD.
  //
  //            SO, WE READ THE VALUE AS A STRING, AND LET THE
  //            NICKNAMED VARIABLE'S ASSIGNMENT HANDLER DO THE 
  //            DISASSEMBLY OF THE VALUE STRING.
  //
  //  r = read_comma_separated_number_list ();

  r = camera_tune_read_ini_value_string ();

  if (r == 0) return r;

  //  Now a variable name sits in camera_tune_symbol
  //  and a value string  sits in camera_tune_variable_string,
  //  so call the assigner for the current OnScreen screen.

  (void)camera_tune_assign_registered_variable_value();

  return r;
}


/****************************************************************************/


static  char  camera_tune_read_section_name ( void )
{
  int  k = read_char();

  if ( k != '[' )
     {
        unread_char( k );
        return  0;  
     }
     else
     {
        //
        //  We already the opening '[' ... so read all through the next ']' or CAMERA_TUNE_EOF.
        //
        
        do    {
                k = read_char();  //  We DO NOT save the section name.
              }
        while (    ( k != CAMERA_TUNE_EOF )
                && ( k != ']' )
              );

        if ( k == ']' )
                {
                //printf( "read \"[ sectionname ]\" ...\n" );
                  return  1 ;
                }
           else return  0;
     }
}


/*****************************************************************************
* 
*   camera_tune_read_configuration ()
*                   should consume sequences of assignments until the 
*                   entire character stream is consumed.
*
*****************************************************************************/


void  camera_tune_read_configuration ( void )
{
  int k;
  space();

  while ( (k=read_char()) != CAMERA_TUNE_EOF )
  {
     unread_char(k);

     if (k == '[')
             (void)camera_tune_read_section_name ();
        else (void)camera_tune_read_assignment ();

     space();
  }
}


//////////////////////////////////////////////////////////////////////////////

/*
void  camera_tune_skip_blank_space( char ** s )
{
   while (    ( **s != '\0' )
           && (    ( **s == ' '  ) || ( **s == '\n' )
                || ( **s == '\r' ) || ( **s == '\t' )
                || ( **s == '\f' )
              )
         )
   {
     ++(*s);
   }
}
*/

#ifndef FEATURE_FTM_CAMERA_TASK
///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_lcd_ui_keypress_handler
//
//  Description:
//    Called by the base application, handles the key that was pressed.
//
//  Parameters:
//    key: Code for the key that was pressed.
//
//  Return value:
//    1 if handled, 0 if not handled.
//
///////////////////////////////////////////////////////////////////////
char  camera_tune_lcd_ui_keypress_handler( uint16  key )
{
  char handled = 0;

  if ( camera_tune_is_visible )
  {
    if ( camera_tune_is_reading_digits )
    {
      if ( (key == CAMERA_TUNE_KEY_0)  || (key == CAMERA_TUNE_KEY_1)  ||
           (key == CAMERA_TUNE_KEY_2)  || (key == CAMERA_TUNE_KEY_3)  ||
           (key == CAMERA_TUNE_KEY_4)  || (key == CAMERA_TUNE_KEY_5)  ||
           (key == CAMERA_TUNE_KEY_6)  || (key == CAMERA_TUNE_KEY_7)  ||
           (key == CAMERA_TUNE_KEY_8)  || (key == CAMERA_TUNE_KEY_9)  || 
           (key == CAMERA_TUNE_KEY_CLR)|| (key == CAMERA_TUNE_KEY_I)  ||
           (key == CAMERA_TUNE_KEY_DOWN)|| (key == CAMERA_TUNE_KEY_PTT) ||
           (key == CAMERA_TUNE_KEY_UP)
         )
      {
        camera_tune_read_a_digit( key );
        handled = 1;
      }

      else if ( (key == CAMERA_TUNE_KEY_STAR )  || (key == CAMERA_TUNE_KEY_POUND) )
      {
        ////////////////////////////////////////////////////////
        //   That ENDS the interactive entry of digits to     //
        //   "spell" a number or a variable's or file's name. //
        ////////////////////////////////////////////////////////

        handled                       = 1;
        camera_tune_is_reading_digits = 0;  // i.e., nothing on hold
        camera_tune_entry_delimiter   = key;
        if ( camera_tune_entry_delimiter == CAMERA_TUNE_KEY_STAR )
        {
          camera_tune_seek_object_by_nickname();
        }
        else
        {
          /////////////////////////////////////////
          //  Send result to setter for object.  //
          /////////////////////////////////////////

          (void)camera_tune_set_value(camera_tune_registry[camera_tune_object].parm, 
                                      camera_tune_table_index, 
                                      camera_tune_entry_string,
                                      VALUE_CHANGE_NONE,
                                      CAMERA_TUNE_PHASE_RUN);
        }
      }
    }
    else  //  normal navigation..
    {
      
      // There may be some cleanup to do when leaving "read ini"
      if (camera_tune_object == num_parms + CAMERA_TUNE_PARM_PAST_READ_INI &&
          key != CAMERA_TUNE_KEY_UP && key != CAMERA_TUNE_KEY_DOWN &&
          key != CAMERA_TUNE_KEY_SELECT)
      {
        (void)safe_strncpy(camera_tune_registry[num_parms + CAMERA_TUNE_PARM_PAST_READ_INI].name, "select to read ini file(7)", CAMERA_TUNE_LONGEST_STRING);
        freeconfigs();
      }

      if ( key == CAMERA_TUNE_KEY_2 )
      {
        ////////////////////////////////////////////////////////////////////
        //  Hide camera_tune (MobiCaT) and use the app's event handler.   //
        ////////////////////////////////////////////////////////////////////

        camera_tune_is_visible = 0;
        handled                = 1;

        MSG_HIGH( "JOHN : Received kepress 2: = %d" , 
                  key , 0 , 0 
                );
        camera_tune_lcd_ui_toggle_visibility();
      }

      else if ( key == CAMERA_TUNE_KEY_7 )
      {
        ////////////////////////////////////////////////
        //  Read .ini file if it is in EFS at "/".  //
        ////////////////////////////////////////////////

        (void)snprintf( camera_tune_ini_file_path ,
                        CAMERA_TUNE_LONGEST_STRING ,
                        "%scamera%s.ini" ,
                        CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                        camera_tune_current_sensor_file_name()
                      );
        is_read_ini_user_initiated = TRUE;
        (void)camera_tune_read_dot_ini_file( camera_tune_ini_file_path );
        handled = 1;  // This was missing from old version
      }

      else if ( key == CAMERA_TUNE_KEY_9 )
      {
        ////////////////////////////////////////
        //  Write .ini file to EFS at "/".  //
        ////////////////////////////////////////

        (void)camera_tune_write_dot_ini_files();
        handled = 1;  // This was missing from old version
      }

      else if (key == CAMERA_TUNE_KEY_1 || 
               ((camera_tune_object == num_parms + CAMERA_TUNE_PARM_PAST_APPLY) && key == CAMERA_TUNE_KEY_SELECT))
      {
        (void)camera_tune_apply_changes();
        handled = 1;
      }

      else if (key == CAMERA_TUNE_KEY_LEFT)
      {
        /////////////////////////////////////////////////////////////////
        //  Search to the left (wrapping around) for at most one loop. //
        /////////////////////////////////////////////////////////////////

        int  started_at = camera_tune_object, table_index = camera_tune_table_index;
        while (! handled)  //  initialized to 0 up above..
        {
           if(camera_tune_table_index > 0)
           {
             --camera_tune_table_index;
           }
           else
           {
             --camera_tune_object;
             if (camera_tune_object < 0)
             {
                camera_tune_object = (num_items > 0 ? num_items - 1 : 0);  //  Wrap around.
             }
             camera_tune_table_index = camera_tune_registry[camera_tune_object].table_elements -1;
           }

           // If excluded from on-target UI or we are where we started then keep going, 
           // otherwise stop
           if ( (!(camera_tune_registry[ camera_tune_object ].flags & (int)CAMERA_PARMS_API_EXCLUDE_TARGET_UI)) || 
               ((camera_tune_object == started_at) && (table_index == camera_tune_table_index))
              )
           {
              handled = 1;
           }
        }
      }

      else if (key == CAMERA_TUNE_KEY_RIGHT)
      {
        //////////////////////////////////////////////////////////////////
        //  Search to the right (wrapping around) for at most one loop. //
        //////////////////////////////////////////////////////////////////

        int  starting_point = camera_tune_object, table_index = camera_tune_table_index;
        while (! handled)  //  initialized to 0 up above..
        {
           if (camera_tune_table_index < camera_tune_registry[camera_tune_object].table_elements - 1)
           {
             ++camera_tune_table_index;
           }
           else
           {
             ++camera_tune_object;
             if ( camera_tune_object >= num_items )
             {
                camera_tune_object = 0;  //  Wrap around.
             }
             camera_tune_table_index = 0;
           }

           // If excluded from on-target UI or we are where we started then keep going, 
           // otherwise stop
           if ( (!(camera_tune_registry[ camera_tune_object ].flags & (int)CAMERA_PARMS_API_EXCLUDE_TARGET_UI)) || 
               ((camera_tune_object == starting_point) && (table_index == camera_tune_table_index))
              )
           {
              handled = 1;
           }
        }
      }

      else if ( (key == CAMERA_TUNE_KEY_STAR ) || 
                ((key == CAMERA_TUNE_KEY_POUND) && 
                 !(camera_tune_registry[ camera_tune_object ].flags & (int)CAMERA_PARMS_API_READ_ONLY))
              )
      {

         //////////////////////////////////////////////////////////////////
         //  The user wants to spell a variable (as in SMS) or a number. //
         //////////////////////////////////////////////////////////////////

         handled                          =    1;
         camera_tune_is_reading_digits    =    1;
         camera_tune_entry_delimiter      =  key;
         camera_tune_entry_head_of_string =    0;  //  no chars so far.
         camera_tune_entry_string[0]      = '\0';
      }

      else if (camera_tune_object == num_parms + CAMERA_TUNE_PARM_PAST_READ_INI)
      {
        config_read_dialog_handle_buttons (key);
        handled = 1;
      }

      else if (camera_tune_object == num_parms + CAMERA_TUNE_PARM_PAST_WRITE_INI)
      {
        config_write_dialog_handle_buttons (key);
        handled = 1;
      }

      else if ((key == CAMERA_TUNE_KEY_UP) && !(camera_tune_registry[ camera_tune_object ].flags & (int)CAMERA_PARMS_API_READ_ONLY))
      {
         (void)camera_tune_set_value(camera_tune_registry[camera_tune_object].parm,
                                     camera_tune_table_index,
                                     NULL,
                                     VALUE_CHANGE_INCREMENT,
                                     CAMERA_TUNE_PHASE_RUN);
         handled = 1;
      }

      else if ((key == CAMERA_TUNE_KEY_DOWN) && !(camera_tune_registry[ camera_tune_object ].flags & (int)CAMERA_PARMS_API_READ_ONLY))
      {
         (void)camera_tune_set_value(camera_tune_registry[camera_tune_object].parm,
                                     camera_tune_table_index,
                                     NULL,
                                     VALUE_CHANGE_DECREMENT,
                                     CAMERA_TUNE_PHASE_RUN);
         handled = 1;
      }
    }
  }
  else  //  camera_tune (MobiCaT) is NOT visible on the LCD.
  {
    if (key == CAMERA_TUNE_KEY_2)
    {
      camera_tune_is_visible = 1;  //  Toggle event handlers.
      handled                = 1;  //  Nothing for the normal app to do..
    }
    //  else the App's event handler will see that this event handler
    //  did NOT handle this (keypress) event, because handled is 0.
  }

  return handled;
}
#endif /* not FEATURE_FTM_CAMERA_TASK */


/////////////////////////////////////////////
//  This is called by the OEM's UI app on  //
//  each frame update/draw.                //
/////////////////////////////////////////////


#ifndef FEATURE_FTM_CAMERA_TASK
void  camera_tune_lcd_ui_updater( void )
{
  // 
  //  The currently observed object may want to update itself.
  //

  if ( camera_tune_is_visible == 1 )
  {
    if ( camera_tune_is_reading_digits )
    {
      camera_tune_lcd_ui_print( camera_tune_show_digits_or_propose_name() );
    }
    else
    {
      camera_tune_write_to_ui ();  //  Have current object print to camera_tune_ascii[].
    }
    camera_tune_lcd_ui_print( camera_tune_ascii );
  }
}
#endif /* not FEATURE_FTM_CAMERA_TASK */
#endif //!FEATURE_WINCE

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_get_list
//
//  Description:
//    Enumerates the list of parameters and corresponding info.
//    Places the data in the buffer supplied.
//
//  Parameters:
//    num_to_start: Parameter to start with. If all data doesn't fit in buffer,
//                  subsequent calls to this function can be made with this
//                  value set for next parameter needed.
//
//    response: Buffer to copy data into. Contains binary data. Format of buffer:
//              status: 1 byte (0 = completed, 1 = more)
//              num_returned: 2 bytes
//                item_num: 2 bytes
//                table_count: 2 bytes
//                flags: 4 bytes (0x1 = read-only, 0x2 = deferred apply, etc.)
//                name: null terminated string
//                (repeated num_returned times)
//
//    response_size: Size of output buffer.
//
//  Return value:
//    0 if success.
//
///////////////////////////////////////////////////////////////////////
int camera_tune_get_list ( uint16 num_to_start, byte *response, uint32 response_size )
{
  byte *cur_byte = response;
  uint32 nSize, parm_size, table_count_size, flags_size, name_size=0;
  uint16 cur_parm = num_to_start;
  uint16 table_count;
  uint16 record_count;
  byte nStatus;
  boolean bContinue = TRUE;

  if (response == NULL)
  {
    return camera_tune_error_INVALID_PARM;
  }

  //Check if response size is big enough to hold status and record count
  nSize = (sizeof(byte) + sizeof(uint16));
  if (response_size < nSize)
  {
    return camera_tune_error_INVALID_PARM;
  }
  cur_byte += nSize;

  // Sizes that don't change
  parm_size =        sizeof(cur_parm);
  table_count_size = sizeof(table_count);
  flags_size =       sizeof(camera_tune_registry[0].flags);
  
  while(bContinue)
  {
    // See if we are past the end of the parameter list
    if (cur_parm >= num_parms)
    {
      bContinue = FALSE;
    }

    if (bContinue)
    {
      // Skip this parm if flagged for remote exclusion
      if (camera_tune_registry[ cur_parm ].flags & (int)CAMERA_PARMS_API_EXCLUDE_REMOTE_UI) 
      {
        ++cur_parm;
        continue;
      }
    }

    // See if there is room in the return buffer to write more info
    if (bContinue)
    {
      name_size = strlen(camera_tune_registry[cur_parm].name) + 1;
      nSize = parm_size + table_count_size + flags_size + name_size;
      if ((cur_byte + nSize) > (response + response_size))
      {
        bContinue = FALSE;
      }
    }

    // Write the data
    if (bContinue)
    {
      // parm number
      memcpy(cur_byte, (void*)&cur_parm, parm_size);
      cur_byte += parm_size;

      // table count (number of items in this parm)
      table_count = (uint16)camera_tune_registry[cur_parm].table_elements;
      memcpy(cur_byte, (void*)&table_count, table_count_size);
      cur_byte += table_count_size;

      // flags
      memcpy(cur_byte, (void*)&camera_tune_registry[cur_parm].flags, flags_size);
      cur_byte += flags_size;

      if(name_size <= CAMERA_TUNE_LONGEST_STRING)
      {
      // name
      memcpy(cur_byte, (void*)camera_tune_registry[cur_parm].name, name_size);
      cur_byte += name_size;
      }
      else
      {
        *cur_byte = 0;
        cur_byte++;
      }	  
      ++cur_parm;
    }
  }

  // Write status
  cur_byte = response;  // Point to beginning of the buffer
  if (cur_parm == num_parms)
  {
    nStatus = 0;
  }
  else
  {
    nStatus = 1;
  }
  nSize = sizeof(nStatus);
  memcpy(cur_byte, (void*)&nStatus, nSize);
  cur_byte += nSize;

  // Write record count
  record_count = cur_parm - num_to_start;
  memcpy(cur_byte, (void*)&record_count, parm_size);

  return CAMERA_TUNE_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_get_parms
//
//  Description:
//    Takes a list of parameters and places their current values
//    in the buffer supplied.
//
//  Parameters:
//    request: Buffer to read list of parameters from. Contains binary data.
//             Format of buffer:
//             num_items: 2 bytes
//               item_num: 2 bytes
//               table_index: 2 bytes
//               (repeated num_items times)
//
//    request_size: size of request buffer
//
//    response: Buffer to copy data into. Contains binary data. 
//              Format of buffer:
//              status: 1 byte 
//                (0 = all parms are applied, 1 = unapplied parms exist)
//              num_items: 2 bytes
//                item_num: 2 bytes
//                table_count: 2 bytes
//                value: null terminated string
//                (repeated num_returned times)
//
//    response_size: Size of output buffer.
//
//  Return value:
//    0 if success.
//
///////////////////////////////////////////////////////////////////////
int camera_tune_get_parms ( byte *request, uint32 request_size,
                            byte *response, uint32 response_size )
{
  byte *cur_byte_request = request;
  byte *cur_byte_response = response;
  uint16 request_count; //number of parms in request
  uint16 parm_count = 0;  // number of parms processed so far
  uint16 parm_num = 0;  //index into parms list
  uint16 table_index = 0;
  uint32 nSize, parm_size, table_index_size, value_size=0;
  char szValue[CAMERA_TUNE_LONGEST_VALUE_STRING] = "";
  byte nStatus;
  boolean bContinue = TRUE;
  int ret;

  if (request == NULL || response == NULL)
  {
    return camera_tune_error_INVALID_PARM;
  }

  //Check if request size is big enough to hold record count
  nSize = sizeof(uint16);
  if (request_size < nSize)
  {
    return camera_tune_error_INVALID_PARM;
  }
  memcpy((void*)&request_count, cur_byte_request, nSize);
  cur_byte_request += nSize;
  MSG_HIGH( "MOBICAT request_count %u" , (request_count) , 0 , 0 );


  //Check if response size is big enough to hold status and record count
  nSize = (sizeof(byte) + sizeof(uint16));
  if (response_size < nSize)
  {
    return camera_tune_error_INVALID_PARM;
  }
  cur_byte_response += nSize;

  // Sizes that don't change
  parm_size =        sizeof(parm_num);
  table_index_size = sizeof(table_index);
  
  while(bContinue)
  {
    // See if there are more parms in the request buffer
    nSize = parm_size + table_index_size;
    if (parm_count >= request_count ||
        cur_byte_request + nSize > request + request_size)
    {
      bContinue = FALSE;
    }

    if (bContinue)
    {
      // Read the next parm from the request buffer
      memcpy((void*)&parm_num, cur_byte_request, parm_size);
      cur_byte_request += parm_size;
      memcpy((void*)&table_index, cur_byte_request, table_index_size);
      cur_byte_request += table_index_size;
      MSG_HIGH( "MOBICAT request parm_num %u, index %u" , parm_num , table_index , 0 );

      // See if the parm is in range
      if (parm_num >= num_parms)
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
    {
      // See if the table_index is in range
      if (table_index >= camera_tune_registry[parm_num].table_elements)
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
    {
      if (parm_num == 0) // 3A live summary
      {
        ret = camera_tune_get_summary_to_string (szValue,
                                                 CAMERA_TUNE_LONGEST_VALUE_STRING,
                                                 FALSE);
      }
      else  // All other parms
      {
        ret = camera_tune_get_to_string(camera_tune_registry[parm_num].parm,
                                        table_index, 
                                        szValue, 
                                        CAMERA_TUNE_LONGEST_VALUE_STRING,
                                        FALSE,
                                        -1,
                                        TRUE);
      }
      
      if (ret != (int)CAMERA_SUCCESS)
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
    {
      // See if there is room in the response buffer to write more info
      value_size = strlen(szValue) + 1;
      nSize = parm_size + table_index_size + value_size;
      if ((cur_byte_response + nSize) > (response + response_size))
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
      {
        // Write the data

        // parm number
        memcpy(cur_byte_response, (void*)&parm_num, parm_size);
        cur_byte_response += parm_size;

        // table_index
        memcpy(cur_byte_response, (void*)&table_index, table_index_size);
        cur_byte_response += table_index_size;

        if(value_size <= CAMERA_TUNE_LONGEST_VALUE_STRING)
        {
        // value
        memcpy(cur_byte_response, (void*)szValue, value_size);
        cur_byte_response += value_size;
        }
        else
        {
           *cur_byte_response = 0;
           cur_byte_response++;
        }		

        ++parm_count;
    }
  }

  // Write status
  cur_byte_response = response;  // Point to beginning of the buffer

  if (unapplied_items)
  {
    nStatus = 1;
  }
  else
  {
    nStatus = 0;
  }
  nSize = sizeof(nStatus);
  memcpy(cur_byte_response, (void*)&nStatus, nSize);
  cur_byte_response += nSize;

  // Write record count
  memcpy(cur_byte_response, (void*)&parm_count, parm_size);

  return CAMERA_TUNE_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_set_parms
//
//  Description:
//    Takes a list of parameters and sets their values.
//
//  Parameters:
//    request: Buffer containing parameters and values. Contains binary data.
//             Format of buffer:
//             num_items: 2 bytes
//               item_num: 2 bytes
//               table_index: 2 bytes
//               value: null terminated string
//               (repeated num_items times)
//
//    request_size: size of request buffer
//
//    status [out]: 0 if all parameters are applied,
//                  1 if unapplied parameters exist
//
//  Return value:
//    0 if success.
//
///////////////////////////////////////////////////////////////////////
int camera_tune_set_parms ( byte *request, uint32 request_size,
                            byte *status )
{
  byte *cur_byte_request = request;
  uint16 request_count; //number of parms in request
  uint16 parm_count = 0;  // number of parms processed so far
  uint16 parm_num = 0;  //index into parms list
  uint16 table_index = 0;
  char *pszValue = NULL;
  uint32 nSize, parm_size, table_index_size, value_size;
  boolean bContinue = TRUE;

  if (request == NULL || status == NULL)
  {
    return camera_tune_error_INVALID_PARM;
  }

  //Check if request size is big enough to hold record count
  nSize = sizeof(uint16);
  if (request_size < nSize)
  {
    return camera_tune_error_INVALID_PARM;
  }
  memcpy((void*)&request_count, cur_byte_request, nSize);
  cur_byte_request += nSize;


  // Sizes that don't change
  parm_size =        sizeof(parm_num);
  table_index_size = sizeof(table_index);
  
  while(bContinue)
  {
    // See if there are more parms in the request buffer
    nSize = parm_size + table_index_size;
    if (parm_count >= request_count ||
        cur_byte_request + nSize > request + request_size)
    {
      bContinue = FALSE;
    }

    if (bContinue)
    {
      // Read the next parm from the request buffer
      memcpy((void*)&parm_num, cur_byte_request, parm_size);
      cur_byte_request += parm_size;
      memcpy((void*)&table_index, cur_byte_request, table_index_size);
      cur_byte_request += table_index_size;

      // See if the parm is in range
      if (parm_num >= num_parms)
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
    {
      // See if the table_index is in range
      if (table_index >= camera_tune_registry[parm_num].table_elements)
      {
        bContinue = FALSE;
      }
    }

    if (bContinue)
    {
      // Get size of value string
      value_size = strlen((char*)cur_byte_request) + 1;
      if (cur_byte_request + nSize > request + request_size)
      {
        bContinue = FALSE;
      }
      pszValue = (char*)cur_byte_request;
      cur_byte_request += value_size;
    }

    if (bContinue)
    {
      (void)camera_tune_set_value(camera_tune_registry[parm_num].parm, 
                                  table_index, 
                                  pszValue,
                                  VALUE_CHANGE_NONE,
                                  CAMERA_TUNE_PHASE_RUN);

      //Do not discontinue processing if user set an invalid value
    }

    ++parm_count;
  }

  // Write status
  if (unapplied_items)
  {
    *status = 1;
  }
  else
  {
    *status = 0;
  }

  return CAMERA_TUNE_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
//
//  Function name:
//    camera_tune_misc_command
//
//  Description:
//    Sends a miscellaneous command to QMobiCaT in the buffer supplied.
//
//  Parameters:
//    request: Buffer containing request data. Contains binary data.
//             Format of buffer:
//             message_id: 1 byte
//               0 = get version
//               1 = apply changes
//               2 = write .ini file
//               3 = read .ini file
//               4 = list .ini files
//             message-specific data: 127 bytes
//               for message_id 0: no data
//               for message_id 1: no data
//               for message_id 2: no data
//               for message_id 3: null terminated file name
//                                 (empty string for default .ini file)
//               for message_id 4: no data
//
//    request_size: size of request buffer
//
//    response: Buffer to copy data into. Contains binary data. 
//              Format of buffer:
//              message-specific data: 128 bytes
//               for message_id 0: null-terminated version string
//               for message_id 1: 1 byte: 0 = ok, 1 = failed
//               for message_id 2: 1 byte: 0 = ok, 1 = failed
//               for message_id 3: 1 byte: 0 = ok, 1 = failed
//               for message_id 4:
//                 num_entries: 1 byte
//                   null-terminated file name
//                   (repeated num_entries times)
//
//    response_size: Size of output buffer.
//
//  Return value:
//    0 if success.
//
///////////////////////////////////////////////////////////////////////
int camera_tune_misc_command ( byte *request, uint32 request_size,
                               byte *response, uint32 response_size )
{
  byte *cur_byte_request = request;
  byte *cur_byte_response = response;
  uint32 nSize;
  byte message_id;
  byte nStatus;
  int nRet;
  boolean bContinue = TRUE;
  byte cur_name;
  byte *name_end;

  if (request == NULL || response == NULL)
  {
    return camera_tune_error_INVALID_PARM;
  }
  
  //Check if request size is big enough to hold message_id
  nSize = sizeof(byte);
  if (request_size < nSize)
  {
    return camera_tune_error_INVALID_PARM;
  }
  memcpy((void*)&message_id, cur_byte_request, nSize);
  cur_byte_request += nSize;

  switch (message_id)
  {
    case 0:  // version id
      //Check if response size is big enough to hold version id
      nSize = strlen(CAMERA_TUNE_DIAG_VERSION_ID) + 1;
      if (response_size < nSize)
      {
        return camera_tune_error_INVALID_PARM;
      }
      memcpy((void*)cur_byte_response, CAMERA_TUNE_DIAG_VERSION_ID, nSize);
      break;

    case 1:  // apply changes
      (void)camera_tune_apply_changes();
      if (unapplied_items)
      {
        nStatus = 1;
      }
      else
      {
        nStatus = 0;
      }

      nSize = sizeof(nStatus);

      // Check if response size is big enough to hold response
      if (response_size >= nSize)
      {
        memcpy(cur_byte_response, (void*)&nStatus, nSize);
      }
      break;
    case 2:  // write .ini
      nRet = camera_tune_write_dot_ini_files();
      if (nRet == 0)
      {
        nStatus = 0;
      }
      else
      {
        nStatus = 1;
      }

      nSize = sizeof(nStatus);

      // Check if response size is big enough to hold response
      if (response_size >= nSize)
      {
        memcpy(cur_byte_response, (void*)&nStatus, nSize);
      }
      break;
    case 3: // read .ini
      // get .ini file name, if supplied
      name_end = cur_byte_request;
      while ((name_end < request + request_size) && (*name_end != 0))
      {
        ++name_end;
      }
      
      nStatus = 1; // assume fail until success
      
      if (*name_end != 0)
      {
        // buffer does not contain required name or empty string
        bContinue = FALSE;
      }

      if (bContinue)
      {
        if (strlen((char*)cur_byte_request) > 0)
        {
          // construct full path with .ini file name supplied
          (void)snprintf( camera_tune_ini_file_path ,
                          CAMERA_TUNE_LONGEST_STRING ,
                          "%s%s" ,
                          CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                          cur_byte_request
                        );
        }
        else
        {
          // use default .ini file name
          (void)snprintf( camera_tune_ini_file_path ,
                          CAMERA_TUNE_LONGEST_STRING ,
                          "%scamera%s.ini" ,
                          CAMERA_TUNE_WHERE_ARE_CONFIGS ,
                          camera_tune_current_sensor_file_name()
                        );
        }

        is_read_ini_user_initiated = TRUE;
        nRet = camera_tune_read_dot_ini_file( camera_tune_ini_file_path );

        if (nRet == 0)
        {
          nStatus = 0;
        }
      }

      nSize = sizeof(nStatus);

      // Check if response size is big enough to hold response
      if (response_size >= nSize)
      {
        memcpy(cur_byte_response, (void*)&nStatus, nSize);
      }
      break;

    case 4: // list .ini files
      //Check if data size is big enough to hold record count
      nSize = (sizeof(byte));
      if (response_size < nSize)
      {
        return camera_tune_error_INVALID_PARM;
      }
      cur_byte_response += nSize;

      nRet = camera_tune_list_desired_files_in_directory
             (
               CAMERA_TUNE_WHERE_ARE_CONFIGS ,
               ".ini"
             );
  
      cur_name = 0;
      bContinue = (nRet == 0);
      while(bContinue)
      {
        // See if we are at end of list
        if (cur_name >= camera_tune_num_of_configs)
        {
          bContinue = FALSE;
        }

        // See if there is room in the return buffer to write more info
        if (bContinue)
        {
          nSize = strlen(camera_tune_config_choices[cur_name]) + 1;
          if ((cur_byte_response + nSize) > (response + response_size))
          {
            bContinue = FALSE;
          }
        }

        // Write the data
        if (bContinue)
        {
          // name
          memcpy(cur_byte_response, (void*)camera_tune_config_choices[cur_name], nSize);
          cur_byte_response += nSize;

          ++cur_name;
        }
      }

      // Write record count
      cur_byte_response = response;  //point to front of buffer
      memcpy(cur_byte_response, (void*)&cur_name, sizeof(cur_name));

      // Free the config file list
      freeconfigs();

      break;

    default:
      return camera_tune_error_INVALID_PARM;
  }

  return CAMERA_TUNE_ERROR_SUCCESS;
}


#ifdef FEATURE_WINCE
#error code not present
#endif // FEATURE_WINCE

//////////////////////////////////////////////////////////////////////////////
#endif//FEATURE_CAMERA_MOBICAT
//////////////////////////////////////////////////////////////////////////////
#ifdef  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
//////////////////////////////////////////////////////////////////////////////


void  camera_tune_init ( void )
{
  camera_api_parms_type parm;
  void* valueptr = NULL;
  camera_tune_parm_format_type format;
  int ret, nameindex;
  int i;
  boolean is_summary_parm;

  camera_tune_application_state = APP_STATE_INIT ;
  
  ret = (int)camera_parms_init();
  if (ret == (int)CAMERA_SUCCESS)
  {
    // Special-case entries for 3A live summary screen
    camera_tune_registry[0].parm = CAMERA_API_LAST;
    (void)safe_strncpy(camera_tune_registry[0].name, "3A", CAMERA_TUNE_LONGEST_STRING);
    camera_tune_registry[0].table_elements = 1;
    camera_tune_registry[0].flags = (int)CAMERA_PARMS_API_READ_ONLY;
    num_parms = 1;
    num_JPEGvalues = NUM_SUMMARY_PARMS;

    for (parm = (camera_api_parms_type)0; parm < CAMERA_API_LAST; ++parm)
    {
      // Exclude parameters on the 3A live summary screen
      for (i = 0, is_summary_parm = FALSE; 
           (i < (int)NUM_SUMMARY_PARMS) && (!is_summary_parm); 
           ++i)
      {
        if (parm == summary_parms[i].parm)
        {
          is_summary_parm = TRUE;
        }
      }

      if (!is_summary_parm)
      {
        ret = (int)camera_parms_read
                    (CAMERA_TUNE_PHASE_INIT, 
                     parm,
                     0,
                     &valueptr,
                     &format);

        if (ret == (int)CAMERA_SUCCESS)  // Parameter was read successfully
        {
          camera_tune_registry[num_parms].parm = parm;
          (void)safe_strncpy(camera_tune_registry[num_parms].name, format.name, CAMERA_TUNE_LONGEST_STRING);
          camera_tune_registry[num_parms].table_elements = format.num_elements;
          camera_tune_registry[num_parms].flags = (uint32)format.flags;
          ++num_parms;
          if (!(format.flags & (int)CAMERA_PARMS_API_EXCLUDE_JPEG))
          {
            num_JPEGvalues += format.num_elements;
          }
        }
      }
    }

    // Special-case entries for read/write ini file, apply changes, etc.
    num_items = num_parms;
    camera_tune_registry[num_items].parm = CAMERA_API_LAST;
    (void)safe_strncpy(camera_tune_registry[num_items].name, "select to read ini file(7)", CAMERA_TUNE_LONGEST_STRING);
    camera_tune_registry[num_items].table_elements = 1;
    camera_tune_registry[num_items].flags = 0;
    ++num_items;

    camera_tune_registry[num_items].parm = CAMERA_API_LAST;
    (void)safe_strncpy(camera_tune_registry[num_items].name, "select to write ini file(9)", CAMERA_TUNE_LONGEST_STRING);
    camera_tune_registry[num_items].table_elements = 1;
    camera_tune_registry[num_items].flags = 0;
    ++num_items;

    camera_tune_registry[num_items].parm = CAMERA_API_LAST;
    (void)safe_strncpy(camera_tune_registry[num_items].name, "select to apply changes(1)", CAMERA_TUNE_LONGEST_STRING);
    camera_tune_registry[num_items].table_elements = 1;
    camera_tune_registry[num_items].flags = 0;
    ++num_items;

    // Initialize from camera config
    for (i = 0; 
         (i < (int)NUM_CAMERA_CONFIG_INIT_VALUES) && (camera_config_init_values[i].name != NULL); 
         ++i)
    {
      nameindex = camera_tune_index_of_object_with_nickname(camera_config_init_values[i].name);
      if (nameindex > -1)
      {
        if (!(camera_tune_registry[nameindex].flags & (int)CAMERA_PARMS_API_READ_ONLY) && 
             camera_config_init_values[i].index < camera_tune_registry[nameindex].table_elements)
        {
          // It's not read-only
          ret = (int)camera_tune_set_value(camera_tune_registry[nameindex].parm, 
                                           camera_config_init_values[i].index, 
                                           camera_config_init_values[i].value,
                                           VALUE_CHANGE_NONE,
                                           CAMERA_TUNE_PHASE_INIT);
        }
      }
    }
  }

  // Since this function is called early in startup, values will be applied as service starts
  // So no need to call: (void)camera_tune_apply_changes();

  #ifdef  FEATURE_CAMERA_MOBICAT
#ifdef FEATURE_WINCE
#error code not present
#else // FEATURE_WINCE
  camera_tune_is_visible        = 0;
  camera_tune_is_reading_digits = 0;
#endif //FEATURE_WINCE
  camera_tune_object            = 0;
  camera_tune_table_index       = 0;
  is_read_ini_user_initiated    = FALSE;
  unapplied_items               = FALSE;
  camera_tune_num_of_configs    = 0;
  memset(camera_tune_config_choices, NULL, sizeof(char*) * CAMERA_TUNE_MOST_CONFIGS);
  #endif//FEATURE_CAMERA_MOBICAT

  camera_tune_application_state = APP_STATE_RUNNING ;

  #ifdef FEATURE_CAMERA_TUNE_VERIFY_CONFIG
  // This is to be able to verify, externally, the values that have been set.
  camera_tune_write_dot_ini_to_filename ( "/config_verify.ini" );
  #endif

#ifdef FEATURE_WINCE
#error code not present
#endif
}


void camera_tune_stop(void)
{
#ifdef  FEATURE_CAMERA_MOBICAT
#ifdef FEATURE_WINCE
#error code not present
#endif //FEATURE_WINCE
#endif //FEATURE_CAMERA_MOBICAT
}

///////////////////////////////////////////////////////////////////////////
#endif//FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
