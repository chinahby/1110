#!/usr/local/bin/perl
# ------------------------------------------------------------------------
# Script to check for certificates in secure boot.
# Copyright (c) 2001-2005 QUALCOMM Inc. All Rights Reserved.
# 
#-------------------------------------------------------------------------
#/*===========================================================================
#
#                           EDIT HISTORY FOR FILE
#
# $Header: //depot/asic/qsc1100/build/ms/check_4_certificates.pl#1 $ $DateTime: 2007/10/25 15:24:48 $ $Author: milanp $
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -------------------------------------------------------
# 03/08/06   amw     Remove the deletion of existing certs.
# 08/26/05   tk      created
#===========================================================================*/

use warnings;
use strict;

my $finish = 0;


# Remove All the Arguments

#for(my $i=1; $i <= $#ARGV; $i++)
#{
#  system("rm -f $ARGV[$i]");
#}

# Wait and Loop here if the certificates dont exist.
while($finish < $#ARGV) {
 
 $finish = 0;
 for(my $i=1; $i <= $#ARGV; $i++)
 {
  if((-e $ARGV[$i])){
        $finish++;
  }
 }
 
 sleep("$ARGV[0]");
 
}
