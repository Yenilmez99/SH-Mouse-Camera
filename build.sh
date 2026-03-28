#!/bin/bash
set -e

echo "== C++ DLL is being compiled =="
cmake -B build -DCMAKE_TOOLCHAIN_FILE=mingw32-toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build
echo "== C++ DLL was compiled =="

echo "== C# Config DLL is being compiled =="
dotnet build SH-Mouse-Camera.csproj -c Release
echo "== C# DLL was compiled =="
echo "== All Project were compiled successfully=="
