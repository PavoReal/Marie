CPP_WARN="-Wall -Wextra"
CPP_FLAGS="-g -O0 -std=c++14 -fno-exceptions -fno-rtti $CPP_WARN"

PRODUCT_EXE=marie-exe
PRODUCT_ASM=marie-asm

mkdir -p build/

pushd build/ > /dev/null

clang++ $CPP_FLAGS ../src/asm_marie.cpp -o $PRODUCT_ASM
clang++ $CPP_FLAGS ../src/exe_marie.cpp -o $PRODUCT_EXE

cp ../src/*.mar ./

popd > /dev/null

mkdir -p data/

cp build/$PRODUCT_ASM data/
cp build/$PRODUCT_EXE data/

cp build/*.mar data/

