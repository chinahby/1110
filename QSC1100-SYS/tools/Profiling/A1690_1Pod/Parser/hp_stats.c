/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         T I M E T E S T   P O S T P R O C E S S I N G   T O O L

GENERAL DESCRIPTION
  This code allows to postprocess timetest result captured with
  an HP logic analyzer in ASCII format.
  It computes median, minimum, and maximum time spent per task /
  interrupt, and overall CPU usage in percent.

  Build the executable with the GNU compiler using cygwin 1.3.1 
  or later via

     gcc -o hp_stats.exe hp_stats.c

  The required initialization file is available as hp_stats.ini
  from the MSM (COMMON) archive. It has corresponding definitions
  for task IDs defined in timetest.h.


EXTERNALIZED FUNCTIONS

  main() - Entry point to program


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.


Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: O:/src/asw/COMMON/vcs/hp_stats.c_v   1.17   12 Sept 2002 19:35   omichaelis  $
  $Header: //depot/asic/msm6800/tools/Profiling/A1690_1Pod/Parser/hp_stats.c#1 $ $DateTime: 2005/08/08 12:10:06 $ $Author: smuckle $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/04   kvd     Added a cmd line arg to choose prefix for output files
04/14/04   kvd     corrected parsing of -n option for non display_mode
04/09/04   kvd     Added display_mode, to not display HTML pages at the end
                   of parsing, for automation
09/12/02   om      Ignore subsequent multiple '0' entries (bad samples).
10/23/01   om      Made data arrays grow dynamically.
10/22/01   om      Updated for LA formats 1661, 16500, 16700, and 1694.
08/16/01   om      Added suppot for 1x data rates (dep. on timetest.h r1.5).
08/15/01   om      Removed Rex Scheduler entry and fixed task plot size
08/06/01   om      Added task scheduling diagram and spawning of browser.
07/27/01   om      Added wait time & timer tick stats and html output.
07/22/01   om      Updated for timetest with tasks, ISRs, and RLP stats.
02/22/00   om      Created Module


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <getopt.h>

/* The Cygnus stdlib header files misses the declaration of optarg */
extern char* optarg;

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define   min(a, b)               ((a) > (b) ? (b) : (a))
#define   max(a, b)               ((a) < (b) ? (b) : (a))

#define   MAX_TASKS                 0x0401

/* 16-bit value used as a unique parse token for the postprocessor */
#define   TT_PARSE_TOKEN            0xffff
#define   TT_PARSE_TOKEN_ISR_CONT   0x8000
#define   TT_PARSE_TOKEN_ISR_LEVEL  0x0f00
#define   TT_PARSE_TOKEN_IS856      0x4000
#define   TT_PARSE_TOKEN_IS2K       0x2000

#define   PARSE_FTOKEN              TT_PARSE_TOKEN

#define   CPU_SAMPLE_INTERVAL       20

#define   TOTAL_RLP_TX_SCALE        100
#define   TOTAL_RLP_IS856_RX_SCALE  1000
#define   TOTAL_RLP_IS2K_RX_SCALE   100
#define   MAX_RLP_DATA              1000
#define   RLP_LOGGING_INTERVAL      500

#define   MAX_TIME_DATA             1000

#define   TIMETEST_SLEEP_TASK_ID    0x14
#define   TIMETEST_TIMER_TICK_ID    0xb5

#define   NAME_MAX_CHAR             32

#define   printd   if ( debug ) printf 

typedef enum
{
  MODEL_UNKNOWN,
  MODEL_HP1661,
  MODEL_HP16500,
  MODEL_AG16700,
  MODEL_AG1694
} logic_analyzer_model_type;

typedef enum
{
  PARSE_NULL_STATE = 0x0,
  PARSE_FTOKEN_STATE,
  PARSE_FTOKEN_NULL_STATE,
  PARSE_FADDR_MSW_STATE,
  PARSE_FADDR_MSW_NULL_STATE,
  PARSE_FADDR_LSW_STATE,
  PARSE_FADDR_LSW_NULL_STATE,
  PARSE_FTASK_STATE,
  PARSE_FTASK_NULL_STATE,
  PARSE_TASKID_STATE,
  PARSE_RLPTOKEN_STATE,
  PARSE_RLPTOKEN_NULL_STATE,
  PARSE_RLP_STATS_STATE,
  PARSE_RLP_STATS_NULL_STATE
} parse_state_type;

typedef struct
{
  double* t_data;
  double* r_data;
  int     d_size;
  int     d_len;
  double  t_total_run;
  double  t_median;
  double  t_variance;
  double  t_min;
  double  t_max;
  double  t_percent;
  double  w_median;
  double  w_min;
  double  w_max;
  int     i_max_nest_level;
} time_data_type;

/* The time_data type struct needs to be first for sorting to work !! */
typedef struct
{
  time_data_type t;
  int            task_id;
  char*          t_name;
} task_data_type;

/* The time_data type struct needs to be first for sorting to work !! */
typedef struct func_data_struct
{
  time_data_type           t;
  long                     f_addr;
  char*                    f_name;
  int                      f_task[MAX_TASKS];
  struct func_data_struct* prev;
  struct func_data_struct* next;
} func_data_type;

task_data_type    task_data[MAX_TASKS];
task_data_type**  task_data_sorted = NULL;
func_data_type*   func_data = NULL;
func_data_type**  func_data_sorted = NULL;
int               task_hash[MAX_TASKS];
int               cpu_data_size = 0;
double*           cpu_data;
double            rlp_tx_data_rate[MAX_RLP_DATA];
double            rlp_rx_data_rate[MAX_RLP_DATA];
double            rlp_avg_dtime_ms = 0.0;
double            rlp_first_dtime_ms = 0.0;
double            rlp_last_dtime_ms = 0.0;
int               rlp_data_count = 0;
char              acquisition_time[128];
char              cpu_file_reference[128];
char              rlp_file_reference[128];
char              latency_file_reference[128];
char              task_file_reference[128];
char              task_file_img_reference[128];
char              html_start_file[192];
char              cur_working_dir[192];

int     num_funcs          = 0;
int     num_tasks          = 1;
int     num_tasks_sorted   = 0;
int     total_tasks        = 0;
double  total_time         = 0.0;
int     total_rlp_tx_bytes = 0;
int     total_rlp_rx_bytes = 0;
double  total_rlp_tx_rate  = 0.0;
double  total_rlp_rx_rate  = 0.0;

logic_analyzer_model_type la_model = MODEL_UNKNOWN;
int     func_mode          = 0;
int     text_only_mode     = 0;
int     display_mode       = 0;
int     output_prefix_given= 0;
int     max_results        = 0x7fffffff;
char*   symbol_file_name   = NULL;
char*   output_file_prefix = NULL;
int     output_format_html = 0;
int     debug              = 0;
int     testapp_format     = 0;

/*===========================================================================

FUNCTION TASK_DATA_INIT

DESCRIPTION
  This function does all data structure initialization from the .ini file.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void task_data_init( char* ini_file_name )
{
  FILE* ini_file = NULL;
  char* fgets_result;
  int   i;
  char  line[135];
  int   num;
  char  name[135];
  
  /* first element is always the non-isr stats */
  task_data[0].task_id       = 0x00;
  task_data[0].t_name        = "TT_Marker";
  task_data[0].t.t_data      = malloc( MAX_TIME_DATA * sizeof( double ) );
  task_data[0].t.r_data      = malloc( MAX_TIME_DATA * sizeof( double ) );
  task_data[0].t.d_size      = MAX_TIME_DATA;
  task_data[0].t.d_len       = 0;
  task_data[0].t.t_median    = 0.0;
  task_data[0].t.t_variance  = 0.0;
  
  /* initialize rest to zero */
  for ( i=1; i<MAX_TASKS; i++ )
  {
    task_data[i].task_id       = 0;
    task_data[i].t_name        = NULL;
    task_data[i].t.t_data      = malloc( MAX_TIME_DATA * sizeof( double ) );
    task_data[i].t.r_data      = malloc( MAX_TIME_DATA * sizeof( double ) );
    task_data[i].t.d_size      = MAX_TIME_DATA;
    task_data[i].t.d_len       = 0;
    task_data[i].t.t_total_run = 0.0;
    task_data[i].t.t_median    = 0.0;
    task_data[i].t.t_variance  = 0.0;
    task_data[i].t.t_percent   = 0.0;
  }
  
  /* Initialize RLP throughput array */
  memset( rlp_tx_data_rate, 0, sizeof( double ) * MAX_RLP_DATA );
  memset( rlp_rx_data_rate, 0, sizeof( double ) * MAX_RLP_DATA );

  /* read TASKs in from .ini file */
  ini_file = fopen( ini_file_name, "r" );
  if ( ini_file == NULL )
  {
    printf( "ERROR: Missing .ini file (%s).\n", ini_file_name );
    exit( 1 );
  }
  
  /* check for correct .ini file */
  memset( line, 0, 135 );
  fgets_result = fgets( line, 135, ini_file );
  if ( strncmp( line, "[hp_stats Tasks / ISRs]", 14 ) )
  {
    printf( 
      "ERROR: Wrong .ini file (expecting '[hp_stats Tasks / ISRs]').\n" );
    exit( 1 );
  }
  
  do 
  {
    memset( line, 0, 135 );
    memset( name, 0, 135 );
    num = 0;
    
    fgets_result = fgets( line, 135, ini_file );
    
    if ( !strncmp( line, "0x", 2 ) )
    {
      sscanf( line, "%x=%s", &num, name );
      
      if ( num >= MAX_TASKS )
      {
        printf( "ERROR: Found invalid TASK number (0x%x)\n", num );
        exit( 1 );
      }
      
      task_data[num_tasks].t_name = (char*) malloc( strlen( name ) + 1 );
      strcpy( task_data[num_tasks].t_name, name );
      task_data[num_tasks].task_id = num;
      
      task_hash[num] = num_tasks;
      printd( "DBG - Taskname %s, ID 0x%x, Index %d\n", name, num, num_tasks );
      num_tasks++;
    }
  } while( fgets_result != NULL );
  
  printd( "DBG - Total tasks found: %d\n", num_tasks );

  fclose( ini_file );
}

