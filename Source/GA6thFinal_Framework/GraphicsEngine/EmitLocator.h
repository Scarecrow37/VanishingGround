#pragma once
class EmitLocator
{
public:
    EmitLocator()          = default;
    virtual ~EmitLocator() = default;
    void                   RandomInitialize();
    virtual Vector3        EmitLocate() = 0;
    Vector3                GetFactor() const { return _factor; }
    void                   SetFactor(const Vector3 factor) { _factor = factor; }
    std::function<float()> RandomVal;
    std::function<float()> RandomVal01; 



    virtual class MeshSurfaceLocator*       AsMeshSurfaceLocator() { return nullptr; }
    virtual const class MeshSurfaceLocator* AsMeshSurfaceLocator() const { return nullptr; }

protected:
    Vector3                               _factor = {100, 100, 100};
    std::random_device                    _randomizer;
    std::mt19937                          _randomGenerator;
    std::uniform_real_distribution<float> _randomRange;
};

class SphereLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();

private:
    bool _isSphere = true;
};
class CubeLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class CylinderLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class ConeLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class TorusLocator : public EmitLocator
{
public:
    Vector3 EmitLocate();
};
class MeshSurfaceLocator : public EmitLocator
{
public:
    virtual ~MeshSurfaceLocator();
    Vector3               EmitLocate();
    void                  SetModelPath(std::wstring_view filepath);
    void                  LoadVerticesFromModel(std::shared_ptr<class Model> model);
    const std::filesystem::path& GetModelPath() const { return _targetModelPath; }

    MeshSurfaceLocator*       AsMeshSurfaceLocator() override { return this; }
    const MeshSurfaceLocator* AsMeshSurfaceLocator() const override { return this; }

private:
    std::wstring                 _targetModelPath = L"";
    std::shared_ptr<class Model> _targetModel;
    std::vector<UINT>            _vertexCountPerMesh;
    UINT                         _totalVertexCount = 0;
    bool                         _modelValidFlag = false;
};