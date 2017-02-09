#!/bin/bash
for COUNTER in {1..16}
do
	echo $COUNTER
	let limit=$((1000000000/$COUNTER))
	echo "Premiere batch dexperimentation:"
	echo "$limit"
	./Primes $limit
	echo "#######################"
done
