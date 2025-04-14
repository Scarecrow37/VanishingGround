#include "EditorInspectorView.h"

EditorInspectorView::EditorInspectorView()
{
    SetLabel("InspectorView");
    SetInitialDockSpaceArea(DockSpaceArea::RIGHT);
}

EditorInspectorView::~EditorInspectorView()
{
}

void  EditorInspectorView::OnStartGui()
{
    return void();
}

void  EditorInspectorView::OnPreFrame()
{
    return void();
}

void  EditorInspectorView::OnFrame()
{
    if (false == _focusedObject.expired())
    {
        auto spFocusedObject = _focusedObject.lock();
        spFocusedObject->OnDrawInspectorView();
    }
    //TestGui();
    return void();
}

void  EditorInspectorView::OnPostFrame()
{
    return void();
}

void EditorInspectorView::OnFocus()
{
}

void EditorInspectorView::TestGui()
{
    const float     TEXT_BASE_WIDTH  = ImGui::CalcTextSize("A").x;
    const float     TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImGuiTableFlags flags            = ImGuiTableFlags_BordersV |
                            ImGuiTableFlags_BordersOuterH |
                            ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_NoBordersInBody;

    static ImGuiTreeNodeFlags tree_node_flags =
        ImGuiTreeNodeFlags_SpanAllColumns;

    ImGuiHelper::AlignedText("asd", ImGuiHelper::CENTER, 1.5f);

    ImGui::TextUnformatted("\xef\x85\x9b"); // FILE 아이콘 (FontAwesome 6)
    ImFont* font = ImGui::GetFont();
    ImGui::Text("Font Debug: %s", font->GetDebugName());
    ImGui::Text(
        "Test: \xef\x80\x95 \xef\x89\x83 \xef\x83\x9b"); // Home, 톱니바퀴, 폴더
                                                         // 아이콘

    ImGui::SameLine();
    ImGuiHelper::LoadingSpinner();

    ImGuiHelper::Separator(15.0f, 0.0f);
    ImGui::Text("Section 2");

    ImGui::Text("Label: %s", GetLabel().c_str());
    ImGui::Text("Flag: %d", _windowFlags);
    ImGui::Text("DockID: %d", ImGui::GetWindowDockID());

    if (ImGui::TreeNode("Reorderable, hideable, with headers"))
    {
        ImGuiHelper::TooltipMarker(
            "Click and drag column headers to reorder columns.\n\n"
            "Right-click on a header to open a context menu.",
            "(_!_)");
        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
            ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersV;

        ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags,
                             ImGuiTableFlags_Resizable);
        ImGui::CheckboxFlags("ImGuiTableFlags_Reorderable", &flags,
                             ImGuiTableFlags_Reorderable);
        ImGui::CheckboxFlags("ImGuiTableFlags_Hideable", &flags,
                             ImGuiTableFlags_Hideable);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBody", &flags,
                             ImGuiTableFlags_NoBordersInBody);
        ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBodyUntilResize",
                             &flags,
                             ImGuiTableFlags_NoBordersInBodyUntilResize);
        ImGui::SameLine();
        ImGuiHelper::TooltipMarker(
            "Disable vertical borders in columns Body until hovered for resize "
            "(borders will always appear in Headers)");
        ImGui::CheckboxFlags("ImGuiTableFlags_HighlightHoveredColumn", &flags,
                             ImGuiTableFlags_HighlightHoveredColumn);

        if (ImGui::BeginTable("table1", 3, flags))
        {
            // Submit columns name with TableSetupColumn() and call
            // TableHeadersRow() to create a row with a header in each column.
            // (Later we will show how TableSetupColumn() has other uses,
            // optional flags, sizing weight etc.)
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");
            ImGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Hello %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }

        // Use outer_size.x == 0.0f instead of default to make the table as
        // tight as possible (only valid when no scrolling and no stretch
        // column)
        if (ImGui::BeginTable("table2", 3,
                              flags | ImGuiTableFlags_SizingFixedFit,
                              ImVec2(0.0f, 0.0f)))
        {
            ImGui::TableSetupColumn("One");
            ImGui::TableSetupColumn("Two");
            ImGui::TableSetupColumn("Three");
            ImGui::TableHeadersRow();
            for (int row = 0; row < 6; row++)
            {
                ImGui::TableNextRow();
                for (int column = 0; column < 3; column++)
                {
                    ImGui::TableSetColumnIndex(column);
                    ImGui::Text("Fixed %d,%d", column, row);
                }
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if (ImGui::BeginTable("3ways", 3, flags))
    {
        // The first column will use the default _WidthStretch when ScrollX is
        // Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed,
                                TEXT_BASE_WIDTH * 12.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed,
                                TEXT_BASE_WIDTH * 18.0f);
        ImGui::TableHeadersRow();

        // Simple storage to output a dummy file-system.
        struct MyTreeNode
        {
            const char* Name;
            const char* Type;
            int         Size;
            int         ChildIdx;
            int         ChildCount;
            static void DisplayNode(const MyTreeNode* node,
                                    const MyTreeNode* all_nodes)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                const bool is_folder = (node->ChildCount > 0);
                if (is_folder)
                {
                    bool open = ImGui::TreeNodeEx(node->Name, tree_node_flags);
                    ImGui::TableNextColumn();
                    ImGui::TextDisabled("--");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(node->Type);
                    if (open)
                    {
                        for (int child_n = 0; child_n < node->ChildCount;
                             child_n++)
                            DisplayNode(&all_nodes[node->ChildIdx + child_n],
                                        all_nodes);
                        ImGui::TreePop();
                    }
                }
                else
                {
                    ImGui::TreeNodeEx(
                        node->Name, tree_node_flags | ImGuiTreeNodeFlags_Leaf |
                                        ImGuiTreeNodeFlags_Bullet |
                                        ImGuiTreeNodeFlags_NoTreePushOnOpen);
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", node->Size);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(node->Type);
                }
            }
        };
        static const MyTreeNode nodes[] = {
            {"Root", "Folder", -1, 1, 3},                           // 0
            {"Music", "Folder", -1, 4, 2},                          // 1
            {"Textures", "Folder", -1, 6, 3},                       // 2
            {"desktop.ini", "System file", 1024, -1, -1},           // 3
            {"File1_a.wav", "Audio file", 123000, -1, -1},          // 4
            {"File1_b.wav", "Audio file", 456000, -1, -1},          // 5
            {"Image001.png", "Image file", 203128, -1, -1},         // 6
            {"Copy of Image001.png", "Image file", 203256, -1, -1}, // 7
            {"Copy of Image001 (Final2).png", "Image file", 203512, -1,
             -1}, // 8
        };

        MyTreeNode::DisplayNode(&nodes[0], nodes);

        ImGui::EndTable();
    }
}