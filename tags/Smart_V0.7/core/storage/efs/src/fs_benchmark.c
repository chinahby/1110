/***********************************************************************

 * fs_benchmark.c
 *
 * Filesystem Benchmark Suite
 *
 * Copyright (C) 2006-2009 QUALCOMM, Inc.
 *
 * This code provides (optional) functions to call the public EFS API
 * and measure the time of typical operations.  This is used to measure
 * on-target performance.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/efs/main/latest/src/fs_benchmark.c#20 $ $DateTime: 2009/07/06 18:43:01 $ $Author: ebuller $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-07-06   ebb   Added #include fs_config.h to pull boot definitions.
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2009-05-08   ebb   Renamed hotplug counters to blockdev counters.
2009-04-20   yg    Return blk_size based count for df bench test.
2009-04-03   wek   Added testing efs_utime() API.
2009-02-18   ebb   Added memory protection test to trigger fault.
2009-02-05   wek   Fix compiler erros on targets builds because of #elif.
2009-01-27   sh    Removed extra arguments from Bab Single Event messages
2009-01-26   wek   Add raw setvbuf & fflush, Fix raw stdio bugs.
2009-01-15   ebb   Added delay after writing files for safe_read_test.
2008-12-12   yg    Fix compiler warning for unused static functions.
2008-12-12   rp    Add counters for the reliable write functions.
2008-12-02   yg    Modify reset test to just return error.
2008-11-25   sh    Move hotplug open/close out of timed test.
2008-11-24   sh    Add random-write hotplug test.
2008-11-19   yg    Move cpu_hog task init from public, disable for safe bench.
2008-11-19   sh    Babylon references must be conditional on feature
2008-11-06   sh    Added Babylon support and port tests
2008-10-30   yg    Fix compiler warning for unused functions.
2008-09-15   yg    For safe enum test return the file/directory counts.
2008-05-07   sh    Correct the no-unsafe-benchmarks case.
2008-04-30   sh    Simplify hotplug stubs.
2008-03-28   ebb   Make hotplug related functions complilation conditional.
2008-03-06   sh    Revert benchmark command numbers to prior values.
2008-01-15   sc    Fixed compiler warning by casting packed array
2008-01-09   sc    Added safe benchmark tests that are always on
2007-12-07   rp    Enhancements to the statvfs struct.
2007-11-16   rp    Added efs_chown_recursive() and efs_chmod_recursive().
2007-08-19   rp    Changed FS_BENCHMARK_MAX_DESCRIPTORS from 128 to 16.
2007-08-17   yg    Aligned benchmark buffer.
2007-08-10   sh    Return a failure count for unrecognized commands.
2007-07-25   sh    Add opendir/readdir test.
2007-02-20   sh    Add efs_deltree test.
2007-01-12   dlb   Use safer string functions.
2006-12-05   dlb   Print maxwrite if it is known.
2006-11-22   dlb   Conditionalize benchmarks based on EFS capabilities.
2006-11-14   sh    Fix INTLOCK use in reset function.
2006-11-02   rp    Added stream based file-op functions.
2006-10-17   sh    Counters now exist in all builds.
2006-10-12   sh    Correct compiler warning
2006-10-06   yg    Add more benchmark functions for file access.
2006-10-06   sh    Correct fs_test_df byte counts
2006-10-02   sh    Add direct media block-access functions. (lint repair)
2006-09-29   sh    Lint cleanup
2006-09-25   yg    Remove unused table entry.
2006-09-14   sh    Removed FTL dependency
2006-08-28   yg    Added some benchmark config func for FTL cache.
2006-06-23   sh    Add stub functions to use when the feature is disabled
2006-05-25   sh    Add a format command
2006-05-15   sh    Include fs_hotplug.h
2006-05-11   sh    Renamed efs_hotplug_... to hotplug_...
2006-05-05   sh    Add hook to set hotplug poll rate
2006-01-26   sh    Treat any incomplete write operations as failure.
2005-11-18   sh    Added EFS1 read/write tests
2005-10-14   sh    Create  (Written by Kiran K, Summer 2005)

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "fs_config.h"

#include "fs_benchmark.h"
#include "fs_pathbuff.h"
#include "fs_util.h"

/*
 * This structure maintains global counters for EFS operations.
 * Specifically, it is useful to measure how many page reads, page writes,
 * block erases, etc happen at the flash driver level.
 *
 * These counters are for INFORMATION and performance measurement
 * only, so no code should depend on them.
 *
 * The counters are present, whether benchmarking is enabled or not.
 */
struct fs_benchmark_counters fs_counter = { 0, 0, 0, 0, 0, 0, 0, 0 };

/* The rest of the code is conditional on this feature.  By default,
 * this includes only the "safe" tests, so there's no real reason not
 * to enable it.
 */
#ifdef FEATURE_FS_BENCHMARK

#ifndef FEATURE_EFS2
#error "FS Benchmark Requires EFS2"
#endif

#include <string.h>
#include <stdio.h>              /* For sprintf */

#include "fs_err.h"
#include "fs_errno.h"
#include "fs_public.h"
#include "fs_hotplug.h"
#include "fs_util.h"
#include "jzap.h"
#include "queue.h"
#include "ran.h"
#include "fs_osal.h"
#include "task.h"
#include "time.h"
#include "time_svc.h"
#include "fs.h"
#include "fs_estdio.h"
#include "fs_privatedir.h"
#include "fs_babylon.h"
#include "fs_util.h"
#include "fs_sys_types.h"

/* This is the amount of time used by the safe benchmark tests to
 * transfer data to the device.  Since bandwidth will vary, we aim for
 * a specific amount of time in writing data.
 *
 * eg: safe read test needs to write data to the device before
 * measuring the read performance. So, it writes data for
 * BENCH_TIME_LIMIT amount of time before reading it back.
 */
#ifndef FS_BENCHMARK_TIME_MS
#define FS_BENCHMARK_TIME_MS (10 * 1000) /* Time in milliseconds */
#endif

/* Off-target, we advance 10% each time you ask */
#ifdef FS_UNIT_TEST
#error code not present
#endif

/*
 * Common header for all queues
 */
typedef struct {
  q_link_type          link;         /* Queue link */
  q_type               *done_q_ptr;  /* Queue to place cmd */
} fs_benchmark_hdr_type;

/* Benchmark command queue element */
typedef struct
{
  fs_benchmark_hdr_type hdr;    /* queue header */
  fs_benchmark_test_cmd cmd;    /* Command packet */
  char whole_path[FS_PATH_MAX+1]; /* Path */
} fs_benchmark_queued_cmd;

/* Benchmark report type */
typedef struct
{
  fs_benchmark_hdr_type hdr;    /* header */
  fs_benchmark_report report;
} fs_benchmark_queued_rpt;


/* Task glue */
#define FS_BENCHMARK_STACK_SIZE 2048
static fs_os_tcb_type fs_benchmark_tcb;
static unsigned char fs_benchmark_stack [ FS_BENCHMARK_STACK_SIZE ];

/* Timers and signals */
#define FS_BENCHMARK_SLEEP_TIMER_SIG    0x0002
#define FS_BENCHMARK_CMD_Q_SIG          0x0004
#define FS_BENCHMARK_RPT_Q_SIG          0x0008

/* How many outstanding commands can be queued at once?
   Each one costs a full pathlength of memory */
#define FS_BENCHMARK_MAX_CMDS 20
#define FS_BENCHMARK_MAX_RPTS FS_BENCHMARK_MAX_CMDS

/* General purpose pause timer for tests */
static fs_os_timer_type fs_benchmark_sleep_timer;

/* Used to see if a command is being processed at a given time;
   set true when commands being processed, false when not */
static int test_running = 0;

/* True when all commands are being discarded from the queue. */
static int flushing = 0;

/*
 * The benchmark uses this buffer for storing the bulk data for read/write
 * It needs to be as big as the biggest required single operation.
 */
#define BENCH_BUFFER_BYTES (64 * 1024)
#define BENCH_BUFFER_DWORDS (BENCH_BUFFER_BYTES / sizeof(dword))

/* The +8 is to allow an intentionally misaligned start address of 0
 * to 31 bytes */
FS_ALIGN static dword bench_buffer[BENCH_BUFFER_DWORDS + 8];
FS_ALIGN static dword read_buffer[BENCH_BUFFER_DWORDS + 8];


#ifdef FEATURE_FS_UNSAFE_BENCHMARK
/*
 * Size of internal fs_stdio buffer.
 * This size was chosen twice FS_MAX_TRANSACTION to test if there is a gain in
 * performance by buffering more than the max transaction.
 */
  #define STDIO_BUF_SIZE               (FS_MAX_TRANSACTION * 2)
  /* Array of pointers to open EFS_EFILE instances. It controls how many files
   * can be opened simultaneously using fs_stdio calls. */
  #define FS_BENCHMARK_MAX_STREAMS      2
#else
  #define STDIO_BUF_SIZE                1
  #define FS_BENCHMARK_MAX_STREAMS      1
#endif
/* Stdio test buffer, only used on unsafe benchmark calls. */
FS_ALIGN static char stdio_buffer[FS_BENCHMARK_MAX_STREAMS][STDIO_BUF_SIZE];

