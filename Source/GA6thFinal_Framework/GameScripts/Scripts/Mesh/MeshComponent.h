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

    /// <summary>
    /// 렌더러가 Model을 가지고 있는지 확인합니다
    /// </summary>
    /// <returns>Model이 등록되어 있으면 true, 아니면 false를 반환합니다.</returns>
    bool HasModel() const;

    /// <summary>
    /// 렌더러가 Animator를 가지고 있는지 확인합니다
    /// </summary>
    /// <returns>Animator가 등록되어 있으면 true, 아니면 false를 반환합니다.</returns>
    bool HasAnimator() const;

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
