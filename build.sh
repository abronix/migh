#!/bin/bash

model=release
current="$(pwd)"
## rm -rf "$current/build"
mkdir -p "$current/build"
cd "$current/build"

function show_build_failed
{
  echo "*********************************************"
  echo "BUILD FAILED"
  echo "*********************************************"
}

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$model ..
if [[ $? -ne 0 ]]; then
  show_build_failed
  exit 1
fi

cmake --build . --config $model -- -j4
if [[ $? -ne 0 ]]; then
  show_build_failed
  exit 1
fi

echo "*********************************************"
echo "BUILD SUCCEEDED"
echo "*********************************************"
