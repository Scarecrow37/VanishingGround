#include "ReflectHelper.h"
#include "pch.h"

using namespace ReflectHelper::ImGuiDraw;

void ReflectHelper::ImGuiDraw::Private::EngineLog(int logLevel, std::string_view message, std::source_location location)
{
    Global::engineCore->Logger.Log(logLevel, message, location);
}

char* ReflectHelper::json::yyjsonValToCStr(yyjson_val* val)
{
    if (val == nullptr)
        return nullptr;

    yyjson_mut_doc* doc     = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* mut_val = yyjson_val_mut_copy(doc, val);
    yyjson_mut_doc_set_root(doc, mut_val);

    char* json = yyjson_mut_write(doc, NULL, NULL);
    yyjson_mut_doc_free(doc);
    return json;
}
