#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;

// Uniform (global) buffer
uniform sampler2D texture_map;

void main() {
	gl_FragColor = texture(texture_map, frag_color.xy) * vec4(frag_color.x * 1.1, frag_color.y, 0.2, 1.0);
}