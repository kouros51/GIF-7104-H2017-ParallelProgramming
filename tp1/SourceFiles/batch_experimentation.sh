#!/bin/bash
echo "Test script for GIF-7104 TP1"
echo " "
echo "Sequential tests"
for COUNTER in {1..16}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 1
	echo "____________________________________"
	echo " "
done


echo "###################################################################"

echo "Parallel tests"
for COUNTER in {1..16}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 4
	echo "____________________________________"
	echo " "
done
