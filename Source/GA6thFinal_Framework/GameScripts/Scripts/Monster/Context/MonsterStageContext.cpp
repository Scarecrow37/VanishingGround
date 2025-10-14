#include "pchScripts.h"
#include "MonsterStageContext.h"

namespace Monster
{
    namespace
    {
        // 문자열 좌우 공백 제거 함수
        inline std::string Trim(const std::string& s)
        {
            size_t start = s.find_first_not_of(" \t\n\r");
            size_t end   = s.find_last_not_of(" \t\n\r");
            if (start == std::string::npos)
                return "";
            return s.substr(start, end - start + 1);
        }
    }

    std::vector<DamageParam> ParseDamageParam(const std::string& paramString)
    {
        std::vector<DamageParam> result;
        std::stringstream        ss(paramString);
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
                result.push_back({value});
            }
            catch (const std::invalid_argument&)
            {
                assert(false && "스킬 데미지 파라미터는 숫자여야합니다.");
                continue;
            }
            catch (const std::out_of_range&)
            {
                assert(false && "스킬 데미지 파라미터 값이 너무 큽니다.");
                continue;
            }
        }

        return result;
    }

    std::vector<TokenParam> ParseTokenParam(const std::string& paramString)
    {
        std::vector<TokenParam> result;
        std::stringstream       ss(paramString);
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
                assert(false && "스킬 토큰 파라미터의 올바른 형식이 아닙니다.");
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
                assert(false && "스킬 토큰 파라미터는 숫자여야합니다.");
            }
            catch (const std::out_of_range&)
            {
                assert(false && "스킬 토큰 파라미터 값이 너무 큽니다.");
            }
        }

        return result;
    }

} // namespace Monster
