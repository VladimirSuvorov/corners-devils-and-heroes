cd ../..
rmdir .\build\Debug /S /Q
mkdir .\build\Debug
rmdir .\bin\Debug /S /Q
mkdir .\bin\Debug
rmdir .\lib\Debug /S /Q
mkdir .\lib\Debug
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build --config Debug
cmake --install ./build --prefix . --config Debug
rem cmake --target ../../. install
PAUSE