#include "pchScripts.h"
#include "TokenSystem.h"
#include "ExcelDataSystem/ExcelDataSystem.h"
#include "Utility/StringHelper.h"

UMREAL_COMPONENT(TokenSystem)

TokenSystem::TokenSystem() = default;

TokenSystem::~TokenSystem() = default;

void TokenSystem::Reset()
{
    Base::Reset();
    _singletonComponent.SetSingleTon();

    if (false == UmCore->IsPlay())
    {
        // 엑셀 데이터를 로드
        if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            _tokenDataTable.clear();
            LoadTokenDataFromExcelData(dataSystem);
        }
        RegisterAllTokenInstance();
        SortByOrder();
    }
}

void TokenSystem::Awake() 
{
    Base::Awake();
    if (_singletonComponent.TrySingleTon() &&
        _singletonObject.TrySingleTon(true))
    {
        // 엑셀 데이터를 로드
        if (ExcelDataSystem* dataSystem = SingletonComponent<ExcelDataSystem>::GetInstance())
        {
            _tokenDataTable.clear();
            LoadTokenDataFromExcelData(dataSystem);
        }
        RegisterAllTokenInstance();
        SortByOrder();
    }
}

void TokenSystem::OnDestroy() 
{
    Clear();
}

void TokenSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNodeEx("Token Data##token system"))
    {
        ImGui::Text("Total Token Data Count: %zu", _tokenDataTable.size());
        ImGui::Separator();
        ImGui::BeginChild("##token instances list", ImVec2(0, 300), ImGuiChildFlags_Border);
        for (const auto& [id, data] : _tokenDataTable)
        {
            ImGuiHelper::StyleBuilder style;
            if (_tokenIDTable.contains(id))
            {
                ImVec4 color = ImColor(100, 255, 100);
                style.PushStyleColor(ImGuiCol_Text, color);
            }
            else
            {
                ImVec4 color = ImColor(255, 100, 100);
                style.PushStyleColor(ImGuiCol_Text, color);
            }
            const std::string label = std::format("{} : {}", data.ID, data.Name);
            ImGui::Selectable(label.c_str());
            if (ImGui::IsItemHovered())
            {
                if (ImGui::BeginTooltip())
                {
                    ImGui::Text("ID: %d", data.ID);
                    ImGui::Text("Tag: %s", data.Tag.c_str());
                    ImGui::Text("Max Stack: %d", data.MaxStack);
                    ImGui::EndTooltip();
                }
            }
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Token Factory##token system"))
    {
        ImGui::Text("Total Registered Token Count: %zu", _registeredFactoryTable.size());
        ImGui::Separator();
        ImGui::BeginChild("##token registered list", ImVec2(0, 300), ImGuiChildFlags_Border);
        for (const auto& [id, _] : _registeredFactoryTable)
        {
            ImGuiHelper::StyleBuilder style;
            if (_tokenIDTable.contains(id))
            {
                ImVec4 color = ImColor(100, 255, 100);
                style.PushStyleColor(ImGuiCol_Text, color);
            }
            else
            {
                ImVec4 color = ImColor(255, 100, 100);
                style.PushStyleColor(ImGuiCol_Text, color);
            }
            ImGui::Text("ID: %d", id);
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }
}

void TokenSystem::RegisterAllTokenInstance()
{
    for (const auto& [id, constructor] : _registeredFactoryTable)
    {
        if (_tokenDataTable.contains(id))
        {
            Token* instance = constructor();
            instance->_tokenData = _tokenDataTable[id];
            RegisterTokenInstanceToTable(instance);
        }
    }
}

void TokenSystem::RegisterTokenInstanceToTable(Token* token)
{
    if (token)
    {
        const int          id   = token->GetTokenID();
        const std::string& name = token->GetTokenName();
        const std::string& tag  = token->GetTokenTag();

        _tokenIDTable[id]       = token;
        _tokenTagTable[tag].insert(id);

        _tokenInstances.push_back(std::unique_ptr<Token>(token));
    }
}

void TokenSystem::UnregisterTokenInstanceToTable(Token* token)
{
    if (token)
    {
        const int           id   = token->GetTokenID();
        const std::string&  name = token->GetTokenName();
        const std::string&  tag  = token->GetTokenTag();

        _tokenIDTable.erase(id);
        if (_tokenTagTable.contains(tag))
        {
            _tokenTagTable[tag].erase(id);
            if (_tokenTagTable[tag].empty())
            {
                _tokenTagTable.erase(tag); // 태그가 비어있으면 제거
            }
        }
        std::erase_if(_tokenInstances, [id](const std::unique_ptr<Token>& ptr) {
            return ptr && ptr->GetTokenID() == id; 
            });
    }
}

void TokenSystem::LoadTokenDataFromExcelData(ExcelDataSystem* dataSystem) 
{
    if (dataSystem)
    {
        std::unique_ptr<ExcelDataBase> dataBase = dataSystem->FindExcelDataBase(u8"토큰");
        assert(dataBase && "엑셀 데이터 시스템에 해당 시트가 존재해야합니다.");
        if (dataBase)
        {
            const size_t rowCount = dataBase->RowCount();
            for (size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
            {
                TokenData        tokenData;
                std::string_view excelData;
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::ID);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    StringHelper::StringToInt(excelData, tokenData.ID);
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::NAME);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    tokenData.Name = excelData;
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::TAG);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    tokenData.Tag = excelData;
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::ORDER);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    StringHelper::StringToInt(excelData, tokenData.Order);
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::MAX_STACK);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    StringHelper::StringToInt(excelData, tokenData.MaxStack);
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::PARAMETER);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    if (false == excelData.empty())
                    {
                        std::vector<int> params = StringHelper::ParseCSVToInt(excelData);
                        for (const auto& param : params)
                        {
                            tokenData.Params.push_back(param);
                        }
                    }
                }
                _tokenDataTable[tokenData.ID] = tokenData;
            }
        }
    }
    bool isValid = _tokenDataTable.size() == _registeredFactoryTable.size();
    assert(isValid && "토큰 엑셀 데이터 시트와 토큰 인스턴스 개수가 다릅니다.");
}

