#include "pchScripts.h"
#include "MonsterCommon.h"

#include "Monster/Context/MonsterStatContext.h"

namespace Monster
{
    // 문자열 좌우 공백 제거 함수
    namespace
    {
        inline std::string Trim(std::string_view str)
        {
            size_t start = str.find_first_not_of(" \t\n\r");
            size_t end   = str.find_last_not_of(" \t\n\r");
            if (start == std::string::npos)
                return "";
            std::string result(str);
            return result.substr(start, end - start + 1);
        }
    }

    int StringToInt(std::string_view str)
    {
        try
        {
            return std::stoi(Trim(str));
        }
        catch (...)
        {
            assert(false && "문자열을 정수로 변환하는데 실패했습니다.");
            return 0;
        }
        return 0;
    }

    bool StringToInt(std::string_view str, int& outValue) 
    {
        try
        {
            outValue = std::stoi(Trim(str));
            return true;
        }
        catch (...)
        {
            assert(false && "문자열을 정수로 변환하는데 실패했습니다.");
            return false;
        }
    }

    std::vector<int> ParseParam(std::string_view paramStr)
    {
        std::vector<int>         result;
        std::stringstream        ss(paramStr.data());
        std::string              token;

        while (std::getline(ss, token, ',')) // ','로 구분
        {
            token = Trim(token);
            if (token.empty())
            {
                continue;
            }
            try
            {
                int value = std::stoi(token);
                result.push_back(value);
            }
            catch (const std::invalid_argument&)
            {
                assert(false && "액션 파라미터는 숫자여야합니다.");
                continue;
            }
            catch (const std::out_of_range&)
            {
                assert(false && "액션 파라미터 값이 너무 큽니다.");
                continue;
            }
        }

        return result;
    }

    std::vector<TokenParam> ParseTokenParam(std::string_view paramStr)
    {
        std::vector<TokenParam> result;
        std::stringstream       ss(paramStr.data());
        std::string             pair;

        while (std::getline(ss, pair, ',')) // ','로 구분
        {
            pair = Trim(pair);
            if (pair.empty())
            {
                continue;
            }

            size_t colonPos = pair.find(':');
            if (colonPos == std::string::npos)
            {
                assert(false && "토큰 파라미터의 올바른 형식이 아닙니다.");
                continue; // ':' 없는 경우 무시
            }

            std::string idStr    = Trim(pair.substr(0, colonPos));
            std::string countStr = Trim(pair.substr(colonPos + 1));

            try
            {
                int id    = std::stoi(idStr);
                int count = std::stoi(countStr);
                result.push_back({id, count});
            }
            catch (const std::invalid_argument&)
            {
                assert(false && "토큰 파라미터는 숫자여야합니다.");
            }
            catch (const std::out_of_range&)
            {
                assert(false && "토큰 파라미터 값이 너무 큽니다.");
            }
        }

        return result;
    }
    SpawnID GetSpawnID(size_t mainLevelIndex, size_t subLevelIndex, size_t battleCount)
    {
        std::string spawnIDStr = std::string(SPAWN_ID_HEADER);
        spawnIDStr += std::to_string(mainLevelIndex);
        spawnIDStr += std::to_string(subLevelIndex);
        spawnIDStr += std::to_string(battleCount);
        return StringToInt(spawnIDStr);
    }
} // namespace Monster
