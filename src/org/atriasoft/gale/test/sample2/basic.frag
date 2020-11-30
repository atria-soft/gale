#version 400 core

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

in vec2 io_textureCoords;

uniform sampler2D in_textureBase;

// output:
out vec4 out_Color;

void main(void) {
	out_Color = texture(in_textureBase, io_textureCoords);
}
