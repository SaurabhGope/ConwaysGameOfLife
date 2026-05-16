# Life3D Architecture Notes

## Main Runtime Flow

Entry point:

- `src/main.cpp`
- constructs `AppConfig`
- runs `Application`

Runtime owner:

- `src/core/Application.cpp`
- initializes GLFW, Window, ImGui, seed engine, simulation, camera, and renderer
- runs fixed-rate simulation stepping based on `stepsPerSecond`
- uploads alive cell indices to renderer each frame
- draws ImGui controls and applies emitted `UiActions`
- handles backend switch requests and OpenGL fallback

## Configuration

Defined in `include/core/AppConfig.hpp`.

Important enums:

- `GraphicsApi`: `Vulkan`, `OpenGL`, `D3D12`, `Metal`, `WebGPU`
- `RenderMode`: `Solid`, `Wireframe`, `PointCloud`
- `BoundaryMode`: `FixedDead`, `Toroidal`

Default render API is `Vulkan` by product direction. The app falls back to OpenGL if the requested backend is unavailable.

## Simulation

Core files:

- `include/sim/Grid3D.hpp`
- `include/sim/RuleSet.hpp`
- `include/sim/SimulationEngine.hpp`
- `src/sim/*.cpp`

`Grid3D` is bit-packed and tracks alive count internally. Direct mutable packed-word access was removed to protect `AliveCount()` correctness.

`RuleSet` still supports classic `B/S` rule strings. Compact legacy rules like `B5/S456` work. Separated multi-digit rules like `B5,10/S4,5,6` also work.

`WeightedRuleSet` adds:

- `std::array<float, 26> neighborWeights`
- birth threshold range
- survival threshold range
- boundary mode
- fixed neighbor offset ordering through `NeighborOffsets()`

`SimulationEngine` uses `WeightedRuleSet` internally. The `RuleSet` constructor remains for compatibility and converts to weighted rules.

## Seeding

Core files:

- `include/seed/SeedTypes.hpp`
- `include/seed/SeedEngine.hpp`
- `src/seed/SeedEngine.cpp`

`SeedEngine` registers default generators in its constructor:

- cube
- sphere
- line
- deterministic noise

Noise uses `deterministicSeed` and density from `SeedRequest`.

## UI

Core files:

- `include/ui/UiLayer.hpp`
- `src/ui/UiLayer.cpp`

`UiLayer::Draw` takes mutable `AppConfig` and `WeightedRuleSet`, plus simulation and renderer stats. It emits `UiActions` rather than owning state transitions.

Current controls:

- run/pause
- single step
- reset
- steps per second
- grid size preset
- boundary mode
- graphics API
- render mode
- shader reload
- weighted thresholds
- 26 neighbor weights
- seed type, size, density, deterministic seed, inject seed
- simulation and renderer stats

## Rendering

Core files:

- `include/render/IRenderer.hpp`
- `src/render/RendererFactory.cpp`
- `include/render/opengl/OpenGLRenderer.hpp`
- `src/render/opengl/OpenGLRenderer.cpp`

`IRenderer` supports:

- initialize
- upload alive cell indices
- render frame
- resize
- reload shaders
- stats
- shutdown

`RendererStats` includes backend/device/status strings, CPU/GPU timing placeholders, dispatch timing placeholder, draw calls, rendered cell count, buffer capacity, and availability.

Only OpenGL is operational today. Planned backends return `UnsupportedRenderer` with clear status messages.

## Shaders

Shader assets live under:

- `shaders/opengl/cells.vert`
- `shaders/opengl/cells.frag`
- `shaders/vulkan/life3d.comp`

The OpenGL renderer currently uses simple compatibility behavior and does not yet compile/use these shader files for real instanced voxel rendering. The shader folder exists for the next renderer pass and is copied into the executable output directory.
