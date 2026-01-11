#version 400 core

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

// Input :
layout (location = 0) in vec3 in_position;
uniform vec4 in_colors;
uniform mat4 in_matrixTransformation;
uniform mat4 in_matrixProjection;
uniform mat4 in_matrixView;

// output :
out vec4 io_color;

void main(void) {
	gl_Position = in_matrixProjection * in_matrixView * in_matrixTransformation * vec4(in_position, 1.0);
	io_color = vec4(in_colors.xyz, 0.3);
}
