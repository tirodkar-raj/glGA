//
//  glGARigMesh.cpp
//  
//
//  Created by George Papagiannakis
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//
//  References: 
//      http://ogldev.atspace.co.uk/, Etay Meiri

#include <iostream>
#include <cassert>

#include "glGARigMesh.h"

// GLM 

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

/*
 This utility function finds a free slot in the VertexBoneData structure and places the bone ID and weight in it. Some vertices will be influenced by less than four bones but since the weight of a non existing bone remains zero (see the constructor of VertexBoneData) it means that we can use the same weight calculation for any number of bones.
 */
void RigMesh::VertexBoneData::addBoneData(glm::uint boneID, float weight)
{
    for (glm::uint i=0; i< ARRAY_SIZE_IN_ELEMENTS(IDs); ++i){
        if (Weights[i] == 0.0) {
            IDs[i]  = (float)boneID;
            Weights[i] = weight;
            return;
        }
    }//end for
    
    // should not reach here - means we have more bones than allocated space
    assert(0);
}

RigMesh::RigMesh()
{
    m_NumBones = 0;
    m_pScene = NULL;
    m_Textures.clear();
    m_Textures.resize(0);
    m_useGAforInterpolation = false;
    m_GAfactorDenominator = 2.0;
}

RigMesh::~RigMesh()
{
    clear();
}

void RigMesh::clear()
{
    for (unsigned int i=0; i<m_Textures.size(); ++i) {
        SAFE_DELETE(m_Textures[i]);
    }
    m_Textures.clear();
    m_Textures.resize(0);
    
    //delete m_pScene;
    //m_pScene = NULL;
    /*delete m_Importer;
	m_Importer = NULL;*/
}

//end clear()

/*
    Basic interface method called from externals as entry method in RigMesh:
    Creates: aiScene* m_pScene object
    Calls: initFromScene()
 */
bool RigMesh::loadRigMesh(const std::string& filename )
{
    //release a previously loaded RigMesh (if it exists)
    clear();
    
    GLExitIfError();
    
    bool Ret = false;
    //create an instance of the importer class, very important to create it on heap and NOT on stack, so that it persists after execution leaves this class and might be accessed later, after load is complete
    Assimp::Importer* importer = new Assimp::Importer();;

    Assimp::DefaultLogger::create( ASSIMP_DEFAULT_LOG_NAME,
                                  Assimp::Logger::VERBOSE,
                                  aiDefaultLogStream_DEBUGGER|aiDefaultLogStream_FILE,
                                  NULL );
    
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example:
    // aiProcess_CalcTangentSpace       | 
    // aiProcess_Triangulate            |
    // aiProcess_JoinIdenticalVertices  |
    // aiProcess_SortByPType |
    // aiProcess_FlipUVs
    m_pScene = importer->ReadFile( filename.c_str(), 
                                 aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (m_pScene) {
        //m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
        aiMat2glmMat(&(m_pScene->mRootNode->mTransformation),m_GlobalInverseTransform);
        m_GlobalInverseTransform=glm::inverse(m_GlobalInverseTransform);
        Ret = initFromScene(m_pScene, filename);
    }
    else
    {
        std::cout<<"Error parsing "<< filename.c_str() <<": "<<importer->GetErrorString() <<std::endl;
    }
    
    return Ret;
}//end loadMesh()

/*
    Called by: RigMesh::loadRigMesh
    Creates:
        - one RigMeshEntry for every Mesh in the Scene
            - each RigMeshEntry stores the numIndices, baseVertex, baseIndex and materialIndex
        - one Texture for every material in the Scene
        - aiMesh* paiMesh for every mesh in the scene
    Calls:
        - initRigMesh() for every mesh in the scene
        - initMaterials()
 */

bool RigMesh::initFromScene(const aiScene* pScene, const std::string& filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
    
    glm::uint numVertices = 0;
    glm::uint numIndices = 0;
    
    // count the num of vertices and indices
    for (unsigned int i=0; i<m_Entries.size(); ++i) 
    {
        m_Entries[i].materialIndex  = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].numIndices     = pScene->mMeshes[i]->mNumFaces *3;
        m_Entries[i].baseVertex     = numVertices;
        m_Entries[i].baseIndex      = numIndices;
        
        numVertices += pScene->mMeshes[i]->mNumVertices;
        numIndices += m_Entries[i].numIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indixes
    Positions.reserve(numVertices);
    Normals.reserve(numVertices);
    TexCoords.reserve(numVertices);
    Bones.resize(numVertices);
    Indices.reserve(numIndices);
    
    GLExitIfError();
    
    // initialize the scene meshes one by one
    for (unsigned int i=0; i<m_Entries.size(); ++i) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        this->initRigMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }
    
    if (!initMaterials(pScene, filename)) {
        return false;
    }
	
    // @@@GPTODO: Colors are missing
    
    bool bGLError = GLCheckError();
    return  bGLError;
    
}//end initFromScene()

