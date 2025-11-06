#pragma once

/* 유물 보상 개수 */ 
constexpr size_t ARTIFACT_DROP_COUNT = 6; 

/* 유물 보상에 등장 가능한 아이템 타입 */
enum class ArtifactDropType
{
    SWORD,            // 검
    DAGGER,           // 단검
    WARHAMMER,        // 대형 망치
    ACCESSORY,        // 장신구
    REVELATION,       // 계시
    ERASE_REVELATION, // 계시 지우기
    Consumable,       // 소모품
};

/*아이템 정보 구조체*/
struct DropItemInfo
{
    ArtifactDropType Category;     // 분류 타입
    int              ID;           // 아이템 아이디
    std::string      Name;         // 아이템 이름

    /// <summary>
    /// 엑셀 데이터 베이스의 이름을 반환합니다.
    /// </summary>
    /// <param name="type :">아이템 타입</param>
    /// <returns>없으면 빈 문자열을 반환합니다.</returns>
    static std::u8string_view GetDataBaseName(ArtifactDropType type);

    /// <summary>
    /// ArtifactDropType으로 에셋 아이디를 반환합니다.
    /// </summary>
    /// <param name="type :">변환할 타입</param>
    /// <returns>실패시 0</returns>
    static int GetArtifactCategoryAssetID(ArtifactDropType type, bool isMapScene);

    /// <summary>
    /// 해당 유물의 에셋 ID를 반환합니다.
    /// </summary>
    /// <param name="itemInfo :">아이템 정보</param>
    /// <returns></returns>
    static int GetArtifactIconID(DropItemInfo itemInfo);

    /// <summary>
    /// 해당 아이템의 툴팁 ID들을 반환합니다.
    /// </summary>
    /// <param name="itemInfo">: 가져올 아이템</param>
    /// <returns></returns>
    static std::vector<int> GetArtifactTooltipIDs(DropItemInfo itemInfo);

    /// <summary>
    /// 해당 아이템의 툴팁 Description을 반환합니다.
    /// </summary>
    /// <param name="itemInfo">: 가져올 아이템</param>
    /// <returns></returns>
    static std::vector<std::string> GetArtifactTooltips(DropItemInfo itemInfo);

    /// <summary>
    /// 해당 유물의 Description을 엑셀 DB에서 반환합니다.
    /// </summary>
    /// <param name="itemInfo :">아이템 정보</param>
    /// <returns></returns>
    static std::string GetArtifactDescription(DropItemInfo itemInfo);
};

/*보상 시스템에 등장 가능한 유물들은 다음 인터페이스를 상속받아야 합니다.*/
class IDropItem
{
public:
    IDropItem() = default;
    virtual ~IDropItem() = default;

    //아이템의 정보들을 반환합니다.
    virtual DropItemInfo GetItemInfo() const = 0;
};

