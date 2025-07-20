#pragma once

namespace YAMLHelper
{
    /// <summary>
    /// 로드 정보를 담는 구조체
    /// </summary>
    struct LoadResult
    {
        LoadResult() = default;
        LoadResult(const LoadResult&) = default;
        LoadResult(std::string_view what, bool result) 
        { 
            _what = what; 
            _result = result;
        }
        ~LoadResult() = default;

        const std::string& What() const { return _what; }
        operator bool() const { return _result; }
        LoadResult& operator=(const LoadResult& rhs) = default;
    private:
        bool _result = true;
        std::string _what = STR_NULL;
    };

    /// <summary>
    /// 예외처리를 포함한 Load를 합니다. result 객체를 같이 반환해줍니다.
    /// </summary>
    /// <param name="data :">로드할 yaml 데이터</param>
    /// <returns>node, result</returns>
    std::pair<YAML::Node, YAMLHelper::LoadResult> SafeLoad(std::string_view data);


    /// <summary>
    /// 예외처리를 포함한 Load를 합니다. result 객체를 같이 반환해줍니다.
    /// </summary>
    /// <param name="data :">로드할 yaml 파일 경로</param>
    /// <returns>node, result</returns>
    std::pair<YAML::Node, YAMLHelper::LoadResult> SafeLoadFile(const std::filesystem::path& path);
}