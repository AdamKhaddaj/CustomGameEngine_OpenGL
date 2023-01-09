#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#define GLEW_STATIC

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "scene_node.h"
#include "resource.h"
#include "camera.h"

// Size of the texture that we will draw

#define FRAME_BUFFER_WIDTH 1920
#define FRAME_BUFFER_HEIGHT 1080

namespace Game {
	class SceneGraph {

	public:
		SceneGraph();
		~SceneGraph();

		glm::vec3 GetBackgroundColor();

		void SetBackgroundColor(glm::vec3 color);

		SceneNode* GetNode(std::string name);
		SceneNode* CreateNode(std::string name, Resource* geometry, Resource* material, Resource* texture = NULL, bool isSkybox = false);
		void AddNode(SceneNode* node);

		void Draw(Camera* camera);

		// Screen space effects

		// Setup the texture
		void SetupDrawToTexture();

		// Draw the scene into a texture
		void DrawToTexture(Camera* camera);

		// Process and draw the texture on the screen
		void DisplayTexture(GLuint program, float param = 0.0f, GLuint overlay = NULL);

	private:
		glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f);

		std::vector<SceneNode*> nodes;

		// Frame buffer for drawing to texture
		GLuint frameBuffer = 0;

		// Quad vertex array for drawing from texture
		GLuint quadArrayBuffer = 0;

		// Render targets

		GLuint texture = 0;
		GLuint depthBuffer = 0;
	};
}

#endif