/* Global count of how many bytes are read/written during a series of tests */
static uint32 total_bytes_write = 0;
static uint32 total_bytes_read = 0;

/*
 * The CPU HOG task will burn some amount of the CPU on command,
 * to simulate loading
 */
#define FS_CPU_HOG_STACK_SIZ 2048

static fs_os_tcb_type fs_cpu_hog_tcb;
static unsigned char fs_cpu_hog_stack [FS_CPU_HOG_STACK_SIZ];

/* These two values set the ratio of Busy to Sleep, in milliseconds */
static uint32 fs_cpu_hog_run_ms = 0;
static uint32 fs_cpu_hog_sleep_ms = 1000;

#define FS_CPU_HOG_TIMER_SIG  0x0001
static fs_os_timer_type fs_cpu_hog_timer;

/* The ran.[ch] code has one global seed value.  We set it at the
 * start of our task, but unfortunately, mc_init_ran() also sets it to
 * the ESN value.
 * Which is usually zero.
 * Which is an invalid seed value.
 */
static dword fs_benchmark_seed = 0xDECAFBAD;

/* Fill the benchmark buffer with a fresh block of psuedo-random data */
static void
randomize_bench_buffer (dword seed)
{
  unsigned int i;

  ran_seed (seed);
  for (i = 0; i < BENCH_BUFFER_DWORDS; i++)
    bench_buffer[i] = ran_next();

  fs_benchmark_seed = ran_next();
}

/* All benchmark tests are expected to return successfully, since this is
   a performance test, not a quality-assurance test.
   In the event we get an errno value, we do this. */
static void
fs_benchmark_errno (char *msg, char *fname)
{
  (void) msg;
  (void) fname;

  ZPRINTF ("ERROR: %s on \"%s\" errno = %d\n\r", msg, fname, efs_errno);
}

#if FS_BENCHMARK_MAX_STREAMS > FS_MAX_DESCRIPTORS
  #error "FS_BENCHMARK_MAX_STREAMS is > FS_MAX_DESCRIPTORS"
#endif

/*
 * This descriptor is used to seperate the open/close routines from
 * the actual read/write routines. If the script doesn't close the
 * file, then it would have a valid fd, which will be closed just
 * before opening another file. It is better to keep protected
 * static variable than getting the fd from script, since it is
 * difficult to verify if we get fd from outside. */
static int raw_fd = -1;

/* Array of streams to use on "fs_test_raw_" stdio calls */
static EFS_EFILE *raw_efs_efile[FS_BENCHMARK_MAX_STREAMS] = {NULL, };

/*
 * Sleep 'ms' milliseconds by yeilding our task.
 */
static void
fs_benchmark_sleep (int ms)
{
  fs_os_clr_sigs (&fs_benchmark_tcb, FS_BENCHMARK_SLEEP_TIMER_SIG);
  fs_os_set_timer(&fs_benchmark_sleep_timer, ms);
  fs_os_wait (FS_BENCHMARK_SLEEP_TIMER_SIG); /* Ignoring other signals */
}



/* ============== Benchmark Tests ============== */

/* Sleep for args[0] milliseconds */
int
fs_test_sleep_ms (int32 *args, char *path)
{
  (void) path;

  ZPRINTF ("Sleeping for %d mS...", args[0]);
  fs_benchmark_sleep (args[0]);
  return 0;
}

/* Do nothing at all */
int
fs_test_nop (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return 0;
}

/* Do nothing at all, and do it poorly.  Always returns an error.  Used to
 * indicate features not present in the build. */
int
fs_test_error (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return -1;
}

/* Fill the benchmark buffer with random values,
 * using the 32-bit seed in arg[0,1] */
int
fs_test_fill_rand (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];

  (void) path;

  randomize_bench_buffer (seed);

  return 0;
}

int
fs_test_stuff_pattern (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];
  int byte_count = args[2];
  unsigned int dword_count;
  unsigned int i;

  (void) path;

  dword_count = (byte_count + sizeof(dword) - 1) / sizeof(dword);
  if (dword_count > BENCH_BUFFER_DWORDS)
    dword_count = BENCH_BUFFER_DWORDS;

  ran_seed (seed);
  for (i = 0; i < dword_count; i++)
    bench_buffer[i] = ran_next();
  return 0;
}

int
fs_test_check_pattern (int32 *args, char *path)
{
  dword seed = (unsigned)args[0] << 16 | args[1];
  int byte_count = args[2];
  unsigned int dword_count;
  unsigned int i;

  (void) path;

  dword_count = (byte_count + sizeof(dword) - 1) / sizeof(dword);
  if (dword_count > BENCH_BUFFER_DWORDS)
    dword_count = BENCH_BUFFER_DWORDS;

  ran_seed (seed);
  for (i = 0; i < dword_count; i++) {
    dword expect;
    expect = ran_next();
    if (bench_buffer[i] != expect) {
      ZPRINTF ("mismatch @ offset %d! %x != %x\r\n",
               i, bench_buffer[i], expect);
      return -1;
    }
  }
  return 0;
}

int
fs_test_write (int32 *args, char *path)
{
  int write_size  = args[0];
  int write_count = args[1];
  int oflags = args[2];
  int write_where = args[3];

  int fd;
  int failed = 0;

  fd = efs_open (path, oflags, 0666);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  if (write_where & SEEK_END)
    efs_lseek (fd, 0, SEEK_END);

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_write (fd, bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_size) {
      ZPRINTF ("Incomplete write: %d out of %d\n\r", written, write_size);
      fs_benchmark_errno ("efs_write()", path);
      failed = 1;
      break;
    }
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return failed ? -1 : 0;
}

int
fs_test_read (int32 *args, char *path)
{
  int32 read_size  = args[0];
  int32 read_count = args[1];
  int32 oflags = args[2];
  int fd;

  if (read_size < 0)
    return -1;
  if (read_size > (signed) sizeof(read_buffer))
    return -1;

  fd = efs_open (path, oflags);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  while (read_count--)
  {
    int read_done;
    read_done = efs_read (fd, read_buffer, read_size);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      break;
    }
    if (read_done != read_size)
    {
      ZPRINTF ("Incomplete read: %d out of %d\n\r", read_done, read_size);
    }
    if (read_done == 0)
      break;
    total_bytes_read += read_done;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return 0;
}

