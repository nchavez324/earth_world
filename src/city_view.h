#ifndef EARTH_WORLD_CITY_VIEW_H
#define EARTH_WORLD_CITY_VIEW_H

#include <string>

#include "city.h"
#include "panda3d/aa_luse.h"
#include "panda3d/nodePath.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/windowFramework.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {

/** Represents the view of city on the map. */
class CityView {
 public:
  CityView(PT<WindowFramework> window, const City &city);
  CityView(const CityView &) = delete;
  CityView(CityView &&) noexcept;
  CityView &operator=(const CityView &) = delete;
  CityView &operator=(CityView &&) noexcept;
  ~CityView();

  int getCityId() const;
  NodePath getPath() const;

 protected:
  int city_id_;
  NodePath path_;
  NodePath icon_path_;
  NodePath label_path_;

  static NodePath buildIconNode();
};

}  // namespace earth_world

#endif  // EARTH_WORLD_CITY_VIEW_H