/*
    Called by: initFromScene()
    Initialises and populates all main vectors:
        - positions, normals, texcoords, indices and bones (VertexBoneData)
    Calls:
        - loadBones (populate for every Mesh and vertex the VertexBoneData with addBoneData() )
 */
void RigMesh::initRigMesh(glm::uint RigMeshIndex,
                          const aiMesh* paiMesh,
                          std::vector<glm::vec3>& positions,
                          std::vector<glm::vec3>& normals,
                          std::vector<glm::vec2>& texCoords,
                          std::vector<VertexBoneData>& bones,
                          std::vector<unsigned int>& indices
                    )
{
    const aiVector3D    zero3D(0.0f,0.0f,0.0f);
    
    // Populate the vertex attribute vectors
    for (unsigned int i=0; i < paiMesh->mNumVertices; ++i) 
    {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &zero3D;
        
        positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        if(pNormal!=NULL)
            normals.push_back(glm::vec3(pNormal->x,pNormal->y,pNormal->z));
        texCoords.push_back(glm::vec2(pTexCoord->x,pTexCoord->y));
    }
    
    loadBones(RigMeshIndex, paiMesh, bones);
    
    // Populate the index buffer
    for (unsigned int i=0; i < paiMesh->mNumFaces; ++i)
    {
        const aiFace& face = paiMesh->mFaces[i];
        assert(face.mNumIndices == 3);
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    
    GLExitIfError();
}//end initFromScene()

/*
    This function loads the vertex bone information for a single aiMesh object. 
    Called from RigMesh::InitRigMesh(). 
    In addition to populating the VertexBoneData structure this function also updates a map between bone names and bone IDs (a running index managed by this function) and stores the offset matrix in a vector based on the bone ID. Note how the vertex ID is calculated. Since vertex IDs are relevant to a single mesh and we store all meshes in a single vector we add the base vertex ID of the current aiMesh to vertex ID from the mWeights array to get the absolute vertex ID.
 */
void RigMesh::loadBones(glm::uint meshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& bones)
{
    for (glm::uint i=0; i < paiMesh->mNumBones; ++i) {
        glm::uint boneIndex = 0;
        std::string boneName(paiMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
            //allocate an index for a new bone
            boneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            glm::mat4 bo;
            aiMat2glmMat( &(paiMesh->mBones[i]->mOffsetMatrix), bo);
            m_BoneInfo[boneIndex].BoneOffset = bo;
            m_BoneMapping[boneName] = boneIndex;
        } //end if
        else {
            boneIndex = m_BoneMapping[boneName];
        }
        for (glm::uint j=0; j < paiMesh->mBones[i]->mNumWeights; ++j) {
            glm::uint vertexID = m_Entries[meshIndex].baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
            float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
            bones[vertexID].addBoneData(boneIndex, weight);
        }
    }//end for i
    
    //DEBUG m_BoneMapping:
    for (std::map<std::string, glm::uint>::iterator j=m_BoneMapping.begin(); j!=m_BoneMapping.end(); ++j) {
        std::cout<<"Bone: "<< (*j).first <<" with index: "<<(*j).second<<std::endl;
    }
    
}//end loadBones()

/*
    Called by: initFromScene()
    Calls:
        - initialises all materials in Scene 
        - loads all Textures in vector m_Textures
 */
bool RigMesh::initMaterials(const aiScene* pScene, const std::string& filename)
{
    // Extract the directory part from the file name
    std::string::size_type  slashIndex = filename.find_last_of("/");
    std::string             dir;
    
    if (slashIndex == std::string::npos) {
        dir = ".";
    }
    else if (slashIndex == 0)
    {   
        dir = "/";
    }
    else
    {
        dir = filename.substr(0, slashIndex);
    }
    
    bool Ret = true;
    
    // initialize the materials
    for (unsigned int i=0; i < pScene->mNumMaterials; ++i) 
    {
        const aiMaterial* pMaterial = pScene->mMaterials[i];
        
        m_Textures[i] = NULL;
        
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) >0) 
        {
            aiString path;
            
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS ) 
            {
                std::string fullPath = dir + "/" + path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());
                
                
                if (!m_Textures[i]->loadTexture()) {
                    std::cout<<"Error Loading texture: "<<fullPath.c_str() <<std::endl;
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else 
                {
                    std::cout<<"Loaded texture: "<<fullPath.c_str() <<std::endl;
                }
            }//end if pMaterial->getTexture
        }//end if pMaterial->GetTextureCount
    }//end for()
    
    return Ret;
    
}//end initMaterials()

