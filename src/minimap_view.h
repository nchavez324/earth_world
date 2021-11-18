#ifndef EARTH_WORLD_MINIMAP_H
#define EARTH_WORLD_MINIMAP_H

#include "globe.h"
#include "panda3d/nodePath.h"
#include "panda3d/windowFramework.h"
#include "typedefs.h"

namespace earth_world {

/** Visualizes a minimap in the corner of the screen. */
class MinimapView {
 public:
  MinimapView(Globe &globe);
  MinimapView(const MinimapView &) = delete;
  MinimapView(MinimapView &&);
  MinimapView &operator=(const MinimapView &) = delete;
  MinimapView &operator=(MinimapView &&);
  ~MinimapView();

  NodePath getPath() const;

  void onWindowResize(LVector2i new_window_size);

 protected:
  NodePath path_;
  NodePath map_path_;
  NodePath border_path_;

  /** Builds the node for rendering the map. */
  static NodePath buildMapNode(Globe &globe);

  /** Builds the node for rendering the border frame behind the map. */
  static NodePath buildBorderNode();
};

}  // namespace earth_world

#endif  // EARTH_WORLD_MINIMAP_H
