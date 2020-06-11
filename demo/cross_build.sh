#!/bin/bash
set -e

COMPILE_PREFIX=/opt/arm-himix200-linux/bin/arm-himix200-linux-

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
make -j $(nproc)

popd