#ifndef MESH_HEADER
#define MESH_HEADER
#include <glad/glad.h>
#include <vector>

#include "glm/glm.hpp"
#include "shader.h"

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 Tangent;
  glm::vec3 BiTangent;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string fName;
};

class Mesh {
public:
  std::vector<Vertex>       vertecies;
  std::vector<unsigned int> indices;
  std::vector<Texture>      textures;

  Mesh(std::vector<Vertex>       vertecies,
       std::vector<unsigned int> indices,
       std::vector<Texture>      textures);
  void Draw(Shader &shader) const;

private:
  unsigned int VBO, VAO, EBO;
  void setupMesh();
};
#endif
