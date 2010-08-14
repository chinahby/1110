#!/usr/local/bin/perl
# ------------------------------------------------------------------------
# pboot_obj_opt.pl
#
# Script to suggest objects which should be non-paged to optimize
# boot time.
#
# What it does is takes hex addresses which caused page fault, and 
# cross references it with the MAP file of the ELF image to figure out
# which code from which object corresponds to the hex address and 
# decides whether the object should reside in non-paged area.
#
# This is a test script, for testing purposes
#
# Command line arguments to the script:
#  argument 1: map file to the ELF image
#  argument 2: file containing hex addresses which caused page fault
#
# Copyright (c) 2006 QUALCOMM Inc. All Rights Reserved.
#
#-------------------------------------------------------------------------
#/*===========================================================================
#
#                           EDIT HISTORY FOR FILE
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# $Header: //depot/asic/msmshared/tools/pboot/pboot_obj_opt.pl#2 $
#
# when       who     what, where, why
# --------   ---     -------------------------------------------------------
# 11/09/06   ty      created
#===========================================================================*/

use strict;
use POSIX qw(ceil floor);

use FileHandle;

my $mem_map_regex;
my %obj_hash; #Keeps size of all objects
my $ratio_limit = 0.4;
my $small_size = 0;
package main;

{
  my $num_args = scalar(@ARGV);
  my @address;
  my @location;
  my $last_location;
  

  if (scalar(@ARGV)<3) {
    die "Need to give parameters. \n".
        "Usage: pboot_obj_opt <MAP_FILE> <LOG_FILE> <RATIO> <SMALL_SIZE>\n".
        "where, \n".
        "\t<MAP_FILE> is the map file for the ELF file ie: KYRQP.map\n".
        "\t<LOG_FILE> is the log file containing the page faults from page fault logging\n".
        "\t<RATIO> is the load ratio (0 to 100). If the number of bytes loaded due to page fault is less than this percentage of the object's size, then it is not suggested to load in non-paged partition\n".
        "\t<SMALL_SIZE> is the size in bytes of files considered small. Small files are always suggested to be loaded. Do not provide parameter, or 0 to disable.\n";
  }

  open(INFILE, "<$ARGV[0]") or die "Cannot open $ARGV[0]\n";
  open(SEARCHFILE, "<$ARGV[1]") or die "Cannot open $ARGV[1]\n";

  if (($ARGV[2] < 0 ) or ($ARGV[2] > 100))
  {
    die "RATIO needs to be between 0 and 100\n";
  }
  $ratio_limit = $ARGV[2]/100;
  if (scalar(@ARGV)>3)
  {
    $small_size = $ARGV[3];
  }
  else
  {
    $small_size = 0;
  }
  print "The ratio is $ratio_limit\n";
  print "Files smaller than $small_size bytes are considered small\n\n";

  #Regex for memory map portion of map file
  $mem_map_regex = qr/^ {4}(0x[0-9a-f]*) *(0x[0-9a-f]*) *((Code)|(Data)) *[A-Z]* *([0-9]*) *([^ ]*) *(\S+)/;
  #Scan through map file to compile list of addresses and objects that exists in image
  #Save position in file, and the addresses.
  $last_location = tell(INFILE);
  while (<INFILE>)
  {
    if ($_ =~ $mem_map_regex)
    {
      #printf "Saving %x %s\n", $1, $_;
      push(@address,[(hex($1), $last_location)]);
    }
    $last_location = tell(INFILE);
  }

  #Save the list in sorted order.
  my @sorted = sort {$$a[0] <=> $$b[0]} @address;
  @address = @sorted;

  my @not_found; #Saves addresses which are not found
  my @found;     #Saves addresses which are found

  #Now loop through the searchfile to do binary search through the parsed arrays
  #for the provided addresses
  #Expects the file being searched to have hex addresses, one per line to resolve into
  #objects
  while (<SEARCHFILE>)
  {
    my $top_bound;
    my $bottom_bound;
    my $mid_pt;
    my $item = hex($_);
    my $quit = 0;
    my $array;

    if (not ($_ =~ /fault_addr = 0x([0-9A-Fa-f]*).*?ret_addr = 0x([0-9A-Fa-f]*)/))
    {
      print "Skipping $_\n";
      next;
    }

    $item = hex($1);
    if ($item == 0)
    {
      next;
    }
    ###printf "Searching for 0x%x\n", $item;
    $bottom_bound = 0;
    $top_bound = @address-1;
    ###print "$bottom_bound : $top_bound\n"; ###
    while (($top_bound > $bottom_bound) and ($quit != 1))
    {
      #check that the number is between bottom and top!

      $mid_pt = floor((($top_bound-$bottom_bound)/2)+$bottom_bound);
      ###printf "Comparing %x to %x:%x:%x\n", $item, $address[$bottom_bound][0], $address[$mid_pt][0], $address[$top_bound][0];
      if (($mid_pt == $top_bound) || ($mid_pt == $bottom_bound))
      {
        $quit = 1;
      }
      elsif ($item < $address[$mid_pt][0])
      {
        $top_bound = $mid_pt;
      }
      elsif ($item > $address[$mid_pt][0])
      {
        $bottom_bound = $mid_pt;
      }
      else
      {
        #print "found exact $mid_pt\n";
        $quit = 1;
      }
      ###print "$top_bound : $bottom_bound\n";
    }
    #Check mid, top, and bottom bounds
    #
    my @ret;
    my $top_str;
    my $bottom_str;

    #Check if we have found a line in the map file which corresponds to an object

    seek(INFILE, $address[$mid_pt][1], 0);
    $_ = <INFILE>;
    @ret=is_correct_object($item, $_);
    if ($ret[0] != 1)
    {
      seek(INFILE, $address[$top_bound][1], 0);
      $top_str = <INFILE>;
      @ret=is_correct_object($item, $top_str);
    }
    if ($ret[0] != 1)
    {
      seek(INFILE, $address[$bottom_bound][1], 0);
      $bottom_str = <INFILE>;
      @ret=is_correct_object($item, $bottom_str);
    }
    if ($ret[0] != 1)
    {
      ###printf "0x%x not found, expected to be between\n\t$top_str\t$bottom_str", $item;
      push(@not_found, $item);
    }
    else
    {
      ###printf "Found 0x%x in [%x, %x] $ret[3]\n", $item, $ret[1], $ret[1]+$ret[2];
      if ($ret[3] =~ /^([^\( ]*)/)
      {
        $ret[3] = $1;
      }
      push(@found, $ret[3]);
    }
    
  }

  # Now we have found all the items we can... All items not found is in @not_found, and 
  # all items found is in @found. Now we want to collect some statistics.
  # a) size of object we will move in
  # b) if object was hit before, and how many times


  #Parse the MAP file to collect the sizes of all the objects
  seek(INFILE, 0, 0); #Start from beginning of file
  while (<INFILE>)
  {
    if ($_ =~ /^ +(\d+) +(\d+) +\d+ +\d+ +\d+ +(\S+)/)
    {
      ###print "$1 : $2 : $3\n";
      $obj_hash{$3} = $1+$2;
    }
  }
  optimize(@found);

}

#Subroutine to check if a given address corresponds to the provided 
#line corresponding to an object.
# Param 1: Address to check
# Param 2: Line in MAP file corresponding to the object to check
#Returns:
# Param 1: 0 if the address does not match the object
# Param 2: Address of matching code
# Param 3: Size of matching code
# Param 4: Filename containing the matching code
sub is_correct_object
{
  my @ret;

  ###print "Checking $_[0] in $_[1]\n";
  $ret[0] = 0;
  if ($_[1] =~ $mem_map_regex)
  {
    #/* Check if address lies within the memory specified by the mem map line */
    my $addr;
    my $size;
    my $filename;
    $addr = hex($1);
    $size = hex($2);
    $filename = $8;
    if (($_[0] >= $addr) and ($_[0] <= $addr+$size))
    {
      @ret = (1, $addr, $size, $filename);
    }
  }
  else
  {
    die "Not proper memory map line xxx $_[1]\n";
  }
  return @ret;
}

#Subroutine to optimize a list of objects touched.
# Accepts an array of objects touched (duplicates as well)
sub optimize
{
  my @found=@_;
  my %dup_hash;
  my $tot_hits=0;
  my $tot_size=0;
  my $tot_files=0;
  my $tot_dup_hits=0;

  #Basic flow in this routine:
  #a) Create a hash that counts duplicate hits for an object
  #b) Total the number of files, and the size to move into non-paged
  #c) Collect stats on how many hit misses would be saved, and number of hits which are duplicates
  #d) Decide whether object should be loaded or not. Object not loaded if the load size is less
  #   than 40% of the object's file size.

  #Go through @found list, create %dup_hash that keeps track of each file and
  #number of duplicate hits for easy index.
  foreach my $o (@found)
  {
    $dup_hash{$o} = $dup_hash{$o}+1;
    if ($dup_hash{$o}==1)
    {
      ###print "Size of $o is $obj_hash{$o}\n";###
      $tot_size+=$obj_hash{$o};
      $tot_files += 1;
    }
  }
  print "!!!To eliminate page faults, have to move $tot_files ($tot_size bytes) into non-paged partition\n";

  #Find duplicate hits
  
  $tot_dup_hits = 0; #keep total # of duplicate hits saved
  $tot_hits = 0; #Keep total # of hits saved
  foreach my$o (keys %dup_hash)
  {
    if ($dup_hash{$o} >1)
    {
      ###print "$o was hit $dup_hash{$o} times\n"; ###
      $tot_dup_hits += $dup_hash{$o};
    }
    $tot_hits += $dup_hash{$o};
  }
  print "!!!Would save $tot_hits hit misses, $tot_dup_hits of which are duplicates\n";
  @found = keys %dup_hash;

  #Suggest objects to load, and objects not to load based on whether 
  #it makes sense to. Example, do not load 1MB object for 1 4096byte page fault.
  #If size loaded is less than 40% of the object size, then do not suggest to load it.
  for (my $i = 0; $i < @found ; $i++)
  {
    my $page_size = 4096;
    ###print "Working on $found[$i] :$i\n";###
    my $size_loaded = $dup_hash{$found[$i]}*$page_size;
    my $ratio;
    if ($obj_hash{$found[$i]}==0)
    {
      print "$found[$i] not found!\n";
      
    }
    $ratio = $size_loaded/$obj_hash{$found[$i]};
    if (($ratio < $ratio_limit) and ($obj_hash{$found[$i]} > $small_size))
    {
      print "Suggest not to load $found[$i] (ratio = $ratio) (size = $obj_hash{$found[$i]})\n";
      delete $found[$i];
    }
    else
    {
      print "Load $found[$i] (ratio = $ratio) (size = $obj_hash{$found[$i]})\n";
    }
  }
  
  #Now we know which objects should be loaded, print it out in a format so that
  #it can be cut and pasted into scatterload file in the non-page section
  #(or use the information as appropriate)
  #
  $tot_size=0;
  $tot_files=0;
  $tot_hits=0;
  $tot_dup_hits=0;
  print "\nSuggested objects to be loaded in non-paged partition:\n{\n";
  foreach my $o (@found)
  {
    # Special case
    if ($o =~ m/anon\$\$obj.o/)
    {
      next;
    }

    if ($obj_hash{$o}>0)
    {
      #print "Size of $o is $obj_hash{$o}\n";
      print "    $o (+RO)\n";
      $tot_size+=$obj_hash{$o};
      $tot_files += 1;
      $tot_hits+=$dup_hash{$o};
      if ($dup_hash{$o}>1)
      {
        $tot_dup_hits+=$dup_hash{$o};
      }
    }
  }
  print "}\n";
  print "!!!Optimized: Would move $tot_files ($tot_size bytes) into MODEM non-paged partition\n";
  print "!!!Would save $tot_hits hits, $tot_dup_hits of which are duplicates\n";


}
