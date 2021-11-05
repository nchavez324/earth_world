#include "panda3d/computeNode.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"

void generateFace(PT(GeomTriangles) triangles, GeomVertexWriter &vertices,
                  GeomVertexWriter &colors, GeomVertexWriter &texUVs,
                  int verticesPerEdge, const LVecBase3 &normal,
                  int vertexOffset);
const PT(GeomNode) generateGlobeNode(int verticesPerEdge);

void generateFace(PT(GeomTriangles) triangles, GeomVertexWriter &vertices,
                  GeomVertexWriter &colors, GeomVertexWriter &texUVs,
                  int verticesPerEdge, const LVecBase3 &normal,
                  int vertexOffset) {
  float patchSize = 2.0 / (verticesPerEdge - 1);
  LVecBase3 axis1 = LVecBase3(normal.get_y(), normal.get_z(), normal.get_x());
  LVecBase3 axis2 = axis1.cross(normal);
  LVecBase3 faceOrigin = normal - axis1 - axis2;

  for (int tick1 = 0; tick1 < verticesPerEdge; tick1++) {
    for (int tick2 = 0; tick2 < verticesPerEdge; tick2++) {
      int vertexIndex = (tick1 * verticesPerEdge) + tick2;
      LVecBase3 cubePoint = faceOrigin + (patchSize * tick1 * axis1) +
                            (patchSize * tick2 * axis2);
      float x2 = pow(cubePoint.get_x(), 2), y2 = pow(cubePoint.get_y(), 2),
            z2 = pow(cubePoint.get_z(), 2);
      LVecBase3 spherePoint = LVecBase3(
          cubePoint.get_x() * sqrt(1 - y2 / 2.0 - z2 / 2.0 + y2 * z2 / 3.0),
          cubePoint.get_y() * sqrt(1 - z2 / 2.0 - x2 / 2.0 + z2 * x2 / 3.0),
          cubePoint.get_z() * sqrt(1 - x2 / 2.0 - y2 / 2.0 + x2 * y2 / 3.0));
      vertices.add_data3(spherePoint);

      LVecBase2 uv = LVecBase2(
          static_cast<float>(tick1) / static_cast<float>(verticesPerEdge),
          static_cast<float>(tick2) / static_cast<float>(verticesPerEdge));
      colors.add_data4(uv.get_x(), uv.get_y(),
                       ((tick1 + tick2) % 2 == 0) ? 1 : 0, 1);
      texUVs.add_data2(uv.get_x(), uv.get_y());

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
      "Globe", GeomVertexFormat::get_v3c4t2(), Geom::UH_static);
  vdata->set_num_rows(vertexCount);
  GeomVertexWriter vertices(vdata, "vertex");
  GeomVertexWriter colors(vdata, "color");
  GeomVertexWriter texUVs(vdata, "texcoord");

  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(+1, 0, 0), 0 * verticesPerFace);
  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(-1, 0, 0), 1 * verticesPerFace);
  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(0, +1, 0), 2 * verticesPerFace);
  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(0, -1, 0), 3 * verticesPerFace);
  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(0, 0, +1), 4 * verticesPerFace);
  generateFace(triangles, vertices, colors, texUVs, verticesPerEdge,
               LVecBase3(0, 0, -1), 5 * verticesPerFace);
  triangles->close_primitive();

  const PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(triangles);

  const PT(GeomNode) node = new GeomNode("Globe");
  node->add_geom(geom);

  return node;
}

int main(int argc, char *argv[]) {
  PandaFramework framework;
  framework.open_framework(argc, argv);
  framework.set_window_title("My Panda3D Window");
  WindowFramework *const window = framework.open_window();

  PT(Texture)
  texture = TexturePool::load_texture(Filename("../../grid.png"));

  NodePath globe = window->get_render().attach_new_node(
      generateGlobeNode(/* verticesPerEdge= */ 5));
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
