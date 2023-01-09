#define GLM_FORCE_RADIANS

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "scene_graph.h"

namespace Game {
	SceneGraph::SceneGraph() {}

	SceneGraph::~SceneGraph() {}

	glm::vec3 SceneGraph::GetBackgroundColor() {
		return backgroundColor;
	}

	void SceneGraph::SetBackgroundColor(glm::vec3 color) {
		backgroundColor = color;
	}

	SceneNode* SceneGraph::GetNode(std::string name) {
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->GetName() == name) {
				return nodes[i];
			}
		}

		return NULL;
	}

	SceneNode* SceneGraph::CreateNode(std::string name, Resource* geometry, Resource* material, Resource* texture, bool isSkybox) {
		SceneNode* node = new SceneNode(name, geometry, material, texture, isSkybox);

		nodes.push_back(node);

		return node;
	}

	void SceneGraph::AddNode(SceneNode* node) {
		nodes.push_back(node);
	}

	void SceneGraph::Draw(Camera* camera) {
		// Clear background

		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw all nodes
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->Draw(camera);
		}
	}

	void SceneGraph::SetupDrawToTexture() {
		// Set up frame buffer

		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		// Set up target texture for rendering

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set up an image for the texture

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Set up a depth buffer for rendering

		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

		// Configure frame buffer (attach rendering buffers)

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		// Check if frame buffer was setup successfully
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw(std::string("Error setting up frame buffer"));
		}

		// Reset frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Set up quad for drawing to the screen
		static const GLfloat quad_vertex_data[] = {
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		};

		// Create buffer for quad

		glGenBuffers(1, &quadArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quadArrayBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data, GL_STATIC_DRAW);
	}

	void SceneGraph::DrawToTexture(Camera* camera) {
		// Save current viewport

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		// Enable frame buffer

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

		// Clear background

		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw all scene nodes
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->Draw(camera);
		}

		// Reset frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Restore viewport
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	void SceneGraph::DisplayTexture(GLuint program, float param, GLuint overlay) {
		// Configure output to the screen

		glDisable(GL_DEPTH_TEST);

		// Set up quad geometry
		glBindBuffer(GL_ARRAY_BUFFER, quadArrayBuffer);

		// Select proper material (shader program)
		glUseProgram(program);

		// Setup attributes of screen-space shader

		GLint pos_att = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(pos_att);
		glVertexAttribPointer(pos_att, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

		GLint tex_att = glGetAttribLocation(program, "uv");
		glEnableVertexAttribArray(tex_att);
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		// Timer

		GLint timer_var = glGetUniformLocation(program, "timer");
		float current_time = glfwGetTime();
		glUniform1f(timer_var, current_time);

		// Distance

		GLint proximity_var = glGetUniformLocation(program, "proximity");
		glUniform1f(proximity_var, param);

		// Bind texture
		glUniform1i(texture, 0); // Assign the first texture to the map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Define texture interpolation
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (overlay != NULL) {
			GLint overlay_map = glGetUniformLocation(program, "overlay");
			glUniform1i(overlay_map, 1); // Assign the second texture to the map
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, overlay); // Second texture we bind

			// Define texture interpolation
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		// Draw geometry
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Reset current geometry
		glEnable(GL_DEPTH_TEST);
	}
}