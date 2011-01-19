# $Header: //depot/asic/msmshared/build/ms/lint_reports.pl#1 $
# $DateTime: 2007/05/06 14:50:43 $
# $Change: 477694 $

##########################################################################
#                                                                        #
# File  : lint_results.pl                                                #
#                                                                        #
# Desc. : Processes lint results and generates LINT status report        #
#         based on severity for each subsystem                           #
#                                                                        #  
# Usage : lint_results [options...]                                      #
#         where options can be any combination of the following:         #
#                                                                        #
#         -d <dir>       Directory in which to put the output reports    #
#                        This is required.                               #
#         -r <root>      Root of the directory to process for lint files #
#                        Default is directory specified by -d above.     #
#         -o <output>    Tag to use for generating output file names.    #
#                        Usually the build ID (e.g. SMAAR). If not       #
#                        specified, summary goes to STDOUT and warning   #
#                        files are not generated.                        #
#         -s <subsystem> Searches for the subsystem specified and        #
#                        generates results based on that.                #
#                        Default is "all"                                #
#         -l <level>     Directory level depth at which to produce       #
#                        results.  Default is 2 directories deep.        #
#                                                                        #
# In order to generate the Perl .exe, execute the following:             #
# perlapp lint_reports.pl                                                #
##########################################################################

use strict;
use English;
use Getopt::Std;

my $LintSummaryBySubsystem;
my $LintErrorsBySubsystem;

##########################################################################
#                                                                        #
# Lint Error Categorization                                              #
#                                                                        #
# As stated in the Lint manual, the following categories exist within    #
# Lint itself.  By default they will be given the following severity.    #
#                                                                        #
# Category                C Error      Severity                          #
# ---------------       ------------   --------                          #
# Makefile Errors                0     High                              #
# Syntax Errors            1 - 199     High                              #
# Internal Errors        200 - 299     Critical                          #
# Fatal Errors           300 - 399     Critical                          #
# Warnings               400 - 699     High                              #
# Informational          700 - 799     Low                               #
# Undocumented           800 - 899     Low                               #
# Elective Notes         900 - 999     Low                               #
#                                                                        #
# Banned API (not allowed!!!)  421     Higher than Critical              #
#                                                                        #
# The above table can be overridden by putting the specific errors into  #
# the following tables.                                                  #
#                                                                        #
#                                                                        #
##########################################################################

my @bannedapi = ( 421 );

my @critical = (  54,  413, 414, 415, 416, 418, 419, 420, 424, 433,
                  650, 661, 662, 665, 666 );

my @high     = (   0, 741, 794, 795, 796, 797, 802, 803, 804);

my @medium   = ( 549, 557, 558, 559, 566, 567, 568, 569, 570, 572, 
                 573, 574, 575, 580, 581, 601, 605, 608, 609, 610, 
                 611, 619, 622, 632, 641, 643, 655, 656, 657, 680, 
                 712, 732, 734, 735, 736, 737);

my @SEVERITIES = ( "bannedapi", "critical", "high", "medium", "low" );

my ($all_mode, $file_mode);
our ($opt_d, $opt_r, $opt_o, $opt_s, $opt_l);
getopts('d:r:o:s:l:');

die "Must specify -d <dir>" if( !$opt_d );

if( $opt_l < 1 ) {
  print "Unsupported level($opt_l).  Using default of 2\n" if( defined( $opt_l ) );
  $opt_l = 2;
}

# See if we're in all mode (i.e. linting everything)
$all_mode = 1 if( $opt_s =~ /^all$/ );
# See if we're in file mode
$file_mode = 1 if( $opt_s =~ /\.cp?p?/ );

