#pragma once
#include "UmFramework.h"

class Model;
class MeshRenderer : public Component
{
    USING_PROPERTY(MeshRenderer)
public:
    enum class RENDER_TYPE { STATIC, SKELETAL };

public:
    const std::shared_ptr<Model>& GetModel() const { return _model; }
    RENDER_TYPE                   GetType() const { return ReflectFields->Type; }

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) 
    { 
        File::Guid guid = ReflectFields->Guid;
        return guid.ToPath().string(); 
    }
    PROPERTY(FilePath)

public:
    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    RENDER_TYPE Type;
    REFLECT_FIELDS_END(MeshRenderer)

protected:
    std::shared_ptr<Model> _model;
};