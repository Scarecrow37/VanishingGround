#include "pchScripts.h"
#include "MeshComponent.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"

REFLECT_FUNCTION(MeshComponent)

MeshComponent::MeshComponent() 
    : Component(Component::TYPE::MESH)
    , Renderer(_pMeshRenderer)
{
}

MeshComponent::~MeshComponent()
{
    IMeshRenderer*& renderer = GetLastSelectMeshRenderer();
    if (renderer == Renderer.Get())
    {
        renderer = nullptr;
    }
}

bool MeshComponent::HasModel() const
{
    return nullptr != Renderer->GetModel();
}

bool MeshComponent::HasAnimator() const
{
    return nullptr != Renderer->GetAnimator();
}

void MeshComponent::MakeMeshRenderer(const Matrix& world)
{
    if (nullptr == _pMeshRenderer)
    {
        UmGraphics.CreateMeshRenderer(&_pMeshRenderer, &world);
        _pMeshRenderer->SetActive(&EnableInHierarchy);
        _pMeshRenderer->OnCustomDepth(PostProcess::BLOOM);
        UmGraphics.RegisterComponent("Game", _pMeshRenderer.Get());

        if constexpr (IS_EDITOR)
        {
            UmGraphics.RegisterComponent("Editor", _pMeshRenderer.Get());
        }
    } 
    else
    {
        //assert(!"이미 MeshRenderer가 존재합니다.");
    }
}

void MeshComponent::SerializedReflectEvent()
{
    if (Renderer)
    {
        const auto& model = Renderer->GetModel();

        if (model)
        {
            auto&  materials = model->GetMaterials();
            size_t meshCount = model->GetMeshCount();

            for (size_t i = 0; i < meshCount; i++)
            {
                ReflectFields->IsTwoSided[i] = materials[i].IsTwoSided;
            }
        }
    }
}

void MeshComponent::ImGuiDrawPropertysEvent()
{
    ImGui::Separator();

    if (ImGui::TreeNodeEx("Materials##MeshComponent"))
    {
        DrawMaterialsList();
        ImGui::TreePop();
    }
}

void MeshComponent::DrawMaterialsList()
{
    static UINT*    lastCustomDepth = nullptr;
    static UINT     lastSelected    = 0;
    IMeshRenderer*& lastRenderer    = GetLastSelectMeshRenderer();

    const auto& model = Renderer->GetModel();
    if (model)
    {
        const auto& customDepths = Renderer->GetCustomDepths();
        auto&       materials    = Renderer->GetMaterials();
        const auto& meshes       = model->GetMeshes();
        UINT        count        = (UINT)model->GetMeshCount();

        for (UINT i = 0; i < count; i++)
        {
            ImGui::PushID(i);
            bool isOpened = ImGui::TreeNodeEx(meshes[i]->GetName().data());

            if (ImGui::IsItemClicked())
            {
                HandleMeshSelection(i, lastRenderer, lastCustomDepth, lastSelected);
            }

            if (isOpened)
            {
                DrawMaterialProperties(i, materials[i], customDepths[i]);
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }
}

void MeshComponent::HandleMeshSelection(UINT index, IMeshRenderer*& lastRenderer, UINT*& lastCustomDepth, UINT& lastSelected)
{
    if (lastRenderer)
    {
        (*lastCustomDepth) &= ~PostProcess::OUTLINE;
        lastRenderer->OffCustomDepth(PostProcess::OUTLINE, lastSelected);
    }

    ReflectFields->CustomDepth[index] |= PostProcess::OUTLINE;
    Renderer->OnCustomDepth(PostProcess::OUTLINE, index);
    
    lastSelected    = index;
    lastRenderer    = Renderer.Get();
    lastCustomDepth = &ReflectFields->CustomDepth[index];
}

void MeshComponent::DrawMaterialProperties(UINT index, Material& material, UINT customDepth)
{
    if (!ImGui::BeginTable("##MeshComponent", 2, ImGuiTableFlags_Borders))
        return;

    DrawShadingModelRow(index, material);

    if (material.ShadingModel == Material::ShadingModelType::CUSTOMLIT)
    {
        DrawCustomLitProperties(index, material);
    }
    else
    {
        DrawDefaultLitProperties(index, material);
    }

    DrawCustomDepthRow(index, customDepth);
    ImGui::EndTable();
}

void MeshComponent::DrawShadingModelRow(UINT index, Material& material)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Shading Model");
    
    ImGui::TableNextColumn();
    static const char* shadingModelNames[] = {"DefaultLit", "CustomLit"};
    if (ImGui::Combo("##ShadingModel", (int*)&material.ShadingModel, shadingModelNames, 2))
    {
        ReflectFields->ShadingModel[index] = material.ShadingModel;
    }
}

void MeshComponent::DrawCustomLitProperties(UINT index, Material& material)
{
    // Custom Light Type
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Custom Light Type");
    
    ImGui::TableNextColumn();
    static const char* customLightTypeNames[] = {"None", "Transparent Rim Light"};
    if (ImGui::Combo("##CustomLightType", (int*)&ReflectFields->CustomLightType, customLightTypeNames, 2))
    {
        Renderer->SetCustomMaterial(ReflectFields->CustomLightType, ReflectFields->RimLightMaterial);
    }

    // Rim Light Material Properties
    if (ReflectFields->CustomLightType == CustomLightType::TRANSPARENT_RIM_LIGHT)
    {
        DrawRimLightProperties(index);
    }
}

void MeshComponent::DrawRimLightProperties(UINT index)
{
    auto& rimLight = ReflectFields->RimLightMaterial;

    // Rim Color
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Rim Color");
    
    ImGui::TableNextColumn();
    if (ImGui::ColorEdit3("##RimColor", (float*)&rimLight.RimColor))
    {
        Renderer->SetCustomMaterial(ReflectFields->CustomLightType, ReflectFields->RimLightMaterial);
    }

    // Rim Power
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Rim Power");
    
    ImGui::TableNextColumn();
    if (ImGui::DragFloat("##RimPower", &rimLight.RimPower, 0.01f, 0.0f, 10.0f))
    {
        Renderer->SetCustomMaterial(ReflectFields->CustomLightType, ReflectFields->RimLightMaterial);
    }

    // Rim Intensity
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Rim Intensity");
    
    ImGui::TableNextColumn();
    if (ImGui::DragFloat("##RimIntensity", &rimLight.RimIntensity, 0.01f, 0.0f, 10.0f))
    {
        Renderer->SetCustomMaterial(ReflectFields->CustomLightType, ReflectFields->RimLightMaterial);
    }
}

void MeshComponent::DrawDefaultLitProperties(UINT index, Material& material)
{
    // Blend Mode
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Blend Mode");
    
    ImGui::TableNextColumn();
    static const char* blendModeNames[] = {"Opaque", "Masked", "Translucent"};
    if (ImGui::Combo("##BlendMode", (int*)&material.BlendMode, blendModeNames, 3))
    {
        ReflectFields->BlendMode[index] = material.BlendMode;
    }

    // Alpha (if Translucent)
    if (material.BlendMode == Material::BlendModeType::TRANSLUCENT)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Alpha");
        
        ImGui::TableNextColumn();
        ImGui::DragFloat("##Alpha", &material.Alpha, 0.01f, 0.f, 1.f);
    }

    // Is Two Sided
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Is Two Sided(Shared)");
    
    ImGui::TableNextColumn();
    if (ImGui::Checkbox("##IsTwoSided", &material.IsTwoSided))
    {
        ReflectFields->IsTwoSided[index] = material.IsTwoSided;

        Material modelMaterial{.IsTwoSided = material.IsTwoSided};
        Renderer->SetMasterMaterial(index, modelMaterial);
    }
}

