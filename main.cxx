#include "panda3d/asyncTaskManager.h"
#include "panda3d/boundingBox.h"
#include "panda3d/clockObject.h"
#include "panda3d/computeNode.h"
#include "panda3d/geomLines.h"
#include "panda3d/geomTriangles.h"
#include "panda3d/load_prc_file.h"
#include "panda3d/pStatClient.h"
#include "panda3d/pandaFramework.h"
#include "panda3d/pandaSystem.h"
#include "panda3d/shader.h"
#include "panda3d/texturePool.h"

template <typename T>
using PT = PointerTo<T>;
template <typename T>
using CPT = ConstPointerTo<T>;

std::uintptr_t const KEY_PRESS_UP = 1;
std::uintptr_t const KEY_RELEASE_UP = 2;
std::uintptr_t const KEY_PRESS_DOWN = 3;
std::uintptr_t const KEY_RELEASE_DOWN = 4;
std::uintptr_t const KEY_PRESS_RIGHT = 5;
std::uintptr_t const KEY_RELEASE_RIGHT = 6;
std::uintptr_t const KEY_PRESS_LEFT = 7;
std::uintptr_t const KEY_RELEASE_LEFT = 8;

int const kGlobeVerticesPerEdge = 100;
PN_stdfloat const kAxesScale = 40.f;
PN_stdfloat const kGlobeScale = 20.f;
PN_stdfloat const kGlobeWaterSurfaceHeight = 0.95f;
PN_stdfloat const kBoatScale = 0.05f;
PN_stdfloat const kBoatSpeed = 0.07f;
PN_stdfloat const kCameraDistance = 6.f;

LVector2 g_inputAxis = LVector2::zero();
LPoint3 g_boatSphericalCoords(0, 0, 1);
PN_stdfloat g_boatHeading = 0.f;
NodePath g_boat;
NodePath g_camera;
ClockObject *g_clock = ClockObject::get_global_clock();
WindowFramework *g_window = nullptr;

PN_stdfloat clamp(PN_stdfloat value, PN_stdfloat min, PN_stdfloat max);
LQuaternion quatFromOrthonormalMatrix(const LMatrix3 &a);
LQuaternion rotationBetweenVectors(const LVector3 &v1, const LVector3 &v2);
LQuaternion rotationLookAt(const LVector3 &forward, const LVector3 &up);
LPoint3 sphericalCoordsFromCartesian(const LPoint3 &coords);
LPoint3 cartesianCoordsFromSpherical(const LPoint3 &coords);
NodePath generateGlobeNode(GraphicsWindow *window, int verticesPerEdge);
NodePath generateAxesNode();
AsyncTask::DoneStatus onFrameTask(GenericAsyncTask *task, void *_);
void onKeyChanged(const Event *event, void *_);

PN_stdfloat clamp(PN_stdfloat value, PN_stdfloat min, PN_stdfloat max) {
  return std::max(min, std::min(max, value));
}

LQuaternion quatFromOrthonormalMatrix(const LMatrix3 &a) {
  PN_stdfloat trace = a[0][0] + a[1][1] + a[2][2];
  if (trace > 0) {
    PN_stdfloat s = 0.5f / sqrtf(trace + 1.f);
    return LQuaternion(0.25f / s, (a[2][1] - a[1][2]) * s,
                       (a[0][2] - a[2][0]) * s, (a[1][0] - a[0][1]) * s);
  } else {
    if (a[0][0] > a[1][1] && a[0][0] > a[2][2]) {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[0][0] - a[1][1] - a[2][2]);
      return LQuaternion((a[2][1] - a[1][2]) / s, 0.25f * s,
                         (a[0][1] + a[1][0]) / s, (a[0][2] + a[2][0]) / s);
    } else if (a[1][1] > a[2][2]) {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[1][1] - a[0][0] - a[2][2]);
      return LQuaternion((a[0][2] - a[2][0]) / s, (a[0][1] + a[1][0]) / s,
                         0.25f * s, (a[1][2] + a[2][1]) / s);
    } else {
      PN_stdfloat s = 2.f * sqrtf(1.f + a[2][2] - a[0][0] - a[1][1]);
      return LQuaternion((a[1][0] - a[0][1]) / s, (a[0][2] + a[2][0]) / s,
                         (a[1][2] + a[2][1]) / s, 0.25f * s);
    }
  }
}

