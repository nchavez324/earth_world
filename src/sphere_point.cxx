#include "sphere_point.h"

namespace earth_world {

SpherePoint3::SpherePoint3(SpherePoint2 &unit_point, PN_stdfloat radial)
    : LVecBase3(unit_point.get_azimuthal(), unit_point.get_polar(), radial) {}

SpherePoint2 SpherePoint3::toUnit() const {
  return SpherePoint2(get_azimuthal(), get_polar());
}

LVecBase3 SpherePoint3::toCartesian() const {
  return LVecBase3(get_radial() * cosf(get_polar()) * cosf(get_azimuthal()),
                   get_radial() * cosf(get_polar()) * sinf(get_azimuthal()),
                   get_radial() * sinf(get_polar()));
}

LVecBase2 SpherePoint3::toUV() const { return toUnit().toUV(); }

SpherePoint3 SpherePoint3::fromCartesian(const LVecBase3 &cartesian) {
  PN_stdfloat azimuth = 0;
  if (cartesian.get_x() != 0 || cartesian.get_y() != 0) {
    // Negate inputs so after addition, 0 is +X and clockwise
    // Returns [-PI,PI], so add PI to move to [0,2PI]
    azimuth = atan2f(-cartesian.get_y(), -cartesian.get_x()) + MathNumbers::pi;
  }
  PN_stdfloat xyLength = cartesian.get_xy().length();
  PN_stdfloat polar = 0;
  if (xyLength != 0 || cartesian.get_z() != 0) {
    polar = atan2f(cartesian.get_z(), xyLength);
  }
  return SpherePoint3(azimuth, polar, cartesian.length());
}

SpherePoint3 SpherePoint2::toRadial() const {
  return SpherePoint3(get_azimuthal(), get_polar(), 1);
}

LVecBase3 SpherePoint2::toCartesian() const { return toRadial().toCartesian(); }

LVecBase2 SpherePoint2::toUV() const {
  return LVecBase2(get_azimuthal() / (2 * MathNumbers::pi),
                   ((-get_polar() / MathNumbers::pi) + 0.5));
}

SpherePoint2 SpherePoint2::fromCartesian(const LVecBase3 &cartesian) {
  return SpherePoint3::fromCartesian(cartesian).toUnit();
}

SpherePoint2 SpherePoint2::fromLatitudeAndLongitude(PN_stdfloat latitude,
                                                    PN_stdfloat longitude) {
  return SpherePoint2(((longitude + 180.f) / 180.f) * MathNumbers::pi,
                      (latitude / 180.f) * MathNumbers::pi);
}

}  // namespace earth_world
