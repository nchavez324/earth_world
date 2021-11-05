#include "panda3d/computeNode.h"
#include "panda3d/geomLines.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/load_prc_file.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"

LVecBase3 sphericalCoordsFromCartesian(const LVecBase3 &coords);
void generateFace(PT(GeomTriangles) triangles, GeomVertexWriter &vertices,
                  GeomVertexWriter &colors, int verticesPerEdge,
                  const LVecBase3 &normal, int vertexOffset);
const PT(GeomNode) generateGlobeNode(int verticesPerEdge);
const PT(GeomNode) generateAxesNode();

/**
 * Returns a spherical coordinate in radians, where the first/x component is the
 * azimuthal angle [0, 2PI] and the second/y component is the polar angle
 * [-PI/2,PI/2] (both in radians), and the third/z component is the radius.
 */
LVecBase3 sphericalCoordsFromCartesian(const LVecBase3 &coords) {
  // In Panda3D, coordinate system is right handed, with Z as up.
  // Convention:
  //   - +X axis is azimuth = 0, increasing clockwise
  //   - XY plane is polar = 0, increasing towards +Z, decreasing towards -Z
  PN_stdfloat azimuth = 0;
  if (coords.get_x() != 0 || coords.get_y() != 0) {
    // Negate inputs so after addition, 0 is +X and clockwise
    // Returns [-PI,PI], so add PI to move to [0,2PI]
    azimuth = atan2f(-coords.get_y(), -coords.get_x()) + MathNumbers::pi;
  }
  PN_stdfloat xyLength = coords.get_xy().length();
  PN_stdfloat polar = 0;
  if (xyLength != 0 || coords.get_z() != 0) {
    polar = atan2f(coords.get_z(), xyLength);
  }
  return LVecBase3(azimuth, polar, coords.length());
}

void generateFace(PT(GeomTriangles) triangles, GeomVertexWriter &vertices,
                  GeomVertexWriter &colors, int verticesPerEdge,
                  const LVecBase3 &normal, int vertexOffset) {
  PN_stdfloat patchSize = 2.0 / (verticesPerEdge - 1);
  LVecBase3 axis1 = LVecBase3(normal.get_y(), normal.get_z(), normal.get_x());
  LVecBase3 axis2 = axis1.cross(normal);
  LVecBase3 faceOrigin = normal - axis1 - axis2;

  for (int tick1 = 0; tick1 < verticesPerEdge; tick1++) {
    for (int tick2 = 0; tick2 < verticesPerEdge; tick2++) {
      int vertexIndex = (tick1 * verticesPerEdge) + tick2;
      LVecBase3 cubePoint = faceOrigin + (patchSize * tick1 * axis1) +
                            (patchSize * tick2 * axis2);
      PN_stdfloat x2 = powf(cubePoint.get_x(), 2),
                  y2 = powf(cubePoint.get_y(), 2),
                  z2 = powf(cubePoint.get_z(), 2);
      LVecBase3 spherePoint = LVecBase3(
          cubePoint.get_x() * sqrtf(1 - y2 / 2.0 - z2 / 2.0 + y2 * z2 / 3.0),
          cubePoint.get_y() * sqrtf(1 - z2 / 2.0 - x2 / 2.0 + z2 * x2 / 3.0),
          cubePoint.get_z() * sqrtf(1 - x2 / 2.0 - y2 / 2.0 + x2 * y2 / 3.0));
      vertices.add_data3(spherePoint);

      LVecBase3 sphericalCoords = sphericalCoordsFromCartesian(spherePoint);
      LVecBase2 uv = LVecBase2(
          (sphericalCoords.get_x()) / (2 * MathNumbers::pi),
          (sphericalCoords.get_y() + MathNumbers::pi / 2) / (MathNumbers::pi));
      colors.add_data4(uv.get_x(), uv.get_y(),
                       ((tick1 + tick2) % 2 == 0) ? 1 : 0, 1);

      if (tick1 < verticesPerEdge - 1 && tick2 < verticesPerEdge - 1) {
        triangles->add_vertices(vertexOffset + vertexIndex + 0,
                                vertexOffset + vertexIndex + 1,
                                vertexOffset + vertexIndex + verticesPerEdge);
        triangles->add_vertices(
            vertexOffset + vertexIndex + verticesPerEdge,
            vertexOffset + vertexIndex + 1,
            vertexOffset + vertexIndex + verticesPerEdge + 1);
      }
    }
  }
}

