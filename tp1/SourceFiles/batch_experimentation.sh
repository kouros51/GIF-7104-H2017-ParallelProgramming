#!/bin/bash
echo "Test script for GIF-7104 TP1"
echo " "
echo "Sequential tests"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 1
	echo "____________________________________"
	echo " "
done


echo "###################################################################"

echo "Parallel tests"

echo "Using 2 thread"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 2
	echo "____________________________________"
	echo " "
done

echo "##################################################################"

echo "Using 3 thread"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 3
	echo "____________________________________"
	echo " "
done

echo "##################################################################"

echo "Using 4 thread"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 4
	echo "____________________________________"
	echo " "
done

echo "##################################################################"

echo "Using 5 thread"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 5
	echo "____________________________________"
	echo " "
done

echo "##################################################################"

echo "Using 6 thread"
for COUNTER in {1..5}
do
	let limit=$((1000000000/$COUNTER))
	echo "Experimentation batch number $COUNTER with a limit of $limit"
	./Primes $limit 6
	echo "____________________________________"
	echo " "
done

echo "##################################################################"
