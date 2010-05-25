##########################################################################################
#
#  Script to append new const file symbols to OEMConstFiles_Dynamic.c,
#  The scrip converts the filenames into their const file system symbols.
#
#  Name : OEMConstFileMod.pl
# 
#  Usage: 1) Copy OEMConstFiles.c to OEMConstFiles_Dynamic.c
#         2) perl $(BUILDTOOLSDIR)/OEMConstFileMod.pl OEMConstFiles_Dynamic.c <file list>
#  
#        Copyright © 2005 QUALCOMM Incorporated.  All Rights Reserved.
#
###########################################################################################

#! /usr/local/bin/perl -w

use strict;

die "\n" . 
    "OEMConstFileMod.pl version 1.0.1\n" .
    "  Adds a new const file symbol to the OEMConstFiles.c for each\n" .
    "  const file filename specified on the command line.\n\n".
    "Usage: perl $0 <OEMConstFiles.c> <filenames>\n\n"  .
    "Example:\n".
    "   perl $0 OEMConstFiles_Dynamic.c sampleapp.mif\n\n" .
    "   This adds the symbol gSAMPLEAPP_MIF to the file OEMConstFiles_Dynamic.c\n"
    unless $#ARGV >= 0;

# Open OEMConstFiles.c in read only mode

open(CONSTFILE,"<".$ARGV[0]) or die "Can't open OEMConstFiles \"$ARGV[0]\": $!\n";

my $line;

#######################################################################################
# Read from OEMConstFiles and store lines read in a buffer. Also the Const symbols to 
# be added in OEMCOnstFiles.c are stored in the filebuffer
#######################################################################################

#Add externs for the new symbols where the "extern AEEConstFile" declarations start
my $filebuffer;

while (($line = <CONSTFILE>) !~ m/$\s*static\s+const\s+AEEConstFile\s+\*\s+gpOEMConstFiles/)
{
	$filebuffer .= $line; 	   
}

$filebuffer .= "\/\/Start Dynamic ConstFile Externs\n";

for(my $argn = 1; $argn <= $#ARGV; $argn++) 
{
	my $symbol = $ARGV[$argn];
	$symbol =~ s/\./\_/g;
	$symbol = "g".uc($symbol);
	$filebuffer .= "extern AEEConstFile ".$symbol.";\n";
}
$filebuffer .= "\/\/End Dynamic ConstFile Externs\n";

$filebuffer .= "\n";
$filebuffer .= $line;
$filebuffer .= "\n";

my $templine;
$filebuffer .= "\/\/Start Dynamic ConstFile Symbols\n";
for(my $argn = 1; $argn <= $#ARGV; $argn++) 
{
	my $symbol = $ARGV[$argn];
	$symbol =~ s/\./\_/g;
	$symbol = "g".uc($symbol);
	$templine = "      \&".$symbol.",\n";
	$filebuffer .= $templine;
}
$filebuffer .= "\/\/End Dynamic ConstFile Symbols\n";

$filebuffer .= "\n";

while ($line = <CONSTFILE>)
{
	$filebuffer .= $line;
}

close(CONSTFILE);

########################################################################################
# OEMConstfiles.c is closed and reopened in write mode to be overwritten with the 
# contents of the filebuffer
########################################################################################

open(CONSTFILE,">".$ARGV[0]) or die "Can't open $ARGV[0]: $!\n";
print CONSTFILE $filebuffer;
close(CONSTFILE);