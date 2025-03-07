# 2 Ship 2 Harkinian Switch
[![generate-builds](https://github.com/YoshiCrystal9/2ship2harkinian-Switch/actions/workflows/main.yml/badge.svg)](https://github.com/YoshiCrystal9/2ship2harkinian-Switch/actions/workflows/main.yml)

### How to use

1. download Windows build from here https://github.com/HarbourMasters/2ship2harkinian/actions/runs/13642391583
2. extract zip
3. put your mm rom *.z64 file in folder
4. start 2ship.exe
5. when done, you have 2ship.o2r and mm.o2r files, keep them
6. download switch files here https://github.com/YoshiCrystal9/2ship2harkinian-Switch/releases/tag/1.1.2
7. extract zip
8. put your previous .o2r files in folder
9. put folder on your Switch's "switch" folder

(thx Jero for the instructions)

### Switch Building
```bash
cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake/ --target Generate2ShipOtr

cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
# Build project and generate nro
cmake --build build-switch --target 2ship_nro

```
