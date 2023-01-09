#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

in float dist;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float timer;

uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogFactor;

void main() {
	// Retrieve texture value
	vec4 pixel = texture(texture_map, uv_interp);

	float xx = uv_interp.x - 0.5;
	float yy = uv_interp.y - 0.5;

	float r2 = xx * xx + yy * yy;
	float theta = atan(xx, yy);
	theta = theta / (6.28); // remap to 0-1 range
	
	pixel = texture(texture_map, vec2(r2 - timer * 0.02, theta - timer * 0.02));
	
	r2 += pixel.g * 0.2;

	// Convert to color
	float red = 0.0;
	float green = 1.1 - 1.2 * sqrt(r2 * 4.15);
	float blue = 1.5 - r2 * 4;
	
	vec4 outcol = vec4(red, green, blue, 1.0);

	gl_FragColor = mix(vec4(fogColor, 0.0), outcol, clamp(exp(-pow((dist * fogDensity), fogFactor)), 0.0, 1.0));
}