/*
 Main RigMesh method called from external render/display callback
 Calls for every Mesh in the scene (RigMeshEntry):
    - glDrawElementsBaseVertex()
    - bindTexture for every texture
 */
void RigMesh::render()
{
    
    for (unsigned int i=0; i < m_Entries.size(); ++i) 
    {
        const unsigned int materialIndex = m_Entries[i].materialIndex;
        
        assert(materialIndex < m_Textures.size());
        
        if (m_Textures[materialIndex]) {
            m_Textures[materialIndex]->bindTexture(GL_TEXTURE0);
        }
        
        // render primitives from array data with a per-element offset
        /*
         glDrawElementsBaseVertex behaves identically to glDrawElements except that the ith element transferred by the corresponding draw call will be taken from element indices[i] + basevertex of each enabled array:
         
         void glDrawElementsBaseVertex(	GLenum  	mode,
         GLsizei  	count,
         GLenum  	type,
         GLvoid * 	indices,
         GLint  	basevertex);
         */
        glDrawElementsBaseVertex(GL_TRIANGLES, 
                                 m_Entries[i].numIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(unsigned int) * m_Entries[i].baseIndex),
                                 m_Entries[i].baseVertex);
    }//end for
    
}//end render()


glm::uint RigMesh::findPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{    
    for (glm::uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);
	return 0;
}

/*
 This utility method finds the key rotation which is immediately before the animation time. If we have N key rotations the result can be 0 to N-2. The animation time is always contained inside the duration of the channel so the last key (N-1) can never be a valid result.
 */
glm::uint RigMesh::findRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);
    
    for (glm::uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);
	return 0;
}


glm::uint RigMesh::findScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    
    for (glm::uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }
    
    assert(0);
	return 0;
}


