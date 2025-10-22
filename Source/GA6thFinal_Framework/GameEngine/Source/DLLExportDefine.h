#pragma once
#include <Windows.h>

#ifdef UMREALSCRIPTS_EXPORT
#define UMREALSCRIPTS_DECLSPEC __declspec(dllexport)
// dllexport는 함수 정의로 대체.
#define UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME)                                                                          \
    {                                                                                                                  \
        return new CLASS_NAME();                                                                                       \
    }
#else
#define UMREALSCRIPTS_DECLSPEC __declspec(dllimport)
// dllimport는 함수 선언으로 대체.
#define UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME) ;
#endif

#define SERIALIZED_REFLECT_FUNC(CLASS_NAME)                                                                            \
    std::string CLASS_NAME::SerializedReflectFields()                                                                  \
    {                                                                                                                  \
        serialized_reflect_event_recursive();                                                                          \
        return ReflectHelper::json::SerializedObjet(*ReflectFields);                                                   \
    }                                                                                                                  

#define DESERIALIZED_REFLECT_FUNC(CLASS_NAME)                                                                          \
    bool CLASS_NAME::DeserializedReflectFields(std::string_view data)                                           \
    {                                                                                                                  \
        bool result = ReflectHelper::json::DeserializedObjet(*ReflectFields, data);                                    \
        deserialized_reflect_event_recursive();                                                                        \
        return result;                                                                                                 \
    }


#define APPLY_REFLECT_FUNC(CLASS_NAME)                                                                                 \
    void CLASS_NAME::applyReflectFields(const std::function<void(std::string_view, void*)>& func)                      \
    {                                                                                                                  \
        const auto view = rfl::to_view(*ReflectFields.Get());                                                          \
        view.apply([&](auto& rflField) { func(rflField.name(), rflField.value()); });                                  \
    }

#define IMGUI_DRAW_REFLECT_FIELDS(CLASS_NAME)                                                                                                                          \
void CLASS_NAME::imgui_draw_reflect_fields_input_auto(std::unordered_set<void*>& reflectionFieldsSet,  const ReflectHelper::ImGuiDraw::InputAutoSetting& setting)      \
{                                                                                                                                                                      \
    const auto view = rfl::to_view(*ReflectFields.Get());                                                                                                              \
    view.apply([&](auto& rflField)                                                                                                                                     \
    {                                                                                                                                                                  \
        if (reflectionFieldsSet.find(rflField.value()) != reflectionFieldsSet.end())                                                                                   \
        {                                                                                                                                                              \
            ReflectHelper::ImGuiDraw::Private::InputAuto(rflField, setting);                                                                                           \
        }                                                                                                                                                              \
    });                                                                                                                                                                \
}
      
#define REFLECT_FUNCTION(CLASS_NAME)                                                                                   \
    SERIALIZED_REFLECT_FUNC(CLASS_NAME)                                                                                \
    DESERIALIZED_REFLECT_FUNC(CLASS_NAME)                                                                              \
    APPLY_REFLECT_FUNC(CLASS_NAME)                                                                                     \
    IMGUI_DRAW_REFLECT_FIELDS(CLASS_NAME)

#define UMREAL_COMPONENT_EXPORT(CLASS_NAME)                                                                            \
    extern "C" UMREALSCRIPTS_DECLSPEC Component* NewTest##CLASS_NAME() UMREALSCRIPT_NEWCOMPONENT(CLASS_NAME)

// 컴포넌트 생성 함수 선언용 매크로
#define UMREAL_COMPONENT(CLASS_NAME)                                                                                   \
    UMREAL_COMPONENT_EXPORT(CLASS_NAME)                                                                                \
    REFLECT_FUNCTION(CLASS_NAME)

                                                                                             

