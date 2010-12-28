#!/usr/bin/perl
#
# This script can be used to find out what the next link address should
# be following another ELF file. It takes as input on the command line
# the path to the ELF file which precedes the ELF yet to be linked.
# It finds the end address of the preceding ELF in memory, rounds that
# up to the nearest 16kb, and prints that address to standard out.
#
# 09/20/05 smuckle

use warnings;
use strict;

my $linebuf;
my @splitline;
my $currentAddr = -1;

# Here are the parsing states:
# 0: Finding the program header section of the output in fromelf.
# 1: Finding the size in memory.
# 2: Finding the virtual address.
my $parseState = 0;

open(FROMELF, "fromelf -text -y $ARGV[0] |");
while($linebuf = <FROMELF>) {
  @splitline = split(" ", $linebuf);

  if ($parseState == 0) {
    # We are looking for a program header description.
    if ((@splitline > 1) && ($splitline[0] eq "**") && 
        ($splitline[1] eq "Program")) {
      $parseState = 1;
    }
  } elsif ($parseState == 1) {
    # We are looking for the memory size.
    if ((@splitline > 1) && ($splitline[0] eq "**") && 
        ($splitline[1] eq "Program")) {
      print "Error in fromelf input - no memory size " . 
        "in program header description.\n";
      exit(-1);
    } elsif ((@splitline >= 4) && ($splitline[0] eq "Size")) {
      $currentAddr = $splitline[2];
      $parseState = 2;
    }
  } else {
    # We are looking for the virtual address.
    if ((@splitline > 1) && ($splitline[0] eq "**") && 
        ($splitline[1] eq "Program")) {
      print "Error in fromelf input - no virtual address " . 
        "in program header description.\n";
      exit(-1);
    } elsif ((@splitline >= 3) && ($splitline[0] eq "Virtual")) {
      $currentAddr += hex($splitline[2]);
      $parseState = 0;
    }
  }
}
close(FROMELF);

if ($currentAddr == -1) {
  print "No link address could be calculated!\n";
  print "Please check that file \"$ARGV[0]\" exists ";
  print "and has the right format.\n";
  exit(-1);
} else {
  # We'll start new ELFs on a 16kb boundary.
  $currentAddr += (0x10000 - ($currentAddr % 0x10000));
  printf("%#x", $currentAddr);
  exit(0);
}
