#version 400

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform sampler2D texture_map;
uniform sampler2D overlay;

void main() {
	vec4 scene = texture(texture_map, uv0);
	vec4 addOverlay = texture(overlay, vec2(uv0.x, 1.0 - uv0.y));
	
	gl_FragColor = scene + addOverlay;
}