$opt_r = "." if( !defined( $opt_r ) );
chdir $opt_d;
my $basename;
# Generate base filename
$basename = "$opt_o" if( $opt_o );
$basename .= ".$opt_s" if( $opt_s );
$basename =~ s/\//_/g;
my $rpt_file = $basename.".rpt";
$rpt_file =~ s/\\/_/g;
$rpt_file =~ s/\//_/g;
if( $opt_o ) {
  if( $all_mode ) {
    # If we're "all", then breakout crit/high/med/low
    open( H_BANNED, ">$basename.bannedapi" ) || die "Unable to open $basename.bannedapi";
    open( H_CRIT, ">$basename.crit" ) || die "Unable to open $basename.crit";
    open( H_HIGH, ">$basename.high" ) || die "Unable to open $basename.high";
    open( H_MED,  ">$basename.med" ) || die "Unable to open $basename.med";
    open( H_LOW,  ">$basename.low" ) || die "Unable to open $basename.low";
  } 

  # All errors sorted in one file
  open( H_ERR,  ">$basename.err" ) || die "Unable to open $basename.err";
  # Summary file
  open( H_RPT,  ">$rpt_file" ) || die "Unable to open report file: $rpt_file";
} else {
  open( H_RPT, ">&STDOUT" ) || die "Unable to open stdout!";
}

# Print the date report was generated and command-line
print H_RPT "Generated on: ".localtime()."\n";
print H_RPT "$0 ";
print H_RPT "-d $opt_d " if( defined($opt_d) );
print H_RPT "-r $opt_r " if( defined($opt_r) );
print H_RPT "-o $opt_o " if( defined($opt_o) );
print H_RPT "-s $opt_s " if( defined($opt_s) );
print H_RPT "-l $opt_l " if( defined($opt_l) );
print H_RPT "\n\n";

if( $file_mode ) {
  ProcessLintFile( $opt_s.".lint" );
} else {
  ProcessDirectory($opt_r) || die "Unable to find lint log files";
}

DisplayResults();

if( defined( $opt_o ) ) {
  print "\n";
  close( H_RPT );
  print "Summary report generated: $opt_d/$rpt_file\n";

  if( $all_mode ) {
    close( H_BANNED );
    close( H_CRIT );
    close( H_HIGH );
    close( H_MED );
    close( H_LOW );

    print "Banned API errors saved:    $opt_d/$basename.bannedapi\n";
    print "Critical errors saved:    $opt_d/$basename.crit\n";
    print "High errors saved:        $opt_d/$basename.high\n";
    print "Medium errors saved:      $opt_d/$basename.med\n";
    print "Low errors saved:         $opt_d/$basename.low\n";
  } 
  close( H_ERR );
  print "Errors sorted in:         $opt_d/$basename.err\n";
}

sub ProcessDirectory($) {
  my ($root) = @_;
  my $found = 0;

  opendir( H_DIR, $root ) || die "Unable to open dir: $root";
  my @allfiles = readdir(H_DIR);
  closedir(H_DIR);

  foreach my $localfile (@allfiles) {
    next if( $localfile eq "." );
    next if( $localfile eq ".." );
#    return if( $localfile =~ /apps/ ); # Used for testing

    my $fullpath = $root."/".$localfile;
    if( $localfile =~ /\.lint$/ ) {
      # Never process .lint files in the root dir
      next if( $root eq "." );

      ProcessLintFile($fullpath);
      $found = 1;
    } elsif( -d $fullpath ) {
      # Recurse
      my $rv = ProcessDirectory($fullpath);
      $found ||= $rv;
    }
  }
  return $found;
}

