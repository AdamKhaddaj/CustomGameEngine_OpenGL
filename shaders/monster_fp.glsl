#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;

uniform sampler2D texture_map;
uniform float timer;

void main() {
	// Use uv coordinates passed through with frag_color vec4
	vec2 uv_use = vec2(frag_color[0], frag_color[1]);
	vec4 pixel = texture(texture_map, uv_use);

	gl_FragColor = pixel * (pow(sin(timer), 2) + 0.6); // have each particle glow
}