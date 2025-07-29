#include "pch.h"
#include "EditorRenderPassData.h"

EditorRenderPassData::EditorRenderPassData()
{
    SetLabel("RenderPassData");
    SetDockLayout(ImGuiDir_Right);
}

EditorRenderPassData::~EditorRenderPassData()
{
}

// ShadowPassProperty를 문자열로 변환
void SerializeShadowProperty(std::ostream& os, const ShadowPassProperty& prop)
{
    os << "        Type = ShadowPassProperty\n";
    os << "        NearPlane = " << prop.NearPlane << "\n";
    os << "        FarPlane = " << prop.FarPlane << "\n";
    os << "        SplitFactor = " << prop.SplitFactor << "\n";
}

// 문자열에서 ShadowPassProperty를 복원
void DeserializeShadowProperty(std::istream& is, ShadowPassProperty& prop)
{
    std::string line, key, equals;
    while (std::getline(is, line) && line.find('}') == std::string::npos)
    {
        std::stringstream ss(line);
        ss >> key >> equals;
        if (key == "NearPlane") ss >> prop.NearPlane;
        else if (key == "FarPlane") ss >> prop.FarPlane;
        else if (key == "SplitFactor") ss >> prop.SplitFactor;
    }
}

void SaveData(const std::string& filePath)
{
    std::filesystem::path path(filePath);   
    
    path.replace_extension(".inl");    
    
    std::ofstream outFile(path);

    if (!outFile.is_open())
    {
        return;
    }

    auto& renderPassProperties = UmGraphics.GetRenderPassProperties();

    for (const auto& [sceneName, properties] : renderPassProperties)
    {
        outFile << "Scene " << sceneName << "\n{\n";
        for (const auto& [passName, pair] : properties)
        {
            outFile << "    Pass " << passName << "\n    {\n";
            const auto& property = pair.first;

            if (property.type() == typeid(ShadowPassProperty))
            {
                SerializeShadowProperty(outFile, std::any_cast<const ShadowPassProperty&>(property));
            }
            // else if (property.type() == typeid(AnotherProperty)) { ... } // 다른 타입 추가

            outFile << "    }\n";
        }
        outFile << "}\n\n";
    }
}

void LoadData(const std::string& filePath)
{
    std::ifstream inFile(filePath);
    if (!inFile.is_open())
    {
        return;
    }

    auto& renderPassProperties = UmGraphics.GetRenderPassProperties();
    std::string line, keyword, name;

    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        ss >> keyword >> name;

        if (keyword == "Scene")
        {
            std::string currentSceneName = name;
            std::getline(inFile, line); // '{' 라인 스킵

            while (std::getline(inFile, line) && line.find('}') == std::string::npos)
            {
                std::stringstream pass_ss(line);
                pass_ss >> keyword >> name;

                if (keyword == "Pass")
                {
                    std::string currentPassName = name;
                    std::getline(inFile, line); // '{' 라인 스킵

                    // Pass에 해당하는 속성 찾기
                    if (renderPassProperties.count(currentSceneName) && renderPassProperties[currentSceneName].count(currentPassName))
                    {
                        auto& property = renderPassProperties[currentSceneName][currentPassName].first;

                        // 타입 확인 및 복원
                        std::string typeLine;
                        std::getline(inFile, typeLine);
                        std::stringstream type_ss(typeLine);
                        type_ss >> keyword >> name >> name; // "Type = ShadowPassProperty"

                        if (name == "ShadowPassProperty" && property.type() == typeid(ShadowPassProperty))
                        {
                            DeserializeShadowProperty(inFile, std::any_cast<ShadowPassProperty&>(property));
                        }
                        // else if (name == "AnotherProperty") { ... }
                    }
                }
            }
        }
    }
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
    if (ImGui::Button("SaveData"))
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Save Files (*.inl)", L"*.inl\0\0"}};
        File::Path exportPath;
        bool result = File::ShowSaveFileDialog(NULL, L"Export Render Pass Data", L"", L"data.inl", filters, exportPath);
        if (result)
        {
            SaveData(exportPath.string());
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("LoadData"))
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters = {{L"Load Files (*.inl)", L"*.inl\0\0"}};
        File::Path importPath;
        bool result = File::ShowOpenFileDialog(NULL, L"Import Render Pass Data", L"", filters, importPath);
        if (result)
        {
            LoadData(importPath.string());
        }
    }
}