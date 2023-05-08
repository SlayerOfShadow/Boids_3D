#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../src-common/glimac/common.hpp"

std::vector<glimac::ShapeVertex> loadObjFile(const std::string& filePath)
{
    std::vector<glimac::ShapeVertex> vertices;

    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return vertices;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            // Vertex position
            std::istringstream ss(line.substr(2));
            glm::vec3          position;
            ss >> position.x >> position.y >> position.z;
            vertices.push_back({position, {}, {}});
        }
        else if (line.substr(0, 2) == "vn")
        {
            // Vertex normal
            std::istringstream ss(line.substr(2));
            glm::vec3          normal;
            ss >> normal.x >> normal.y >> normal.z;
            if (!vertices.empty())
            {
                vertices.back().normal = normal;
            }
        }
        else if (line.substr(0, 2) == "vt")
        {
            // Texture coordinate
            std::istringstream ss(line.substr(2));
            glm::vec2          texCoord;
            ss >> texCoord.x >> texCoord.y;
            if (!vertices.empty())
            {
                vertices.back().texCoords = texCoord;
            }
        }
        else if (line.substr(0, 2) == "f ")
        {
            // Face
            std::istringstream ss(line.substr(2));
            std::string        token;
            while (std::getline(ss, token, ' '))
            {
                std::istringstream fs(token);
                std::string        vertexIndexStr, texCoordIndexStr, normalIndexStr;
                std::getline(fs, vertexIndexStr, '/');
                std::getline(fs, texCoordIndexStr, '/');
                std::getline(fs, normalIndexStr, '/');
                int vertexIndex                 = std::stoi(vertexIndexStr) - 1;
                int texCoordIndex               = std::stoi(texCoordIndexStr) - 1;
                int normalIndex                 = std::stoi(normalIndexStr) - 1;
                vertices[vertexIndex].normal    = vertices[normalIndex].normal;
                vertices[vertexIndex].texCoords = vertices[texCoordIndex].texCoords;
            }
        }
    }

    inFile.close();
    return vertices;
}