int
fs_test_raw_open (int32 *args, char *path)
{
  int oflags = args[2];
  int write_where = args[3];

  if (raw_fd > 0)
  {
    if (efs_close (raw_fd))
    {
      fs_benchmark_errno ("efs_close()", path);
      return -1;
    }
  }

  raw_fd = efs_open (path, oflags, 0666);
  if (raw_fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  if (write_where & SEEK_END)
    efs_lseek (raw_fd, 0, SEEK_END);

  return 0;
}

int
fs_test_raw_close (int32 *args, char *path)
{
  (void) args;

  if (raw_fd < 0)
  {
    return -1;
  }

  if (efs_close (raw_fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  raw_fd = -1;

  return 0;
}

int
fs_test_raw_write (int32 *args, char *path)
{
  int write_size  = args[0];
  int write_count = args[1];
  int failed = 0;

  if (raw_fd < 0)
  {
    return -1;
  }

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_write (raw_fd, bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_size) {
      ZPRINTF ("Incomplete write: %d out of %d\n\r", written, write_size);
      fs_benchmark_errno ("efs_write()", path);
      failed = 1;
      break;
    }
  }

  return failed ? -1 : 0;
}

int
fs_test_raw_read (int32 *args, char *path)
{
  int read_size  = args[0];
  int read_count = args[1];

  if (raw_fd < 0)
  {
    return -1;
  }

  if (read_size > (signed) sizeof(read_buffer))
    return -1;

  while (read_count--)
  {
    int read_done;
    read_done = efs_read (raw_fd, read_buffer, read_size);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      break;
    }
    if (read_done != read_size)
    {
      ZPRINTF ("Incomplete read: %d out of %d\n\r", read_done, read_size);
    }
    if (read_done == 0)
      break;
    total_bytes_read += read_done;
  }

  return 0;
}

int
fs_test_write_efs1_core (int need_hash, int32 *args, char *path)
{
#ifdef FS_UNIT_TEST
#error code not present
#else
  unsigned int write_size  = args[0];
  int write_count = args[1];

  fs_rsp_msg_type response;
  fs_handle_type handle;
  fs_open_xparms_type options;

  options.create.cleanup_option = FS_OC_CLOSE;
  options.create.buffering_option = FS_OB_PROHIBIT;
  options.create.attribute_mask = FS_FA_UNRESTRICTED;
  options.create.base_address = 0;

  fs_open (path, FS_OA_CREATE, &options, NULL, &response);
  if (response.open.status != FS_OKAY_S) {
    ZPRINTF ("unable to create file (%d): %s\r\n",
              response.open.status, path);
    return -1;
  }
  handle = response.open.handle;

  if (need_hash)
    ran_seed (0xDECAFBAD);

  while (write_count--)
  {
    if (need_hash) {
      unsigned int i;
      for (i = 0; i < (write_size / sizeof(bench_buffer[0])); i++)
        bench_buffer[i] = ran_next();
    }

    fs_write (handle, bench_buffer, write_size, NULL, &response);

    if (response.write.status != FS_OKAY_S) {
      ZPRINTF ("write err: %d\r\n", response.write.status);
      return -1;
    }

    if (response.write.count != write_size) {
      ZPRINTF ("short write: %d\r\n", response.write.count);
      return -1;
    }

    total_bytes_write += write_size;
  }

  fs_close (handle, NULL, &response);
  if (response.close.status != FS_OKAY_S) {
    ZPRINTF ("Error closing file: %d\r\n", response.close.status);
    return -1;
  }
  return 0;
#endif /* ! FS_UNIT_TEST */
}

int
fs_test_write_efs1 (int32 *args, char *path)
{
  return fs_test_write_efs1_core (0, args, path);
}

int
fs_test_write_efs1_hash (int32 *args, char *path)
{
  return fs_test_write_efs1_core (1, args, path);
}

int
fs_test_read_efs1_core (int check_hash, int32 *args, char *path)
{
#ifdef FS_UNIT_TEST
#error code not present
#else
  unsigned int read_size  = args[0];
  int read_count = args[1];

  fs_rsp_msg_type  response;
  fs_handle_type handle;

  if (read_size > sizeof(read_buffer))
    return -1;

  fs_open (path, FS_OA_READONLY, NULL, NULL, &response);
  if (response.open.status != FS_OKAY_S) {
    ZPRINTF ("error opening file (%d): %s\r\n",
              response.open.status, path);
    return -1;
  }
  handle = response.open.handle;

  if (check_hash)
    ran_seed (0xDECAFBAD);

  while (read_count--)
  {
    fs_read (handle, read_buffer, read_size, NULL, &response);
    if (response.read.status != FS_OKAY_S) {
      ZPRINTF ("read errr: %d\r\n", response.read.status);
      return -1;
    }
    if (response.read.count == 0) {
      ZAP ("End of file on read");
      break;
    }
    total_bytes_read += response.read.count;

    if (check_hash) {
      unsigned int i;
      for (i = 0; i < (response.read.count / sizeof(read_buffer[0])); i++) {
        dword expect;
        expect = ran_next();
        if (read_buffer[i] != expect) {
          ZPRINTF ("mismatch @ offset %d! %x != %x\r\n",
                   i, read_buffer[i], expect);
          return -1;
        }
      }
    }
  }

  fs_close (handle, NULL, &response);
  if (response.close.status != FS_OKAY_S) {
    ZPRINTF ("Error closing file (%d)\r\n", response.close.status);
    return -1;
  }
  return 0;
#endif /* ! FS_UNIT_TEST */
}

int
fs_test_read_efs1 (int32 *args, char *path)
{
  return fs_test_read_efs1_core (0, args, path);
}

int
fs_test_read_efs1_hash (int32 *args, char *path)
{
  return fs_test_read_efs1_core (1, args, path);
}

/* The CPU Hog can be set to busy out the CPU for a specified load and
   duty cycle.  When enabled, it will sleep for "sleep" milliseconds,
   then churn for "run" milliseconds. */
int
fs_test_set_cpu_hog (int32 *args, char *path)
{
  (void) path;

  /* Initialize CPU hog. This will initialize only if its already not */
  fs_cpu_hog_init ();

  fs_cpu_hog_sleep_ms = args[0];
  fs_cpu_hog_run_ms = args[1];
  ZPRINTF ("fs_set_cpu_hog (sleep %d ms, run %d ms)\n\r",
           fs_cpu_hog_sleep_ms, fs_cpu_hog_run_ms);
  return 0;
}

/* similar to fs_test_write, but writes random sizes each time */
int
fs_test_random_write (int32 *args, char *path)
{
  int write_size_min  = args[0];
  int write_size_max  = args[1];
  int write_count_min = args[2];
  int write_count_max = args[3];

  int write_size, write_count;

  char pathname[FS_PATH_MAX];
  int fd;
  int oflag;

  oflag = O_RDWR | O_CREAT;
  fd = efs_open (path, oflag, 0666);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", pathname);
    return -1;
  }

  ran_seed (fs_benchmark_seed);
  write_count = ran_dist(ran_next(), write_count_min, write_count_max + 1);

  while (write_count--)
  {
    int written;

    write_size = ran_dist(ran_next(), write_size_min, write_size_max + 1);
    written = efs_write (fd, bench_buffer, write_size);
    if (written != write_size)
    {
      ZPRINTF ("Incomplete write: %d out of %d\n\r", written, write_size);
      fs_benchmark_errno ("efs_write()", pathname);
    }
    total_bytes_write += written;
  }
  fs_benchmark_seed = ran_next();

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", pathname);
    return -1;
  }

  return 0;
}

int
fs_test_truncate (int32 *args, char *path)
{
  fs_off_t length = args[0];

  if (efs_truncate (path, length) != 0)
  {
    fs_benchmark_errno ("efs_truncate()", path);
    return -1;
  }
  else
  {
    return 0;
  }
}

/* This is the preferred write buffer size */
#define IDEAL_CHUNK   MIN (FS_MAX_TRANSACTION, BENCH_BUFFER_BYTES)

/* Fill the file-system to a given percentage */
int
fs_test_fill_fs_to_pct (int32 *args, char *path)
{
  int pct_full = args[0];

  int fd;

  fd = efs_open (path, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  ZPRINTF ("Filling to %d%% full...\n\r", pct_full);

  while (1)
  {
    int write_size = IDEAL_CHUNK;
    int written;
    struct fs_statvfs sbuf;
    uint32 block_goal, block_now;

    if (efs_statvfs ("/", &sbuf))
    {
      ZAP ("efs_statvfs() failed!");
      return -1;
    }

    block_goal = sbuf.f_blocks * pct_full / 100;
    block_now  = sbuf.f_blocks - sbuf.f_bavail;

    if (block_goal <= block_now)
      break;

    write_size = (block_goal - block_now) * sbuf.f_bsize;
    if (write_size > IDEAL_CHUNK)
      write_size = IDEAL_CHUNK;

    written = efs_write (fd, bench_buffer, write_size);
    if (written != write_size)
    {
      ZPRINTF ("Incomplete write: %d out of %d\n\r", written, write_size);
      fs_benchmark_errno ("efs_write()", path);
      break;
    }
    total_bytes_write += written;
  }

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return 0;
}

int
fs_test_delete (int32 *args, char *path)
{
  (void) args;

  if (efs_unlink (path) < 0)
  {
    fs_benchmark_errno ("efs_unlink()", path);
    return -1;
  }
  return 0;
}

int
fs_test_df (int32 *args, char *path)
{
  struct fs_statvfs sbuf;

  (void) args;

  if (efs_statvfs (path, &sbuf) == 0)
  {
    uint32 total_blocks = sbuf.f_blocks;
    uint32 free_blocks  = sbuf.f_bavail;
    uint32 used_blocks =  sbuf.f_blocks - sbuf.f_bavail;
    ZPRINTF ("df: %d bytes  (%02d%% free, %02d%% used)\n\r",
             free_blocks * sbuf.f_bsize,
             free_blocks * 100 / total_blocks,
             used_blocks * 100 / total_blocks);

    args[0] = total_blocks;
    args[1] = free_blocks;
    args[2] = used_blocks;
    args[3] = sbuf.f_bsize;
    return 0;
  }
  else
  {
    ZAP ("efs_statvfs() failed!");
    return -1;
  }
}

int
fs_test_mkdir (int32 *args, char *path)
{
  int result;

  (void) args;

  result = efs_mkdir ((const char *)path, 0777);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_mkdir()", path);
    return -1;
  }

  return 0;
}

int
fs_test_rmdir (int32 *args, char *path)
{
  int result;

  (void) args;

  result = efs_rmdir(path);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_rmdir()", path);
    return -1;
  }

  return 0;
}

int
fs_test_statvfs (int32 *args, char *path)
{
  struct fs_statvfs sbuf;

  (void) args;

  if (efs_statvfs (path, &sbuf) == 0)
  {
    ZPRINTF ("f_bsize   %d\n\r", (int)sbuf.f_bsize);
    ZPRINTF ("f_blocks  %d\n\r", (int)sbuf.f_blocks);
    ZPRINTF ("f_bfree   %d\n\r", (int)sbuf.f_bfree);
    ZPRINTF ("f_bavail  %d\n\r", (int)sbuf.f_bavail);
    ZPRINTF ("f_files   %d\n\r", (int)sbuf.f_files);
    ZPRINTF ("f_ffree   %d\n\r", (int)sbuf.f_ffree);
    ZPRINTF ("f_favail  %d\n\r", (int)sbuf.f_favail);
    ZPRINTF ("f_fsid    %d\n\r", (int)sbuf.f_fsid);
    ZPRINTF ("f_flag    %d\n\r", (int)sbuf.f_flag);
    ZPRINTF ("f_namemax %d\n\r", (int)sbuf.f_namemax);
    ZPRINTF ("f_maxwrite %d\n\r", (int)sbuf.f_maxwrite);
    ZPRINTF ("f_balloc %d\n\r", (int)sbuf.f_balloc);
    ZPRINTF ("f_hardalloc %d\n\r", (int)sbuf.f_hardalloc);
    ZPRINTF ("f_pathmax %d\n\r", (int)sbuf.f_pathmax);
    ZPRINTF ("f_is_case_sensitive %d\n\r", (int)sbuf.f_is_case_sensitive);
    ZPRINTF ("f_is_case_preserving %d\n\r", (int)sbuf.f_is_case_preserving);
    ZPRINTF ("f_max_file_size %d\n\r", (int)sbuf.f_max_file_size);
    ZPRINTF ("f_max_file_size_unit %d\n\r", (int)sbuf.f_max_file_size_unit);
    ZPRINTF ("f_max_open_files %d\n\r", (int)sbuf.f_max_open_files);
    ZPRINTF ("f_name_rule %d\n\r", (int)sbuf.f_name_rule);
    ZPRINTF ("f_name_encoding %d\n\r", (int)sbuf.f_name_encoding);
    return 0;
  }
  else
  {
    fs_benchmark_errno ("efs_statvfs()", "");
    return -1;
  }
}

