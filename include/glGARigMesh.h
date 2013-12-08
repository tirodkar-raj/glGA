//
//  glGARigMesh.h
//  
//
//  Created by George Papagiannakis
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#ifndef glGACharacterApp_glGARigMesh_h
#define glGACharacterApp_glGARigMesh_h



#include <map>
#include <vector>
#include <string>
#include <cassert>                  //includes C assert.h
// Assimp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/anim.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/Logger.hpp>        // Logging facility
#include <assimp/DefaultLogger.hpp>
#include <assimp/matrix4x4.h>

//GLEW
#include <GL/glew.h>


#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>


#include "glGAHelper.h"



// Basic rigged mesh data structure for loading any object animated and rigged mesh with the Assimp library
class RigMesh
{
public:
    RigMesh();
    
    ~RigMesh();
    
    bool        loadRigMesh(const std::string& filename);
    
    void        render();
    
    glm::uint numBones() const
    {
        return m_NumBones;
    }
    
    glm::mat4 boneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms);
    
    /*
     * ASSIMP uses row-major 4x4 matrices whereas OpenGL and 
     * GLM assume column major 4x4 matrices.
     ASSIMP:                   GLM:
     a1 a2 a3 a4            a1 b1 c1 d1
     b1 b2 b3 b4            a2 b2 c2 d2
     c1 c2 c3 c4            a3 b3 c3 d3
     d1 d2 d3 d4            a4 b4 c4 d4
     */
    void aiMat2glmMat(const aiMatrix4x4 *from, glm::mat4 &to) {
        to[0][0] = from->a1; to[1][0] = from->a2;
        to[2][0] = from->a3; to[3][0] = from->a4;
        to[0][1] = from->b1; to[1][1] = from->b2;
        to[2][1] = from->b3; to[3][1] = from->b4;
        to[0][2] = from->c1; to[1][2] = from->c2;
        to[2][2] = from->c3; to[3][2] = from->c4;
        to[0][3] = from->d1; to[1][3] = from->d2;
        to[2][3] = from->d3; to[3][3] = from->d4;
    }
    

#define NUM_BONES_PER_VERTEX    4
#define MAX_BONES               100
    
#ifndef INDEX_BUFFER
#define INDEX_BUFFER        0
#endif
#ifndef POS_VB
#define POS_VB              1
#endif
#ifndef NORMAL_VB
#define NORMAL_VB           2
#endif
#ifndef TEXCOORD_VB
#define TEXCOORD_VB         3
#endif
    
    class BoneInfo
    {
    public:
        glm::mat4   BoneOffset;
        glm::mat4   FinalTransformation;
        
        BoneInfo()
        {
            BoneOffset = glm::mat4( 0.0 ); // initialize matrix to zero
            FinalTransformation = glm::mat4( 0.0 );
        }
        
    };
    
    // Private class containing for a single vertex, an array of 4 Bone IDs and 4 Weights
    class VertexBoneData
    {
    public:
        float IDs[NUM_BONES_PER_VERTEX];
        float Weights[NUM_BONES_PER_VERTEX];
        
        VertexBoneData()
        {
            reset();
        };
        
        void reset()
        {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }
        
        void addBoneData(glm::uint boneID, float weight);
    };
    
private:    
    void calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim);
    void calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
    glm::uint findScaling(float animationTime, const aiNodeAnim* pNodeAnim);
    glm::uint findRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    glm::uint findPosition(float animationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string nodeName);
    void readNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform);
    
    bool initFromScene(const aiScene* pScene, const std::string& filename);
    void initRigMesh(glm::uint RigMeshIndexB,
                     const aiMesh* paiMesh,
                     std::vector<glm::vec3>& positions,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec2>& texCoords,
                     std::vector<VertexBoneData>& bones,
                     std::vector<unsigned int>& indices
                  );
    void loadBones(glm::uint meshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& bones);
    bool initMaterials(const aiScene* pScene, const std::string& filename);
    void clear();
    
    
#define INVALID_MATERIAL    0xFFFFFFFF

    class  RigMeshEntry
    {
    public:
        RigMeshEntry()
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
  
public:
    
    std::vector<glm::vec3>          Positions;
    std::vector<glm::vec3>          Normals;
    std::vector<glm::vec2>          TexCoords;
    std::vector<VertexBoneData>     Bones;
    std::vector<unsigned int>       Indices;
    
    enum VBO_TYPES
    {
        INDEX_VBO,
        POS_VBO,
        NORMAL_VBO,
        TEXCOORD_VBO,
        BONE_VBO,
        NUM_VBOs
    };
    
//private:
    std::vector<RigMeshEntry>       m_Entries;
    std::vector<Texture*>           m_Textures;
    
    std::map<std::string, glm::uint>     m_BoneMapping; //maps a bone name to its bone index
    glm::uint                            m_NumBones;
    std::vector<BoneInfo>           m_BoneInfo;
    glm::mat4                       m_GlobalInverseTransform;
    bool                            m_useGAforInterpolation;
    float                           m_GAfactorDenominator;
    const aiScene*                  m_pScene;
    //Assimp::Importer                m_Importer;
    Assimp::Importer*                m_Importer; //a pointer to Importer is crucial otherwise it goes out of scope and invalidates aiScene
    
}; // end class RigMesh


#endif //glGACharacterApp_glGARigMesh_h
