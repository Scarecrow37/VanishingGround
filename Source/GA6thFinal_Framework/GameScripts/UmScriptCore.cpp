#include "UmFramework.h"
#include "UmScriptCore.h"

using namespace Global;

UMREALSCRIPTS_DECLSPEC void InitalizeUmrealScript(const std::shared_ptr<EngineCores> engineCores, ImGuiContext* ImguiContext)
{
    Global::engineCore = engineCores;  //코어 동기화
    ImGui::SetCurrentContext(ImguiContext);  //Imguicontext 동기화
}

static bool IncludeInProject(const std::filesystem::path& filePath);

constexpr const wchar_t* SCRIPT_PROJECT_PATH   = L"..\\GameScripts";
constexpr const wchar_t* UMREAL_SCRIPTS_HEADER = L"..\\GameScripts\\UmScripts.h";

UMREALSCRIPTS_DECLSPEC void CreateUmrealcSriptFile(const char* fileName)
{
    using namespace std::string_literals;
    if (!std::filesystem::exists(SCRIPT_PROJECT_PATH) || 
        !std::filesystem::exists(UMREAL_SCRIPTS_HEADER)
        )
    {
        MessageBox(engineCore->App.GetHwnd(),
                   L"스크립트 프로젝트가 존재하지 않습니다.",
                   L"스크립트 생성 오류", 
                   NULL);
        return;
    }

    std::filesystem::path include = L"Scripts";
    include /= fileName;
    std::filesystem::path filePath = SCRIPT_PROJECT_PATH / include;
    std::wstring ClassName = include.filename();
    std::string  typeIdName = "class " + include.filename().string();
    std::wstring ScriptsHeaderData = std::format(L"#include \"{}.h\"", include.c_str());
    std::wstring UmrealScriptsHeaderData = std::format(L"UMREAL_COMPONENT({})", ClassName.c_str());
    filePath.replace_extension(L".h");
    if (std::filesystem::exists(filePath))
    {
        MessageBox(engineCore->App.GetHwnd(),
                   L"이미 존재하는 스크립트 파일입니다.", 
                   L"스크립트 생성 오류",
                   NULL);
        return;
    }
    else if (engineCore->ComponentFactory.HasComponent(typeIdName))
    {
        MessageBox(engineCore->App.GetHwnd(),
                   L"이미 존재하는 컴포넌트 이름입니다.",
                   L"스크립트 생성 오류", 
                   NULL);
    }
    else
    {
        std::wofstream wofs(UMREAL_SCRIPTS_HEADER, std::ios::app);
        if (wofs.is_open())
        {
            wofs << L"\n";
            wofs << L"\n" << ScriptsHeaderData;
            wofs << L"\n" << UmrealScriptsHeaderData;
        }
        wofs.close();

        std::filesystem::create_directories(filePath.parent_path());

        //h 파일 생성
        wofs.open(filePath, std::ios::trunc);
        if (wofs.is_open())
        {
            wofs <<             LR"(#pragma once)"                                                  << L"\n";
            wofs <<             LR"(#include "UmFramework.h")"                                      << L"\n";
            wofs <<   std::format(L"class {} : public Component", ClassName)                        << L"\n";
            wofs <<             LR"({)"                                                             << L"\n";
            wofs <<   std::format(L"    USING_PROPERTY({})", ClassName)                             << L"\n";
            wofs <<             LR"(public:)"                                                       << L"\n";
            wofs <<             LR"(    REFLECT_PROPERTY())"                                        << L"\n";
            wofs <<             LR"()"                                                              << L"\n";
            wofs <<             LR"(public:)"                                                       << L"\n";
            wofs <<   std::format(L"    {}();", ClassName)                                          << L"\n";
            wofs <<   std::format(L"    virtual ~{}();", ClassName)                                 << L"\n";
            wofs <<             LR"()"                                                              << L"\n";
            wofs <<             LR"(    REFLECT_FIELDS_BEGIN(Component))"                           << L"\n";
            wofs <<   std::format(L"    REFLECT_FIELDS_END({})", ClassName)                         << L"\n";
            wofs <<             LR"(};)" << L"\n";
        }
        wofs.close();

        //cpp 파일 생성
        filePath.replace_extension(L".cpp");
        wofs.open(filePath, std::ios::trunc);
        if (wofs.is_open())
        {
            wofs << std::format(L"#include \"{}.h\"", ClassName)                  << L"\n";
            wofs << std::format(L"{0}::{0}() = default;", ClassName)                 << L"\n";
            wofs << std::format(L"{0}::~{0}() = default;", ClassName)                << L"\n";
        }
        wofs.close();

        if (IncludeInProject(filePath) == false)
        {
            UmLogger.Log(
                LogLevel::LEVEL_WARNING,
                (const char*)u8"프로젝트 XML 파싱 실패. 직접 프로젝트에 추가해주세요.");
        }

        auto message = std::format(L"스크립트 파일 생성 완료되었습니다.\n 솔루션을 실행하겠습니까? \n {}", filePath.c_str());
        int result = MessageBox(NULL, message.c_str(), L"스크립트 파일 생성 완료", MB_YESNO | MB_ICONINFORMATION);

        if (result == IDYES)
        {
            constexpr const wchar_t* ScriptSlnFilePath = L"..\\UmrealScripts.sln";
            HINSTANCE result = ShellExecuteW(NULL, L"open", ScriptSlnFilePath, NULL, NULL, SW_SHOWNORMAL);
        }
    }   
}

