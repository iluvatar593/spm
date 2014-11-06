#!/bin/sh
SQUARE_SIZES="240 480 960 1080 1200 1320 1440 1560 1920"
WORKERS="1 2 4 8 16 20 32 50 59 60 61 70 90 100 119 120 121 140 160 179 180 181 200 220 239 240"
for size in $SQUARE_SIZES
do
	for work in $WORKERS 
	do
		for i in `seq 1 $2`
		do
			./FarmIntrinsicDouble_Mic.o $1 $work $size $size $size 1 >> "./farmTests/square-$size.txt"
		done
	done

done
