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


    

public:

    REFLECT_PROPERTY(ImagePath)
    
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
            path.string();
        }        
    }
    //사용할 이미지 리소스 경로입니다.
    PROPERTY(ImagePath)




protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    std::string             ImageGuid;
    std::string             Name            = STR_NULL;
    RevelationConditionType Condition       = RevelationConditionType::GREATER_THAN_OR_EQUAL; // 조건
    int                     ConditionValueA = 5;                                              // 조건값 A
    int                     ConditionValueB = 5;                                              // 조건값 B
    RevelationKeyword       Keyword         = RevelationKeyword::DEFAULT;                     // 키워드
    RevelationGrade         Grade           = RevelationGrade::COMMON;                        // 등급
    std::string             Action          = STR_NULL; // 해당되는 액션에 대한 key
    REFLECT_FIELDS_END(RevelationElement)

};