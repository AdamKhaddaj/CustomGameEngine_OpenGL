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

void main() {
	// Time cycle using the value stored at color.x as offset
	float offset = timer + mod(color[0], 15); // have each particle be offsetted by random number between 0 and 8
	float circtime = mod(offset, 25); // lifespan is random between 30 minus 0 to 8
	float t = circtime; // Our time parameter

	// Let's first work in model space (apply only world matrix)
	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 0.0);

	// for y component, have the leaf accelearte and slow down, like a leaf would fall
	position.y -= (t + (pow(sin(t / 1.2), 2))) * 0.30;
	
	// x component is found using derivative
	position.x += 0.30 * (1 + 1.666 * sin(0.415 * t) * cos(0.415 * t));

	// Now apply view transformation
	gl_Position = view_mat * position;

	vertex_color = vec3(0.5, 0.7, 0.0); // Green yellowish for leaves

	// Forward time step to geometry shader
	timestep = t;
}