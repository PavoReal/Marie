CPP_WARN="-Wall -Wextra -Werror -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wold-style-cast -Wuseless-cast -Wshadow -Wformat=2"
CPP_FLAGS="-g -O0 -std=c++14 -fno-exceptions -fno-rtti $CPP_WARN"

PRODUCT_EXE=marie-exe
PRODUCT_ASM=marie-asm

mkdir -p build/

pushd build/ > /dev/null

echo
echo -e "\e[7masm_marie.cpp...\e[0m"
g++ $CPP_FLAGS ../src/asm_marie.cpp -o $PRODUCT_ASM

echo -e "\e[7mexe_marie.cpp...\e[0m"
g++ $CPP_FLAGS ../src/exe_marie.cpp -o $PRODUCT_EXE
echo -e "\e[7m\ncompilation done...\e[0m"
cp ../src/*.mar ./

popd > /dev/null

mkdir -p data/

cp build/$PRODUCT_ASM data/
cp build/$PRODUCT_EXE data/

cp build/*.mar data/

