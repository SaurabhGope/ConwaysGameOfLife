# Next-Agent Handoff

## Current Git State

The repository had no commits when this context was saved. `git status --short` showed all project files as untracked.

Do not assume a clean committed baseline. Check status before editing and avoid reverting unrelated user work.

## What Was Just Implemented

The requested development plan was implemented as a first staged, buildable pass:

- architecture ownership moved into `Application`
- weighted CPU reference simulation added
- ImGui UI controls added
- renderer factory added
- OpenGL compatibility backend kept operational
- Vulkan/D3D12/etc. represented as planned unavailable backends
- seed generators added
- CPack ZIP packaging added
- stateful structured logger added and integrated
- smoke tests expanded
- README updated

## Important Caveats

- Vulkan is not a real backend yet.
- D3D12 is not a real backend yet.
- The OpenGL renderer still does not render actual instanced cells; it reports uploaded alive cells and clears with activity-dependent color.
- Shader files exist, are copied/packaged, but are not yet loaded by the OpenGL renderer.
- GPU simulation buffers and compute dispatch are not implemented yet.
- UI is functional but utilitarian; no docking/layout persistence or advanced editor UX yet.
- Logger is thread-safe and structured, but currently synchronous. If simulation/render threads are split later, consider adding a bounded async queue and backpressure policy.

## Recommended Next Stage

Implement an actual OpenGL visible-cell renderer before Vulkan work.

Reason:

- It validates the renderer data path with visible output.
- It makes the UI immediately meaningful.
- It establishes shader loading, buffer upload, VAO/VBO lifecycle, and render stats before duplicating those concerns in Vulkan.

Concrete next tasks:

- Compile `shaders/opengl/cells.vert` and `cells.frag` at runtime or embed them in `OpenGLShaders.cpp`.
- Convert alive linear indices into point or cube instance positions.
- Add VBO/VAO lifecycle to `OpenGLRenderer`.
- Draw alive cells in `PointCloud` mode first.
- Add basic camera orbit/pan/zoom controls.
- Update smoke or add a renderer unit boundary test for shader asset discovery.

## Vulkan Stage Guidance

After visible OpenGL rendering works:

- Require or document Vulkan SDK setup.
- Add `life3d_render_vulkan` files rather than bloating the OpenGL backend.
- Implement Vulkan instance, physical device selection, logical device, queues, swapchain, render pass, pipeline, command buffers.
- Add storage buffers for current/next cell state and weighted rule data.
- Start with compute shader correctness on small grids before tuning dispatch sizes.
- Keep CPU reference simulation as the correctness oracle.

## Testing Guardrails

Keep these behaviors covered:

- classic `B/S` parsing
- separated multi-digit rule parsing
- weighted threshold birth/survival
- toroidal wrapping
- deterministic seed reproducibility
- backend factory reports unsupported planned APIs clearly
- package target still includes shaders and README

## Style Guardrails

- Preserve C++20 and existing CMake target structure.
- Keep app state ownership in `Application`; keep `UiLayer` action-oriented.
- Do not make unsupported backends silently no-op without status reporting.
- Keep optimization separate from correctness and observability work.