LQuaternion rotationBetweenVectors(const LVector3 &v1, const LVector3 &v2) {
  // From Sam Hocevar's article "Quaternion from two vectors: the final version"
  PN_stdfloat a = sqrtf(v1.length_squared() * v2.length_squared());
  PN_stdfloat b = a + v1.dot(v2);
  if (IS_NEARLY_EQUAL(b, 2 * a) || IS_NEARLY_ZERO(a)) {
    return LQuaternion::ident_quat();
  }
  LVector3 axis(0);
  if (b < 1e-06 * a) {
    b = 0;
    if (abs(v1.get_x()) > abs(v1.get_z())) {
      axis = LVector3(-v1.get_y(), v1.get_x(), 0);
    } else {
      axis = LVector3(0, -v1.get_z(), v1.get_y());
    }
  } else {
    axis = v1.cross(v2);
  }
  LQuaternion rotation(b, axis.get_x(), axis.get_y(), axis.get_z());
  rotation.normalize();
  return rotation;
}

/**
 * Returns a rotation such that forward (+Y) rotates to the given forward, and
 * right (+X) rotates to the cross product of the given forward and up.
 */
LQuaternion rotationLookAt(const LVector3 &newForward, const LVector3 &upIsh) {
  // Create orthonormal transformation.
  LVector3 newRight = newForward.cross(upIsh);
  newRight.normalize();
  LVector3 newUp = newRight.cross(newForward);
  newUp.normalize();
  LMatrix3 transformation;
  transformation.set_col(0, newRight);
  transformation.set_col(1, newForward.normalized());
  transformation.set_col(2, newUp);
  // Convert into a Quaternion.
  return quatFromOrthonormalMatrix(transformation);
}

/**
 * Returns a spherical coordinate in radians, where the first/x component is the
 * azimuthal angle [0, 2PI] and the second/y component is the polar angle
 * [-PI/2,PI/2] (both in radians), and the third/z component is the radius.
 */
LPoint3 sphericalCoordsFromCartesian(const LPoint3 &coords) {
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
  return LPoint3(azimuth, polar, coords.length());
}

LPoint3 cartesianCoordsFromSpherical(const LPoint3 &coords) {
  // x is azimuth [0,2PI], y is polar [-PI/2,PI/2], z is radius.
  return LPoint3(coords.get_z() * cosf(coords.get_y()) * cosf(coords.get_x()),
                 coords.get_z() * cosf(coords.get_y()) * sinf(coords.get_x()),
                 coords.get_z() * sinf(coords.get_y()));
}

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
      new BoundingBox(LPoint3(-1, -1, -1), LPoint3(1, 1, 1));
  geom->set_bounds(bounds);
  PT<GeomNode> node = new GeomNode("Globe");
  node->add_geom(geom);
  node->set_bounds_type(BoundingVolume::BT_box);

  PT<Texture> topologyTexture = TexturePool::load_texture(
      "../../../../Downloads/topology/topology_2048x1024.png");
  PT<Texture> bathymetryTexture = TexturePool::load_texture(
      "../../../../Downloads/bathymetry/bathymetry_2048x1024.png");
  PT<Texture> albedoTexture = TexturePool::load_texture(
      "../../../../Downloads/albedo/1_2048x1024.png");
  PT(TextureStage) topologyStage = new TextureStage("TopologyStage");
  PT(TextureStage) bathymetryStage = new TextureStage("BathymetryStage");
  PT(TextureStage) albedoStage = new TextureStage("AlbedoStage");
  topologyTexture->set_wrap_u(SamplerState::WM_repeat);
  bathymetryTexture->set_wrap_u(SamplerState::WM_repeat);
  albedoTexture->set_wrap_u(SamplerState::WM_repeat);

  PT<Shader> materialShader =
      Shader::load(Shader::SL_GLSL, "../../simple.vert", "../../simple.frag");
  NodePath globe(node);
  globe.set_shader(materialShader);
  globe.set_texture(topologyStage, topologyTexture, /* priority= */ 0);
  globe.set_texture(bathymetryStage, bathymetryTexture, /* priority= */ 1);
  globe.set_texture(albedoStage, albedoTexture, /* priority= */ 2);
  globe.set_shader_input("VertexBuffer", vertexBuffer);

  PT<Shader> computeShader =
      Shader::load_compute(Shader::SL_GLSL, "../../compute.comp");
  NodePath compute("compute");
  compute.set_shader(computeShader);
  compute.set_shader_input("VerticesPerEdge", LVector2i(verticesPerEdge, 0));
  compute.set_shader_input("VertexBuffer", vertexBuffer);
  compute.set_shader_input("TopologyTex", topologyTexture);
  compute.set_shader_input("BathymetryTex", bathymetryTexture);
  CPT<ShaderAttrib> attributes =
      DCAST(ShaderAttrib, compute.get_attrib(ShaderAttrib::get_class_type()));

  LVector3i work_groups(verticesPerEdge, verticesPerEdge, faceCount);
  GraphicsEngine *engine = GraphicsEngine::get_global_ptr();
  engine->dispatch_compute(work_groups, attributes, window->get_gsg());

  return globe;
}