/*===========================================================================

FUNCTION GET_TIME

DESCRIPTION
  This function parses a time entry from the HP Logic Analyzer
  and does the appropriate conversion into milliseconds.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
double get_time( float rtime, char* unit )
{
  if ( !strncmp( unit, "s", 1 ) )
  {
    return( (double) rtime * 1000.0 );
  }
  else if ( !strncmp( unit, "ms", 2 ) )
  {
    return( (double) rtime );
  }
  else if ( !strncmp( unit, "us", 2 ) )
  {
    return( ((double) rtime) / 1000.0 );
  }
  else if ( !strncmp( unit, "ns", 2 ) )
  {
    return( ((double) rtime) / 1000000.0 );
  }
  else
  {
    printf( "ERROR: Found invalid unit (%s)\n", unit );
    exit( 1 );
  }
}

/*===========================================================================

FUNCTION FUNC_DATA_UPDATE

DESCRIPTION
  This function updates the time info on an existing function
  data structure. If the function list does not contain an
  entry for the given address, a new entry is created.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
func_data_type* func_data_update
(
  long f_addr,
  double time_ms
)
{
  func_data_type* itr;
  func_data_type* new_itr;

  /* Check if we already have an entryy for this function */
  for ( itr = func_data; itr != NULL; itr = itr->next )
  {
    if ( itr->f_addr == f_addr )
    {
      itr->f_addr = f_addr;
      itr->t.t_data[itr->t.d_len++] = time_ms;

      if ( itr->t.d_len >= MAX_TIME_DATA )
      {
        printf( "Error: Found more than %d occurences of function 0x%08x\n",
                MAX_TIME_DATA,
                (unsigned int) itr->f_addr );
        exit( 1 );
      }

      return itr;
    }
  }

  /* Appearently not - so add a new one ... */
  new_itr = (func_data_type*) malloc( sizeof( func_data_type ) );
  memset( new_itr, 0, sizeof( func_data_type ) );
  new_itr->f_addr = f_addr;
  new_itr->t.t_data[new_itr->t.d_len++] = time_ms;

  if ( func_data )
  {
    for ( itr = func_data; itr->next != NULL; itr = itr->next );
    new_itr->prev = itr;
    itr->next = new_itr;
  }

  if ( ! func_data ) func_data = new_itr;
  num_funcs++;

  return new_itr;
}

/*===========================================================================

FUNCTION PRINT_FUNCS

DESCRIPTION
  This function is for debug purposes only.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void print_funcs( void )
{
  func_data_type* itr;
  int i = 1;

  printf( "===========================================================\n" );
  for ( itr = func_data; itr != NULL; itr = itr->next )
  {
    if ( itr->f_name )
      printf( "  Func %s (0x%08x) with %d occurences [%d]\n", 
              itr->f_name, (unsigned int) itr->f_addr, itr->t.d_len, i++ );
    else
      printf( "  Func at 0x%08x with %d occurences [%d]\n", 
              (unsigned int) itr->f_addr, itr->t.d_len, i++ );
  }
  printf( "===========================================================\n" );
}

/*===========================================================================

FUNCTION IDENTIFY_LA_MODEL

DESCRIPTION
  This function parses the first data file to identify the output
  format and set the logic analyzer model accordingly.

DEPENDENCIES
  None

PARAMETERS
  file -  Name of input file

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void identify_la_model( char* file )
{
  FILE*         data;
  struct stat   f_props;
  struct tm*    modtime;
  char*         fgets_result;
  char          line[512];
  
  la_model = MODEL_UNKNOWN;
  memset( acquisition_time, 0, 128 );

  data = fopen( file, "r" );

  if ( data == NULL )
  {
    printf( "ERROR: Data file %s not found.\n", file );
    exit( 1 );
  }
  
  do
  {
    memset( line, 0, 512 );
    
    fgets_result = fgets( line, 512, data );

    if ( la_model == MODEL_UNKNOWN )
    {
      /* Test for Model HP 1661 */
      if ( !strncmp( line, "      0    ", 11 ) )
      {
        printf( "Using file format for HP 1661 ...\n" );
        la_model = MODEL_HP1661;
        
        /* Get the timestamp from the end of the file */
      }
      
      /* Test for Model HP 16500 */
      if ( !strncmp( line, "        0  ", 11 ) )
      {
        printf( "Using file format for HP 16500 ...\n" );
        la_model = MODEL_HP16500;
        
        /* Get the timestamp from the end of the file */
      }
      
      /* Test for Model Agilent 16700 */
      if ( !strncmp( line, "Listing(Listing", 15 ) )
      {
        printf( "Using file format for Agilent 16700 ...\n" );
        la_model = MODEL_AG16700;
        
        /* Get the timestamp from the same line */
        strcpy( acquisition_time, fgets_result + 22 );
        
        break;
      }
      
      /* Test for Model Agilent 1694 */
      if ( !strncmp( line, "Time,Sample,Trigger,", 20 ) )
      {
        printf( "Using file format for Agilent 1694 ...\n" );
        la_model = MODEL_AG1694;
        
        /* Get the timestamp from the last write to the file */
        stat( file, &f_props );
        modtime = localtime( &(f_props.st_mtime) );
        sprintf( acquisition_time,
                 " %02d %02d %02d %02d:%02d:%02d\n",
                 modtime->tm_mon + 1,
                 modtime->tm_mday,
                 modtime->tm_year + 1900,
                 modtime->tm_hour,
                 modtime->tm_min,
                 modtime->tm_sec );
        break;
      }
    }

    /* Get the data acquisition timestamp */
    if ( fgets_result && ! strncmp( fgets_result, "Time ", 5 ) )
    {
      strcpy( acquisition_time, fgets_result + 40 );
    }

  } while ( fgets_result != NULL );
  
  fclose( data );

  printd( "DBG - Acquisition Time: %s\n", acquisition_time );

  if ( la_model == MODEL_UNKNOWN )
  {
    printf( "Unknown data file format - exiting ...\n" );
    exit( 1 );
  }
}

/*===========================================================================

FUNCTION READ_DATA

DESCRIPTION
  This function parses the HP data file and stores the time values
  in the internal structure array.

DEPENDENCIES
  None

PARAMETERS
  file -  Name of input file

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void read_data( char* file )
{
  FILE* data;
  char*  fgets_result;
  char   line[512];

  parse_state_type  parse_state = PARSE_NULL_STATE;
  
  int    event_count;
  int    event_id;
  float  rtime;
  char   unit[128];
  int    total_rlp_rx_scale;
  int    old_size;
  int    new_size;
  
  long   current_f_addr;
  double current_time_ms = 0.0;
  double time_ms = 0.0;
  double rlp_time_ms = 0.0;
  int    event_old = 0;
  int    task_old = 0;
  int    isr_cont = 0;
  int    isr_level = 0;
  int    rx_bytes;
  int    tx_bytes;

  int    dummy;
  float  dummyf;
  char   dummys1[128];
  char   dummys2[128];

  #define ps_update( cond, new_state )                                \
    if ( cond ) parse_state = new_state;                              \
    else { printf( "Data parse error at event # %d (data = 0x%x)\n",  \
           event_count, event_id );                                   \
           exit( 1 ); }

  data = fopen( file, "r" );

  if ( data == NULL )
  {
    printf( "ERROR: Data file %s not found.\n", file );
    exit( 1 );
  }
  
  do
  {
    memset( line, 0, 512 );
    memset( unit, 0, 128 );
    
    fgets_result = fgets( line, 512, data );
    
    if ( (((la_model == MODEL_HP1661) || (la_model == MODEL_HP16500))
          &&
          (!strncmp( line, "   ", 3 ) && strncmp( line, "         ", 9 )))
         ||
         (((la_model == MODEL_AG16700) || (la_model == MODEL_AG1694))
          &&
          ((line[0] >=48) && (line[0] <=57))) )
    {
      if ( parse_state == PARSE_NULL_STATE )
      {
        event_count = 0;
        event_id    = 0;
        event_old   = 0;
        task_old    = 0;
        
        if ( func_mode )
        {
          parse_state = PARSE_FTOKEN_STATE;
        }
        else
        {
          parse_state = PARSE_TASKID_STATE;
        }
      }
      else
      {
        switch ( la_model )
        {
          case MODEL_HP1661:
            sscanf( line, "%6d    %6x  %f %s\n", 
                    &event_count,
                    &event_id,
                    &rtime,
                    &unit[0] );
            time_ms = get_time( rtime, unit );
            break;

          case MODEL_HP16500:
            sscanf( line, "   %6d    %5x  %f %s\n", 
                    &event_count,
                    &event_id,
                    &rtime,
                    &unit[0] );
            time_ms = get_time( rtime, unit );
            break;

          case MODEL_AG16700:
            sscanf( line, "%12d    %5x    %2d         %s    %f %s    %f %s\n", 
                    &event_count,
                    &event_id,
                    &dummy,
                    &dummys1[0],
                    &rtime,
                    &unit[0],
                    &dummyf,
                    &dummys2[0]
                    );
            event_id &= 0x80ff;
            time_ms = get_time( rtime, unit );
            break;

          case MODEL_AG1694:
            sscanf( line, "%f,%d,%d,%x,%s\n",
                    &rtime,
                    &event_count,
                    &dummy,
                    &event_id,
                    &dummys1[0] );
            time_ms = (rtime * 1000.0) - total_time;
            break;

          default:
            printf( "Error: Logic Analyzer model not set - exiting ...\n" );
            exit( 1 );
        }

        total_time += time_ms;
        rlp_time_ms += time_ms;
        rlp_last_dtime_ms = rlp_time_ms;

        switch ( parse_state )
        {
          case PARSE_TASKID_STATE:
            /* Check if we got RLP stats inbetween */
            if ( event_id == TT_PARSE_TOKEN_IS856 )
            {
              parse_state = PARSE_RLPTOKEN_NULL_STATE;
              total_rlp_rx_scale = TOTAL_RLP_IS856_RX_SCALE;
              printd( "DBG - Found IS-856 RLP stats @ %d ms\n", 
                      (int) total_time);
              break;
            }
            if ( event_id == TT_PARSE_TOKEN_IS2K )
            {
              parse_state = PARSE_RLPTOKEN_NULL_STATE;
              total_rlp_rx_scale = TOTAL_RLP_IS2K_RX_SCALE;
              printd( "DBG - Found IS-2K RLP stats @ %d ms\n", 
                      (int) total_time );
              break;
            }

            /* Check if we have a nested ISR (msb set) */
            if ( event_id & TT_PARSE_TOKEN_ISR_CONT )
            {
              isr_cont = 1;
              event_id &= 0xff;
              if ( event_id < 0x7f )
              {
                printf( "ISR continuation bit found with Task ID !!\n" );
                printd( "DBG - Task ID 0x%x @ event %d\n",
                        event_id, event_count );
              }
            }
