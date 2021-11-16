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
LVector2i kTextureSize(16384, 8192);
PN_stdfloat const kLandMaskCutoff = 0.5f;

Globe::Globe(NodePath path, PT<GeomNode> node, NodePath visibility_compute_path,
             PNMImage land_mask_image)
    : path_(path),
      node_(node),
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

  PT<Texture> visibility_texture = new Texture("VisibilityTexture");
  // To save you some headache, just trying r8 or r16 doesn't work. It appears
  // that when it passes to the fragment shader, it chokes trying to interpret.
  visibility_texture->setup_2d_texture(2048, 1024, Texture::T_float,
                                       Texture::F_rg16);
  LColor clear_color(0, 0, 0, 0);
  visibility_texture->set_clear_color(clear_color);
  visibility_texture->set_wrap_u(SamplerState::WM_repeat);

  LoaderOptions options;
  options.set_texture_flags(LoaderOptions::TF_float);
  PT<Texture> topology_texture = TexturePool::load_texture(
      filename::forTexture("topology_" + std::to_string(kTextureSize.get_x()) +
                           "x" + std::to_string(kTextureSize.get_y()) + ".png"),
      /* primaryFileNumChannels= */ 1, /* readMipmaps= */ false, options);
  topology_texture->set_format(Texture::F_red);
  topology_texture->set_wrap_u(SamplerState::WM_repeat);

  PT<Texture> bathymetry_texture = TexturePool::load_texture(
      filename::forTexture("bathymetry_" +
                           std::to_string(kTextureSize.get_x()) + "x" +
                           std::to_string(kTextureSize.get_y()) + ".png"),
      /* primaryFileNumChannels= */ 1, /* readMipmaps= */ false, options);
  bathymetry_texture->set_format(Texture::F_red);
  bathymetry_texture->set_wrap_u(SamplerState::WM_repeat);

  PT<Texture> land_mask_texture = TexturePool::load_texture(
      filename::forTexture("land_mask_" + std::to_string(kTextureSize.get_x()) +
                           "x" + std::to_string(kTextureSize.get_y()) + ".png"),
      /* primaryFileNumChannels= */ 1, /* readMipmaps= */ false, options);
  land_mask_texture->set_format(Texture::F_red);
  land_mask_texture->set_wrap_u(SamplerState::WM_repeat);

  PNMImage land_mask_image;
  land_mask_image.read(filename::forTexture(
      "land_mask_" + std::to_string(kTextureSize.get_x()) + "x" +
      std::to_string(kTextureSize.get_y()) + ".png"));

  PT<Texture> albedo_texture = TexturePool::load_texture(
      filename::forTexture("albedo_" + std::to_string(kTextureSize.get_x()) +
                           "x" + std::to_string(kTextureSize.get_y()) +
                           "_1.png"),
      /* primaryFileNumChannels= */ 3, /* readMipmaps= */ false, options);
  albedo_texture->set_wrap_u(SamplerState::WM_repeat);
  albedo_texture->set_format(Texture::F_rgb);

  PT<Texture> incognita_texture = TexturePool::load_texture(
      filename::forTexture("paper.jpeg"),
      /* primaryFileNumChannels= */ 3, /* readMipmaps= */ false, options);
  incognita_texture->set_format(Texture::F_rgb);
  incognita_texture->set_wrap_u(SamplerState::WM_repeat);
  incognita_texture->set_wrap_v(SamplerState::WM_repeat);

  PT<TextureStage> topology_stage = new TextureStage("TopologyStage");
  PT<TextureStage> bathymetry_stage = new TextureStage("BathymetryStage");
  PT<TextureStage> land_mask_stage = new TextureStage("LandMaskStage");
  PT<TextureStage> albedo_stage = new TextureStage("AlbedoStage");
  PT<TextureStage> visibility_stage = new TextureStage("VisibilityStage");
  PT<TextureStage> incongnita_stage = new TextureStage("IncognitaStage");

  PT<Shader> material_shader =
      Shader::load(Shader::SL_GLSL, filename::forShader("simple.vert"),
                   filename::forShader("simple.frag"));

  NodePath path(node);
  path.set_shader(material_shader);
  path.set_texture(topology_stage, topology_texture, /* priority= */ 0);
  path.set_texture(bathymetry_stage, bathymetry_texture, /* priority= */ 1);
  path.set_texture(land_mask_stage, land_mask_texture, /* priority= */ 2);
  path.set_texture(albedo_stage, albedo_texture, /* priority= */ 3);
  path.set_texture(visibility_stage, visibility_texture, /* priority= */ 4);
  path.set_texture(incongnita_stage, incognita_texture, /* priority= */ 5);
  path.set_shader_input("u_VertexBuffer", vertex_buffer);
  path.set_shader_input("u_LandMaskCutoff", LVector2(kLandMaskCutoff, 0));

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
  position_vertices.set_shader_input("u_TopologyTex", topology_texture);
  position_vertices.set_shader_input("u_BathymetryTex", bathymetry_texture);
  position_vertices.set_shader_input("u_LandMaskTex", land_mask_texture);
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib,
            position_vertices.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        face_count);
  graphics_engine->dispatch_compute(work_groups, attributes,
                                    graphics_state_guardian);

  // Set up recurring shader to update visibility mask.
  NodePath visibility_compute_path("VisibilityCompute");
  PT<Shader> visibility_shader = Shader::load_compute(
      Shader::SL_GLSL, filename::forShader("updateVisibility.comp"));
  visibility_compute_path.set_shader(visibility_shader);
  visibility_compute_path.set_shader_input("u_VisibilityTex",
                                           visibility_texture);

  PT<Globe> globe =
      new Globe(path, node, visibility_compute_path, land_mask_image);
  return globe;
}

}  // namespace earth_world
