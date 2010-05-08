@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!perl -w
#line 15
#==============================================================================
#  This utility transforms an input file in A169x v1.4 output format to 
#  A169x v1.0 format. The motivation for the conversion is because hp_stats.exe 
#  understands A169x v1.0 format, but not that of A169x v1.4
#
# Each line of the input  file contains fields [$LA_1_4_Header]
# Each line of the output file contains fields [$LA_1_0_Header]
# 
#
#  RELEASE INFORMATION
#     DATE        WHO         REASON
#  01/05/04    rmalli   Created the first version
#  01/22/04    rmalli   Added code to detect huge time differences between
#                       successive samples
#  01/27/04    rmalli   Using "My Signal 1" value instead of "My Bus 1" value
#==============================================================================

# Use / Require
use strict;          # Mandatory package inclusion
use diagnostics;
use warnings;

# Globals
my $LA_1_0_Header = "Time,Sample,Trigger,allData,Pod1:Ch15,Pod1:Ch14,Pod1:Ch13,Pod1:Ch12,Pod1:Ch11,Pod1:Ch10,Pod1:Ch9,Pod1:Ch8,Pod1:Ch7,Pod1:Ch6,Pod1:Ch5,Pod1:Ch4,Pod1:Ch3,Pod1:Ch2,Pod1:Ch1,Pod1:Ch0,Pod1:Clk0,";
my $LA_1_4_Header = '"?Sample Number"?,"?My Bus 1"?,"?My Bus 2"?,"?My Signal 1"?,"?My Signal 2"?,"?Time"?';

my $NO_ERROR      = 0;
my $VALUE_ERROR   = -65;
my $FAILED        = -1;

my $MAX_TIME_DIFF = 2;  # Maximum allowed time between any two samples

# Validate command-line
exit Usage() if (@ARGV != 2);

# Read in the contents of the input file
my $inFile = $ARGV[0];
ErrorNExit("Unable to open [$inFile] for reading", $VALUE_ERROR) 
   if !open(RFH, "<$inFile"); 
my @fileContents = <RFH>;
close(RFH);

# Validate input file format
my $firstLine = shift(@fileContents);
ErrorNExit("Incorrect File Format", "First line is not [$LA_1_4_Header]", $VALUE_ERROR) 
   if ($firstLine !~ /$LA_1_4_Header/);

# Create output file
my $outFile = $ARGV[1];
ErrorNExit("Unable to open [$outFile] for writing", $FAILED)
   if !open(WFH, ">$outFile"); 

# Transform A169x v1.4 format to A169x v1.0 format
# The last line may not always be full, so skip it if it is incomplete
pop(@fileContents) 
   if ((my @a = split(/,/, $fileContents[-1])) != (my @b = split(/,/, $LA_1_4_Header)));         

# Print header line to WFH
print WFH "$LA_1_0_Header\n";

# Print the remaining lines to WFH
my $lastSampleTime;

my $sample = 0;
my $need_zero_id = 1;
foreach my $line (@fileContents) {
   chomp($line);
   my @fields = split(/,/, $line);
      
   # "My Bus 1" column ($fields[1]) occasionally does not contain the correct value.
   # So, get $allData from "My Signal 1" ($fields[3]). Ideally "My Bus 1"
   # value and "My Signal 1" value should be identical                                                                               
   my ($time, $allData) = (TimeToSecs($fields[-1]), $fields[3]);
   my $allDataExpanded = ExpandAllData($allData) .",1"; # Final 1 is clock info

   if ($need_zero_id && hex($allData) != 0)
   {
     my $zid_time    = $time - 0.0000000001;
     my $zid_allData = 0;
     my $zid_allDataExpanded = ExpandAllData($zid_allData) .",1";
     printf(WFH "%2.10f,%d,%d,%s,%s\n", $zid_time, $sample, 0, $zid_allData, $zid_allDataExpanded);
     $sample++;
   }
   
   # The third item is trigger which is always set to 0
   printf(WFH "%2.10f,%d,%d,%s,%s\n", $time, $sample, 0, $allData, $allDataExpanded);
   $sample++;

   $need_zero_id = 1 if (hex($allData) != 0);
   
   # Detect if time difference between samples is within acceptable limits.
   # If not, print to STDERR and continue
   if (defined $lastSampleTime) {
      my $timeDiff = $time - $lastSampleTime;
      printf(STDERR "Unacceptable time difference of [%2.10f] secs between samples %d and %d. " .
                    "Please check LA connectivity to the SURF's debug port\n",
             $timeDiff, $sample-1, $sample) if ($timeDiff > $MAX_TIME_DIFF); 
   }
   $lastSampleTime = $time;
}

close(WFH);
exit $NO_ERROR;

# Function to convert input of the form 
#  <Time> s | <Time> ms | <Time> us | <Time> ns | <Time>
# to value in seconds without any unit suffix
sub TimeToSecs {
   my $time = shift;

   return $1                  if ($time =~ /(.+?) s$/);
   return ($1/1000.0)         if ($time =~ /(.+?) ms$/);
   return ($1/1000000.0)      if ($time =~ /(.+?) us$/);
   return ($1/1000000000.0)   if ($time =~ /(.+?) ns$/);

   return $time;
}

# Function to convert input hex value to its binary representation 
# (16 bits) with the bits separated from one another using commas
# Eg: 14 (decimal 20) is 0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0
sub ExpandAllData {
   my $allData = shift;

   $allData = hex($allData);
   my @bits = ();
   for (1..16) {
      unshift(@bits, $allData % 2);
      $allData /= 2;
   }

   return join(',', @bits);
}

sub Usage {
   my $usage = <<USAGE_END;
Usage: $0 <InputFileName> <OutputFileName>

This utility transforms an input file in A169x v1.4 format to A169x v1.0 format. 
The motivation for the conversion is because hp_stats.exe understands A169x v1.0 
format, but not that of A169x v1.4

Each line of the input  file (A169x v1.4 format) contains fields 
[$LA_1_4_Header]

Each line of the output file (A169x v1.0 format) contains fields 
[$LA_1_0_Header]
USAGE_END

   print STDERR $usage;
   return $VALUE_ERROR;
}

sub ErrorNExit {
   my $exitCode = pop(@_);   
   print STDERR join("\n", @_);   
   exit $exitCode;
}

#==============================================================================
# QUALCOMM Proprietary                                                         
# 
# Export of this software and/or technology may be controlled 
# by the U.S. Government.  Diversion contrary to U.S. law 
# prohibited.             
#
# Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.            
#                                                                              
# All data and information contained in or disclosed by this document          
# is confidential and proprietary information of QUALCOMM Incorporated         
# and all rights therein are expressly reserved.  By accepting this            
# material the recipient agrees that this material and the information         
# contained therein is held in confidence and in trust and will not be         
# used, copied, reproduced in whole or in part, nor its contents               
# revealed in any manner to others without the express written                 
# permission of QUALCOMM Incorporated.       
##==============================================================================

__END__
:endofperl
