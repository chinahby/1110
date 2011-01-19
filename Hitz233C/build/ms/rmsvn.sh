#!/usr/bin/sh
#write by aming 2010-0915
#将文件copy到cygwin的bin目录下，用sh rmsvn.sh执行实现递归删除svn的信息
ScanFolder()
{
	CURDIR=`pwd`
	FileName=`ls -a`
#	echo "current folder is $CURDIR"
	for file in $FileName
	do
		if [ $file = "." -o $file = ".." ];then
			continue
		fi
		
		Suffix=`echo $file | sed 's!.*\.!!'`
		if [ $Suffix = "svn" ];then
			rm -Rf $file
			echo "Delete svn folder $file"
		elif [ -d $file ];then
			echo "Enter folder $file"
			cd $file
			ScanFolder 
			cd ..		
		else
		:				
		fi
	done
}			

ScanFolder