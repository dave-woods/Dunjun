#version 120

attribute vec2 a_position;
attribute vec3 a_color;
attribute vec2 a_texCoord;

varying vec3 v_color;
varying vec2 v_texCoord;

uniform mat4 u_camera;
uniform mat4 u_model;

void main()
{
	v_color = a_color;
	v_texCoord = a_texCoord.st;
	gl_Position = u_camera * u_model * vec4(a_position, 0.0, 1.0);
}
