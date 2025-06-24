#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cstring>
#include <vector>

#include "model.h"
#include "mesh.h"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION //oml this one line kills me every time
#include "stb_image.h"

static unsigned int textureFromFile(std::string fName, std::string directory);

void Model::Draw(Shader &shader){
  for (Mesh mesh : meshes){
    mesh.Draw(shader);
  }
}

void Model::loadModel(std::string path){
  Assimp::Importer importer;
  const aiScene *scene = 
    importer.ReadFile(path,
                      aiProcess_Triangulate |
                      aiProcess_GenSmoothNormals |
                      aiProcess_FlipUVs |
                      aiProcess_CalcTangentSpace);

  if (!scene
    || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
    || !scene->mRootNode){
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  
  }
  //eg: proj/models/foo.obj -> proj/models
  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
} 

void Model::processNode(aiNode *aiNode, const aiScene *scene){
  for (size_t i{}; i < aiNode->mNumMeshes; ++i){
    //somewhat confusingly, each aiNode contains a list of
    //INDECIES called mMeshes that keys into their corresponding
    //meshes in the SCENE'S mMeshes member.
    aiMesh *aiMesh = scene->mMeshes[aiNode->mMeshes[i]];
    meshes.push_back(processMesh(aiMesh, scene));
  }

  for (size_t i{}; i < aiNode->mNumChildren; ++i){
    processNode(aiNode->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *aiMesh, const aiScene *scene){
  std::vector<Vertex> vertecies;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  //load up our vertecies
  for (size_t i{}; i < aiMesh->mNumVertices; ++i) {
    Vertex vertex;
    //positions
    vertex.Position.x = aiMesh->mVertices[i].x;
    vertex.Position.y = aiMesh->mVertices[i].y;
    vertex.Position.z = aiMesh->mVertices[i].z;
    if (aiMesh->HasNormals()){
      //normals
      vertex.Normal.x = aiMesh->mNormals[i].x;
      vertex.Normal.y = aiMesh->mNormals[i].y;
      vertex.Normal.z = aiMesh->mNormals[i].z;
    }
    if (aiMesh->mTextureCoords[0]) { 
      //texture coords
      vertex.TexCoords.x = aiMesh->mTextureCoords[0][i].x;
      vertex.TexCoords.y = aiMesh->mTextureCoords[0][i].y;
      //tangents
      vertex.Tangent.x = aiMesh->mTangents[i].x;
      vertex.Tangent.y = aiMesh->mTangents[i].y;
      vertex.Tangent.z = aiMesh->mTangents[i].z;
      //bitangents
      vertex.BiTangent.x = aiMesh->mBitangents[i].x;
      vertex.BiTangent.y = aiMesh->mBitangents[i].y;
      vertex.BiTangent.z = aiMesh->mBitangents[i].z;
    } else {
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    }
    vertecies.push_back(vertex);
  }

  //load up our indices
  for (size_t i{}; i < aiMesh->mNumFaces; ++i) {
    //leaving this as a pointer cuz idk how big a face is
    aiFace *face = &(aiMesh->mFaces[i]); 
    for (size_t j{}; j < face->mNumIndices; ++j) {
      indices.push_back(face->mIndices[j]);
    }
  }

  //load up our textures
  if (aiMesh->mMaterialIndex >= 0) { //if textures exist at all
    aiMaterial *aiMat = scene->mMaterials[aiMesh->mMaterialIndex];
    std::vector<Texture> diffuseMap = 
      loadMaterialTextures(aiMat,
                          aiTextureType_DIFFUSE,
                          "texture_diffuse");
    textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());

    std::vector<Texture> specularMap = 
      loadMaterialTextures(aiMat,
                          aiTextureType_SPECULAR,
                          "texture_specular");
    textures.insert(textures.end(), specularMap.begin(), specularMap.end());

    std::vector<Texture> normalMap = 
      loadMaterialTextures(aiMat,
                          aiTextureType_HEIGHT,//for map_Bump types in mtl file
                          "texture_normal");
    textures.insert(textures.end(), normalMap.begin(), normalMap.end());

  }
  return Mesh(vertecies, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                aiTextureType type,
                                                std::string typeName) {
  std::vector<Texture> textures;
  for (size_t i{}; i < mat->GetTextureCount(type); ++i) {
    aiString texFPath;
    mat->GetTexture(type, i, &texFPath); 
    bool loaded{false};
    //check if texture has been loaded before (optimization)
    for (size_t j{}; j < texturesLoaded.size(); ++j){
      if (std::strcmp(texturesLoaded[j].fName.c_str(), texFPath.C_Str()) == 0){
        textures.push_back(texturesLoaded[j]);
        loaded = true;
        break;
      }
    }

    if (!loaded) {
      Texture tex;
      tex.id = textureFromFile(texFPath.C_Str(), directory);
      tex.type = typeName;
      tex.fName = texFPath.C_Str();
      textures.push_back(tex);
      texturesLoaded.push_back(tex);
    }
  }
  return textures;
}

//loads a texture and returns its id
static unsigned int textureFromFile(std::string fName, std::string directory) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  std::string path = (directory + '/' + fName);
  std::cout << path << std::endl;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels,
                                 0);
  if (data) {
    GLenum format;
    switch (nrChannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;

}
