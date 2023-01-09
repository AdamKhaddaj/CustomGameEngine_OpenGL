#include <exception>
#include "resource.h"

namespace Game {
	Resource::Resource(ResourceType type, std::string name, GLuint resource, GLsizei size) {
		Resource::type = type;
		Resource::name = name;
		Resource::resource = resource;
		Resource::size = size;
	}

	Resource::Resource(ResourceType type, std::string name, GLuint arrayBuffer, GLuint elementArrayBuffer, GLsizei size) {
		Resource::type = type;
		Resource::name = name;
		Resource::arrayBuffer = arrayBuffer;
		Resource::elementArrayBuffer = elementArrayBuffer;
		Resource::size = size;
	}

	Resource::~Resource() {}

	ResourceType Resource::GetType() const {
		return type;
	}

	const std::string Resource::GetName() const {
		return name;
	}

	GLuint Resource::GetResource() const {
		return resource;
	}

	GLuint Resource::GetArrayBuffer() const {
		return arrayBuffer;
	}

	GLuint Resource::GetElementArrayBuffer() const {
		return elementArrayBuffer;
	}

	GLsizei Resource::GetSize() const {
		return size;
	}
}