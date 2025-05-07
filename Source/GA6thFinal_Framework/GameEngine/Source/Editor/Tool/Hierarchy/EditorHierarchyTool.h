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

namespace Command
{
    namespace Hierarchy
    {
        class FocusCommand : public Command::Inspector::FocusObject
        {
            using Super = Command::Inspector::FocusObject;
        public:
            FocusCommand(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp) 
                : FocusObject(oldWp, newWp)                
            {
                
            }
            virtual ~FocusCommand() override;

            virtual void Execute() override;
            virtual void Undo() override;
        };
    }
}
