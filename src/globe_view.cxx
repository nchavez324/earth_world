#include "globe_view.h"

#include "filename.h"
#include "panda3d/aa_luse.h"
#include "panda3d/boundingBox.h"
#include "panda3d/boundingVolume.h"
#include "panda3d/computeNode.h"
#include "panda3d/fontPool.h"
#include "panda3d/geom.h"
#include "panda3d/geomNode.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/geomVertexData.h"
#include "panda3d/geomVertexFormat.h"
#include "panda3d/graphicsEngine.h"
#include "panda3d/graphicsStateGuardian.h"
#include "panda3d/loaderOptions.h"
#include "panda3d/mathNumbers.h"
#include "panda3d/pnmImage.h"
#include "panda3d/samplerState.h"
#include "panda3d/shader.h"
#include "panda3d/shaderAttrib.h"
#include "panda3d/shaderBuffer.h"
#include "panda3d/textFont.h"
#include "panda3d/textNode.h"
#include "panda3d/texture.h"
#include "panda3d/texturePool.h"
#include "panda3d/textureStage.h"
#include "panda3d/windowFramework.h"
#include "quaternion.h"
#include "typedefs.h"

namespace earth_world {

GlobeView::GlobeView(PT<GraphicsOutput> graphics_output, Globe& globe,
                     int vertices_per_edge)
    : path_{"Globe"} {
  // Build the texture used to denote unexplored terrain.
  LoaderOptions loader_options;
  loader_options.set_texture_flags(LoaderOptions::TF_float);
  PT<Texture> incognita_texture =
      TexturePool::load_texture(filename::forTexture("paper_3000x3000.png"),
                                /* primaryFileNumChannels= */ 3,
                                /* readMipmaps= */ false, loader_options);
  incognita_texture->set_format(Texture::F_rgb);
  incognita_texture->set_wrap_u(SamplerState::WM_repeat);
  incognita_texture->set_wrap_v(SamplerState::WM_repeat);
  incognita_texture->set_name("incognita");

  // Build the material shader and apply all texture stages.
  PT<Shader> material_shader =
      Shader::load(Shader::SL_GLSL, filename::forShader("globe.vert"),
                   filename::forShader("globe.frag"));
  NodePath mesh_path = buildGeometry(graphics_output, globe, vertices_per_edge);
  mesh_path.set_shader(material_shader);
  mesh_path.set_shader_input("u_LandMaskCutoff",
                             LVector2(globe.getLandMaskCutoff(), 0));
  setTextureStage(mesh_path, globe.getTopologyTexture(), /* prio= */ 0);
  setTextureStage(mesh_path, globe.getBathymetryTexture(), /* prio= */ 1);
  setTextureStage(mesh_path, globe.getLandMaskTexture(), /* prio= */ 2);
  setTextureStage(mesh_path, globe.getAlbedoTexture(), /* prio= */ 3);
  setTextureStage(mesh_path, globe.getVisibilityTexture(), /* prio= */ 4);
  setTextureStage(mesh_path, incognita_texture, /* prio= */ 5);
  mesh_path.reparent_to(path_);
}

GlobeView::GlobeView(GlobeView&& other) noexcept : path_{other.path_} {
  other.path_.clear();
}

GlobeView& GlobeView::operator=(GlobeView&& other) noexcept {
  if (path_ == other.path_) {
    return *this;
  }
  path_.remove_node();
  path_ = other.path_;
  other.path_.clear();
  return *this;
}

GlobeView::~GlobeView() { path_.remove_node(); }

NodePath GlobeView::getPath() const { return path_; }

NodePath GlobeView::buildGeometry(PT<GraphicsOutput> graphics_output,
                                  Globe& globe, int vertices_per_edge) {
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
                                     LVector2(globe.getLandMaskCutoff(), 0));
  position_vertices.set_shader_input("u_VertexBuffer", vertex_buffer);
  position_vertices.set_shader_input("u_TopologyTex",
                                     globe.getTopologyTexture());
  position_vertices.set_shader_input("u_BathymetryTex",
                                     globe.getBathymetryTexture());
  position_vertices.set_shader_input("u_LandMaskTex",
                                     globe.getLandMaskTexture());
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib,
            position_vertices.get_attrib(ShaderAttrib::get_class_type()));
  LVector3i work_groups(static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        static_cast<int>(std::ceil(vertices_per_edge / 16.0)),
                        face_count);

  PT<GraphicsEngine> engine = graphics_output->get_engine();
  PT<GraphicsStateGuardian> state_guardian = graphics_output->get_gsg();
  engine->dispatch_compute(work_groups, attributes, state_guardian);

  // Send the vertex buffer to the material shader to pull the vertices for
  // rendering.
  NodePath globe_path(node);
  globe_path.set_shader_input("u_VertexBuffer", vertex_buffer);

  return globe_path;
}

void GlobeView::setTextureStage(NodePath path, PT<Texture> texture,
                                int priority) {
  PT<TextureStage> texture_stage =
      new TextureStage(texture->get_name() + "_stage");
  path.set_texture(texture_stage, texture, priority);
}

}  // namespace earth_world
