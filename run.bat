@echo off

pushd build\

call marie-asm.exe test.mar 
call marie-exe.exe a.marie

popd
