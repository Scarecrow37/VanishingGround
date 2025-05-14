#include "pch.h"
#include "FBXConverter.h"
#include "Model.h"
#include "BaseMesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "VIBuffer.h"

FBXConverter::FBXConverter()
	: _boneCount(0)
	, _isStaticMesh(true)
{    
}

FBXConverter::~FBXConverter()
{
}

void FBXConverter::CreateModel(Model* model)
{
}

void FBXConverter::ExportFBX(const std::filesystem::path& filePath)
{    
    auto WriteData = [this, &filePath](const auto& vertices, unsigned long long vertexStride)
        {
            std::filesystem::path newPath = filePath;
            newPath.replace_extension("UmModel");
            std::ofstream outFile(newPath, std::ios::out | std::ios::binary);

            unsigned int meshCount = (unsigned int)vertices.size();
            outFile.write((char*)&meshCount, sizeof(unsigned int));

            for (unsigned int i = 0; i < meshCount; i++)
            {
                unsigned int vertexCount = (unsigned int)vertices[i].size();
                unsigned int indexCount  = (unsigned int)_indices[i].size();

                outFile.write((char*)&vertexCount, sizeof(unsigned int));
                outFile.write((char*)&indexCount, sizeof(unsigned int));
                outFile.write((char*)&vertexStride, sizeof(unsigned long long));
                outFile.write((char*)vertices[i].data(), vertexStride * vertexCount);
                outFile.write((char*)_indices[i].data(), sizeof(unsigned int) * indexCount);
            }

            outFile.close();
        };

    if (_isStaticMesh)
    {
        WriteData(_staticVertices, sizeof(StaticMeshVertex));
    }
    else
    {
        WriteData(_skeletalVertices, sizeof(SkeletalMeshVertex));
    }    
}

void FBXConverter::ImportFBX(const std::filesystem::path& filePath)
{
    if (filePath.extension() == L".fbx")
    {
        LoadFromAssimp(filePath);
    }
    else if (filePath.extension() == L".UmModel")
    {
        LoadFromBinary(filePath);
    }    
}

void FBXConverter::LoadNode(aiNode* node,
                           const aiScene* scene, 
                           std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, 
                           std::vector<unsigned int>& materialIndex)
{    
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        LoadMesh(node, scene->mMeshes[node->mMeshes[i]], boneInfo, materialIndex);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(node->mChildren[i], scene, boneInfo, materialIndex);
    }
}

void FBXConverter::LoadMesh(aiNode* node,
                            aiMesh* mesh, 
                            std::unordered_map<std::string, 
                            std::pair<unsigned int, Matrix>>& boneInfo, 
                            std::vector<unsigned int>& materialIndex)
{   
    auto LoadVertexData = [node, mesh](auto& vertices)
        {
			vertices.resize(mesh->mNumVertices);
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				XMMATRIX transform = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1));
				vertices[i].Position = XMVector3TransformCoord(XMVectorSet(mesh->mVertices[i].x, 
                                                                           mesh->mVertices[i].y, 
                                                                           mesh->mVertices[i].z, 
                                                                           1.f), 
                                                               transform);

                
                vertices[i].Normal = XMVector3TransformNormal(XMVectorSet(mesh->mNormals[i].x, 
                                                                         mesh->mNormals[i].y, 
                                                                         mesh->mNormals[i].z, 
                                                                         0.f), 
                                                             transform);

				if (mesh->HasTangentsAndBitangents())
				{
					vertices[i].Tangent = XMVectorSet(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 0.f);
					vertices[i].BiTangent = XMVectorSet(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z, 0.f);
				}

                if (mesh->mTextureCoords[0])
                {
                    vertices[i].UV = XMVectorSet(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, 0.f, 0.f);
                }
                else
                {
                    vertices[i].UV = Vector2::Zero;
                }
			}
        };

    if (_isStaticMesh)
    {
        _staticVertices.emplace_back();
		LoadVertexData(_staticVertices.back());
    }
    else
    {
        _skeletalVertices.emplace_back();
		LoadVertexData(_skeletalVertices.back());
    }

    _indices.emplace_back();
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            _indices.back().emplace_back(face.mIndices[j]);
        }
    }

    if (!_isStaticMesh)
    {
        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* paiBone = mesh->mBones[i];
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
                    if (0.f == _skeletalVertices.back()[id].BlendWeights[k])
                    {
                        _skeletalVertices.back()[id].BlendIndices[k] = boneID;
                        _skeletalVertices.back()[id].BlendWeights[k] = weight;

                        break;
                    }
                }
            }
        }

        // Weight 정규화
        for (auto& vertex : _skeletalVertices.back())
        {
            float totalWeight = 0.f;

            for (auto& BlendWeight : vertex.BlendWeights)
            {
                totalWeight += BlendWeight;
            }

            if (totalWeight > 0.f)
            {
                for (auto& BlendWeight : vertex.BlendWeights)
                {
                    BlendWeight /= totalWeight;
                }
            }
        }
    }

    materialIndex.push_back(mesh->mMaterialIndex);
}

void FBXConverter::LoadMaterials(const aiScene* paiScene,
                                const std::filesystem::path& filePath)
{    
    _textures.resize(paiScene->mNumMaterials);

    for (unsigned int i = 0; i < paiScene->mNumMaterials; i++)
    {
        aiMaterial* material = paiScene->mMaterials[i];
        aiColor3D color{};
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        aiString texturePath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        _textures[i].push_back(ConvertPath(filePath, texturePath, material, aiTextureType_DIFFUSE));
        _textures[i].push_back(ConvertPath(filePath, texturePath, material, aiTextureType_NORMALS));
        _textures[i].push_back(ConvertPath(filePath, texturePath, material, aiTextureType_SHININESS));
        _textures[i].push_back(ConvertPath(filePath, texturePath, material, aiTextureType_EMISSIVE));
        //textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_OPACITY));
    }
}