/*
 * This function ensures that deleted files are not kept pending but
 * actually returned to the free pool.
 */
int
fs_test_flush_deleted_data (int32 *args, char *path)
{
  (void) path;
  (void) args;
  while (efs_image_prepare() > 0)
    ;
  return 0;
}

/* These functions are only meaningful if Hotplug is present */
#if defined(FEATURE_EFS_HOTPLUG) || defined(FEATURE_EFS_COLDPLUG)

/* Global hotplug device pointer */
static struct hotplug_device *bench_hdev = NULL;

/* Open the Hotplug device */
int
fs_test_hotplug_setup (char *path)
{
  bench_hdev = hotplug_find_dev_by_path (path);
  if (!bench_hdev)
    return -1;

  if (hotplug_dev_open (bench_hdev) != 0)
    return -1;

  return 0;
}

/* Close the Hotplug device */
int
fs_test_hotplug_teardown (char *path)
{
  (void) path;
  (void) hotplug_dev_close (bench_hdev);

  return 0;
}

/* This function sets the Hotplug poll interval */
int
fs_test_hotplug_poll_interval (int32 *args, char *path)
{
  (void) path;
  hotplug_set_polling_interval (args[0]);
  return 0;
}

/* This function formats media that would be mounted at 'path' */
int
fs_test_hotplug_format (int32 *args, char *path)
{
  (void) args;
  return hotplug_format (path);
}

/* All hotplug media has 512 bytes per sector*/
#define HOTPLUG_BYTES_PER_BLOCK 512

/* SDCC can only buffer 64KB or less at a time */
#define MAX_BYTES_XFER (HOTPLUG_BYTES_PER_BLOCK * 128)

/* Helper function to do a chunked read or write */
static int
fs_test_hotplug_read_write_core (int32 *args, char *path,
                                 char *display,
                                 int xfer_func (struct hotplug_device *,
                                                uint32,
                                                unsigned char *,
                                                uint16),
                                 uint32 *bytecount)
{
  int lba_start        = args[0];
  int blocks_remaining = args[1];
  int chunk_blocks     = args[2];
  unsigned int misalign = args[3];
  int result = -1;

  (void) path;                  /* only used by hotplug prep */

  if (!bench_hdev)
    return -1;

  if (chunk_blocks == 0)
    chunk_blocks = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;

  (void) display;               /* may not have ZPRINTF */
#if 0
  /* Debug only.  Causes significant slowdown */
  ZPRINTF ("%s %d blocks (chunks of %d) @ %d on %s...\n\r",
           display, blocks_remaining, chunk_blocks, lba_start, path);
#endif

  while (blocks_remaining > 0) {
    int blocks = blocks_remaining;

    if (blocks > chunk_blocks)
      blocks = chunk_blocks;

    /* Chunks can't be bigger than our buffer in RAM */
    if ((blocks * HOTPLUG_BYTES_PER_BLOCK) > (signed) sizeof(bench_buffer))
      blocks = sizeof(bench_buffer) / HOTPLUG_BYTES_PER_BLOCK;

#ifdef MAX_BYTES_XFER
    /* Due to an SDCC bug, reads can't be bigger than 64K */
    if ((blocks * HOTPLUG_BYTES_PER_BLOCK) > MAX_BYTES_XFER)
      blocks = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;
#endif

    result = xfer_func (bench_hdev, lba_start,
                        ((unsigned char *) bench_buffer) + (misalign & 31),
                        blocks);
    if (result != 0)
      break;
    *bytecount += blocks * HOTPLUG_BYTES_PER_BLOCK;
    lba_start += blocks;
    blocks_remaining -= blocks;
  }

  return result;
}


/* Read blocks from a hotplug device */
int
fs_test_hotplug_read (int32 *args, char *path)
{
  return fs_test_hotplug_read_write_core
    (args, path, "Reading",
     hotplug_dev_read, &total_bytes_read);
}


/* Read blocks from a hotplug device */
int
fs_test_hotplug_write (int32 *args, char *path)
{
  return fs_test_hotplug_read_write_core
    (args, path, "Writing",
     hotplug_dev_write, &total_bytes_write);
}


/* Write randomly through hotplug */
int
fs_test_hotplug_random_write (int32 *args, char *path)
{
  int lba_min          = args[0];
  int lba_max          = args[1];
  int block_count_min  = args[2];
  int block_count_max  = args[3];
  int remaining_blocks = args[4];
  int result = -1;
  void *buf = bench_buffer;

  (void) path;                  /* Only used by hotplug setup */

  if (!bench_hdev)
    return -1;

  /* Limited by Hotplug maximum blocks */
  if (block_count_max > (MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK))
    block_count_max = MAX_BYTES_XFER / HOTPLUG_BYTES_PER_BLOCK;

  /* Limited by benchmark's buffer size */
  if (block_count_max > (BENCH_BUFFER_BYTES / HOTPLUG_BYTES_PER_BLOCK))
    block_count_max = BENCH_BUFFER_BYTES / HOTPLUG_BYTES_PER_BLOCK;

  /* Positive values only for count */
  if (block_count_min < 1)
    block_count_min = 1;

  ran_seed (fs_benchmark_seed);

  while (remaining_blocks) {
    int lba, count;
    count = ran_dist(ran_next(), block_count_min, block_count_max + 1);
    if (count > remaining_blocks)
      count = remaining_blocks;

    lba = ran_dist(ran_next(), lba_min, lba_max + 1 - count);

    result = hotplug_dev_write (bench_hdev, lba, buf, count);
    if (result != 0)
      break;
    total_bytes_write += count * HOTPLUG_BYTES_PER_BLOCK;
    remaining_blocks -= count;
  }

  fs_benchmark_seed = ran_next();

  return result;
}


/* Check the media that might be mounted and return the size and
 * status.  The longest part of this function is the is_present()
 * call, which varies quite a bit between media and between
 * targets.  */
int
fs_test_hotplug_check_media (int32 *args, char *path)
{
  struct hotplug_device *hdev;
  uint32 sector_cnt = 0;
  uint16 sector_size = 0;
  int is_mounted;
  int is_present;

  hdev = hotplug_find_dev_by_path (path);
  if (!hdev)
    return -1;

  is_present = hotplug_dev_is_present (hdev);
  is_mounted = hotplug_dev_is_mounted (hdev);

  if (is_present)
    hotplug_dev_get_size (hdev, &sector_cnt, &sector_size);

  args[0] = sector_cnt;
  args[1] = sector_size;
  args[2] = is_present;
  args[3] = is_mounted;

  ZPRINTF ("[%s]: %dMB (%d * %d) %s, %s\n\r",
           path,
           sector_cnt / 1024 * sector_size / 1024,
           sector_cnt, sector_size,
           is_present ? " IS present" : "not present",
           is_mounted ? " IS mounted" : "not mounted" );

  return 0;
}

int
fs_test_hotplug_lock (int32 *args, char *path)
{
  struct hotplug_device *hdev;

  (void) args;

  hdev = hotplug_find_dev_by_path (path);
  if (!hdev)
    return -1;

  hotplug_lock_dev (hdev);

  return 0;
}

int
fs_test_hotplug_unlock (int32 *args, char *path)
{
  struct hotplug_device *hdev;

  (void) args;

  hdev = hotplug_find_dev_by_path (path);
  if (!hdev)
    return -1;

  hotplug_unlock_dev (hdev);

  return 0;
}

#else /* !FEATURE_EFS_HOTPLUG && !FEATURE_EFS_COLDPLUG */

/* Without Hotplug code present, all these functions return error */
#define fs_test_hotplug_poll_interval   fs_test_error
#define fs_test_hotplug_format          fs_test_error
#define fs_test_hotplug_read            fs_test_error
#define fs_test_hotplug_write           fs_test_error
#define fs_test_hotplug_random_write    fs_test_error
#define fs_test_hotplug_check_media     fs_test_error
#define fs_test_hotplug_lock            fs_test_error
#define fs_test_hotplug_unlock          fs_test_error
#define fs_test_hotplug_setup           NULL
#define fs_test_hotplug_teardown        NULL

#endif /* No Hotplug */

/* This function treats the first parameter mode as a string packed with
 * 4 characters, it extracts these 4 characters and stores it as a string
 * in the second parameter mode_str */
