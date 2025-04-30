#pragma once

class EditorHierarchyTool
    : public EditorTool
{
public:
    EditorHierarchyTool();
    virtual ~EditorHierarchyTool();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnPopup() override;

    //드래그 드롭 이벤트
    void HierarchyDropEvent();

    //빈 공간 우클릭시
    void HierarchyRightClickEvent() const;

    ImGuiWindow* window = nullptr;
};

class SampleCommand : public UmCommand
{
public:
    SampleCommand() : UmCommand("Inspector Focused") {}
    ~SampleCommand() override = default;

public:
    // UmCommand을(를) 통해 상속됨
    void Execute() override;
    void Undo() override;
};