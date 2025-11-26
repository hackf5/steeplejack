# Cubes One → ECS Migration Plan

Purpose: move `src/scenes/cubes_one` off the legacy `Scene/Model/Node/Mesh/Lights/Camera` stack and onto the ECS scaffolding. Keep the scene minimal and beautiful (one textured cube, ambient + two animated spotlights, shadows, orbit camera), and drop the ImGui/debug UI.

## Target Shape
- Scene host: ECS scene runner that plugs into the existing engine slot where `RenderScene` sits today. It owns an `ecs::Scene` plus references to `Device`, `GraphicsBuffers`, and `MaterialFactory` via `ecs::Resources`.
- Render contracts: still use the existing descriptor layouts (`config/descriptor_layouts/default.toml` and `shadow.toml`). Bindings must match: camera (0), model (1), baseColor (2), materialParams (3), normal (4), metallicRoughness (5), emissive (6), ambient (7), shadowMapArray (8), spot matrices (9). Shadow pass uses lightSpaceMatrix (0) and modelMatrix (1).
- Geometry/material: reuse the in-memory cube verts/indices from `cubes_one.cpp`; upload through `GraphicsBuffers`. Reuse the rock glTF material via `MaterialFactory`.
- ECS entities:
  - Root node: `Transform` + `Relationship` (depth 0).
  - Child node: `Transform` + `Relationship` (attached to root) for a second rotation layer.
  - Mesh node: `Transform` + `Relationship` (attached to child) + `RenderMesh` + `MaterialRef` + optional `ShadowReceiver`.
  - Camera: `Camera` + `MainCamera`.
  - Ambient: `Ambient`.
  - Spot 0/1: `SpotLight` (enable true, shadow indices 0/1).
- Systems per frame: transforms (hierarchy), cameras, spot lights (viewProj), light UBO flush, model UBO flush, camera UBO flush, draw-list build, render shadow per enabled spot, render forward.

## Bootstrapping the Scene
- Resources wiring: populate `ecs::Scene::resources` with `device`, `graphics_buffers`, `material_factory`. Keep `legacy_lights` null once ECS lighting is active.
- Geometry load: call the existing cube vertex/index builders; upload once via `GraphicsBuffers::load_vertexes` / `load_indexes`. Register a `GeometryId` in `GeometryStore`. Store `DrawRange{firstIndex=0, indexCount=INDEXES_COUNT, vertexOffset=0}` in `RenderMesh`.
- Material load: `MaterialFactory::load_gltf("rock", "rock_wall_15_1k/rock_wall_15_1k.gltf")`; store the returned `Material*` in `ecs::MaterialStore` to get a `MaterialId`.
- Entity construction:
  - Create root, child, mesh entities; attach via `ecs::attach`. Zero translation/scale, identity rotations. Mesh entity gets `RenderMesh{geometryId, draws=[range], casts_shadow=true}` and `MaterialRef{materialId}`.
  - Camera entity: set position via an orbit default (e.g., radius 3 at ~35°/45°), target = origin, fov 60°, near 0.1, far 10. Mark `dirty=true`, aspect set each frame.
  - Ambient: color (1,1,1), intensity 0.1.
  - Spot 0: enable true, radius 2.5, speed 45°/s around X, inner 5.5°, outer 14.5°, intensity 2.0, range 6.0, color (1.0, 0.95, 0.9), shadow_index = 0, casts_shadow = true.
  - Spot 1: enable true, radius 2.5, speed 60°/s around Y, inner 12°, outer 22°, intensity 0.0 (current default), range 6.0, color (0.9, 0.95, 1.0), shadow_index = 1, casts_shadow = true.

## Frame Update Logic (no ImGui)
- Time source: reuse the existing `time_delta()` helper (or engine-provided delta) to drive animation.
- Camera orbit: keep UI defaults from the old scene—rotX 35°, rotY 45°, rotZ 0°, distance 3.0. Recompute position from these angles and distance, set `Camera.position/target`, mark dirty, update aspect ratio.
- Model animation: optional flag defaulting to on; apply three rotations over time (x=90°/s, y=60°/s, z=30°/s) to the root/child/mesh transforms, then mark transforms dirty.
- Spot animation: if enabled, rotate spot 0 around X by `speed_deg * t`, spot 1 around Y by `speed_deg * t`; update position, direction (normalize toward origin), inner/outer cosines, intensity, color, range.
- Ambient: keep the defaults (1,1,1 @ 0.1) until a future UI replaces ImGui.
- Systems order each frame:
  1) Write transient component data (camera aspect, transforms, spot params).
  2) `update_transforms(reg)`.
  3) `update_cameras(reg)`.
  4) `update_spot_lights(reg)` to fill `viewProj`.
  5) Flush UBOs: model (binding 1), camera (binding 0), ambient/spot arrays (graphics binding 7/9), shadow light matrix (shadow binding 0) per light; leave material UBO/texture binding untouched (materials flush themselves when needed).
  6) Build draw list and render shadow for each enabled spot with a shadow index, then main forward pass.

