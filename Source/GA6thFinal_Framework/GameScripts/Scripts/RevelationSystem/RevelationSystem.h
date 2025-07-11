#pragma once
#include "RevelationAction/Base/RevelationActionBase.h"
#include "RevelationElement/RevelationElement.h"

class RevelationSystem : public Component, public FactoryConstructor<RevelationActionBase>
{
    USING_PROPERTY(RevelationSystem)      
    using ActionDataType = std::unordered_map<std::string, std::string>;
    using ElementDataType = std::vector<std::string>;
public:
    static RevelationSystem* GetInstance() 
    {
        if (static_instance)
        {
            return static_instance; 
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Revelation System이 존재하지 않습니다.!!!!!!!!");
            return nullptr;
        }
    }

public:
    RevelationSystem();
    ~RevelationSystem() override;

public:
    const std::unordered_map<std::string, std::function<RevelationActionBase* ()>>& GetActionFactory()
    {
        return _actionConstructors;
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

    /// <summary>
    /// ElementTable과 Action들을 Json으로 직렬화해 반환합니다.
    /// </summary>
    /// <returns></returns>
   std::string SaveElementTable() 
    { 
        ElementsToElementDatas();
        ActionsToActionDatas();
        std::string result = rfl::json::write(std::pair{rfl::json::write(ReflectFields->RevelationElementDatas),
                                                        rfl::json::write(ReflectFields->RevelationActionDatas)});
        return result; 
    }

    /// <summary>
    /// Json으로 직렬화한 Element Table과 Action들을 역직렬화 합니다.
    /// </summary>
    /// <param name="data :">json 형식의 문자열</param>
    /// <returns>결과</returns>
    bool LoadElementTable(std::string_view data)
    {
        auto result = rfl::json::read<std::pair<std::string, std::string>>(data.data());
        if (result)
        {
            auto& [elementData, actionData] = result.value();
            auto element = rfl::json::read<ElementDataType>(elementData.data());
            if (element)
            {
                ReflectFields->RevelationElementDatas = element.value();
                ElementDatasToElements();
            }
            auto action = rfl::json::read<ActionDataType>(actionData.data());
            if (action)
            {
                ReflectFields->RevelationActionDatas = action.value();
                ActionDatasToActions();
            }
        }
        return result;
    }

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
    REFLECT_PROPERTY(
        MaxRevelations, 
        RevelationsPerRound)

    GETTER(int, MaxRevelations) { return ReflectFields->MaxRevelations; }
    SETTER(int, MaxRevelations) 
    { 
        ReflectFields->MaxRevelations = std::max(value, 1); 
        _playerElementList.resize(ReflectFields->MaxRevelations);
    }
    // 최대 계시 수용량
    PROPERTY(MaxRevelations)

    GETTER(int, RevelationsPerRound) { return ReflectFields->RevelationsPerRound; }
    SETTER(int, RevelationsPerRound) { ReflectFields->RevelationsPerRound = value; }
    // 라운드당 뽑는 계시 개수
    PROPERTY(RevelationsPerRound)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    ActionDataType  RevelationActionDatas;
    ElementDataType RevelationElementDatas;
    int             MaxRevelations = 10;    //최대 계시 수용량
    int             RevelationsPerRound = 3;// 라운드당 계시
    ElementDataType PlayerElementDatas;
    REFLECT_FIELDS_END(RevelationSystem)

    void ActionsToActionDatas();
    void ActionDatasToActions();

    void ElementsToElementDatas();
    void ElementDatasToElements();

    void PlayerElementDatasToPlayerElements();
    void PlayerElementsToPlayerElementDatas();

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
    std::unordered_map<std::string, RevelationElement>                      _elementsTable;         //계시 테이블
    std::unordered_map<std::string, std::function<RevelationActionBase*()>> _actionConstructors;    //Action 이름으과 한쌍인 Action 생성자
    ImVec2                                                                  _tableEditorCenterPos{};

private:
    void ResetActions();

private:
    std::vector<std::unique_ptr<RevelationElement>> _playerElementList; // 플레이어가 사용중인 계시

private:
    void ImGuiDrawPlayerElementEditor();

};
