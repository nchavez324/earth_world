#version 140

uniform sampler2D p3d_Texture0;

// Input from vertex shader
in vec4 v_Color;
in vec2 v_TexCoord;

void main() {
  // gl_FragColor = v_Color;
  gl_FragColor = texture(p3d_Texture0, v_TexCoord);
}