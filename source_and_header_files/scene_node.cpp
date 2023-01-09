#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>
#include "scene_node.h"

namespace Game {
	const glm::vec3 FOG_COLOR(0.8f, 0.8f, 0.8f);

	const float FOG_DENSITY = 0.02f;
	const float FOG_FACTOR = 2.0f;

	SceneNode::SceneNode(const std::string name, const Resource* geometry, const Resource* material, const Resource* texture, bool isSkybox) {
		SceneNode::name = name;

		parent = NULL;

		arrayBuffer = geometry->GetArrayBuffer();
		elementArrayBuffer = geometry->GetElementArrayBuffer();
		size = geometry->GetSize();

		// Set geometry
		if (geometry->GetType() == ResourceType::PointSet) {
			mode = GL_POINTS;
		} else if (geometry->GetType() == ResourceType::Mesh) {
			mode = GL_TRIANGLES;
		} else {
			throw(std::string("Invalid type of geometry"));
		}

		// Set material (shader program)
		if (material->GetType() != ResourceType::Material) {
			throw(std::string("Invalid type of material"));
		}

		SceneNode::material = material->GetResource();

		SceneNode::isSkybox = isSkybox;

		// Set texture
		if (texture) {
			SceneNode::texture = texture->GetResource();
		} else {
			SceneNode::texture = 0;
		}

		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	SceneNode::~SceneNode() {}

	const std::string SceneNode::GetName() const {
		return name;
	}

	SceneNode* SceneNode::GetParent() {
		return parent;
	}

	void SceneNode::SetParent(SceneNode* parent) {
		SceneNode::parent = parent;
	}

	std::vector<SceneNode*> SceneNode::GetChildren() {
		return children;
	}

	SceneNode* SceneNode::GetChild(int i) {
		return children[i];
	}

	void SceneNode::AddChild(SceneNode* child) {
		children.push_back(child);
	}

	GLuint SceneNode::GetArrayBuffer() const {
		return arrayBuffer;
	}

	GLuint SceneNode::GetElementArrayBuffer() const {
		return elementArrayBuffer;
	}

	GLsizei SceneNode::GetSize() const {
		return size;
	}

	GLenum SceneNode::GetMode() const {
		return mode;
	}

	GLuint SceneNode::GetMaterial() const {
		return material;
	}

	glm::mat4 SceneNode::GetTransform(bool useScale = false) {
		// World transformation

		glm::mat4 scaling = useScale ? glm::scale(glm::mat4(1.0f), scale) : glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4_cast(orientation);
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 transf = translation * rotation * scaling;

		glm::mat4 orbit = glm::mat4(1.0f);

		// Tree
		if (name == "Branch") {
			// Position subtract the current up vector * half the cylinder height
			glm::vec3 orbitPoint = position - (orientation * glm::vec3(0.0f, 1.0f, 0.0f)) * (scale.y / 2.0f);

			orbit = glm::translate(glm::mat4(1.0f), -orbitPoint);
			orbit = glm::rotate(orbit, (float)sin(glfwGetTime() * 0.75f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f)); // Apply wind
			orbit = glm::translate(orbit, orbitPoint);

			// Crow
		} else if (name == "LeftWing" || name == "RightWing") {
			// Calculating axis of rotation
			glm::vec3 movement = glm::vec3(0.0f, 0.0f, -1.0f); // direction of movement
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // "world" up value
			glm::vec3 axis = glm::normalize(glm::cross(movement, up)); // rotational axis

			// Calculate point of orbit
			glm::vec3 orbitPoint = GetPosition() - (glm::vec3(0.0f, (GetScale()[1] / 2.0f), 0.0f)); // position - (0, h/2, 0)

			float angle = sin(glfwGetTime() * 10.0f) * 25.0f;

			// Calculate wing transformation matrix (T^-1 * R * T)
			orbit = glm::translate(glm::mat4(1.0f), -orbitPoint);
			orbit = glm::rotate(orbit, angle, axis);
			orbit = glm::translate(orbit, orbitPoint);
		}

		if (parent == NULL) {
			return transf;
		}

		return parent->GetTransform() * transf * orbit * glm::mat4(1.0f);
	}

	glm::vec3 SceneNode::GetPosition() const {
		return position;
	}

	glm::quat SceneNode::GetOrientation() const {
		return orientation;
	}

	glm::vec3 SceneNode::GetScale() const {
		return scale;
	}

	void SceneNode::SetPosition(glm::vec3 position) {
		SceneNode::position = position;
	}

	void SceneNode::SetOrientation(glm::quat orientation) {
		SceneNode::orientation = orientation;
	}

	void SceneNode::SetScale(glm::vec3 scale) {
		SceneNode::scale = scale;
	}

	void SceneNode::Translate(glm::vec3 translation) {
		position += translation;
	}

	void SceneNode::Rotate(glm::quat rotation) {
		orientation = glm::normalize(orientation * rotation);
	}

	void SceneNode::Scale(glm::vec3 scale) {
		SceneNode::scale *= scale;
	}

	void SceneNode::Draw(Camera* camera) {
		// Select proper material (shader program)
		glUseProgram(material);

		// Set geometry to draw

		glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBuffer);

		// Set globals for camera
		camera->SetupShader(material);

		// Set world matrix and other shader input variables
		SetupShader(material);

		// Draw geometry
		if (mode == GL_POINTS) {
			if (name != "Maze") {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}

			glDrawArrays(mode, 0, size);

			glDisable(GL_BLEND);
		} else {
			glDrawElements(mode, size, GL_UNSIGNED_INT, 0);
		}

		// Draw children
		for (int i = 0; i < children.size(); i++) {
			children[i]->Draw(camera);
		}
	}