bool IncludeInProject(const std::filesystem::path& filePath)
{
    constexpr const char* projectPath = "..\\GameScripts\\GameScripts.vcxproj";
 
    pugi::xml_document    doc;
    if (!doc.load_file(projectPath))
    {
        return false;
    }

     pugi::xml_node projectNode = doc.child("Project");

     pugi::xml_node clIncludeGroup;
     pugi::xml_node clCompileGroup;

    for (pugi::xml_node node : projectNode.children("ItemGroup"))
     {
         if (!clIncludeGroup)
         {
             for (pugi::xml_node child : node.children("ClInclude"))
             {
                 if (child.attribute("Include"))
                 {
                     clIncludeGroup = node;
                     break;
                 }
             }
         }

         if (!clCompileGroup)
         {
             for (pugi::xml_node child : node.children("ClCompile"))
             {
                 if (child.attribute("Include"))
                 {
                     clCompileGroup = node;
                     break;
                 }
             }
         }

         if (clIncludeGroup && clCompileGroup)
             break;
     }

     // 중복 체크 후 추가하는 람다
     auto AddFile = [](pugi::xml_node& group, const char* tag,
                                      const char* file) {
         for (pugi::xml_node node : group.children(tag))
         {
             if (std::string(node.attribute("Include").value()) == file)
                 return false;
         }
         pugi::xml_node newNode              = group.append_child(tag);
         newNode.append_attribute("Include") = file;
         return true;
     };

     std::filesystem::path newHeader{filePath};
     newHeader.replace_extension(".h");
     newHeader = std::filesystem::relative(newHeader, SCRIPT_PROJECT_PATH);

     std::filesystem::path newCpp{filePath};
     newCpp.replace_extension(".cpp");
     newCpp = std::filesystem::relative(newCpp, SCRIPT_PROJECT_PATH);

     if (clIncludeGroup)
     {
         AddFile(clIncludeGroup, "ClInclude", newHeader.string().c_str());
     }
     else
     {
         return false;  
     }

     if (clCompileGroup)
     {
         AddFile(clCompileGroup, "ClCompile",newCpp.string().c_str());
     }
     else
     {
         return false;  
     }

     if (doc.save_file(projectPath) == false)
     {
         return false;  
     }

     return true;
}
