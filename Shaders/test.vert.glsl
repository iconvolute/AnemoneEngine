#version 450
#if false
// uber_shader.glsl

// --------------------------------------------
// Common code shared between all shader stages
// --------------------------------------------
vec4 applyLighting(vec3 normal, vec3 lightDir, vec4 baseColor) {
    float ndotl = max(dot(normalize(normal), normalize(lightDir)), 0.0);
    return baseColor * ndotl;
}

// --------------------------------------------
// Vertex shader entry point
// --------------------------------------------
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 vNormal;

void vert_main() {
    gl_Position = vec4(inPosition, 1.0);
    vNormal = inNormal;
}

// --------------------------------------------
// Fragment shader entry point
// --------------------------------------------
layout(location = 0) in vec3 vNormal;
layout(location = 0) out vec4 outColor;

void frag_main() {
    outColor = applyLighting(vNormal, vec3(0.0, 0.0, 1.0), vec4(1.0, 0.6, 0.2, 1.0));
}
#endif

// from-source-code resolver for includes needed first
#include "hello.glslh"

layout(set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) uniform BUBU {
    float scale;
} ububu;

layout(set= 2, binding = 5) uniform UBO2 {
    float time;
} ubo2;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor * ubo2.time;
    fragTexCoord = inTexCoord;
}