void FBXConverter::FindMissingBone(aiNode*                                                           node,
                                   std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo)
{
    if (boneInfo.find(node->mName.C_Str()) == boneInfo.end())
    {
        boneInfo[node->mName.C_Str()] = { -1, XMMatrixIdentity()};  // -1로 처리하여 본이 아님을 표시
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        FindMissingBone(node->mChildren[i], boneInfo);
    }
}

std::shared_ptr<Texture> FBXConverter::LoadTexture(const std::filesystem::path& rootPath,
                                                   aiString& filePath, 
                                                   aiMaterial* pMaterial, 
                                                   aiTextureType type)
{
    std::filesystem::path newPath = rootPath.parent_path().wstring() + L"/";
    std::filesystem::path fileName = filePath.C_Str();
    newPath += fileName.filename();

    if (AI_SUCCESS == pMaterial->GetTexture(type, 0, &filePath))
    {
        fileName = filePath.C_Str();
        newPath = rootPath.parent_path().wstring() + L"/" + fileName.filename().wstring();

        return UmResourceManager.LoadResource<Texture>(newPath.c_str());
    }

    return nullptr;
}

std::string FBXConverter::ConvertPath(const std::filesystem::path& rootPath, 
                                      aiString& filePath, 
                                      aiMaterial* pMaterial,
                                      aiTextureType type)
{
    std::filesystem::path newPath  = rootPath.parent_path().wstring() + L"/";
    std::filesystem::path fileName = filePath.C_Str();
    newPath += fileName.filename();

    if (AI_SUCCESS == pMaterial->GetTexture(type, 0, &filePath))
    {
        fileName = filePath.C_Str();
        newPath  = rootPath.parent_path().wstring() + L"/" + fileName.filename().wstring();

        return newPath.string();
    }

    return "";
}

void FBXConverter::LoadFromAssimp(const std::filesystem::path& filePath)
{
    Assimp::Importer impoter;
    impoter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    unsigned int importFlags = aiProcess_Triangulate | 
                               aiProcess_GenNormals | 
                               aiProcess_GenUVCoords |
                               aiProcess_CalcTangentSpace | 
                               aiProcess_LimitBoneWeights | 
                               aiProcess_ConvertToLeftHanded;

    const aiScene* scene = impoter.ReadFile(filePath.string(), importFlags);

    if (scene->HasAnimations())
        _isStaticMesh = false;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        ASSERT(false, L"The model could't be found by that path.");
        return;
    }

    std::unordered_map<std::string, std::pair<unsigned int, Matrix>> boneInfo;
    std::vector<std::vector<std::shared_ptr<Texture>>>               textures;
    std::vector<unsigned int>                                        materialIndex;

    LoadNode(scene->mRootNode, scene, boneInfo, materialIndex);
    LoadMaterials(scene, filePath);

    if (!boneInfo.empty())
    {
        FindMissingBone(scene->mRootNode, boneInfo);

        _skeleton = std::make_shared<Skeleton>();
        _skeleton->Initialize(scene, boneInfo);
    }

    if (scene->HasAnimations())
    {
        _animation = UmResourceManager.LoadResource<Animation>(filePath.c_str());
        _animation->LoadAnimation(scene);
    }
}

void FBXConverter::LoadFromBinary(const std::filesystem::path& filePath)
{
    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if (!inFile.is_open())
    {
        ASSERT(false, L"The model could't be found by that path.");
        return;
    }

    unsigned int meshCount = 0;
    inFile.read((char*)&meshCount, sizeof(unsigned int));

    _staticVertices.resize(meshCount);
    _indices.resize(meshCount);
    for (unsigned int i = 0; i < meshCount; i++)
    {
        unsigned int vertexCount = 0;
        inFile.read((char*)&vertexCount, sizeof(unsigned int));

        unsigned int indexCount = 0;
        inFile.read((char*)&indexCount, sizeof(unsigned int));

        unsigned long long vertexStride = 0;
        inFile.read((char*)&vertexStride, sizeof(unsigned long long));

        char* vertices = new char[vertexStride * vertexCount];
        inFile.read((char*)vertices, vertexStride * vertexCount);

        char* indices = new char[sizeof(unsigned int) * indexCount];
        inFile.read((char*)indices, sizeof(unsigned int) * indexCount);

        /*_staticVertices[i].resize(vertexCount);
        inFile.read((char*)_staticVertices[i].data(), vertexStride * vertexCount);

        _indices[i].resize(indexCount);
        inFile.read((char*)_indices[i].data(), sizeof(unsigned int) * indexCount);*/

        VIBuffer::Descriptor descriptor{
            .vertexData   = vertices,
            .indexData    = indices,
            .vertexSize   = (UINT)vertexStride * vertexCount,
            .vertexStride = (UINT)vertexStride,
            .indexSize    = (sizeof(UINT) * indexCount),
            .indexCount   = indexCount,
        };

        std::unique_ptr<BaseMesh> baseMesh = std::make_unique<BaseMesh>();
        baseMesh->Initialize(descriptor);
        //model->AddMesh(std::move(baseMesh));
        //materialIndex.push_back(mesh->mMaterialIndex);
    }

    int a = 0;
}