#include "debug_axes.h"

#include "panda3d/geom.h"
#include "panda3d/geomLines.h"
#include "panda3d/geomNode.h"
#include "panda3d/geomVertexData.h"
#include "panda3d/geomVertexFormat.h"
#include "panda3d/geomVertexWriter.h"
#include "typedefs.h"

namespace earth_world {
namespace debug_axes {

NodePath build() {
  PT<GeomLines> lines = new GeomLines(Geom::UH_static);
  PT<GeomVertexData> vertex_data =
      new GeomVertexData("Axes", GeomVertexFormat::get_v3c4(), Geom::UH_static);
  vertex_data->set_num_rows(12);
  GeomVertexWriter vertices(vertex_data, "vertex");
  GeomVertexWriter colors(vertex_data, "color");

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(1, 0, 0);
  colors.add_data4(1, 0, 0, 1);
  colors.add_data4(1, 0, 0, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(-1, 0, 0);
  colors.add_data4(0, 1, 1, 1);
  colors.add_data4(0, 1, 1, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, 1, 0);
  colors.add_data4(0, 1, 0, 1);
  colors.add_data4(0, 1, 0, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, -1, 0);
  colors.add_data4(1, 0, 1, 1);
  colors.add_data4(1, 0, 1, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, 0, 1);
  colors.add_data4(0, 0, 1, 1);
  colors.add_data4(0, 0, 1, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, 0, -1);
  colors.add_data4(1, 1, 0, 1);
  colors.add_data4(1, 1, 0, 1);

  lines->add_consecutive_vertices(0, 12);
  lines->close_primitive();

  PT<Geom> geom = new Geom(vertex_data);
  geom->add_primitive(lines);
  PT<GeomNode> node = new GeomNode("Axes");
  node->add_geom(geom);
  NodePath path(node);
  path.set_render_mode_thickness(2);
  return path;
}

}  // namespace debug_axes
}  // namespace earth_world
