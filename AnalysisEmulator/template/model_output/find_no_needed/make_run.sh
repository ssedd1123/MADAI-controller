for (( i=0; i<500; i++ ))
do
	printf -v SET "run%04d" $i
	mkdir $SET
done
