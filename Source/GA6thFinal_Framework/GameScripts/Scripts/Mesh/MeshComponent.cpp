#include "pchScripts.h"
#include "MeshComponent.h"
#include "GraphicsEngine/Interface/IMeshRenderer.h"

REFLECT_FUNCTION(MeshComponent)

MeshComponent::MeshComponent() 
    : Component(Component::TYPE::MESH)
    , Renderer(_pMeshRenderer)
{
}

MeshComponent::~MeshComponent() = default;

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
                ReflectFields->BlendMode[i]  = materials[i].BlendMode;
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
        static UINT*          lastCustomDepth = 0;
        static IMeshRenderer* lastRenderer    = nullptr;
        static UINT           lastSelected    = 0;

        const auto& model        = Renderer->GetModel();
        const auto& customDepths = Renderer->GetCustomDepths();
        const auto& meshes       = model->GetMeshes();
        auto&       materials    = model->GetMaterials();
        UINT        count        = (UINT)model->GetMeshCount();

        for (UINT i = 0; i < count; i++)
        {
            ImGui::PushID(i);
            bool isOpened = ImGui::TreeNodeEx(meshes[i]->GetName().data());
            if (ImGui::IsItemClicked())
            {
                if (lastRenderer)
                {
                    (*lastCustomDepth) &= ~PostProcess::OUTLINE;
                    lastRenderer->OffCustomDepth(PostProcess::OUTLINE, lastSelected);
                }

                ReflectFields->CustomDepth[i] |= PostProcess::OUTLINE;
                Renderer->OnCustomDepth(PostProcess::OUTLINE, i);
                
                // 마지막 선택한 값 기억
                lastSelected    = i;
                lastRenderer    = Renderer.Get();
                lastCustomDepth = &ReflectFields->CustomDepth[i];
            }
            if (isOpened)
            {
                ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable;
                if (ImGui::BeginTable("##MeshComponent", 2, ImGuiTableFlags_Borders))
                {
                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        {
                            ImGui::Text("Blend Mode");
                        }
                        ImGui::TableNextColumn();
                        {
                            static const char* blendModeNames[] = {"Opaque", "Masked", "Translucent"};
                            if (ImGui::Combo("##BlendMode", (int*)&materials[i].BlendMode, blendModeNames, 3))
                            {
                                ReflectFields->BlendMode[i] = materials[i].BlendMode;
                            }
                        }
                        ImGui::TableNextColumn();
                    }
                    if (materials[i].BlendMode == Material::BlendModeType::TRANSLUCENT)
                    {
                        ImGui::TableNextRow();
                        {
                            ImGui::TableNextColumn();
                            {
                                ImGui::Text("Alpha");
                            }
                            ImGui::TableNextColumn();
                            {
                                ImGui::DragFloat("##Alpha", &materials[i].Alpha, 0.01f, 0.f, 1.f);
                            }
                            ImGui::TableNextColumn();
                        }
                    }
                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        {
                            ImGui::Text("Is Two Sided(Shared)");
                        }
                        ImGui::TableNextColumn();
                        {
                            if (ImGui::Checkbox("##IsTwoSided", &materials[i].IsTwoSided))
                            {
                                ReflectFields->IsTwoSided[i] = materials[i].IsTwoSided;
                            }
                        }
                        ImGui::TableNextColumn();
                    }

                    ImGui::TableNextRow();
                    {
                        ImGui::TableNextColumn();
                        {
                            ImGui::Text("CustomDepth");
                        }
                        ImGui::TableNextColumn();
                        {
                            if (ImGui::BeginCombo("##CustomDepth", "Drop Down List"))
                            {
                                bool isBloom = customDepths[i] & PostProcess::BLOOM ? true : false;
                                ImGuiHelper::TextWithVerticalSeparator("Bloom");
                                if (ImGui::Checkbox("##Bloom", &isBloom))
                                {
                                    isBloom ? Renderer->OnCustomDepth(PostProcess::BLOOM, i) : 
                                              Renderer->OffCustomDepth(PostProcess::BLOOM, i);

                                }

                                ReflectFields->CustomDepth[i] = customDepths[i];
                                ImGui::EndCombo();
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        ImGui::TreePop();
    }    
}

void MeshComponent::InitMaterial()
{
    const auto& model     = Renderer->GetModel();
    auto&       materials = model->GetMaterials();
    size_t      meshCount = model->GetMeshCount();

    if (ReflectFields->CustomDepth.size() < meshCount)
    {
        ReflectFields->BlendMode.resize(meshCount, 0);
        ReflectFields->IsTwoSided.resize(meshCount, false);
        ReflectFields->CustomDepth.resize(meshCount, PostProcess::BLOOM);
    }

    for (size_t i = 0; i < meshCount; i++)
    {
        Renderer->OnCustomDepth(ReflectFields->CustomDepth[i], (UINT)i);

        //materials[i].ShadingModel = (Material::ShadingModelType)ReflectFields->ShadingModel[i];
        materials[i].BlendMode    = (Material::BlendModeType)ReflectFields->BlendMode[i];
        materials[i].IsTwoSided   = ReflectFields->IsTwoSided[i];
    }

    Renderer->OffCustomDepth(PostProcess::OUTLINE);
}