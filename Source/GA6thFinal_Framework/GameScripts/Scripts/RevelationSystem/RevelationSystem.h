#pragma once
#include "RevelationAction/Base/RevelationActionBase.h"
#include "RevelationElement/RevelationElement.h"

//RevelationActionBase를 상속받은 클래스를 팩토리에 등록할때 사용합니다.
#define REGISTER_REVELATION_CLASS(REVELATION) REGISTER_CLASS(RevelationSystem, REVELATION)

class RevelationSystem : public Component, public FactoryConstructor<RevelationActionBase>
{
    USING_PROPERTY(RevelationSystem)      
public:
    static RevelationSystem* GetInstance() { return static_instance; }

public:
    RevelationSystem();
    ~RevelationSystem() override;

public:
    /// <summary>
    /// 액션을 가져옵니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    template<typename T>
    RevelationActionBase* GetAction()
    {
        static_assert(std::is_base_of_v<RevelationActionBase, T>("T must be derived from RevelationActionBase."));
        return GetActionToName(typeid(T).name());
    }

    /// <summary>
    /// 액션을 typeid로 가져옵니다
    /// </summary>
    /// <param name="typeidName"></param>
    /// <returns></returns>
    RevelationActionBase* GetActionToName(std::string_view typeidName) 
    { 
        const char* key = typeidName.data();
        return _actions[key].get(); 
    }

    /// <summary>
    /// Element를 가져옵니다.
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    const RevelationElement* GetElement(std::string_view name) const
    { 
        const RevelationElement* element  = nullptr;
        auto findIter = _elements.find(name.data());
        if (findIter != _elements.end())
        {
            element = &findIter->second;
        }
        return element;
    }

    /// <summary>
    /// 새로운 Element를 테이블에 추가합니다.
    /// </summary>
    /// <param name="element"></param>
    bool InsertElement(const RevelationElement& element);

    /// <summary>
    /// Element를 테이블에서 제거합니다.
    /// </summary>
    /// <param name="elementName"></param>
    /// <returns></returns>
    bool EraseElement(std::string_view elementName);

private:
    bool _tableEditorOpen = false;

    struct ImGuiEvent
    {
        bool               OpenRenamePopup   = false;
        std::string        RenameBuffer      = STR_NULL;
        RevelationElement* SelectElement     = nullptr;

        std::string        DeleteTableBuffer = STR_NULL;
        bool               OpenDeletePopup   = false;
    }
    _imguiEvent;

    void DrawImGuiElementTableEditor();

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::pair<std::string, std::string>> RevelationActionDatas;
    std::vector<std::pair<std::string, std::string>> RevelationElementDatas;
    REFLECT_FIELDS_END(RevelationSystem)

    void ActionsToActionDatas();
    void ActionDatasToActions();

    void ElementsToElementDatas();
    void ElementDatasToElements();

    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    void SerializedReflectEvent() override;

    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    void DeserializedReflectEvent() override;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

    void Reset() override;

private:
    inline static RevelationSystem* static_instance = nullptr;

private:
    std::unordered_map<std::string, std::unique_ptr<RevelationActionBase>> _actions;
    std::unordered_map<std::string, RevelationElement>                     _elements;

};
