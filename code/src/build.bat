@echo off
cd build/
set "DIRECTORY=%cd%"

for /r "%DIRECTORY%" %%i in (*) do (
    del /q "%%i"
)

cmake .. -G "MinGW Makefiles"
make

pause