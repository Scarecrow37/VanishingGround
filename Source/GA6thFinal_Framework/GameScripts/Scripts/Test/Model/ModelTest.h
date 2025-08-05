#pragma once
#include "UI/Elements/Text/TextElement.h"
#include "UmFramework.h"

class IntToString : public MVVM::ViewModel<int, std::string>
{
public:
    explicit IntToString(MVVM::Model<int>& model);

protected:
    std::string Convert(const int& value) override;
};

class ModelTest : public Component
{
    USING_PROPERTY(ModelTest)
public:
    ModelTest();
    ~ModelTest() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(ModelTest)

    void Reset() override;

    void ImGuiDrawPropertysEvent() override;

private:
    MVVM::Model<int> _value;
};

class ViewTest : public TextElement
{
    USING_PROPERTY(ViewTest)
public:
    ViewTest()           = default;
    ~ViewTest() override = default;

protected:
    REFLECT_FIELDS_BEGIN(TextElement)
    REFLECT_FIELDS_END(ViewTest)

    void Reset() override;
private:
};