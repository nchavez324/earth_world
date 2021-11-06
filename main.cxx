#include "panda3d/boundingBox.h"
#include "panda3d/computeNode.h"
#include "panda3d/geomLines.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/load_prc_file.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"

template <typename T>
using PT = PointerTo<T>;
template <typename T>
using CPT = ConstPointerTo<T>;

NodePath generateGlobeNode(GraphicsWindow *window, int verticesPerEdge);
NodePath generateAxesNode();

NodePath generateGlobeNode(GraphicsWindow *window, int verticesPerEdge) {
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

  // I ended up making it a vec4 anyway, since I'm sending other data with the
  // vertex position, but note the above!!
  verticesPerEdge = std::max(verticesPerEdge, 2);
  int faceCount = 6;
  int verticesPerFace = verticesPerEdge * verticesPerEdge;
  int vertexCount = verticesPerFace * faceCount;
  uint64_t bufferSize = sizeof(float) * 4 * static_cast<uint64_t>(vertexCount);
  // Pad to 16 bytes, based on advice in panda3d/shaderBuffer.i.
  if ((bufferSize & 15u) != 0) {
    bufferSize = ((bufferSize + 15u) & ~15u);
  }
  std::cout << "bufferSize: " << bufferSize << std::endl;

  PT<ShaderBuffer> vertexBuffer =
      new ShaderBuffer("positions", bufferSize, Geom::UH_static);
  PT<GeomVertexData> vertexData = new GeomVertexData(
      "Globe", GeomVertexFormat::get_empty(), Geom::UH_static);
  PT<GeomTriangles> triangles = new GeomTriangles(Geom::UH_static);
  for (int face = 0; face < faceCount; face++) {
    for (int vertexX = 0; vertexX < verticesPerEdge - 1; vertexX++) {
      for (int vertexY = 0; vertexY < verticesPerEdge - 1; vertexY++) {
        int v0 =
            (verticesPerFace * face) + (verticesPerEdge * vertexY) + vertexX;
        int v1 = v0 + 1;
        int v2 = v0 + verticesPerEdge;
        int v3 = v0 + verticesPerEdge + 1;
        triangles->add_vertices(v2, v1, v0);
        triangles->add_vertices(v3, v1, v2);
      }
    }
  }

  PT<Geom> geom = new Geom(vertexData);
  geom->add_primitive(triangles);
  PT<BoundingBox> bounds =
      new BoundingBox(LPoint3(-2, -2, -2), LPoint3(2, 2, 2));
  geom->set_bounds(bounds);
  PT<GeomNode> node = new GeomNode("Globe");
  node->add_geom(geom);
  node->set_bounds_type(BoundingVolume::BT_box);

  PT<Texture> texture = TexturePool::load_texture(
      Filename("../../../../Downloads/topology_2048x1024.png"));
  texture->set_wrap_u(SamplerState::WrapMode::WM_repeat);

  PT<Shader> materialShader =
      Shader::load(Shader::SL_GLSL, "../../simple.vert", "../../simple.frag");
  NodePath globe(node);
  globe.set_shader(materialShader);
  globe.set_texture(texture);
  globe.set_shader_input("VertexBuffer", vertexBuffer);

  PT<Shader> computeShader =
      Shader::load_compute(Shader::SL_GLSL, "../../compute.comp");
  NodePath compute("compute");
  compute.set_shader(computeShader);
  compute.set_shader_input("VerticesPerEdge", LVecBase2i(verticesPerEdge, 0));
  compute.set_shader_input("VertexBuffer", vertexBuffer);
  compute.set_shader_input("HeightmapTex", texture);
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib, compute.get_attrib(ShaderAttrib::get_class_type()));

  LVecBase3i work_groups(verticesPerEdge, verticesPerEdge, faceCount);
  GraphicsEngine *engine = GraphicsEngine::get_global_ptr();
  engine->dispatch_compute(work_groups, attributes, window->get_gsg());

  return globe;
}

NodePath generateAxesNode() {
  PT<GeomLines> lines = new GeomLines(Geom::UH_static);
  PT<GeomVertexData> vdata =
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

  PT<Geom> geom = new Geom(vdata);
  geom->add_primitive(lines);
  PT<GeomNode> node = new GeomNode("Axes");
  node->add_geom(geom);
  NodePath nodePath(node);
  nodePath.set_render_mode_thickness(2);
  return nodePath;
}

int main(int argc, char *argv[]) {
  load_prc_file("../../config.prc");
  PandaFramework framework;
  framework.open_framework(argc, argv);
  framework.set_window_title("My Panda3D Window");
  WindowFramework *const window = framework.open_window();

  NodePath axes = generateAxesNode();
  axes.reparent_to(window->get_render());
  axes.set_scale(10);

  NodePath globe = generateGlobeNode(window->get_graphics_window(),
                                     /* verticesPerEdge= */ 250);
  globe.reparent_to(window->get_render());
  globe.set_scale(5);

  window->setup_trackball();

  framework.main_loop();
  framework.close_framework();
  return (0);
}