static void
fs_test_convert_mode_int_to_str (int mode, char mode_str[])
{
  int i;

  for ( i=0; i <= 3; ++i) {
    mode_str[3-i] = (char )(mode & 0xFF);
    mode >>= 8;
  }

  mode_str[4] = '\0';
}


/* This function opens the file-stream.
 * args[0] -> [IN]
      an int which contains 4 characters to specify the mode string.
 * args[1] -> [IN]
      where to start writing.
 * args[2] <- [OUT]
      The file-descriptor to the opened file-stream.
 */
int
fs_test_raw_fopen (int32 *args, char *path) {
  char mode[5];
  int write_where = args[1];
  int i;

  /* Doesn't need exclusive access to raw_efs_efile[] because only one task
   * (benchmark) is executing this code
   */
  /* find an empty slot on raw_efs_efile array */
  for (i = 0; i < FS_BENCHMARK_MAX_STREAMS && raw_efs_efile[i] != NULL;
       i++)
    ;

  if (i == FS_BENCHMARK_MAX_STREAMS)
  {
    /* to many files opened using benchmark. */
    return -1;
  }

  /* Extract the mode-string from the args[0] */
  fs_test_convert_mode_int_to_str (args[0], mode);

  raw_efs_efile[i] = efs_efopen (path, mode);
  if (raw_efs_efile[i] == NULL)
  {
    fs_benchmark_errno ("efs_efopen()", path);
    return -1;
  }

  if (write_where & SEEK_END)
  {
    efs_efseek (raw_efs_efile[i], 0, SEEK_END);
  }

  /* return the index in raw_efs_efile array */
  args[2] = i;

  return 0;
}

/* This function closes the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 */
int
fs_test_raw_fclose (int32 *args, char *path)
{
  int i = args[0];
  EFS_EFILE * fp;

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  /* Don't check for NULL because efs_efclose should handle NULLs gracefully.
   */
  if (efs_efclose (fp))
  {
    fs_benchmark_errno ("efs_efclose()", path);
    return -1;
  }

  raw_efs_efile[i] = NULL;

  return 0;
}


/* This function writes to the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      how many bytes to write.
 * args[2] -> [IN]
      how many times to write.
 */
int
fs_test_raw_fwrite (int32 *args, char *path)
{
  int i = args[0];
  int write_items  = args[1];
  int write_count = args[2];
  int failed = 0;
  EFS_EFILE * fp;

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  /* Don't check for NULL because stdio should handle NULLs gracefully. */

  while (write_count--)
  {
    fs_ssize_t written;

    written = efs_efwrite (bench_buffer, 1, write_items, fp);

    if (written > 0)
      total_bytes_write += written;

    if (written != write_items)
    {
      ZPRINTF ("Incomplete write: %d out of %d\n\r", written, write_items);
      fs_benchmark_errno ("efs_efwrite()", path);
      failed = 1;
      break;
    }
  }

  if (efs_efflush (fp))
    failed = 1;

  return failed ? -1 : 0;
}


/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      how many bytes to read.
 * args[2] -> [IN]
      how many times to read.
 */
int
fs_test_raw_fread (int32 *args, char *path)
{
  int i = args[0];
  int read_items  = args[1];
  int read_count = args[2];
  EFS_EFILE * fp;

  /* Check if index is inside array. */
  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];

  /* Don't check for NULL because stdio should handle NULLs gracefully. */
  if (read_items > (signed) sizeof(read_buffer))
    return -1;

  while (read_count--)
  {
    int read_done;
    read_done = efs_efread (read_buffer, 1, read_items, fp);
    if (read_done == -1)
    {
      fs_benchmark_errno ("efs_efread()", path);
      break;
    }
    if (read_done != read_items)
      ZPRINTF ("Incomplete read: %d out of %d\n\r", read_done, read_items);

    if (read_done == 0)
      break;

    total_bytes_read += read_items;
  }

  return 0;
}

/* This functions allows/restricts access to the /.efs_privatedir folder.
 * args[0]: 0 to restrict access, non-zero to allow access */
int
fs_test_privatedir_set_access (int32 *args, char *path)
{
  int allow_access = args[0];

  (void) path;

  efs_privatedir_set_access (allow_access);

  return 0;
}

/* This function Doesn't reset the target anymore.
 * Use either brutforce method or power supply to reset or a
 * regular Diag packet to reset the target using QXDM */
int
fs_test_reset (int32 *args, char *path)
{
  (void) args;
  (void) path;

  return -1;
}

/* This function deletes an entire tree of files, without any checks.
 * Use with care!
 * (This is one of many reasons Benchmarking should NEVER be enabled
 * in production builds.) */
int
fs_test_deltree (int32 *args, char *path)
{
  (void) args;

  return efs_deltree (path);
}

/* This function enumerates every file in a given path.
 * If args[0] is TRUE, then also stat each file. */
int
fs_test_enumerate (int32 *args, char *path)
{
  int do_stat = args[0];
  struct fs_dirent *dirent;
  EFSDIR *dirp;

  dirp = efs_opendir (path);
  if (!dirp)
    return -1;

  do {
    dirent = efs_readdir (dirp);

    if (dirent && do_stat) {
      struct fs_stat stat_buf;
      (void) efs_lstat (dirent->d_name, &stat_buf);
    }
  } while (dirent);

  efs_closedir (dirp);

  return 0;
}

int
fs_test_chown_recursive (int32 *args, char *path)
{
  return efs_chown_recursive (path, args[0], args[1]);
}

int
fs_test_chmod_recursive (int32 *args, char *path)
{
  return efs_chmod_recursive (path, args[0]);
}

/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
 * args[1] -> [IN]
      Size of the buffer. If 0 No buffering mode is used.
 */
int
fs_test_raw_setvbuf (int32 *args, char *path)
{
  EFS_EFILE * fp;
  int i = args[0];
  uint32 buf_size  = args[1];

  if (buf_size > STDIO_BUF_SIZE ||
      i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];
  if (buf_size)
  {
    if (efs_esetvbuf (fp, stdio_buffer[i], EFS_IOFBF, buf_size))
    {
      fs_benchmark_errno ("efs_setvbuf()",path);
      return -1;
    }
  }
  else
  {
    if (efs_esetvbuf (fp, NULL, EFS_IONBF, 0))
    {
      fs_benchmark_errno ("efs_setvbuf()",path);
      return -1;
    }
  }
  return 0;
}

/* This function reads from the file-stream.
 * args[0] -> [IN]
      Number returned by fs_test_raw_fopen() in args[2].
      Remember if this index is not valid and the file poitner
      is NULL it will flush all open streams.
  */
int
fs_test_raw_fflush (int32 *args, char *path)
{
  EFS_EFILE * fp;
  int i = args[0];

  if (i < 0 || i >= FS_BENCHMARK_MAX_STREAMS)
    return -1;

  fp = raw_efs_efile[i];

  if (efs_efflush (fp))
  {
    fs_benchmark_errno ("efs_efflush()",path);
    return -1;
  }

  return 0;
}

int
fs_test_utime (int32 *args, char *path)
{
  struct fs_utimbuf times;
  int result;

  times.actime = args[0];
  times.modtime = args[1];

  result = efs_utime (path, &times);
  if (result < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  return 0;
}


/* The following function is used to create a directory called 'bench' in
   the given path. All the safe benchmark tests are done inside 'bench'
   sub directory. If there is already a 'bench directory present, delete it.
   Note: This function updates fs_path_buff[0] */
static int
create_bench_dir (char *path)
{
  fs_strlcpy (fs_path_buff[0], path, sizeof(fs_path_buff[0]));
  fs_strlcat (fs_path_buff[0], "/bench", sizeof(fs_path_buff[0]));

  (void) efs_deltree (fs_path_buff[0]); /* Okay to fail */

  return efs_mkdir ((const char *) fs_path_buff[0], 0777);
}

/* This function deletes the 'bench' directory. It is called by the
   cleanup routine after the safe benchmark test is performed */
static int
cleanup_bench_dir (char *path)
{
  fs_strlcpy (fs_path_buff[0], path, sizeof(fs_path_buff[0]));
  fs_strlcat (fs_path_buff[0], "/bench", sizeof(fs_path_buff[0]));
  return efs_deltree (fs_path_buff[0]);
}

int
fs_safe_write_prepare (char *path)
{
  return create_bench_dir (path);
}

/* The following function writes data to the device for 'FS_BENCHMARK_TIME_MS'
   period of time */
int
fs_test_safe_write (int32 *args, char *path)
{
  int fd;
  uint64 start_time, time_elapsed;
  int write_size = BENCH_BUFFER_BYTES;
  int written;
  int failed = 0;

  (void) args; /* Ignored */

  fs_strlcpy (fs_path_buff[0], path, sizeof (fs_path_buff[0]));
  fs_strlcat (fs_path_buff[0], "/bench/bench_file", sizeof (fs_path_buff[0]));

  fd = efs_open (fs_path_buff[0], O_CREAT | O_RDWR, 0666);

  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  randomize_bench_buffer (fs_benchmark_seed);

  start_time = time_get_ms_native();

  do {

    written = efs_write (fd, bench_buffer, write_size);

    if (written > 0)
      total_bytes_write += written;
    else {
      fs_benchmark_errno ("efs_write()", path);
      failed = 1;
      break;
    }

    time_elapsed = time_get_ms_native() - start_time;

  } while (time_elapsed < FS_BENCHMARK_TIME_MS);

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    return -1;
  }

  return failed ? -1 : 0;
}

