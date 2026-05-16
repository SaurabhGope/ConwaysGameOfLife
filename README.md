# Life3D Engine

Life3D Engine is a from-scratch 3D cellular automata simulation project focused on performance-oriented architecture and graphics pipeline understanding.

## Current status

- C++20 + CMake project scaffold
- Modular targets (`life3d_core`, `life3d_sim`, `life3d_seed`, `life3d_render`, `life3d_ui`, `life3d_app`, `life3d_tests`)
- GLFW + ImGui application loop with simulation, UI, renderer, camera, and backend lifecycle ownership
- Weighted-neighbor cellular automata reference simulation with fixed-dead and toroidal boundaries
- Built-in cube, sphere, line, and deterministic noise seed generators
- Renderer factory with OpenGL as the active compatibility backend and explicit staged backends for Vulkan, D3D12, Metal, and WebGPU
- Shader asset layout for OpenGL and staged Vulkan compute work
- Thread-safe structured logger with console/file sinks, source locations, categories, and `LIFE3D_LOG_LEVEL` / `LIFE3D_LOG_DIR` overrides
- Dependency bootstrap for GLFW, GLM, and ImGui
- CTest-integrated smoke coverage for rules, weighted simulation, toroidal boundaries, seeds, and backend factory behavior
- CPack ZIP packaging for a portable Windows x64 build

## Build

```powershell
./scripts/configure.ps1
./scripts/build.ps1
./scripts/test.ps1
./scripts/package.ps1
```

The package script emits both:

- `build/Life3D-0.1.0-windows-x64-Debug.zip`
- `build/Life3D-0.1.0-windows-x64-Release.zip`

## Logging

Runtime logs are written to `logs/` by default and mirrored to the console. Use these environment variables to tune logging:

```powershell
$env:LIFE3D_LOG_LEVEL = "debug" # trace, debug, info, warn, error, critical, off
$env:LIFE3D_LOG_DIR = "D:\Life3DLogs"
```

## Backend roadmap

The app is Vulkan-first by product direction, but this build keeps Vulkan and D3D12 as selectable planned backends until their device, swapchain, shader, and GPU simulation implementations are complete. If Vulkan is selected without a Vulkan backend implementation, the app falls back to OpenGL and reports the backend status in the UI.
