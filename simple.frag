#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0; // topology
uniform sampler2D p3d_Texture1; // bathymetry
uniform sampler2D p3d_Texture2; // albedo

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
  vec3 albedo = texture(p3d_Texture2, uv).rgb;

  float waterDepth = depthFromBathymetryTexValue(bathymetry);
  vec4 waterColor = mix(vec4(0, 0.5, 1, 1), vec4(0, 0, 0.5, 1), waterDepth);
  vec4 landColor = vec4(albedo, 1);
  // There's a black halo around the land because the bathymetry texture isn't
  // perfect.
  p3d_FragColor = bathymetry > BATHYMETRY_CUTOFF ? waterColor : landColor;
  }
}