# clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
cl.exe /nologo /EHsc /std:c++20 /I"..\mesh\include" /I"include" /I"..\math\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\fields.cc /Fo".\build\fields.obj"

# link static library
lib.exe /nologo /OUT:build\utilities.lib build\fields.obj
# compile tests
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include" /I"..\mesh\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\test\test_fields\test_fields.cc /Fo".\build\test_fields.obj"

# link object files to any necessary libraries and dependencies, then execute
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_fields.obj /Fe".\build\fieldTest.exe" /link /MACHINE:x64 /LIBPATH:"..\math\build" mathLib.lib /LIBPATH:"C:\libraries\lib" gtest.lib /LIBPATH:"..\mesh\build" meshReader.lib /LIBPATH:".\build" utilities.lib /LIBPATH:"..\solver\build" Solver.lib

# Clean up
Remove-Item build\*.obj

# test executable
.\build\fieldTest.exe
