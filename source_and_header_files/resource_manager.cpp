#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SOIL/SOIL.h>
#include <stack>
#include "resource_manager.h"
#include "model_loader.h"

namespace Game {
	ResourceManager::ResourceManager() {}

	ResourceManager::~ResourceManager() {}

	void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size) {
		Resource* res;

		res = new Resource(type, name, resource, size);

		resources.push_back(res);
	}

	void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint arrayBuffer, GLuint elementArrayBuffer, GLsizei size) {
		Resource* res;

		res = new Resource(type, name, arrayBuffer, elementArrayBuffer, size);

		resources.push_back(res);
	}

	void ResourceManager::LoadResource(ResourceType type, const std::string name, const char* filename) {
		// Call appropriate method depending on type of resource
		if (type == ResourceType::Material) {
			LoadMaterial(name, filename);
		} else if (type == ResourceType::Texture) {
			LoadTexture(name, filename);
		} else if (type == ResourceType::Mesh) {
			LoadMesh(name, filename);
		} else {
			throw(std::string("Invalid type of resource"));
		}
	}

	void ResourceManager::LoadCubemap(const std::string name, const char* xpos, const char* xneg, const char* ypos, const char* yneg, const char* zpos, const char* zneg) {
		// Load cubemap from file
		GLuint texture = SOIL_load_OGL_cubemap(xpos, xneg, ypos, yneg, zpos, zneg, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

		if (!texture) {
			throw(std::string("Error loading cubemap ") + std::string(name) + std::string(": ") + std::string(SOIL_last_result()));
		}

		// Create resource
		AddResource(ResourceType::Texture, name, texture, 0);
	}

	Resource* ResourceManager::GetResource(const std::string name) const {
		// Find resource with the specified name
		for (int i = 0; i < resources.size(); i++) {
			if (resources[i]->GetName() == name) {
				return resources[i];
			}
		}

		return NULL;
	}

	void ResourceManager::LoadMaterial(const std::string name, const char* prefix) {
		// Load vertex program source code

		std::string filename = std::string(prefix) + std::string(VERTEX_PROGRAM_EXTENSION);
		std::string vp = LoadTextFile(filename.c_str());

		// Load fragment program source code

		filename = std::string(prefix) + std::string(FRAGMENT_PROGRAM_EXTENSION);
		std::string fp = LoadTextFile(filename.c_str());

		// Create a shader from the vertex program source code

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		const char* source_vp = vp.c_str();
		glShaderSource(vs, 1, &source_vp, NULL);
		glCompileShader(vs);

		// Check if shader compiled successfully

		GLint status;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(vs, 512, NULL, buffer);
			throw(std::ios_base::failure(std::string("Error compiling vertex shader: ") + std::string(buffer)));
		}

		// Create a shader from the fragment program source code

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		const char* source_fp = fp.c_str();
		glShaderSource(fs, 1, &source_fp, NULL);
		glCompileShader(fs);

		// Check if shader compiled successfully

		glGetShaderiv(fs, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(fs, 512, NULL, buffer);
			throw(std::string("Error compiling fragment shader: ") + std::string(buffer));
		}

		// Try to also load a geometry shader

		filename = std::string(prefix) + std::string(GEOMETRY_PROGRAM_EXTENSION);
		bool geometry_program = false;
		std::string gp = "";
		GLuint gs;

		try {
			gp = LoadTextFile(filename.c_str());
			geometry_program = true;
		} catch (std::string exception) {}

		if (geometry_program) {
			// Create a shader from the geometry program source code

			gs = glCreateShader(GL_GEOMETRY_SHADER);
			const char* source_gp = gp.c_str();
			glShaderSource(gs, 1, &source_gp, NULL);
			glCompileShader(gs);

			// Check if shader compiled successfully

			GLint status;
			glGetShaderiv(gs, GL_COMPILE_STATUS, &status);

			if (status != GL_TRUE) {
				char buffer[512];
				glGetShaderInfoLog(gs, 512, NULL, buffer);
				throw(std::string("Error compiling geometry shader: ") + std::string(buffer));
			}
		}

		// Create a shader program linking both vertex and fragment shaders together

		GLuint sp = glCreateProgram();
		glAttachShader(sp, vs);
		glAttachShader(sp, fs);

		if (geometry_program) {
			glAttachShader(sp, gs);
		}

		glLinkProgram(sp);

		// Check if shaders were linked successfully

		glGetProgramiv(sp, GL_LINK_STATUS, &status);

		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(sp, 512, NULL, buffer);
			throw(std::string("Error linking shaders: ") + std::string(buffer));
		}

		// Delete memory used by shaders, since they were already compiled and linked

		glDeleteShader(vs);
		glDeleteShader(fs);

		if (geometry_program) {
			glDeleteShader(gs);
		}

		// Add a resource for the shader program
		AddResource(ResourceType::Material, name, sp, 0);
	}

	std::string ResourceManager::LoadTextFile(const char* filename) {
		// Open file

		std::ifstream f;
		f.open(filename);

		if (f.fail()) {
			throw(std::string("Error opening file ") + std::string(filename));
		}

		// Read file

		std::string content;
		std::string line;

		while (std::getline(f, line)) {
			content += line + "\n";
		}

		// Close file
		f.close();

		return content;
	}

	void ResourceManager::LoadTexture(const std::string name, const char* filename) {
		// Load texture from file

		GLuint texture = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

		if (!texture) {
			throw(std::string("Error loading texture ") + std::string(filename) + std::string(": ") + std::string(SOIL_last_result()));
		}

		// Create resource
		AddResource(ResourceType::Texture, name, texture, 0);
	}

	void ResourceManager::LoadMesh(const std::string name, const char* filename) {
		// First load model into memory. If that goes well, we transfer the mesh to an OpenGL buffer

		TriMesh mesh;

		// Parse file
		// Open file

		std::ifstream f;
		f.open(filename);

		if (f.fail()) {
			throw(std::string("Error opening file ") + std::string(filename));
		}

		// Parse lines

		std::string line;
		std::string ignore(" \t\r\n");
		std::string part_separator(" \t");
		std::string face_separator("/");

		bool added_normal = false;

		while (std::getline(f, line)) {
			// Clean extremities of the string
			string_trim(line, ignore);

			// Ignore comments
			if ((line.size() <= 0) ||
				(line[0] == '#')) {
				continue;
			}

			// Parse string
			std::vector<std::string> part = string_split(line, part_separator);

			// Check commands
			if (!part[0].compare(std::string("v"))) {
				if (part.size() >= 4) {
					glm::vec3 position(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
					mesh.position.push_back(position);
				} else {
					throw(std::string("Error: v command should have exactly 3 parameters"));
				}
			} else if (!part[0].compare(std::string("vn"))) {
				if (part.size() >= 4) {
					glm::vec3 normal(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
					mesh.normal.push_back(normal);
					added_normal = true;
				} else {
					throw(std::string("Error: vn command should have exactly 3 parameters"));
				}
			} else if (!part[0].compare(std::string("vt"))) {
				if (part.size() >= 3) {
					glm::vec2 tex_coord(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()));
					mesh.tex_coord.push_back(tex_coord);
				} else {
					throw(std::string("Error: vt command should have exactly 2 parameters"));
				}
			} else if (!part[0].compare(std::string("f"))) {
				if (part.size() >= 4) {
					if (part.size() > 5) {
						throw(std::string("Error: f commands with more than 4 vertices not supported"));
					} else if (part.size() == 5) {
						// Break a quad into two triangles

						Quad quad;

						for (int i = 0; i < 4; i++) {
							std::vector<std::string> fd = string_split_once(part[i + 1], face_separator);

							if (fd.size() == 1) {
								quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
								quad.t[i] = -1;
								quad.n[i] = -1;
							} else if (fd.size() == 2) {
								quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
								quad.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
								quad.n[i] = -1;
							} else if (fd.size() == 3) {
								quad.i[i] = str_to_num<float>(fd[0].c_str()) - 1;

								if (std::string("").compare(fd[1]) != 0) {
									quad.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
								} else {
									quad.t[i] = -1;
								}

								quad.n[i] = str_to_num<float>(fd[2].c_str()) - 1;
							} else {
								throw(std::string("Error: f parameter should have 1 or 3 parameters separated by '/'"));
							}
						}

						Face face1, face2;

						face1.i[0] = quad.i[0]; face1.i[1] = quad.i[1]; face1.i[2] = quad.i[2];
						face1.n[0] = quad.n[0]; face1.n[1] = quad.n[1]; face1.n[2] = quad.n[2];
						face1.t[0] = quad.t[0]; face1.t[1] = quad.t[1]; face1.t[2] = quad.t[2];
						face2.i[0] = quad.i[0]; face2.i[1] = quad.i[2]; face2.i[2] = quad.i[3];
						face2.n[0] = quad.n[0]; face2.n[1] = quad.n[2]; face2.n[2] = quad.n[3];
						face2.t[0] = quad.t[0]; face2.t[1] = quad.t[2]; face2.t[2] = quad.t[3];

						mesh.face.push_back(face1);
						mesh.face.push_back(face2);
					} else if (part.size() == 4) {
						Face face;

						for (int i = 0; i < 3; i++) {
							std::vector<std::string> fd = string_split_once(part[i + 1], face_separator);

							if (fd.size() == 1) {
								face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
								face.t[i] = -1;
								face.n[i] = -1;
							} else if (fd.size() == 2) {
								face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;
								face.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
								face.n[i] = -1;
							} else if (fd.size() == 3) {
								face.i[i] = str_to_num<float>(fd[0].c_str()) - 1;

								if (std::string("").compare(fd[1]) != 0) {
									face.t[i] = str_to_num<float>(fd[1].c_str()) - 1;
								} else {
									face.t[i] = -1;
								}

								face.n[i] = str_to_num<float>(fd[2].c_str()) - 1;
							} else {
								throw(std::string("Error: f parameter should have 1, 2, or 3 parameters separated by '/'"));
							}
						}

						mesh.face.push_back(face);
					}
				} else {
					throw(std::string("Error: f command should have 3 or 4 parameters"));
				}
			}
		}

		// Close file
		f.close();

		// Check if vertex references are correct
		for (unsigned int i = 0; i < mesh.face.size(); i++) {
			for (int j = 0; j < 3; j++) {
				if (mesh.face[i].i[j] >= mesh.position.size()) {
					throw(std::string("Error: index for triangle ") + num_to_str<int>(mesh.face[i].i[j]) + std::string(" is out of bounds"));
				}
			}
		}

		// Compute degree of each vertex

		std::vector<int> degree(mesh.position.size(), 0);

		for (unsigned int i = 0; i < mesh.face.size(); i++) {
			for (int j = 0; j < 3; j++) {
				degree[mesh.face[i].i[j]]++;
			}
		}

		// Compute vertex normals if no normals were ever added
		if (!added_normal) {
			mesh.normal = std::vector<glm::vec3>(mesh.position.size(), glm::vec3(0.0, 0.0, 0.0));

			for (unsigned int i = 0; i < mesh.face.size(); i++) {
				// Compute face normal

				glm::vec3 vec1, vec2;

				vec1 = mesh.position[mesh.face[i].i[0]] -
					mesh.position[mesh.face[i].i[1]];
				vec2 = mesh.position[mesh.face[i].i[0]] -
					mesh.position[mesh.face[i].i[2]];

				glm::vec3 norm = glm::cross(vec1, vec2);
				norm = glm::normalize(norm);

				// Add face normal to vertices
				mesh.normal[mesh.face[i].i[0]] += norm;
				mesh.normal[mesh.face[i].i[1]] += norm;
				mesh.normal[mesh.face[i].i[2]] += norm;
			}

			for (unsigned int i = 0; i < mesh.normal.size(); i++) {
				if (degree[i] > 0) {
					mesh.normal[i] /= degree[i];
				}
			}
		}

		// Number of attributes for vertices and faces

		const int vertex_att = 11;
		const int face_att = 3;

		// Create OpenGL buffers and copy data
		GLuint vbo, ebo;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.face.size() * 3 * vertex_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.face.size() * face_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

		unsigned int vertex_index = 0;

		for (unsigned int i = 0; i < mesh.face.size(); i++) {
			// Add three vertices and their attributes
			GLfloat att[3 * vertex_att] = { 0 };

			for (int j = 0; j < 3; j++) {
				// Position
				att[j * vertex_att + 0] = mesh.position[mesh.face[i].i[j]][0];
				att[j * vertex_att + 1] = mesh.position[mesh.face[i].i[j]][1];
				att[j * vertex_att + 2] = mesh.position[mesh.face[i].i[j]][2];

				// Normal
				if (!added_normal) {
					att[j * vertex_att + 3] = mesh.normal[mesh.face[i].i[j]][0];
					att[j * vertex_att + 4] = mesh.normal[mesh.face[i].i[j]][1];
					att[j * vertex_att + 5] = mesh.normal[mesh.face[i].i[j]][2];
				} else {
					if (mesh.face[i].n[j] >= 0) {
						att[j * vertex_att + 3] = mesh.normal[mesh.face[i].n[j]][0];
						att[j * vertex_att + 4] = mesh.normal[mesh.face[i].n[j]][1];
						att[j * vertex_att + 5] = mesh.normal[mesh.face[i].n[j]][2];
					}
				}

				// No color in (6, 7, 8)
				// Texture coordinates

				if (mesh.face[i].t[j] >= 0) {
					att[j * vertex_att + 9] = mesh.tex_coord[mesh.face[i].t[j]][0];
					att[j * vertex_att + 10] = mesh.tex_coord[mesh.face[i].t[j]][1];
				}
			}

			// Copy attributes to buffer
			glBufferSubData(GL_ARRAY_BUFFER, i * 3 * vertex_att * sizeof(GLfloat), 3 * vertex_att * sizeof(GLfloat), att);

			// Add triangle

			GLuint findex[face_att] = { 0 };
			findex[0] = vertex_index;
			findex[1] = vertex_index + 1;
			findex[2] = vertex_index + 2;
			vertex_index += 3;

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, i * face_att * sizeof(GLuint), face_att * sizeof(GLuint), findex);
		}

		// Create resource
		AddResource(ResourceType::Mesh, name, vbo, ebo, mesh.face.size() * face_att);
	}

	void string_trim(std::string str, std::string to_trim) {
		// Trim any character in to_trim from the beginning of the string str
		while ((str.size() > 0) &&
			(to_trim.find(str[0]) != std::string::npos)) {
			str.erase(0);
		}

		// Trim any character in to_trim from the end of the string str
		while ((str.size() > 0) &&
			(to_trim.find(str[str.size() - 1]) != std::string::npos)) {
			str.erase(str.size() - 1);
		}
	}

	std::vector<std::string> string_split(std::string str, std::string separator) {
		// Initialize output

		std::vector<std::string> output;
		output.push_back(std::string(""));
		int string_index = 0;

		// Analyze string
		unsigned int i = 0;

		while (i < str.size()) {
			// Check if character i is a separator
			if (separator.find(str[i]) != std::string::npos) {
				// Split string
				string_index++;
				output.push_back(std::string(""));

				// Skip separators
				while ((i < str.size()) && (separator.find(str[i]) != std::string::npos)) {
					i++;
				}
			} else {
				// Otherwise, copy string
				output[string_index] += str[i];
				i++;
			}
		}

		return output;
	}

	std::vector<std::string> string_split_once(std::string str, std::string separator) {
		// Initialize output

		std::vector<std::string> output;
		output.push_back(std::string(""));
		int string_index = 0;

		// Analyze string
		unsigned int i = 0;

		while (i < str.size()) {
			// Check if character i is a separator
			if (separator.find(str[i]) != std::string::npos) {
				// Split string
				string_index++;
				output.push_back(std::string(""));

				// Skip single separator
				i++;
			} else {
				// Otherwise, copy string
				output[string_index] += str[i];
				i++;
			}
		}

		return output;
	}

	void print_mesh(TriMesh& mesh) {
		for (unsigned int i = 0; i < mesh.position.size(); i++) {
			std::cout << "v " <<
				mesh.position[i].x << " " <<
				mesh.position[i].y << " " <<
				mesh.position[i].z << std::endl;
		}

		for (unsigned int i = 0; i < mesh.normal.size(); i++) {
			std::cout << "vn " <<
				mesh.normal[i].x << " " <<
				mesh.normal[i].y << " " <<
				mesh.normal[i].z << std::endl;
		}

		for (unsigned int i = 0; i < mesh.tex_coord.size(); i++) {
			std::cout << "vt " <<
				mesh.tex_coord[i].x << " " <<
				mesh.tex_coord[i].y << std::endl;
		}

		for (unsigned int i = 0; i < mesh.face.size(); i++) {
			std::cout << "f " <<
				mesh.face[i].i[0] << " " <<
				mesh.face[i].i[1] << " " <<
				mesh.face[i].i[2] << " " << std::endl;
		}
	}

	template <typename T> std::string num_to_str(T num) {
		std::ostringstream ss;
		ss << num;
		return ss.str();
	}

	template <typename T> T str_to_num(const std::string& str) {
		std::istringstream ss(str);
		T result;
		ss >> result;

		if (ss.fail()) {
			throw(std::string("Invalid number: ") + str);
		}

		return result;
	}

	void ResourceManager::CreateTerrain() {
		const int dimension = MAP_SIZE * 2;

		const GLuint vertex_num = dimension * dimension;
		const GLuint face_num = dimension * dimension * 2;

		// Number of attributes for vertices and faces

		const int vertex_att = 11;
		const int face_att = 3;

		// Data buffers

		GLfloat* vertex = NULL;
		GLuint* face = NULL;

		// Allocate memory for buffers
		try {
			vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
			face = new GLuint[face_num * face_att]; // 3 indices per face
		} catch (std::exception& e) {
			throw(std::string("Terrain error: ") + e.what());
		}

		// Basic loop to determine heightfield
		for (int i = 0; i < dimension; i++) {
			for (int j = 0; j < dimension; j++) {
				float height = 0.0f;

				float rand1 = ((double)rand() / (RAND_MAX));

				height = 2.0f * glm::pow(glm::max(0.0f, 20.0f - (glm::distance(glm::vec2(dimension / 2.0f, dimension / 2.0f), glm::vec2(i, j)) - 1.25f * rand1)), 0.9f);

				// Now place height in the 2D heightfield array
				heights[i][j] = height / 4.0f;
			}
		}

		for (int i = 0; i < dimension; i++) {
			for (int j = 0; j < dimension; j++) {
				glm::vec3 vertex_position(j, heights[i][j], i);
				glm::vec3 vertex_color(0.0f, 1.0f, 0.0f);
				glm::vec3 vertex_normal(0.0f, 1.0f, 0.0f);
				glm::vec2 vertex_coords(i / 2.0f, j / 2.0f);

				if (((i < dimension - 33) && (i > 32)) && ((j < dimension - 33) && (j > 32))) {
					// Calculate normal using adjacent heighfield values to create four triangles, take the average of the normals of those four triangles

					glm::vec3 curvec, secondvec, thirdvec, firstline, secondline, cross1, cross2, cross3, cross4;

					// First triangle

					curvec = vertex_position;
					secondvec = glm::vec3(j + 1, heights[i][j + 1] / 4, i);
					thirdvec = glm::vec3(j + 1, heights[i + 1][j + 1] / 4, i + 1);

					firstline = secondvec - curvec;
					secondline = thirdvec - curvec;

					cross1 = glm::cross(secondline, firstline);

					// Second triangle

					curvec = vertex_position;
					secondvec = glm::vec3(j, heights[i + 1][j] / 4, i + 1);
					thirdvec = glm::vec3(j - 1, heights[i + 1][j - 1] / 4, i + 1);

					firstline = secondvec - curvec;
					secondline = thirdvec - curvec;

					cross2 = glm::cross(secondline, firstline);;

					// Third triangle

					curvec = vertex_position;
					secondvec = glm::vec3(j - 1, heights[i][j - 1] / 4, i);
					thirdvec = glm::vec3(j - 1, heights[i - 1][j - 1] / 4, i - 1);

					firstline = secondvec - curvec;
					secondline = thirdvec - curvec;

					cross3 = glm::cross(secondline, firstline);

					// Fourth triangle

					curvec = vertex_position;
					secondvec = glm::vec3(j, heights[i - 1][j] / 4, i - 1);
					thirdvec = glm::vec3(j + 1, heights[i + 1][j + 1] / 4, i + 1);

					firstline = secondvec - curvec;
					secondline = thirdvec - curvec;

					cross4 = glm::cross(secondline, firstline);

					// Take average (add them) and use that

					vertex_normal = glm::normalize(cross1 + cross2 + cross3 + cross4);
				}

				for (int k = 0; k < 3; k++) {
					vertex[(i * dimension + j) * vertex_att + k] = vertex_position[k];
					vertex[(i * dimension + j) * vertex_att + k + 3] = vertex_normal[k];
					vertex[(i * dimension + j) * vertex_att + k + 6] = vertex_color[k];
				}

				vertex[(i * dimension + j) * vertex_att + 9] = vertex_coords[0];
				vertex[(i * dimension + j) * vertex_att + 10] = vertex_coords[1];
			}
		}

		// Use global variable for heighfield for now

		// Make triangles
		for (int i = 0; i < dimension - 1; i++) {
			for (int j = 0; j < dimension - 1; j++) {
				// Two triangles per quad

				glm::vec3 t1(((i + 1) % dimension) * dimension + j,
					i * dimension + ((j + 1) % dimension),
					i * dimension + j);

				glm::vec3 t2(((i + 1) % dimension) * dimension + j,
					((i + 1) % dimension) * dimension + ((j + 1) % dimension),
					i * dimension + ((j + 1) % dimension));

				// Add two triangles to the data buffer
				for (int k = 0; k < 3; k++) {
					face[(i * dimension + j) * face_att * 2 + k] = (GLuint)t1[k];
					face[(i * dimension + j) * face_att * 2 + k + face_att] = (GLuint)t2[k];
				}
			}
		}

		// Create OpenGL buffers and copy data

		GLuint vbo, ebo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

		// Free data buffers

		delete[] vertex;
		delete[] face;

		// Create resource
		AddResource(ResourceType::Mesh, "Terrain", vbo, ebo, face_num * face_att);
	}

	void ResourceManager::CreateMaze() {
		const int size = MAP_SIZE;
		int maze[MAP_SIZE][MAP_SIZE];

		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				if (x % 2 == 0 || y % 2 == 0) {
					maze[x][y] = 1;
				} else {
					maze[x][y] = -1;
				}
			}
		}

		std::stack<glm::vec2> stack;

		maze[1][1] = 0;
		stack.push(glm::vec2(1, 1));

		srand((unsigned int)time(NULL));

		while (stack.size() > 0) {
			glm::vec2 cell = stack.top();
			stack.pop();

			int x = cell.x;
			int y = cell.y;

			std::vector<int> unvisited;

			if (x + 2 < size && maze[x + 2][y] == -1) {
				unvisited.push_back(0);
			}

			if (x - 2 > 0 && maze[x - 2][y] == -1) {
				unvisited.push_back(1);
			}

			if (y + 2 < size && maze[x][y + 2] == -1) {
				unvisited.push_back(2);
			}

			if (y - 2 > 0 && maze[x][y - 2] == -1) {
				unvisited.push_back(3);
			}

			if (unvisited.size() == 0) {
				continue;
			}

			stack.push(glm::vec2(x, y));

			switch (unvisited[rand() % unvisited.size()]) {
			case 0:
				maze[x + 1][y] = 0;
				maze[x + 2][y] = 0;
				stack.push(glm::vec2(x + 2, y));
				break;
			case 1:
				maze[x - 1][y] = 0;
				maze[x - 2][y] = 0;
				stack.push(glm::vec2(x - 2, y));
				break;
			case 2:
				maze[x][y + 1] = 0;
				maze[x][y + 2] = 0;
				stack.push(glm::vec2(x, y + 2));
				break;
			case 3:
				maze[x][y - 1] = 0;
				maze[x][y - 2] = 0;
				stack.push(glm::vec2(x, y - 2));
				break;
			default:
				break;
			}
		}

		std::vector<glm::vec2> cells;

		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {
				collisions[x][y] = false;

				// Maze edges
				if (x == 0 || x == size - 1 || y == 0 || y == size - 1) {
					cells.push_back(glm::vec2(x, y));
					collisions[x][y] = true;
					continue;
				}

				// Maze open spaces
				if ((x > 14 && x < size - 15 && y > 14 && y < size - 15) || (x < 10 && y < 10) || (x > size - 11 && y > size - 11) || (x < 10 && y > size - 11) || (x > size - 11 && y < 10)) {
					continue;
				} else if (maze[x][y] == 1) {
					cells.push_back(glm::vec2(x, y));
					collisions[x][y] = true;
				}
			}
		}

		int numVertices = cells.size();

		// Data buffer
		GLfloat* vertices = NULL;

		// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
		const int vertexAtt = 11;

		// Allocate memory for buffer
		try {
			vertices = new GLfloat[numVertices * vertexAtt];
		} catch (std::exception& e) {
			throw e;
		}

		for (int i = 0; i < numVertices; i++) {
			glm::vec3 position(cells[i].x * 2.0f, 1.0f, cells[i].y * 2.0f);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertices[i * vertexAtt + k] = position[k];
			}
		}

		// Create OpenGL buffer and copy data
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, numVertices * vertexAtt * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		// Free data buffers
		delete[] vertices;

		// Create resource
		AddResource(ResourceType::PointSet, "Maze", vbo, 0, numVertices);
	}

	void ResourceManager::CreateSkybox() {
		// Hardcode in values for cube that surrounds game world

		GLfloat vertex[] = {
			// Position, normal, color, texture coordinates
			// Here, color stores the tangent of the vertex
			/*
			-1.0, -1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 0.0,
			-1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 1.0,
			 1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 1.0,
			 1.0, -1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 0.0 */

			-2.0, -2.0, 2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  0.0, 2.0,  //3 	//		7-------6
			 2.0, -2.0, 2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  2.0, 2.0,	 //2    //	   /|	   /|
			 2.0, -2.0, -2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  2.0, 0.0, //1 	//	  4-------5 |
			 -2.0, -2.0, -2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  0.0, 2.0,//0	//	  | |     | |
			 -2.0, 2.0, 2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  0.0, 2.0,	 //7    //	  | 3-----|-2
			 2.0 , 2.0, 2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  2.0, 2.0,	 //6    //    |/      |/
			 2.0, 2.0, -2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  2.0, 0.0,	 //5    //    0-------1
			 -2.0, 2.0, -2.0, 0.0, 0.0,  2.0,  2.0, 0.0, 0.0,  0.0, 0.0, //4
		};

		GLuint face[] = {
			//right 
			1, 2, 6,
			6, 5, 1,
			//left
			0,4,7,
			7,3,0,
			//top
			4,5,6,
			6,7,4,
			//bottom
			0,3,2,
			2,1,0,
			//back
			0,1,5,
			5,4,0,
			//front
			3,7,6,
			6,2,3
		};

		// Create OpenGL buffers and copy data
		GLuint vbo, ebo;

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 8 * 11 * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * 3 * sizeof(GLuint), face, GL_STATIC_DRAW);

		// Create resource
		AddResource(ResourceType::Mesh, "Skybox", vbo, ebo, 12 * 3);
	}

	void ResourceManager::CreateCylinder(std::string objectName, float height, float circleRadius, int numHeightSamples, int numCircleSamples) {
		// Create a cylinder

		// Number of vertices and faces to be created
		const GLuint vertex_num = numHeightSamples * numCircleSamples + 2; // plus two for top and bottom
		const GLuint face_num = numHeightSamples * (numCircleSamples - 1) * 2 + 2 * numCircleSamples; // two extra rings worth for top and bottom

		// Number of attributes for vertices and faces
		const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		const int face_att = 3; // Vertex indices (3)

		// Data buffers for the shape
		GLfloat* vertex = NULL;
		GLuint* face = NULL;

		GLuint vbo, ebo;

		// Allocate memory for buffers
		try {
			vertex = new GLfloat[vertex_num * vertex_att];
			face = new GLuint[face_num * face_att];
		} catch (std::exception& e) {
			throw e;
		}

		// Create vertices 

		float theta; // Angle for circle
		float h; // height
		float s, t; // parameters zero to one

		glm::vec3 loop_center;
		glm::vec3 vertex_position;
		glm::vec3 vertex_normal;
		glm::vec3 vertex_color;
		glm::vec2 vertex_coord;

		for (int i = 0; i < numHeightSamples; i++) { // along the side
			s = i / (float)numHeightSamples; // parameter s (vertical)
			h = (-0.5 + s) * height;

			for (int j = 0; j < numCircleSamples; j++) { // small circle
				t = j / (float)numCircleSamples;
				theta = 2.0 * glm::pi<GLfloat>() * t; // circle sample (angle theta)

				// Define position, normal and color of vertex
				vertex_normal = glm::vec3(cos(theta), 0.0f, sin(theta));
				vertex_position = glm::vec3(cos(theta) * circleRadius, h, sin(theta) * circleRadius);
				vertex_color = glm::vec3(1.0 - s, t, s);
				vertex_coord = glm::vec2(s, t);

				// Add vectors to the data buffer
				for (int k = 0; k < 3; k++) {
					vertex[(i * numCircleSamples + j) * vertex_att + k] = vertex_position[k];
					vertex[(i * numCircleSamples + j) * vertex_att + k + 3] = vertex_normal[k];
					vertex[(i * numCircleSamples + j) * vertex_att + k + 6] = vertex_color[k];
				}

				vertex[(i * numCircleSamples + j) * vertex_att + 9] = vertex_coord[0];
				vertex[(i * numCircleSamples + j) * vertex_att + 10] = vertex_coord[1];
			}
		}

		int topvertex = numCircleSamples * numHeightSamples;
		int bottomvertex = numCircleSamples * numHeightSamples + 1; // indices for top and bottom vertex

		vertex_position = glm::vec3(0, height * (numHeightSamples - 1) / (float)numHeightSamples - height * 0.5, 0); // location of top middle of cylinder
		vertex_normal = glm::vec3(0, 1, 0);
		vertex_color = glm::vec3(1, 0.6, 0.4);
		vertex_coord = glm::vec2(0, 0); // no good way to texture top and bottom

		for (int k = 0; k < 3; k++) {
			vertex[topvertex * vertex_att + k] = vertex_position[k];
			vertex[topvertex * vertex_att + k + 3] = vertex_normal[k];
			vertex[topvertex * vertex_att + k + 6] = vertex_color[k];
		}

		vertex[(topvertex)*vertex_att + 9] = vertex_coord[0];
		vertex[(topvertex)*vertex_att + 10] = vertex_coord[1];

		//================== bottom vertex

		vertex_position = glm::vec3(0, -0.5 * height, 0); // location of top middle of cylinder
		vertex_normal = glm::vec3(0, -1, 0);

		// leave the color and uv alone

		for (int k = 0; k < 3; k++) {
			vertex[bottomvertex * vertex_att + k] = vertex_position[k];
			vertex[bottomvertex * vertex_att + k + 3] = vertex_normal[k];
			vertex[bottomvertex * vertex_att + k + 6] = vertex_color[k];
		}

		vertex[(bottomvertex)*vertex_att + 9] = vertex_coord[0];
		vertex[(bottomvertex)*vertex_att + 10] = vertex_coord[1];

		//===================== end of vertices

		// Create triangles
		for (int i = 0; i < numHeightSamples - 1; i++) {
			for (int j = 0; j < numCircleSamples; j++) {
				// Two triangles per quad
				glm::vec3 t1(((i + 1) % numHeightSamples) * numCircleSamples + j,
					i * numCircleSamples + ((j + 1) % numCircleSamples),
					i * numCircleSamples + j);
				glm::vec3 t2(((i + 1) % numHeightSamples) * numCircleSamples + j,
					((i + 1) % numHeightSamples) * numCircleSamples + ((j + 1) % numCircleSamples),
					i * numCircleSamples + ((j + 1) % numCircleSamples));
				// Add two triangles to the data buffer
				for (int k = 0; k < 3; k++) {
					face[(i * numCircleSamples + j) * face_att * 2 + k] = (GLuint)t1[k];
					face[(i * numCircleSamples + j) * face_att * 2 + k + face_att] = (GLuint)t2[k];
				}
			}
		}

		int cylbodysize = numCircleSamples * (numHeightSamples - 1) * 2; // amount of array filled so far, start adding from here
		// triangles for top disc (fan shape)
		int i = numHeightSamples - 1;

		for (int j = 0; j < numCircleSamples; j++) {
			// Bunch of wedges pointing to the centre
			glm::vec3 topwedge(
				i * numCircleSamples + j,
				topvertex,
				i * numCircleSamples + (j + 1) % numCircleSamples
			);

			// note order reversed so that all triangles point outward
			glm::vec3 botwedge(
				0 + (j + 1) % numCircleSamples,
				bottomvertex,
				0 + j
			);

			// Add the triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(cylbodysize + j) * face_att + k] = (GLuint)topwedge[k];
				face[(cylbodysize + j + numCircleSamples) * face_att + k] = (GLuint)botwedge[k];
			}
		}

		// Create OpenGL buffer for vertices
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

		// Create OpenGL buffer for faces
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

		// Free data buffers
		delete[] vertex;
		delete[] face;

		// Create resource
		AddResource(ResourceType::Mesh, objectName, vbo, ebo, face_num * face_att);
	}

	void ResourceManager::CreateFountainParticles(std::string object_name, int num_particles) {
		// Create a set of points which will be the particles

		// Data buffer
		GLfloat* particle = NULL;

		// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
		const int particle_att = 11;

		// Allocate memory for buffer
		try {
			particle = new GLfloat[num_particles * particle_att];
		} catch (std::exception& e) {
			throw e;
		}

		float trad = 0.2; // Defines the starting point of the particles along the normal
		float u, v, theta, phi; // Work variables

		for (int i = 0; i < num_particles; i++) {
			// Get three random numbers

			u = ((double)rand() / (RAND_MAX));
			v = ((double)rand() / (RAND_MAX));

			// Use u to define the angle theta along one direction of the sphere
			theta = u * 2.0 * glm::pi<float>();
			// Use v to define the angle phi along the other direction of the sphere
			phi = acos(2.0 * v - 1.0);

			// Define the normal and point based on theta and phi
			glm::vec3 normal(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));
			glm::vec3 position(normal.x * trad, normal.y * trad, normal.z * trad);
			glm::vec3 color((float)i / num_particles); // Using color to store phase

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				particle[i * particle_att + k] = position[k];
				particle[i * particle_att + k + 3] = normal[k];
				particle[i * particle_att + k + 6] = color[k];
			}
		}

		// Create OpenGL buffer and copy data

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, num_particles * particle_att * sizeof(GLfloat), particle, GL_STATIC_DRAW);

		// Free data buffers
		delete[] particle;

		AddResource(ResourceType::PointSet, object_name, vbo, 0, num_particles);
	}

	void ResourceManager::CreateMonsterParticles(int num_particles) {
		// Data buffer
		GLfloat* particle = NULL;

		// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
		const int particle_att = 11;

		// Allocate memory for buffer
		try {
			particle = new GLfloat[num_particles * particle_att];
		} catch (std::exception& e) {
			throw e;
		}

		for (int i = 0; i < num_particles; i++) {

			//idea is to have a bunch of particles swarming around a point

			//firstly, make random x,y, and z points on the surface of a sphere

			float twopi = 2 * glm::pi<float>();

			float r1 = ((double)rand() / (RAND_MAX));
			float r2 = ((double)rand() / (RAND_MAX));

			float lat = acos((2 * r1) - 1) - twopi;
			float lon = twopi * r2;

			float x = (cos(lat) * cos(lon)) * 0.2;
			float y = (cos(lat) * sin(lon)) * 0.2;
			float z = (sin(lat)) * 0.2;

			//do nothing with normal since motion can be determined with simple trig
			glm::vec3 normal(0, 0, 0);
			glm::vec3 position(x, y, z);
			//use color x for offset
			glm::vec3 color(i, 0, 0);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				particle[i * particle_att + k] = position[k];
				particle[i * particle_att + k + 3] = normal[k];
				particle[i * particle_att + k + 6] = color[k];
			}
		}

		// Create OpenGL buffer and copy data
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, num_particles * particle_att * sizeof(GLfloat), particle, GL_STATIC_DRAW);

		// Free data buffers
		delete[] particle;

		// Create resource
		AddResource(ResourceType::PointSet, "MonsterParticles", vbo, 0, num_particles);
	}

	void ResourceManager::CreateLeafParticles(int num_particles) {
		// Data buffer
		GLfloat* particle = NULL;

		// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
		const int particle_att = 11;

		// Allocate memory for buffer
		try {
			particle = new GLfloat[num_particles * particle_att];
		} catch (std::exception& e) {
			throw e;
		}

		for (int i = 0; i < num_particles; i++) {

			//get random numbers for x and z offset position on the disc, 
			//the y is statically determined based off camera position.
			//Math is done assuming center of circle is at (0,0) and radius is 0.5

			//get random points in flat circle

			double r = 5 * sqrt(((double)rand() / (RAND_MAX)));
			float theta = ((double)rand() / (RAND_MAX)) * 2 * glm::pi<float>();

			double x, z;

			x = r * cos(theta);
			z = r * sin(theta);

			// Normal will be straight down, since leaves are gonna be falling in downward half-arcs, rest of math handled in vertex shader
			glm::vec3 normal(0, -1, 0);
			//y position is gonna be a small interval above the camera, and then the random x and z coordinates
			float y = ((double)rand() / (RAND_MAX)) * 0.5;
			glm::vec3 position(x, y, z);
			//use value of color vec3 for offset
			glm::vec3 color(i, 0, 0);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				particle[i * particle_att + k] = position[k];
				particle[i * particle_att + k + 3] = normal[k];
				particle[i * particle_att + k + 6] = color[k];
			}
		}

		// Create OpenGL buffer and copy data
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, num_particles * particle_att * sizeof(GLfloat), particle, GL_STATIC_DRAW);

		// Free data buffers
		delete[] particle;

		// Create resource
		AddResource(ResourceType::PointSet, "LeafParticles", vbo, 0, num_particles);
	}

	void ResourceManager::CreateLineParticles(std::string object_name, int num_particles) {
		// Create a set of points which will be the particles
		// This is similar to drawing a line: we will sample points on a line

		// Data buffer
		GLfloat* particle = NULL;

		// Number of attributes per particle: position (3), normal (3), and color (3), texture coordinates (2)
		const int particle_att = 11;

		// Allocate memory for buffer
		try {
			particle = new GLfloat[num_particles * particle_att];
		} catch (std::exception& e) {
			throw e;
		}

		for (int i = 0; i < num_particles; i++) {
			glm::vec3 position(0.0, 2.0 - (i / (float)num_particles) * 4.0, 0.0);
			glm::vec3 normal(0.0, 0.0, 0.0);
			glm::vec3 color(i, 0.0, 0.0);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				particle[i * particle_att + k] = position[k];
				particle[i * particle_att + k + 3] = normal[k];
				particle[i * particle_att + k + 6] = color[k];
			}
		}

		// Create OpenGL buffer and copy data
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, num_particles * particle_att * sizeof(GLfloat), particle, GL_STATIC_DRAW);

		// Free data buffers
		delete[] particle;

		// Create resource
		AddResource(ResourceType::PointSet, object_name, vbo, 0, num_particles);
	}

	float ResourceManager::GetTerrainHeightAt(float x, float y) {
		if (x < 0.0f || x >= MAP_SIZE * 2.0f || y < 0.0f || y >= MAP_SIZE * 2.0f) {
			return 0.0f;
		}

		int x1 = glm::floor(x);
		int x2 = glm::ceil(x);

		int y1 = glm::floor(y);
		int y2 = glm::ceil(y);

		float h1 = heights[x1][y1] * (1 - (x - x1)) + heights[x2][y1] * (x - x1);
		float h2 = heights[x1][y2] * (1 - (x - x1)) + heights[x2][y2] * (x - x1);

		return h1 * (1 - (y - y1)) + h2 * (y - y1);
	}

	bool ResourceManager::GetMazeCollisions(int x, int y, glm::vec3 position) {
		if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
			return true;
		}

		if (collisions[x][y] && position.x > (x * 2.0f - 1.1f) && position.x < (x * 2.0f + 1.1f) && position.z >(y * 2.0f - 1.1f) && position.z < (y * 2.0f + 1.1f)) {
			return true;
		}

		return false;
	}
}