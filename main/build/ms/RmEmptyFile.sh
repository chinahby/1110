lfs=`find . -name "*.lst"`
prefix=`echo $lfs | sed 's/\.lst//g'`
echo $prefix

for i in $prefix 
do		
	rm -f $i.lst
	rm -f $i.i
	rm -f $i.o
done	

seconddir=./W021/W021


rm $seconddir/boot_vectors.o
rm $seconddir/boot_data.o
rm $seconddir/boot_reset_handler.o
rm $seconddir/disp.o