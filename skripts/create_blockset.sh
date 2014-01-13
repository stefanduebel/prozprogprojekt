#!/usr/bin/bash
set -x

rm blockset*.png
COUNTER_1="0"
COUNTER_2="0"
COUNTER_3="0"
while [ "$COUNTER_1" -le 9 ]
do
	while [ "$COUNTER_2" -le 9 ]
	do
		if [ -f $COUNTER_3.png ]
		then
			if [ "$COUNTER_2" -eq 0 ]
			then
				cp $COUNTER_3.png blockset_$COUNTER_1.png
			else
				convert blockset_$COUNTER_1.png $COUNTER_3.png +append blockset_$COUNTER_1.png
			fi
		else
			if [ "$COUNTER_2" -eq 0 ]
			then
				cp blank.png blockset_$COUNTER_1.png
			else
				convert blockset_$COUNTER_1.png blank.png +append blockset_$COUNTER_1.png
			fi
		fi
		COUNTER_2=$((COUNTER_2 + 1))
		COUNTER_3=$((COUNTER_3 + 1))
	done
	if [ "$COUNTER_1" -eq 0 ]
	then
		cp blockset_$COUNTER_1.png blockset.png
	else
		convert blockset.png blockset_$COUNTER_1.png -append blockset.png
	fi
	COUNTER_1=$((COUNTER_1 + 1))
	COUNTER_2="0"
done
