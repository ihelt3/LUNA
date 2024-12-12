# clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /c /O2 /DCOMPILELIB .\src\linearSolvers.cc /Fo".\build\linearSolvers.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /c /O2 /DCOMPILELIB .\src\sparseMatrix.cc /Fo".\build\sparseMatrix.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /c /O2 /DCOMPILELIB .\src\Vector.cc /Fo".\build\Vector.obj"

# link static library
lib.exe /nologo /OUT:build\mathLib.lib build\linearSolvers.obj build\sparseMatrix.obj build\Vector.obj

# compile tests
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /c /O2 /DCOMPILELIB .\test\testSparseMatrix\testSparseMatrix.cc /Fo".\build\testSparseMatrix.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /c /O2 /DCOMPILELIB .\test\testLinearSolvers\testLinearSolvers.cc /Fo".\build\testLinearSolvers.obj"

# link library into main executable
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\testSparseMatrix.obj .\testLinearSolvers.obj /Fe".\build\mathTest.exe" /link /MACHINE:x64 /LIBPATH:.\build mathLib.lib /LIBPATH:"C:\libraries\lib" gtest.lib

# clean up
Remove-Item build\*.obj

# test executable
.\build\mathTest.exe