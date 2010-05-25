#!/bin/sh

# This script will first clean the input .csv file of faulty lines.
# It will then extract the specified portion of the input file into another
# .csv file by the name <input_file_base_name>_<start_time>_<end_time>.csv
# and shift the timestamps and sample numbers in the output file to start at 0.
# Next it runs hp_stats.exe on the output file
#
# Usage: extract.sh <input .csv file> <start time in millisec> <end time in ms>
# The above will extract the portion of the input file from the specified 
# start time to end time
#
# eg: if tmp/tmp.csv has samples from 0 sec to 45 sec,
# extract.sh tmp/tmp.csv 100 1200
# will extract the portion of tmp/tmp.csv from 100 millisec to 1.2 sec
# into the file tmp/tmp_100_1200.csv
# It then runs hp_stats.exe tmp/tmp_100_1200.csv
#
# Dependencies: The files extract.awkscript, hp_stats.exe and hp_stats.ini
# should be in the same directory as this script
#
# Note: sometime hp_stats.exe gives a seg_fault. If this happens, try changing
# the start/end time by a little bit.
#

outfile=${1%.csv}_${2}_${3}.csv
echo "Output in "${outfile}
rm -f temp
sed -e "/,. ....,/d" $1 > temp; mv temp $1
awk -f extract.awkscript -v "start_time="$2 -v "end_time="$3 $1 > ${outfile}
./hp_stats.exe -n -p $4 ${outfile}



