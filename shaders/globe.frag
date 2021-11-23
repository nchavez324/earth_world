#version 430

#pragma include "common.glsl"

uniform sampler2D p3d_Texture0;  // topology
uniform sampler2D p3d_Texture1;  // bathymetry
uniform sampler2D p3d_Texture2;  // land_mask
uniform sampler2D p3d_Texture3;  // albedo
uniform sampler2D p3d_Texture4;  // normal
uniform sampler2D p3d_Texture5;  // visibility
uniform sampler2D p3d_Texture6;  // incognita

uniform mat3 p3d_NormalMatrix;
uniform struct { vec4 ambient; } p3d_LightModel;
uniform struct p3d_LightSourceParameters {
  vec4 color;
  vec4 position;
} p3d_LightSource[1];

// Input from vertex shader
in vec4 v_ViewPosition;
in vec4 v_Position;

out vec4 p3d_FragColor;

void main() {
  // UVs need to be calculated here, and not passed in via vertex data since
  // otherwise when u goes back from 1 to 0, the texture is quickly lerped,
  // instead of discontinued.
  vec2 uv = sphericalUVFromCartesian(v_Position.xyz);
  float topology = texture(p3d_Texture0, uv).r;
  float bathymetry = texture(p3d_Texture1, uv).r;
  float landMask = texture(p3d_Texture2, uv).r;
  vec3 albedo = texture(p3d_Texture3, uv).rgb;
  vec3 normal = texture(p3d_Texture4, uv).rgb;
  float totalVisibility = texture(p3d_Texture5, uv).r;
  float immediateVisibility = texture(p3d_Texture5, uv).g;
  vec3 incognitaColor = texture(p3d_Texture6, 4 * uv).rgb;

  float waterDepth = 1 - bathymetry;
  vec3 waterColor = mix(vec3(0, 0.5, 1), vec3(0, 0, 0.5), waterDepth);
  vec3 earthUnlitColor = mix(albedo, waterColor, landMask);
  vec3 flatNormal = cartesianFromSphericalUV(uv);
  // Use a flat, smooth surface normal for water
  normal = mix(normal, flatNormal, landMask);

  vec3 fragmentViewPosition = v_ViewPosition.xyz;
  vec3 fragmentViewNormal = normalize(p3d_NormalMatrix * normal);
  vec3 diffuseEarthColor = vec3(0);
  for (int i = 0; i < p3d_LightSource.length(); ++i) {
    vec3 lightViewDirection = calculateLightViewDirection(
        v_ViewPosition.xyz, p3d_LightSource[i].position);
    // Diffuse
    float diffuseIntensity = dot(fragmentViewNormal, lightViewDirection);
    if (diffuseIntensity < 0.0) {
      continue;
    }
    diffuseEarthColor +=
        clamp(earthUnlitColor * p3d_LightSource[i].color.rgb * diffuseIntensity,
              0, 1);
  }
  diffuseEarthColor = clamp(diffuseEarthColor, vec3(0), earthUnlitColor);

  vec3 ambientEarthColor = p3d_LightModel.ambient.rgb * earthUnlitColor;

  vec3 earthColor = diffuseEarthColor + ambientEarthColor;
  vec3 obscuredEarthColor = mix(earthColor, incognitaColor, 0.5);
  vec3 incongitaEarthColor =
      mix(incognitaColor, obscuredEarthColor, totalVisibility);
  vec3 finalColor = mix(incongitaEarthColor, earthColor, immediateVisibility);
  p3d_FragColor = vec4(finalColor, 1);
}