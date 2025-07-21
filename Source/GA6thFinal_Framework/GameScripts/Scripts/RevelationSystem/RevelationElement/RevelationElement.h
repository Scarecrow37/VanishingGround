#pragma once
class TurnAction;

// 연격 조건
enum class RevelationConditionType
{
    // 연격 ≥ A : A 이상
    GREATER_THAN_OR_EQUAL,

    // 연격 ≤ A : A 이하
    LESS_THAN_OR_EQUAL,

    // 연격 A~B : A와 B 사이 (양쪽 포함)
    BETWEEN_INCLUSIVE,

    // 연격 = A : A와 정확히 같음
    EQUAL,

    // A의 배수
    MULTIPLE_OF
};


enum class RevelationKeyword
{
    //조건 참일시 발동
    DEFAULT,
    //라운드당 한번만 발동 가능
    ROUND_ONCE
};

enum class RevelationGrade
{
    //일반
    COMMON,
    //희귀
    RARE,
    //전설
    LEGENDARY,
    //소멸
    EXTINCTION
};

 class CharacterBase;

/*
* 계시의 정보를 가지고있는 class 입니다.
*/
class RevelationElement : public ReflectSerializer
{
    USING_PROPERTY(RevelationElement)
public:
    RevelationElement() = default;
    ~RevelationElement() override = default; 
   
public:
    REFLECT_PROPERTY(
        Name,
        ReflectFields->Condition, 
        ReflectFields->ConditionValueA,
        ReflectFields->ConditionValueB,
        ReflectFields->Keyword, 
        ReflectFields->Grade)
    
    void SetName(std::string_view name) { ReflectFields->Name = name; }
    GETTER_ONLY(const std::string&, Name) { return ReflectFields->Name; }
    //계시 이름
    PROPERTY(Name)

    /*액션 존재 여부를 반환합니다.*/
    bool IsAction() const { return _action != nullptr; }

    /*해당 계시의 액션을 반환합니다. IsAction()을 확인해야합니다.*/
    TurnAction& GetAction() { return *_action; }

    /*계시 발동 조건 여부를 검사합니다.*/
    bool Evaluate(); 

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string             Name            = STR_NULL;
    RevelationConditionType Condition       = RevelationConditionType::GREATER_THAN_OR_EQUAL; // 조건
    int                     ConditionValueA = 5;                                              // 조건값 A
    int                     ConditionValueB = 5;                                              // 조건값 B
    RevelationKeyword       Keyword         = RevelationKeyword::DEFAULT;                     // 키워드
    RevelationGrade         Grade           = RevelationGrade::COMMON;                        // 등급
    std::string             ActionName      = STR_NULL; 
    REFLECT_FIELDS_END(RevelationElement)

    std::unique_ptr<TurnAction> _action;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

public:
    /// <summary>
    /// ImGui 에디터를 위한 Table 인덱스를 설정합니다.
    /// </summary>
    void SetImGuiTableIndex()
    {
        if (nullptr != ImGui::GetCurrentTable())
        {
            ImGui::TableSetColumnIndex(_imguiDrawIndex++);
        }
    }

    ImU32 GetGradeColor() 
    { 
        RevelationGrade garde = ReflectFields->Grade;
        switch (garde)
        {
        case RevelationGrade::COMMON:
            return ImColor{255, 255, 255, 255};
        case RevelationGrade::RARE:
            return ImColor{45, 205, 255, 255};
        case RevelationGrade::LEGENDARY:
            return ImColor{245, 200, 0, 255};
        case RevelationGrade::EXTINCTION:
            return ImColor{210, 0, 170, 255};
        default:
            return ImColor{100, 100, 100, 255};
        }
    }

private:
    int  _imguiDrawIndex = 0;
    bool _showActionEditor = false;

private:
    using DatasType = reflect_fields_struct;
    void DeepCopyAction(const TurnAction& action);
    RevelationElement& CopyElement(const RevelationElement& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        DatasType& myDatas = *ReflectFields;
        const DatasType& rhsDatas = *rhs.ReflectFields;
        myDatas = rhsDatas;
        if (rhs._action)
        {
            DeepCopyAction(*rhs._action);
        }
        return *this;
    }

public:
    RevelationElement(const RevelationElement& rhs) { CopyElement(rhs); }
    RevelationElement& operator=(const RevelationElement& rhs)
    {
        return CopyElement(rhs);
    }

};