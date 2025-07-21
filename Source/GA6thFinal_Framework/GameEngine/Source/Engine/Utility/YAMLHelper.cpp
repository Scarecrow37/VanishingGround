#include "pch.h"

std::pair<YAML::Node, YAMLHelper::LoadResult> YAMLHelper::SafeLoad(std::string_view data)
{
    YAML::Node node;
    YAMLHelper::LoadResult result;
    try
    {
        node = YAML::Load(data.data());
    }
    catch (const YAML::ParserException& e)
    {
        result = YAMLHelper::LoadResult(e.what(), false);
    }
    catch (const YAML::Exception& e)
    {
        result = YAMLHelper::LoadResult(e.what(), false);
    }
    return std::make_pair(node, result);
}

std::pair<YAML::Node, YAMLHelper::LoadResult> YAMLHelper::SafeLoadFile(const std::filesystem::path& path)
{
    YAML::Node node;
    YAMLHelper::LoadResult result;
    try
    {
        node = YAML::LoadFile(path.generic_string());
    }
    catch (const YAML::ParserException& e)
    {
        result = YAMLHelper::LoadResult(e.what(), false);
    }
    catch (const YAML::Exception& e)
    {
        result = YAMLHelper::LoadResult(e.what(), false);
    }
    return std::make_pair(node, result);
}
