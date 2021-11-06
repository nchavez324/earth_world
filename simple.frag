#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0;

// Input from vertex shader
in vec4 v_Position;
in float v_Height;

out vec4 p3d_FragColor;


void main() {
  // UVs need to be calculated here, and not passed in via vertex data since
  // otherwise when u goes back from 1 to 0, the texture is quickly lerped,
  // instead of discontinued.
  vec2 uv = sphericalUVFromCartesian(v_Position.xyz);
  if (v_Height < 0.00001) {
    p3d_FragColor = vec4(0, 0.5, 1, 1);
  } else {
    vec3 texColor = texture(p3d_Texture0, uv).rgb;
    p3d_FragColor = vec4(texColor.rgb, 1);
  }
}