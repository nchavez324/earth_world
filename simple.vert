#version 140

// Uniform inputs
uniform mat4 p3d_ModelViewProjectionMatrix;

// Vertex inputs
in vec4 p3d_Vertex;
in vec4 p3d_Color;
in vec2 p3d_MultiTexCoord0;

// Output to fragment shader
out vec4 v_Color;
out vec2 v_TexCoord;

void main() {
  gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
  v_Color = p3d_Color;
  v_TexCoord = p3d_MultiTexCoord0;
}
