#pragma once

class EditorMenu;
class EditorMenuNode;
class EditorMenu;

/*
Menu를 출력하기 위한 컨테이너
MenuNode = 경로를 이어주기 위한 객체 (EditorMenu)
MenuLeaf = 실제 동작을 위한 객체 (EditorMenu를 상속받은 사용자 정의 객체)
*/
class EditorMenuBar : public EditorGui
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
        T menu;
        Path fullPath = menu.GetPath();
        fullPath = fullPath.generic_string();

        auto itr = _menuTable.find(typeName);
        if (itr == _menuTable.end())
        {
            T* instance = new T();
            _menuTable[typeName].reset(instance);
            // ex) Project/Setting = O (경로에 filename를 포함하지 말것)
            EditorMenuNode* parent = BuildMenuNode(fullPath);
            if (nullptr != parent)
            {
                parent->_menuList[instance->GetLabel()].push_back(instance);
                Sort(parent);
            }
        }
        return GetMenu<T>();
    }

    template <typename T>
    T* GetMenu()
    {
        static_assert(std::is_base_of_v<EditorMenu, T>, "T is not a EditorMenu.");
        auto itr = _menuTable.find(typeid(T).name());
        if (itr == _menuTable.end())
            return nullptr;
        return dynamic_cast<T*>(itr->second.get());
    }

    EditorMenuNode* GetMenuFromPath(Path path);
private:
    std::unordered_map<Path, std::unique_ptr<EditorMenuNode>> _nodeTable;
    std::unordered_map<Name, std::unique_ptr<EditorMenu>>     _menuTable;
    EditorMenuNode* _root;
private:
    EditorMenuNode* BuildMenuNode(Path path);
    void Sort(EditorMenuNode* root);
};

class EditorMenuBase : public EditorGui
{
public:
    EditorMenuBase();
    virtual ~EditorMenuBase();

public:
    /* 클릭 활성화 여부를 설정 */
    inline void SetActive(bool v) { _isActive = v; }
    inline bool GetActive() { return _isActive; }

    inline void SetPath(std::string_view path) { _path = path; }
    inline const auto& GetPath() { return _path; }

protected:
    void DefaultDebugFrame();

private:
    std::string _path;
    bool _isActive = true;  /* 클릭을 비활성화 함 (회색 표시) */
};

/*
EditorMenuNode:
MenuNode 혹은 MenuLeaf를 컨테이너로 들고 메뉴를 계층적으로 나타내게 해주는 객체.
ImGui::BeginMenu로 열고 닫음.
사용자가 상속하여 사용할 수 없음.
*/
class EditorMenuNode : public EditorMenuBase
{
    friend class EditorMenuBar;

public:
    EditorMenuNode();
    virtual ~EditorMenuNode();

private:
    virtual void OnTickGui() override final;
    virtual void OnStartGui() override final;
    virtual void OnDrawGui() override final;
    virtual void OnEndGui() override final;

private:
    std::unordered_map<std::string, std::vector<EditorMenuBase*>> _menuList;
};

/*
EditorMenu:
메뉴 노드에 추가되는 Leaf 객체.
사용자가 상속하여 사용할 수 있음.
*/
class EditorMenu : public EditorMenuBase
{
public:
    EditorMenu();
    virtual ~EditorMenu();

private:
    virtual void OnDrawGui() override final;

private:
    /* 재정의 가능 */
    virtual void OnTickGui() override {};
    virtual void OnStartGui() override {};
    virtual void OnEndGui() override {};
    virtual void OnMenu() = 0;
};