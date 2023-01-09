#version 400

// Definition of the geometry shader
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

// Uniform (global) buffer
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Attributes passed to the fragment shader
out vec2 uv;
out vec3 normal;

out float dist;

void createVertex(vec3 offset) {
	vec4 actualOffset = vec4(offset, 0.0);
	vec4 worldPosition = gl_in[0].gl_Position + actualOffset;

	vec4 viewWorld = view_mat * worldPosition;

	dist = length(viewWorld.xyz);

	gl_Position = projection_mat * viewWorld;

	EmitVertex();
}

void main() {
	// Create the 5 faces of the hedge cube (not the bottom face)

	uv = vec2(0.0, 0.0);
	normal = vec3(0.0, 0.0, 1.0);
	createVertex(vec3(-1.0, 1.0, 1.0));
	uv = vec2(1.0, 0.0);
	normal = vec3(0.0, 0.0, 1.0);
	createVertex(vec3(-1.0, -1.0, 1.0));
	uv = vec2(0.0, 1.0);
	normal = vec3(0.0, 0.0, 1.0);
	createVertex(vec3(1.0, 1.0, 1.0));
	uv = vec2(1.0, 1.0);
	normal = vec3(0.0, 0.0, 1.0);
	createVertex(vec3(1.0, -1.0, 1.0));
	
	EndPrimitive();
	
	uv = vec2(0.0, 0.0);
	normal = vec3(1.0, 0.0, 0.0);
	createVertex(vec3(1.0, 1.0, 1.0));
	uv = vec2(1.0, 0.0);
	normal = vec3(1.0, 0.0, 0.0);
	createVertex(vec3(1.0, -1.0, 1.0));
	uv = vec2(0.0, 1.0);
	normal = vec3(1.0, 0.0, 0.0);
	createVertex(vec3(1.0, 1.0, -1.0));
	uv = vec2(1.0, 1.0);
	normal = vec3(1.0, 0.0, 0.0);
	createVertex(vec3(1.0, -1.0, -1.0));
	
	EndPrimitive();
	
	uv = vec2(0.0, 0.0);
	normal = vec3(0.0, 0.0, -1.0);
	createVertex(vec3(1.0, 1.0, -1.0));
	uv = vec2(1.0, 0.0);
	normal = vec3(0.0, 0.0, -1.0);
	createVertex(vec3(1.0, -1.0, -1.0));
	uv = vec2(0.0, 1.0);
	normal = vec3(0.0, 0.0, -1.0);
	createVertex(vec3(-1.0, 1.0, -1.0));
	uv = vec2(1.0, 1.0);
	normal = vec3(0.0, 0.0, -1.0);
	createVertex(vec3(-1.0, -1.0, -1.0));
	
	EndPrimitive();
	
	uv = vec2(0.0, 0.0);
	normal = vec3(-1.0, 0.0, 0.0);
	createVertex(vec3(-1.0, 1.0, -1.0));
	uv = vec2(1.0, 0.0);
	normal = vec3(-1.0, 0.0, 0.0);
	createVertex(vec3(-1.0, -1.0, -1.0));
	uv = vec2(0.0, 1.0);
	normal = vec3(-1.0, 0.0, 0.0);
	createVertex(vec3(-1.0, 1.0, 1.0));
	uv = vec2(1.0, 1.0);
	normal = vec3(-1.0, 0.0, 0.0);
	createVertex(vec3(-1.0, -1.0, 1.0));
	
	EndPrimitive();
	
	uv = vec2(0.0, 0.0);
	normal = vec3(0.0, 1.0, 0.0);
	createVertex(vec3(1.0, 1.0, 1.0));
	uv = vec2(1.0, 0.0);
	normal = vec3(0.0, 1.0, 0.0);
	createVertex(vec3(1.0, 1.0, -1.0));
	uv = vec2(0.0, 1.0);
	normal = vec3(0.0, 1.0, 0.0);
	createVertex(vec3(-1.0, 1.0, 1.0));
	uv = vec2(1.0, 1.0);
	normal = vec3(0.0, 1.0, 0.0);
	createVertex(vec3(-1.0, 1.0, -1.0));
	
	EndPrimitive();
}