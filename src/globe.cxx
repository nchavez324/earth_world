#include "globe.h"

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
#include "panda3d/texturePool.h"
#include "panda3d/textureStage.h"
#include "typedefs.h"

namespace earth_world {

bool const kEnableLandCollision = true;
LVector2i kMainTexSize(16384, 8192);
LVector2i kIncognitaTexSize(3000, 3000);
LVector2i kVisibilityTexSize(2048, 1024);
PN_stdfloat const kLandMaskCutoff = 0.5f;

Globe::Globe(NodePath path, NodePath visibility_compute_path,
             PNMImage land_mask_image)
    : path_(path),
      visibility_compute_path_(visibility_compute_path),
      land_mask_image_(land_mask_image) {}

bool Globe::isLandAtPoint(SpherePoint2 point) {
  if (!kEnableLandCollision) {
    return false;
  }
  LPoint2 pixelUV = point.toUV();
  LPoint2i pixel = LPoint2i(
      static_cast<int>(land_mask_image_.get_x_size() * pixelUV.get_x()),
      static_cast<int>(land_mask_image_.get_y_size() * pixelUV.get_y()));
  pixel.set_x(std::max(0, pixel.get_x()));
  pixel.set_y(std::max(0, pixel.get_y()));
  pixel.set_x(std::min(land_mask_image_.get_x_size() - 1, pixel.get_x()));
  pixel.set_y(std::min(land_mask_image_.get_y_size() - 1, pixel.get_y()));
  PN_stdfloat sample =
      land_mask_image_.get_bright(pixel.get_x(), pixel.get_y());
  return sample <= kLandMaskCutoff;
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
                       int vertices_per_edge) {
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
  land_mask_image.read(filename::forTexture(
      "land_mask_" + std::to_string(kMainTexSize.get_x()) + "x" +
      std::to_string(kMainTexSize.get_y()) + ".png"));

  PT<Shader> material_shader =
      Shader::load(Shader::SL_GLSL, filename::forShader("simple.vert"),
                   filename::forShader("simple.frag"));
  NodePath path =
      buildGeometry(graphics_engine, graphics_state_guardian, topology_tex,
                    bathymetry_tex, land_mask_tex, vertices_per_edge);
  path.set_shader(material_shader);
  path.set_shader_input("u_LandMaskCutoff", LVector2(kLandMaskCutoff, 0));
  setTextureStage(path, topology_tex, /* prio= */ 0);
  setTextureStage(path, bathymetry_tex, /* prio= */ 1);
  setTextureStage(path, land_mask_tex, /* prio= */ 2);
  setTextureStage(path, albedo_tex, /* prio= */ 3);
  setTextureStage(path, visibility_tex, /* prio= */ 4);
  setTextureStage(path, incognita_tex, /* prio= */ 5);

  // Set up recurring shader to update visibility mask.
  NodePath visibility_compute_path("VisibilityCompute");
  PT<Shader> visibility_shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("updateVisibility.comp"));
  visibility_compute_path.set_shader(visibility_shader);
  visibility_compute_path.set_shader_input("u_VisibilityTex", visibility_tex);

  PT<Globe> globe = new Globe(path, visibility_compute_path, land_mask_image);
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
  NodePath path(node);
  path.set_shader_input("u_VertexBuffer", vertex_buffer);

  return path;
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
  LColor clear_color(0, 0, 0, 0);
  visibility_texture->set_clear_color(clear_color);
  visibility_texture->set_wrap_u(SamplerState::WM_repeat);
  return visibility_texture;
}

}  // namespace earth_world
