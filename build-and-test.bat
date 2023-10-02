set CMAKE="C:\Program Files\JetBrains\CLion 2023.2.2\bin\cmake\win\x64\bin\cmake.exe"
set GENERATOR=Ninja
set MAKE="C:\Program Files\JetBrains\CLion 2023.2.2\bin\ninja\win\x64\ninja.exe"
set CC="C:/msys64/mingw64/bin/clang.exe"
set CTEST="C:\Program Files\JetBrains\CLion 2023.2.2\bin\cmake\win\x64\bin\ctest.exe"

set BUILD_DIR cmake-build

%CMAKE% . -B %BUILD_DIR% -G %GENERATOR% -DCMAKE_MAKE_PROGRAM=%MAKE% -DCMAKE_C_COMPILER=%CC%
%CMAKE% —build %BUILD_DIR% —target all -j 6
cd %BUILD_DIR%
%CTEST%