#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0;
uniform sampler2D p3d_Texture1;

// Input from vertex shader
in vec4 v_Position;

out vec4 p3d_FragColor;


void main() {
  // UVs need to be calculated here, and not passed in via vertex data since
  // otherwise when u goes back from 1 to 0, the texture is quickly lerped,
  // instead of discontinued.
  vec2 uv = sphericalUVFromCartesian(v_Position.xyz);
  float topology = texture(p3d_Texture0, uv).r;
  float bathymetry = texture(p3d_Texture1, uv).r;

  if (bathymetry > BATHYMETRY_CUTOFF) {
    float waterDepth = depthFromBathymetryTexValue(bathymetry);
    p3d_FragColor = mix(vec4(0, 0.5, 1, 1), vec4(0, 0, 0.5, 1), waterDepth);
  } else {
    p3d_FragColor = vec4(topology, topology, topology, 1);
  }
}