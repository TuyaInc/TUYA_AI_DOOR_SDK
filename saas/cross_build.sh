#!/bin/bash
set -e

COMPILE_PREFIX=/home/rqg/toolchains/gcc-linaro-5.4.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

export AR=${COMPILE_PREFIX}ar
export AS=${COMPILE_PREFIX}as
export CC=${COMPILE_PREFIX}gcc
export CXX=${COMPILE_PREFIX}g++
export LD=${COMPILE_PREFIX}ld
export RANLIB='${COMPILE_PREFIX}ar -s'
export STRIP=${COMPILE_PREFIX}strip

rm -rf build
mkdir build
pushd build

cmake ..

make clean
make -j4

popd