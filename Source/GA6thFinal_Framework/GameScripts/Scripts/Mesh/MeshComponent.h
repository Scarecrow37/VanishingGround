#pragma once

class IMeshRenderer;
class MeshComponent abstract : public Component
{
    USING_PROPERTY(MeshComponent)
public:
    REFLECT_PROPERTY()

private:
    GraphicsPointer<IMeshRenderer> _pMeshRenderer;

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
    void MakeMeshRenderer(const Matrix& world);

    //meshRenderer 입니다. MakeMeshRenderer를 호출해야만 생성됩니다.
    const GraphicsPointer<IMeshRenderer>& Renderer;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::string                 Guid;
    std::vector<unsigned int>   ShadingModel;
    std::vector<unsigned int>   BlendMode;
    std::vector<unsigned int>   CullMode;
    std::vector<unsigned int>   CustomDepth;
    std::vector<bool>           IsTwoSided;

    // CustomMaterials
    CustomLightType             CustomLightType = CustomLightType::NONE;
    TransparentRimLightMaterial RimLightMaterial{};
    REFLECT_FIELDS_END(MeshComponent)

protected:
    void SerializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

protected:
    void InitMaterial();

private:
    static IMeshRenderer*& GetLastSelectMeshRenderer();
    
    // ImGui Helper Methods
    void DrawMaterialsList();
    void HandleMeshSelection(UINT index, IMeshRenderer*& lastRenderer, UINT*& lastCustomDepth, UINT& lastSelected);
    void DrawMaterialProperties(UINT index, Material& material, UINT customDepth);
    void DrawShadingModelRow(UINT index, Material& material);
    void DrawCustomLitProperties(UINT index, Material& material);
    void DrawRimLightProperties(UINT index);
    void DrawDefaultLitProperties(UINT index, Material& material);
    void DrawCustomDepthRow(UINT index, UINT customDepth);
};
