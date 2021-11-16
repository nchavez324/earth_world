#ifndef DEBUG_AXES_H
#define DEBUG_AXES_H

#include "panda3d/nodePath.h"

namespace earth_world {
namespace debug_axes {

/** Creates a set of 3D axes with unit-length for debugging. */
NodePath build();

}  // namespace debug_axes
}  // namespace earth_world

#endif  // DEBUG_AXES_H
