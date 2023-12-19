#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));
    float intensity = max(0.0, dot(fragNormal, lightDirection));

    // Apply basic toon shading
    vec3 toonColor = vec3(0.0, 0.8, 0.0); // Set your desired toon color
    outColor = vec4(toonColor * intensity, 1.0);

    // Add black outline
    float outline = fwidth(intensity);
    outline = smoothstep(0.5 - outline, 0.5 + outline, intensity);
    outColor.rgb *= outline;
}
