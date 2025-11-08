# Conway's Game of Life

## Setup
CMakeList was set up to be a subdirectory of the Beginning C++ Game Programming - Second Edition repository.

Follow the cloning instructions:
```
git clone https://github.com/PacktPublishing/Beginning-Cpp-Game-Programming-Second-Edition.git
cd Beginning-Cpp-Game-Programming-Second-Edition.git
git clone https://github.com/pamin1/GameOfLife.git
cd ..
```

Now add the following line to the game programming CMakeLists.txt:
```
add_subdirectory(GameOfLife)
```

## Build
Follow the build instructions:
```
cd Beginning-Cpp-Game-Programming-Second-Edition
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 20
make
```