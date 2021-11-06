#pragma once

#define PI_OVER_TWO 1.570796327
#define PI 3.1415926538
#define TWO_PI 6.283185308

// The bathymetry texture has land around 0, then the Marianas Trench at around
// ~0.83, increasing in height until sea level at 1.
#define BATHYMETRY_CUTOFF 0.8

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

vec2 sphericalUVFromCartesian(vec3 coords) {
  vec3 sphericalCoords = sphericalCoordsFromCartesian(coords);
  return vec2(
    sphericalCoords.x / TWO_PI,
    (sphericalCoords.y + PI_OVER_TWO) / PI);
}

float inverseMix(float from, float to, float value) {
  return (value - from) / (to - from);
}

/**
 * Transforms a sample from the bathymetry texture to a value in range
 * [0:sea_level,1:marianas_trench].
 */
float depthFromBathymetryTexValue(float value) {
  // The bathymetry texture has land around 0, then the Marianas Trench at
  // around ~0.83, increasing in height until sea level at 1.
  return 1 - inverseMix(0.8, 1, clamp(value, 0.8, 1));
}