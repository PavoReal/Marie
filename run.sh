#! /bin/bash

pushd build/ > /dev/null

./marie-asm test2.mar
./marie-exe a.marie

popd > /dev/null

