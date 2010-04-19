#!/usr/bin/perl
#
# This perl script should be called incrementally during the build process.
# Different arguments to the PERL script determines the behavior of this script. 
# This script would create miaddress.h file with different #defines used during 
# link stage of different images.
# This script should be called multiple times in the build process where it 
# incrementally adds #defines in the miaddress.h file. It should be called in 
# following order. 
# 1. -amss
# 2. -fsbl
# 4. -osbl
#
# If  -sign option is used, it would add 0xA00 bytes offset after every image 
# to account for certificates and signatures
#
# Usage : *.pl <-fsbl>|<-osbl>|<-amss> <build_id> [<-sign>] 

$miheader_file = "../../core/boot/mibib/miheader.h";
$miaddress_file = "../../core/boot/mibib/miaddress.h";
$dbl_mbn_file = "../../core/boot/romboot/dbl/dbl_$ARGV[1].mbn";
$fsbl_mbn_file = "../../core/boot/romboot/fsbl/fsbl_$ARGV[1].mbn";
$osbl_mbn_file = "../../core/boot/romboot/osbl/osbl_$ARGV[1].mbn";

$fsbl_src_define = "MI_NOR_FSBL_IMG_SRC_ADDR " ;
$dbl_src_define = "MI_NOR_DBL_IMG_SRC_ADDR";
$osbl_src_define  = "MI_NOR_OSBL_IMG_SRC_ADDR";

$ARGC = @ARGV;
if($ARGC < 2 )
{
  die " Wrong number of arguments.\n Usage : *.pl <-fsbl>|<-dbl>|<-osbl>|<-amss> <build_id> [<-sign>]\n";
}

if($ARGV[2] eq "-sign")
{
  $certificate_offset = 0xA00; #size of certificate chain + signature
}
else
{
  $certificate_offset = 0;
}

if ($ARGV[0] eq "-amss") #just create miaddress.h file
{
   if((-e $miaddress_file))
   {
      unlink($miaddress_file);
      print " Old $miaddress_file deleted sucessfully.\n" 
   }
   open(MIADDRESS, ">$miaddress_file");
   print MIADDRESS "/* This is auto generated file. Do not edit. */\n\n\n";
   close(MIADDRESS);
}

if ($ARGV[0] eq "-fsbl") 
{
   if((-e $miaddress_file))
   {
      unlink($miaddress_file);
      print " Old $miaddress_file deleted sucessfully.\n" 
   }
   if(!(-r $miheader_file))
   {
      die "$miheader_file not found or it is not readeable\n";
   }
   else
   {
      open(MIHEADER,"$miheader_file");
      @lines = <MIHEADER>;
      close(MIHEADER);
      foreach $line (@lines)
      {
        if( index($line, $dbl_src_define) != -1)
        {
           @words = split(/\s+/,$line);
           $dbl_offset = hex($words[2]);
           last;
        }
      }
      if($dbl_offset == 0)
      {
         die "#define $fsbl_src_define not found in $miheader_file\n";
      }
      else
      {
         print "$fsbl_offset\n";
      }

      #Find the size of FSBL mbn
      if(!(-e $dbl_mbn_file))
      {
        die "$dbl_mbn_file not found\n";
      }

      @stat = stat($dbl_mbn_file);
      print "The size of dbl_mbn files is $dbl_mbn_file is $stat[7] bytes\n";

	  $fsbl_src_addr  = dec2hex($dbl_offset + $stat[7] + $certificate_offset);
      open(MIADDRESS, ">$miaddress_file");

      print MIADDRESS "/* This is auto generated file .Do not edit. */\n\n\n";
      print MIADDRESS "#define $fsbl_src_define  0x$fsbl_src_addr \n\n";
      close(MIADDRESS);
   }
}


if ($ARGV[0] eq "-osbl") 
{
   if(!(-r $miaddress_file))
   {
      die "$miaddress_file not found or it is not readeable\n";
   }
   else
   {
      open(MIADDRESS,"$miaddress_file");
      @lines = <MIADDRESS>;
      close(MIADDRESS);
      foreach $line (@lines)
      {
        if( index($line, $fsbl_src_define) != -1)
        {
           @words = split(/\s+/,$line);
           $fsbl_offset = hex($words[2]);
           last;
        }
      }
      if($fsbl_offset == 0)
      {
         die "#define $fsbl_src_define not found in $miheader_file\n";
      }
      else
      {
         print "$fsbl_offset\n";
      }

      #Find the size of DBL mbn
      if(!(-e $fsbl_mbn_file))
      {
        die "$fsbl_mbn_file not found\n";
      }

      @stat = stat($fsbl_mbn_file);
      print "The size of $fsbl_mbn_file is $stat[7] bytes\n";

      $osbl_src_addr  = dec2hex($fsbl_offset + $stat[7] + $certificate_offset);
     
      open(MIADDRESS, ">$miaddress_file");

      print MIADDRESS @lines;
      print MIADDRESS "#define $osbl_src_define  0x$osbl_src_addr\n\n";
      close(MIADDRESS);

   }
}

sub dec2hex {
    # parameter passed to the subfunction
    my $decnum = $_[0];
    # the final hex number
    my $hexnum;
    my $tempval;
    while ($decnum != 0) {
    # get the remainder (modulus function) by dividing by 16
    $tempval = $decnum % 16;
    # convert to the appropriate letter if the value is greater than 9
    if ($tempval > 9) {
    $tempval = chr($tempval + 55);
    }
    # 'concatenate' the number to what we have so far in what will
    # be the final variable
    $hexnum = $tempval . $hexnum ;
    # new actually divide by 16, and keep the integer value of the answer
    $decnum = int($decnum / 16); 
    # if we cant divide by 16, this is the last step
    if ($decnum < 16) {
    # convert to letters again..
    if ($decnum > 9) {
    $decnum = chr($decnum + 55);
    }
    
    # add this onto the final answer.. reset decnum variable to zero so loop
    # will exit
    $hexnum = $decnum . $hexnum; 
    $decnum = 0 
    }
    }
    return $hexnum;
    } # end sub




