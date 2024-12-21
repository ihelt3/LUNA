# Clean up before building
Remove-Item .\build -Force -Recurse
New-Item -Name "build" -ItemType "directory"

# Compile source file to object file
cl.exe /nologo /EHsc /std:c++20 /I. /I"..\..\src\solver/include/" /I"..\..\src\mesh\include" /I"..\..\src\math\include" /I"..\..\src\utilities\include" /I"..\..\src\boundary\include" /I"..\..\src\postprocessing\include" /O2 /c /O2 /DCOMPILELIB NACA0012.cc /Fo".\build\NACA0012.obj"

# Link object file to libraries and dependencies
cl.exe /nologo /EHsc /std:c++20 .\build\NACA0012.obj /Fe".\build\NACA0012.exe" /link /MACHINE:x64 /LIBPATH:"..\..\src\boundary\build" boundaries.lib /LIBPATH:"..\..\src\math\build" mathLib.lib /LIBPATH:"..\..\src\mesh\build" meshReader.lib /LIBPATH:"..\..\src\solver\build" Solver.lib /LIBPATH:"..\..\src\utilities\build" utilities.lib /LIBPATH:"..\..\src\postprocessing\build" postprocessing.lib

# Clean up
Remove-Item build\*.obj

# run executable
.\build\NACA0012.exe