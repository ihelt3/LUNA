# clean up before building
Write-Host "Initializing build directory..."
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source files into object files
Write-Host "Compiling source files into object files..."
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I "C:\libraries\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\read_base.cc /Fo".\build\read_base.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I "C:\libraries\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\read_msh.cc /Fo".\build\read_msh.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I "C:\libraries\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\read_su2.cc /Fo".\build\read_su2.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I "C:\libraries\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\MeshEntities.cc /Fo".\build\MeshEntities.obj"
cl.exe /nologo /EHsc /std:c++20 /I. /I"include" /I "C:\libraries\include" /I"..\math\include" /c /O2 /DCOMPILELIB .\src\mesh.cc /Fo".\build\mesh.obj"
# cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\src\partitioning.cc /Fo".\build\partitioning.obj"
# cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\mainTest.cc /Fo".\build\mainTest.obj"
# cl.exe /nologo /EHsc /std:c++20 /I. /I "C:\libraries\include" /c /O2 /DCOMPILELIB .\test\su2\test_su2.cc /Fo".\build\test_su2.obj"
cl.exe /nologo /EHsc /std:c++20 /I"include" /I "C:\libraries\include" /I"..\math\include" /c /DCOMPILELIB .\main.cc /Fo".\build\main.obj"

# link object files to any necessary libraries and dependences and execute
# cl.exe /nologo /EHsc /std:c++20 .\build\mainTest.obj .\build\test_su2.obj .\build\read_base.obj .\build\read_su2.obj .\build\read_msh.obj .\build\MeshEntities.obj .\build\partitioning.obj /Fe".\build\meshTest.exe" /link /MACHINE:x64 /LIBPATH:"C:\libraries\lib" gtest.lib
cl.exe /nologo /EHsc /std:c++20 .\build\main.obj .\build\read_base.obj .\build\read_su2.obj .\build\read_msh.obj .\build\MeshEntities.obj .\build\mesh.obj /Fe".\build\meshTest.exe" /link /MACHINE:x64 /LIBPATH:"..\math\build" mathLib.lib  /LIBPATH:"C:\libraries\lib" gtest.lib
.\build\meshTest.exe