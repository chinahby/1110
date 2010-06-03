#!/usr/bin/perl -w
# ------------------------------------------------------------------------
# Remove #line directives and extra blank lines from a preprocessed file
# Copyright (c) 1999, 2003 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/ppasm.pl#1 $ $DateTime: 2008/06/23 20:28:49 $ $Author: sukumara $
# ------------------------------------------------------------------------
while(<>) {
   s/^\s*(#line )+(.)+//; 
   if (!/^\s*$/) {
      print $_ ;
   }
}
