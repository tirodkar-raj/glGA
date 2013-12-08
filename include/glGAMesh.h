//
//  glGAMesh.h
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2012 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#ifndef glGACharacterApp_glGAMesh_h
#define glGACharacterApp_glGAMesh_h



#include <map>
#include <vector>
#include <string>
#include <cassert>                  //includes C assert.h
// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/Logger.hpp>        // Logging facility
#include <assimp/DefaultLogger.hpp>
// GLM
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
//GLEW
#include <GL/glew.h>

#include "glGAHelper.h"

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

// Basic vertex data structure
class Vertex
{
 
public: 
    
    glm::vec3   m_pos;
    glm::vec2   m_tex;
    glm::vec3   m_normal;
    
    Vertex() {}
    
    Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
    {
        m_pos       = pos;
        m_tex       = tex;
        m_normal    = normal;
    }
    
};//end class glGAVertex

// Basic static mesh data structure for loading any object static mesh with the Assimp library
class Mesh
{
public:
    Mesh();
    
    ~Mesh();
    
    bool        loadMesh(const std::string& filename);
    
    void        render();
    
    //GLuint                      m_Buffers[4];
    //GLuint                      m_VAO;
    std::vector<Texture*>       m_Textures;
    std::vector<GLuint>         m_TextureSamplers;
    std::vector<glm::vec3>      Positions;
    std::vector<glm::vec3>      Normals;
    std::vector<glm::vec2>      TexCoords;
	std::vector<glm::vec3>		Tangents;
    std::vector<unsigned int>   Indices;
    
    unsigned int numVertices ;
    unsigned int numIndices ;
    
private:
    bool initFromScene(const aiScene* pScene, const std::string& filename);
    void initMesh(const aiMesh* paiMesh,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec2>& texCoords,
                  std::vector<unsigned int>& indices,
				  std::vector<glm::vec3>& tangents
                  );
    bool initMaterials(const aiScene* pScene, const std::string& filename);
    void clear();

    
#define INVALID_MATERIAL    0xFFFFFFFF
#define INDEX_BUFFER        0
#define POS_VB              1
#define NORMAL_VB           2
#define TEXCOORD_VB         3
#define TANGENT_VB			4
    
    
    class  MeshEntry
    {
    public:
        MeshEntry()
        {
            numIndices      = 0;
            baseVertex      = 0;
            baseIndex       = 0;
            materialIndex   = INVALID_MATERIAL;
        }
        
        unsigned int numIndices;
        unsigned int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;
    };//MeshEntry
    
    std::vector<MeshEntry>  m_Entries;
   
}; // end class Mesh





#endif //glGACharacterApp_glGAMesh_h