#ifdef TASK_ID_INCLUDES_ISR_LEVEL
            /* Get the nesting level for ISRs */
            isr_level = event_id & TT_PARSE_TOKEN_ISR_LEVEL;

            /* From here on event_id contains the valid task/ISR ID */
            event_id &= 0x00ff;
#endif            
            /* Process the task / ISR id found */
            if ( event_id >= MAX_TASKS )
            {
              printf( "ERROR: Found invalid TASK number (0x%x)\n", event_id );
              exit( 1 );
            }
            
            /* Make sure no entries are missing in file */
            if ( event_count != ++event_old )
            {
              printf( "Warning: Event count off (%d -> %d)\n",
                      --event_old, event_count );
              event_old = event_count;
            }

            /* Ignore subsequent multiple '0' entries (bad samples) */
            if ( event_id == 0 && task_old == 0 ) break;
            
            /* Store the nesting level with the current ID */
            task_data[task_hash[event_id]].t.i_max_nest_level =
              max( isr_level, 
                   task_data[task_hash[event_id]].t.i_max_nest_level );

            /* If this is a continuing ISR measurement,  */
            /* just add it to the previous one.          */
            if ( isr_cont && task_old )
            {
              task_data[task_hash[task_old]].t.d_len--;
              isr_cont = 0;
            }

            /* Store time with previous task number */
            task_data[
              task_hash[task_old]].t.t_data[
                task_data[task_hash[task_old]].t.d_len] += time_ms;

            /* Store the starting time of this slice */
            if ( ! isr_cont )
            {
              task_data[
                task_hash[task_old]].t.r_data[
                  task_data[task_hash[task_old]].t.d_len] = 
              total_time - time_ms;
            }
            
            /* Update the data count and grow the array if necessary */
            task_data[task_hash[task_old]].t.d_len++;
            if ( task_data[task_hash[task_old]].t.d_len
                 >= 
                 task_data[task_hash[task_old]].t.d_size )
            {
              /* Add another increment to the data array */
              old_size = task_data[task_hash[task_old]].t.d_size;
              new_size = old_size + MAX_TIME_DATA;
              task_data[task_hash[task_old]].t.d_size = new_size;
              printd( "DBG - Incrementing buffer of task 0x%02x to %d ...\n",
                      task_hash[task_old],  new_size );

              /* For malloc we need these in byte */
              old_size *= sizeof( double );
              new_size *= sizeof( double );

              /* Re-alloc the nwe space needed for run and total times */
              task_data[task_hash[task_old]].t.t_data =
                realloc( task_data[task_hash[task_old]].t.t_data,
                         task_data[task_hash[task_old]].t.d_size 
                         * sizeof( double ) );
              task_data[task_hash[task_old]].t.r_data =
                realloc( task_data[task_hash[task_old]].t.r_data,
                         task_data[task_hash[task_old]].t.d_size 
                         * sizeof( double ) );

              /* Re-alloc is not guaranteed to work - check it */
              if ( (task_data[task_hash[task_old]].t.t_data == NULL)
                   ||
                   (task_data[task_hash[task_old]].t.r_data == NULL) )
              {
                printf( "Error: Out of memory (Array size = %d).\n",
                        task_data[task_hash[task_old]].t.d_size );
                exit( 1 );
              }
            
              /* Initialize the new part of the array to zero */
              memset( ((char*) task_data[task_hash[task_old]].t.t_data)
                      + old_size,
                      0,
                      new_size - old_size );
              memset( ((char*) task_data[task_hash[task_old]].t.r_data)
                      + old_size,
                      0,
                      new_size - old_size );
            }

            if ( task_old ) total_tasks++;
            
            task_old = event_id;
            event_old = event_count;
            break;

          case PARSE_RLPTOKEN_NULL_STATE:
            ps_update( event_id == 0x0000, PARSE_RLP_STATS_STATE );
            event_old = event_count;
            break;

          case PARSE_RLP_STATS_STATE:
	    if ( testapp_format )
	    {
	      /* Get the testapp logged pkt counts */
	      rx_bytes = ( (event_id & 0xff00) >> 8 ) * 2;
	      tx_bytes = ( (event_id & 0x00ff) >> 0 ) * 2;
	    }
	    else
	    {
	      /* Get the RLP logged byte counts */
	      rx_bytes = ( (event_id & 0xff00) >> 8 ) * total_rlp_rx_scale;
	      tx_bytes = ( (event_id & 0x00ff) >> 0 ) * TOTAL_RLP_TX_SCALE;
	    }
	    /* Get the offset of the first data point */
	    /* and adjust the first interval.         */
	    if ( rlp_first_dtime_ms == 0.0 )
	    {
	      rlp_first_dtime_ms = rlp_time_ms;
	      rx_bytes = rx_bytes * (rlp_time_ms / RLP_LOGGING_INTERVAL);
	      tx_bytes = tx_bytes * (rlp_time_ms / RLP_LOGGING_INTERVAL);
	    }
	    /* Update the RLP throughput and byte counts */
	    if ( rlp_data_count < MAX_RLP_DATA )
            {
	      if ( testapp_format )
	      {
		rlp_rx_data_rate[rlp_data_count] = 
		  min( rx_bytes / (rlp_time_ms / 1.666) * 100.0, 100.0 );
		rlp_tx_data_rate[rlp_data_count] = 
		  min( tx_bytes / (rlp_time_ms / 1.666) * 100.0, 100.0 );
	      }
	      else
	      {
		rlp_rx_data_rate[rlp_data_count] = rx_bytes / rlp_time_ms;
		rlp_tx_data_rate[rlp_data_count] = tx_bytes / rlp_time_ms;
	      }
	      total_rlp_rx_bytes += rx_bytes;
	      total_rlp_tx_bytes += tx_bytes;
	    }
	    else
	    {
	      printf( "RLP Data Overflow - ignoring samples ...\n" );
	    }
	    /* Update the average time between RLP measurements */
	    rlp_avg_dtime_ms = ( (rlp_avg_dtime_ms * rlp_data_count)
				 + rlp_time_ms )
	      / ( rlp_data_count + 1 );
	    
	    rlp_time_ms = 0.0;
	    rlp_data_count++;
	    
            parse_state = PARSE_RLP_STATS_NULL_STATE;
            event_old = event_count;
            break;

          case PARSE_RLP_STATS_NULL_STATE:
            ps_update( event_id == 0x0000, PARSE_TASKID_STATE );
            event_old = event_count;
            break;

          case PARSE_FTOKEN_STATE:
            if ( event_id == PARSE_FTOKEN )
            {
              parse_state = PARSE_FTOKEN_NULL_STATE;
            }
            event_old = event_count;
            break;

          case PARSE_FTOKEN_NULL_STATE:
            ps_update( event_id == 0x0000, PARSE_FADDR_MSW_STATE ); 
            event_old = event_count;
            break;

          case PARSE_FADDR_MSW_STATE:
            /* Account for repeating tokens ... */
            if ( event_id == PARSE_FTOKEN )
            {
              parse_state = PARSE_FTOKEN_NULL_STATE;
            }
            else
            {
              current_f_addr = ((long) event_id) << 16;
              parse_state = PARSE_FADDR_MSW_NULL_STATE;
            }
            break;

          case PARSE_FADDR_MSW_NULL_STATE:
            ps_update( event_id == 0x0000, PARSE_FADDR_LSW_STATE );
            break;

          case PARSE_FADDR_LSW_STATE:
            current_f_addr |= (long) event_id;
            parse_state = PARSE_FADDR_LSW_NULL_STATE;
            break;

          case PARSE_FADDR_LSW_NULL_STATE:
            ps_update( event_id == 0x0000, PARSE_FTOKEN_STATE );

            /* Check if we already have this function and update it */
            /* or create a new entry in the list                    */
            func_data_update( current_f_addr, current_time_ms );
            current_time_ms = time_ms;
            break;

          case PARSE_FTASK_STATE:
            break;

          default:
            printf( "Parse Error (State %d) - exiting.\n",
                    parse_state );
            exit( 1 );
        }
      }
    }
  } while ( fgets_result != NULL );
  
  fclose( data );
}

