#ifndef EARTH_WORLD_TYPEDEFS_H
#define EARTH_WORLD_TYPEDEFS_H

namespace earth_world {
/** Typedefs to shorthand Panda3D types. */

template <typename T>
using PT = PointerTo<T>;
template <typename T>
using CPT = ConstPointerTo<T>;

}  // namespace earth_world

#endif  // EARTH_WORLD_TYPEDEFS_H