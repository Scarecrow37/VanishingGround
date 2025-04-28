#include "pch.h"
using namespace Global;
using namespace u8_literals;

EComponentFactory::EComponentFactory()
{
    SetDllDirectory(EComponentFactory::Engine::SCRIPTS_DLL_PATH);
}
EComponentFactory::~EComponentFactory() = default;

bool EComponentFactory::InitalizeComponentFactory()
{  
    static std::vector<std::tuple<GameObject*, std::string, int, std::string>> addList; //복구해야할 컴포넌트 항목들
    addList.clear();

    if constexpr (Application::IsEditor())
    {
        DWORD exitCodeOut{};
        if (!dllUtility::RunBatchFile(Engine::BUILD_BATCH_PATH, &exitCodeOut))
        {
            engineCore->Logger.Log(LogLevel::LEVEL_ERROR, "Scripts build Fail!");
            return false;
        }
    }
    else
    {
        if (m_scriptsDll != NULL)
            return false;
    }
    SetForegroundWindow(UmApplication.GetHwnd());
    if (m_scriptsDll != NULL)
    {
        //모든 컴포넌트 자원 회수
        for (auto& [key, wptr] : _componentInstanceVec)
        {
            if (auto component = wptr.lock())
            {
                int index = component->GetIndex();
                addList.emplace_back(component->_gameObect, key, index, component->SerializedReflectFields());
                component->_gameObect->_components[index].reset(); //컴포넌트 파괴
            }
        }
        _componentInstanceVec.clear();

        FreeLibrary(m_scriptsDll);
        m_scriptsDll = NULL;
    }

    _newScriptsFunctionMap.clear();
    m_NewScriptsKeyVec.clear();
    SetDllDirectory(EComponentFactory::Engine::SCRIPTS_DLL_PATH);
    m_scriptsDll = LoadLibraryW(L"GameScripts.dll");
    if (m_scriptsDll == NULL)
    {
        //DLL Load Fail
        __debugbreak();
        return false;
    }

    //기존 DLL, PDB 삭제
    std::filesystem::path prevPath = EComponentFactory::Engine::SCRIPTS_DLL_PATH;
    prevPath /= L"Prev_GameScripts.dll";
    if (std::filesystem::exists(prevPath))
    {
        std::error_code ec;
        std::filesystem::remove(prevPath, ec);
        if (ec) 
        {
            __debugbreak(); //삭제 실패
        }
    }
    prevPath.replace_extension(L".pdb");
    if (std::filesystem::exists(prevPath))
    {
        std::error_code ec;
        std::filesystem::remove(prevPath, ec);
        if (ec)
        {
            __debugbreak(); //삭제 실패
        }
    }

    //스크립트 파일 생성 함수 등록
    std::vector<std::string> funcList = dllUtility::GetDLLFuntionNameList(m_scriptsDll);
    MakeScriptFunc = (MakeUmScriptsFile)GetProcAddress(m_scriptsDll, funcList[0].c_str());
    if (funcList[0] != "CreateUmrealcSriptFile")
    {
        FreeLibrary(m_scriptsDll);
        m_scriptsDll = NULL;
        __debugbreak(); //초기화 함수 에러.
        return false;
    }

    //스크립트 초기화 함수 등록
    if (funcList[1] != "InitalizeUmrealScript")
    {
        FreeLibrary(m_scriptsDll);
        m_scriptsDll = NULL;
        __debugbreak(); //초기화 함수 에러.
        return false;
    }
    auto InitDLLCores = (InitScripts)GetProcAddress(m_scriptsDll, funcList[1].c_str());
    std::shared_ptr<EngineCores> cores = engineCore;
    InitDLLCores(
        cores,
        ImGui::GetCurrentContext());

    //스크립트 생성자들 등록
    AddEngineComponentsToScripts();
    for (size_t i = 0; i < funcList.size(); i++)
    {
        std::string& funcName = funcList[i];
        if (funcName.find("New") != std::string::npos)
        {
            auto NewComponentFunc = (NewScripts)GetProcAddress(m_scriptsDll, funcName.c_str());
            Component* component = NewComponentFunc();
            const char* key = typeid(*component).name();
            _newScriptsFunctionMap[key] = NewComponentFunc;
            m_NewScriptsKeyVec.emplace_back(key);
            delete component;
        }
    }

    //파괴된 컴포넌트 재생성 및 복구
    MissingComponent missingTemp;
    for (auto& [gameObject, key, index, reflectData] : addList)
    {
        bool isMissing = false;
        if (key == typeid(MissingComponent).name())
        {
            //Missing 컴포넌트면 데이터 복구
            missingTemp.DeserializedReflectFields(reflectData);
            key = missingTemp.ReflectFields->typeName;
            isMissing = true;
        }
        std::shared_ptr<Component> newComponent;
        auto findIter = _newScriptsFunctionMap.find(key);
        bool isFind   = findIter != _newScriptsFunctionMap.end();
        if (isFind)
        {
            //컴포넌트 존재하면 다시 생성
            newComponent = NewComponent(key);
        }
        else
        {
            //없어진 컴포넌트면 Missing으로 대체
            std::shared_ptr<MissingComponent> missing = NewMissingComponent();
            missing->ReflectFields->typeName = key;
            missing->ReflectFields->reflectData = reflectData;
            newComponent = std::move(missing);
        }
        ResetComponent(gameObject, newComponent.get());       // 엔진에서 사용하기 위해 초기화
        newComponent->_initFlags.SetAwake();                  // 초기화 플래그 설정
        newComponent->_initFlags.SetStart();                  // 초기화 플래그 설정
        gameObject->_components[index] = newComponent;  
        if (isFind == true)
        {
            if (isMissing == true)
            {
                //Missing 컴포넌트면 데이터 복구
                reflectData = missingTemp.ReflectFields->reflectData;
            }
            if (reflectData.empty() == false)
            {
                newComponent->DeserializedReflectFields(reflectData); // 데이터 복구
            }          
        }     
    }

    //존재 안하는거는 전부 제거
    for (auto& [gameObject, key, index, reflectData] : addList)
    {
        std::erase_if(gameObject->_components, [](auto& sptr)
            {
                return sptr == nullptr;
            });
    }

    return true;
}

