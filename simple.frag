#version 430

#define PI_OVER_TWO 1.570796327
#define PI 3.1415926538
#define TWO_PI 6.283185308

uniform sampler2D p3d_Texture0;

// Input from vertex shader
in vec4 v_Position;

out vec4 p3d_FragColor;

vec3 sphericalCoordsFromCartesian(vec3 coords) {
  // Panda3D is right-hand z-up, and world coords are still in Panda coords
  vec3 spherical = vec3(0, 0, 0);
  if (coords.x != 0 || coords.y != 0) {
    // Negate inputs so after addition, 0 is +X and clockwise
    spherical.x = atan(-coords.y, -coords.x) + PI;
  }
  float xyLength = length(coords.xy);
  if (xyLength != 0 || coords.z != 0) {
    spherical.y = atan(coords.z, xyLength);
  }
  spherical.z = length(coords);
  return spherical;
}

void main() {
  vec3 sphericalCoords = sphericalCoordsFromCartesian(v_Position.xyz);
  vec2 uv = vec2(
    sphericalCoords.x / TWO_PI,
    (sphericalCoords.y + PI_OVER_TWO) / PI);
  p3d_FragColor = texture(p3d_Texture0, uv);
}