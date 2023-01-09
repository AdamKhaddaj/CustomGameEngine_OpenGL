#version 400

#define PI 3.1415926538

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
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.25; // Gravity
float speed = 0.25; // Controls the speed of the snow

void main() {
	float time = mod(timer * 0.1 + (color.x / 100.0), 8 * PI);

	// Let's first work in model space (apply only world matrix)
	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 0.0);

	// portal movement equations
	position.x += cos(time * 10.0);
	position.y += cos(time * 50.0) * 0.02;
	position.z += sin(time * 10.0);

	// Now apply view transformation
	gl_Position = view_mat * position;

	// Define outputs
	// Define color of vertex
	vertex_color = vec3(1.0, 1.0, 1.0);

	// Forward time step to geometry shader
	timestep = time;
}