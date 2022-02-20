echo off

set model=debug
set current=%cd%
:: rmdir /q /s %current%\build
md %current%\build
pushd %current%\build

:: cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=%model% ..
cmake -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=%model% ..
if errorlevel 1 goto fail

cmake --build . --config %model%
if errorlevel 1 goto fail

echo *********************************************
echo BUILD SUCCEEDED
echo *********************************************

goto end:

:fail
echo *********************************************
echo BUILD FAILED
echo *********************************************

:end
popd