/*===========================================================================

FUNCTION GET_FUNC_BY_ADDR

DESCRIPTION
  This function resolves the given function address into the
  function name through the build's symbol table.

  NOTE: The entries in the symbol table are off by one byte
        compared to the function pointer itself.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
char* get_func_by_addr( long f_addr )
{
  FILE*         sym_file;
  char          line[256];
  char          sym_name[256];
  unsigned int  sym_addr;
  char*         func_name = NULL;
  char*         fgets_result;

  if ( (sym_file = fopen( symbol_file_name, "r" )) == NULL )
  {
    printf( "Error: Symbol file %s not found - exiting.\n",
            symbol_file_name );
    exit( 1 );
  }

  do
  {
    memset( line, 0, 256 );
    memset( sym_name, 0, 256 );
    sym_addr = 0x00;

    fgets_result = fgets( line, 512, sym_file );
   
    sscanf( line, "%s %x", sym_name, &sym_addr );
   
    if ( sym_addr == (f_addr - 1) )
    {
      func_name = (char*) malloc( strlen( sym_name ) + 1 );
      strcpy( func_name, sym_name );
      break;
    }
  } while( fgets_result != NULL );

  fclose( sym_file );

  return func_name;
}

/*===========================================================================

FUNCTION CALC_STATS

DESCRIPTION
  This function calculates minimum, maximum, average, and percentage
  for the given task- / function entry.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void calc_stats
(
  time_data_type*  time_data
)
{
  int j;
  double t_sum = 0.0;
  double w_sum = 0.0;
  double waittime;

  time_data->t_min = time_data->t_data[0];
  time_data->t_max = 0.0;
  time_data->w_min = 10000.0;
  time_data->w_max = 0.0;
  
  /* figure median and min / max runtime */
  for ( j=0; j<time_data->d_len; j++ )
  {
    time_data->t_total_run += time_data->t_data[j];
    
    time_data->t_min = min( time_data->t_min, time_data->t_data[j] );
    time_data->t_max = max( time_data->t_max, time_data->t_data[j] );
    
    t_sum += time_data->t_data[j] / (double) time_data->d_len;
  }
  time_data->t_median = t_sum;

  /* figure median and min / max wait time */
  for ( j=1; j<time_data->d_len; j++ )
  {
    /* Wait time = this_run_time_start - previous_run_time_start */
    /*             - runtime                                     */
    waittime = max( 0.0,
                    time_data->r_data[j] - time_data->r_data[j-1]
                    - time_data->t_data[j-1] );

    time_data->w_min = min( time_data->w_min, waittime );
    time_data->w_max = max( time_data->w_max, waittime );
    
    w_sum += waittime / (double) (time_data->d_len - 1);
  }
  time_data->w_median = w_sum;
  
  /* figure variance */
  if ( time_data->d_len > 1 )
  {
    t_sum = 0.0;
    
    for ( j=0; j<time_data->d_len; j++ )
    {
      t_sum += 
      pow( time_data->t_data[j] - time_data->t_median, 2 ) 
      / (time_data->d_len - 1);
    }
    time_data->t_variance = t_sum;
  }
  
  /* figure percent of total time */
  if ( total_time )
  {
    time_data->t_percent = time_data->t_total_run 
    / total_time * 100.0;
  }
}

/*===========================================================================

FUNCTION SORT_ORDER

DESCRIPTION
  This function is used by qsort to sort by CPU utilization.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int sort_order( const void* a1, const void* a2 )
{
  return ( (int)( ((*((time_data_type**) a2))->t_percent 
                   - (*((time_data_type**) a1))->t_percent) * 100) );
}

/*===========================================================================

FUNCTION GET_STATS

DESCRIPTION
  This function calculates the timing statistics on the task and / or
  function list and resolves function addresses into their names.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void get_stats( void )
{
  int i;
  int index1;
  int index2;
  func_data_type* itr;
  time_data_type* t_ptr;
  
  /* Calculate the task statictics */
  task_data_sorted = (task_data_type**) malloc( 
                       sizeof( task_data_type* ) * num_tasks );
  for ( i=1; i<num_tasks; i++ )
  {
    calc_stats( &(task_data[i].t) );
    task_data_sorted[i-1] = &(task_data[i]);
  }

  /* Don't include the '0' task */
  num_tasks_sorted = num_tasks - 1;
  
  /* Get the total data transmitted / received rates in kBps */
  if ( testapp_format )
  {
    total_rlp_tx_rate = total_rlp_tx_bytes / 
                          ((total_time - rlp_last_dtime_ms) / 1.6) * 100.0;
    total_rlp_rx_rate = total_rlp_rx_bytes / 
                          ((total_time - rlp_last_dtime_ms) / 1.6) * 100.0;
  }
  else
  {
    total_rlp_tx_rate = total_rlp_tx_bytes / (total_time - rlp_last_dtime_ms);
    total_rlp_rx_rate = total_rlp_rx_bytes / (total_time - rlp_last_dtime_ms);
  }

  qsort( (void*) task_data_sorted, 
         num_tasks_sorted, 
         sizeof( task_data_type* ), 
         sort_order ); 

  /* Calculate the CPU usage graph w/ a 10 ms resolution */
  cpu_data_size = (total_time / CPU_SAMPLE_INTERVAL) + 1;
  cpu_data = (double*) malloc( sizeof( double ) * cpu_data_size );
  memset( cpu_data, 0, sizeof( double ) * cpu_data_size );

  t_ptr = &(task_data[task_hash[TIMETEST_SLEEP_TASK_ID]].t);
  for ( i=0;  i<t_ptr->d_len; i++ )
  {
    index1 = (int)(t_ptr->r_data[i] / CPU_SAMPLE_INTERVAL);
    index2 = (int)((t_ptr->r_data[i] + t_ptr->t_data[i]) 
                   / CPU_SAMPLE_INTERVAL);

    /* Check if the the run interval spans two sample intervals */
    if ( index2 > index1 )
    {
      cpu_data[index1] += ((CPU_SAMPLE_INTERVAL * index2) - t_ptr->r_data[i]);
      cpu_data[index2] += ((t_ptr->r_data[i] + t_ptr->t_data[i]) 
                           - (CPU_SAMPLE_INTERVAL * index2));
    }
    else
    {
      cpu_data[index1] += t_ptr->t_data[i];
    }
  }

  /* Normalize the data and get cpu utilization (100 % - sleep) */
  for ( i=0; i<cpu_data_size; i++ )
  {
    cpu_data[i] = 100.0 - ((cpu_data[i] / CPU_SAMPLE_INTERVAL) * 100.00);
  }

  /* Calculate the function statictics */
  func_data_sorted = (func_data_type**) malloc( 
                       sizeof( func_data_type* ) * num_funcs );
  memset( func_data_sorted, 0, sizeof( func_data_type* ) * num_funcs );
  i = 0;
  if ( num_funcs ) printf( "Resolving function names " );
  for ( itr = func_data; itr != NULL; itr = itr->next )
  {
    printf( "." );
    fflush( stdout );
    itr->f_name = get_func_by_addr( itr->f_addr );
    calc_stats( &(itr->t) );
    func_data_sorted[i++] = itr;
  }
  printf( "\n" );
  
  qsort( (void*) func_data_sorted, 
         num_funcs, 
         sizeof( func_data_type* ), 
         sort_order );
}

/*===========================================================================

FUNCTION SHOW_STATS

DESCRIPTION
  This function prints the results.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void show_stats( void )
{
  func_data_type*  itr;
  int              i;
  int              num_list;
  int              no_isrs = 0;
 
  if ( func_mode )
    num_list = min( num_funcs, max_results );
  else
    num_list = min( num_tasks, max_results );

  printf( "-------------------------------------------------------------------------------\n" );
  if ( func_mode )
    printf( "| Function                    CPU Usage    Average      Minimum       Maximum |\n" );
  else
    printf( "| Task/ISR                 CPU Usage     Average        Minimum       Maximum |\n" );
  printf( "-------------------------------------------------------------------------------\n" );
  
  if ( func_mode )
  {
    /* for ( itr = func_data; itr != NULL; itr = itr->next ) */
    for ( i=0; i<num_funcs; i++ )
    {
      char name[23];
      memset( name, 0, 23 );
      
      itr = func_data_sorted[i];

      if ( itr->t.d_len )
      {
        if ( itr->f_name )
        {
          strncpy( name, itr->f_name, 22 );
          strncat( name, "                    ", max( 0, 22-strlen( name ) ) );
        }
        else
        {
          sprintf( name, "UF (0x%08x)      ", (unsigned int) itr->f_addr );
        }
        
        printf( "%s (%4d): %5.2f %%   %8.3f ms  %8.3f ms  %8.3f ms\n", 
                name, 
                itr->t.d_len,
                itr->t.t_percent,
                itr->t.t_median,
                itr->t.t_min,
                itr->t.t_max );
      }
    }
  }
  else
  {
    /* Check if we need to omit individual ISR's */
    no_isrs = (task_data[task_hash[0x7f]].t.d_len > 0);

    for ( i=0; i<num_tasks_sorted; i++ )
    {
      char name[14];
      memset( name, 0, 14 );
      
      if ( (! no_isrs
            ||
            (no_isrs && strncmp( task_data_sorted[i]->t_name, "I_", 2 )))
           &&
           (task_data_sorted[i]->t.d_len) )
      {
        strncpy( name, task_data_sorted[i]->t_name, NAME_MAX_CHAR );
        strncat( name, "              ", max( 0, NAME_MAX_CHAR-strlen( name ) ) );
        
        printf( 
          "%s (%4d occ.): %5.2f %%    %8.3f ms    %8.3f ms   %8.3f ms\n", 
          name, 
          task_data_sorted[i]->t.d_len,
          task_data_sorted[i]->t.t_percent,
          task_data_sorted[i]->t.t_median,
          task_data_sorted[i]->t.t_min,
          task_data_sorted[i]->t.t_max );
      }
    } 
    
    printf( "-------------------------------------------------------------------------------\n" );
    printf( "| %8d total measured Context Switches in %9.2f ms                    |\n",
            total_tasks, total_time );
  }
  if ( (total_rlp_tx_rate > 0.0) || (total_rlp_rx_rate > 0.0) )
  {
    printf( "|   RLP Rx rate: %8.3f kBps, Tx rate: %8.3f kBps  (Total Rx: %4d kB)   |\n",
            total_rlp_rx_rate,
            total_rlp_tx_rate,
            total_rlp_rx_bytes / 1000 );
  }

  printf( "-------------------------------------------------------------------------------\n" );
}

