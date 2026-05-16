# Life3D Project Context

## Product Direction

Life3D is a C++20/CMake 3D cellular automata engine. The intended USP is a cellular automata simulation that scales on GPU and exposes mainstream graphics APIs as selectable backends.

Current product priorities:

- Windows x64 first.
- Simple portable ZIP package output.
- Vulkan-first direction for the GPU-scale path.
- OpenGL compatibility backend remains available.
- D3D12 is planned after Vulkan reaches feature parity.
- Performance optimization is a separate later stage after correctness and architecture are stable.

## Current Implementation State

The project has moved beyond the original Sprint 0 scaffold into a first interactive architecture pass.

Implemented now:

- GLFW window ownership through `Window`.
- `Application` owns the app loop, simulation, UI, renderer, camera, timing, seed engine, and backend switching.
- ImGui control panel for simulation and render controls.
- Weighted-neighbor CPU reference simulation.
- Fixed-dead and toroidal boundary behavior.
- Built-in seed generators: cube, sphere, line, deterministic noise.
- Renderer factory with OpenGL as active backend.
- Explicit planned backends: Vulkan, D3D12, Metal, WebGPU.
- Shader asset folder copied into build output and installed into packages.
- CPack ZIP packaging.
- Smoke test coverage for rules, weighted simulation, toroidal boundaries, seeds, and renderer factory behavior.

## Important Product Decisions

- "Weights" means a 26-neighbor editable kernel. Each neighbor position around a cell has a float weight.
- Weighted neighbor sum is compared against birth/survival threshold ranges.
- Existing `B/S` rule strings remain as presets and convert into weighted thresholds with equal weights.
- Vulkan and D3D12 are selectable API identities, but they are not real GPU render/simulation backends yet.
- If Vulkan is selected in the current build, initialization fails intentionally and the app falls back to OpenGL.

## Known Environment Result

On this machine, CMake did not find a Vulkan SDK:

```text
Vulkan SDK not found. Vulkan backend will remain unavailable in this build.
```

That is expected for the current staged implementation.
