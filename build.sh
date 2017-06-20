#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

Warnings="-Wno-null-dereference -Wno-potentially-evaluated-expression -Wno-c++11-compat-deprecated-writable-strings"

pushd build > /dev/null

clang ../code/transaction_import/budget_import.cpp -Wno-null-dereference -g -lsqlite3 -o import_trans

popd > /dev/null
