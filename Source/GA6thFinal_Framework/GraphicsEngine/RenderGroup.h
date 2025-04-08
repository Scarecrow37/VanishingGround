#pragma once
#include "Base.h"

namespace GE
{
	class IMeshRenderer;
	class IUnlitRenderer;
}

class MeshRenderer;
class UnlitRenderer;
class RenderGroup : public Base
{
public:
	explicit RenderGroup() = default;
	virtual ~RenderGroup() = default;

public:
	const std::list<std::pair<MeshRenderer*, Matrix*>>& GetRenderDatas() { return _datas; }

public:
	void RegisterRenderQueue(GE::IMeshRenderer* pMeshRenderer, Matrix* pMatrix);
	void UnRegisterRenderQueue(GE::IMeshRenderer* pMeshRenderer);
	void RegisterUnlitRenderQueue(GE::IUnlitRenderer* pUnlitRenderer, Matrix* pMatrix);
	void UnRegisterUnlitRenderQueue(GE::IUnlitRenderer* pUnlitRenderer);

private:
	// Base을(를) 통해 상속됨
	void Free() override;

private:
	std::list<std::pair<MeshRenderer*, Matrix*>> _datas;
	std::list<std::pair<UnlitRenderer*, Matrix*>> _unlitDatas;
};