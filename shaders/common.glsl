#pragma once

#define FLOAT_MAX 3.402823466e+38

#define PI_OVER_TWO 1.570796327
#define PI 3.1415926538
#define TWO_PI 6.283185308

uniform float u_LandMaskCutoff;

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

vec3 cartesianFromSphericalUV(vec2 uv) {
  vec3 sphericalCoords = vec3(uv.x * TWO_PI, (uv.y * PI) - PI_OVER_TWO, 1);
  return cartesianCoordsFromSpherical(sphericalCoords);
}

/** This assumes both are normalized, radius 1. */
float unitSphereDistance(vec2 s1, vec2 s2) {
  // Find the angle between the two, when in cartesian.
  vec3 v1 = cartesianCoordsFromSpherical(vec3(s1, 1));
  vec3 v2 = cartesianCoordsFromSpherical(vec3(s2, 1));
  float dotProduct = dot(v1, v2);
  // acos starts going haywire when the vectors are really close together.
  // TODO: estimate a better metric than this.
  if (dotProduct <= 0.00001) return FLOAT_MAX;
  if (dotProduct >= 0.99999) return 0;
  return acos(dotProduct);
}

float inverseMix(float from, float to, float value) {
  return clamp((value - from) / (to - from), 0, 1);
}

vec3 calculateLightViewDirection(vec3 fragmentViewPosition,
                                 vec4 lightViewPosition) {
  return normalize(lightViewPosition.xyz -
                   (fragmentViewPosition * lightViewPosition.w));
}
