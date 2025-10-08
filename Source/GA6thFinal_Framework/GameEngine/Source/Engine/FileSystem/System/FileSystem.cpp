#include "pch.h"
#include "FileSystem.h"

using namespace File;
using namespace u8_literals;


EFileSystem::EFileSystem() 
{
    _originPath = fs::current_path().generic_wstring();
    _extToSubscriberTable[STR_NULL] = EventSubscriberSet{};
}


bool EFileSystem::LoadGameDirectory()
{
    std::filesystem::path path = fs::current_path().generic_wstring();
    _projectName        = path.stem().string();
    _originPath         = fs::current_path().generic_wstring();
    _rootPath           = fs::absolute(path).generic_wstring();
    _assetPath          = fs::absolute(_rootPath / ASSET_FOLDER_NAME).generic_wstring();
    _projectSettingPath = fs::absolute(_rootPath / PROJECT_SETTING_PATH).generic_wstring();
    _buildSettingPath   = fs::absolute(_rootPath / BUILD_SETTING_PATH).generic_wstring(); 

    for (auto& subscriber : _subscriberSet)
        subscriber->OnRequestedLoad();

    ReadDirectory();

    for (auto& subscriber : _subscriberSet)
        subscriber->OnPostRequestedLoad();

    return true;
}

bool EFileSystem::CreateProject(const File::Path& path)
{
    bool isExists = fs::exists(path);
    bool isValid  = path.extension() == PROJECT_EXTENSION;

    if (true == isExists)
    {
        OutputLog(L"Failed to EFileSystem::CreateProject. Already exists to project.");
        return false;
    }

    if (false == isValid)
    {
        OutputLog(L"Failed to EFileSystem::CreateProject. File is unvaild extension.");
        return false;
    }

    if (false == _projectData.Create(path, false))
    {
        OutputLog(L"Failed to EFileSystem::CreateProject. Failed to create project file.");
        return false;
    }

    File::Path rootPath    = path.parent_path();
    File::Path assetPath   = rootPath / ASSET_FOLDER_NAME;
    File::Path settingPath = rootPath / PROJECT_SETTING_PATH;
    File::CreateFolder(assetPath.generic_wstring());
    File::CreateFolder(settingPath.generic_wstring());

    return true;
}

bool EFileSystem::LoadProject(const File::Path& path)
{
    bool isExists = fs::exists(path);
    bool isValid  = path.extension() == PROJECT_EXTENSION;

    std::pair<bool, std::wstring> result = {true, L""};
    if (true == result.first && false == isExists)
    {
        result.second = L"존재하지 않는 경로입니다.";
        result.first  = false;
    }
    if (true == result.first && false == isValid)
    {
        result.second = L"프로젝트 확장자가 올바르지 않습니다.";
        result.first  = false;
    }

    if (true == result.first && false == _projectData.Load(path))
    {
        result.second = L"프로젝트 파일 로드에 실패하였습니다.";
        result.first  = false;
    }

    if (false == result.first)
    {
        const std::wstring& msg  = result.second;
        const std::wstring title = L"Project load failed";
        int result = MessageBox(
            GetFocus(),                 // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
            msg.c_str(),                // 메시지 텍스트
            title.c_str(),              // 메시지 박스 제목
            MB_OK | MB_ICONWARNING      // 스타일: 예/아니오 버튼 + 질문 아이콘
        );
        File::OutputLog(msg);
        return false;
    }

    File::Path directory = path.parent_path();

    _projectName = path.stem().string();
    _rootPath    = fs::absolute(directory).generic_wstring();
    _assetPath   = fs::absolute(_rootPath / ASSET_FOLDER_NAME).generic_wstring();
    _projectSettingPath = fs::absolute(_rootPath / PROJECT_SETTING_PATH).generic_wstring();
    _buildSettingPath   = fs::absolute(_rootPath / BUILD_SETTING_PATH).generic_wstring(); 

    File::CreateFolder(_assetPath);
    File::CreateFolder(_projectSettingPath);

    if (nullptr != _observer)
    {
        _observer->Stop();
        _observer->SetObservingPath(_rootPath);
        _observer->Start();
    }

    for (auto& subscriber : _subscriberSet)
        subscriber->OnRequestedLoad();

    ReadDirectory();

    for (auto& subscriber : _subscriberSet)
        subscriber->OnPostRequestedLoad();

    return true;
}