NodePath generateAxesNode() {
  PT<GeomLines> lines = new GeomLines(Geom::UH_static);
  PT<GeomVertexData> vdata =
      new GeomVertexData("Axes", GeomVertexFormat::get_v3c4(), Geom::UH_static);
  vdata->set_num_rows(12);
  GeomVertexWriter vertices(vdata, "vertex");
  GeomVertexWriter colors(vdata, "color");

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

  PT<Geom> geom = new Geom(vdata);
  geom->add_primitive(lines);
  PT<GeomNode> node = new GeomNode("Axes");
  node->add_geom(geom);
  NodePath nodePath(node);
  nodePath.set_render_mode_thickness(2);
  return nodePath;
}

AsyncTask::DoneStatus onFrameTask(GenericAsyncTask *task, void *_) {
  // 1. Determine the velocity, by using the input in the camera's basis.
  CPT<TransformState> cameraXform = g_camera.get_net_transform();
  LVector3 cameraRight = cameraXform->get_quat().get_right();
  LVector3 cameraUp = cameraXform->get_quat().get_up();
  LVector3 cameraBack = -cameraXform->get_quat().get_forward();
  LVector2 normalizedInput = g_inputAxis.normalized();
  LVector3 heading =
      (cameraRight * g_inputAxis.get_x()) + (cameraUp * g_inputAxis.get_y());
  heading.normalize();
  LVector3 positionDelta = heading * kBoatSpeed * g_clock->get_dt();

  // 2. Update the boat's position in cartesian space and snap onto sphere.
  LVector3 newBoatUnitPosition =
      (cartesianCoordsFromSpherical(g_boatSphericalCoords) + positionDelta)
          .normalized();
  LVector3 newBoatUnitSphericalPosition =
      sphericalCoordsFromCartesian(newBoatUnitPosition);
  g_boatSphericalCoords = newBoatUnitSphericalPosition;

  // 4. Convert it into the world position.
  LVector3 newBoatPosition =
      kGlobeWaterSurfaceHeight * kGlobeScale * newBoatUnitPosition;
  g_boat.set_pos(newBoatPosition);

  // 5. Update the heading if the input was non zero.
  if (!IS_NEARLY_ZERO(g_inputAxis.get_x()) ||
      !IS_NEARLY_ZERO(g_inputAxis.get_y())) {
    PN_stdfloat newHeading =
        atan2f(normalizedInput.get_y(), normalizedInput.get_x());
    if (newHeading < 0) {
      newHeading += 2 * MathNumbers::pi;
    }
    g_boatHeading = newHeading;
  }

  // 6. Transform the heading into a world rotation.
  LVector3 boatRight = LVector3::up().cross(newBoatUnitPosition);
  LVector3 boatUp = newBoatUnitPosition.cross(boatRight);
  LVector3 boatHeading =
      (cosf(g_boatHeading) * boatRight) + (sinf(g_boatHeading) * boatUp);
  boatHeading.normalize();
  LQuaternion newRotation = rotationLookAt(boatHeading, newBoatUnitPosition);
  g_boat.set_quat(newRotation);

  // 7. Float the camera above the boat and look at it.
  LVector3 newCameraPosition =
      newBoatPosition + (kCameraDistance * newBoatUnitPosition);
  LQuaternion newCameraRotation =
      rotationLookAt(-newBoatUnitPosition, LVector3::up());
  g_camera.set_pos(newCameraPosition);
  g_camera.set_quat(newCameraRotation);

  return AsyncTask::DoneStatus::DS_cont;
}