void TokenSystem::Clear()
{
    _tokenInstances.clear();
    _tokenIDTable.clear();
    _tokenTagTable.clear();
}

IToken* TokenSystem::GetTokenFromID(TokenID tokenID)
{
    if (_tokenIDTable.contains(tokenID))
    {
        return _tokenIDTable[tokenID];
    }
    return nullptr;
}

const std::string& TokenSystem::GetTokenNameFromID(TokenID tokenID)
{
    if (_tokenIDTable.contains(tokenID))
    {
        return _tokenIDTable[tokenID]->GetTokenName();
    }
    static const std::string emptyString = "";
    return emptyString;
}

const TokenData* TokenSystem::GetTokenDataFromID(TokenID tokenID)
{
    if (_tokenDataTable.contains(tokenID))
    {
        return &_tokenDataTable[tokenID];
    }
    return nullptr;
}

const std::set<TokenID>* TokenSystem::GetTokenIDSetFromTag(const std::string& tag)
{
    if (_tokenTagTable.contains(tag))
    {
        return &_tokenTagTable[tag];
    }
    return nullptr;
}

void TokenSystem::SortByOrder()
{   // 토큰을 Order에 따라 오름차순 정렬합니다.
    std::sort(_tokenInstances.begin(), _tokenInstances.end(),
        [](std::unique_ptr<Token>& a, std::unique_ptr<Token>& b) {
            int aOrder = a->GetTokenOrder();
            int bOrder = b->GetTokenOrder();
            return aOrder < bOrder;
        });
}

const char* TokenSystem::TokenIDToName(TokenID tokenID)
{
    if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
    {
        return system->GetTokenNameFromID(tokenID).c_str();
    }
    return STR_NULL;
}

const char* TokenSystem::TokenIDToTag(TokenID tokenID)
{
    if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
    {
        if (const TokenData* data = system->GetTokenDataFromID(tokenID))
        {
            return data->Tag.c_str();
        }
    }
    return STR_NULL;
}

int TokenSystem::TokenIDToOrder(TokenID tokenID)
{
    if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
    {
        if (const TokenData* data = system->GetTokenDataFromID(tokenID))
        {
            return data->Order;
        }
    }
    return 0;
}

int TokenSystem::TokenIDToMaxStack(TokenID tokenID)
{
    if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
    {
        if (const TokenData* data = system->GetTokenDataFromID(tokenID))
        {
            return data->MaxStack;
        }
    }
    return 0;
}
