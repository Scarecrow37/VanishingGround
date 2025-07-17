#include "pch.h"
#include "FBXConverter.h"
#include "Model.h"
#include "BaseMesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "VIBuffer.h"
#include "../XAtlasThirdParty/xatlas.h"

FBXConverter::FBXConverter()
	: _boneCount(0)
	, _isStaticMesh(true)
{    
}

FBXConverter::~FBXConverter()
{
}

void FBXConverter::ImportModel(const std::filesystem::path& filePath, std::shared_ptr<Model> model)
{
    Reset();

    _model    = model;

    if (filePath.extension() == L".fbx")
    {
        LoadFromAssimp(filePath, model.get());
    }
    else if (filePath.extension() == L".UmModel")
    {
        LoadFromBinary(filePath, model.get());
    }
}

void FBXConverter::ImportModel(const std::filesystem::path& filePath, Model* model)
{
    if (filePath.extension() == L".fbx")
    {
        LoadFromAssimp(filePath, model);
    }
    else if (filePath.extension() == L".UmModel")
    {
        LoadFromBinary(filePath, model);
    }
}

void FBXConverter::ExportModel(const std::filesystem::path& filePath)
{    
    auto WriteData = [this, &filePath](const auto& vertices, unsigned long long vertexStride)
        {
            std::filesystem::path newPath = filePath;
            newPath.replace_extension("UmModel");
            std::ofstream outFile(newPath, std::ios::out | std::ios::binary);

            unsigned int meshCount = (unsigned int)vertices.size();
            outFile.write((char*)&meshCount, sizeof(unsigned int));

            const auto& materials = _model->GetMaterials();
            for (unsigned int i = 0; i < meshCount; i++)
            {
                unsigned int vertexCount = (unsigned int)vertices[i].size();
                outFile.write((char*)&vertexCount, sizeof(unsigned int));

                unsigned int indexCount  = (unsigned int)_indices[i].size();
                outFile.write((char*)&indexCount, sizeof(unsigned int));

                outFile.write((char*)&vertexStride, sizeof(unsigned long long));
                outFile.write((char*)vertices[i].data(), vertexStride * vertexCount);
                outFile.write((char*)_indices[i].data(), sizeof(unsigned int) * indexCount);

                unsigned int meshNameSize = (unsigned int)_meshNames[i].size() + 1;
                outFile.write((char*)&meshNameSize, sizeof(unsigned int));
                outFile.write(_meshNames[i].c_str(), meshNameSize);

                unsigned int materialIndex = _materialIndex[i];
                outFile.write((char*)&materialIndex, sizeof(unsigned int));

                unsigned int materialCount = (unsigned int)_textures[materialIndex].size();                
                outFile.write((char*)&materialCount, sizeof(unsigned int));

                for (unsigned int j = 0; j < materialCount; j++)
                {
                    auto& path = _textures[materialIndex][j];
                    unsigned int pathSize = (unsigned int)path.size() + 1;

                    outFile.write((char*)&pathSize, sizeof(unsigned int));
                    outFile.write(path.c_str(), pathSize);
                }

                outFile.write((char*)&materials[i], sizeof(Material));
            }

            if (sizeof(SkeletalMeshVertex) == vertexStride)
            {
                const auto animation = _model->GetAnimation();

                unsigned int animationSize = (unsigned int)animation->_animations.size();
                outFile.write((char*)&animationSize, sizeof(unsigned int));
                for (auto& [channelName, channel] : animation->_animations)
                {
                    unsigned int channelNameSize = (unsigned int)channelName.size() + 1;
                    outFile.write((char*)&channelNameSize, sizeof(unsigned int));
                    outFile.write(channelName.data(), channelNameSize);
                    outFile.write((char*)&channel.LastTime, sizeof(float));

                    unsigned int boneTransformSize = (unsigned int)channel.BoneTransforms.size();
                    outFile.write((char*)&boneTransformSize, sizeof(unsigned int));
                    for (auto& [boneName, track] : channel.BoneTransforms)
                    {
                        unsigned int boneNameSize = (unsigned int)boneName.size() + 1;
                        outFile.write((char*)&boneNameSize, sizeof(unsigned int));
                        outFile.write(boneName.data(), boneNameSize);

                        unsigned int PositionsSize = (unsigned int)track.Positions.size();
                        outFile.write((char*)&PositionsSize, sizeof(unsigned int));
                        for (auto& position : track.Positions)
                        {
                            outFile.write((char*)&position, sizeof(std::pair<float, Vector3>));
                        }

                        unsigned int RotationSize = (unsigned int)track.Rotations.size();
                        outFile.write((char*)&RotationSize, sizeof(unsigned int));
                        for (auto& rotation : track.Rotations)
                        {
                            outFile.write((char*)&rotation, sizeof(std::pair<float, Vector4>));
                        }

                        unsigned int ScalesSize = (unsigned int)track.Scales.size();
                        outFile.write((char*)&ScalesSize, sizeof(unsigned int));
                        for (auto& scale : track.Scales)
                        {
                            outFile.write((char*)&scale, sizeof(std::pair<float, Vector3>));
                        }
                    }
                }

                const auto skeleton = _model->GetSkeleton();
                WriteBoneData(outFile, skeleton->_rootBone);
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

void FBXConverter::LoadNode(aiNode* node,
                            const aiScene* scene, 
                            std::unordered_map<std::string, std::pair<unsigned int, Matrix>>& boneInfo, 
                            Model* model)
{    
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        LoadMesh(node, scene->mMeshes[node->mMeshes[i]], boneInfo, model);
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(node->mChildren[i], scene, boneInfo, model);
    }
}

void FBXConverter::LoadMesh(aiNode* node,
                            aiMesh* mesh, 
                            std::unordered_map<std::string, 
                            std::pair<unsigned int, Matrix>>& boneInfo, 
                            Model* model)
{   
    auto LoadVertexData = [node, mesh](auto& vertices)
        {
			vertices.resize(mesh->mNumVertices);
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				XMMATRIX transform = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1));
                if (mesh->HasPositions())
                {
                    vertices[i].Position = XMVector3TransformCoord(XMVectorSet(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.f), transform);
                }
                if (mesh->HasNormals())
                {
                    vertices[i].Normal = XMVector3TransformNormal(XMVectorSet(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.f), transform);
                }
				if (mesh->HasTangentsAndBitangents())
				{
					vertices[i].Tangent = XMVectorSet(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 0.f);
					vertices[i].BiTangent = XMVectorSet(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z, 0.f);
				}

                if (mesh->mTextureCoords[0])
                {
                    vertices[i].UV[0] = XMVectorSet(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, 0.f, 0.f);
                }
                else
                {
                    vertices[i].UV[0] = Vector2::Zero;
                }                
			}
        };

    auto GenerateLightmapUV = [](auto& vertices, auto& indices, UINT vertexStride)
        {
            xatlas::Atlas*   atlas = xatlas::Create();
            xatlas::MeshDecl meshDecl;
            meshDecl.vertexCount          = (uint32_t)vertices.size();
            meshDecl.vertexPositionData   = vertices.data();
            meshDecl.vertexPositionStride = vertexStride;
            meshDecl.indexCount           = (uint32_t)indices.size();
            meshDecl.indexData            = indices.data();
            meshDecl.indexFormat          = xatlas::IndexFormat::UInt32;
            xatlas::AddMesh(atlas, meshDecl);
            xatlas::Generate(atlas, xatlas::ChartOptions(), xatlas::PackOptions());

            for (size_t i = 0; i < vertices.size(); ++i)
            {
                vertices[i].UV[1] = Vector2(atlas->meshes[0].vertexArray[i].uv);
            }
            xatlas::Destroy(atlas);
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

    if (_isStaticMesh)
    {
        GenerateLightmapUV(_staticVertices.back(), _indices.back(), sizeof(StaticMeshVertex));
    }
    else
    {
        GenerateLightmapUV(_skeletalVertices.back(), _indices.back(), sizeof(SkeletalMeshVertex));
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
    }

    void* vertices     = nullptr;
    UINT  vertexSize   = 0;
    UINT  vertexStride = 0;

    if (_isStaticMesh)
    {
        vertices     = static_cast<void*>(_staticVertices.back().data());
        vertexStride = static_cast<UINT>(sizeof(StaticMeshVertex));
        vertexSize   = static_cast<UINT>(vertexStride * _staticVertices.back().size());
    }
    else
    {
        vertices     = static_cast<void*>(_skeletalVertices.back().data());
        vertexStride = static_cast<UINT>(sizeof(SkeletalMeshVertex));
        vertexSize   = static_cast<UINT>(vertexStride * _skeletalVertices.back().size());
    }

    VIBuffer::Descriptor descriptor{
        .vertexData   = vertices,
        .indexData    = _indices.back().data(),
        .vertexSize   = vertexSize,
        .vertexStride = vertexStride,
        .indexSize    = static_cast<UINT>(sizeof(UINT) * _indices.back().size()),
        .indexCount   = static_cast<UINT>(_indices.back().size()),
    };

    std::unique_ptr<BaseMesh> baseMesh = std::make_unique<BaseMesh>();
    baseMesh->Initialize(descriptor, true);
    baseMesh->SetName(node->mName.C_Str());
    model->AddMesh(std::move(baseMesh));

    _materialIndex.push_back(mesh->mMaterialIndex);
    _meshNames.push_back(node->mName.C_Str());
}

