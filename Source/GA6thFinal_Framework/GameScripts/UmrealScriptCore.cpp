#include "UmFramework.h"
#include "UmrealScriptCore.h"

using namespace Global;

UMREALSCRIPTS_DECLSPEC void InitalizeUmrealScript(const std::shared_ptr<EngineCores> engineCores, ImGuiContext* ImguiContext)
{
    Global::engineCore = engineCores;  //코어 동기화
    ImGui::SetCurrentContext(ImguiContext);  //Imguicontext 동기화
}

UMREALSCRIPTS_DECLSPEC void CreateUmrealcSriptFile(const char* fileName)
{
    using namespace std::string_literals;
    constexpr const wchar_t* scriptPorjectPath   = L"..\\GameScripts";
    constexpr const wchar_t* UmrealScriptsHeader = L"..\\GameScripts\\UmrealScripts.h";
    if (!std::filesystem::exists(scriptPorjectPath) || 
        !std::filesystem::exists(UmrealScriptsHeader)
        )
    {
        MessageBox(NULL, L"스크립트 프로젝트가 존재하지 않습니다.", L"스크립트 생성 오류", NULL);
        return;
    }

    std::filesystem::path include = L"Scripts";
    include /= fileName;
    std::filesystem::path filePath = scriptPorjectPath / include;
    std::wstring ClassName = include.filename();
    std::string  typeIdName = "class " + include.filename().string();
    std::wstring ScriptsHeaderData = std::format(L"#include \"{}.h\"", include.c_str());
    std::wstring UmrealScriptsHeaderData = std::format(L"UMREAL_COMPONENT({})", ClassName.c_str());
    filePath.replace_extension(L".h");
    if (std::filesystem::exists(filePath))
    {
        MessageBox(NULL, L"이미 존재하는 스크립트 파일입니다.", L"스크립트 생성 오류", NULL);
        return;
    }
    else if (engineCore->ComponentFactory.HasComponent(typeIdName))
    {
        MessageBox(NULL, L"이미 존재하는 컴포넌트 이름입니다.",
                   L"스크립트 생성 오류", NULL);
    }
    else
    {
        std::wofstream wofs(UmrealScriptsHeader, std::ios::app);
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
            wofs <<             LR"(REFLECT_PROPERTY())"                                            << L"\n";
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

        auto message = std::format(L"스크립트 파일 생성 완료되었습니다.\n 솔루션을 실행하겠습니까? \n {}", filePath.c_str());
        int result = MessageBox(NULL, message.c_str(), L"스크립트 파일 생성 완료", MB_YESNO | MB_ICONINFORMATION);

        if (result == IDYES)
        {
            constexpr const wchar_t* ScriptSlnFilePath = L"..\\UmrealScripts.sln";
            HINSTANCE result = ShellExecuteW(NULL, L"open", ScriptSlnFilePath, NULL, NULL, SW_SHOWNORMAL);
        }
    }   
}