bool EFileSystem::SaveProject()
{
    if (true == _rootPath.empty())
    {
        OutputLog(L"Failed to EFileSystem::SaveProject. Root path is empty.");
        return false;
    }
        

    if (false == fs::exists(_rootPath))
    {
        OutputLog(L"Failed to EFileSystem::SaveProject. Root path is Invalid.");
        return false;
    }

    for (auto& subscriber : _subscriberSet)
        subscriber->OnRequestedSave();

    for (auto& subscriber : _subscriberSet)
        subscriber->OnPostRequestedSave();

    return true;
}

bool EFileSystem::SaveAsProject(const File::Path& to)
{
    if (true == _projectData.IsNull())
    {
        OutputLog(L"Failed to EFileSystem::SaveAsProject. Project is not loaded");
        return false;
    }

    if (false == fs::exists(_rootPath))
    {
        OutputLog(L"Failed to EFileSystem::SaveAsProject. Root path is Invalid.");
        return false;
    }

    std::wstring msg    = L"현재 프로젝트를 저장하고 다른 이름으로 저장합니다.";
    std::wstring title  = L"다른 이름으로 저장";

    int result = MessageBox(
        GetFocus(),               // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),              // 메시지 텍스트
        title.c_str(),            // 메시지 박스 제목
        MB_YESNO                  // 스타일: 예/아니오 버튼
    );
    
    if (result == IDYES)
    {
        SaveProject();
        fs::copy(_rootPath, to, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        return true;
    }
    else
    {
        return false;
    }
}

int EFileSystem::LoadProjectWithMessageBox(const File::Path& path)
{
    File::Path projectName = path.filename();

    std::wstring msg    = projectName.wstring() + L" 프로젝트를 로드하시겠습니까?";
    std::wstring title  = L"Load Project";
    HWND         hwnd   = UmApplication.GetHwnd();

    int msgResult = MessageBox(hwnd,    // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),                // 메시지 텍스트
        title.c_str(),              // 메시지 박스 제목
        MB_YESNO                    // 스타일: 예/아니오 버튼
    );

    if (msgResult == IDYES)
    {
        LoadProject(path);
    }
    return msgResult;
}

int EFileSystem::SaveProjectWithMessageBox()
{
    if (true == _rootPath.empty())
        return false;

    std::wstring msg    = L"현재 프로젝트를 저장하시겠습니까?"; 
    std::wstring title  = L"프로젝트 저장";
    HWND         hwnd   = UmApplication.GetHwnd();
    UINT         style  = MB_YESNOCANCEL | MB_DEFBUTTON1; // 기본 버튼을 YES로 설정

    int msgResult = MessageBox(
        hwnd,                       // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),                // 메시지 텍스트
        title.c_str(),              // 메시지 박스 제목
        style                       // 스타일
    );

    if (msgResult == IDYES)
    {
        SaveProject();
    }
    return msgResult;
}

bool EFileSystem::SaveSetting(const File::Path& path)
{
    auto setting = rfl::yaml::save(path.string(), _setting);
    if (false == setting)
    {
        return false;
    }
    else
    {
        return LoadSetting(path);
    }
}

bool EFileSystem::LoadSetting(const File::Path& path) 
{
    auto setting = rfl::yaml::load<File::SystemSetting>(path.string());
    if (false == setting)
    {
        return false;
    }
    else
    {
        _setting = setting.value();
        return true;
    }
}

void EFileSystem::ObserverSetUp(const CallBackFunc& callback)
{
    if (nullptr == _observer)
    {
        _observer = new File::FileEventObserver();
        _observer->SetCallbackFunc(callback);
        _observer->SetObservingPath(_rootPath);
        _observer->Start();
    }
}

void EFileSystem::ObserverShutDown() 
{
    if (nullptr != _observer)
    {
        _observer->Stop();
        delete _observer;
        _observer = nullptr;
    }
}

bool EFileSystem::IsLoadedProject() const
{
    return !_projectData.IsNull();
}

bool EFileSystem::IsValidExtension(const File::FString& ext) const
{
    return (ext == "") || (_extToSubscriberTable.find(ext) != _extToSubscriberTable.end());
}

bool EFileSystem::IsSameContext(std::weak_ptr<File::Context> left, std::weak_ptr<File::Context> right) const
{
    if (false == left.expired() && false == right.expired())
    {
        auto spLeft  = left.lock();
        auto spRight = right.lock();
        if (spLeft == spRight)
        {
            return true;
        }
    }
    return false;
}

File::Path EFileSystem::GetRelativePath(const File::Path& path) const
{
    if (false == _projectData.IsNull())
    {
        File::Path out = fs::absolute(path);
        out = fs::relative(out, _rootPath);
        return out;
    }
    return File::NULL_PATH;
}

const File::Path& EFileSystem::GetPathFromGuid(const File::Guid& guid) const
{
    auto wpContext = GetContext(guid);
    if (false == wpContext.expired())
    {
        auto& path = wpContext.lock()->GetPath();
        return path;
    }
    return NULL_PATH;
}

const File::Path& EFileSystem::GetPathFromAssetID(int assetID) const
{
    auto itr = _assetIDTable.find(assetID);
    if (itr != _assetIDTable.end())
    {
        return itr->second;
    }
    return NULL_PATH;
}

const File::Guid& EFileSystem::GetGuidFromAssetID(int assetID) const
{
    return GetGuidFromPath(GetPathFromAssetID(assetID));
}

const File::Guid& EFileSystem::GetGuidFromPath(const File::Path& path) const
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        const MetaData& meta = wpContext.lock()->GetMeta();
        return meta.GetGuid();
    }
    return NULL_GUID;
}

