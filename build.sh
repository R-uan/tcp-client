mkdir -p build
cmake -S . -B build -G "Ninja"
cmake --build build
