#pragma once
#include "ItemDropSystem/Interface/IDropItem.h"
#include "TurnSystem/TurnAction/TurnAction.h"

enum class AccessoryGrade
{
    // 일반
    COMMON,

    // 희귀
    RARE,

    // 기이
    BIZARRE,

    // 전설
    LEGENDARY,
};

class AccessoryElement : public ReflectSerializer, public IDropItem
{
    friend class AccessorySystem;
    USING_PROPERTY(AccessoryElement)
public: 
    AccessoryElement();
    ~AccessoryElement() override;

    inline static constexpr ImU32 GetGradeImGuiColor(AccessoryGrade grade)
    {
        switch (grade)
        {
        case AccessoryGrade::COMMON:
            return ImColor{255, 255, 255, 255};
        case AccessoryGrade::RARE:
            return ImColor{45, 205, 255, 255};
        case AccessoryGrade::BIZARRE:
            return ImColor{255, 0, 255, 255};
        case AccessoryGrade::LEGENDARY:
            return ImColor{245, 200, 0, 255};
        default:
            return ImColor{100, 100, 100, 255};
        }
    }

    ImU32 GetGradeColor() const
    {
        AccessoryGrade grade = ReflectFields->Grade;
        return GetGradeImGuiColor(grade);
    }

    REFLECT_PROPERTY(
        AccessoryID, 
        AccessoryName,
        Grade
    )

    const std::vector<std::unique_ptr<TurnAction>>& GetActions() const { return _actions; }

    SETTER(int, AccessoryID) { ReflectFields->ID = value; }
    GETTER(int, AccessoryID) { return ReflectFields->ID; }
    //type : int
    PROPERTY(AccessoryID)

    GETTER_ONLY(const std::string&, AccessoryName) { return ReflectFields->AccessoryName; }
    //type : const std::string&
    PROPERTY(AccessoryName)
    void SetName(std::string_view name) { ReflectFields->AccessoryName = name; }

    SETTER(AccessoryGrade, Grade) { ReflectFields->Grade = value; }
    GETTER(AccessoryGrade, Grade) { return ReflectFields->Grade; }
    //type : AccessoryGrade
    PROPERTY(Grade)

protected:
    using ActionNameDataPair = std::pair<std::string, std::string>;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    int                             ID            = 0;
    std::string                     AccessoryName = STR_NULL;
    AccessoryGrade                  Grade         = AccessoryGrade::COMMON;
    std::vector<ActionNameDataPair> Actions;
    REFLECT_FIELDS_END(AccessoryElement)

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    std::vector<std::unique_ptr<TurnAction>> _actions;
    using DatasType = reflect_fields_struct;
    void              DeepCopyAction(const std::vector<std::unique_ptr<TurnAction>>& actions);
    AccessoryElement& CopyElement(const AccessoryElement& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        DatasType&       myDatas  = *ReflectFields;
        const DatasType& rhsDatas = *rhs.ReflectFields;
        myDatas                   = rhsDatas;
        if (false == rhs._actions.empty())
        {
            DeepCopyAction(rhs._actions);
        }
        return *this;
    }

public:
    AccessoryElement(const AccessoryElement& rhs) { CopyElement(rhs); }
    AccessoryElement& operator=(const AccessoryElement& rhs) { return CopyElement(rhs); }

    bool operator!=(const AccessoryElement& rhs) const { return this->AccessoryID != rhs.AccessoryID; }
    bool operator==(const AccessoryElement& rhs) const { return this->AccessoryID == rhs.AccessoryID; }

public:
    DropItemInfo GetItemInfo() const override;
};