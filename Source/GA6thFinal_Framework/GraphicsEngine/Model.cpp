#include "pch.h"
#include "Model.h"
#include "BaseMesh.h"
#include "FBXConverter.h"
#include "Skeleton.h"
#include "Animation.h"

Model::Model()
{
}

Model::~Model()
{
}

const std::vector<std::string>& Model::GetBoneNameList() const
{
    return _skeleton->GetBoneNameList();
}

void Model::SetMaterial(const UINT meshIndex, const Material& material)
{
    if (meshIndex < _material.size())
    {
        _material[meshIndex] = material;
    }
}

void Model::AddMesh(std::unique_ptr<BaseMesh> mesh)
{
	_meshes.emplace_back(std::move(mesh));
}

void Model::InitMaterials(UINT materialCount)
{
    _textures.resize(materialCount);
    _material.resize(materialCount);
}

void Model::BindTexture(const UINT meshIndex, std::shared_ptr<Texture> texture)
{
	_textures[meshIndex].emplace_back(texture);
}

void Model::BindMaterial(const UINT meshIndex, const Material& material)
{
    _material[meshIndex] = material;
}

void Model::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{   
    //FBXConverter fbxConverter;
    //fbxConverter.ImportModel(filePath, this);

    Global::threadPool->AddTask(ThreadPool::ThreadType::PARALLEL, [this, filePath, callback](ID3D12GraphicsCommandList* commandList)
    {
        FBXConverter fbxConverter;
        fbxConverter.ImportModel(commandList, filePath, this);
        Global::resourceManager->EnqueueCallback(callback);
    });
}