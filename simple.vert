#version 430

// Uniform inputs
uniform mat4 p3d_ModelViewProjectionMatrix;

// Vertex inputs
layout(std430) buffer VertexBuffer {
  vec4 positions[];
};

// Output to fragment shader
out vec4 v_Position;
out float v_Height;

void main() {
  vec4 modelPosition = vec4(positions[gl_VertexID].xyz, 1);
  gl_Position = p3d_ModelViewProjectionMatrix * modelPosition;
  v_Position = modelPosition;
  v_Height = positions[gl_VertexID].w;
}
