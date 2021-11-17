#include "globe.h"

#include "city.h"
#include "filename.h"
#include "panda3d/aa_luse.h"
#include "panda3d/boundingBox.h"
#include "panda3d/boundingVolume.h"
#include "panda3d/computeNode.h"
#include "panda3d/geom.h"
#include "panda3d/geomNode.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/geomVertexData.h"
#include "panda3d/geomVertexFormat.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/loaderOptions.h"
#include "panda3d/mathNumbers.h"
#include "panda3d/pnmImage.h"
#include "panda3d/samplerState.h"
#include "panda3d/shader.h"
#include "panda3d/shaderAttrib.h"
#include "panda3d/shaderBuffer.h"
#include "panda3d/texture.h"
#include "panda3d/textNode.h"
#include "panda3d/textFont.h"
#include "panda3d/fontPool.h"
#include "panda3d/texturePool.h"
#include "panda3d/textureStage.h"
#include "quaternion.h"
#include "typedefs.h"

namespace earth_world {

bool const kEnableLandCollision = false;
PN_stdfloat const kCityScale = 0.005f;
PN_stdfloat const kCityLabelScale = 0.007f;
LVector2i kMainTexSize(16384, 8192);
LVector2i kIncognitaTexSize(3000, 3000);
LVector2i kVisibilityTexSize(2048, 1024);
PN_stdfloat const kLandMaskCutoff = 0.5f;

int const kCityCount = 4;
CityStaticData const kCities[] = {
    CityStaticData(
        "New York", "USA",
        SpherePoint2::fromLatitudeAndLongitude(40.712776f, -74.005974)),
    CityStaticData(
        "Bogota", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(4.710989f, -74.072090f)),
    CityStaticData(
        "Honolulu", "USA",
        SpherePoint2::fromLatitudeAndLongitude(21.309919f, -157.858154f)),
    CityStaticData(
        "Lisbon", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(38.685108f, -9.238115f)),
};

Globe::Globe(NodePath path, NodePath visibility_compute_path,
             PNMImage land_mask_image)
    : path_(path),
      visibility_compute_path_(visibility_compute_path),
      land_mask_image_(land_mask_image) {}

bool Globe::isLandAtPoint(SpherePoint2 point) {
  if (!kEnableLandCollision) {
    return false;
  }
  PN_stdfloat land_mask_sample = sampleImage(land_mask_image_, point.toUV());
  return land_mask_sample <= kLandMaskCutoff;
}

void Globe::updateVisibility(PT<GraphicsEngine> graphics_engine,
                             PT<GraphicsStateGuardian> graphics_state_guardian,
                             SpherePoint2 player_position) {
  SpherePoint3 coords = player_position.toRadial();
  visibility_compute_path_.set_shader_input("u_PlayerSphericalCoords", coords);
  CPT<ShaderAttrib> attributes = DCAST(
      ShaderAttrib,
      visibility_compute_path_.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(2048 / 16, 1024 / 16, 1);
  graphics_engine->dispatch_compute(work_groups, attributes,
                                    graphics_state_guardian);
}

PT<Globe> Globe::build(PT<GraphicsEngine> graphics_engine,
                       PT<GraphicsStateGuardian> graphics_state_guardian,
                       NodePath city_prefab, int vertices_per_edge) {
  PT<Texture> topology_tex =
      loadTexture("topology", kMainTexSize, Texture::F_red);
  PT<Texture> bathymetry_tex =
      loadTexture("bathymetry", kMainTexSize, Texture::F_red);
  PT<Texture> land_mask_tex =
      loadTexture("land_mask", kMainTexSize, Texture::F_red);
  PT<Texture> albedo_tex =
      loadTexture("albedo_1", kMainTexSize, Texture::F_rgb);
  PT<Texture> visibility_tex = buildVisibilityTexture(kVisibilityTexSize);
  PT<Texture> incognita_tex =
      loadTexture("paper", kIncognitaTexSize, Texture::F_rgb);

  // Load another copy of the land mask for the CPU to use in collision
  // detection.
  PNMImage land_mask_image;
  land_mask_tex->store(land_mask_image);
  // Load another copy of the topology to place the cities.
  PNMImage topology_image;
  topology_tex->store(topology_image);

  PT<Shader> material_shader =
      Shader::load(Shader::SL_GLSL, filename::forShader("simple.vert"),
                   filename::forShader("simple.frag"));
  NodePath globe_path =
      buildGeometry(graphics_engine, graphics_state_guardian, topology_tex,
                    bathymetry_tex, land_mask_tex, vertices_per_edge);
  globe_path.set_shader(material_shader);
  globe_path.set_shader_input("u_LandMaskCutoff", LVector2(kLandMaskCutoff, 0));
  setTextureStage(globe_path, topology_tex, /* prio= */ 0);
  setTextureStage(globe_path, bathymetry_tex, /* prio= */ 1);
  setTextureStage(globe_path, land_mask_tex, /* prio= */ 2);
  setTextureStage(globe_path, albedo_tex, /* prio= */ 3);
  setTextureStage(globe_path, visibility_tex, /* prio= */ 4);
  setTextureStage(globe_path, incognita_tex, /* prio= */ 5);

  // Set up recurring shader to update visibility mask.
  NodePath visibility_compute_path("VisibilityCompute");
  PT<Shader> visibility_shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("updateVisibility.comp"));
  visibility_compute_path.set_shader(visibility_shader);
  visibility_compute_path.set_shader_input("u_VisibilityTex", visibility_tex);

  NodePath root_path("GlobeRoot");
  globe_path.reparent_to(root_path);

  // Place cities on the globe.
  for (int i = 0; i < kCityCount; i++) {
    const CityStaticData &city_static_data = kCities[i];
    SpherePoint2 city_unit_sphere_position = city_static_data.getLocation();
    PN_stdfloat topology_sample =
        sampleImage(topology_image, city_unit_sphere_position.toUV());
    PN_stdfloat city_height = (topology_sample * (1.f - 0.95f)) + 0.95f;

    PT<TextFont> font = FontPool::load_font("cmr12.egg");
    PT<TextNode> city_label =
        new TextNode(city_static_data.getName() + "_label");
    city_label->set_text(city_static_data.getName());
    city_label->set_font(font);
    NodePath city_label_path = root_path.attach_new_node(city_label);

    NodePath city_path = city_prefab.copy_to(root_path);
    City city(city_path, city_label_path, city_static_data, city_height);
    city_path.set_pos(city.getLocation().toCartesian());
    city_path.set_quat(city.getRotation());
    city_path.set_scale(kCityScale);

    SpherePoint3 city_label_position = city.getLocation();
    city_label_position.set(
      city_label_position.get_azimuthal() + (0.01f / MathNumbers::pi),
      city_label_position.get_polar() + (0.01f / MathNumbers::pi),
      city_label_position.get_radial());
    city_label_path.set_pos(city_label_position.toCartesian());
    city_label_path.set_quat(quaternion::fromLookAt(
        -city_unit_sphere_position.toCartesian(), LVector3::up()));
    city_label_path.set_scale(kCityLabelScale);
    city_label_path.set_depth_test(false); 
  }

  PT<Globe> globe =
      new Globe(root_path, visibility_compute_path, land_mask_image);
  return globe;
}

NodePath Globe::buildGeometry(PT<GraphicsEngine> graphics_engine,
                              PT<GraphicsStateGuardian> graphics_state_guardian,
                              PT<Texture> topology_tex,
                              PT<Texture> bathymetry_tex,
                              PT<Texture> land_mask_tex,
                              int vertices_per_edge) {
  // Among the most important is the fact that arrays of types are not
  // necessarily tightly packed. An array of floats in such a block will not be
  // the equivalent to an array of floats in C/C++. The array stride (the bytes
  // between array elements) is always rounded up to the size of a vec4 (ie:
  // 16-bytes). So arrays will only match their C/C++ definitions if the type is
  // a multiple of 16 bytes. Warning: Implementations sometimes get the std140
  // layout wrong for vec3 components. You are advised to manually pad your
  // structures/arrays out and avoid using vec3 at all. std430: This layout
  // works like std140, except with a few optimizations in the alignment and
  // strides for arrays and structs of scalars and vector elements (except for
  // vec3 elements, which remain unchanged from {code|std140}}). Specifically,
  // they are no longer rounded up to a multiple of 16 bytes. So an array of
  // `float`s will match with a C++ array of `float`s.

  vertices_per_edge = std::max(vertices_per_edge, 2);
  int face_count = 6;
  int vertices_per_face = vertices_per_edge * vertices_per_edge;
  int vertex_count = vertices_per_face * face_count;
  uint64_t buffer_size =
      sizeof(float) * 4 * static_cast<uint64_t>(vertex_count);
  // Pad to 16 bytes, based on advice in panda3d/shaderBuffer.i.
  if ((buffer_size & 15u) != 0) {
    buffer_size = ((buffer_size + 15u) & ~15u);
  }

  PT<ShaderBuffer> vertex_buffer =
      new ShaderBuffer("positions", buffer_size, Geom::UH_static);
  PT<GeomVertexData> vertex_data = new GeomVertexData(
      "Globe", GeomVertexFormat::get_empty(), Geom::UH_static);
  PT<GeomTriangles> triangles = new GeomTriangles(Geom::UH_static);
  // Lay out the triangle indices, since the mesh topology is fixed.
  for (int face = 0; face < face_count; face++) {
    for (int vertex_x = 0; vertex_x < vertices_per_edge - 1; vertex_x++) {
      for (int vertex_y = 0; vertex_y < vertices_per_edge - 1; vertex_y++) {
        int v0 = (vertices_per_face * face) + (vertices_per_edge * vertex_y) +
                 vertex_x;
        int v1 = v0 + 1;
        int v2 = v0 + vertices_per_edge;
        int v3 = v0 + vertices_per_edge + 1;
        triangles->add_vertices(v2, v1, v0);
        triangles->add_vertices(v3, v1, v2);
      }
    }
  }

  PT<Geom> geom = new Geom(vertex_data);
  geom->add_primitive(triangles);
  PT<BoundingBox> bounds =
      new BoundingBox(LPoint3(-1, -1, -1), LPoint3(1, 1, 1));
  geom->set_bounds(bounds);
  PT<GeomNode> node = new GeomNode("Globe");
  node->add_geom(geom);
  node->set_bounds_type(BoundingVolume::BT_box);

  // Run one off position vertices compute shader.
  PT<Shader> position_vertices_shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("positionVertices.comp"));
  NodePath position_vertices("PositionVerticesCompute");
  position_vertices.set_shader(position_vertices_shader);
  position_vertices.set_shader_input("u_VerticesPerEdge",
                                     LVector2i(vertices_per_edge, 0));
  position_vertices.set_shader_input("u_LandMaskCutoff",
                                     LVector2(kLandMaskCutoff, 0));
  position_vertices.set_shader_input("u_VertexBuffer", vertex_buffer);
  position_vertices.set_shader_input("u_TopologyTex", topology_tex);
  position_vertices.set_shader_input("u_BathymetryTex", bathymetry_tex);
  position_vertices.set_shader_input("u_LandMaskTex", land_mask_tex);
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib,
            position_vertices.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        face_count);
  graphics_engine->dispatch_compute(work_groups, attributes,
                                    graphics_state_guardian);

  // Send the vertex buffer to the material shader to pull the vertices for
  // rendering.
  NodePath globe_path(node);
  globe_path.set_shader_input("u_VertexBuffer", vertex_buffer);

  return globe_path;
}

