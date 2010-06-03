#!/usr/bin/perl -w
# ------------------------------------------------------------------------
# B2A - Binary to ASCII Conversion 
# Copyright (c) 2003 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/b2a.pl#1 $
# ------------------------------------------------------------------------
use strict;

my $value = 0;
my $byte_count = 0;

if( scalar @ARGV != 1 ) {
  print "USAGE: b2a <input file> \n\n";
  print "example: b2a engdat1.prm > eng1prm.txt \n";
  exit;
}

my $filename = shift;

open INPUT, "<$filename" or die "Could not read from file '$filename': $!\n";
binmode(INPUT);

$value = ord(getc(INPUT));

if( !eof(INPUT) ) {
  printf("0x%02x", $value);  	
  $byte_count++;
  while( !eof(INPUT) ) {
    $value = ord(getc(INPUT));
    if( $byte_count == 16 ) {
      printf(",\n0x%02x", $value);
      $byte_count = 0;
    } else {
      printf(", 0x%02x", $value);
    }
    $byte_count++;
  }  
}

close INPUT;
