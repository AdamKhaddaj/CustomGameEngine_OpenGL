#version 400

out vec4 FragColor;

in vec3 uvcoords;

uniform samplerCube skybox_map;

void main() {
	gl_FragColor = texture(skybox_map, uvcoords);
}