void FBXConverter::LoadMaterials(const aiScene* paiScene,
                                 const std::filesystem::path& filePath)
{    
    _textures.resize(paiScene->mNumMaterials);

    for (unsigned int i = 0; i < paiScene->mNumMaterials; i++)
    {
        aiMaterial* material = paiScene->mMaterials[i];
        aiString texturePath;

        _textures[i].push_back(std::move(ConvertPath(texturePath, material, aiTextureType_DIFFUSE)));
        _textures[i].push_back(std::move(ConvertPath(texturePath, material, aiTextureType_NORMALS)));
        _textures[i].push_back(std::move(ConvertPath(texturePath, material, aiTextureType_SHININESS)));
        _textures[i].push_back(std::move(ConvertPath(texturePath, material, aiTextureType_EMISSIVE)));
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

std::string FBXConverter::ConvertPath(aiString& filePath, 
                                      aiMaterial* pMaterial,
                                      aiTextureType type)
{
    if (AI_SUCCESS == pMaterial->GetTexture(type, 0, &filePath))
    {
        std::filesystem::path fileName = filePath.C_Str();
        return fileName.filename().string();
    }

    return "";
}

void FBXConverter::Reset()
{
    _staticVertices.clear();
    _skeletalVertices.clear();
    _materialIndex.clear();
    _indices.clear();
    _textures.clear();
    _boneCount    = 0;
    _isStaticMesh = true;
}

void FBXConverter::LoadFromAssimp(const std::filesystem::path& filePath, Model* model)
{
    Assimp::Importer impoter;
    impoter.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    unsigned int importFlags = //aiProcessPreset_TargetRealtime_MaxQuality |
                               aiProcessPreset_TargetRealtime_Quality |
                               //aiProcessPreset_TargetRealtime_Fast |
                               aiProcess_ConvertToLeftHanded;

    /*importFlags ^= aiProcess_GenSmoothNormals;
    importFlags |= aiProcess_GenNormals;*/

    const aiScene* scene = impoter.ReadFile(filePath.string(), importFlags);

    if (scene->HasAnimations())
        _isStaticMesh = false;

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        GRAPHICS_ASSERT(false, L"The model could't be found by that path.");
        return;
    }

    std::unordered_map<std::string, std::pair<unsigned int, Matrix>> boneInfo;
    
    LoadNode(scene->mRootNode, scene, boneInfo, model);
    LoadMaterials(scene, filePath);

    model->InitMaterials((UINT)_materialIndex.size());
    UINT size = (UINT)_materialIndex.size();

    Material material{
        .Model = Material::ShadingModel::DEFAULTLIT,
        .Mode    = Material::BlendMode::OPAQUE,
        .IsTwoSided   = false,
    };

    for (UINT i = 0; i < size; i++)
    {
        auto& paths = _textures[_materialIndex[i]];
        model->BindMaterial(i, material);

        for (auto& path : paths)
        {
            std::filesystem::path newPath;

            if (path.empty())
            {
                newPath = L"BlackTexture";
            }
            else
            {
                newPath = filePath;
                newPath.replace_filename(path);
            }

            model->BindTexture(i, Global::resourceManager->LoadResource<Texture>(newPath));
        }
    }

    if (!boneInfo.empty())
    {
        FindMissingBone(scene->mRootNode, boneInfo);

        std::shared_ptr<Skeleton> skeleton = std::make_shared<Skeleton>();
        skeleton->Initialize(scene, boneInfo);
        model->_skeleton = skeleton;
    }

    if (scene->HasAnimations())
    {
        std::shared_ptr<Animation> animation = Global::resourceManager->LoadResource<Animation>(filePath.c_str());
        animation->LoadAnimation(scene);
        model->_animation = animation;
    }
}

void FBXConverter::LoadFromBinary(const std::filesystem::path& filePath, Model* model)
{    
    std::ifstream inFile(filePath, std::ios::in | std::ios::binary);
    if (!inFile.is_open())
    {
        GRAPHICS_ASSERT(false, L"The model could't be found by that path.");
        return;
    }

    unsigned int meshCount = 0;
    inFile.read((char*)&meshCount, sizeof(unsigned int));

    _staticVertices.resize(meshCount);
    _skeletalVertices.resize(meshCount);
    _indices.resize(meshCount);
    _textures.resize(meshCount);
    _meshNames.resize(meshCount);
    model->InitMaterials(meshCount);

    bool isStaticMesh = true;
    for (unsigned int i = 0; i < meshCount; i++)
    {
        unsigned int vertexCount = 0;
        inFile.read((char*)&vertexCount, sizeof(unsigned int));

        unsigned int indexCount = 0;
        inFile.read((char*)&indexCount, sizeof(unsigned int));

        unsigned long long vertexStride = 0;
        inFile.read((char*)&vertexStride, sizeof(unsigned long long));

        void* vertices = nullptr;

        if (vertexStride != sizeof(StaticMeshVertex))
            isStaticMesh = false;

        if (isStaticMesh)
        {
            _staticVertices[i].resize(vertexCount);
            inFile.read((char*)_staticVertices[i].data(), vertexStride * vertexCount);
            vertices = static_cast<void*>(_staticVertices[i].data());
        }
        else if (!isStaticMesh)
        {
            _skeletalVertices[i].resize(vertexCount);
            inFile.read((char*)_skeletalVertices[i].data(), vertexStride * vertexCount);
            vertices = static_cast<void*>(_skeletalVertices[i].data());
        }

        _indices[i].resize(indexCount);
        inFile.read((char*)_indices[i].data(), sizeof(unsigned int) * indexCount);
        
        unsigned int meshNameSize  = 0;
        inFile.read((char*)&meshNameSize, sizeof(unsigned int));

        _meshNames[i].resize(meshNameSize);
        inFile.read((char*)_meshNames[i].data(), meshNameSize);

        VIBuffer::Descriptor descriptor{
            .vertexData   = vertices,
            .indexData    = _indices[i].data(),
            .vertexSize   = (UINT)vertexStride * vertexCount,
            .vertexStride = (UINT)vertexStride,
            .indexSize    = (sizeof(UINT) * indexCount),
            .indexCount   = indexCount,
        };

        std::unique_ptr<BaseMesh> baseMesh = std::make_unique<BaseMesh>();
        baseMesh->Initialize(descriptor, true);
        baseMesh->SetName(_meshNames[i].c_str());

        unsigned int materialIndex = 0;
        inFile.read((char*)&materialIndex, sizeof(unsigned int));
        _materialIndex.push_back(materialIndex);

        unsigned int materialCount = 0;
        inFile.read((char*)&materialCount, sizeof(unsigned int));
        _textures[i].resize(materialCount);

        for (unsigned int j = 0; j < materialCount; j++)
        {
            unsigned int pathSize = 0;
            inFile.read((char*)&pathSize, sizeof(unsigned int));
            _textures[i][j].resize(pathSize);
            inFile.read((char*)_textures[i][j].data(), pathSize);
        }

        model->AddMesh(std::move(baseMesh));
        
        auto& paths = _textures[materialIndex];
        for (auto& path : paths)
        {
            std::filesystem::path newPath;

            if (1 == path.size() || path.empty())
            {
                newPath = L"BlackTexture";
            }
            else
            {
                newPath = filePath;
                newPath.replace_filename(path);
            }

            model->BindTexture(i, Global::resourceManager->LoadResource<Texture>(newPath));
        }

        Material material{};
        inFile.read((char*)&material, sizeof(Material));
        model->BindMaterial(i, material);        
    }

    if (!isStaticMesh)
    {
        model->_animation = std::make_shared<Animation>();
        auto& animations  = model->_animation->_animations;

        unsigned int animationSize = 0;
        inFile.read((char*)&animationSize, sizeof(unsigned int));
        for (unsigned int i = 0; i < animationSize; i++)
        {
            unsigned int channelNameSize = 0;
            inFile.read((char*)&channelNameSize, sizeof(unsigned int));

            std::string channelName(channelNameSize, '\0');
            inFile.read(channelName.data(), channelNameSize);

            Animation::Channel channel{};
            inFile.read((char*)&channel.LastTime, sizeof(float));

            unsigned int boneTransformSize = 0;
            inFile.read((char*)&boneTransformSize, sizeof(unsigned int));
            for (unsigned int i = 0; i < boneTransformSize; i++)
            {
                Animation::BoneTransformTrack track{};

                unsigned int boneNameSize = 0;
                inFile.read((char*)&boneNameSize, sizeof(unsigned int));

                std::string boneName(boneNameSize, '\0');
                inFile.read(boneName.data(), boneNameSize);

                unsigned int PositionsSize = 0;
                inFile.read((char*)&PositionsSize, sizeof(unsigned int));
                for (unsigned int i = 0; i < PositionsSize; i++)
                {
                    std::pair<float, Vector3> position;
                    inFile.read((char*)&position, sizeof(std::pair<float, Vector3>));
                    track.Positions.push_back(position);
                }

                unsigned int RotationSize = 0;
                inFile.read((char*)&RotationSize, sizeof(unsigned int));
                for (unsigned int i = 0; i < RotationSize; i++)
                {
                    std::pair<float, Vector4> rotation;
                    inFile.read((char*)&rotation, sizeof(std::pair<float, Vector4>));
                    track.Rotations.push_back(rotation);
                }

                unsigned int ScalesSize = 0;
                inFile.read((char*)&ScalesSize, sizeof(unsigned int));
                for (unsigned int i = 0; i < ScalesSize; i++)
                {
                    std::pair<float, Vector3> scale;
                    inFile.read((char*)&scale, sizeof(std::pair<float, Vector3>));
                    track.Scales.push_back(scale);
                }

                channel.BoneTransforms.emplace(boneName, track);
            }

            animations[channelName.c_str()] = channel;
        }

        _model->_skeleton = std::make_shared<Skeleton>();
        ReadBoneData(inFile, _model->_skeleton->_rootBone);

        for (auto& [name, channel] : _model->_animation->_animations)
        {
            if (name.empty())
                continue;
            _model->_animation->_animationNames.push_back(name.data());
        }
    }    

    inFile.close();
}

void FBXConverter::WriteBoneData(std::ofstream& outFile, const Bone& bone)
{
    outFile.write((char*)&bone.Offset, sizeof(Matrix) * 3);
    outFile.write((char*)&bone.ID, sizeof(int));

    unsigned int skeletonNameSize = (unsigned int)bone.Name.size() + 1;
    outFile.write((char*)&skeletonNameSize, sizeof(unsigned int));
    outFile.write(bone.Name.c_str(), skeletonNameSize);

    unsigned int children = (unsigned int)bone.Children.size();
    outFile.write((char*)&children, sizeof(unsigned int));
    for (auto& child : bone.Children)
    {
        WriteBoneData(outFile, child);
    }
}

void FBXConverter::ReadBoneData(std::ifstream& inFile, Bone& bone)
{
    inFile.read((char*)&bone.Offset, sizeof(Matrix) * 3);
    inFile.read((char*)&bone.ID, sizeof(int));

    unsigned int skeletonNameSize = 0;
    inFile.read((char*)&skeletonNameSize, sizeof(unsigned int));

    bone.Name.resize(skeletonNameSize);
    inFile.read(bone.Name.data(), skeletonNameSize);

    unsigned int children = 0;
    inFile.read((char*)&children, sizeof(unsigned int));
    bone.Children.resize(children);
    for (unsigned int i = 0; i < children; i++)
    {
        ReadBoneData(inFile, bone.Children[i]);
    }
}
