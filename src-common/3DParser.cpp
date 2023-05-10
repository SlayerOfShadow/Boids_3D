#include "3DParser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

std::vector<glimac::ShapeVertex> loadObjFile(const char* filename)
{
    std::vector<glimac::ShapeVertex> vertices;
    std::ifstream                    file(filename);
    if (!file.is_open())
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::string            line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string       type;
        ss >> type;
        if (type == "v")
        {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (type == "vn")
        {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (type == "vt")
        {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (type == "f")
        {
            std::string         vertexStr;
            glimac::ShapeVertex vertex;
            for (int i = 0; i < 3; ++i) // Only parse triangles (3 vertices per face)
            {
                ss >> vertexStr;
                std::stringstream vss(vertexStr);
                int               posIndex, texIndex, normalIndex;
                char              slash;
                vss >> posIndex >> slash >> texIndex >> slash >> normalIndex;
                vertex.position  = positions[posIndex - 1];
                vertex.normal    = normals[normalIndex - 1];
                vertex.texCoords = texCoords[texIndex - 1];
                vertices.push_back(vertex);
            }
        }
    }
    file.close();
    return vertices;
}