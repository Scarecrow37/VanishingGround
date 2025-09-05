#pragma once

struct DropItemInfo
{
    int ID;
    std::string_view Name;
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