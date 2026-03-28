#!/bin/bash
set -e

BUILD_TYPE=${1:-Release}

echo "== C++ DLL is being compiled ($BUILD_TYPE) =="
cmake -B build -DCMAKE_TOOLCHAIN_FILE=mingw32-toolchain.cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build
echo "== C++ DLL was compiled =="

echo "== C# Config DLL is being compiled ($BUILD_TYPE) =="
dotnet build SH-Mouse-Camera.csproj -c Release
echo "== C# DLL was compiled =="
echo "== All Project were compiled successfully=="
