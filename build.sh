#! /bin/bash

CPP_WARN="-Wall -Wextra"
CPP_FLAGS="-g -O0 -std=c++14 -fno-exceptions -fno-rtti $CPP_WARN"

PRODUCT=marie-vm

mkdir -p build/

pushd build/ > /dev/null

time clang++ $CPP_FLAGS ../src/marie.cpp -o $PRODUCT

cp ../src/*.mar ./

popd > /dev/null

mkdir -p data/

cp build/$PRODUCT data/
cp build/*.mar data/