const PT(GeomNode) generateGlobeNode(int verticesPerEdge) {
  verticesPerEdge = std::max(verticesPerEdge, 2);
  int verticesPerFace = verticesPerEdge * verticesPerEdge;
  int vertexCount = verticesPerFace * 6;
  const PT(GeomTriangles) triangles = new GeomTriangles(Geom::UH_static);
  const PT(GeomVertexData) vdata = new GeomVertexData(
      "Globe", GeomVertexFormat::get_v3c4(), Geom::UH_static);
  vdata->set_num_rows(vertexCount);
  GeomVertexWriter vertices(vdata, "vertex");
  GeomVertexWriter colors(vdata, "color");

  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(+1, 0, 0), 0 * verticesPerFace);
  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(-1, 0, 0), 1 * verticesPerFace);
  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(0, +1, 0), 2 * verticesPerFace);
  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(0, -1, 0), 3 * verticesPerFace);
  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(0, 0, +1), 4 * verticesPerFace);
  generateFace(triangles, vertices, colors, verticesPerEdge,
               LVecBase3(0, 0, -1), 5 * verticesPerFace);
  triangles->close_primitive();

  const PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(triangles);

  const PT(GeomNode) node = new GeomNode("Globe");
  node->add_geom(geom);

  return node;
}

const PT(GeomNode) generateAxesNode() {
  const PT(GeomLines) lines = new GeomLines(Geom::UH_static);
  const PT(GeomVertexData) vdata =
      new GeomVertexData("Axes", GeomVertexFormat::get_v3c4(), Geom::UH_static);
  vdata->set_num_rows(6);
  GeomVertexWriter vertices(vdata, "vertex");
  GeomVertexWriter colors(vdata, "color");

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(1, 0, 0);
  colors.add_data4(1, 0, 0, 1);
  colors.add_data4(1, 0, 0, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, 1, 0);
  colors.add_data4(0, 1, 0, 1);
  colors.add_data4(0, 1, 0, 1);

  vertices.add_data3(0, 0, 0);
  vertices.add_data3(0, 0, 1);
  colors.add_data4(0, 0, 1, 1);
  colors.add_data4(0, 0, 1, 1);

  lines->add_vertices(0, 1);
  lines->add_vertices(2, 3);
  lines->add_vertices(4, 5);
  lines->close_primitive();

  const PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(lines);

  const PT(GeomNode) node = new GeomNode("Axes");
  node->add_geom(geom);

  return node;
}

int main(int argc, char *argv[]) {
  load_prc_file("../../config.prc");
  PandaFramework framework;
  framework.open_framework(argc, argv);
  framework.set_window_title("My Panda3D Window");
  WindowFramework *const window = framework.open_window();

  NodePath axes = window->get_render().attach_new_node(generateAxesNode());
  axes.set_scale(10);
  axes.set_render_mode_thickness(2);

  PT(Texture) = TexturePool::load_texture(
      Filename("../../../../Downloads/topology_16384x8192.png"));
  texture->set_wrap_u(SamplerState::WrapMode::WM_repeat);

  NodePath globe = window->get_render().attach_new_node(
      generateGlobeNode(/* verticesPerEdge= */ 10));
  globe.set_scale(5);
  const PT(Shader) materialShader =
      Shader::load(Shader::SL_GLSL, "../../simple.vert", "../../simple.frag");
  globe.set_shader(materialShader);
  globe.set_texture(texture);

  // const PT(Shader) computeShader = Shader::load_compute(
  //   Shader::SL_GLSL,
  //   "../../compute.glsl");
  // const PT(ComputeNode) computeNode = new ComputeNode("compute");
  // computeNode->add_dispatch(512 / 16, 512 / 16, 1);
  // NodePath compute = window->get_render().attach_new_node(computeNode);
  // compute.set_shader(computeShader);
  // compute.set_shader_input("fromTex", myTex1);
  // compute.set_shader_input("toTex", myTex2);

  window->setup_trackball();

  framework.main_loop();
  framework.close_framework();
  return (0);
}
