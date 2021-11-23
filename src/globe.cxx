#include "globe.h"

#include "filename.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/graphicsStateGuardian.h"
#include "panda3d/nodePath.h"
#include "panda3d/texturePool.h"
#include "sphere_point.h"

namespace earth_world {

const bool kEnableLandCollision = true;
const PN_stdfloat kLandMaskCutoff = 0.5f;
const LVector2i kMainTexSize(16384, 8192);
const LVector2i kNormalTexSize(16384, 8192);
const LVector2i kVisibilityTexSize(2048, 1024);

Globe::Globe(GraphicsOutput *graphics_output)
    : Globe(graphics_output, loadTex("topology", kMainTexSize, Texture::F_red),
            loadTex("bathymetry", kMainTexSize, Texture::F_red),
            loadTex("land_mask", kMainTexSize, Texture::F_red),
            loadTex("albedo_1", kMainTexSize, Texture::F_rgb),
            buildVisibilityTex(kVisibilityTexSize), kLandMaskCutoff) {}

Globe::Globe(GraphicsOutput *graphics_output, PT<Texture> topology_texture,
             PT<Texture> bathymetry_texture, PT<Texture> land_mask_texture,
             PT<Texture> albedo_texture, PT<Texture> visibility_texture,
             PN_stdfloat land_mask_cutoff)
    : topology_texture_{topology_texture},
      bathymetry_texture_{bathymetry_texture},
      land_mask_texture_{land_mask_texture},
      albedo_texture_{albedo_texture},
      normal_texture_{buildNormalTex(graphics_output, topology_texture,
                                     bathymetry_texture, land_mask_texture,
                                     land_mask_cutoff)},
      visibility_texture_{visibility_texture},
      visibility_compute_{"VisibilityCompute"},
      land_mask_cutoff_{land_mask_cutoff} {
  // Load the topology into the CPU for city placement.
  topology_texture->store(topology_image_);
  // Load the land mask into the CPU for collision detection.
  land_mask_texture->store(land_mask_image_);

  // Set up recurring shader to update visibility mask.
  PT<Shader> visibility_shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("updateVisibility.comp"));
  visibility_compute_.set_shader(visibility_shader);
  visibility_compute_.set_shader_input("u_VisibilityTex", visibility_texture_);
}

PT<Texture> Globe::getTopologyTexture() { return topology_texture_; }

PT<Texture> Globe::getBathymetryTexture() { return bathymetry_texture_; }

PT<Texture> Globe::getLandMaskTexture() { return land_mask_texture_; }

PT<Texture> Globe::getAlbedoTexture() { return albedo_texture_; }

PT<Texture> Globe::getNormalTexture() { return normal_texture_; }

PT<Texture> Globe::getVisibilityTexture() { return visibility_texture_; }

PN_stdfloat Globe::getLandMaskCutoff() const { return land_mask_cutoff_; }

bool Globe::isLandAtPoint(const SpherePoint2 &point) const {
  if (!kEnableLandCollision) {
    return false;
  }
  PN_stdfloat land_mask_sample = sampleImage(land_mask_image_, point.toUV());
  return land_mask_sample <= land_mask_cutoff_;
}

PN_stdfloat Globe::getHeightAtPoint(const SpherePoint2 &point) const {
  LPoint2 uv = point.toUV();
  PN_stdfloat topology_sample = sampleImage(topology_image_, uv);
  return (topology_sample * (1.f - kGlobeWaterSurfaceHeight)) +
         kGlobeWaterSurfaceHeight;
}

void Globe::updateVisibility(GraphicsOutput *graphics_output,
                             const SpherePoint2 &player_position) {
  if (graphics_output == nullptr) {
    return;
  }
  GraphicsEngine *engine = graphics_output->get_engine();
  GraphicsStateGuardian *state_guardian = graphics_output->get_gsg();
  if (engine == nullptr || state_guardian == nullptr) {
    return;
  }

  SpherePoint3 coords = player_position.toRadial();
  visibility_compute_.set_shader_input("u_PlayerSphericalCoords", coords);
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib,
            visibility_compute_.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(2048 / 16, 1024 / 16, 1);

  engine->dispatch_compute(work_groups, attributes, state_guardian);
}

