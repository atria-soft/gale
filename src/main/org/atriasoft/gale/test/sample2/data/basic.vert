#version 400 core

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

// Input:
in vec3 in_position;
in vec2 in_textureCoords;
uniform mat4 in_matrixTransformation;
uniform mat4 in_matrixProjection;
uniform mat4 in_matrixView;

// output:
out vec2 io_textureCoords;

void main(void) {
	gl_Position = in_matrixProjection * in_matrixView * in_matrixTransformation * vec4(in_position, 1.0);
	io_textureCoords = in_textureCoords;
}
