#!/usr/bin/sh
CURDIR=${PWD##*/}
FileName=`ls -a`
echo CURDIR=$CURDIR
echo FileName=$FileName
for file in $FileName
    do
        if [ $file = "." -o $file = ".." -o $file = ".svn" ]; then
            continue
        fi
        
        Suffix=`echo $file | sed 's!.*\.!!'`
        echo $Suffix
        if [ $Suffix = "mod" -o $Suffix = "mif" -o $Suffix = "bar" ]; then
            continue
        fi
        
        echo $file
    	mv $file $CURDIR"_"$file
    done


