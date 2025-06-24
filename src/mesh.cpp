#include <glad/glad.h>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "mesh.h"
#include "shader.h"


Mesh::Mesh(std::vector<Vertex> vertecies,
           std::vector<unsigned int> indices,
           std::vector<Texture> textures) { 
  //TODO: make sure this is move constructed
  this->vertecies = vertecies;
  this->indices = indices;
  this->textures = textures;
  
  setupMesh();
}

void Mesh::Draw(Shader &shader) const {
  unsigned int diffuseNr, specularNr, normalNr{1};
  for (unsigned int i{}; i < textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number;
    std::string name = textures[i].type;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNr++); //note incr. order
    }
    else if (name == "texture_specular") {
      number = std::to_string(specularNr++); //note incr. order
    }
    else if (name == "texture_normal") {
      number = std::to_string(normalNr++); //note incr. order
    }

    shader.setInt((name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0); //optional: reset texture unit (good practice)
}


void Mesh::setupMesh(){
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  //naming is a bit missleading here;; Vertex contains position,
  //normal, and texturecoord information. Hence, the size calculation
  //is the number of these triplets times the size of a triplet
  //(which is correct).
  //TODO: may need to make draw type configurable in future
  glBufferData(GL_ARRAY_BUFFER, vertecies.size() * sizeof(Vertex),
               vertecies.data(), GL_STATIC_DRAW); 
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW); 

  // vertex data
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  // normals data
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)offsetof(Vertex, Normal));
  // TexCoord data
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)offsetof(Vertex, BiTangent));
  glBindVertexArray(0);
}

