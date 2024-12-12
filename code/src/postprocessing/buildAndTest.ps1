# clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\solver\include" /I"..\mesh\include" /I"..\math\include" /I"..\utilities\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\postprocessingBase.cc /Fo".\build\postprocessingBase.obj"
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\solver\include" /I"..\mesh\include" /I"..\math\include" /I"..\utilities\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\writeASCII.cc /Fo".\build\writeASCII.obj"

# link static library
lib.exe /nologo /OUT:build\postprocessing.lib build\postprocessingBase.obj build\writeASCII.obj

# compile tests
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include"  /I"..\mesh\include" /I"..\solver\include" /I"..\utilities\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\test\test_postprocessingBase\test_postprocessingBase.cc /Fo".\build\test_postprocessingBase.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include"  /I"..\mesh\include" /I"..\solver\include" /I"..\utilities\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\test\test_writeASCII\test_writeASCII.cc /Fo".\build\test_writeASCII.obj"

# link object files to any necessary libraries and dependencies, then execute
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_postprocessingBase.obj .\build\test_writeASCII.obj /Fe".\build\postprocessingTest.exe" /link /MACHINE:x64 /LIBPATH:".\build" postprocessing.lib /LIBPATH:"..\mesh\build" meshReader.lib /LIBPATH:"..\solver\build" Solver.lib /LIBPATH:"..\math\build" mathLib.lib /LIBPATH:"..\utilities\build" utilities.lib /LIBPATH:"C:\libraries\lib" gtest.lib

# Clean up
Remove-Item build\*.obj

# test executable
.\build\postprocessingTest.exe