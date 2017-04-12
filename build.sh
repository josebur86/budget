#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

WxWidgetIncludes="-I/usr/local/lib/wx/include/osx_cocoa-unicode-3.0 -I/usr/local/include/wx-3.0
-D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXMAC__ -D__WXOSX__ -D__WXOSX_COCOA__"
WxWidgetLibs="-L/usr/local/lib   -framework IOKit -framework Carbon -framework Cocoa -framework AudioToolbox -framework System -framework OpenGL -lwx_osx_cocoau_core-3.0 -lwx_baseu-3.0 -lwx_osx_cocoau_adv-3.0"

Warnings="-Wno-null-dereference -Wno-potentially-evaluated-expression -Wno-c++11-compat-deprecated-writable-strings"

pushd build > /dev/null

clang ../code/budget_import.cpp -Wno-null-dereference -g -lsqlite3 -o import_trans

clang++ ../code/budget.cpp $Warnings -g $WxWidgetIncludes $WxWidgetLibs -lsqlite3 -o budget

popd > /dev/null
