cd ../..
rmdir .\build\Release /S /Q
mkdir .\build\Release
rmdir .\bin\Release /S /Q
mkdir .\bin\Release
rmdir .\lib\Release /S /Q
mkdir .\lib\Release
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build --config Release
cmake --install ./build --prefix .
rem cmake --build . --target install
rem C:\Windows\explorer.exe .
rem cd bin/Release
rem C:\Windows\explorer.exe .
PAUSE