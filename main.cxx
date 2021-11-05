#include "panda3d/geomTriangles.h"
#include "panda3d/shader.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"

int main(int argc, char *argv[]) {
  PandaFramework framework;
  framework.open_framework(argc, argv);
  framework.set_window_title("My Panda3D Window");
  WindowFramework *window = framework.open_window();

  PT(GeomVertexData) vdata;
  vdata = new GeomVertexData("name", GeomVertexFormat::get_v3n3c4(), Geom::UH_static);
  vdata->set_num_rows(4);
  GeomVertexWriter vertices(vdata, "vertex");
  GeomVertexWriter normals(vdata, "normal");
  GeomVertexWriter colors(vdata, "color");

  vertices.add_data3(-20, -20, 0);
  vertices.add_data3(20, -20, 0);
  vertices.add_data3(-20, 20, 0);
  vertices.add_data3(20, 20, 0);

  normals.add_data3(0, 0, 1);
  normals.add_data3(0, 0, 1);
  normals.add_data3(0, 0, 1);
  normals.add_data3(0, 0, 1);

  colors.add_data4(1, 0, 0, 1);
  colors.add_data4(1, 1, 0, 1);
  colors.add_data4(0, 1, 0, 1);
  colors.add_data4(0, 0, 1, 1);

  PT(GeomTriangles) primitive;
  primitive = new GeomTriangles(Geom::UH_static);
  primitive->add_vertices(0, 1, 2);
  primitive->add_vertices(2, 1, 3);
  primitive->close_primitive();

  PT(Geom) geom;
  geom = new Geom(vdata);
  geom->add_primitive(primitive);

  PT(GeomNode) node;
  node = new GeomNode("gnode");
  node->add_geom(geom);

  NodePath geomScene = window->get_render().attach_new_node(node);
  PT(Shader) shader = Shader::load(Shader::SL_GLSL, "simple.vert", "simple.frag");
  geomScene.set_shader(shader);
  geomScene.set_scale(1, 1, 1);
  geomScene.set_pos(0, 0, 0);

  NodePath camera = window->get_camera_group();
  camera.set_pos(0, 0, 80);
  camera.set_hpr(0, -90, 0);

  framework.main_loop();
  framework.close_framework();
  return (0);
}
