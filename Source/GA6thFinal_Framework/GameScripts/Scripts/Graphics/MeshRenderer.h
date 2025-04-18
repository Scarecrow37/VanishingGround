#pragma once
#include "UmFramework.h"

class Model;
class MeshRenderer : public Component
{
    USING_PROPERTY(MeshRenderer)
public:
	const std::shared_ptr<Model>& GetModel() const { return _model; }

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string_view, FilePath)
    { 
        return ReflectFields->FilePath;
    }
    PROPERTY(FilePath)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string FilePath;
    REFLECT_FIELDS_END(MeshRenderer)

protected:
	std::shared_ptr<Model> _model;
};