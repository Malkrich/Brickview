#!/bin/bash

shopt -s globstar; clang-format --verbose -i ./Brickview/Src/**/**.{h,cpp}
shopt -s globstar; clang-format --verbose -i ./BrickviewApp/Src/**/**.{h,cpp}
