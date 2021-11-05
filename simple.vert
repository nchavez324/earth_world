#version 140

// Uniform inputs
uniform mat4 p3d_ModelViewProjectionMatrix;

// Vertex inputs
in vec4 p3d_Vertex;
in vec4 p3d_Color;

// Output to fragment shader
out vec4 v_Position;
out vec4 v_Color;

void main() {
  gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
  v_Position = p3d_Vertex;
  v_Color = p3d_Color;
}
