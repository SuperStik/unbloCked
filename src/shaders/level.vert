#version 330
layout (location = 0) in vec3 aPos;
uniform mat4 proj;
uniform mat4 view;

void main() {
	vec4 campos = vec4(aPos, 1.0);
	gl_Position = proj * view * campos;
}
