#pragma once

#define PI_OVER_TWO 1.570796327
#define PI 3.1415926538
#define TWO_PI 6.283185308

// The bathymetry texture has land around 0, then the Marianas Trench at around
// ~0.83, increasing in height until sea level at 1.
uniform float u_BathymetryCutoff;

// The fade for sight visibility around the boat.
#define VISIBILITY_FADE_START 0.07
#define VISIBILITY_FADE_END 0.1

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

vec3 cartesianCoordsFromSpherical(vec3 coords) {
  // x is azimuth [0,2PI], y is polar [-PI/2,PI/2], z is radius.
  return vec3(
    coords.z * cos(coords.y) * cos(coords.x),
    coords.z * cos(coords.y) * sin(coords.x),
    coords.z * sin(coords.y));
}

/** This assumes both are normalized, radius 1. */
float unitSphereDistance(vec2 s1, vec2 s2) {
  // Find the angle between the two, when in cartesian.
  vec3 v1 = cartesianCoordsFromSpherical(vec3(s1, 1));
  vec3 v2 = cartesianCoordsFromSpherical(vec3(s2, 1));
  float dotProduct = dot(v1, v2);
  // acos starts going haywire when the vectors are really close together.
  // TODO: estimate a better metric than this.
  if (dotProduct >= 0.99999) return 0;
  return acos(dotProduct);
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
  if (value > u_BathymetryCutoff) {
    return 1 - inverseMix(u_BathymetryCutoff, 1,
                          clamp(value, u_BathymetryCutoff, 1));
  } else {
    return 0;
  }
}