	void SceneNode::SetupShader(GLuint program) {
		// Set attributes for shaders

		GLint vertexAttribute = glGetAttribLocation(program, "vertex");
		glVertexAttribPointer(vertexAttribute, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertexAttribute);

		GLint normalAttribute = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normalAttribute);

		GLint colorAttribute = glGetAttribLocation(program, "color");
		glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(colorAttribute);

		GLint textureAttribute = glGetAttribLocation(program, "uv");
		glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(textureAttribute);

		// World transformation
		glm::mat4 transform = GetTransform(true);

		// World matrix

		GLint worldMatrix = glGetUniformLocation(program, "world_mat");
		glUniformMatrix4fv(worldMatrix, 1, GL_FALSE, glm::value_ptr(transform));

		// Normal matrix

		glm::mat4 normalMatrix = glm::transpose(glm::inverse(transform));
		GLint normalMatrixLocation = glGetUniformLocation(program, "normal_mat");
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		// Texture
		if (texture) {
			if (isSkybox) {
				glm::mat4 modelMatrix = glm::mat4();

				GLint texture = glGetUniformLocation(program, "skybox_map");

				glDepthFunc(GL_LEQUAL);

				glUniform1i(texture, 0); // Assign the first texture to the map
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, SceneNode::texture); // First texture we bind

				// Define texture interpolation
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			} else {
				GLint texture = glGetUniformLocation(program, "texture_map");
				glUniform1i(texture, 0); // Assign the first texture to the map
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, SceneNode::texture); // First texture we bind

				// Define texture interpolation
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
		}

		// Timer

		GLint timer = glGetUniformLocation(program, "timer");
		double currentTime = glfwGetTime();
		glUniform1f(timer, (float)currentTime);

		// Fog

		GLint fogColor = glGetUniformLocation(program, "fogColor");
		glUniform3fv(fogColor, 1, glm::value_ptr(FOG_COLOR));

		GLint fogDensity = glGetUniformLocation(program, "fogDensity");
		glUniform1f(fogDensity, FOG_DENSITY);

		GLint fogFactor = glGetUniformLocation(program, "fogFactor");
		glUniform1f(fogFactor, FOG_FACTOR);
	}
}