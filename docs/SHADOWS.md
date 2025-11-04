# Spotlight Shadow Mapping — Design, Status, and Plan

Overview

- Goal: Add shadow mapping for up to 8 spotlights using a single depth 2D array (one layer per spotlight), rendered in a pre‑pass and sampled during the main lighting pass.
- Scope: Keep architecture modular and mirror existing pipeline patterns. No immediate UI/ImGui wiring; focus on core rendering bricks.

High‑Level Flow

1) Shadow pass (per spotlight layer): render scene into a depth 2D array layer using a depth‑only render pass and a shadow pipeline.
2) Barrier: make the shadow map writes visible to fragment sampling.
3) Main pass: sample the shadow map array in the lighting shader with a comparison sampler (PCF), modulating each spotlight’s contribution.

Core Components (in repo)

- Shadow image and views
  - `src/vulkan/shadow_map_array.h/.cpp` — Owns the layered depth image (usage: DEPTH_ATTACHMENT | SAMPLED), one array view, and per‑layer views.
- Shadow framebuffers
  - `src/vulkan/shadow_framebuffers.h/.cpp` — Builds one framebuffer per array layer (depth attachment only).
- Shadow render pass
  - `src/vulkan/shadow_render_pass.h/.cpp` — Depth‑only subpass; final layout `DEPTH_STENCIL_READ_ONLY_OPTIMAL` for sampling.
- Shadow pipeline
  - `src/vulkan/shadow_pipeline.h/.cpp` — Depth‑only graphics pipeline, position‑only vertex input, front‑face culling, depth bias enabled; viewport/scissor/depth‑bias are dynamic.
- Shadow sampler
  - `src/vulkan/shadow_sampler.h/.cpp` — Comparison sampler: `compareEnable = true`, `compareOp = LEQUAL`, `CLAMP_TO_BORDER` with white border, linear filter (gives 2×2 PCF with arrayShadow).
- Pipeline helpers
  - `src/vulkan/pipeline.h/.cpp` — Shared creation helpers (input assembly, dynamic viewport, rasterization, depth‑stencil, color blend, dynamic states). Includes `create_shadow_rasterization_state()`.
- Scene lighting UBO + spots
  - `src/model/scene.h` — Holds ambient + up to 8 spotlights (kMaxSpots = 8), already bound at graphics binding 7.
- Default shaders
  - `shaders/shader.default.vert` / `shaders/shader.default.frag` — Forward shading with ambient + spotlights (no shadows yet). Fragment declares `kMaxSpots = 8` and loops through `spots`.

Rasterization and Depth Rules (shadow pass)

- Cull mode: FRONT (render back faces for the map; reduces self‑shadowing).
- Depth bias: enabled and set dynamically (typical start: constant ≈ 1.25, slope ≈ 1.75, clamp = 0.0).
- Depth compare (pipeline): use LESS when writing the shadow map. In the main pass shadow test, use LEQUAL so ties resolve as lit.
- Depth clamp: Only enable if the device feature is requested; otherwise keep `depthClampEnable = VK_FALSE`.
- Fragment stage: no color outputs. Use discard only for alpha‑cutout geometry; do not write `gl_FragDepth`.

What’s Implemented

- Data + infrastructure:
  - Depth array image + views per layer (`ShadowMapArray`).
  - Framebuffers per layer (`ShadowFramebuffers`).
  - Depth‑only render pass with final layout suitable for sampling (`ShadowRenderPass`).
  - Shadow pipeline with dynamic viewport/scissor/depth‑bias and front‑face culling (`ShadowPipeline`).
  - Comparison shadow sampler (`ShadowSampler`).
  - Graphics pipeline continues to use dynamic viewport/scissor; color blend state intact.

What’s Left To Do

1) Shadow shaders (user has implemented)
   - Vertex: consumes position only (location 0), UBO bindings: 0 = lightViewProj, 1 = model; outputs `gl_Position` only.
   - Fragment: no color outputs (optional discard for alpha‑cutout).

2) Per‑frame/per‑layer matrices
   - Main pass (fragment): Add a UBO with `mat4 lightViewProj[8]` bound at graphics binding 9.
   - Shadow pass (vertex): Provide a single `mat4` for the current layer at binding 0. Recommended: allocate one UBO sized `stride * layers`, where `stride` is aligned to `minUniformBufferOffsetAlignment`, and write at `offset = layer * stride`.
   - Compute each light’s VP:
     - view = `lookAt(pos, pos + dir, up)` (z‑up in current code; fall back to y‑up if near‑parallel).
     - proj = `perspective(2*acos(outerCos), aspect=1, near≈0.05–0.1, far=spot.range)`; flip Y: `proj[1][1] *= -1`.
     - VP = `proj * view`.

3) Engine integration (recording)
   - Use the same primary command buffer for the whole frame.
   - For each layer i:
     - `ShadowRenderPass::begin(cmd, framebuffer[i], resolution)`.
     - Bind `ShadowPipeline`.
     - Set dynamic state: viewport (0,0,w,h,0,1), scissor (0,0,w,h), depth bias (e.g., 1.25, 0.0, 1.75).
     - Update/push descriptors: binding 0 = lightViewProj[i] (offset slice), binding 1 = mesh model.
     - Draw the scene (same meshes/primitives).
     - `ShadowRenderPass::end(cmd)`.
   - Insert an image barrier (outside any render pass) to make depth writes visible to sampling:
     - srcStage: EARLY_FRAGMENT_TESTS_BIT | LATE_FRAGMENT_TESTS_BIT
     - srcAccess: DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
     - dstStage: FRAGMENT_SHADER_BIT
     - dstAccess: SHADER_READ_BIT
     - old/new layout: VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL (final layout of shadow pass; maintain for sampling)
     - subresource: aspect = DEPTH, baseArrayLayer = 0, layerCount = all, levelCount = 1.

