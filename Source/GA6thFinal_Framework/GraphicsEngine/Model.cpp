#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "Texture.h"
#include "Material.h"

Model::~Model()
{
    Free();
}

void Model::BindVertexShader(std::shared_ptr<VertexShader>& vertexShader)
{
    for (auto& mesh : _meshs)
    {
        mesh->BindVertexShader(vertexShader);
    }
}

HRESULT Model::LoadFBX(const std::filesystem::path& filePath, bool isStaticMesh)
{
    Assimp::Importer impoter;
    impoter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    unsigned int importFlags =
        aiProcess_Triangulate |         // vertex 삼각형 으로 출력
        aiProcess_GenNormals |          // Normal 정보 생성  
        aiProcess_GenUVCoords |         // 텍스처 좌표 생성
        aiProcess_CalcTangentSpace |    // 탄젠트 벡터 생성
        aiProcess_LimitBoneWeights |    // 본 weight 제한
        aiProcess_ConvertToLeftHanded;  // DX용 왼손좌표계 변환

    if (isStaticMesh) importFlags |= aiProcess_PreTransformVertices;

    const aiScene* paiScene = impoter.ReadFile(filePath.string(), importFlags);

    if (!paiScene || paiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !paiScene->mRootNode)
    {
        ASSERT(false, L"The model could not be found by that path.");
        return E_FAIL;
    }

    std::unordered_map<std::string, std::pair<unsigned int, Matrix>> boneInfo;
    std::vector<std::vector<std::shared_ptr<Texture>>>	textures;
    std::vector<unsigned int> materialIndex;

    LoadNode(paiScene->mRootNode, paiScene, boneInfo, materialIndex);
    LoadMaterials(paiScene, filePath, textures);

    if (!boneInfo.empty())
    {
        FindMissingBone(paiScene->mRootNode, boneInfo);

        _pSkeleton = new Skeleton;
        _pSkeleton->Initialize(paiScene, boneInfo);
    }

    if (paiScene->HasAnimations())
    {
        _animation = g_pResourceMgr->LoadResource<Animation>(filePath.c_str());
        _animation->LoadAnimation(paiScene);
    }

    for (size_t i = 0; i < materialIndex.size(); i++)
    {
        Material* pMaterial = new Material;
        auto& texture = textures[materialIndex[i]];

        for (size_t j = 0; j < texture.size(); j++)
        {
            pMaterial->BindTextureMap((TextureType)j, texture[j]);
        }

        _meshs[i]->BindMaterial(pMaterial);
    }

    return S_OK;
}

void Model::LoadNode(aiNode* paiNode,
                             const aiScene* paiScene, 
                             std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, 
                             std::vector<unsigned int>& materialIndex)
{    
    for (unsigned int i = 0; i < paiNode->mNumMeshes; i++)
    {
        LoadMesh(paiNode, paiScene->mMeshes[paiNode->mMeshes[i]], boneInfo, materialIndex);
    }

    for (unsigned int i = 0; i < paiNode->mNumChildren; i++)
    {
        LoadNode(paiNode->mChildren[i], paiScene, boneInfo, materialIndex);
    }
}

