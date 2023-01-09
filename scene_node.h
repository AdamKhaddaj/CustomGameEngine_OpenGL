#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#define GLEW_STATIC
#define GLM_FORCE_RADIANS

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "resource.h"
#include "camera.h"
#include <vector>

namespace Game {
	class SceneNode {

	public:
		SceneNode(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture = NULL, bool isSkybox = false);
		~SceneNode();

		const std::string GetName() const;

		// Get/Set parent of node
		SceneNode* GetParent();
		void SetParent(SceneNode* parent);

		// Get/Set children of node
		std::vector<SceneNode*> GetChildren();
		SceneNode* GetChild(int i);
		void AddChild(SceneNode* child);

		// OpenGL variables

		GLuint GetArrayBuffer() const;
		GLuint GetElementArrayBuffer() const;
		GLsizei GetSize() const;
		GLenum GetMode() const;
		GLuint GetMaterial() const;
		virtual glm::mat4 GetTransform(bool useScale);

		glm::vec3 GetPosition() const;
		glm::quat GetOrientation() const;
		glm::vec3 GetScale() const;

		void SetPosition(glm::vec3 position);
		void SetOrientation(glm::quat orientation);
		void SetScale(glm::vec3 scale);

		void Translate(glm::vec3 translation);
		void Rotate(glm::quat rotation);
		void Scale(glm::vec3 scale);

		virtual void Draw(Camera* camera);

	private:
		std::string name;

		// Parent of the node
		SceneNode* parent;
		// Children of the node
		std::vector<SceneNode*> children;

		GLuint arrayBuffer;
		GLuint elementArrayBuffer;
		GLsizei size;
		GLenum mode;
		GLuint material;
		GLuint texture;

		bool isSkybox;

		glm::vec3 position;
		glm::quat orientation;
		glm::vec3 scale;

		void SetupShader(GLuint program);
	};
}

#endif