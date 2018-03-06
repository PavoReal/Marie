#! /bin/bash

pushd data/ > /dev/null

./marie-vm test.mar output.txt

cat output.txt | less

popd > /dev/null