#define DELAY_BEFORE_READ_MS  3000  /* 3 seconds */
int
fs_safe_read_prepare (char *path)
{
  /* Prepare a file for reading... by writing it: */
  int rv =  (fs_safe_write_prepare (path)
             || fs_test_safe_write (NULL, path));

  /* Add delay before read to ensure all NOR block
   * erases as a result of GC have completed. */
  fs_util_wait (DELAY_BEFORE_READ_MS);

  return rv;
}

int
fs_test_safe_read (int32 *args, char *path)
{
  int fd;
  int read_count;
  int read_size = BENCH_BUFFER_BYTES;
  int failed = 0;

  (void) args;

  fs_strlcpy (fs_path_buff[0], path, sizeof (fs_path_buff[0]));
  fs_strlcat (fs_path_buff[0], "/bench/bench_file", sizeof(fs_path_buff[0]));

  fd = efs_open (fs_path_buff[0], O_RDWR, 0666 );

  if (fd < 0)
  {
    fs_benchmark_errno ("efs_open()", path);
    return -1;
  }

  do {
    read_count = efs_read (fd, read_buffer, read_size);

    if (read_count == -1)
    {
      fs_benchmark_errno ("efs_read()", path);
      failed++;
      break;
    }

    total_bytes_read += read_count;

  } while (read_count > 0);

  if (efs_close (fd))
  {
    fs_benchmark_errno ("efs_close()", path);
    failed++;
  }

  return failed ? -1 : 0;
}

/* The following function fills the subdirectory "bench" with files
   and subdirectories. Each subdirectory contains 10 files. All the
   subdirectories are named "subdir". */
int
fs_safe_enum_prepare (char *path)
{
  int fd;
  uint64 start_time, time_elapsed;
  unsigned file_counter = 0;

  create_bench_dir (path);

  start_time = time_get_ms_native(); /* Go! */

  do {
    snprintf (fs_path_buff[1], sizeof (fs_path_buff[1]),
              "%s/file%d", fs_path_buff[0], file_counter);

    fd = efs_open (fs_path_buff[1], O_CREAT | O_RDWR, 0666);

    if (fd < 0)
    {
      fs_benchmark_errno ("efs_open()", fs_path_buff[1]);
      return -1;
    }

    /* Just create the file. No need to write any data to it */

    if (efs_close (fd))
    {
      fs_benchmark_errno ("efs_close()", fs_path_buff[1]);
      return -1;
    }

    file_counter++;

    if (file_counter % 10 == 0) /* If there are 10 files in the directory */
    {
      fs_strlcat (fs_path_buff[0], "/subdir", sizeof (fs_path_buff[0]));

      /* Create a new directory */
      if (efs_mkdir (fs_path_buff[0], 0777))
        return -1;
    }

    time_elapsed = time_get_ms_native() - start_time;

  } while (time_elapsed < FS_BENCHMARK_TIME_MS);

  return 0;
}

int
fs_test_safe_enum (int32 *args, char *path)
{
  struct fs_dirent *dirent;
  EFSDIR *dirp;
  int found_dir = 0;
  struct fs_stat stat_buf;

  (void) args; /* Ignore always */

  /* Since this test doesn't change and read/write counters, we
     can use total_bytes_read to specify the number of subdirectories
     written to the device and total_bytes_written to specify the
     number of files enumerated. These counters need to be here
     since just before invoking this test these counters are cleared. */
  total_bytes_read = 0;
  total_bytes_write = 0;

  fs_strlcpy (fs_path_buff[0], path, sizeof (fs_path_buff[0]));
  fs_strlcat (fs_path_buff[0], "/bench", sizeof (fs_path_buff[0]));

  dirp = efs_opendir (fs_path_buff[0]);
  total_bytes_write++;

  if (!dirp)
    return -1;

  while (1)
  {
    dirent = efs_readdir (dirp);

    total_bytes_read++;

    if (!dirent)
    {
      if (!found_dir)
        break;

      /* This means we found a sub directory. Close the previous one
         and open the new sub directory */
      efs_closedir (dirp);

      fs_strlcat (fs_path_buff[0], "/subdir", sizeof (fs_path_buff[0]));

      dirp = efs_opendir (fs_path_buff[0]);
      total_bytes_write++;
      found_dir = 0;
    }
    else
    {
      if (!strcmp (dirent->d_name, ".") ||
          !strcmp (dirent->d_name, ".."))
        continue;

      if (FS_MASK_CHECK ( dirent->d_stats_present, FS_DIRENT_HAS_ST_MODE))
      {
        if (S_ISDIR (dirent->d_stat.st_mode))
          found_dir = 1;
      }
      else
      {
        fs_strlcpy (fs_path_buff[1], fs_path_buff[0],
                    sizeof (fs_path_buff[1]));
        fs_strlcat (fs_path_buff[1], "/" , sizeof(fs_path_buff[1]));
        fs_strlcat (fs_path_buff[1], dirent->d_name, sizeof(fs_path_buff[1]));

        if (efs_lstat (fs_path_buff[1], &stat_buf))
          return -1;

        if (S_ISDIR (stat_buf.st_mode))
          found_dir = 1;
      }
    }
  }

  efs_closedir (dirp);

  return 0;
}

int
fs_test_babylon_short_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];

  (void) path;
  EFS_BAB_START (EFS_BAB_TESTING);
  while (count--) {
    /* (Type: BAB_TYPE_SINGLE_EVENT) << 8 | (Stream: BAB_STREAM_EFS)
     * (Extended: EFS_BAB_TEST) << 8 | count */
    BAB_SINGLE_EVENT (BAB_STREAM_EFS, /* Stream */
                      (count & 0xFF), /* Event ID */
                      EFS_BAB_TEST); /* Extended */
    }
  EFS_BAB_END (EFS_BAB_TESTING);
#endif

  return 0;
}

int
fs_test_babylon_speed_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];
  int cycle;

  (void) path;

  if (count == 0)
    count = 1;

  /* Single Event */
  for (cycle = count; cycle; cycle--)
  {
    EFS_BAB_API (EFS_BAB_TESTING);
    EFS_BAB_API (EFS_BAB_TESTING);
  }

  /* Markers */
  for (cycle = count; cycle; cycle--)
  {
    EFS_BAB_START (EFS_BAB_TESTING);
    EFS_BAB_END (EFS_BAB_TESTING);
    EFS_BAB_START (EFS_BAB_TESTING);
    EFS_BAB_END (EFS_BAB_TESTING);
  }

  /* 16-bit value */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, FALSE);
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, FALSE);
  }

  /* 32-bit value */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, TRUE);
    EFS_BAB_VALUE (EFS_BAB_TESTING, cycle, TRUE, TRUE, TRUE);
  }

  /* Fixed string */
  for (cycle = count; cycle; cycle--) {
    EFS_BAB_PRINT (EFS_BAB_TESTING, "Fixed A");
    EFS_BAB_PRINT (EFS_BAB_TESTING, "Fixed B");
  }
#endif
  return 0;
}

int
fs_test_babylon_word_test (int32 *args, char *path)
{
#ifndef FEATURE_BABYLON
  (void) args;
  (void) path;
#else
  int count = args[0];
  int start = args[1];

  (void) path;
  EFS_BAB_START (EFS_BAB_TESTING);
  while (count--) {
    EFS_BAB_VALUE (EFS_BAB_TESTING,
                      start++ & 0xFFFF,
                   TRUE,        /* decimal */
                   FALSE,       /* unsigned  */
                   FALSE);      /* 16-bit */
  }
  EFS_BAB_END (EFS_BAB_TESTING);
#endif

  return 0;
}

int
fs_test_babylon_port_test (int32 *args, char *path)
{
  (void) args;
  (void) path;
#ifdef FEATURE_BABYLON
  bab_port_test();
#endif

  return 0;
}

/* Exists in fs_public.c but we don't want to publish it. */
extern int fs_dummy_protected_var;
int
fs_test_memory_protection (int32 *args, char *path)
{
  (void) args;
  (void) path;

  fs_dummy_protected_var++;
  return -1;
}


/* ============== Task Handling ============== */