void Globe::setTextureStage(NodePath path, PT<Texture> texture, int priority) {
  PT<TextureStage> texture_stage =
      new TextureStage(texture->get_name() + "_stage");
  path.set_texture(texture_stage, texture, priority);
}

PT<Texture> Globe::loadTexture(std::string texture_base_name,
                               LVector2i texture_size, Texture::Format format) {
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

PT<Texture> Globe::buildVisibilityTexture(LVector2i texture_size) {
  // In the red channel, store everything that's ever been seen, and in the
  // green channel store what's immediately visible.
  PT<Texture> visibility_texture = new Texture("VisibilityTexture");
  // To save you some headache, just trying r8 or r16 doesn't work. It appears
  // that when it passes to the fragment shader, it chokes trying to interpret.
  visibility_texture->setup_2d_texture(texture_size.get_x(),
                                       texture_size.get_y(), Texture::T_float,
                                       Texture::F_rg16);
  LColor clear_color(1, 1, 1, 1);
  visibility_texture->set_clear_color(clear_color);
  visibility_texture->set_wrap_u(SamplerState::WM_repeat);
  return visibility_texture;
}

PN_stdfloat Globe::sampleImage(PNMImage &image, LPoint2 UV) {
  LPoint2i pixel = LPoint2i(static_cast<int>(image.get_x_size() * UV.get_x()),
                            static_cast<int>(image.get_y_size() * UV.get_y()));
  pixel.set_x(std::max(0, pixel.get_x()));
  pixel.set_y(std::max(0, pixel.get_y()));
  pixel.set_x(std::min(image.get_x_size() - 1, pixel.get_x()));
  pixel.set_y(std::min(image.get_y_size() - 1, pixel.get_y()));
  return image.get_bright(pixel.get_x(), pixel.get_y());
}

}  // namespace earth_world
