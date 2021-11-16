#ifndef EARTH_WORLD_GLOBE_H
#define EARTH_WORLD_GLOBE_H

#include "panda3d/aa_luse.h"
#include "panda3d/geomNode.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/graphicsStateGuardian.h"
#include "panda3d/nodePath.h"
#include "panda3d/pandaNode.h"
#include "panda3d/pnmImage.h"
#include "panda3d/referenceCount.h"
#include "panda3d/texture.h"
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
  /** The node that contains the compute shader for updating visibility. */
  NodePath visibility_compute_path_;
  /** The image representing a boolean mask of land vs. sea. */
  PNMImage land_mask_image_;

 private:
  /**
   * @param path The globe's node.
   * @param visibility_compute_path The node that contains the compute shader
   *     for updating visibility.
   * @param land_mask_image The image representing a boolean mask of land vs.
   *     sea.
   */
  explicit Globe(NodePath path, NodePath visibility_compute_path,
                 PNMImage land_mask_image);

  /** Helper function to build the geometry of the globe. */
  static NodePath buildGeometry(
      PT<GraphicsEngine> graphics_engine,
      PT<GraphicsStateGuardian> graphics_state_guardian,
      PT<Texture> topology_tex, PT<Texture> bathymetry_tex,
      PT<Texture> land_mask_tex, int vertices_per_edge);

  /** Sets the given texture as a new stage on the given path and priority. */
  static void setTextureStage(NodePath path, PT<Texture> texture, int priority);

  /**
   * Loads a texture with the given parameters, with filename
   * "{texture_base_name}_{texture_size.x}x{texture_size.y}.png".
   * @param texture_base_name The prefix of the texture file.
   * @param texture_size The pixel dimensions of the texture.
   * @param format The texture's format. Only accepts r,g,b,a,rg,rgb, and rgba.
   *     If not in the above, defaults to r.
   * @return The loaded texture.
   */
  static PT<Texture> loadTexture(std::string texture_base_name,
                                 LVector2i texture_size,
                                 Texture::Format format);

  /** Creates the texture used for keeping track of what's visible. */
  static PT<Texture> buildVisibilityTexture(LVector2i texture_size);
};

}  // namespace earth_world

#endif  // EARTH_WORLD_GLOBE_H
