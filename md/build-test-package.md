# Build, Test, and Package Notes

## Environment

Project root:

```text
D:\Codes\CellularAutomata3DEngine
```

Build system:

- CMake 3.24 minimum
- C++20
- Visual Studio/MSBuild on Windows
- FetchContent dependencies: GLFW, GLM, ImGui

The local PowerShell execution policy may block direct script execution. Use the bypass form if needed.

## Configure

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\configure.ps1
```

The configure script writes to `build/`.

## Build

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1
```

Expected executable:

```text
build\Debug\life3d.exe
```

The `shaders/` directory is copied next to the executable after build.

## Test

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\test.ps1
```

Last known result:

```text
1/1 Test #1: life3d_smoke ... Passed
100% tests passed
```

## Package

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\package.ps1
```

Last known package output:

```text
build\Life3D-0.1.0-windows-x64.zip
```

Last known package size:

```text
548914 bytes
```

## Sandbox Note

In the Codex desktop environment, CMake/MSBuild may need escalation to access Windows SDK metadata under the user profile. The observed sandbox failure was:

```text
Access to the path 'C:\Users\PC\AppData\Local\Microsoft SDKs' is denied.
```

Rerunning configure/build/test/package with approved elevated shell permissions succeeded.
