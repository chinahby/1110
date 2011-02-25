#!/usr/bin/perl
#===================================================================
# FILE: redirect.pl
#===================================================================
open (FILEHANDLE, "> $ARGV[0]");
foreach $i (1..$#ARGV)
{
  $_=$ARGV[$i];
  print FILEHANDLE "$_ ";
}
close FILEHANDLE;

