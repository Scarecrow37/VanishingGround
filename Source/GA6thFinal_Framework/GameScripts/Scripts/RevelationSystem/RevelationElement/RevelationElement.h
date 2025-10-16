#pragma once
#include <ItemDropSystem/Interface/IDropItem.h>
#include <TurnSystem/TurnAction/TurnAction.h>

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
 class RevelationElement : public ReflectSerializer, public IDropItem
 {
    friend class RevelationSystem;
    USING_PROPERTY(RevelationElement)
public:
    static constexpr int GetGradeID(RevelationGrade grade)
    {
        switch (grade)
        {
        case RevelationGrade::COMMON:
            return 300000;
        case RevelationGrade::RARE:
            return 300001;
        case RevelationGrade::LEGENDARY:
            return 300003;
        case RevelationGrade::EXTINCTION:
            return 300004;
        default:
            return -1;
        }
    }

    RevelationElement() = default;
    ~RevelationElement() override = default; 
   
public:
    REFLECT_PROPERTY(
        ElementName, 
        Grade, 
        RevelationID)
    
    void SetName(std::string_view elementName) { ReflectFields->ElementName = elementName; }
    GETTER_ONLY(const std::string&, ElementName) { return ReflectFields->ElementName; }
    //계시 이름
    PROPERTY(ElementName)

    SETTER(RevelationGrade, Grade) { ReflectFields->Grade = value; }
    GETTER(RevelationGrade, Grade) { return ReflectFields->Grade; }
    PROPERTY(Grade)

    GETTER(int, RevelationID) { return ReflectFields->ID; }
    SETTER(int, RevelationID) { ReflectFields->ID = std::clamp(value, 0, std::numeric_limits<int>::max()); }
    PROPERTY(RevelationID)

    /*액션 존재 여부를 반환합니다.*/
    bool IsAction() const { return _action != nullptr; }

    /*해당 계시의 액션을 반환합니다. IsAction()을 확인해야합니다.*/
    TurnAction& GetAction() { return *_action; }

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    int             ID          = 0;
    std::string     ElementName = STR_NULL;
    RevelationGrade Grade       = RevelationGrade::COMMON;
    std::string     ActionName  = STR_NULL; 
    REFLECT_FIELDS_END(RevelationElement)

    std::unique_ptr<TurnAction> _action;

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

public:
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

public:
    // IDropItem을(를) 통해 상속됨
    DropItemInfo GetItemInfo() const override;
 };