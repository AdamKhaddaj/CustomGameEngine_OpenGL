#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 uv_interp;

in float dist_;

// Uniform (global) buffer
uniform sampler2D texture_map;

uniform vec3 fogColor;
uniform float fogDensity;
uniform float fogFactor;

void main() {
	// Apply raindrop texture
	vec4 pixel = texture(texture_map, uv_interp);

	gl_FragColor = mix(vec4(fogColor, 0.0), pixel * frag_color, clamp(exp(-pow((dist_ * fogDensity), fogFactor)), 0.0, 1.0));
}