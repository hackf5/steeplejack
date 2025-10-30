# 🧱 Idiot’s Guide to UBOs (`std140`) in GLSL

---

## 🧠 What’s a UBO?

A **Uniform Buffer Object** (UBO) is a chunk of GPU memory that holds constants shared by many shaders — like lighting data, camera matrices, or material parameters.
You define it in GLSL, upload the same data from C++, and both sides must **agree exactly** on layout and alignment.

---

## ⚙️ Why `layout(std140)` Matters

GLSL doesn’t guess how to pack your structs.
`layout(std140)` forces a **predictable, fixed alignment scheme** that you can mirror on the CPU.

If you don’t set it, the layout becomes **implementation-defined** (`shared`), meaning:
💥 *Your GPU might silently shuffle fields around.*

Always write:

```glsl
layout(std140, binding = 3) uniform MyBlock {
    ...
};
```

---

## 🧹 The Golden Rules of `std140`

| Type                           | Base Alignment                                          | Size (bytes) | Notes                                          |
| ------------------------------ | ------------------------------------------------------- | ------------ | ---------------------------------------------- |
| `float`, `int`, `uint`, `bool` | 4                                                       | 4            | Scalars align to 4                             |
| `vec2`                         | 8                                                       | 8            | 2× scalars                                     |
| `vec3`, `vec4`                 | 16                                                      | 12 or 16     | **Always 16-aligned** (vec3 behaves like vec4) |
| `mat4`                         | 16 per column                                           | 64           | Treated as array of 4 vec4s                    |
| Arrays                         | 16 per element                                          | 16 × n       | Each element padded to 16                      |
| Structs                        | Max(member alignment), then rounded to multiple of that | —            | Each field obeys these same rules              |
| Array of Structs               | ...                                                     | sizeof(struct) × n |                                          |

<https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf#page=168>

---

## 🧮 What That Means in Practice

* Every **`vec3` takes up 16 bytes** (not 12).
* Pair each `vec3` with a `float` to “fill” its 16-byte slot:

  ```glsl
  vec3 position;  float intensity;   // one slot
  vec3 direction; float innerCos;    // next slot
  ```

* Structs and arrays pad up to multiples of 16 bytes.

---

## 💡 Example: Spotlight Struct

### GLSL

```glsl
layout(std140, binding = 7) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;

    struct Spot {
        vec3 position;  float intensity;
        vec3 direction; float innerCos;
        vec3 color;     float outerCos;
        vec3 _pad;      float range; // 64 bytes total
    } spots[2];
};
```

Each `Spot` = 64 bytes, so `spots[2]` = 128 bytes.

### C++ Mirror

```cpp
struct Spot {
    glm::vec3 position;  float intensity;
    glm::vec3 direction; float innerCos;
    glm::vec3 color;     float outerCos;
    glm::vec3 _pad;      float range;
};

struct SceneLights {
    glm::vec3 ambientColor;
    float ambientIntensity;
    Spot spots[2];
};
```

✅ Works **without** `alignas`
✅ No GLM macros needed (because you paired vec3+float)
✅ CPU `sizeof(Spot)` == GPU `64`

---

## 🚫 Common Gotchas

| Problem                                      | Cause                           | Fix                                                                         |
| -------------------------------------------- | ------------------------------- | --------------------------------------------------------------------------- |
| Fields misaligned                            | No `layout(std140)`             | Add it                                                                      |
| Everything works on one GPU but not another  | Using default (`shared`) layout | Explicitly set `std140`                                                     |
| Values wrong when uploading array of structs | Struct stride mismatch          | Use `vec3+float` pattern                                                    |
| `sizeof(vec3)` = 12 on CPU                   | GLM default types               | Still fine if paired correctly |

---

## 🧮 Quick Checklist Before Upload

1. ✅ Always use `layout(std140)` in GLSL.
2. ✅ Pair every `vec3` with a `float`.
3. ✅ Make sure structs end up multiple of 16 bytes.
4. ✅ Mirror the same field order on CPU.
5. ✅ Use `glBindBufferBase(GL_UNIFORM_BUFFER, binding, bufferId)` with the same binding number.
6. ✅ Profit 💸 — no mysterious lighting bugs.

---

## 🗳 TL;DR

> `std140` = *predictable, padded, safe*.
> Treat each field like a 16-byte Tetris block.
> Pair `vec3 + float`.
> Never forget `layout(std140)`.

---

Now you can build any UBO you want — safely and pr
