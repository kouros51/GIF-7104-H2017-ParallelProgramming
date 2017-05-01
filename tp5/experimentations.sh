#!/bin/bash
echo "Experimentation script for GIF-7104 TP5 - 2D Thermal Simulation"
echo " "
echo "Building the program"
mkdir build
cd build
cmake ..
make
echo "Building succeeded."
echo "___________________"

echo " For those tests we will fix the video 10 frame/sec"
echo ""
echo "Sequential tests:"
echo "================="
echo ""
echo ""
echo "===> matrix:100*100 threshold:1"
./executables/tp5_cpu_sequential -d 100 100 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:1"
./executables/tp5_cpu_sequential -d 200 300 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:1"
./executables/tp5_cpu_sequential -d 500 600 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:100*100 threshold:0.1"
./executables/tp5_cpu_sequential -d 100 100 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:0.1"
./executables/tp5_cpu_sequential -d 200 300 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:0.1"
./executables/tp5_cpu_sequential -d 500 600 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:100*100 threshold:0.01"
./executables/tp5_cpu_sequential -d 100 100 -s 0.01 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:0.01"
./executables/tp5_cpu_sequential -d 200 300 -s 0.01 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:0.01"
./executables/tp5_cpu_sequential -d 500 600 -s 0.01 -f ../configs/configFile -i 10 &&

echo "################### Sequential tests finished #####################"
echo "###################################################################"


echo ""
echo "Parallel tests"
echo "=============="
echo ""
echo ""
echo "===> matrix:100*100 threshold:1"
./executables/tp5_opencl -d 100 100 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:1"
./executables/tp5_opencl -d 200 300 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:1"
./executables/tp5_opencl -d 500 600 -s 1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:100*100 threshold:0.1"
./executables/tp5_opencl -d 100 100 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:0.1"
./executables/tp5_opencl -d 200 300 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:0.1"
./executables/tp5_opencl -d 500 600 -s 0.1 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:100*100 threshold:0.01"
./executables/tp5_opencl -d 100 100 -s 0.01 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:200*300 threshold:0.01"
./executables/tp5_opencl -d 200 300 -s 0.01 -f ../configs/configFile -i 10 &&

echo ""
echo ""
echo "===> matrix:500*600 threshold:0.01"
./executables/tp5_opencl -d 500 600 -s 0.01 -f ../configs/configFile -i 10 &&

echo "################### Parallel tests finished #######################"
echo "###################################################################"