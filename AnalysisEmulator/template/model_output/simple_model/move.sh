for (( i=40; i<100; ++i ))
do
	printf -v NAME "run%04d" $i
	mv $NAME notused/
done 