4) Descriptor wiring for the main pass
   - Extend graphics descriptor set layout (application builder) with:
     - binding 8: combined image sampler (fragment) for `sampler2DArrayShadow`.
     - binding 9: uniform buffer (fragment) for shadow matrices array.
   - Before `push_descriptor_set()` in scene render:
     - Write binding 8 with { sampler = `ShadowSampler`, imageView = shadow array view, layout = `DEPTH_STENCIL_READ_ONLY_OPTIMAL` }.
     - Write binding 9 with the matrices UBO for the current frame.

5) Fragment shader sampling
   - Add uniforms:
     - `layout(binding = 8) uniform sampler2DArrayShadow uShadowMap;`
     - `layout(binding = 9, std140) uniform ShadowMatrices { mat4 lightViewProj[8]; };`
   - For each enabled spot i:
     - `l = lightViewProj[i] * vec4(worldPos,1)`; `p = l.xyz / l.w`.
     - `uvz = p * 0.5 + 0.5` (maps clip −1..1 → 0..1; GLM’s default projection gives OpenGL clip z in −1..1).
     - If uv outside [0,1] or `uvz.z > 1.0` → visibility = 1.0.
     - Else `vis = texture(uShadowMap, vec4(uvz.xy, float(i), uvz.z))` (compare LEQUAL in sampler).
     - Multiply the light’s diffuse term by `vis`.

6) Small convenience APIs (optional but helpful)
   - `ShadowFramebuffers::layer(uint32_t i) const` to index the per‑layer framebuffer.
   - `ShadowMapArray::image()` or `operator VkImage()` to build the barrier.
   - `BufferHost::copy_from_at(const T&, VkDeviceSize offset)` to write into a UBO slice at an aligned offset (used for per‑layer shadow VP).

Constants and Consistency

- Max spots: `SceneLights::kMaxSpots = 8` (`src/model/scene.h`) and `const int kMaxSpots = 8` in the fragment shader. Keep them in sync (later we can generate a shared header or define a compile‑time constant for the shader compiler).
- Depth compare contract:
  - Shadow pass (write): LESS (or LEQUAL; LESS + sampler LEQUAL is a common combination that treats ties as lit).
  - Shadow test (sample): LEQUAL (in comparison sampler).
  - Main scene depth compare is independent (typically LESS/LEQUAL unless you use reversed Z across the whole pipeline).

Tuning and Pitfalls

- Bias tuning: Start with constant ≈ 1.25 and slope ≈ 1.75; adjust per scene/resolution/FOV. Too small → acne; too large → peter‑panning.
- Depth clamp: Only enable if requested at device creation; otherwise leave it off in the shadow raster state.
- Up vector for lookAt: If the spotlight direction is near collinear with your world up (z‑up), switch to y‑up to avoid singularities.
- Coordinate spaces: With GLM default (OpenGL clip), remap x/y/z from clip space to [0,1] for shadow sampling (`* 0.5 + 0.5`). If you adopt `GLM_FORCE_DEPTH_ZERO_TO_ONE` later, drop the z remap.
- Fragment stage for shadow pass: Do not write color outputs; for alpha‑tested geometry, use discard only.

Verification Checklist

- Shadow pass draws: see depth activity in a debug overlay (optional) and confirm no validation errors.
- Barrier present: no sampling‑before‑write hazards; image layout is `DEPTH_STENCIL_READ_ONLY_OPTIMAL` when sampling.
- Main pass renders with `outColor` forced to visible color during debugging.
- Dynamic state: viewport/scissor/depth‑bias set before shadow draws each layer.
- Descriptor bindings present before `push_descriptor_set()` for meshes (bindings 0..7 existing, plus 8 and 9 for shadows).

Roadmap

1) Finish/verify shadow shaders compilation and names used by `ShadowPipeline`.
2) Add graphics descriptor bindings 8 (sampler) and 9 (matrices) in `src/application.cpp` and wire writes in `Scene::render()`.
3) Add per‑layer VP UBO (aligned) and `BufferHost::copy_from_at`; record the shadow pass in `VulkanEngine::render()` before the main pass.
4) Insert the image barrier between shadow and main passes.
5) Update `shaders/shader.default.frag` to sample `sampler2DArrayShadow` and multiply each spotlight’s contribution by the visibility.
6) Tune bias; validate with tight cones; verify front‑face culling reduces self‑shadowing.
7) Enhancements (later): 3×3 PCF or Poisson disk sampling, per‑light resolution, contact hardening, spot masks, debug visualizations.

Notes on the Earlier “Light Bleeding” Observation

- The “spotlight shining on adjacent edges” under tight cones was caused by insufficient self‑occlusion and back‑face effects. Rendering back faces (cull FRONT) and applying appropriate depth bias typically removes this. The remaining glow is usually bias/PCF related and is tuned away with slope‑scaled bias and proper comparison (LESS/LEQUAL pairing).
