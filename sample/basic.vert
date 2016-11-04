#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

// Input:
attribute vec3 EW_coord3d;
attribute vec4 EW_color;
uniform mat4 EW_MatrixTransformation;

// output:
varying vec4 f_color;

void main(void) {
	gl_Position = EW_MatrixTransformation * vec4(EW_coord3d, 1.0);
	f_color = EW_color;
}
