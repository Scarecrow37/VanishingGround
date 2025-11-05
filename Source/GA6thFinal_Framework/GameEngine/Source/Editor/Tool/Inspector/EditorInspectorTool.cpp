#include "pch.h"
#include "EditorInspectorTool.h"

#define SAFE_CALL(ptr, func) if (nullptr != ptr) ptr->func; // nullptr 체크용

EditorInspectorTool::EditorInspectorTool()
{
    SetLabel("Inspector");
    SetDockLayout(ImGuiDir_Right);

    SetImGuiWindowFlag(ImGuiWindowFlags_MenuBar); // 메뉴바 사용
}

EditorInspectorTool::~EditorInspectorTool()
{
}

void EditorInspectorTool::OnEndGui() 
{
    _currFocused.reset();
    _nextFocused.reset();
    _rowPtrCurrFocused = nullptr;
    _rowPtrNextFocused = nullptr;
}


void EditorInspectorTool::OnPostFrameBegin() 
{
    if (false == _currFocused.expired())
    {
        _rowPtrCurrFocused = _currFocused.lock().get();
    }
    else
    {
        _rowPtrCurrFocused = nullptr;
    }

    if (false == _nextFocused.expired())
    {
        _rowPtrNextFocused = _nextFocused.lock().get();
    }
    else
    {
        _rowPtrNextFocused = nullptr;
    }

    if (nullptr == _rowPtrCurrFocused)
    {
        _isLockFocus = false;
    }

    SAFE_CALL(_rowPtrCurrFocused, OnInspectorBegin());
}
 
void EditorInspectorTool::OnFrameRender()
{
    ShowMenuBarFrame();
    if (true == _isFocusChanged)
    {
        SAFE_CALL(_rowPtrCurrFocused, OnInspectorExit());
        SAFE_CALL(_rowPtrNextFocused, OnInspectorEnter());
        _rowPtrCurrFocused = _rowPtrNextFocused;
        _currFocused       = _nextFocused;
        _isFocusChanged    = false;
    }
    SAFE_CALL(_rowPtrCurrFocused, OnInspectorStay());
}

void EditorInspectorTool::OnFrameEnd()
{
}

void EditorInspectorTool::ShowMenuBarFrame()
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::MenuItem("Lock", nullptr, &_isLockFocus);

        ImGui::EndMenuBar();
    }
}

bool EditorInspectorTool::IsFocusObject(std::weak_ptr<IEditorObject> obj)
{
    bool ownerIsExpired = _currFocused.expired();
    bool objIsExpired   = obj.expired();
    if (false == ownerIsExpired && false == objIsExpired)
    {
        bool isSame = obj.lock() == _currFocused.lock();
        return isSame;
    }
    return false;
}

bool EditorInspectorTool::SetFocusObject(std::weak_ptr<IEditorObject> obj, bool breakLock)
{
    if (false == _isLockFocus || true == breakLock)
    {
        _nextFocused = obj;
        _isFocusChanged = true;
        if (true == breakLock)
        {
            _isLockFocus = false;
        }
        return true;
    }
    return false;
}

void EditorInspectorTool::ResetFocusObject() 
{
    if (true == SetFocusObject(std::weak_ptr<IEditorObject>()))
    {
        EditorHierarchyTool::SetFocusObject(std::weak_ptr<GameObject>());  // HierarchyTool의 포커스도 초기화
        EditorSceneTool::SetManipulateObject(std::weak_ptr<GameObject>()); // SceneTool의 조작 오브젝트도 초기화
    }
}

std::weak_ptr<IEditorObject> EditorInspectorTool::GetFocusObject()
{
    return _currFocused;
}

bool EditorInspectorTool::SetLockFocus(bool isLock)
{
    _isLockFocus = isLock;
    return true;
}

namespace Command
{
    namespace Inspector
    {
        bool FocusObject::Execute()
        {
            if (false == _newFocused.expired())
            {
                EditorInspectorTool::SetFocusObject(_newFocused);
            }
            return true;
        }

        void FocusObject::Undo()
        {
            if (false == _oldFocused.expired())
            {
                EditorInspectorTool::SetFocusObject(_oldFocused);
            }
        }

        bool LockFocus::Execute() 
        {
            EditorInspectorTool::SetLockFocus(_isLock);
            return true;
        }

        void LockFocus::Undo() 
        {
            EditorInspectorTool::SetLockFocus(!_isLock);
        }
    } // namespace Inspector
}
