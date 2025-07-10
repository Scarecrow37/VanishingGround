#pragma once
#include "../RevelationAction/Base/RevelationActionBase.h"

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
        ImagePath, 
        Name, 
        ReflectFields->Condition, 
        ReflectFields->ConditionValueA,
        ReflectFields->ConditionValueB,
        ReflectFields->Keyword, 
        ReflectFields->Grade)
    
    GETTER_ONLY(std::string, ImagePath)
    {   
        File::Guid guid = ReflectFields->ImageGuid;
        File::Path path = guid.ToPath();
        if (true == path.IsNull())
        {
            return STR_NULL;
        }
        else
        {
            return path.string();
        }        
    }
    //사용할 이미지 리소스 경로입니다.
    PROPERTY(ImagePath)

    void SetName(std::string_view name) { ReflectFields->Name = name; }
    GETTER_ONLY(std::string_view, Name) { return ReflectFields->Name; }
    //계시 이름
    PROPERTY(Name)

    GETTER_ONLY(std::string_view, Action) { return ReflectFields->Action; }
    //바인딩된 액션
    PROPERTY(Action)

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string             ImageGuid       = "";
    std::string             Name            = STR_NULL;
    RevelationConditionType Condition       = RevelationConditionType::GREATER_THAN_OR_EQUAL; // 조건
    int                     ConditionValueA = 5;                                              // 조건값 A
    int                     ConditionValueB = 5;                                              // 조건값 B
    RevelationKeyword       Keyword         = RevelationKeyword::DEFAULT;                     // 키워드
    RevelationGrade         Grade           = RevelationGrade::COMMON;                        // 등급
    std::string             Action          = STR_NULL; // 해당되는 액션에 대한 key
    REFLECT_FIELDS_END(RevelationElement)

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

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

    ImVec4 GetGradeColor() 
    { 
        RevelationGrade garde = ReflectFields->Grade;
        switch (garde)
        {
        case RevelationGrade::COMMON:
            return {255, 255, 255, 1};
        case RevelationGrade::RARE:
            return {45, 205, 255, 1};
        case RevelationGrade::LEGENDARY:
            return {245, 200, 0, 1};
        case RevelationGrade::EXTINCTION:
            return {210, 0, 170, 1};
        default:
            return {100, 100, 100, 1};
        }
    }

private:
    int _imguiDrawIndex = 0;

private:
    using DatasType = reflect_fields_struct;
    RevelationElement& CopyElement(const RevelationElement& rhs)
    {
        if (this == &rhs)
        {
            return *this;
        }
        DatasType& myDatas = *ReflectFields;
        const DatasType& rhsDatas = *rhs.ReflectFields;
        myDatas = rhsDatas;
        return *this;
    }

public:
    RevelationElement(const RevelationElement& rhs) { CopyElement(rhs); }
    RevelationElement& operator=(const RevelationElement& rhs)
    {
        return CopyElement(rhs);
    }

};