#version 400

// Vertex buffer for leaves
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.15; // Gravity
float speed = 1.5; // Controls the speed of leaves falling
float upward = 0.0; // additional y velocity for all particles

void main() {
	// set offset
	float offset = timer + mod(color[0], 100);
	float t = offset; // Our time parameter
	
	// Let's first work in model space (apply only world matrix)
	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 0.0);

	// use trig functions with interval to determine how the particles will orbit
	position.y += sin(t) * (mod(color[0], 3)) / 2;
	position.x += cos(t) * (mod(color[0] + 1, 3)) / 2;
	position.z += sin(t);
	
	// Now apply view transformation
	gl_Position = view_mat * position;

	// Forward time step to geometry shader
	timestep = t;
}