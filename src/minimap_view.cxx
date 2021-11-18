#include "minimap_view.h"

#include "filename.h"
#include "panda3d/geom.h"
#include "panda3d/geomNode.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/geomVertexData.h"
#include "panda3d/geomVertexFormat.h"
#include "panda3d/geomVertexWriter.h"
#include "panda3d/nodePath.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"
#include "typedefs.h"

namespace earth_world {

const PN_stdfloat kScreenWidthXPct = 0.25f;
const PN_stdfloat kScreenMarginPct = 0.01f;
const PN_stdfloat kBorderWidthPx = 2.f;
const PN_stdfloat kAspectRatio = 2.f;
const LColor kBorderColor(0.4, 0.2, 0.1, 1);

MinimapView::MinimapView(Globe &globe) : path_{"Minimap"} {
  map_path_ = buildMapNode(globe);
  border_path_ = buildBorderNode();

  border_path_.reparent_to(path_);
  map_path_.reparent_to(path_);
}

MinimapView::MinimapView(MinimapView &&other)
    : path_{other.path_},
      map_path_{other.map_path_},
      border_path_{other.border_path_} {
  other.path_.clear();
  other.map_path_.clear();
  other.border_path_.clear();
}

MinimapView &MinimapView::operator=(MinimapView &&other) {
  if (path_ == other.path_) {
    return *this;
  }
  path_.remove_node();
  path_ = other.path_;
  map_path_ = other.map_path_;
  border_path_ = other.border_path_;
  other.path_.clear();
  other.map_path_.clear();
  other.border_path_.clear();
  return *this;
}

MinimapView::~MinimapView() { path_.remove_node(); }

NodePath MinimapView::getPath() const { return path_; }

void MinimapView::onWindowResize(LVector2i new_window_size) {
  PN_stdfloat min_dimension =
      std::min(new_window_size.get_x(), new_window_size.get_y());
  PN_stdfloat margin = kScreenMarginPct * min_dimension;

  PN_stdfloat frame_width = kScreenWidthXPct * new_window_size.get_x();
  PN_stdfloat frame_height = frame_width / kAspectRatio;
  PN_stdfloat frame_x = frame_width / 2.f + margin;
  PN_stdfloat frame_y = -new_window_size.get_y() + frame_height / 2.f + margin;

  PN_stdfloat map_width = frame_width - (2 * kBorderWidthPx);
  PN_stdfloat map_height = frame_height - (2 * kBorderWidthPx);

  border_path_.set_pos(frame_x, 1, frame_y);
  border_path_.set_scale(frame_width, 1, frame_height);

  map_path_.set_pos(frame_x, 1, frame_y);
  map_path_.set_scale(map_width, 1, map_height);
}

NodePath MinimapView::buildMapNode(Globe &globe) {
  PT<GeomTriangles> triangles = new GeomTriangles(Geom::UH_static);
  PT<GeomVertexData> vertex_data = new GeomVertexData(
      "MinimapMap", GeomVertexFormat::get_v3t2(), Geom::UH_static);
  vertex_data->set_num_rows(4);
  GeomVertexWriter vertices(vertex_data, "vertex");
  GeomVertexWriter uvs(vertex_data, "texcoord");
  vertices.add_data3(-0.5f, 0, -0.5f);
  vertices.add_data3(+0.5f, 0, -0.5f);
  vertices.add_data3(-0.5f, 0, +0.5f);
  vertices.add_data3(+0.5f, 0, +0.5f);
  uvs.add_data2(0, 0);
  uvs.add_data2(1, 0);
  uvs.add_data2(0, 1);
  uvs.add_data2(1, 1);
  triangles->add_vertices(0, 1, 2);
  triangles->add_vertices(2, 1, 3);
  triangles->close_primitive();
  PT<Geom> geom = new Geom(vertex_data);
  geom->add_primitive(triangles);
  PT<GeomNode> geom_node = new GeomNode("MinimapMap");
  geom_node->add_geom(geom);

  NodePath path = NodePath(geom_node);
  PT<Shader> minimap_shader =
      Shader::load(Shader::SL_GLSL, filename::forShader("minimap.vert"),
                   filename::forShader("minimap.frag"));
  path.set_shader(minimap_shader);
  path.set_shader_input("u_LandMaskCutoff",
                        LVector2(globe.getLandMaskCutoff(), 0));

  PT<Texture> land_mask_tex = globe.getLandMaskTexture();
  PT<Texture> visibility_tex = globe.getVisibilityTexture();
  LoaderOptions loader_options;
  loader_options.set_texture_flags(LoaderOptions::TF_float);
  PT<Texture> incognita_tex =
      TexturePool::load_texture(filename::forTexture("paper_3000x3000.png"),
                                /* primaryFileNumChannels= */ 3,
                                /* readMipmaps= */ false, loader_options);
  incognita_tex->set_format(Texture::F_rgb);
  incognita_tex->set_wrap_u(SamplerState::WM_repeat);
  incognita_tex->set_wrap_v(SamplerState::WM_repeat);
  incognita_tex->set_name("incognita");

  path.set_texture(new TextureStage("land_mask_stage"), land_mask_tex,
                   /* prio= */ 0);
  path.set_texture(new TextureStage("visibility_stage"), visibility_tex,
                   /* prio= */ 1);
  path.set_texture(new TextureStage("incognita_stage"), incognita_tex,
                   /* prio= */ 2);
  return path;
}

NodePath MinimapView::buildBorderNode() {
  PT<GeomTriangles> triangles = new GeomTriangles(Geom::UH_static);
  PT<GeomVertexData> vertex_data = new GeomVertexData(
      "MinimapBorder", GeomVertexFormat::get_v3c4(), Geom::UH_static);
  vertex_data->set_num_rows(4);
  GeomVertexWriter vertices(vertex_data, "vertex");
  GeomVertexWriter colors(vertex_data, "color");
  vertices.add_data3(-0.5f, 0, -0.5f);
  vertices.add_data3(+0.5f, 0, -0.5f);
  vertices.add_data3(-0.5f, 0, +0.5f);
  vertices.add_data3(+0.5f, 0, +0.5f);
  colors.add_data4(kBorderColor);
  colors.add_data4(kBorderColor);
  colors.add_data4(kBorderColor);
  colors.add_data4(kBorderColor);
  triangles->add_vertices(0, 1, 2);
  triangles->add_vertices(2, 1, 3);
  triangles->close_primitive();
  PT<Geom> geom = new Geom(vertex_data);
  geom->add_primitive(triangles);
  PT<GeomNode> geom_node = new GeomNode("MinimapBorder");
  geom_node->add_geom(geom);

  NodePath path = NodePath(geom_node);
  return path;
}

}  // namespace earth_world
