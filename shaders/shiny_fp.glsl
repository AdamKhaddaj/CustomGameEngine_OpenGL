#version 400

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

in float dist;

// Uniform (global) buffer
uniform sampler2D texture_map;

uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogFactor;

uniform float timer;

const vec3 light = vec3(0.3, 1.2, 1.0);

void main() {
	// Retrieve texture value
	vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0) * length(texture(texture_map, uv_interp + vec2(0.0, timer * 0.1))) + vec4(0.0, 1.0, 0.0, 1.0) * 
	length(texture(texture_map, uv_interp + vec2(0.0, 1.0 + timer * 0.2)))
	+ vec4(0.0, 0.0, 1.0, 1.0) * length(texture(texture_map, uv_interp + vec2(0.0, 2.0 + timer * 0.3)));

	float diffuse = 0.6 * max(0.0, dot(normalize(normal_interp), normalize(light)));
	float amb = 0.4;

	gl_FragColor = mix(vec4(fogColor, 0.0), pixel * diffuse + pixel * amb, clamp(exp(-pow((dist * fogDensity), fogFactor)), 0.0, 1.0));
}