sub ProcessLintFile($)
{
  my ($lintfile) = @_;

  open(F,$lintfile) || die "Can't open $lintfile: $!\n";

  # Determine default filename based $lintfile
  my $DefaultFileName = $lintfile;
  $DefaultFileName =~ s/^\.\///;
  $DefaultFileName =~ s/\//\\/g;
  $DefaultFileName =~ s/(.*)\/.*?/$1\/unknown/;

  my $DefaultPath = $lintfile;
  $DefaultPath =~ s/^\.\///;
  $DefaultPath =~ s/\//\\/g;
  $DefaultPath =~ s/\\[^\\]*\.lint//;

  # Process the entire file
  while( my $line = <F> ) {

    my ($FileName, $ErrorId, $ErrorString);
    if( $line =~ /^(.+) : Error \(.*?\) (\d+): (.*)/ ) {
      $FileName     = $1;
      $ErrorId      = $2;
      $ErrorString  = $3;
    } elsif( $line =~ /^ : Error \(.*?\) (\d+): (.*) \(line (\d+), file (.+)\)/ ) {
      $FileName     = $4;
      $ErrorId      = $1;
      $ErrorString  = $2;
    } elsif( $line =~ /^ : Error \(.*?\) (\d+): (.*)/ ) {
      $FileName     = $DefaultFileName;
      $ErrorId      = $1;
      $ErrorString  = $2;
    } else {
      # print "Ignoring '$line'";
      next;
    }
    
    # Save the default file name if we didn't determine it yet
    $DefaultFileName = $FileName if( $DefaultFileName !~ /\\/ );

    # Check for special cases:
    # Check if a path was specified
    if( $FileName =~ /\\/ ) {
      # Remove absolute path
      $FileName =~ s/(.+?):\\//;
      # Get rid of preceeding ..\..\
      $FileName =~ s/\.\.\\//g;
    } else {
      # Add the path to the current .lint file as the path
      $FileName = $DefaultPath."\\".$FileName;
    }

    my @subsystems = split( /\\/, $FileName );
    pop( @subsystems );   # Ditch the filename portion
    my $Subsystem = @subsystems[0];
    for( my $i = 1; $i < $opt_l && $i < @subsystems; $i++ ) {
      $Subsystem .= "/@subsystems[$i]";
    }
    $Subsystem = lc($Subsystem);

    die "Internal error: bad parse ($lintfile, line $.)" if( $Subsystem eq "" || $Subsystem eq "/" );

    # Initialize the summary for the subsystem
    if( !defined($LintSummaryBySubsystem->{$Subsystem}) ) {
      $LintSummaryBySubsystem->{$Subsystem} = {
        bannedapi => 0,
        critical  => 0,
        high      => 0,
        medium    => 0,
        low       => 0,
        total     => 0,
      };
    }
    # Update the summary
    my $severity = WhatSeverity($ErrorId);
    $LintSummaryBySubsystem->{$Subsystem}->{$severity}++;
    $LintSummaryBySubsystem->{$Subsystem}->{$ErrorId}->{cnt}++;
    $LintSummaryBySubsystem->{$Subsystem}->{$ErrorId}->{str} = $ErrorString;
    $LintSummaryBySubsystem->{$Subsystem}->{total}++;

    # Initialize the list of errors for the subsystem
    if( !defined($LintErrorsBySubsystem->{$Subsystem}) ) {
      $LintErrorsBySubsystem->{$Subsystem} = {
        bannedapi => "",
        critical  => "",
        high      => "",
        medium    => "",
        low       => "",
      };
    }
    # Save the error on the list
    $LintErrorsBySubsystem->{$Subsystem}->{$severity} .= $line;
  }

  close(F);
}

sub WhatSeverity($)
{
  my ($id) = @_;
  return "bannedapi" if( grep( /^$id$/, @bannedapi ) );
  return "critical" if( grep( /^$id$/, @critical ) );
  return "high"     if( grep( /^$id$/, @high ) );
  return "medium"   if( grep( /^$id$/, @medium ) );

  return "high"     if( ($id >=   1 && $id <= 199) || ($id >= 1001 && $id <= 1199) );
  return "critical" if( ($id >= 200 && $id <= 299) );
  return "critical" if( ($id >= 300 && $id <= 399) );
  return "high"     if( ($id >= 400 && $id <= 699) || ($id >= 1400 && $id <= 1699) );
  return "low"      if( ($id >= 700 && $id <= 799) || ($id >= 1700 && $id <= 1799) );
  return "low"      if( ($id >= 800 && $id <= 899) );
  return "low"      if( ($id >= 900 && $id <= 999) || ($id >= 1900 && $id <= 1999) );

  # Charlie removed die "Unknown severity: $id";
  # Unknown errors are flagged as critical until further notice
  return "critical"
}