std::weak_ptr<Context> EFileSystem::GetContext(const File::Guid& guid) const
{
    if (NULL_GUID == guid)
    {
        return std::weak_ptr<Context>();
    }

    auto itr = _guidToPathTable.find(guid);
    if (itr != _guidToPathTable.end())
    {
        return itr->second; 
    }
    else
    {
        return std::weak_ptr<Context>();
    }
}

std::weak_ptr<Context> EFileSystem::GetContext(const File::Path& path) const
{
    auto itr = _pathToGuidTable.find(path);
    if (itr != _pathToGuidTable.end())
    {
        return itr->second; 
    }
    else
    {
        return std::weak_ptr<Context>();
    }
}

std::weak_ptr<File::Context> EFileSystem::GetContext(const fs::path& path) const
{
    auto itr = _pathToGuidTable.find(path);
    if (itr != _pathToGuidTable.end())
    {
        return itr->second;
    }
    else
    {
        return std::weak_ptr<Context>();
    }
}

const EFileSystem::EventSubscriberSet& EFileSystem::GetEventSubscribers(const File::FString& ext)
{
    auto itr = _extToSubscriberTable.find(ext);
    if (itr != _extToSubscriberTable.end())
    {
        return itr->second;
    }
    return _extToSubscriberTable["null"];
}

int EFileSystem::GetAssetIDFromPath(const File::Path& path) const
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        const MetaData& meta = wpContext.lock()->GetMeta();
        return meta.GetAssetID();
    }
    return 0;
}

int EFileSystem::GetAssetIDFromGuid(const File::Guid& guid) const
{
    auto wpContext = GetContext(guid);
    if (false == wpContext.expired())
    {
        const MetaData& meta = wpContext.lock()->GetMeta();
        return meta.GetAssetID();
    }
    return 0;
}

bool EFileSystem::IsExistsAssetID(int assetID) const
{
    return _assetIDTable.find(assetID) != _assetIDTable.end();
}

bool EFileSystem::IsExistsGuid(const File::Guid& guid) const
{
    return _guidToPathTable.find(guid) != _guidToPathTable.end();
}

bool EFileSystem::HasContext(const File::Path& path) const
{
    auto itr = _pathToGuidTable.find(path);
    if (itr != _pathToGuidTable.end())
    {
        return true;
    }
    return false;
}

bool EFileSystem::HasContext(const File::Guid& guid) const
{
    auto itr = _guidToPathTable.find(guid);
    if (itr != _guidToPathTable.end())
    {
        return true;
    }
    return false;
}

void EFileSystem::CheckFileContextIntegrity(const File::Path& path)
{
    if (fs::exists(path))
    {
        if (IsValidExtension(path.extension()) && false == HasContext(path))
        {
            RegisterContext(path);
        }
    }
    else if (HasContext(path))
    {
        UnregisterContext(path);
    }
}