/* Here is the map of command codes to test functions */
const struct fs_benchmark_test fs_benchmark_tests[] = {

  /* Most benchmark tests are "unsafe" or security-violating, so they
   * should only be accessible when requested. */
#ifdef FEATURE_FS_UNSAFE_BENCHMARK
  // Code   Function to call for test       Prep  Cleanup
  { 0x0001, fs_test_sleep_ms,               NULL, NULL },
  { 0x0002, fs_test_nop,                    NULL, NULL },
  { 0x0003, fs_test_df,                     NULL, NULL },
  { 0x0004, fs_test_write,                  NULL, NULL },
  { 0x0005, fs_test_set_cpu_hog,            NULL, NULL },
  { 0x0006, fs_test_random_write,           NULL, NULL },
  { 0x0007, fs_test_truncate,               NULL, NULL },
  { 0x0008, fs_test_fill_fs_to_pct,         NULL, NULL },
  { 0x0009, fs_test_delete,                 NULL, NULL },
  { 0x000A, fs_test_mkdir,                  NULL, NULL },
  { 0x000B, fs_test_rmdir,                  NULL, NULL },
  { 0x000C, fs_test_statvfs,                NULL, NULL },
  { 0x000D, fs_test_flush_deleted_data,     NULL, NULL },
  { 0x000E, fs_test_read,                   NULL, NULL },
  { 0x000F, fs_test_write_efs1,             NULL, NULL },
  { 0x0010, fs_test_read_efs1,              NULL, NULL },
  { 0x0011, fs_test_write_efs1_hash,        NULL, NULL },
  { 0x0012, fs_test_read_efs1_hash,         NULL, NULL },
  { 0x0013, fs_test_hotplug_poll_interval,  NULL, NULL },
  { 0x0014, fs_test_hotplug_format,         fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0015, fs_test_hotplug_read,           fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0016, fs_test_hotplug_write,          fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0017, fs_test_hotplug_check_media,    NULL, NULL },
  { 0x0018, fs_test_hotplug_lock,           NULL, NULL },
  { 0x0019, fs_test_hotplug_unlock,         NULL, NULL },
  { 0x001A, fs_test_raw_open,               NULL, NULL },
  { 0x001B, fs_test_raw_read,               NULL, NULL },
  { 0x001C, fs_test_raw_write,              NULL, NULL },
  { 0x001D, fs_test_raw_close,              NULL, NULL },
  { 0x001E, fs_test_stuff_pattern,          NULL, NULL },
  { 0x001f, fs_test_check_pattern,          NULL, NULL },
  { 0x0020, fs_test_raw_fopen,              NULL, NULL },
  { 0x0021, fs_test_raw_fclose,             NULL, NULL },
  { 0x0022, fs_test_raw_fwrite,             NULL, NULL },
  { 0x0023, fs_test_raw_fread,              NULL, NULL },
  { 0x0024, fs_test_privatedir_set_access,  NULL, NULL },
  { 0x0025, fs_test_reset,                  NULL, NULL },
  { 0x0026, fs_test_deltree,                NULL, NULL },
  { 0x0027, fs_test_enumerate,              NULL, NULL },
  { 0x0028, fs_test_chown_recursive,        NULL, NULL },
  { 0x0029, fs_test_chmod_recursive,        NULL, NULL },
  { 0x002A, fs_test_raw_setvbuf,            NULL, NULL },
  { 0x002B, fs_test_raw_fflush,             NULL, NULL },
  { 0x002C, fs_test_utime,                  NULL, NULL },
  { 0x0032, fs_test_babylon_short_test,     NULL, NULL },
  { 0x0033, fs_test_babylon_word_test,      NULL, NULL },
  { 0x0034, fs_test_hotplug_random_write,   fs_test_hotplug_setup,
                                               fs_test_hotplug_teardown },
  { 0x0035, fs_test_babylon_speed_test,     NULL, NULL },
  { 0x0036, fs_test_babylon_port_test,      NULL, NULL },
  { 0x0037, fs_test_memory_protection,      NULL, NULL },
#endif

  /* These functions are considered "safe", and should be permitted on
   * any commercial phone, for use by any user. */
  /* ---------- START OF SAFE TESTS  ---------- */
  { 0x0080,
    fs_test_safe_read, fs_safe_read_prepare, cleanup_bench_dir },
  { 0x0081,
    fs_test_safe_write, fs_safe_write_prepare, cleanup_bench_dir },
  { 0x0082,
    fs_test_safe_enum, fs_safe_enum_prepare, cleanup_bench_dir },
  { 0x0083, fs_test_hotplug_format,         NULL, NULL },
  /* ---------- END OF SAFE TESTS ---------- */

  { 0x0000, NULL, NULL, NULL}   /* Must be last! */
};


static q_type cmd_q;            /* Command queue for benchmark task */
static q_type cmd_free_q;       /* Free command queue. */
static q_type rpt_q;            /* Report queue */
static q_type rpt_free_q;       /* Free report queue */

static fs_benchmark_queued_cmd cmds[FS_BENCHMARK_MAX_CMDS];
static fs_benchmark_queued_rpt rpts[FS_BENCHMARK_MAX_RPTS];
static void fs_benchmark_do_cmd (fs_benchmark_queued_cmd *cmd);
static void process_next_benchmark_cmd (void);

/*
 * Receives a single command packet from DIAG and queues it for processing
 * Returns 0 on success.
 */
int
fs_benchmark_put_cmd (fs_benchmark_test_cmd *cmd)
{
  fs_benchmark_queued_cmd *new_cmd;

  if (!cmd)
    return 1;

  /* Make sure they didn't give us an enormous path */
  if ((strlen ((const char *) cmd->path) + 1) > sizeof (new_cmd->whole_path))
    return 2;

  /* Get a new command item */
  new_cmd = (fs_benchmark_queued_cmd *) q_get (&cmd_free_q);
  if (new_cmd == NULL)
    return 3;

  /* Copy the command packet and the path */
  new_cmd->cmd = *cmd;
  fs_strlcpy (new_cmd->whole_path, (const char *) cmd->path,
                                    sizeof (new_cmd->whole_path));

  /* Queue the new command */
  q_link (new_cmd, &new_cmd->hdr.link);
  q_put (&cmd_q, &new_cmd->hdr.link);

  /* Signal the benchmark task */
  fs_os_set_sigs (&fs_benchmark_tcb, FS_BENCHMARK_CMD_Q_SIG);

#ifdef FS_UNIT_TEST
#error code not present
#endif

  return 0;
}

/* Dequeue a single command packet from our benchmark queue and process it */
static void
process_next_benchmark_cmd (void)
{
  fs_benchmark_queued_cmd *got_cmd;

  test_running = 1;
  while ((got_cmd = (fs_benchmark_queued_cmd *) q_get (&cmd_q)) != NULL)
  {
    if (!flushing)
      fs_benchmark_do_cmd (got_cmd);

    /* return command buffer to free pool */
    if (got_cmd->hdr.done_q_ptr)
      q_put (got_cmd->hdr.done_q_ptr, &got_cmd->hdr.link);
  }
  flushing = 0;
  test_running = 0;
}

/* Takes a report off the rpt_q, copies it to the user's provided
   report buffer, and returns the status of the queue.

   Note that this allocates ONE static report buffer, and
   returns a pointer to it.  Use while fresh!
*/
enum fs_benchmark_queue_status_t
fs_benchmark_get_rpt (fs_benchmark_report *report)
{
  fs_benchmark_queued_rpt *rpt_ptr;

  rpt_ptr = q_get (&rpt_q);
  if (rpt_ptr)
  {
    *report = rpt_ptr->report;  /* Copy the whole structure */
    /* Free the report */
    if (rpt_ptr->hdr.done_q_ptr)
      q_put (rpt_ptr->hdr.done_q_ptr, &rpt_ptr->hdr.link);
    return FS_BENCHMARK_RESULT;
  }
  else
  {
    /* We have no report to give */
    memset ((void *)report, 0, sizeof(*report));

    if (!test_running && !q_cnt(&cmd_q))
      return FS_BENCHMARK_IDLE;
    else
      return FS_BENCHMARK_RUNNING; /* Please come again */
  }
}

/*
  Stops all currently running tests; however, the single test running
  at the time this function is called does not get interrupted, as the
  execution might have gone into filesyestem code already
*/
void
fs_benchmark_flush (void)
{
    flushing = 1;
}

