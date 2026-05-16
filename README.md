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

The packaged build is emitted as `build/Life3D-0.1.0-windows-x64.zip`.

## Backend roadmap

The app is Vulkan-first by product direction, but this build keeps Vulkan and D3D12 as selectable planned backends until their device, swapchain, shader, and GPU simulation implementations are complete. If Vulkan is selected without a Vulkan backend implementation, the app falls back to OpenGL and reports the backend status in the UI.
