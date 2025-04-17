#pragma once
#include "UmFramework.h"

class Model;
class MeshRenderer : public Component
{    
public:
	const std::shared_ptr<Model>& GetModel() const { return _model; }

protected:
	std::shared_ptr<Model> _model;
    std::wstring           _filePath;
};