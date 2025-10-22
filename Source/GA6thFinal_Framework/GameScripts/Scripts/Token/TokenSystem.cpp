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
    }
    RegisterAllTokenInstance();
    SortByOrder();
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
    }
}

void TokenSystem::OnDestroy() 
{
    Clear();
}

void TokenSystem::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNodeEx("Token Instances##Token System"))
    {
        ImGui::Text("Total Token Instances: %zu", _tokenInstances.size());
        ImGui::Separator();
        for (const auto& token : _tokenInstances)
        {
            if (token)
            {
                const std::string& name = token->GetTokenName();
                ImGui::Selectable(name.c_str());
                if (ImGui::IsItemHovered())
                {
                    if (ImGui::BeginTooltip())
                    {
                        const std::string& tag = token->GetTokenTag();
                        ImGui::Text("ID: %d", token->GetTokenID());
                        ImGui::Text("Tag: %s", tag.c_str());
                        ImGui::Text("Max Stack: %d", token->GetMaxStackCount());
                        ImGui::EndTooltip();
                    }
                }
            }
        }
        ImGui::TreePop();
    }
}

void TokenSystem::RegisterAllTokenInstance()
{
    for (const auto& [id, constructor] : _tokenIDFactoryTable)
    {
        auto it = _tokenIDTable.find(id);
        if (it == _tokenIDTable.end())
        {
            Token* instance = constructor();
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
        _tokenNameTable[name]   = token;
        _tokenTagTable[tag].insert(token);

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
        _tokenNameTable.erase(name);
        if (_tokenTagTable.contains(tag))
        {
            _tokenTagTable[tag].erase(token);
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
        assert(dataBase); // [assert] 엑셀 데이터 시스템에 해당 시트가 존재해야합니다.
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
                    tokenData.ID = StringHelper::StringToInt(excelData);
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
                    tokenData.Order = StringHelper::StringToInt(excelData);
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::MAX_STACK);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    tokenData.MaxStack = StringHelper::StringToInt(excelData);
                }
                excelData = dataBase->FindData(rowIndex, TokenExcelData::Key::PARAMETER);
                if (excelData != ExcelDataBase::FIND_STR_FAIL)
                {
                    std::vector<int> params = StringHelper::ParseCSVToInt(excelData);
                    for (const auto& param : params)
                    {
                        tokenData.Params.push_back(param);
                    }
                }
                _tokenDataTable[tokenData.ID] = tokenData;
            }
        }
    }
}

void TokenSystem::Clear()
{
    _tokenInstances.clear();
    _tokenIDTable.clear();
    _tokenNameTable.clear();
    _tokenTagTable.clear();
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

IToken* TokenSystem::GetTokenFromID(TokenID tokenID)
{
    if (_tokenIDTable.contains(tokenID))
    {
        return _tokenIDTable[tokenID];
    }
    return nullptr;
}

const std::string& TokenSystem::GetTokenNameFromID(int tokenID)
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

const std::set<Token*>* TokenSystem::GetTokenInstancesFromTag(const std::string& tag)
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