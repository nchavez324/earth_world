#version 430

// Uniform inputs
uniform mat4 p3d_ModelViewMatrix;
uniform mat4 p3d_ModelViewProjectionMatrix;

// Vertex inputs
layout(std430) buffer u_VertexBuffer {
  vec3 positions[];
};

// Output to fragment shader
out vec4 v_ViewPosition;
out vec4 v_Position;

void main() {
  vec4 modelPosition = vec4(positions[gl_VertexID], 1);
  gl_Position = p3d_ModelViewProjectionMatrix * modelPosition;
  v_ViewPosition = p3d_ModelViewMatrix * modelPosition;
  v_Position = modelPosition;
}
