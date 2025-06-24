#ifndef MODEL_HEADER
#define MODEL_HEADER


#include <assimp/scene.h>
#include <vector>

#include "mesh.h"
#include "shader.h"

class Model {
public:
  Model(std::string path){
    loadModel(path);
  }

  void Draw(Shader &shader);

private: 
  std::vector<Mesh> meshes; //processed meshes (not assimp's)
  std::string directory;
  std::vector<Texture> texturesLoaded; //going with a vector here over a 
  //hashmap because the total number of textures ever loaded at a time is small
  //enough to where a linear search over a vector is more efficient than a 
  //hashtable lookup

  void loadModel(std::string path);
  void processNode(aiNode *aiNode, const aiScene *scene);
  Mesh processMesh(aiMesh *aiMesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                            aiTextureType type,
                                            std::string typeName);
};
#endif
