#include "pch.h"
#include "FBXImporter.h"
#include "Model.h"
#include "BaseMesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "VIBuffer.h"

FBXImporter::FBXImporter()
	: _boneCount(0)
	, _isStaticMesh(true)
{
	_impoter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);
}

FBXImporter::~FBXImporter()
{
}

void FBXImporter::CreateModel(const std::filesystem::path& filePath, bool isStaticMesh, Model* model)
{
    if (filePath.extension() == L".fbx")
    {

    }

    unsigned int importFlags =
        aiProcess_Triangulate |         // vertex 삼각형 으로 출력
        aiProcess_GenNormals |          // Normal 정보 생성  
        aiProcess_GenUVCoords |         // 텍스처 좌표 생성
        aiProcess_CalcTangentSpace |    // 탄젠트 벡터 생성
        aiProcess_LimitBoneWeights |    // 본 weight 제한
        aiProcess_ConvertToLeftHanded;  // DX용 왼손좌표계 변환

    const aiScene* scene = _impoter.ReadFile(filePath.string(), importFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        ASSERT(false, L"The model could't be found by that path.");
        return;
    }  

    if (scene->HasAnimations())
        _isStaticMesh = false;

    std::unordered_map<std::string, std::pair<unsigned int, Matrix>> boneInfo;
    std::vector<std::vector<std::shared_ptr<Texture>>>	textures;
    std::vector<unsigned int> materialIndex;    

    LoadNode(scene->mRootNode, scene, boneInfo, materialIndex, model);
    LoadMaterials(scene, filePath, textures);

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

    model->InitMaterials((UINT)materialIndex.size());

    UINT size = (UINT)materialIndex.size();
    for (UINT i = 0; i < size; i++)
    {
        auto& texture = textures[materialIndex[i]];

        for (auto& j : texture)
        {
            model->BindMaterial(i, j);
        }
    }
}

void FBXImporter::LoadNode(aiNode* node,
                           const aiScene* scene, 
                           std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, 
                           std::vector<unsigned int>& materialIndex, 
                           Model* model)
{    
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        LoadMesh(node, scene->mMeshes[node->mMeshes[i]], boneInfo, materialIndex, model);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(node->mChildren[i], scene, boneInfo, materialIndex, model);
    }
}

void FBXImporter::LoadMesh(aiNode* node,
                           aiMesh* mesh, 
                           std::unordered_map<std::string, 
                           std::pair<unsigned int, Matrix>>& boneInfo, 
                           std::vector<unsigned int>& materialIndex,
                           Model* model)
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

	std::vector<StaticMeshVertex> staticVertices;
	std::vector<SkeletalMeshVertex> skeletalVertices;
    if (_isStaticMesh)
    {
		LoadVertexData(staticVertices);
    }
    else
    {
		LoadVertexData(skeletalVertices);
    }

	std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace& face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
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
                    if (0.f == skeletalVertices[id].BlendWeights[k])
                    {
                        skeletalVertices[id].BlendIndices[k] = boneID;
                        skeletalVertices[id].BlendWeights[k] = weight;

                        break;
                    }
                }
            }
        }

        // Weight 정규화
        for (auto& vertex : skeletalVertices)
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

	void* vertices = nullptr;
    UINT vertexSize = 0;
	UINT vertexStride = 0;
    if (_isStaticMesh)
    {
		vertices = static_cast<void*>(staticVertices.data());
        vertexStride = static_cast<UINT>(sizeof(StaticMeshVertex));
		vertexSize = static_cast<UINT>(vertexStride * staticVertices.size());
    }
    else
    {
		vertices = static_cast<void*>(skeletalVertices.data());
		vertexStride = static_cast<UINT>(sizeof(SkeletalMeshVertex));
		vertexSize = static_cast<UINT>(vertexStride * skeletalVertices.size());
    }

	VIBuffer::Descriptor descriptor
	{
		.vertexData = vertices,
		.indexData = indices.data(),
		.vertexSize = vertexSize,
		.vertexStride = vertexStride,
		.indexSize = static_cast<UINT>(sizeof(UINT) * indices.size()),
		.indexCount = static_cast<UINT>(indices.size()),
	};

	std::unique_ptr<BaseMesh> baseMesh = std::make_unique<BaseMesh>();
    baseMesh->Initialize(descriptor);
    model->AddMesh(std::move(baseMesh));
    materialIndex.push_back(mesh->mMaterialIndex);
}

void FBXImporter::LoadMaterials(const aiScene* paiScene,
                                const std::filesystem::path& filePath, 
                                std::vector<std::vector<std::shared_ptr<Texture>>>& textures)
{    
    textures.resize(paiScene->mNumMaterials);

    for (unsigned int i = 0; i < paiScene->mNumMaterials; i++)
    {
        aiMaterial* material = paiScene->mMaterials[i];
        aiColor3D color{};
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

        aiString texturePath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_DIFFUSE));
        textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_NORMALS));
        textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_SHININESS)); // RMA
        textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_EMISSIVE));
        //textures[i].push_back(LoadTexture(filePath, texturePath, material, aiTextureType_OPACITY));
    }
}

void FBXImporter::FindMissingBone(aiNode* node, std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo)
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

std::shared_ptr<Texture> FBXImporter::LoadTexture(const std::filesystem::path& rootPath,
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