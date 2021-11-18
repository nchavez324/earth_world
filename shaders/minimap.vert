#version 430

// Uniform inputs
uniform mat4 p3d_ModelViewProjectionMatrix;

// Vertex inputs
in vec4 p3d_Vertex;
in vec2 p3d_MultiTexCoord0;

// Output to fragment shader
out vec2 v_TexCoord0;

void main() {
  gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
  v_TexCoord0 = p3d_MultiTexCoord0;
}
