#pragma once
#undef max
#undef min
#include "rfl.hpp"
#include "Engine/Utility/BoxSpacing.h"
#include "rfl/json.hpp"
#include "rfl/yaml.hpp"
#include "Engine/Utility/YAMLHelper.h"

//전방 선언
namespace ReflectHelper
{
    namespace ImGuiDraw
    {
        struct InputAutoSetting;
    }
}

// reflect-cpp 라이브러리 docs https://rfl.getml.com/docs-readme/#the-basics
// reflect-cpp github https://github.com/getml/reflect-cpp
// reflect-helper wiki https://github.com/Scarecrow37/VanishingGround/wiki/%5BEngine%5D-ReflectHelper

// 자동 직렬화 및 REFLECT_PROPERTY를 사용하기 위한 클래스 입니다.
// 기능을 사용할 클래스는 이 클래스를 상속받아야 합니다.
struct ReflectSerializer
{
protected:
    /// <summary>
    /// <para> 직렬화 직전 자동으로 호출되는 이벤트 함수입니다. </para>
    /// <para> 직접 override 해서 사용합니다.                 </para>
    /// </summary>
    virtual void SerializedReflectEvent() {}
  
    /// <summary>
    /// <para> 역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.  </para>
    /// <para> 직접 override 해서 사용합니다.                     </para>
    /// </summary>
    virtual void DeserializedReflectEvent() {}

    /// <summary>
    /// <para>  ImGuiDrawPropertys() 호출 이후 콜되는 이벤트 함수입니다. </para>
    /// </summary>
    virtual void ImGuiDrawPropertysEvent() {}

public:
    virtual void ImGuiDrawPropertys(ReflectHelper::ImGuiDraw::InputAutoSetting& setting, bool isTail) 
    {

    } // REFLECT_PROPERTY() 매크로를 통해 자동으로 override 됩니다.

    virtual std::string SerializedReflectFields()
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        return "{}";
    } // REFLECT_FIELDS_END() 매크로를 통해 자동으로 override 됩니다.
    virtual bool DeserializedReflectFields(std::string_view data)
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        return false;
    } // REFLECT_FIELDS_END() 매크로를 통해 자동으로 override 됩니다.

    ReflectSerializer() = default;
    virtual ~ReflectSerializer() 
    { 
        FreeReflectFields();
    }

    ReflectSerializer(const ReflectSerializer& rhs) = delete;
    ReflectSerializer& operator=(const ReflectSerializer& rhs) = delete;

private:
    void*              _reflectFields = nullptr;
    unsigned long long _fieldsSize    = 0;

private:
    void FreeReflectFields()
    {
        if (_reflectFields != nullptr)
        {
            free(_reflectFields);
            _reflectFields = nullptr;
            _fieldsSize    = 0;
        }
    }

#pragma region 매크로가 생성하는 가상함수들.
protected:
    struct reflect_fields_struct
    {
    };
    virtual void make_reflect_fields(void*& fields, unsigned long long& size)
    {
        assert(!"REFLECT_FIELDS가 정의되지 않았습니다.");
        size_t size_of = sizeof(ReflectSerializer::reflect_fields_struct);
        fields         = malloc(size_of);
        new (fields) ReflectSerializer::reflect_fields_struct();
        size = size_of;
    }
    virtual void  serialized_reflect_event_recursive() {}
    virtual void  deserialized_reflect_event_recursive() {}
    virtual void* get_reflect_fields() final
    {
        if (_reflectFields == nullptr)
        {
            make_reflect_fields(_reflectFields, _fieldsSize);
        }
        return _reflectFields;
    }
#pragma endregion
};

