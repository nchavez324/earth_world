#ifndef GLOBE_H
#define GLOBE_H

#include "panda3d/aa_luse.h"
#include "panda3d/geomNode.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/graphicsStateGuardian.h"
#include "panda3d/nodePath.h"
#include "panda3d/pandaNode.h"
#include "panda3d/pnmImage.h"
#include "panda3d/referenceCount.h"
#include "sphere_point.h"
#include "typedefs.h"

namespace earth_world {

/** A representation of the planet Earth. */
class Globe : public ReferenceCount {
 public:
  Globe() = delete;

  /**
   * Tests whether there is land at the given unit sphere point.
   * @param point The point to test.
   * @return True if the given point rests on land.
   */
  bool isLandAtPoint(SpherePoint2 point);

  /**
   * Updates the visible area of the globe to include what would be visible at
   * the given player's spherical position.
   * @param graphics_engine The engine rendering the globe.
   * @param graphics_state_guardian The graphics state guardian responsible for
   *     the globe.
   * @param player_position The unit sphere position the player is currently at.
   */
  void updateVisibility(PT<GraphicsEngine> graphics_engine,
                        PT<GraphicsStateGuardian> graphics_state_guardian,
                        SpherePoint2 player_position);

  inline NodePath getPath() { return path_; }

  /**
   * Builds a new globe instance.
   * @param graphics_engine The engine rendering the globe.
   * @param graphics_state_guardian The graphics state guardian responsible for
   *     the globe.
   * @param vertices_per_edge The number of vertices to use for each edge of the
   *     sphere-cube used for the globe's topology.
   */
  static PT<Globe> build(PT<GraphicsEngine> graphics_engine,
                         PT<GraphicsStateGuardian> graphics_state_guardian,
                         int vertices_per_edge);

 protected:
  NodePath path_;
  PT<GeomNode> node_;
  /** The node that contains the compute shader for updating visibility. */
  NodePath visibility_compute_path_;
  /** The image representing a boolean mask of land vs. sea. */
  PNMImage land_mask_image_;

 private:
  /**
   * @param path The globe's node.
   * @param node The geometry node rendering the globe.
   * @param visibility_compute_path The node that contains the compute shader
   *     for updating visibility.
   * @param land_mask_image The image representing a boolean mask of land vs.
   *     sea.
   */
  explicit Globe(NodePath path, PT<GeomNode> node,
                 NodePath visibility_compute_path, PNMImage land_mask_image);
};

}  // namespace earth_world

#endif  // GLOBE_H
