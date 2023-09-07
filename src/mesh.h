#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include "shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture
{
	std::string type;
	unsigned int id;
	std::string path;
};

class Mesh
{
public:
	// Constructors and Destructor
	Mesh() = delete;  // Deleted default constructor
	Mesh(const std::vector<Vertex>& vertices,
		const std::vector<unsigned int>& indices,
		const std::vector<Texture>& textures,
		bool hasTangentAndBitangent);  // Parameterized constructor
	~Mesh();  // Destructor

	// Move Semantics
	Mesh(Mesh&& other) noexcept;  // Move constructor
	Mesh& operator=(Mesh&& other) noexcept;  // Move assignment operator

	// Deleted Copy Semantics
	// Prevent copying as this class manages OpenGL resources
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// Public Methods
	void Draw(Shader& shader) const;  // Draw the mesh

	// Accessors
	unsigned int GetVAO() { return VAO; }
	const unsigned int GetVAO() const { return VAO; }

	// Public Members
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

private:
	// Private Methods
	void SetupMesh();  // Initialize OpenGL objects

	// Private Members
	unsigned int VAO, VBO, IBO;
	bool hasTangentAndBitangent = false;
};

Mesh::Mesh(const std::vector<Vertex>& _vertices, 
	const std::vector<unsigned int>& _indices, 
	const std::vector<Texture>& _textures,
	bool _hasTangentAndBitangent)
{
	this->vertices = _vertices;
	this->indices = _indices;
	this->textures = _textures;
	this->hasTangentAndBitangent = _hasTangentAndBitangent;
#ifdef _DEBUG
	if (hasTangentAndBitangent) {
		std::cout << "Mesh has tangents and bitangents.\n";
	}
	else {
		std::cout << "Mesh does not have tangents and bitangents.\n";
	}
#endif
	SetupMesh();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
}

// Move constructor
Mesh::Mesh(Mesh&& other) noexcept
	: VAO(other.VAO), VBO(other.VBO), IBO(other.IBO),
	vertices(std::move(other.vertices)), indices(std::move(other.indices)),
	textures(std::move(other.textures)), hasTangentAndBitangent(other.hasTangentAndBitangent)
{
	// Invalidate the moved-from object's OpenGL handles
	other.VAO = 0;
	other.VBO = 0;
	other.IBO = 0;
}

// Move assignment operator
Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		// Release any resources held by *this
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &IBO);

		// Steal the resources from other
		VAO = other.VAO;
		VBO = other.VBO;
		IBO = other.IBO;
		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
		textures = std::move(other.textures);
		hasTangentAndBitangent = other.hasTangentAndBitangent;

		// Invalidate the moved-from object's OpenGL handles
		other.VAO = 0;
		other.VBO = 0;
		other.IBO = 0;
	}
	return *this;
}
// first calculate the N-component per texture type and concatenate it to the texture's type string to get the uniform name. 
// then locate the appropriate sampler, give it the location value to correspond with the currently active texture unit, and bind the texture. 
void Mesh::Draw(Shader& shader) const
{
	// Start from material.diffuse1 or material.specular1
	size_t diffuseNr = 1, specularNr = 1, normalNr = 1, heightNr = 1;

	for (size_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); 
		// Get texture number��N in diffuse_textureN ��
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		shader.SetInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	//glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	// VAO, VBO, and IBO(EBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// TexCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	if (hasTangentAndBitangent) {
		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	}
	// Unbind VAO
	glBindVertexArray(0);
}