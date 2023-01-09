#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;
uniform float proximity;

void main() {
	vec4 pixel;
	vec2 pos = uv0;

	pixel = texture(texture_map, uv0);
	
	if(proximity <= 20) {
		// start a timer at start of screen effect
		float timerstart = timer;

		// blur effect via covolution
		float blur_weight = 9;
		vec4 sum = vec4(0.0);
		float dist = ((20 - proximity) * 0.001);
	
		// create timestamps where dark edges "burst" to simulate a heartbeat effect
		float heartbeat = abs(sin(6.28 * ((abs(mod(timerstart, 3) - 2) + (mod(timerstart, 3)) - 2) / 2))) * 0.1;

		float edges = (abs(0.5 - uv0.x)) + (abs(0.5 - uv0.y)); // value between 0 and 1 representing proximity to center of screen

		// start on edges of screen
		float totalblur = ((abs(0.5 - uv0.x)) + (abs(0.5 - uv0.y))) * dist;

		// texture lookups for blur
		sum += texture(texture_map, uv0);
		sum += texture(texture_map, vec2((uv0.x - totalblur), uv0.y));
		sum += texture(texture_map, vec2((uv0.x - totalblur), uv0.y + totalblur));
		sum += texture(texture_map, vec2(uv0.x, (uv0.y + totalblur)));
		sum += texture(texture_map, vec2((uv0.x + totalblur), uv0.y + totalblur));
		sum += texture(texture_map, vec2((uv0.x + totalblur), uv0.y));
		sum += texture(texture_map, vec2((uv0.x + totalblur), uv0.y - totalblur));
		sum += texture(texture_map, vec2(uv0.x, uv0.y - totalblur));
		sum += texture(texture_map, vec2(uv0.x - totalblur, uv0.y - totalblur));
	
		// divide by 9 (number of adjacent pixels checked per pixel) and subtract to get dark effect
		pixel = (sum / blur_weight) - (heartbeat * edges);
	}

	gl_FragColor = pixel;
}