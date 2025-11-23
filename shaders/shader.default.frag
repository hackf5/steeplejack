#version 460

#extension GL_GOOGLE_include_directive : require

#include "types.glsl"
#include "lighting.glsl"

const int kMaxSpots = 8;

layout(binding = 2) uniform sampler2D inSampler;  // baseColor
layout(binding = 4) uniform sampler2D inNormal;   // normal
layout(binding = 5) uniform sampler2D inMR;       // metallic-roughness
layout(binding = 6) uniform sampler2D inEmissive; // emissive

layout(binding = 3, std140) uniform MaterialParams {
    vec4 baseColorFactor;
    vec4 materialFactors; // x: metallic factor, y: roughness factor, z: normal scale
};

layout(binding = 7, std140) uniform SceneLights {
    vec3 ambientColor;  float ambientIntensity;
    Spot spots[kMaxSpots];
};

layout (binding = 9, std140) uniform ShadowParams {
    mat4 lightViewProj[kMaxSpots];
    vec4 debugParams; // x: shadowsEnabled, y: debugMode, z: debugSpotIndex
};

layout(binding = 8) uniform sampler2DArrayShadow uShadowMap;


layout(location = 0) in vec2 inUV;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inWorldNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 baseSample = texture(inSampler, inUV) * baseColorFactor;
    vec3 baseColor = baseSample.rgb;
    float alpha = baseSample.a;
    vec3 emissiveCol = texture(inEmissive, inUV).rgb;

    vec3 mrSample = texture(inMR, inUV).rgb;
    float metallic = clamp(mrSample.b * materialFactors.x, 0.0, 1.0);
    float roughness = clamp(mrSample.g * materialFactors.y, 0.04, 1.0);
    float normalScale = materialFactors.z;

    // Build TBN from partial derivatives to support normal mapping without per-vertex tangents
    vec3 N = normalize(inWorldNormal);
    vec3 dp1 = dFdx(inWorldPos);
    vec3 dp2 = dFdy(inWorldPos);
    vec2 duv1 = dFdx(inUV);
    vec2 duv2 = dFdy(inUV);
    vec3 tangent = dp1 * duv2.y - dp2 * duv1.y;
    if (dot(tangent, tangent) < 1e-8) {
        vec3 axis = (abs(N.y) > 0.9) ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 0.0);
        tangent = normalize(cross(N, axis));
    }
    vec3 T = normalize(tangent);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    vec3 nTex = texture(inNormal, inUV).xyz * 2.0 - 1.0;
    nTex.xy *= normalScale;
    N = normalize(TBN * nTex);

    vec3 V = normalize(-inWorldPos); // approximate view direction (camera at origin)

    vec3 ambient = baseColor * ambientColor * ambientIntensity * (1.0 - metallic);

    // Debug modes: 0=none, 1=cone (selected), 2=NdotL (selected), 3=shadow vis (selected), 4=normals, 5=normals pass/fail
    int dbgMode = int(floor(debugParams.y + 0.5));
    int dbgIdx  = clamp(int(floor(debugParams.z + 0.5)), 0, kMaxSpots - 1);
    if (dbgMode == 4) {
        outColor = vec4(0.5 * (N + 1.0), 1.0);
        return;
    }
    if (dbgMode == 5) {
        // Pass/fail: expected outward normal based on dominant axis of world position
        vec3 Pw = inWorldPos;
        vec3 a = abs(Pw);
        vec3 expected = vec3(0.0);
        if (a.x > a.y && a.x > a.z)      expected = vec3(sign(Pw.x), 0.0, 0.0);
        else if (a.y > a.z)              expected = vec3(0.0, sign(Pw.y), 0.0);
        else                             expected = vec3(0.0, 0.0, sign(Pw.z));
        float d = dot(normalize(N), expected);
        // Use a tight threshold to avoid grazing false positives
        vec3 col = (d > 0.95) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        outColor = vec4(col, 1.0);
        return;
    }
    if (dbgMode != 0) {
        if (spots[dbgIdx].enable) {
            Spot L = spots[dbgIdx];
            vec3 Ls = normalize(L.position - inWorldPos);
            vec3 Lf = -Ls;
            vec3 Df = normalize(L.direction);
            float ndotl = max(dot(N, Ls), 0.0);
            float cosAng = dot(Df, Lf);
            float cone = smoothstep(min(L.outerCos, L.innerCos), max(L.outerCos, L.innerCos), cosAng);

            float vis = 1.0;
            if (debugParams.x > 0.5) {
                vec4 l = lightViewProj[dbgIdx] * vec4(inWorldPos, 1.0);
                vec3 ndc = l.xyz / l.w;
                vec2 uv  = ndc.xy * 0.5 + 0.5;
                float z  = clamp(ndc.z, 0.0, 1.0);
                if (all(greaterThanEqual(uv, vec2(0.0))) && all(lessThanEqual(uv, vec2(1.0))) && z >= 0.0 && z <= 1.0) {
                    vis = texture(uShadowMap, vec4(uv, float(dbgIdx), z));
                }
            }

            if (dbgMode == 1) { outColor = vec4(vec3(cone), 1.0); return; }
            if (dbgMode == 2) { outColor = vec4(vec3(ndotl), 1.0); return; }
            if (dbgMode == 3) { outColor = vec4(vec3(vis), 1.0); return; }
        }
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 diffuseSum = vec3(0.0);
    vec3 specSum = vec3(0.0);
    for (int i = 0; i < kMaxSpots; ++i) {
        if (!spots[i].enable) {
            continue;
        }

        // Project into light i clip space
        vec4 l = lightViewProj[i] * vec4(inWorldPos, 1.0);
        vec3 ndc = l.xyz / l.w;                  // [-1,1]
        vec2 uv  = ndc.xy * 0.5 + 0.5;           // [0,1]
        float z  = clamp(ndc.z, 0.0, 1.0);            // [0,1]

        float vis = 1.0;
        if (debugParams.x > 0.5) {
            if (all(greaterThanEqual(uv, vec2(0.0))) && all(lessThanEqual(uv, vec2(1.0))) && z >= 0.0 && z <= 1.0) {
                vis = texture(uShadowMap, vec4(uv, float(i), z));
            }
        }

        vec3 Ls = normalize(spots[i].position - inWorldPos);
        vec3 Lf = -Ls;
        vec3 Df = normalize(spots[i].direction);
        float ndotl = max(dot(N, Ls), 0.0);
        float cosAng = dot(Df, Lf);
        float cone = smoothstep(min(spots[i].outerCos, spots[i].innerCos), max(spots[i].outerCos, spots[i].innerCos), cosAng);
        float dist = length(inWorldPos - spots[i].position);
        float range = max(spots[i].range, 1e-3);
        float r = dist / range;
        float att = 1.0 / (1.0 + r * r);
        vec3 radiance = spots[i].color * (spots[i].intensity * cone * att);

        if (ndotl > 0.0) {
            vec3 F0 = mix(vec3(0.04), baseColor, metallic);
            vec3 H = normalize(Ls + V);
            float ndoth = max(dot(N, H), 0.0);
            float specPower = pow(ndoth, mix(128.0, 1.0, roughness));
            vec3 specColor = mix(vec3(0.04), baseColor, metallic);
            specSum += vis * specColor * specPower * ndotl * radiance;
            diffuseSum += vis * (1.0 - metallic) * baseColor * ndotl * radiance;
        }
    }

    outColor = vec4(ambient + diffuseSum + specSum + emissiveCol, alpha);
}
