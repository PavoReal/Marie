@echo off

pushd D:\ 
WHERE cl >NUL 2> NUL
IF %ERRORLEVEL% NEQ 0 call dev
popd

set PRODUCT_ASM=marie-asm.exe
set PRODUCT_EXE=marie-exe.exe

set CL_FLAGS=/nologo /W3 /diagnostics:column /EHsc /D_CRT_SECURE_NO_WARNINGS
set CL_FLAGS_DEBUG=/Zi /Od

set LINK_LIBS=
set LINK_FLAGS=%LINK_LIBS% /nologo /MACHINE:x64 -incremental:no
set LINK_FLAGS_DEBUG=/DEBUG

mkdir build 2> NUL
pushd build\

cl %CL_FLAGS% %CL_FLAGS_DEBUG% ..\src\asm_marie.cpp /link %LINK_FLAGS% /OUT:%PRODUCT_ASM%
cl %CL_FLAGS% %CL_FLAGS_DEBUG% ..\src\exe_marie.cpp /link %LINK_FLAGS% /OUT:%PRODUCT_EXE% 

copy ..\src\*.mar .\ >NUL

popd

mkdir data\ 2> NUL

copy build\%PRODUCT_ASM% data\ >NUL
copy build\%PRODUCT_EXE% data\ >NUL
copy build\*.mar data\ >NUL