void Model::LoadMesh(aiNode* paiNode, 
                             aiMesh* paiMesh, 
                             std::unordered_map<std::string, 
                             std::pair<unsigned int, Matrix>>& boneInfo, 
                             std::vector<unsigned int>& materialIndex)
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
    {
        MeshVertex vertex{};        
        vertex.position = XMVectorSet(paiMesh->mVertices[i].x, paiMesh->mVertices[i].y, paiMesh->mVertices[i].z, 1.f);
        vertex.normal = XMVectorSet(paiMesh->mNormals[i].x, paiMesh->mNormals[i].y, paiMesh->mNormals[i].z, 0.f);
        
        if (paiMesh->HasTangentsAndBitangents())
        {
            vertex.tangent = XMVectorSet(paiMesh->mTangents[i].x, paiMesh->mTangents[i].y, paiMesh->mTangents[i].z, 0.f);
            vertex.biTangent = XMVectorSet(paiMesh->mBitangents[i].x, paiMesh->mBitangents[i].y, paiMesh->mBitangents[i].z, 0.f);
        }
        
        if (paiMesh->mTextureCoords[0])
            vertex.texCoord = XMVectorSet(paiMesh->mTextureCoords[0][i].x, paiMesh->mTextureCoords[0][i].y, 0.f, 0.f);
        else
            vertex.texCoord = Vector2::Zero;

        vertices.emplace_back(vertex);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
    {
        aiFace& face = paiMesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }        

    // 해당 본이 어느 버텍스에 영향을 주는지에 대한 세팅
    for (unsigned int i = 0; i < paiMesh->mNumBones; i++)
    {
        aiBone* paiBone = paiMesh->mBones[i];
        unsigned int boneID = -1;

        if (boneInfo.find(paiBone->mName.C_Str()) == boneInfo.end())
        {
            boneInfo[paiBone->mName.C_Str()] = { _boneCount, XMMatrixTranspose(XMMATRIX(&paiBone->mOffsetMatrix.a1)) };
            boneID = _boneCount;
            _boneCount++;
        }
        else
        {
            boneID = boneInfo[paiBone->mName.C_Str()].first;
        }

        for (unsigned int j = 0; j < paiBone->mNumWeights; j++)
        {
            unsigned int id = paiBone->mWeights[j].mVertexId;
            float weight = paiBone->mWeights[j].mWeight;

            for (unsigned int k = 0; k < 4; k++)
            {
                if (0.f == vertices[id].blendWeights[k])
                {
                    vertices[id].blendIndices[k] = boneID;
                    vertices[id].blendWeights[k] = weight;
                    
                    break;
                }
            }
        }
    }

    // Weight 정규화
    for (auto& vertex : vertices)
    {
        float totalWeight = 0.f;
        float test = 0.f;
        for (int i = 0; i < 4; i++)
            totalWeight += vertex.blendWeights[i];

        if (totalWeight > 0.f)
        {
            for (int i = 0; i < 4; i++)
            {
                vertex.blendWeights[i] /= totalWeight;
                test += vertex.blendWeights[i];
            }

            if (1.f - 0.00001 > test)
            {
                int a = 0;
            }
        }
    }

    Mesh* pMesh = new Mesh(vertices, indices);
    _meshs.push_back(pMesh);
    materialIndex.push_back(paiMesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* paiScene, 
                                  const std::filesystem::path& filePath, 
                                  std::vector<std::vector<std::shared_ptr<Texture>>>& textures)
{    
    textures.resize(paiScene->mNumMaterials);

    for (unsigned int i = 0; i < paiScene->mNumMaterials; i++)
    {
        aiMaterial* pMaterial = paiScene->mMaterials[i];
        aiColor3D color{};
        pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        int a = 0;
        aiString texturePath;
        pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        textures[i].push_back(LoadTexture(filePath, paiScene, texturePath, pMaterial, aiTextureType_DIFFUSE, L""));
        textures[i].push_back(LoadTexture(filePath, paiScene, texturePath, pMaterial, aiTextureType_NORMALS, L""));
        textures[i].push_back(LoadTexture(filePath, paiScene, texturePath, pMaterial, aiTextureType_SHININESS, L"")); // RMA
        textures[i].push_back(LoadTexture(filePath, paiScene, texturePath, pMaterial, aiTextureType_EMISSIVE, L""));
        textures[i].push_back(LoadTexture(filePath, paiScene, texturePath, pMaterial, aiTextureType_OPACITY, L""));
    }
}

void Model::FindMissingBone(aiNode* paiNode, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo)
{
    if (boneInfo.find(paiNode->mName.C_Str()) == boneInfo.end())
    {
        boneInfo[paiNode->mName.C_Str()] = { -1, XMMatrixIdentity()};  // -1로 처리하여 본이 아님을 표시
    }
    
    for (unsigned int i = 0; i < paiNode->mNumChildren; i++)
    {
        FindMissingBone(paiNode->mChildren[i], boneInfo);
    }
}

std::shared_ptr<Texture> Model::LoadTexture(const std::filesystem::path& rootPath,
                                                    const aiScene* paiScene,
                                                    aiString& filePath, 
                                                    aiMaterial* pMaterial, 
                                                    aiTextureType type, 
                                                    const wchar_t* format)
{
    std::filesystem::path newPath = rootPath.parent_path().wstring() + L"/";
    std::filesystem::path fileName = filePath.C_Str();
    newPath += fileName.filename();

    if (AI_SUCCESS == pMaterial->GetTexture(type, 0, &filePath))
    {
        fileName = filePath.C_Str();
        newPath = rootPath.parent_path().wstring() + L"/" + fileName.filename().wstring();

        if (type == aiTextureType_OPACITY)
            printf(newPath.string().c_str());

        return g_pResourceMgr->LoadResource<Texture>(newPath.c_str());
    }

    return nullptr;
}

void Model::Free()
{
    SafeRelease(_pSkeleton);

    for (auto& mesh : _meshs)
        SafeRelease(mesh);
}
