#pragma once
#include "UmFramework.h"
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
    const std::vector<AccessoryElement*> GetAccessoryTableElements() { return _elementTableOrderID; }

public:
    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::vector<std::pair<std::string, std::string>> ElementTableData;
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

private:
    void ElementTableSerialized();
    void ElementTableDeserialized();

private:
    SingletonComponent<AccessorySystem> _singletonComponent{this};

    std::map<std::string, AccessoryElement> _elementTable;        // 이름-액세서리 테이블
    std::vector<AccessoryElement*>          _elementTableOrderID; // 아이디 순 정렬된 테이블

private:
    bool RenameAccessory(AccessoryElement& accessory, const std::string& newName);
    bool InsertAccessory(AccessoryElement& accessory);
    bool EraseAccessory(AccessoryElement& accessory);
    void SortTableIDOrder();

    std::string SaveAccessoryTable();
    bool        LoadAccessoryTable(const std::string& data);

    void ImGuiTableEditor();
    void ImGuiDrawExcelParser();

    bool ExcelAccessoryElement(AccessoryElement& element, const std::string& key, const std::string& data);

private:
    struct EditorOnly
    {
        bool ShowTableEditor = false;

        std::string           DeleteTableBuffer = STR_NULL;
        bool                  OpenDeletePopup   = false;
        AccessoryElement*     SelectAccessory   = nullptr;
        std::function<void()> RenameFunc;

        ImGuiColumnSheetParser        ColumnParser{"1191B534-B4B7-425C-8638-EFE3B662DB9C", u8"ID"};
        std::queue<AccessoryElement*> DirtyAccessoryQueue;
        bool                          ShowDirtyAccessoryPopup = false;
    };
#ifdef _UMEDITOR
    EditorOnly _editorOnly;
#endif

};
