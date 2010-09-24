#!perl -w
#
# Given Lint output file, count number of Lint issues
# in given subsystem files and print Lint statistics
# Intended to be run in builds where Lint is integral part of the build,
# ie. lintg / lintf / etc commands are supported 
#
# To run:
# lintg <build flavor> <subsystem> | perl lint_stats_txt.pl <subsystem> 
#
# Example:
# cd build/ms
# ads12.bat or rvct21.bat
# lintg SMAAR usb | perl lint_stats_txt.pl usb 
#
# Prints the following:
# ------------------------------------------------------------------
#
# NAME             INFOS   WARNINGS    ERRORS    FATALS    TOTALS
#
# ------------------------------------------------------------------
# <file a>.c            5        13         1         0        19
# <file b>.c            0         0         0         0         0
# [ ... ]
# ------------------------------------------------------------------
# TOTALS                X        X         X          X        X
# ------------------------------------------------------------------

#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# $Header: //depot/asic/qsc1100/build/ms/lint_stats_txt.pl#1 $
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 10/26/05   ic      Corrected regex to match on subsystems where source files
#                    sit below subsystem directory, like for example 
#                    ..\..\services\mgp\libs\calib\calib.c
# 10/19/05   ic      Initial revision 
#==============================================================================
use strict; 

my $key       = ""; 
my $total     = 0;
my $errors    = 0;
my $warnings  = 0;
my $infos     = 0;
my $fatals    = 0;
my $file_name = "";
my %w_count   = ();
my %i_count   = ();
my %e_count   = ();
my %f_count   = ();
my %t_count   = ();

# Assume subsystem to lint is given as first argument
my $subsystem = $ARGV[0];

# Drop subsystem so now we can read from file or pipe 
shift @ARGV;

print "Searching for lint issues in subsystem $subsystem ...\n";

# Read from the input file or pipe and process each line
while(<>) 
{

  # Lines in Lint output with -v option look like for example
  # ..\..\drivers\usb\usbotg.c(717) : Error (Info) 715: Symbol 'parm' (line 518) not referenced  
  # or
  #  : Error (Info) 766: Header file '..\..\services\task\task.h' not used in module '..\..\drivers\usb\usbtest.c'
  # so first match on <subsystem>\*.c to find your .c file
  if($_ =~ /\\$subsystem\\?.*\\(\w+\.c)/)
  {
        
    # We found first .c file
    if($file_name eq "")
    {
      $file_name      = $1;
    }
  
    if($file_name ne $1)
    {
      # We've found next .c file
      # Print totals for the file we just finished processing
      # Reset variables
      $total = $errors + $warnings + $infos +$fatals;

      $w_count{$file_name} += $warnings;
      $i_count{$file_name} += $infos;
      $e_count{$file_name} += $errors;
      $f_count{$file_name} += $fatals;
      $t_count{$file_name} += $total;
      $file_name       = $1;
      $errors          = 0;
      $warnings        = 0;
      $infos           = 0;
      $fatals          = 0;
    }

    # Count Lint errors, warnings, info for current file
    # If there are any, the next line will look something like
    # ..\..\drivers\usb\usbotg.c(717) : Error (Info) 715: Symbol 'parm' (line 518) not referenced  
    # or
    #  : Error (Info) 766: Header file '..\..\services\task\task.h' not used in module '..\..\drivers\usb\usbtest.c'
    # when a function is not used across the entire file in some configuration
    #
    # So just match on (Error) / (Warning) / (Info) / (Fatal) strings 
    # in case we see Lint issues printed in some other format than the 
    # two above 
    # 
    if($_ =~ /\(Error\)/)
    {
      $errors++;
    }
    if($_ =~ /\(Warning\)/)
    {
      $warnings++;
    }
    if($_ =~ /\(Info\)/)
    {
      $infos++;
    }
    if($_ =~ /\(Fatal\)/)
    {
      $fatals++;
    }
  } # if we found .c file with Lint issues
} # while

# Print totals for the last file
$total = $errors + $warnings + $infos +$fatals;

$w_count{$file_name} += $warnings;
$i_count{$file_name} += $infos;
$e_count{$file_name} += $errors;
$f_count{$file_name} += $fatals;
$t_count{$file_name} += $total;

$infos    = 0;
$warnings = 0;
$errors   = 0;
$fatals   = 0;
$total    = 0;


foreach $key ( sort keys %t_count )
{

format STDOUT_TOP =
------------------------------------------------------------------------------

NAME                  INFOS       WARNINGS    ERRORS      FATALS      TOTALS

------------------------------------------------------------------------------
.

format  STDOUT    =
@<<<<<<<<<<<<<< @>>>>>>>    @>>>>>>>    @>>>>>>>    @>>>>>>>    @>>>>>>>
$key,      $i_count{$key}, $w_count{$key},  $e_count{$key}, $f_count{$key}, $t_count{$key}
.

      write;
      $infos    += $i_count{$key};
      $warnings += $w_count{$key};
      $errors   += $e_count{$key};
      $fatals   += $f_count{$key};
      $total    += $t_count{$key};

   }

format  TOTALS    =
------------------------------------------------------------------------------
TOTALS          @>>>>>>>    @>>>>>>>    @>>>>>>>    @>>>>>>>    @>>>>>>>
$infos, $warnings,  $errors, $fatals, $total
------------------------------------------------------------------------------
.

$~ = "TOTALS";
write;


