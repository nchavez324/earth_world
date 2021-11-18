#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0;  // land_mask
uniform sampler2D p3d_Texture1;  // visibility
uniform sampler2D p3d_Texture2;  // incognita

// Input from vertex shader
in vec2 v_TexCoord0;

out vec4 p3d_FragColor;

void main() {
  float landMask = texture(p3d_Texture0, v_TexCoord0).r;
  float totalVisibility = texture(p3d_Texture1, v_TexCoord0).r;
  float immediateVisibility = texture(p3d_Texture1, v_TexCoord0).g;
  vec4 incognitaColor = texture(
      p3d_Texture2, vec2(0.1 * v_TexCoord0.x, 0.1 * 0.5 * v_TexCoord0.y));
  vec4 waterColor = vec4(0, 0.3, 0.8, 1);
  vec4 landColor = vec4(0.2, 0.25, 0.1, 1);
  vec4 earthColor = mix(landColor, waterColor, landMask);
  vec4 obscuredEarthColor = mix(earthColor, incognitaColor, 0.5);
  vec4 incongitaEarthColor =
      mix(incognitaColor, obscuredEarthColor, totalVisibility);
  p3d_FragColor = mix(incongitaEarthColor, earthColor, immediateVisibility);
}