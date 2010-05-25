#===================================================================
# FILE: asm.pl
#
# SERVICES: Strips #line directives from preprocessed files
#           Filters out blank lines.
#
# $Header: //depot/asic/msmshared/secboot2/shared/1_0/tools/asm.pl#1 $
#
# (c) COPYRIGHT 1998 Qualcomm Incorporated.  All Rights Reserved.
# (c) COPYRIGHT 1999 Qualcomm Incorporated.  All Rights Reserved.
#                   QUALCOMM Proprietary/GTDR
#===================================================================
while(<>) {
   s/^\s*(#line )+(.)+//; 
   if (!/^\s*$/) {
      print $_ ;
   }
}
