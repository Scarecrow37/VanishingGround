#pragma once

class EditorMenu;
class EditorMenuNode;
class EditorMenuLeaf;

/*
Menu를 출력하기 위한 컨테이너
MenuNode = 경로를 이어주기 위한 객체 (EditorMenu)
MenuLeaf = 실제 동작을 위한 객체 (EditorMenu를 상속받은 사용자 정의 객체)
*/
class EditorMenuBar : public EditorBase
{
    using Path = File::FString;
    using Name = std::string;
public:
    EditorMenuBar();
    virtual ~EditorMenuBar();
public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override;
public:
    template <typename T>
    T* RegisterMenu()
    {
        static_assert(std::is_base_of_v<EditorMenu, T>, "T is not a EditorMenu.");
        const char* typeName = typeid(T).name();
        T* instance = new T;
        std::string fullPath = instance->GetMenuPath().string() + "/" + instance->GetLabel();
        auto itr = _pathTable.find(fullPath);
        if (itr == _pathTable.end())
        {
            // ex) Project/Setting = O (경로에 Leaf를 포함하지 말것)
            EditorMenuNode* parent = BuildMenuNode(instance->GetMenuPath());
            parent->_MenuNodeVec.push_back(instance);
            Sort(parent);
            _nameTable[typeName] = instance;
        }
        else
        {
            delete instance;
        }
        return GetMenu<T>();
    }
    template <typename T>
    T* GetMenu()
    {
        static_assert(std::is_base_of_v<EditorMenu, T>, "T is not a EditorMenu.");
        auto itr = _nameTable.find(typeid(T).name());
        if (itr == _nameTable.end())
            return nullptr;
        return dynamic_cast<T*>(itr->second);
    }
    EditorMenuNode* GetMenuFromPath(Path path);
private:
    std::unordered_map<Path, EditorMenuNode*> _pathTable;   // 전체 경로에 대한 매핑 테이블
    std::unordered_map<Name, EditorMenu*> _nameTable;       // 클래스 이름에 대한 매핑 테이블 (Leaf만 존재)
    EditorMenuNode* _root;
private:
    EditorMenuNode* BuildMenuNode(Path path);
    void Sort(EditorMenuNode* root);
};


/*
메인 메뉴 바에 메뉴를 추가할 수 있는 객체
*/
class EditorMenu : public EditorBase
{
public:
    EditorMenu() = default;
    virtual ~EditorMenu() = default;
public:
    virtual void    OnTickGui() override {};
    virtual void    OnStartGui() override {};
    virtual void    OnDrawGui() override {};
    virtual void    OnEndGui() override {};
public:
    /* 메뉴 경로를 설정 */
    inline void         SetMenuPath(const File::FString& path) { _menuPath = path; }
    inline const auto&  GetMenuPath() { return _menuPath; }
    /* 클릭 활성화 여부를 설정 */
    inline void         SetActive(bool v) { _isActive = v; }
    inline bool         GetActive() { return _isActive; }
private:
    File::FString       _menuPath = "";    /* 메뉴 경로 (ex. "Project/Setting/" + "#Label" */
    bool                _isActive = true;  /* 클릭을 비활성화 함 (회색 표시) */
};

/*
EditorMenuNode:
MenuNode 혹은 MenuLeaf를 컨테이너로 들고 메뉴를 계층적으로 나타내게 해주는 객체.
ImGui::BeginMenu로 열고 닫음.
*/
class EditorMenuNode : public EditorMenu
{
    friend class EditorMenuBar;
public:
    virtual void OnDrawGui() override final;
public:
    virtual void OnTickGui() override {};
    virtual void OnPreMenu() {};
    virtual void OnMenu() {};
    virtual void OnPostMenu() {};
public:
    void Sort();
private:
    std::vector<EditorMenu*> _MenuNodeVec;
};

/*
EditorMenuLeaf:
메뉴의 끝을 나타내며, 해당 메뉴를 클릭할 시 동작을 정의할 수 있는 객체.
ImGui::MenuItem을 열고 닫음.
*/
class EditorMenuLeaf : public EditorMenu
{
public:
    virtual void OnDrawGui() override final;
public:
    virtual void OnTickGui() override {};
    virtual void OnSelected() {};
    virtual void OnMenu() {};
public:
    /* 메뉴 아이템의 숏컷(키보드 단축키) 설정 */
    inline void         SetShortcut(std::string_view path) { _shortcut = path; }
    inline const auto&  GetShortcut() const { return _shortcut; }
    /* 토글 가능 여부를 설정 */
    inline void         SetToggleValue(bool* pValue) { _toggleValue = pValue; }
    inline const bool*  GetToggleValue() const { return _toggleValue; }
private:
    std::string _shortcut = "-";
    bool*       _toggleValue = nullptr;
};