#version 400 core

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

// Input:
in vec3 in_position;
in vec4 in_colors;
uniform mat4 in_matrixTransformation;
uniform mat4 in_matrixProjection;
uniform mat4 in_matrixView;

// output:
out vec4 io_color;

void main(void) {
	gl_Position = in_matrixProjection * in_matrixView * in_matrixTransformation * vec4(in_position, 1.0);
	f_color = in_colors;
}
