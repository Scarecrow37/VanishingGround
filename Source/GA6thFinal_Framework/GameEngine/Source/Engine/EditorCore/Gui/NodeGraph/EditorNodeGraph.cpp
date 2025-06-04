#include "pch.h"
#include "EditorNodeGraph.h"

EditorNodeGraph::EditorNodeGraph() 
    : _editor(nullptr)
    , _serializeData("")
{
    ed::Config config;
    config.UserPointer = this;

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason,
                                 void* userPointer) -> bool {
        auto self = static_cast<EditorNodeGraph*>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return false;

        node->State.assign(data, size);
        //self->TouchNode(nodeId);

        return true;
    };

    config.SaveSettings = [](const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
    {
        auto self = static_cast<EditorNodeGraph*>(userPointer);
        self->SaveData(data, size);

        return true;
    };

    _editor = ed::CreateEditor(&config);
}

EditorNodeGraph::~EditorNodeGraph() 
{
    ed::DestroyEditor(_editor);
    _editor = nullptr;
}

void EditorNodeGraph::Render() 
{
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ed::SetCurrentEditor(_editor);

    // 그리드 출력
    ImGui::PushID(this);
    ed::Begin("Node editor");
    {

    }
    ed::End();
    ImGui::PopID();
}

NodeGraph::Node* EditorNodeGraph::FindNode(ed::NodeId id)
{
    for (auto& node : _nodeVector)
        if (node.ID == id)
            return &node;

    return nullptr;
}

NodeGraph::Link* EditorNodeGraph::FindLink(ed::LinkId id)
{
    for (auto& link : _linkedVector)
        if (link.ID == id)
            return &link;

    return nullptr;
}

void EditorNodeGraph::SaveData(const char* data, size_t size) 
{

}

void EditorNodeGraph::LoadData(const std::string& data) {

}

const char* EditorNodeGraph::SaveNodeSettingsToMemory()
{

    return _serializeData.data();
}

void EditorNodeGraph::LoadNodeSettingsFromMemory(const std::string& data) 
{
}

const char* EditorNodeGraph::GetNodeSettingsData()
{
    return _serializeData.data();
}
