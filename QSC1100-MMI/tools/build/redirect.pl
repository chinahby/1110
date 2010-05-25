#!/usr/bin/perl -w
# ------------------------------------------------------------------------
# Redirect output to a file
# Copyright (c) 2003-2005 QUALCOMM Inc. All Rights Reserved.
# ------------------------------------------------------------------------
open (FILEHANDLE, ">> $ARGV[0]");
foreach $i (1..$#ARGV)
{
  $_=$ARGV[$i];
  print FILEHANDLE "$_ ";
}
close FILEHANDLE;