bool EFileSystem::ChangeAssetID(const File::Path& path, int changeID)
{
    auto wpContext = GetContext(path);
    return ChangeAssetID(wpContext, changeID);
}

bool EFileSystem::ChangeAssetID(std::weak_ptr<File::Context> context, int changeID)
{
    auto spContext = context.lock();
    if (spContext)
    {
        File::MetaData& meta = spContext->GetMeta();
        int oldID = meta.GetAssetID();
        // 바꿀 ID가 이미 있거나 현재 ID가 0이 아닌데 존재하지 않는 경우 실패
        if (IsExistsAssetID(changeID) || (0 != oldID && false == IsExistsAssetID(oldID)))
        {
            File::OutputLog(L"Failed to change AssetID. Already exists AssetID : " + std::to_wstring(changeID));
            return false;
        }
        meta.SetAssetID(changeID);
        meta.FileCreate();
        _assetIDTable.erase(oldID);
        if (0 != changeID)
        {
            _assetIDTable[changeID] = spContext->GetPath();
        }
        return true;
    }
    return false;
}

void EFileSystem::RequestInspectFile(const File::Path& path)
{
    const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
    for (auto& subscriber : subscriberSet)
    {
        subscriber->OnRequestedInspect(path);
    }
}

void EFileSystem::RequestOpenFile(const File::Path& path) 
{
    const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
    for (auto& subscriber : subscriberSet)
    {
        subscriber->OnRequestedOpen(path);
    }
}

void EFileSystem::RequestCopyFile(const File::Path& path) 
{
    const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
    for (auto& subscriber : subscriberSet)
    {
        subscriber->OnRequestedCopy(path);
    }
}

void EFileSystem::RequestPasteFile(const File::Path& path) 
{
    const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
    for (auto& subscriber : subscriberSet)
    {
        subscriber->OnRequestedPaste(path);
    }
}

void EFileSystem::RequestDragDropFile(const File::Path& path) 
{
    const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
    for (auto& subscriber : subscriberSet)
    {
        subscriber->OnRequestedDragDrop(path);
    }
}

