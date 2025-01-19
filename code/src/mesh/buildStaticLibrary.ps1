# clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\read_base.cc /Fo".\build\read_base.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I"..\math\include" /I"..\utlities\include" /c /O2 /DCOMPILELIB .\src\read_su2.cc /Fo".\build\read_su2.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\MeshEntities.cc /Fo".\build\MeshEntities.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\mesh.cc /Fo".\build\mesh.obj"

# link static library
lib.exe /nologo /OUT:build\meshReader.lib build\read_base.obj build\read_su2.obj build\MeshEntities.obj build\mesh.obj

# compile tests
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /I"include" /I"..\math\include" /c /O2 /DCOMPILELIB .\test\su2\test_su2.cc /Fo".\build\test_su2.obj"

# link library into main executable
cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_su2.obj /Fe".\build\meshTest.exe" /link /MACHINE:x64 /LIBPATH:"..\math\build\Release" mathLib.lib /LIBPATH:.\build meshReader.lib /LIBPATH:"C:\libraries\lib" gtest.lib

# clean up
Remove-Item build\*.obj

# test executable
.\build\meshTest.exe