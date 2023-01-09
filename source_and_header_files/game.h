#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC

#include <string>
#include <exception>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/Noise.hpp>
#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include <vector>

namespace Game {
	class Game {
		// Used for collisions
		struct Collision {
			bool isPoint;

			glm::vec2 position;
			glm::vec3 size;
		};

		// Used for gem items
		struct Gem {
			int id;
			glm::vec2 location;
		};

	public:
		Game();
		~Game();

		void Initialize();

		void SetupResources();
		void SetupScene();

		void MainLoop();

	private:
		GLFWwindow* window = NULL;

		ResourceManager resourceManager;

		SceneGraph scene;

		Camera camera;

		// Movement

		double lastFrame = 0.0f;

		bool wPressed = false;
		bool aPressed = false;
		bool sPressed = false;
		bool dPressed = false;

		// Keeps track of game phase
		int phase = 0;

		//		0 - Start screen
		//		1 - Gameplay
		//		2 - Loss screen
		//		3 - Win screen
		//		4 - Paused

		float pauseBuffer = 0.0f;

		std::vector<Collision> collisions;
		std::vector<Gem> gems;

		void InitializeWindow();
		void InitializeView();
		void InitializeEventHandlers();

		// Event handlers
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void CursorCallback(GLFWwindow* window, double x, double y);
		static void ResizeCallback(GLFWwindow* window, int width, int height);

		SceneNode* CreateInstance(std::string entityName, std::string objectName, std::string materialName, std::string textureName = std::string(""));

		// Create tree

		void CreateTree(int i);
		void CreateTree(int i, SceneNode* parent);

		SceneNode* CreateBranchInstance(std::string entityName, std::string objectName, std::string materialName);

		// Create crow
		SceneNode* CreateCrow(std::string entityName, std::string objectName, std::string materialName, std::string textureName = std::string(""));

		// Check for collisions with objects

		bool CheckCollisions(glm::vec2 position);
		void CheckGems(glm::vec2 position);
	};
}

#endif