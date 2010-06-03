#===================================================================
# FILE: asm.pl
#
# SERVICES: Strips #line directives from preprocessed files
#           Filters out blank lines.
#
# $PVCSPath:  L:/src/asw/MSM6050/vcs/asm.plv   1.0   23 Oct 2001 15:28:06   donb  $
# $Header: //depot/asic/msmshared/tools/jflash/JFLASH.03.00/asm.pl#1 $ $DateTime: 2007/03/02 18:20:12 $ $Author: rtilak $
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
