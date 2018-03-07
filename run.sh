#! /bin/bash

pushd data/ > /dev/null

./marie-asm test.mar
./marie-exe a.marie

popd > /dev/null

