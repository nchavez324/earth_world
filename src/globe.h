#ifndef EARTH_WORLD_GLOBE_H
#define EARTH_WORLD_GLOBE_H

#include "city.h"
#include "city_static_data.h"
#include "panda3d/aa_luse.h"
#include "panda3d/graphicsOutput.h"
#include "panda3d/pnmImage.h"
#include "panda3d/texture.h"
#include "typedefs.h"

namespace earth_world {

class Globe {
 public:
  Globe();
  Globe(PT<Texture> topology_texture, PT<Texture> bathymetry_texture,
        PT<Texture> land_mask_texture, PT<Texture> albedo_texture,
        PT<Texture> visibility_texture, PN_stdfloat land_mask_cutoff,
        const std::vector<CityStaticData>& city_static_data);
  Globe(const Globe&) = delete;
  Globe(Globe&&) = delete;
  Globe& operator=(const Globe&) = delete;
  Globe& operator=(Globe&&) = delete;
  ~Globe() = default;

  PT<Texture> getTopologyTexture();
  PT<Texture> getBathymetryTexture();
  PT<Texture> getLandMaskTexture();
  PT<Texture> getAlbedoTexture();
  PT<Texture> getVisibilityTexture();
  PN_stdfloat getLandMaskCutoff() const;
  std::vector<City>& getCities();

  /**
   * Tests whether there is land at the given unit sphere point.
   * @param point The point to test.
   * @return True if the given point rests on land.
   */
  bool isLandAtPoint(const SpherePoint2& point);

  /**
   * Updates the visible area of the globe to include what would be visible at
   * the given player's spherical position.
   * @param graphics_output The graphics output to run compute shaders on.
   * @param player_position The unit sphere position the player is currently at.
   */
  void updateVisibility(GraphicsOutput* graphics_output,
                        const SpherePoint2& player_position);

 protected:
  PT<Texture> topology_texture_;
  PT<Texture> bathymetry_texture_;
  PT<Texture> land_mask_texture_;
  PT<Texture> albedo_texture_;
  PT<Texture> visibility_texture_;

  PNMImage topology_image_;
  PNMImage land_mask_image_;

  NodePath visibility_compute_;
  const PN_stdfloat land_mask_cutoff_;
  std::vector<City> cities_;

  /**
   * Loads a texture with the given parameters, with filename
   * "{texture_base_name}_{texture_size.x}x{texture_size.y}.png".
   * @param texture_base_name The prefix of the texture file.
   * @param texture_size The pixel dimensions of the texture.
   * @param format The texture's format. Only accepts r,g,b,a,rg,rgb, and rgba.
   *     If not in the above, defaults to r.
   * @return The loaded texture.
   */
  static PT<Texture> loadTex(const std::string& texture_base_name,
                             const LVector2i& texture_size,
                             Texture::Format format);

  /** Creates the texture used for keeping track of what's visible. */
  static PT<Texture> buildVisibilityTex(const LVector2i& texture_size);

  /** Samples the intensity of the image at the given point. */
  static PN_stdfloat sampleImage(const PNMImage& image, const LPoint2& uv);
};

}  // namespace earth_world

#endif  // EARTH_WORLD_GLOBE_H
