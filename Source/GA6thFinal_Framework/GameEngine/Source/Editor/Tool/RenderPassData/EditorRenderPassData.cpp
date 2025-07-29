#include "pch.h"
#include "EditorRenderPassData.h"

EditorRenderPassData::EditorRenderPassData()
{
    //SetVisible(false);
    SetLabel("RenderPassData");
    SetDockLayout(ImGuiDir_Right);
}

EditorRenderPassData::~EditorRenderPassData()
{
}

void EditShadowProperty(std::any& property)
{
    auto& shadowProps = std::any_cast<ShadowPassProperty&>(property);

    ImGui::DragFloat("Near Plane", &shadowProps.NearPlane, 0.01f, 0.01f, 100.0f);
    ImGui::DragFloat("Far Plane", &shadowProps.FarPlane, 0.1f, 0.1f, 1000.0f);
    ImGui::DragFloat("Split Factor", &shadowProps.SplitFactor, 0.01f, 0.01f, 1.0f);
}

void EditorRenderPassData::OnFrameRender()
{
    auto& renderPassProperties = UmGraphics.GetRenderPassProperties();
    for (auto& [sceneName, properties] : renderPassProperties)
    {
        if (ImGui::TreeNodeEx(sceneName.c_str()))
        {
            for (auto& [passName, pair] : properties)
            {
                if (ImGui::TreeNodeEx(passName.c_str()))
                {
                    auto& [property, images] = pair;
                    
                    if (ImGui::TreeNodeEx("Properties"))
                    {
                        if (property.type() == typeid(ShadowPassProperty))
                        {                            
                            EditShadowProperty(property);
                        }

                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNodeEx("Images"))
                    {
                        for (const auto& [dataName, handles] : images)
                        {
                            if (ImGui::TreeNodeEx(dataName.c_str()))
                            {
                                for (const auto& handle : handles)
                                {
                                    ImVec2 availSize = ImGui::GetContentRegionAvail();
                                    ImGui::Image((ImTextureID)handle.ptr, ImVec2(availSize.x * 0.5f, availSize.x * 0.5f));
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
}