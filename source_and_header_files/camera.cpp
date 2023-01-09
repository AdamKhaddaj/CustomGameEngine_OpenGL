#define GLM_FORCE_RADIANS

#include <iostream>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

namespace Game {
	const float MOUSE_SENSITIVITY = 0.002f;

	Camera::Camera() {}

	Camera::~Camera() {}

	glm::vec3 Camera::GetPosition() {
		return position;
	}

	glm::vec3 Camera::GetForward() {
		return glm::normalize(orientation * forward);
	}

	glm::vec3 Camera::GetSide() {
		return glm::normalize(glm::cross(GetForward(), GetUp()));
	}

	glm::vec3 Camera::GetUp() {
		return glm::normalize(orientation * up);
	}

	// Only used in gameplay phase
	glm::vec3 Camera::GetPlayerForward() {
		return glm::normalize(glm::angleAxis(yaw, up) * forward);
	}

	void Camera::SetPosition(glm::vec3 position) {
		Camera::position = position;
	}

	void Camera::SetOrientation(glm::quat orientation) {
		Camera::orientation = glm::normalize(orientation);

		// Reset all orientation variables

		mouseStart = true;

		mouseX = 0.0f;
		mouseY = 0.0f;

		yaw = 0.0f;
		pitch = 0.0f;
	}

	void Camera::SetView(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
		Camera::position = position;

		Camera::forward = glm::normalize(forward);
		Camera::up = glm::normalize(up);;

		// Reset orientation
		SetOrientation(glm::quat());
	}

	void Camera::SetProjection(GLfloat fov, GLfloat near, GLfloat far, GLfloat width, GLfloat height) {
		float top = tan((fov / 2.0f) * (glm::pi<float>() / 180.0f)) * near;
		float right = top * (width / height);

		projectionMatrix = glm::frustum(-right, right, -top, top, near, far);
	}

	void Camera::SetupShader(GLuint program) {
		SetupViewMatrix();

		GLint viewMatrix = glGetUniformLocation(program, "view_mat");
		glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(Camera::viewMatrix));

		GLint projectionMatrix = glGetUniformLocation(program, "projection_mat");
		glUniformMatrix4fv(projectionMatrix, 1, GL_FALSE, glm::value_ptr(Camera::projectionMatrix));
	}

	// Only used in gameplay phase
	void Camera::Look(float x, float y, float width, float height) {
		if (mouseStart) {
			mouseStart = false;

			mouseX = x;
			mouseY = y;

			pitch = 0.0f;
			yaw = 0.0f;
		}

		float dx = (x - mouseX) * MOUSE_SENSITIVITY;
		float dy = (y - mouseY) * MOUSE_SENSITIVITY;

		mouseX = x;
		mouseY = y;

		yaw += dx;
		pitch += dy;

		// Clamp view angle

		if (pitch > glm::radians(85.0f)) {
			pitch = glm::radians(85.0f);
		} else if (pitch < glm::radians(-85.0f)) {
			pitch = glm::radians(-85.0f);
		}

		// Set orientation

		orientation = glm::normalize(glm::angleAxis(yaw, up));
		orientation = glm::normalize(glm::angleAxis(pitch, GetSide()) * orientation);
	}

	void Camera::SetupViewMatrix() {
		glm::vec3 forward = GetForward();
		glm::vec3 side = GetSide();
		glm::vec3 up = GetUp();

		viewMatrix = glm::mat4(1.0f);

		viewMatrix[0][0] = side[0];
		viewMatrix[1][0] = side[1];
		viewMatrix[2][0] = side[2];
		viewMatrix[0][1] = up[0];
		viewMatrix[1][1] = up[1];
		viewMatrix[2][1] = up[2];
		viewMatrix[0][2] = forward[0];
		viewMatrix[1][2] = forward[1];
		viewMatrix[2][2] = forward[2];

		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position);

		viewMatrix *= translation;
	}
}