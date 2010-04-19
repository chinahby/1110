lfs=`find . -name "*.lst"`
prefix=`echo $lfs | sed 's/\.lst//g'`
echo $prefix

for i in $prefix 
do		
	rm -f $i.lst
	rm -f $i.i
	rm -f $i.o
done	

rm SBNAALZ/boot_vectors.o
rm SBNAALZ/boot_data.o
rm SBNAALZ/boot_reset_handler.o