void EFileSystem::DrawGuiSettingEditor() 
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                HWND    owner = UmApplication.GetHwnd();
                LPCWSTR title = L"폴더를 선택하세요.";

                File::Path directory = _rootPath / PROJECT_SETTING_PATH;
                directory            = directory.generic_wstring();
                if (File::ShowOpenFolderDialog(owner, title, _rootPath.c_str(), directory))
                {
                    File::Path filename  = L"FileSystem.UmSetting";
                    SaveSetting(directory / filename);
                }
            }
            if (ImGui::MenuItem("Load"))
            {
                HWND       owner   = UmApplication.GetHwnd();
                LPCWSTR    title = L"폴더를 선택하세요.";
                File::Path path;
                if (File::ShowOpenFolderDialog(owner, title, _rootPath.c_str(), path))
                {
                    File::Path filename  = L"fileSystem.setting";
                    File::Path directory = PROJECT_SETTING_PATH;
                    SaveSetting(directory / filename);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if (ImGui::CollapsingHeader("Debug##CollapsingHeader"))
    {
        ImGuiHelper::TextWithVerticalSeparator("Debug Level");
        ImGui::SliderInt("##Debug Level", &_setting.DebugLevel, 0, 3);
    }
}


void EFileSystem::RegisterFileEventSubscriber(FileEventSubscriber* subscriber, const std::initializer_list<const char*>& exts)
{
    if (subscriber == nullptr)
        return;

    auto itr = _subscriberSet.find(subscriber);
    if (itr == _subscriberSet.end())
    {
        _subscriberSet.insert(subscriber);
        for (const auto& ext : exts)
        {
            subscriber->_triggerExtTable.insert(ext);
            _extToSubscriberTable[ext].insert(subscriber);
        }
    }
}

void EFileSystem::UnRegisterFileEventSubscriber(FileEventSubscriber* subscriber)
{
    const std::vector<FString> exts = subscriber->GetTriggerExtensions();

    for (const auto& ext : exts)
    {
        auto itr = _extToSubscriberTable.find(ext);
        if (itr != _extToSubscriberTable.end())
        {
            auto& subscriberSet = itr->second;
            subscriberSet.erase(subscriber);
            if (subscriberSet.empty())
            {
                _extToSubscriberTable.erase(itr);
            }
        }
    }
    auto itr = _subscriberSet.find(subscriber);
    if (itr != _subscriberSet.end())
    {
        _subscriberSet.erase(itr);
    }
}

void EFileSystem::Clear()
{        
    ClearContext();
    ClearEventSubscriber();
}

void EFileSystem::ClearContext()
{
    
    for (auto& context : _contextTable)
    {
        if (nullptr != context.get())
        {
            auto& path = context->GetPath();
            File::Path  extension   = path.extension();
            const EventSubscriberSet& subscriberSet = GetEventSubscribers(extension);
            for (auto& subscriber : subscriberSet)
            {
                subscriber->OnFileUnregistered(path);
            }
        }
    }
    _pathToGuidTable.clear();
    _guidToPathTable.clear();
    _contextTable.clear();
}

void EFileSystem::ClearEventSubscriber()
{
    _subscriberSet.clear();
    _extToSubscriberTable.clear();
}

void EFileSystem::ReadDirectory() 
{
    ClearContext();
    ReadDirectory(_rootPath);
}

void EFileSystem::ReadDirectory(const File::Path& path) 
{
    std::stack<File::Path> dirStack;
    dirStack.push(path);

    while (!dirStack.empty())
    {
        File::Path curPath  = dirStack.top();
        dirStack.pop();

        File::Path extesion = curPath.extension();
        File::Path genPath  = curPath.generic_string();
        bool isValidExt     = IsValidExtension(extesion);
        bool isDirectory    = fs::is_directory(genPath);

        if (true == isDirectory || true == isValidExt)
        {
            RegisterContext(genPath);
        }
        if (true == isDirectory)
        {
            for (const auto& entry : fs::directory_iterator(genPath))
            {
                File::Path entryPath        = entry.path();
                File::Path genericEntryPath = entryPath.generic_string();
                dirStack.push(genericEntryPath);
            }
        }
       
    }
}

void EFileSystem::RegisterContext(const File::Path& path) 
{
    // 파일이 없으면 return
    if (false == fs::exists(path))
        return;

     auto find = GetContext(path);
     if (false != find.expired())
     {
         std::shared_ptr<Context> context;

         auto absPath = fs::weakly_canonical(path);
         absPath      = absPath.generic_wstring();
         if (true == fs::is_regular_file(absPath))
         {
             // 확장자가 유효하지 않으면 return
             if (true == IsValidExtension(path.extension()))
             {
                 context = std::make_shared<FileContext>(absPath);
             }
             else
             {
                 return;
             }
         }
         else if (true == fs::is_directory(absPath))
         {
             context = std::make_shared<FolderContext>(absPath);
         }
         else
         {
             return;
         }

         // 부모 폴더에서 자신을 추가한다.
         File::Path parentPath = absPath.parent_path().generic_string();
         if (parentPath == "")  // 비어있으면 현재 디렉터리임
             parentPath = ".";

         auto parentContext = UmFileSystem.GetContext<FolderContext>(parentPath);
         if (false == parentContext.expired())
         {
             auto spParentContext = parentContext.lock();
             File::Path filename  = absPath.filename();
             spParentContext->_contextTable[filename] = context;
         }

         auto& meta     = context->GetMeta();
         auto& guid     = meta.GetGuid();
         auto  assetID  = meta.GetAssetID();

         _pathToGuidTable[absPath] = context;
         _guidToPathTable[guid] = context;
         _contextTable.insert(context);
         if (assetID != 0)
         {
             _assetIDTable[assetID] = absPath; // Guid는 경로를 가지고 있어야 한다.
         }

         context->OnFileRegistered(absPath);

         File::Path extension = absPath.extension();
         const EventSubscriberSet& subscriberSet = GetEventSubscribers(extension);
         for (auto& subscriber : subscriberSet)
         {
             subscriber->OnFileRegistered(absPath);
         }
     }
}

void EFileSystem::UnregisterContext(const File::Path& path) 
{
    // 이미 삭제된 파일이므로 존재 검사를 하면 안된다...
   
    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);

    if (false == wpContext.expired())
    {
        auto spContext  = wpContext.lock();
        auto& meta      = spContext->GetMeta();
        auto& guid      = meta.GetGuid();
        auto  assetID   = meta.GetAssetID();

        File::Path  extension   = path.extension();
        const EventSubscriberSet& subscriberSet = GetEventSubscribers(extension);
        for (auto& subscriber : subscriberSet)
        {
            subscriber->OnFileUnregistered(path);
        }

        _contextTable.erase(spContext);
        _assetIDTable.erase(assetID);
        _pathToGuidTable.erase(path);
        _guidToPathTable.erase(guid);

    }
}

