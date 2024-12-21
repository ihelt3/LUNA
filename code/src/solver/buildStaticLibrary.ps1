# clean up before building
Write-Host "Initializing build directory..."
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
Write-Host "Compiling source files into object files..."
cl.exe /nologo /EHsc /std:c++20 /I. /I "include" /I "..\mesh\include" /I "..\utilities\include" /I"..\boundary\include" /I "..\math\include" /c /O2 /DCOMPILELIB .\src\Solver.cc /Fo".\build\Solver.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "include" /I "..\mesh\include" /I "..\utilities\include" /I"..\boundary\include" /I "..\math\include" /c /O2 /DCOMPILELIB .\src\SIMPLE.cc /Fo".\build\SIMPLE.obj"

# Link static library
lib.exe /nologo /OUT:build\Solver.lib build\Solver.obj build\SIMPLE.obj

# Compile Tests
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /I "..\mesh\include" /I "..\utilities\include" /I"..\boundary\include" /I "..\math\include" /c /O2 /DCOMPILELIB .\test\Solver\test_Solver.cc /Fo".\build\test_Solver.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /I "..\mesh\include" /I "..\utilities\include" /I"..\boundary\include" /I "..\math\include" /c /O2 /DCOMPILELIB .\test\SIMPLE\test_SIMPLE.cc /Fo".\build\test_SIMPLE.obj"

# link library to main executable
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_Solver.obj .\build\test_SIMPLE.obj /Fe".\build\solverTest.exe" /link /MACHINE:x64 /LIBPATH:"..\math\build" mathLib.lib /LIBPATH:"..\utilities\build" utilities.lib /LIBPATH:"..\mesh\build" meshReader.lib /LIBPATH:.\build Solver.lib /LIBPATH:"..\boundary\build" boundaries.lib /LIBPATH:"C:\libraries\lib" gtest.lib

# clean up
# Remove-Item build\*.obj

# test executable
.\build\solverTest.exe