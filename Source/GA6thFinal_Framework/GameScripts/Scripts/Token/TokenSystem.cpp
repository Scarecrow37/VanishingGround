#include "pchScripts.h"
#include "TokenSystem.h"

TokenSystem::TokenSystem() {}

TokenSystem::~TokenSystem()
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
}

void TokenSystem::Reset()
{
    _staticInstance = this;
    InitTokenInstance();
    SortByOrder();
}

void TokenSystem::OnDestroy() 
{
    for (auto& token : _tokenInstances)
    {
        if (token)
        {
            delete token; // 토큰 인스턴스 삭제
        }
    }
    _tokenInstances.clear();
    _tokenIDTable.clear();
    _tokenNameTable.clear();
}

void TokenSystem::OnDrawDebug()
{
    if (true == _isOpenEditor)
    {
        ImGui::PushID("TokenSystem");
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("TokenDataTable", &_isOpenEditor, ImGuiWindowFlags_MenuBar);
        {
            ImGuiDrawDataTable();
            ImGuiDrawMenuBar();
        }
        ImGui::End();
        ImGui::PopID();
    }
}

void TokenSystem::SerializedReflectEvent() 
{
    // 토큰 인스턴스의 데이터를 직렬화합니다.
    ReflectFields->TokenSerializeData.clear();
    for (const auto& [id, token] : _tokenIDTable)
    {
        if (token)
        {
            ReflectFields->TokenSerializeData[id] = token->SerializedReflectFields();
        }
    }
}

void TokenSystem::DeserializedReflectEvent() 
{
    // 토큰 인스턴스의 데이터를 역직렬화합니다.
    for (const auto& [id, data] : ReflectFields->TokenSerializeData)
    {
        Token* token = GetTokenFromID(id);
        if (token)
        {
            token->DeserializedReflectFields(data);
        }
    }
    SortByOrder();
}

void TokenSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::Button("Open Editor##TokenSystem"))
    {
        _isOpenEditor = !_isOpenEditor;
    }
}

void TokenSystem::InitTokenInstance()
{
    for (const auto& [id, constructor] : _tokenIDFactoryTable)
    {
        auto it = _tokenIDTable.find(id);
        if (it == _tokenIDTable.end())
        {
            Token* newToken = constructor();
            if (newToken)
            {
                newToken->SetDirtyOrderCallback([](int id) { SortByOrder(); });
                _tokenInstances.push_back(newToken);
                _tokenIDTable[id]                         = newToken;
                _tokenNameTable[newToken->GetTokenName()] = newToken;
            }
        }
    }
}

void TokenSystem::ImGuiDrawDataTable()
{
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    ImVec2 left          = ImVec2(130.0f, availableSize.y);
    ImVec2 right         = ImVec2(availableSize.x - left.x, availableSize.y);

     // Left Window
    ImGui::BeginChild("Left", left, ImGuiChildFlags_Border);
    if (ImGui::CollapsingHeader("Token List##token", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (const auto& token : _tokenInstances)
        {
            if (token)
            {
                bool isSelected = (_selectedToken == token);
                if (ImGui::Selectable(token->GetTokenName(), isSelected))
                {
                    _selectedToken = token; // 선택된 토큰을 저장
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right Window
    ImGui::BeginChild("Right", right, ImGuiChildFlags_Border);
    if (_selectedToken)
    {
        if (ImGui::CollapsingHeader("Edit Properties##token", ImGuiTreeNodeFlags_DefaultOpen))
        {
            _selectedToken->ImGuiDrawPropertys();
        }
        ImGui::Separator();
        if (ImGui::CollapsingHeader("Show Property Member##token"))
        {
            _selectedToken->ShowReflectFieldView();
        }
    }
    ImGui::EndChild();
}

void TokenSystem::ImGuiDrawMenuBar() 
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open Editor##TokenSystem"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Token System"))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

bool TokenSystem::CreateTokenInstanceFromID(int tokenID, Token** ppToken)
{
    auto it = _tokenIDFactoryTable.find(tokenID);
    if (it != _tokenIDFactoryTable.end())
    {
        (*ppToken) = it->second();
        return true;
    }
    return false;
}

bool TokenSystem::CreateTokenInstanceFromName(std::string_view tokenName, Token** ppToken)
{
    auto it = _tokenNameFactoryTable.find(tokenName.data());
    if (it != _tokenNameFactoryTable.end())
    {
        (*ppToken) = it->second();
        return true;
    }
    return false;
}

int TokenSystem::GetTokenIDFromName(std::string_view tokenName)
{
    auto it = _tokenNameToIDTable.find(tokenName.data());
    if (it != _tokenNameToIDTable.end())
    {
        return it->second;
    }
    return 0;
}

Token* TokenSystem::GetTokenFromID(int tokenID)
{
    auto it = _tokenIDTable.find(tokenID);
    if (it != _tokenIDTable.end())
    {
        return it->second;
    }
    return nullptr;
}

Token* TokenSystem::GetTokenFromName(std::string_view name)
{
    auto it = _tokenNameTable.find(name.data());
    if (it != _tokenNameTable.end())
    {
        return it->second;
    }
    return nullptr;
}

void TokenSystem::SortByOrder()
{ // 토큰을 Order에 따라 오름차순 정렬합니다.
    std::sort(_tokenInstances.begin(), _tokenInstances.end(),
              [](Token* a, Token* b) { 
            int aOrder = a->GetTokenOrder();
            int bOrder = b->GetTokenOrder();
            return aOrder < bOrder;
        });
}

const std::string& TokenSystem::GetTokenNameFromID(int tokenID)
{
    auto it = _tokenIDToNameTable.find(tokenID);
    if (it != _tokenIDToNameTable.end())
    {
        return it->second;
    }
    static const std::string emptyString;
    return emptyString; // 토큰이 존재하지 않으면 빈 문자열 반환
}
