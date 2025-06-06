#pragma once
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

#include <string>
#include <vector>

using namespace std;

class Mesh
{
public:
    unsigned int numVertices;
    std::shared_ptr <Vertex> vertices;
    std::string name;

    unsigned int numIndexes;
    std::shared_ptr <unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

    Mesh(std::string name, const vector<Vertex>& vertices, const vector<unsigned int>& indices, const vector<Texture>& textures);
    Mesh(std::string name, unsigned int numVertices, std::shared_ptr <Vertex> vertices, unsigned int numIndexes, std::shared_ptr <unsigned int> indices, const vector<Texture>& textures);
    void Draw(Shader& shader);
private:
    unsigned int VBO, EBO;
    void setupMesh();
};