/*===========================================================================

FUNCTION BUILD_FILE_NAME

DESCRIPTION
  This function takes a file prefix and make it unique by 
  appending the data collection time to it.

DEPENDENCIES
  None

PARAMETERS
  file_prefix  - The preix part of the file name

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void build_file_name( char* file_prefix )
{
  char*  time_start;
  int    i;

  if ( (la_model == MODEL_HP1661) || (la_model == MODEL_HP16500) )
  {
    time_start = strstr( acquisition_time, ":" );
  }
  else
  {
    time_start = acquisition_time;
  }

  if ( time_start )
  { 
    /* Adjust the beginning and cut the EOL (only the first time !) */
    time_start++;
    if ( time_start[strlen( time_start )-1] == '\n' )
      time_start[strlen( time_start )-1] = '\0';

    /* Make sure DOS and UNIX can read this file name */
    strcat( file_prefix, "_" );
    strcat( file_prefix, time_start );
    for ( i=0; i<strlen( file_prefix ); i++ )
    {
      if ( file_prefix[i] == ' ' ) file_prefix[i] = '_';
      if ( file_prefix[i] == '(' ) file_prefix[i] = '_';
      if ( file_prefix[i] == ')' ) file_prefix[i] = '_';
      if ( file_prefix[i] == ':' ) file_prefix[i] = '-';
    }
  }
}