void MeshComponent::DrawCustomDepthRow(UINT index, UINT customDepth)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("CustomDepth");
    
    ImGui::TableNextColumn();
    if (ImGui::BeginCombo("##CustomDepth", "Drop Down List"))
    {
        bool isBloom = customDepth & PostProcess::BLOOM;
        ImGuiHelper::TextWithVerticalSeparator("Bloom");
        
        if (ImGui::Checkbox("##Bloom", &isBloom))
        {
            isBloom ? Renderer->OnCustomDepth(PostProcess::BLOOM, index) 
                    : Renderer->OffCustomDepth(PostProcess::BLOOM, index);
        }

        ReflectFields->CustomDepth[index] = Renderer->GetCustomDepths()[index];
        ImGui::EndCombo();
    }
}

void MeshComponent::InitMaterial()
{
    const auto& model     = Renderer->GetModel();
    auto&       materials = Renderer->GetMaterials();
    size_t      meshCount = model->GetMeshCount();
    
    ReflectFields->ShadingModel.resize(meshCount, Material::ShadingModelType::DEFAULTLIT);
    ReflectFields->BlendMode.resize(meshCount, 0);
    ReflectFields->IsTwoSided.resize(meshCount, false);
    ReflectFields->CustomDepth.resize(meshCount, PostProcess::BLOOM);    

    for (size_t i = 0; i < meshCount; i++)
    {
        Renderer->OnCustomDepth(ReflectFields->CustomDepth[i], (UINT)i);

        materials[i].ShadingModel = (Material::ShadingModelType)ReflectFields->ShadingModel[i];
        materials[i].BlendMode    = (Material::BlendModeType)ReflectFields->BlendMode[i];
        materials[i].IsTwoSided   = ReflectFields->IsTwoSided[i];
        Renderer->SetMasterMaterial((UINT)i, materials[i]);
    }

    Renderer->OffCustomDepth(PostProcess::OUTLINE);
    Renderer->SetCustomMaterial(ReflectFields->CustomLightType, ReflectFields->RimLightMaterial);
}

IMeshRenderer*& MeshComponent::GetLastSelectMeshRenderer()
{
    static IMeshRenderer* renderer = nullptr;
    return renderer;
}