PT<Texture> Globe::loadTex(const std::string &texture_base_name,
                           const LVector2i &texture_size,
                           Texture::Format format) {
  LoaderOptions loader_options;
  loader_options.set_texture_flags(LoaderOptions::TF_float);
  int channel_count = 1;
  switch (format) {
    case Texture::F_red:
    case Texture::F_green:
    case Texture::F_blue:
    case Texture::F_alpha:
      channel_count = 1;
      break;
    case Texture::F_rg:
      channel_count = 2;
      break;
    case Texture::F_rgb:
      channel_count = 3;
      break;
    case Texture::F_rgba:
      channel_count = 4;
      break;
    default:
      break;
  }
  PT<Texture> texture = TexturePool::load_texture(
      filename::forTexture(texture_base_name + "_" +
                           std::to_string(texture_size.get_x()) + "x" +
                           std::to_string(texture_size.get_y()) + ".png"),
      /* primaryFileNumChannels= */ channel_count, /* readMipmaps= */ false,
      loader_options);
  texture->set_format(format);
  texture->set_wrap_u(SamplerState::WM_repeat);
  texture->set_wrap_v(SamplerState::WM_repeat);
  texture->set_name(texture_base_name);
  return texture;
}

PT<Texture> Globe::buildVisibilityTex(const LVector2i &texture_size) {
  // In the red channel, store everything that's ever been seen, and in the
  // green channel store what's immediately visible.
  PT<Texture> visibility_texture = new Texture("VisibilityTexture");
  // To save you some headache, just trying r8 or r16 doesn't work. It appears
  // that when it passes to the fragment shader, it chokes trying to interpret.
  visibility_texture->setup_2d_texture(texture_size.get_x(),
                                       texture_size.get_y(), Texture::T_float,
                                       Texture::F_rg16);
  LColor clear_color(0, 0, 0, 0);
  visibility_texture->set_clear_color(clear_color);
  visibility_texture->set_wrap_u(SamplerState::WM_repeat);
  return visibility_texture;
}

PT<Texture> Globe::buildNormalTex(GraphicsOutput *graphics_output,
                                  PT<Texture> topology_texture,
                                  PT<Texture> bathymetry_texture,
                                  PT<Texture> land_mask_texture,
                                  PN_stdfloat land_mask_cutoff) {
  // Store x,y,z in the r,g,b channels.
  PT<Texture> normal_texture = new Texture("NormalTexture");
  normal_texture->setup_2d_texture(kNormalTexSize.get_x(),
                                   kNormalTexSize.get_y(), Texture::T_float,
                                   Texture::F_r11_g11_b10);
  LColor clear_color(0, 0, 0, 0);
  normal_texture->set_clear_color(clear_color);
  normal_texture->set_wrap_u(SamplerState::WM_repeat);

  // Run one off calculate normals compute shader.
  PT<Shader> shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("calculateNormals.comp"));
  NodePath compute_path("CalculateNormalsCompute");
  compute_path.set_shader(shader);
  compute_path.set_shader_input("u_LandMaskCutoff",
                                LVector2(land_mask_cutoff, 0));
  compute_path.set_shader_input("u_TopologyTex", topology_texture);
  compute_path.set_shader_input("u_BathymetryTex", bathymetry_texture);
  compute_path.set_shader_input("u_LandMaskTex", land_mask_texture);
  compute_path.set_shader_input("u_NormalTex", normal_texture);
  CPT<ShaderAttrib> attributes = DCAST(
      ShaderAttrib, compute_path.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(
      static_cast<int>(std::ceil(kNormalTexSize.get_x() / 16.0)),
      static_cast<int>(std::ceil(kNormalTexSize.get_y() / 16.0)), 1);
  GraphicsEngine *engine = graphics_output->get_engine();
  GraphicsStateGuardian *state_guardian = graphics_output->get_gsg();
  engine->dispatch_compute(work_groups, attributes, state_guardian);
  return normal_texture;
}

PN_stdfloat Globe::sampleImage(const PNMImage &image, const LPoint2 &uv) {
  LPoint2i pixel = LPoint2i(static_cast<int>(image.get_x_size() * uv.get_x()),
                            static_cast<int>(image.get_y_size() * uv.get_y()));
  pixel.set_x(std::max(0, pixel.get_x()));
  pixel.set_y(std::max(0, pixel.get_y()));
  pixel.set_x(std::min(image.get_x_size() - 1, pixel.get_x()));
  pixel.set_y(std::min(image.get_y_size() - 1, pixel.get_y()));
  return image.get_bright(pixel.get_x(), pixel.get_y());
}

}  // namespace earth_world
