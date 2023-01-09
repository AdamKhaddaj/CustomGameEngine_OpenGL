#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#define GLEW_STATIC

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "resource.h"

// Default extensions for different shader source files

#define VERTEX_PROGRAM_EXTENSION "_vp.glsl"
#define FRAGMENT_PROGRAM_EXTENSION "_fp.glsl"
#define GEOMETRY_PROGRAM_EXTENSION "_gp.glsl"

// Minimum size is 55
const unsigned int MAP_SIZE = 55;

namespace Game {
	class ResourceManager {

	public:
		ResourceManager();
		~ResourceManager();

		// Add a resource that was already loaded and allocated to memory

		void AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size);
		void AddResource(ResourceType type, const std::string name, GLuint arrayBuffer, GLuint elementArrayBuffer, GLsizei size);

		// Load a resource from a file, according to the specified type
		void LoadResource(ResourceType type, const std::string name, const char* filename);

		// Load cubemap texture
		void LoadCubemap(const std::string name, const char* xpos, const char* xneg, const char* ypos, const char* yneg, const char* zpos, const char* zneg);

		// Get the resource with the specified name
		Resource* GetResource(const std::string name) const;

		// Methods to create specific resources

		void CreateTerrain();
		void CreateMaze();

		void CreateSkybox();

		// Create the geometry for a cylinder
		void CreateCylinder(std::string objectName, float height = 1.0f, float radius = 0.6f, int numSamplesTheta = 90, int numSamplesPhi = 45);

		// Particles

		void CreateFountainParticles(std::string object_name, int num_particles = 8000);
		void CreateMonsterParticles(int num_particles);
		void CreateLeafParticles(int num_particles);
		void CreateLineParticles(std::string object_name, int num_particles = 20000);

		// Returns the height at (x, y)
		float GetTerrainHeightAt(float x, float y);
		// Returns if collision maze cell exists at (x, y)
		bool GetMazeCollisions(int x, int y, glm::vec3 position);

	private:
		// List storing all resources
		std::vector<Resource*> resources;

		// Stores heightmap heights
		float heights[MAP_SIZE * 2][MAP_SIZE * 2];
		// Stores maze collision matrix
		bool collisions[MAP_SIZE][MAP_SIZE];

		// Methods to load specific types of resources

		// Load shaders programs
		void LoadMaterial(const std::string name, const char* prefix);

		// Load a text file into memory (could be source code)
		std::string LoadTextFile(const char* filename);

		// Load a texture from an image file: png, jpg, etc.
		void LoadTexture(const std::string name, const char* filename);

		// Loads a mesh in obj format
		void LoadMesh(const std::string name, const char* filename);
	};
}

#endif