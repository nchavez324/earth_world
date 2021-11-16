#ifndef SPHERICAL_CONVERSION_H
#define SPHERICAL_CONVERSION_H

#include "panda3d/aa_luse.h"

namespace earth_world {
/**
 * Utilities for conversions between spherical and cartesian coordinates.
 *
 * The convention used is that spherical coordinates use radians, where the
 * first/x component is the azimuthal angle [0, 2PI] and the second/y component
 * is the polar angle [-PI/2,PI/2], and the third/z component is the radius.
 */

/**
 * Converts cartesian coordinates into spherical coordinates.
 * @param coords The cartesian coordinates.
 * @return The corresponding spherical coordinates.
 */
LPoint3 sphericalCoordsFromCartesian(const LPoint3 &coords);

/**
 * Converts spherical coordinates into cartesian coordinates.
 * @param coords The spherical coordinates.
 * @return The corresponding cartesian coordinates.
 */
LPoint3 cartesianCoordsFromSpherical(const LPoint3 &coords);

}  // namespace earth_world

#endif  // SPHERICAL_CONVERSION_H
