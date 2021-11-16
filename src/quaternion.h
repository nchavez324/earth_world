#ifndef EARTH_WORLD_QUATERNION_H
#define EARTH_WORLD_QUATERNION_H

#include "panda3d/aa_luse.h"

namespace earth_world {
namespace quaternion {
/** Utilities for common quaternion creation and manipulation. */

/**
 * Creates a quaternion that corresponds to the rotation embedded in the given
 * orthonormal transformation matrix.
 * @param a The orthonormal transformation matrix.
 * @return A quaternion to rotate the same way the matrix does.
 */
LQuaternion fromOrthonormalMatrix(const LMatrix3 &a);

/**
 * Returns a rotation from one vector to another.
 * @param v1 The vector to rotate from.
 * @param v2 The vector to rotate to.
 * @return A quaternion to rotate between the two.
 */
LQuaternion fromBetweenVectors(const LVector3 &v1, const LVector3 &v2);

/**
 * Returns a rotation such that forward (+Y) rotates to the given forward, and
 * right (+X) rotates to the cross product of the given forward and up.
 * @param forward The new forward to aim at.
 * @param up The new up, which is "roughly" kept.
 * @return A quaternion to rotate to this new basis.
 */
LQuaternion fromLookAt(const LVector3 &forward, const LVector3 &up);

}  // namespace quaternion
}  // namespace earth_world

#endif  // EARTH_WORLD_QUATERNION_H
