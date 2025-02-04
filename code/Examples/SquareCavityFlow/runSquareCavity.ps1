# Clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source file to object file
cl.exe /nologo /EHsc /std:c++20 /I. /I"..\..\src\solver\include" /I"..\..\src\mesh\include" /I"..\..\src\math\include" /I"..\..\src\utilities\include" /I"..\..\src\boundary\include" /I"..\..\src\postprocessing\include" /O2 /c /O2 /DCOMPILELIB squareCavityFlow.cc /Fo".\build\squareCavityFlow.obj"

# Link object file to libraries and dependencies
cl.exe /nologo /EHsc /std:c++20 .\build\squareCavityFlow.obj /Fe".\build\squareCavityFlow.exe" /link /MACHINE:x64 /LIBPATH:"..\..\src\build\boundary\Release" boundary.lib /LIBPATH:"..\..\src\build\math\Release" math.lib /LIBPATH:"..\..\src\build\mesh\Release" meshReader.lib /LIBPATH:"..\..\src\build\solver\Release" solver.lib /LIBPATH:"..\..\src\build\utilities\Release" utilities.lib /LIBPATH:"..\..\src\build\postprocessing\Release" postprocessing.lib

# Clean up
Remove-Item build\*.obj

# run executable
.\build\squareCavityFlow.exe