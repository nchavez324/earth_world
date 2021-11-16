#ifndef EARTH_WORLD_SPHERE_POINT_H
#define EARTH_WORLD_SPHERE_POINT_H

#include "panda3d/aa_luse.h"

namespace earth_world {

class SpherePoint2;

/**
 * Representations of spherical coordinates.
 * In Panda3D, the cartesian coordinate system is right handed, with Z as up.
 * These following spherical coordinates use the following conventions:
 *  - Radians for angles
 *  - +X axis is azimuth = 0, increasing clockwise
 *  - XY plane is polar = 0, increasing towards +Z, decreasing towards -Z
 *  - Azimuth is in the interval [0, 2PI]
 *  - Zenith is in the interval [-PI/2, PI/2]
 */

/** Represents a point in spherical coordinates. */
class SpherePoint3 : public LVecBase3 {
 public:
  SpherePoint3() = default;
  SpherePoint3(PN_stdfloat azimuthal, PN_stdfloat polar, PN_stdfloat radial)
      : LVecBase3(azimuthal, polar, radial) {}

  PN_stdfloat get_azimuthal() const { return get_x(); }
  PN_stdfloat get_polar() const { return get_y(); }
  PN_stdfloat get_radial() const { return get_z(); }

  /** @return The corresponding unit sphere point. */
  SpherePoint2 toUnit() const;

  /** @return The corresponding cartesian coordinates. */
  LVecBase3 toCartesian() const;

  /** @return The corresponding UV coordinates. */
  LVecBase2 toUV() const;

  /**
   * Converts cartesian coordinates into spherical coordinates.
   * @param cartesian The cartesian coordinates.
   * @return The corresponding spherical coordinates.
   */
  static SpherePoint3 fromCartesian(const LVecBase3 &cartesian);
};

/**
 * Represents a point in spherical coordinates on the unit sphere, with an
 * implicit radius of 1.
 */
class SpherePoint2 : public LVecBase2 {
 public:
  SpherePoint2() = default;
  SpherePoint2(PN_stdfloat azimuthal, PN_stdfloat polar)
      : LVecBase2(azimuthal, polar) {}

  PN_stdfloat get_azimuthal() const { return get_x(); }
  PN_stdfloat get_polar() const { return get_y(); }

  /** @return The corresponding non-unit sphere point, with radius 1. */
  SpherePoint3 toRadial() const;

  /** @return The corresponding cartesian coordinates. */
  LVecBase3 toCartesian() const;

  /** @return The corresponding UV coordinates. */
  LVecBase2 toUV() const;

  /**
   * Converts cartesian coordinates into spherical coordinates.
   * @param cartesian The cartesian coordinates.
   * @return The corresponding spherical coordinates.
   */
  static SpherePoint2 fromCartesian(const LVecBase3 &cartesian);
};

}  // namespace earth_world

#endif  // EARTH_WORLD_SPHERE_POINT_H
