## game

My simple game made with SDL3

## Installation

Game requires:

- SDL3
- GLM
- CMake 3.20+

## Android:

- API level 16+

### Build

Linux:

```
mkdir build
cd build
cmake ..
cmake --build .
```

Windows:

```
mkdir build
cd build
mingw64-cmake
cmake --build .
```

Android:

1) Open folder android-project in Android Studio
2) Provide path to SDL with variable `sdl3.dir` and to GLM with variable `glm.dir` in local.properties
3) Provide paths to SDK and NDK
4) Build