/* Main work loop to process one test command */
static void
fs_benchmark_do_cmd (fs_benchmark_queued_cmd *cmd)
{
  fs_benchmark_queued_rpt *rpt;
  const struct fs_benchmark_test *test = fs_benchmark_tests;

  if (!cmd || !cmd->cmd.command)
    return;                     /* ??? */

  /* Search for a matching function call for this cmd */
  while (test->func != NULL)
  {
    if (test->cmd == cmd->cmd.command)
      break;
    test++;
  }
  /* No Match? test now points to the null test */

  /* If only the safe tests are allowed, validate the path by making
   * sure it is exactly one of these allowed strings: */
#ifndef FEATURE_FS_UNSAFE_BENCHMARK
  if (strcmp (cmd->whole_path, "/") &&
      strcmp (cmd->whole_path, "/mmc1") &&
      strcmp (cmd->whole_path, "/usb1") &&
      strcmp (cmd->whole_path, "/ftl1"))
    return;
#endif

  /* Process the test command */
  {
    int failure_count;          /* Number of times a test failed */
    uint64 stopwatch;
    uint16 cycles_remaining;

    failure_count = 0;

    memset (&fs_counter, 0, sizeof(fs_counter)); /* Reset counters */
    cycles_remaining = cmd->cmd.repeat;

    /* Unknown tests (not listed in table) return a failure */
    if (test->func == NULL) {
      ZPRINTF ("Unrecognized test command %d", cmd->cmd.command);
      cycles_remaining = 0;
      failure_count = 1;
    }

    /* Do any necessary prep before starting the actual test.
       eg. create/delete directories */
    if (test->prepare && (test->prepare (cmd->whole_path) != 0))
    {
      ZPRINTF ("Test prepare failed! %d", cmd->cmd.command);
      cycles_remaining = 0;
      failure_count = 1;
    }

    /* Forget about any read/write counts during preparation */
    total_bytes_write = 0;
    total_bytes_read = 0;

    stopwatch = time_get_ms_native(); /* Go! */
    while (cycles_remaining && !flushing)
    {
      int result;
      int32 args[FS_BENCHMARK_ARGS];
      int i;
      ZPRINTF ("Starting test 0x%04X", test->cmd);

      /* The cmd.args[] are PACKED, and likely misaligned.
       * We have to copy them out into native words */
      for (i = 0; i < FS_BENCHMARK_ARGS; i++)
        args[i] = cmd->cmd.args[i];
      EFS_BAB_START (EFS_BAB_BENCHMARK_TEST);
      result = test->func (args, cmd->whole_path);
      EFS_BAB_END (EFS_BAB_BENCHMARK_TEST);

      /* Because some commands (like df) can pass values back through args,
       * we have to copy them back into the Diag command. */
      for (i = 0; i < FS_BENCHMARK_ARGS; i++)
        cmd->cmd.args[i] = args[i];

      if (result != 0)
        failure_count++;

      cycles_remaining--;
    }
    stopwatch = time_get_ms_native() - stopwatch; /* Stop! */

    /* Do any necessary cleanup after the test is done. */
    if (test->cleanup && (test->cleanup (cmd->whole_path) != 0))
    {
      ZPRINTF ("Test cleanup failed! %d", cmd->cmd.command);
      failure_count++;
    }

    rpt = (fs_benchmark_queued_rpt *) q_get (&rpt_free_q);
    if (!rpt) {
      ZAP ("Failed to get report buffer?");
    } else {
      /* fill the report buffer */
      fs_benchmark_report *r = &rpt->report;

      r->cmd = cmd->cmd;        /* Copy whole command to report */
      r->failures = failure_count; /* Record any errors */

      if (stopwatch > 0xFFFFFFFF)
        r->milliseconds = 0xFFFFFFFF;
      else
        r->milliseconds = (uint32) stopwatch;

      r->total_bytes_write           = total_bytes_write;
      r->total_bytes_read            = total_bytes_read;
      r->flash_write_partial_counter = fs_counter.flash_write_partial_counter;
      r->flash_erase_block_counter   = fs_counter.flash_erase_block_counter;
      r->flash_erase_suspend_counter = fs_counter.flash_erase_suspend_counter;

      /* I'm not proud of this.. because of the fixed nature of the
         Hotplug diag packet, we can't add new fields easily.  So, for
         the sake of benchmarking hotplug, we substitute the blockdev
         counters for flash counters if it's obvious the test did only
         non-flash operations */
      if (fs_counter.flash_write_page_counter == 0
          && fs_counter.flash_read_page_counter == 0
          && (fs_counter.blockdev_write_sector_counter != 0
              || fs_counter.blockdev_read_sector_counter != 0))
      {
        r->flash_write_page_counter = fs_counter.blockdev_write_sector_counter;
        r->flash_read_page_counter = fs_counter.blockdev_read_sector_counter;
      }
      else
      {
        r->flash_write_page_counter   = fs_counter.flash_write_page_counter;
        r->flash_read_page_counter    = fs_counter.flash_read_page_counter;
      }

      /* Queue the response in the report queue */
      q_link (rpt, &rpt->hdr.link);
      q_put (&rpt_q, &rpt->hdr.link);
    }
  }
}

/* The benchmark task loop */
static void
fs_benchmark_task (dword parm)
{
  fs_os_sigs_type task_sigs;
  unsigned int i;

  (void) parm;

  /* Init */
  fs_os_def_timer (&fs_benchmark_sleep_timer,
                   &fs_benchmark_tcb,
                   FS_BENCHMARK_SLEEP_TIMER_SIG);

  ran_seed (fs_benchmark_seed);
  for (i = 0; i < BENCH_BUFFER_DWORDS; i++)
    bench_buffer[i] = ran_next();
  fs_benchmark_seed = ran_next();

  ZAP ("Started fs_benchmark_task.");

  /* Loop forever handling commands. */
  while (1) {
    task_sigs = fs_os_wait (TASK_STOP_SIG |
                            TASK_OFFLINE_SIG |
                            FS_BENCHMARK_CMD_Q_SIG);

    /* If asked to go offline, acknowledge, and continue. */
    if (task_sigs & TASK_OFFLINE_SIG)
    {
      ZAP ("TASK_OFFLINE_SIG");
      fs_os_clr_sigs (&fs_benchmark_tcb, TASK_OFFLINE_SIG);
      task_offline ();
    }

    /* If asked to powerdown, acknowledge. */
    if (task_sigs & TASK_STOP_SIG)
    {
      ZAP ("TASK_STOP_SIG");
      fs_os_clr_sigs (&fs_benchmark_tcb, TASK_STOP_SIG);
      task_stop ();
    }

    /* when new command arrives */
    if (task_sigs & FS_BENCHMARK_CMD_Q_SIG)
    {
      fs_os_clr_sigs (&fs_benchmark_tcb, FS_BENCHMARK_CMD_Q_SIG);
      process_next_benchmark_cmd ();
    }
  }
}

/* Benchmark init function;
   initializes the cmd_q, cmd_free_q, rpt_q and rpt_free_q
   Also starts the benchmark task running */
void
fs_benchmark_init (void)
{
  static int fs_benchmark_init_complete = 0;
  int i;

  /* Only init once */
  if (!fs_benchmark_init_complete) {
    fs_benchmark_init_complete = 1;

    /* ------------- Initialize Command Buffers --------------- */
    (void) q_init (&cmd_q);
    (void) q_init (&cmd_free_q);

    for (i = 0; i < FS_BENCHMARK_MAX_CMDS; i++)
    {
      cmds[i].hdr.done_q_ptr = &cmd_free_q;
      q_put (&cmd_free_q, q_link (&cmds[i], &cmds[i].hdr.link));
    }

    /* ------------- Initialize Report Buffers --------------- */
    (void) q_init (&rpt_q);
    (void) q_init (&rpt_free_q);

    for (i = 0; i < FS_BENCHMARK_MAX_RPTS; i++)
    {
      rpts[i].hdr.done_q_ptr = &rpt_free_q;
      q_put (&rpt_free_q, q_link (&rpts[i], &rpts[i].hdr.link));
    }

    fs_os_def_task_ext (&fs_benchmark_tcb,
                        fs_benchmark_stack,
                        FS_BENCHMARK_STACK_SIZE,
                        FS_BENCHMARK_PRI,
                        fs_benchmark_task,
                        0,
                        "FS Benchmark",
                        0);
  }
}


/* A task that tries to hog a specified amount of CPU. */
static void
fs_cpu_hog_task (dword parm)
{
  static int fs_cpu_hog_dummy_var = 0;
  uint64 t0, t1;

  (void) parm;

  fs_os_def_timer (&fs_cpu_hog_timer,
                   &fs_cpu_hog_tcb,
                   FS_CPU_HOG_TIMER_SIG);

  while (1) {
    /* Work */
    t0 = time_get_ms_native();  /* get start time */
    do {
      int i;
      for (i = 0; i < 10000; i++) /* waste valuable computations */
        fs_cpu_hog_dummy_var++;

      t1 = time_get_ms_native();  /* get current time */
    } while ((t1 - t0) < fs_cpu_hog_run_ms);

    /* Then sleep */
    fs_os_clr_sigs (&fs_cpu_hog_tcb, FS_CPU_HOG_TIMER_SIG);
    fs_os_set_timer (&fs_cpu_hog_timer, fs_cpu_hog_sleep_ms);
    fs_os_wait (FS_CPU_HOG_TIMER_SIG);
  } /* Repeat endlessly, and try to find time to smell the flowers */
}

/* Initialization function for the CPU Hog task, must be called once. */
void
fs_cpu_hog_init (void)
{
  static int cpu_hog_initialized = 0;

  if (!cpu_hog_initialized) {
    /* Start up the fs_cpu_hog task */
    fs_os_def_task_ext (&fs_cpu_hog_tcb,
                       fs_cpu_hog_stack,
                       FS_CPU_HOG_STACK_SIZ,
                       FS_CPU_HOG_PRI,
                       fs_cpu_hog_task,
                       0,
                       "FS CPU Hog",
                       FALSE);
    cpu_hog_initialized = 1;
  }
}

#else  /* !FEATURE_FS_BENCHMARK */
/*
 * If FEATURE_FS_BENCHMARK is NOT defined, provide failing stubs for
 * all public functions, to avoid #ifdefs elsewhere.
 */
void fs_benchmark_init (void) {}
void fs_cpu_hog_init (void) {}
void fs_benchmark_flush (void) {}

int  fs_benchmark_put_cmd (fs_benchmark_test_cmd *cmd)
{
  (void) cmd;

  return -1;                    /* Always Fail */
}

enum fs_benchmark_queue_status_t
fs_benchmark_get_rpt (fs_benchmark_report *report)
{
  (void) report;

  return FS_BENCHMARK_IDLE;
}

#endif /* !FEATURE_FS_BENCHMARK */
