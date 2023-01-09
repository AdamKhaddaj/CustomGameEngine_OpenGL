#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;

uniform float timer;

// Attributes forwarded to the geometry shader
out vec4 vertex_color;
out float timestep;

out float dist;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);

float grav = -2.0; // Gravity
float speed = 0.75; // Controls the speed of the fountain
float upward = 1.5; // Additional y velocity for all particles

void main() {
	vec4 viewWorld = view_mat * world_mat * vec4(vertex, 1.0);

	dist = length(viewWorld.xyz);

	// Calculate particle output
	float cycle_length = 1.5;
	float p = color.x * cycle_length;
	float t = mod((timer + p), cycle_length);
	
	// Let's first work in model space (apply only world matrix)
	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 0.0);

	// Using trajectory
	position.x += norm.x * t * speed + grav * speed * up_vec.x * t * t;
	position.y += upward + norm.y * t * speed + grav * speed * up_vec.y * t * t;
	position.z += norm.z * t * speed + grav * speed * up_vec.z * t * t;

	// Now apply view transformation
	gl_Position = view_mat * position;

	// Define outputs
	// Define color of vertex
	float transparency = 1.0 - (t / 5.0);
	vertex_color = vec4(1.0, 1.0, 1.0, transparency); // Water

	// Forward time step to geometry shader
	timestep = t;
}