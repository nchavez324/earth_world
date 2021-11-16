#include "quaternion.h"

namespace earth_world {
namespace quaternion {

LQuaternion fromOrthonormalMatrix(const LMatrix3 &a) {
  PN_stdfloat trace = a[0][0] + a[1][1] + a[2][2];
  if (trace > 0) {
    PN_stdfloat s = 0.5f / sqrtf(trace + 1.f);
    return LQuaternion(0.25f / s, (a[2][1] - a[1][2]) * s,
                       (a[0][2] - a[2][0]) * s, (a[1][0] - a[0][1]) * s);
  } else {
    if (a[0][0] > a[1][1] && a[0][0] > a[2][2]) {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[0][0] - a[1][1] - a[2][2]);
      return LQuaternion((a[2][1] - a[1][2]) / s, 0.25f * s,
                         (a[0][1] + a[1][0]) / s, (a[0][2] + a[2][0]) / s);
    } else if (a[1][1] > a[2][2]) {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[1][1] - a[0][0] - a[2][2]);
      return LQuaternion((a[0][2] - a[2][0]) / s, (a[0][1] + a[1][0]) / s,
                         0.25f * s, (a[1][2] + a[2][1]) / s);
    } else {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[2][2] - a[0][0] - a[1][1]);
      return LQuaternion((a[1][0] - a[0][1]) / s, (a[0][2] + a[2][0]) / s,
                         (a[1][2] + a[2][1]) / s, 0.25f * s);
    }
  }
}

LQuaternion fromBetweenVectors(const LVector3 &v1, const LVector3 &v2) {
  // From Sam Hocevar's article "Quaternion from two vectors: the final version"
  PN_stdfloat a = sqrtf(v1.length_squared() * v2.length_squared());
  PN_stdfloat b = a + v1.dot(v2);
  if (IS_NEARLY_EQUAL(b, 2 * a) || IS_NEARLY_ZERO(a)) {
    return LQuaternion::ident_quat();
  }
  LVector3 axis(0);
  if (b < 1e-06 * a) {
    b = 0;
    if (abs(v1.get_x()) > abs(v1.get_z())) {
      axis = LVector3(-v1.get_y(), v1.get_x(), 0);
    } else {
      axis = LVector3(0, -v1.get_z(), v1.get_y());
    }
  } else {
    axis = v1.cross(v2);
  }
  LQuaternion rotation(b, axis.get_x(), axis.get_y(), axis.get_z());
  rotation.normalize();
  return rotation;
}

LQuaternion fromLookAt(const LVector3 &newForward, const LVector3 &upIsh) {
  // Create orthonormal transformation.
  LVector3 newRight = newForward.cross(upIsh);
  newRight.normalize();
  LVector3 newUp = newRight.cross(newForward);
  newUp.normalize();
  LMatrix3 transformation;
  transformation.set_col(0, newRight);
  transformation.set_col(1, newForward.normalized());
  transformation.set_col(2, newUp);
  // Convert into a Quaternion.
  return fromOrthonormalMatrix(transformation);
}
}  // namespace quaternion
}  // namespace earth_world
