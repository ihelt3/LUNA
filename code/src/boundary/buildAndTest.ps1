# clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\BoundaryConditions.cc /Fo".\build\BoundaryConditions.obj"
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\wall.cc /Fo".\build\wall.obj"
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\inlet.cc /Fo".\build\inlet.obj"
cl.exe /nologo /EHsc /std:c++20 /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /O2 /I. /c /O2 /DCOMPILELIB .\src\outlet.cc /Fo".\build\outlet.obj"

# link static library
lib.exe /nologo /OUT:build\boundaries.lib build\BoundaryConditions.obj build\wall.obj build\inlet.obj build\outlet.obj

# compile tests
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /c /O2 /DCOMPILELIB .\test\test_BoundaryConditions.cc /Fo".\build\test_BoundaryConditions.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /c /O2 /DCOMPILELIB .\test\test_wall.cc /Fo".\build\test_wall.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /c /O2 /DCOMPILELIB .\test\test_inlet.cc /Fo".\build\test_inlet.obj"
cl.exe /nologo /EHsc /std:c++20 /O2 /I. /I "C:\libraries\include" /I"include" /I"..\utilities\include" /I"..\mesh\include" /I"..\math\include" /I"..\solver\include" /c /O2 /DCOMPILELIB .\test\test_outlet.cc /Fo".\build\test_outlet.obj"

# link object files to any necessary libraries and dependencies, then execute
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_BoundaryConditions.obj .\build\test_wall.obj .\build\test_inlet.obj .\build\test_outlet.obj /Fe".\build\fieldTest.exe" /link /MACHINE:x64 /LIBPATH:"..\math\build" mathLib.lib /LIBPATH:"C:\libraries\lib" gtest.lib /LIBPATH:"..\mesh\build" meshReader.lib /LIBPATH:".\build" boundaries.lib /LIBPATH:"..\solver\build" Solver.lib /LIBPATH:"..\utilities\build" utilities.lib

# Clean up
Remove-Item build\*.obj

# test executable
.\build\fieldTest.exe
