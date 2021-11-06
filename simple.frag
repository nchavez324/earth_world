#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0;

// Input from vertex shader
in vec4 v_Position;

out vec4 p3d_FragColor;


void main() {
  vec2 uv = sphericalUVFromCartesian(v_Position.xyz);
  vec3 texColor = texture(p3d_Texture0, uv).rgb;
  texColor = vec3(
    mix(0.1, 1, texColor.r),
    mix(0.1, 1, texColor.g),
    mix(0.1, 1, texColor.b));
  p3d_FragColor = vec4(texColor.rgb, 1);
}