#include "pch.h"
#include "EditorNodeGraph.h"

EditorNodeGraph::EditorNodeGraph() 
    : _editor(nullptr), _state({}), _uniqueID(0)
{
    ed::Config config;
    config.UserPointer = this;

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason,
                                 void* userPointer) -> bool {
        auto self = static_cast<EditorNodeGraph*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return false;

        //node->State.assign(data, size);
        //self->TouchNode(nodeId);

        return true;
    };

    //config.SaveSettings = [](const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    //{
    //    auto self = static_cast<EditorNodeGraph*>(userPointer);
    //    self->SaveData(data, size);
    //
    //    return true;
    //};

    _editor = ed::CreateEditor(&config);
}

EditorNodeGraph::~EditorNodeGraph() 
{
    ed::DestroyEditor(_editor);
    _editor = nullptr;
}

namespace util = ax::NodeEditor::Utilities;

void EditorNodeGraph::Render() 
{
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ed::SetCurrentEditor(_editor);

    // 그리드 출력
    ImGui::PushID(this);
    ed::Begin("Node editor");
    {
        DrawNodes();
    }
    ed::End();
    ImGui::PopID();
}

NodeGraph::Node* EditorNodeGraph::FindNode(ed::NodeId id)
{
    auto itr = _nodeTable.find(id.Get());
    if (itr != _nodeTable.end())
        return itr->second;
    return nullptr;
}

NodeGraph::Link* EditorNodeGraph::FindLink(ed::LinkId id)
{
    auto itr = _linkTable.find(id.Get());
    if (itr != _linkTable.end())
        return itr->second;
    return nullptr;
}

void EditorNodeGraph::SaveData(const char* data, size_t size) 
{
}

void EditorNodeGraph::LoadData(const std::string& data) 
{

}

const char* EditorNodeGraph::SaveNodeSettingsToMemory()
{
    return ReflectFields->SerializeData.data();
}

void EditorNodeGraph::LoadNodeSettingsFromMemory(const std::string& data) 
{
}

const char* EditorNodeGraph::GetNodeSettingsData()
{
    return ReflectFields->SerializeData.data();
}

void EditorNodeGraph::SerializedReflectEvent() 
{
    ReflectFields->SerializeData = ed::SaveIniSettingsToMemory();
}

void EditorNodeGraph::DeserializedReflectEvent() 
{
}

void EditorNodeGraph::DrawNodes()
{
    util::BlueprintNodeBuilder builder;
    for (auto& node : _nodeVector)
    {
        node->Draw();
    }
}