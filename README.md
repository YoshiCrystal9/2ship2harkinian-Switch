# 2 Ship 2 Harkinian Switch
[![generate-builds](https://github.com/YoshiCrystal9/2ship2harkinian-Switch/actions/workflows/main.yml/badge.svg)](https://github.com/YoshiCrystal9/2ship2harkinian-Switch/actions/workflows/main.yml)

### How to use

1. Download a PC build from 2ship
2. extract zip
3. put your mm rom *.z64 file in folder
4. start 2ship.exe
5. when done, you will get mm.o2r
6. download switch files from release
7. extract zip on sd card and put folder on your Switch's "switch" folder
8. put your previous .o2r file in the folder

(thx Jero for the instructions)

### Switch Building
```bash
cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake/ --target Generate2ShipOtr

cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
# Build project and generate nro
cmake --build build-switch --target 2ship_nro

```
