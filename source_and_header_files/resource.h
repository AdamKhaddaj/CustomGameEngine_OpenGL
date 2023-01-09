#ifndef RESOURCE_H_
#define RESOURCE_H_

#define GLEW_STATIC

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Game {
	typedef enum class ResourceType { Material, PointSet, Mesh, Texture };

	class Resource {

	public:
		Resource(ResourceType type, std::string name, GLuint resource, GLsizei size);
		Resource(ResourceType type, std::string name, GLuint arrayBuffer, GLuint elementArrayBuffer, GLsizei size);
		~Resource();

		ResourceType GetType() const;
		const std::string GetName() const;
		GLuint GetResource() const;
		GLuint GetArrayBuffer() const;
		GLuint GetElementArrayBuffer() const;
		GLsizei GetSize() const;

	private:
		ResourceType type;
		std::string name;

		union {
			struct {
				GLuint resource;
			};

			struct {
				GLuint arrayBuffer;
				GLuint elementArrayBuffer;
			};
		};

		GLsizei size;
	};
}

#endif