void EComponentFactory::UninitalizeComponentFactory()
{
    if (m_scriptsDll != NULL)
    {
        //모든 컴포넌트 자원 회수
        for (auto& [key, wptr] : _componentInstanceVec)
        {
            if (auto component = wptr.lock())
            {
                int index = component->GetIndex();
                component->_gameObect->_components[index].reset(); //컴포넌트 파괴
            }
        }
        _componentInstanceVec.clear();
        FreeLibrary(m_scriptsDll);
        m_scriptsDll = NULL;
    }
}

Component* EComponentFactory::AddComponentToObject(GameObject* ownerObject, std::string_view typeid_name)
{
    if(std::shared_ptr<Component> sptr_component = NewComponent(typeid_name))
    {
        const char* name = typeid_name.data();
        ResetComponent(ownerObject, sptr_component.get());
        ESceneManager::Engine::AddComponentToLifeCycle(sptr_component); //씬에 등록
        return sptr_component.get();
    }
    return nullptr;
}

void EComponentFactory::MakeScriptFile(const char* fileName) const
{
    if (m_scriptsDll != NULL)
    {
        MakeScriptFunc(fileName);
    }
    else
    {
        engineCore->Logger.Log(LogLevel::LEVEL_WARNING, u8"Script DLL을 빌드해주세요!"_c_str);
    }
}

YAML::Node EComponentFactory::SerializeToYaml(Component* component)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"스크립트 빌드를 해주세요."_c_str);
        return YAML::Node();
    }
    return MakeYamlToComponent(component);
}

bool EComponentFactory::DeserializeToYaml(GameObject* ownerObject,
                                          YAML::Node* componentNode)
{
    if (UmComponentFactory.HasScript() == false)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"스크립트 빌드를 해주세요."_c_str);
        return false;
    }
    if (std::shared_ptr<Component> component = MakeComponentToYaml(ownerObject, componentNode))
    {
        ESceneManager::Engine::AddComponentToLifeCycle(component); // 씬에 등록
    }
    else
    {
        return false;
    }
    return true;
}

void EComponentFactory::AddEngineComponentsToScripts() 
{
    for (auto& [key, func] : _engineComponets)
    {
        _newScriptsFunctionMap[key] = func;
        m_NewScriptsKeyVec.emplace_back(key);
    }
}

std::shared_ptr<Component> EComponentFactory::NewComponent(std::string_view typeid_name)
{
    std::shared_ptr<Component> newComponent;
    auto findIter = _newScriptsFunctionMap.find(typeid_name.data());
    if (findIter != _newScriptsFunctionMap.end())
    {
        auto& [name, func] = *findIter;
        newComponent.reset(func());                                //컴포넌트 생성
        _componentInstanceVec.emplace_back(name, newComponent);   //추적용 weak_ptr 생성 
    }
    return newComponent;
}

std::shared_ptr<MissingComponent> EComponentFactory::NewMissingComponent()
{
    std::shared_ptr<MissingComponent> missing = std::make_shared<MissingComponent>();
    _componentInstanceVec.emplace_back(typeid(MissingComponent).name(), missing);
    return missing;
}

void EComponentFactory::ResetComponent(GameObject* ownerObject, Component* component)
{
    //여긴 엔진에서 사용하기 위한 초기화 코드 
    component->_className = (typeid(*component).name() + 5);
    component->_gameObect = ownerObject;

    component->Reset();
    //end
}

YAML::Node EComponentFactory::MakeYamlToComponent(Component* component)
{
    YAML::Node node;
    node["Type"] = typeid(*component).name();
    node["ReflectFields"] = component->SerializedReflectFields();
    return node;
}

std::shared_ptr<Component> EComponentFactory::MakeComponentToYaml(GameObject* ownerObject, YAML::Node* pComponentNode)
{
    YAML::Node& node = *pComponentNode;
    std::string Type = node["Type"].as<std::string>();
    std::shared_ptr<Component> component = NewComponent(Type);
    ResetComponent(ownerObject, component.get());
    std::string ReflectFields = node["ReflectFields"].as<std::string>();
    component->DeserializedReflectFields(ReflectFields);
    return component;
}
