#include "pch.h"
#include "ScriptTestEditor.h"
using namespace u8_literals;

ScriptTestEditor::ScriptTestEditor() 
{

}

ScriptTestEditor::~ScriptTestEditor() 
{

}

void ScriptTestEditor::OnStartGui() 
{
    SetLabel(u8"테스트 에디터"_c_str);
}

void ScriptTestEditor::OnPreFrame() 
{

}

class TestObject : public GameObject
{
public:
    TestObject() = default;
    ~TestObject() = default;
    
    REFLECT_FIELDS_BEGIN(GameObject)
    float fTest = 10;
    std::array<float, 3> array {0.1, 0.2, 0.3};
    std::vector<int> vecInt{1, 2, 3};
    REFLECT_FIELDS_END(TestObject)
};

static std::string yyjson_get_val_raw_str(yyjson_val* val)
{
    if (val == nullptr)
        return "";

    yyjson_mut_doc* doc     = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* mut_val = yyjson_val_mut_copy(doc, val);
    yyjson_mut_doc_set_root(doc, mut_val);

    size_t len;
    char* json = yyjson_mut_write(doc, NULL, &len);

    std::string result;
    if (json != nullptr)
    {
        result = std::string(json, len);
    }
    free(json);
    yyjson_mut_doc_free(doc);
    return result;
}

void ScriptTestEditor::OnFrame() 
{
    using namespace Global;
    using namespace u8_literals;
    {
        static TestObject prefab;
        static TestObject prefabInstance;
        std::string prefabData = prefab.SerializedReflectFields();
        std::string instanceData = prefabInstance.SerializedReflectFields();
        yyjson_doc* prefabDoc = yyjson_read(prefabData.c_str(), prefabData.size(), 0);
        yyjson_doc* instanceDoc = yyjson_read(instanceData.c_str(), instanceData.size(), 0);
        yyjson_val* prefabRoot = yyjson_doc_get_root(prefabDoc);
        yyjson_val* instanceRoot = yyjson_doc_get_root(instanceDoc);

        auto* data = prefabInstance.ReflectFields.Get();
        auto  view = rfl::to_view(*data);
        view.apply([=](const auto& field) 
        {       
            std::string_view name = field.name();
            ImGui::Text(u8"맴버 : %s"_c_str, name.data());

            yyjson_val* prefabVal = yyjson_obj_get(prefabRoot, name.data());
            std::string prefabRaw = yyjson_get_val_raw_str(prefabVal);
            ImGui::Text(u8"원본 : %s"_c_str, prefabRaw.data());

            yyjson_val* instanceVal = yyjson_obj_get(instanceRoot, name.data());
            std::string instanceRaw = yyjson_get_val_raw_str(instanceVal);
            ImGui::Text(u8"객체 : %s"_c_str, instanceRaw.data());

            ImGui::BeginDisabled();
            bool isSame = prefabRaw == instanceRaw;
            ImGui::Checkbox("same", &isSame);
            ImGui::EndDisabled();

            ReflectHelper::ImGuiDraw::Private::InputAuto(field);
            ImGui::Separator();
        });     
        yyjson_doc_free(prefabDoc);
        yyjson_doc_free(instanceDoc);
    }
}
