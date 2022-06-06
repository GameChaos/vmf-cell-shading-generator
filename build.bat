@echo off

where /q cl
IF ERRORLEVEL 1 call "set_up_msvc.bat"

set CommonCompilerFlags=-MTd -nologo -fp:fast -GR- -EHa- -O2 -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4533 -FC -Z7 -DGC_DEBUG=1 -D_CRT_SECURE_NO_WARNINGS 
set CommonLinkerFlags= -incremental:no -opt:ref

pushd project
IF NOT EXIST build mkdir build
pushd build

set startTime=%time%
cl %CommonCompilerFlags% ..\code\vmf_cell_shading.cpp /link -subsystem:console %CommonLinkerFlags%

popd
popd
echo Compile Started: %startTime%
echo Compile Ended:   %time%