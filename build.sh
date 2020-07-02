#!/bin/bash

echo "Cleaning build..."
make clean
make mrproper

echo "Compiling..."
export ARCH=arm
export CROSS_COMPILE=$(pwd)/arm-linux-androideabi-4.9/bin/arm-linux-androideabi-

mkdir output

make -C $(pwd) O=output msm8909-draco-1gb-perf_defconfig
make -j$(nproc) -C $(pwd) O=output