sub DisplayResults()
{
  my $Total = {
    bannedapi => 0,
    critical => 0,
    high     => 0,
    medium   => 0,
    low      => 0,
    total    => 0,
  };

  my $bErrorsFound = 0;
  print  H_RPT  "Subsystem Summary:\n" if( !$file_mode );
  foreach my $subsystem (sort(keys(%$LintSummaryBySubsystem))) {
    if( !$file_mode ) { 
      # Generate a technology/subsystem report
      WriteSubsystemResults($subsystem);

      my @dirs = split(/\//, $subsystem);
      my $fmt = "%-12s";
      for( my $i = 1; $i < $opt_l; $i++ ) {
        $fmt .= " %-20s";
      }
      $fmt = sprintf( $fmt, @dirs );
      $fmt .= "B=%5s  C=%5s  H=%5s M=%5s L=%5s T=%5s\n";
      printf H_RPT ($fmt, 
                    $LintSummaryBySubsystem->{$subsystem}->{bannedapi}, 
                    $LintSummaryBySubsystem->{$subsystem}->{critical}, 
                    $LintSummaryBySubsystem->{$subsystem}->{high}, 
                    $LintSummaryBySubsystem->{$subsystem}->{medium}, 
                    $LintSummaryBySubsystem->{$subsystem}->{low}, 
                    $LintSummaryBySubsystem->{$subsystem}->{total});
    }
    # Accumulate the statistics
    $Total->{total}     += $LintSummaryBySubsystem->{$subsystem}->{total};
    $Total->{bannedapi} += $LintSummaryBySubsystem->{$subsystem}->{bannedapi};
    $Total->{critical}  += $LintSummaryBySubsystem->{$subsystem}->{critical};
    $Total->{high}      += $LintSummaryBySubsystem->{$subsystem}->{high};
    $Total->{medium}    += $LintSummaryBySubsystem->{$subsystem}->{medium};
    $Total->{low}       += $LintSummaryBySubsystem->{$subsystem}->{low};
  }
  print  H_RPT  "\n" if( !$file_mode );

  print  H_RPT  "Error Summary:\n";
  WriteErrorSummaryResults();
  print  H_RPT  "\n";

  print  H_RPT  " ********* Results ********* \n";
  printf H_RPT ("  Banned API      = %8s\n", $Total->{bannedapi});
  printf H_RPT ("  Critical        = %8s\n", $Total->{critical});
  printf H_RPT ("  High            = %8s\n", $Total->{high});
  printf H_RPT ("  Medium          = %8s\n", $Total->{medium});
  printf H_RPT ("  Low             = %8s\n", $Total->{low});
  printf H_RPT ("Total Lint errors = %8s\n", $Total->{total});
  print  H_RPT  " *************************** \n";

  # Print a summary to the screen if a report was chosen
  if( defined( $opt_o ) ) {
    print   "\n";
    print   " ********* Results ********* \n";
    printf ("  Banned API      = %8s\n", $Total->{bannedapi});
    printf ("  Critical        = %8s\n", $Total->{critical});
    printf ("  High            = %8s\n", $Total->{high});
    printf ("  Medium          = %8s\n", $Total->{medium});
    printf ("  Low             = %8s\n", $Total->{low});
    printf ("Total Lint errors = %8s\n", $Total->{total});
    print   " *************************** \n";
  }

  # Write the list of errors
  WriteErrors();
}

sub GenSubsystemSeveritySummary($$) 
{
  my ($lint, $severity) = @_;
  my $result;
  $result = "$severity:\n";
  foreach my $id (sort { $a <=> $b } (keys(%$lint))) {
    next if( $id eq "bannedapi" );
    next if( $id eq "critical" );
    next if( $id eq "high" );
    next if( $id eq "medium" );
    next if( $id eq "low" );
    next if( $id eq "total" );

    if( WhatSeverity($id) eq $severity ) {
      # Remove specifics that are in single quotes ''
      $lint->{$id}->{str} =~ s/\'.*?\'//g;
      # Remove specifics that are in parens ()
      $lint->{$id}->{str} =~ s/\(.*?\)//g;
      # Remove specifics that are in brackets []
      $lint->{$id}->{str} =~ s/\[.*?\]//g;
      # Remove double spaces that may have been caused by the above
      $lint->{$id}->{str} =~ s/  / /g;

      $result .= sprintf("  %3d: (%6d): %-60s\n", $id, $lint->{$id}->{cnt}, $lint->{$id}->{str});
    }
  }
  $result .= "\n";
  return $result;
}

sub GenSubsystemErrors($$)
{
  my ($lint, $severity) = @_;

  my $result = ucfirst($severity)." errors:\n";
  $result .= "===============\n";
  $result .= $lint->{$severity};
  $result .= "\n";
  return $result;
}

sub WriteSubsystemResults($)
{
  my ($subsystem) = @_;

  my @path = split( /\//, $subsystem );
  my $filepath = @path[0];
  # If the parent directory doesn't exist, create it
  if( ! -d $filepath ) { 
    print "'$filepath' doesn't exist.  Creating...\n";
    mkdir( $filepath );
  }
  if( @path > 1 ) {
    $filepath .= "/".@path[1];
    # If the second-level dir doesn't exist, create it
    if( ! -d $filepath ) { 
      print "'$filepath' doesn't exist.  Creating...\n";
      mkdir( $filepath );
    }
  }

  # Create the filename for the subsystem
  my $filename = $filepath."/".@path[0];
  for( my $i = 1; ($i < $opt_l && $i < @path); $i++ ) {
    $filename .= "_@path[$i]";
  }

  # Create the summary report for the subsystem
  open( F, ">$filename.rpt" ) || die "Unable to open $filename.rpt";
  my $LintSub = $LintSummaryBySubsystem->{$subsystem};
  for( my $i = 0; $i < @SEVERITIES; $i++ ) {
    print F GenSubsystemSeveritySummary($LintSub, @SEVERITIES[$i]);
  }
  close( F );

  # Create the sorted list of errors for the subsystem
  open( F, ">$filename.err" ) || die "Unable to open $filename.err";
  $LintSub = $LintErrorsBySubsystem->{$subsystem};
  for( my $i = 0; $i < @SEVERITIES; $i++ ) {
    print F GenSubsystemErrors($LintSub, @SEVERITIES[$i]);
  }
  close( F );
}

sub WriteErrorSummaryResults()
{
  my $LintResultsByErrorId;
  
  foreach my $subsystem (keys(%$LintSummaryBySubsystem)) {
    next if( $subsystem eq "" );
    foreach my $error (keys(%{$LintSummaryBySubsystem->{$subsystem}})) {
      next if( $error eq "bannedapi" );
      next if( $error eq "critical" );
      next if( $error eq "high" );
      next if( $error eq "medium" );
      next if( $error eq "low" );
      next if( $error eq "total" );

      $LintResultsByErrorId->{$error}->{cnt} += $LintSummaryBySubsystem->{$subsystem}->{$error}->{cnt};
      $LintResultsByErrorId->{$error}->{str} = $LintSummaryBySubsystem->{$subsystem}->{$error}->{str};
    }
  }

  for( my $i = 0; $i < @SEVERITIES; $i++ ) {
    print H_RPT GenSubsystemSeveritySummary($LintResultsByErrorId, @SEVERITIES[$i]);
  }
}

sub WriteErrors()
{
  my $ErrorsBySeverity = {
    bannedapi => "",
    critical  => "",
    high      => "",
    medium    => "",
    low       => "",
  };

  foreach my $subsystem (sort(keys(%$LintErrorsBySubsystem))) {
    foreach my $severity (@SEVERITIES) {
      $ErrorsBySeverity->{$severity} .= $LintErrorsBySubsystem->{$subsystem}->{$severity};
    }
  }

  if( $all_mode ) {
    print H_BANNED $ErrorsBySeverity->{bannedapi};
    print H_CRIT   $ErrorsBySeverity->{critical};
    print H_HIGH   $ErrorsBySeverity->{high};
    print H_MED    $ErrorsBySeverity->{medium};
    print H_LOW    $ErrorsBySeverity->{low};
  } 
  for( my $i = 0; $i < @SEVERITIES; $i++ ) {
    print H_ERR  GenSubsystemErrors($ErrorsBySeverity, @SEVERITIES[$i]);
  }
}

#
# find_subsystem_dir($) - Looks for a given directory (subsystem)
# starting at the specified root.  Returns all exact case-insensitive
# matches.  
#
sub find_subsystem_dir($$)
{
  my ($root, $id) = @_;
  my @matches;

  return @matches if( $root =~ /build\/ms/ );

  opendir( H_DIR, $root ) || die "Unable to open dir: $root";
  my @subdirs = grep { /^\w/ && -d "$root/$_" } readdir(H_DIR);

  foreach my $subdir (@subdirs) {
    my $full_dir = "$root/$subdir";
    if( $full_dir =~ /$id$/i ) {
      push( @matches, $full_dir );
    }
    push( @matches, find_subsystem_dir( $full_dir, $id ) );
  }

  close( H_DIR );
  return @matches;
}
