@echo off

setlocal

if not exist .\build mkdir .\build
pushd .\build

REM Build the Transation Import Tool
cl /nologo /c /Tc..\code\sqlite\sqlite3.c
cl /nologo ..\code\budget_import.cpp /link sqlite3.obj /out:import_trans.exe

REM Build the Main Budget App
cl /nologo ..\code\budget.cpp /link sqlite3.obj 
popd

endlocal
