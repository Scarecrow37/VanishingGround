#pragma once
#include "Component.h"
#include "includes/IMeshRenderer.h"

class Model;
class VertexShader;
class MeshRenderer : public Component, public GE::IMeshRenderer
{
protected:
	explicit MeshRenderer() = default;
	virtual ~MeshRenderer() = default;

public:
	std::shared_ptr<Model>& GetModel() { return _model; }
	MeshType::Type GetType() const { return _type; }
	const unsigned int GetPostEffectFlag() const { return _description.postEffectFlag; }
	bool IsActiveDraw() const { return _description.activeDraw; }
	bool IsActiveShadow() const { return _description.activeShadow; }
	bool IsZWrite() const { return _description.zWrite; }

public:
	virtual void Initialize(const wchar_t* filePath) = 0;
	void SetVertexShader();

public:
	// IMeshRenderer을(를) 통해 상속됨
	void Release() override;
	void Query(void** ppOut) override {};
	void SetDesc(const GE::MeshDescription* in) override;
	void GetDesc(GE::MeshDescription* out) override;

protected:
	std::shared_ptr<Model>			_model;
	std::shared_ptr<VertexShader>	_vertexShader;
	MeshType::Type					_type{ MeshType::End };
	GE::MeshDescription				_description;
};