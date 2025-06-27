#pragma once
#include "UmFramework.h"
#include "Engine/GraphicsCore/MeshRenderer.h"

class MeshComponent abstract : public Component
{
    USING_PROPERTY(MeshComponent)
public:
    REFLECT_PROPERTY()

private:
    std::unique_ptr<MeshRenderer> _pMeshRenderer;

public:
    MeshComponent();
    virtual ~MeshComponent();

    bool HasModel();
    bool HasAnimator();

    /// <summary>
    /// 메시 렌더러를 생성합니다. 이미 존재하면 생성하지 않습니다.
    /// </summary>
    /// <param name="renderType"></param>
    /// <param name="world"></param>
    void MakeMeshRenderer(MeshRenderType renderType, const Matrix& world);

    //meshRenderer 입니다. MakeMeshRenderer를 호출해야만 생성됩니다.
    const std::unique_ptr<MeshRenderer>& Renderer;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(MeshComponent)
};