void RigMesh::calcInterpolatedPosition(aiVector3D& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
    
    glm::uint PositionIndex = findPosition(animationTime, pNodeAnim);
    glm::uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
     // GPTODO CHECK THIS ASSERT
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

/*
 This method interpolates the rotation quaternion of the specified channel based on the animation time (remember that the channel contains an
 ￼
 array of key quaternions). First we find the index of the key quaternion which is just before the required animation time. We calculate the ratio between the distance from the animation time to the key before it and the distance between that key and the next. We need to interpolate between these two keys using that factor. We use an Assimp code to do the interpolation and normalize the result. The corresponding methods for position and scaling are very similar so they are not quoted here.
 */
void RigMesh::calcInterpolatedRotation(aiQuaternion& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    glm::uint RotationIndex = findRotation(animationTime, pNodeAnim);
    glm::uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void RigMesh::calcInterpolatedScaling(aiVector3D& Out, float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }
    
    glm::uint ScalingIndex = findScaling(animationTime, pNodeAnim);
    glm::uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
    float Factor = (animationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    // GPTODO CHECK THIS ASSERT
    //    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

/*
 This function traverses the node tree and generates the final transformation for each node/bone according to the specified animation time. It is limited in the sense that it assumes that the mesh has only a single animation sequence. If you want to support multiple animations you will need to tell it the animation name and search for it in the m_pScene->mAnimations[] array. The code above is good enough for the demo mesh that we use.
 The node transformation is initialized from the mTransformation member in the node. If the node does not correspond to a bone then that is its final transformation. If it does we overwrite it with a matrix that we generate. This is done as follows: first we search for the node name in the channel array of the animation. Then we interpolate the scaling vector, rotation quaternion and translation vector based on the animation time. We combine them into a single matrix and multiply with the matrix we got as a parameter (named GlobablTransformation). This function is recursive and is called for the root node with the GlobalTransformation param being the identity matrix. Each node recursively calls this function for all of its children and passes its own transformation as GlobalTransformation. Since we start at the top and work our way down, we get the combined transformation chain at every node.
 The m_BoneMapping array maps a node name to the index that we generate and we use that index to as an entry into the m_BoneInfo array where the final transformations are stored. The final transformation is calculated as follows: we start with the node offset matrix which brings the vertices from their local space position into there node space. We then multiple with the combined transformations of all of the nodes parents plus the specific transformation that we calculated for the node according to the animation time.
 */
void RigMesh::readNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy()"<<std::endl;
    assert(pNode!=NULL);
    std::string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
    
    glm::mat4 NodeTransformation;
    aiMat2glmMat( &(pNode->mTransformation), NodeTransformation);
    
    //std::cout<<"@@@GPTEMP: inside readNodeHierarchy() before findNodeAnim()"<<std::endl;
    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        calcInterpolatedScaling(Scaling, animationTime, pNodeAnim);
        glm::mat4 ScalingM;
        ScalingM= glm::scale( glm::mat4(1.0), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
        
        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        calcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);        
        //Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
        aiMatrix4x4 rotM(RotationQ.GetMatrix()); 
        glm::mat4 RotationM;
        aiMat2glmMat(&rotM, RotationM);
        
        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        calcInterpolatedPosition(Translation, animationTime, pNodeAnim);
        //Matrix4f TranslationM;
        //TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
        glm::mat4 TranslationM = glm::translate(glm::mat4(1.0), glm::vec3(Translation.x, Translation.y, Translation.z));
        
        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
    
    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        glm::uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (glm::uint i = 0 ; i < pNode->mNumChildren ; i++) {
        readNodeHierarchy(animationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

/*
 Main entry method called from inside external diplay callback.
 Loading of the bone information at the vertex level that we saw earlier is done only once when the mesh is loading during startup with loadBones(). 
 Here we come to the second part which is calculating the bone transformations that go into the shader every frame. This function above is the entry point to this activity. The caller (e.g. display/render application callback) reports the current time in seconds (which can be a fraction) and provides a vector of matrices which we must update. We find the relative time inside the animation cycle and process the node heirarchy. The result is an array of transformations which is returned to the caller.
 Calls:
    - readNodeHierarchy()
 */
glm::mat4 RigMesh::boneTransform(float TimeInSeconds, std::vector<glm::mat4>& transforms)
{
//    std::cout<<"@@@ entered RigMesh::boneTransform()"<< std::endl;
    glm::mat4 Identity(1.0);
    
    if (m_pScene->HasAnimations()) {
        
        //std::cout<<"found "<<m_pScene->mNumAnimations<<" animations in loaded Scene!"<<std::endl;
        assert(m_pScene!=NULL);
        assert( (m_pScene->mAnimations[0]) !=NULL);
        const aiAnimation* basicAnim = m_pScene->mAnimations[0];
        //std::cout<<"found basicAnim with name: "<< basicAnim->mName.C_Str()<<std::endl;
        
        assert(basicAnim !=NULL);
        //float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TicksPerSecond;
        if ( basicAnim->mTicksPerSecond != 0) 
            TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond;
        else 
            TicksPerSecond = 25.0f;;
        //std::cout<<"found TicksPerSecond in animation: "<< TicksPerSecond<<std::endl;
        
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float animationTime = fmod((float)TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
    
        //std::cout<<"before calling readNodeHierarchy"<< std::endl;
        readNodeHierarchy(animationTime, m_pScene->mRootNode, Identity);
        //std::cout<<"after calling readNodeHierarchy"<< std::endl;
        
        transforms.resize(m_NumBones);
    
        for (glm::uint i = 0 ; i < m_NumBones ; i++) {
            transforms[i] = m_BoneInfo[i].FinalTransformation;
        }
       
    }//if m_pScene!=NULL
    else
        std::cout<<"no animations found in aiScene: "<<m_pScene->mNumAnimations<<std::endl;

//    std::cout<<"@@@ leaving RigMesh::boneTransform()"<< std::endl;
    
    return Identity;
}


const aiNodeAnim* RigMesh::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    //std::cout<<"@@@GPTEMP: inside findNodeAnim() "<<std::endl;
    for (glm::uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        
        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }
    
    return NULL;
}





