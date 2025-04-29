#include "ReflectHelper.h"
#include "pch.h"

using namespace ReflectHelper::ImGuiDraw;

void ReflectHelper::ImGuiDraw::Private::EngineLog(int logLevel, std::string_view message, std::source_location location)
{
    Global::engineCore->Logger.Log(logLevel, message, location);
}

std::string ReflectHelper::json::yyjsonValToString(yyjson_val* val)
{
    if (val == nullptr)
        return "";

    yyjson_mut_doc* doc     = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* mut_val = yyjson_val_mut_copy(doc, val);
    yyjson_mut_doc_set_root(doc, mut_val);

    size_t len;
    char*  json = yyjson_mut_write(doc, NULL, &len);

    std::string result;
    if (json != nullptr)
    {
        result = std::string(json, len);
    }
    free(json);
    yyjson_mut_doc_free(doc);
    return result;
}
