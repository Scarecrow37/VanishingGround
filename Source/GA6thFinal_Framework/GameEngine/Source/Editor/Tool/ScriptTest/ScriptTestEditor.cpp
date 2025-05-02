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

void ScriptTestEditor::OnFrame() 
{
    using namespace Global;
    using namespace u8_literals;
    {

    }
}