void onKeyChanged(const Event *event, void *userData) {
  std::uintptr_t keyEvent = reinterpret_cast<std::uintptr_t>(userData);
  switch (keyEvent) {
    case KEY_PRESS_UP:
    case KEY_RELEASE_DOWN:
      g_inputAxis.set_y(std::min(1.f, g_inputAxis.get_y() + 1));
      break;
    case KEY_PRESS_DOWN:
    case KEY_RELEASE_UP:
      g_inputAxis.set_y(std::max(-1.f, g_inputAxis.get_y() - 1));
      break;
    case KEY_PRESS_RIGHT:
    case KEY_RELEASE_LEFT:
      g_inputAxis.set_x(std::min(1.f, g_inputAxis.get_x() + 1));
      break;
    case KEY_PRESS_LEFT:
    case KEY_RELEASE_RIGHT:
      g_inputAxis.set_x(std::max(-1.f, g_inputAxis.get_x() - 1));
      break;
    default:
      return;
  }
}

int main(int argc, char *argv[]) {
  load_prc_file("../../config.prc");
  if (PStatClient::is_connected()) {
    PStatClient::disconnect();
  }
  if (!PStatClient::connect()) {
    std::cout << "Could not connect to PStat server." << std::endl;
  }

  PandaFramework framework;
  framework.open_framework(argc, argv);

  WindowProperties windowProperties;
  windowProperties.set_title("Earth World");
  windowProperties.set_size(LVector2i(800, 600));
  windowProperties.set_fixed_size(false);
  int flags = GraphicsPipe::BF_require_window;
  g_window = framework.open_window(windowProperties, flags);
  g_window->enable_keyboard();

  NodePath axes = generateAxesNode();
  axes.reparent_to(g_window->get_render());
  axes.set_scale(kAxesScale);

  NodePath globe =
      generateGlobeNode(g_window->get_graphics_window(),
                        /* verticesPerEdge= */ kGlobeVerticesPerEdge);
  globe.reparent_to(g_window->get_render());
  globe.set_scale(kGlobeScale);

  g_boat = g_window->load_model(framework.get_models(),
                                "../../../../Downloads/boat/S_Boat.bam");
  g_boat.reparent_to(g_window->get_render());
  g_boat.set_scale(kBoatScale);

  g_camera = g_window->get_camera_group();
  g_camera.set_pos((kGlobeScale + kCameraDistance) * LVector3::right());
  g_camera.set_quat(rotationLookAt(LVector3::left(), LVector3::up()));

  PT<GenericAsyncTask> frameTask =
      new GenericAsyncTask("Frame", &onFrameTask, /* userData= */ nullptr);
  framework.get_task_mgr().add(frameTask);

  framework.define_key("w", "Up pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_UP));
  framework.define_key("w-up", "Up released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_UP));
  framework.define_key("s", "Down pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_DOWN));
  framework.define_key("s-up", "Down released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_DOWN));
  framework.define_key("d", "Right pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_RIGHT));
  framework.define_key("d-up", "Right released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_RIGHT));
  framework.define_key("a", "Left pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_LEFT));
  framework.define_key("a-up", "Left released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_LEFT));
  framework.define_key("arrow_up", "Up pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_UP));
  framework.define_key("arrow_up-up", "Up released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_UP));
  framework.define_key("arrow_down", "Down pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_DOWN));
  framework.define_key("arrow_down-up", "Down released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_DOWN));
  framework.define_key("arrow_right", "Right pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_RIGHT));
  framework.define_key("arrow_right-up", "Right released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_RIGHT));
  framework.define_key("arrow_left", "Left pressed", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_PRESS_LEFT));
  framework.define_key("arrow_left-up", "Left released", &onKeyChanged,
                       reinterpret_cast<void *>(KEY_RELEASE_LEFT));

  framework.main_loop();
  framework.close_framework();
  return (0);
}
