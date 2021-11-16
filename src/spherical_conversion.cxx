#include "spherical_conversion.h"

namespace earth_world {

LPoint3 sphericalCoordsFromCartesian(const LPoint3 &coords) {
  // In Panda3D, coordinate system is right handed, with Z as up.
  // Convention:
  //   - +X axis is azimuth = 0, increasing clockwise
  //   - XY plane is polar = 0, increasing towards +Z, decreasing towards -Z
  PN_stdfloat azimuth = 0;
  if (coords.get_x() != 0 || coords.get_y() != 0) {
    // Negate inputs so after addition, 0 is +X and clockwise
    // Returns [-PI,PI], so add PI to move to [0,2PI]
    azimuth = atan2f(-coords.get_y(), -coords.get_x()) + MathNumbers::pi;
  }
  PN_stdfloat xyLength = coords.get_xy().length();
  PN_stdfloat polar = 0;
  if (xyLength != 0 || coords.get_z() != 0) {
    polar = atan2f(coords.get_z(), xyLength);
  }
  return LPoint3(azimuth, polar, coords.length());
}

LPoint3 cartesianCoordsFromSpherical(const LPoint3 &coords) {
  // x is azimuth [0,2PI], y is polar [-PI/2,PI/2], z is radius.
  return LPoint3(coords.get_z() * cosf(coords.get_y()) * cosf(coords.get_x()),
                 coords.get_z() * cosf(coords.get_y()) * sinf(coords.get_x()),
                 coords.get_z() * sinf(coords.get_y()));
}

}  // namespace earth_world
