#!/usr/bin/perl
# ------------------------------------------------------------------------
# Send output to STDOUT and to a named file
# Copyright (c) 2003 QUALCOMM Inc. All Rights Reserved.
# $Header: //depot/asic/qsc1100/tools/build/qtee.pl#1 $
# ------------------------------------------------------------------------
$filename=$ARGV[0];

open(OUT,">$filename");
while(<STDIN>){
        print;
        print OUT;
}
