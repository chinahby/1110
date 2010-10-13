#!/usr/bin/perl -w
# ------------------------------------------------------------------------
# AMSS Display and Diff Time
# Copyright (c) 2003 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/btime.pl#1 $
#
# - Shows current time
# - Writes start/end time to spec'd file
# - Calc's difference in time for a start/end
# - Pretty prints start/end/lap time for use in build environments
# ------------------------------------------------------------------------

# Indicates if -start
#
my $StartFile = "";

# Indicates if -end
#
my $EndFile   = "";

# Indicates whether to also write a diff time to stdout
#
my $opt_p = "";

# Indicates whether to "pretty print" output to the screen
#   for use in the AMSS build environment
#
my $opt_b = "";

# Months
#
my @Months = qw {
   January
   February
   March
   April
   May
   June
   July
   August
   September
   October
   November
   December
};

# ------------------------------------------------------------------------
# SUBROUTINE: Usage
#
# ------------------------------------------------------------------------
sub Usage
{
   ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
   $year += 1900;

   print <<usage;
$0 - Copyright (c) $year QUALCOMM Inc.
% perl $0 [-help][-start <file>, -end <file>][-p]
  <no arguments>   : Current date and time
  -help            : This message
  -start <file>    : Write a start time to the named file
                     - always creates a new file
  -end <file>      : Write an end time to the named file and display the difference
                       if the file contains a start time
                     - always appends end times to file
  -p               : Whether to write a diff time to stdout
usage
   return;
}

# ------------------------------------------------------------------------
# SUBROUTINE: ProcessCommandLine
#
# ------------------------------------------------------------------------
sub ProcessCommandLine
{
   for( $i=0; $i<$#ARGV+1; $i++)
   {
      my $arg = $ARGV[$i];

      if ( $arg eq "-help" )
      {
         Usage();
         exit;
      }

      elsif ( $arg eq "-start" )
      {
         $i++;
         $StartFile = $ARGV[$i];
      }

      elsif ( $arg eq "-end" )
      {
         $i++;
         $EndFile  = $ARGV[$i];
      }

      elsif ( $arg eq "-p" )
      {
         $opt_p = "stdout";
      }

      elsif ( $arg eq "-b" )
      {
         $opt_b = "stdout";
      }

      else
      {
         print "$arg not understood.\n";
         Usage();
         exit;
      }

   }

   if( ($StartFile ne "") && ($EndFile ne "") )
   {
      print "Cannot specify both -start and -end at the same time.\n";
      Usage();
      exit;
   }
   return;
} # ProcessCommandLine


# ------------------------------------------------------------------------
# MAIN SCRIPT
#
# ------------------------------------------------------------------------

ProcessCommandLine();

$rtime = time;
($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
$year += 1900;
$Time  = sprintf "%02d:%02d:%02d - %d %s %d\n", $hour,$min,$sec,$mday,$Months[ $mon ],$year;

my $TTime = $Time;

if( ($StartFile eq "") && ($EndFile eq "") )
{
   print $Time;
}
elsif( $StartFile ne "" )
{
   open( TFILE, ">$StartFile" ) or die "Failed to create file $StartFile\n";
   print TFILE "$rtime START: $Time";
   close( TFILE );

   if( $opt_b ne "" )
   {
      print "Build started at $Time";
   }
}
elsif( $EndFile ne "" )
{
   open( TFILE, ">>$EndFile" ) or die "Failed to open/create file $EndFile\n";
   print TFILE "$rtime END:   $Time";
   close( TFILE );

   open( TFILE, "<$EndFile" ) or die "Failed to open file $EndFile\n";
   @File = <TFILE>;
   close( TFILE );

   @start_time = split / /, $File[0];
   @end_time   = split / /, $File[1];

   $lap_time = $end_time[0] - $start_time[0];

   $tot_secs = $lap_time;

   open( TFILE, ">>$EndFile" ) or die "Failed to open file $EndFile\n";

   $sec      = $lap_time % 60;
   $lap_time = ($lap_time - $sec) / 60;
   $min      = $lap_time % 60;
   $lap_time = ($lap_time - $min) / 60;
   $hour     = $lap_time % 24;

   my $ElapsedTime = sprintf( "%02d:%02d:%02d\n", $hour,$min,$sec );
   my $LTime       = sprintf( "%010d LAP:   %s\n",$tot_secs,$hour,$min,$sec,$ElapsedTime );

   print TFILE "$LTime";

   close( TFILE );

   if( $opt_b ne "" )
   {
      print "Build completed at $Time\n";
      print "Build took $ElapsedTime";
   }
}

1;
