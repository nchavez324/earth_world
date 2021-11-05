#version 140

// Input from vertex shader
in vec4 v_Color;

void main() {
  gl_FragColor = v_Color;
}