#ifndef CAMERA_H_
#define CAMERA_H_

#define GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Game {
	class Camera {

	public:
		Camera();
		~Camera();

		glm::vec3 GetPosition();

		glm::vec3 GetForward();
		glm::vec3 GetSide();
		glm::vec3 GetUp();

		// Returns the player forward vector of the camera
		glm::vec3 GetPlayerForward();

		void SetPosition(glm::vec3 position);

		void SetOrientation(glm::quat orientation);

		// Sets up the camera
		void SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
		// Sets the projection matrix of the camera
		void SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat width, GLfloat height);

		void SetupShader(GLuint program);

		// Rotate the camera based on mouse input
		void Look(float x, float y, float width, float height);

	private:
		glm::vec3 position;

		glm::vec3 forward;
		glm::vec3 up;

		glm::quat orientation;

		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;

		// Flag for first mouse position set
		bool mouseStart = true;

		float mouseX = 0.0f;
		float mouseY = 0.0f;

		float yaw = 0.0f;
		float pitch = 0.0f;

		// Sets the view matrix of the camera
		void SetupViewMatrix();
	};
}

#endif