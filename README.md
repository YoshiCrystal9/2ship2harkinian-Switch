# 2 Ship 2 Harkinian Switch

### Switch Build
```bash
cmake -H. -Bbuild-cmake -GNinja
cmake --build build-cmake/ --target Generate2ShipOtr

cmake -H. -Bbuild-switch -GNinja -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
# Build project and generate nro
cmake --build build-switch --target 2ship_nro

```