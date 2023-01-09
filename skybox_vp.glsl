#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Going to fragment shader
out vec3 uvcoords;

void main() {
	vec4 pos = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

	gl_Position = pos.xyww;

	uvcoords = vertex;
}