#pragma once

/* 유물 보상 개수 */ 
constexpr size_t ARTIFACT_DROP_COUNT = 6; 

struct DropItemInfo
{
    int         ID;         //아이템 아이디
    int         CategoryID; //분류 아이디
    std::string Name;       //아이템 이름
};

/*보상 시스템에 등장 가능한 유물들은 다음 인터페이스를 상속받아야 합니다.*/
class IDropItem
{
public:
    IDropItem() = default;
    virtual ~IDropItem() = default;

    //아이템의 정보들을 반환합니다.
    virtual DropItemInfo GetItemInfo() = 0;
};