## Rendering Contracts
- Shadow pass: for each enabled `SpotLight` with `casts_shadow` and `shadow_index >= 0`, bind that index’s framebuffer and descriptor slice, set viewport/scissor/depth bias as today, write light VP at binding 0 and model UBOs at binding 1 (via `flush_model_ubos` + per-draw writes), draw ranges.
- Barrier: keep the existing image barrier before the main pass (`ShadowMapArray` to `DEPTH_STENCIL_READ_ONLY_OPTIMAL` visible to FRAGMENT_SHADER).
- Forward pass: reuse `render_forward` helper (or equivalent) to write descriptors:
  - model UBO → binding 1 per draw,
  - material params + textures → bindings 3/2/4/5/6,
  - camera UBO → binding 0 (once),
  - ambient/spot matrices UBOs → bindings 7/9 (once),
  - shadow map array sampler → binding 8 (once).

## Engine Wiring
- Replace the `RenderScene` allocation in `VulkanContextBuilder::add_scene` with an ECS scene runner (factory signature stays similar).
- Expose the ECS scene to the render loop: `VulkanEngine::run()` should call the ECS load hook once, then each frame call ECS `update(frame_index, aspect)` and the render hook.
- Shadow loop: iterate ECS spotlights rather than legacy `Lights::spots_size()`. Pick spots with `enable && casts_shadow && shadow_index >= 0`, use `shadow_index` to select the framebuffer layer and descriptor slice.
- Graphics loop: bind the same pipelines and buffers; delegate draws to ECS render routines after descriptor writes.

## Cleanup Path (after ECS draw parity)
- Remove `src/model` headers/impls and their includes from `RenderScene` and the build. Delete legacy scene plumbing that duplicates ECS responsibilities.
- Delete `src/scenes/cubes_one.*` once the ECS version lives alongside or replace its usage in `application.cpp`.
- Prune ImGui debug code; keep only minimal toggles in code (constants/flags) until a new UI path exists.

## Step-by-Step Execution Order
1) Add an ECS scene runner type that mirrors `RenderScene`’s lifecycle (`load`, `update`, `render_shadow`, `render`), owns `ecs::Scene`, and fills `ecs::Resources`.
2) Port cube geometry/index generation into a reusable helper (can live near the new scene) and load into `GraphicsBuffers`; register `GeometryId` + draw range.
3) Load the rock material via `MaterialFactory`, register it in `ecs::MaterialStore`, and capture the `MaterialId`.
4) Create entities (root/child/mesh, camera, ambient, spot0/spot1) with the components and defaults above; wire hierarchy via `ecs::attach`.
5) Implement per-frame update: camera orbit, optional model spin, animated spotlights; mark dirty where needed.
6) Implement ECS render hooks: flush UBOs (model, camera, lights), write descriptors to match current layouts, draw shadow pass per enabled spot, then forward pass via the ECS draw list.
7) Swap the engine to use the ECS scene runner in `application.cpp`/`VulkanContextBuilder`/`VulkanEngine`.
8) Validate (run): confirm cube renders with rock material, lights/shadows move; fix descriptor binding or system ordering if anything is black/missing.
9) Remove legacy `src/model` and old `cubes_one` wiring; update CMake/build files accordingly.

## Open Questions / Future Notes
- Should light data stay in ECS-only UBOs, or should we temporarily mirror into legacy `Lights` for shader parity? (Prefer ECS-only; only bridge if necessary for existing shader uniforms.)
- Do we want a small config to flip model/spot animation defaults without ImGui? (Could be constants in the scene runner for now.)
- Shadow rendering currently stubs `render_shadow` in ECS; wiring it during this migration is required for parity.
