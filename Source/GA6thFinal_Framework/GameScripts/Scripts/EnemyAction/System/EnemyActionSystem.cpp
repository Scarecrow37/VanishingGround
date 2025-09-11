#include "pchScripts.h"
#include "EnemyActionSystem.h"

EnemyActionSystem::EnemyActionSystem() 
{

}

EnemyActionSystem::~EnemyActionSystem()
{

    for (auto& [id, data] : _enemyActionTable)
    {
        if (data)
        {
            delete data;
        }
    }
}

void EnemyActionSystem::Reset() 
{
    _singletonComponent.SetSingleTon();
}

void EnemyActionSystem::Awake() 
{
    _singletonComponent.TrySingleTon();
}

void EnemyActionSystem::SerializedReflectEvent() 
{
    for (auto& [id, data] : _enemyActionTable)
    {
        if (data)
        {
            ReflectFields->ActionSerializeDataTable[id] = data->SerializedReflectFields();
        }
    }
}

void EnemyActionSystem::DeserializedReflectEvent() 
{
    for (const auto& [id, str] : ReflectFields->ActionSerializeDataTable)
    {
        EnemyAction::ActionData* data = new EnemyAction::ActionData;
        data->DeserializedReflectFields(str);
        _enemyActionTable[id] = data;
    }
}

void EnemyActionSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Show Editor"))
    {
        _isShowEditor = !_isShowEditor;
    }
    for (auto& [id, data] : _enemyActionTable)
    {
    }
    ShowEditor();
}

EnemyAction::ActionData* EnemyActionSystem::GetEnemyActionDataFromID(int actionID)
{
    auto itr = _enemyActionTable.find(actionID);
    if (itr != _enemyActionTable.end())
    {
        return itr->second;
    }
    return nullptr;
}

EnemyAction::ActionData* EnemyActionSystem::AddEnemyActionDataFromID(int actionID)
{
    auto* data = GetEnemyActionDataFromID(actionID);
    if (nullptr == data)
    {
        data = new EnemyAction::ActionData(actionID, "New Action", "", 0);
        _enemyActionTable[actionID] = data;
        return data;
    }
    return data;
}

Timeline::SequencerEditor& EnemyActionSystem::GetSequencerEditor()
{
    static bool isInitialized = false;
    static Timeline::SequencerEditor sequencerEditor;
    if (false == isInitialized)
    {
        // 콜백 처리
        auto& callback = sequencerEditor.GetCallback();
        //sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_ALL_INPUT);
        //sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_USE_DEBUG_MODE);
        sequencerEditor.AddFlags(Timeline::SequencerEditor::FLAGS_ALLOW_POPUP_LOWER_CANVAS_MENU);
        isInitialized = true;
    }
    return sequencerEditor;
}

void EnemyActionSystem::ShowEditor()
{
    Timeline::SequencerEditor& sequencerEditor = GetSequencerEditor();

    if (_isShowEditor)
    {
        ImGui::Begin("Enemy Action Editor", &_isShowEditor, ImGuiWindowFlags_NoMove);

        ImRect tabRect  = ImGuiHelper::GetWindowTabBarRect();
        ImVec2 mousePos = ImGui::GetMousePos();

        int state = _dragHandler.BeginDragState("WindowTab", tabRect, mousePos, ImGuiMouseButton_Left);
        switch (state)
        {
            case EditorDragState::DRAG_STATE_DRAGGING:
            {
                ImVec2 newPos = tabRect.Min + ImGui::GetIO().MouseDelta;
                ImGui::SetWindowPos(newPos);
                break;
            }
            default:
                break;
        }

        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        ImVec2 left          = ImVec2(200.0f, availableSize.y);
        ImVec2 right         = ImVec2(availableSize.x - left.x, availableSize.y);

        LeftGuiFrame(left);
        ImGui::SameLine();
        RightGuiFrame(right);
       
        ImGui::End();
    }
}