/*===========================================================================

FUNCTION WRITE_STATS_HTML

DESCRIPTION
  This function prints the results.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void write_html_stats
(
  char* data_file_ptr,
  int   printable
)
{
  FILE*            hfile = NULL;
  FILE*            tfile = NULL;
  char             html_file[192];
  char             print_file[128];
  char             task_file[192];
  char             color[8];
  int              i, j;
  int              num_list;
  int              no_isrs = 0;
  int              count = 0;

  memset( html_file, 0, 192 );
  memset( task_file, 0, 192 );

  if(!output_prefix_given)
  {
    /* Find the acquisition time stamp from the last line in the data file */
    for ( i=strlen( data_file_ptr ); i>=0 && data_file_ptr[i]!='.'; i-- );
    if ( i == 0 ) i = strlen( data_file_ptr );
    strncpy( html_file, data_file_ptr, min( i, 128 ) );
    build_file_name( html_file );
  }
  else
  {
    strncpy( html_file, output_file_prefix,
             min( strlen(output_file_prefix), 128 ) );
  }

  strcpy( task_file, html_file );

  if ( printable )
  {
    strcat( html_file, ".p.html" );
  }
  else
  {
    memset( print_file, 0, 128 );
    for ( i=strlen( html_file )-1; i>=0; i-- )
    {
      if ( (html_file[i] == '/') || (html_file[i] == '\\') )
      {
        break;
      }
      strcpy( print_file, html_file + i );
    }
    strcat( html_file, ".html" );
    strcat( task_file, "_task.html" );

    strcpy( html_start_file, html_file );

    if ( (tfile = fopen( task_file, "w" )) == NULL ) 
    {
      printf( "ERROR: Could not open HTML Task file %s.\n", html_file );
      return;
    }
  }

  if ( (hfile = fopen( html_file, "w" )) == NULL ) 
  {
    printf( "ERROR: Could not open HTML file %s.\n", html_file );
    return;
  }

  num_list = min( num_tasks_sorted, max_results );

  /* HTML header info */
  fprintf( hfile, "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 3.2//EN'>\n\n" );
  fprintf( hfile, "<html><head>\n" );
  fprintf( hfile, "<title>HP-Stats Results [%s]</title>\n", html_file );
  fprintf( hfile, "  <style TYPE='text/css'>\n" );
  fprintf( hfile, "    .newpage {page-break-before: always}\n" );
  fprintf( hfile, "  </style>\n" );
  fprintf( hfile, "</head><body bgcolor=#f0f0f0>\n" );

  /* Overall heading */
  fprintf( hfile, "<h2><center><font color=#1010a0>Profiling results for [%s]</font></center></h2>\n", html_file );
  if ( ! printable )
  {
    fprintf( hfile, "<br><center>Click <a href='%s.p.html'>here</a> for a printable version.</center><br>\n", print_file );
  }
  /* Summary table */
  if ( testapp_format )
    fprintf( hfile, "<h3>1. Profiling Summary (IS-890 mode)</h3>\n" );
  else
    fprintf( hfile, "<h3>1. Profiling Summary</h3>\n" );
  if ( printable )
  {
    fprintf( hfile, "The following table shows a summary of the profiling run:<br><br>\n" );
  }
  fprintf( hfile, "<table border=2 bgcolor=#e0e0e0>\n" );
  if ( testapp_format )
  {
    fprintf( hfile, "<tr><td align=center><b>CPU Utilization</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Duration</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Forward Link Utilization</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Context Switches</b></td>\n" );
    fprintf( hfile, "</tr>\n" );
    fprintf( hfile, 
             "<tr><td align=center>%5.2f %%</td>\n",
             100.0 - task_data[task_hash[TIMETEST_SLEEP_TASK_ID]].t.t_percent );
    fprintf( hfile, 
             "    <td align=center>%9.3f sec</td>\n",
             total_time / 1000.0 );
    fprintf( hfile, 
             "    <td align=center>%9.2f %%</td>\n",
             total_rlp_rx_rate );
    fprintf( hfile, 
             "    <td align=center>%d</td>\n",
             total_tasks );
    fprintf( hfile, "</tr>\n" );
  }
  else
  {
    fprintf( hfile, "<tr><td align=center><b>CPU Utilization</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Duration</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Total Data Transferred</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Total RLP Throughput</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Context Switches</b></td>\n" );
    fprintf( hfile, "</tr>\n" );
    fprintf( hfile, 
             "<tr><td align=center>%5.2f %%</td>\n",
             100.0 - task_data[task_hash[TIMETEST_SLEEP_TASK_ID]].t.t_percent );
    fprintf( hfile, 
             "    <td align=center>%9.3f sec</td>\n",
             total_time / 1000.0 );
    fprintf( hfile, 
             "    <td align=center>%9.3f kB</td>\n",
             (total_rlp_tx_bytes + total_rlp_rx_bytes) / 1000.0 );
    fprintf( hfile, 
             "    <td align=center>%9.3f kBps</td>\n",
             total_rlp_tx_rate + total_rlp_rx_rate );
    fprintf( hfile, 
             "    <td align=center>%d</td>\n",
             total_tasks );
    fprintf( hfile, "</tr>\n" );
  }
  fprintf( hfile, "</table>\n" );
  if ( printable )
  {
    fprintf( hfile, "<ol><ol><b>Table 1: <i>Profiling Summary</i></b></ol></ol><br>\n" );
    fprintf( hfile, "<i>Section 2</i> shows the detailed results for CPU utilization sorted by activity, \n" );
    fprintf( hfile, "and the run times of the individual tasks and interrupts with their average, minimum, and maximum values. A maximum task run time exceeding 5 ms implies an issue with INTLOCK's by that task as the 5 ms timer tick interrupt should have been scheduled inbetween. Similarly, a maximum interrupt runtime exceeding 1 ms implies overall latency problems in DMSS and should be investigated.<br>\n" );
    fprintf( hfile, "The wait times in <i>Table 3</i> show the average, minimum, and maximum intervals between slices granted to the task / interrupt. The maximum wait timw for known periodic interrupt / tasks should be checked for sanity.<br><br>\n" );
    fprintf( hfile, "<i>Section 3</i> shows the CPU utilization detailed over time using a %d ms sampling interval.<br><br>\n", CPU_SAMPLE_INTERVAL );
    fprintf( hfile, "<i>Section 4</i> shows the RLP throughput with 0.5 sec sample intervals. Both transmission and retransmissions are counted.<br><br>\n" );
    fprintf( hfile, "<i>Section 5</i> gives an indication for CPU overload conditions. The 5 ms timer tick interrupt should be scheduled within every 5 &plusmn; 1 ms for proper operation.\n" );
  }

  /* Individual tasks table */
  fprintf( hfile, "<h3 CLASS=NEWPAGE>2. Task CPU Utilization Data</h3>\n" );
  fprintf( hfile, "<table border=2 bgcolor=#e0e0e0>\n" );
  fprintf( hfile, "<tr><td align=center><b>Task / ISR</b></td>\n" );
  fprintf( hfile, "    <td align=center><b>Occurences</b></td>\n" );
  fprintf( hfile, "    <td align=center bgcolor=#c0c0c0><b>CPU Usage</b></td>\n" );
  fprintf( hfile, "    <td align=center bgcolor=#a0f0a0><b>Average RT</b></td>\n" );
  fprintf( hfile, "    <td align=center><b>Minimum RT</b></td>\n" );
  fprintf( hfile, "    <td align=center><b>Maximum RT</b></td>\n" );
  if ( ! printable )
  {
    fprintf( hfile, "    <td align=center bgcolor=#f0a0a0><b>Average WT</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Minimum WT</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Maximum WT</b></td>\n" );
  }
  fprintf( hfile, "</tr>\n" );

  /* Check if we need to omit individual ISR's */
  no_isrs = (task_data[task_hash[0x7f]].t.d_len > 0);
  
  for ( i=0; i<num_tasks_sorted; i++ )
  {
    char name[14];

    memset( name, 0, 14 );
    
    if ( (! no_isrs
          ||
          (no_isrs && strncmp( task_data_sorted[i]->t_name, "I_", 2 )))
         &&
         (task_data_sorted[i]->t.d_len) )
    {
      strncpy( name, task_data_sorted[i]->t_name, NAME_MAX_CHAR );
      strncat( name, "              ", max( 0, NAME_MAX_CHAR-strlen( name ) ) );

      if ( ! strncmp( name, "Sleep", 5 ) )
      {
        /* Sleep task in green */
        strcpy( color, "10a010" );
      }
      else
      {
        if ( (! strncmp( name, "I_", 2 )
              &&
              (task_data_sorted[i]->t.t_max > 1.5))
             ||
             (strncmp( name, "I_", 2 )
              &&(task_data_sorted[i]->t.t_max > 5.0)) )
        {
          /* Too long ISR or Task run */
          strcpy( color, "a01010" );
        }
        else
        {
          /* Normal entry */
          strcpy( color, "202020" );
        }
      }
      
      /* Print the table cells */
      fprintf( hfile, 
               "<tr><td align=left><font color=#%s>%s</font></td>\n",
               color,
               name );
      fprintf( hfile, 
               "   <td align=right><font color=#%s>%d</font></td>\n",
               color,
               task_data_sorted[i]->t.d_len );
      fprintf( hfile, 
               "    <td align=right bgcolor=#c0c0c0><font color=#%s>%5.2f %%</font></td>\n",
               color,
               task_data_sorted[i]->t.t_percent );
      fprintf( hfile, 
               "    <td align=right bgcolor=#a0f0a0><font color=#%s>%8.3f ms</font></td>\n",
               color,
               task_data_sorted[i]->t.t_median );
      fprintf( hfile, 
               "    <td align=right><font color=#%s>%8.3f ms</font></td>\n",
               color,
               task_data_sorted[i]->t.t_min );
      fprintf( hfile, 
               "    <td align=right><font color=#%s>%8.3f ms</font></td>\n",
               color,
               task_data_sorted[i]->t.t_max );
      if ( ! printable )
      {
        fprintf( hfile, 
                 "    <td align=right bgcolor=#f0a0a0><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_median );
        fprintf( hfile, 
                 "    <td align=right><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_min );
        fprintf( hfile, 
                 "    <td align=right><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_max );
      }
      fprintf( hfile, "</tr>\n" );
      
    }
  }

  /* Print the end-of-table */
  fprintf( hfile, "</table>\n" );
  if ( printable )
    fprintf( hfile, "<ol><ol><b>Table 2: <i>Task Run Times</i></b></ol></ol><br>\n" );

  /* For printed output the above table is split in two */
  if ( printable )
  {
    /* Repeat the wait times in a separate table */
    fprintf( hfile, "<table border=2 bgcolor=#e0e0e0 CLASS=NEWPAGE>\n" );
    fprintf( hfile, "<tr><td align=center><b>Task / ISR</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Occurences</b></td>\n" );
    fprintf( hfile, "    <td align=center bgcolor=#c0c0c0><b>CPU Usage</b></td>\n" );
    fprintf( hfile, "    <td align=center bgcolor=#f0a0a0><b>Average WT</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Minimum WT</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Maximum WT</b></td>\n" );
    fprintf( hfile, "</tr>\n" );
    
    /* Check if we need to omit individual ISR's */
    no_isrs = (task_data[task_hash[0x7f]].t.d_len > 0);
    
    for ( i=0; i<num_tasks_sorted; i++ )
    {
      char name[14];
      
      memset( name, 0, 14 );
      
      if ( (! no_isrs
            ||
            (no_isrs && strncmp( task_data_sorted[i]->t_name, "I_", 2 )))
           &&
           (task_data_sorted[i]->t.d_len) )
      {
        strncpy( name, task_data_sorted[i]->t_name, NAME_MAX_CHAR );
        strncat( name, "              ", max( 0, NAME_MAX_CHAR-strlen( name ) ) );
        
        if ( ! strncmp( name, "Sleep", 5 ) )
        {
          /* Sleep task in green */
          strcpy( color, "10a010" );
        }
        else
        {
          if ( (! strncmp( name, "I_", 2 )
                &&
                (task_data_sorted[i]->t.t_max > 1.5))
               ||
               (strncmp( name, "I_", 2 )
                &&(task_data_sorted[i]->t.t_max > 5.0)) )
          {
            /* Too long ISR or Task run */
            strcpy( color, "a01010" );
          }
          else
          {
            /* Normal entry */
            strcpy( color, "202020" );
          }
        }
      
        /* Print the table cells */
        fprintf( hfile, 
                 "<tr><td align=left><font color=#%s>%s</font></td>\n",
                 color,
                 name );
        fprintf( hfile, 
                 "   <td align=right><font color=#%s>%d</font></td>\n",
                 color,
                 task_data_sorted[i]->t.d_len );
        fprintf( hfile, 
                 "    <td align=right bgcolor=#c0c0c0><font color=#%s>%5.2f %%</font></td>\n",
                 color,
                 task_data_sorted[i]->t.t_percent );
        fprintf( hfile, 
                 "    <td align=right bgcolor=#f0a0a0><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_median );
        fprintf( hfile, 
                 "    <td align=right><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_min );
        fprintf( hfile, 
                 "    <td align=right><font color=#%s>%7.2f ms</font></td>\n",
                 color,
                 task_data_sorted[i]->t.w_max );
        fprintf( hfile, "</tr>\n" );    
      }
    }
    
    /* Print the end-of-table */
    fprintf( hfile, "</table>\n" );
    if ( printable )
      fprintf( hfile, "<ol><ol><b>Table 3: <i>Task Waiting Times</i></b></ol></ol><br>\n" );
  }
  
  /* CPU utilization data graph */
  fprintf( hfile, 
           "<h3 CLASS=NEWPAGE>3. CPU Utilization (%d ms samples)</h3>\n",
           CPU_SAMPLE_INTERVAL );
  if ( cpu_data_size )
  {
    if ( ! text_only_mode )
    {
      fprintf( hfile, 
               "<center><img src='%s'></center>\n", 
               cpu_file_reference );
    }
  }
  else
  {
    fprintf( hfile, "<i>CPU utilization graph is not available</i>\n" );
  }
  if ( printable ) fprintf( hfile, "<br>\n" );

  /* RLP data table */
  if ( testapp_format )
    fprintf( hfile, "<h3 CLASS=NEWPAGE>4. TestApp Link Utilization</h3>\n" );
  else
    fprintf( hfile, "<h3 CLASS=NEWPAGE>4. RLP Throughput Data</h3>\n" );
  if ( rlp_data_count )
  {
    fprintf( hfile, "<table border=0 cellpadding=10 align=middle>\n" );
    fprintf( hfile, "<tr><td>\n" );
    fprintf( hfile, "<table border=2 bgcolor=#e0e0e0>\n" );
    fprintf( hfile, "<tr><td align=center><b>Time [sec]</b></td>\n" );
  if ( testapp_format )
  {
    fprintf( hfile, "    <td align=center><b>Fwd Link [%%]</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Rev Link [%%]</b></td>\n" );
  }
  else
  {
    fprintf( hfile, "    <td align=center><b>Rx Rate [kBps]</b></td>\n" );
    fprintf( hfile, "    <td align=center><b>Tx Rate [kBps]</b></td>\n" );
  }
    fprintf( hfile, "</tr>\n" );
    
    for ( i=0; i<rlp_data_count; i++ )
    {
      fprintf( hfile, 
               "<tr><td align=center> %8.3f </td>\n",
               ( i * rlp_avg_dtime_ms + rlp_first_dtime_ms ) / 1000.0 );
      fprintf( hfile, 
	       "    <td align=center> %9.3f </td>\n",
	       rlp_rx_data_rate[i] );
      fprintf( hfile, 
	       "    <td align=center> %9.3f </td>\n",
	       rlp_tx_data_rate[i] );
      fprintf( hfile, "</tr>\n" );
    }
    
    if ( ! testapp_format )
    {
      fprintf( hfile, 
	       "<tr><td align=center><b>Average</b></td>\n" );
      fprintf( hfile, 
	       "    <td align=center><b> %9.3f kBps </b></td>\n",
	       total_rlp_rx_rate );
      fprintf( hfile, 
	       "    <td align=center><b> %9.3f kBps </b></td>\n",
	       total_rlp_tx_rate );
      fprintf( hfile, "</tr>\n" );
      fprintf( hfile, 
	       "<tr><td align=center></td>\n" );
      fprintf( hfile, 
	       "    <td align=center><b> %9.3f kbps</b></td>\n",
	       total_rlp_rx_rate * 8 );
      fprintf( hfile, 
	       "    <td align=center><b> %9.3f kbps</b></td>\n",
	       total_rlp_tx_rate * 8 );
      fprintf( hfile, "</tr>\n" );
    }

    /* Print the end-of-table */
    fprintf( hfile, "</table>\n" );
    if ( printable )
    {
      if ( testapp_format )
        fprintf( hfile, 
		 "<ol><b>Table 4: <i>TestApp Link Utilization</i></b></ol><br>\n" );
      else
        fprintf( hfile, 
		 "<ol><b>Table 4: <i>RLP Data Rates</i></b></ol><br>\n" );
    }

    /* Add the RLP graph */
    fprintf( hfile, "</td><td>\n" );
    if ( ! text_only_mode )
    {
      fprintf( hfile, "<img src='%s'>\n", rlp_file_reference );
    }
    fprintf( hfile, "</td></tr>\n" );
    fprintf( hfile, "</table>\n" );
  }
  else
  {
    fprintf( hfile, "<ol>\n" );
    fprintf( hfile, 
             "<i>RLP throughput data is not available for this run.</i>\n" );
    fprintf( hfile, "</ol><br>\n" );
  }

  /* Overload (timer tick data table) */
  fprintf( hfile, "<h3 CLASS=NEWPAGE>5. CPU Overload (5 ms Timer Tick Latency)</h3>\n" );
  fprintf( hfile, "<table border=0 cellpadding=10 align=middle>\n" );
  fprintf( hfile, "<tr><td>\n" );
  fprintf( hfile, "<table border=2 bgcolor=#e0e0e0>\n" );
  fprintf( hfile, "<tr><td align=center><b>Scheduling Interval</b></td>\n" );
  fprintf( hfile, "    <td align=center><b>Occurences</b></td>\n" );
  fprintf( hfile, "</tr>\n" );

  for ( i=0; i<=((int) task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.w_max)+2; i++ )
  {
    count = 0;
    for ( j=1; j<task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.d_len; j++ )
    {
      if ( ((task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[j]
             - task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[j-1])
            >= ((float) i - 0.5))
           &&
           ((task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[j]
             - task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[j-1])
            < ((float) i + 0.5)) )
      count++;
    }
    if ( i == 3 || i == 7 ) strcpy( color, "#c8f0a0" );
    else if ( i == 4 || i == 6 ) strcpy( color, "#80c080" );
    else if ( i == 5 ) strcpy( color, "#40a040" );
    else strcpy( color, "#f0c0c0" );
    fprintf( hfile, 
             "<tr><td align=center bgcolor=%s>%4.1f - %4.1f ms</td>\n", 
             color,
             max( ((float) i - 0.5), 0.0 ), 
             ((float) i + 0.5) );
    fprintf( hfile, 
             "    <td align=center bgcolor=%s>%d</td>\n", 
             color,
             count );
    fprintf( hfile, "</tr>\n" );
  }

  /* Print the end-of-table */
  fprintf( hfile, "</table>\n" );
  if ( printable )
    fprintf( hfile, "<ol><b>Table 5: <i>Scheduling Latency</i></b></ol><br>\n" );

  /* Add the latency graph */
  fprintf( hfile, "</td><td>\n" );
  if ( ! text_only_mode )
  {
    fprintf( hfile, "<img src='%s'>\n", latency_file_reference );
  }
  fprintf( hfile, "</td></tr>\n" );
  fprintf( hfile, "</table>\n" );

  /* For the online version give a link to the task data */
  if ( ! printable )
  {
    fprintf( hfile, "<h3 CLASS=NEWPAGE>6. Task Scheduling</h3>\n" );
    fprintf( hfile, "For a detailed graph of scheduling times granted to the 20 most busy tasks / interrupts, please click <a href='%s' target=_new>here</a>.\n",
             task_file_reference );
  }

  /* Print the end-of-page */
  fprintf( hfile, "<br><br><br><hr><i>Copyright 2000, 2001 by Qualcomm Inc.</i><br><br>\n" );
  fprintf( hfile, "</body></html>\n" );

  fclose( hfile );

  if ( ! printable )
  {
    /* HTML header info */
    fprintf( tfile, "<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 3.2//EN'>\n\n" );
    fprintf( tfile, "<html><head>\n" );
    fprintf( tfile, 
             "<title>HP-Stats Task Scheduling [%s]</title>\n", 
             html_file );
    fprintf( tfile, "</head><body bgcolor=#f0f0f0>\n" );
    
    fprintf( tfile, "<img src='%s'>\n", task_file_img_reference );
    
    /* Print the end-of-page */
    fprintf( tfile, "</body></html>\n" );
    
    fclose( tfile );
  }

  /* Let the user know we are still here ... */
  printf( "." );
  fflush( stdout );
}


/*===========================================================================

FUNCTION WRITE_HTML_PLOTS

DESCRIPTION
  This function creates plots from the profiling data using gnuplot..

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void write_html_plots
(
  char* data_file_ptr
)
{
  FILE*            file = NULL;
  char             data_file_prefix[128];
  char             cpu_file[192];
  char             rlp_file[192];
  char             latency_file[192];
  char             task_file[192];
  char             gnu_cmd_file[192];
  char             cpu_dat_file[192];
  char             rlp_rx_dat_file[192];
  char             rlp_tx_dat_file[192];
  char             latency_dat_file[192];
  char             task_dat_file[MAX_TASKS][192];
  char             print_str[512];
  int              i, j, k;
  double           rlp_max_rate = 0.0;

  memset( data_file_prefix, 0, 128 );

  if(!output_prefix_given)
  {
    /* Find the acquisition time stamp from the last line in the data file */
    for ( i=strlen( data_file_ptr ); i>=0 && data_file_ptr[i]!='.'; i-- );
    if ( i == 0 ) i = strlen( data_file_ptr );
    strncpy( data_file_prefix, data_file_ptr, min( i, 128 ) );

    build_file_name( data_file_prefix );
  }
  else
  {
    strncpy( data_file_prefix, output_file_prefix,
             min( strlen(output_file_prefix), 128 ) );
  }

  memset( cpu_dat_file, 0, 192 );
  memset( rlp_rx_dat_file, 0, 192 );
  memset( rlp_tx_dat_file, 0, 192 );
  memset( latency_dat_file, 0, 192 );
  memset( task_dat_file, 0, 192 * MAX_TASKS );
  memset( rlp_file, 0, 192 );
  memset( latency_file, 0, 192 );
  memset( task_file, 0, 192 );
  memset( rlp_file_reference, 0, 128 );
  memset( latency_file_reference, 0, 128 );
  memset( task_file_reference, 0, 128 );
  memset( task_file_img_reference, 0, 128 );
  memset( gnu_cmd_file, 0, 192 );

  sprintf( cpu_dat_file, "%s_cpu.dat", data_file_prefix );
  sprintf( rlp_rx_dat_file, "%s_rlprx.dat", data_file_prefix );
  sprintf( rlp_tx_dat_file, "%s_rlptx.dat", data_file_prefix );
  sprintf( latency_dat_file, "%s_latency.dat", data_file_prefix );
  for ( i=0; i<num_tasks_sorted; i++ )
  {
    sprintf( task_dat_file[i], 
             "%s_task_0x%02x.dat", 
             data_file_prefix,
             task_data_sorted[i]->task_id );
  }

  for ( i=strlen( data_file_prefix )-1; i>=0; i-- )
  {
    if ( (data_file_prefix[i] == '/') || (data_file_prefix[i] == '\\') )
    {
      break;
    }
    strcpy( cpu_file_reference, data_file_prefix + i );
    strcpy( rlp_file_reference, data_file_prefix + i );
    strcpy( latency_file_reference, data_file_prefix + i );
    strcpy( task_file_reference, data_file_prefix + i );
    strcpy( task_file_img_reference, data_file_prefix + i );
  }
  strcat( cpu_file_reference, "_cpu.gif" );
  strcat( rlp_file_reference, "_rlp.gif" );
  strcat( latency_file_reference, "_latency.gif" );
  strcat( task_file_img_reference, "_task.gif" );
  strcat( task_file_reference, "_task.html" );
  sprintf( cpu_file, "%s_cpu.gif", data_file_prefix );
  sprintf( rlp_file, "%s_rlp.gif", data_file_prefix );
  sprintf( latency_file, "%s_latency.gif", data_file_prefix );
  sprintf( task_file, "%s_task.gif", data_file_prefix );

  sprintf( gnu_cmd_file, "%s_gplot.gpl", data_file_prefix );

  /* Write the cpu utilization data file */
  if ( (file = fopen( cpu_dat_file, "w" )) == NULL )
  {
    text_only_mode = 1;
    printf( "ERROR: could not write CPU data file %s\n", cpu_dat_file );
    return;
  }

  for ( i=0; i<cpu_data_size-1; i++ )
  {
    fprintf( file, 
             "%9.3f    %9.3f\n",
             ((float) i) * CPU_SAMPLE_INTERVAL / 1000.0, 
             cpu_data[i] );
  }

  fclose( file );

  /* Write the rlp data file */
  if ( (file = fopen( rlp_rx_dat_file, "w" )) == NULL )
  {
    text_only_mode = 1;
    printf( "ERROR: could not write RLP data file %s\n", rlp_rx_dat_file );
    return;
  }

  for ( i=0; i<rlp_data_count; i++ )
  {
    fprintf( file, 
             "%8.3f    %9.3f\n",
             ( i * rlp_avg_dtime_ms + rlp_first_dtime_ms ) / 1000.0,
             rlp_rx_data_rate[i] );
    rlp_max_rate = max( rlp_rx_data_rate[i], rlp_max_rate );
  }

  fclose( file );
  
  if ( (file = fopen( rlp_tx_dat_file, "w" )) == NULL )
  {
    text_only_mode = 1;
    printf( "ERROR: could not write RLP data file %s\n", rlp_tx_dat_file );
    return;
  }

  for ( i=0; i<rlp_data_count; i++ )
  {
    fprintf( file, 
             "%8.3f    %9.3f\n",
             ( i * rlp_avg_dtime_ms + rlp_first_dtime_ms ) / 1000.0,
             rlp_tx_data_rate[i] );
    rlp_max_rate = max( rlp_tx_data_rate[i], rlp_max_rate );
  }

  rlp_max_rate *= 1.1;

  fclose( file );
  
  /* Let the user know we are still here ... */
  printf( "." );
  fflush( stdout );

  /* Write the latency data file */
  if ( (file = fopen( latency_dat_file, "w" )) == NULL )
  {
    text_only_mode = 1;
    printf( "ERROR: could not write latency data file %s\n", 
            latency_dat_file );
    return;
  }

  for ( i=1; i<task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.d_len; i++ )
  {
    fprintf( file, "%9.3f    %9.3f\n",
             task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[i] / 1000.0,
             task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[i]
             - task_data[task_hash[TIMETEST_TIMER_TICK_ID]].t.r_data[i-1] );
  }

  fclose( file );
           
  /* Let the user know we are still here ... */
  printf( "." );
  fflush( stdout );

  /* Write the task data files */
  for ( i=0; i<min(num_tasks_sorted, 20); i++ )
  {
    if ( task_data_sorted[i]->t.d_len )
    {
      if ( (file = fopen( task_dat_file[i], "w" )) == NULL )
      {
        text_only_mode = 1;
        printf( "ERROR: could not write task data file %s\n", 
                task_dat_file[i] );
        return;
      }
      
      for ( j=0; j<task_data_sorted[i]->t.d_len; j++ )
      {
        int tid = task_data_sorted[i]->task_id;

        /* Adjust the upper ISR range for better plotting */
        if ( tid >= 0x80 ) tid -= 0x50;

        fprintf( file, 
                 "%9.3f   %3d\n", 
                 task_data_sorted[i]->t.r_data[j], 
                 tid );
        if ( task_data_sorted[i]->t.t_data[j] > 0.5 )
        {
          for ( k=0; k<(int)((task_data_sorted[i]->t.t_data[j]) * 2.0); k++ )
          {
            fprintf( file, 
                     "%9.3f   %3d\n",
                     task_data_sorted[i]->t.r_data[j] + ((float)(k) / 2.0),
                     tid );
          }
        }
        fprintf( file, 
                 "%9.3f   %3d\n", 
                 task_data_sorted[i]->t.r_data[j] 
                 + task_data_sorted[i]->t.t_data[j], 
                 tid );
      }
      
      fclose( file );    
    }
  }

  /* Let the user know we are still here ... */
  printf( "." );
  fflush( stdout );

  /* Write the gnuplot command file */
  if ( (file = fopen( gnu_cmd_file, "w" )) == NULL )
  {
    text_only_mode = 1;
    printf( "ERROR: could not write GnuPlot cmd file %s\n", gnu_cmd_file );
    return;
  }

  fprintf( file, "set terminal gif medium size 700,400\n" );
  fprintf( file, "set output \"%s\"\n", cpu_file );
  fprintf( file, "set xlabel \"Profiling Runtime [sec]\"\n" );
  fprintf( file, "set ylabel \"CPU Utilization [%%]\"\n" );
  fprintf( file, "set yrange [0.0:105.0]\n" );
  fprintf( file, "plot '%s' notitle with lines\n", cpu_dat_file );

  fprintf( file, "set terminal gif medium size 400,300\n" );
  fprintf( file, "set output \"%s\"\n", rlp_file );
  fprintf( file, "set xlabel \"Profiling Runtime [sec]\"\n" );
  if ( testapp_format )
  {
    fprintf( file, "set ylabel \"TestApp Link Utilization [%%]\"\n" );
    fprintf( file, "set yrange [0.0:119.0]\n" );
    fprintf( file, 
	     "plot '%s' title \"Fwd Link\" with lines, '%s' title \"Rev Link\" with lines\n", 
	     rlp_rx_dat_file, 
	     rlp_tx_dat_file );
  }
  else
  {
    fprintf( file, "set ylabel \"RLP Throughput [kBps]\"\n" );
    fprintf( file, "set yrange [0.0:%9.3f]\n", rlp_max_rate );
    fprintf( file, 
	     "plot '%s' title \"RLP Rx\" with lines, '%s' title \"RLP Tx\" with lines\n", 
	     rlp_rx_dat_file, 
	     rlp_tx_dat_file );
  }
  fprintf( file, "set output \"%s\"\n", latency_file );
  fprintf( file, "set xlabel \"5 ms Timer Tick Starttime [sec]\"\n" );
  fprintf( file, "set ylabel \"Scheduling Interval [ms]\"\n" );
  fprintf( file, "set yrange [0.0:10.0]\n" );
  fprintf( file, 
           "plot '%s' with dots\n", 
           latency_dat_file );

  /* Browser can only display images up to 8000 pixels wide */
  fprintf( file, 
           "set terminal gif medium size %d,600\n", 
           min( (int) total_time, 8000 ) ); 
  fprintf( file, "set output \"%s\"\n", task_file );
  fprintf( file, "set xlabel \"Scheduling Times [ms]\"\n" );
  fprintf( file, "set ylabel \"Tasks\"\n" );
  fprintf( file, "set noytics\n" );
  fprintf( file, "set xtics 100\n" );
  fprintf( file, "set mxtics 20\n" );
  fprintf( file, "set yrange [-10:%d]\n", 0x70 );
  fprintf( file, 
           "plot '%s'  title \"%s\" with dots", 
           task_dat_file[0],
           task_data_sorted[0]->t_name );
  for ( i=1; i<min(num_tasks_sorted, 20); i++ )
  {
    fprintf( file, 
             ", '%s'  title \"%s\" with dots", 
             task_dat_file[i],
             task_data_sorted[i]->t_name );
  }
  fprintf( file, "\n" );

  fclose( file );

  /* Let the user know we are still here ... */
  printf( "." );
  fflush( stdout );

  /* Generate the plots */
  sprintf( print_str, "%s/wgnuplot.exe %s\n", cur_working_dir, gnu_cmd_file );
  system( print_str );

  /* Delete temporary files */
  sprintf( print_str, 
           "rm -f %s*.dat %s\n", 
           data_file_prefix,
           gnu_cmd_file );
  system( print_str );
}

/*===========================================================================

FUNCTION USAGE

DESCRIPTION
  This function prints out the correct usage of hp_stats.

DEPENDENCIES
  None

PARAMETERS
  pname  - name of the executable

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void usage( char* pname )
{
  printf( "\nUsage: %s <.tt_file_names>\n\n", pname );
  printf( "       -c   Generate console output only\n" );
  printf( "       -f   Parse data file for function profiling\n" );
  printf( "       -i   Specify an alternate .ini file\n" );
  printf( "       -n   Do not display HTMl pages after generating the report\n" );
  printf( "       -p   Specify prefix for output files generated\n" );
  printf( "       -s   Specify the symbol file name for function lookup\n" );
  printf( "       -t   Generate text-only HTML report\n\n" );
}

/*===========================================================================

FUNCTION MAIN

DESCRIPTION
  This function is the entry point. It does command line parsing
  and calls all helper functions.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Std POSIX return value.

SIDE EFFECTS
  None
===========================================================================*/
int main( int argc, char* argv[] )
{
  FILE* gfile = NULL;
  int   i, j, o;
  int   opt_cnt = 1;
  int   task_count = 0;
  char* ini_file_name;
  char* os_type = NULL;
  char* drive_letter = NULL;
  char  ini_file_w_path[192];
  char  sys_cmd[192];
  
  /* Set command line defaults */
  ini_file_name      = "hp_stats.ini";
  symbol_file_name   = "SURFTE.sym";
  output_file_prefix = "ProfileData";
  output_format_html = 1;
  func_mode          = 0;
  display_mode 	     = 1;

  /* For now we only run from the Cygnus shell */
  os_type = getenv( "OSTYPE" );
  if ( os_type == NULL )
  {
    printf( "\nPlease run hp_stats from the Cygnus shell.\n\n" );
    exit( 1 );
  }

  /* Parse command line options */
  while ( (o = getopt( argc, argv, "acdflntw:i:p:s:" )) != EOF )
  {
    switch ( o )
    {
      case 'a':
        testapp_format = 1;
        opt_cnt++;
        break;

      case 'c':
        output_format_html = 0;
        opt_cnt++;
        break;

      case 'd':
        debug = 1;
        opt_cnt++;
        break;

      case 'f':
        func_mode = 1;
        opt_cnt++;
        break;

      case 'l':
        max_results = atoi( optarg );
        opt_cnt += 2;
        break;

      case 'i':
        ini_file_name = optarg;
        opt_cnt += 2;
        break;

      case 'n':
        display_mode = 0;
        output_format_html = 1;
        opt_cnt++;
        break;

      case 'p':
        output_prefix_given = 1;
        output_file_prefix = optarg;
        opt_cnt += 2;
        break;

      case 's':
        symbol_file_name = optarg;
        opt_cnt += 2;
        break;

      case 't':
        text_only_mode = 1;
        output_format_html = 1;
        opt_cnt++;
        break;

      case '?':
        usage( argv[0] );
        exit( 1 );
        break;
    }
  }
  
  /* Check cmd line syntax */
  if ( argc == opt_cnt ) 
  {
    usage( argv[0] );
    exit( 1 );
  }

  /* Save the directory hp_stats.exe is in */
  memset( cur_working_dir, 0, 192 );
  for ( i=strlen( argv[0] ); i>=0; i-- ) {
    if ( argv[0][i] == '/' ||
         argv[0][i] == '\\') 
      break;
  }
  strncpy( cur_working_dir, argv[0], i );

  /* Check if gnuplot is available */
  sprintf( ini_file_w_path, "%s/wgnuplot.exe", cur_working_dir );
  if ( output_format_html && ! text_only_mode )
  {
    if ( (gfile = fopen( ini_file_w_path, "r" )) == NULL )
    {
      printf( "ERROR: wgnuplot required but not found.\n" );
      exit( 1 );
    }
    else
    {
      fclose( gfile );
    }
  }

  /* Initialize internal data structures */
  sprintf( ini_file_w_path, "%s/%s", cur_working_dir, ini_file_name );
  task_data_init( ini_file_w_path );
  
  /* Figure out from first input file what the format is */
  identify_la_model( argv[opt_cnt] );

  /* Parse all input files and accumulate data in internal array */
  for( i=opt_cnt; i<argc; i++ )
  {
    read_data( argv[i] );

    if ( (! func_mode) && (total_tasks == task_count) )
    {
      printf(
        "Found inconsistent format between input files - exiting ...\n" );
      exit( 1 );
    }
    task_count = total_tasks;
  }

  /* Calculate the statistics on the whole data set */
  get_stats();
  
  /* Print the results */
  if ( ! output_format_html ) show_stats();

  /* Print a report */
  if ( output_format_html )
  {
    printf( "Generating HTML report .." );
    fflush( stdout );

    /* Create the plots for the html pages */
    if ( ! text_only_mode )
    {
      write_html_plots( argv[opt_cnt] );
    }

    /* Write the results in HTML format */
    write_html_stats( argv[opt_cnt], 0 );

    /* Write a printable version of above */
    write_html_stats( argv[opt_cnt], 1 );

    printf( " done.\n" );

    if(display_mode){

      memset( sys_cmd, 0, 192 );

      if ( os_type && ! strcmp( os_type, "cygwin" ) )
      {
        sprintf( sys_cmd, 
                 "cmd.exe /C %s &\n", 
                 html_start_file );

        drive_letter = strstr( sys_cmd, "//" );
        if ( drive_letter )
        {
          *drive_letter = ' ';
          *(drive_letter + 1) = *(drive_letter + 2);
          *(drive_letter + 2) = ':';
        }
        
        for ( i=strlen( sys_cmd ); i>10; i-- )
        {
          if ( sys_cmd[i] == '/' )
          {
            for ( j=strlen( sys_cmd ); j>i; j-- ) sys_cmd[j+1] = sys_cmd[j];
            sys_cmd[i+1] = '\\';
            sys_cmd[i] = '\\';
          }
        }
      }
      else
      {
        sprintf( sys_cmd, 
                 "cmd.exe /C %s\n", 
                 html_start_file );
      }
      system( sys_cmd );
    }
  }
  return 0;
}