void EFileSystem::ProcessRemovedFile(const File::Path& path)
{
    if (path.extension() == File::META_EXTENSION)
    {
        // 메타 파일을 메모리에 존재하는 guid로 재생성
        File::Path filePath = path;
        filePath.replace_extension("");
        // 원본 파일이 존재할 때 만
        if (true == fs::exists(filePath))
        {
            auto wpContext = GetContext(filePath);
            if (false == wpContext.expired())
            {
                auto  spContext = wpContext.lock();
                auto& meta      = spContext->GetMeta();
                meta.FileCreate();
            }
        }
        return;
    }

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto  spContext = wpContext.lock();
        auto& meta      = spContext->GetMeta();
        auto& guid      = meta.GetGuid();

        const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
        for (auto& subscriber : subscriberSet)
        {
            subscriber->OnFileRemoved(path);
        }
        UmFileSystem.UnregisterContext(path);

        spContext->OnFileRemoved(path);

        // 부모 폴더에서 자신을 제거한다.
        File::Path parentPath = path.parent_path().generic_string();
        auto wpFolderContext = EFileSystem::GetContext<FolderContext>(parentPath);
        if (false == wpFolderContext.expired())
        {
            wpFolderContext.lock()->_contextTable.erase(path.filename());
        }
    }
}

void EFileSystem::ProcessModifiedFile(const File::Path& path)
{
    // 파일이 없으면 return
    if (false == fs::exists(path))
        return;

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto spContext = wpContext.lock();

        spContext->OnFileModified(path);

        const EventSubscriberSet& subscriberSet = GetEventSubscribers(path.extension());
        for (auto& subscriber : subscriberSet)
        {
            subscriber->OnFileModified(path);
        } 
    }
}

void EFileSystem::ProcessMovedFile(const File::Path& oldPath, const File::Path& newPath) 
{
    // 파일이 없으면 return
    if (false == fs::exists(newPath))
        return;

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(newPath.extension()))
        return;

    // 이전 경로에서 컨텍스트를 찾아 새 경로로 옮기기
    auto wpContext = GetContext(oldPath);
    if (false == wpContext.expired())
    {
        auto spContext = wpContext.lock();

        File::Path oldFileName = oldPath.filename();
        File::Path oldFolderPath = oldPath.parent_path().generic_string();
        File::Path oldExtension  = oldPath.extension();
        File::Path newFileName = newPath.filename();
        File::Path newFolderPath = newPath.parent_path().generic_string();
        File::Path newExtension  = newPath.extension();

        _pathToGuidTable.erase(oldPath);
        _pathToGuidTable[newPath] = spContext;
        // Guid는 동일하므로 안지워도 된다.

        if (oldFolderPath == newFolderPath)
        {   // 같은 폴더 내에서 이름만 변경
            spContext->OnFileRenamed(oldPath, newPath);
        }
        else
        {   // 다른 폴더로 이동
            spContext->OnFileMoved(oldPath, newPath);
        }

        auto oldFolderContext =
            UmFileSystem.GetContext<FolderContext>(oldFolderPath);
        auto newFolderContext =
            UmFileSystem.GetContext<FolderContext>(newFolderPath);

        if (false == oldFolderContext.expired() && false == newFolderContext.expired())
        {
            FString oldFileName = oldPath.filename();
            FString newFileName = newPath.filename();
            oldFolderContext.lock()->_contextTable.erase(oldFileName);
            newFolderContext.lock()->_contextTable[newFileName] = spContext;
        }

        if (oldFolderPath == newFolderPath)
        {
            const EventSubscriberSet& subscriberSet = GetEventSubscribers(newExtension);
            for (auto& subscriber : subscriberSet)
            {
                subscriber->OnFileRenamed(oldPath, newPath);
            } 
        }
        else
        {
            const EventSubscriberSet& subscriberSet = GetEventSubscribers(newExtension);
            for (auto& subscriber : subscriberSet)
            {
                subscriber->OnFileMoved(oldPath, newPath);
            } 
        }
    }
}