void EnemyActionSystem::LeftGuiFrame(ImVec2 size) 
{
    // Left Window
    ImGui::BeginChild("##left", size, ImGuiChildFlags_Border);
    if (ImGui::CollapsingHeader("Action List##left", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& [id, data] : _enemyActionTable)
        {
            if (data)
            {
                bool isSelected = (_selectedActionID == id);
                std::string_view actionName = data->ActionName;
                std::string label = std::format("{} : {}", id, actionName); // ID를 포함한 라벨 생성
                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    _selectedActionID = id; // 선택된 액션 ID를 저장
                    //auto& sequenceEditor = GetSequencerEditor();
                    //sequenceEditor.SetEventTrack(data->EventTrack);
                    //data->EventTrack->SetMaxFrame(2.0f); // 기본 최대 프레임 설정
                }
            }
        }
        ImGui::Separator();
        if (ImGui::Selectable("Add Action##left"))
        {
            ImGui::OpenPopup("##AddActionPopup");
        }
        if (ImGui::BeginPopup("##AddActionPopup"))
        {
            static int id = 0;
            ImVec2      popupSize       = ImGui::GetContentRegionAvail();
            const char* addActionLabel  = "Add Action";
            const char* cancelLabel     = "Cancel";
            ImVec2      padding         = ImGui::GetStyle().FramePadding;
            ImVec2      labelSize       = ImGui::CalcTextSize(addActionLabel);
            ImVec2      buttonSize      = ImVec2(labelSize + padding * 2.0f);

            ImGuiHelper::TextWithVerticalSeparatorEx("Action ID");
            ImGui::InputInt("##Action ID", &id, 0, 0);

            if (ImGui::Button(addActionLabel, buttonSize))
            {
                if (id > 0)
                {
                    AddEnemyActionDataFromID(id);
                    _selectedActionID = id; // 새로 추가한 액션을 선택
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(cancelLabel, buttonSize))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
}

void EnemyActionSystem::RightGuiFrame(ImVec2 size) 
{
    // Right Window
    auto& sequenceEditor = GetSequencerEditor();
    ImGui::BeginChild("##right", size, ImGuiChildFlags_Border);
    auto* selectedAction = GetEnemyActionDataFromID(_selectedActionID);
    if (selectedAction)
    {
        if (ImGui::CollapsingHeader("Action Properties", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float width = ImGui::CalcTextSize("#############").x;

            // ID
            ImGuiHelper::TextWithVerticalSeparator("Action ID", width);
            int id = selectedAction->ActionID;
            if (ImGui::InputInt("##Action ID", &id, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                ChangeActionID(_selectedActionID, id);
            }   

            // Name
            ImGuiHelper::TextWithVerticalSeparator("Action Name", width);
            std::string actionName = std::string(selectedAction->ActionName); // ImGui::InputText는 std::string을 사용하므로 변환
            if (ImGui::InputText("##Action Name", &actionName))
            {
                selectedAction->ActionName = actionName;
            }

            ImGui::Separator();

            ImGui::BeginChild("##sequencer", ImVec2(0,0), ImGuiChildFlags_Border, ImGuiWindowFlags_MenuBar);
            sequenceEditor.Show();
            ImGui::EndChild();
        }
    }
    ImGui::EndChild();
}

bool EnemyActionSystem::ChangeActionID(int oldID, int newID) 
{
    auto oldIt = _enemyActionTable.find(oldID);
    auto newIt = _enemyActionTable.find(newID);
    // 새로운 ID가 이미 존재하지 않는 경우에만 변경
    if (newIt == _enemyActionTable.end())
    {
        EnemyAction::ActionData* oldData = oldIt->second;
        _enemyActionTable.erase(oldIt);
        if (oldData)
        {
            oldData->ActionID        = newID;   // ID 변경
            _selectedActionID        = newID;   // 새로 추가한 액션을 선택
            _enemyActionTable[newID] = oldData; // 새 ID로 재등록
            return true;
        }
    }
    return false;
}