#define REFLECT_FIELDS_BEGIN(BASE)                                                                              \
    using Base = BASE;                                                                                          \
    struct reflect_fields_struct                                                                                \
    {                                                                                                           \
        rfl::Flatten<Base::reflect_fields_struct> Basefields{};

#define REFLECT_FIELDS_END(CLASS)                                                                               \
    };                                                                                                          \
    struct reflection_safe_ptr                                                                                  \
    {                                                                                                           \
        reflection_safe_ptr(CLASS##* owner)                                                                     \
        {                                                                                                       \
            _owner = owner;                                                                                     \
        }                                                                                                       \
        ~reflection_safe_ptr() = default;                                                                       \
        reflect_fields_struct* operator->()                                                                     \
        {                                                                                                       \
            return Get();                                                                                       \
        }                                                                                                       \
        reflect_fields_struct& operator*()                                                                      \
        {                                                                                                       \
            return *Get();                                                                                      \
        }                                                                                                       \
        const reflect_fields_struct* operator->() const                                                         \
        {                                                                                                       \
            return Get();                                                                                       \
        }                                                                                                       \
        const reflect_fields_struct& operator*() const                                                          \
        {                                                                                                       \
            return *Get();                                                                                      \
        }                                                                                                       \
        reflect_fields_struct* Get()                                                                            \
        {                                                                                                       \
            if (_reflection == nullptr)                                                                         \
            {                                                                                                   \
                _reflection = reinterpret_cast<CLASS## ::reflect_fields_struct*>(_owner->get_reflect_fields()); \
            }                                                                                                   \
            return _reflection;                                                                                 \
        }                                                                                                       \
        const reflect_fields_struct* Get() const                                                                \
        {                                                                                                       \
            if (_reflection == nullptr)                                                                         \
            {                                                                                                   \
                _reflection = reinterpret_cast<CLASS## ::reflect_fields_struct*>(_owner->get_reflect_fields()); \
            }                                                                                                   \
            return _reflection;                                                                                 \
        }                                                                                                       \
                                                                                                                \
    private:                                                                                                    \
        mutable CLASS## ::reflect_fields_struct* _reflection = nullptr;                                         \
        CLASS##*                         _owner      = nullptr;                                                 \
    };                                                                                                          \
    reflection_safe_ptr ReflectFields{this};                                                                    \
                                                                                                                \
public:                                                                                                         \
    virtual std::string SerializedReflectFields();                                                              \
    virtual bool DeserializedReflectFields(std::string_view data);                                              \
                                                                                                                \
protected:                                                                                                      \
    virtual void make_reflect_fields(void*& fields, unsigned long long& size)                                   \
    {                                                                                                           \
        size_t size_of = sizeof(CLASS## ::reflect_fields_struct);                                               \
        fields         = malloc(size_of);                                                                       \
        size           = size_of;                                                                               \
        new (fields) CLASS## ::reflect_fields_struct();                                                         \
    }                                                                                                           \
    virtual void serialized_reflect_event_recursive()                                                           \
    {                                                                                                           \
        Base::serialized_reflect_event_recursive();                                                             \
        if constexpr (std::is_same_v<                                                                           \
                          decltype(Base::SerializedReflectEvent),                                               \
                          decltype(CLASS## ::SerializedReflectEvent)> ==                                        \
                      false)                                                                                    \
        {                                                                                                       \
            CLASS##::SerializedReflectEvent();                                                                  \
        }                                                                                                       \
    }                                                                                                           \
    virtual void deserialized_reflect_event_recursive()                                                         \
    {                                                                                                           \
        Base::deserialized_reflect_event_recursive();                                                           \
        if constexpr (std::is_same_v<                                                                           \
                          decltype(Base::DeserializedReflectEvent),                                             \
                          decltype(CLASS##::DeserializedReflectEvent)> ==                                       \
                      false)                                                                                    \
        {                                                                                                       \
            CLASS##::DeserializedReflectEvent();                                                                \
        }                                                                                                       \
    }                                                                                                           \
    virtual void applyReflectFields(const std::function<void(std::string_view, void*)>& func);                  \
                                                                                                                \
private:                                                                                                        \
    void imgui_draw_reflect_fields_input_auto(std::unordered_set<void*>& reflectionFieldsSet, const ReflectHelper::ImGuiDraw::InputAutoSetting& setting);              
    

                       
// 에디터 편집을 허용할 프로퍼티들을 등록합니다. Get, Set 함수가 모두 존재하는
// 프로퍼티만 편집 가능합니다.
#define REFLECT_PROPERTY(...)                                                                               \
    virtual void ImGuiDrawPropertys(ReflectHelper::ImGuiDraw::InputAutoSetting& setting = UmCore->ImGuiDrawPropertysSetting, bool isTail = true)    \
    {                                                                                                       \
        __super::ImGuiDrawPropertys(setting, false);                                                        \
        auto fields = std::tie(__VA_ARGS__);                                                                \
        static std::unordered_set<void*> reflectionFieldsSet;                                               \
        reflectionFieldsSet.clear();                                                                        \
                                                                                                            \
        ImGui::PushID(this);                                                                                \
        StdHelper::for_each_tuple(fields, [&](auto& field) {                                                \
            using FieldType = std::remove_cvref_t<decltype(field)>;                                         \
            if constexpr (PropertyUtils::is_TProperty_v<FieldType>)                                         \
            {                                                                                               \
                static_assert(FieldType::is_getter,                                                         \
                              "This property does not have a getter.");                                     \
                ReflectHelper::ImGuiDraw::Private::InputAuto(field, setting);                               \
            }                                                                                               \
            else                                                                                            \
            {                                                                                               \
                reflectionFieldsSet.insert(&field);                                                         \
            }                                                                                               \
        });                                                                                                 \
        imgui_draw_reflect_fields_input_auto(reflectionFieldsSet, setting);                                 \
        if (true == isTail)                                                                                 \
        {                                                                                                   \
            setting.InputEndEvent = nullptr;                                                                \
            ImGuiDrawPropertysEvent();                                                                      \
        }                                                                                                   \
        ImGui::PopID();                                                                                     \
    }                                                                                                       

namespace ReflectHelper
{
    namespace ImGuiDraw
    {
        struct InputAutoSetting
        {
            struct Int
            {
                float            v_speed = 1.f;
                int              min     = 0;
                int              max     = 0;
                const char*      format  = "%d";
                ImGuiSliderFlags flags   = 0;
            } _int;

            struct Float
            {
                float            v_speed = 0.1f;
                float            min     = 0.f;
                float            max     = 0.f;
                const char*      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            } _float;

            struct Double
            {
                float            v_speed = 0.01f;
                double           min     = 0.0;
                double           max     = 0.0;
                const char*      format  = "%.6f";
                ImGuiSliderFlags flags   = 0;
            } _double;

            struct String
            {
                ImGuiInputTextFlags    flags     = 0;
                ImGuiInputTextCallback callback  = nullptr;
                void*                  user_data = nullptr;
            } _string;

            struct Vector2
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            } _Vector2;

            struct Vector3
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            } _Vector3;

            struct Vector4
            {
                float            v_speed = 0.1f;
                float            v_min   = 0.f;
                float            v_max   = 0.f;
                std::string      format  = "%.3f";
                ImGuiSliderFlags flags   = 0;
            } _Vector4;

            // InputAuto return 직전에 호출해주는 함수객체 입니다.
            // 매개변수로 Input 여부랑 접근한 맴버의 이름을 전달해줍니다.
            // ImGuiDrawPropertys() 함수 호출 이후 NULL로 초기화됩니다.
            std::function<void(bool, std::string_view)> InputEndEvent;

            /// <summary>
            /// 이름 표시 여부 입니다.
            /// </summary>
            bool ShowName = true;

        };
    } // namespace ImGuiDraw

    namespace json
    {
        char* yyjsonValToCStr(yyjson_val* val);

        template <typename Type>
        inline std::string SerializedObjet(Type& obj)
        {
            return rfl::json::write(obj);
        }

        template <typename Type>
        bool DeserializedObjet(Type& obj, std::string_view data)
        {
            auto result = rfl::json::read<Type>(data.data());
            if (result)
            {
                obj = result.value();
            }
            else
            {
                yyjson_doc* doc = yyjson_read(data.data(), data.size(), 0);
                if (doc)
                {
                    yyjson_val* root = yyjson_doc_get_root(doc);
                    if (root)
                    {
                        const auto view = rfl::to_view(obj);
                        view.apply([&](auto& field) {
                            using FieldTpye     = std::remove_cvref_t<decltype(*field.value())>;
                            auto        name    = field.name();
                            auto&       value   = *field.value();
                            yyjson_val* jsonVal = yyjson_obj_get(root, name.data());
                            if (jsonVal)
                            {
                                if constexpr (std::is_signed_v<FieldTpye>)
                                {
                                    if constexpr (std::is_floating_point_v<FieldTpye>)
                                    {
                                        if (yyjson_is_real(jsonVal))
                                        {
                                            value = yyjson_get_real(jsonVal);
                                        }
                                    }
                                    else
                                    {
                                        if (yyjson_is_sint(jsonVal))
                                        {
                                            value = yyjson_get_sint(jsonVal);
                                        }
                                    }                               
                                }
                                else if constexpr (std::is_unsigned_v<FieldTpye>)
                                {
                                    if constexpr (std::is_same_v<bool, FieldTpye>)
                                    {
                                        if (yyjson_is_bool(jsonVal))
                                        {
                                            value = yyjson_get_bool(jsonVal);
                                        }
                                    }
                                    else
                                    {
                                        if (yyjson_is_uint(jsonVal))
                                        {
                                            value = yyjson_get_uint(jsonVal);
                                        }
                                    }
                                }                                                          
                                else if constexpr (std::is_same_v<FieldTpye, std::string>)
                                {
                                    if (yyjson_is_str(jsonVal))
                                    {
                                        value = yyjson_get_str(jsonVal);
                                    }
                                }
                                else if constexpr (std::is_same_v<FieldTpye, SIZE>)
                                {
                                    char* data = yyjsonValToCStr(jsonVal);
                                    if (nullptr != data)
                                    {
                                        auto result = rfl::json::read<SIZE>(data);
                                        if (result)
                                        {
                                            value = result.value();
                                        }
                                        free(data);
                                    }
                                }
                                else if constexpr (std::is_same_v<FieldTpye, POINT>)
                                {
                                    char* data = yyjsonValToCStr(jsonVal);
                                    if (nullptr != data)
                                    {
                                        auto result = rfl::json::read<POINT>(data);
                                        if (result)
                                        {
                                            value = result.value();
                                        }
                                        free(data);
                                    }
                                }
                                else if constexpr (std::is_same_v<FieldTpye, RECT>)
                                {
                                    char* data = yyjsonValToCStr(jsonVal);
                                    if (nullptr != data)
                                    {
                                        auto result = rfl::json::read<RECT>(data);
                                        if (result)
                                        {
                                            value = result.value();
                                        }
                                        free(data);
                                    }
                                }
                                else if constexpr (std::is_same_v<FieldTpye, std::array<float, 4>>)
                                {
                                    char* data = yyjsonValToCStr(jsonVal);
                                    if (nullptr != data)
                                    {
                                        auto result = rfl::json::read<std::array<float, 4>>(data);
                                        if (result)
                                        {
                                            value = result.value();
                                        }
                                        free(data);
                                    }
                                }
                            }
                        });
                    }
                }
                yyjson_doc_free(doc);
            }
            return result;
        }
    } // namespace json
} // namespace ReflectHelper