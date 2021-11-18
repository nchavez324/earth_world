#ifndef EARTH_WORLD_GLOBE_VIEW_H
#define EARTH_WORLD_GLOBE_VIEW_H

#include <vector>

#include "city_view.h"
#include "globe.h"
#include "panda3d/aa_luse.h"
#include "panda3d/geomNode.h"
#include "panda3d/graphicsOutput.h"
#include "panda3d/nodePath.h"
#include "panda3d/pandaNode.h"
#include "panda3d/pnmImage.h"
#include "panda3d/referenceCount.h"
#include "panda3d/texture.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {

/** A view of the planet Earth. */
class GlobeView {
 public:
  /**
   * @param window The window in which this globe will be viewed.
   * @param globe The globe model to render.
   * @param vertices_per_edge The number of vertices to use for each edge of the
   *     sphere-cube used for the globe's topology.
   */
  GlobeView(PT<WindowFramework> window, Globe &globe, int vertices_per_edge);
  GlobeView(const GlobeView &) = delete;
  GlobeView(GlobeView &&) noexcept;
  GlobeView &operator=(const GlobeView &) = delete;
  GlobeView &operator=(GlobeView &&) noexcept;
  ~GlobeView();

  NodePath getPath() const;

 protected:
  NodePath path_;
  std::vector<CityView> city_views_;

  /** Helper function to build a node containing the globe's geometry. */
  static NodePath buildGeometry(PT<GraphicsOutput> graphics_output,
                                Globe &globe, int vertices_per_edge);

  /** Sets the given texture as a new stage on the given path and priority. */
  static void setTextureStage(NodePath path, PT<Texture> texture, int priority);
};

}  // namespace earth_world

#endif  // EARTH_WORLD_GLOBE_VIEW_H
