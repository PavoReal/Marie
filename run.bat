@echo off

pushd build\

call marie-asm.exe test2.mar 
call marie-exe.exe a.marie

popd
