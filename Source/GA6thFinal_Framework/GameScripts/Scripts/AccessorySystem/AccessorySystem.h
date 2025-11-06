#pragma once

#include "Utility/SingletonHelper.h"
#include "AccessoryElement/AccessoryElement.h"
#include "ExcelParser/ImGuiColumnSheetParser.h"

class AccessorySystem : public Component
{
    USING_PROPERTY(AccessorySystem)
public:
    inline static constexpr int GetGradeID(AccessoryGrade grade)
    {
        switch (grade)
        {
        case AccessoryGrade::COMMON:
            return 300000;
        case AccessoryGrade::RARE:
            return 300001;
        case AccessoryGrade::BIZARRE:
            return 300002;
        case AccessoryGrade::LEGENDARY:
            return 300003;
        default:
            return 0;
        }
    }

public:
    AccessorySystem();
    ~AccessorySystem() override;

    /// <summary>
    /// ID 순으로 정렬된 장신구 테이블을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const std::vector<AccessoryElement*> GetAccessoryTableElements() const { return _elementTableOrderID; }

    /// <summary>
    /// 장신구 테이블에 해당 이름의 장신구가 존재하는지 여부를 확인합니다.
    /// </summary>
    /// <param name="accessoryName :">장신구 이름</param>
    /// <returns>존재 여부</returns>
    bool IsAccessoryInTable(const std::string& accessoryName)
    {
        return _elementTable.find(accessoryName) != _elementTable.end();
    }

    /// <summary>
    /// 장신구 테이블에 해당 이름의 장신구가 존재하는지 여부를 확인합니다.
    /// </summary>
    /// <param name="element :">확인할 장신구</param>
    /// <returns>존재 여부</returns>
    bool IsAccessoryInTable(const AccessoryElement& element) { return IsAccessoryInTable(element.AccessoryName);}

    /// <summary>
    /// 장신구 이름으로 테이블에서 가져옵니다.
    /// </summary>
    /// <param name="name :">생성할 장신구 이름</param>
    /// <returns>실패시 nullptr</returns>
    std::unique_ptr<AccessoryElement> TryMakeAccessoryToName(const std::string& name)
    { 
        std::unique_ptr<AccessoryElement> accessory;
        if (auto findIter = _elementTable.find(name); findIter != _elementTable.end())
        {
            accessory.reset(new AccessoryElement(findIter->second));
        }
        return accessory;
    }

    /// <summary>
    /// 플레이어가 착용중인 아이템을 반환합니다.
    /// </summary>
    /// <returns></returns>
    const MVVM::Model<std::vector<AccessoryElement>>& GetPlayerAccessoryItems() const { return _playerAccessoryItems; }

    /// <summary>
    /// 플레이어에게 장신구를 장착합니다 (중복된 장신구는 장착 불가능합니다.)
    /// </summary>
    /// <param name="accessory :">장착할 장신구</param>
    /// <returns>성공 여부</returns>
    bool EquipAccessory(const AccessoryElement& accessory);

    /// <summary>
    /// 장착된 장신구를 해제합니다. 
    /// </summary>
    /// <param name="accessory :">해제할 장신구</param>
    /// <returns>성공 여부</returns>
    bool UnequipAccessory(const AccessoryElement& accessory);

    /// <summary>
    /// 플레이어가 착용 목록에 존재하는 장신구인지 확인합니다.
    /// </summary>
    /// <param name="id :">장신구 ID</param>
    /// <returns>존재 여부</returns>
    bool HasPlayerAccessory(const int id) { return _playerAccessoryItemSet.find(id) != _playerAccessoryItemSet.end(); }
     
    /// <summary>
    /// 플레이어가 착용 목록에 존재하는 장신구인지 확인합니다.
    /// </summary>
    /// <param name="element :">장신구</param>
    /// <returns>존재 여부</returns>
    bool HasPlayerAccessory(const AccessoryElement& element) { return HasPlayerAccessory(element.AccessoryID); }

    void NotifyUIModel() { _playerAccessoryItems.Notify(); }
    
public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::pair<std::string, std::string>> ElementTableData;
    std::vector<std::string> PlayerAccessoriesNames;
    REFLECT_FIELDS_END(AccessorySystem)

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    void ImGuiDrawPropertysEvent() override;

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

    void Reset() override;
    void Awake() override;
    void OnDestroy() override;

private:
    void ElementTableSerialized();
    void ElementTableDeserialized();
    void PlayerAccessoriesSerialized();
    void PlayerAccessoriesDeserialized();

private:
    SingletonComponent<AccessorySystem> _singletonComponent{this};

    std::map<std::string, AccessoryElement> _elementTable;        // 이름-액세서리 테이블
    std::vector<AccessoryElement*>          _elementTableOrderID; // 아이디 순 정렬된 테이블

private:
    MVVM::Model<std::vector<AccessoryElement>> _playerAccessoryItems; // 플레이어가 장착중인 장신구
    std::unordered_set<int> _playerAccessoryItemSet; // 플레이어가 장착중인 장신구 ID 기록용 set (중복 방지)

private:
    bool RenameAccessory(AccessoryElement& accessory, const std::string& newName);
    bool InsertAccessory(AccessoryElement& accessory);
    bool EraseAccessory(AccessoryElement& accessory);
    void SortTableIDOrder();

    std::string SaveAccessoryTable();
    bool        LoadAccessoryTable(const std::string& data);

    void ImGuiTableEditor();
    void ImGuiDrawExcelParser();
    void ImGuiDrawPlayerAccsessoryItems();

    bool ExcelAccessoryElement(AccessoryElement& element, const std::string& key, const std::string& data);

private:
    struct EditorOnly
    {
        bool ShowTableEditor = false;

        std::string           DeleteTableBuffer = STR_NULL;
        bool                  OpenDeletePopup   = false;
        AccessoryElement*     SelectAccessory   = nullptr;
        std::function<void()> RenameFunc;

        std::queue<AccessoryElement*> DirtyAccessoryQueue;
        bool                          ShowDirtyAccessoryPopup = false;
        std::unordered_map<std::string, std::vector<bool>> ShowActionEditor;
    };
#ifdef _UMEDITOR
    EditorOnly _editorOnly;
#endif

};
